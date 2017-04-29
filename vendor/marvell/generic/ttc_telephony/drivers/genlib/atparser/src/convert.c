/*****************************************************************************
* Utility Library
*
* DESCRIPTION
*    Data conversion utilities
*****************************************************************************/

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include <utlTypes.h>
#include <utlError.h>

#include <utlConvert.h>


/*--- Configuration ---------------------------------------------------------*/



/*---------------------------------------------------------------------------*
* FUNCTION
*      utlDecimalToString(string_p, value, string_siz)
* INPUT
*        string_p == where to place the converted result
*           value == the decimal number to convert
*      string_siz == size (in characters) of the array pointed to by `string_p'
* OUTPUT
*      none
* RETURNS
*      the number of characters written to `string_p' (excluding null terminator)
* DESCRIPTION
*      Converts an integer number to a null-terminated decimal character string.
*---------------------------------------------------------------------------*/
size_t utlDecimalToString(      char         *string_p,
				unsigned int value,
				const size_t string_siz)
{
	size_t i;
	char digits[20]; /* worst case: 19 digits are required for 64 bit ints */
	char   *c_p;
	char   *term_c_p;

	utlAssert(string_p   != NULL);
	utlAssert(string_siz >  (size_t)0);

	/*--- generate digits ---*/
	i = utlNumberOf(digits) - 1;
	for (; value != (unsigned int)0; value /= 10U)
		digits[i--] = '0' + (value % 10U);

	/*--- were no digits generated? ---*/
	if (i == (utlNumberOf(digits) - 1))
		digits[i] = '0';
	else
		i++;

	/*--- transfer digits to `string_p' ---*/
	term_c_p = string_p + string_siz;
	for (c_p = string_p; (i < utlNumberOf(digits)) && (c_p < term_c_p); )
		*c_p++ = digits[i++];

	/*--- if output buffer was too short, return an empty string ---*/
	if (c_p >= term_c_p)
	{
		utlError("Buffer overflow.");
		c_p = string_p;
	}

	*c_p = '\0';

	return c_p - string_p;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlHexadecimalToString(string_p, value, string_siz)
* INPUT
*        string_p == where to place the converted result
*           value == the hexadecimal number to convert
*      string_siz == size (in characters) of the array pointed to by `string_p'
* OUTPUT
*      none
* RETURNS
*      the number of characters written to `string_p' (excluding null terminator)
* DESCRIPTION
*      Converts an integer number to a null-terminated hexadecimal character
*      string.
*---------------------------------------------------------------------------*/
size_t utlHexadecimalToString(      char         *string_p,
				    unsigned int value,
				    const size_t string_siz)
{
	static const char hex_digits[] = "0123456789ABCDEF";

	size_t i;
	char digits[17]; /* worst case: 16 hex digits are required for 64 bit ints */
	char   *c_p;
	char   *term_c_p;

	utlAssert(string_p   != NULL);
	utlAssert(string_siz >  (size_t)0);

	/*--- generate digits ---*/
	i = utlNumberOf(digits) - 1;
	for (; value != (unsigned int)0; value >>= 4)
		digits[i--] = hex_digits[value & 0xf];

	/*--- were no digits generated? ---*/
	if (i == (utlNumberOf(digits) - 1))
		digits[i] = '0';
	else
		i++;

	/*--- transfer digits to `string_p' ---*/
	term_c_p = string_p + string_siz;
	for (c_p = string_p; (i < utlNumberOf(digits)) && (c_p < term_c_p); )
		*c_p++ = digits[i++];

	/*--- if output buffer was too short, return an empty string ---*/
	if (c_p >= term_c_p)
	{
		utlError("Buffer overflow");
		c_p = string_p;
	}

	*c_p = '\0';

	return c_p - string_p;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*      utlBinaryToString(string_p, value, string_siz)
* INPUT
*        string_p == where to place the converted result
*           value == the binary number to convert
*      string_siz == size (in characters) of the array pointed to by `string_p'
* OUTPUT
*      none
* RETURNS
*      the number of characters written to `string_p' (excluding null terminator)
* DESCRIPTION
*      Converts an integer number to a null-terminated binary character string.
*---------------------------------------------------------------------------*/
size_t utlBinaryToString(      char         *string_p,
			       unsigned int value,
			       const size_t string_siz)
{
	unsigned int mask;
	char         *c_p;
	char         *term_c_p;

	utlAssert(string_p   != NULL);
	utlAssert(string_siz >  (size_t)0);

	c_p = string_p;
	term_c_p = string_p + string_siz;

	mask = 0x1 << ((sizeof(unsigned int) * 8) - 1);

	/*--- skip leading zeros ---*/
	while ((mask & value) == (unsigned int)0)
	{
		if (mask == (unsigned int)0)
		{
			*c_p++ = '0';
			break;
		}

		mask >>= 1;
	}

	/*--- compose binary number ---*/
	while ((mask != (unsigned int)0) && (c_p < term_c_p))
	{

		if ((mask & value) != (unsigned int)0) *c_p++ = '1';
		else *c_p++ = '0';

		mask >>= 1;
	}

	/*--- if output buffer was too short, return an empty string ---*/
	if (c_p >= term_c_p)
	{
		utlError("Buffer overflow.");
		c_p = string_p;
	}

	*c_p = '\0';

	return c_p - string_p;
}

#ifdef utlTEST

/*---------------------------------------------------------------------------*
* FUNCTION
*      convertTest()
* INPUT
*      none
* OUTPUT
*      none
* RETURNS
*      "true" for pass, "false" for failure
*---------------------------------------------------------------------------*/
bool convertTest(void)
{
	char buf[200];

	/*-- decimal-to-string --------------------------------------------------*/
	if (utlDecimalToString(buf, 0, utlNumberOf(buf)) != 1)
	{
		fprintf(stderr, "atParserTest: utlDecimalToString(0) failed\n");
		return false;
	}
	if (strcmp(buf, "0") != 0)
	{
		fprintf(stderr, "atParserTest: utlDecimalToString(1) failed\n");
		return false;
	}
	if (utlDecimalToString(buf, 1, utlNumberOf(buf)) != 1)
	{
		fprintf(stderr, "atParserTest: utlutlDecimalToString(2) failed\n");
		return false;
	}
	if (strcmp(buf, "1") != 0)
	{
		fprintf(stderr, "atParserTest: utlDecimalToString(3) failed\n");
		return false;
	}
	if (utlDecimalToString(buf, 10, utlNumberOf(buf)) != 2)
	{
		fprintf(stderr, "atParserTest: utlDecimalToString(4) failed\n");
		return false;
	}
	if (strcmp(buf, "10") != 0)
	{
		fprintf(stderr, "atParserTest: utlDecimalToString(5) failed\n");
		return false;
	}
	if (utlDecimalToString(buf, 100, utlNumberOf(buf)) != 3)
	{
		fprintf(stderr, "atParserTest: utlDecimalToString(6) failed\n");
		return false;
	}
	if (strcmp(buf, "100") != 0)
	{
		fprintf(stderr, "atParserTest: utlDecimalToString(7) failed\n");
		return false;
	}

	/*-- hex-to-string ------------------------------------------------------*/
	if (utlHexadecimalToString(buf, 0, utlNumberOf(buf)) != 1)
	{
		fprintf(stderr, "atParserTest: utlHexadecimalToString(1) failed\n");
		return false;
	}
	if (strcmp(buf, "0") != 0)
	{
		fprintf(stderr, "atParserTest: utlHexadecimalToString(2) failed\n");
		return false;
	}
	if (utlHexadecimalToString(buf, 1, utlNumberOf(buf)) != 1)
	{
		fprintf(stderr, "atParserTest: utlHexadecimalToString(3) failed\n");
		return false;
	}
	if (strcmp(buf, "1") != 0)
	{
		fprintf(stderr, "atParserTest: utlHexadecimalToString(4) failed\n");
		return false;
	}
	if (utlHexadecimalToString(buf, 10, utlNumberOf(buf)) != 1)
	{
		fprintf(stderr, "atParserTest: utlHexadecimalToString(5) failed\n");
		return false;
	}
	if (strcmp(buf, "A") != 0)
	{
		fprintf(stderr, "atParserTest: utlHexadecimalToString(6) failed\n");
		return false;
	}
	if (utlHexadecimalToString(buf, 100, utlNumberOf(buf)) != 2)
	{
		fprintf(stderr, "atParserTest: utlHexadecimalToString(7) failed\n");
		return false;
	}
	if (strcmp(buf, "64") != 0)
	{
		fprintf(stderr, "atParserTest: utlHexadecimalToString(8) failed\n");
		return false;
	}

	/*-- binary-to-string ---------------------------------------------------*/
	if (utlBinaryToString(buf, 0, utlNumberOf(buf)) != 1)
	{
		fprintf(stderr, "atParserTest: utlBinaryToString(1) failed\n");
		return false;
	}
	if (strcmp(buf, "0") != 0)
	{
		fprintf(stderr, "atParserTest: utlBinaryToString(2) failed\n");
		return false;
	}
	if (utlBinaryToString(buf, 1, utlNumberOf(buf)) != 1)
	{
		fprintf(stderr, "atParserTest: utlBinaryToString(3) failed\n");
		return false;
	}
	if (strcmp(buf, "1") != 0)
	{
		fprintf(stderr, "atParserTest: utlBinaryToString(4) failed\n");
		return false;
	}
	if (utlBinaryToString(buf, 10, utlNumberOf(buf)) != 4)
	{
		fprintf(stderr, "atParserTest: utlBinaryToString(5) failed\n");
		return false;
	}
	if (strcmp(buf, "1010") != 0)
	{
		fprintf(stderr, "atParserTest: utlBinaryToString(6) failed\n");
		return false;
	}
	if (utlBinaryToString(buf, 100, utlNumberOf(buf)) != 7)
	{
		fprintf(stderr, "atParserTest: utlBinaryToString(7) failed\n");
		return false;
	}
	if (strcmp(buf, "1100100") != 0)
	{
		fprintf(stderr, "atParserTest: utlBinaryToString(8) failed\n");
		return false;
	}

	return true;
}
#endif  /* utlTEST */

