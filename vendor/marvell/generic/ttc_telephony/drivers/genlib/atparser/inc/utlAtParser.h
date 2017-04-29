/*****************************************************************************
* Utility Library
*
*  AT-command parser utilities header file.
*
*****************************************************************************/

#ifndef _UTL_AT_PARSER_INCLUDED
#define _UTL_AT_PARSER_INCLUDED

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <utlTypes.h>

#include <utlLinkedList.h>
#include <utlStateMachine.h>
#include <utlTimer.h>
#include <utlVString.h>

/*--- state machine state IDs ---*/
#define utlAT_STATE_ID_ANSWER                 utlDEFINE_STATE_ID( 1)
#define utlAT_STATE_ID_ANSWER_WAIT            utlDEFINE_STATE_ID( 2)
#define utlAT_STATE_ID_CARRIER_CONFIRM        utlDEFINE_STATE_ID( 3)
#define utlAT_STATE_ID_CARRIER_LOST           utlDEFINE_STATE_ID( 4)
#define utlAT_STATE_ID_CARRIER_WAIT           utlDEFINE_STATE_ID( 5)
#define utlAT_STATE_ID_DELAY_DIALING          utlDEFINE_STATE_ID( 6)
#define utlAT_STATE_ID_DIALING                utlDEFINE_STATE_ID( 7)
#define utlAT_STATE_ID_DIALING_CREDIT_WAIT    utlDEFINE_STATE_ID( 8)
#define utlAT_STATE_ID_DIALING_DIAL_TONE_WAIT utlDEFINE_STATE_ID( 9)
#define utlAT_STATE_ID_DIALING_FLASH_OFF_WAIT utlDEFINE_STATE_ID(10)
#define utlAT_STATE_ID_DIALING_FLASH_ON_WAIT  utlDEFINE_STATE_ID(11)
#define utlAT_STATE_ID_DIALING_HOOK_FLASH     utlDEFINE_STATE_ID(12)
#define utlAT_STATE_ID_DIALING_PAUSED         utlDEFINE_STATE_ID(13)
#define utlAT_STATE_ID_DIALING_RING_BACK_WAIT utlDEFINE_STATE_ID(14)
#define utlAT_STATE_ID_DIALING_SILENCE_WAIT   utlDEFINE_STATE_ID(15)
#define utlAT_STATE_ID_DIAL_TONE_WAIT         utlDEFINE_STATE_ID(16)
#define utlAT_STATE_ID_DISCONNECT_WAIT        utlDEFINE_STATE_ID(17)
#define utlAT_STATE_ID_OFF_HOOK_ANSWER_WAIT   utlDEFINE_STATE_ID(18)
#define utlAT_STATE_ID_OFF_HOOK_WAIT          utlDEFINE_STATE_ID(19)
#define utlAT_STATE_ID_OFF_LINE               utlDEFINE_STATE_ID(20)
#define utlAT_STATE_ID_ON_HOOK                utlDEFINE_STATE_ID(21)
#define utlAT_STATE_ID_ON_LINE                utlDEFINE_STATE_ID(22)
#define utlAT_STATE_ID_ON_HOOK_RESET          utlDEFINE_STATE_ID(23)
#define utlAT_STATE_ID_ON_HOOK_WAIT           utlDEFINE_STATE_ID(24)
#define utlAT_STATE_ID_PREDIAL_PAUSE          utlDEFINE_STATE_ID(25)
#define utlAT_STATE_ID_PULSE_DIGITS_WAIT      utlDEFINE_STATE_ID(26)
#define utlAT_STATE_ID_RESET_WAIT             utlDEFINE_STATE_ID(27)
#define utlAT_STATE_ID_RINGING                utlDEFINE_STATE_ID(28)
#define utlAT_STATE_ID_RING_BACK_WAIT         utlDEFINE_STATE_ID(29)
#define utlAT_STATE_ID_TONE_DIGITS_WAIT       utlDEFINE_STATE_ID(30)

