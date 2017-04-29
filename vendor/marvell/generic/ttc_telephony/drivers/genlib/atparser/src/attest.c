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
#include <utlStateMachine.h>
#include <utlTime.h>
#include <utlTimer.h>
#include <utlTrace.h>
#include <utlVString.h>

#include <utlAtParser.h>

#include <teldef.h>

#ifdef utlTEST
#   include <termios.h>
#   include <utlEventHandler.h>
#endif



/*---------------------------------------------------------------------------*/
typedef struct {
	enum {
		utlMODE_NULL,
		utlMODE_EXIT_EVENT_HANDLER,
	} mode;
	size_t num_cycles;
	size_t curr_cycle;
	double last;
	double period;
} testInfo_T;

static struct {
	char reply_string[500];

	unsigned int s_parameter_num;
	unsigned int s_parameter_value;

	size_t num_commands;
	struct {
		char name[20];
		utlAtParameterValue_T parameter_values[20];
		size_t num_parameters;
	} commands[10];

	unsigned int driver_request_test;
	unsigned int driver_request_test_state;

	char string_buf[500];
	size_t next_unused;  /* index into string_buf[] */
} test_results;

static unsigned int test_async_xid = utlFAILED;


/*---------------------------------------------------------------------------*
* FUNCTION
*      dumpTestResults(f_p)
* INPUT
*      f_p == a valid file pointer
* OUTPUT
*      none
* RETURNS
*      nothing
* DESCRIPTION
*      Dumps the `test_results' structure to stderr
*---------------------------------------------------------------------------*/
static void dumpTestResults(FILE *f_p)
{
	int i, j;

	fprintf(f_p, "Num Commands: %d\n", (int)test_results.num_commands);
	for (j = 0; j < test_results.num_commands; j++)
	{
		fprintf(f_p, "     Command: '%s'\n",     test_results.commands[j].name);
		fprintf(f_p, "  Num Params: %d\n", (int)test_results.commands[j].num_parameters);

		for (i = 0; i < test_results.commands[j].num_parameters; i++)
		{
			fprintf(f_p, "values[%d].type: %d\n",       i, test_results.commands[j].parameter_values[i].type);
			fprintf(f_p, "values[%d].access: %d\n",     i, test_results.commands[j].parameter_values[i].access);
			fprintf(f_p, "values[%d].is_default: %d\n", i, test_results.commands[j].parameter_values[i].is_default);
			fprintf(f_p, "values[%d].value: ",          i);
			switch (test_results.commands[j].parameter_values[i].type)
			{
			case utlAT_DATA_TYPE_DECIMAL:     fprintf(f_p, "%d\n",     test_results.commands[j].parameter_values[i].value.decimal);        break;
			case utlAT_DATA_TYPE_HEXADECIMAL: fprintf(f_p, "%d\n",     test_results.commands[j].parameter_values[i].value.hexadecimal);    break;
			case utlAT_DATA_TYPE_BINARY:      fprintf(f_p, "%d\n",     test_results.commands[j].parameter_values[i].value.binary);         break;
			case utlAT_DATA_TYPE_STRING:      fprintf(f_p, "\"%s\"\n", test_results.commands[j].parameter_values[i].value.string_p);       break;
			case utlAT_DATA_TYPE_QSTRING:     fprintf(f_p, "\"%s\"\n", test_results.commands[j].parameter_values[i].value.qstring_p);      break;
			case utlAT_DATA_TYPE_DIAL_STRING: fprintf(f_p, "%s\n",     test_results.commands[j].parameter_values[i].value.dial_string_p);  break;
			default:                          fprintf(f_p, "???\n");
			}
		}
	}

	for (i = 0; test_results.reply_string[i] != '\0'; i++)
		if ((isalnum(test_results.reply_string[i])) ||
		    (ispunct(test_results.reply_string[i])))
			fprintf(f_p, "echo[%d] = %d  ('%c')\n", i, test_results.reply_string[i], test_results.reply_string[i]);
		else
			fprintf(f_p, "echo[%d] = %d\n", i, test_results.reply_string[i]);

	fprintf(f_p, "echo: '%s'\n", test_results.reply_string);
}
/*----------------------------------------------------------------------*
* FUNCTION
*      testFunc(id, timeout_count, arg, curr_time)
* INPUT
*                 id == timer ID
*      timeout_count == the time-out count
*              arg_p == pointer to user-defined data
*        curr_time_p == current time since the epoch
* OUTPUT
*      modifies the data pointed to by `arg_p'
* RETURNS
*      utlSUCCESS for success, utlFAILED for failure.
* DESCIPTION
*      Handles timer time outs.
*----------------------------------------------------------------------*/
static utlReturnCode_T testFunc(const utlTimerId_T id,
				const unsigned long timeout_count,
				void                *arg_p,
				const utlAbsoluteTime_P2c curr_time_p)
{
	testInfo_T *test_info_p;
	double delta;

	utlAssert(arg_p       != NULL);
	utlAssert(curr_time_p != NULL);

	test_info_p = (testInfo_T *)arg_p;
	test_info_p->curr_cycle++;

	delta =             curr_time_p->seconds + (curr_time_p->nanoseconds / 1000000000.0) - test_info_p->last;
	test_info_p->last = curr_time_p->seconds + (curr_time_p->nanoseconds / 1000000000.0);

	/*--- time-out time OK? ---*/
	if ((test_info_p->curr_cycle > (size_t)1) && (fabs(test_info_p->period - delta) > 0.15))
		fprintf(stderr, "atParserTest: warning, timing error = %f\n", fabs(test_info_p->period - delta));

	if (test_info_p->curr_cycle == test_info_p->num_cycles)
	{
		switch (test_info_p->mode)
		{
		case utlMODE_NULL:
			break;

		case utlMODE_EXIT_EVENT_HANDLER:
			utlExitEventLoop();
			break;
		}
	}

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      updateTestResults(command_name_p, parameter_values_p, num_parameters,
*                        info_text_p, arg_p)
* INPUT
*          command_name_p == the AT-command expressed as a null-terminated character string
*      parameter_values_p == NULL, or a pointer to a list of parameter-value structures
*          num_parameters == number of parameters pointed to by `parameter_values_p'
*             info_text_p == pointer to info text, or a NULL pointer
*                   arg_p == pointer to user-defined data
* OUTPUT
*      none
* RETURNS
*      utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*      Provides values for get-parameter-value requests.
*---------------------------------------------------------------------------*/
static utlReturnCode_T updateTestResults(const char                    *command_name_p,
					 const utlAtParameterValue_P2c parameter_values_p,
					 const size_t num_parameters,
					 const char                    *info_text_p,
					 void                    *arg_p)
{
	if (command_name_p == NULL)
		return utlFAILED;

	strcpy(test_results.commands[test_results.num_commands].name, command_name_p);

	if (parameter_values_p == NULL)
	{
		if (num_parameters != (size_t)0)
			return utlFAILED;
	}
	else
	{
		size_t i;

		if (num_parameters == (size_t)0)
			return utlFAILED;

		for (i = 0; i < num_parameters; i++)
		{
			test_results.commands[test_results.num_commands].parameter_values[i] = parameter_values_p[i];

			if (parameter_values_p[i].type == utlAT_DATA_TYPE_STRING)
			{
				strcpy(test_results.string_buf + test_results.next_unused, parameter_values_p[i].value.string_p);
				test_results.commands[test_results.num_commands].parameter_values[i].value.string_p = test_results.string_buf + test_results.next_unused;
				test_results.next_unused += strlen(parameter_values_p[i].value.string_p) + 1;

			}
			else if (parameter_values_p[i].type == utlAT_DATA_TYPE_QSTRING)
			{
				strcpy(test_results.string_buf + test_results.next_unused, parameter_values_p[i].value.qstring_p);
				test_results.commands[test_results.num_commands].parameter_values[i].value.qstring_p = test_results.string_buf + test_results.next_unused;
				test_results.next_unused += strlen(parameter_values_p[i].value.qstring_p) + 1;

			}
			else if (parameter_values_p[i].type == utlAT_DATA_TYPE_DIAL_STRING)
			{
				strcpy(test_results.string_buf + test_results.next_unused, parameter_values_p[i].value.dial_string_p);
				test_results.commands[test_results.num_commands].parameter_values[i].value.dial_string_p = test_results.string_buf + test_results.next_unused;
				test_results.next_unused += strlen(parameter_values_p[i].value.dial_string_p) + 1;
			}
		}
	}

	test_results.commands[test_results.num_commands].num_parameters = num_parameters;

	test_results.num_commands++;

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      syntaxFunction(command_name_p, xid_p, arg_p)
* INPUT
*      command_name_p == the AT-command expressed as a null-terminated character string
*               xid_p == pointer to a unique transaction ID
*               arg_p == pointer to user-defined data
* OUTPUT
*      *xid_p == possibly modified transaction ID
* RETURNS
*      utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*      Provides values for AT-command syntax query requests.
*---------------------------------------------------------------------------*/
static utlReturnCode_T syntaxFunction(const char         *command_name_p,
				      unsigned int *xid_p,
				      void         *arg_p)
{
	if (command_name_p == NULL)
		return utlFAILED;

	if (strcasecmp(command_name_p, "+EVSYN") == 0)
	{
		if (utlAtCommandResponse(*xid_p, utlAT_COMMAND_RESPONSE_TYPE_COMMAND_SYNTAX, "+EVSYN=(0-9),(10-20),(20-30)") != utlSUCCESS)
			return utlFAILED;

	}
	else if (strcasecmp(command_name_p, "+AVSYN") == 0)
	{
		if (utlAtCommandResponse(*xid_p, utlAT_COMMAND_RESPONSE_TYPE_COMMAND_SYNTAX, "+AVSYN=(10-19),(20-30),(30-40)") != utlSUCCESS)
			return utlFAILED;

	}
	else
		return utlFAILED;

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      syntaxAsyncFunction(command_name_p, xid_p, arg_p)
* INPUT
*      command_name_p == the AT-command expressed as a null-terminated character string
*               xid_p == pointer to a unique transaction ID
*               arg_p == pointer to user-defined data
* OUTPUT
*      *xid_p == possibly modified transaction ID
* RETURNS
*      utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*      Provides values for AT-command syntax query requests.
*---------------------------------------------------------------------------*/
static utlReturnCode_T syntaxAsyncFunction(const char         *command_name_p,
					   unsigned int *xid_p,
					   void         *arg_p)
{
	if (command_name_p == NULL)
		return utlFAILED;

	if (strcasecmp(command_name_p, "+EVSYNA") == 0)
	{
		test_async_xid = *xid_p;

	}
	else if (strcasecmp(command_name_p, "+AVSYNA") == 0)
	{
		test_async_xid = *xid_p;

	}
	else if (strcasecmp(command_name_p, "+EVSYNAS") == 0)
	{
		if (utlAtCommandResponse(*xid_p, utlAT_COMMAND_RESPONSE_TYPE_COMMAND_SYNTAX, "+EVSYNAS=(0-9),(10-20),(20-30)") != utlSUCCESS)
			return utlFAILED;

	}
	else if (strcasecmp(command_name_p, "+AVSYNAS") == 0)
	{
		if (utlAtCommandResponse(*xid_p, utlAT_COMMAND_RESPONSE_TYPE_COMMAND_SYNTAX, "+AVSYNAS=(10-19),(20-30),(30-40)") != utlSUCCESS)
			return utlFAILED;

	}
	else
		return utlFAILED;

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      getFunction(op, command_name_p, parameter_values_p, num_parameters,
*                  info_text_p, xid_p, arg_p)
* INPUT
*                      op == the parameter operation
*          command_name_p == the AT-command expressed as a null-terminated character string
*      parameter_values_p == NULL, or a pointer to a list of parameter-value structures
*          num_parameters == number of parameters pointed to by `parameter_values_p'
*             info_text_p == pointer to info text, or a NULL pointer
*                   xid_p == pointer to a unique transaction ID
*                   arg_p == pointer to user-defined data
* OUTPUT
*      *xid_p == possibly modified transaction ID
* RETURNS
*      utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*      Provides values for get-parameter-value requests.
*---------------------------------------------------------------------------*/
static utlReturnCode_T getFunction(const utlAtParameterOp_T op,
				   const char                    *command_name_p,
				   const utlAtParameterValue_P2c parameter_values_p,
				   const size_t num_parameters,
				   const char                    *info_text_p,
				   unsigned int            *xid_p,
				   void                    *arg_p)
{
	static utlAtParameterValue_T parameter_values[10];

	if (op != utlAT_PARAMETER_OP_GET)
		return utlFAILED;
	if (command_name_p == NULL)
		return utlFAILED;

	/*--- init parameter_values structure ---*/
	if (num_parameters > 0)
	{
		if (utlAtParserInitCommandParameters(*xid_p, parameter_values, 10) == utlFAILED)
			return utlFAILED;
	}

	if ((strcasecmp(command_name_p, "+CBACK") == 0) ||
	    (strcasecmp(command_name_p, "+GMR")   == 0))
	{
		static char new_info_text[400];

		if (info_text_p != NULL)
		{
			strcpy(new_info_text, info_text_p);
			strcat(new_info_text, "\r\n");
		}
		else
			new_info_text[0] = '\0';

		strcat(new_info_text, "Custom response string");

		if (utlAtCommandResponse(*xid_p, utlAT_COMMAND_RESPONSE_TYPE_INFO_TEXT, new_info_text) != utlSUCCESS)
			return utlFAILED;

	}
	else if ((info_text_p != NULL) && (info_text_p[0] != '\0'))
	{

		if (utlAtCommandResponse(*xid_p, utlAT_COMMAND_RESPONSE_TYPE_INFO_TEXT, info_text_p) != utlSUCCESS)
			return utlFAILED;

		/*--- else assign test return values ---*/
	}
	else
	{
		size_t n;

		if ((n = utlAtParserInitCommandParameters(*xid_p, parameter_values, utlNumberOf(parameter_values))) == utlFAILED)
			return utlFAILED;

		for (n = 0; n < num_parameters; n++)
		{

			parameter_values[n].is_default = false;

			switch (parameter_values[n].type)
			{
			case utlAT_DATA_TYPE_DECIMAL:     parameter_values[n].value.decimal       = 9;          break;
			case utlAT_DATA_TYPE_HEXADECIMAL: parameter_values[n].value.hexadecimal   = 10;         break;
			case utlAT_DATA_TYPE_BINARY:      parameter_values[n].value.binary        = 11;         break;
			case utlAT_DATA_TYPE_STRING:      parameter_values[n].value.string_p      = "abc";      break;
			case utlAT_DATA_TYPE_QSTRING:     parameter_values[n].value.qstring_p     = "abc";      break;
			case utlAT_DATA_TYPE_DIAL_STRING: parameter_values[n].value.dial_string_p = "5551212";  break;

			default:
				break;
			}
		}

		if (utlAtCommandResponse(*xid_p, utlAT_COMMAND_RESPONSE_TYPE_PARAMETERS, parameter_values, n) != utlSUCCESS)
			return utlFAILED;
	}

	if (num_parameters > 0) return updateTestResults(command_name_p, parameter_values, num_parameters, info_text_p, arg_p);
	else return updateTestResults(command_name_p, NULL,             num_parameters, info_text_p, arg_p);
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      getAsyncFunction(op, command_name_p, parameter_values_p, num_parameters,
*                       info_text_p, xid_p, arg_p)
* INPUT
*                      op == the parameter operation
*          command_name_p == the AT-command expressed as a null-terminated character string
*      parameter_values_p == NULL, or a pointer to a list of parameter-value structures
*          num_parameters == number of parameters pointed to by `parameter_values_p'
*             info_text_p == pointer to info text, or a NULL pointer
*                   xid_p == pointer to a unique transaction ID
*                   arg_p == pointer to user-defined data
* OUTPUT
*      *xid_p == possibly modified transaction ID
* RETURNS
*      utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*      Initiates a get-asynchronous-parameter-value request.
*---------------------------------------------------------------------------*/
static utlReturnCode_T getAsyncFunction(const utlAtParameterOp_T op,
					const char                    *command_name_p,
					const utlAtParameterValue_P2c parameter_values_p,
					const size_t num_parameters,
					const char                    *info_text_p,
					unsigned int            *xid_p,
					void                    *arg_p)
{
	static utlAtParameterValue_T parameter_values[10];

	if (op != utlAT_PARAMETER_OP_GET)
		return utlFAILED;
	if (command_name_p == NULL)
		return utlFAILED;

	/*--- init parameter_values structure ---*/
	if (num_parameters > 0)
	{
		if (utlAtParserInitCommandParameters(*xid_p, parameter_values, 10) == utlFAILED)
			return utlFAILED;
	}

	/*--- note asynchronous "get" ---*/
	if (strcasecmp(command_name_p, "+AINFO")  == 0)
	{
		test_async_xid = *xid_p;

	}
	else if (strcasecmp(command_name_p, "+APARAM") == 0)
	{
		/*--- we'll always modify the transaction ID of this command (to test the ability
		   of applications to specify their own transaction ID's */
		*xid_p += 1000;

		test_async_xid = *xid_p;

	}
	else if (strcasecmp(command_name_p, "+SINFO")  == 0)
	{
		if (utlAtCommandResponse(*xid_p, utlAT_COMMAND_RESPONSE_TYPE_CUSTOM, "the quick brown fox") != utlSUCCESS)
			return utlFAILED;

	}
	else if (strcasecmp(command_name_p, "+SPARAM") == 0)
	{
		size_t n;

		test_async_xid = *xid_p;

		if ((n = utlAtParserInitCommandParameters(*xid_p, parameter_values, 10)) == utlFAILED)
			return utlFAILED;

		parameter_values[0].is_default        = false;
		parameter_values[0].value.decimal     = 9;
		parameter_values[1].is_default        = false;
		parameter_values[1].value.hexadecimal = 0xa;
		parameter_values[2].is_default        = false;
		parameter_values[2].value.binary      = 11;
		parameter_values[3].is_default        = false;
		parameter_values[3].value.string_p    = "abc";
		parameter_values[4].is_default        = false;
		parameter_values[4].value.string_p    = "abc";
		parameter_values[5].is_default        = false;
		parameter_values[5].value.decimal     = 9;

		if (utlAtCommandResponse(*xid_p, utlAT_COMMAND_RESPONSE_TYPE_PARAMETERS, &parameter_values, n) != utlSUCCESS)
			return utlFAILED;

	}
	else if (strcasecmp(command_name_p, "+SABORT") == 0)
	{
		if (utlAtCommandResponse(*xid_p, utlAT_COMMAND_RESPONSE_TYPE_ABORT, "abort1") != utlSUCCESS)
			return utlFAILED;

	}
	else if (strcasecmp(command_name_p, "+SERROR") == 0)
	{
		if (utlAtCommandResponse(*xid_p, utlAT_COMMAND_RESPONSE_TYPE_ERROR) != utlSUCCESS)
			return utlFAILED;

	}
	else
		return utlFAILED;

	if (num_parameters > 0) return updateTestResults(command_name_p, parameter_values, num_parameters, info_text_p, arg_p);
	else return updateTestResults(command_name_p, NULL,             num_parameters, info_text_p, arg_p);
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      setFunction(op, command_name_p, parameter_values_p, num_parameters,
*                  info_text_p, xid_p, arg_p)
* INPUT
*                      op == the parameter operation
*          command_name_p == the AT-command expressed as a null-terminated character string
*      parameter_values_p == NULL, or a pointer to a list of parameter-value structures
*          num_parameters == number of parameters pointed to by `parameter_values_p'
*             info_text_p == pointer to info text, or a NULL pointer
*                   xid_p == pointer to a unique transaction ID
*                   arg_p == pointer to user-defined data
* OUTPUT
*      *xid_p == possibly modified transaction ID
* RETURNS
*      utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*      Records set-parameter-value activity.
*---------------------------------------------------------------------------*/
static utlReturnCode_T setFunction(const utlAtParameterOp_T op,
				   const char                    *command_name_p,
				   const utlAtParameterValue_P2c parameter_values_p,
				   const size_t num_parameters,
				   const char                    *info_text_p,
				   unsigned int            *xid_p,
				   void                    *arg_p)
{
	if ((op != utlAT_PARAMETER_OP_SET)  &&
	    (op != utlAT_PARAMETER_OP_EXEC) &&
	    (op != utlAT_PARAMETER_OP_ACTION))
		return utlFAILED;

	if (command_name_p == NULL)
		return utlFAILED;

	if (strcasecmp(command_name_p, "+TMO") == 0)
	{
		return utlSUCCESS;

	}
	if ((strcmp(command_name_p, "+CBACK") == 0) ||
	    (strcmp(command_name_p, "+GMR")   == 0))
	{
		static char new_info_text[400];

		if (info_text_p != NULL)
		{
			strcpy(new_info_text, info_text_p);
			strcat(new_info_text, "\r\n");
		}
		else
			new_info_text[0] = '\0';

		strcat(new_info_text, "Custom response string");

		if (utlAtCommandResponse(*xid_p, utlAT_COMMAND_RESPONSE_TYPE_INFO_TEXT, new_info_text) != utlSUCCESS)
			return utlFAILED;

	}
	else if ((info_text_p != NULL) && (info_text_p[0] != '\0'))
	{
		if (utlAtCommandResponse(*xid_p, utlAT_COMMAND_RESPONSE_TYPE_INFO_TEXT, info_text_p) != utlSUCCESS)
			return utlFAILED;

	}
	else if ((strcasecmp(command_name_p, "+AINFO")  != 0) &&
		 (strcasecmp(command_name_p, "+SABORT") != 0) &&
		 (strcasecmp(command_name_p, "+SERROR") != 0) &&
		 (strcasecmp(command_name_p, "N")       != 0) &&
		 (strcasecmp(command_name_p, "B")       != 0) &&
		 (strcasecmp(command_name_p, "U")       != 0))
	{
		if (utlAtCommandResponse(*xid_p, utlAT_COMMAND_RESPONSE_TYPE_REQUEST_COMPLETED) != utlSUCCESS)
			return utlFAILED;
	}

	strcpy(test_results.commands[test_results.num_commands].name, command_name_p);

	if (parameter_values_p == NULL)
	{
		if (num_parameters != (size_t)0)
			return utlFAILED;
	}
	else
	{
		size_t i;

		if (num_parameters == (size_t)0)
			return utlFAILED;

		for (i = 0; i < num_parameters; i++)
		{
			test_results.commands[test_results.num_commands].parameter_values[i] = parameter_values_p[i];

			if (parameter_values_p[i].type == utlAT_DATA_TYPE_STRING)
			{
				strcpy(test_results.string_buf + test_results.next_unused, parameter_values_p[i].value.string_p);
				test_results.commands[test_results.num_commands].parameter_values[i].value.string_p = test_results.string_buf + test_results.next_unused;
				test_results.next_unused += strlen(parameter_values_p[i].value.string_p) + 1;

			}
			else if (parameter_values_p[i].type == utlAT_DATA_TYPE_QSTRING)
			{
				strcpy(test_results.string_buf + test_results.next_unused, parameter_values_p[i].value.qstring_p);
				test_results.commands[test_results.num_commands].parameter_values[i].value.qstring_p = test_results.string_buf + test_results.next_unused;
				test_results.next_unused += strlen(parameter_values_p[i].value.qstring_p) + 1;

			}
			else if (parameter_values_p[i].type == utlAT_DATA_TYPE_DIAL_STRING)
			{
				strcpy(test_results.string_buf + test_results.next_unused, parameter_values_p[i].value.dial_string_p);
				test_results.commands[test_results.num_commands].parameter_values[i].value.dial_string_p = test_results.string_buf + test_results.next_unused;
				test_results.next_unused += strlen(parameter_values_p[i].value.dial_string_p) + 1;
			}
		}
	}

	test_results.commands[test_results.num_commands].num_parameters = num_parameters;

	test_results.num_commands++;

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      setAsyncFunction(op, command_name_p, parameter_values_p, num_parameters,
*                       info_text_p, xid_p, arg_p)
* INPUT
*                      op == the parameter operation
*          command_name_p == the AT-command expressed as a null-terminated character string
*      parameter_values_p == NULL, or a pointer to a list of parameter-value structures
*          num_parameters == number of parameters pointed to by `parameter_values_p'
*             info_text_p == pointer to info text, or a NULL pointer
*                   xid_p == pointer to a unique transaction ID
*                   arg_p == pointer to user-defined data
* OUTPUT
*      *xid_p == possibly modified transaction ID
* RETURNS
*      utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*      Records set-parameter-value activity.
*---------------------------------------------------------------------------*/
static utlReturnCode_T setAsyncFunction(const utlAtParameterOp_T op,
					const char                    *command_name_p,
					const utlAtParameterValue_P2c parameter_values_p,
					const size_t num_parameters,
					const char                    *info_text_p,
					unsigned int            *xid_p,
					void                    *arg_p)
{
	if ((op != utlAT_PARAMETER_OP_SET)  &&
	    (op != utlAT_PARAMETER_OP_EXEC) &&
	    (op != utlAT_PARAMETER_OP_ACTION))
		return utlFAILED;
	if (command_name_p == NULL)
		return utlFAILED;

	if (strcasecmp(command_name_p, "+TMO") == 0)
	{
		return utlSUCCESS;

	}
	if (strcasecmp(command_name_p, "+AINFO") == 0)
	{
		test_async_xid = *xid_p;

	}
	else if (strcasecmp(command_name_p, "N") == 0)
	{
		test_async_xid = *xid_p;

	}
	else if (strcasecmp(command_name_p, "B") == 0)
	{
		test_async_xid = *xid_p;
		if (utlAtCommandResponse(*xid_p, utlAT_COMMAND_RESPONSE_TYPE_REQUEST_COMPLETED) != utlSUCCESS)
			return utlFAILED;

	}
	else if (strcasecmp(command_name_p, "U") == 0)
	{
		test_async_xid = *xid_p;
		if (utlAtCommandResponse(*xid_p, utlAT_COMMAND_RESPONSE_TYPE_CUSTOM, "MOO") != utlSUCCESS)
			return utlFAILED;

	}
	else if (strcasecmp(command_name_p, "W") == 0)
	{
		if (utlAtCommandResponse(*xid_p, utlAT_COMMAND_RESPONSE_TYPE_ERROR) != utlSUCCESS)
			return utlFAILED;

	}
	else if (strcasecmp(command_name_p, "+SABORT") == 0)
	{
		if (utlAtCommandResponse(*xid_p, utlAT_COMMAND_RESPONSE_TYPE_ABORT, "abort2") != utlSUCCESS)
			return utlFAILED;

	}
	else if (strcasecmp(command_name_p, "+SERROR") == 0)
	{
		if (utlAtCommandResponse(*xid_p, utlAT_COMMAND_RESPONSE_TYPE_ERROR) != utlSUCCESS)
			return utlFAILED;
	}
	else
		test_async_xid = *xid_p;

	return setFunction(op, command_name_p, parameter_values_p, num_parameters, info_text_p, xid_p, arg_p);
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      replyFunction(string_p, arg_p)
* INPUT
*      string_p == the reply text expressed as a null-terminated character string
*         arg_p == pointer to user-defined data
* OUTPUT
*      none
* RETURNS
*      utlSUCCESS
* DESCRIPTION
*      Records DCE reply text.
*---------------------------------------------------------------------------*/
static utlReturnCode_T replyFunction(const char *string_p, void *arg_p)
{
	(void)strcat(test_results.reply_string, string_p);

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      sParameterFunction(parameter_num, parameter_value, arg_p)
* INPUT
*        parameter_num == S-parameter number
*      parameter_value == new S-parameter value
*                arg_p == pointer to user-defined data
* OUTPUT
*      none
* RETURNS
*      utlSUCCESS
* DESCRIPTION
*      Records S-parameter value changes.
*---------------------------------------------------------------------------*/
static utlReturnCode_T sParameterFunction(const unsigned int parameter_num,
					  const unsigned int parameter_value,
					  void         *arg_p)
{
	test_results.s_parameter_num   = parameter_num;
	test_results.s_parameter_value = parameter_value;

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      driverRequestFunction(parser_p, request_type, arg_p, ...)
* INPUT
*          parser_p == pointer to an initialized utlParser_T structure
*      request_type == type of request
*             arg_p == pointer to user-defined data
*               ... == request-specific parameters
* OUTPUT
*      none
* RETURNS
*      utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*      Handles requests issued to the driver.
*---------------------------------------------------------------------------*/
static utlReturnCode_T driverRequestFunction(const utlAtParser_P parser_p,
					     const utlAtDriverRequest_T request_type,
					     void                 *arg_p,
					     ...)
{
	va_list va_arg_p;
	const char                 *digits_p;
	utlAtDialStringOptions_P2c options_p;

	utlAssert(parser_p != NULL);

	va_start(va_arg_p, arg_p);

	switch (test_results.driver_request_test)
	{
	case 0:         /*--- just trace the request... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_TONE_DIAL:
		case utlAT_DRIVER_REQUEST_PULSE_DIAL:
			digits_p  = va_arg(va_arg_p, const char *);
			options_p = va_arg(va_arg_p, const utlAtDialStringOptions_P2c);

			fprintf(stderr, "        Dial digits: %s\n", digits_p);
			fprintf(stderr, "      International: %s\n", (options_p->international       == true) ? "true" : "false");
			fprintf(stderr, "Do call origination: %s\n", (options_p->do_call_origination == true) ? "true" : "false");
			fprintf(stderr, "   Use CCUG SS info: %s\n", (options_p->use_CCUG_SS_info    == true) ? "true" : "false");
			fprintf(stderr, "   CLI presentation: %s\n", (options_p->CLI_presentation    == 'I') ? "restrict" :
				(options_p->CLI_presentation    == 'i') ? "allow" : "default");
			break;

		case utlAT_DRIVER_REQUEST_OFF_HOOK:        fprintf(stderr, "go off-hook\n");                 break;
		case utlAT_DRIVER_REQUEST_ANSWER_OFF_HOOK: fprintf(stderr, "go off-hook for answer\n");      break;
		case utlAT_DRIVER_REQUEST_ON_HOOK:         fprintf(stderr, "go on-hook\n");                  break;
		case utlAT_DRIVER_REQUEST_FLASH_ON_HOOK:   fprintf(stderr, "go on-hook for hook-flash\n");   break;
		case utlAT_DRIVER_REQUEST_FLASH_OFF_HOOK:  fprintf(stderr, "go off-hook for hook-flash\n");  break;
		case utlAT_DRIVER_REQUEST_RESET_ON_HOOK:   fprintf(stderr, "go on-hook for reset\n");        break;

		default:
			break;
		}
		break;

	case 1:         /*--- simple out-going call that is answered... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_TONE_DIAL:
		case utlAT_DRIVER_REQUEST_PULSE_DIAL:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_CONF) != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ANSWERED)  != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_CARRIER)   != utlSUCCESS) return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_FORCE_TIMEOUT);

		case utlAT_DRIVER_REQUEST_ON_HOOK:
		case utlAT_DRIVER_REQUEST_FLASH_ON_HOOK:
		case utlAT_DRIVER_REQUEST_RESET_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		case utlAT_DRIVER_REQUEST_ANSWER_OFF_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF);

		case utlAT_DRIVER_REQUEST_OFF_HOOK:
		case utlAT_DRIVER_REQUEST_FLASH_OFF_HOOK:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF) != utlSUCCESS)
				return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_TONE);

		default:
			break;
		}
		break;

	case 2:         /*--- more complex out-going call that is answered... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_TONE_DIAL:
		case utlAT_DRIVER_REQUEST_PULSE_DIAL:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_CONF) != utlSUCCESS)
				return utlFAILED;
			switch (test_results.driver_request_test_state++)
			{
			case 0:
				return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_FORCE_TIMEOUT);
			case 1:
				break;
			case 2:
				break;
			case 3:
				return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_TONE);
			case 4:
				if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)          != utlSUCCESS) return utlFAILED;
				if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_FORCE_TIMEOUT) != utlSUCCESS) return utlFAILED;
				if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)          != utlSUCCESS) return utlFAILED;
				if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)          != utlSUCCESS) return utlFAILED;
				if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ANSWERED)      != utlSUCCESS) return utlFAILED;
				if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_CARRIER)       != utlSUCCESS) return utlFAILED;
				return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_FORCE_TIMEOUT);
			}
			break;

		case utlAT_DRIVER_REQUEST_FLASH_ON_HOOK:
			if ( utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF) != utlSUCCESS)
				return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_FORCE_TIMEOUT);

		case utlAT_DRIVER_REQUEST_ON_HOOK:
		case utlAT_DRIVER_REQUEST_RESET_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		case utlAT_DRIVER_REQUEST_ANSWER_OFF_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF);

		case utlAT_DRIVER_REQUEST_OFF_HOOK:
		case utlAT_DRIVER_REQUEST_FLASH_OFF_HOOK:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF) != utlSUCCESS)
				return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_TONE);

		default:
			break;
		}
		break;

	case 3:         /*--- incoming call that is answered... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_ANSWER:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ANSWER_CONF) != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_CARRIER)     != utlSUCCESS) return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_FORCE_TIMEOUT);

		case utlAT_DRIVER_REQUEST_ON_HOOK:
		case utlAT_DRIVER_REQUEST_RESET_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;

	case 4:         /*--- simple out-going call that is not answered... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_TONE_DIAL:
		case utlAT_DRIVER_REQUEST_PULSE_DIAL:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_CONF) != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_NO_ANSWER);

		case utlAT_DRIVER_REQUEST_ON_HOOK:
		case utlAT_DRIVER_REQUEST_FLASH_ON_HOOK:
		case utlAT_DRIVER_REQUEST_RESET_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		case utlAT_DRIVER_REQUEST_ANSWER_OFF_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF);

		case utlAT_DRIVER_REQUEST_OFF_HOOK:
		case utlAT_DRIVER_REQUEST_FLASH_OFF_HOOK:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF) != utlSUCCESS)
				return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_TONE);

		default:
			break;
		}
		break;

	case 5:         /*--- simple out-going call that is not answered... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_ON_HOOK:
		case utlAT_DRIVER_REQUEST_FLASH_ON_HOOK:
		case utlAT_DRIVER_REQUEST_RESET_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		case utlAT_DRIVER_REQUEST_ANSWER_OFF_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF);

		case utlAT_DRIVER_REQUEST_OFF_HOOK:
		case utlAT_DRIVER_REQUEST_FLASH_OFF_HOOK:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF) != utlSUCCESS)
				return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_NO_DIAL_TONE);

		default:
			break;
		}
		break;

	case 6:         /*--- simple out-going call that is busy... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_TONE_DIAL:
		case utlAT_DRIVER_REQUEST_PULSE_DIAL:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_CONF) != utlSUCCESS)
				return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_BUSY);

		case utlAT_DRIVER_REQUEST_ON_HOOK:
		case utlAT_DRIVER_REQUEST_FLASH_ON_HOOK:
		case utlAT_DRIVER_REQUEST_RESET_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		case utlAT_DRIVER_REQUEST_ANSWER_OFF_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF);

		case utlAT_DRIVER_REQUEST_OFF_HOOK:
		case utlAT_DRIVER_REQUEST_FLASH_OFF_HOOK:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF) != utlSUCCESS)
				return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_TONE);

		default:
			break;
		}
		break;

	case 7:         /*--- simple out-going call that fails to detect a carrier... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_TONE_DIAL:
		case utlAT_DRIVER_REQUEST_PULSE_DIAL:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_CONF) != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ANSWERED)  != utlSUCCESS) return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_NO_CARRIER);

		case utlAT_DRIVER_REQUEST_ON_HOOK:
		case utlAT_DRIVER_REQUEST_FLASH_ON_HOOK:
		case utlAT_DRIVER_REQUEST_RESET_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		case utlAT_DRIVER_REQUEST_ANSWER_OFF_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF);

		case utlAT_DRIVER_REQUEST_OFF_HOOK:
		case utlAT_DRIVER_REQUEST_FLASH_OFF_HOOK:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF) != utlSUCCESS)
				return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_TONE);

		default:
			break;
		}
		break;

	case 8:         /*--- simple answered call that is we abandon during answer... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;

	case 9:         /*--- manually answered call that fails during off-hook (synchronous)... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_ANSWER_OFF_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_REQUEST_FAILED);

		case utlAT_DRIVER_REQUEST_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;

	case 10:         /*--- manually answered call that fails during off-hook (async)... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_ANSWER_OFF_HOOK:
			return utlSUCCESS;

		case utlAT_DRIVER_REQUEST_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;

	case 11:         /*--- manually answered call that fails during answer (synchronous)... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_ANSWER_OFF_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF);

		case utlAT_DRIVER_REQUEST_ANSWER:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_REQUEST_FAILED);

		case utlAT_DRIVER_REQUEST_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;

	case 12:         /*--- manually answered call that fails during answer (asynchronous)... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_ANSWER_OFF_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF);

		case utlAT_DRIVER_REQUEST_ANSWER:
			return utlSUCCESS;

		case utlAT_DRIVER_REQUEST_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;

	case 13:         /*--- manually answered call that fails during carrier detect (synchronous)... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_ANSWER_OFF_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF);

		case utlAT_DRIVER_REQUEST_ANSWER:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ANSWER_CONF) != utlSUCCESS)
				return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_REQUEST_FAILED);

		case utlAT_DRIVER_REQUEST_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;

	case 14:         /*--- manually answered call that fails during carrier detect (asynchronous)... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_ANSWER_OFF_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF);

		case utlAT_DRIVER_REQUEST_ANSWER:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ANSWER_CONF) != utlSUCCESS)
				return utlFAILED;
			return utlSUCCESS;

		case utlAT_DRIVER_REQUEST_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;

	case 15:         /*--- manually answered call that fails during carrier confirm (synchronous)... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_ANSWER_OFF_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF);

		case utlAT_DRIVER_REQUEST_ANSWER:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ANSWER_CONF) != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_CARRIER)     != utlSUCCESS) return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_REQUEST_FAILED);

		case utlAT_DRIVER_REQUEST_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;

	case 16:         /*--- manually answered call that fails during carrier confirm (asynchronous)... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_ANSWER_OFF_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF);

		case utlAT_DRIVER_REQUEST_ANSWER:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ANSWER_CONF) != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_CARRIER)     != utlSUCCESS) return utlFAILED;
			return utlSUCCESS;

		case utlAT_DRIVER_REQUEST_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;

	case 17:         /*--- out-going call that fails during off-hook (synchronous)... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_OFF_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_REQUEST_FAILED);

		case utlAT_DRIVER_REQUEST_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;

	case 18:         /*--- out-going call that fails during off-hook (asynchronous)... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_OFF_HOOK:
			return utlSUCCESS;

		case utlAT_DRIVER_REQUEST_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;

	case 19:         /*--- out-going call that fails during wait for dial-tone (synchronous)... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_OFF_HOOK:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF) != utlSUCCESS)
				return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_REQUEST_FAILED);

		case utlAT_DRIVER_REQUEST_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;

	case 20:         /*--- out-going call that fails during wait for dial-tone (asynchronous)... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_OFF_HOOK:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF) != utlSUCCESS)
				return utlFAILED;
			return utlSUCCESS;

		case utlAT_DRIVER_REQUEST_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;

	case 21:         /*--- out-going call that fails during dialing (synchronous)... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_OFF_HOOK:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF) != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_TONE)     != utlSUCCESS) return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_REQUEST_FAILED);

		case utlAT_DRIVER_REQUEST_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;

	case 22:         /*--- out-going call that fails during dialing (asynchronous)... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_OFF_HOOK:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF) != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_TONE)     != utlSUCCESS) return utlFAILED;
			return utlSUCCESS;

		case utlAT_DRIVER_REQUEST_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;

	case 23:         /*--- out-going call that fails during ring-back wait (synchronous)... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_OFF_HOOK:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF) != utlSUCCESS)
				return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_TONE);

		case utlAT_DRIVER_REQUEST_TONE_DIAL:
		case utlAT_DRIVER_REQUEST_PULSE_DIAL:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_CONF) != utlSUCCESS) return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_REQUEST_FAILED);

		case utlAT_DRIVER_REQUEST_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;

	case 24:         /*--- out-going call that fails during ring-back wait (asynchronous)... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_OFF_HOOK:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF) != utlSUCCESS)
				return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_TONE);

		case utlAT_DRIVER_REQUEST_TONE_DIAL:
		case utlAT_DRIVER_REQUEST_PULSE_DIAL:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_CONF) != utlSUCCESS) return utlFAILED;
			return utlSUCCESS;

		case utlAT_DRIVER_REQUEST_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;

	case 25:         /*--- out-going call that fails during answer wait (synchronous)... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_OFF_HOOK:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF) != utlSUCCESS)
				return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_TONE);

		case utlAT_DRIVER_REQUEST_TONE_DIAL:
		case utlAT_DRIVER_REQUEST_PULSE_DIAL:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_CONF) != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_REQUEST_FAILED);

		case utlAT_DRIVER_REQUEST_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;

	case 26:         /*--- out-going call that fails during answer wait (asynchronous)... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_OFF_HOOK:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF) != utlSUCCESS)
				return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_TONE);

		case utlAT_DRIVER_REQUEST_TONE_DIAL:
		case utlAT_DRIVER_REQUEST_PULSE_DIAL:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_CONF) != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
			return utlSUCCESS;

		case utlAT_DRIVER_REQUEST_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;

	case 27:         /*--- out-going call that fails during carrier wait (synchronous)... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_OFF_HOOK:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF) != utlSUCCESS)
				return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_TONE);

		case utlAT_DRIVER_REQUEST_TONE_DIAL:
		case utlAT_DRIVER_REQUEST_PULSE_DIAL:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_CONF) != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ANSWERED)  != utlSUCCESS) return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_REQUEST_FAILED);

		case utlAT_DRIVER_REQUEST_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;

	case 28:         /*--- out-going call that fails during carrier wait (asynchronous)... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_OFF_HOOK:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF) != utlSUCCESS)
				return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_TONE);

		case utlAT_DRIVER_REQUEST_TONE_DIAL:
		case utlAT_DRIVER_REQUEST_PULSE_DIAL:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_CONF) != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ANSWERED)  != utlSUCCESS) return utlFAILED;
			return utlSUCCESS;

		case utlAT_DRIVER_REQUEST_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;

	case 29:         /*--- out-going call that fails during carrier confirm (synchronous)... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_OFF_HOOK:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF) != utlSUCCESS)
				return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_TONE);

		case utlAT_DRIVER_REQUEST_TONE_DIAL:
		case utlAT_DRIVER_REQUEST_PULSE_DIAL:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_CONF) != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ANSWERED)  != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_CARRIER)   != utlSUCCESS) return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_REQUEST_FAILED);

		case utlAT_DRIVER_REQUEST_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;

	case 30:         /*--- out-going call that fails during carrier confirm (asynchronous)... ---*/
		switch (request_type)
		{
		case utlAT_DRIVER_REQUEST_OFF_HOOK:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF) != utlSUCCESS)
				return utlFAILED;
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_TONE);

		case utlAT_DRIVER_REQUEST_TONE_DIAL:
		case utlAT_DRIVER_REQUEST_PULSE_DIAL:
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_CONF) != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ANSWERED)  != utlSUCCESS) return utlFAILED;
			if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_CARRIER)   != utlSUCCESS) return utlFAILED;
			return utlSUCCESS;

		case utlAT_DRIVER_REQUEST_ON_HOOK:
			return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

		default:
			break;
		}
		break;
	}

	va_end(va_arg_p);

	return utlSUCCESS;
}
/*---------------------------------------------------------------------------*/

static utlAtParameter_T j_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T l_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T amp_z_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DIAL_STRING) };

static utlAtParameter_T dec_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };
static utlAtParameter_T hex_params[] = { utlDEFINE_HEXADECIMALL_AT_PARAMETER(utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };
static utlAtParameter_T bin_params[] = { utlDEFINE_BINARY_AT_PARAMETER(      utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };
static utlAtParameter_T str_params[] = { utlDEFINE_STRING_AT_PARAMETER(      utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };
static utlAtParameter_T qstr_params[] = { utlDEFINE_QSTRING_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };
static utlAtParameter_T dial_params[] = { utlDEFINE_DIAL_STRING_AT_PARAMETER( utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };
static utlAtParameter_T mix_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					 utlDEFINE_HEXADECIMALL_AT_PARAMETER(utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					 utlDEFINE_BINARY_AT_PARAMETER(      utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					 utlDEFINE_STRING_AT_PARAMETER(      utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					 utlDEFINE_QSTRING_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					 utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };
static utlAtParameter_T act_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };

static utlAtParameter_T one_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };
static utlAtParameter_T two_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					 utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };
static utlAtParameter_T three_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					   utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					   utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };

static utlAtParameter_T opt_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL) };
static utlAtParameter_T req_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };
static utlAtParameter_T alt_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					 utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),
					 utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					 utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL) };
static utlAtParameter_T opts_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),
					  utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),
					  utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL) };

static utlAtParameter_T rw_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };
static utlAtParameter_T ro_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_ONLY, utlAT_PARAMETER_PRESENCE_REQUIRED),
					utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_ONLY, utlAT_PARAMETER_PRESENCE_REQUIRED) };
static utlAtParameter_T r_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ, utlAT_PARAMETER_PRESENCE_REQUIRED),
				       utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ, utlAT_PARAMETER_PRESENCE_REQUIRED) };
static utlAtParameter_T wo_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_WRITE_ONLY, utlAT_PARAMETER_PRESENCE_REQUIRED),
					utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_WRITE_ONLY, utlAT_PARAMETER_PRESENCE_REQUIRED) };
static utlAtParameter_T rwro_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_ONLY,	 utlAT_PARAMETER_PRESENCE_REQUIRED),
					  utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					  utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_ONLY,	 utlAT_PARAMETER_PRESENCE_REQUIRED),
					  utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };

static utlAtParameter_T syn_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					 utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					 utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };

static utlAtParameter_T evsyn_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					   utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					   utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };

static utlAtParameter_T avsyn_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					   utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					   utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };

static utlAtParameter_T plusTM_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };

static utlAtParameter_T cback_params[] = { utlDEFINE_HEXADECIMALL_AT_PARAMETER(utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					   utlDEFINE_HEXADECIMALL_AT_PARAMETER(utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };

static utlAtParameter_T ipr_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };

static utlAtParameter_T asto_params[] = { utlDEFINE_STRING_AT_PARAMETER(      utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					  utlDEFINE_DIAL_STRING_AT_PARAMETER( utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };

static utlAtParameter_T async_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					   utlDEFINE_HEXADECIMALL_AT_PARAMETER(utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					   utlDEFINE_BINARY_AT_PARAMETER(      utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					   utlDEFINE_STRING_AT_PARAMETER(      utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					   utlDEFINE_QSTRING_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					   utlDEFINE_DECIMAL_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };

static utlAtParameter_T n_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T b_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T u_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T w_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };

static utlAtParameter_T a_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T d_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DIAL_STRING) };
static utlAtParameter_T e_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T h_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T i_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T q_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T v_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T x_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };

static utlAtCommand_T commands[] = { utlDEFINE_BASIC_AT_COMMAND("J",			j_params,	    setFunction),
				     utlDEFINE_BASIC_AT_COMMAND("K",			NULL,		    setFunction),
				     utlDEFINE_BASIC_AT_COMMAND("&L",			l_params,	    setFunction),
				     utlDEFINE_BASIC_AT_COMMAND("&M",			NULL,		    setFunction),
				     utlDEFINE_BASIC_AT_COMMAND("&Z",			amp_z_params,	    setFunction),

				     utlDEFINE_EXTENDED_AT_COMMAND("+DEC",		dec_params,	    NULL,			 getFunction,			  setFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+HEX",		hex_params,	    NULL,			 getFunction,			  setFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+BIN",		bin_params,	    NULL,			 getFunction,			  setFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+STRING",		str_params,	    NULL,			 getFunction,			  setFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+QSTRING",		qstr_params,	    NULL,			 getFunction,			  setFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+DIAL",		dial_params,	    NULL,			 getFunction,			  setFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+MIX",		mix_params,	    NULL,			 getFunction,			  setFunction),
				     utlDEFINE_EXACTION_AT_COMMAND("+ACT",		act_params,	    NULL,			 setFunction),

				     utlDEFINE_EXTENDED_AT_COMMAND("+ONE",		one_params,	    NULL,			 getFunction,			  setFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+TWO",		two_params,	    NULL,			 getFunction,			  setFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+THREE",		three_params,	    NULL,			 getFunction,			  setFunction),

				     utlDEFINE_EXTENDED_AT_COMMAND("+OPT",		opt_params,	    NULL,			 getFunction,			  setFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+REQ",		req_params,	    NULL,			 getFunction,			  setFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+ALT",		alt_params,	    NULL,			 getFunction,			  setFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+OPTS",		opts_params,	    NULL,			 getFunction,			  setFunction),

				     utlDEFINE_EXTENDED_AT_COMMAND("+RW",		rw_params,	    NULL,			 getFunction,			  setFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+RO",		ro_params,	    NULL,			 getFunction,			  setFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+WO",		wo_params,	    NULL,			 getFunction,			  setFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND( "+R",		r_params,	    NULL,			 getFunction,			  setFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+RWRO",		rwro_params,	    NULL,			 getFunction,			  setFunction),

				     utlDEFINE_EXTENDED_AT_COMMAND("+GSYN",		syn_params,	    "+GSYN=(0-6),(1-7),(2-9,11)",getFunction,			  setFunction),

				     utlDEFINE_EXTENDED_VSYNTAX_AT_COMMAND("+EVSYN",	evsyn_params,	    syntaxFunction,		 getFunction,			  setFunction),
				     utlDEFINE_EXACTION_VSYNTAX_AT_COMMAND("+AVSYN",	avsyn_params,	    syntaxFunction,		 setFunction),
				     utlDEFINE_EXTENDED_VSYNTAX_AT_COMMAND("+EVSYNA",	evsyn_params,	    syntaxAsyncFunction,	 getFunction,			  setFunction),
				     utlDEFINE_EXACTION_VSYNTAX_AT_COMMAND("+AVSYNA",	avsyn_params,	    syntaxAsyncFunction,	 setFunction),
				     utlDEFINE_EXTENDED_VSYNTAX_AT_COMMAND("+EVSYNAS",	evsyn_params,	    syntaxAsyncFunction,	 getFunction,			  setFunction),
				     utlDEFINE_EXACTION_VSYNTAX_AT_COMMAND("+AVSYNAS",	avsyn_params,	    syntaxAsyncFunction,	 setFunction),

				     utlDEFINE_EXTENDED_AT_COMMAND("+TMO",		plusTM_params,	    "+TMO: (0-9)",		 getFunction,			  setFunction),

				     utlDEFINE_EXTENDED_AT_COMMAND("+CBACK",		cback_params,	    NULL,			 getFunction,			  setFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+GMR",		NULL,		    NULL,			 getFunction,			  setFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+S234567890123456", NULL,		    "+S",			 getFunction,			  setFunction),

				     utlDEFINE_EXACTION_AT_COMMAND("+GMI",		NULL,		    NULL,			 setFunction),
				     utlDEFINE_EXACTION_AT_COMMAND("+GCAP",		NULL,		    NULL,			 setFunction),
				     utlDEFINE_EXACTION_AT_COMMAND("+GCI",		NULL,		    NULL,			 setFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+MV18S",		NULL,		    "ab",			 getFunction,			  setFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+IPR",		ipr_params,	    NULL,			 getFunction,			  setFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+ASTO",		asto_params,	    NULL,			 getFunction,			  setFunction),

				     utlDEFINE_EXTENDED_AT_COMMAND("+AINFO",		async_params,	    NULL,			 getAsyncFunction,		  setAsyncFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+APARAM",		async_params,	    NULL,			 getAsyncFunction,		  setAsyncFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+SINFO",		async_params,	    NULL,			 getAsyncFunction,		  setAsyncFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+SPARAM",		async_params,	    NULL,			 getAsyncFunction,		  setAsyncFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+SABORT",		async_params,	    NULL,			 getAsyncFunction,		  setAsyncFunction),
				     utlDEFINE_EXTENDED_AT_COMMAND("+SERROR",		async_params,	    NULL,			 getAsyncFunction,		  setAsyncFunction),

				     utlDEFINE_BASIC_AT_COMMAND("N",			n_params,	    setAsyncFunction),
				     utlDEFINE_BASIC_AT_COMMAND("B",			b_params,	    setAsyncFunction),
				     utlDEFINE_BASIC_AT_COMMAND("U",			u_params,	    setAsyncFunction),
				     utlDEFINE_BASIC_AT_COMMAND("W",			w_params,	    setAsyncFunction),

				     utlDEFINE_BASIC_AT_COMMAND("A",			a_params,	    setFunction),
				     utlDEFINE_BASIC_AT_COMMAND("D",			d_params,	    setFunction),
				     utlDEFINE_BASIC_AT_COMMAND("E",			e_params,	    setFunction),
				     utlDEFINE_BASIC_AT_COMMAND("H",			h_params,	    setFunction),
				     utlDEFINE_BASIC_AT_COMMAND("I",			i_params,	    setFunction),
				     utlDEFINE_BASIC_AT_COMMAND("Q",			q_params,	    setFunction),
				     utlDEFINE_BASIC_AT_COMMAND("V",			v_params,	    setFunction),
				     utlDEFINE_BASIC_AT_COMMAND("X",			x_params,	    setFunction) };


/*---------------------------------------------------------------------------*
* FUNCTION
*      atParserTest()
* INPUT
*      none
* OUTPUT
*      none
* RETURNS
*      "true" for pass, "false" for failure
*---------------------------------------------------------------------------*/
bool atParserTest(void)
{
	utlAtParser_P parser_p;
	char                     *s_p;
	size_t i, j;
	unsigned int parameter_value;
	char buf[utlAT_MAX_LINE_LENGTH + 100];
	utlStateMachineStateId_T state_ids[50];

#define CLEAR_TEST_RESULTS() {	       \
		test_results.num_commands    =   0; \
		test_results.reply_string[0] = '\0'; \
		test_results.next_unused     =   0;   \
}

	/*--- setup for tests ---------------------------------------------------*/
	if ((parser_p = utlOpenAtParser(commands, utlNumberOf(commands), NULL)) == NULL)
	{
		fprintf(stderr, "atParserTest: utlOpenAtParser(0.1.0) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_REPLY_HANDLER, replyFunction) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParserOp(0.1.1) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_S_PARAMETER_HANDLER, sParameterFunction) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParserOp(0.1.2) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_DRIVER_REQUEST_HANDLER, driverRequestFunction) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParserOp(0.1.3) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_ECHO_ON) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParserOp(0.1.4) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_ID, "Intel") != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParserOp(0.1.5) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_MANUFACTURER, "Intel") != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParserOp(0.1.6) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_MODEL, "Linux") != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParserOp(0.1.7) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_REVISION, "1.0") != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParserOp(0.1.8) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_SERIAL_NUMBER, "0") != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParserOp(0.1.9) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_OBJECT_ID, "OBJ ID") != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParserOp(0.1.10) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_COUNTRY_CODE, "(20,3C,3D,42,50,58,00,61,FE,A9,B4,B5)") != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParserOp(0.1.11) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_COUNTRY_CODE, "20") != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParserOp(0.1.12) failed\n");
		return false;
	}

	test_results.driver_request_test = 0;


/*--- test s-parameters -------------------------------------------------*/

/*--- set ---*/
	test_results.s_parameter_num   =   0;
	test_results.s_parameter_value =   0;
	test_results.reply_string[0]   = '\0';

	CLEAR_TEST_RESULTS();
	s_p = "ATS30=99\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(1.1) failed\n");
		return false;
	}
	if ((parser_p->parameters.S[30]     != 99) ||
	    (test_results.s_parameter_num   != 30) ||
	    (test_results.s_parameter_value != 99) ||
	    (strcmp(test_results.reply_string, "ATS30=99\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(1.2) failed\n");
		return false;
	}

/*--- set, no value specified: use default value ---*/
	test_results.s_parameter_num   =   0;
	test_results.s_parameter_value =   0;
	test_results.reply_string[0]   = '\0';

	CLEAR_TEST_RESULTS();
	s_p = "ATS30=\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(1.3) failed\n");
		return false;
	}
	if ((parser_p->parameters.S[30]     !=  0) ||
	    (test_results.s_parameter_num   != 30) ||
	    (test_results.s_parameter_value !=  0) ||
	    (strcmp(test_results.reply_string, "ATS30=\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(1.4) failed\n");
		return false;
	}

/*--- set, no value specified: generate error ---*/
	parser_p->options.allow_default_S_parameter_values = false;

	test_results.s_parameter_num   =   0;
	test_results.s_parameter_value =   0;
	test_results.reply_string[0]   = '\0';

	s_p = "ATS40=\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(1.5) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "ATS40=\r\r\nERROR\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(1.6) failed\n");
		return false;
	}

	parser_p->options.allow_default_S_parameter_values = true;

/*--- get ---*/
	test_results.s_parameter_num   =   0;
	test_results.s_parameter_value =   0;
	test_results.reply_string[0]   = '\0';

	parser_p->parameters.S[41] = 7;

	CLEAR_TEST_RESULTS();
	s_p = "ATS41?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(1.7) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "ATS41?\r\r\n007\r\n\r\nOK\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(1.8) failed\n");
		return false;
	}

/*--- set/get really big value ---*/
	test_results.s_parameter_num   =   0;
	test_results.s_parameter_value =   0;
	test_results.reply_string[0]   = '\0';

	CLEAR_TEST_RESULTS();
	s_p = "ATS42=32765\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(1.9) failed\n");
		return false;
	}
	if ((parser_p->parameters.S[42]     != 32765) ||
	    (test_results.s_parameter_num   != 42)    ||
	    (test_results.s_parameter_value != 32765) ||
	    (strcmp(test_results.reply_string, "ATS42=32765\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(1.10) failed\n");
		return false;
	}

	test_results.s_parameter_num   =   0;
	test_results.s_parameter_value =   0;
	test_results.reply_string[0]   = '\0';

	CLEAR_TEST_RESULTS();
	s_p = "ATS42?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(1.11) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "ATS42?\r\r\n32765\r\n\r\nOK\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(1.12) failed\n");
		return false;
	}

/*--- multiples ---*/
	test_results.s_parameter_num   =   0;
	test_results.s_parameter_value =   0;
	test_results.reply_string[0]   = '\0';

	parser_p->parameters.S[47] = 7;

	CLEAR_TEST_RESULTS();
	s_p = "ATS45=5S46=6S47?S48=8\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(1.13) failed\n");
		return false;
	}
	if ((parser_p->parameters.S[45] != 5) ||
	    (parser_p->parameters.S[46] != 6) ||
	    (parser_p->parameters.S[47] != 7) ||
	    (parser_p->parameters.S[48] != 8) ||
	    (strcmp(test_results.reply_string, "ATS45=5S46=6S47?S48=8\r\r\n007\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(1.14) failed\n");
		return false;
	}

/*--- errors ---*/
	s_p = "ATSabc\r"; /* 'S' not followed by digit */
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(1.15) failed\n");
		return false;
	}
	s_p = "ATS10\r"; /* 'S' incomplete */
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(1.16) failed\n");
		return false;
	}
	s_p = "ATS10mm\r"; /* truncated */
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(1.17) failed\n");
		return false;
	}
	s_p = "ATS60?\r"; /* 'S' number too big */
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(1.18) failed\n");
		return false;
	}


/*--- test basic AT commands --------------------------------------------*/

/*--- set "J" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "ATJ18\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.1) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "J")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 18)                                ||
	    (strcmp(test_results.reply_string, "ATJ18\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.2) failed\n");
		return false;
	}

/*--- set "J" to default ---*/
	CLEAR_TEST_RESULTS();
	s_p = "atj\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.3) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "j")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "atj\r\r\nOK\r\n") != 0))
	{

		fprintf(stderr, "atParserTest: utlAtParse(2.4) failed\n");
		return false;
	}

/*--- set "K" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "ATK\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.5) failed\n");
		return false;
	}
	if ((test_results.num_commands                  != 1) ||
	    (strcmp(test_results.commands[0].name, "K") != 0) ||
	    (test_results.commands[0].num_parameters    != 0) ||
	    (strcmp(test_results.reply_string, "ATK\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.6) failed\n");
		return false;
	}

/*--- set "&L" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT&L19\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.7) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "&L")                != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 19)                                ||
	    (strcmp(test_results.reply_string, "AT&L19\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.8) failed\n");
		return false;
	}

/*--- set "&L" to default ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT&L\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.9) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "&L")                != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "AT&L\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.10) failed\n");
		return false;
	}

/*--- set "&M" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT&M\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.11) failed\n");
		return false;
	}
	if ((test_results.num_commands                   != 1) ||
	    (strcmp(test_results.commands[0].name, "&M") != 0) ||
	    (test_results.commands[0].num_parameters     != 0) ||
	    (strcmp(test_results.reply_string, "AT&M\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.12) failed\n");
		return false;
	}

/*--- multiple basic AT-Commands ---*/
	CLEAR_TEST_RESULTS();
	s_p = "ATJ66K&L33&M\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.11) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 4)                                 ||
	    (strcmp(test_results.commands[0].name, "J")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 66)                                ||
	    (strcmp(test_results.commands[1].name, "K")                 != 0)                                 ||
	    (test_results.commands[1].num_parameters                    != 0)                                 ||
	    (strcmp(test_results.commands[2].name, "&L")                != 0)                                 ||
	    (test_results.commands[2].num_parameters                    != 1)                                 ||
	    (test_results.commands[2].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[2].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[2].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[2].parameter_values[0].value.decimal != 33)                                ||
	    (strcmp(test_results.commands[3].name, "&M")                != 0)                                 ||
	    (test_results.commands[3].num_parameters                    != 0)                                 ||
	    (strcmp(test_results.reply_string, "ATJ66K&L33&M\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.12) failed\n");
		return false;
	}

/*--- set "&Z" (with explicit location) ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT&Z1=555-1212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.13) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "&Z")                != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[0].value.dial_string_p, "5551212") != 0)        ||
	    (strcmp(test_results.reply_string, "AT&Z1=555-1212\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.14) failed\n");
		return false;
	}

/*--- set "&Z" (with implicit location) ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT&Z=555-1213\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.15) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "&Z")                != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[0].value.dial_string_p, "5551213") != 0)        ||
	    (strcmp(test_results.reply_string, "AT&Z=555-1213\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.16) failed\n");
		return false;
	}

/*--- get "&Z" (with explicit location) ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT&Z1?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.17) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "&Z")                != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[0].value.dial_string_p, "5551212") != 0)        ||
	    (strcmp(test_results.reply_string, "AT&Z1?\r\r\n5551212\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.18) failed\n");
		return false;
	}

/*--- get "&Z" (with implicit location) ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT&Z?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.19) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "&Z")                != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[0].value.dial_string_p, "5551213") != 0)        ||
	    (strcmp(test_results.reply_string, "AT&Z?\r\r\n5551213\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.20) failed\n");
		return false;
	}

/*--- error: command that does not support a parameter is provided with one ---*/
	CLEAR_TEST_RESULTS();
	s_p = "ATK99999\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.21) failed\n");
		return false;
	}
	if ((test_results.num_commands != 0) ||
	    (strcmp(test_results.reply_string, "ATK99999\r\r\nERROR\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.22) failed\n");
		return false;
	}

/*--- error: unknown basic AT-Command ---*/
	CLEAR_TEST_RESULTS();
	s_p = "ATY4\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.23) failed\n");
		return false;
	}
	if ((test_results.num_commands != 0) ||
	    (strcmp(test_results.reply_string, "ATY4\r\r\nERROR\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(2.24) failed\n");
		return false;
	}


/*--- repeat last command -----------------------------------------------*/

	CLEAR_TEST_RESULTS();
	s_p = "ATJ18\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(3.1) failed\n");
		return false;
	}
	s_p = "A/";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(3.2) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 2)                                 ||
	    (strcmp(test_results.commands[0].name, "J")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 18)                                ||
	    (strcmp(test_results.commands[1].name, "J")                 != 0)                                 ||
	    (test_results.commands[1].num_parameters                    != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[1].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[1].parameter_values[0].value.decimal != 18)                                ||
	    (strcmp(test_results.reply_string, "ATJ18\r\r\nOK\r\nA/\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(3.3) failed\n");
		return false;
	}


/*--- test extended AT commands --------------------------------------------*/

/*--- set "+DEC" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+DEC=44\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.1) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+DEC")              != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 44)                                ||
	    (strcmp(test_results.reply_string, "AT+DEC=44\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.2) failed\n");
		return false;
	}

/*--- set "+DEC" (with leading zeros) ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+DEC=0044\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.3) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+DEC")              != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 44)                                ||
	    (strcmp(test_results.reply_string, "AT+DEC=0044\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.4) failed\n");
		return false;
	}

/*--- set "+DEC" to zero ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+DEC=0\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.5) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+DEC")              != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "AT+DEC=0\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.6) failed\n");
		return false;
	}

/*--- set "+HEX" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+HEX=4c\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.7) failed\n");
		return false;
	}
	if ((test_results.num_commands                                      != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+HEX")                  != 0)                                 ||
	    (test_results.commands[0].num_parameters                        != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type              != utlAT_DATA_TYPE_HEXADECIMAL)       ||
	    (test_results.commands[0].parameter_values[0].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.hexadecimal != 0x4c)                              ||
	    (strcmp(test_results.reply_string, "AT+HEX=4c\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.8) failed\n");
		return false;
	}

/*--- set "+HEX" (with leading zeros) ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+HEX=004c\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.9) failed\n");
		return false;
	}
	if ((test_results.num_commands                                      != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+HEX")                  != 0)                                 ||
	    (test_results.commands[0].num_parameters                        != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type              != utlAT_DATA_TYPE_HEXADECIMAL)       ||
	    (test_results.commands[0].parameter_values[0].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.hexadecimal != 0x4c)                              ||
	    (strcmp(test_results.reply_string, "AT+HEX=004c\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.10) failed\n");
		return false;
	}

/*--- set "+HEX" to zero ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+HEX=0\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.11) failed\n");
		return false;
	}
	if ((test_results.num_commands                                      != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+HEX")                  != 0)                                 ||
	    (test_results.commands[0].num_parameters                        != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type              != utlAT_DATA_TYPE_HEXADECIMAL)       ||
	    (test_results.commands[0].parameter_values[0].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.hexadecimal != 0x0)                               ||
	    (strcmp(test_results.reply_string, "AT+HEX=0\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.12) failed\n");
		return false;
	}

/*--- set "+BIN" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+BIN=10100101\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.13) failed\n");
		return false;
	}
	if ((test_results.num_commands                                 != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+BIN")             != 0)                                 ||
	    (test_results.commands[0].num_parameters                   != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type         != utlAT_DATA_TYPE_BINARY)            ||
	    (test_results.commands[0].parameter_values[0].access       != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default   != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.binary != 0xa5)                              ||
	    (strcmp(test_results.reply_string, "AT+BIN=10100101\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.14) failed\n");
		return false;
	}

/*--- set "+BIN" (with leading zeros) ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+BIN=0010100101\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.15) failed\n");
		return false;
	}
	if ((test_results.num_commands                                 != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+BIN")             != 0)                                 ||
	    (test_results.commands[0].num_parameters                   != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type         != utlAT_DATA_TYPE_BINARY)            ||
	    (test_results.commands[0].parameter_values[0].access       != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default   != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.binary != 0xa5)                              ||
	    (strcmp(test_results.reply_string, "AT+BIN=0010100101\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.16) failed\n");
		return false;
	}

/*--- set "+BIN" to zero ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+BIN=0\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.17) failed\n");
		return false;
	}
	if ((test_results.num_commands                                 != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+BIN")             != 0)                                 ||
	    (test_results.commands[0].num_parameters                   != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type         != utlAT_DATA_TYPE_BINARY)            ||
	    (test_results.commands[0].parameter_values[0].access       != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default   != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.binary != 0x0)                               ||
	    (strcmp(test_results.reply_string, "AT+BIN=0\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.18) failed\n");
		return false;
	}

/*--- set "+STRING" (with escape sequences disabled) ---*/
	parser_p->options.allow_string_escapes = false;

	CLEAR_TEST_RESULTS();
	s_p = "AT+STRING=\"The quick \\62\\72\\6f\\77\\6e fox\"\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.19) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+STRING")           != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_STRING)            ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[0].value.string_p, "The quick \\62\\72\\6f\\77\\6e fox") != 0) ||
	    (strcmp(test_results.reply_string, "AT+STRING=\"The quick \\62\\72\\6f\\77\\6e fox\"\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.20) failed\n");
		return false;
	}

/*--- set "+STRING" (with escape sequences enabled and embedded <cr>, '\', and '"') ---*/
	parser_p->options.allow_string_escapes = true;

	CLEAR_TEST_RESULTS();
	s_p = "AT+STRING=\"The quick \\62\\72\\6f\\77\\6e fox\\0d\\5c\\22\"\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.21) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+STRING")           != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_STRING)            ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[0].value.string_p, "The quick brown fox\x0d\\\"") != 0) ||
	    (strcmp(test_results.reply_string, "AT+STRING=\"The quick \\62\\72\\6f\\77\\6e fox\\0d\\5c\\22\"\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.22) failed\n");
		return false;
	}

/*--- set "+STRING" (with null string) ---*/
	parser_p->options.allow_string_escapes = true;

	CLEAR_TEST_RESULTS();
	s_p = "AT+STRING=\"\"\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.23) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+STRING")           != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_STRING)            ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[0].value.string_p, "") != 0) ||
	    (strcmp(test_results.reply_string, "AT+STRING=\"\"\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.24) failed\n");
		return false;
	}

/*--- set "+STRING" (omit quotes, line terminator) ---*/
	parser_p->options.allow_string_escapes = true;

	CLEAR_TEST_RESULTS();
	s_p = "AT+STRING=987\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.25) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+STRING")           != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_STRING)            ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[0].value.string_p, "987") != 0) ||
	    (strcmp(test_results.reply_string, "AT+STRING=987\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.26) failed\n");
		return false;
	}

/*--- set "+STRING" (omit quotes, semi-colon terminator, trailing space character) ---*/
	parser_p->options.allow_string_escapes = true;

	CLEAR_TEST_RESULTS();
	s_p = "AT+STRING=987 ;\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.27) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+STRING")           != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_STRING)            ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[0].value.string_p, "987") != 0) ||
	    (strcmp(test_results.reply_string, "AT+STRING=987 ;\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.28) failed\n");
		return false;
	}

/*--- set "+QSTRING" (with escape sequences disabled) ---*/
	parser_p->options.allow_string_escapes = false;

	CLEAR_TEST_RESULTS();
	s_p = "AT+QSTRING=\"The quick \\62\\72\\6f\\77\\6e fox\"\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.29) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+QSTRING")          != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_QSTRING)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[0].value.string_p, "The quick \\62\\72\\6f\\77\\6e fox") != 0) ||
	    (strcmp(test_results.reply_string, "AT+QSTRING=\"The quick \\62\\72\\6f\\77\\6e fox\"\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.30) failed\n");
		return false;
	}

/*--- set "+QSTRING" (with escape sequences enabled and embedded <cr>, '\', and '"') ---*/
	parser_p->options.allow_string_escapes = true;

	CLEAR_TEST_RESULTS();
	s_p = "AT+QSTRING=\"The quick \\62\\72\\6f\\77\\6e fox\\0d\\5c\\22\"\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.31) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+QSTRING")          != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_QSTRING)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[0].value.string_p, "The quick brown fox\x0d\\\"") != 0) ||
	    (strcmp(test_results.reply_string, "AT+QSTRING=\"The quick \\62\\72\\6f\\77\\6e fox\\0d\\5c\\22\"\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.32) failed\n");
		return false;
	}

/*--- set "+DIAL" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+DIAL=9,402 555-1212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.33) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+DIAL")             != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[0].value.string_p, "9,4025551212") != 0)        ||
	    (strcmp(test_results.reply_string, "AT+DIAL=9,402 555-1212\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.34) failed\n");
		return false;
	}

/*--- set "+MIX" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+MIX=33,af,1001,\"foxy\",\"mousey\",55\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.35) failed\n");
		return false;
	}
	if ((test_results.num_commands                                      != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+MIX")                  != 0)                                 ||
	    (test_results.commands[0].num_parameters                        != 6)                                 ||
	    (test_results.commands[0].parameter_values[0].type              != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal     != 33)                                ||
	    (test_results.commands[0].parameter_values[1].type              != utlAT_DATA_TYPE_HEXADECIMAL)       ||
	    (test_results.commands[0].parameter_values[1].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[1].value.hexadecimal != 0xaf)                              ||
	    (test_results.commands[0].parameter_values[2].type              != utlAT_DATA_TYPE_BINARY)            ||
	    (test_results.commands[0].parameter_values[2].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[2].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[2].value.binary      != 0x09)                              ||
	    (test_results.commands[0].parameter_values[3].type              != utlAT_DATA_TYPE_STRING)            ||
	    (test_results.commands[0].parameter_values[3].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[3].is_default        != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[3].value.string_p, "foxy") != 0)                    ||
	    (test_results.commands[0].parameter_values[4].type              != utlAT_DATA_TYPE_QSTRING)           ||
	    (test_results.commands[0].parameter_values[4].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[4].is_default        != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[4].value.string_p, "mousey") != 0)                  ||
	    (test_results.commands[0].parameter_values[5].type              != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[5].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[5].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[5].value.decimal     != 55)                                ||
	    (strcmp(test_results.reply_string, "AT+MIX=33,af,1001,\"foxy\",\"mousey\",55\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.36) failed\n");
		return false;
	}

/*--- get "+DEC" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+DEC?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.37) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+DEC")              != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 9)                                 ||
	    (strcmp(test_results.reply_string, "AT+DEC?\r\r\n9\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.38) failed\n");
		return false;
	}

/*--- get "+HEX" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+HEX?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.39) failed\n");
		return false;
	}
	if ((test_results.num_commands                                      != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+HEX")                  != 0)                                 ||
	    (test_results.commands[0].num_parameters                        != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type              != utlAT_DATA_TYPE_HEXADECIMAL)       ||
	    (test_results.commands[0].parameter_values[0].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.hexadecimal != 0x0A)                              ||
	    (strcmp(test_results.reply_string, "AT+HEX?\r\r\nA\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.40) failed\n");
		return false;
	}

/*--- get "+BIN" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+BIN?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.41) failed\n");
		return false;
	}
	if ((test_results.num_commands                                 != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+BIN")             != 0)                                 ||
	    (test_results.commands[0].num_parameters                   != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type         != utlAT_DATA_TYPE_BINARY)            ||
	    (test_results.commands[0].parameter_values[0].access       != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default   != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.binary != 11)                                ||
	    (strcmp(test_results.reply_string, "AT+BIN?\r\r\n1011\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.42) failed\n");
		return false;
	}

/*--- get "+STRING" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+STRING?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.43) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+STRING")           != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_STRING)            ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[0].value.string_p, "abc") != 0)                 ||
	    (strcmp(test_results.reply_string, "AT+STRING?\r\r\n\"abc\"\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.44) failed\n");
		return false;
	}

/*--- get "+MIX" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+MIX?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.45) failed\n");
		return false;
	}
	if ((test_results.num_commands                                      != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+MIX")                  != 0)                                 ||
	    (test_results.commands[0].num_parameters                        != 6)                                 ||
	    (test_results.commands[0].parameter_values[0].type              != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal     != 9)                                 ||
	    (test_results.commands[0].parameter_values[1].type              != utlAT_DATA_TYPE_HEXADECIMAL)       ||
	    (test_results.commands[0].parameter_values[1].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[1].value.hexadecimal != 0xa)                               ||
	    (test_results.commands[0].parameter_values[2].type              != utlAT_DATA_TYPE_BINARY)            ||
	    (test_results.commands[0].parameter_values[2].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[2].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[2].value.binary      != 11)                                ||
	    (test_results.commands[0].parameter_values[3].type              != utlAT_DATA_TYPE_STRING)            ||
	    (test_results.commands[0].parameter_values[3].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[3].is_default        != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[3].value.string_p, "abc") != 0)                     ||
	    (test_results.commands[0].parameter_values[4].type              != utlAT_DATA_TYPE_QSTRING)           ||
	    (test_results.commands[0].parameter_values[4].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[4].is_default        != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[4].value.string_p, "abc") != 0)                     ||
	    (test_results.commands[0].parameter_values[5].type              != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[5].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[5].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[5].value.decimal     != 9)                                 ||
	    (strcmp(test_results.reply_string, "AT+MIX?\r\r\n9,A,1011,\"abc\",\"abc\",9\r\n\r\nOK\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(4.46) failed\n");
		return false;
	}

/*--- multiple extended commands ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+DEC=21;+HEX=2f;+BIN=100\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.47) failed\n");
		return false;
	}
	if ((test_results.num_commands                                      != 3)                                 ||
	    (strcmp(test_results.commands[0].name, "+DEC")                  != 0)                                 ||
	    (test_results.commands[0].num_parameters                        != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type              != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal     != 21)                                ||
	    (strcmp(test_results.commands[1].name, "+HEX")                  != 0)                                 ||
	    (test_results.commands[1].num_parameters                        != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type              != utlAT_DATA_TYPE_HEXADECIMAL)       ||
	    (test_results.commands[1].parameter_values[0].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default        != false)                             ||
	    (test_results.commands[1].parameter_values[0].value.hexadecimal != 0x2f)                              ||
	    (strcmp(test_results.commands[2].name, "+BIN")                  != 0)                                 ||
	    (test_results.commands[2].num_parameters                        != 1)                                 ||
	    (test_results.commands[2].parameter_values[0].type              != utlAT_DATA_TYPE_BINARY)            ||
	    (test_results.commands[2].parameter_values[0].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[2].parameter_values[0].is_default        != false)                             ||
	    (test_results.commands[2].parameter_values[0].value.binary      != 4)                                 ||
	    (strcmp(test_results.reply_string, "AT+DEC=21;+HEX=2f;+BIN=100\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.48) failed\n");
		return false;
	}

/*--- multiple basic and extended commands ---*/
	CLEAR_TEST_RESULTS();
	s_p = "ATJ66+DEC=21;K+HEX=2f;&L33+BIN=100;&M\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.49) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 7)                                 ||
	    (strcmp(test_results.commands[0].name, "J")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 66)                                ||
	    (strcmp(test_results.commands[1].name, "+DEC")              != 0)                                 ||
	    (test_results.commands[1].num_parameters                    != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[1].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[1].parameter_values[0].value.decimal != 21)                                ||
	    (strcmp(test_results.commands[2].name, "K")                 != 0)                                 ||
	    (test_results.commands[2].num_parameters                    != 0)                                 ||
	    (strcmp(test_results.commands[3].name, "+HEX")              != 0)                                 ||
	    (test_results.commands[3].num_parameters                    != 1)                                 ||
	    (test_results.commands[3].parameter_values[0].type          != utlAT_DATA_TYPE_HEXADECIMAL)       ||
	    (test_results.commands[3].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[3].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[3].parameter_values[0].value.decimal != 0x2f)                              ||
	    (strcmp(test_results.commands[4].name, "&L")                != 0)                                 ||
	    (test_results.commands[4].num_parameters                    != 1)                                 ||
	    (test_results.commands[4].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[4].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[4].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[4].parameter_values[0].value.decimal != 33)                                ||
	    (strcmp(test_results.commands[5].name, "+BIN")              != 0)                                 ||
	    (test_results.commands[5].num_parameters                    != 1)                                 ||
	    (test_results.commands[5].parameter_values[0].type          != utlAT_DATA_TYPE_BINARY)            ||
	    (test_results.commands[5].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[5].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[5].parameter_values[0].value.decimal != 4)                                 ||
	    (strcmp(test_results.commands[6].name, "&M")                != 0)                                 ||
	    (test_results.commands[6].num_parameters                    != 0)                                 ||
	    (strcmp(test_results.reply_string, "ATJ66+DEC=21;K+HEX=2f;&L33+BIN=100;&M\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.50) failed\n");
		return false;
	}

/*--- action "+ACT" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+ACT=27\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.51) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+ACT")              != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 27)                                ||
	    (strcmp(test_results.reply_string, "AT+ACT=27\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.52) failed\n");
		return false;
	}

/*--- get "+ACT" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+ACT?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.53) failed\n");
		return false;
	}
	if ((test_results.num_commands                                   != 0) ||
	    (strcmp(test_results.reply_string, "AT+ACT?\r\r\nERROR\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.54) failed\n");
		return false;
	}

/*--- error: embedded space in decimal number ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+DEC=4 4\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.55) failed\n");
		return false;
	}
	if ((test_results.num_commands != 0) ||
	    (strcmp(test_results.reply_string, "AT+DEC=4 4\r\r\nERROR\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.56) failed\n");
		return false;
	}

/*--- error: trailing H in hexadecimal number ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+HEX=4bH\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.57) failed\n");
		return false;
	}
	if ((test_results.num_commands != 0) ||
	    (strcmp(test_results.reply_string, "AT+HEX=4bH\r\r\nERROR\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.58) failed\n");
		return false;
	}

/*--- error: unknown extended command ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+MOOSE=5\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.59) failed\n");
		return false;
	}
	if ((test_results.num_commands != 0) ||
	    (strcmp(test_results.reply_string, "AT+MOOSE=5\r\r\nERROR\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.60) failed\n");
		return false;
	}

/*--- error: too many parameters ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+DEC=4,5\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.61) failed\n");
		return false;
	}
	if ((test_results.num_commands != 0) ||
	    (strcmp(test_results.reply_string, "AT+DEC=4,5\r\r\nERROR\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.62) failed\n");
		return false;
	}

/*--- error: invalid decimal digit ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+DEC=a\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.63) failed\n");
		return false;
	}
	if ((test_results.num_commands != 0) ||
	    (strcmp(test_results.reply_string, "AT+DEC=a\r\r\nERROR\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.64) failed\n");
		return false;
	}

/*--- error: invalid hexadecimal digit ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+HEX=g\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.65) failed\n");
		return false;
	}
	if ((test_results.num_commands != 0) ||
	    (strcmp(test_results.reply_string, "AT+HEX=g\r\r\nERROR\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.66) failed\n");
		return false;
	}

/*--- error: invalid binary digit ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+BIN=0120\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.67) failed\n");
		return false;
	}
	if ((test_results.num_commands != 0) ||
	    (strcmp(test_results.reply_string, "AT+BIN=0120\r\r\nERROR\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(4.68) failed\n");
		return false;
	}


/*--- test initial parsing ----------------------------------------------*/

/*--- garbage text preceding AT sequence ---*/
	CLEAR_TEST_RESULTS();
	s_p = "mooATJ18\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(5.1) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "J")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 18)                                ||
	    (strcmp(test_results.reply_string, "mooATJ18\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(5.2) failed\n");
		return false;
	}

/*--- extra white space ---*/
	CLEAR_TEST_RESULTS();
	s_p = "  A  T  J  18  \r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(5.3) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "J")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 18)                                ||
	    (strcmp(test_results.reply_string, "  A  T  J  18  \r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(5.4) failed\n");
		return false;
	}

/*--- most-significant bits set ---*/
	CLEAR_TEST_RESULTS();
	s_p = "\xC1\xD4\xCa\xB1\xB8\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(5.5) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "J")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 18)                                ||
	    (strcmp(test_results.reply_string, "ATJ18\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(5.6) failed\n");
		return false;
	}

/*--- character accumulation ---*/
	CLEAR_TEST_RESULTS();
	s_p = "A";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(5.7) failed\n");
		return false;
	}
	s_p = "T";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(5.8) failed\n");
		return false;
	}
	s_p = "J";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(5.9) failed\n");
		return false;
	}
	s_p = "1";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(5.10) failed\n");
		return false;
	}
	s_p = "8";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(5.11) failed\n");
		return false;
	}
	s_p = "\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(5.12) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "J")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 18)                                ||
	    (strcmp(test_results.reply_string, "ATJ18\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(5.13) failed\n");
		return false;
	}

/*--- invalid AT command characters ---*/
	CLEAR_TEST_RESULTS();
	s_p = "\30\31A\30\31T\30\31J\30\31\61\70\30\31\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(5.14) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "J")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 18)                                ||
	    (strcmp(test_results.reply_string, "ATJ18\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(5.15) failed\n");
		return false;
	}

/*--- just AT ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(5.16) failed\n");
		return false;
	}
	if ((test_results.num_commands != 0) ||
	    (strcmp(test_results.reply_string, "AT\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(5.17) failed\n");
		return false;
	}

/*--- empty line ---*/
	CLEAR_TEST_RESULTS();
	s_p = "\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(5.18) failed\n");
		return false;
	}
	if ((test_results.num_commands != 0) ||
	    (strcmp(test_results.reply_string, "\r") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(5.19) failed\n");
		return false;
	}


/*--- test back-space ---------------------------------------------------*/

/*--- embedded back-space ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AZZZ\b\b\bTJ18\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(6.1) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "J")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 18)                                ||
	    (strcmp(test_results.reply_string, "AZZZ\b \b\b \b\b \bTJ18\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(6.2) failed\n");
		return false;
	}

/*--- back-space too far ---*/
	CLEAR_TEST_RESULTS();
	s_p = "Z\b\b\bATJ18\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(6.3) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "J")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 18)                                ||
	    (strcmp(test_results.reply_string, "Z\b \bATJ18\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(6.4) failed\n");
		return false;
	}


/*--- test non-verbose results ------------------------------------------*/

/*--- turn off verbose mode ---*/
	s_p = "ATV0\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(7.0) failed\n");
		return false;
	}

/*--- OK result code ---*/
	test_results.s_parameter_num   =   0;
	test_results.s_parameter_value =   0;
	test_results.reply_string[0]   = '\0';

	s_p = "ATS10=99\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(7.1) failed\n");
		return false;
	}
	if ((parser_p->parameters.S[10]     != 99) ||
	    (test_results.s_parameter_num   != 10) ||
	    (test_results.s_parameter_value != 99) ||
	    (strcmp(test_results.reply_string, "ATS10=99\r0\r") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(7.2) failed\n");
		return false;
	}

/*--- ERROR result code ---*/
	test_results.s_parameter_num   =   0;
	test_results.s_parameter_value =   0;
	test_results.reply_string[0]   = '\0';

	s_p = "ATW\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(7.3) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "ATW\r4\r") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(7.4) failed\n");
		return false;
	}

/*--- S-parameter value ---*/
	test_results.s_parameter_num   =   0;
	test_results.s_parameter_value =   0;
	test_results.reply_string[0]   = '\0';

	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_S_PARAMETER_VALUE, 11, 7) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(7.5) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_GET_S_PARAMETER_VALUE, 11, &parameter_value) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(7.6) failed\n");
		return false;
	}
	if (parameter_value != 7)
	{
		fprintf(stderr, "atParserTest: utlAtParse(7.7) failed\n");
		return false;
	}

	s_p = "ATS11?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(7.8) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "ATS11?\r007\r\n0\r") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(7.9) failed\n");
		return false;
	}

/*--- Extended AT-command parameter values ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+MIX?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(7.10) failed\n");
		return false;
	}
	if ((test_results.num_commands                                      != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+MIX")                  != 0)                                 ||
	    (test_results.commands[0].num_parameters                        != 6)                                 ||
	    (test_results.commands[0].parameter_values[0].type              != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal     != 9)                                 ||
	    (test_results.commands[0].parameter_values[1].type              != utlAT_DATA_TYPE_HEXADECIMAL)       ||
	    (test_results.commands[0].parameter_values[1].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[1].value.hexadecimal != 0xa)                               ||
	    (test_results.commands[0].parameter_values[2].type              != utlAT_DATA_TYPE_BINARY)            ||
	    (test_results.commands[0].parameter_values[2].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[2].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[2].value.binary      != 11)                                ||
	    (test_results.commands[0].parameter_values[3].type              != utlAT_DATA_TYPE_STRING)            ||
	    (test_results.commands[0].parameter_values[3].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[3].is_default        != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[3].value.string_p, "abc") != 0)                     ||
	    (test_results.commands[0].parameter_values[4].type              != utlAT_DATA_TYPE_QSTRING)           ||
	    (test_results.commands[0].parameter_values[4].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[4].is_default        != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[4].value.string_p, "abc") != 0)                     ||
	    (test_results.commands[0].parameter_values[5].type              != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[5].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[5].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[5].value.decimal     != 9)                                 ||
	    (strcmp(test_results.reply_string, "AT+MIX?\r9,A,1011,\"abc\",\"abc\",9\r\n0\r") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(7.11) failed\n");
		return false;
	}

/*--- Info-text response ---*/
	test_results.s_parameter_num   =   0;
	test_results.s_parameter_value =   0;
	test_results.reply_string[0]   = '\0';

	s_p = "ATI\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(7.12) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "ATI\rIntel\r\n0\r") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(7.13) failed\n");
		return false;
	}


/*--- restore verbose mode ---*/
	s_p = "ATV1\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(7.14) failed\n");
		return false;
	}


/*--- test one, two, and three parameters -------------------------------*/

/*--- set "+ONE" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+ONE=1234\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(8.1) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+ONE")              != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 1234)                              ||
	    (strcmp(test_results.reply_string, "AT+ONE=1234\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(8.2) failed\n");
		return false;
	}

/*--- get "+ONE" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+ONE?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(8.3) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+ONE")              != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 9)                                 ||
	    (strcmp(test_results.reply_string, "AT+ONE?\r\r\n9\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(8.4) failed\n");
		return false;
	}

/*--- set "+TWO" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+TWO=1234,5678\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(8.5) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+TWO")              != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 2)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 1234)                              ||
	    (test_results.commands[0].parameter_values[1].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[1].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[1].value.decimal != 5678)                              ||
	    (strcmp(test_results.reply_string, "AT+TWO=1234,5678\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(8.6) failed\n");
		return false;
	}

/*--- get "+TWO" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+TWO?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(8.7) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+TWO")              != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 2)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 9)                                 ||
	    (test_results.commands[0].parameter_values[1].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[1].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[1].value.decimal != 9)                                 ||
	    (strcmp(test_results.reply_string, "AT+TWO?\r\r\n9,9\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(8.8) failed\n");
		return false;
	}

/*--- set "+THREE" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+THREE=1234,5678,90\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(8.9) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+THREE")            != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 3)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 1234)                              ||
	    (test_results.commands[0].parameter_values[1].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[1].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[1].value.decimal != 5678)                              ||
	    (test_results.commands[0].parameter_values[2].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[2].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[2].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[2].value.decimal != 90)                                ||
	    (strcmp(test_results.reply_string, "AT+THREE=1234,5678,90\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(8.10) failed\n");
		return false;
	}

/*--- get "+THREE" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+THREE?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(8.11) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+THREE")            != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 3)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 9)                                 ||
	    (test_results.commands[0].parameter_values[1].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[1].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[1].value.decimal != 9)                                 ||
	    (test_results.commands[0].parameter_values[2].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[2].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[2].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[2].value.decimal != 9)                                 ||
	    (strcmp(test_results.reply_string, "AT+THREE?\r\r\n9,9,9\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(8.12) failed\n");
		return false;
	}

/*--- test optional parameters ------------------------------------------*/

/*--- set "+OPT" to a specified value ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+OPT=1234\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(9.1) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+OPT")              != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 1234)                              ||
	    (strcmp(test_results.reply_string, "AT+OPT=1234\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(9.2) failed\n");
		return false;
	}

/*--- set "+OPT" to the default value ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+OPT\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(9.3) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+OPT")              != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "AT+OPT\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(9.4) failed\n");
		return false;
	}

/*--- set "+OPTS", with all values specified ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+OPTS=12,34,56\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(9.5) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+OPTS")             != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 3)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 12)                                ||
	    (test_results.commands[0].parameter_values[1].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[1].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[1].value.decimal != 34)                                ||
	    (test_results.commands[0].parameter_values[2].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[2].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[2].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[2].value.decimal != 56)                                ||
	    (strcmp(test_results.reply_string, "AT+OPTS=12,34,56\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(9.6) failed\n");
		return false;
	}

/*--- set "+OPTS", with no values specified, using commas ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+OPTS=,,\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(9.7) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+OPTS")             != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 3)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 0)                                 ||
	    (test_results.commands[0].parameter_values[1].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[1].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[1].value.decimal != 0)                                 ||
	    (test_results.commands[0].parameter_values[2].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[2].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[2].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[2].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "AT+OPTS=,,\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(9.8) failed\n");
		return false;
	}

/*--- set "+OPTS", with no values specified, excluding commas ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+OPTS\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(9.9) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+OPTS")             != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 3)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 0)                                 ||
	    (test_results.commands[0].parameter_values[1].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[1].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[1].value.decimal != 0)                                 ||
	    (test_results.commands[0].parameter_values[2].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[2].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[2].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[2].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "AT+OPTS\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(9.10) failed\n");
		return false;
	}

/*--- set "+OPTS", with one value specified ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+OPTS=,123456,\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(9.11) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+OPTS")             != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 3)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 0)                                 ||
	    (test_results.commands[0].parameter_values[1].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[1].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[1].value.decimal != 123456)                            ||
	    (test_results.commands[0].parameter_values[2].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[2].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[2].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[2].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "AT+OPTS=,123456,\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(9.12) failed\n");
		return false;
	}

/*--- test required parameters ------------------------------------------*/

/*--- set "+REQ" to a specified value ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+REQ=1234\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(10.1) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+REQ")              != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 1234)                              ||
	    (strcmp(test_results.reply_string, "AT+REQ=1234\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(10.2) failed\n");
		return false;
	}

/*--- set "+REQ" omitting the value ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+REQ\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(10.3) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 0) ||
	    (strcmp(test_results.reply_string, "AT+REQ\r\r\nERROR\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(10.4) failed\n");
		return false;
	}


/*--- test mix of optional and required parameters ----------------------*/

/*--- set "+ALT" with values for all parameters ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+ALT=12,34,56,78\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(11.1) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+ALT")              != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 4)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 12)                                ||
	    (test_results.commands[0].parameter_values[1].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[1].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[1].value.decimal != 34)                                ||
	    (test_results.commands[0].parameter_values[2].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[2].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[2].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[2].value.decimal != 56)                                ||
	    (test_results.commands[0].parameter_values[3].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[3].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[3].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[3].value.decimal != 78)                                ||
	    (strcmp(test_results.reply_string, "AT+ALT=12,34,56,78\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(11.2) failed\n");
		return false;
	}

/*--- set "+ALT" with only values for required parameters ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+ALT=12,,56\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(11.4) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+ALT")              != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 4)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 12)                                ||
	    (test_results.commands[0].parameter_values[1].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[1].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[1].value.decimal != 0)                                 ||
	    (test_results.commands[0].parameter_values[2].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[2].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[2].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[2].value.decimal != 56)                                ||
	    (test_results.commands[0].parameter_values[3].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[3].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[3].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[3].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "AT+ALT=12,,56\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(11.4) failed\n");
		return false;
	}

/*--- set "+ALT" but omit value for first optional parameter ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+ALT=12,,56,78\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(11.5) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+ALT")              != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 4)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 12)                                ||
	    (test_results.commands[0].parameter_values[1].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[1].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[1].value.decimal != 0)                                 ||
	    (test_results.commands[0].parameter_values[2].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[2].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[2].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[2].value.decimal != 56)                                ||
	    (test_results.commands[0].parameter_values[3].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[3].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[3].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[3].value.decimal != 78)                                ||
	    (strcmp(test_results.reply_string, "AT+ALT=12,,56,78\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(11.6) failed\n");
		return false;
	}

/*--- set "+ALT" but omit value for last optional parameter (with tailing comma) ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+ALT=12,34,56,\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(11.7) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+ALT")              != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 4)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 12)                                ||
	    (test_results.commands[0].parameter_values[1].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[1].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[1].value.decimal != 34)                                ||
	    (test_results.commands[0].parameter_values[2].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[2].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[2].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[2].value.decimal != 56)                                ||
	    (test_results.commands[0].parameter_values[3].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[3].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[3].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[3].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "AT+ALT=12,34,56,\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(11.8) failed\n");
		return false;
	}

/*--- set "+ALT" but omit value for last optional parameter (omit tailing comma) ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+ALT=12,34,56\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(11.9) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+ALT")              != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 4)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 12)                                ||
	    (test_results.commands[0].parameter_values[1].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[1].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[1].value.decimal != 34)                                ||
	    (test_results.commands[0].parameter_values[2].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[2].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[2].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[2].value.decimal != 56)                                ||
	    (test_results.commands[0].parameter_values[3].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[3].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[3].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[3].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "AT+ALT=12,34,56\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(11.10) failed\n");
		return false;
	}


/*--- test R/W parameters -----------------------------------------------*/

/*--- set "+RW" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+RW=1234,5678\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(12.1) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+RW")               != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 2)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 1234)                              ||
	    (test_results.commands[0].parameter_values[1].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[1].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[1].value.decimal != 5678)                              ||
	    (strcmp(test_results.reply_string, "AT+RW=1234,5678\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(12.2) failed\n");
		return false;
	}

/*--- get "+RW" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+RW?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(12.3) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+RW")               != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 2)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 9)                                 ||
	    (test_results.commands[0].parameter_values[1].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[1].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[1].value.decimal != 9)                                 ||
	    (strcmp(test_results.reply_string, "AT+RW?\r\r\n9,9\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(12.4) failed\n");
		return false;
	}


/*--- test R/O parameters -----------------------------------------------*/

/*--- set "+RO" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+RO=1234,5678\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(13.1) failed\n");
		return false;
	}
	if ((test_results.num_commands                                           != 0) ||
	    (strcmp(test_results.reply_string, "AT+RO=1234,5678\r\r\nERROR\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(13.2) failed\n");
		return false;
	}

/*--- get "+RO" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+RO?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(13.3) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                ||
	    (strcmp(test_results.commands[0].name, "+RO")               != 0)                                ||
	    (test_results.commands[0].num_parameters                    != 2)                                ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)          ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_ONLY) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                            ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 9)                                ||
	    (test_results.commands[0].parameter_values[1].type          != utlAT_DATA_TYPE_DECIMAL)          ||
	    (test_results.commands[0].parameter_values[1].access        != utlAT_PARAMETER_ACCESS_READ_ONLY) ||
	    (test_results.commands[0].parameter_values[1].is_default    != false)                            ||
	    (test_results.commands[0].parameter_values[1].value.decimal != 9)                                ||
	    (strcmp(test_results.reply_string, "AT+RO?\r\r\n9,9\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(13.4) failed\n");
		return false;
	}


/*--- test W/O parameters -----------------------------------------------*/

/*--- set "+WO" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+WO=1234,5678\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(14.1) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+WO")               != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 2)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_WRITE_ONLY) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 1234)                              ||
	    (test_results.commands[0].parameter_values[1].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[1].access        != utlAT_PARAMETER_ACCESS_WRITE_ONLY) ||
	    (test_results.commands[0].parameter_values[1].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[1].value.decimal != 5678)                              ||
	    (strcmp(test_results.reply_string, "AT+WO=1234,5678\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(14.2) failed\n");
		return false;
	}

/*--- get "+WO" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+WO?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(14.3) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 0) ||
	    (strcmp(test_results.reply_string, "AT+WO?\r\r\nERROR\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(14.4) failed\n");
		return false;
	}


/*--- test R parameters -----------------------------------------------*/

/*--- set "+R" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+R=1234,5678\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(15.1) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                            ||
	    (strcmp(test_results.commands[0].name, "+R")                != 0)                            ||
	    (test_results.commands[0].num_parameters                    != 2)                            ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)      ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ)  ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                        ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 1234)                         ||
	    (test_results.commands[0].parameter_values[1].type          != utlAT_DATA_TYPE_DECIMAL)      ||
	    (test_results.commands[0].parameter_values[1].access        != utlAT_PARAMETER_ACCESS_READ)  ||
	    (test_results.commands[0].parameter_values[1].is_default    != false)                        ||
	    (test_results.commands[0].parameter_values[1].value.decimal != 5678)                         ||
	    (strcmp(test_results.reply_string, "AT+R=1234,5678\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(15.2) failed\n");
		return false;
	}

/*--- get "+R" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+R?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(15.3) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                            ||
	    (strcmp(test_results.commands[0].name, "+R")                != 0)                            ||
	    (test_results.commands[0].num_parameters                    != 2)                            ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)      ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ)  ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                        ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 9)                            ||
	    (test_results.commands[0].parameter_values[1].type          != utlAT_DATA_TYPE_DECIMAL)      ||
	    (test_results.commands[0].parameter_values[1].access        != utlAT_PARAMETER_ACCESS_READ)  ||
	    (test_results.commands[0].parameter_values[1].is_default    != false)                        ||
	    (test_results.commands[0].parameter_values[1].value.decimal != 9)                            ||
	    (strcmp(test_results.reply_string, "AT+R?\r\r\n9,9\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(15.4) failed\n");
		return false;
	}


/*--- test mix of R/W and R/O parameters --------------------------------*/

/*--- set "+RWRO" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+RWRO=,34,,78\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(16.1) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+RWRO")             != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 4)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_ONLY)  ||
	    (test_results.commands[0].parameter_values[0].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 0)                                 ||
	    (test_results.commands[0].parameter_values[1].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[1].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[1].value.decimal != 34)                                ||
	    (test_results.commands[0].parameter_values[2].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[2].access        != utlAT_PARAMETER_ACCESS_READ_ONLY)  ||
	    (test_results.commands[0].parameter_values[2].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[2].value.decimal != 0)                                 ||
	    (test_results.commands[0].parameter_values[3].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[3].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[3].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[3].value.decimal != 78)                                ||
	    (strcmp(test_results.reply_string, "AT+RWRO=,34,,78\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(16.2) failed\n");
		return false;
	}

/*--- set "+RWRO" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+RWRO=12,34,,78\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(16.3) failed\n");
		return false;
	}
	if ((test_results.num_commands                                             != 0) ||
	    (strcmp(test_results.reply_string, "AT+RWRO=12,34,,78\r\r\nERROR\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(16.4) failed\n");
		return false;
	}

/*--- set "+RWRO" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+RWRO=,34,56,78\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(16.5) failed\n");
		return false;
	}
	if ((test_results.num_commands                                             != 0) ||
	    (strcmp(test_results.reply_string, "AT+RWRO=,34,56,78\r\r\nERROR\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(16.6) failed\n");
		return false;
	}

/*--- get "+RWRO" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+RWRO?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(16.7) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+RWRO")             != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 4)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_ONLY)  ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 9)                                 ||
	    (test_results.commands[0].parameter_values[1].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[1].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[1].value.decimal != 9)                                 ||
	    (test_results.commands[0].parameter_values[2].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[2].access        != utlAT_PARAMETER_ACCESS_READ_ONLY)  ||
	    (test_results.commands[0].parameter_values[2].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[2].value.decimal != 9)                                 ||
	    (test_results.commands[0].parameter_values[3].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[3].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[3].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[3].value.decimal != 9)                                 ||
	    (strcmp(test_results.reply_string, "AT+RWRO?\r\r\n9,9,9,9\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(16.8) failed\n");
		return false;
	}

/*--- fetch "static" command syntax ---*/

/*--- "+GSYN" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+GSYN=?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(17.1) failed\n");
		return false;
	}
	if ((test_results.num_commands != 0) ||
	    (strcmp(test_results.reply_string, "AT+GSYN=?\r\r\n+GSYN=(0-6),(1-7),(2-9,11)\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(17.2) failed\n");
		return false;
	}

/*--- fetch "dynamic command syntax ---*/

/*--- "+EVSYN" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+EVSYN=?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(17.3) failed\n");
		return false;
	}
	if ((test_results.num_commands != 0) ||
	    (strcmp(test_results.reply_string, "AT+EVSYN=?\r\r\n+EVSYN=(0-9),(10-20),(20-30)\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(17.4) failed\n");
		return false;
	}

/*--- "+AVSYN" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+AVSYN=?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(17.5) failed\n");
		return false;
	}
	if ((test_results.num_commands != 0) ||
	    (strcmp(test_results.reply_string, "AT+AVSYN=?\r\r\n+AVSYN=(10-19),(20-30),(30-40)\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(17.6) failed\n");
		return false;
	}

/*--- fetch asynchronous command syntax ---*/

/*--- "+EVSYNA" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+EVSYNA=?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(17.7) failed\n");
		return false;
	}
	if (test_results.num_commands != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(17.8) failed\n");
		return false;
	}
	if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_COMMAND_SYNTAX, "+EVSYNA=(0-9),(10-20),(20-30)") != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(17.9) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "AT+EVSYNA=?\r\r\n+EVSYNA=(0-9),(10-20),(20-30)\r\n\r\nOK\r\n") != 0)
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(17.10) failed\n");
		return false;
	}

/*--- "+AVSYNA" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+AVSYNA=?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(17.11) failed\n");
		return false;
	}
	if (test_results.num_commands != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(17.12) failed\n");
		return false;
	}
	if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_COMMAND_SYNTAX, "+AVSYNA=(10-19),(20-30),(30-40)") != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(17.13) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "AT+AVSYNA=?\r\r\n+AVSYNA=(10-19),(20-30),(30-40)\r\n\r\nOK\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(17.14) failed\n");
		return false;
	}

/*--- fetch asynchronous-with-synchronous-reply  command syntax ---*/

/*--- "+EVSYNAS" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+EVSYNAS=?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(17.15) failed\n");
		return false;
	}
	if ((test_results.num_commands != 0) ||
	    (strcmp(test_results.reply_string, "AT+EVSYNAS=?\r\r\n+EVSYNAS=(0-9),(10-20),(20-30)\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(17.16) failed\n");
		return false;
	}

/*--- "+AVSYNAS" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+AVSYNAS=?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(17.17) failed\n");
		return false;
	}
	if ((test_results.num_commands != 0) ||
	    (strcmp(test_results.reply_string, "AT+AVSYNAS=?\r\r\n+AVSYNAS=(10-19),(20-30),(30-40)\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(17.18) failed\n");
		return false;
	}

/*--- error: unknown command ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+MOOSE=?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(17.19) failed\n");
		return false;
	}
	if ((test_results.num_commands != 0) ||
	    (strcmp(test_results.reply_string, "AT+MOOSE=?\r\r\nERROR\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(17.20) failed\n");
		return false;
	}

/*--- some built-in AT commands -----------------------------------------*/

/*--- "+GMI" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+GMI\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(18.1) failed\n");
		return false;
	}
	if ((test_results.num_commands                     != 1) ||
	    (strcmp(test_results.commands[0].name, "+GMI") != 0) ||
	    (test_results.commands[0].num_parameters       != 0) ||
	    (strcmp(test_results.reply_string, "AT+GMI\r\r\nIntel\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(18.2) failed\n");
		return false;
	}

/*--- "+GCAP" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+GCAP\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(18.3) failed\n");
		return false;
	}
	if ((test_results.num_commands                      != 1) ||
	    (strcmp(test_results.commands[0].name, "+GCAP") != 0) ||
	    (test_results.commands[0].num_parameters        != 0) ||
	    (strcmp(test_results.reply_string, "AT+GCAP\r\r\n+GCAP: +DEC,+HEX,+BIN,+STRING,+QSTRING,+DIAL,+MIX,+ACT,+ONE,+TWO,+THREE,+OPT,+REQ,+ALT,+OPTS,+RW,+RO,+WO,+R,+RWRO,+GSYN,+EVSYN,+AVSYN,+EVSYNA,+AVSYNA,+EVSYNAS,+AVSYNAS,+TMO,+CBACK,+GMR,+S234567890123456,+GMI,+GCAP,+GCI,+MV18S,+IPR,+ASTO,+AINFO,+APARAM,+SINFO,+SPARAM,+SABORT,+SERROR\r\n\r\nOK\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(18.4) failed\n");
		return false;
	}

/*--- "+GCI" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+GCI\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(18.5) failed\n");
		return false;
	}
	if ((test_results.num_commands                     != 1) ||
	    (strcmp(test_results.commands[0].name, "+GCI") != 0) ||
	    (test_results.commands[0].num_parameters       != 0) ||
	    (strcmp(test_results.reply_string, "AT+GCI\r\r\n+GCI: 20\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(18.6) failed\n");
		return false;
	}

/*--- "+IPR" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+IPR=9600\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(18.7) failed\n");
		return false;
	}
	if ((test_results.num_commands                     != 1) ||
	    (strcmp(test_results.commands[0].name, "+IPR") != 0) ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 9600)                              ||
	    (strcmp(test_results.reply_string, "AT+IPR=9600\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(18.8) failed\n");
		return false;
	}

	CLEAR_TEST_RESULTS();
	s_p = "AT+IPR?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(18.9) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+IPR")              != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "AT+IPR?\r\r\n+IPR: 9600\r\n\r\nOK\r\n") != 0))
	{

		fprintf(stderr, "atParserTest: utlAtParse(18.10) failed\n");
		return false;
	}


/*--- extended command with embedded digits in name ---------------------*/

/*--- "+MV18S" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+MV18S=?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(19.1) failed\n");
		return false;
	}
	if ((test_results.num_commands != 0) ||
	    (strcmp(test_results.reply_string, "AT+MV18S=?\r\r\nab\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(19.2) failed\n");
		return false;
	}


/*--- suppressed result code --------------------------------------------*/

/*--- suppress result codes ---*/
	s_p = "ATQ1\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(20.0) failed\n");
		return false;
	}

/*--- "+GMI" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+GMI\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(20.1) failed\n");
		return false;
	}
	if ((test_results.num_commands                     != 1) ||
	    (strcmp(test_results.commands[0].name, "+GMI") != 0) ||
	    (test_results.commands[0].num_parameters       != 0) ||
	    (strcmp(test_results.reply_string, "AT+GMI\r\r\nIntel\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(20.2) failed\n");
		return false;
	}

/*--- restore result codes ---*/
	s_p = "ATQ0\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(20.3) failed\n");
		return false;
	}


/*--- repeat last command --------------------------------------------------*/

/*--- repeat once ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+DEC=43\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(21.1) failed\n");
		return false;
	}
	s_p = "A/";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(21.2) failed\n");
		return false;
	}
	if ((test_results.num_commands                                    != 2)                                 ||
	    (strcmp(test_results.commands[0].name, "+DEC")                != 0)                                 ||
	    (test_results.commands[0].num_parameters                      != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type            != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access          != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default      != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal   != 43)                                ||
	    (strcmp(test_results.commands[1].name, "+DEC")                != 0)                                 ||
	    (test_results.commands[1].num_parameters                      != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type            != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[1].parameter_values[0].access          != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default      != false)                             ||
	    (test_results.commands[1].parameter_values[0].value.decimal   != 43)                                ||
	    (strcmp(test_results.reply_string, "AT+DEC=43\r\r\nOK\r\nA/\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(21.3) failed\n");
		return false;
	}

/*--- repeat twice ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+DEC=43\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(21.4) failed\n");
		return false;
	}
	s_p = "A/a/";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(21.5) failed\n");
		return false;
	}
	if ((test_results.num_commands                                    != 3)                                 ||
	    (strcmp(test_results.commands[0].name, "+DEC")                != 0)                                 ||
	    (test_results.commands[0].num_parameters                      != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type            != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access          != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default      != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal   != 43)                                ||
	    (strcmp(test_results.commands[1].name, "+DEC")                != 0)                                 ||
	    (test_results.commands[1].num_parameters                      != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type            != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[1].parameter_values[0].access          != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default      != false)                             ||
	    (test_results.commands[1].parameter_values[0].value.decimal   != 43)                                ||
	    (strcmp(test_results.commands[2].name, "+DEC")                != 0)                                 ||
	    (test_results.commands[2].num_parameters                      != 1)                                 ||
	    (test_results.commands[2].parameter_values[0].type            != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[2].parameter_values[0].access          != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[2].parameter_values[0].is_default      != false)                             ||
	    (test_results.commands[2].parameter_values[0].value.decimal   != 43)                                ||
	    (strcmp(test_results.reply_string, "AT+DEC=43\r\r\nOK\r\nA/\r\nOK\r\na/\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(21.6) failed\n");
		return false;
	}


/*--- call-back modified response ------------------------------------------*/

/*--- set, generating a custom response ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+CBACK=1a,1b\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(22.1) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+CBACK")            != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 2)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_HEXADECIMAL)       ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 0x1a)                              ||
	    (test_results.commands[0].parameter_values[1].type          != utlAT_DATA_TYPE_HEXADECIMAL)       ||
	    (test_results.commands[0].parameter_values[1].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[1].value.decimal != 0x1b)                              ||
	    (strcmp(test_results.reply_string, "AT+CBACK=1a,1b\r\r\nCustom response string\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(22.2) failed\n");
		return false;
	}

/*--- get, generating a custom response ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+CBACK?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(22.3) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+CBACK")            != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 2)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_HEXADECIMAL)       ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 0)                                 ||
	    (test_results.commands[0].parameter_values[1].type          != utlAT_DATA_TYPE_HEXADECIMAL)       ||
	    (test_results.commands[0].parameter_values[1].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[1].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "AT+CBACK?\r\r\nCustom response string\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(22.4) failed\n");
		return false;
	}


/*--- get, generating a combination of the original plus a custom response ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+GMR?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(22.5) failed\n");
		return false;
	}
	if ((test_results.num_commands                     != 1) ||
	    (strcmp(test_results.commands[0].name, "+GMR") != 0) ||
	    (test_results.commands[0].num_parameters       != 0) ||
	    (strcmp(test_results.reply_string, "AT+GMR?\r\r\n1.0\r\nCustom response string\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(22.6) failed\n");
		return false;
	}


/*--- stuff that's too long ------------------------------------------------*/

/*--- line too long ---*/
	CLEAR_TEST_RESULTS();
	(void)strcpy(buf, "AT+STRING=\"");
	for (i = strlen(buf), j = 0; i < utlAT_MAX_LINE_LENGTH + 1; i++, j++)
		buf[i] = 'a' + (j % 26);
	buf[i++] = '"';
	buf[i++] = '\r';
	buf[i] = '\0';
	s_p = buf;
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(23.1) failed\n");
		return false;
	}
	if ((test_results.num_commands != 0) ||
	    (strcmp(test_results.reply_string, "") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(23.2) failed\n");
		return false;
	}

/*--- extended AT-Command name almost too long ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+S234567890123456=?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(23.3) failed\n");
		return false;
	}
	if ((test_results.num_commands != 0) ||
	    (strcmp(test_results.reply_string, "AT+S234567890123456=?\r\r\n+S\r\n\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(23.4) failed\n");
		return false;
	}


/*--- extended AT-Command name just-barely too long ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+S2345678901234567?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(23.5) failed\n");
		return false;
	}
	if ((test_results.num_commands != 0) ||
	    (strcmp(test_results.reply_string, "AT+S2345678901234567?\r\r\nERROR\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(23.6) failed\n");
		return false;
	}


/*--- dialing (ATD) --------------------------------------------------------*/

/*--- dial string (simple) ---*/
	CLEAR_TEST_RESULTS();
	parser_p->states.dial_string.active = false;
	s_p = "ATD0123456789*#ABCD\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(24.1) failed\n");
		return false;
	}
	if ((strcmp(parser_p->states.dial_string.buf, "0123456789*#ABCD") != 0) ||
	    (parser_p->states.dial_string.active                      != true)    ||
	    (parser_p->states.dial_string.dial_string_delay           != 0)       ||
	    (parser_p->states.dial_string.options.international       != false)   ||
	    (parser_p->states.dial_string.options.do_call_origination != true)    ||
	    (parser_p->states.dial_string.options.use_CCUG_SS_info    != false)   ||
	    (parser_p->states.dial_string.options.CLI_presentation    != '\0'))
	{
		fprintf(stderr, "atParserTest: utlAtParse(24.2) failed\n");
		return false;
	}

/*--- dial string (references last-dialed string) ---*/
	CLEAR_TEST_RESULTS();
	parser_p->states.dial_string.active = false;
	s_p = "ATD89L98;\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(24.3) failed\n");
		return false;
	}
	if ((strcmp(parser_p->states.dial_string.buf, "890123456789*#ABCD98;") != 0) ||
	    (parser_p->states.dial_string.active                      != true)  ||
	    (parser_p->states.dial_string.dial_string_delay           != 0)     ||
	    (parser_p->states.dial_string.options.international       != false) ||
	    (parser_p->states.dial_string.options.do_call_origination != false) ||
	    (parser_p->states.dial_string.options.use_CCUG_SS_info    != false) ||
	    (parser_p->states.dial_string.options.CLI_presentation    != '\0'))
	{
		fprintf(stderr, "atParserTest: utlAtParse(24.4) failed\n");
		return false;
	}

/*--- dial string (delay dialing, ignore invalid dialing characters) ---*/
	CLEAR_TEST_RESULTS();
	parser_p->states.dial_string.active = false;
	s_p = "ATD//(403) 555-1212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(24.5) failed\n");
		return false;
	}
	if ((strcmp(parser_p->states.dial_string.buf, "//4035551212") != 0) ||
	    (parser_p->states.dial_string.active                      != true)  ||
	    (parser_p->states.dial_string.dial_string_delay           != 2)     ||
	    (parser_p->states.dial_string.options.international       != false) ||
	    (parser_p->states.dial_string.options.do_call_origination != true)  ||
	    (parser_p->states.dial_string.options.use_CCUG_SS_info    != false) ||
	    (parser_p->states.dial_string.options.CLI_presentation    != '\0'))
	{
		fprintf(stderr, "atParserTest: utlAtParse(24.6) failed\n");
		return false;
	}

/*--- dial string (references stored strings) ---*/
	CLEAR_TEST_RESULTS();
	parser_p->states.dial_string.active = false;
	s_p = "AT+ASTO=0,111;\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(24.7) failed\n");
		return false;
	}
	s_p = "AT+ASTO=1,222;\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(24.8) failed\n");
		return false;
	}
	s_p = "AT+ASTO=2,333;\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(24.9) failed\n");
		return false;
	}
	s_p = "AT+ASTO=3,\"GHI\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(24.10) failed\n");
		return false;
	}
	s_p = "ATD0123,4P567@89012345S-1,S,S-2,S-3;\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(24.11) failed\n");
		return false;
	}
	if ((strcmp(parser_p->states.dial_string.buf, "0123,4P567@89012345222,111,333,444;") != 0) ||
	    (parser_p->states.dial_string.active                      != true)  ||
	    (parser_p->states.dial_string.dial_string_delay           != 0)     ||
	    (parser_p->states.dial_string.options.international       != false) ||
	    (parser_p->states.dial_string.options.do_call_origination != false) ||
	    (parser_p->states.dial_string.options.use_CCUG_SS_info    != false) ||
	    (parser_p->states.dial_string.options.CLI_presentation    != '\0'))
	{
		fprintf(stderr, "atParserTest: utlAtParse(24.12) failed\n");
		return false;
	}

	s_p = "AT+ASTO?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(24.13) failed\n");
		return false;
	}

/*--- dial string (delay dialing, ignore invalid dialing characters) ---*/
	CLEAR_TEST_RESULTS();
	parser_p->states.dial_string.active = false;
	s_p = "ATD(403) 555-1212I\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(24.14) failed\n");
		return false;
	}
	if ((strcmp(parser_p->states.dial_string.buf, "4035551212I")  != 0)     ||
	    (parser_p->states.dial_string.active                      != true)  ||
	    (parser_p->states.dial_string.dial_string_delay           != 0)     ||
	    (parser_p->states.dial_string.options.international       != false) ||
	    (parser_p->states.dial_string.options.do_call_origination != true)  ||
	    (parser_p->states.dial_string.options.use_CCUG_SS_info    != false) ||
	    (parser_p->states.dial_string.options.CLI_presentation    != 'I'))
	{
		fprintf(stderr, "atParserTest: utlAtParse(24.15) failed\n");
		return false;
	}

/*--- dial string (delay dialing, ignore invalid dialing characters) ---*/
	CLEAR_TEST_RESULTS();
	s_p = "ATD(403) 555-1212i\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(24.16) failed\n");
		return false;
	}
	if ((strcmp(parser_p->states.dial_string.buf, "4035551212i")  != 0)     ||
	    (parser_p->states.dial_string.active                      != true)  ||
	    (parser_p->states.dial_string.dial_string_delay           != 0)     ||
	    (parser_p->states.dial_string.options.international       != false) ||
	    (parser_p->states.dial_string.options.do_call_origination != true)  ||
	    (parser_p->states.dial_string.options.use_CCUG_SS_info    != false) ||
	    (parser_p->states.dial_string.options.CLI_presentation    != 'i'))
	{
		fprintf(stderr, "atParserTest: utlAtParse(24.17) failed\n");
		return false;
	}

/*--- dial string (delay dialing, ignore invalid dialing characters) ---*/
	CLEAR_TEST_RESULTS();
	parser_p->states.dial_string.active = false;
	s_p = "ATD(403) 555-1212g\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(24.18) failed\n");
		return false;
	}
	if ((strcmp(parser_p->states.dial_string.buf, "4035551212g")  != 0)     ||
	    (parser_p->states.dial_string.active                      != true)  ||
	    (parser_p->states.dial_string.dial_string_delay           != 0)     ||
	    (parser_p->states.dial_string.options.international       != false) ||
	    (parser_p->states.dial_string.options.do_call_origination != true)  ||
	    (parser_p->states.dial_string.options.use_CCUG_SS_info    != true)  ||
	    (parser_p->states.dial_string.options.CLI_presentation    != '\0'))
	{
		fprintf(stderr, "atParserTest: utlAtParse(24.19) failed\n");
		return false;
	}


/*--- two lines of text in a single request --------------------------------*/

/*--- successfully set "J" and "K" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "ATJ18\rATK\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(25.1) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 2)                                 ||
	    (strcmp(test_results.commands[0].name, "J")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 18)                                ||
	    (strcmp(test_results.commands[1].name, "K")                 != 0)                                 ||
	    (test_results.commands[1].num_parameters                    != 0)                                 ||
	    (strcmp(test_results.reply_string, "ATJ18\r\r\nOK\r\nATK\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(25.2) failed\n");
		return false;
	}

/*--- fail to set "A" (unknown command) and successfully set "K" ---*/
	CLEAR_TEST_RESULTS();
	s_p = "ATA\rATK\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(25.3) failed\n");
		return false;
	}
	if ((test_results.num_commands                  != 1) ||
	    (strcmp(test_results.commands[0].name, "K") != 0) ||
	    (test_results.commands[0].num_parameters    != 0) ||
	    (strcmp(test_results.reply_string, "ATA\r\r\nERROR\r\nATK\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(25.4) failed\n");
		return false;
	}

/*--- successfully set "J" and fail to set "A" (unknown command) ---*/
	CLEAR_TEST_RESULTS();
	s_p = "ATJ18\rATA\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(25.5) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "J")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 18)                                ||
	    (strcmp(test_results.reply_string, "ATJ18\r\r\nOK\r\nATA\r\r\nERROR\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(25.6) failed\n");
		return false;
	}

/*--- fail to execute previous command (unknown command) and successfully set "K" ---*/
	CLEAR_TEST_RESULTS();
	(void)strcpy(parser_p->buffers.previous_line, "ATA"); /* force previous command to be unknown */
	s_p = "A/\rATK\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(25.7) failed\n");
		return false;
	}
	if ((test_results.num_commands                  != 1) ||
	    (strcmp(test_results.commands[0].name, "K") != 0) ||
	    (test_results.commands[0].num_parameters    != 0) ||
	    (strcmp(test_results.reply_string, "A/\r\nERROR\r\n\rATK\r\r\nOK\r\n") != 0))
	{
		fprintf(stderr, "atParserTest: utlAtParse(25.8) failed\n");
		return false;
	}


/*--- state transistions ---------------------------------------------------*/

	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_RESET) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.1) failed\n");
		return false;
	}
	if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.2) failed\n");
		return false;
	}

/*--- simple out-going call that is answered ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test = 1;
	s_p = "ATX4DT555-1212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.3) failed\n");
		return false;
	}
	sleep(1);
	s_p = "+++";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.4) failed\n");
		return false;
	}
	s_p = "ATH\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.5) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_DIAL_TONE_WAIT;
	state_ids[ 2] = utlAT_STATE_ID_DIALING;
	state_ids[ 3] = utlAT_STATE_ID_TONE_DIGITS_WAIT;
	state_ids[ 4] = utlAT_STATE_ID_DIALING;
	state_ids[ 5] = utlAT_STATE_ID_RING_BACK_WAIT;
	state_ids[ 6] = utlAT_STATE_ID_ANSWER_WAIT;
	state_ids[ 7] = utlAT_STATE_ID_ANSWER_WAIT;
	state_ids[ 8] = utlAT_STATE_ID_CARRIER_WAIT;
	state_ids[ 9] = utlAT_STATE_ID_CARRIER_CONFIRM;
	state_ids[10] = utlAT_STATE_ID_ON_LINE;
	state_ids[11] = utlAT_STATE_ID_OFF_LINE;
	state_ids[12] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[13] = utlAT_STATE_ID_ON_HOOK;
	state_ids[14] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.6) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 3)                                 ||
	    (strcmp(test_results.commands[0].name, "X")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 4)                                 ||
	    (strcmp(test_results.commands[1].name, "D")                 != 0)                                 ||
	    (test_results.commands[1].num_parameters                    != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type          != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[1].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default    != false)                             ||
	    (strcmp(test_results.commands[1].parameter_values[0].value.dial_string_p, "T5551212") != 0)       ||
	    (strcmp(test_results.commands[2].name, "H")                 != 0)                                 ||
	    (test_results.commands[2].num_parameters                    != 1)                                 ||
	    (test_results.commands[2].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[2].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[2].parameter_values[0].is_default    != true)                              ||
	    (test_results.commands[2].parameter_values[0].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "ATX4DT555-1212\r\r\nCONNECT\r\n+++\r\nOK\r\nATH\r\r\nOK\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.7) failed\n");
		return false;
	}

/*--- more complex out-going call that is answered ---*/
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_CONNECT_TEXT, "19200") != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.8) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SELECT_CARRIER_RESULT_CODE) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.9) failed\n");
		return false;
	}
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 2;
	test_results.driver_request_test_state = 0;
	s_p = "ATE1X4DT55,5-P1212T!66W77@\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.10) failed\n");
		return false;
	}
	sleep(1);
	s_p = "+++";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.11) failed\n");
		return false;
	}
	s_p = "ATH\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.12) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_DIAL_TONE_WAIT;
	state_ids[ 2] = utlAT_STATE_ID_DIALING;
	state_ids[ 3] = utlAT_STATE_ID_TONE_DIGITS_WAIT;
	state_ids[ 4] = utlAT_STATE_ID_DIALING;
	state_ids[ 5] = utlAT_STATE_ID_DIALING_PAUSED;
	state_ids[ 6] = utlAT_STATE_ID_DIALING;
	state_ids[ 7] = utlAT_STATE_ID_TONE_DIGITS_WAIT;
	state_ids[ 8] = utlAT_STATE_ID_DIALING;
	state_ids[ 9] = utlAT_STATE_ID_PULSE_DIGITS_WAIT;
	state_ids[10] = utlAT_STATE_ID_DIALING;
	state_ids[11] = utlAT_STATE_ID_DIALING_FLASH_ON_WAIT;
	state_ids[12] = utlAT_STATE_ID_DIALING_HOOK_FLASH;
	state_ids[13] = utlAT_STATE_ID_DIALING_FLASH_OFF_WAIT;
	state_ids[14] = utlAT_STATE_ID_DIALING;
	state_ids[15] = utlAT_STATE_ID_TONE_DIGITS_WAIT;
	state_ids[16] = utlAT_STATE_ID_DIALING;
	state_ids[17] = utlAT_STATE_ID_DIALING_DIAL_TONE_WAIT;
	state_ids[18] = utlAT_STATE_ID_DIALING;
	state_ids[19] = utlAT_STATE_ID_TONE_DIGITS_WAIT;
	state_ids[20] = utlAT_STATE_ID_DIALING;
	state_ids[21] = utlAT_STATE_ID_DIALING_RING_BACK_WAIT;
	state_ids[22] = utlAT_STATE_ID_DIALING_SILENCE_WAIT;
	state_ids[23] = utlAT_STATE_ID_DIALING;
	state_ids[24] = utlAT_STATE_ID_RING_BACK_WAIT;
	state_ids[25] = utlAT_STATE_ID_ANSWER_WAIT;
	state_ids[26] = utlAT_STATE_ID_ANSWER_WAIT;
	state_ids[27] = utlAT_STATE_ID_CARRIER_WAIT;
	state_ids[28] = utlAT_STATE_ID_CARRIER_CONFIRM;
	state_ids[29] = utlAT_STATE_ID_ON_LINE;
	state_ids[30] = utlAT_STATE_ID_OFF_LINE;
	state_ids[31] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[32] = utlAT_STATE_ID_ON_HOOK;
	state_ids[33] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.13) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 4)                                 ||
	    (strcmp(test_results.commands[0].name, "E")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 1)                                 ||
	    (strcmp(test_results.commands[1].name, "X")                 != 0)                                 ||
	    (test_results.commands[1].num_parameters                    != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[1].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[1].parameter_values[0].value.decimal != 4)                                 ||
	    (strcmp(test_results.commands[2].name, "D")                 != 0)                                 ||
	    (test_results.commands[2].num_parameters                    != 1)                                 ||
	    (test_results.commands[2].parameter_values[0].type          != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[2].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[2].parameter_values[0].is_default    != false)                             ||
	    (strcmp(test_results.commands[2].parameter_values[0].value.dial_string_p, "T55,5P1212T!66W77@") != 0) ||
	    (strcmp(test_results.commands[3].name, "H")                 != 0)                                 ||
	    (test_results.commands[3].num_parameters                    != 1)                                 ||
	    (test_results.commands[3].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[3].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[3].parameter_values[0].is_default    != true)                              ||
	    (test_results.commands[3].parameter_values[0].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "ATE1X4DT55,5-P1212T!66W77@\r\r\nCARRIER (19200)\r\n+++\r\nOK\r\nATH\r\r\nOK\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.14) failed\n");
		return false;
	}

/*--- incoming call that we answer ---*/
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_CONNECT_TEXT, "230400") != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.15) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SELECT_CONNECT_RESULT_CODE) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.16) failed\n");
		return false;
	}
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 3;
	test_results.driver_request_test_state = 0;
	s_p = "ATS0=5\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.17) failed\n");
		return false;
	}
	if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.18) failed\n");
		return false;
	}
	s_p = "ATA\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.19) failed\n");
		return false;
	}
	sleep(1);
	s_p = "+++";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.20) failed\n");
		return false;
	}
	s_p = "ATH\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.21) failed\n");
		return false;
	}

/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_RINGING;
	state_ids[ 1] = utlAT_STATE_ID_ANSWER;
	state_ids[ 2] = utlAT_STATE_ID_CARRIER_WAIT;
	state_ids[ 3] = utlAT_STATE_ID_CARRIER_CONFIRM;
	state_ids[ 4] = utlAT_STATE_ID_ON_LINE;
	state_ids[ 5] = utlAT_STATE_ID_OFF_LINE;
	state_ids[ 6] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[ 7] = utlAT_STATE_ID_ON_HOOK;
	state_ids[ 8] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.22) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 2)                                 ||
	    (strcmp(test_results.commands[0].name, "A")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 0)                                 ||
	    (strcmp(test_results.commands[1].name, "H")                 != 0)                                 ||
	    (test_results.commands[1].num_parameters                    != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[1].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default    != true)                              ||
	    (test_results.commands[1].parameter_values[0].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "ATS0=5\r\r\nOK\r\n\r\nRING\r\nATA\r\r\nCONNECT (230400)\r\n+++\r\nOK\r\nATH\r\r\nOK\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.23) failed\n");
		return false;
	}

/*--- simple out-going call that is not answered ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test = 4;
	s_p = "ATX4DT555-1212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.24) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_DIAL_TONE_WAIT;
	state_ids[ 2] = utlAT_STATE_ID_DIALING;
	state_ids[ 3] = utlAT_STATE_ID_TONE_DIGITS_WAIT;
	state_ids[ 4] = utlAT_STATE_ID_DIALING;
	state_ids[ 5] = utlAT_STATE_ID_RING_BACK_WAIT;
	state_ids[ 6] = utlAT_STATE_ID_ANSWER_WAIT;
	state_ids[ 7] = utlAT_STATE_ID_ANSWER_WAIT;
	state_ids[ 8] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[ 9] = utlAT_STATE_ID_ON_HOOK;
	state_ids[10] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.25) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 2)                                 ||
	    (strcmp(test_results.commands[0].name, "X")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 4)                                 ||
	    (strcmp(test_results.commands[1].name, "D")                 != 0)                                 ||
	    (test_results.commands[1].num_parameters                    != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type          != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[1].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default    != false)                             ||
	    (strcmp(test_results.commands[1].parameter_values[0].value.dial_string_p, "T5551212") != 0)       ||
	    (strcmp(test_results.reply_string, "ATX4DT555-1212\r\r\nNO ANSWER\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.26) failed\n");
		return false;
	}

/*--- simple out-going call that lacks dial tone ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test = 5;
	s_p = "ATX4DT555-1212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.27) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_DIAL_TONE_WAIT;
	state_ids[ 2] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[ 3] = utlAT_STATE_ID_ON_HOOK;
	state_ids[ 4] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.28) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 2)                                 ||
	    (strcmp(test_results.commands[0].name, "X")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 4)                                 ||
	    (strcmp(test_results.commands[1].name, "D")                 != 0)                                 ||
	    (test_results.commands[1].num_parameters                    != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type          != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[1].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default    != false)                             ||
	    (strcmp(test_results.commands[1].parameter_values[0].value.dial_string_p, "T5551212") != 0)       ||
	    (strcmp(test_results.reply_string, "ATX4DT555-1212\r\r\nNO DIAL TONE\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.29) failed\n");
		return false;
	}

/*--- simple out-going call that is busy ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test = 6;
	s_p = "ATX4DT555-1212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.30) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_DIAL_TONE_WAIT;
	state_ids[ 2] = utlAT_STATE_ID_DIALING;
	state_ids[ 3] = utlAT_STATE_ID_TONE_DIGITS_WAIT;
	state_ids[ 4] = utlAT_STATE_ID_DIALING;
	state_ids[ 5] = utlAT_STATE_ID_RING_BACK_WAIT;
	state_ids[ 6] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[ 7] = utlAT_STATE_ID_ON_HOOK;
	state_ids[ 8] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.31) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 2)                                 ||
	    (strcmp(test_results.commands[0].name, "X")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 4)                                 ||
	    (strcmp(test_results.commands[1].name, "D")                 != 0)                                 ||
	    (test_results.commands[1].num_parameters                    != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type          != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[1].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default    != false)                             ||
	    (strcmp(test_results.commands[1].parameter_values[0].value.dial_string_p, "T5551212") != 0)       ||
	    (strcmp(test_results.reply_string, "ATX4DT555-1212\r\r\nBUSY\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.32) failed\n");
		return false;
	}

/*--- simple out-going call that fails to find a carrier ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test = 7;
	s_p = "ATX4DT555-1212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.33) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_DIAL_TONE_WAIT;
	state_ids[ 2] = utlAT_STATE_ID_DIALING;
	state_ids[ 3] = utlAT_STATE_ID_TONE_DIGITS_WAIT;
	state_ids[ 4] = utlAT_STATE_ID_DIALING;
	state_ids[ 5] = utlAT_STATE_ID_RING_BACK_WAIT;
	state_ids[ 6] = utlAT_STATE_ID_ANSWER_WAIT;
	state_ids[ 7] = utlAT_STATE_ID_ANSWER_WAIT;
	state_ids[ 8] = utlAT_STATE_ID_CARRIER_WAIT;
	state_ids[ 9] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[10] = utlAT_STATE_ID_ON_HOOK;
	state_ids[11] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.34) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 2)                                 ||
	    (strcmp(test_results.commands[0].name, "X")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 4)                                 ||
	    (strcmp(test_results.commands[1].name, "D")                 != 0)                                 ||
	    (test_results.commands[1].num_parameters                    != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type          != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[1].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default    != false)                             ||
	    (strcmp(test_results.commands[1].parameter_values[0].value.dial_string_p, "T5551212") != 0)       ||
	    (strcmp(test_results.reply_string, "ATX4DT555-1212\r\r\nNO CARRIER\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.35) failed\n");
		return false;
	}


/*--- simple answered call that is we abandon during answer ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test = 8;
	s_p = "ATA\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.36) failed\n");
		return false;
	}
	if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.37) failed\n");
		return false;
	}
	if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_HANG_UP) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.38) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[0] = utlAT_STATE_ID_OFF_HOOK_ANSWER_WAIT;
	state_ids[1] = utlAT_STATE_ID_ANSWER;
	state_ids[2] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[3] = utlAT_STATE_ID_ON_HOOK;
	state_ids[4] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.39) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "A")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "ATA\r") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.40) failed\n");
		return false;
	}

/*--- ATA failure due to modem off-hook request failure (synchronous) ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 9;
	test_results.driver_request_test_state = 0;
	s_p = "ATA\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.41) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_ANSWER_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_ON_HOOK;
	state_ids[ 2] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.43) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "A")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "ATA\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.44) failed\n");
		return false;
	}

/*--- ATA failure due to modem off-hook request failure (asynchronous) ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 10;
	test_results.driver_request_test_state = 0;
	s_p = "ATA\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.45) failed\n");
		return false;
	}
	if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_REQUEST_FAILED) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.46) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_ANSWER_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_ON_HOOK;
	state_ids[ 2] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.47) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "A")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "ATA\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.48) failed\n");
		return false;
	}

/*--- ATA failure due to modem answer request failure (synchronous) ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 11;
	test_results.driver_request_test_state = 0;
	s_p = "ATA\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.49) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_ANSWER_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_ANSWER;
	state_ids[ 2] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[ 3] = utlAT_STATE_ID_ON_HOOK;
	state_ids[ 4] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.50) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "A")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "ATA\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.51) failed\n");
		return false;
	}

/*--- ATA failure due to modem answer request failure (asynchronous) ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 12;
	test_results.driver_request_test_state = 0;
	s_p = "ATA\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.52) failed\n");
		return false;
	}
	if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_REQUEST_FAILED) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.53) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_ANSWER_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_ANSWER;
	state_ids[ 2] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[ 3] = utlAT_STATE_ID_ON_HOOK;
	state_ids[ 4] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.54) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "A")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "ATA\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.55) failed\n");
		return false;
	}

/*--- ATA failure due to modem carrier failure (synchronous) ---*/
	utlClearStateMachineHistory(parser_p->states.machine_p);
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 13;
	test_results.driver_request_test_state = 0;
	s_p = "ATA\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.56) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_ANSWER_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_ANSWER;
	state_ids[ 2] = utlAT_STATE_ID_CARRIER_WAIT;
	state_ids[ 3] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[ 4] = utlAT_STATE_ID_ON_HOOK;
	state_ids[ 5] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.57) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "A")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "ATA\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.58) failed\n");
		return false;
	}

/*--- ATA failure due to modem carrier failure (asynchronous) ---*/
	utlClearStateMachineHistory(parser_p->states.machine_p);
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 14;
	test_results.driver_request_test_state = 0;
	s_p = "ATA\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.59) failed\n");
		return false;
	}
	if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_REQUEST_FAILED) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.60) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_ANSWER_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_ANSWER;
	state_ids[ 2] = utlAT_STATE_ID_CARRIER_WAIT;
	state_ids[ 3] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[ 4] = utlAT_STATE_ID_ON_HOOK;
	state_ids[ 5] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.61) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "A")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "ATA\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.62) failed\n");
		return false;
	}

/*--- ATA failure due to modem carrier confirm failure (synchronous) ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 15;
	test_results.driver_request_test_state = 0;
	s_p = "ATA\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.63) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_ANSWER_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_ANSWER;
	state_ids[ 2] = utlAT_STATE_ID_CARRIER_WAIT;
	state_ids[ 3] = utlAT_STATE_ID_CARRIER_CONFIRM;
	state_ids[ 4] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[ 5] = utlAT_STATE_ID_ON_HOOK;
	state_ids[ 6] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.64) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "A")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "ATA\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.65) failed\n");
		return false;
	}

/*--- ATA failure due to modem carrier confirm failure (asynchronous) ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 16;
	test_results.driver_request_test_state = 0;
	s_p = "ATA\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.66) failed\n");
		return false;
	}
	if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_REQUEST_FAILED) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.67) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_ANSWER_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_ANSWER;
	state_ids[ 2] = utlAT_STATE_ID_CARRIER_WAIT;
	state_ids[ 3] = utlAT_STATE_ID_CARRIER_CONFIRM;
	state_ids[ 4] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[ 5] = utlAT_STATE_ID_ON_HOOK;
	state_ids[ 6] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.68) failed\n");
		return false;
	}
	if ((test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "A")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != true)                              ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 0)                                 ||
	    (strcmp(test_results.reply_string, "ATA\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.69) failed\n");
		return false;
	}

/*--- out-going call that fails during off-hook attempt (synchronous) ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 17;
	test_results.driver_request_test_state = 0;
	parser_p->states.dial_string.active = false;
	s_p = "ATDT5551212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.70) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_ON_HOOK;
	state_ids[ 2] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.71) failed\n");
		return false;
	}
	if ((test_results.num_commands                               != 1)                                 ||
	    (strcmp(test_results.commands[1].name, "D")              != 0)                                 ||
	    (test_results.commands[1].num_parameters                 != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type       != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[1].parameter_values[0].access     != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default != false)                             ||
	    (strcmp(test_results.commands[1].parameter_values[0].value.dial_string_p, "T5551212") != 0)    ||
	    (strcmp(test_results.reply_string, "ATDT5551212\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.72) failed\n");
		return false;
	}

/*--- out-going call that fails during off-hook attempt (asynchronous) ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 18;
	test_results.driver_request_test_state = 0;
	parser_p->states.dial_string.active = false;
	s_p = "ATDT5551212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.73) failed\n");
		return false;
	}
	if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_REQUEST_FAILED) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.74) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_ON_HOOK;
	state_ids[ 2] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.75) failed\n");
		return false;
	}
	if ((test_results.num_commands                               != 1)                                 ||
	    (strcmp(test_results.commands[1].name, "D")              != 0)                                 ||
	    (test_results.commands[1].num_parameters                 != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type       != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[1].parameter_values[0].access     != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default != false)                             ||
	    (strcmp(test_results.commands[1].parameter_values[0].value.dial_string_p, "T5551212") != 0)    ||
	    (strcmp(test_results.reply_string, "ATDT5551212\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.76) failed\n");
		return false;
	}

/*--- out-going call that fails during dial-tone wait (synchronous) ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 19;
	test_results.driver_request_test_state = 0;
	parser_p->states.dial_string.active = false;
	s_p = "ATDT5551212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.77) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_DIAL_TONE_WAIT;
	state_ids[ 2] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[ 3] = utlAT_STATE_ID_ON_HOOK;
	state_ids[ 4] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.78) failed\n");
		return false;
	}
	if ((test_results.num_commands                               != 1)                                 ||
	    (strcmp(test_results.commands[1].name, "D")              != 0)                                 ||
	    (test_results.commands[1].num_parameters                 != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type       != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[1].parameter_values[0].access     != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default != false)                             ||
	    (strcmp(test_results.commands[1].parameter_values[0].value.dial_string_p, "T5551212") != 0)    ||
	    (strcmp(test_results.reply_string, "ATDT5551212\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.79) failed\n");
		return false;
	}

/*--- out-going call that fails during dial-tone wait (asynchronous) ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 20;
	test_results.driver_request_test_state = 0;
	parser_p->states.dial_string.active = false;
	s_p = "ATDT5551212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.80) failed\n");
		return false;
	}
	if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_REQUEST_FAILED) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.81) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_DIAL_TONE_WAIT;
	state_ids[ 2] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[ 3] = utlAT_STATE_ID_ON_HOOK;
	state_ids[ 4] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.82) failed\n");
		return false;
	}
	if ((test_results.num_commands                               != 1)                                 ||
	    (strcmp(test_results.commands[1].name, "D")              != 0)                                 ||
	    (test_results.commands[1].num_parameters                 != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type       != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[1].parameter_values[0].access     != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default != false)                             ||
	    (strcmp(test_results.commands[1].parameter_values[0].value.dial_string_p, "T5551212") != 0)    ||
	    (strcmp(test_results.reply_string, "ATDT5551212\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.83) failed\n");
		return false;
	}

/*--- out-going call that fails during tone dialing (synchronous) ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 21;
	test_results.driver_request_test_state = 0;
	parser_p->states.dial_string.active = false;
	s_p = "ATDT5551212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.84) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_DIAL_TONE_WAIT;
	state_ids[ 2] = utlAT_STATE_ID_DIALING;
	state_ids[ 3] = utlAT_STATE_ID_TONE_DIGITS_WAIT;
	state_ids[ 4] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[ 5] = utlAT_STATE_ID_ON_HOOK;
	state_ids[ 6] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.85) failed\n");
		return false;
	}
	if ((test_results.num_commands                               != 1)                                 ||
	    (strcmp(test_results.commands[1].name, "D")              != 0)                                 ||
	    (test_results.commands[1].num_parameters                 != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type       != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[1].parameter_values[0].access     != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default != false)                             ||
	    (strcmp(test_results.commands[1].parameter_values[0].value.dial_string_p, "T5551212") != 0)    ||
	    (strcmp(test_results.reply_string, "ATDT5551212\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.86) failed\n");
		return false;
	}

/*--- out-going call that fails during tone dialing (asynchronous) ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 22;
	test_results.driver_request_test_state = 0;
	parser_p->states.dial_string.active = false;
	s_p = "ATDT5551212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.87) failed\n");
		return false;
	}
	if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_REQUEST_FAILED) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.88) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_DIAL_TONE_WAIT;
	state_ids[ 2] = utlAT_STATE_ID_DIALING;
	state_ids[ 3] = utlAT_STATE_ID_TONE_DIGITS_WAIT;
	state_ids[ 4] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[ 5] = utlAT_STATE_ID_ON_HOOK;
	state_ids[ 6] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.89) failed\n");
		return false;
	}
	if ((test_results.num_commands                               != 1)                                 ||
	    (strcmp(test_results.commands[1].name, "D")              != 0)                                 ||
	    (test_results.commands[1].num_parameters                 != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type       != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[1].parameter_values[0].access     != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default != false)                             ||
	    (strcmp(test_results.commands[1].parameter_values[0].value.dial_string_p, "T5551212") != 0)    ||
	    (strcmp(test_results.reply_string, "ATDT5551212\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.90) failed\n");
		return false;
	}

/*--- out-going call that fails during pulse dialing (synchronous) ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 21;
	test_results.driver_request_test_state = 0;
	parser_p->states.dial_string.active = false;
	s_p = "ATDP5551212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.91) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_DIAL_TONE_WAIT;
	state_ids[ 2] = utlAT_STATE_ID_DIALING;
	state_ids[ 3] = utlAT_STATE_ID_PULSE_DIGITS_WAIT;
	state_ids[ 4] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[ 5] = utlAT_STATE_ID_ON_HOOK;
	state_ids[ 6] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.92) failed\n");
		return false;
	}
	if ((test_results.num_commands                               != 1)                                 ||
	    (strcmp(test_results.commands[1].name, "D")              != 0)                                 ||
	    (test_results.commands[1].num_parameters                 != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type       != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[1].parameter_values[0].access     != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default != false)                             ||
	    (strcmp(test_results.commands[1].parameter_values[0].value.dial_string_p, "P5551212") != 0)    ||
	    (strcmp(test_results.reply_string, "ATDP5551212\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.93) failed\n");
		return false;
	}

/*--- out-going call that fails during pulse dialing (asynchronous) ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 22;
	test_results.driver_request_test_state = 0;
	parser_p->states.dial_string.active = false;
	s_p = "ATDP5551212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.94) failed\n");
		return false;
	}
	if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_REQUEST_FAILED) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.95) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_DIAL_TONE_WAIT;
	state_ids[ 2] = utlAT_STATE_ID_DIALING;
	state_ids[ 3] = utlAT_STATE_ID_PULSE_DIGITS_WAIT;
	state_ids[ 4] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[ 5] = utlAT_STATE_ID_ON_HOOK;
	state_ids[ 6] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.96) failed\n");
		return false;
	}
	if ((test_results.num_commands                               != 1)                                 ||
	    (strcmp(test_results.commands[1].name, "D")              != 0)                                 ||
	    (test_results.commands[1].num_parameters                 != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type       != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[1].parameter_values[0].access     != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default != false)                             ||
	    (strcmp(test_results.commands[1].parameter_values[0].value.dial_string_p, "P5551212") != 0)    ||
	    (strcmp(test_results.reply_string, "ATDP5551212\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.97) failed\n");
		return false;
	}

/*--- out-going call that fails during ring-back wait (synchronous) ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 23;
	test_results.driver_request_test_state = 0;
	parser_p->states.dial_string.active = false;
	s_p = "ATD5551212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.98) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_DIAL_TONE_WAIT;
	state_ids[ 2] = utlAT_STATE_ID_DIALING;
	state_ids[ 3] = utlAT_STATE_ID_PULSE_DIGITS_WAIT;
	state_ids[ 4] = utlAT_STATE_ID_DIALING;
	state_ids[ 5] = utlAT_STATE_ID_RING_BACK_WAIT;
	state_ids[ 6] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[ 7] = utlAT_STATE_ID_ON_HOOK;
	state_ids[ 8] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.99) failed\n");
		return false;
	}
	if ((test_results.num_commands                               != 1)                                 ||
	    (strcmp(test_results.commands[1].name, "D")              != 0)                                 ||
	    (test_results.commands[1].num_parameters                 != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type       != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[1].parameter_values[0].access     != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default != false)                             ||
	    (strcmp(test_results.commands[1].parameter_values[0].value.dial_string_p, "5551212") != 0)    ||
	    (strcmp(test_results.reply_string, "ATD5551212\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.100) failed\n");
		return false;
	}

/*--- out-going call that fails during ring-back wait (asynchronous) ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 24;
	test_results.driver_request_test_state = 0;
	parser_p->states.dial_string.active = false;
	s_p = "ATD5551212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.101) failed\n");
		return false;
	}
	if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_REQUEST_FAILED) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.102) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_DIAL_TONE_WAIT;
	state_ids[ 2] = utlAT_STATE_ID_DIALING;
	state_ids[ 3] = utlAT_STATE_ID_PULSE_DIGITS_WAIT;
	state_ids[ 4] = utlAT_STATE_ID_DIALING;
	state_ids[ 5] = utlAT_STATE_ID_RING_BACK_WAIT;
	state_ids[ 6] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[ 7] = utlAT_STATE_ID_ON_HOOK;
	state_ids[ 8] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.103) failed\n");
		return false;
	}
	if ((test_results.num_commands                               != 1)                                 ||
	    (strcmp(test_results.commands[1].name, "D")              != 0)                                 ||
	    (test_results.commands[1].num_parameters                 != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type       != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[1].parameter_values[0].access     != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default != false)                             ||
	    (strcmp(test_results.commands[1].parameter_values[0].value.dial_string_p, "5551212") != 0)    ||
	    (strcmp(test_results.reply_string, "ATD5551212\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.104) failed\n");
		return false;
	}

/*--- out-going call that fails during answer wait (synchronous) ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 25;
	test_results.driver_request_test_state = 0;
	parser_p->states.dial_string.active = false;
	s_p = "ATD5551212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.105) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_DIAL_TONE_WAIT;
	state_ids[ 2] = utlAT_STATE_ID_DIALING;
	state_ids[ 3] = utlAT_STATE_ID_PULSE_DIGITS_WAIT;
	state_ids[ 4] = utlAT_STATE_ID_DIALING;
	state_ids[ 5] = utlAT_STATE_ID_RING_BACK_WAIT;
	state_ids[ 6] = utlAT_STATE_ID_ANSWER_WAIT;
	state_ids[ 7] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[ 8] = utlAT_STATE_ID_ON_HOOK;
	state_ids[ 9] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.106) failed\n");
		return false;
	}
	if ((test_results.num_commands                               != 1)                                 ||
	    (strcmp(test_results.commands[1].name, "D")              != 0)                                 ||
	    (test_results.commands[1].num_parameters                 != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type       != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[1].parameter_values[0].access     != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default != false)                             ||
	    (strcmp(test_results.commands[1].parameter_values[0].value.dial_string_p, "5551212") != 0)    ||
	    (strcmp(test_results.reply_string, "ATD5551212\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.107) failed\n");
		return false;
	}

/*--- out-going call that fails during answer wait (asynchronous) ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 26;
	test_results.driver_request_test_state = 0;
	parser_p->states.dial_string.active = false;
	s_p = "ATD5551212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.108) failed\n");
		return false;
	}
	if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_REQUEST_FAILED) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.109) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_DIAL_TONE_WAIT;
	state_ids[ 2] = utlAT_STATE_ID_DIALING;
	state_ids[ 3] = utlAT_STATE_ID_PULSE_DIGITS_WAIT;
	state_ids[ 4] = utlAT_STATE_ID_DIALING;
	state_ids[ 5] = utlAT_STATE_ID_RING_BACK_WAIT;
	state_ids[ 6] = utlAT_STATE_ID_ANSWER_WAIT;
	state_ids[ 7] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[ 8] = utlAT_STATE_ID_ON_HOOK;
	state_ids[ 9] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.110) failed\n");
		return false;
	}
	if ((test_results.num_commands                               != 1)                                 ||
	    (strcmp(test_results.commands[1].name, "D")              != 0)                                 ||
	    (test_results.commands[1].num_parameters                 != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type       != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[1].parameter_values[0].access     != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default != false)                             ||
	    (strcmp(test_results.commands[1].parameter_values[0].value.dial_string_p, "5551212") != 0)    ||
	    (strcmp(test_results.reply_string, "ATD5551212\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.111) failed\n");
		return false;
	}

/*--- out-going call that fails during answer wait (synchronous) ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 27;
	test_results.driver_request_test_state = 0;
	parser_p->states.dial_string.active = false;
	s_p = "ATD5551212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.112) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_DIAL_TONE_WAIT;
	state_ids[ 2] = utlAT_STATE_ID_DIALING;
	state_ids[ 3] = utlAT_STATE_ID_PULSE_DIGITS_WAIT;
	state_ids[ 4] = utlAT_STATE_ID_DIALING;
	state_ids[ 5] = utlAT_STATE_ID_RING_BACK_WAIT;
	state_ids[ 6] = utlAT_STATE_ID_ANSWER_WAIT;
	state_ids[ 7] = utlAT_STATE_ID_CARRIER_WAIT;
	state_ids[ 8] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[ 9] = utlAT_STATE_ID_ON_HOOK;
	state_ids[10] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.113) failed\n");
		return false;
	}
	if ((test_results.num_commands                               != 1)                                 ||
	    (strcmp(test_results.commands[1].name, "D")              != 0)                                 ||
	    (test_results.commands[1].num_parameters                 != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type       != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[1].parameter_values[0].access     != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default != false)                             ||
	    (strcmp(test_results.commands[1].parameter_values[0].value.dial_string_p, "5551212") != 0)    ||
	    (strcmp(test_results.reply_string, "ATD5551212\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.114) failed\n");
		return false;
	}

/*--- out-going call that fails during answer wait (asynchronous) ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 28;
	test_results.driver_request_test_state = 0;
	parser_p->states.dial_string.active = false;
	s_p = "ATD5551212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.115) failed\n");
		return false;
	}
	if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_REQUEST_FAILED) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.116) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_DIAL_TONE_WAIT;
	state_ids[ 2] = utlAT_STATE_ID_DIALING;
	state_ids[ 3] = utlAT_STATE_ID_PULSE_DIGITS_WAIT;
	state_ids[ 4] = utlAT_STATE_ID_DIALING;
	state_ids[ 5] = utlAT_STATE_ID_RING_BACK_WAIT;
	state_ids[ 6] = utlAT_STATE_ID_ANSWER_WAIT;
	state_ids[ 7] = utlAT_STATE_ID_CARRIER_WAIT;
	state_ids[ 8] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[ 9] = utlAT_STATE_ID_ON_HOOK;
	state_ids[10] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.117) failed\n");
		return false;
	}
	if ((test_results.num_commands                               != 1)                                 ||
	    (strcmp(test_results.commands[1].name, "D")              != 0)                                 ||
	    (test_results.commands[1].num_parameters                 != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type       != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[1].parameter_values[0].access     != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default != false)                             ||
	    (strcmp(test_results.commands[1].parameter_values[0].value.dial_string_p, "5551212") != 0)    ||
	    (strcmp(test_results.reply_string, "ATD5551212\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.118) failed\n");
		return false;
	}

/*--- out-going call that fails during answer wait (synchronous) ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 29;
	test_results.driver_request_test_state = 0;
	parser_p->states.dial_string.active = false;
	s_p = "ATD5551212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.119) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_DIAL_TONE_WAIT;
	state_ids[ 2] = utlAT_STATE_ID_DIALING;
	state_ids[ 3] = utlAT_STATE_ID_PULSE_DIGITS_WAIT;
	state_ids[ 4] = utlAT_STATE_ID_DIALING;
	state_ids[ 5] = utlAT_STATE_ID_RING_BACK_WAIT;
	state_ids[ 6] = utlAT_STATE_ID_ANSWER_WAIT;
	state_ids[ 7] = utlAT_STATE_ID_CARRIER_WAIT;
	state_ids[ 8] = utlAT_STATE_ID_CARRIER_CONFIRM;
	state_ids[ 9] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[10] = utlAT_STATE_ID_ON_HOOK;
	state_ids[11] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.120) failed\n");
		return false;
	}
	if ((test_results.num_commands                               != 1)                                 ||
	    (strcmp(test_results.commands[1].name, "D")              != 0)                                 ||
	    (test_results.commands[1].num_parameters                 != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type       != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[1].parameter_values[0].access     != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default != false)                             ||
	    (strcmp(test_results.commands[1].parameter_values[0].value.dial_string_p, "5551212") != 0)    ||
	    (strcmp(test_results.reply_string, "ATD5551212\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.121) failed\n");
		return false;
	}

/*--- out-going call that fails during answer wait (asynchronous) ---*/
	CLEAR_TEST_RESULTS();
	utlClearStateMachineHistory(parser_p->states.machine_p);
	test_results.driver_request_test       = 30;
	test_results.driver_request_test_state = 0;
	parser_p->states.dial_string.active = false;
	s_p = "ATD5551212\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.122) failed\n");
		return false;
	}
	if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_REQUEST_FAILED) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(26.123) failed\n");
		return false;
	}
/* utlAT_STATE_ID_ON_HOOK was cleared */
	state_ids[ 0] = utlAT_STATE_ID_OFF_HOOK_WAIT;
	state_ids[ 1] = utlAT_STATE_ID_DIAL_TONE_WAIT;
	state_ids[ 2] = utlAT_STATE_ID_DIALING;
	state_ids[ 3] = utlAT_STATE_ID_PULSE_DIGITS_WAIT;
	state_ids[ 4] = utlAT_STATE_ID_DIALING;
	state_ids[ 5] = utlAT_STATE_ID_RING_BACK_WAIT;
	state_ids[ 6] = utlAT_STATE_ID_ANSWER_WAIT;
	state_ids[ 7] = utlAT_STATE_ID_CARRIER_WAIT;
	state_ids[ 8] = utlAT_STATE_ID_CARRIER_CONFIRM;
	state_ids[ 9] = utlAT_STATE_ID_ON_HOOK_WAIT;
	state_ids[10] = utlAT_STATE_ID_ON_HOOK;
	state_ids[11] = utlSTATE_ID_NULL;
	if (utlIsStateMachineStateSequence(parser_p->states.machine_p, state_ids) != true)
	{
		utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
		fprintf(stderr, "atParserTest: utlAtParse(26.124) failed\n");
		return false;
	}
	if ((test_results.num_commands                               != 1)                                 ||
	    (strcmp(test_results.commands[1].name, "D")              != 0)                                 ||
	    (test_results.commands[1].num_parameters                 != 1)                                 ||
	    (test_results.commands[1].parameter_values[0].type       != utlAT_DATA_TYPE_DIAL_STRING)       ||
	    (test_results.commands[1].parameter_values[0].access     != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[1].parameter_values[0].is_default != false)                             ||
	    (strcmp(test_results.commands[1].parameter_values[0].value.dial_string_p, "5551212") != 0)    ||
	    (strcmp(test_results.reply_string, "ATD5551212\r\r\nERROR\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(26.125) failed\n");
		return false;
	}


/*--- Asynchronous Gets ----------------------------------------------------*/

/*--- simple asynchronous get-info ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+AINFO?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.1) failed\n");
		return false;
	}
	if ((test_async_xid                                                 == utlFAILED)                         ||
	    (test_results.num_commands                                      != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+AINFO")                != 0)                                 ||
	    (test_results.commands[0].num_parameters                        != 6)                                 ||
	    (test_results.commands[0].parameter_values[0].type              != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default        != true)                              ||
	    (test_results.commands[0].parameter_values[0].value.decimal     != 0)                                 ||
	    (test_results.commands[0].parameter_values[1].type              != utlAT_DATA_TYPE_HEXADECIMAL)       ||
	    (test_results.commands[0].parameter_values[1].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default        != true)                              ||
	    (test_results.commands[0].parameter_values[1].value.hexadecimal != 0)                                 ||
	    (test_results.commands[0].parameter_values[2].type              != utlAT_DATA_TYPE_BINARY)            ||
	    (test_results.commands[0].parameter_values[2].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[2].is_default        != true)                              ||
	    (test_results.commands[0].parameter_values[2].value.binary      != 0)                                 ||
	    (test_results.commands[0].parameter_values[3].type              != utlAT_DATA_TYPE_STRING)            ||
	    (test_results.commands[0].parameter_values[3].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[3].is_default        != true)                              ||
	    (strcmp(test_results.commands[0].parameter_values[3].value.string_p, "") != 0)                        ||
	    (test_results.commands[0].parameter_values[4].type              != utlAT_DATA_TYPE_QSTRING)           ||
	    (test_results.commands[0].parameter_values[4].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[4].is_default        != true)                              ||
	    (strcmp(test_results.commands[0].parameter_values[4].value.string_p, "") != 0)                        ||
	    (test_results.commands[0].parameter_values[5].type              != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[5].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[5].is_default        != true)                              ||
	    (test_results.commands[0].parameter_values[5].value.decimal     != 0)                                 ||
	    (strcmp(test_results.reply_string, "AT+AINFO?\r") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(27.2) failed\n");
		return false;
	}
	if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_CUSTOM, "the quick brown fox") != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.3) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "AT+AINFO?\r\r\nthe quick brown fox\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.4) failed\n");
		return false;
	}


/*--- simple asynchronous get parameters ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+APARAM?\r";

	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.5) failed\n");
		return false;
	}
	if ((test_async_xid                                                 == utlFAILED)                         ||
	    (test_results.num_commands                                      != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+APARAM")               != 0)                                 ||
	    (test_results.commands[0].num_parameters                        != 6)                                 ||
	    (test_results.commands[0].parameter_values[0].type              != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default        != true)                              ||
	    (test_results.commands[0].parameter_values[0].value.decimal     != 0)                                 ||
	    (test_results.commands[0].parameter_values[1].type              != utlAT_DATA_TYPE_HEXADECIMAL)       ||
	    (test_results.commands[0].parameter_values[1].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default        != true)                              ||
	    (test_results.commands[0].parameter_values[1].value.hexadecimal != 0)                                 ||
	    (test_results.commands[0].parameter_values[2].type              != utlAT_DATA_TYPE_BINARY)            ||
	    (test_results.commands[0].parameter_values[2].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[2].is_default        != true)                              ||
	    (test_results.commands[0].parameter_values[2].value.binary      != 0)                                 ||
	    (test_results.commands[0].parameter_values[3].type              != utlAT_DATA_TYPE_STRING)            ||
	    (test_results.commands[0].parameter_values[3].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[3].is_default        != true)                              ||
	    (strcmp(test_results.commands[0].parameter_values[3].value.string_p, "") != 0)                        ||
	    (test_results.commands[0].parameter_values[4].type              != utlAT_DATA_TYPE_QSTRING)           ||
	    (test_results.commands[0].parameter_values[4].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[4].is_default        != true)                              ||
	    (strcmp(test_results.commands[0].parameter_values[4].value.string_p, "") != 0)                        ||
	    (test_results.commands[0].parameter_values[5].type              != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[5].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[5].is_default        != true)                              ||
	    (test_results.commands[0].parameter_values[5].value.decimal     != 0)                                 ||
	    (strcmp(test_results.reply_string, "AT+APARAM?\r") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(27.6) failed\n");
		return false;
	}
	{
		utlAtParameterValue_T parameter_values[10];
		size_t n;

		if ((n = utlAtParserInitCommandParameters(test_async_xid, parameter_values, 10)) == utlFAILED)
		{
			fprintf(stderr, "atParserTest: utlAtParse(27.7) failed\n");
			return false;
		}

		if ((n                                     != 6)                                 ||
		    (parameter_values[0].type              != utlAT_DATA_TYPE_DECIMAL)           ||
		    (parameter_values[0].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
		    (parameter_values[0].is_default        != true)                              ||
		    (parameter_values[0].value.decimal     != 0)                                 ||
		    (parameter_values[1].type              != utlAT_DATA_TYPE_HEXADECIMAL)       ||
		    (parameter_values[1].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
		    (parameter_values[1].is_default        != true)                              ||
		    (parameter_values[1].value.hexadecimal != 0)                                 ||
		    (parameter_values[2].type              != utlAT_DATA_TYPE_BINARY)            ||
		    (parameter_values[2].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
		    (parameter_values[2].is_default        != true)                              ||
		    (parameter_values[2].value.binary      != 0)                                 ||
		    (parameter_values[3].type              != utlAT_DATA_TYPE_STRING)            ||
		    (parameter_values[3].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
		    (parameter_values[3].is_default        != true)                              ||
		    (strcmp(parameter_values[3].value.string_p, "") != 0)                        ||
		    (parameter_values[4].type              != utlAT_DATA_TYPE_QSTRING)           ||
		    (parameter_values[4].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
		    (parameter_values[4].is_default        != true)                              ||
		    (strcmp(parameter_values[4].value.string_p, "") != 0)                        ||
		    (parameter_values[5].type              != utlAT_DATA_TYPE_DECIMAL)           ||
		    (parameter_values[5].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
		    (parameter_values[5].is_default        != true)                              ||
		    (parameter_values[5].value.decimal     != 0))
		{
			dumpTestResults(stderr);
			fprintf(stderr, "atParserTest: utlAtParse(27.8) failed\n");
			return false;
		}

		parameter_values[0].is_default        = false;
		parameter_values[0].value.decimal     = 9;
		parameter_values[1].is_default        = false;
		parameter_values[1].value.hexadecimal = 0xa;
		parameter_values[2].is_default        = false;
		parameter_values[2].value.binary      = 11;
		parameter_values[3].is_default        = false;
		parameter_values[3].value.string_p    = "abc";
		parameter_values[4].is_default        = false;
		parameter_values[4].value.string_p    = "abc";
		parameter_values[5].is_default        = false;
		parameter_values[5].value.decimal     = 9;

		if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_PARAMETERS, &parameter_values, n) != utlSUCCESS)
		{
			fprintf(stderr, "atParserTest: utlAtParse(27.9) failed\n");
			return false;
		}
	}
	if (strcmp(test_results.reply_string, "AT+APARAM?\r\r\n9,A,1011,\"abc\",\"abc\",9\r\n\r\nOK\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.10) failed\n");
		return false;
	}


/*--- simple asynchronous abort ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+APARAM?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.11) failed\n");
		return false;
	}
	if (test_async_xid == utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.12) failed\n");
		return false;
	}
	if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_ABORT, "abort3") != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.13) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "AT+APARAM?\r\r\nabort3\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.14) failed\n");
		return false;
	}

/*--- simple asynchronous error ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+APARAM?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.15) failed\n");
		return false;
	}
	if (test_async_xid == utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.16) failed\n");
		return false;
	}
	if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_ERROR) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.17) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "AT+APARAM?\r\r\nERROR\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.18) failed\n");
		return false;
	}


/*--- async "GET" info with subsequent pending AT commands ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+AINFO?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.19) failed\n");
		return false;
	}
	s_p = "AT+GMI\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.20) failed\n");
		return false;
	}
	s_p = "AT+GCI\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.21) failed\n");
		return false;
	}
	if (test_async_xid == utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.22) failed\n");
		return false;
	}
	if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_CUSTOM, "the quick brown fox") != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.23) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "AT+AINFO?\r\r\nthe quick brown fox\r\nAT+GMI\r\r\nIntel\r\n\r\nOK\r\nAT+GCI\r\r\n+GCI: 20\r\n\r\nOK\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.24) failed\n");
		return false;
	}

/*--- async "GET" params with subsequent pending AT commands ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+APARAM?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.25) failed\n");
		return false;
	}
	s_p = "AT+GMI\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.26) failed\n");
		return false;
	}
	s_p = "AT+GCI\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.27) failed\n");
		return false;
	}
	if (test_async_xid == utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.28) failed\n");
		return false;
	}
	{
		utlAtParameterValue_T parameter_values[10];
		size_t n;

		if ((n = utlAtParserInitCommandParameters(test_async_xid, parameter_values, 10)) == utlFAILED)
		{
			fprintf(stderr, "atParserTest: utlAtParse(27.29) failed\n");
			return false;
		}

		if ((n                                     != 6)                                 ||
		    (parameter_values[0].type              != utlAT_DATA_TYPE_DECIMAL)           ||
		    (parameter_values[0].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
		    (parameter_values[0].is_default        != true)                              ||
		    (parameter_values[0].value.decimal     != 0)                                 ||
		    (parameter_values[1].type              != utlAT_DATA_TYPE_HEXADECIMAL)       ||
		    (parameter_values[1].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
		    (parameter_values[1].is_default        != true)                              ||
		    (parameter_values[1].value.hexadecimal != 0)                                 ||
		    (parameter_values[2].type              != utlAT_DATA_TYPE_BINARY)            ||
		    (parameter_values[2].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
		    (parameter_values[2].is_default        != true)                              ||
		    (parameter_values[2].value.binary      != 0)                                 ||
		    (parameter_values[3].type              != utlAT_DATA_TYPE_STRING)            ||
		    (parameter_values[3].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
		    (parameter_values[3].is_default        != true)                              ||
		    (strcmp(parameter_values[3].value.string_p, "") != 0)                        ||
		    (parameter_values[4].type              != utlAT_DATA_TYPE_QSTRING)           ||
		    (parameter_values[4].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
		    (parameter_values[4].is_default        != true)                              ||
		    (strcmp(parameter_values[4].value.string_p, "") != 0)                        ||
		    (parameter_values[5].type              != utlAT_DATA_TYPE_DECIMAL)           ||
		    (parameter_values[5].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
		    (parameter_values[5].is_default        != true)                              ||
		    (parameter_values[5].value.decimal     != 0))
		{
			dumpTestResults(stderr);
			fprintf(stderr, "atParserTest: utlAtParse(27.30) failed\n");
			return false;
		}

		parameter_values[0].is_default        = false;
		parameter_values[0].value.decimal     = 9;
		parameter_values[1].is_default        = false;
		parameter_values[1].value.hexadecimal = 0xa;
		parameter_values[2].is_default        = false;
		parameter_values[2].value.binary      = 11;
		parameter_values[3].is_default        = false;
		parameter_values[3].value.string_p    = "abc";
		parameter_values[4].is_default        = false;
		parameter_values[4].value.string_p    = "abc";
		parameter_values[5].is_default        = false;
		parameter_values[5].value.decimal     = 9;

		if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_PARAMETERS, &parameter_values, n) != utlSUCCESS)
		{
			fprintf(stderr, "atParserTest: utlAtParse(27.31) failed\n");
			return false;
		}
	}
	if (strcmp(test_results.reply_string, "AT+APARAM?\r\r\n9,A,1011,\"abc\",\"abc\",9\r\n\r\nOK\r\nAT+GMI\r\r\nIntel\r\n\r\nOK\r\nAT+GCI\r\r\n+GCI: 20\r\n\r\nOK\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.32) failed\n");
		return false;
	}

/*--- multiple simultanius async "GET" requests ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+AINFO?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.33) failed\n");
		return false;
	}
	s_p = "AT+APARAM?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.34) failed\n");
		return false;
	}
	s_p = "AT+AINFO?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.35) failed\n");
		return false;
	}
	if (test_async_xid == utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.36) failed\n");
		return false;
	}
	if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_CUSTOM, "the quick brown fox") != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.37) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "AT+AINFO?\r\r\nthe quick brown fox\r\nAT+APARAM?\r") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.38) failed\n");
		return false;
	}
	{
		utlAtParameterValue_T parameter_values[10];
		size_t n;

		if ((n = utlAtParserInitCommandParameters(test_async_xid, parameter_values, 10)) == utlFAILED)
		{
			fprintf(stderr, "atParserTest: utlAtParse(27.39) failed\n");
			return false;
		}

		if ((n                                     != 6)                                 ||
		    (parameter_values[0].type              != utlAT_DATA_TYPE_DECIMAL)           ||
		    (parameter_values[0].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
		    (parameter_values[0].is_default        != true)                              ||
		    (parameter_values[0].value.decimal     != 0)                                 ||
		    (parameter_values[1].type              != utlAT_DATA_TYPE_HEXADECIMAL)       ||
		    (parameter_values[1].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
		    (parameter_values[1].is_default        != true)                              ||
		    (parameter_values[1].value.hexadecimal != 0)                                 ||
		    (parameter_values[2].type              != utlAT_DATA_TYPE_BINARY)            ||
		    (parameter_values[2].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
		    (parameter_values[2].is_default        != true)                              ||
		    (parameter_values[2].value.binary      != 0)                                 ||
		    (parameter_values[3].type              != utlAT_DATA_TYPE_STRING)            ||
		    (parameter_values[3].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
		    (parameter_values[3].is_default        != true)                              ||
		    (strcmp(parameter_values[3].value.string_p, "") != 0)                        ||
		    (parameter_values[4].type              != utlAT_DATA_TYPE_QSTRING)           ||
		    (parameter_values[4].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
		    (parameter_values[4].is_default        != true)                              ||
		    (strcmp(parameter_values[4].value.string_p, "") != 0)                        ||
		    (parameter_values[5].type              != utlAT_DATA_TYPE_DECIMAL)           ||
		    (parameter_values[5].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
		    (parameter_values[5].is_default        != true)                              ||
		    (parameter_values[5].value.decimal     != 0))
		{
			dumpTestResults(stderr);
			fprintf(stderr, "atParserTest: utlAtParse(27.40) failed\n");
			return false;
		}

		parameter_values[0].is_default        = false;
		parameter_values[0].value.decimal     = 9;
		parameter_values[1].is_default        = false;
		parameter_values[1].value.hexadecimal = 0xa;
		parameter_values[2].is_default        = false;
		parameter_values[2].value.binary      = 11;
		parameter_values[3].is_default        = false;
		parameter_values[3].value.string_p    = "abc";
		parameter_values[4].is_default        = false;
		parameter_values[4].value.string_p    = "abc";
		parameter_values[5].is_default        = false;
		parameter_values[5].value.decimal     = 9;

		if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_PARAMETERS, &parameter_values, n) != utlSUCCESS)
		{
			fprintf(stderr, "atParserTest: utlAtParse(27.41) failed\n");
			return false;
		}
	}
	if (strcmp(test_results.reply_string, "AT+AINFO?\r\r\nthe quick brown fox\r\nAT+APARAM?\r\r\n9,A,1011,\"abc\",\"abc\",9\r\n\r\nOK\r\nAT+AINFO?\r") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.42) failed\n");
		return false;
	}
	if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_CUSTOM, "the quick brown fox") != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.43) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "AT+AINFO?\r\r\nthe quick brown fox\r\nAT+APARAM?\r\r\n9,A,1011,\"abc\",\"abc\",9\r\n\r\nOK\r\nAT+AINFO?\r\r\nthe quick brown fox\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.44) failed\n");
		return false;
	}


/*--- simple asynchronous get-info with synchronous reply ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+SINFO?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.45) failed\n");
		return false;
	}
	if ((test_async_xid                                                 == utlFAILED)                         ||
	    (test_results.num_commands                                      != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+SINFO")                != 0)                                 ||
	    (test_results.commands[0].num_parameters                        != 6)                                 ||
	    (test_results.commands[0].parameter_values[0].type              != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default        != true)                              ||
	    (test_results.commands[0].parameter_values[0].value.decimal     != 0)                                 ||
	    (test_results.commands[0].parameter_values[1].type              != utlAT_DATA_TYPE_HEXADECIMAL)       ||
	    (test_results.commands[0].parameter_values[1].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default        != true)                              ||
	    (test_results.commands[0].parameter_values[1].value.hexadecimal != 0)                                 ||
	    (test_results.commands[0].parameter_values[2].type              != utlAT_DATA_TYPE_BINARY)            ||
	    (test_results.commands[0].parameter_values[2].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[2].is_default        != true)                              ||
	    (test_results.commands[0].parameter_values[2].value.binary      != 0)                                 ||
	    (test_results.commands[0].parameter_values[3].type              != utlAT_DATA_TYPE_STRING)            ||
	    (test_results.commands[0].parameter_values[3].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[3].is_default        != true)                              ||
	    (strcmp(test_results.commands[0].parameter_values[3].value.string_p, "") != 0)                        ||
	    (test_results.commands[0].parameter_values[4].type              != utlAT_DATA_TYPE_QSTRING)           ||
	    (test_results.commands[0].parameter_values[4].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[4].is_default        != true)                              ||
	    (strcmp(test_results.commands[0].parameter_values[4].value.string_p, "") != 0)                        ||
	    (test_results.commands[0].parameter_values[5].type              != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[5].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[5].is_default        != true)                              ||
	    (test_results.commands[0].parameter_values[5].value.decimal     != 0)                                 ||
	    (strcmp(test_results.reply_string, "AT+SINFO?\r\r\nthe quick brown fox\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(27.46) failed\n");
		return false;
	}


/*--- simple asynchronous get parameters with synchronous reply ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+SPARAM?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.47) failed\n");
		return false;
	}
	if ((test_async_xid                                                 == utlFAILED)                         ||
	    (test_results.num_commands                                      != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+SPARAM")               != 0)                                 ||
	    (test_results.commands[0].num_parameters                        != 6)                                 ||
	    (test_results.commands[0].parameter_values[0].type              != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal     != 9)                                 ||
	    (test_results.commands[0].parameter_values[1].type              != utlAT_DATA_TYPE_HEXADECIMAL)       ||
	    (test_results.commands[0].parameter_values[1].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[1].value.hexadecimal != 10)                                ||
	    (test_results.commands[0].parameter_values[2].type              != utlAT_DATA_TYPE_BINARY)            ||
	    (test_results.commands[0].parameter_values[2].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[2].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[2].value.binary      != 11)                                ||
	    (test_results.commands[0].parameter_values[3].type              != utlAT_DATA_TYPE_STRING)            ||
	    (test_results.commands[0].parameter_values[3].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[3].is_default        != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[3].value.string_p, "abc") != 0)                     ||
	    (test_results.commands[0].parameter_values[4].type              != utlAT_DATA_TYPE_QSTRING)           ||
	    (test_results.commands[0].parameter_values[4].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[4].is_default        != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[4].value.string_p, "abc") != 0)                     ||
	    (test_results.commands[0].parameter_values[5].type              != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[5].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[5].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[5].value.decimal     != 9)                                 ||
	    (strcmp(test_results.reply_string, "AT+SPARAM?\r\r\n9,A,1011,\"abc\",\"abc\",9\r\n\r\nOK\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(27.48) failed\n");
		return false;
	}


/*--- simple synchronous abort ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+SABORT?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.49) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "AT+SABORT?\r\r\nabort1\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.50) failed\n");
		return false;
	}


/*--- simple synchronous error ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+SERROR?\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.51) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "AT+SERROR?\r\r\nERROR\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(27.52) failed\n");
		return false;
	}


/*--- Asynchronous Sets ----------------------------------------------------*/

/*--- simple asynchronous set ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+AINFO=5,f,101,hello,\"HELLO\",99\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.1) failed\n");
		return false;
	}
	if ((test_async_xid                                                 == utlFAILED)                         ||
	    (test_results.num_commands                                      != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+AINFO")                != 0)                                 ||
	    (test_results.commands[0].num_parameters                        != 6)                                 ||
	    (test_results.commands[0].parameter_values[0].type              != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal     != 5)                                 ||
	    (test_results.commands[0].parameter_values[1].type              != utlAT_DATA_TYPE_HEXADECIMAL)       ||
	    (test_results.commands[0].parameter_values[1].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[1].value.hexadecimal != 15)                                ||
	    (test_results.commands[0].parameter_values[2].type              != utlAT_DATA_TYPE_BINARY)            ||
	    (test_results.commands[0].parameter_values[2].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[2].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[2].value.binary      != 5)                                 ||
	    (test_results.commands[0].parameter_values[3].type              != utlAT_DATA_TYPE_STRING)            ||
	    (test_results.commands[0].parameter_values[3].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[3].is_default        != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[3].value.string_p, "hello") != 0)                   ||
	    (test_results.commands[0].parameter_values[4].type              != utlAT_DATA_TYPE_QSTRING)           ||
	    (test_results.commands[0].parameter_values[4].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[4].is_default        != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[4].value.string_p, "HELLO") != 0)                   ||
	    (test_results.commands[0].parameter_values[5].type              != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[5].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[5].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[5].value.decimal     != 99)                                ||
	    (strcmp(test_results.reply_string, "AT+AINFO=5,f,101,hello,\"HELLO\",99\r") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(28.2) failed\n");
		return false;
	}
	if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_REQUEST_COMPLETED) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.3) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "AT+AINFO=5,f,101,hello,\"HELLO\",99\r\r\nOK\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.4) failed\n");
		return false;
	}


/*--- simple asynchronous abort ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+AINFO=5,f,101,hello,\"HELLO\",99\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.5) failed\n");
		return false;
	}
	if (test_async_xid == utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.6) failed\n");
		return false;
	}
	if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_ABORT, "abort4") != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.7) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "AT+AINFO=5,f,101,hello,\"HELLO\",99\r\r\nabort4\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.8) failed\n");
		return false;
	}


/*--- simple asynchronous error ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+AINFO=5,f,101,hello,\"HELLO\",99\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.9) failed\n");
		return false;
	}
	if (test_async_xid == utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.10) failed\n");
		return false;
	}
	if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_ERROR) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.11) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "AT+AINFO=5,f,101,hello,\"HELLO\",99\r\r\nERROR\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.12) failed\n");
		return false;
	}


/*--- async "SET" info with subsequent pending AT commands ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+AINFO=5,f,101,hello,\"HELLO\",99\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.13) failed\n");
		return false;
	}
	s_p = "AT+GMI\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.14) failed\n");
		return false;
	}
	s_p = "AT+GCI\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.15) failed\n");
		return false;
	}
	if (test_async_xid == utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.16) failed\n");
		return false;
	}
	if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_REQUEST_COMPLETED) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.17) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "AT+AINFO=5,f,101,hello,\"HELLO\",99\r\r\nOK\r\nAT+GMI\r\r\nIntel\r\n\r\nOK\r\nAT+GCI\r\r\n+GCI: 20\r\n\r\nOK\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.18) failed\n");
		return false;
	}


/*--- multiple simultanius async "SET" requests ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+AINFO=5,f,101,hello,\"HELLO\",99\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.19) failed\n");
		return false;
	}
	s_p = "AT+AINFO=6,f,110,hEllo,\"HELLo\",98\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.20) failed\n");
		return false;
	}
	s_p = "AT+AINFO=7,f,111,heLlo,\"HELlO\",97\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.21) failed\n");
		return false;
	}
	if (test_async_xid == utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.22) failed\n");
		return false;
	}
	if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_REQUEST_COMPLETED) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.23) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "AT+AINFO=5,f,101,hello,\"HELLO\",99\r\r\nOK\r\nAT+AINFO=6,f,110,hEllo,\"HELLo\",98\r") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.24) failed\n");
		return false;
	}
	if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_REQUEST_COMPLETED) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.25) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "AT+AINFO=5,f,101,hello,\"HELLO\",99\r\r\nOK\r\nAT+AINFO=6,f,110,hEllo,\"HELLo\",98\r\r\nOK\r\nAT+AINFO=7,f,111,heLlo,\"HELlO\",97\r") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.26) failed\n");
		return false;
	}
	if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_REQUEST_COMPLETED) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.27) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "AT+AINFO=5,f,101,hello,\"HELLO\",99\r\r\nOK\r\nAT+AINFO=6,f,110,hEllo,\"HELLo\",98\r\r\nOK\r\nAT+AINFO=7,f,111,heLlo,\"HELlO\",97\r\r\nOK\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.28) failed\n");
		return false;
	}

/*--- simple asynchronous set with synchronous reply ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+SINFO=5,f,101,hello,\"HELLO\",99\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.29) failed\n");
		return false;
	}
	if ((test_async_xid                                                 == utlFAILED)                         ||
	    (test_results.num_commands                                      != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "+SINFO")                != 0)                                 ||
	    (test_results.commands[0].num_parameters                        != 6)                                 ||
	    (test_results.commands[0].parameter_values[0].type              != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal     != 5)                                 ||
	    (test_results.commands[0].parameter_values[1].type              != utlAT_DATA_TYPE_HEXADECIMAL)       ||
	    (test_results.commands[0].parameter_values[1].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[1].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[1].value.hexadecimal != 15)                                ||
	    (test_results.commands[0].parameter_values[2].type              != utlAT_DATA_TYPE_BINARY)            ||
	    (test_results.commands[0].parameter_values[2].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[2].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[2].value.binary      != 5)                                 ||
	    (test_results.commands[0].parameter_values[3].type              != utlAT_DATA_TYPE_STRING)            ||
	    (test_results.commands[0].parameter_values[3].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[3].is_default        != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[3].value.string_p, "hello") != 0)                   ||
	    (test_results.commands[0].parameter_values[4].type              != utlAT_DATA_TYPE_QSTRING)           ||
	    (test_results.commands[0].parameter_values[4].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[4].is_default        != false)                             ||
	    (strcmp(test_results.commands[0].parameter_values[4].value.string_p, "HELLO") != 0)                   ||
	    (test_results.commands[0].parameter_values[5].type              != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[5].access            != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[5].is_default        != false)                             ||
	    (test_results.commands[0].parameter_values[5].value.decimal     != 99)                                ||
	    (strcmp(test_results.reply_string, "AT+SINFO=5,f,101,hello,\"HELLO\",99\r\r\nOK\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(28.30) failed\n");
		return false;
	}


/*--- simple synchronous abort ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+SABORT=5,f,101,hello,\"HELLO\",99\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.31) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "AT+SABORT=5,f,101,hello,\"HELLO\",99\r\r\nabort2\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.32) failed\n");
		return false;
	}


/*--- simple synchronous error ---*/
	CLEAR_TEST_RESULTS();
	s_p = "AT+SERROR=5,f,101,hello,\"HELLO\",99\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.33) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "AT+SERROR=5,f,101,hello,\"HELLO\",99\r\r\nERROR\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.34) failed\n");
		return false;
	}


/*--- Asynchronous Execs ---------------------------------------------------*/

/*--- simple asynchronous exec ---*/
	CLEAR_TEST_RESULTS();
	s_p = "ATN5\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.35) failed\n");
		return false;
	}
	if ((test_async_xid                                             == utlFAILED)                         ||
	    (test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "N")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 5)                                 ||
	    (strcmp(test_results.reply_string, "ATN5\r") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(28.36) failed\n");
		return false;
	}
	if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_REQUEST_COMPLETED) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.37) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "ATN5\r\r\nOK\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.38) failed\n");
		return false;
	}

/*--- simple asynchronous abort ---*/
	CLEAR_TEST_RESULTS();
	s_p = "ATN5\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.39) failed\n");
		return false;
	}
	if (test_async_xid == utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.40) failed\n");
		return false;
	}
	if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_ABORT, "abort5") != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.41) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "ATN5\r\r\nabort5\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.42) failed\n");
		return false;
	}

/*--- simple asynchronous error ---*/
	CLEAR_TEST_RESULTS();
	s_p = "ATN5\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.43) failed\n");
		return false;
	}
	if (test_async_xid == utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.44) failed\n");
		return false;
	}
	if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_ERROR) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.45) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "ATN5\r\r\nERROR\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.46) failed\n");
		return false;
	}


/*--- async "EXEC" info with subsequent pending AT commands ---*/
	CLEAR_TEST_RESULTS();
	s_p = "ATN5\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.47) failed\n");
		return false;
	}
	s_p = "AT+GMI\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.48) failed\n");
		return false;
	}
	s_p = "AT+GCI\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.49) failed\n");
		return false;
	}
	if (test_async_xid == utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.50) failed\n");
		return false;
	}
	if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_REQUEST_COMPLETED) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.51) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "ATN5\r\r\nOK\r\nAT+GMI\r\r\nIntel\r\n\r\nOK\r\nAT+GCI\r\r\n+GCI: 20\r\n\r\nOK\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.52) failed\n");
		return false;
	}


/*--- multiple simultanious async "EXEC" requests ---*/
	CLEAR_TEST_RESULTS();
	s_p = "ATN5\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.53) failed\n");
		return false;
	}
	s_p = "ATN6\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.54) failed\n");
		return false;
	}
	s_p = "ATN7\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.55) failed\n");
		return false;
	}
	if (test_async_xid == utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.56) failed\n");
		return false;
	}
	if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_REQUEST_COMPLETED) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.57) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "ATN5\r\r\nOK\r\nATN6\r") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.58) failed\n");
		return false;
	}
	if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_REQUEST_COMPLETED) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.59) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "ATN5\r\r\nOK\r\nATN6\r\r\nOK\r\nATN7\r") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.61) failed\n");
		return false;
	}
	if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_REQUEST_COMPLETED) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.61) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "ATN5\r\r\nOK\r\nATN6\r\r\nOK\r\nATN7\r\r\nOK\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.62) failed\n");
		return false;
	}


/*--- simple asynchronous exec with synchronous reply ---*/
	CLEAR_TEST_RESULTS();
	s_p = "ATB5\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(28.63) failed\n");
		return false;
	}
	if ((test_async_xid                                             == utlFAILED)                         ||
	    (test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "B")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 5)                                 ||
	    (strcmp(test_results.reply_string, "ATB5\r\r\nOK\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(28.64) failed\n");
		return false;
	}


/*--- simple asynchronous exec with synchronous info-text response ---*/
	CLEAR_TEST_RESULTS();
	s_p = "ATU5\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.65) failed\n");
		return false;
	}
	if ((test_async_xid                                             == utlFAILED)                         ||
	    (test_results.num_commands                                  != 1)                                 ||
	    (strcmp(test_results.commands[0].name, "U")                 != 0)                                 ||
	    (test_results.commands[0].num_parameters                    != 1)                                 ||
	    (test_results.commands[0].parameter_values[0].type          != utlAT_DATA_TYPE_DECIMAL)           ||
	    (test_results.commands[0].parameter_values[0].access        != utlAT_PARAMETER_ACCESS_READ_WRITE) ||
	    (test_results.commands[0].parameter_values[0].is_default    != false)                             ||
	    (test_results.commands[0].parameter_values[0].value.decimal != 5)                                 ||
	    (strcmp(test_results.reply_string, "ATU5\r\r\nMOO\r\n") != 0))
	{
		dumpTestResults(stderr);
		fprintf(stderr, "atParserTest: utlAtParse(28.66) failed\n");
		return false;
	}


/*--- simple synchronous error ---*/
	CLEAR_TEST_RESULTS();
	s_p = "ATW5\r";
	if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlFAILED)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.67) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "ATW5\r\r\nERROR\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(28.68) failed\n");
		return false;
	}


/*--- Unsolicited text/strings ---------------------------------------------*/

/*--- unsolicited info text ---*/
	CLEAR_TEST_RESULTS();
	if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_UNSOLICITED_INFO_TEXT, "my test info text") != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(29.1) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "\r\nmy test info text\r\n") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(29.2) failed\n");
		return false;
	}

/*--- unsolicited string ---*/
	CLEAR_TEST_RESULTS();
	if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_UNSOLICITED_DTE_MESSAGE, "my test message") != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlAtParse(29.3) failed\n");
		return false;
	}
	if (strcmp(test_results.reply_string, "my test message") != 0)
	{
		fprintf(stderr, "atParserTest: utlAtParse(29.4) failed\n");
		return false;
	}


/*--- Timeouts -------------------------------------------------------------*/

/*--- Raw Modem Timeout ---*/
	{
		utlRelativeTime_T period;
		testInfo_T exit_test_info;
		utlTimerId_T exit_timer_id;

		/* exit timer */
		period.seconds     = 7;
		period.nanoseconds = 0;

		exit_test_info.mode       = utlMODE_EXIT_EVENT_HANDLER;
		exit_test_info.num_cycles = 1;
		exit_test_info.curr_cycle = 0;
		exit_test_info.last       = 0.0;
		exit_test_info.period     =  period.seconds +
					    (period.nanoseconds / 1000000000.0);

		if ((exit_timer_id = utlStartTimer(&period, exit_test_info.num_cycles, &testFunc, &exit_test_info)) == utlFAILED)
		{
			fprintf(stderr, "atParserTest: utlAtParse(30.0) failed\n");
			return false;
		}

		/*--- raw modem timeout ---*/
		CLEAR_TEST_RESULTS();
		s_p = "AT+TMO=2\r";
		if (utlAtParse(parser_p, (unsigned char *)s_p, strlen(s_p)) != utlSUCCESS)
		{
			fprintf(stderr, "atParserTest: utlAtParse(30.1) failed\n");
			return false;
		}

		if (utlEventLoop(true) != utlSUCCESS)
		{
			fprintf(stderr, "atParserTest: utlAtParse(30.2) failed\n");
			return false;
		}

		if (strcmp(test_results.reply_string, "AT+TMO=2\r\r\nERROR\r\n") != 0)
		{
			fprintf(stderr, "atParserTest: utlAtParse(30.3) failed\n");
			return false;
		}
	}

/*
   useful for debugging:
   utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
   dumpTestResults(stderr);
 */

/*--- Dump -----------------------------------------------------------------*/

	utlDumpAtParser(stdout, parser_p);

/*--------------------------------------------------------------------------*/

	if (utlCloseAtParser(parser_p) != utlSUCCESS)
	{
		fprintf(stderr, "atParserTest: utlCloseAtParser() failed\n");
		return false;
	}

	return true;
}

/*------------------------------------------------------------------------------*
* INTERACTIVE AT-PARSER SHELL (for manual testing)
*------------------------------------------------------------------------------*/
static struct termios tio;
static struct termios raw_tio;
static utlTimerId_T incoming_ring_timer_id = utlFAILED;


/*------------------------------------------------------------------------------*/
static utlReturnCode_T atParserShellIncomingCallTimeoutHandler(const utlTimerId_T id,
							       const unsigned long timeout_count,
							       void                *arg_p,
							       const utlAbsoluteTime_P2c curr_time_p)
{
	utlAtParser_P parser_p;

	utlAssert(arg_p != NULL);

	parser_p = (utlAtParser_P)arg_p;

	if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING) != utlSUCCESS)
		return utlFAILED;

	return utlSUCCESS;
}

/*------------------------------------------------------------------------------*/
static utlReturnCode_T atParserShellFdHandler(const utlEventHandlerType_T handler_type,
					      const utlEventHandlerType_T event_type,
					      const int fd,
					      const utlRelativeTime_P2c period_p,
					      void                  *arg_p)
{
	utlAtParser_P parser_p;
	unsigned char buf[100];
	size_t n;

	utlAssert(handler_type == utlEVENT_HANDLER_TYPE_READ);
	utlAssert(event_type   == utlEVENT_HANDLER_TYPE_READ);
	utlAssert(arg_p        != NULL);

	parser_p = (utlAtParser_P)arg_p;

	/*--- read input ---*/
	if ((n = read(fd, buf, sizeof(buf))) < 0)
	{
		utlError("Call to %s() failed, %s", "read", strerror(errno));
		return utlFAILED;
	}

	/*--- convert <nl> to <cr>, <del> to <bs>, and execute any embedded commands ---*/
	{
		size_t i;

		for (i = 0; i < n; i++)
			if (buf[i] ==  10) buf[i] = 13;
			else if (buf[i] == 127) buf[i] =  8;

			/* dump state machine info? */
			else if (buf[i] ==  19 /* control-S */)
			{
				tcsetattr(0, TCSANOW, &tio);
				utlDumpStateMachineHistory(stderr, parser_p->states.machine_p);
				fflush(stderr);
				tcsetattr(0, TCSADRAIN, &raw_tio);

				/* lose carrier? */
			}
			else if (buf[i] ==  12 /* control-l */)
			{
				if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_NO_CARRIER) != utlSUCCESS)
					return utlFAILED;

				/* restore carrier? */
			}
			else if (buf[i] ==  18 /* control-r */)
			{
				if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_CARRIER) != utlSUCCESS)
					return utlFAILED;

				/* dump AT-parser info? */
			}
			else if (buf[i] ==  16 /* control-P */)
			{
				tcsetattr(0, TCSANOW, &tio);
				utlDumpAtParser(stderr, parser_p);
				fflush(stderr);
				tcsetattr(0, TCSADRAIN, &raw_tio);

				/* initiate incoming call? */
			}
			else if (buf[i] == 9 /* control-I */)
			{
				if (incoming_ring_timer_id == utlFAILED)
				{
					utlRelativeTime_T period;

					period.seconds     = 2;
					period.nanoseconds = 500000000;
					if ((incoming_ring_timer_id = utlStartTimer(&period, 25, &atParserShellIncomingCallTimeoutHandler, parser_p)) == utlFAILED)
						return utlFAILED;
				}
				else
				{
					if (utlStopTimer(incoming_ring_timer_id) == utlFAILED)
						return utlFAILED;
					incoming_ring_timer_id = utlFAILED;
				}

			}
			else if (buf[i] == 2 /* control-B */)
			{
				if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_CREDIT_BONG) != utlSUCCESS)
					return utlFAILED;

			}
			else if (buf[i] == 1 /* control-A */)
			{
				utlAtParameterValue_T parameter_values[3];
				size_t n;

				if ((n = utlAtParserInitCommandParameters(test_async_xid, parameter_values, 3)) == utlFAILED)
					return utlFAILED;

				parameter_values[0].is_default    = false;
				parameter_values[0].value.decimal = 888;

				if (utlAtCommandResponse(test_async_xid, utlAT_COMMAND_RESPONSE_TYPE_PARAMETERS, &parameter_values, n) != utlSUCCESS)
					return utlFAILED;

				/* abort? */
			}
			else if (buf[i] == 3 /* control-C */)
			{
				/*--- restore tty attributes ---*/
				tcsetattr(0, TCSANOW, &tio);

				exit(1);
			}
	}

	(void)utlAtParse(parser_p, buf, n);

	return utlSUCCESS;
}

/*------------------------------------------------------------------------------*/
static utlReturnCode_T atParserShellReplyFunction(const char *string_p, void *arg_p)
{
	fputs(string_p, stdout);
	fflush(stdout);

	return utlSUCCESS;
}

/*------------------------------------------------------------------------------*/
static utlReturnCode_T atParserShellSParameterFunction(const unsigned int parameter_num,
						       const unsigned int parameter_value,
						       void         *arg_p)
{
	test_results.s_parameter_num   = parameter_num;
	test_results.s_parameter_value = parameter_value;

	return utlSUCCESS;
}

/*------------------------------------------------------------------------------*/
static utlReturnCode_T atParserShellDriverRequestFunction(const utlAtParser_P parser_p,
							  const utlAtDriverRequest_T request_type,
							  void                 *arg_p,
							  ...)
{
	va_list va_arg_p;

	utlAssert(parser_p != NULL);

	va_start(va_arg_p, arg_p);

	switch (request_type)
	{
	case utlAT_DRIVER_REQUEST_TONE_DIAL:
	case utlAT_DRIVER_REQUEST_PULSE_DIAL:

		/* senario: dial-out, get carrier (normal condition) */
		if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_CONF) != utlSUCCESS) return utlFAILED;
		if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
		if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
		if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
		if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ANSWERED)  != utlSUCCESS) return utlFAILED;
		return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_CARRIER);

		/* senario: dial-out, get carrier then immediately loose it */
		if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_CONF) != utlSUCCESS) return utlFAILED;
		if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
		if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
		if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
		if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ANSWERED)  != utlSUCCESS) return utlFAILED;
		if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_CARRIER)   != utlSUCCESS) return utlFAILED;
		return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_NO_CARRIER);

		/* senario: dial-out, get busy signal */
		if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_CONF) != utlSUCCESS) return utlFAILED;
		return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_BUSY);

		/* senario: dial-out, get no carrier (implicit) */
		if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_CONF) != utlSUCCESS) return utlFAILED;
		if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
		if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
		if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
		return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ANSWERED);

		/* senario: dial-out, get no carrier (explicit) */
		if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_CONF) != utlSUCCESS) return utlFAILED;
		if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
		if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
		if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_RING)      != utlSUCCESS) return utlFAILED;
		if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ANSWERED)  != utlSUCCESS) return utlFAILED;
		return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_NO_CARRIER);

	case utlAT_DRIVER_REQUEST_ON_HOOK:
	case utlAT_DRIVER_REQUEST_FLASH_ON_HOOK:
	case utlAT_DRIVER_REQUEST_RESET_ON_HOOK:
		return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ON_HOOK_CONF);

	case utlAT_DRIVER_REQUEST_ANSWER_OFF_HOOK:
		return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF);

	case utlAT_DRIVER_REQUEST_OFF_HOOK:
	case utlAT_DRIVER_REQUEST_FLASH_OFF_HOOK:
		if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_OFF_HOOK_CONF) != utlSUCCESS)
			return utlFAILED;
		return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_DIAL_TONE);

	case utlAT_DRIVER_REQUEST_ANSWER:
		if (utlStopTimer(incoming_ring_timer_id) == utlFAILED)
			return utlFAILED;
		incoming_ring_timer_id = utlFAILED;

		if (utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_ANSWER_CONF) != utlSUCCESS) return utlFAILED;
		return utlAtDriverEvent(parser_p, utlAT_DRIVER_EVENT_CARRIER);

	default:
		break;
	}

	va_end(va_arg_p);

	return utlSUCCESS;
}

/*------------------------------------------------------------------------------*/
static utlReturnCode_T atParserShellGetFunction(const utlAtParameterOp_T op,
						const char                    *command_name_p,
						const utlAtParameterValue_P2c parameter_values_p,
						const size_t num_parameters,
						const char                    *info_text_p,
						unsigned int            *xid_p,
						void                    *arg_p)
{
	size_t i;

	/* do nothing */
	/*
	   return utlSUCCESS;
	 */
	/*
	   fprintf(stderr, "Get\n");
	   fprintf(stderr, "    command: %s\n", command_name_p);
	 */
	if (strcasecmp(command_name_p, "+ASYNCG") == 0)
	{
		/* do nothing */
		DBGMSG("ATD: processing +ASYNCG\n");
		return utlSUCCESS;

	}
	else if (strcasecmp(command_name_p, "+SSYNCG") == 0)
	{
		utlAtParameterValue_T parameter_values[3];
		size_t n;

		DBGMSG("ATD: processing +SSYNCG\n");
		if ((n = utlAtParserInitCommandParameters(*xid_p, parameter_values, 3)) == utlFAILED)
			return utlFAILED;

		parameter_values[0].is_default    = false;
		parameter_values[0].value.decimal = 999;

		if (utlAtCommandResponse(*xid_p, utlAT_COMMAND_RESPONSE_TYPE_PARAMETERS,
					 &parameter_values, n) != utlSUCCESS)
			return utlFAILED;

		return utlSUCCESS;

	}
	else if (strcasecmp(command_name_p, "+ASYNCS") == 0)
	{
		/* do nothing */

		DBGMSG("ATD: processing +ASYNCS\n");
		return utlSUCCESS;

	}
	else if (strcasecmp(command_name_p, "+SSYNCS") == 0)
	{
		DBGMSG("ATD: processing +SSYNCS\n");
		if (utlAtCommandResponse(*xid_p, utlAT_COMMAND_RESPONSE_TYPE_REQUEST_COMPLETED) != utlSUCCESS)
			return utlFAILED;

		return utlSUCCESS;
	}
	else
	{
		utlAtParameterValue_T parameter_values[10];
		size_t n;

		DBGMSG("ATD: processing other command in %s\n", __FUNCTION__);
		n = utlAtParserInitCommandParameters(*xid_p, parameter_values, num_parameters);
		if (n == utlFAILED)
			return utlFAILED;

		/*--- assign test return values ---*/
		for (i = 0; i < n; i++)
		{

			parameter_values[i].is_default = false;

			switch (parameter_values[i].type)
			{
			case utlAT_DATA_TYPE_DECIMAL:
				parameter_values[i].value.decimal = 1;
				break;
			case utlAT_DATA_TYPE_HEXADECIMAL:
				parameter_values[i].value.hexadecimal   = 2;
				break;
			case utlAT_DATA_TYPE_BINARY:
				parameter_values[i].value.binary        = 3;
				break;
			case utlAT_DATA_TYPE_STRING:
				parameter_values[i].value.string_p      = "one";
				break;
			case utlAT_DATA_TYPE_QSTRING:
				parameter_values[i].value.qstring_p     = "two";
				break;
			case utlAT_DATA_TYPE_DIAL_STRING:
				parameter_values[i].value.dial_string_p = "555-1212";
				break;
			default:
				utlError("default case.");
				return utlFAILED;
			}
		}

		if (utlAtCommandResponse(*xid_p, utlAT_COMMAND_RESPONSE_TYPE_PARAMETERS,
					 &parameter_values, n) != utlSUCCESS)
			return utlFAILED;
	}

	return utlSUCCESS;
}

/*------------------------------------------------------------------------------*/
static utlReturnCode_T atParserShellSetFunction(const utlAtParameterOp_T op,
						const char                    *command_name_p,
						const utlAtParameterValue_P2c parameter_values_p,
						const size_t num_parameters,
						const char                    *info_text_p,
						unsigned int            *xid_p,
						void                    *arg_p)
{
	size_t i;

	if (strcasecmp(command_name_p, "+TMO") == 0)
	{
		fprintf(stderr, "ATCMD: process +TMO\n");
		return utlSUCCESS;
	}

	if (utlAtCommandResponse(*xid_p, utlAT_COMMAND_RESPONSE_TYPE_REQUEST_COMPLETED) != utlSUCCESS)
	{
		fprintf(stderr, "utlAtCommandResponse failed\n");
		return utlFAILED;
	}

	/* do nothing */
	// return utlSUCCESS;

	DBGMSG("Set command: %s (%d)\r\n", command_name_p, num_parameters);
	if (info_text_p)
	{
		DBGMSG("Transaction ID = %d (%s)\n", *xid_p, info_text_p);
	}
	else
	{
		DBGMSG("Transaction ID = %d\n", *xid_p);
	}

	for (i = 0; i < num_parameters; i++)
	{
		DBGMSG("parameter[%u].type: %u\r\n",
		       (unsigned int)i, parameter_values_p[i].type);
		DBGMSG("parameter[%u].access: %u\r\n",
		       (unsigned int)i, parameter_values_p[i].access);
		DBGMSG("parameter[%u].is_default: %u\r\n",
		       (unsigned int)i, parameter_values_p[i].is_default);
		DBGMSG("parameter[%u].value: ", (unsigned int)i);

		switch (parameter_values_p[i].type)
		{
		case utlAT_DATA_TYPE_DECIMAL:
			DBGMSG("%u\n", parameter_values_p[i].value.decimal);
			break;
		case utlAT_DATA_TYPE_HEXADECIMAL:
			DBGMSG("%u\n", parameter_values_p[i].value.hexadecimal);
			break;
		case utlAT_DATA_TYPE_BINARY:
			DBGMSG("%u\n", parameter_values_p[i].value.binary);
			break;
		case utlAT_DATA_TYPE_STRING:
			DBGMSG("\"%s\"\n", parameter_values_p[i].value.string_p);
			break;
		case utlAT_DATA_TYPE_QSTRING:
			DBGMSG("\"%s\"\n", parameter_values_p[i].value.qstring_p);
			break;
		case utlAT_DATA_TYPE_DIAL_STRING:
			DBGMSG("%s\n", parameter_values_p[i].value.dial_string_p);
			break;
		default:
			DBGMSG("???\n");
			break;
		}
	}

	return utlSUCCESS;
}

/*------------------------------------------------------------------------------*/
static utlAtParameter_T A_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T D_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DIAL_STRING) };
static utlAtParameter_T E_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T H_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T I_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T L_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T M_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T O_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T P_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T Q_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T T_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T V_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T X_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T Z_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T ampC_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T ampD_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T ampF_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T ampS_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DECIMAL) };
static utlAtParameter_T ampZ_params[] = { utlDEFINE_BASIC_AT_PARAMETER(utlAT_DATA_TYPE_DIAL_STRING) };

static utlAtParameter_T plusASTO_params[] = { utlDEFINE_STRING_AT_PARAMETER(     utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					      utlDEFINE_DIAL_STRING_AT_PARAMETER(utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL) };
static utlAtParameter_T plusIPR_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };
static utlAtParameter_T plusICF_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					     utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };
static utlAtParameter_T plusIFC_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
					     utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };
static utlAtParameter_T plusIDSR_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };

static utlAtParameter_T plusTMO_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };

static utlAtParameter_T plusASYNCG_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };
static utlAtParameter_T plusSSYNCG_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };
static utlAtParameter_T plusASYNCS_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };
static utlAtParameter_T plusSSYNCS_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };
static utlAtParameter_T plusEXAMPLE_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED) };

static utlAtCommand_T shell_commands[] = { utlDEFINE_BASIC_AT_COMMAND("A",						      A_params,																       atParserShellSetFunction),
					   utlDEFINE_BASIC_AT_COMMAND("D",						      D_params,																       atParserShellSetFunction),
					   utlDEFINE_BASIC_AT_COMMAND("E",						      E_params,																       atParserShellSetFunction),
					   utlDEFINE_BASIC_AT_COMMAND("H",						      H_params,																       atParserShellSetFunction),
					   utlDEFINE_BASIC_AT_COMMAND("I",						      I_params,																       atParserShellSetFunction),
					   utlDEFINE_BASIC_AT_COMMAND("L",						      L_params,																       atParserShellSetFunction),
					   utlDEFINE_BASIC_AT_COMMAND("M",						      M_params,																       atParserShellSetFunction),
					   utlDEFINE_BASIC_AT_COMMAND("O",						      O_params,																       atParserShellSetFunction),
					   utlDEFINE_BASIC_AT_COMMAND("P",						      P_params,																       atParserShellSetFunction),
					   utlDEFINE_BASIC_AT_COMMAND("Q",						      Q_params,																       atParserShellSetFunction),
					   utlDEFINE_BASIC_AT_COMMAND("T",						      T_params,																       atParserShellSetFunction),
					   utlDEFINE_BASIC_AT_COMMAND("V",						      V_params,																       atParserShellSetFunction),
					   utlDEFINE_BASIC_AT_COMMAND("X",						      X_params,																       atParserShellSetFunction),
					   utlDEFINE_BASIC_AT_COMMAND("Z",						      Z_params,																       atParserShellSetFunction),
					   utlDEFINE_BASIC_AT_COMMAND("&C",						      ampC_params,															       atParserShellSetFunction),
					   utlDEFINE_BASIC_AT_COMMAND("&D",						      ampD_params,															       atParserShellSetFunction),
					   utlDEFINE_BASIC_AT_COMMAND("&F",						      ampF_params,															       atParserShellSetFunction),
					   utlDEFINE_BASIC_AT_COMMAND("&S",						      ampS_params,															       atParserShellSetFunction),
					   utlDEFINE_BASIC_AT_COMMAND("&Z",						      ampZ_params,															       atParserShellSetFunction),

					   utlDEFINE_EXTENDED_AT_COMMAND("+ASTO",					      plusASTO_params,															       "+ASTO: (0-" utlStringize(utlAT_MAX_STORED_DIAL_STRINGS) "),(" utlStringize(utlAT_MAX_STORED_DIAL_STRING_LENGTH) ")",atParserShellGetFunction,	     atParserShellSetFunction),
					   utlDEFINE_EXACTION_AT_COMMAND("+GMI",					      NULL,																       "+GMI",														    atParserShellSetFunction),
					   utlDEFINE_EXACTION_AT_COMMAND("+CGMI",					      NULL,																       "+CGMI",														    atParserShellSetFunction),
					   utlDEFINE_EXACTION_AT_COMMAND("+GMM",					      NULL,																       "+GMM",														    atParserShellSetFunction),
					   utlDEFINE_EXACTION_AT_COMMAND("+GMR",					      NULL,																       "+GMR",														    atParserShellSetFunction),
					   utlDEFINE_EXACTION_AT_COMMAND("+GSN",					      NULL,																       "+GSN",														    atParserShellSetFunction),
					   utlDEFINE_EXACTION_AT_COMMAND("+GOI",					      NULL,																       "+GOI",														    atParserShellSetFunction),
					   utlDEFINE_EXTENDED_AT_COMMAND("+GCAP",					      NULL,																       "+GCAP",
									 atParserShellGetFunction,			      atParserShellSetFunction),
					   utlDEFINE_EXACTION_AT_COMMAND("+GCI",					      NULL,																       "+GCI",														    atParserShellSetFunction),
					   utlDEFINE_EXTENDED_AT_COMMAND("+IPR",
									 plusIPR_params,				      "+IPR: (0,1200,9600),(0,50,75,110,134,150,200,300,600,1200,1800,2400,4800,7200,9600,14400,19200,28800,38400,57600,76800,115200,230400)",
									 atParserShellGetFunction,			      atParserShellSetFunction),
					   utlDEFINE_EXTENDED_AT_COMMAND("+ICF",					      plusICF_params,															       "+ICF: (0-6),(0-3)",
									 atParserShellGetFunction,			      atParserShellSetFunction),
					   utlDEFINE_EXTENDED_AT_COMMAND("+IFC",					      plusIFC_params,															       "+IFC: (0-3),(0-2)",
									 atParserShellGetFunction,			      atParserShellSetFunction),
					   utlDEFINE_EXTENDED_AT_COMMAND("+IDSR",					      plusIDSR_params,															       "+IDSR: (0-2)",
									 atParserShellGetFunction,			      atParserShellSetFunction),

					   utlDEFINE_EXTENDED_AT_COMMAND("+TMO",					      plusTMO_params,															       "+TMO: (0-9)",													    atParserShellGetFunction,	     atParserShellSetFunction),

					   utlDEFINE_EXTENDED_AT_COMMAND("+ASYNCG",					      plusASYNCG_params,														       "+ASYNCG: (2-7)",												    atParserShellGetFunction,	     atParserShellSetFunction),
					   utlDEFINE_EXTENDED_AT_COMMAND("+SSYNCG",					      plusSSYNCG_params,														       "+ASYNCG: (1-8)",												    atParserShellGetFunction,	     atParserShellSetFunction),
					   utlDEFINE_EXTENDED_AT_COMMAND("+ASYNCS",					      plusASYNCS_params,														       "+ASYNCS: (2-7)",												    atParserShellGetFunction,	     atParserShellSetFunction),
					   utlDEFINE_EXTENDED_AT_COMMAND("+SSYNCS",					      plusSSYNCS_params,														       "+ASYNCS: (1-8)",												    atParserShellGetFunction,	     atParserShellSetFunction),
					   utlDEFINE_EXTENDED_AT_COMMAND("+EXAMPLE",					      plusEXAMPLE_params,														       "+EXAMPLE: (0-9)",												    atParserShellGetFunction,	     atParserShellSetFunction) };

/*------------------------------------------------------------------------------*/
bool atParserShell(void)
{
	utlAtParser_P parser_p;

	if ((parser_p = utlOpenAtParser(shell_commands, utlNumberOf(shell_commands), NULL)) == NULL)
	{
		fprintf(stderr, "atParserShell: utlOpenAtParser(1.0) failed\n");
		return false;
	}

	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_REPLY_HANDLER, atParserShellReplyFunction) != utlSUCCESS)
	{
		fprintf(stderr, "atParserShell: utlAtParserOp(1.1) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_S_PARAMETER_HANDLER, atParserShellSParameterFunction) != utlSUCCESS)
	{
		fprintf(stderr, "atParserShell: utlAtParserOp(1.2) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_DRIVER_REQUEST_HANDLER, atParserShellDriverRequestFunction) != utlSUCCESS)
	{
		fprintf(stderr, "atParserShell: utlAtParserOp(1.3) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_ECHO_ON) != utlSUCCESS)
	{
		fprintf(stderr, "atParserShell: utlAtParserOp(1.4) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_ID, "Intel") != utlSUCCESS)
	{
		fprintf(stderr, "atParserShell: utlAtParserOp(1.5) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_MANUFACTURER, "Intel") != utlSUCCESS)
	{
		fprintf(stderr, "atParserShell: utlAtParserOp(1.6) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_MODEL, "Linux") != utlSUCCESS)
	{
		fprintf(stderr, "atParserShell: utlAtParserOp(1.7) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_REVISION, "1.0") != utlSUCCESS)
	{
		fprintf(stderr, "atParserShell: utlAtParserOp(1.8) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_SERIAL_NUMBER, "0") != utlSUCCESS)
	{
		fprintf(stderr, "atParserShell: utlAtParserOp(1.9) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_OBJECT_ID, "OBJ ID") != utlSUCCESS)
	{
		fprintf(stderr, "atParserShell: utlAtParserOp(1.10) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_COUNTRY_CODE, "(20,3C,3D,42,50,58,00,61,FE,A9,B4,B5)") != utlSUCCESS)
	{
		fprintf(stderr, "atParserShell: utlAtParserOp(1.11) failed\n");
		return false;
	}
	if (utlAtParserOp(parser_p, utlAT_PARSER_OP_SET_COUNTRY_CODE, "20") != utlSUCCESS)
	{
		fprintf(stderr, "atParserShell: utlAtParserOp(1.12) failed\n");
		return false;
	}
	CLEAR_TEST_RESULTS();

	/*--- fd read handler for <stdin> ---*/
	{
		utlEventHandlerId_T handler_id;

		if ((handler_id = utlSetFdEventHandler(utlEVENT_HANDLER_TYPE_READ,
						       utlEVENT_HANDLER_PRIORITY_MEDIUM,
						       0, atParserShellFdHandler, parser_p)) == utlFAILED)
		{
			fprintf(stderr, "atParserShell: utlSetFdEventHandler() failed\n");
			return false;
		}
	}

	/*--- configure i/o ---*/
	{
		tcgetattr(0, &tio);
		tcgetattr(0, &raw_tio);

		cfmakeraw(&raw_tio);

		tcsetattr(0, TCSANOW, &raw_tio);
	}

	/*--- main event loop ---*/
	if (utlEventLoop(true) != utlSUCCESS)
	{
		fprintf(stderr, "atParserShell: utlEventLoop() failed\n");

		/*--- restore tty attributes ---*/
		tcsetattr(0, TCSANOW, &tio);

		return false;
	}

	/*--- restore tty attributes ---*/
	tcsetattr(0, TCSANOW, &tio);

	return true;
}

