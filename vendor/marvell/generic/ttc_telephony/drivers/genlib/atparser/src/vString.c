/*****************************************************************************
* Utility Library
*
* DESCRIPTION
*     Functions to manage variable-length character strings.
*
* EXAMPLE USAGE
*
*     {
*     utlVString_T v_string;
*
*         utlInitVString(&v_string);
*
*         if (utlVStringCat(&v_string, "one ") != utlSUCCESS)
*             <handle error here>
*
*         if (utlVStringCat(&v_string, "two ") != utlSUCCESS)
*             <handle error here>
*
*         if (utlVStringCat(&v_string, "three ") != utlSUCCESS)
*             <handle error here>
*
*         (void) printf("string: %s\n", utlVString(&v_string))
*
*         <use `v_string' here>
*
*         if (utlVStringFree(&v_string) != utlSUCCESS)
*             <handle error here>
*
*         {
*         utlVString_T print_v_string;
*
*             utlInitVString(&print_v_string);
*
*             (void) utlVStringPrintF(&print_v_string, "my %s string", "format");
*
*             <use `print_v_string' here>
*
*             if (utlVStringFree(&v_string) != utlSUCCESS)
*                 <handle error here>
*         }
*     }
*
*****************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include <utlTypes.h>

#include <utlError.h>
#include <utlMalloc.h>
#include <utlTime.h>
#include <utlVString.h>


/*---------------------------------------------------------------------------*
* FUNCTION
*	utlVStringReserve(v_string_p, n)
* INPUT
*      v_string_p == pointer to an initialized utlVString_T structure
*               n == the number of characters to reserve space for
* OUTPUT
*	*v_string_p == the updated utlVString_T structure
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure.
* DESCRIPTION
*	Reserves space for `n' additional characters in the variable-length
*	string specified by `v_string_p'.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlVStringReserve(const utlVString_P v_string_p,
				  const size_t n)
{
	utlAssert(v_string_p != NULL);

	/*--- make room ---*/
	if (v_string_p->size == (size_t)0)
	{
		size_t size;

		size = n + 1;

		/*--- for improved performance, add some bonus space ---*/
		size += 32;

		if ((v_string_p->s_p = utlMalloc(size * sizeof(char))) == NULL)
			return utlFAILED;

		v_string_p->size = size;

		v_string_p->s_p[0] = '\0';

	}
	else if ((n + v_string_p->length + 1) > v_string_p->size)
	{
		size_t new_size;
		char   *new_s_p;

		new_size = n + v_string_p->length + 1;

		/*--- for improved performance, add some bonus space ---*/
		if (v_string_p->size <  128) new_size +=   32;
		else if (v_string_p->size < 1024) new_size +=  128;
		else if (v_string_p->size < 4096) new_size += 1024;
		else if (v_string_p->size < 8192) new_size += 4096;
		else new_size += 8192;

		if ((new_s_p = (char *)utlRealloc(v_string_p->s_p, new_size * sizeof(char))) == NULL)
			return utlFAILED;

		v_string_p->s_p  = new_s_p;
		v_string_p->size = new_size;
	}

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlVStringCat(v_string_p, s_p)
* INPUT
*      v_string_p == pointer to an initialized utlVString_T structure
*             s_p == pointer to the string to concatenate
* OUTPUT
*	*v_string_p == the updated utlVString_T structure
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure.
* DESCRIPTION
*	Concatenates the string `s_p' onto the end of the string stored in
*      the variable-length string specified by `v_string_p'.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlVStringCat(const utlVString_P v_string_p,
			      const char         *s_p)
{
	size_t len;

	utlAssert(v_string_p != NULL);

	/*--- nothing to concatenate? ---*/
	if (s_p == NULL)
	{
		if (utlVStringReserve(v_string_p, 0) != utlSUCCESS)
			return utlFAILED;

		return utlSUCCESS;
	}

	len = strlen(s_p);

	if (utlVStringReserve(v_string_p, len) != utlSUCCESS)
		return utlFAILED;

	/*--- concatenate! ---*/
	{
		const char *src_p;
		char *dest_p;

		for (src_p = s_p, dest_p = v_string_p->s_p + v_string_p->length; *src_p != '\0'; src_p++)
		{
			*dest_p++ = *src_p;
		}
		*dest_p = '\0';
	}

	v_string_p->length += len;

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlVStringFree(v_string_p)
* INPUT
*      v_string_p == pointer to an initialized utlVString_T structure
* OUTPUT
*	*v_string_p == the updated utlVString_T structure
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure.
* DESCRIPTION
*	Frees the resource consumed by the variable-length string specified
*      by `v_string_p'.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlVStringFree(const utlVString_P v_string_p)
{
	utlAssert(v_string_p != NULL);

	if (v_string_p->s_p != NULL)
	{
		utlFree(v_string_p->s_p);
		v_string_p->length = 0;
		v_string_p->size   = 0;
	}

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlVStringVPrintF(v_string_p, format_p, va_arg_p)
* INPUT
*      v_string_p == pointer to an initialized utlVString_T structure
*        format_p == a null terminated format string
*        va_arg_p == argument pointer
* OUTPUT
*	*v_string_p == the updated utlVString_T structure
* RETURNS
*      the number of characters placed into `v_string_p' for success,
*      utlFAILED for failure.
* DESCRIPTION
*      Similar to vsprintf(), but prints into a variable-length string.
*---------------------------------------------------------------------------*/
int utlVStringVPrintF(const utlVString_P v_string_p,
		      const char         *format_p,
		      va_list va_arg_p)
{
	int c_count;
	size_t len;

	/*--- check parameters ---*/
	utlAssert(v_string_p != NULL);
	utlAssert(format_p   != NULL);

	/*--- compute length of format string (for use in estimating) ---*/
	len = strlen(format_p);

	c_count = 0;

	/*--- keep trying to sprintf() until the string buffer is big enough ---*/
	do {
		/* is string buffer too small? */
		if ((v_string_p->length + len + 250 + c_count) > v_string_p->size)
		{
			size_t new_size;
			char   *new_v_string_p;

			/*--- compute new string-buffer size ---*/
			new_size = (2 * v_string_p->size) + len + 256;

			/*--- make string-buffer a little bigger ---*/
			if ((new_v_string_p = utlRealloc(v_string_p->s_p, new_size * sizeof(char))) == NULL)
				return utlFAILED;

			v_string_p->size = new_size;
			v_string_p->s_p  = new_v_string_p;
		}

		/* attempt to append new string */
		c_count = vsnprintf(v_string_p->s_p  + v_string_p->length,
				    v_string_p->size - v_string_p->length,
				    format_p,
				    va_arg_p);
	} while (((size_t)c_count >= (v_string_p->size - len - 1)) || (c_count < 0));

	v_string_p->length += strlen(v_string_p->s_p + v_string_p->length);

	return c_count;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlVStringPrintF(v_string_p, format_p, ...)
* INPUT
*      v_string_p == pointer to an initialized utlVString_T structure
*        format_p == a null terminated format string
*             ... == zero or more additional arguments
* OUTPUT
*	*v_string_p == the updated utlVString_T structure
* RETURNS
*      the number of characters placed into `v_string_p' for success,
*      utlFAILED for failure.
* DESCRIPTION
*      Similar to sprintf(), but prints into a variable-length string.
*---------------------------------------------------------------------------*/
int utlVStringPrintF(const utlVString_P v_string_p,
		     const char         *format_p,
		     ...)
{
	va_list va_arg_p;
	int rv;

	utlAssert(v_string_p != NULL);
	utlAssert(format_p   != NULL);

	va_start(va_arg_p, format_p);
	rv = utlVStringVPrintF(v_string_p, format_p, va_arg_p);
	va_end(va_arg_p);

	return rv;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlVStringPuts(v_string_p, file_p)
* INPUT
*      v_string_p == pointer to an initialized utlVString_T structure
*          file_p == the stream to direct the output to
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure.
* DESCRIPTION
*	Prints the character string specified by `v_string_p' to the specified
*      file descriptor.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlVStringPuts(const utlVString_P2c v_string_p,
			       FILE           *file_p)
{
	char *c_p;

	utlAssert(v_string_p != NULL);

	if ((c_p = utlVString(v_string_p)) == NULL)
		return utlSUCCESS;

	/*--- the following loop addresses a problem on the target hardware
	      where TX buffer over-run errors occur when we try to print very
	      long variable-length strings.  This loop prints one line at a
	      time, pausing 10 msec between each iteration. */
	while (*c_p != '\0')
	{
		char *end_p;

		/*--- search for end of this line ---*/
		if ((end_p = strchr(c_p, '\n')) == NULL)
		{
			(void)fprintf(file_p, "%s\n", c_p);
			break;
		}

		/*--- print line ---*/
		(void)fprintf(file_p, "%.*s\n", end_p - c_p, c_p);

		/*--- pause 10 milliseconds ---*/
		{
			utlRelativeTime_T delay;

			delay.seconds     = 0;
			delay.nanoseconds = 10000;
			(void)utlPause(&delay);
		}

		/*--- note where the next line starts ---*/
		c_p = end_p + 1;
	}

	return utlSUCCESS;
}

#ifdef utlTEST
/*---------------------------------------------------------------------------*
* FUNCTION
*	vStringTest()
* INPUT
*	none
* OUTPUT
*	none
* RETURNS
*	"true" for pass, "false" for failure
*---------------------------------------------------------------------------*/
bool vStringTest(void)
{
	utlVString_T v_string = utlEMPTY_VSTRING;
	int i;

	/*--- test utlVStringCat() -----------------------------------------------*/
	utlInitVString(&v_string);

	if (utlVStringCat(&v_string, "abc") != utlSUCCESS)
	{
		(void)fprintf(stderr, "vStringTest: utlVStringCat(1) failed\n");
		return false;
	}

	if (utlVStringCat(&v_string, "def") != utlSUCCESS)
	{
		(void)fprintf(stderr, "vStringTest: utlVStringCat(2) failed\n");
		return false;
	}

	if (utlVStringCat(&v_string, "ghi") != utlSUCCESS)
	{
		(void)fprintf(stderr, "vStringTest: utlVStringCat(3) failed\n");
		return false;
	}

	if (strcmp("abcdefghi", utlVString(&v_string)) != 0)
	{
		(void)fprintf(stderr, "vStringTest: utlVStringCat(4) failed\n");
		return false;
	}

	if (utlVStringFree(&v_string) != utlSUCCESS)
	{
		(void)fprintf(stderr, "vStringTest: utlVStringCat(5) failed\n");
		return false;
	}


	if (utlVStringCat(&v_string, "") != utlSUCCESS)
	{
		(void)fprintf(stderr, "vStringTest: utlVStringCat(6) failed\n");
		return false;
	}

	for (i = 0; i < 2000; i++)
	{
		if (utlVStringCat(&v_string, "x") != utlSUCCESS)
		{
			(void)fprintf(stderr, "vStringTest: utlVStringCat(7) failed\n");
			return false;
		}
	}

	if (utlVStringFree(&v_string) != utlSUCCESS)
	{
		(void)fprintf(stderr, "vStringTest: utlVStringCat(8) failed\n");
		return false;
	}


	/*--- test utlVStringPrintF() -----------------------------------------------*/
	utlInitVString(&v_string);

	if (utlVStringPrintF(&v_string, "abc") != 3)
	{
		(void)fprintf(stderr, "vStringTest: utlVStringPrintF(1) failed\n");
		return false;
	}

	if (utlVStringPrintF(&v_string, "[%d]", 1) != 3)
	{
		(void)fprintf(stderr, "vStringTest: utlVStringPrintF(2) failed\n");
		return false;
	}

	if (strcmp(utlVString(&v_string), "abc[1]") != 0)
	{
		(void)fprintf(stderr, "vStringTest: utlVStringPrintF(3) failed\n");
		return false;
	}

	if (utlVStringFree(&v_string) != utlSUCCESS)
	{
		(void)fprintf(stderr, "vStringTest: utlVStringPrintF(4) failed\n");
		return false;
	}

	return true;
}
#endif /* utlTEST */