/*--- state machine event IDs ---*/
#define utlAT_STATE_EVENT_ID_ANSWER_CONF        utlDEFINE_STATE_EVENT_ID( 1)
#define utlAT_STATE_EVENT_ID_ANSWERED           utlDEFINE_STATE_EVENT_ID( 2)
#define utlAT_STATE_EVENT_ID_ATA                utlDEFINE_STATE_EVENT_ID( 3)
#define utlAT_STATE_EVENT_ID_ATD                utlDEFINE_STATE_EVENT_ID( 4)
#define utlAT_STATE_EVENT_ID_ATO                utlDEFINE_STATE_EVENT_ID( 5)
#define utlAT_STATE_EVENT_ID_BUSY               utlDEFINE_STATE_EVENT_ID( 6)
#define utlAT_STATE_EVENT_ID_CARRIER            utlDEFINE_STATE_EVENT_ID( 7)
#define utlAT_STATE_EVENT_ID_CREDIT_BONG        utlDEFINE_STATE_EVENT_ID( 8)
#define utlAT_STATE_EVENT_ID_DELAY_DIALING      utlDEFINE_STATE_EVENT_ID( 9)
#define utlAT_STATE_EVENT_ID_DIAL_DIGITS        utlDEFINE_STATE_EVENT_ID(10)
#define utlAT_STATE_EVENT_ID_DIAL_TONE          utlDEFINE_STATE_EVENT_ID(11)
#define utlAT_STATE_EVENT_ID_DIGITS_DIALED      utlDEFINE_STATE_EVENT_ID(12)
#define utlAT_STATE_EVENT_ID_END_OF_DIALING     utlDEFINE_STATE_EVENT_ID(13)
#define utlAT_STATE_EVENT_ID_ESCAPE             utlDEFINE_STATE_EVENT_ID(14)  /* means +++ while on-line */
#define utlAT_STATE_EVENT_ID_HANG_UP            utlDEFINE_STATE_EVENT_ID(15)
#define utlAT_STATE_EVENT_ID_HOOK_FLASH         utlDEFINE_STATE_EVENT_ID(16)
#define utlAT_STATE_EVENT_ID_LINE_SOUND         utlDEFINE_STATE_EVENT_ID(17)
#define utlAT_STATE_EVENT_ID_NO_ANSWER          utlDEFINE_STATE_EVENT_ID(18)
#define utlAT_STATE_EVENT_ID_NO_CARRIER         utlDEFINE_STATE_EVENT_ID(19)
#define utlAT_STATE_EVENT_ID_NO_DIAL_TONE       utlDEFINE_STATE_EVENT_ID(20)
#define utlAT_STATE_EVENT_ID_ON_HOOK_CONF       utlDEFINE_STATE_EVENT_ID(21)
#define utlAT_STATE_EVENT_ID_OFF_HOOK_CONF      utlDEFINE_STATE_EVENT_ID(22)
#define utlAT_STATE_EVENT_ID_PAUSE_DIALING      utlDEFINE_STATE_EVENT_ID(23)
#define utlAT_STATE_EVENT_ID_REDIAL             utlDEFINE_STATE_EVENT_ID(24)
#define utlAT_STATE_EVENT_ID_REQUEST_FAILED     utlDEFINE_STATE_EVENT_ID(25)
#define utlAT_STATE_EVENT_ID_RING               utlDEFINE_STATE_EVENT_ID(26)
#define utlAT_STATE_EVENT_ID_WAIT_FOR_CREDIT    utlDEFINE_STATE_EVENT_ID(27)
#define utlAT_STATE_EVENT_ID_WAIT_FOR_DIAL_TONE utlDEFINE_STATE_EVENT_ID(28)
#define utlAT_STATE_EVENT_ID_WAIT_FOR_RING_BACK utlDEFINE_STATE_EVENT_ID(29)


/*--- Configuration ---------------------------------------------------------*/

/*--- maximum length of an AT-command line (in characters) ---*/
#define utlAT_MAX_LINE_LENGTH ((size_t)1024)

/*--- maximum length of a dial-string (ATD command parameter) (in characters) ---*/
#define utlAT_MAX_DIAL_STRING_LENGTH ((size_t)96)

/*--- total number of s-parameters (Note: S-parameter numbering starts from 0) ---*/
#define utlAT_NUM_S_PARAMETERS ((size_t)50)

/*--- maximum number of simultanious async AT-commands pending ---*/
#define utlAT_MAX_ASYNC_RESPONSES_PENDING ((size_t)8)

/*--- maximum possible number of stored dial strings ---*/
#define utlAT_MAX_STORED_DIAL_STRINGS 32

/*--- maximum possible location name length (in characters) ---*/
#define utlAT_MAX_STORED_LOCATION_NAME_LENGTH 20

/*--- maximum possible stored-dial-string length (in characters) ---*/
#define utlAT_MAX_STORED_DIAL_STRING_LENGTH 64

/*--- how long to wait for an asynchronous reply (in seconds) ---*/
#ifdef utlTEST
#    define utlAT_AWAIT_ASYNC_TIMEOUT_TIME ((unsigned int)4)
#else
#    define utlAT_AWAIT_ASYNC_TIMEOUT_TIME ((unsigned int)8)
#endif

/*--- maximum length of AT command (in characters) ---*/
#define utlAT_MAX_COMMAND_LENGTH ((size_t)16)

/*--- length of AT command prefix (in characters) ---*/
#define utlAT_COMMAND_PREFIX_LENGTH ((size_t)1)

/*--- maximum length of AT command trailing (in characters) ---*/
#define utlAT_MAX_COMMAND_TRAILING_LENGTH ((size_t)2)

/*--- maximum length of AT command with prefix no trailing + '\0' (in characters) ---*/
#define utlAT_MAX_COMMAND_LENGTH_WITH_PREFIX (utlAT_COMMAND_PREFIX_LENGTH + utlAT_MAX_COMMAND_LENGTH + 1)

