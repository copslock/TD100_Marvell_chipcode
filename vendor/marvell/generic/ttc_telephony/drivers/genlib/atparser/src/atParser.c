/*****************************************************************************
 * Utility Library
 *
 * DESCRIPTION
 *    ITU-T V.250 AT-command parser.  Parses a stream of text looking for AT
 *    commands to process.  When AT-commands are found they are parsed, a table
 *    lookup is performed, and application-supplied call-back functions are
 *    invoked.
 *
 *    This parser supports both basic and extended AT commands.  For basic AT
 *    commands this parser does not distinguish between "action", "set" or
 *    "get" requests, since the basic AT-command syntax does not specify which
 *    result is desired (instead this attribute is intrinsic to each AT-command).
 *    For extended AT commands this parser distinguishes between "action", "set"
 *    "get", and "query syntax" requests.
 *
 * OVERALL STRUCTURE
 *
 *  .--------------.                        .--------------.
 *  | AT Manager * |                        | DTE Client * |
 *  |              |                        `--------------'
 *  `--------------'                               ^
 *      | ^                                        |AT-commands
 *      | |utlAtGetParameterFunction_P             v
 *      | |utlAtSetParameterFunction_P  .-------------------------.          .-------------------.
 *      | |utlAtSParameterFunction_P    |    DCE I/O Interface *  |          | Sound Interface * |
 *      | |                             `-------------------------'          `-------------------'
 *      | |                                ^  |               ^                       ^
 *      | |      utlAtDceIoConfigFunction_P|  |utlAtParse()   |utlAtReplyFunction_P   |
 *      | |                                |  v               |                       |utlAtSoundConfigFunction_P
 *      | |                             .-------------------------.                   |
 *      | |                             |                         |-------------------'
 *      | `-----------------------------|      AT PARSER (DCE)    |<----------------------------.
 *      `------------------------------>|                         |<---------------.            |
 *                utlAtParserOp()       |                         |--------------. |            v
 *                                      `-------------------------'              | |   ---------------------
 *                                           ^    |                              | |    AT-commands Table *
 *                         utlAtDriverEvent()|    |utlAtDriverRequestFunction_P  | |   ---------------------
 *                     utlAtCommandResponse()|    |utlAtTxLineDataFunction_P     | |
 *                                           |    v                              | |
 *                                 .------------------------.                    | |
 *                                 | Raw Modem Adaptation * |                    | |
 *                                 `------------------------'                    | |utlAtRetrieveDialStringFunction_P
 *                                             ^                                 | |
 *                                             |    utlAtSaveDialStringFunction_P| |
 *                                             v                                 | |
 *                                     .--------------.                          v |
 *                                     |  Raw Modem * |                  ---------------------
 *                                     `--------------'                   Dial String Storage
 *                                                                       ---------------------
 *     * == must be supplied by application using this AT parser
 *******************************************************************************************/

#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include <utlTypes.h>
#include <utlConvert.h>
#include <utlError.h>
#include <utlLinkedList.h>
#include <utlMalloc.h>
#include <utlTime.h>
#include <utlTimer.h>
#include <utlTrace.h>
#include <utlVString.h>

#include <utlAtParser.h>

#include <teldef.h>
#include <telatci.h>
#include <telatparamdef.h>
#include <eeh_assert_notify.h>

#ifdef utlTEST
#   include <termios.h>
#   include <utlEventHandler.h>
#endif

/*--- Configuration ---------------------------------------------------------*/

/*--- version info ---*/
#define utlAT_VERSION "2.04"

/*--- maximum length of an AT-command response (in characters) ---*/
#define utlAT_MAX_RESPONSE_LENGTH ((size_t)400)

/*--- maximum length of connect-text string (in characters) ---*/
#define utlAT_MAX_CONNECT_TEXT_LENGTH ((size_t)16)

/*--- maximum possible number of parameters in an extended-AT command ---*/
#define utlAT_MAX_PARAMETERS ((size_t)16)

/*--- array of supported basic-AT command prefix characters ---*/
#define utlAT_BASIC_COMMAND_PREFIXES "&"

/*--- array of supported extended-AT command prefix characters ---*/
#define utlAT_EXTENDED_COMMAND_PREFIXES "+*$^"

/*--- array of characters allowed in dial strings ---*/
#define utlAT_DIALING_CHARACTERS "0123456789aAbBcCdD#*+,/\">gGiIlLpPsStTwW@!$"

/*--- for tracing AT parsers ---*/
#define utlTRACE_AT_PARSER "AT parsers"

#define UNUSEDPARAM(param) (void)param;

char revisionId[128] = "unknown";

extern utlTimeOutCount_T auto_answer_delay_cycles;


/*--- Data Types ------------------------------------------------------------*/

typedef enum {
	utlAT_REQUEST_TYPE_GET,         /* set  value(s)  request */
	utlAT_REQUEST_TYPE_SYNTAX,      /* command syntax request */
	utlAT_REQUEST_TYPE_SET,         /* get  value(s)  request */
} utlAtRequestType_T;

typedef enum {
	utlAT_RESULT_CODE_OK           = 0,
	utlAT_RESULT_CODE_CONNECT      = 1,
	utlAT_RESULT_CODE_RING         = 2,
	utlAT_RESULT_CODE_NO_CARRIER   = 3,
	utlAT_RESULT_CODE_ERROR        = 4,
	utlAT_RESULT_CODE_NO_DIAL_TONE = 6,
	utlAT_RESULT_CODE_BUSY         = 7,
	utlAT_RESULT_CODE_NO_ANSWER    = 8,
	utlAT_RESULT_CODE_SUPPRESS     = 100,
	utlAT_RESULT_CODE_NULL
} utlAtResultCode_T;

typedef enum {
	utlAT_STRING_STATE_PASS_THROUGH,
	utlAT_STRING_STATE_EXTRACT_HEX_DIGIT1,
	utlAT_STRING_STATE_EXTRACT_HEX_DIGIT2,
} utlAtStringState_T;


/*--- Global Variables ------------------------------------------------------*/

/*--- Local Variables -------------------------------------------------------*/

static utlLinkedList_T parsers = utlEMPTY_LINKED_LIST;

/*****************************************************************************
* FORWARD DECLARATIONS
*****************************************************************************/

static utlReturnCode_T utlAtReset(const utlAtParser_P parser_p);

static utlReturnCode_T utlAtDceIoConfigEvent(const utlAtParser_P parser_p);

static utlReturnCode_T utlAtSoundConfigEvent(const utlAtParser_P parser_p);

static utlAtResultCode_T utlProcessSParameter(const utlAtParser_P parser_p,
					      const unsigned int parameter_num,
					      const unsigned int parameter_value);

static utlReturnCode_T utlSendBasicSyntaxResultCode(const utlAtParser_P2c parser_p,
						    const utlAtResultCode_T code,
						    const bool ignore_state);

static utlReturnCode_T utlGenerateFormattedGetResponse(const utlAtParser_P parser_p,
						       const char                    *command_name_p,
						       const utlAtParameterValue_P2c parameters_p,
						       const size_t num_parameters);

static void utlAbandonAllPendingAsyncResponse(utlAtParser_P parser_p);

extern int triggerAtParser(TelAtParserID sAtpIndex);

static unsigned int utlAtGetTimeOut(const utlAtCommand_P2c cmd_p, const utlAtAsyncOp_T op)
{
	ATCMD_TimeOut *timeout_p = atcmd_timeout_table;
	int index = -1;
	switch(op)
	{
	case utlAT_ASYNC_OP_SET:
	case utlAT_ASYNC_OP_ACTION:
	case utlAT_ASYNC_OP_EXEC:
            index = 0;
            break;
	case utlAT_ASYNC_OP_GET:
	    index = 1;
	    break;
	case utlAT_ASYNC_OP_SYNTAX:
	    index = 2;
	    break;
	case utlAT_ASYNC_OP_NULL:
	case utlAT_ASYNC_OP_UNKNOWN:
	default:
	    break;
	}
	if(index >= 0)
	{
		while(timeout_p->name_p != NULL)
		{
			if(strcmp(timeout_p->name_p, cmd_p->name_p) == 0)
				return timeout_p->time_second[index];
			timeout_p++;
		}
	}
	return utlAT_AWAIT_ASYNC_TIMEOUT_TIME;
}
bool utlAtCommandTimeoutModify(const char *commandName, int mode, int timeout)
{
	utlAtCommand_P2c command_p = pshell_commands;
	utlAtCommand_P2c term_command_p = command_p + shell_commands_num;
	ATCMD_TimeOut *timeout_p;
	ATCMD_TimeOut *term_timeout_p = atcmd_timeout_table + shell_commands_num;
	bool cmdValid = false;
	unsigned int j;
	if((commandName == NULL) || ((term_timeout_p - 1)->name_p != NULL))
		return false;
	/*--- for each command... ---*/
	for (; command_p < term_command_p; command_p++)
	{
		if(strcasecmp(command_p->name_p, commandName) == 0)
		{
			if((command_p->type == utlAT_COMMAND_TYPE_BASIC) && (mode != TEL_AT_HTCCTO_2_MODE_SET))
				cmdValid = false;
			else
				cmdValid = true;
			break;
		}
	}
	if(cmdValid)
	{
		pthread_mutex_lock(&gTimeoutTableLock);
		timeout_p = atcmd_timeout_table;
		while(timeout_p->name_p != NULL)
		{
			if(strcasecmp(command_p->name_p, timeout_p->name_p) == 0)
			{
				for(j = 0; j < 3; j++)
				{
					if(((mode >> j) & 1UL) != 0)
					{
						timeout_p->time_second[j] = timeout;
					}
				}
				break;
			}
			timeout_p++;
		}
		if(timeout_p->name_p == NULL)
		{
			for(j = 0; j < 3; j++)
			{
				if(((mode >> j) & 1UL) != 0)
				{
					timeout_p->time_second[j] = timeout;
				}
				else
				{
					timeout_p->time_second[j] = utlAT_AWAIT_ASYNC_TIMEOUT_TIME;
				}
			}
			timeout_p->name_p = command_p->name_p;
		}
		pthread_mutex_unlock(&gTimeoutTableLock);
		return true;
	}
	return false;
}
/*****************************************************************************
* APPLICATION-REPLACEABLE STUBS
*****************************************************************************/

