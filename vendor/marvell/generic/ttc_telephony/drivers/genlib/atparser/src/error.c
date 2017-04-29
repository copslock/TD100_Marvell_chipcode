/*****************************************************************************
* Utility Library
*
* DESCRIPTION
*	Error reporting functions.
*
* EXAMPLE USAGE
*
*      --- assert ---
*
*      utlAssert(num == 42);
*
*      --- reporting errors ---
*
*      utlError("my error message");
*
*      utlError("my error message with parameters: %d %s", 99, "my string");
*
*****************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include <utlTypes.h>
#include <utlError.h>


/*---------------------------------------------------------------------------*
* FUNCTION
*	utlDoAssert(utlHERE_DECLARATION, result, assertion_p)
* INPUT
*      utlHERE_DECLARATION == source of assertion
*                   result == assertion test result
*              assertion_p == assertion as a character string
* OUTPUT
*	none
* RETURNS
*	"true" if the assertion passed, else "false"
* DESCRIPTION
*	Handles assertions, reporting an error if the assert fails.
*---------------------------------------------------------------------------*/
bool utlDoAssert(utlHERE_DECLARATION,
		 const int result,
		 const char *assertion_p)
{
	if (!result)
	{

/* HACK: for now just print the failed assertion... */

		(void)fprintf(stderr, "ASSERT:\n");
		(void)fprintf(stderr, "    file path: %s\n",                utlHERE_FILE);
		(void)fprintf(stderr, "    func name: %s\n",                utlHERE_FUNC);
		(void)fprintf(stderr, "  line number: %u\n", (unsigned int)utlHERE_LINE);
		(void)fprintf(stderr, "    assertion: %s\n",                assertion_p);

		(void)fflush(stderr);

		return false;
	}

	return true;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlDoError(utlHERE_DECLARATION, msg_p, ...)
* INPUT
*      utlHERE_DECLARATION == source of error
*                    msg_p == pointer to an error message
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Handles error reporting.
*---------------------------------------------------------------------------*/
void utlDoError(utlHERE_DECLARATION,
		const char *msg_p,
		...)
{
	utlAssert(msg_p != NULL);

/* HACK: for now just print the error... */

	(void)fprintf(stderr, "ERROR:\n");
	(void)fprintf(stderr, "    file path: %s\n",                utlHERE_FILE);
	(void)fprintf(stderr, "    func name: %s\n",                utlHERE_FUNC);
	(void)fprintf(stderr, "  line number: %u\n", (unsigned int)utlHERE_LINE);
	(void)fprintf(stderr, "      message: ");

	{
		va_list va_arg_p;

		va_start(va_arg_p, msg_p);
		(void)vfprintf(stderr, msg_p, va_arg_p);
		va_end(va_arg_p);
	}

	(void)fprintf(stderr, "\n");

	(void)fflush(stderr);
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlDoSigError(utlHERE_DECLARATION, msg_p, ...)
* INPUT
*      utlHERE_DECLARATION == source of error
*                    msg_p == pointer to an error message
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Handles error reporting from with-in a signal handler.
*---------------------------------------------------------------------------*/
void utlDoSigError(utlHERE_DECLARATION,
		   const char *msg_p,
		   ...)
{
	char buf[200];
	size_t n;

	utlAssert(msg_p != NULL);

/* HACK: for now just print the error... */

	/*--- accessing buffered I/O in signal handlers is not safe, so we'll do
	      it the hard way using only local variables and unbuffered I/O... */

	n = 0;
	(void)strncpy(buf + n, "SIG-ERROR:",        sizeof(buf) - n);                                     n = strlen(buf);
	(void)strncpy(buf + n, "\n    file path: ", sizeof(buf) - n);                                     n = strlen(buf);
	(void)strncpy(buf + n, utlHERE_FILE,        sizeof(buf) - n);                                     n = strlen(buf);
	(void)strncpy(buf + n, "\n    func name: ", sizeof(buf) - n);                                     n = strlen(buf);
	(void)strncpy(buf + n, utlHERE_FUNC,        sizeof(buf) - n);                                     n = strlen(buf);
	(void)strncpy(buf + n, "\n  line number: ", sizeof(buf) - n);                                     n = strlen(buf);
	(void)snprintf(buf + n,                      sizeof(buf) - n, "%u", (unsigned int)utlHERE_LINE);  n = strlen(buf);
	(void)strncpy(buf + n, "\n      message: ", sizeof(buf) - n);                                     n = strlen(buf);

	/*--- append a formated message string ---*/
	{
		va_list va_arg_p;

		va_start(va_arg_p, msg_p);
		(void)vsnprintf(buf + n, sizeof(buf) - n, msg_p, va_arg_p);  n = strlen(buf);
		va_end(va_arg_p);
	}

	/*--- append a trailing <new-line> character ---*/
	(void)strncpy(buf + n, "\n", sizeof(buf) - n);  n = strlen(buf);

	/*--- write output ---*/
	{
		char    *buf_p;
		ssize_t rv;

		buf_p = buf;
		for (; n > (size_t)0; buf_p += rv, n -= rv)
			if ((rv = write(STDERR_FILENO, buf_p, n)) < (ssize_t)0)
				break;
	}
}

#ifdef utlTEST
/*---------------------------------------------------------------------------*
* FUNCTION
*	errorTest()
* INPUT
*	none
* OUTPUT
*	none
* RETURNS
*	"true" for pass, "false" for failure
*---------------------------------------------------------------------------*/
bool errorTest(void)
{
	if (utlDoAssert(utlHERE, 1 == 1, "1 == 1") != true)
	{
		(void)fprintf(stderr, "errorTest: utlDoAssert(1) failed\n");
		return false;
	}

	utlAssert(1);

	if (utlDoAssert(utlHERE, 1 == 0, "1 == 0") != false)
	{
		(void)fprintf(stderr, "errorTest: utlDoAssert(2) failed\n");
		return false;
	}

	utlError("Test message %d", 1);

	utlSigError("Test message %d", 2);

	return true;
}
#endif /* utlTEST */