/*--- maximum length of AT command with prefix and trailing + '\0' (in characters) ---*/
#define utlAT_MAX_COMMAND_LENGTH_WITH_PREFIX_TRAILING (utlAT_MAX_COMMAND_LENGTH_WITH_PREFIX + utlAT_MAX_COMMAND_TRAILING_LENGTH)

/*--- Data Types ------------------------------------------------------------*/

typedef struct utlAtParser_S *utlAtParser_P;  /* forward declaration */

typedef enum {
	utlAT_DATA_TYPE_DECIMAL,
	utlAT_DATA_TYPE_HEXADECIMAL,
	utlAT_DATA_TYPE_BINARY,
	utlAT_DATA_TYPE_STRING,         /* string double-quotes are optional */
	utlAT_DATA_TYPE_QSTRING,        /* string double-quotes are required */
	utlAT_DATA_TYPE_DIAL_STRING,
} utlAtDataType_T;

typedef union utlAtDataValue_U {
	unsigned int decimal;
	unsigned int hexadecimal;
	unsigned int binary;
	char         *string_p;
	char         *qstring_p;
	char         *dial_string_p;
} utlAtDataValue_T, *utlAtDataValue_P;

typedef enum {
	utlAT_PARAMETER_ACCESS_READ_WRITE,      /* reads and writes  are permitted    */
	utlAT_PARAMETER_ACCESS_READ,            /* attempts to write are ignored      */
	utlAT_PARAMETER_ACCESS_READ_ONLY,       /* attempts to write generates errors */
	utlAT_PARAMETER_ACCESS_WRITE_ONLY       /* attempts to read  generates errors */
} utlAtParameterAccess_T;

typedef enum {
	utlAT_PARAMETER_PRESENCE_OPTIONAL,      /* parameter presence is optional */
	utlAT_PARAMETER_PRESENCE_REQUIRED,      /* parameter presence is required */
} utlAtParameterPresence_T;

typedef struct utlAtParameterValue_S {
	utlAtDataType_T type;           /* data type of `value' */
	utlAtParameterAccess_T access;
	bool is_default;                /* is `value' set to the default value? */
	utlAtDataValue_T value;         /* parameter's value                    */
}             utlAtParameterValue_T, *utlAtParameterValue_P;
typedef const utlAtParameterValue_T  *utlAtParameterValue_P2c;

typedef struct utlAtParameter_S {
	utlAtDataType_T type;       /* parameter's data type */
	utlAtParameterAccess_T access;
	utlAtParameterPresence_T presence;
}             utlAtParameter_T, *utlAtParameter_P;
typedef const utlAtParameter_T  *utlAtParameter_P2c;

/*--- convenience macros ---*/
#define utlDEFINE_BASIC_AT_PARAMETER(type)                    { type, utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL }
#define utlDEFINE_DECIMAL_AT_PARAMETER(     access, presence) { utlAT_DATA_TYPE_DECIMAL, access, presence }
#define utlDEFINE_HEXADECIMALL_AT_PARAMETER(access, presence) { utlAT_DATA_TYPE_HEXADECIMAL, access, presence }
#define utlDEFINE_BINARY_AT_PARAMETER(      access, presence) { utlAT_DATA_TYPE_BINARY, access, presence }
#define utlDEFINE_STRING_AT_PARAMETER(      access, presence) { utlAT_DATA_TYPE_STRING, access, presence }
#define utlDEFINE_QSTRING_AT_PARAMETER(     access, presence) { utlAT_DATA_TYPE_QSTRING, access, presence }
#define utlDEFINE_DIAL_STRING_AT_PARAMETER( access, presence) { utlAT_DATA_TYPE_DIAL_STRING, access, presence }

typedef struct utlAtDceIoConfig_S {
	unsigned int data_rate;                 /* 0 == auto-detect data rate                */
	unsigned char data_bits;                /* slaved from utAtParser.parameters.framing */
	unsigned char parity_bits;              /* slaved from utAtParser.parameters.framing */
	unsigned char stop_bits;                /* slaved from utAtParser.parameters.framing */
	utlDataParity_T parity;                 /* slaved from utAtParser.parameters.parity  */
	struct {
		unsigned char dce_by_dte;       /* 0 == none, 1 == DC1/DC2, 2 == RTS, 3 == both RTS and DC1/DC3 */
		unsigned char dte_by_dce;       /* 0 == none, 1 == DC1/DC2, 2 == CTS                            */
		char xon_char;
		char xoff_char;
	} flow_control;
	struct {
		bool dcd_always_on;
		bool ignore_dtr;
		bool drop_call_on_dtr_loss;
		unsigned int dsr_mode; /* 0 == always on, 1 == as per v.24, 2 == always on execpt for 5 secs after disconnect */
	} modes;
	utlRelativeTime_T dtr_delay_time;
}             utlAtDceIoConfig_T, *utlAtDceIoConfig_P;
typedef const utlAtDceIoConfig_T  *utlAtDceIoConfig_P2c;