/*--- dial-string storage ---*/
static struct {
	char location_name[utlAT_MAX_STORED_LOCATION_NAME_LENGTH + 1];
	char dial_string[utlAT_MAX_STORED_DIAL_STRING_LENGTH   + 1];
}           dial_string_storage[utlAT_MAX_STORED_DIAL_STRINGS];
static bool dial_string_storage_initialized = false;

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlAtParserInitDialStringStorage()
* INPUT
*      none
* OUTPUT
*      none
* RETURNS
*      nothing
* DESCRIPTION
*      Initializes the AT Parser's internal dial string storage.
*---------------------------------------------------------------------------*/
static void utlAtParserInitDialStringStore(void)
{
	size_t i;

	if (dial_string_storage_initialized == true)
		return;

	/*--- initialize internal static dial-string storage ---*/
	for (i = 0; i < utlAT_MAX_STORED_DIAL_STRINGS; i++)
	{
		dial_string_storage[i].location_name[0] = '\0';
		dial_string_storage[i].dial_string[0]   = '\0';
	}

	dial_string_storage_initialized = true;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlAtParserStoreSaveDialString(location_name_p, dial_string_p, arg_p)
* INPUT
*      location_name_p == location to store dial-string to
*        dial_string_p == NULL pointer, or a pointer to the dial string to be
*                         saved expressed as a null-terminated string
*                arg_p == pointer to user-defined data
* OUTPUT
*      none
* RETURNS
*      utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*      Saves the dial string specified by `dial_string_p' to the AT Parser's
*      internal dial-string store.  If `dial_string_p' is NULL, clears any
*      dial string previously saved to the specified location.
*---------------------------------------------------------------------------*/
static utlReturnCode_T utlAtParserStoreSaveDialString(const char *location_name_p,
						      const char *dial_string_p,
						      void *arg_p)
{
	size_t i;
	size_t insertion_i;

	utlAssert(location_name_p != NULL);
	// arg_p is not used in this function
	arg_p = NULL;

	if (dial_string_storage_initialized != true)
		utlAtParserInitDialStringStore();

	/*--- search for specified dial string (ignoring character case) ---*/
	insertion_i = -1;
	for (i = 0; i < utlAT_MAX_STORED_DIAL_STRINGS; i++)
	{
		int cmp;

		/*--- no more filled entries? ---*/
		if (dial_string_storage[i].location_name[0] == '\0')
			break;

		cmp = strcasecmp(location_name_p, dial_string_storage[i].location_name);

		/*--- found an existing entry with the same location name? ---*/
		if (cmp == 0)
		{

			/*--- if no dial-string was supplied, clear/remove the specified entry ---*/
			if ((dial_string_p == NULL) || (strlen(dial_string_p) == (size_t)0))
			{

				for (; i < (utlAT_MAX_STORED_DIAL_STRINGS - 1); i++)
				{
					(void)strcpy(dial_string_storage[i].location_name, dial_string_storage[i + 1].location_name);
					(void)strcpy(dial_string_storage[i].dial_string,   dial_string_storage[i + 1].dial_string);
				}
				dial_string_storage[i].location_name[0] = '\0';
				dial_string_storage[i].dial_string[0]   = '\0';

				return utlSUCCESS;
			}

			/*--- check that dial-string fits into static storage ---*/
			if (strlen(dial_string_p) > utlAT_MAX_STORED_DIAL_STRING_LENGTH)
			{
				utlError("Dial-string too long to save.");
				return utlFAILED;
			}

			/*--- save dial string--- */
			(void)strcpy(dial_string_storage[i].dial_string, dial_string_p);

			return utlSUCCESS;
		}

		/*--- found where to insert new dial string? ---*/
		if (cmp < 0)
		{
			insertion_i = i;

			for (i++; i < utlAT_MAX_STORED_DIAL_STRINGS; i++)
				if (dial_string_storage[i].location_name[0] == '\0')
					break;

			break;
		}
	}

	/*--- was specified dial-string not found and is there no room for another dial string? ---*/
	if (i >= utlAT_MAX_STORED_DIAL_STRINGS)
	{

		/*--- were we just going to clear the location anyway? ---*/
		if ((dial_string_p == NULL) || (strlen(dial_string_p) == (size_t)0))
			return utlSUCCESS;

		utlError("Dial-string storage full.");
		return utlFAILED;
	}

	/*--- check that dial-string fits into static storage ---*/
	if ((strlen(location_name_p) > utlAT_MAX_STORED_LOCATION_NAME_LENGTH) ||
	    (strlen(dial_string_p)   > utlAT_MAX_STORED_DIAL_STRING_LENGTH))
	{
		utlError("Location name and/or Dial-string too long to save.");
		return utlFAILED;
	}

	/*--- insertion sort ---*/
	if (insertion_i != (size_t)-1)
		for (; i > insertion_i; i--)
		{
			(void)strcpy(dial_string_storage[i].location_name, dial_string_storage[i - 1].location_name);
			(void)strcpy(dial_string_storage[i].dial_string,   dial_string_storage[i - 1].dial_string);
		}
	(void)strcpy(dial_string_storage[i].location_name, location_name_p);
	(void)strcpy(dial_string_storage[i].dial_string,   dial_string_p);

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlAtParserStoreRetrieveDialString(location_name_pp, dial_string_pp, arg_p)
* INPUT
*      location_name_pp == pointer to where a location-name pointer resides
*                          or a pointer to a NULL pointer
*        dial_string_pp == pointer to where a pointer to the retrieved dial
*                          string should be placed
*                 arg_p == pointer to user-defined data
* OUTPUT
*      *location_name_pp == next unread location-name, or NULL
*        *dial_string_pp == pointer to the retrieved dial string, where the
*                           dial string is expressed as a null-terminated
*                           character string
* RETURNS
*      utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*      Retrieves a previously saved dial string from the AT Parser's internal
*      dial-string store.  To allow dial strings to be seqentially retrieved
*      when the dial string location names are unknown, the first passed in
*      `location_name_pp' should be set to NULL.  Upon return
*      `*location_name_pp' will be set to the next unread location name, and
*      `*dial_string_pp' will be null.  All subsequent calls will always set
*      `*location_name_pp' to the next unread location name, and
*      `*dial_string_pp' to the dial string associated with the passed-in
*      location name.  When this function returns from reading the last dial
*      string, `*location_name_pp' will be set to NULL.
*---------------------------------------------------------------------------*/
static utlReturnCode_T utlAtParserStoreRetrieveDialString(const char **location_name_pp,
							  const char **dial_string_pp,
							  void  *arg_p)
{
	size_t i;

	utlAssert(location_name_pp != NULL);
	// arg_p is not used in this function
	arg_p = NULL;

	if (dial_string_storage_initialized != true)
		utlAtParserInitDialStringStore();

	/*--- fetch location name for first filled slot? ---*/
	if (*location_name_pp == NULL)
	{

		/*--- search for first filled location ---*/
		for (i = 0; i < utlAT_MAX_STORED_DIAL_STRINGS; i++)
			if (dial_string_storage[i].location_name[0] != '\0')
			{
				*location_name_pp = dial_string_storage[i].location_name;
				break;
			}

		if ( dial_string_pp != NULL)
			*dial_string_pp  = NULL;

		return utlSUCCESS;

	}

	utlAssert(dial_string_pp != NULL);

	/*--- search for specified dial string (ignoring character case) ---*/
	for (i = 0; i < utlAT_MAX_STORED_DIAL_STRINGS; i++)
		if (strcasecmp(*location_name_pp, dial_string_storage[i].location_name) == 0)
			break;

	/*--- nothing found? ---*/
	if (i >= utlAT_MAX_STORED_DIAL_STRINGS)
	{
		*location_name_pp = NULL;
		*dial_string_pp   = NULL;

		return utlSUCCESS;
	}

	*location_name_pp = NULL;
	*dial_string_pp   = dial_string_storage[i].dial_string;

	/*--- search for next unread dial string (ignoring character case) ---*/
	for (i++; i < utlAT_MAX_STORED_DIAL_STRINGS; i++)
		if (dial_string_storage[i].location_name[0] != '\0')
		{
			*location_name_pp = dial_string_storage[i].location_name;
			break;
		}

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlNextAvailableXID()
* INPUT
*      none
* OUTPUT
*      none
* RETURNS
*      a transaction ID
* DESCRIPTION
*      Fetches the next available (unused) transaction ID.  The transaction
*      ID is a 32-bit integer that we allow to wrap.  We assume that by the
*      time the integer has wrapped, the oldest values are no longer in use.
*      Assuming a consumption rate of 50 transaction ID's per second, the
*      value would wrap in about 990 days.
*
*      Transaction ID's 0 and utlFAILED are reserved and thus never used.
*---------------------------------------------------------------------------*/
static unsigned int utlNextAvailableXID(void)
{
	static unsigned int next_free_xid = 2;

	unsigned int xid;
	bool unique;

	/*--- fetch next free transaction ID ---*/
	xid = next_free_xid;

	do {
		utlAtParser_P parser_p;

		next_free_xid++;

		unique = (next_free_xid != (unsigned int)0) &&
			 (next_free_xid != 0xFFFFFFFF);

		/*--- for each parser... ---*/
		for (parser_p = (utlAtParser_P)parsers.head_p; parser_p != NULL;
		     parser_p = parser_p->next_p)
		{
			utlAtAsyncResponse_P async_response_p;

			/*--- for each async response this parser is waiting for... ---*/
			async_response_p = (utlAtAsyncResponse_P)parser_p->states.async_responses.pending.head_p;
			for (; async_response_p != NULL; async_response_p = async_response_p->next_p)
			{
				if (async_response_p->xid == xid)
				{
					unique = false;
					break;
				}
			}

			if (unique == false)
				break;
		}

	} while (unique == false);

	return xid;
}

/*****************************************************************************
* AT-PARSER
*****************************************************************************/

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlAtReset(parser_p)
* INPUT
*      parser_p == an open AT-command parser
* OUTPUT
*      none
* RETURNS
*      utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*      Initializes AT-command parser parameters.
*---------------------------------------------------------------------------*/
static utlReturnCode_T utlAtReset(const utlAtParser_P parser_p)
{
	utlAssert(parser_p != NULL);

	/*--- S-parameters ---*/
	{
		size_t i;

		parser_p->parameters.S[utlAT_AUTO_ANSWER]           = 0;        /* disable auto-answer */
		parser_p->parameters.S[utlAT_RING_COUNTER]          = 0;        /* cleared             */
		parser_p->parameters.S[utlAT_ESCAPE_CHAR]           = '+';
		parser_p->parameters.S[utlAT_LINE_TERM_CHAR]        = '\015';   /* <carriage-return>   */
		parser_p->parameters.S[utlAT_FORMATTING_CHAR]       = '\012';   /* <new-line>          */
		parser_p->parameters.S[utlAT_LINE_EDIT_CHAR]        = '\010';   /* <back-space>        */
		parser_p->parameters.S[utlAT_BLIND_DIAL_PAUSE_TIME] = 2;        /* 2 seconds           */
		parser_p->parameters.S[utlAT_CONN_COMPLETE_TIMEOUT] = 1;        /* 1 second            */
		parser_p->parameters.S[utlAT_DIALING_PAUSE_TIME]    = 2;        /* 2 seconds           */
		parser_p->parameters.S[utlAT_CARRIER_DETECT_TIME]   = 6;        /* 0.6 seconds         */
		parser_p->parameters.S[utlAT_CARRIER_LOSS_TIME]     = 7;        /* 0.7 seconds         */
		parser_p->parameters.S[utlAT_DTMF_TONE_DURATION]    = 63;       /* 0.63 seconds        */
		parser_p->parameters.S[utlAT_ESCAPE_GUARD_TIME]     = 50;       /* 1 second            */
		parser_p->parameters.S[utlAT_SEPARATER_CHAR]        = ',';
		parser_p->parameters.S[utlAT_TERMINATER_CHAR]       = ';';
		parser_p->parameters.S[utlAT_XON_CHAR]              = 0x11;     /* <DC1>               */
		parser_p->parameters.S[utlAT_XOFF_CHAR]             = 0x13;     /* <DC3>               */
		parser_p->parameters.S[utlAT_SLEEP_TIME]            = 0;        /* 0 seconds           */
		parser_p->parameters.S[utlAT_DTR_DELAY_TIME]        = 10;       /* 0.1 seconds         */
		parser_p->parameters.S[utlAT_HOOK_FLASH_TIME]       = 5;        /* 0.5 seconds         */
		parser_p->parameters.S[utlAT_INACTIVITY_TIME]       = 10;       /* 100 seconds         */
		parser_p->parameters.S[utlAT_DISCONNECT_WAIT_TIME]  = 0;        /* 0 seconds           */

		/*--- notify application of S-parameter value changes ---*/
		if (parser_p->call_backs.s_parameter_function_p != NULL)
		{
			for (i = 0; i < utlAT_NUM_S_PARAMETERS; i++)
				if ((parser_p->call_backs.s_parameter_function_p)(i, parser_p->parameters.S[i], parser_p->call_backs.arg_p) != utlSUCCESS)
					return utlFAILED;
		}
	}

	/*--- parameters ---*/
	{
		parser_p->parameters.echo_text            = false;
		parser_p->parameters.verbose_results      = true;
		parser_p->parameters.suppress_results     = false;
		parser_p->parameters.include_connect_text = false;
		parser_p->parameters.detect_dial_tone     = false;
		parser_p->parameters.detect_busy_signal   = false;
		parser_p->parameters.dialing_mode         = utlAT_DIALING_MODE_TONE;
		parser_p->parameters.raw_framing          = 3;
		parser_p->parameters.raw_parity           = 3;
	}

	/*--- buffers ---*/
	{
		(void)memset(parser_p->buffers.queue, 0, sizeof(parser_p->buffers.queue));

		parser_p->buffers.echo_p = parser_p->buffers.queue;
		parser_p->buffers.head_p = parser_p->buffers.queue;
		parser_p->buffers.tail_p = parser_p->buffers.queue;
		parser_p->buffers.term_p = parser_p->buffers.queue + utlNumberOf(parser_p->buffers.queue);

		parser_p->buffers.overflow = false;

		(void)strcpy(parser_p->buffers.previous_line, "AT");
	}

	/*--- DCE I/O config ---*/
	{
		parser_p->dce_io_config.data_rate                   = 0;        /* auto-detect */
		parser_p->dce_io_config.data_bits                   = 8;
		parser_p->dce_io_config.parity_bits                 = 0;        /* no parity */
		parser_p->dce_io_config.stop_bits                   = 1;
		parser_p->dce_io_config.parity                      = utlDATA_PARITY_SPACE;
		parser_p->dce_io_config.flow_control.dce_by_dte     = 2;
		parser_p->dce_io_config.flow_control.dte_by_dce     = 2;
		parser_p->dce_io_config.flow_control.xon_char       = parser_p->parameters.S[utlAT_XON_CHAR];
		parser_p->dce_io_config.flow_control.xoff_char      = parser_p->parameters.S[utlAT_XOFF_CHAR];
		parser_p->dce_io_config.modes.dcd_always_on         = false;
		parser_p->dce_io_config.modes.ignore_dtr            = false;
		parser_p->dce_io_config.modes.drop_call_on_dtr_loss = false;
		parser_p->dce_io_config.modes.dsr_mode              = 0;

		parser_p->states.dce_io_config_pending_mask = ~0u;  /* all config-changes are pending */
	}

	/*--- sound config ---*/
	{
		parser_p->sound_config.monitor_speaker.mode  = 1;
		parser_p->sound_config.monitor_speaker.level = 0;

		parser_p->states.sound_config_pending_mask = ~0u;  /* all config-changes are pending */
	}

	/*--- peg counts ---*/
	{
		parser_p->peg_counts.basic_commands     = 0;
		parser_p->peg_counts.extended_commands  = 0;
		parser_p->peg_counts.undefined_commands = 0;
		parser_p->peg_counts.bad_commands       = 0;
	}

	/*--- parser states ---*/
	{

		parser_p->states.go_on_line           = false;
		parser_p->states.discard_current_line = false;
		parser_p->states.bypass_mode          = false;

		{
			utlAtAsyncResponse_P async_response_p;

			if (parser_p->queue_semaphore.initialized && utlAcquireExclusiveAccess(&parser_p->queue_semaphore) != utlSUCCESS)
			{
				utlError("Cannot exclusively acquire semaphore!\n");
				return utlFAILED;
			}

			/*--- discard any pending async responses ---*/
			while ((async_response_p = utlGetHeadNode(&(parser_p->states.async_responses.pending), utlAtAsyncResponse_T)) != NULL)
			{

				if (parser_p->queue_semaphore.initialized) utlReleaseExclusiveAccess(&parser_p->queue_semaphore);

				/*--- stop timer ---*/
				if (                 async_response_p->timer_id  != (utlTimerId_T)utlFAILED)
				{
					if (utlStopTimer(async_response_p->timer_id) != utlSUCCESS)
						return utlFAILED;

					async_response_p->timer_id = utlFAILED;
				}

				if (parser_p->queue_semaphore.initialized && utlAcquireExclusiveAccess(&parser_p->queue_semaphore) != utlSUCCESS)
				{
					utlError("Cannot exclusively acquire semaphore!\n");
					return utlFAILED;
				}

				utlPutTailNode(&(parser_p->states.async_responses.unused), utlAtAsyncResponse_T, async_response_p);
			}

			/*--- re-initialize nodes ---*/
			for (async_response_p = (utlAtAsyncResponse_P)parser_p->states.async_responses.unused.head_p; async_response_p != NULL; async_response_p = async_response_p->next_p)
			{
				async_response_p->parser_p  = parser_p;
				async_response_p->xid       = utlFAILED;
				async_response_p->op        = utlAT_ASYNC_OP_UNKNOWN;
				async_response_p->command_p = NULL;

			}

			if (parser_p->queue_semaphore.initialized) utlReleaseExclusiveAccess(&parser_p->queue_semaphore);
		}

		parser_p->states.sync_response.complete_reported  = false;
		parser_p->states.sync_response.abort_reported     = false;
		parser_p->states.sync_response.error_reported     = false;
		parser_p->states.sync_response.parameter_values_p = NULL;
		parser_p->states.sync_response.custom_p           = NULL;
		parser_p->states.sync_response.info_text_p        = NULL;
		parser_p->states.sync_response.abort_response_p   = NULL;
		parser_p->states.sync_response.command_syntax_p   = NULL;

		parser_p->states.dce_io_config_pending_mask = ~0;
		parser_p->states.sound_config_pending_mask  = ~0;
		parser_p->states.line_off                   = 0;

		parser_p->states.escape.last_tx_time.seconds     = 0;
		parser_p->states.escape.last_tx_time.nanoseconds = 0;
		parser_p->states.escape.state                    = utlAT_PARSER_ESCAPE_STATE_BEGIN;

		parser_p->states.dial_string.active                      = false;
		parser_p->states.dial_string.dial_string_delay           = 0;  /* no delay */
		(void)memset(parser_p->states.dial_string.last, 0, sizeof(parser_p->states.dial_string.last));
		(void)memset(parser_p->states.dial_string.buf,  0, sizeof(parser_p->states.dial_string.buf));
		parser_p->states.dial_string.c_p                         = parser_p->states.dial_string.buf;  /* reset */
		parser_p->states.dial_string.options.international       = false;
		parser_p->states.dial_string.options.do_call_origination = true;
		parser_p->states.dial_string.options.use_CCUG_SS_info    = false;
		parser_p->states.dial_string.options.CLI_presentation    = '\0';

		parser_p->states.delay_dialing_time.seconds     = 0;
		parser_p->states.delay_dialing_time.nanoseconds = 125000000;
		parser_p->states.min_silence_time.seconds       = 5;
		parser_p->states.min_silence_time.nanoseconds   = 0;
	}

	/*--- set values slaved from s-parameter settings ---*/
	if ((utlProcessSParameter(parser_p, utlAT_BLIND_DIAL_PAUSE_TIME, parser_p->parameters.S[utlAT_BLIND_DIAL_PAUSE_TIME]) != utlSUCCESS) ||
	    (utlProcessSParameter(parser_p, utlAT_CONN_COMPLETE_TIMEOUT, parser_p->parameters.S[utlAT_CONN_COMPLETE_TIMEOUT]) != utlSUCCESS) ||
	    (utlProcessSParameter(parser_p, utlAT_DIALING_PAUSE_TIME,    parser_p->parameters.S[utlAT_DIALING_PAUSE_TIME])    != utlSUCCESS) ||
	    (utlProcessSParameter(parser_p, utlAT_CARRIER_DETECT_TIME,   parser_p->parameters.S[utlAT_CARRIER_DETECT_TIME])   != utlSUCCESS) ||
	    (utlProcessSParameter(parser_p, utlAT_CARRIER_LOSS_TIME,     parser_p->parameters.S[utlAT_CARRIER_LOSS_TIME])     != utlSUCCESS) ||
	    (utlProcessSParameter(parser_p, utlAT_XON_CHAR,              parser_p->parameters.S[utlAT_XON_CHAR])              != utlSUCCESS) ||
	    (utlProcessSParameter(parser_p, utlAT_XOFF_CHAR,             parser_p->parameters.S[utlAT_XOFF_CHAR])             != utlSUCCESS) ||
	    (utlProcessSParameter(parser_p, utlAT_DTR_DELAY_TIME,        parser_p->parameters.S[utlAT_DTR_DELAY_TIME])        != utlSUCCESS) ||
	    (utlProcessSParameter(parser_p, utlAT_HOOK_FLASH_TIME,       parser_p->parameters.S[utlAT_HOOK_FLASH_TIME])       != utlSUCCESS) ||
	    (utlProcessSParameter(parser_p, utlAT_INACTIVITY_TIME,       parser_p->parameters.S[utlAT_INACTIVITY_TIME])       != utlSUCCESS) ||
	    (utlProcessSParameter(parser_p, utlAT_DISCONNECT_WAIT_TIME,  parser_p->parameters.S[utlAT_DISCONNECT_WAIT_TIME])  != utlSUCCESS))
		return utlFAILED;

	if (parser_p->cmd_cnt_semaphore.initialized && utlAcquireExclusiveAccess(&parser_p->cmd_cnt_semaphore) != utlSUCCESS)
	{
		utlError("Cannot exclusively acquire semaphore!\n");
		return utlFAILED;
	}

	parser_p->commands_in_line = 0;

	if (parser_p->cmd_cnt_semaphore.initialized) utlReleaseExclusiveAccess(&parser_p->cmd_cnt_semaphore);

	if( utlInitSemaphore(&parser_p->queue_semaphore, utlSEMAPHORE_ATTR_SHARING_ENABLE |
			 utlSEMAPHORE_ATTR_NESTING_ENABLE) != utlSUCCESS )
		return utlFAILED;

	if( utlInitSemaphore(&parser_p->cmd_cnt_semaphore, utlSEMAPHORE_ATTR_SHARING_ENABLE |
			 utlSEMAPHORE_ATTR_NESTING_ENABLE) != utlSUCCESS )
		return utlFAILED;

	return utlSUCCESS;
}

/*------------------------------------------------------------------------------
 * Function that check parameters for utlOpenAtParser
 *------------------------------------------------------------------------------*/
static int CheckParamForOpenAtParser(const utlAtCommand_P2c commands_p,
				     const size_t num_commands,
				     void         *arg_p)
{
#ifdef utlDEBUG
	utlAtCommand_P2c command_p;
	utlAtCommand_P2c term_command_p;

	/*--- for each command... ---*/
	term_command_p = commands_p + num_commands;
	for (command_p = commands_p; command_p < term_command_p; command_p++)
	{
		/*--- name missing? ---*/
		if (command_p->name_p == NULL)
		{
			utlError("Command at %d has no name.", command_p - commands_p);
			return -1;
		}

		if (command_p->type == utlAT_COMMAND_TYPE_BASIC)
		{

			/*--- check command name syntax (must be a single character,
			   or a single character preceded by one of the pre-defined
			   basic-AT-command prefixes) */
			{
				const char *c_p;
				c_p = command_p->name_p;
				if (strchr(utlAT_BASIC_COMMAND_PREFIXES, *c_p) != NULL)
					c_p++;
				if (((*c_p < 'A') || (*c_p > 'Z')) &&
				    ((*c_p < 'a') || (*c_p > 'z')))
				{
					utlError("Invalid basic AT-command-name character '%c'.", *c_p);
					return -1;
				}
				c_p++;
				if (*c_p != '\0')
				{
					utlError("Basic AT-command name '%s' too long.",
						 command_p->name_p);
					return -1;
				}
			}

			/*--- check command parameter info (can either be zero or one parameter) ---*/
			{
				/*--- no parameters? ---*/
				if (command_p->parameters_p == NULL)
				{
					if (command_p->num_parameters != (size_t)0)
					{
						utlError("Command '%s' has an invalid number of parameters (%d).", command_p->name_p, command_p->num_parameters);
						return -1;
					}
				}
				else
				{
					utlAtParameter_P2c parameter_p;
					if (command_p->num_parameters != (size_t)1)
					{
						utlError("Command '%s' has an invalid number of parameters (%d).", command_p->name_p, command_p->num_parameters);
						return -1;
					}
					parameter_p = command_p->parameters_p;
					/*--- parameter (if present) must be decimal, except for the D and &Z commands ---*/
					if ((((strcasecmp(command_p->name_p, "D") == 0) || (strcasecmp(command_p->name_p, "&Z") == 0)) && (parameter_p->type != utlAT_DATA_TYPE_DIAL_STRING)) ||
					    (((strcasecmp(command_p->name_p, "D") != 0) && (strcasecmp(command_p->name_p, "&Z") != 0)) && (parameter_p->type != utlAT_DATA_TYPE_DECIMAL)))
					{
						utlError("Command '%s' has an invalid parameter type.", command_p->name_p);
						return -1;
					}
					if (parameter_p->access != utlAT_PARAMETER_ACCESS_READ_WRITE)
					{
						utlError("Command '%s' specifies an invalid parameter access.", command_p->name_p);
						return -1;
					}
				}
			}

		}
		else if ((command_p->type == utlAT_COMMAND_TYPE_EXTENDED) ||
			 (command_p->type == utlAT_COMMAND_TYPE_EXACTION))
		{
			/*--- check command name syntax ---*/
			{
				const char *c_p;

				if (strlen(command_p->name_p) > 17)
				{
					utlError("Command name %s too long.", command_p->name_p);
					return -1;
				}
				c_p = command_p->name_p;

				/*--- first character must be a valid extended command prefix, second must
				   be alphabetic, rest must belong to a specific set of characters */
				if (strchr(utlAT_EXTENDED_COMMAND_PREFIXES, *c_p) == NULL)
				{
					utlError("Invalid extended AT-command-name character '%c' in command '%s'.", *c_p, command_p->name_p);
					return -1;
				}
				c_p++;
				if (((*c_p  < 'A') || (*c_p  > 'Z')) &&
				    ((*c_p  < 'a') || (*c_p  > 'z')))
				{
					utlError("Invalid extended AT-command-name character '%c' in command '%s'.", *c_p, command_p->name_p);
					return -1;
				}
				c_p++;
				for (; *c_p != '\0'; c_p++)
					if (((*c_p  < 'A') || (*c_p  > 'Z')) &&
					    ((*c_p  < 'a') || (*c_p  > 'z')) &&
					    ((*c_p  < '0') || (*c_p  > '9')) &&
					    (*c_p != '!') && (*c_p != '%')  &&
					    (*c_p != '-') && (*c_p != '.')  &&
					    (*c_p != '/') && (*c_p != ':')  && (*c_p != '_'))
					{
						utlError("Invalid extended AT-command-name character '%c' in command '%s'.", *c_p, command_p->name_p);
						return -1;
					}
			}

			/*--- check command parameter(s) info ---*/
			{
				/*--- no parameters? ---*/
				if (command_p->parameters_p == NULL)
				{
					/* FIXME: The definition of utlDEFINE_EXACTION_AT_COMMAND has problem and will result in wrong parameter; "telcontroller.c" cannot initialize AT parser because of this error.
					   if (command_p->num_parameters != (size_t) 0) {
					   utlError("Command '%s' has an invalid number of parameters (%d).", command_p->name_p, command_p->num_parameters);
					   return -1;
					   }
					 */

				}
				else
				{
					utlAtParameter_P2c parameter_p;
					utlAtParameter_P2c term_parameter_p;
					if (command_p->num_parameters > utlAT_MAX_PARAMETERS)
					{
						utlError("Command '%s' has too many parameters (%d).", command_p->name_p, command_p->num_parameters);
						return -1;
					}
					/*--- for each parameter... ---*/
					term_parameter_p = command_p->parameters_p + command_p->num_parameters;
					for ( parameter_p = command_p->parameters_p; parameter_p < term_parameter_p; parameter_p++)
					{
						if ((parameter_p->type != utlAT_DATA_TYPE_DECIMAL)     &&
						    (parameter_p->type != utlAT_DATA_TYPE_HEXADECIMAL) &&
						    (parameter_p->type != utlAT_DATA_TYPE_BINARY)      &&
						    (parameter_p->type != utlAT_DATA_TYPE_STRING)      &&
						    (parameter_p->type != utlAT_DATA_TYPE_QSTRING)     &&
						    (parameter_p->type != utlAT_DATA_TYPE_DIAL_STRING))
						{
							utlError("Command '%s' has an invalid parameter type.", command_p->name_p);
							return -1;
						}
						if ((parameter_p->access != utlAT_PARAMETER_ACCESS_READ_WRITE) &&
						    (parameter_p->access != utlAT_PARAMETER_ACCESS_READ)       &&
						    (parameter_p->access != utlAT_PARAMETER_ACCESS_READ_ONLY)  &&
						    (parameter_p->access != utlAT_PARAMETER_ACCESS_WRITE_ONLY))
						{
							utlError("Command '%s' specifies an invalid parameter access.", command_p->name_p);
							return -1;
						}

						/*--- check that dial-string parameter is the very last parameter ---*/
						if (( parameter_p->type == utlAT_DATA_TYPE_DIAL_STRING) &&
						    ((parameter_p + 1) != term_parameter_p))
						{
							utlError("Dial-string parameter is not the last parameter in command '%s'.", command_p->name_p);
							return -1;
						}
					}
				}
			}
		}
	}
#endif
	UNUSEDPARAM(commands_p)
	UNUSEDPARAM(num_commands)
	UNUSEDPARAM(arg_p)
	return 0;

}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utOpenAtParser(commands_p, num_commands, arg_p)
* INPUT
*        commands_p == pointer to an array of utlAtCommand_T structures
*      num_commands == number of commands pointed to by `commands_p'
*             arg_p == user-defined argument for call-backs
* OUTPUT
*      none
* RETURNS
*      pointer to an initialized utlAtParser_T structure.
* DESCRIPTION
*      Opens a new AT command parser.
*---------------------------------------------------------------------------*/
utlAtParser_P utlOpenAtParser(const utlAtCommand_P2c commands_p,
			      const size_t num_commands,
			      void         *arg_p)
{
	utlAtParser_P parser_p;

	utlAssert(commands_p   != NULL);
	utlAssert(num_commands >  (size_t)0);

	if (CheckParamForOpenAtParser(commands_p, num_commands, arg_p) < 0)
		return NULL;

	/*--- allocate and initialize a new AT command parser ---*/
	{
		size_t parser_size;

		parser_size = sizeof(utlAtParser_T);

		if ((parser_p = (utlAtParser_P)utlMalloc(parser_size)) == NULL)
			return NULL;

		(void)memset(parser_p, 0, sizeof(*parser_p));

		parser_p->next_p = NULL;

		/*--- info ---*/
		{
			parser_p->info.id_p            = "Marvell";
			parser_p->info.manufacturer_p  = "Marvell";
			parser_p->info.model_p         = "Linux";
			parser_p->info.revision_p      = "1.0";
			parser_p->info.serial_number_p = NULL;
			parser_p->info.object_id_p     = NULL;
			parser_p->info.country_code_p  = NULL;
			parser_p->info.connect_text_p  = NULL;
		}

		/*--- options ---*/
		{
			parser_p->options.allow_default_S_parameter_values = true;
			parser_p->options.allow_string_escapes             = true;
			parser_p->options.use_carrier_result_code          = false;
			parser_p->options.report_each_ring                 = false;
		}

		/*--- call backs ---*/
		{
			parser_p->call_backs.dce_io_config_function_p        = NULL;
			parser_p->call_backs.sound_config_function_p         = NULL;
			parser_p->call_backs.s_parameter_function_p          = NULL;
			parser_p->call_backs.save_dial_string_function_p     = utlAtParserStoreSaveDialString;
			parser_p->call_backs.retrieve_dial_string_function_p = utlAtParserStoreRetrieveDialString;
			parser_p->call_backs.reply_function_p                = NULL;
			parser_p->call_backs.tx_line_data_function_p         = NULL;
			parser_p->call_backs.driver_request_function_p       = NULL;
			parser_p->call_backs.arg_p                           = arg_p;
		}

		parser_p->commands_p   = commands_p;
		parser_p->num_commands = num_commands;

		/*--- parser states ---*/
		{
			utlAtAsyncResponse_P async_response_p;
			utlAtAsyncResponse_P term_async_response_p;

			utlInitLinkedList(&(parser_p->states.async_responses.unused));
			utlInitLinkedList(&(parser_p->states.async_responses.pending));

			term_async_response_p = parser_p->states.async_responses.nodes +
						utlNumberOf(parser_p->states.async_responses.nodes);

			for (async_response_p = parser_p->states.async_responses.nodes;
			     async_response_p < term_async_response_p; async_response_p++)
			{
				async_response_p->parser_p = parser_p;
				async_response_p->timer_id = utlFAILED;

				utlPutTailNode(&(parser_p->states.async_responses.unused),
					       utlAtAsyncResponse_T, async_response_p);
			}
		}

		if (utlAtReset(parser_p) != utlSUCCESS)
		{
			/*--- clean ---*/
			utlFree(parser_p);
			return NULL;
		}

	}

	/*--- apply any pending configuration changes ---*/
	if ((utlAtDceIoConfigEvent(parser_p) != utlSUCCESS) ||
	    (utlAtSoundConfigEvent(parser_p) != utlSUCCESS))
	{
		/*--- clean ---*/
		utlFree(parser_p);

		return NULL;
	}

	/*--- add to list of known AT parsers ---*/
	utlPutTailNode(&parsers, utlAtParser_T, parser_p);

	return parser_p;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlCloseAtParser(parser_p)
* INPUT
*      parser_p == an open AT-command parser
* OUTPUT
*      none
* RETURNS
*      utlSUCCESS
* DESCRIPTION
*      Closes an open AT command parser.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlCloseAtParser(const utlAtParser_P parser_p)
{
	utlAssert(parser_p != NULL);

	/*--- parser states ---*/
	{
		utlAtAsyncResponse_P async_response_p;

		/*--- discard any pending responsess ---*/
		while ((async_response_p = utlGetHeadNode(&(parser_p->states.async_responses.pending), utlAtAsyncResponse_T)) != NULL)
			if (                 async_response_p->timer_id  != (utlTimerId_T) utlFAILED)
			{
				if (utlStopTimer(async_response_p->timer_id) != utlSUCCESS)
					return utlFAILED;

				async_response_p->timer_id = utlFAILED;
			}
	}

	/*--- remove AT parser from list of known parsers ---*/
	if (utlGetNode(&parsers, utlAtParser_T, NULL, parser_p) != parser_p)
		return utlFAILED;

	/*--- destroy parser ---*/
	utlFreeConst(parser_p);

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlAtParserOp(parser_p, op, ...)
*
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_HANG_UP)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_RESET)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_DCE_IO_CONFIG_HANDLER,        dce_io_config_function_p)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_SOUND_CONFIG_HANDLER,         sound_config_function_p)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_S_PARAMETER_HANDLER,          s_parameter_function_p)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_SAVE_DIAL_STRING_HANLDER,     save_dial_string_function_p)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_RETRIEVE_DIAL_STRING_HANLDER, retrieve_dial_string_function_p)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_REPLY_HANDLER,                reply_function_p)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_TX_LINE_DATA_HANDLER,         tx_line_data_function_p)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_DRIVER_REQUEST_HANDLER,       driver_request_function_p)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_ECHO_ON)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_ECHO_OFF)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_SELECT_CARRIER_RESULT_CODE)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_SELECT_CONNECT_RESULT_CODE)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_S_PARAMETER_VALUE, parameter_num, parameter_value)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_GET_S_PARAMETER_VALUE, parameter_num, parameter_value_p)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_ID,            id_p)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_MANUFACTURER,  manufacturer_p)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_MODEL,         model_p)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_REVISION,      revision_p)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_SERIAL_NUMBER, serial_number_p)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_OBJECT_ID,     object_id_p)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_COUNTRY_CODE,  country_code_p)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_CONNECT_TEXT,  connect_text_p)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_ENABLE_BYPASS_MODE)
*      utlAtParserOp(parser_p, utlAT_PARSER_OP_DISABLE_BYPASS_MODE)
* INPUT
*      parser_p == an open AT-command parser
*            op == the operation to perform
*           ... == zero or more operation-specific parameters
* OUTPUT
*      none
* RETURNS
*      utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*      Performs the requested operation on an open At-command parser.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlAtParserOp(const utlAtParser_P parser_p,
			      const utlAtParserOp_T op,
			      ...)
{
	va_list va_arg_p;
	const char   *s_p               = NULL;
	unsigned int parameter_num     = 0;
	unsigned int parameter_value   = 0;
	unsigned int *parameter_value_p = NULL;

	utlAssert(parser_p != NULL);

	va_start(va_arg_p, op);

	/*--- check parameter(s) (where possible) ---*/
	switch (op)
	{
	case utlAT_PARSER_OP_SET_S_PARAMETER_VALUE:
		parameter_num   = va_arg(va_arg_p, unsigned int);
		parameter_value = va_arg(va_arg_p, unsigned int);

		/*--- parameter-number too large? ---*/
		if (parameter_num >= utlNumberOf(parser_p->parameters.S))
		{
			utlError("Invalid S-parameter number %d", parameter_num);
			va_end(va_arg_p);
			return utlFAILED;
		}
		break;

	case utlAT_PARSER_OP_GET_S_PARAMETER_VALUE:
		parameter_num     = va_arg(va_arg_p, unsigned int);
		parameter_value_p = va_arg(va_arg_p, unsigned int *);

		/*--- parameter-number too large? ---*/
		if (parameter_num >= utlNumberOf(parser_p->parameters.S))
		{
			utlError("Invalid S-parameter number %d", parameter_num);
			va_end(va_arg_p);
			return utlFAILED;
		}

		/*--- bad pointer ---*/
		if (parameter_value_p == NULL)
		{
			utlError("Invalid S-parameter value pointer");
			va_end(va_arg_p);
			return utlFAILED;
		}
		break;

	case utlAT_PARSER_OP_SET_ID:
	case utlAT_PARSER_OP_SET_MANUFACTURER:
	case utlAT_PARSER_OP_SET_MODEL:
	case utlAT_PARSER_OP_SET_REVISION:
	case utlAT_PARSER_OP_SET_SERIAL_NUMBER:
	case utlAT_PARSER_OP_SET_OBJECT_ID:
		s_p = va_arg(va_arg_p, const char *);

		/*--- is string parameter too long? ---*/
		if (strlen(s_p) > (size_t)2048)
		{
			utlError("Character string parameter is too long (> 2048 characters).");

			/*--- clean ---*/
			va_end(va_arg_p);

			return utlFAILED;
		}

		/*--- does string parameter contain reserved character sequences? ---*/
		if ((    strstr(s_p, "0\r")  != NULL) ||
		    (strcasestr(s_p, "OK\r") != NULL))
		{
			utlError("Character string contains reserved character sequences (0<cr> or OK<cr>).");

			/*--- clean ---*/
			va_end(va_arg_p);

			return utlFAILED;
		}
		break;

	case utlAT_PARSER_OP_SET_COUNTRY_CODE:
		s_p = va_arg(va_arg_p, const char *);

		/*--- Country codes are encoded using T.35 8-bit country codes, and
		   consist of a single 2-digit hexadecimal value, or multple comma-
		   separated 2-digit hexadecimal values all enclosed in parenthesis.
		   Some sample country code strings are shown below:
		   "20"
		   "(20,B5,00)"

		   Some T.35 country codes:
		   20 == Canada
		   3C == Finland
		   3D == France
		   42 == Germany
		   50 == Hongkong
		   58 == Israel
		   00 == Japan
		   61 == Korea
		   FE == Taiwan
		   A9 == Thailand
		   B4 == UK
		   B5 == USA */

		/*--- is string parameter too long? ---*/
		if (strlen(s_p) > (size_t)2048)
		{
			utlError("Character string parameter is too long (> 2048 characters).");

			/*--- clean ---*/
			va_end(va_arg_p);

			return utlFAILED;
		}

		/*--- check that syntax is of the form: (hh,hh,hh,..,hh) ---*/
		if (s_p[0] == '(')
		{
			const char *c_p;

			c_p = s_p;

			/*--- ensure there's an opening parenthesis ---*/
			if (*c_p != '(')
			{
				utlError("Invalid country-code syntax.");

				/*--- clean ---*/
				va_end(va_arg_p);

				return utlFAILED;
			}

			for (c_p++; *c_p != '\0'; c_p++)
			{
				/*--- check 2-digit hexadecimal value ---*/
				if ((((c_p[0] < '0') || (c_p[0] > '9')) &&
				     ((c_p[0] < 'A') || (c_p[0] > 'F')) &&
				     ((c_p[0] < 'a') || (c_p[0] > 'f'))) ||
				    (((c_p[1] < '0') || (c_p[1] > '9')) &&
				     ((c_p[1] < 'A') || (c_p[1] > 'F')) &&
				     ((c_p[1] < 'a') || (c_p[1] > 'f'))))
				{
					utlError("Invalid country-code syntax.");

					/*--- clean ---*/
					va_end(va_arg_p);

					return utlFAILED;
				}

				c_p += 2;

				if (*c_p == ')')
					break;
				else if (*c_p != ',')
				{
					utlError("Invalid country-code syntax.");

					/*--- clean ---*/
					va_end(va_arg_p);

					return utlFAILED;
				}
			}

			/*--- ensure there's a closing parenthesis ---*/
			if (*c_p != ')')
			{
				utlError("Invalid country-code syntax.");

				/*--- clean ---*/
				va_end(va_arg_p);

				return utlFAILED;
			}

			/*--- check that syntax of single values have the form: hh ---*/
		}
		else
		{
			/*--- check 2-digit hexadecimal value ---*/
			if ((((s_p[0] < '0') || (s_p[0] > '9')) &&
			     ((s_p[0] < 'A') || (s_p[0] > 'F')) &&
			     ((s_p[0] < 'a') || (s_p[0] > 'f'))) ||
			    (((s_p[1] < '0') || (s_p[1] > '9')) &&
			     ((s_p[1] < 'A') || (s_p[1] > 'F')) &&
			     ((s_p[1] < 'a') || (s_p[1] > 'f'))) ||
			    ( s_p[2] != '\0'))
			{
				utlError("Invalid country-code syntax.");

				/*--- clean ---*/
				va_end(va_arg_p);

				return utlFAILED;
			}
		}
		break;

	case utlAT_PARSER_OP_SET_CONNECT_TEXT:
		s_p = va_arg(va_arg_p, const char *);

		/*--- is string parameter too long? ---*/
		if (strlen(s_p) > (size_t)utlAT_MAX_CONNECT_TEXT_LENGTH)
		{
			utlError("Character string parameter is too long (> %d characters).", utlAT_MAX_CONNECT_TEXT_LENGTH);

			/*--- clean ---*/
			va_end(va_arg_p);

			return utlFAILED;
		}
		break;

	default:
		break;
	}

	/*--- execute operation ---*/
	switch (op)
	{
	case utlAT_PARSER_OP_HANG_UP:

		break;

	case utlAT_PARSER_OP_RESET:

		break;

	case utlAT_PARSER_OP_SET_DCE_IO_CONFIG_HANDLER:
		parser_p->call_backs.dce_io_config_function_p = va_arg(va_arg_p, utlAtDceIoConfigFunction_P);

		/*--- notify application of current I/O config settings ---*/
		if (parser_p->call_backs.dce_io_config_function_p != NULL)
		{
			if (utlAtDceIoConfigEvent(parser_p) != utlSUCCESS)
			{
				/*--- clean ---*/
				va_end(va_arg_p);

				return utlAT_RESULT_CODE_ERROR;
			}
		}
		break;

	case utlAT_PARSER_OP_SET_SOUND_CONFIG_HANDLER:
		parser_p->call_backs.sound_config_function_p = va_arg(va_arg_p, utlAtSoundConfigFunction_P);

		/*--- notify application of current sound config settings ---*/
		if (parser_p->call_backs.sound_config_function_p != NULL)
		{
			if (utlAtSoundConfigEvent(parser_p) != utlSUCCESS)
			{
				/*--- clean ---*/
				va_end(va_arg_p);

				return utlAT_RESULT_CODE_ERROR;
			}
		}
		break;

	case utlAT_PARSER_OP_SET_S_PARAMETER_HANDLER:
		parser_p->call_backs.s_parameter_function_p = va_arg(va_arg_p, utlAtSParameterFunction_P);

		/*--- notify application of current S-parameter settings ---*/
		if (parser_p->call_backs.s_parameter_function_p != NULL)
		{
			size_t i;

			for (i = 0; i < utlAT_NUM_S_PARAMETERS; i++)
				if ((parser_p->call_backs.s_parameter_function_p)(i, parser_p->parameters.S[i], parser_p->call_backs.arg_p) != utlSUCCESS)
				{

					/*--- clean ---*/
					va_end(va_arg_p);

					return utlFAILED;
				}
		}
		break;

	case utlAT_PARSER_OP_SET_REPLY_HANDLER:
	{
		utlAtReplyFunction_P reply_function_p;

		reply_function_p = va_arg(va_arg_p, utlAtReplyFunction_P);

		/*--- when the first handler is set, emit an "OK" message ---*/
		if ((parser_p->call_backs.reply_function_p == NULL) && (reply_function_p != NULL))
		{
			parser_p->call_backs.reply_function_p = reply_function_p;

			/* Fix me: We don't need to emit "OK" here */
			//if (utlSendBasicSyntaxResultCode(parser_p, utlAT_RESULT_CODE_OK, false) != utlSUCCESS)
			//	return utlFAILED;

		}
		else
			parser_p->call_backs.reply_function_p = reply_function_p;
	}
	break;

	case utlAT_PARSER_OP_SET_SAVE_DIAL_STRING_HANDLER:     parser_p->call_backs.save_dial_string_function_p     = va_arg(va_arg_p, utlAtSaveDialStringFunction_P);      break;
	case utlAT_PARSER_OP_SET_RETRIEVE_DIAL_STRING_HANDLER: parser_p->call_backs.retrieve_dial_string_function_p = va_arg(va_arg_p, utlAtRetrieveDialStringFunction_P);  break;
	case utlAT_PARSER_OP_SET_TX_LINE_DATA_HANDLER:         parser_p->call_backs.tx_line_data_function_p         = va_arg(va_arg_p, utlAtTxLineDataFunction_P);          break;
	case utlAT_PARSER_OP_SET_DRIVER_REQUEST_HANDLER:       parser_p->call_backs.driver_request_function_p       = va_arg(va_arg_p, utlAtDriverRequestFunction_P);       break;

	case utlAT_PARSER_OP_ECHO_ON:  parser_p->parameters.echo_text = true;   break;
	case utlAT_PARSER_OP_ECHO_OFF: parser_p->parameters.echo_text = false;  break;

	case utlAT_PARSER_OP_SELECT_CARRIER_RESULT_CODE: parser_p->options.use_carrier_result_code = true;   break;
	case utlAT_PARSER_OP_SELECT_CONNECT_RESULT_CODE: parser_p->options.use_carrier_result_code = false;  break;

	case utlAT_PARSER_OP_SET_S_PARAMETER_VALUE:
		/*--- apply S-parameter value change ---*/
		if (utlProcessSParameter(parser_p, parameter_num, parameter_value) != utlSUCCESS)
		{
			va_end(va_arg_p);
			return utlAT_RESULT_CODE_ERROR;
		}

		/*--- notify application of S-parameter value change ---*/
		if (     parser_p->call_backs.s_parameter_function_p != NULL)
			if ((parser_p->call_backs.s_parameter_function_p)(parameter_num,
									  parameter_value,
									  parser_p->call_backs.arg_p) != utlSUCCESS)
		{
				va_end(va_arg_p);
				return utlAT_RESULT_CODE_ERROR;
		}

		/*--- save new S-parameter value ---*/
		parser_p->parameters.S[parameter_num] = parameter_value;
		break;

	case utlAT_PARSER_OP_GET_S_PARAMETER_VALUE:
		/*--- fetch value ---*/
		*parameter_value_p = parser_p->parameters.S[parameter_num];
		break;

	case utlAT_PARSER_OP_REPORT_EACH_RING:       parser_p->options.report_each_ring = true;   break;
	case utlAT_PARSER_OP_ONLY_REPORT_FIRST_RING: parser_p->options.report_each_ring = false;  break;

	case utlAT_PARSER_OP_SET_ID:              parser_p->info.id_p            = s_p;    break;
	case utlAT_PARSER_OP_SET_MANUFACTURER:    parser_p->info.manufacturer_p  = s_p;    break;
	case utlAT_PARSER_OP_SET_MODEL:           parser_p->info.model_p         = s_p;    break;
	case utlAT_PARSER_OP_SET_REVISION:        parser_p->info.revision_p      = s_p;    break;
	case utlAT_PARSER_OP_SET_SERIAL_NUMBER:   parser_p->info.serial_number_p = s_p;    break;
	case utlAT_PARSER_OP_SET_OBJECT_ID:       parser_p->info.object_id_p     = s_p;    break;
	case utlAT_PARSER_OP_SET_COUNTRY_CODE:    parser_p->info.country_code_p  = s_p;    break;
	case utlAT_PARSER_OP_SET_CONNECT_TEXT:    parser_p->info.connect_text_p  = s_p;    break;
	case utlAT_PARSER_OP_ENABLE_BYPASS_MODE:  parser_p->states.bypass_mode   = true;   break;
	case utlAT_PARSER_OP_DISABLE_BYPASS_MODE: parser_p->states.bypass_mode   = false;  break;

	default:
		utlError("Invalid op-code %d.", op);

		/*--- clean ---*/
		va_end(va_arg_p);

		return utlFAILED;
	}

	va_end(va_arg_p);

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlSendString(parser_p, string_p)
* INPUT
*      parser_p == an open AT-command parser
*      string_p == pointer to the string to send expressed as a null-
*                  terminated character string
* OUTPUT
*      none
* RETURNS
*      utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*      Sends the specified null-terminated reply string to the DTE.
*---------------------------------------------------------------------------*/
static utlReturnCode_T utlSendString(const utlAtParser_P2c parser_p,
				     const char            *string_p)
{
	utlAssert(parser_p != NULL);
	utlAssert(string_p != NULL);

	/*--- is bypass mode enabled? ---*/
	if (parser_p->states.bypass_mode)
		return utlSUCCESS;

	/*--- nothing to send? ---*/
	if (*string_p == '\0')
		return utlSUCCESS;

	/*--- invoke reply call-back (if any) ---*/
	if (        parser_p->call_backs.reply_function_p != NULL)
		return (parser_p->call_backs.reply_function_p)(string_p, parser_p->call_backs.arg_p);

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlSendSubstring(parser_p, string_p, len)
* INPUT
*      parser_p == an open AT-command parser
*      string_p == pointer to the string to send
*           len == number of characters to send, starting from the character
*                  pointed to by `string_p'
* OUTPUT
*      none
* RETURNS
*      utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*      Sends the specified reply sub-string to the DTE.
*---------------------------------------------------------------------------*/
static utlReturnCode_T utlSendSubstring(const utlAtParser_P2c parser_p,
					const char            *string_p,
					size_t len)
{
	char buf[utlAT_MAX_RESPONSE_LENGTH];
	const char *c_p;

	utlAssert(parser_p != NULL);
	utlAssert(string_p != NULL);

	/*--- is bypass mode enabled? ---*/
	if (parser_p->states.bypass_mode)
		return utlSUCCESS;

	/*--- no reply call-back defined? ---*/
	if (parser_p->call_backs.reply_function_p == NULL)
		return utlSUCCESS;

	c_p = string_p;

	/*--- while there's more data to send... ---*/
	while (len > (size_t)0)
	{
		size_t n;

		if (len >= utlNumberOf(buf)) n = utlNumberOf(buf) - 1;
		else n = len;

		/*--- copy data into buffer, filtering out illegal characters ---*/
		{
			const char *src_p;
			const char *term_src_p;
			char *dest_p;

			src_p = c_p;
			term_src_p = c_p + n;
			for (dest_p = buf; src_p < term_src_p; src_p++)
				/*--- only process characters that are valid in AT-commands... ---*/
				if ((*src_p >= '\040') ||
				    (*src_p == parser_p->parameters.S[utlAT_ESCAPE_CHAR])     ||
				    (*src_p == parser_p->parameters.S[utlAT_LINE_TERM_CHAR])  ||
				    (*src_p == parser_p->parameters.S[utlAT_FORMATTING_CHAR]) ||
				    (*src_p == parser_p->parameters.S[utlAT_LINE_EDIT_CHAR])  ||
				    (*src_p == parser_p->parameters.S[utlAT_SEPARATER_CHAR])  ||
				    (*src_p == parser_p->parameters.S[utlAT_TERMINATER_CHAR]))
					*dest_p++ = *src_p;
			*dest_p = '\0';
		}

		/*--- invoke reply call-back ---*/
		if ((parser_p->call_backs.reply_function_p)(buf, parser_p->call_backs.arg_p) != utlSUCCESS)
			return utlFAILED;

		len -= n;
		c_p += n;
	}

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlSendInfoResponse(parser_p, string_p)
* INPUT
*      parser_p == an open AT-command parser
*      string_p == pointer to the info-text response expressed as a null-
*                  terminated character string
* OUTPUT
*      none
* RETURNS
*      utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*      Sends the specified info-response string to the DTE.
*---------------------------------------------------------------------------*/
static utlReturnCode_T utlSendInfoResponse(const utlAtParser_P2c parser_p,
					   const char            *string_p)
{
	char buf[utlAT_MAX_RESPONSE_LENGTH];
	size_t len;
	size_t i;

	utlAssert(parser_p != NULL);
	utlAssert(string_p != NULL);

	len = strlen(string_p);

	/*--- nothing to send? ---*/
	if (len == (size_t)0)
		return utlSUCCESS;

	/*--- no room? ---*/
	if (len >= (utlNumberOf(buf) - 5))
	{
		utlError("Response-buffer overflow.");
		return utlFAILED;
	}

	i = 0;

	/*--- prefix ---*/
	if (parser_p->parameters.verbose_results == true)
	{
		buf[i++] = parser_p->parameters.S[utlAT_LINE_TERM_CHAR];
		buf[i++] = parser_p->parameters.S[utlAT_FORMATTING_CHAR];
	}

	/*--- text ---*/
	(void)strcpy(buf + i, string_p);
	i += len;

	/*--- suffix ---*/
	buf[i++] = parser_p->parameters.S[utlAT_LINE_TERM_CHAR];
	buf[i++] = parser_p->parameters.S[utlAT_FORMATTING_CHAR];

	buf[i] = '\0';

	return utlSendString(parser_p, buf);
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlSendBasicSyntaxResultCode(parser_p, code, ignore_state)
* INPUT
*          parser_p == an open AT-command parser
*              code == a result code
*      ignore_state == ignore current state?
* OUTPUT
*      none
* RETURNS
*      utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*      Sends the specified basic-AT-Command result code to the DTE.
*---------------------------------------------------------------------------*/
static utlReturnCode_T utlSendBasicSyntaxResultCode(const utlAtParser_P2c parser_p,
						    const utlAtResultCode_T code,
						    const bool ignore_state)
{
	UNUSEDPARAM(ignore_state)

	char buf[40];
	size_t i;

	utlAssert(parser_p != NULL);

	/*--- suppress result code? ---*/
	if ((parser_p->parameters.suppress_results == true) ||
	    (code == utlAT_RESULT_CODE_SUPPRESS))
		return utlSUCCESS;

	if (utlAcquireExclusiveAccess(&((utlAtParser_P)parser_p)->cmd_cnt_semaphore) != utlSUCCESS)
	{
		utlError("%s: Cannot exclusively acquire semaphore!\n", __FUNCTION__);
		return utlFAILED;
	}

	/* For multiple AT commands in one line, only for the last AT command, we can return OK */
	if (code == utlAT_RESULT_CODE_OK && parser_p->commands_in_line != 0)
	{
		utlReleaseExclusiveAccess(&((utlAtParser_P)parser_p)->cmd_cnt_semaphore);
		return utlSUCCESS;
	}

	utlReleaseExclusiveAccess(&((utlAtParser_P)parser_p)->cmd_cnt_semaphore);

	if (parser_p->parameters.verbose_results == true)
	{
		const char *s_p;

		switch (code)
		{
		case utlAT_RESULT_CODE_CONNECT:
			if (parser_p->options.use_carrier_result_code == true) s_p = "CARRIER";
			else s_p = "CONNECT";
			break;

		case utlAT_RESULT_CODE_OK:           s_p = "OK";            break;
		case utlAT_RESULT_CODE_RING:         s_p = "RING";          break;
		case utlAT_RESULT_CODE_NO_CARRIER:   s_p = "NO CARRIER";    break;
		case utlAT_RESULT_CODE_ERROR:        s_p = "ERROR";         break;
		case utlAT_RESULT_CODE_NO_DIAL_TONE: s_p = "NO DIAL TONE";  break;
		case utlAT_RESULT_CODE_BUSY:         s_p = "BUSY";          break;
		case utlAT_RESULT_CODE_NO_ANSWER:    s_p = "NO ANSWER";     break;
		default:
			utlError("Invalid result code %d.", code);
			return utlFAILED;
		}

		i = 0;

		/*--- prefix ---*/
		buf[i++] = parser_p->parameters.S[utlAT_LINE_TERM_CHAR];
		buf[i++] = parser_p->parameters.S[utlAT_FORMATTING_CHAR];

		/*--- text ---*/
		{
			(void)strcpy(buf + i, s_p);
			i =    strlen(buf);

			/*--- if we're connecting, insert connect text (if configured) ---*/
			if ((parser_p->parameters.include_connect_text == true) &&
			    (code == utlAT_RESULT_CODE_CONNECT) &&
			    (parser_p->info.connect_text_p != NULL))
			{
				buf[i++] = ' ';
				buf[i++] = '(';

				(void)strncpy(buf + i, parser_p->info.connect_text_p, utlNumberOf(buf) - 8);
				i += strlen(parser_p->info.connect_text_p);

				buf[i++] = ')';
			}
		}

		/*--- suffix ---*/
		buf[i++] = parser_p->parameters.S[utlAT_LINE_TERM_CHAR];
		buf[i++] = parser_p->parameters.S[utlAT_FORMATTING_CHAR];
		buf[i]   = '\0';

	}
	else
	{
		i = utlDecimalToString(buf, code, utlNumberOf(buf) - 3);

		buf[i++] = parser_p->parameters.S[utlAT_LINE_TERM_CHAR];
		buf[i++] = parser_p->parameters.S[utlAT_FORMATTING_CHAR];
		buf[i]   = '\0';
	}

	return utlSendString(parser_p, buf);
}

static utlReturnCode_T __utlAtCommandResponse(utlAtParser_P parser_p,
				     utlAtCommand_P2c command_p,
				     const utlAtCommandResponseType_T response_type,
				     va_list va_arg_p)
{
	utlAtParameterValue_P2c parameter_values_p = NULL;
	size_t num_parameters     = 0;
	const char              *custom_p           = NULL;
	const char              *info_text_p        = NULL;
	const char              *abort_response_p   = NULL;
	const char              *command_syntax_p   = NULL;

	/*--- fetch remaining parameters... ---*/
	switch (response_type)
	{
	case utlAT_COMMAND_RESPONSE_TYPE_PARAMETERS:
		parameter_values_p = va_arg(va_arg_p, utlAtParameterValue_P2c);
		num_parameters     = va_arg(va_arg_p, size_t);

		/*--- verify parameter count ---*/
		if ((num_parameters != command_p->num_parameters) ||
		    (num_parameters  > utlAT_MAX_PARAMETERS))
		{
			utlError("Invalid number of AT command parameters: %d", num_parameters);

			return utlFAILED;
		}

		/*--- verify parameter types ---*/
		{
			utlAtParameter_P parameter_p;
			utlAtParameterValue_P2c parameter_value_p;

			parameter_p       = command_p->parameters_p;
			parameter_value_p = parameter_values_p;

			/*--- for each of this AT-Command's known parameters... ---*/
			while (parameter_p < (command_p->parameters_p + command_p->num_parameters))
			{
				if (parameter_p->type != parameter_value_p->type)
				{
					utlError("Invalid data type for parameter %d", parameter_p - command_p->parameters_p);

					return utlFAILED;
				}

				parameter_p++;
				parameter_value_p++;
			}
		}
		break;

	case utlAT_COMMAND_RESPONSE_TYPE_CUSTOM:
		custom_p = va_arg(va_arg_p, const char *);
		break;

	case utlAT_COMMAND_RESPONSE_TYPE_INFO_TEXT:
		info_text_p = va_arg(va_arg_p, const char *);
		break;

	case utlAT_COMMAND_RESPONSE_TYPE_COMMAND_SYNTAX:
		command_syntax_p = va_arg(va_arg_p, char *);
		break;

	case utlAT_COMMAND_RESPONSE_TYPE_REQUEST_COMPLETED:
		break;

	case utlAT_COMMAND_RESPONSE_TYPE_ABORT:
		abort_response_p = va_arg(va_arg_p, const char *);
		break;

	case utlAT_COMMAND_RESPONSE_TYPE_ERROR:
		/* do nothing */
		break;

	default:
		utlError("Invalid response-type %d.", response_type);

		return utlFAILED;
	}

	/*--- generate reply ---*/
	switch (response_type)
	{
	case utlAT_COMMAND_RESPONSE_TYPE_PARAMETERS:
		if (utlGenerateFormattedGetResponse(parser_p, command_p->name_p, parameter_values_p, num_parameters) != utlSUCCESS)
		{
			return utlFAILED;
		}

		if (utlSendBasicSyntaxResultCode(parser_p, utlAT_RESULT_CODE_OK, false) != utlSUCCESS)
			return utlFAILED;
		break;

	case utlAT_COMMAND_RESPONSE_TYPE_CUSTOM:
		if (utlSendInfoResponse(parser_p, custom_p) != utlSUCCESS)
		{
			return utlFAILED;
		}

		if (utlSendBasicSyntaxResultCode(parser_p, utlAT_RESULT_CODE_SUPPRESS, false) != utlSUCCESS)
			return utlFAILED;

		break;

	case utlAT_COMMAND_RESPONSE_TYPE_INFO_TEXT:
		if (utlSendInfoResponse(parser_p, info_text_p) != utlSUCCESS)
		{
			return utlFAILED;
		}

		if (utlSendBasicSyntaxResultCode(parser_p, utlAT_RESULT_CODE_OK, false) != utlSUCCESS)
			return utlFAILED;
		break;

	case utlAT_COMMAND_RESPONSE_TYPE_COMMAND_SYNTAX:
		if (utlSendInfoResponse(parser_p, command_syntax_p) != utlSUCCESS)
		{
			return utlAT_RESULT_CODE_ERROR;
		}

		if (utlSendBasicSyntaxResultCode(parser_p, utlAT_RESULT_CODE_OK, false) != utlSUCCESS)
			return utlFAILED;
		break;

	case utlAT_COMMAND_RESPONSE_TYPE_REQUEST_COMPLETED:

		if (utlSendBasicSyntaxResultCode(parser_p, utlAT_RESULT_CODE_OK, false) != utlSUCCESS)
			return utlFAILED;
		break;

	case utlAT_COMMAND_RESPONSE_TYPE_ABORT:
	case utlAT_COMMAND_RESPONSE_TYPE_ERROR:
		/*--- if we are in a state that requires state-machine intervention ---*/

		/*--- just echo failure string ---*/
		if (response_type == utlAT_COMMAND_RESPONSE_TYPE_ABORT)
		{
			if (utlSendInfoResponse(parser_p, abort_response_p) != utlSUCCESS)
			{
				return utlFAILED;
			}

		}
		else
		{
			if (utlSendBasicSyntaxResultCode(parser_p, utlAT_RESULT_CODE_ERROR, false) != utlSUCCESS)
			{
				return utlFAILED;
			}

		}
		break;

	default:
		utlError("Invalid response-type %d.", response_type);
		return utlFAILED;
	}

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlAtCommandResponse(xid, response_type, ...)
*
*      utlAtCommandResponse(xid, utlAT_COMMAND_RESPONSE_TYPE_PARAMETERS,     parameter_values_p, num_parameters)
*      utlAtCommandResponse(xid, utlAT_COMMAND_RESPONSE_TYPE_CUSTOM,         custom_p)
*      utlAtCommandResponse(xid, utlAT_COMMAND_RESPONSE_TYPE_INFO_TEXT,      info_text_p)
*      utlAtCommandResponse(xid, utlAT_COMMAND_RESPONSE_TYPE_COMMAND_SYNTAX, command_syntax_p)
*      utlAtCommandResponse(xid, utlAT_COMMAND_RESPONSE_TYPE_REQUEST_COMPLETED)
*      utlAtCommandResponse(xid, utlAT_COMMAND_RESPONSE_TYPE_ABORT,          abort_response_p)
*      utlAtCommandResponse(xid, utlAT_COMMAND_RESPONSE_TYPE_SYNC_ERROR)
* INPUT
*                xid == transaction ID
*      response_type == the type of response being reported
*                ... == type-specific parameters
* OUTPUT
*      none
* RETURNS
*      utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*      Forwards an AT-command response to the At-command parser waiting for
*      `xid'.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlAtCommandResponse(const unsigned int xid,
				     const utlAtCommandResponseType_T response_type,
				     ...)
{
	va_list va_arg_p;
	utlAtParser_P parser_p;
	utlAtAsyncResponse_P async_response_p;
	utlAtCommand_P2c command_p;
	utlReturnCode_T         ret                 = utlFAILED;

	/*--- ignore command response if transaction ID is 0 or 1 (both are reserved) ---*/
	if ((xid == (unsigned int)0) ||
	    (xid == (unsigned int)1))
		return utlSUCCESS;


	/*FUTURE
	   oxoxoxoxoxoxoxoxoxoxoxoxoxoxoxoxoxoxoxo
	   Interleaving requests...

	   Currently:
	   - We get characters and place them into a circular queue (line accumulation).
	   Once the circular queue contains a complete AT-command, we echo it, and then invoke the call-back
	   function for that command (if any).  If call-back is synchronous, we immediately emit the response.
	   if call-back is asynchronous, we wait until we get the response, at which time we emit the response.
	   if another AT-command arrives, we place the characters into the circular queue and wait until we've
	   echoed the asynchronous response from the current command.

	   Want:
	   - The ability to have multiple asynchronous AT command call-backs outstanding at any given time.

	   Thoughts:
	   - We already have the technology to queue AT-commands, I would like to keep that feature in order to
	   limit the max number of active AT commands.
	   - AT-command echoing is done very early.  I some how need to defer this.
	   - Typical sequence:
	   AT-command 1
	   echo "AT-command 1"
	   AT-command 2
	   queue "AT-command 2
	   AT-command 1 response"
	   echo "AT-command 1 response"
	   echo "AT-command 2
	   AT-command 2 response"
	   echo "AT-command 2 response"
	   AT-command 3
	   echo "AT-command 3"
	   - "parser_p->states.async_response" must be converted into an array AT-commands awaiting an async response.
	   There is no longer a single await-async timer.  Instead there is one for each outstanding async command.
	   I need to keep track of what is pending an echo.  DONE
	   - Need to keep track of echoed back-space chars too.
	   - Right now echoing and processing are done at the same time.  I need to separate these.  Instead of echoing
	   via "utlSendSubstring()", I could queue it for future echoing.  But how to demark what gets echoed from what
	   gets emitted due to replies?
	   What if each "await" node had a pointer to where to echo up to (where that command ends). There would then
	   be two head pointers, one indicating how far we've processed, and another indicating where we should stop
	   echoing.
	   - when issuing new commands while existing ones are not done, don't allow new ones with the same OP to
	   be issued (to prevent abiguity)

	   ----------------------------------

	   Solution 1: Receive AT command and issue call-back
	   immediately, queue echos and responses.

	   Synchronous: Call-back function returns and specifies the contents of
	   the response, we just format the response and then echo it.
	   We need to hold-back the echo'ed text too.

	   Asynchronous: Call-back function starts the command rolling.  We then
	   wait for the driver event containing the contents of the
	   response, wich we format and echo.
	   We need to hold-back the echo'ed text too.

	   Solution 2:
	   Receive AT command and issue call-back immediately.  Only echo command
	   once we've received the response.  We'll need to preserve command ordering somehow.

	   Solution 3:
	   Every time we issue a command, create a node and add to pending-commands list.
	   Every time we receive a response, match the response to a pending command in the
	   pending commands list.  Every time we receive a response, check the pending-commands
	   list for finished commands--echo any that are found.  Note that the pending-commands
	   list must also contain the command-string to be echo'ed.

	   Could have two types of command nodes: waiting for an AT-command reply, strings
	   that need to be echoed.  Nodes must be ordered.  Can echo strings at the head
	   of the list immediately.  Can only echo AT-command reply nodes once the AT command
	   reply has been received.

		Notes:
		- We want a command like ATH to be able to abort a pending ATD command.
		- Can all commands be issued while a current one is pending?
		- How many commands will we allow to be pending at the same time?
		- I will need a separate async_response structure for each pending command.
		- Timeout function needs to know which async_response structure has timed out.
		"arg_p" structure should point to async_response function, not parser structure.
		-
		oxoxoxoxoxoxoxoxoxoxoxoxoxoxoxoxoxoxoxo

		Notes:
		- Echoed text will always be in the correct order, we just need to insert the responses into the right places
		- Responses are a problem.  They can arrive in any order.
		- I need to match up the responses to the associated requests.  When the xid is assigned (first by this library,
				then later optionally by the call-back), I need to save it with the text that needs to be echoed
		- Sequence:
		receive text, echo into a text-fragment node
		assign xid, invoke call-back (which may modify the xid)
		save xid to text-fragment node
		receive reply text, place into a new text-fragment node
		insertion sort new text-fragment node into list of waiting text fragments


		typedef struct utlTextFragment_S *utlTextFragment_P;
	   typedef struct utlTextFragment_S {
		utlTextFragment_P next_p;
		unsigned int      xid;
		char              text[100];
	   } utlTextFragment_T;

	   struct {
		utlLinkedList_T text_fragments;
	   } pending_text;

	   oxoxoxoxoxoxoxoxoxoxoxoxoxoxoxoxoxoxoxo
	 */

	/*--- determine the parser and async_response node associated with the given transaction ID ---*/
	{
		/*--- for each parser... ---*/
		for (parser_p = (utlAtParser_P)parsers.head_p; parser_p != NULL; parser_p = parser_p->next_p)
		{

			/*--- for each async response this parser is waiting for... ---*/
			for (async_response_p = (utlAtAsyncResponse_P)parser_p->states.async_responses.pending.head_p; async_response_p != NULL; async_response_p = async_response_p->next_p)
				if (async_response_p->xid == xid)
					break;

			/*--- found it? ---*/
			if (async_response_p != NULL)
				break;
		}

		/*--- is XID invalid/unknown? ---*/
		if (parser_p == NULL)
		{
			/*	utlError("Invalid/unknown transaction ID %d\n", xid);  */
			return utlFAILED;
		}
	}

	utlAssert(async_response_p != NULL);

	/*--- if we're not expecting this response, we'll ignore it ---*/
	switch (response_type)
	{
	case utlAT_COMMAND_RESPONSE_TYPE_PARAMETERS:
		if (async_response_p->op != utlAT_ASYNC_OP_GET)
			return utlSUCCESS;
		break;

	case utlAT_COMMAND_RESPONSE_TYPE_CUSTOM:
	case utlAT_COMMAND_RESPONSE_TYPE_INFO_TEXT:
		if ((async_response_p->op != utlAT_ASYNC_OP_EXEC) &&
		    (async_response_p->op != utlAT_ASYNC_OP_GET)  &&
		    (async_response_p->op != utlAT_ASYNC_OP_SET)  &&
		    (async_response_p->op != utlAT_ASYNC_OP_ACTION) &&
		    (async_response_p->op != utlAT_ASYNC_OP_SYNTAX))
			return utlSUCCESS;
		break;

	case utlAT_COMMAND_RESPONSE_TYPE_COMMAND_SYNTAX:
		if (async_response_p->op != utlAT_ASYNC_OP_SYNTAX)
			return utlSUCCESS;
		break;

	case utlAT_COMMAND_RESPONSE_TYPE_REQUEST_COMPLETED:
		if ((async_response_p->op != utlAT_ASYNC_OP_EXEC) &&
		    (async_response_p->op != utlAT_ASYNC_OP_SET)  &&
		    (async_response_p->op != utlAT_ASYNC_OP_ACTION))
			return utlSUCCESS;
		break;

	case utlAT_COMMAND_RESPONSE_TYPE_ABORT:
	case utlAT_COMMAND_RESPONSE_TYPE_ERROR:
		if (async_response_p->op == utlAT_ASYNC_OP_UNKNOWN)
			return utlSUCCESS;
		break;

	default:
		utlError("Invalid response type %d.", response_type);
		return utlFAILED;
	}

	/*--- now that we've received an expected asynchronous response, stop timer first ---*/
	{
		/*--- stop timer ---*/
		if (async_response_p->timer_id != (utlTimerId_T)utlFAILED)
		{
			if (utlStopTimer(async_response_p->timer_id) != utlSUCCESS)
				return utlFAILED;

			async_response_p->timer_id = utlFAILED;
		}
	}

	/*--- update commands_in_line ---*/
	if (utlAcquireExclusiveAccess(&parser_p->cmd_cnt_semaphore) != utlSUCCESS)
	{
		utlError("%s: Cannot exclusively acquire semaphore!\n", __FUNCTION__);
		ret = utlFAILED;
		goto exit;
	}

	if (parser_p->commands_in_line == 0)
	{
		utlReleaseExclusiveAccess(&parser_p->cmd_cnt_semaphore);
		ret = utlSUCCESS;
		goto exit;
	}

	if(response_type == utlAT_COMMAND_RESPONSE_TYPE_ABORT 
		|| response_type == utlAT_COMMAND_RESPONSE_TYPE_ERROR)
		parser_p->commands_in_line = 0;
	else
		parser_p->commands_in_line--;

	utlPrintTrace("%s: parser_p->commands_in_line:%d\n", __FUNCTION__, parser_p->commands_in_line);

	utlReleaseExclusiveAccess(&parser_p->cmd_cnt_semaphore);

	/*--- ensure we actually reference a command ---*/
	utlAssert(  async_response_p->command_p != NULL);
	command_p = async_response_p->command_p;
	async_response_p->command_p  = NULL;

	/*--- fetch parameter and send reply ---*/
	va_start(va_arg_p, response_type);
	ret = __utlAtCommandResponse(parser_p, command_p, response_type, va_arg_p);
	va_end(va_arg_p);

exit:
	/*--- update pending list ---*/
	if(response_type == utlAT_COMMAND_RESPONSE_TYPE_ABORT 
		|| response_type == utlAT_COMMAND_RESPONSE_TYPE_ERROR)
	{
		utlAbandonAllPendingAsyncResponse(parser_p);
	}
	else
	{
		if (utlAcquireExclusiveAccess(&parser_p->queue_semaphore) != utlSUCCESS)
		{
			utlError("Cannot exclusively acquire semaphore!\n");
			return utlFAILED;
		}

		if (utlGetNode(    &(parser_p->states.async_responses.pending), utlAtAsyncResponse_T, NULL, async_response_p) != NULL)
			utlPutTailNode(&(parser_p->states.async_responses.unused),  utlAtAsyncResponse_T,       async_response_p);

		utlReleaseExclusiveAccess(&parser_p->queue_semaphore);
	}

	async_response_p->xid = utlFAILED;

	/*--- trigger AT parser again ---*/
	{
		if (triggerAtParser(*(TelAtParserID *)(parser_p->call_backs.arg_p)) < 0)
			return utlFAILED;
	}

	return ret;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlAtDceIoConfigEvent(parser_p)
* INPUT
*      parser_p == an open AT-command parser
* OUTPUT
*      none
* RETURNS
*      utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*      Generates notifications of DCE I/O configuration-changes.
*---------------------------------------------------------------------------*/
static utlReturnCode_T utlAtDceIoConfigEvent(const utlAtParser_P parser_p)
{
	utlAssert(parser_p != NULL);

	/*--- nothing to report? ---*/
	if (parser_p->states.dce_io_config_pending_mask == (unsigned int)0)
		return utlSUCCESS;

	/*--- update dependent fields ---*/
	switch (parser_p->parameters.raw_framing)
	{
	case 0: parser_p->dce_io_config.data_bits   = 0;
		parser_p->dce_io_config.parity_bits = 0;
		parser_p->dce_io_config.stop_bits   = 0;  break;
	case 1: parser_p->dce_io_config.data_bits   = 8;
		parser_p->dce_io_config.parity_bits = 0;
		parser_p->dce_io_config.stop_bits   = 2;  break;
	case 2: parser_p->dce_io_config.data_bits   = 8;
		parser_p->dce_io_config.parity_bits = 1;
		parser_p->dce_io_config.stop_bits   = 1;  break;
	case 3: parser_p->dce_io_config.data_bits   = 8;
		parser_p->dce_io_config.parity_bits = 0;
		parser_p->dce_io_config.stop_bits   = 1;  break;
	case 4: parser_p->dce_io_config.data_bits   = 7;
		parser_p->dce_io_config.parity_bits = 0;
		parser_p->dce_io_config.stop_bits   = 2;  break;
	case 5: parser_p->dce_io_config.data_bits   = 7;
		parser_p->dce_io_config.parity_bits = 1;
		parser_p->dce_io_config.stop_bits   = 1;  break;
	case 6: parser_p->dce_io_config.data_bits   = 7;
		parser_p->dce_io_config.parity_bits = 0;
		parser_p->dce_io_config.stop_bits   = 1;  break;

	default:
		return utlFAILED;
	}
	switch (parser_p->parameters.raw_parity)
	{
	case 0: parser_p->dce_io_config.parity = utlDATA_PARITY_ODD; break;
	case 1: parser_p->dce_io_config.parity = utlDATA_PARITY_EVEN; break;
	case 2: parser_p->dce_io_config.parity = utlDATA_PARITY_MARK; break;
	case 3: parser_p->dce_io_config.parity = utlDATA_PARITY_SPACE;
		break;

	default:
		return utlFAILED;
	}

	/*--- notify I/O subsystem of DCE I/O configuration change ---*/
	if (     parser_p->call_backs.dce_io_config_function_p != NULL)
	{
		if ((parser_p->call_backs.dce_io_config_function_p)(&(parser_p->dce_io_config), parser_p->call_backs.arg_p) != utlSUCCESS)
			return utlFAILED;

		parser_p->states.dce_io_config_pending_mask = 0;
	}

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlAtSoundConfigEvent(parser_p)
* INPUT
*      parser_p == an open AT-command parser
* OUTPUT
*      none
* RETURNS
*      utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*      Generates notifications of sound configuration-changes.
*---------------------------------------------------------------------------*/
static utlReturnCode_T utlAtSoundConfigEvent(const utlAtParser_P parser_p)
{
	utlAssert(parser_p != NULL);

	/*--- nothing to report? ---*/
	if (parser_p->states.sound_config_pending_mask == (unsigned int)0)
		return utlSUCCESS;

	/*--- notify sound subsystem of DCE sound configuration change ---*/
	if (     parser_p->call_backs.sound_config_function_p != NULL)
	{
		if ((parser_p->call_backs.sound_config_function_p)(&(parser_p->sound_config), parser_p->call_backs.arg_p) != utlSUCCESS)
			return utlFAILED;

		parser_p->states.sound_config_pending_mask = 0;
	}

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlProcessSParameter(parser_p, parameter_num, parameter_value)
* INPUT
*             parser_p == an open AT-command parser
*        parameter_num == Number of S-parameter to process
*      parameter_value == S-parameter's value
* OUTPUT
*      none
* RETURNS
*      returns utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*      Processes the specified S-parameter: update dependent fields.
*---------------------------------------------------------------------------*/
static utlAtResultCode_T utlProcessSParameter(const utlAtParser_P parser_p,
					      const unsigned int parameter_num,
					      const unsigned int parameter_value)
{
	utlAssert(parser_p != NULL);

	/*--- check value ---*/
	{
		bool bad_value;

		bad_value = false;

		switch (parameter_num)
		{
		case utlAT_ESCAPE_CHAR:
		case utlAT_SEPARATER_CHAR:
		case utlAT_TERMINATER_CHAR:
		case utlAT_XON_CHAR:
		case utlAT_XOFF_CHAR:
			if (parameter_value > (unsigned int)127)
				bad_value = true;
			break;

		/* for S3, only 13 is valid */
		case utlAT_LINE_TERM_CHAR:
			if (parameter_value != (unsigned int)13)
				bad_value = true;
			break;

		/* for S4, only 10 is valid */
		case utlAT_FORMATTING_CHAR:
			if (parameter_value != (unsigned int)10)
				bad_value = true;
			break;

		/* for S5, only 8 is valid */
		case utlAT_LINE_EDIT_CHAR:
			if (parameter_value != (unsigned int)8)
				bad_value = true;
			break;

		case utlAT_AUTO_ANSWER:
		case utlAT_RING_COUNTER:
		case utlAT_DIALING_PAUSE_TIME:
		case utlAT_ESCAPE_GUARD_TIME:
		case utlAT_DTR_DELAY_TIME:
		case utlAT_HOOK_FLASH_TIME:
		case utlAT_INACTIVITY_TIME:
		case utlAT_DISCONNECT_WAIT_TIME:
			if (parameter_value > (unsigned int)255)
				bad_value = true;
			break;

		case utlAT_BLIND_DIAL_PAUSE_TIME:
			if ((parameter_value < (unsigned int)2) ||
			    (parameter_value > (unsigned int)10))
				bad_value = true;
			break;

		case utlAT_CONN_COMPLETE_TIMEOUT:
		case utlAT_CARRIER_DETECT_TIME:
			if ((parameter_value < (unsigned int)1) ||
			    (parameter_value > (unsigned int)255))
				bad_value = true;
			break;

		/* the range of S11 is 50 - 255 */
		case utlAT_DTMF_TONE_DURATION:
			if ((parameter_value < (unsigned int)50) ||
			    (parameter_value > (unsigned int)255))
				bad_value = true;
			break;

		case utlAT_CARRIER_LOSS_TIME:
			if ((parameter_value < (unsigned int)1) ||
			    (parameter_value > (unsigned int)254))
				bad_value = true;
			break;

		default:
			/*--- will S-parameter value not fit into S-parameter data type? ---*/
			if (parameter_value > ( 1u << ((sizeof(parser_p->parameters.S[0]) * 8u) - 1u)) +
			    ((1u << ((sizeof(parser_p->parameters.S[0]) * 8u) -  1u)) - 1u))
				bad_value = true;
			break;
		}

		if (bad_value == true)
		{
			utlError("Invalid S-parameter value (%d) for S-parameter %d.", parameter_value, parameter_num);
			return utlAT_RESULT_CODE_ERROR;
		}
	}

	/*--- update dependencies (if any) ---*/
	switch (parameter_num)
	{
	case utlAT_AUTO_ANSWER:
		auto_answer_delay_cycles = parameter_value; break;
	case utlAT_BLIND_DIAL_PAUSE_TIME: parser_p->states.blind_dial_pause_time.seconds      =  parameter_value;
		parser_p->states.blind_dial_pause_time.nanoseconds  =  0;                                          break;
	case utlAT_CONN_COMPLETE_TIMEOUT: parser_p->states.conn_complete_timeout.seconds      =  parameter_value;
		parser_p->states.conn_complete_timeout.nanoseconds  =  0;                                          break;
	case    utlAT_DIALING_PAUSE_TIME: parser_p->states.dialing_pause_time.seconds         =  parameter_value;
		parser_p->states.dialing_pause_time.nanoseconds     =  0;                                          break;
	case   utlAT_CARRIER_DETECT_TIME: parser_p->states.carrier_detect_time.seconds        =  parameter_value / 10;
		parser_p->states.carrier_detect_time.nanoseconds    = (parameter_value % 10) * (1000000000 / 10);  break;
	case     utlAT_CARRIER_LOSS_TIME: parser_p->states.carrier_loss_time.seconds          =  parameter_value / 10;
		parser_p->states.carrier_loss_time.nanoseconds      = (parameter_value % 10) * (1000000000 / 10);  break;
	case              utlAT_XON_CHAR: parser_p->dce_io_config.flow_control.xon_char       =  parameter_value;
		parser_p->states.dce_io_config_pending_mask        |= utlAT_DCE_IO_CONFIG_PENDING_FLOW_CONTROL;    break;
	case             utlAT_XOFF_CHAR: parser_p->dce_io_config.flow_control.xoff_char      =  parameter_value;
		parser_p->states.dce_io_config_pending_mask        |= utlAT_DCE_IO_CONFIG_PENDING_FLOW_CONTROL;    break;
	case        utlAT_DTR_DELAY_TIME: parser_p->dce_io_config.dtr_delay_time.seconds      =  parameter_value / 100;
		parser_p->dce_io_config.dtr_delay_time.nanoseconds  =  parameter_value % 100 * (1000000000 / 100);
		parser_p->states.dce_io_config_pending_mask        |= utlAT_DCE_IO_CONFIG_PENDING_DTR_DELAY;       break;
	case       utlAT_HOOK_FLASH_TIME: parser_p->states.hook_flash_time.seconds            =  parameter_value / 10;
		parser_p->states.hook_flash_time.nanoseconds        = (parameter_value % 10) * (1000000000 / 10);  break;
	case       utlAT_INACTIVITY_TIME: parser_p->states.inactivity_time.seconds            =  parameter_value * 10;
		parser_p->states.inactivity_time.nanoseconds        =  0;                                          break;
	case  utlAT_DISCONNECT_WAIT_TIME: parser_p->states.disconnect_wait_time.seconds       =  parameter_value;
		parser_p->states.disconnect_wait_time.nanoseconds   =  0;                                          break;
	default:
		break;
	}

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlProcessBasicAtCommand(parser_p, command_p, parameter_value_p,
*                               response_p, response_siz, syntax_off)
* INPUT
*               parser_p == an open AT-command parser
*              command_p == the AT command to process
*      parameter_value_p == the found parameter value (there's only ever one)
*             response_p == pointer to where the response (if any) should be placed
*           response_siz == length of array pointed to by `response_p'
*             syntax_off == offset from start of line to the AT-command (for
*                           error reporting)
* OUTPUT
*      *response_p == the response (if any)
* RETURNS
*      utlAT_RESULT_CODE_ERROR for failure, other values for success
* DESCRIPTION
*      Processes a basic AT-command.
*---------------------------------------------------------------------------*/
static utlAtResultCode_T utlProcessBasicAtCommand(const utlAtParser_P parser_p,
						  const utlAtCommand_P2c command_p,
						  const utlAtParameterValue_P2c parameter_value_p,
						  char                    *response_p,
						  const size_t response_siz,
						  const size_t syntax_off)
{
	utlAtResultCode_T rc;

	utlAssert(parser_p          != NULL);
	utlAssert(command_p         != NULL);
	utlAssert(parameter_value_p != NULL);
	utlAssert(response_p        != NULL);

	rc = utlAT_RESULT_CODE_OK;

	switch (command_p->name_p[0])
	{
	case 'e':         /*--- command echo ---*/
	case 'E':      if (parameter_value_p->value.decimal == (unsigned int)0) parser_p->parameters.echo_text = false;
		else if (parameter_value_p->value.decimal == (unsigned int)1) parser_p->parameters.echo_text = true;
		else
			rc = utlAT_RESULT_CODE_ERROR;
		break;

	case 'i':         /*--- request identification information ---*/
	case 'I': if (parameter_value_p->value.decimal != (unsigned int)0)
		{
			rc = utlAT_RESULT_CODE_ERROR;
			break;
		}

		(void)strncpy(response_p, parser_p->info.id_p, response_siz);
		break;

	case 'l':         /*--- set monitor-speaker level ---*/
	case 'L': switch (parameter_value_p->value.decimal)
		{
		case 0:
		case 1:
		case 2:
		case 3: parser_p->sound_config.monitor_speaker.level  = parameter_value_p->value.decimal;
			parser_p->states.sound_config_pending_mask   |= utlAT_SOUND_CONFIG_PENDING_LEVEL;
			break;
		default:
			rc = utlAT_RESULT_CODE_ERROR;
			break;
		}
		break;

	case 'm':         /*--- set monitor-speaker mode ---*/
	case 'M': switch (parameter_value_p->value.decimal)
		{
		case 0:
		case 1:
		case 2: parser_p->sound_config.monitor_speaker.mode  = parameter_value_p->value.decimal;
			parser_p->states.sound_config_pending_mask  |= utlAT_SOUND_CONFIG_PENDING_MODE;
			break;
		default:
			rc = utlAT_RESULT_CODE_ERROR;
			break;
		}
		break;

	case 'o':         /*--- enter on-line mode ---*/
	case 'O': if ((parameter_value_p->value.decimal == (unsigned int)0) ||
		      (parameter_value_p->value.decimal == (unsigned int)1))
		{

			parser_p->states.go_on_line = true;
			rc = utlAT_RESULT_CODE_CONNECT;

		}
		else
		{
			rc = utlAT_RESULT_CODE_ERROR;
			break;
		}
		break;

	case 'p':         /*--- select pulse dialing ---*/
	case 'P': if (parameter_value_p->value.decimal != (unsigned int)0)
		{
			rc = utlAT_RESULT_CODE_ERROR;
			break;
		}

		parser_p->parameters.dialing_mode = utlAT_DIALING_MODE_PULSE;
		break;

	case 'q':         /*--- result-code suppression ---*/
	case 'Q':      if (parameter_value_p->value.decimal == (unsigned int)0) parser_p->parameters.suppress_results = false;
		else if (parameter_value_p->value.decimal == (unsigned int)1) parser_p->parameters.suppress_results = true;
		else
		{
			rc = utlAT_RESULT_CODE_ERROR;
			break;
		}
		break;

	case 't':         /*--- select DTMF tone dialing ---*/
	case 'T': if (parameter_value_p->value.decimal != (unsigned int)0)
		{
			rc = utlAT_RESULT_CODE_ERROR;
			break;
		}

		parser_p->parameters.dialing_mode = utlAT_DIALING_MODE_TONE;
		break;

	case 'v':         /*--- DCE response format ---*/
	case 'V':      if (parameter_value_p->value.decimal == (unsigned int)0) parser_p->parameters.verbose_results = false;
		else if (parameter_value_p->value.decimal == (unsigned int)1) parser_p->parameters.verbose_results = true;
		else
		{
			rc = utlAT_RESULT_CODE_ERROR;
			break;
		}
		break;

	case 'x':         /*--- result code selection and call progress monitoring control ---*/
	case 'X': switch (parameter_value_p->value.decimal)
		{
		case 0: parser_p->parameters.include_connect_text    = false;
			parser_p->parameters.detect_dial_tone        = false;
			parser_p->parameters.detect_busy_signal      = false;
			parser_p->states.dce_io_config_pending_mask |= utlAT_DCE_IO_CONFIG_PENDING_DIAL_TONE |
								       utlAT_DCE_IO_CONFIG_PENDING_BUSY_SIGNAL;  break;
		case 1: parser_p->parameters.include_connect_text    = true;
			parser_p->parameters.detect_dial_tone        = false;
			parser_p->parameters.detect_busy_signal      = false;
			parser_p->states.dce_io_config_pending_mask |= utlAT_DCE_IO_CONFIG_PENDING_DIAL_TONE |
								       utlAT_DCE_IO_CONFIG_PENDING_BUSY_SIGNAL;  break;
		case 2: parser_p->parameters.include_connect_text    = true;
			parser_p->parameters.detect_dial_tone        = true;
			parser_p->parameters.detect_busy_signal      = false;
			parser_p->states.dce_io_config_pending_mask |= utlAT_DCE_IO_CONFIG_PENDING_DIAL_TONE |
								       utlAT_DCE_IO_CONFIG_PENDING_BUSY_SIGNAL;  break;
		case 3: parser_p->parameters.include_connect_text    = true;
			parser_p->parameters.detect_dial_tone        = false;
			parser_p->parameters.detect_busy_signal      = true;
			parser_p->states.dce_io_config_pending_mask |= utlAT_DCE_IO_CONFIG_PENDING_DIAL_TONE |
								       utlAT_DCE_IO_CONFIG_PENDING_BUSY_SIGNAL;  break;
		case 4: parser_p->parameters.include_connect_text    = true;
			parser_p->parameters.detect_dial_tone        = true;
			parser_p->parameters.detect_busy_signal      = true;
			parser_p->states.dce_io_config_pending_mask |= utlAT_DCE_IO_CONFIG_PENDING_DIAL_TONE |
								       utlAT_DCE_IO_CONFIG_PENDING_BUSY_SIGNAL;  break;
		default:
			rc = utlAT_RESULT_CODE_ERROR;
			break;
		}
		break;

	case 'z':         /*--- Reset to default configuration ---*/
	case 'Z': if (parameter_value_p->value.decimal != (unsigned int)0)
		{
			rc = utlAT_RESULT_CODE_ERROR;
			break;
		}
		{
			if (utlAcquireExclusiveAccess(&parser_p->cmd_cnt_semaphore) != utlSUCCESS)
			{
				utlError("%s: Cannot exclusively acquire semaphore!\n", __FUNCTION__);
				return utlAT_RESULT_CODE_ERROR;
			}

			size_t commands_in_line = parser_p->commands_in_line;

			utlReleaseExclusiveAccess(&parser_p->cmd_cnt_semaphore);

			if (utlAtReset(parser_p) != utlSUCCESS)
				rc = utlAT_RESULT_CODE_ERROR;

			if (utlAcquireExclusiveAccess(&parser_p->cmd_cnt_semaphore) != utlSUCCESS)
			{
				utlError("%s: Cannot exclusively acquire semaphore!\n", __FUNCTION__);
				return utlAT_RESULT_CODE_ERROR;
			}

			parser_p->commands_in_line = commands_in_line;

			utlReleaseExclusiveAccess(&parser_p->cmd_cnt_semaphore);
		}
		break;

	case '&':         /*--- basic-command prefix character ---*/
		switch (command_p->name_p[1])
		{
		case 'c':                   /*--- DCD (output) control ---*/
		case 'C': switch (parameter_value_p->value.decimal)
			{
			case 0: parser_p->dce_io_config.modes.dcd_always_on  = true;
				parser_p->states.dce_io_config_pending_mask |= utlAT_DCE_IO_CONFIG_PENDING_DCD;  break;
			case 1: parser_p->dce_io_config.modes.dcd_always_on  = false;
				parser_p->states.dce_io_config_pending_mask |= utlAT_DCE_IO_CONFIG_PENDING_DCD;  break;
			default:
				rc = utlAT_RESULT_CODE_ERROR;
				break;
			}
			break;

		case 'd':                   /*--- DTR (input) monitoring ---*/
		case 'D': switch (parameter_value_p->value.decimal)
			{
			case 0: parser_p->dce_io_config.modes.ignore_dtr             = true;
				parser_p->dce_io_config.modes.drop_call_on_dtr_loss  = false;
				parser_p->states.dce_io_config_pending_mask         |= utlAT_DCE_IO_CONFIG_PENDING_DTR;  break;
			case 1: parser_p->dce_io_config.modes.ignore_dtr             = false;
				parser_p->dce_io_config.modes.drop_call_on_dtr_loss  = false;
				parser_p->states.dce_io_config_pending_mask         |= utlAT_DCE_IO_CONFIG_PENDING_DTR;  break;
			case 2: parser_p->dce_io_config.modes.ignore_dtr             = false;
				parser_p->dce_io_config.modes.drop_call_on_dtr_loss  = true;
				parser_p->states.dce_io_config_pending_mask         |= utlAT_DCE_IO_CONFIG_PENDING_DTR;  break;
			default:
				rc = utlAT_RESULT_CODE_ERROR;
				break;
			}
			break;

		case 'f':                   /*--- set to factory configuration ---*/
		case 'F': if (parameter_value_p->value.decimal != (unsigned int)0)
			{
				rc = utlAT_RESULT_CODE_ERROR;
				break;
			}
			{
				if (utlAcquireExclusiveAccess(&parser_p->cmd_cnt_semaphore) != utlSUCCESS)
				{
					utlError("%s: Cannot exclusively acquire semaphore!\n", __FUNCTION__);
					return utlAT_RESULT_CODE_ERROR;
				}

				size_t commands_in_line = parser_p->commands_in_line;

				utlReleaseExclusiveAccess(&parser_p->cmd_cnt_semaphore);

				if (utlAtReset(parser_p) != utlSUCCESS)
					rc = utlAT_RESULT_CODE_ERROR;

				if (utlAcquireExclusiveAccess(&parser_p->cmd_cnt_semaphore) != utlSUCCESS)
				{
					utlError("%s: Cannot exclusively acquire semaphore!\n", __FUNCTION__);
					return utlAT_RESULT_CODE_ERROR;
				}

				parser_p->commands_in_line = commands_in_line;

				utlReleaseExclusiveAccess(&parser_p->cmd_cnt_semaphore);
			}

			break;

		case 's':                   /*--- DSR (output) control ---*/
		case 'S': switch (parameter_value_p->value.decimal)
			{
			case 0: parser_p->dce_io_config.modes.dsr_mode       = 0;
				parser_p->states.dce_io_config_pending_mask |= utlAT_DCE_IO_CONFIG_PENDING_DSR;  break;
			case 1: parser_p->dce_io_config.modes.dsr_mode       = 0;
				parser_p->states.dce_io_config_pending_mask |= utlAT_DCE_IO_CONFIG_PENDING_DSR;  break;
			default:
				rc = utlAT_RESULT_CODE_ERROR;
				break;
			}
			break;

		default: break;
		}

	default: break;
	}

	/*--- if execution of the basic build-in AT-command failed ---*/
	if (rc == utlAT_RESULT_CODE_ERROR)
	{
		switch (parameter_value_p->type)
		{
		case utlAT_DATA_TYPE_DECIMAL:     utlError("Invalid parameter value (%d) for `%s' AT command at col=%d.", parameter_value_p->value.decimal,       command_p->name_p, syntax_off);  break;
		case utlAT_DATA_TYPE_DIAL_STRING: utlError("Invalid parameter value (%s) for `%s' AT command at col=%d.", parameter_value_p->value.dial_string_p, command_p->name_p, syntax_off);  break;
		default:                          utlError("Invalid parameter value for `%s' AT command at col=%d.",                                              command_p->name_p, syntax_off);  break;
		}
	}

	return rc;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlAsyncResponseTimeoutHandler(id, timeout_count, arg_p, curr_time_p)
* INPUT
*                 id == timer ID
*      timeout_count == the time-out count
*              arg_p == pointer to the response we timed-out on
*        curr_time_p == current time since the epoch
* OUTPUT
*      modifies the data pointed to by `arg_p'
* RETURNS
*      utlSUCCESS for success, utlFAILED for failure.
* DESCRIPTION
*      Handles async response time-out events: terminates the wait by
*      generating an error.
*---------------------------------------------------------------------------*/
static utlReturnCode_T utlAsyncResponseTimeoutHandler(const utlTimerId_T id,
						      const unsigned long timeout_count,
						      void                *arg_p,
						      const utlAbsoluteTime_P2c curr_time_p)
{
	UNUSEDPARAM(id)
	UNUSEDPARAM(timeout_count)
	UNUSEDPARAM(curr_time_p)

	utlAtAsyncResponse_P async_response_p;
	utlReturnCode_T rc;
	char resetInfo[256];
	utlAssert(arg_p != NULL);

	async_response_p = (utlAtAsyncResponse_P)arg_p;

	if(async_response_p->command_p != NULL)
		utlError("AT Command %s timeout\n", async_response_p->command_p->name_p);
	rc = handle_CMEE_code(async_response_p->xid, CME_COMMMAND_TIMEOUT_ERR);

	async_response_p->op        = utlAT_ASYNC_OP_UNKNOWN;
	async_response_p->command_p = NULL;
	async_response_p->timer_id  = utlFAILED;

	{
		utlAtParser_P parser_p;

		parser_p = async_response_p->parser_p;
		utlAssert(parser_p != NULL);

		parser_p->states.sync_response.complete_reported  = false;
		parser_p->states.sync_response.abort_reported     = false;
		parser_p->states.sync_response.error_reported     = false;
		parser_p->states.sync_response.parameter_values_p = NULL;
		parser_p->states.sync_response.custom_p           = NULL;
		parser_p->states.sync_response.info_text_p        = NULL;
		parser_p->states.sync_response.abort_response_p   = NULL;
		parser_p->states.sync_response.command_syntax_p   = NULL;
	}
	{
		pthread_mutex_lock(&gTimeoutLock);
		if(++gTimeoutNum >= MAX_TIMEOUT_NUM)
		{
			sprintf(resetInfo, "timeout nums is %d, reach limit\n", gTimeoutNum);
			if(cp_silent_reset_on_req(resetInfo) == 0)
				gTimeoutNum = 0;
		}
		pthread_mutex_unlock(&gTimeoutLock);
	}
	return rc;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlSetupPendingAsyncResponse(parser_p, op, command_p)
* INPUT
*       parser_p == an open AT-command parser
*             op == a pending asyncchronous operation
*      command_p == pointer to the AT-command about to be executed
* OUTPUT
*      none
* RETURNS
*      a pointer to a new async-response node success, NULL for failure.
* DESCRIPTION
*      Sets up for an async response.
*---------------------------------------------------------------------------*/
static utlAtAsyncResponse_P utlSetupPendingAsyncResponse(const utlAtParser_P parser_p,
							 const utlAtAsyncOp_T op,
							 const utlAtCommand_P2c command_p)
{
	utlAtAsyncResponse_P async_response_p;

	utlAssert(parser_p != NULL);

	{
		if (utlAcquireExclusiveAccess(&parser_p->queue_semaphore) != utlSUCCESS)
		{
			utlError("Cannot exclusively acquire semaphore!\n");
			return NULL;
		}

		if ((async_response_p = utlGetHeadNode(&(parser_p->states.async_responses.unused), utlAtAsyncResponse_T)) == NULL)
		{
			utlError("There is no unused response node!\n");
			utlReleaseExclusiveAccess(&parser_p->queue_semaphore);
			return NULL;
		}

		utlReleaseExclusiveAccess(&parser_p->queue_semaphore);

		async_response_p->parser_p        = parser_p;
		async_response_p->xid             = utlNextAvailableXID();
		async_response_p->op              = op;
		async_response_p->command_p       = command_p;
		async_response_p->text_response_p = NULL;

		/*--- start timer ---*/
		{
			utlRelativeTime_T period;

			period.seconds = utlAtGetTimeOut(command_p, op);
			period.nanoseconds = 0;
			if ((async_response_p->timer_id = utlStartTimer(&period, 1, &utlAsyncResponseTimeoutHandler, async_response_p)) == (utlTimerId_T)utlFAILED)
			{

				utlError("Fail to start the timer!\n");
				if (utlAcquireExclusiveAccess(&parser_p->queue_semaphore) != utlSUCCESS)
				{
					utlError("Cannot exclusively acquire semaphore!\n");
					return NULL;
				}

				/*--- clean ---*/
				(void)utlPutTailNode(&(parser_p->states.async_responses.unused), utlAtAsyncResponse_T, async_response_p);

				utlReleaseExclusiveAccess(&parser_p->queue_semaphore);
				return NULL;
			}
		}

		if (utlAcquireExclusiveAccess(&parser_p->queue_semaphore) != utlSUCCESS)
		{
			utlError("Cannot exclusively acquire semaphore!\n");
			return NULL;
		}

		utlPutTailNode(&(parser_p->states.async_responses.pending), utlAtAsyncResponse_T, async_response_p);

		utlReleaseExclusiveAccess(&parser_p->queue_semaphore);
	}

	parser_p->states.sync_response.complete_reported  = false;
	parser_p->states.sync_response.abort_reported     = false;
	parser_p->states.sync_response.error_reported     = false;
	parser_p->states.sync_response.parameter_values_p = NULL;
	parser_p->states.sync_response.custom_p           = NULL;
	parser_p->states.sync_response.info_text_p        = NULL;
	parser_p->states.sync_response.abort_response_p   = NULL;
	parser_p->states.sync_response.command_syntax_p   = NULL;

	return async_response_p;
}
/*---------------------------------------------------------------------------*
* FUNCTION
*      utlModifyPendingAsyncResponse(period_p, xid)
* INPUT
*      period_p == pointer to the timeout time you want to set
*           xid == transaction ID
* OUTPUT
*      none
* RETURNS
*      utlFAILED for failure, utlSUCCESS for success
* DESCRIPTION
*     Modify the Pending time for an async response.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlModifyPendingAsyncResponse(	const utlRelativeTime_P period_p,
							 const unsigned int xid)
{
	utlAtParser_P parser_p = NULL;
	utlAtAsyncResponse_P async_response_p = NULL;
	utlAssert(period_p != NULL);
	for (parser_p = (utlAtParser_P)parsers.head_p; parser_p != NULL; parser_p = parser_p->next_p)
	{
		async_response_p = (utlAtAsyncResponse_P)parser_p->states.async_responses.pending.head_p;
		for (; async_response_p != NULL; async_response_p = async_response_p->next_p)
		{
			if (async_response_p->xid == xid)
			{
				break;
			}
		}
		if (async_response_p != NULL)
			break;
	}
	if(parser_p == NULL)
	{
		utlError("Invalid/unknown transaction ID %d\n", xid);
		return utlFAILED;
	}
	utlAssert(async_response_p != NULL);
	if (async_response_p->timer_id != (utlTimerId_T)utlFAILED)
	{
		if (utlStopTimer(async_response_p->timer_id) != utlSUCCESS)
			return utlFAILED;
		async_response_p->timer_id = utlFAILED;
	}
	if (utlAcquireExclusiveAccess(&parser_p->queue_semaphore) != utlSUCCESS)
	{
		utlError("Cannot exclusively acquire semaphore!\n");
		return utlFAILED;
	}
	if (utlGetNode(&(parser_p->states.async_responses.pending), utlAtAsyncResponse_T, NULL, async_response_p) == NULL)
	{
		utlError("There is no such pending response node!\n");
		utlReleaseExclusiveAccess(&parser_p->queue_semaphore);
		return utlFAILED;
	}
	utlReleaseExclusiveAccess(&parser_p->queue_semaphore);
	if ((async_response_p->timer_id = utlStartTimer(period_p, 1, &utlAsyncResponseTimeoutHandler, async_response_p)) == (utlTimerId_T)utlFAILED)
	{
		utlError("Fail to start the timer!\n");
		/*--- clean ---*/
		if (utlAcquireExclusiveAccess(&parser_p->queue_semaphore) != utlSUCCESS)
		{
			utlError("Cannot exclusively acquire semaphore!\n");
			return utlFAILED;
		}
		utlPutTailNode(&(parser_p->states.async_responses.unused),  utlAtAsyncResponse_T,       async_response_p);
		utlReleaseExclusiveAccess(&parser_p->queue_semaphore);
		return utlFAILED;
	}

	if (utlAcquireExclusiveAccess(&parser_p->queue_semaphore) != utlSUCCESS)
	{
		utlError("Cannot exclusively acquire semaphore!\n");
		return utlFAILED;
	}

	utlPutTailNode(&(parser_p->states.async_responses.pending), utlAtAsyncResponse_T, async_response_p);

	utlReleaseExclusiveAccess(&parser_p->queue_semaphore);
	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlAbandonPendingAsyncResponse(async_response_p)
* INPUT
*      async_response_p == the response to abandon
* OUTPUT
*      none
* RETURNS
*      nothing
* DESCRIPTION
*      Abandons the setup for a pending async response.
*---------------------------------------------------------------------------*/
static void utlAbandonPendingAsyncResponse(const utlAtAsyncResponse_P async_response_p)
{
	utlAtParser_P parser_p;

	utlAssert(async_response_p != NULL);

	parser_p = async_response_p->parser_p;
	utlAssert(parser_p != NULL);

	/*--- release ---*/
	if (utlAcquireExclusiveAccess(&parser_p->queue_semaphore) != utlSUCCESS)
	{
		utlError("Cannot exclusively acquire semaphore!\n");
		return;
	}

	if (utlGetNode(    &(parser_p->states.async_responses.pending), utlAtAsyncResponse_T, NULL, async_response_p) != NULL)
	{
		utlPutTailNode(&(parser_p->states.async_responses.unused),  utlAtAsyncResponse_T,       async_response_p);

		utlReleaseExclusiveAccess(&parser_p->queue_semaphore);

		/*--- stop timer (as necessary) ---*/
		if (                    async_response_p->timer_id != (utlTimerId_T)utlFAILED)
		{
			(void)utlStopTimer(async_response_p->timer_id);

			async_response_p->timer_id = utlFAILED;
		}
	}
	else
		utlReleaseExclusiveAccess(&parser_p->queue_semaphore);

}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlAbandonAllPendingAsyncResponse(parser_p)
* INPUT
*      parser_p
* OUTPUT
*      none
* RETURNS
*      nothing
* DESCRIPTION
*      Abandons all the pending async responses for one parser.
*---------------------------------------------------------------------------*/
static void utlAbandonAllPendingAsyncResponse(utlAtParser_P parser_p)
{
	utlAtAsyncResponse_P async_response_p;

	utlAssert(parser_p != NULL);

	if (utlAcquireExclusiveAccess(&parser_p->queue_semaphore) != utlSUCCESS)
	{
		utlError("Cannot exclusively acquire semaphore!\n");
		return;
	}

	/* just fetch the head node without removing it from pending list */
	async_response_p = (utlAtAsyncResponse_P)parser_p->states.async_responses.pending.head_p;

	if (async_response_p != NULL)
	{
		while (async_response_p != NULL)
		{
			utlReleaseExclusiveAccess(&parser_p->queue_semaphore);

			/*--- stop timer (as necessary) ---*/
			if (async_response_p->timer_id != (utlTimerId_T)utlFAILED)
			{
				(void)utlStopTimer(async_response_p->timer_id);
				async_response_p->timer_id = utlFAILED;
			}

			if (utlAcquireExclusiveAccess(&parser_p->queue_semaphore) != utlSUCCESS)
			{
				utlError("Cannot exclusively acquire semaphore!\n");
				return;
			}
			/* remove node from pending list, and append it to unused list */
			if (utlGetNode(    &(parser_p->states.async_responses.pending), utlAtAsyncResponse_T, NULL, async_response_p) != NULL)
				utlPutTailNode(&(parser_p->states.async_responses.unused),  utlAtAsyncResponse_T,       async_response_p);

			/* fetch next head node from pending list */
			async_response_p = (utlAtAsyncResponse_P)parser_p->states.async_responses.pending.head_p;
		}
		utlReleaseExclusiveAccess(&parser_p->queue_semaphore);

	}
	else
		utlReleaseExclusiveAccess(&parser_p->queue_semaphore);
}

#if 0
/*---------------------------------------------------------------------------*
* FUNCTION
*      utlAtResultCode_T utlHandleLateEvent(const utlAtParser_P            parser_p,
*		utlAtAsyncResponse_P async_response_p)
* INPUT
*               parser_p == an open AT-command parser
*         async_response_p == name of the AT command to execute
*
* OUTPUT
*      none
* RETURNS
*      utlAT_RESULT_CODE_ERROR for failure, other values for success
* DESCRIPTION
*       Handle driver events that occur during the invocation of an application-supplied call-back function
*---------------------------------------------------------------------------*/
static utlAtResultCode_T utlHandleLateEvent(const utlAtParser_P parser_p,
					    utlAtAsyncResponse_P async_response_p)
{
	utlAtResultCode_T rc = utlAT_RESULT_CODE_NULL;

	/*--- utlAT_COMMAND_RESPONSE_TYPE_ERROR response ---*/
	if (parser_p->states.sync_response.error_reported)
	{
		parser_p->states.sync_response.error_reported = false;
		utlAbandonPendingAsyncResponse(async_response_p);

		parser_p->peg_counts.bad_commands++;
		return utlAT_RESULT_CODE_ERROR;
	}

	/*--- utlAT_COMMAND_RESPONSE_TYPE_ABORT response ---*/
	if (parser_p->states.sync_response.abort_reported)
	{
		parser_p->states.sync_response.abort_reported = false;
		utlAbandonPendingAsyncResponse(async_response_p);

		if (utlSendInfoResponse(parser_p, parser_p->states.sync_response.abort_response_p) != utlSUCCESS)
			return utlAT_RESULT_CODE_ERROR;
		parser_p->states.sync_response.abort_response_p = NULL;

		parser_p->peg_counts.bad_commands++;
		return utlAT_RESULT_CODE_SUPPRESS;
	}

	/*--- utlAT_COMMAND_RESPONSE_TYPE_REQUEST_COMPLETED response ---*/
	if (parser_p->states.sync_response.complete_reported)
	{
		utlAbandonPendingAsyncResponse(async_response_p);

		parser_p->peg_counts.basic_commands++;
		return utlAT_RESULT_CODE_OK;
	}

	/*--- utlAT_COMMAND_RESPONSE_TYPE_CUSTOM response ---*/
	if (parser_p->states.sync_response.custom_p != NULL)
	{
		utlAbandonPendingAsyncResponse(async_response_p);

		if (utlSendInfoResponse(parser_p, parser_p->states.sync_response.custom_p) != utlSUCCESS)
			return utlAT_RESULT_CODE_ERROR;
		parser_p->states.sync_response.custom_p = NULL;

		parser_p->peg_counts.basic_commands++;
		return utlAT_RESULT_CODE_SUPPRESS;
	}

	/*--- utlAT_COMMAND_RESPONSE_TYPE_INFO_TEXT response ---*/
	if (parser_p->states.sync_response.info_text_p != NULL)
	{
		utlAbandonPendingAsyncResponse(async_response_p);

		if (utlSendInfoResponse(parser_p, parser_p->states.sync_response.info_text_p) != utlSUCCESS)
			return utlAT_RESULT_CODE_ERROR;
		parser_p->states.sync_response.info_text_p = NULL;

		parser_p->peg_counts.basic_commands++;
		return utlAT_RESULT_CODE_OK;
	}

	/*--- utlAT_COMMAND_RESPONSE_TYPE_COMMAND_SYNTAX events ---*/
	if (parser_p->states.sync_response.command_syntax_p != NULL)
	{
		utlAbandonPendingAsyncResponse(async_response_p);

		if (utlSendInfoResponse(parser_p, parser_p->states.sync_response.command_syntax_p) != utlSUCCESS)
			return utlAT_RESULT_CODE_ERROR;
		parser_p->states.sync_response.command_syntax_p = NULL;

		parser_p->peg_counts.extended_commands++;
		return utlAT_RESULT_CODE_OK;
	}

	return rc;
}
#endif

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlExecuteBasicAtCommand(parser_p, command_name_p, parameter_found,
*                               parameter_value_p, syntax_off)
* INPUT
*               parser_p == an open AT-command parser
*         command_name_p == name of the AT command to execute
*        parameter_found == was the parameter found?
*      parameter_value_p == a pointer to the parameter value
*             syntax_off == offset from start of line to the AT-command (for
*                           error reporting)
* OUTPUT
*      none
* RETURNS
*      utlAT_RESULT_CODE_ERROR for failure, other values for success
* DESCRIPTION
*      Executes the basic AT-command specified by `command_name_p'.
*---------------------------------------------------------------------------*/
static utlAtResultCode_T utlExecuteBasicAtCommand(const utlAtParser_P parser_p,
						  const char                    *command_name_p,
						  const bool parameter_found,
						  const utlAtParameterValue_P2c parameter_value_p,
						  const size_t syntax_off)
{
	utlAtCommand_P2c command_p;
	char text_response[utlAT_MAX_RESPONSE_LENGTH];
	utlAtResultCode_T rc;

	utlAssert(parser_p          != NULL);
	utlAssert(parameter_value_p != NULL);
	utlAssert(command_name_p    != NULL);

	/*--- search for AT-command info ---*/
	{
		utlAtCommand_P2c term_command_p;

		term_command_p = parser_p->commands_p + parser_p->num_commands;
		for (command_p = parser_p->commands_p; command_p < term_command_p; command_p++)
			if ((command_p->type == utlAT_COMMAND_TYPE_BASIC) &&
			    (tolower(   command_p->name_p[0]) == tolower(command_name_p[0])) &&
			    (strcasecmp(command_p->name_p,               command_name_p) == 0))
				break;

		if (command_p >= term_command_p)
		{
			parser_p->peg_counts.undefined_commands++;
			return utlAT_RESULT_CODE_ERROR;
		}
	}

	/*-- complain if command should not have a parameter, yet a parameter was found ---*/
	if ((parameter_found == true) && (command_p->parameters_p == NULL))
	{
		utlError("Unexpected parameter supplied to AT command at col=%d.", syntax_off);
		parser_p->peg_counts.bad_commands++;
		return utlAT_RESULT_CODE_ERROR;
	}

	utlTrace(utlTRACE_AT_PARSER,
		 switch (parameter_value_p->type)
		 {
		 case utlAT_DATA_TYPE_DECIMAL:     utlPrintTrace("AT%s (set: %d)\n",     command_name_p, parameter_value_p->value.decimal);        break;
		 case utlAT_DATA_TYPE_HEXADECIMAL: utlPrintTrace("AT%s (set: 0x%x)\n",   command_name_p, parameter_value_p->value.hexadecimal);    break;
		 case utlAT_DATA_TYPE_BINARY:      utlPrintTrace("AT%s (set: 0x%x)\n",   command_name_p, parameter_value_p->value.binary);         break;
		 case utlAT_DATA_TYPE_STRING:      utlPrintTrace("AT%s (set: \"%s\")\n", command_name_p, parameter_value_p->value.string_p);       break;
		 case utlAT_DATA_TYPE_QSTRING:     utlPrintTrace("AT%s (set: \"%s\")\n", command_name_p, parameter_value_p->value.qstring_p);      break;
		 case utlAT_DATA_TYPE_DIAL_STRING: utlPrintTrace("AT%s (set: %s)\n",     command_name_p, parameter_value_p->value.dial_string_p);  break;
		 }
		 );

	/*--- process built-in AT commands (as necessary) ---*/
	{
		/*--- initially assume no text response is required ---*/
		text_response[0] = '\0';

		if ((rc = utlProcessBasicAtCommand(parser_p,
						   command_p,
						   parameter_value_p,
						   text_response,
						   utlNumberOf(text_response),
						   syntax_off)) == utlAT_RESULT_CODE_ERROR)
		{
			parser_p->peg_counts.bad_commands++;
			return utlAT_RESULT_CODE_ERROR;
		}
	}

	/*--- invoke AT-command call-back (as appropriate) ---*/
	if (command_p->call_backs.set_parameter_function_p != NULL)
	{
		utlAtAsyncResponse_P async_response_p;

		/*--- setup for pending asynchronous response ---*/
		if ((async_response_p = utlSetupPendingAsyncResponse(parser_p, utlAT_ASYNC_OP_EXEC, command_p)) == NULL)
			return utlAT_RESULT_CODE_ERROR;

		if ((command_p->call_backs.set_parameter_function_p)(utlAT_PARAMETER_OP_EXEC,
								     command_name_p,
								     (command_p->num_parameters == (size_t)0) ? NULL : parameter_value_p,
								     command_p->num_parameters,
								     text_response,
								     &(async_response_p->xid),
								     parser_p->call_backs.arg_p) != utlSUCCESS)
		{
			/*--- clean ---*/
			utlAbandonPendingAsyncResponse(async_response_p);

			parser_p->peg_counts.bad_commands++;
			return utlAT_RESULT_CODE_ERROR;
		}

		rc = utlAT_RESULT_CODE_SUPPRESS;
	}

	if (utlAcquireExclusiveAccess(&parser_p->cmd_cnt_semaphore) != utlSUCCESS)
	{
		utlError("%s: Cannot exclusively acquire semaphore!\n", __FUNCTION__);
		return utlAT_RESULT_CODE_ERROR;
	}

	if (rc == utlAT_RESULT_CODE_OK)
		parser_p->commands_in_line--;

	utlReleaseExclusiveAccess(&parser_p->cmd_cnt_semaphore);

	parser_p->peg_counts.basic_commands++;
	return rc;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlFormatExtendedResultString(parser_p, result_p, result_siz, name_p, string_p)
* INPUT
*        parser_p == an open AT-command parser
*        result_p == pointer to where to place the formated result
*      result_siz == length of array pointed to by `result_p'
*          name_p == pointer to an AT-Command name expressed as a null-terminated
*                    character string
*        string_p == pointer to a null-terminated character string
* OUTPUT
*      *result_p == the formated result
* RETURNS
*      the number of characters written to `string_p'
* DESCRIPTION
*      Formats the specified extended-AT-Command result string.
*---------------------------------------------------------------------------*/
static size_t utlFormatExtendedResultString(const utlAtParser_P parser_p,
					    char          *result_p,
					    const size_t result_siz,
					    const char          *name_p,
					    const char          *string_p)
{
	size_t name_len;
	size_t string_len;
	size_t i;

	utlAssert(parser_p != NULL);
	utlAssert(result_p != NULL);
	utlAssert(name_p   != NULL);
	utlAssert(string_p != NULL);

	/*--- are results being suppressed? ---*/
	if (parser_p->parameters.suppress_results == true)
		return 0;

	name_len = strlen(  name_p);
	string_len = strlen(string_p);

	/*--- no room for result? ---*/
	if ((name_len + string_len) >= (result_siz - 3))
	{
		utlError("Buffer overflow.");
		return 0;
	}

	/*--- format for result is: "<name_p>: <string_p>" ---*/
	(void)strcpy(result_p, name_p);
	i = name_len;

	result_p[i++] = ':';
	result_p[i++] = ' ';

	(void)strcpy(result_p + i, string_p);
	i += string_len;

	result_p[i] = '\0';

	return i;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlProcessExtendedAtCommand(parser_p, request_type, command_p,
*                                  parameter_values_p, response_p,
*                                  response_siz,  syntax_off)
* INPUT
*                parser_p == an open AT-command parser
*            request_type == type of request
*               command_p == pointer to the AT command to process
*      parameter_values_p == command's associated parameter values
*              response_p == where to place the response (if any)
*            response_siz == length of array pointed to by `response_p'
*              syntax_off == offset from start of line to AT-command (only
*                            used for error reporting)
* OUTPUT
*      response_p == where the response was placed (if any)
* RETURNS
*      utlAT_RESULT_CODE_ERROR for failure, other values for success
* DESCRIPTION
*      Processes the extended built-in AT command specified by `command_p'.
*---------------------------------------------------------------------------*/
static utlAtResultCode_T utlProcessExtendedAtCommand(const utlAtParser_P parser_p,
						     const utlAtRequestType_T request_type,
						     const utlAtCommand_P2c command_p,
						     utlAtParameterValue_P parameter_values_p,
						     char                  *response_p,
						     const size_t response_siz,
						     const size_t syntax_off)
{
	utlAtResultCode_T rc;

	utlAssert(parser_p     != NULL);
	utlAssert(command_p    != NULL);
	utlAssert(response_p   != NULL);
	utlAssert(response_siz >  (size_t)2);

	utlAssert(command_p->name_p != NULL);

	/*--- nothing to do (we only process commands starting with "+A", "+G", "+I", or "+C")? ---*/
	if ((command_p->name_p[0] != '+') || ((command_p->name_p[1] != 'a') &&
					      (command_p->name_p[1] != 'A') &&
					      (command_p->name_p[1] != 'c') &&
					      (command_p->name_p[1] != 'C') &&
					      (command_p->name_p[1] != 'g') &&
					      (command_p->name_p[1] != 'G') &&
					      (command_p->name_p[1] != 'i') &&
					      (command_p->name_p[1] != 'I')))
		return utlAT_RESULT_CODE_OK;

	rc = utlAT_RESULT_CODE_OK;

	/*--- save/fetch dial-string ---*/
	if (strcasecmp(command_p->name_p, "+ASTO") == 0)
	{
		utlAssert((parameter_values_p[0].type == utlAT_DATA_TYPE_DECIMAL) ||
			  (parameter_values_p[0].type == utlAT_DATA_TYPE_STRING)  ||
			  (parameter_values_p[0].type == utlAT_DATA_TYPE_QSTRING));
		utlAssert(parameter_values_p[1].type == utlAT_DATA_TYPE_DIAL_STRING);

		/*--- fetch? ---*/
		if (request_type == utlAT_REQUEST_TYPE_GET)
		{
			char            *c_p;
			char       *term_c_p;
			const char *next_location_name_p;
			bool need_cr_lf;

			c_p = response_p;
			term_c_p = response_p + response_siz;

			next_location_name_p = NULL;

			/*--- retrieve name of first location (if any) ---*/
			if (     parser_p->call_backs.retrieve_dial_string_function_p != NULL)
				if ((parser_p->call_backs.retrieve_dial_string_function_p)(&next_location_name_p, NULL, parser_p->call_backs.arg_p) != utlSUCCESS)
				{
					rc = utlAT_RESULT_CODE_ERROR;
					next_location_name_p = NULL;
				}

			/*--- for each stored dial-string... ---*/
			need_cr_lf = false;
			while (next_location_name_p != NULL)
			{
				const char *location_name_p;
				const char *dial_string_p;

				/*--- save location name reference ---*/
				location_name_p = next_location_name_p;

				dial_string_p = NULL;

				/*--- retrieve previously saved dial string (if any) ---*/
				if (     parser_p->call_backs.retrieve_dial_string_function_p != NULL)
					if ((parser_p->call_backs.retrieve_dial_string_function_p)(&next_location_name_p, &dial_string_p, parser_p->call_backs.arg_p) != utlSUCCESS)
					{
						rc = utlAT_RESULT_CODE_ERROR;
						break;
					}

				/*--- found a previously-saved dial string? ---*/
				if ((dial_string_p != NULL) && (strlen(dial_string_p) > (size_t)0))
				{
					char buf[utlAT_MAX_RESPONSE_LENGTH];

					/*--- generate dial-string info: <location>,<dial-string><cr><lf> ---*/
					{
						size_t i;

						(void)strncpy(buf, location_name_p, utlNumberOf(buf));
						i = strlen(location_name_p);

						buf[i++] = parser_p->parameters.S[utlAT_SEPARATER_CHAR];

						(void)strncpy(buf + i, dial_string_p, utlNumberOf(buf) - i - 4);
						i += strlen(dial_string_p);

						buf[i] = '\0';

					}

					/*--- append dial-string info ---*/
					{
						size_t rv;

						/*--- separate multiple entries with line-termination sequences (if there's room) ---*/
						if (need_cr_lf == true)
						{
							if (response_siz > (size_t)(c_p - response_p + 2))
							{
								*c_p++ = parser_p->parameters.S[utlAT_LINE_TERM_CHAR];
								*c_p++ = parser_p->parameters.S[utlAT_FORMATTING_CHAR];
							}
						}
						else
							need_cr_lf = true;

						if ((rv = utlFormatExtendedResultString(parser_p, c_p,
											response_siz - (c_p - response_p),
											command_p->name_p,
											buf)) == (size_t)0)
						{
							rc = utlAT_RESULT_CODE_ERROR;
							break;
						}

						c_p += rv;
					}
				}
			}

			/*--- generate empty response if there's nothing to report ---*/
			if (c_p == response_p)
			{
				*c_p++ = parser_p->parameters.S[utlAT_LINE_TERM_CHAR];
				*c_p++ = parser_p->parameters.S[utlAT_FORMATTING_CHAR];
			}

			*c_p = '\0';

			/*--- else save ---*/
		}
		else if (request_type == utlAT_REQUEST_TYPE_SET)
		{
			const char *location_name_p;
			const char *dial_string_p;
			char location_name[20];          /* plenty big for a 64-bit unsigned integer */
			/*--- we'll accept either a decimal or string for "location".  According
			   to V.250 this parameter should be a decimal number, but our dial-string
			   storage implementation also supports storing phone numbers under
			   names.  So allowing ASTO to also store under names is convenient for
			   testing. */
			if (parameter_values_p[0].type == utlAT_DATA_TYPE_DECIMAL)
			{
				unsigned int location;

				location = parameter_values_p[0].value.decimal;

				if (utlDecimalToString(location_name, location, utlNumberOf(location_name)) == (size_t)0)
					rc = utlAT_RESULT_CODE_ERROR;

				location_name_p = location_name;

			}
			else if (parameter_values_p[0].type == utlAT_DATA_TYPE_STRING)
				location_name_p = parameter_values_p[0].value.string_p;

			else if (parameter_values_p[0].type == utlAT_DATA_TYPE_STRING)
				location_name_p = parameter_values_p[0].value.qstring_p;

			else
				location_name_p = "";

			if (rc == utlAT_RESULT_CODE_OK)
			{
				dial_string_p = parameter_values_p[1].value.dial_string_p;

				/*--- invoke save-dial-string call-back (if any) ---*/
				if (     parser_p->call_backs.save_dial_string_function_p != NULL)
				{
					if ((parser_p->call_backs.save_dial_string_function_p)(location_name_p, dial_string_p, parser_p->call_backs.arg_p) != utlSUCCESS)
						rc = utlAT_RESULT_CODE_ERROR;
				}
			}

		}
		else
			rc = utlAT_RESULT_CODE_ERROR;

		/*--- request manufacturer identification ---*/
	}
	else if ((strcasecmp(command_p->name_p,  "+GMI") == 0) ||
		 (strcasecmp(command_p->name_p, "+CGMI") == 0))
	{

		if ((request_type == utlAT_REQUEST_TYPE_GET) ||
		    (request_type == utlAT_REQUEST_TYPE_SET))
//			(void) strncpy(response_p, parser_p->info.manufacturer_p, response_siz);
			sprintf(response_p, "%s: \"%s\"", command_p->name_p, parser_p->info.manufacturer_p);
		else
			rc = utlAT_RESULT_CODE_ERROR;

		/*--- request model identification ---*/
	}
	else if ((strcasecmp(command_p->name_p,  "+GMM") == 0) ||
		 (strcasecmp(command_p->name_p, "+CGMM") == 0))
	{

		if ((request_type == utlAT_REQUEST_TYPE_GET) ||
		    (request_type == utlAT_REQUEST_TYPE_SET))
//			(void) strncpy(response_p, parser_p->info.model_p, response_siz);
			sprintf(response_p, "%s: \"%s\"", command_p->name_p, parser_p->info.model_p);
		else
			rc = utlAT_RESULT_CODE_ERROR;

		/*--- request revision identification ---*/
	}
	else if ((strcasecmp(command_p->name_p,  "+GMR") == 0) ||
		 (strcasecmp(command_p->name_p, "+CGMR") == 0))
	{

		if ((request_type == utlAT_REQUEST_TYPE_GET) ||
		    (request_type == utlAT_REQUEST_TYPE_SET))
//			(void) strncpy(response_p, parser_p->info.revision_p, response_siz);
			sprintf(response_p, "%s: \"%s\"", command_p->name_p, revisionId);
		else
			rc = utlAT_RESULT_CODE_ERROR;

		/*--- request product serial number (optional command) ---*/
	}
	else if ((strcasecmp(command_p->name_p,  "+GSN") == 0) ||
		 (strcasecmp(command_p->name_p, "+CGSN") == 0))
	{

		if (parser_p->info.serial_number_p == NULL)
			rc = utlAT_RESULT_CODE_ERROR;
		else if ((request_type == utlAT_REQUEST_TYPE_GET) ||
			 (request_type == utlAT_REQUEST_TYPE_SET))
			(void)strncpy(response_p, parser_p->info.serial_number_p, response_siz);
		else
			rc = utlAT_RESULT_CODE_ERROR;

		/*--- request global object identification (optional command) ---*/
	}
	else if ((strcasecmp(command_p->name_p, "+GOI")  == 0) ||
		 (strcasecmp(command_p->name_p, "+CGOI") == 0))
	{

		if (parser_p->info.object_id_p == NULL)
			rc = utlAT_RESULT_CODE_ERROR;
		else if ((request_type == utlAT_REQUEST_TYPE_GET) ||
			 (request_type == utlAT_REQUEST_TYPE_SET))
			(void)strncpy(response_p, parser_p->info.object_id_p, response_siz);
		else
			rc = utlAT_RESULT_CODE_ERROR;

		/*--- request complete capabilities list ---*/
	}
	else if (strcasecmp(command_p->name_p, "+GCAP") == 0)
	{

		if ((request_type == utlAT_REQUEST_TYPE_GET) ||
		    (request_type == utlAT_REQUEST_TYPE_SET))
		{
			utlAtCommand_P2c gcap_p;
			utlAtCommand_P2c term_gcap_p;
			char                  *c_p;
			char             *term_c_p;
			bool need_sep;

			c_p = response_p;
			term_c_p = response_p + response_siz;

			*c_p = '\0';

			{
				size_t rv;

				if ((rv = utlFormatExtendedResultString(parser_p, response_p, response_siz, command_p->name_p, "")) == (size_t)0)
					rc = utlAT_RESULT_CODE_ERROR;
				else
					c_p += rv;
			}

			/*--- compose a list of all supported extended AT-Commands ---*/
			need_sep = false;
			term_gcap_p = parser_p->commands_p + parser_p->num_commands;
			for (gcap_p = parser_p->commands_p; gcap_p < term_gcap_p; gcap_p++)
				if ((gcap_p->type == utlAT_COMMAND_TYPE_EXTENDED) ||
				    (gcap_p->type == utlAT_COMMAND_TYPE_EXACTION))
				{
					size_t len;

					/*--- omit unconfigured "optional" commands ---*/
					if ((((strcasecmp(gcap_p->name_p, "+GSN")  == 0) ||
					      (strcasecmp(gcap_p->name_p, "+CGSN") == 0)) && (parser_p->info.serial_number_p == NULL)) ||
					    (((strcasecmp(gcap_p->name_p, "+GOI")  == 0) ||
					      (strcasecmp(gcap_p->name_p, "+CGOI") == 0)) && (parser_p->info.object_id_p     == NULL)) ||
					    (((strcasecmp(gcap_p->name_p, "+GCI")  == 0) ||
					      (strcasecmp(gcap_p->name_p, "+CGCI") == 0)) && (parser_p->info.country_code_p  == NULL)))
						continue;

					len = strlen(gcap_p->name_p);

					if ((need_sep == true) && ((c_p + 1) < term_c_p))
						*c_p++ = parser_p->parameters.S[utlAT_SEPARATER_CHAR];

					if ((c_p + len + 1) >= term_c_p)
					{
						utlError("Reply-buffer overflow.");
						/*return utlAT_RESULT_CODE_ERROR;*/
						break;
					}

					(void)strcpy(c_p, gcap_p->name_p);
					c_p += len;

					need_sep = true;
				}

		}
		else
			rc = utlAT_RESULT_CODE_ERROR;

		/*--- country of installation (optional command) ---*/
	}
	else if ((strcasecmp(command_p->name_p, "+GCI")  == 0) ||
		 (strcasecmp(command_p->name_p, "+CGCI") == 0))
	{

		if ((request_type == utlAT_REQUEST_TYPE_GET) ||
		    (request_type == utlAT_REQUEST_TYPE_SET))
		{
			if (utlFormatExtendedResultString(parser_p, response_p, response_siz, command_p->name_p, parser_p->info.country_code_p) == (size_t)0)
				rc = utlAT_RESULT_CODE_ERROR;
		}
		else
			rc = utlAT_RESULT_CODE_ERROR;

		/*--- fixed DTE rate (optional command) ---*/
	}
	else if (strcasecmp(command_p->name_p, "+IPR") == 0)
	{

		if (request_type == utlAT_REQUEST_TYPE_GET)
		{
			char buf[20];

			if (utlDecimalToString(buf, parser_p->dce_io_config.data_rate, utlNumberOf(buf))  == (size_t)0)
				rc = utlAT_RESULT_CODE_ERROR;

			else if (utlFormatExtendedResultString(parser_p, response_p, response_siz, command_p->name_p, buf) == (size_t)0)
				rc = utlAT_RESULT_CODE_ERROR;

		}
		else if (request_type == utlAT_REQUEST_TYPE_SET)
		{

			/*--- check parameter value and save if OK ---*/
			switch (parameter_values_p[0].value.decimal)
			{
			case   0:  case 134:  case  600:  case  4800:  case 19200:  case  76800:
			case  50:  case 150:  case 1200:  case  7200:  case 28800:  case 115200:
			case  75:  case 200:  case 1800:  case  9600:  case 38400:  case 230400:
			case 110:  case 300:  case 2400:  case 14400:  case 57600:
				parser_p->dce_io_config.data_rate            = parameter_values_p[0].value.decimal;
				parser_p->states.dce_io_config_pending_mask |= utlAT_DCE_IO_CONFIG_PENDING_BIT_RATE;  break;
				break;

			default:
				rc = utlAT_RESULT_CODE_ERROR;
			}

		}
		else
			rc = utlAT_RESULT_CODE_ERROR;

		/*--- DTE-DCE character framing (optional command) ---*/
	}
	else if (strcasecmp(command_p->name_p, "+ICF") == 0)
	{

		if (request_type == utlAT_REQUEST_TYPE_GET)
		{
			char buf[40];
			size_t i;

			i = utlDecimalToString(buf, parser_p->parameters.raw_framing, utlNumberOf(buf) - 1);

			buf[i++] = parser_p->parameters.S[utlAT_SEPARATER_CHAR];

			i += utlDecimalToString(buf + i, parser_p->parameters.raw_parity, utlNumberOf(buf) - i);

			if (utlFormatExtendedResultString(parser_p, response_p, response_siz, command_p->name_p, buf) == (size_t)0)
				rc = utlAT_RESULT_CODE_ERROR;

		}
		else if (request_type == utlAT_REQUEST_TYPE_SET)
		{
			unsigned int raw_framing;
			unsigned int raw_parity;

			raw_framing = parameter_values_p[0].value.decimal;

			if (parameter_values_p[1].is_default == false)
				raw_parity = parameter_values_p[1].value.decimal;
			else raw_parity = parser_p->parameters.raw_parity;

			/*--- check parameter values ---*/
			switch (raw_framing)
			{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
				break;

			default:
				rc = utlAT_RESULT_CODE_ERROR;
			}
			switch (raw_parity)
			{
			case 0:
			case 1:
			case 2:
			case 3:
				break;

			default:
				rc = utlAT_RESULT_CODE_ERROR;
			}

			/*--- save parameter values if OK ---*/
			if (rc == utlAT_RESULT_CODE_OK)
			{
				parser_p->parameters.raw_framing             = raw_framing;
				parser_p->parameters.raw_parity              = raw_parity;
				parser_p->states.dce_io_config_pending_mask |= utlAT_DCE_IO_CONFIG_PENDING_FRAMING |
									       utlAT_DCE_IO_CONFIG_PENDING_PARITY;
			}

		}
		else
			rc = utlAT_RESULT_CODE_ERROR;

		/*--- DTE-DCE local flow control (optional command) ---*/
	}
	else if (strcasecmp(command_p->name_p, "+IFC") == 0)
	{
		utlAssert(parameter_values_p[0].type == utlAT_DATA_TYPE_DECIMAL);
		utlAssert(parameter_values_p[1].type == utlAT_DATA_TYPE_DECIMAL);

		if (request_type == utlAT_REQUEST_TYPE_GET)
		{
			char buf[40];
			size_t i;

			i = utlDecimalToString(buf, parser_p->dce_io_config.flow_control.dce_by_dte, utlNumberOf(buf) - 1);

			buf[i++] = parser_p->parameters.S[utlAT_SEPARATER_CHAR];

			i += utlDecimalToString(buf + i, parser_p->dce_io_config.flow_control.dte_by_dce, utlNumberOf(buf) - i);

			if (utlFormatExtendedResultString(parser_p, response_p, response_siz, command_p->name_p, buf) == (size_t)0)
				rc = utlAT_RESULT_CODE_ERROR;

		}
		else if (request_type == utlAT_REQUEST_TYPE_SET)
		{
			unsigned int dce_by_dte;
			unsigned int dte_by_dce;

			dce_by_dte = parameter_values_p[0].value.decimal;

			if (parameter_values_p[1].is_default == false)
				dte_by_dce = parameter_values_p[1].value.decimal;
			else dte_by_dce = parser_p->dce_io_config.flow_control.dte_by_dce;

			/*--- check parameter values ---*/
			switch (dce_by_dte)
			{
			case 0:         /* none */
			case 1:         /* DC1/DC3 (and don't pass DC1/DC3 through) */
			case 2:         /* RTS */
			case 3:         /* both RTS and DC1/DC3 (and pass DC1/DC3 through) */
				break;

			default:
				rc = utlAT_RESULT_CODE_ERROR;
			}
			switch (dte_by_dce)
			{
			case 0:         /* none */
			case 1:         /* DC1/DC3 */
			case 2:         /* CTS */
				break;

			default:
				rc = utlAT_RESULT_CODE_ERROR;
			}

			/*--- save parameter values if OK ---*/
			if (rc == utlAT_RESULT_CODE_OK)
			{
				parser_p->dce_io_config.flow_control.dce_by_dte  = dce_by_dte;
				parser_p->dce_io_config.flow_control.dte_by_dce  = dte_by_dce;
				parser_p->states.dce_io_config_pending_mask     |= utlAT_DCE_IO_CONFIG_PENDING_FLOW_CONTROL;
			}

		}
		else
			rc = utlAT_RESULT_CODE_ERROR;

		/*--- select DSR option (optional command) ---*/
	}
	else if (strcasecmp(command_p->name_p, "+IDSR") == 0)
	{
		utlAssert(parameter_values_p[0].type == utlAT_DATA_TYPE_DECIMAL);

		if (request_type == utlAT_REQUEST_TYPE_GET)
		{
			char buf[20];

			if (utlDecimalToString(buf, parser_p->dce_io_config.modes.dsr_mode, utlNumberOf(buf)) == (size_t)0)
				rc = utlAT_RESULT_CODE_ERROR;

			else if (utlFormatExtendedResultString(parser_p, response_p, response_siz, command_p->name_p, buf) == (size_t)0)
				rc = utlAT_RESULT_CODE_ERROR;

		}
		else if (request_type == utlAT_REQUEST_TYPE_SET)
		{

			/*--- check parameter value and save if OK ---*/
			switch (parameter_values_p[0].value.decimal)
			{
			case  0:                /* DSR always on   */
			case  1:                /* DSR as per V.24 */
			case  2:                /* DSR always on except for 5 seconds after disconnect */
				parser_p->dce_io_config.modes.dsr_mode       = parameter_values_p[0].value.decimal;
				parser_p->states.dce_io_config_pending_mask |= utlAT_DCE_IO_CONFIG_PENDING_DSR;
				break;

			default:
				rc = utlAT_RESULT_CODE_ERROR;
			}

		}
		else
			rc = utlAT_RESULT_CODE_ERROR;
	}

	/*--- if the execution of any basic build-in commands failed ---*/
	if (rc != utlAT_RESULT_CODE_OK)
		utlError("Invalid parameter for `%s' AT command at col=%d.", command_p->name_p, syntax_off);

	return rc;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlSetDefaultParameterValue(parameter_value_p, type, access)
* INPUT
*      parameter_value_p == pointer to the parameter to set
*                   type == the data type of `parameter_value_p'
*                 access == the access rights associated with `parameter_value_p'
* OUTPUT
*      *parameter_value_p == a modified parameter
* RETURNS
*      nothing
* DESCRIPTION
*      Assigns the specified parameter a default value according to it's type.
*      Modified by Rovin Yu: other file can also use this function
*---------------------------------------------------------------------------*/
void utlSetDefaultParameterValue(const utlAtParameterValue_P parameter_value_p,
				 const utlAtDataType_T type,
				 const utlAtParameterAccess_T access)
{
	utlAssert(parameter_value_p != NULL);

	parameter_value_p->type       = type;
	parameter_value_p->access     = access;
	parameter_value_p->is_default = true;

	switch (type)
	{
	case utlAT_DATA_TYPE_DECIMAL:     parameter_value_p->value.decimal       = 0;   break;
	case utlAT_DATA_TYPE_HEXADECIMAL: parameter_value_p->value.hexadecimal   = 0;   break;
	case utlAT_DATA_TYPE_BINARY:      parameter_value_p->value.binary        = 0;   break;
	case utlAT_DATA_TYPE_STRING:      parameter_value_p->value.string_p      = "";  break;
	case utlAT_DATA_TYPE_QSTRING:     parameter_value_p->value.qstring_p     = "";  break;
	case utlAT_DATA_TYPE_DIAL_STRING: parameter_value_p->value.dial_string_p = "";  break;

	default:
		(void)memset(&(parameter_value_p->value), 0, sizeof(parameter_value_p->value));
		break;
	}
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlAtParserInitCommandParameters(xid, parameter_values_p, num_parameters)
* INPUT
*                     xid == an active transaction ID
*      parameter_values_p == an uninitialized utlAtParameterValue_T array
*          num_parameters == number of elements in array pointed to by `parameter_values_p'
* OUTPUT
*      *parameter_values_p == the initialized utlAtParameterValue_T array
* RETURNS
*      the number of parameters for success, utlFAILED for failure
* DESCRIPTION
*      Initializes the parameter-values structure appropriately for the
*      AT-command associated with `xid'.
*---------------------------------------------------------------------------*/
size_t utlAtParserInitCommandParameters(const unsigned int xid,
					const utlAtParameterValue_P parameter_values_p,
					const size_t num_parameters)
{
	utlAtParser_P parser_p;
	utlAtAsyncResponse_P async_response_p;
	utlAtCommand_P2c command_p;
	utlAtParameter_P parameter_p;
	utlAtParameterValue_P parameter_value_p;

	utlAssert(parameter_values_p != NULL);

	/*--- determine the parser and async_response node associated with the given transaction ID ---*/
	{
		/*--- for each parser... ---*/
		for (parser_p = (utlAtParser_P)parsers.head_p; parser_p != NULL; parser_p = parser_p->next_p)
		{

			/*--- for each pending async response for this parser... ---*/
			for (async_response_p = (utlAtAsyncResponse_P)parser_p->states.async_responses.pending.head_p; async_response_p != NULL; async_response_p = async_response_p->next_p)
				if (async_response_p->xid == xid)
					break;

			/*--- bail out? ---*/
			if (async_response_p != NULL)
				break;
		}

		/*--- is XID invalid/unknown? ---*/
		if (parser_p == NULL)
		{
			utlError("Invalid/unknown transaction ID %d\n", xid);
			return utlFAILED;
		}
	}

	utlAssert(async_response_p->command_p != NULL);
	command_p = async_response_p->command_p;

	/*--- check that output array is big enough ---*/
	if (num_parameters < command_p->num_parameters)
	{
		utlError("Not enough room in `parameter_values_p' for all of command `%s's parameters (got %d, need %d)",
			 command_p->name_p, num_parameters, command_p->num_parameters);
		return utlFAILED;
	}

	parameter_p       = command_p->parameters_p;
	parameter_value_p = parameter_values_p;

	/*--- for each of this AT-Command's known parameters... ---*/
	while (parameter_p < (command_p->parameters_p + command_p->num_parameters))
	{

		/*--- assign a default parameter value, according to it's data-type... ---*/
		utlSetDefaultParameterValue(parameter_value_p, parameter_p->type, parameter_p->access);

		parameter_p++;
		parameter_value_p++;
	}

	return command_p->num_parameters;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlGenerateFormattedGetResponse(parser_p, command_name_p, parameters_p,
*                                      num_parameters)
* INPUT
*            parser_p == an open AT-command parser
*      command_name_p == name of the AT command to execute
*        parameters_p == pointer to an array of parameters
*      num_parameters == the number of elements in the array pointed to by `parameters_p'
* OUTPUT
*      none
* RETURNS
*      utlFAILED for failure, otherwise utlSUCCESS
* DESCRIPTION
*      Handles the generation (format and transmission) of standard responses
*      to Extended AT command "GET" requests: converts parameter values into
*      a properly formattted text string and issues the string.
*---------------------------------------------------------------------------*/
static utlReturnCode_T utlGenerateFormattedGetResponse(const utlAtParser_P parser_p,
						       const char                    *command_name_p,
						       const utlAtParameterValue_P2c parameter_values_p,
						       const size_t num_parameters)
{
	char buf[utlAT_MAX_RESPONSE_LENGTH];
	utlAtParameterValue_P2c parameter_value_p;
	utlAtParameterValue_P2c term_parameter_value_p;
	size_t i;

	utlAssert(parser_p           != NULL);
	utlAssert(command_name_p     != NULL);
	utlAssert(parameter_values_p != NULL);

	i = 0;

	/*--- for each known parameter value... ---*/
	term_parameter_value_p = parameter_values_p + num_parameters;
	for (parameter_value_p = parameter_values_p; parameter_value_p < term_parameter_value_p; parameter_value_p++)
	{
		size_t rv;

		/*--- append parameter separator (as necessary) ---*/
		if (parameter_value_p > parameter_values_p)
		{
			if (i < utlNumberOf(buf))
				buf[i++] = parser_p->parameters.S[utlAT_SEPARATER_CHAR];
		}

		/*--- append parameter value according to type ---*/
		switch (parameter_value_p->type)
		{
		case utlAT_DATA_TYPE_DECIMAL:
			if ((rv = utlDecimalToString(buf + i, parameter_value_p->value.decimal, sizeof(buf) - i)) == (size_t)0)
			{
				utlError("Reply-buffer overflow.");
				return utlFAILED;
			}

			i += rv;
			break;

		case utlAT_DATA_TYPE_HEXADECIMAL:
			if ((rv = utlHexadecimalToString(buf + i, parameter_value_p->value.hexadecimal, sizeof(buf) - i)) == (size_t)0)
			{
				utlError("Reply-buffer overflow.");
				return utlFAILED;
			}

			i += rv;
			break;

		case utlAT_DATA_TYPE_BINARY:
			if ((rv = utlBinaryToString(buf + i, parameter_value_p->value.binary, sizeof(buf) - i)) == (size_t)0)
			{
				utlError("Reply-buffer overflow.");
				return utlFAILED;
			}

			i += rv;
			break;

		case utlAT_DATA_TYPE_STRING:
		case utlAT_DATA_TYPE_QSTRING:
		{
			const char *c_p;

			if (i < utlNumberOf(buf))
				buf[i++] = '"';

			if (parameter_value_p->type == utlAT_DATA_TYPE_STRING) c_p = parameter_value_p->value.string_p;
			else c_p = parameter_value_p->value.qstring_p;

			while ((*c_p != '\0') && (i < utlNumberOf(buf)))
				buf[i++] = *c_p++;

			if (i < utlNumberOf(buf))
				buf[i++] = '"';

			/*--- no room? ---*/
			if (i >= utlNumberOf(buf))
			{
				utlError("Reply-buffer overflow.");
				return utlFAILED;
			}
		}
		break;

		case utlAT_DATA_TYPE_DIAL_STRING:
		{
			const char *c_p;

			c_p = parameter_value_p->value.dial_string_p;

			while ((*c_p != '\0') && (i < utlNumberOf(buf)))
				buf[i++] = *c_p++;

			/*--- no room? ---*/
			if (i >= utlNumberOf(buf))
			{
				utlError("Reply-buffer overflow.");
				return utlFAILED;
			}
		}
		break;

		default:
			utlError("Invalid parameter_value_p->type!\n");
			break;
		}
	}

	/*--- no room? ---*/
	if (i >= (utlNumberOf(buf) - 1))
	{
		utlError("Reply-buffer overflow.");
		return utlFAILED;
	}

	buf[i] = '\0';

	utlTrace(utlTRACE_AT_PARSER,
		 utlPrintTrace("AT%s (get: %s)\n", command_name_p, buf);
		 );

	if (utlSendInfoResponse(parser_p, buf) != utlSUCCESS)
		return utlFAILED;

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlExecuteExtendedAtCommand(parser_p, request_type, command_name_p,
*                                  parameters_string_p, syntax_off)
* INPUT
*                 parser_p == an open AT-command parser
*             request_type == type of extended AT-command request
*           command_name_p == name of the AT command to execute
*      parameters_string_p == was a parameter value found?
*               syntax_off == offset from start of line to AT-command (only
*                             used for error reporting)
* OUTPUT
*      none
* RETURNS
*      utlAT_RESULT_CODE_ERROR for failure, other values for success
* DESCRIPTION
*      Executes the extended AT command specified by `command_name_p'.
*---------------------------------------------------------------------------*/
static utlAtResultCode_T utlExecuteExtendedAtCommand(const utlAtParser_P parser_p,
						     const utlAtRequestType_T request_type,
						     const char               *command_name_p,
						     const char               *parameters_string_p,
						     const size_t syntax_off)
{
	utlAtCommand_P2c command_p;
	utlAtParameterValue_T parameter_values[utlAT_MAX_PARAMETERS];
	char string_buf[utlAT_MAX_LINE_LENGTH + utlAT_MAX_PARAMETERS];
	char info_text[utlAT_MAX_RESPONSE_LENGTH];
	utlAtResultCode_T rc;

	utlAssert(parser_p       != NULL);
	utlAssert(command_name_p != NULL);

	/*--- search for AT-command info ---*/
	{
		utlAtCommand_P2c term_command_p;

		term_command_p = parser_p->commands_p + parser_p->num_commands;
		for (command_p = parser_p->commands_p; command_p < term_command_p; command_p++)
			if (((command_p->type == utlAT_COMMAND_TYPE_EXTENDED) ||
			     (command_p->type == utlAT_COMMAND_TYPE_EXACTION)) &&
			    (tolower(   command_p->name_p[1]) == tolower(command_name_p[1])) &&
			    (strcasecmp(command_p->name_p,               command_name_p) == 0))
				break;

		if (command_p >= term_command_p)
		{
			parser_p->peg_counts.undefined_commands++;
			return utlAT_RESULT_CODE_ERROR;
		}
	}

	/*--- if we're only fetching the command syntax ---*/
	if (request_type == utlAT_REQUEST_TYPE_SYNTAX)
	{

		rc = utlAT_RESULT_CODE_OK;

		/*--- if the syntax for this command is static ---*/
		if (command_p->usage.command_syntax_p != NULL)
		{

			utlTrace(utlTRACE_AT_PARSER,
				 utlPrintTrace("AT%s (query syntax: %s)\n", command_name_p, command_p->usage.command_syntax_p);
				 );

			if (utlSendInfoResponse(parser_p, command_p->usage.command_syntax_p) != utlSUCCESS)
				return utlAT_RESULT_CODE_ERROR;

			/*--- else if the syntax for this command can be fetched ---*/
		}
		else if (command_p->usage.call_backs.command_syntax_function_p != NULL)
		{
			utlAtAsyncResponse_P async_response_p;

			/*--- setup for pending asynchronous response ---*/
			if ((async_response_p = utlSetupPendingAsyncResponse(parser_p, utlAT_ASYNC_OP_SYNTAX, command_p)) == NULL)
				return utlAT_RESULT_CODE_ERROR;

			if ((command_p->usage.call_backs.command_syntax_function_p)(utlAT_PARAMETER_OP_SYNTAX,
										    command_name_p,
										    NULL,
										    0,
										    NULL,
										    &(async_response_p->xid),
										    parser_p->call_backs.arg_p) != utlSUCCESS)
			{
				/*--- clean ---*/
				utlAbandonPendingAsyncResponse(async_response_p);

				parser_p->peg_counts.bad_commands++;
				return utlAT_RESULT_CODE_ERROR;
			}

			rc = utlAT_RESULT_CODE_SUPPRESS;
		}

		parser_p->peg_counts.extended_commands++;

		if (utlAcquireExclusiveAccess(&parser_p->cmd_cnt_semaphore) != utlSUCCESS)
		{
			utlError("%s: Cannot exclusively acquire semaphore!\n", __FUNCTION__);
			return utlAT_RESULT_CODE_ERROR;
		}

		if (rc == utlAT_RESULT_CODE_OK)
			parser_p->commands_in_line--;

		utlReleaseExclusiveAccess(&parser_p->cmd_cnt_semaphore);

		return rc;

		/*--- else if this is a set-request, parse for AT-command's parameters (if any)... ---*/
	}
	else if (request_type == utlAT_REQUEST_TYPE_SET)
	{
		utlAtParameter_P parameter_p;
		utlAtParameterValue_P parameter_value_p;
		char                  *string_buf_p;
		const char            *c_p;

		parameter_p       = command_p->parameters_p;
		parameter_value_p = parameter_values;   /* we'll stuff the found parameter values here */

		string_buf_p = string_buf;              /* we'll store decoded string-parameters here */

		/*--- if any parameter values were found... ---*/
		if (parameters_string_p != NULL)
		{

			utlTrace(utlTRACE_AT_PARSER,
				 utlPrintTrace("AT%s (set: %s)\n", command_name_p, parameters_string_p);
				 );

			/*--- for each found parameter value... ---*/
			for (c_p = parameters_string_p; *c_p != '\0'; )
			{
				char value_string[utlAT_MAX_LINE_LENGTH];
				size_t value_string_off;
				bool parameter_found;
				char parameter_term_char;
				bool quote_state;

				/*--- note offset to this parameter (for error reporting) ---*/
				value_string_off = c_p - parameters_string_p;

				/*--- extract parameter value (parameters separated by ',' and terminated by null) ---*/
				{
					char *dest_p;
					quote_state = true;
					parameter_found     = false;
					parameter_term_char = '\0';
					for (dest_p = value_string; *c_p != '\0'; c_p++)
					{

						if(*c_p == '"')
							quote_state = !quote_state;
						if ((*c_p == parser_p->parameters.S[utlAT_SEPARATER_CHAR]) && quote_state)
						{
							parameter_term_char = *c_p++;  /* skip parameter-separator character */
							break;
						}

						*dest_p++ = *c_p;

						parameter_found = true;
					}
					*dest_p = '\0';
				}

				/*--- if this command should not actually have any parameter values ---*/
				if (parameter_p == NULL)
				{

					/*--- complain if one or more parameter values were found ---*/
					if (parameter_found == true)
					{
						utlError("Unexpected AT command parameter at col=%d.", syntax_off);
						parser_p->peg_counts.bad_commands++;
						return utlAT_RESULT_CODE_ERROR;
					}

					/*--- else this command should have one or more parameters...  ---*/
				}
				else
				{
					/*--- were more parameter values supplied than we can parse? ---*/
					if (parameter_value_p >= (parameter_values + utlAT_MAX_PARAMETERS))
					{
						utlError("Too many AT-command parameters at col=%d.", syntax_off);
						parser_p->peg_counts.bad_commands++;
						return utlAT_RESULT_CODE_ERROR;
					}

					/*--- if no parameter value was found ---*/
					if (parameter_found == false)
					{

						/*-- complain if omitted parameter value is not Read, Read-only, and is required ---*/
						if (((parameter_p->access   != utlAT_PARAMETER_ACCESS_READ) &&
						     (parameter_p->access   != utlAT_PARAMETER_ACCESS_READ_ONLY)) &&
						    ( parameter_p->presence == utlAT_PARAMETER_PRESENCE_REQUIRED))
						{
							utlError("Missing AT command parameter at col=%d.", syntax_off + value_string_off + 1);
							parser_p->peg_counts.bad_commands++;
							return utlAT_RESULT_CODE_ERROR;
						}

						/*--- otherwise assign optional parameter a default value, according to it's type... ---*/
						utlSetDefaultParameterValue(parameter_value_p, parameter_p->type, parameter_p->access);

						/*--- else if a parameter value was found ---*/
					}
					else
					{
						unsigned int num = 0;
						char         *d_p;

						/*--- are we attempting to set a read-only parameter? ---*/
						if (parameter_p->access == utlAT_PARAMETER_ACCESS_READ_ONLY)
						{
							utlError("Attempt to modify read-only AT command parameter at col=%d.", syntax_off + value_string_off + 1);
							parser_p->peg_counts.bad_commands++;
							return utlAT_RESULT_CODE_ERROR;
						}

						/*--- grab rest of parameter string if we are expecting a dial string ---*/
						if ((parameter_p->type   == utlAT_DATA_TYPE_DIAL_STRING) &&
						    (parameter_term_char == parser_p->parameters.S[utlAT_SEPARATER_CHAR]))
						{
							char *dest_p;

							dest_p = value_string + strlen(value_string);

							*dest_p++ = parser_p->parameters.S[utlAT_SEPARATER_CHAR];

							while (*c_p != '\0')
								*dest_p++ = *c_p++;

							*dest_p = '\0';
						}

						parameter_value_p->type       = parameter_p->type;
						parameter_value_p->access     = parameter_p->access;
						parameter_value_p->is_default = false;

						/*--- parse found parameter value according to it's known data-type... ---*/
						switch (parameter_p->type)
						{
						case utlAT_DATA_TYPE_DECIMAL:
							num = 0;
							for (d_p = value_string; *d_p != '\0'; d_p++)
							{
								if ((*d_p >= '0') && (*d_p <= '9'))
									num = (10 * num) + (*d_p - '0');
								else
								{
									utlError("Extended-AT command syntax error at col=%d.", syntax_off + value_string_off + 1);
									parser_p->peg_counts.bad_commands++;
									return utlAT_RESULT_CODE_ERROR;
								}
							}

							parameter_value_p->value.decimal = num;
							break;

						case utlAT_DATA_TYPE_HEXADECIMAL:
							num = 0;
							for (d_p = value_string; *d_p != '\0'; d_p++)
							{
								if ((*d_p >= '0') && (*d_p <= '9')) num = (16 * num) + (*d_p - '0');
								else if ((*d_p >= 'A') && (*d_p <= 'F')) num = (16 * num) + (*d_p - 'A') + 10;
								else if ((*d_p >= 'a') && (*d_p <= 'f')) num = (16 * num) + (*d_p - 'a') + 10;
								else
								{
									utlError("Extended-AT command syntax error at col=%d.", syntax_off + value_string_off + 1);
									parser_p->peg_counts.bad_commands++;
									return utlAT_RESULT_CODE_ERROR;
								}
							}

							parameter_value_p->value.hexadecimal = num;
							break;

						case utlAT_DATA_TYPE_BINARY:
							num = 0;
							for (d_p = value_string; *d_p != '\0'; d_p++)
							{
								if (*d_p == '0') num = (num * 2);
								else if (*d_p == '1') num = (num * 2) + 1;
								else
								{
									utlError("Extended-AT command syntax error at col=%d.", syntax_off + value_string_off + 1);
									parser_p->peg_counts.bad_commands++;
									return utlAT_RESULT_CODE_ERROR;
								}
							}

							parameter_value_p->value.binary = num;
							break;

						case utlAT_DATA_TYPE_STRING:
						case utlAT_DATA_TYPE_QSTRING:
						{
							utlAtStringState_T state;
							char               *src_p;
							bool quoted;

							if (parameter_p->type == utlAT_DATA_TYPE_STRING) parameter_value_p->value.string_p  = string_buf_p;
							else parameter_value_p->value.qstring_p = string_buf_p;

							src_p = value_string;

							/*--- our regular string parsing is a little more flexible than the
							   V.250 standard specifies: strings do not have to be enclosed in
							   double-quotes, but if double-quotes are omitted, the end of
							   the string will be delimited by the first parameter separator
							   character, colon character, or end-of-line encountered. */
							if (*src_p == '"')
							{
								src_p++;
								quoted = true;

							}
							else if (parameter_p->type == utlAT_DATA_TYPE_QSTRING)
							{
								utlError("Extended-AT command syntax error at col=%d.", syntax_off + value_string_off + 1);
								parser_p->peg_counts.bad_commands++;
								return utlAT_RESULT_CODE_ERROR;

							}
							else
								quoted = false;

							/*--- extract string, replacing hex escape sequences with the ASCII
							   characters they represent */
							state = utlAT_STRING_STATE_PASS_THROUGH;

							for (; *src_p != '\0'; src_p++)
							{

								if (quoted)
								{
									if (*src_p == '"')
										break;
								}
								else
								{
									if ((*src_p == parser_p->parameters.S[utlAT_SEPARATER_CHAR]) || (*src_p == parser_p->parameters.S[utlAT_TERMINATER_CHAR]))
										break;
								}

								/*--- this a possible embedded hex escape sequence? ---*/
								if ((*src_p == '\\') && (parser_p->options.allow_string_escapes == true))
								{
									state = utlAT_STRING_STATE_EXTRACT_HEX_DIGIT1;
									num   = 0;                  /* initialize hex num */

								}
								else if ((state == utlAT_STRING_STATE_EXTRACT_HEX_DIGIT1) ||
									 (state == utlAT_STRING_STATE_EXTRACT_HEX_DIGIT2))
								{

									/*--- extract hexadecimal escape sequence digit ---*/
									if ((*src_p >= '0') && (*src_p <= '9')) num = (16 * num) + (*src_p - '0');
									else if ((*src_p >= 'A') && (*src_p <= 'F')) num = (16 * num) + (*src_p - 'A') + 10;
									else if ((*src_p >= 'a') && (*src_p <= 'f')) num = (16 * num) + (*src_p - 'a') + 10;
									else
									{
										/*--- if reverse-solidus was not followed by a hex character, just
										   interpret found characters as-is */
										if (string_buf_p < (string_buf + utlNumberOf(string_buf) - 2))
										{
											*string_buf_p++ = '\\';
											*string_buf_p++ = *src_p;
										}

										state = utlAT_STRING_STATE_PASS_THROUGH;
										continue;
									}

									if (state == utlAT_STRING_STATE_EXTRACT_HEX_DIGIT1)
									{
										state =  utlAT_STRING_STATE_EXTRACT_HEX_DIGIT2;

									}
									else if (state == utlAT_STRING_STATE_EXTRACT_HEX_DIGIT2)
									{
										state = utlAT_STRING_STATE_PASS_THROUGH;

										/*--- save hex value as a character (if there's room) ---*/
										if (string_buf_p < (string_buf + utlNumberOf(string_buf) - 1))
											*string_buf_p++ = num;
									}
									else
										state = utlAT_STRING_STATE_PASS_THROUGH;

									/*--- else save character (if there's room) ---*/
								}
								else if (string_buf_p < (string_buf + utlNumberOf(string_buf) - 1))
									*string_buf_p++ = *src_p;
							}
							if (string_buf_p >= (string_buf + utlNumberOf(string_buf)))
								string_buf_p  =  string_buf + utlNumberOf(string_buf) - 1;

							*string_buf_p++ = '\0';

							if (quoted == true)
							{
								/*--- check that string ends with a double quote ---*/
								if (*src_p != '"')
								{
									utlError("Extended-AT command syntax error at col=%d.", syntax_off + value_string_off + 1);
									parser_p->peg_counts.bad_commands++;
									return utlAT_RESULT_CODE_ERROR;
								}
							}
							else
							{
								char *first_p;
								char *last_p;

								if (parameter_p->type == utlAT_DATA_TYPE_STRING) first_p = parameter_value_p->value.string_p;
								else first_p = parameter_value_p->value.qstring_p;

								/*--- trim trailing white space from end of unquoted string ---*/
								for (last_p = string_buf_p - 1; last_p > first_p; last_p--)
									if (*(last_p - 1) != ' ')
									{
										*last_p = '\0';
										break;
									}
							}
						}
						break;

						case utlAT_DATA_TYPE_DIAL_STRING:
						{
							utlAtStringState_T state;
							bool alphabetical_digits;
							char               *src_p;

							parameter_value_p->value.dial_string_p = string_buf_p;

							/*--- extract string, replacing hex escape sequences with the ASCII
							   characters they represent */
							state = utlAT_STRING_STATE_PASS_THROUGH;

							alphabetical_digits = false;
							for (src_p = value_string; *src_p != '\0'; src_p++)
							{

								/*--- are we expecting alphabetical dial-string digits? ---*/
								if (alphabetical_digits == true)
								{
									if (((*src_p >= 'a') || (*src_p <= 'z')) ||
									    ((*src_p >= 'A') || (*src_p <= 'Z')))
									{
										if (string_buf_p < (string_buf + utlNumberOf(string_buf) - 1))
											*string_buf_p++ = *src_p;

									}
									else if (*src_p != ' ')
										alphabetical_digits = false;
								}

								if (alphabetical_digits == false)
								{

									/*--- this a possible embedded hex escape sequence? ---*/
									if ((*src_p == '\\') && (parser_p->options.allow_string_escapes == true))
									{
										state = utlAT_STRING_STATE_EXTRACT_HEX_DIGIT1;
										num   = 0;                  /* initialize hex num */

									}
									else if ((state == utlAT_STRING_STATE_EXTRACT_HEX_DIGIT1) ||
										 (state == utlAT_STRING_STATE_EXTRACT_HEX_DIGIT2))
									{

										/*--- extract hexadecimal escape sequence digit ---*/
										if ((*src_p >= '0') && (*src_p <= '9')) num = (16 * num) + (*src_p - '0');
										else if ((*src_p >= 'A') && (*src_p <= 'F')) num = (16 * num) + (*src_p - 'A') + 10;
										else if ((*src_p >= 'a') && (*src_p <= 'f')) num = (16 * num) + (*src_p - 'a') + 10;
										else
										{
											/*--- if reverse-solidus was not followed by a hex character, just
											   interpret found characters as-is */
											if (string_buf_p < (string_buf + utlNumberOf(string_buf) - 2))
											{
												*string_buf_p++ = '\\';
												*string_buf_p++ = *src_p;
											}

											state = utlAT_STRING_STATE_PASS_THROUGH;
											continue;
										}

										if (state == utlAT_STRING_STATE_EXTRACT_HEX_DIGIT1)
										{
											state =  utlAT_STRING_STATE_EXTRACT_HEX_DIGIT2;

										}
										else if (state == utlAT_STRING_STATE_EXTRACT_HEX_DIGIT2)
										{
											state = utlAT_STRING_STATE_PASS_THROUGH;

											/*--- process recognized dial-string characters ---*/
											if (strchr(utlAT_DIALING_CHARACTERS, num) != NULL)
											{

												/*--- are zero or more alphabetical dial-string digits to follow? ---*/
												if (num == '"')
													alphabetical_digits = true;

												/*--- save hex value as a character (if there's room) ---*/
												if (string_buf_p < (string_buf + utlNumberOf(string_buf) - 1))
													*string_buf_p++ = num;
											}

										}
										else
											state = utlAT_STRING_STATE_PASS_THROUGH;

										/*--- else process recognized dial-string characters ---*/
									}
									else if (strchr(utlAT_DIALING_CHARACTERS, *src_p) != NULL)
									{

										/*--- are zero or more alphabetical dial-string digits to follow? ---*/
										if (*src_p == '"')
											alphabetical_digits = true;

										/*--- save character (if there's room) ---*/
										if (string_buf_p < (string_buf + utlNumberOf(string_buf) - 1))
											*string_buf_p++ = *src_p;
									}
								}

								/*--- ignore all characters following a terminator character ---*/
								if (*src_p == parser_p->parameters.S[utlAT_TERMINATER_CHAR])
									break;
							}
							if (string_buf_p >= (string_buf + utlNumberOf(string_buf)))
								string_buf_p  =  string_buf + utlNumberOf(string_buf) - 1;

							*string_buf_p++ = '\0';
						}
						break;

						default:
							utlError("parameter_value_p->type!\n");
							break;
						}
					}

					/*--- advance to next parameter ---*/
					if (parameter_p++ >= (command_p->parameters_p + command_p->num_parameters))
					{
						utlError("Extended-AT command has too many parameters at col=%d.", syntax_off + c_p - parameters_string_p);
						parser_p->peg_counts.bad_commands++;
						return utlAT_RESULT_CODE_ERROR;
					}

					parameter_value_p++;
				}
			}

		}
		else
		{
			utlTrace(utlTRACE_AT_PARSER,
				 utlPrintTrace("AT%s (set)\n", command_name_p);
				 );
		}

		/*--- for each unspecified parameter value (if any)... ---*/
		while (parameter_p < (command_p->parameters_p + command_p->num_parameters))
		{

			/*--- too many parameter values? ---*/
			if (parameter_value_p >= (parameter_values + utlAT_MAX_PARAMETERS))
			{
				utlError("Too many AT-command parameters at col=%d.", syntax_off);
				parser_p->peg_counts.bad_commands++;
				return utlAT_RESULT_CODE_ERROR;
			}

			/*-- complain if unspecified parameter value is not Read, Read-only, and is required ---*/
			if (((parameter_p->access   != utlAT_PARAMETER_ACCESS_READ) &&
			     (parameter_p->access   != utlAT_PARAMETER_ACCESS_READ_ONLY)) &&
			    ( parameter_p->presence == utlAT_PARAMETER_PRESENCE_REQUIRED))
			{
				utlError("Missing parameter(s) in Extended-AT command at col=%d.", syntax_off);
				parser_p->peg_counts.bad_commands++;
				return utlAT_RESULT_CODE_ERROR;
			}

			/*--- assign unspecified parameter a default value, according to it's data-type... ---*/
			utlSetDefaultParameterValue(parameter_value_p, parameter_p->type, parameter_p->access);

			parameter_p++;
			parameter_value_p++;
		}

		/*--- else if this is a get-request, prepare `parameter_values' structure... ---*/
	}
	else if (request_type == utlAT_REQUEST_TYPE_GET)
	{
		utlAtParameter_P parameter_p;
		utlAtParameterValue_P parameter_value_p;

		/*--- this AT-command an "action" command (which we can't fetch) ---*/
		if (command_p->type == utlAT_COMMAND_TYPE_EXACTION)
		{
			if(strcasecmp(command_p->name_p, "*EnVsim") != 0) {
				utlError("Attempted to perform a \"read\" for an Extended-AT \"action\" command at col=%d.", syntax_off);
				parser_p->peg_counts.bad_commands++;
				return utlAT_RESULT_CODE_ERROR;
			}
		}

		parameter_p       = command_p->parameters_p;
		parameter_value_p = parameter_values;

		/*--- for each of this AT-Command's known parameters... ---*/
		while (parameter_p < (command_p->parameters_p + command_p->num_parameters))
		{

			/*--- are we attempting to fetch a write-only parameter? ---*/
			if (parameter_p->access == utlAT_PARAMETER_ACCESS_WRITE_ONLY)
			{
				utlError("Attempt to read write-only AT command parameter at col=%d.", syntax_off);
				parser_p->peg_counts.bad_commands++;
				return utlAT_RESULT_CODE_ERROR;
			}

			/*--- too many parameter values? ---*/
			if (parameter_value_p >= (parameter_values + utlAT_MAX_PARAMETERS))
			{
				utlError("Too many AT-command parameters at col=%d.", syntax_off);
				parser_p->peg_counts.bad_commands++;
				return utlAT_RESULT_CODE_ERROR;
			}

			/*--- assign a default parameter value, according to it's data-type... ---*/
			utlSetDefaultParameterValue(parameter_value_p, parameter_p->type, parameter_p->access);

			parameter_p++;
			parameter_value_p++;
		}
	}

	/*--- process extended built-in AT-commands ---*/
	{
		/*--- initially assume no test response is required ---*/
		info_text[0] = '\0';

		if ((rc = utlProcessExtendedAtCommand(parser_p,
						      request_type,
						      command_p,
						      parameter_values,
						      info_text,
						      utlNumberOf(info_text),
						      syntax_off)) == utlAT_RESULT_CODE_ERROR)
		{
			parser_p->peg_counts.bad_commands++;
			return utlAT_RESULT_CODE_ERROR;
		}
	}

	/*--- invoke AT-command call-back (as appropriate) ---*/
	switch (request_type)
	{
	case utlAT_REQUEST_TYPE_SET:
	{
		char *info_text_p;

		if (info_text[0] == '\0') info_text_p = NULL;
		else info_text_p = info_text;

		if (command_p->call_backs.set_parameter_function_p != NULL)
		{
			utlAtParameterValue_P parameter_values_p;
			utlAtAsyncResponse_P async_response_p;

			if (command_p->num_parameters > (size_t)0) parameter_values_p = parameter_values;
			else parameter_values_p = NULL;

			/*--- setup for pending asynchronous response ---*/
			if ((async_response_p = utlSetupPendingAsyncResponse(parser_p, (command_p->type == utlAT_COMMAND_TYPE_EXACTION) ? utlAT_ASYNC_OP_ACTION : utlAT_ASYNC_OP_SET, command_p)) == NULL)
			{
				utlError("utlSetupPendingAsyncResponse failed!\n");
				return utlAT_RESULT_CODE_ERROR;
			}

			if ((command_p->call_backs.set_parameter_function_p)((command_p->type == utlAT_COMMAND_TYPE_EXACTION) ? utlAT_PARAMETER_OP_ACTION : utlAT_PARAMETER_OP_SET,
									     command_name_p,
									     parameter_values_p,
									     command_p->num_parameters,
									     info_text_p,
									     &(async_response_p->xid),
									     parser_p->call_backs.arg_p) != utlSUCCESS)
			{

				utlError("set_parameter_function_p failed!\n");
				/*--- clean ---*/
				utlAbandonPendingAsyncResponse(async_response_p);

				parser_p->peg_counts.bad_commands++;
				return utlAT_RESULT_CODE_ERROR;
			}

		}

		rc = utlAT_RESULT_CODE_SUPPRESS;
	}
	break;

	case utlAT_REQUEST_TYPE_GET:
	{
		char *info_text_p;

		if (info_text[0] == '\0') info_text_p = NULL;
		else info_text_p = info_text;

		if (command_p->call_backs.get_parameter_function_p != NULL)
		{
			utlAtParameterValue_P parameter_values_p;
			utlAtAsyncResponse_P async_response_p;

			if (command_p->num_parameters > (size_t)0) parameter_values_p = parameter_values;
			else parameter_values_p = NULL;

			/*--- setup for pending asynchronous response ---*/
			if ((async_response_p = utlSetupPendingAsyncResponse(parser_p, utlAT_ASYNC_OP_GET, command_p)) == NULL)
			{
				utlError("utlSetupPendingAsyncResponse failed!\n");
				return utlAT_RESULT_CODE_ERROR;
			}

			if ((command_p->call_backs.get_parameter_function_p)(utlAT_PARAMETER_OP_GET,
									     command_name_p,
									     parameter_values_p,
									     command_p->num_parameters,
									     info_text_p,
									     &(async_response_p->xid),
									     parser_p->call_backs.arg_p) != utlSUCCESS)
			{
				/*--- clean ---*/
				utlError("get_parameter_function_p failed!\n");
				utlAbandonPendingAsyncResponse(async_response_p);

				parser_p->peg_counts.bad_commands++;
				return utlAT_RESULT_CODE_ERROR;
			}

		}

		rc = utlAT_RESULT_CODE_SUPPRESS;
	}
	break;

	default:
		utlError("Invalid request_type: %d!\n", request_type);
		break;
	}

	parser_p->peg_counts.extended_commands++;

	if (utlAcquireExclusiveAccess(&parser_p->cmd_cnt_semaphore) != utlSUCCESS)
	{
		utlError("%s: Cannot exclusively acquire semaphore!\n", __FUNCTION__);
		return utlAT_RESULT_CODE_ERROR;
	}

	if (rc == utlAT_RESULT_CODE_OK)
		parser_p->commands_in_line--;

	utlReleaseExclusiveAccess(&parser_p->cmd_cnt_semaphore);

	return rc;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlAddDialString(parser_p, dest_p, dest_size, dial_string_p)
* INPUT
*           parser_p == an open AT-command parser
*             dest_p == where to start adding `dial_string_p'
*          dest_size == maximum number of character to write to `dest_p'
*      dial_string_p == the dial-string to add
* OUTPUT
*      none
* RETURNS
*      the number of characters appended for success, otherwise utlFAILED
* DESCRIPTION
*      Add `dial_string_p' to the current dial string at position `dest_p',
*      converting any embedded alphabetical-digits to numeric digits, and
*      noting any embedded dial-string traits.
*---------------------------------------------------------------------------*/
static size_t utlAddDialString(const utlAtParser_P parser_p,
			       char          *dest_p,
			       const size_t dest_size,
			       const char          *dial_string_p)
{
	bool alphabetical_digits;
	char *start_dest_p;
	char  *term_dest_p;

	utlAssert(parser_p      != NULL);
	utlAssert(dest_p        != NULL);
	utlAssert(dest_size     >  (size_t)0);
	utlAssert(dial_string_p != NULL);

	start_dest_p = dest_p;
	term_dest_p = dest_p + dest_size;

	alphabetical_digits = false;
	for (; *dial_string_p != '\0'; dial_string_p++)
	{

		/*--- are we expecting alphabetical dial-string digits? ---*/
		if (alphabetical_digits == true)
		{

			/*--- map alphabetical dial-string digit to an actual number ---*/
			switch (*dial_string_p)
			{
			case 'a':  case 'A':  case 'b':  case 'B':  case 'c':  case 'C':                       *dest_p++ = '2';  break;
			case 'd':  case 'D':  case 'e':  case 'E':  case 'f':  case 'F':                       *dest_p++ = '3';  break;
			case 'g':  case 'G':  case 'h':  case 'H':  case 'i':  case 'I':                       *dest_p++ = '4';  break;
			case 'j':  case 'J':  case 'k':  case 'K':  case 'l':  case 'L':                       *dest_p++ = '5';  break;
			case 'm':  case 'M':  case 'n':  case 'N':  case 'o':  case 'O':                       *dest_p++ = '6';  break;
			case 'p':  case 'P':  case 'q':  case 'Q':  case 'r':  case 'R':  case 's':  case 'S': *dest_p++ = '7';  break;
			case 't':  case 'T':  case 'u':  case 'U':  case 'v':  case 'V':                       *dest_p++ = '8';  break;
			case 'w':  case 'W':  case 'x':  case 'X':  case 'y':  case 'Y':  case 'z':  case 'Z': *dest_p++ = '9';  break;

			case ' ':
				break;

			default:
				alphabetical_digits = false;
				break;
			}

			if (dest_p >= term_dest_p)
			{
				utlError("Dial-string buffer overflow.");
				return utlFAILED;
			}
		}

		if (alphabetical_digits == false)
		{
			if (*dial_string_p == '"')
				alphabetical_digits = true;

			else
			{
				*dest_p++ = *dial_string_p;

				if (dest_p >= term_dest_p)
				{
					utlError("Dial-string buffer overflow.");
					return utlFAILED;
				}

				switch (*dial_string_p)
				{
				case '/': parser_p->states.dial_string.dial_string_delay++;              break;
				case '+': parser_p->states.dial_string.options.international    = true;  break;
				case 'g': parser_p->states.dial_string.options.use_CCUG_SS_info = true;  break;
				case 'G': parser_p->states.dial_string.options.use_CCUG_SS_info = true;  break;
				case 'i': parser_p->states.dial_string.options.CLI_presentation = 'i';   break;
				case 'I': parser_p->states.dial_string.options.CLI_presentation = 'I';   break;
				default:                                                                 break;
				}
			}
		}
	}

	return dest_p - start_dest_p;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlParseATZ(parser_p, string_p, string_off)
* INPUT
*        parser_p == an open AT-command parser
*        string_p == the text characters to be parsed after the name "AT&Z"
*      string_off == parmater offset (for error reporting)
*      parse_len == length of string that has been parsed
* OUTPUT
*      none
* RETURNS
*      utlAT_RESULT_CODE_ERROR for failure, other values for success
* DESCRIPTION
*      Parse the  AT&Z command
*---------------------------------------------------------------------------*/

static utlAtResultCode_T utlParseATZ(const utlAtParser_P parser_p,
				     const char *string_p,
				     const size_t string_off,
				     int *parse_len)

{
	utlAtParameterValue_T parameter_value;
	unsigned int location;
	size_t location_off;
	const char *c_p = string_p;
	utlAtResultCode_T rc = utlAT_RESULT_CODE_OK;


	/*--- The following forms are accepted:
	   &Z<location>=<dial-string>
	   &Z<location>=L          (save last number dialed to <location>)
	   &Z=<dial-string>        (location defaults to 0)
	   &Z=L                    (location defaults to 0)
	   &Z<location>?
	   &ZL?                    (print last number dialed)
	   &Z?                     (location defaults to 0)

	   Note that due to the wide variety of characters that can legally
	   appear in dial strings (thus making it impossible, in general, to
	   determine where one &Z command ends and the next begins), the &Z
	   command must be the very last AT-command on the line. */

	location     = 0;
	location_off = string_off;

	/*---  extract (if present) location to save to (all digits following '&Z') ---*/
	while ((*c_p >= '0') &&
	       (*c_p <= '9'))
	{
		location = (10 * location) + (*c_p - '0');

		/*--- advance to next digit, skipping space characters ---*/
		do
			c_p++;
		while (*c_p == ' ');

		if (*c_p == '\0')
		{
			utlError("Incomplete AT-command at col=%d.", string_off + c_p - string_p + 1);
			parser_p->peg_counts.bad_commands++;
			return utlAT_RESULT_CODE_ERROR;
		}
	}

	/*--- set default parameter value ---*/
	utlSetDefaultParameterValue(&parameter_value, utlAT_DATA_TYPE_DIAL_STRING, utlAT_PARAMETER_ACCESS_READ_WRITE);

	/*--- have we possibly been asked for the last-dialed dial string? ---*/
	if ((*c_p == 'l') || (*c_p == 'L'))
	{

		/*--- advance to character following 'L', skipping space characters ---*/
		do
			c_p++;
		while (*c_p == ' ');

		/*--- if we have been asked for the last-dialed dial string ---*/
		if (*c_p == '?')
		{

			/*--- advance to character following '?', skipping space characters ---*/
			do
				c_p++;
			while (*c_p == ' ');

			/*--- reply with last-dialed dial string ---*/
			{
				if (parser_p->states.dial_string.last[0] != '\0')
				{
					parameter_value.is_default = false;

					utlTrace(utlTRACE_AT_PARSER,
						 utlPrintTrace("AT&ZL (get: %s)\n", parser_p->states.dial_string.last);
						 );
				}
				else
				{
					utlTrace(utlTRACE_AT_PARSER,
						 utlPrintTrace("AT&ZL (get)\n", location);
						 );
				}

				if (utlSendInfoResponse(parser_p, parser_p->states.dial_string.last) != utlSUCCESS)
					return utlAT_RESULT_CODE_ERROR;

				parameter_value.value.dial_string_p = parser_p->states.dial_string.last;
			}

			parser_p->peg_counts.basic_commands++;

		}
		else
		{
			utlError("Incomplete AT-command at col=%d.", string_off + c_p - string_p + 1);
			parser_p->peg_counts.bad_commands++;
			return utlAT_RESULT_CODE_ERROR;
		}

		/*--- have we been asked for the current value? ---*/
	}
	else if (*c_p == '?')
	{

		/*--- advance to character following '?', skipping space characters ---*/
		do
			c_p++;
		while (*c_p == ' ');

		/*--- reply with current value ---*/
		{
			const char *dial_string_p;

			dial_string_p = NULL;

			/*--- retrieve previously saved dial string ---*/
			if (parser_p->call_backs.retrieve_dial_string_function_p != NULL)
			{
				char location_name[20];
				const char *location_name_p;

				location_name_p = location_name;

				if ((utlDecimalToString(location_name, location, utlNumberOf(location_name)) == (size_t)0) ||
				    ((parser_p->call_backs.retrieve_dial_string_function_p)(&location_name_p, &dial_string_p, parser_p->call_backs.arg_p) != utlSUCCESS))
				{
					parser_p->peg_counts.bad_commands++;
					return utlAT_RESULT_CODE_ERROR;
				}
			}

			if (dial_string_p != NULL)
			{
				parameter_value.is_default = false;

				utlTrace(utlTRACE_AT_PARSER,
					 utlPrintTrace("AT&Z%d (get: %s)\n", location, dial_string_p);
					 );

				if (utlSendInfoResponse(parser_p, dial_string_p) != utlSUCCESS)
					return utlAT_RESULT_CODE_ERROR;

			}
			else
			{
				dial_string_p = "";

				utlTrace(utlTRACE_AT_PARSER,
					 utlPrintTrace("AT&Z%d (get)\n", location);
					 );
			}

			parameter_value.value.dial_string_p = (char *)dial_string_p;
		}

		parser_p->peg_counts.basic_commands++;

		/*--- have we been asked to save a new dial-string? ---*/
	}
	else if (*c_p == '=')
	{
		char dial_string[utlAT_MAX_LINE_LENGTH];
		char *term_dial_string_p;
		char      *dial_string_p;
		bool alphabetical_digits;

		/*--- advance to character following '=', skipping space characters ---*/
		do
			c_p++;
		while (*c_p == ' ');

		/*--- extract dial string ---*/
		alphabetical_digits = false;
		term_dial_string_p = dial_string + utlNumberOf(dial_string) - 1;
		for (dial_string_p = dial_string; *c_p != '\0'; c_p++)
		{

			/*--- are we expecting alphabetical dial-string digits? ---*/
			if (alphabetical_digits == true)
			{
				if (((*c_p >= 'a') || (*c_p <= 'z')) ||
				    ((*c_p >= 'A') || (*c_p <= 'Z')))
				{
					*dial_string_p++ = *c_p;

					if (dial_string_p >= term_dial_string_p)
					{
						utlError("Dial-string buffer overflow.");
						return utlAT_RESULT_CODE_ERROR;
					}

				}
				else if (*c_p != ' ')
					alphabetical_digits = false;
			}

			if (alphabetical_digits == false)
			{

				/*--- insert last dialed dial-string? ---*/
				if ((*c_p == 'l') || (*c_p == 'L'))
				{
					char *last_p;

					/*--- insert last dialed dial-string ---*/
					for (last_p = parser_p->states.dial_string.last; *last_p != '\0'; last_p++)
					{

						*dial_string_p++ = *last_p;

						if (dial_string_p >= term_dial_string_p)
						{
							utlError("Dial-string buffer overflow.");
							return utlAT_RESULT_CODE_ERROR;
						}
					}

					/*--- else save recognized dial-string characters ---*/
				}
				else if (strchr(utlAT_DIALING_CHARACTERS, *c_p) != NULL)
				{

					/*--- are zero or more alphabetical dial-string digits to follow? ---*/
					if (*c_p == '"')
						alphabetical_digits = true;

					*dial_string_p++ = *c_p;

					if (dial_string_p >= term_dial_string_p)
					{
						utlError("Dial-string buffer overflow.");
						return utlAT_RESULT_CODE_ERROR;
					}
				}
			}
		}
		*dial_string_p = '\0';

		utlTrace(utlTRACE_AT_PARSER,
			 utlPrintTrace("AT&Z%d (set: %s)\n", location, dial_string);
			 );

		/*--- invoke save-dial-string call-back (if any) ---*/
		if (parser_p->call_backs.save_dial_string_function_p != NULL)
		{
			char location_name[20];  /* plenty big for a 64-bit unsigned integer */

			if ((utlDecimalToString(location_name, location, utlNumberOf(location_name)) == (size_t)0) ||
			    ((parser_p->call_backs.save_dial_string_function_p)(     location_name, dial_string, parser_p->call_backs.arg_p) != utlSUCCESS))
			{
				parser_p->peg_counts.bad_commands++;
				return utlAT_RESULT_CODE_ERROR;
			}
		}

		parameter_value.value.dial_string_p = dial_string;
		parameter_value.is_default          = false;
		parser_p->peg_counts.basic_commands++;

	}
	else
	{
		utlError("Syntax error at col=%d.", string_off + c_p - string_p + 1);
		parser_p->peg_counts.bad_commands++;
		return utlAT_RESULT_CODE_ERROR;
	}

	*parse_len = c_p - string_p;

	if (utlAcquireExclusiveAccess(&parser_p->cmd_cnt_semaphore) != utlSUCCESS)
	{
		utlError("%s: Cannot exclusively acquire semaphore!\n", __FUNCTION__);
		return utlAT_RESULT_CODE_ERROR;
	}

	parser_p->commands_in_line--;

	utlReleaseExclusiveAccess(&parser_p->cmd_cnt_semaphore);

	return rc;

	/*--- extract decimal parameter (if any) ---*/
}


/*---------------------------------------------------------------------------*
* FUNCTION
*      utlParseATD(parser_p, string_p, string_off)
* INPUT
*        parser_p == an open AT-command parser
*        string_p == the text characters to be parsed after the name "ATD"
*      string_off == parmater offset (for error reporting)
*      parseATD == length of string that has been parsed
* OUTPUT
*      none
* RETURNS
*      utlAT_RESULT_CODE_ERROR for failure, other values for success
* DESCRIPTION
*      Parse the  ATD command
*---------------------------------------------------------------------------*/

static utlAtResultCode_T utlParseATD(const utlAtParser_P parser_p,
				     const char *string_p,
				     const size_t string_off,
				     int *parse_len)
{
	char command_name[] = "D";
	bool parameter_found = FALSE;
	char *term_dial_string_p;
	char      *dial_string_p;
	bool alphabetical_digits;
	const char * c_p = string_p;
	utlAtParameterValue_T parameter_value;
	utlAtResultCode_T rc;

	/*--- Notes on dial string processing:
	   - The end of the dial-string (and thus the `D' AT-command) is
	   demarked by either a semicolon character or by the end of the line.
	   - If not immediately preceded by `>', `"' causes subsequent characters
	   to be interpreted as alphabetic dial-string characters (which we
	   translate into digits here).
	   - If immediately preceded by `>', `"' delimits the start of a
	   dial-string-store location-name.	A second subsequenct `"' character
	   delimits the end of the dial-string-store location-name.
	   - 'L' is replaced by the last number dialed.
	   - 'S' is replaced by the stored dial-string associated with location 0.
	   - `S-<id>' is replaced by the stored dial-string associated with the specified location.
	   - Each '/' character delays initial dialing by an additional 0.125 seconds.
	   - A trailing ';' sets the do-call-origination flag to false.
	   - 'G' sets the use-CCUG-SS-info option to true.
	   - 'i' sets the CLI-presentation option to "allow".
	   - 'I' sets the CLI-presentation option to "restrict".
	   - '+' sets the international option to true. */
	parser_p->states.dial_string.active                                      = false;
	parser_p->states.dial_string.dial_string_delay                   = 0;                                                   /* no delay */
	parser_p->states.dial_string.c_p                                                 = parser_p->states.dial_string.buf;    /* reset */
	parser_p->states.dial_string.options.international               = false;
	parser_p->states.dial_string.options.do_call_origination = true;
	parser_p->states.dial_string.options.use_CCUG_SS_info    = false;
	parser_p->states.dial_string.options.CLI_presentation    = '\0';
	alphabetical_digits                                                                      = false;
	term_dial_string_p = parser_p->states.dial_string.buf + utlAT_MAX_DIAL_STRING_LENGTH;
	for (dial_string_p = parser_p->states.dial_string.buf; *c_p != '\0'; c_p++)
	{

		/*--- are we expecting alphabetical dial-string digits? ---*/
		if (alphabetical_digits == true)
		{

			/*--- map alphabetical dial-string digit to an actual number ---*/
			switch (*c_p)
			{
			case 'a':  case 'A':  case 'b':  case 'B':      case 'c':  case 'C':                                       *dial_string_p++ = '2';      break;
			case 'd':  case 'D':  case 'e':  case 'E':      case 'f':  case 'F':                                       *dial_string_p++ = '3';      break;
			case 'g':  case 'G':  case 'h':  case 'H':      case 'i':  case 'I':                                       *dial_string_p++ = '4';      break;
			case 'j':  case 'J':  case 'k':  case 'K':      case 'l':  case 'L':                                       *dial_string_p++ = '5';      break;
			case 'm':  case 'M':  case 'n':  case 'N':      case 'o':  case 'O':                                       *dial_string_p++ = '6';      break;
			case 'p':  case 'P':  case 'q':  case 'Q':      case 'r':  case 'R':  case 's':  case 'S': *dial_string_p++ = '7';      break;
			case 't':  case 'T':  case 'u':  case 'U':      case 'v':  case 'V':                                       *dial_string_p++ = '8';      break;
			case 'w':  case 'W':  case 'x':  case 'X':      case 'y':  case 'Y':  case 'z':  case 'Z': *dial_string_p++ = '9';      break;

			case ' ':
				break;

			default:
				alphabetical_digits = false;
				break;
			}

			if (dial_string_p >= term_dial_string_p)
			{
				utlError("Dial-string buffer overflow.");
				return utlAT_RESULT_CODE_ERROR;
			}
		}

		if (alphabetical_digits == false)
		{

			/*--- process recognized dial-string characters ---*/
			if (strchr(utlAT_DIALING_CHARACTERS, *c_p) != NULL)
			{

				/*--- are zero or more alphabetical dial-string digits to follow? ---*/
				if (*c_p == '"')
				{
					alphabetical_digits = true;

					/*--- else insert last dialed dial-string? ---*/
				}
				else if ((*c_p == 'l') || (*c_p == 'L'))
				{
					size_t rv;

					/*--- insert it... ---*/
					if ((rv = utlAddDialString(parser_p, dial_string_p, term_dial_string_p - dial_string_p, parser_p->states.dial_string.last)) == (size_t)utlFAILED)
						return utlAT_RESULT_CODE_ERROR;

					dial_string_p += rv;

					/*--- else this a reference to a stored dial-string? (syntax: S or S-<id>) ---*/
				}
				else if ((*c_p == 's') || (*c_p == 'S'))
				{
					unsigned int location;

					alphabetical_digits = false;

					/*--- advance to character following 'S', skipping space characters ---*/
					do
						c_p++;
					while (*c_p == ' ');

					location = 0;  /* default location */

					/*--- found a location lead-in character? ---*/
					if (*c_p == '-')
					{

						/*--- advance to character following '-', skipping space characters ---*/
						do
							c_p++;
						while (*c_p == ' ');

						/*--- extract location of stored dial-string to insert ---*/
						while ((*c_p >= '0') &&
						       (*c_p <= '9'))
						{

							location = (10 * location) + (*c_p - '0');

							/*--- advance to next digit, skipping space characters ---*/
							do
								c_p++;
							while (*c_p == ' ');
						}
						c_p--;

					}
					else
						c_p--;

					/*--- retrieve and insert dial-string... ---*/
					{
						const char *retrieved_p;

						retrieved_p = NULL;

						/*--- retrieve previously (hopefully) saved dial string ---*/
						if (parser_p->call_backs.retrieve_dial_string_function_p != NULL)
						{
							char location_name[20];
							const char *location_name_p;

							location_name_p = location_name;

							if ((utlDecimalToString(location_name, location, utlNumberOf(location_name)) == (size_t)0) ||
							    ((parser_p->call_backs.retrieve_dial_string_function_p)(&location_name_p, &retrieved_p, parser_p->call_backs.arg_p) != utlSUCCESS))
							{
								parser_p->peg_counts.bad_commands++;
								return utlAT_RESULT_CODE_ERROR;
							}
						}

						/*--- insert dial string (if any)... ---*/
						if (retrieved_p != NULL)
						{
							size_t rv;

							if ((rv = utlAddDialString(parser_p, dial_string_p, term_dial_string_p - dial_string_p, retrieved_p)) == (size_t)utlFAILED)
								return utlAT_RESULT_CODE_ERROR;

							dial_string_p += rv;
						}
					}

					/*--- else this a 3G phone-book dial-string reference? ---*/
				}
				else if (*c_p == '>')
				{
					char location_name[utlAT_MAX_STORED_LOCATION_NAME_LENGTH + 1];
					char *term_location_name_p;
					char      *location_name_p;
					enum {
						utl3G_LOCATION_NAME_TYPE_NUMBER,
						utl3G_LOCATION_NAME_TYPE_MEMORY_TEXT,
						utl3G_LOCATION_NAME_TYPE_MEMORY_NUMBER,
						utl3G_LOCATION_NAME_TYPE_STRING_START,
						utl3G_LOCATION_NAME_TYPE_STRING_END
					} location_name_type;

					/*--- The following forms of dial-string storage references are accepted:

					   ATD><number>			(eg: ATD>55)
					   ATD><mem><number>    (eg: ATD>usim89)
					   ATD>"<string>"		(eg: ATD>"fred")

					   Note that dial-string storage entries are indexed by
					   what ever text follows the "ATD>" command.  This means
					   that dial-string storage entries that are indexed by
					   strings must include the double-quotes in the dial-string
					   storage entry.  You can store phone numbers under strings
					   as follows (note the use of hex escapes to enter the
					   needed double-quotes):  AT+ASTO="\22fred\22",123-4567 */

					alphabetical_digits = false;

					/*--- advance to character following '>', skipping space characters ---*/
					do
						c_p++;
					while (*c_p == ' ');

					/*--- identify type of location_name ---*/
					if (*c_p == '"') location_name_type = utl3G_LOCATION_NAME_TYPE_STRING_START;
					else if (isdigit(*c_p)) location_name_type = utl3G_LOCATION_NAME_TYPE_NUMBER;
					else if (isalpha(*c_p)) location_name_type = utl3G_LOCATION_NAME_TYPE_MEMORY_TEXT;
					else
					{
						utlError("Dial-string syntax error.");
						parser_p->peg_counts.bad_commands++;
						return utlAT_RESULT_CODE_ERROR;
					}

					/*--- extract phone-book location name ---*/
					term_location_name_p = location_name + utlAT_MAX_STORED_LOCATION_NAME_LENGTH;
					for (location_name_p = location_name; *c_p != '\0'; )
					{
						bool found_end;

						found_end = false;
						switch (location_name_type)
						{
						case utl3G_LOCATION_NAME_TYPE_NUMBER:
							if (!isdigit(*c_p))
								found_end = true;
							break;

						case utl3G_LOCATION_NAME_TYPE_MEMORY_TEXT:
							if (!isalpha(*c_p))
							{
								if (!isdigit(*c_p))
									found_end = true;
								else
									location_name_type = utl3G_LOCATION_NAME_TYPE_MEMORY_NUMBER;
							}
							break;

						case utl3G_LOCATION_NAME_TYPE_MEMORY_NUMBER:
							if (!isdigit(*c_p))
								found_end = true;
							break;

						case utl3G_LOCATION_NAME_TYPE_STRING_START:
							location_name_type = utl3G_LOCATION_NAME_TYPE_STRING_END;
							break;

						case utl3G_LOCATION_NAME_TYPE_STRING_END:
							if (*c_p == '"')
							{
								found_end = true;

								*location_name_p++ = *c_p++;

								if (location_name_p >= term_location_name_p)
								{
									utlError("Dial-string buffer overflow.");
									return utlAT_RESULT_CODE_ERROR;
								}
							}
							break;

						default:
							utlError("Invalid location_name_type: %d\n", location_name_type);
							break;
						}

						/*--- we reach the end of the location name? ---*/
						if ((found_end == true) || (*c_p == parser_p->parameters.S[utlAT_TERMINATER_CHAR]))
							break;

						*location_name_p++ = *c_p++;

						if (location_name_p >= term_location_name_p)
						{
							utlError("Dial-string buffer overflow.");
							return utlAT_RESULT_CODE_ERROR;
						}
					}
					c_p--;
					*location_name_p = '\0';

					/*--- retrieve and insert dial-string... ---*/
					{
						const char *retrieved_p;

						retrieved_p = NULL;

						/*--- retrieve previously (hopefully) saved dial string ---*/
						if (parser_p->call_backs.retrieve_dial_string_function_p != NULL)
						{
							location_name_p = location_name;

							if ((parser_p->call_backs.retrieve_dial_string_function_p)((const char **)&location_name_p, &retrieved_p, parser_p->call_backs.arg_p) != utlSUCCESS)
							{
								parser_p->peg_counts.bad_commands++;
								return utlAT_RESULT_CODE_ERROR;
							}
						}

						/*--- insert dial string (if any)... ---*/
						if (retrieved_p != NULL)
						{
							size_t rv;

							if ((rv = utlAddDialString(parser_p, dial_string_p, term_dial_string_p - dial_string_p, retrieved_p)) == (size_t)utlFAILED)
								return utlAT_RESULT_CODE_ERROR;

							dial_string_p += rv;
						}
					}

					/*--- insert regular dial-string digit ---*/
				}
				else
				{
					*dial_string_p++ = *c_p;

					if (dial_string_p >= term_dial_string_p)
					{
						utlError("Dial-string buffer overflow.");
						return utlAT_RESULT_CODE_ERROR;
					}
				}

				switch (*c_p)
				{
				case '/': parser_p->states.dial_string.dial_string_delay++;                      break;
				case '+': parser_p->states.dial_string.options.international    = true;  break;
				case 'g': parser_p->states.dial_string.options.use_CCUG_SS_info = true;  break;
				case 'G': parser_p->states.dial_string.options.use_CCUG_SS_info = true;  break;
				case 'i': parser_p->states.dial_string.options.CLI_presentation = 'i';   break;
				case 'I': parser_p->states.dial_string.options.CLI_presentation = 'I';   break;
				default:                                                                                                                                 break;
				}

			}
			else
			{
				/*--- ignore illegal dial-string characters ---*/
			}

			/*--- found end of dialing command? ---*/
			if (*c_p == parser_p->parameters.S[utlAT_TERMINATER_CHAR])
			{
				*dial_string_p++ = *c_p++;      /* save & skip semicolon character */

				if (dial_string_p >= term_dial_string_p)
				{
					utlError("Dial-string buffer overflow.");
					return utlAT_RESULT_CODE_ERROR;
				}

				parser_p->states.dial_string.options.do_call_origination = false;
				break;
			}
		}
	}
	*dial_string_p = '\0';

	/*--- is dial-string empty? ---*/
	if (strlen(parser_p->states.dial_string.buf) == (size_t)0)
	{
		parser_p->peg_counts.bad_commands++;
		return utlAT_RESULT_CODE_ERROR;
	}

	parser_p->states.dial_string.active = true;

	/*--- update "last dialed" dial string ---*/
	strncpy(parser_p->states.dial_string.last, parser_p->states.dial_string.buf, utlAT_MAX_DIAL_STRING_LENGTH);
	parser_p->states.dial_string.last[utlAT_MAX_DIAL_STRING_LENGTH] = '\0';

	/*--- execute basic AT-command ---*/
	{
		utlSetDefaultParameterValue(&parameter_value, utlAT_DATA_TYPE_DIAL_STRING, utlAT_PARAMETER_ACCESS_READ_WRITE);

		parameter_value.is_default                      = false;
		parameter_value.value.dial_string_p = parser_p->states.dial_string.buf;

		if ((rc = utlExecuteBasicAtCommand(parser_p, command_name, parameter_found, &parameter_value, string_off)) == utlAT_RESULT_CODE_ERROR)
			return utlAT_RESULT_CODE_ERROR;
	}

	/*--- for ATD, we'll defer issuing a result code until we know the status of
	   the request, which will require traversal of the AT-parser state machine */
	*parse_len = c_p - string_p;
	return utlAT_RESULT_CODE_SUPPRESS;

}

/*---------------------------------------------------------------------------*
* FUNCTION
*     utlGetDefaultSParameterValue(int parameter_num)
* INPUT
*      parameter_num == S parameter index
* OUTPUT
* RETURNS
*      default value of given S parameter
* DESCRIPTION
*      get default value of given S parameter
*---------------------------------------------------------------------------*/

static unsigned short utlGetDefaultSParameterValue(int parameter_num)
{
	unsigned short val = 0;

	switch(parameter_num)
	{
	case utlAT_AUTO_ANSWER:           val = 0;        break; /* disable auto-answer */
	case utlAT_RING_COUNTER:          val = 0;        break; /* cleared             */
	case utlAT_ESCAPE_CHAR:           val = '+';      break;
	case utlAT_LINE_TERM_CHAR:        val = '\015';   break; /* <carriage-val =>   */
	case utlAT_FORMATTING_CHAR:       val = '\012';   break; /* <new-line>          */
	case utlAT_LINE_EDIT_CHAR:        val = '\010';   break; /* <back-space>        */
	case utlAT_BLIND_DIAL_PAUSE_TIME: val = 2;        break; /* 2 seconds           */
	case utlAT_CONN_COMPLETE_TIMEOUT: val = 1;        break; /* 1 second            */
	case utlAT_DIALING_PAUSE_TIME:    val = 2;        break; /* 2 seconds           */
	case utlAT_CARRIER_DETECT_TIME:   val = 6;        break; /* 0.6 seconds         */
	case utlAT_CARRIER_LOSS_TIME:     val = 7;        break; /* 0.7 seconds         */
	case utlAT_DTMF_TONE_DURATION:    val = 63;       break; /* 0.63 seconds        */
	case utlAT_ESCAPE_GUARD_TIME:     val = 50;       break; /* 1 second            */
	case utlAT_SEPARATER_CHAR:        val = ',';      break;
	case utlAT_TERMINATER_CHAR:       val = ';';      break;
	case utlAT_XON_CHAR:              val = 0x11;     break; /* <DC1>               */
	case utlAT_XOFF_CHAR:             val = 0x13;     break; /* <DC3>               */
	case utlAT_SLEEP_TIME:            val = 0;        break; /* 0 seconds           */
	case utlAT_DTR_DELAY_TIME:        val = 10;       break; /* 0.1 seconds         */
	case utlAT_HOOK_FLASH_TIME:       val = 5;        break; /* 0.5 seconds         */
	case utlAT_INACTIVITY_TIME:       val = 10;       break; /* 100 seconds         */
	case utlAT_DISCONNECT_WAIT_TIME:  val = 0;        break; /* 0 seconds           */
	default:                          val = 0;        break;
	}

	return val;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlParseATS(parser_p, string_p, string_off)
* INPUT
*        parser_p == an open AT-command parser
*        string_p == the text characters to be parsed after "ATS"
*        string_off == parmater offset (for error reporting)
*        parse_len == length of string that has been parsed
* OUTPUT
*      none
* RETURNS
*      utlAT_RESULT_CODE_ERROR for failure, other values for success
* DESCRIPTION
*      Parse the ATS command
*---------------------------------------------------------------------------*/
static utlAtResultCode_T utlParseATS(const utlAtParser_P parser_p,
				     const char          *string_p,
				     const size_t string_off,
				     int *parse_len)
{
	unsigned int parameter_num;
	size_t parameter_num_off;
	const char *c_p = string_p;

	/*--- The following forms are accepted:
	   S<parameter-num>=<value>
	   S=<value>                     (parameter-num defaults to 0)
	   S<parameter-num>?
	   S?					 (parameter-num defaults to 0)

	   Note that we support having multiple "S" AT-commands on
	   the same line. */


	parameter_num     = 0;
	parameter_num_off = string_off; // c_p - string_p + 1;

	/*--- extract (if present) S-parameter number (all digits following 'S') ---*/
	while ((*c_p >= '0') &&
	       (*c_p <= '9'))
	{
		parameter_num = (10 * parameter_num) + (*c_p - '0');

		/*--- advance to next digit, skipping space characters ---*/
		do
			c_p++;
		while (*c_p == ' ');

		if (*c_p == '\0')
		{
			utlError("Incomplete S-parameter at col=%d.", string_off + c_p - string_p + 1);
			parser_p->peg_counts.bad_commands++;
			return utlAT_RESULT_CODE_ERROR;
		}
	}

	/*--- check sanity of found S-parameter number ---*/
	if (parameter_num >= utlNumberOf(parser_p->parameters.S))
	{
		utlError("Invalid S-parameter number %d at col=%d.", parameter_num, string_off + parameter_num_off);
		parser_p->peg_counts.bad_commands++;
		return utlAT_RESULT_CODE_ERROR;
	}

	/*--- have we been asked for the S-parameter's current value? ---*/
	if (*c_p == '?')
	{

		/*--- advance to character following '?', skipping space characters ---*/
		do
			c_p++;
		while (*c_p == ' ');

		utlTrace(utlTRACE_AT_PARSER,
			 utlPrintTrace("ATS%d (get: %d)\n", parameter_num, parser_p->parameters.S[parameter_num]);
			 );

		/*--- reply with S-parameter's current value (a 3-digit decimal value) ---*/
		{
			char buf[10];
			size_t i;
			size_t rv;

			i = 0;

			/*--- pad value with leading zeros (as necessary) ---*/
			if (parser_p->parameters.S[parameter_num] < 100) buf[i++] = '0';
			if (parser_p->parameters.S[parameter_num] <  10) buf[i++] = '0';

			if ((rv = utlDecimalToString(buf + i, parser_p->parameters.S[parameter_num], utlNumberOf(buf) - i)) == (size_t)0)
			{
				utlError("Value-buffer overflow.");
				return utlAT_RESULT_CODE_ERROR;
			}
			i += rv;

			buf[i] = '\0';

			if (utlSendInfoResponse(parser_p, buf) != utlSUCCESS)
				return utlAT_RESULT_CODE_ERROR;
		}

		parser_p->peg_counts.basic_commands++;

		/*--- have we been asked to set S-parameter to a new value? ---*/
	}
	else if (*c_p == '=')
	{
		unsigned int parameter_value;
		size_t parameter_value_off;

		/*--- advance to character following '=', skipping space characters ---*/
		do
			c_p++;
		while (*c_p == ' ');

		parameter_value         = 0;
		parameter_value_off = c_p - string_p + 1;

		/*--- was no S-parameter value specified? ---*/
		if (((*c_p < '0') ||
		     (*c_p > '9')) && ((parser_p->options.allow_default_S_parameter_values == false) ||
			 ((*c_p != parser_p->parameters.S[utlAT_TERMINATER_CHAR]) && 
			 (*c_p != parser_p->parameters.S[utlAT_LINE_TERM_CHAR]) && (*c_p != 0))))
		{
			utlError("S-parameter syntax error at col=%d.", string_off + c_p - string_p + 1);
			parser_p->peg_counts.bad_commands++;
			return utlAT_RESULT_CODE_ERROR;
		}

		/* if allow_default_S_parameter_values is set to true, we need to set default
		 * value for parameter_value */
		if(((*c_p < '0') || (*c_p >'9')) && (parser_p->options.allow_default_S_parameter_values == true))
		{
			parameter_value = utlGetDefaultSParameterValue(parameter_num);
		}

		/*--- extract S-parameter's new (numeric) value ---*/
		while ((*c_p >= '0') &&
		       (*c_p <= '9'))
		{
			parameter_value = (10 * parameter_value) + (*c_p - '0');

			if (*++c_p == '\0')
				break;
		}

		utlTrace(utlTRACE_AT_PARSER,
			 utlPrintTrace("ATS%d (set: %d)\n", parameter_num, parameter_value);
			 );

		/*--- process new S-parameter value ---*/
		if (utlProcessSParameter(parser_p, parameter_num, parameter_value) != utlSUCCESS)
		{
			parser_p->peg_counts.bad_commands++;
			return utlAT_RESULT_CODE_ERROR;
		}

		/*--- notify application of S-parameter value change ---*/
		if (     parser_p->call_backs.s_parameter_function_p != NULL)
		{
			if ((parser_p->call_backs.s_parameter_function_p)(parameter_num,
									  parameter_value,
									  parser_p->call_backs.arg_p) != utlSUCCESS)
			{
				parser_p->peg_counts.bad_commands++;
				return utlAT_RESULT_CODE_ERROR;
			}
		}

		/*--- save new S-parameter value ---*/
		parser_p->parameters.S[parameter_num] = parameter_value;
		parser_p->peg_counts.basic_commands++;

	}
	else
	{
		utlError("S-parameter syntax error at col=%d.", string_off + c_p - string_p + 1);
		parser_p->peg_counts.bad_commands++;
		return utlAT_RESULT_CODE_ERROR;
	}

	*parse_len = c_p - string_p;

	if (utlAcquireExclusiveAccess(&parser_p->cmd_cnt_semaphore) != utlSUCCESS)
	{
		utlError("%s: Cannot exclusively acquire semaphore!\n", __FUNCTION__);
		return utlAT_RESULT_CODE_ERROR;
	}

	parser_p->commands_in_line--;

	utlReleaseExclusiveAccess(&parser_p->cmd_cnt_semaphore);

	return utlAT_RESULT_CODE_OK;

}


/*---------------------------------------------------------------------------*
* FUNCTION
*      utlParseLine(parser_p, string_p, string_off)
* INPUT
*        parser_p == an open AT-command parser
*        string_p == the text characters to be parsed
*      string_off == offset from start of line to `string_p' (for error
*                    reporting)
* OUTPUT
*      none
* RETURNS
*      utlAT_RESULT_CODE_ERROR for failure, other values for success
* DESCRIPTION
*      Parses a line of text, searching for an AT command to execute, and
*      executing it when found.  Note that as-per the standard, only the first
*      collection of AT commands on each line is parsed and executed.
*---------------------------------------------------------------------------*/
static utlAtResultCode_T utlParseLine(const utlAtParser_P parser_p,
				      char          *string_p,
				      const size_t string_off)
{
	const char        *c_p;
	utlAtResultCode_T rc;

	utlAssert(parser_p != NULL);
	utlAssert(string_p != NULL);

	rc = utlAT_RESULT_CODE_OK;

	/*--- scan for AT prefix ---*/
	{
		char last_c;

		last_c = '\0';
		for (c_p = string_p; *c_p != '\0'; c_p++)
		{

			/*--- ignore space characters ---*/
			if (*c_p == ' ')
				continue;

			/*--- found an AT prefix? ---*/
			if (((last_c == 'A') && (*c_p == 'T')) ||
			    ((last_c == 'a') && (*c_p == 't')))
				break;

			last_c = *c_p;
		}

		/*--- no AT prefix found? ---*/
		if (*c_p == '\0')
			return utlAT_RESULT_CODE_SUPPRESS;
	}

	/*--- advance to character following AT prefix, skipping space characters ---*/
	do
		c_p++;
	while (*c_p == ' ');

	/* Special process for the command "AT", we will delay the "OK" response until all the services have been registered */
	if (*c_p == '\0')
	{
		utlAtParameterValue_T parameter_value;

		utlSetDefaultParameterValue(&parameter_value, utlAT_DATA_TYPE_DIAL_STRING, utlAT_PARAMETER_ACCESS_READ_WRITE);

		if (utlAcquireExclusiveAccess(&parser_p->cmd_cnt_semaphore) != utlSUCCESS)
		{
			utlError("%s: Cannot exclusively acquire semaphore!\n", __FUNCTION__);
			return utlAT_RESULT_CODE_ERROR;
		}

		parser_p->commands_in_line++;

		utlReleaseExclusiveAccess(&parser_p->cmd_cnt_semaphore);

		if ((rc = utlExecuteBasicAtCommand(parser_p, " ", FALSE, &parameter_value, string_off)) == utlAT_RESULT_CODE_ERROR)
			return utlAT_RESULT_CODE_ERROR;
		return rc;
	}

	/* Count how many AT commands in one line*/
	{
		const char *temp = c_p;

		if (utlAcquireExclusiveAccess(&parser_p->cmd_cnt_semaphore) != utlSUCCESS)
		{
			utlError("%s: Cannot exclusively acquire semaphore!\n", __FUNCTION__);
			return utlAT_RESULT_CODE_ERROR;
		}

		parser_p->commands_in_line = 0;
		while (*temp != '\0')
		{
			bool quoted = false;
			parser_p->commands_in_line++;

			/*--- skip to next AT command ---*/
			for (; *temp != '\0'; temp++)
			{
				if(*temp == '"')
					quoted = !quoted;
				if (*temp == parser_p->parameters.S[utlAT_TERMINATER_CHAR] && !quoted)
				{
					temp++;  /* skip terminator char */
					break;
				}
			}

			/*--- skip white space ---*/
			while (*temp == ' ')
				temp++;
		}
		utlPrintTrace("%s: parser_p->commands_in_line:%d\n", __FUNCTION__, parser_p->commands_in_line);

		utlReleaseExclusiveAccess(&parser_p->cmd_cnt_semaphore);
	}

	/*--- extract and process each AT-command following the AT prefix... ---*/
	while (*c_p != '\0')
	{

		/*--- if this is an "extended" AT command ---*/
		if (strchr(utlAT_EXTENDED_COMMAND_PREFIXES, *c_p) != NULL)
		{
			char prefix_char;
			char command_name[utlAT_MAX_COMMAND_LENGTH_WITH_PREFIX];   /* prefix character + 16 characters + null */
			size_t command_name_off;

			prefix_char = *c_p;

			/*--- advance to character following extended command prefix, skipping space characters ---*/
			do
				c_p++;
			while (*c_p == ' ');

			command_name_off = c_p - string_p + 1;

			/*--- character immediately following command prefix must be alphabetic ---*/
			if (((*c_p < 'A') || (*c_p > 'Z')) &&
			    ((*c_p < 'a') || (*c_p > 'z')))
			{
				utlError("Invalid extended-AT command name syntax at col=%d.", string_off + command_name_off);
				return utlAT_RESULT_CODE_ERROR;
			}

			/*--- extract extended command name (can be up to 16 characters long) ---*/
			{
				size_t i;

				command_name[0] = prefix_char;

				for (i = 1; *c_p != '\0'; )
				{

					if (i >= (utlNumberOf(command_name) - 1))
					{
						utlError("Extended-AT command name too long at col=%d.", string_off + command_name_off);
						return utlAT_RESULT_CODE_ERROR;
					}

					command_name[i++] = *c_p;

					do
						c_p++;
					while (*c_p == ' ');

					/*--- only certain characters can appear in command names ---*/
					if (((*c_p  < 'A') || (*c_p  > 'Z')) &&
					    ((*c_p  < 'a') || (*c_p  > 'z')) &&
					    ((*c_p  < '0') || (*c_p  > '9')) &&
					    (*c_p != '!') && (*c_p != '%')  &&
					    (*c_p != '-') && (*c_p != '.')  &&
					    (*c_p != '/') && (*c_p != ':')  && (*c_p != '_'))
						break;
				}
				command_name[i++] = '\0';
				utlAssert(i <= utlNumberOf(command_name));
			}

			/*--- process extended AT command... ---*/
			{
				/*--- have we been asked for the current parameter value(s)? ---*/
				if (*c_p == '?')
				{

					/*--- advance to character following '?', skipping space characters ---*/
					do
						c_p++;
					while (*c_p == ' ');

					/*--- get current parameter value(s) ---*/
					if ((rc = utlExecuteExtendedAtCommand(parser_p, utlAT_REQUEST_TYPE_GET, command_name, NULL, string_off + c_p - string_p)) == utlAT_RESULT_CODE_ERROR)
						return utlAT_RESULT_CODE_ERROR;

					/*--- else have we been asked to set default parameter value(s)? ---*/
				}
				else if ((*c_p == '\0') || (*c_p == parser_p->parameters.S[utlAT_TERMINATER_CHAR]))
				{

					/*--- set parameters using default parameter value(s) ---*/
					if ((rc = utlExecuteExtendedAtCommand(parser_p, utlAT_REQUEST_TYPE_SET, command_name, NULL, string_off + c_p - string_p)) == utlAT_RESULT_CODE_ERROR)
						return utlAT_RESULT_CODE_ERROR;

					/*--- else have we been asked to set parameter values or for the command's syntax? ---*/
				}
				else if (*c_p == '=')
				{

					/*--- advance to character following '=', skipping space characters ---*/
					do
						c_p++;
					while (*c_p == ' ');

					/*--- have we been asked for the extended AT command's syntax? ---*/
					if (*c_p == '?')
					{

						/*--- advance to character following '?', skipping space characters ---*/
						do
							c_p++;
						while (*c_p == ' ');

						/*--- reply with command syntax ---*/
						if ((rc = utlExecuteExtendedAtCommand(parser_p, utlAT_REQUEST_TYPE_SYNTAX, command_name, NULL, string_off + c_p - string_p)) == utlAT_RESULT_CODE_ERROR)
							return utlAT_RESULT_CODE_ERROR;

						/*--- else have we been asked to set new parameter value(s)? ---*/
					}
					else
					{
						char parameters_string[utlAT_MAX_LINE_LENGTH];
						size_t parameters_string_off;
						bool quoted = false;

						parameters_string_off = c_p - string_p;

						/*--- collect AT-command's parameters into a string ---*/
						{
							char *dest_p;

							for (dest_p = parameters_string; *c_p != '\0'; c_p++)
							{
								if(*c_p == '"')
									quoted = !quoted;

								if((*c_p == parser_p->parameters.S[utlAT_TERMINATER_CHAR]) && !quoted)
									break;

								*dest_p++ = *c_p;
							}
							*dest_p = '\0';
						}

						/*--- set current parameter value(s) ---*/
						if ((rc = utlExecuteExtendedAtCommand(parser_p, utlAT_REQUEST_TYPE_SET, command_name, parameters_string, string_off + parameters_string_off)) == utlAT_RESULT_CODE_ERROR)
							return utlAT_RESULT_CODE_ERROR;
					}

				}
				else
				{
					utlError("Invalid extended-AT command syntax at col=%d.", string_off + c_p - string_p + 1);
					parser_p->peg_counts.bad_commands++;
					return utlAT_RESULT_CODE_ERROR;
				}
			}

			/*--- else assume this is a "basic" AT command ---*/
		}
		else
		{
			char command_name[3];
			size_t command_name_off;
			bool parameter_found;
			utlAtParameterValue_T parameter_value;
			int parse_len = 0;

			/*--- extract command name ---*/
			{
				size_t i;

				i = 0;

				command_name_off = c_p - string_p + 1;

				/*--- this a two-character basic AT command? ---*/
				if (strchr(utlAT_BASIC_COMMAND_PREFIXES, *c_p) != NULL)
					command_name[i++] = *c_p++;

				/*--- skip white space ---*/
				while (*c_p == ' ')
					c_p++;

				/*--- command character must be alphabetic ---*/
				if (((*c_p < 'A') || (*c_p > 'Z')) &&
				    ((*c_p < 'a') || (*c_p > 'z')))
				{
					utlError("AT command syntax error at col=%d.", string_off + command_name_off);
					parser_p->peg_counts.bad_commands++;
					return utlAT_RESULT_CODE_ERROR;
				}

				command_name[i++] = *c_p;
				command_name[i++] = '\0';
			}

			/*--- advance to character following command character, skipping space characters ---*/
			do
				c_p++;
			while (*c_p == ' ');

			parameter_found = false;

			/*--- this an "S-parameter" (ATS) command? ---*/
			if (strcasecmp(command_name, "S") == 0)
			{
				rc = utlParseATS(parser_p, c_p, (string_off + c_p - string_p), &parse_len);
				if (rc != utlAT_RESULT_CODE_ERROR)
					c_p += parse_len + 1;
				else
					return rc;
				/*--- else this a "dial" (ATD) command? ---*/
			}
			else if (strcasecmp(command_name, "D") == 0)
			{

				rc = utlParseATD(parser_p, c_p, (string_off + c_p - string_p), &parse_len);
				if (rc != utlAT_RESULT_CODE_ERROR)
					c_p += parse_len + 1;
				else
					return rc;
				/*--- else this a "save dial string" (AT&Z) command? ---*/
			}
			else if (strcasecmp(command_name, "&Z") == 0)
			{
				rc = utlParseATZ(parser_p, c_p, (string_off + c_p - string_p), &parse_len);
				if (rc != utlAT_RESULT_CODE_ERROR)
					c_p += parse_len + 1;
				else
					return rc;
			}
			else
			{
				utlSetDefaultParameterValue(&parameter_value, utlAT_DATA_TYPE_DECIMAL, utlAT_PARAMETER_ACCESS_READ_WRITE);

				/* check the parameter */
				if(((*c_p < '0') || (*c_p > '9')) && 
					(*c_p != parser_p->parameters.S[utlAT_TERMINATER_CHAR]) && 
					(*c_p != parser_p->parameters.S[utlAT_LINE_TERM_CHAR]) && 
					(*c_p != 0))
					return utlAT_RESULT_CODE_ERROR;

				/*--- parameter (if present) must be composed of digits ---*/
				while ((*c_p >= '0') &&
				       (*c_p <= '9'))
				{
					parameter_value.value.decimal = (10 * parameter_value.value.decimal) + (*c_p - '0');
					parameter_found = true;

					/*--- advance to next digit, skipping space characters ---*/
					do
						c_p++;
					while (*c_p == ' ');
				}

				parameter_value.is_default = !parameter_found;

				/*--- execute basic AT-command ---*/
				if ((rc = utlExecuteBasicAtCommand(parser_p, command_name, parameter_found, &parameter_value, string_off + command_name_off)) == utlAT_RESULT_CODE_ERROR)
					return utlAT_RESULT_CODE_ERROR;
			}
		}

		/*--- skip to end of current extended-AT command ---*/
		bool quoted = false;
		for (; *c_p != '\0'; c_p++)
		{
			if(*c_p == '"')
				quoted = !quoted;
			if (*c_p == parser_p->parameters.S[utlAT_TERMINATER_CHAR] && !quoted)
			{
				c_p++;  /* skip terminator char */
				break;
			}
		}

		/*--- skip white space ---*/
		while (*c_p == ' ')
			c_p++;
	}

	/*--- apply any pending configuration changes ---*/
	if (parser_p->states.dce_io_config_pending_mask != (unsigned int)0)
	{
		if (utlAtDceIoConfigEvent(parser_p) != utlSUCCESS)
			return utlAT_RESULT_CODE_ERROR;
	}
	if (parser_p->states.sound_config_pending_mask != (unsigned int)0)
	{
		if (utlAtSoundConfigEvent(parser_p) != utlSUCCESS)
			return utlAT_RESULT_CODE_ERROR;
	}

	/*--- return result-code of last command executed ---*/
	return rc;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utAtParse(parser_p, octets_p, n)
* INPUT
*      parser_p == an open AT-command parser
*      octets_p == more octets to be parsed
*             n == number of octets pointed to by `octets_p'
* OUTPUT
*      none
* RETURNS
*      utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*      Appends the characters specified by `string_p' to a line buffer,
*      parsing AT-commands when possible.  This routine is responsible for:
*
*        - Collecting characters into lines, where lines are terminated using
*          the character specified by parser_p->parameters.S[utlAT_LINE_TERM_CHAR]
*        - Echoing accepted text (when echoing is enabled).
*        - Processing back-space requests.  In this implementation one cannot
*          back-space beyond the first character of the current line.
*        - Masking out the MSB of each 8-bit character.
*        - Filtering out characters that are invalid AT-command characters.
*        - Identifying and processing repeat-last-AT-command requests.
*        - Identifying and processing embedded AT commands.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlAtParse(const utlAtParser_P parser_p,
			   const unsigned char *octets_p,
			   const size_t n)
{
	utlReturnCode_T parse_rc;

	utlAssert(parser_p != NULL);
	utlAssert(octets_p != NULL);

	parse_rc = utlSUCCESS;

	/*--- if bypass mode is enabled ---*/
	if (parser_p->states.bypass_mode)
	{
		// FIXME
		// BYPASS mode is not supported yet
		utlAssert(0);

		utlAbsoluteTime_T curr_time;

		/*--- fetch current time ---*/
		if (utlCurrentTime(&curr_time) != utlSUCCESS)
			curr_time = parser_p->states.escape.last_tx_time;

		/*--- clear queue ---*/
		parser_p->buffers.echo_p = parser_p->buffers.head_p;
		parser_p->buffers.tail_p = parser_p->buffers.head_p;

		/*--- pass data directly to modem implementation (if possible) ---*/
		if ( parser_p->call_backs.tx_line_data_function_p != NULL)
			parse_rc = (parser_p->call_backs.tx_line_data_function_p)(octets_p, n, parser_p->call_backs.arg_p);

		/*--- update time of last TX ---*/
		parser_p->states.escape.last_tx_time = curr_time;

		/*--- else if we are on-line ---*/
	}
	else
	{
		bool overflow;

		overflow = false;

		/*--- transfer received characters into queue... ---*/
		{
			const unsigned char *o_p;
			const unsigned char *term_o_p;

			term_o_p = octets_p + n;
			for (o_p = octets_p; o_p < term_o_p; o_p++)
			{
				char c;

				c = 0x7f & ((char)*o_p);   /* strip most-significant bit */

				/*--- if we're dealing with a queue overflow, we're going to ignore
				   all characters from the point of overflow until we reach the
				   end of the current line */
				if (parser_p->buffers.overflow)
				{

					/*--- have we reached the end of the current line? ---*/
					if (c == parser_p->parameters.S[utlAT_LINE_TERM_CHAR])
					{

						parser_p->buffers.overflow = false;

						/*--- arrange to report overflow just before we return ---*/
						overflow = true;
					}

					continue;
				}

				/*--- append character to queue of accumulated characters ---*/
				{
					char *next_head_p;

					/*--- compute where to place next character ---*/
					next_head_p =  parser_p->buffers.head_p + 1;
					if (next_head_p >= parser_p->buffers.term_p)
						next_head_p =  parser_p->buffers.queue;

					/*--- if queue is full ---*/
					if (next_head_p == parser_p->buffers.tail_p)
					{
						parser_p->buffers.overflow = true;

						utlAssert(parser_p->buffers.head_p != parser_p->buffers.tail_p);

						/*--- turf from current character back to start of current line ---*/
						{
							char *prev_p;

							if (parser_p->buffers.head_p == parser_p->buffers.queue)
								prev_p = parser_p->buffers.term_p - 1;
							else
								prev_p = parser_p->buffers.head_p - 1;

							for (;; )
							{

								/*--- we reach the end of the previous line? ---*/
								if (*prev_p == parser_p->parameters.S[utlAT_LINE_TERM_CHAR])
								{

									/*--- set head pointer to point just past end of previous line ---*/
									prev_p++;
									if (prev_p >= parser_p->buffers.term_p)
										prev_p = parser_p->buffers.queue;
									parser_p->buffers.head_p = prev_p;

									break;
								}

								/*--- no more data? ---*/
								if (prev_p == parser_p->buffers.tail_p)
								{

									/*--- queue must be empty ---*/
									parser_p->buffers.head_p = prev_p;
									break;
								}

								/*--- backup on character ---*/
								if (prev_p == parser_p->buffers.queue) prev_p = parser_p->buffers.term_p - 1;
								else prev_p--;
							}
						}

						/*--- move on, process next character ---*/
						continue;
					}

					/*--- append character to queue ---*/
					*(parser_p->buffers.head_p) = c;
					parser_p->buffers.head_p  = next_head_p;
				}
			}
		}

		/*--- if we're not waiting for the raw modem to respond to an async request,
		   process queued characters... */
		/*oxoxoxo
		   OLD:
			if (parser_p->states.async_response.timer_id == utlFAILED) {
		   FUTURE:
			if (!utlIsListEmpty(parser_p->states.async_responses.unused)) {
		   TEMP HACK:
		 */
		if (utlAcquireExclusiveAccess(&parser_p->queue_semaphore) != utlSUCCESS)
		{
			utlError("Cannot exclusively acquire semaphore!\n");
			return utlFAILED;
		}
		if (utlIsListEmpty(parser_p->states.async_responses.pending))
		{
			char *c_p;

			utlReleaseExclusiveAccess(&parser_p->queue_semaphore);

			for (c_p = parser_p->buffers.tail_p; c_p != parser_p->buffers.head_p; )
			{

				/*--- if we're currently discarding the current line ---*/
				if (parser_p->states.discard_current_line)
				{

					/*--- discard characters until we hit a line-termination character
					   (or run out of characters) */
					while (c_p != parser_p->buffers.head_p)
					{

						/*--- found line-termination character? ---*/
						if (*c_p == parser_p->parameters.S[utlAT_LINE_TERM_CHAR])
						{

							/*--- skip line-termination character ---*/
							if (++c_p >= parser_p->buffers.term_p)
								c_p = parser_p->buffers.queue;

							parser_p->states.discard_current_line = false;
							break;
						}

						/*--- advance one character ---*/
						if (++c_p >= parser_p->buffers.term_p)
							c_p = parser_p->buffers.queue;
					}

					continue;
				}

				/*--- have we found a potential repeat-last-AT-command (A/) request? ---*/
				if (*c_p == '/')
				{

					/*--- was there a character preceding '/'? ---*/
					if (c_p != parser_p->buffers.tail_p)
					{
						const char *prev_c_p;

						/*--- back-up to character preceding '/' ---*/
						if (c_p == parser_p->buffers.queue) prev_c_p = parser_p->buffers.term_p - 1;
						else prev_c_p = c_p - 1;

						/*--- if we found the sequence "A/" or "a/" ---*/
						if ((*prev_c_p == 'A') ||
						    (*prev_c_p == 'a'))
						{

							/*--- if echoing is enabled, echo everything up to and including the "A/" or "a/" ---*/
							if (parser_p->parameters.echo_text == true)
							{

								if (c_p >= parser_p->buffers.echo_p)
								{
									if (utlSendSubstring(parser_p, parser_p->buffers.echo_p, c_p - parser_p->buffers.echo_p + 1) != utlSUCCESS)
										return utlFAILED;

								}
								else if (c_p < parser_p->buffers.echo_p)
								{
									if (utlSendSubstring(parser_p, parser_p->buffers.echo_p, parser_p->buffers.term_p - parser_p->buffers.echo_p) != utlSUCCESS)
										return utlFAILED;
									if (c_p >= parser_p->buffers.queue)
										if (utlSendSubstring(parser_p, parser_p->buffers.queue, c_p - parser_p->buffers.queue + 1) != utlSUCCESS)
											return utlFAILED;
								}
							}
							parser_p->buffers.echo_p = c_p + 1;

							/*--- advance one character ---*/
							if (++c_p >= parser_p->buffers.term_p)
								c_p = parser_p->buffers.queue;

							/*--- we've now processed up to here ---*/
							parser_p->buffers.tail_p = c_p;


							/*--- re-execute previously executed AT command ---*/
							{
								utlAtResultCode_T rc;

								rc = utlParseLine(parser_p, parser_p->buffers.previous_line, 0);

								utlPrintTrace("[%s][%d]utlParseLine for %s is: %d\n", __FUNCTION__, __LINE__, parser_p->buffers.previous_line, rc);

								if (utlSendBasicSyntaxResultCode(parser_p, rc, true) != utlSUCCESS)
									return utlFAILED;

								if (rc == utlAT_RESULT_CODE_ERROR)
								{

									/*--- discard rest of current line ---*/
									parser_p->states.discard_current_line = true;

									if (utlAcquireExclusiveAccess(&parser_p->cmd_cnt_semaphore) != utlSUCCESS)
									{
										utlError("%s: Cannot exclusively acquire semaphore!\n", __FUNCTION__);
										return utlFAILED;
									}

									parser_p->commands_in_line = 0;

									utlReleaseExclusiveAccess(&parser_p->cmd_cnt_semaphore);

									utlAbandonAllPendingAsyncResponse(parser_p);

									parse_rc = utlFAILED;
								}
							}
						}
					}
				}

				/*--- have we found a command-line editing character?  Note that
				   if S3 (command-line termination) and S5 (command-line edit)
				   are both set to the same character and if this character is
				   encountered, we must treat the found character as an S3
				   (command-line termination) event */
				if ((*c_p == parser_p->parameters.S[utlAT_LINE_EDIT_CHAR]) &&
				    ( parser_p->parameters.S[utlAT_LINE_EDIT_CHAR] != parser_p->parameters.S[utlAT_LINE_TERM_CHAR]))
				{

					/*--- if echoing is enabled, echo everything up to (but excluding) line-edit character ---*/
					if (parser_p->parameters.echo_text == true)
					{

						if (c_p > parser_p->buffers.echo_p)
						{
							if (utlSendSubstring(parser_p, parser_p->buffers.echo_p, c_p - parser_p->buffers.echo_p) != utlSUCCESS)
								return utlFAILED;

						}
						else if (c_p < parser_p->buffers.echo_p)
						{
							if (utlSendSubstring(parser_p, parser_p->buffers.echo_p, parser_p->buffers.term_p - parser_p->buffers.echo_p) != utlSUCCESS)
								return utlFAILED;

							if (c_p > parser_p->buffers.queue)
								if (utlSendSubstring(parser_p, parser_p->buffers.queue, c_p - parser_p->buffers.queue) != utlSUCCESS)
									return utlFAILED;
						}
					}
					parser_p->buffers.echo_p = c_p;

					/*--- fix up queue ---*/
					{
						char *prev_c_p;
						char *prev_tail_p;
						char *src_p;
						char *dest_p;

						/*--- back-up to character preceding line-edit character (if any) ---*/
						if (c_p == parser_p->buffers.queue)
							prev_c_p = parser_p->buffers.term_p - 1;
						else
							prev_c_p = c_p - 1;

						/*--- identify queue-slot preceeding tail ---*/
						if (parser_p->buffers.tail_p == parser_p->buffers.queue)
							prev_tail_p = parser_p->buffers.term_p - 1;
						else
							prev_tail_p = parser_p->buffers.tail_p - 1;

						/*--- if there's a character to back-space over ---*/
						if (( prev_c_p != prev_tail_p) &&
						    (*prev_c_p != parser_p->parameters.S[utlAT_LINE_TERM_CHAR]))
						{

							/*--- if echoing is enabled and we have something to back-space over, echo a destructive <bs> ---*/
							if ((parser_p->parameters.echo_text == true) && (isprint(*prev_c_p)))
								if (utlSendString(parser_p, "\b \b") != utlSUCCESS)
									return utlFAILED;

							/*--- arrange to remove line-edit character and character
							   preceeding line-edit character from queue */
							src_p = c_p;
							dest_p = prev_c_p;

							c_p = prev_c_p;

						}
						else
						{
							/*--- arrange to remove line-edit character from queue ---*/
							src_p = c_p;
							dest_p = c_p;
						}

						for (;; )
						{
							if (++src_p >= parser_p->buffers.term_p)
								src_p =  parser_p->buffers.queue;

							if (src_p == parser_p->buffers.head_p)
								break;

							*dest_p = *src_p;

							if (++dest_p >= parser_p->buffers.term_p)
								dest_p =  parser_p->buffers.queue;
						}
						parser_p->buffers.head_p = dest_p;
						parser_p->buffers.echo_p = c_p;

						continue;
					}

					/*--- have we found a command-line termination character? ---*/
				}
				else if (*c_p == parser_p->parameters.S[utlAT_LINE_TERM_CHAR])
				{
					char line[utlAT_MAX_LINE_LENGTH + 1];
					size_t line_off;
					memset(line, 0x0, sizeof(line));
					line_off                  = parser_p->states.line_off;
					parser_p->states.line_off = 0;

					/*--- if echoing is enabled, echo everything up to and including the termination character ---*/
					if (parser_p->parameters.echo_text == true)
					{
						if (c_p >= parser_p->buffers.echo_p)
						{
							if (utlSendSubstring(parser_p, parser_p->buffers.echo_p, c_p - parser_p->buffers.echo_p + 1) != utlSUCCESS)
								return utlFAILED;

						}
						else if (c_p < parser_p->buffers.echo_p)
						{
							if (utlSendSubstring(parser_p, parser_p->buffers.echo_p, parser_p->buffers.term_p - parser_p->buffers.echo_p) != utlSUCCESS)
								return utlFAILED;
							if (c_p >= parser_p->buffers.queue)
								if (utlSendSubstring(parser_p, parser_p->buffers.queue, c_p - parser_p->buffers.queue + 1) != utlSUCCESS)
									return utlFAILED;
						}
					}
					parser_p->buffers.echo_p = c_p + 1;

					/*--- transfer one line of text from queue to line buffer... ---*/
					{
						char *this_p;
						char *term_p;
						char *src_p;

						this_p = line;
						term_p = line + utlNumberOf(line);

						for (src_p = parser_p->buffers.tail_p; src_p != c_p; )
						{

							/*--- no more room in line buffer? ---*/
							if (this_p >= term_p)
							{
								utlError("Line too long.");

								return utlFAILED;
							}

							/*--- only process characters that are valid in AT-commands... ---*/
							if ((*src_p >= '\040') ||
							    (*src_p == parser_p->parameters.S[utlAT_ESCAPE_CHAR])     ||
							    (*src_p == parser_p->parameters.S[utlAT_LINE_TERM_CHAR])  ||
							    (*src_p == parser_p->parameters.S[utlAT_FORMATTING_CHAR]) ||
							    (*src_p == parser_p->parameters.S[utlAT_LINE_EDIT_CHAR])  ||
							    (*src_p == parser_p->parameters.S[utlAT_SEPARATER_CHAR])  ||
							    (*src_p == parser_p->parameters.S[utlAT_TERMINATER_CHAR]))
							{

								/*--- append character to line buffer ---*/
								*this_p++ = *src_p;
							}

							if (++src_p >= parser_p->buffers.term_p)
								src_p =  parser_p->buffers.queue;
						}
						*this_p = '\0';
					}

					parser_p->buffers.tail_p = c_p;
					if (++(parser_p->buffers.tail_p) >= parser_p->buffers.term_p)
						parser_p->buffers.tail_p  =  parser_p->buffers.queue;

					/*--- parse line (if not empty) for AT commands ---*/
					if (line[0] != '\0')
					{
						utlAtResultCode_T rc;

						rc = utlParseLine(parser_p, line, line_off);

						// comment this line as for some illegal string input might result to printf parsing failure.
						//utlPrintTrace("[%s][%d]utlParseLine for %s is: %d\n", __FUNCTION__, __LINE__, line, rc);

						if (utlSendBasicSyntaxResultCode(parser_p, rc, true) != utlSUCCESS)
							return utlFAILED;

						if (rc == utlAT_RESULT_CODE_ERROR)
						{
							if (utlAcquireExclusiveAccess(&parser_p->cmd_cnt_semaphore) != utlSUCCESS)
							{
								utlError("%s: Cannot exclusively acquire semaphore!\n", __FUNCTION__);
								return utlFAILED;
							}

							parser_p->commands_in_line = 0;

							utlReleaseExclusiveAccess(&parser_p->cmd_cnt_semaphore);

							utlAbandonAllPendingAsyncResponse(parser_p);
							parse_rc = utlFAILED;

						}
						else
						{
							/*--- save good AT-command to support repeat-last-AT-command feature ---*/
							strncpy(parser_p->buffers.previous_line, line, sizeof(parser_p->buffers.previous_line) - 1);
						}
					}
				}

				/*--- advance to next character ---*/
				if (++c_p >= parser_p->buffers.term_p)
					c_p =  parser_p->buffers.queue;

				/*--- if we're waiting for an asynchronous request to complete ---*/
				/*oxoxoxo
				   OLD:
					if (parser_p->states.async_response.timer_id != utlFAILED)
				   FUTURE:
					if (utlIsListEmpty(parser_p->states.async_responses.unused))
				   TEMP HACK:
				 */
				if (!utlIsListEmpty(parser_p->states.async_responses.pending))
					break;
			}

			/*--- if echoing is enabled, echo what has not yet been echoed ---*/
			if (parser_p->parameters.echo_text == true)
			{

				if (c_p > parser_p->buffers.echo_p)
				{
					if (utlSendSubstring(parser_p, parser_p->buffers.echo_p, c_p - parser_p->buffers.echo_p) != utlSUCCESS)
						return utlFAILED;
				}
				else if (c_p < parser_p->buffers.echo_p)
				{
					if (utlSendSubstring(parser_p, parser_p->buffers.echo_p, parser_p->buffers.term_p - parser_p->buffers.echo_p) != utlSUCCESS)
						return utlFAILED;
					if (c_p > parser_p->buffers.queue)
						if (utlSendSubstring(parser_p, parser_p->buffers.queue, c_p - parser_p->buffers.queue) != utlSUCCESS)
							return utlFAILED;
				}
			}
			parser_p->buffers.echo_p = c_p;
		}
		else
		{
			utlReleaseExclusiveAccess(&parser_p->queue_semaphore);
		}

		if (overflow)
		{
			utlError("buffer overflow");
			return utlFAILED;
		}
	}

	return parse_rc;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// DEBUG FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(utlDEBUG) || defined(utlTEST)
/*---------------------------------------------------------------------------*
* FUNCTION
*      utlVStringDumpAtParser(v_string_p, parser_p, prefix_p)
* INPUT
*      v_string_p == pointer to an initialized utlVString_T structure
*        parser_p == pointer to an initialized utlAtParser_T structure
*        prefix_p == pointer to a prefix string
* OUTPUT
*      *v_string_p == the updated utlVString_T structure
* RETURNS
*      nothing
* DESCRIPTION
*      Dumps AT-command parser information to `v_string_p'.
*---------------------------------------------------------------------------*/
void utlVStringDumpAtParser(const utlVString_P v_string_p,
			    const utlAtParser_P parser_p,
			    const char          *prefix_p)
{
	char buf[40];
	size_t i, j, k;

	utlAssert(v_string_p != NULL);
	utlAssert(parser_p   != NULL);
	utlAssert(prefix_p   != NULL);

	(void)utlVStringPrintF(v_string_p, "%s:\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s: AT-command Parser\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s:\n", prefix_p);

	if (parser_p->info.id_p            != NULL) (void)utlVStringPrintF(v_string_p, "%s:                               ID: %s\n", prefix_p, parser_p->info.id_p);
	if (parser_p->info.manufacturer_p  != NULL) (void)utlVStringPrintF(v_string_p, "%s:                     Manufacturer: %s\n", prefix_p, parser_p->info.manufacturer_p);
	if (parser_p->info.model_p         != NULL) (void)utlVStringPrintF(v_string_p, "%s:                            Model: %s\n", prefix_p, parser_p->info.model_p);
	if (parser_p->info.revision_p      != NULL) (void)utlVStringPrintF(v_string_p, "%s:                         Revision: %s\n", prefix_p, parser_p->info.revision_p);
	if (parser_p->info.serial_number_p != NULL) (void)utlVStringPrintF(v_string_p, "%s:                    Serial Number: %s\n", prefix_p, parser_p->info.serial_number_p);
	if (parser_p->info.object_id_p     != NULL) (void)utlVStringPrintF(v_string_p, "%s:                 Global Object ID: %s\n", prefix_p, parser_p->info.object_id_p);
	if (parser_p->info.country_code_p  != NULL) (void)utlVStringPrintF(v_string_p, "%s:                     Country Code: %s\n", prefix_p, parser_p->info.country_code_p);
	if (parser_p->info.connect_text_p  != NULL) (void)utlVStringPrintF(v_string_p, "%s:                     Connect Text: %s\n", prefix_p, parser_p->info.connect_text_p);

	(void)utlVStringPrintF(v_string_p, "%s: \n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s: S-Parameters:\n", prefix_p);
	k = (utlAT_NUM_S_PARAMETERS + 3) / 4;
	for (i = 0; i < k; i++)
	{
		(void)utlVStringPrintF(v_string_p, "%s:   ", prefix_p);
		for (j = 0; j < 4; j++)
		{
			size_t l;

			l = i + (j * k);
			if (l < utlAT_NUM_S_PARAMETERS)
				(void)utlVStringPrintF(v_string_p, "   S[%02u]: %-5u", l, parser_p->parameters.S[l]);
		}
		(void)utlVStringPrintF(v_string_p, "\n");
	}

	(void)utlVStringPrintF(v_string_p, "%s: \n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s: Allow default S-parameter values: %s\n", prefix_p, (parser_p->options.allow_default_S_parameter_values == true) ? "yes" : "no");
	(void)utlVStringPrintF(v_string_p, "%s:             Allow string escapes: %s\n", prefix_p, (parser_p->options.allow_string_escapes             == true) ? "yes" : "no");
	(void)utlVStringPrintF(v_string_p, "%s:                      Result code: %s\n", prefix_p, (parser_p->options.use_carrier_result_code          == true) ? "CARRIER" : "CONNECT");

	(void)utlVStringPrintF(v_string_p, "%s: \n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s: Previous Line: \"%s\"\n", prefix_p, parser_p->buffers.previous_line);

	(void)utlVStringPrintF(v_string_p, "%s: \n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s: Recognized AT Commands:\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s:        Name       |   Type    |               Parameters\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s:                   |           |    Type     |   Access   | Presence\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s:  -----------------+-----------+-------------+------------+-----------\n", prefix_p);
	for (i = 0; i < parser_p->num_commands; i++)
	{
		size_t j;

		if (i > (size_t)0)
			(void)utlVStringPrintF(v_string_p, "%s:                   |           |             |            |           \n", prefix_p);

		(void)utlVStringPrintF(v_string_p, "%s: %17s | ", prefix_p, parser_p->commands_p[i].name_p);

		switch (parser_p->commands_p[i].type)
		{
		case utlAT_COMMAND_TYPE_BASIC:    (void)utlVStringPrintF(v_string_p, "Basic     | ");  break;
		case utlAT_COMMAND_TYPE_EXTENDED: (void)utlVStringPrintF(v_string_p, "Extended  | ");  break;
		case utlAT_COMMAND_TYPE_EXACTION: (void)utlVStringPrintF(v_string_p, "Ex-action | ");  break;
		default:
			(void)utlVStringPrintF(v_string_p, "%8d | ", parser_p->commands_p[i].type);
			break;
		}

		if (parser_p->commands_p[i].num_parameters == (size_t)0)
			(void)utlVStringPrintF(v_string_p, "     -      |      -     |     -\n");

		else for (j = 0; j < parser_p->commands_p[i].num_parameters; j++)
			{
				if (j > (size_t)0)
					(void)utlVStringPrintF(v_string_p, "%s:                   |           | ", prefix_p);

				switch (parser_p->commands_p[i].parameters_p[j].type)
				{
				case utlAT_DATA_TYPE_DECIMAL:     (void)utlVStringPrintF(v_string_p, "  Decimal   | ");  break;
				case utlAT_DATA_TYPE_HEXADECIMAL: (void)utlVStringPrintF(v_string_p, "Hexadecimal | ");  break;
				case utlAT_DATA_TYPE_BINARY:      (void)utlVStringPrintF(v_string_p, "  Binary    | ");  break;
				case utlAT_DATA_TYPE_STRING:      (void)utlVStringPrintF(v_string_p, "  String    | ");  break;
				case utlAT_DATA_TYPE_QSTRING:     (void)utlVStringPrintF(v_string_p, "  QString   | ");  break;
				case utlAT_DATA_TYPE_DIAL_STRING: (void)utlVStringPrintF(v_string_p, "Dial-string | ");  break;
				default:
					(void)utlVStringPrintF(v_string_p, "%10d | ", parser_p->commands_p[i].parameters_p[j].type);
					break;
				}

				switch (parser_p->commands_p[i].parameters_p[j].access)
				{
				case utlAT_PARAMETER_ACCESS_READ_WRITE: (void)utlVStringPrintF(v_string_p, "Read/Write | ");  break;
				case utlAT_PARAMETER_ACCESS_READ:       (void)utlVStringPrintF(v_string_p, "  Read     | ");  break;
				case utlAT_PARAMETER_ACCESS_READ_ONLY:  (void)utlVStringPrintF(v_string_p, "Read-only  | ");  break;
				case utlAT_PARAMETER_ACCESS_WRITE_ONLY: (void)utlVStringPrintF(v_string_p, "Write-only | ");  break;
				default:
					(void)utlVStringPrintF(v_string_p, "%10d | ", parser_p->commands_p[i].parameters_p[j].access);
					break;
				}

				switch (parser_p->commands_p[i].parameters_p[j].presence)
				{
				case utlAT_PARAMETER_PRESENCE_OPTIONAL: (void)utlVStringPrintF(v_string_p, "Optional\n");  break;
				case utlAT_PARAMETER_PRESENCE_REQUIRED: (void)utlVStringPrintF(v_string_p, "Required\n");  break;
				default:
					(void)utlVStringPrintF(v_string_p, "%d\n", parser_p->commands_p[i].parameters_p[j].presence);
					break;
				}
			}
	}

	(void)utlVStringPrintF(v_string_p, "%s: \n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s:    Current dial string: %s\n", prefix_p,  parser_p->states.dial_string.buf);
	(void)utlVStringPrintF(v_string_p, "%s:       Last dial string: %s\n", prefix_p,  parser_p->states.dial_string.last);
	(void)utlVStringPrintF(v_string_p, "%s:      Dial string delay: %d\n", prefix_p,  parser_p->states.dial_string.dial_string_delay);
	(void)utlVStringPrintF(v_string_p, "%s:          International: %s\n", prefix_p, (parser_p->states.dial_string.options.international       == true) ? "true" : "false");
	(void)utlVStringPrintF(v_string_p, "%s:    Do call origination: %s\n", prefix_p, (parser_p->states.dial_string.options.do_call_origination == true) ? "true" : "false");
	(void)utlVStringPrintF(v_string_p, "%s:       Use CCUG SS info: %s\n", prefix_p, (parser_p->states.dial_string.options.use_CCUG_SS_info    == true) ? "true" : "false");
	(void)utlVStringPrintF(v_string_p, "%s:       CLI presentation: %s\n", prefix_p, (parser_p->states.dial_string.options.CLI_presentation    == 'I') ? "restrict" :
			       (parser_p->states.dial_string.options.CLI_presentation    == 'i') ? "allow" : "default");

	(void)utlVStringPrintF(v_string_p, "%s: \n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s:             Go on-line: %s\n", prefix_p, (parser_p->states.go_on_line           == true) ? "true" : "false");
	(void)utlVStringPrintF(v_string_p, "%s:   Discard Current Line: %s\n", prefix_p, (parser_p->states.discard_current_line == true) ? "true" : "false");
	(void)utlVStringPrintF(v_string_p, "%s: DCE I/O config pending: %x\n", prefix_p,  parser_p->states.dce_io_config_pending_mask);
	(void)utlVStringPrintF(v_string_p, "%s:   Sound config pending: %x\n", prefix_p,  parser_p->states.sound_config_pending_mask);
	(void)utlVStringPrintF(v_string_p, "%s:            Line Offset: %d\n", prefix_p,  parser_p->states.line_off);

	(void)utlVStringPrintF(v_string_p, "%s: \n", prefix_p);
	if (utlFormatDateTime(buf, utlNumberOf(buf), &(parser_p->states.escape.last_tx_time)) == utlSUCCESS)
		(void)utlVStringPrintF(v_string_p, "%s:           Last TX time: %s\n", prefix_p, buf);
	(void)utlVStringPrintF(v_string_p, "%s:           Escape state: %u\n", prefix_p, parser_p->states.escape.state);

	(void)utlVStringPrintF(v_string_p, "%s:  Blind dial pause time: %u.%09u\n", prefix_p, parser_p->states.blind_dial_pause_time.seconds,
			       parser_p->states.blind_dial_pause_time.nanoseconds);
	(void)utlVStringPrintF(v_string_p, "%s: Conn. complete timeout: %u.%09u\n", prefix_p, parser_p->states.conn_complete_timeout.seconds,
			       parser_p->states.conn_complete_timeout.nanoseconds);
	(void)utlVStringPrintF(v_string_p, "%s:     Dialing pause time: %u.%09u\n", prefix_p, parser_p->states.dialing_pause_time.seconds,
			       parser_p->states.dialing_pause_time.nanoseconds);
	(void)utlVStringPrintF(v_string_p, "%s:    Carrier detect time: %u.%09u\n", prefix_p, parser_p->states.carrier_detect_time.seconds,
			       parser_p->states.carrier_detect_time.nanoseconds);
	(void)utlVStringPrintF(v_string_p, "%s:      Carrier loss time: %u.%09u\n", prefix_p, parser_p->states.carrier_loss_time.seconds,
			       parser_p->states.carrier_loss_time.nanoseconds);
	(void)utlVStringPrintF(v_string_p, "%s:        Hook flash time: %u.%09u\n", prefix_p, parser_p->states.hook_flash_time.seconds,
			       parser_p->states.hook_flash_time.nanoseconds);
	(void)utlVStringPrintF(v_string_p, "%s:   Disconnect wait time: %u.%09u\n", prefix_p, parser_p->states.inactivity_time.seconds,
			       parser_p->states.inactivity_time.nanoseconds);
	(void)utlVStringPrintF(v_string_p, "%s:   Disconnect wait time: %u.%09u\n", prefix_p, parser_p->states.disconnect_wait_time.seconds,
			       parser_p->states.disconnect_wait_time.nanoseconds);
	(void)utlVStringPrintF(v_string_p, "%s:     Delay dialing time: %u.%09u\n", prefix_p, parser_p->states.delay_dialing_time.seconds,
			       parser_p->states.delay_dialing_time.nanoseconds);
	(void)utlVStringPrintF(v_string_p, "%s:   Minimum Silence time: %u.%09u\n", prefix_p, parser_p->states.min_silence_time.seconds,
			       parser_p->states.min_silence_time.nanoseconds);

	(void)utlVStringPrintF(v_string_p, "%s: \n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s:     Basic-commands peg count: %d\n", prefix_p, parser_p->peg_counts.basic_commands);
	(void)utlVStringPrintF(v_string_p, "%s:  Extended-commands peg count: %d\n", prefix_p, parser_p->peg_counts.extended_commands);
	(void)utlVStringPrintF(v_string_p, "%s: Undefined-commands peg count: %d\n", prefix_p, parser_p->peg_counts.undefined_commands);
	(void)utlVStringPrintF(v_string_p, "%s:       Bad-commands peg count: %d\n", prefix_p, parser_p->peg_counts.bad_commands);

}
/*---------------------------------------------------------------------------*
* FUNCTION
*      utlDumpAtParser(file_p, parser_p)
* INPUT
*        file_p == pointer to an open file
*      parser_p == pointer to an initialized utlParser_T structure
* OUTPUT
*      none
* RETURNS
*      nothing
* DESCRIPTION
*      Dumps AT-command parser information to `file_p'.
*---------------------------------------------------------------------------*/
void utlDumpAtParser(      FILE          *file_p,
			   const utlAtParser_P parser_p)
{
	utlVString_T v_string;

	utlAssert(file_p != NULL);

	utlInitVString(&v_string);

	utlVStringDumpAtParser(&v_string, parser_p, "AtParser");

	utlVStringPuts(&v_string, file_p);

	utlVStringFree(&v_string);
}
#endif