typedef struct utlAtSoundConfig_S {
	struct {
		unsigned int mode;      /* 0 == off, 1 == on til CD, 2 = always on */
		unsigned int level;     /* 0 (low) to 3 (high)                     */
	} monitor_speaker;
}             utlAtSoundConfig_T, *utlAtSoundConfig_P;
typedef const utlAtSoundConfig_T  *utlAtSoundConfig_P2c;

/*--- parser operations ---*/
typedef enum {
	utlAT_PARSER_OP_HANG_UP,
	utlAT_PARSER_OP_RESET,
	utlAT_PARSER_OP_SET_DCE_IO_CONFIG_HANDLER,
	utlAT_PARSER_OP_SET_SOUND_CONFIG_HANDLER,
	utlAT_PARSER_OP_SET_S_PARAMETER_HANDLER,
	utlAT_PARSER_OP_SET_SAVE_DIAL_STRING_HANDLER,
	utlAT_PARSER_OP_SET_RETRIEVE_DIAL_STRING_HANDLER,
	utlAT_PARSER_OP_SET_REPLY_HANDLER,
	utlAT_PARSER_OP_SET_TX_LINE_DATA_HANDLER,
	utlAT_PARSER_OP_SET_DRIVER_REQUEST_HANDLER,
	utlAT_PARSER_OP_ECHO_ON,
	utlAT_PARSER_OP_ECHO_OFF,
	utlAT_PARSER_OP_SELECT_CARRIER_RESULT_CODE,
	utlAT_PARSER_OP_SELECT_CONNECT_RESULT_CODE,
	utlAT_PARSER_OP_SET_S_PARAMETER_VALUE,
	utlAT_PARSER_OP_GET_S_PARAMETER_VALUE,
	utlAT_PARSER_OP_REPORT_EACH_RING,
	utlAT_PARSER_OP_ONLY_REPORT_FIRST_RING,
	utlAT_PARSER_OP_SET_ID,
	utlAT_PARSER_OP_SET_MANUFACTURER,
	utlAT_PARSER_OP_SET_MODEL,
	utlAT_PARSER_OP_SET_REVISION,
	utlAT_PARSER_OP_SET_SERIAL_NUMBER,
	utlAT_PARSER_OP_SET_OBJECT_ID,
	utlAT_PARSER_OP_SET_COUNTRY_CODE,
	utlAT_PARSER_OP_SET_CONNECT_TEXT,
	utlAT_PARSER_OP_ENABLE_BYPASS_MODE,
	utlAT_PARSER_OP_DISABLE_BYPASS_MODE
} utlAtParserOp_T;

/*--- driver requests ---*/
typedef enum {
	utlAT_DRIVER_REQUEST_TONE_DIAL,
	utlAT_DRIVER_REQUEST_PULSE_DIAL,
	utlAT_DRIVER_REQUEST_ON_HOOK,
	utlAT_DRIVER_REQUEST_OFF_HOOK,
	utlAT_DRIVER_REQUEST_ANSWER_OFF_HOOK,
	utlAT_DRIVER_REQUEST_FLASH_ON_HOOK,
	utlAT_DRIVER_REQUEST_FLASH_OFF_HOOK,
	utlAT_DRIVER_REQUEST_RESET_ON_HOOK,
	utlAT_DRIVER_REQUEST_ANSWER
} utlAtDriverRequest_T;

/*--- driver events ---*/
typedef enum {
	utlAT_DRIVER_EVENT_ANSWER_CONF,
	utlAT_DRIVER_EVENT_ANSWERED,
	utlAT_DRIVER_EVENT_BUSY,
	utlAT_DRIVER_EVENT_CARRIER,
	utlAT_DRIVER_EVENT_CREDIT_BONG,
	utlAT_DRIVER_EVENT_DIAL_CONF,
	utlAT_DRIVER_EVENT_DIAL_TONE,
	utlAT_DRIVER_EVENT_HANG_UP,
	utlAT_DRIVER_EVENT_NO_ANSWER,
	utlAT_DRIVER_EVENT_NO_CARRIER,
	utlAT_DRIVER_EVENT_NO_DIAL_TONE,
	utlAT_DRIVER_EVENT_OFF_HOOK_CONF,
	utlAT_DRIVER_EVENT_ON_HOOK_CONF,
	utlAT_DRIVER_EVENT_RING,
	utlAT_DRIVER_EVENT_REQUEST_FAILED,
	utlAT_DRIVER_EVENT_UNSOLICITED_INFO_TEXT,
	utlAT_DRIVER_EVENT_UNSOLICITED_DTE_MESSAGE,

#   ifdef utlTEST
	utlAT_DRIVER_EVENT_FORCE_TIMEOUT,
#   endif
} utlAtDriverEventId_T;

/*--- AT-command responses ---*/
typedef enum {
	utlAT_COMMAND_RESPONSE_TYPE_PARAMETERS,
	utlAT_COMMAND_RESPONSE_TYPE_CUSTOM,     /* totally over-rides the usual response string        */
	utlAT_COMMAND_RESPONSE_TYPE_INFO_TEXT,  /* for text responses followed by a normal result-code */
	utlAT_COMMAND_RESPONSE_TYPE_REQUEST_COMPLETED,
	utlAT_COMMAND_RESPONSE_TYPE_COMMAND_SYNTAX,
	utlAT_COMMAND_RESPONSE_TYPE_ABORT,
	utlAT_COMMAND_RESPONSE_TYPE_ERROR,
} utlAtCommandResponseType_T;

/*--- parameter operations ---*/
typedef enum {
	utlAT_PARAMETER_OP_UNKNOWN,
	utlAT_PARAMETER_OP_EXEC,
	utlAT_PARAMETER_OP_GET,
	utlAT_PARAMETER_OP_SET,
	utlAT_PARAMETER_OP_ACTION,
	utlAT_PARAMETER_OP_SYNTAX
} utlAtParameterOp_T, *utlAtParameterOp_P;

/*--- call-backs ---*/
typedef utlReturnCode_T (*utlAtGetParameterFunction_P)(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
typedef utlReturnCode_T (*utlAtSetParameterFunction_P)(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
typedef utlReturnCode_T (*utlAtDceIoConfigFunction_P)(const utlAtDceIoConfig_P2c dce_io_config_p, void *arg_p);
typedef utlReturnCode_T (*utlAtSoundConfigFunction_P)(const utlAtSoundConfig_P2c sound_config_p, void *arg_p);
typedef utlReturnCode_T (*utlAtSParameterFunction_P)(const unsigned int parameter_num, const unsigned int parameter_value, void *arg_p);
typedef utlReturnCode_T (*utlAtSaveDialStringFunction_P)(const char  *location_name_p,  const char  *dial_string_p,  void *arg_p);
typedef utlReturnCode_T (*utlAtRetrieveDialStringFunction_P)(const char **location_name_pp, const char **dial_string_pp, void *arg_p);
typedef utlReturnCode_T (*utlAtReplyFunction_P)(const char *string_p, void *arg_p);
typedef utlReturnCode_T (*utlAtTxLineDataFunction_P)(const unsigned char *octets_p, const size_t n, void *arg_p);
typedef utlReturnCode_T (*utlAtDriverRequestFunction_P)(const utlAtParser_P parser_p, const utlAtDriverRequest_T request, void *arg_p, ...);
typedef utlReturnCode_T (*utlAtCommandSyntaxFunction_P)(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);


typedef enum {
	utlAT_COMMAND_TYPE_BASIC,       /* basic AT command              */
	utlAT_COMMAND_TYPE_EXTENDED,    /* extended-parameter AT command */
	utlAT_COMMAND_TYPE_EXACTION     /* extended-action    AT command */
} utlAtCommandType_T;

typedef struct utlAtCommand_S {
	utlAtCommandType_T type;        /* AT-Command type */
	const char         *name_p;     /* AT-Command name */

	utlAtParameter_P parameters_p;  /* NULL pointer, or pointer to a variable-length array      */
	size_t num_parameters;          /* number of elements in array pointed to by `parameters_p' */

	struct {
		const char *command_syntax_p; /* AT-Command usage string (for syntax queries) */
		struct {
			utlAtCommandSyntaxFunction_P command_syntax_function_p;
		} call_backs;
	} usage;

	struct {
		utlAtGetParameterFunction_P get_parameter_function_p;   /* for get requests          */
		utlAtSetParameterFunction_P set_parameter_function_p;   /* for set & action requests */
	} call_backs;
}             utlAtCommand_T, *utlAtCommand_P;
typedef const utlAtCommand_T  *utlAtCommand_P2c;

#define utlDEFINE_BASIC_AT_COMMAND(name, parameters_p, exec_function_p)	 \
	{ utlAT_COMMAND_TYPE_BASIC, name, parameters_p,		   \
	  sizeof(parameters_p) / sizeof(utlAtParameter_T),	    \
	  { NULL, { NULL } }, { NULL, exec_function_p } }

#define utlDEFINE_EXTENDED_AT_COMMAND(name, parameters_p, syntax, get_function_p, set_function_p) \
	{ utlAT_COMMAND_TYPE_EXTENDED, name, parameters_p,	 \
	  sizeof(parameters_p) / sizeof(utlAtParameter_T),	    \
	  { syntax, { NULL } }, { get_function_p, set_function_p } }

#define utlDEFINE_EXACTION_AT_COMMAND(name, parameters_p, syntax, action_function_p) \
	{ utlAT_COMMAND_TYPE_EXACTION, name, parameters_p,	 \
	  sizeof(parameters_p) / sizeof(utlAtParameter_T),	    \
	  { syntax, { NULL } }, { NULL, action_function_p } }

#define utlDEFINE_EXTENDED_VSYNTAX_AT_COMMAND(name, parameters_p,	\
					      syntax_function_p, get_function_p, set_function_p)       \
	{ utlAT_COMMAND_TYPE_EXTENDED, name, parameters_p,	 \
	  sizeof(parameters_p) / sizeof(utlAtParameter_T),	    \
	  { NULL, { syntax_function_p } }, { get_function_p, set_function_p } }

#define utlDEFINE_EXACTION_VSYNTAX_AT_COMMAND(name, parameters_p,	\
					      syntax_function_p, action_function_p)		      \
	{ utlAT_COMMAND_TYPE_EXACTION, name, parameters_p,	 \
	  sizeof(parameters_p) / sizeof(utlAtParameter_T),	    \
	  { NULL, { syntax_function_p } }, { NULL, action_function_p } }

#define utlDEFINE_EXTENDED_EXACTION_AT_COMMAND(name, parameters_p,   \
					       syntax, get_function_p, action_function_p) \
	{ utlAT_COMMAND_TYPE_EXACTION, name, parameters_p,	 \
	  sizeof(parameters_p) / sizeof(utlAtParameter_T),	    \
	  { syntax, { NULL } }, { get_function_p, action_function_p } }

typedef enum {
	utlAT_DIALING_MODE_TONE,
	utlAT_DIALING_MODE_PULSE
} utlAtDialingMode_T;

typedef struct utlAtDialStringOptions_S {
	bool international;
	bool do_call_origination;
	bool use_CCUG_SS_info;
	char CLI_presentation; /* 'I' == restrict, 'i' == allow, '\0' == use default */
}             utlAtDialStringOptions_T, *utlAtDialStringOptions_P;
typedef const utlAtDialStringOptions_T  *utlAtDialStringOptions_P2c;

typedef enum {
	utlAT_ASYNC_OP_NULL,
	utlAT_ASYNC_OP_UNKNOWN,
	utlAT_ASYNC_OP_EXEC,
	utlAT_ASYNC_OP_GET,
	utlAT_ASYNC_OP_SET,
	utlAT_ASYNC_OP_ACTION,
	utlAT_ASYNC_OP_SYNTAX
} utlAtAsyncOp_T, *utlAtAsyncOp_P;

typedef struct utlAtAsyncResponse_S *utlAtAsyncResponse_P;
typedef struct utlAtAsyncResponse_S {
	utlAtAsyncResponse_P next_p;
	utlAtParser_P parser_p;
	unsigned int xid;               /* transaction ID                 */
	utlAtAsyncOp_T op;              /* pending asynchronous operation */
	utlAtCommand_P2c command_p;     /* pending asynchronous command   */
	const char           *text_response_p;
	utlTimerId_T timer_id;
}             utlAtAsyncResponse_T;
typedef const utlAtAsyncResponse_T *utlAtAsyncResponse_P2c;

typedef struct utlAtAsyncResponses_S {
	utlLinkedList_T unused;         /* list unused/free nodes */
	utlLinkedList_T pending;        /* list of pending events */
	utlAtAsyncResponse_T nodes[utlAT_MAX_ASYNC_RESPONSES_PENDING];
}             utlAtAsyncResponses_T, *utlAtAsyncResponses_P;
typedef const utlAtAsyncResponses_T  *utlAtAsyncResponses_P2c;

typedef struct utlAtParser_S {
	utlAtParser_P next_p;

	struct {
		const char *id_p;
		const char *manufacturer_p;
		const char *model_p;
		const char *revision_p;
		const char *serial_number_p;
		const char *object_id_p;
		const char *country_code_p;
		const char *connect_text_p;
	} info;

	struct {
		/*--- element assignments in following S-parameters array ---*/
#       define utlAT_AUTO_ANSWER           ((size_t)  0)        /* 0, or number of rings before answer */
#       define utlAT_RING_COUNTER          ((size_t)  1)        /* counts number of rings              */
#       define utlAT_ESCAPE_CHAR           ((size_t)  2)        /* on-line escape character            */
#       define utlAT_LINE_TERM_CHAR        ((size_t)  3)        /* command-line termination character  */
#       define utlAT_FORMATTING_CHAR       ((size_t)  4)        /* response formatting character       */
#       define utlAT_LINE_EDIT_CHAR        ((size_t)  5)        /* command-line editing character      */
#       define utlAT_BLIND_DIAL_PAUSE_TIME ((size_t)  6)        /* dial tone wait time (in seconds)    */
#       define utlAT_CONN_COMPLETE_TIMEOUT ((size_t)  7)        /* carrier wait time (in seconds)      */
#       define utlAT_DIALING_PAUSE_TIME    ((size_t)  8)        /* dial-string comma time (in seconds) */
#       define utlAT_CARRIER_DETECT_TIME   ((size_t)  9)        /* (in tenths of a second)             */
#       define utlAT_CARRIER_LOSS_TIME     ((size_t) 10)        /* (in tenths of a second)             */
#       define utlAT_DTMF_TONE_DURATION    ((size_t) 11)        /* dial tone spacing (in 1/100 of a second) */
#       define utlAT_ESCAPE_GUARD_TIME     ((size_t) 12)        /* escape code time  (in 1/50  of a second) */
#       define utlAT_FEATURES_MASK         ((size_t) 13)  /* bit map: 1 = reset on DTR loss
								      2 = reduced non-ARQ TX buffer
								      4 = set DEL==<back space>
								      8 = Do DS0 on DTR
								     16 = Do DS0 on Reset
								     32 = reserved
								     64 = disable quick retrains
								    128 = escape code hang-up */
#       define utlAT_SEPARATER_CHAR        ((size_t) 20)        /* extended AT command parameter separator */
#       define utlAT_TERMINATER_CHAR       ((size_t) 21)        /* extended AT command terminator          */
#       define utlAT_XON_CHAR              ((size_t) 22)        /* software flow control     */
#       define utlAT_XOFF_CHAR             ((size_t) 23)        /* software flow control     */
#       define utlAT_SLEEP_TIME            ((size_t) 24)        /* (seconds)                 */
#       define utlAT_DTR_DELAY_TIME        ((size_t) 25)        /* (in tens of milliseconds) */
#       define utlAT_HOOK_FLASH_TIME       ((size_t) 29)        /* (in tenths of a second)   */
#       define utlAT_INACTIVITY_TIME       ((size_t) 30)        /* (in tens of seconds)      */
#       define utlAT_DISCONNECT_WAIT_TIME  ((size_t) 38)        /* disconnect wait time (in seconds) */
		unsigned short S[utlAT_NUM_S_PARAMETERS];

		bool echo_text;
		bool verbose_results;
		bool suppress_results;
		bool include_connect_text;
		bool detect_dial_tone;
		bool detect_busy_signal;
		utlAtDialingMode_T dialing_mode;
		unsigned int raw_framing;
		unsigned int raw_parity;
	} parameters;

	struct {
		bool allow_default_S_parameter_values;
		bool allow_string_escapes;      /* allow "\hh" to specify ordinal character values in strings */
		bool use_carrier_result_code;
		bool report_each_ring;          /* if false, only the first ring is reported */
	} options;

	struct {
		char queue[utlAT_MAX_LINE_LENGTH + 1];  /* queue for accumulating in-coming AT-command data */
		char *echo_p;                           /* pointer to first unechoed character   */
		char *head_p;                           /* pointer to next unfilled slot         */
		char *tail_p;                           /* pointer to oldest filled slot         */
		char *term_p;                           /* pointer to slot just-beyond last slot */

		bool overflow;

		char previous_line[utlAT_MAX_LINE_LENGTH + 1]; /* previously executed AT command */
	} buffers;

	struct {
		utlAtDceIoConfigFunction_P dce_io_config_function_p;
		utlAtSoundConfigFunction_P sound_config_function_p;
		utlAtSParameterFunction_P s_parameter_function_p;
		utlAtSaveDialStringFunction_P save_dial_string_function_p;
		utlAtRetrieveDialStringFunction_P retrieve_dial_string_function_p;
		utlAtReplyFunction_P reply_function_p;
		utlAtTxLineDataFunction_P tx_line_data_function_p;
		utlAtDriverRequestFunction_P driver_request_function_p;
		void                              *arg_p;
	} call_backs;

	utlAtDceIoConfig_T dce_io_config;
	utlAtSoundConfig_T sound_config;

	utlAtCommand_P2c commands_p;    /* pointer to a variable-length array                  */
	size_t num_commands;            /* number of array elements pointed to by `commands_p' */

	struct {
		unsigned int basic_commands;
		unsigned int extended_commands;
		unsigned int undefined_commands;
		unsigned int bad_commands;
	} peg_counts;

	struct {
		bool go_on_line;
		bool lost_rx_data;
		bool discard_current_line;
		bool bypass_mode;

		utlAtAsyncResponses_T async_responses;

		struct {
			bool complete_reported;
			bool abort_reported;
			bool error_reported;
			utlAtParameterValue_P2c parameter_values_p;
			const char              *custom_p;
			const char              *info_text_p;
			const char              *abort_response_p;
			const char              *command_syntax_p;
		} sync_response;

		/*--- bit assignments in `dce_io_config_pending_mask' ---*/
#       define utlAT_DCE_IO_CONFIG_PENDING_DIAL_TONE    0x0001
#       define utlAT_DCE_IO_CONFIG_PENDING_BUSY_SIGNAL  0x0002
#       define utlAT_DCE_IO_CONFIG_PENDING_DCD          0x0004
#       define utlAT_DCE_IO_CONFIG_PENDING_DTR          0x0008
#       define utlAT_DCE_IO_CONFIG_PENDING_DSR          0x0010
#       define utlAT_DCE_IO_CONFIG_PENDING_BIT_RATE     0x0020
#       define utlAT_DCE_IO_CONFIG_PENDING_PARITY       0x0040
#       define utlAT_DCE_IO_CONFIG_PENDING_FRAMING      0x0080
#       define utlAT_DCE_IO_CONFIG_PENDING_FLOW_CONTROL 0x0100
#       define utlAT_DCE_IO_CONFIG_PENDING_DTR_DELAY    0x0200
		unsigned int dce_io_config_pending_mask; /* unreported DCE I/O configuration changes */

		/*--- bit assignments in `sound_config_pending_mask' ---*/
#       define utlAT_SOUND_CONFIG_PENDING_MODE  0x0001
#       define utlAT_SOUND_CONFIG_PENDING_LEVEL 0x0002
		unsigned int sound_config_pending_mask; /* unreported sound configuration changes */

		size_t line_off;                        /* character offset on current line (for error reporting) */

		struct {
			utlAbsoluteTime_T last_tx_time;
			enum {
				utlAT_PARSER_ESCAPE_STATE_BEGIN,
				utlAT_PARSER_ESCAPE_STATE_FOUND_ONE,
				utlAT_PARSER_ESCAPE_STATE_FOUND_TWO,
			} state;
		} escape;

		struct {
			bool active;                                    /* is dial-string stored in `buf' pending or in use? */
			unsigned int dial_string_delay;                 /* (in 0.125 second increments) */
			char last[utlAT_MAX_DIAL_STRING_LENGTH + 1];    /* last number dialed          */
			char buf[utlAT_MAX_DIAL_STRING_LENGTH + 1];     /* current number being dialed */
			char         *c_p;                              /* pointer to next unprocessed character in `buf' */

			utlAtDialStringOptions_T options;
		} dial_string;

		utlRelativeTime_T blind_dial_pause_time;        /* slaved from S-parameter 6  */
		utlRelativeTime_T conn_complete_timeout;        /* slaved from S-parameter 7  */
		utlRelativeTime_T dialing_pause_time;           /* slaved from S-parameter 8  */
		utlRelativeTime_T carrier_detect_time;          /* slaved from S-parameter 9  */
		utlRelativeTime_T carrier_loss_time;            /* slaved from S-parameter 10 */
		utlRelativeTime_T hook_flash_time;              /* slaved from S-parameter 29 */
		utlRelativeTime_T inactivity_time;              /* slaved from S-parameter 30 */
		utlRelativeTime_T disconnect_wait_time;         /* slaved from S-parameter 38 */
		utlRelativeTime_T delay_dialing_time;           /* 0.125 seconds fixed        */
		utlRelativeTime_T min_silence_time;             /* 5.0   seconds fixed        */

	} states;
	utlSemaphore_T queue_semaphore;                                 /* Used for multiple commands queue operation */
	size_t commands_in_line;                                        /* the number of commands in one line */
	utlSemaphore_T cmd_cnt_semaphore;                               /* Used for protecting commands_in_line */
}             utlAtParser_T;
typedef const utlAtParser_T  *utlAtParser_P2c;

typedef struct ATCmd_TimeOut{
	const char *name_p;
	unsigned int time_second[3];
}ATCMD_TimeOut;

/*--- Prototypes ------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

extern utlAtParser_P   utlOpenAtParser(const utlAtCommand_P2c commands_p,
				       const size_t num_commands,
				       void                       *arg_p);
extern utlReturnCode_T utlCloseAtParser(const utlAtParser_P parser_p);
extern utlReturnCode_T utlAtParserOp(const utlAtParser_P parser_p,
				     const utlAtParserOp_T op,
				     ...);
extern utlReturnCode_T utlAtDriverEvent(const utlAtParser_P parser_p,
					const utlAtDriverEventId_T event_id,
					...);
extern utlReturnCode_T utlAtCommandResponse(const unsigned int xid,
					    const utlAtCommandResponseType_T response_type,
					    ...);
extern size_t          utlAtParserInitCommandParameters(const unsigned int xid,
							const utlAtParameterValue_P parameter_values_p,
							const size_t num_parameters);
extern utlReturnCode_T utlAtParse(const utlAtParser_P parser_p,
				  const unsigned char              *octets_p,
				  const size_t n);

extern utlReturnCode_T utlModifyPendingAsyncResponse(	const utlRelativeTime_P period_p,
							 const unsigned int xid);
extern bool utlAtCommandTimeoutModify(const char *commandName, int mode, int timeout);

#if defined(utlDEBUG) || defined(utlTEST)
extern void            utlVStringDumpAtParser(const utlVString_P v_string_p,
					      const utlAtParser_P parser_p,
					      const char                       *prefix_p);
extern void            utlDumpAtParser(      FILE                       *file_p,
					     const utlAtParser_P parser_p);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _UTL_AT_PARSER_INCLUDED */

