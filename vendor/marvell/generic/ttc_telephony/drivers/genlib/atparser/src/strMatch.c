/*****************************************************************************
* Utility Library
*
* DESCRIPTION
*     Functions to perform GLOB string comparisons.
*****************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <utlTypes.h>
#include <utlError.h>
#include <utlStrMatch.h>

#define UNUSEDPARAM(param) (void)param;
/*---------------------------------------------------------------------------*
* FUNCTION
*	utlStrIncrMatch(s_p, size, pattern_p, ignore_case, largest_match)
* INPUT
*                s_p == pointer to a character string
*               size == number of characters pointed to by `s_p'
*          pattern_p == the pattern we are looking for, expressed in extended
*                       pattern matching notation.
*        ignore_case == ignore character case
*      largest_match == look for the largest possible match
* OUTPUT
*	none
* RETURNS
*      The number of characters matched for success, utlTOO_SHORT if `s_p' is
*      too short for a complete match, utlFAILED for no match.
* DESCRIPTION
*       Checks to see if the pattern pointed to by `pattern_p' appears at
*       the beginning of `s_p'.
*
*       The pattern string may contain any of the following special characters:
*
*         ?             matches a single character except <new-line>
*         *             matches zero or more characters
*         $             matches zero or more characters until end of line
*         #             matches zero or more digits (same as {[:digit:]})
*         \             escapes the next character to prevent special interpretation
*         []            matching collections, e.g. [abc-k]
*         [!]           non-matching collections, e.g. [!abc-k]
*         [[:alpha:]]   matches exactly one  isalpha()    character
*         [[:upper:]]   matches exactly one  isupper()    character
*         [[:lower:]]   matches exactly one  islower()    character
*         [[:digit:]]   matches exactly one  isdigit()    character
*         [[:xdigit:]]  matches exactly one  isxdigit()   character
*         [[:fdigit:]]  matches exactly one  fp value     character
*         [[:alnum:]]   matches exactly one  isalnum()    character
*         [[:space:]]   matches exactly one  isspace()    character
*         [[:blank:]]   matches exactly one  space or tab character
*         [[:newln:]]   matches exactly one  cr, nl, ff   character
*         [[:print:]]   matches exactly one  isprint()    character
*         [[:punct:]]   matches exactly one  ispunct()    character
*         [[:graph:]]   matches exactly one  isgraph()    character
*         [[:cntrl:]]   matches exactly one  iscntrl()    character
*         {}            matches zero or more occurrences of the given character, eg {a}
*         {!}           non-matching repeated occurrences, eg {!a}
*         {[:alpha:]}   matches zero or more isalpha()    characters
*         {[:upper:]}   matches zero or more isupper()    characters
*         {[:lower:]}   matches zero or more islower()    characters
*         {[:digit:]}   matches zero or more isdigit()    characters
*         {[:xdigit:]}  matches zero or more isxdigit()   characters
*         {[:fdigit:]}  matches zero or more fp value     characters
*         {[:alnum:]}   matches zero or more isalnum()    characters
*         {[:space:]}   matches zero or more isspace()    characters
*         {[:blank:]}   matches zero or more space & tab  characters
*         {[:newln:]}   matches zero or more cr & nl      characters
*         {[:print:]}   matches zero or more isprint()    characters
*         {[:punct:]}   matches zero or more ispunct()    characters
*         {[:graph:]}   matches zero or more isgraph()    characters
*         {[:cntrl:]}   matches zero or more iscntrl()    characters
*
*---------------------------------------------------------------------------*/
int utlStrIncrMatch(const char   *s_p,
		    const size_t size,
		    const char   *pattern_p,
		    const bool ignore_case,
		    const bool largest_match)
{
	const char *c_p;
	const char *collection_end_p;
	bool non_match;
	char range_first;
	size_t n;

	utlAssert(s_p       != NULL);
	utlAssert(pattern_p != NULL);

	if (size == (size_t)0)
		return utlTOO_SHORT;

	for (n = 0; *pattern_p != '\0'; pattern_p++)
	{
		switch (*pattern_p)
		{

		case '?': /* match any single character except <new-line> */
			if (*s_p == '\n')
				return utlFAILED;

			s_p++;
			n++;
			break;

		case '#':               /* match zero or more digits */
			pattern_p++;    /* skip number sign */

			/*--- if number-sign appears at end of pattern, match all remaining digits in `s_p' ---*/
			if (*pattern_p == '\0')
			{
				while (utlIsDigit(*s_p))
				{
					s_p++;
					n++;

					if (n == size)
						break;
				}

				return n;
			}

			{
				int rv,   largest_rv;
				const char *c_p, *largest_c_p;

				rv         = utlFAILED;
				largest_rv = utlFAILED;

				largest_c_p = NULL;

				/*--- try each possibility in turn... ---*/
				for (c_p = s_p; *c_p != '\0'; c_p++)
				{
					if ((n + (c_p - s_p)) >= size)
					{
						rv = utlTOO_SHORT;
						break;
					}

					if ((rv = utlStrIncrMatch(c_p, size - (n + (c_p - s_p)), pattern_p, ignore_case, largest_match)) > 0)
					{
						if (largest_match == false)
							return n + (c_p - s_p) + rv;

						largest_c_p = c_p;
						largest_rv  = rv;
					}

					if (!utlIsDigit(*c_p))
						break;
				}

				if (largest_rv != utlFAILED)
					return n + (largest_c_p - s_p) + largest_rv;

				return rv;
			}
			break;

		case '*':               /* match zero or more characters */
			pattern_p++;    /* skip asterisk character */

			/*--- if asterisk appears at end of pattern, match all remaining characters in `s_p' ---*/
			if (*pattern_p == '\0')
				return n + strlen(s_p);

			{
				int rv,   largest_rv;
				const char *c_p, *largest_c_p;

				rv         = utlFAILED;
				largest_rv = utlFAILED;

				largest_c_p = NULL;

				/*--- try each possibility in turn... ---*/
				for (c_p = s_p; *c_p != '\0'; c_p++)
				{
					if ((n + (c_p - s_p)) >= size)
					{
						rv = utlTOO_SHORT;
						break;
					}

					if ((rv = utlStrIncrMatch(c_p, size - (n + (c_p - s_p)), pattern_p, ignore_case, largest_match)) > 0)
					{
						if (largest_match == false)
							return n + (c_p - s_p) + rv;

						largest_c_p = c_p;
						largest_rv  = rv;
					}
				}

				if (largest_rv != utlFAILED)
					return n + (largest_c_p - s_p) + largest_rv;

				return rv;
			}
			break;

		case '$':               /* match zero or more characters until end of line */
			pattern_p++;    /* skip dollar-sign */

			{
				int first_rv;
				int rv;
				const char *c_p;
				bool found_eol;

				/*--- first match rest of pattern against what's left on the current line ---*/
				if ((first_rv = utlStrIncrMatch(s_p, size - n, pattern_p, ignore_case, largest_match)) > 0)
					return n + first_rv;

				/*--- skip to end of current line... ---*/
				found_eol = false;
				for (c_p = s_p; *c_p != '\0'; c_p++)
				{
					if ((size_t)(c_p - s_p) == size)
						break;

					if (*c_p == '\n')
					{
						c_p++; /* skip carriage-return */

						if (*c_p == '\r')
							c_p++;  /* skip line-feed */

						n  += c_p - s_p;
						s_p = c_p;

						found_eol = true;
						break;
					}

					if (*c_p == '\r')
					{
						c_p++; /* skip line-feed */

						if (*c_p == '\n')
							c_p++;  /* skip carriage-return */

						n  += c_p - s_p;
						s_p = c_p;

						found_eol = true;
						break;
					}
				}

				if (found_eol != true)
					return utlTOO_SHORT;

/*oxoxo largest match???*/
				/*--- if we've reached the end of `pattern_p' ---*/
				if (*pattern_p == '\0')
					return n;

				/*--- if we've reached the end of `s_p' ---*/
				if ((*s_p == '\0') || (n >= size))
					return utlTOO_SHORT;

				/* then try matching against what's left after the current line */
				if ((rv = utlStrIncrMatch(s_p, size - n, pattern_p, ignore_case, largest_match)) > 0)
					return n + rv;

				if (first_rv == utlTOO_SHORT)
					return utlTOO_SHORT;

				return rv;
			}
			break;

		case '[':               /* match a collection */
			pattern_p++;    /* skip opening bracket character */

			/*--- this a non-matching collection? ---*/
			if (*pattern_p == '!')
			{
				pattern_p++; /* skip exclaim character */
				non_match = true;
			}
			else
				non_match = false;

			/*--- at this point `pattern_p' points to the first character
			      of the matching/non-matching collection */

			/*--- find end of collection ---*/
			for (collection_end_p = pattern_p;; )
			{

				/*--- unexpected end-of-pattern? (due to missing close-bracket) ---*/
				if (*collection_end_p == '\0')
					return utlFAILED;

				else if (*collection_end_p == ']')
				{
					/*--- if ']' does not appear right after collection's open-bracket, we're done ---*/
					if (collection_end_p != pattern_p)
						break;

					collection_end_p++; /* skip close-bracket */

					/*--- is next character escaped? ---*/
					if (*collection_end_p == '\\')
						collection_end_p++;  /* skip escaped character */

					/*--- else does `collection_end_p' point to a possible collection class? ---*/
				}
				else if (*collection_end_p == '[')
				{
					if (strncmp(collection_end_p, "[:alpha:]",   9) == 0) collection_end_p +=  9;
					else if (strncmp(collection_end_p, "[:upper:]",   9) == 0) collection_end_p +=  9;
					else if (strncmp(collection_end_p, "[:lower:]",   9) == 0) collection_end_p +=  9;
					else if (strncmp(collection_end_p, "[:digit:]",   9) == 0) collection_end_p +=  9;
					else if (strncmp(collection_end_p, "[:xdigit:]", 10) == 0) collection_end_p += 10;
					else if (strncmp(collection_end_p, "[:fdigit:]", 10) == 0) collection_end_p += 10;
					else if (strncmp(collection_end_p, "[:alnum:]",   9) == 0) collection_end_p +=  9;
					else if (strncmp(collection_end_p, "[:space:]",   9) == 0) collection_end_p +=  9;
					else if (strncmp(collection_end_p, "[:blank:]",   9) == 0) collection_end_p +=  9;
					else if (strncmp(collection_end_p, "[:newln:]",   9) == 0) collection_end_p +=  9;
					else if (strncmp(collection_end_p, "[:print:]",   9) == 0) collection_end_p +=  9;
					else if (strncmp(collection_end_p, "[:punct:]",   9) == 0) collection_end_p +=  9;
					else if (strncmp(collection_end_p, "[:graph:]",   9) == 0) collection_end_p +=  9;
					else if (strncmp(collection_end_p, "[:cntrl:]",   9) == 0) collection_end_p +=  9;
					else
					{
						collection_end_p++; /* skip open-bracket */

						/*--- is next character escaped? ---*/
						if (*collection_end_p == '\\')
							collection_end_p++;  /* skip escaped character */
					}
				}
				else
				{
					collection_end_p++; /* skip character */

					/*--- is next character escaped? ---*/
					if (*collection_end_p == '\\')
						collection_end_p++;  /* skip escaped character */
				}
			}

			/*--- at this point `pattern_p' points to the first character of the
			      matching/non-matching collection and `collection_end_p' points
			      to the first cahracter following the matching/non-matching
			      collection */

			/*--- note where range should start (in case one follows) ---*/
			range_first = *pattern_p;

			/*--- compare each entry in the collection to `*s_p'... ---*/
			for (c_p = pattern_p; c_p < collection_end_p; c_p++)
			{

				/*--- found a range? ---*/
				if (*c_p == '-')
				{

					/*--- if `-' is not the first or last member of the collection ---*/
					if ((c_p > pattern_p) && (c_p < (collection_end_p - 1)))
					{
						char a, b, c;

						c_p++; /* skip range character */

						/*--- compare `*s_p' to range ---*/
						if (ignore_case == true) a = utlToLower(*s_p), b = utlToLower(*c_p), c = utlToLower(range_first);
						else a =           *s_p, b =           *c_p, c =           range_first;

						if ((a >= c) && (a <= b))
							break;

						range_first = *s_p;

					}
					else
					{
						/*--- compare `*s_p' to range character ---*/
						if (*s_p == *c_p)
							break;
					}

					/*--- else does `c_p' point to a possible collection class? ---*/
				}
				else if (*c_p == '[')
				{
					int match;

					match = 0;

					if (strncmp(c_p, "[:alpha:]",   9) == 0) match = utlIsAlpha(*s_p),  c_p += 7;
					else if (strncmp(c_p, "[:upper:]",   9) == 0) match = utlIsUpper(*s_p),  c_p += 7;
					else if (strncmp(c_p, "[:lower:]",   9) == 0) match = utlIsLower(*s_p),  c_p += 7;
					else if (strncmp(c_p, "[:digit:]",   9) == 0) match = utlIsDigit(*s_p),  c_p += 7;
					else if (strncmp(c_p, "[:xdigit:]", 10) == 0) match = utlIsXDigit(*s_p), c_p += 8;
					else if (strncmp(c_p, "[:fdigit:]", 10) == 0) match = utlIsDigit(*s_p) ||
											      (*s_p == '+')   ||
											      (*s_p == '-')   ||
											      (*s_p == '.')   ||
											      (*s_p == 'e')   ||
											      (*s_p == 'E'),   c_p += 8;
					else if (strncmp(c_p, "[:alnum:]",   9) == 0) match = utlIsAlNum(*s_p), c_p += 7;
					else if (strncmp(c_p, "[:space:]",   9) == 0) match = utlIsSpace(*s_p), c_p += 7;
					else if (strncmp(c_p, "[:blank:]",   9) == 0) match = (*s_p == ' ')   ||
											      (*s_p == '\t'),  c_p += 7;
					else if (strncmp(c_p, "[:newln:]",   9) == 0) match = (*s_p == '\f')  ||
											      (*s_p == '\n')  ||
											      (*s_p == '\r'),  c_p += 7;
					else if (strncmp(c_p, "[:print:]",   9) == 0) match = utlIsPrint(*s_p), c_p += 7;
					else if (strncmp(c_p, "[:punct:]",   9) == 0) match = utlIsPunct(*s_p), c_p += 7;
					else if (strncmp(c_p, "[:graph:]",   9) == 0) match = utlIsGraph(*s_p), c_p += 7;
					else if (strncmp(c_p, "[:cntrl:]",   9) == 0) match = utlIsCntrl(*s_p), c_p += 7;
					else
					{
						/*--- compare `*s_p' to `['---*/
						if (*s_p == *c_p)
							match = 1;
					}

					if (match)
						break;

				}
				else if (*c_p == '\\')
				{
					char a, b;

					c_p++; /* skip escape character */

					/*--- compare `*s_p' to `*c_p' ---*/
					if (ignore_case == true) a = utlToLower(*s_p), b = utlToLower(*c_p);
					else a =           *s_p, b =           *c_p;

					if (a == b)
						break;

				}
				else
				{
					char a, b;

					/*--- compare `*s_p' to `*c_p' ---*/
					if (ignore_case == true) a = utlToLower(*s_p), b = utlToLower(*c_p);
					else a =           *s_p, b =           *c_p;

					if (a == b)
						break;
				}
			}

			if (((non_match == false) && (c_p >= collection_end_p)) ||
			    ((non_match ==  true) && (c_p <  collection_end_p)))
				return utlFAILED;

			/*--- skip collection ---*/
			pattern_p = collection_end_p;

			s_p++;
			n++;
			break;

		case '{':               /* match zero or more characters of a given collection class */
			pattern_p++;    /* skip opening brace character */

			/*--- this a non-matching collection? ---*/
			if (*pattern_p == '!')
			{
				pattern_p++; /* skip exclaim character */
				non_match = true;
			}
			else
				non_match = false;
/*oxo
   need to add support for "longest_match" flag
 */

			/*--- at this point `pattern_p' points to the first character
			      of the matching/non-matching collection */

			/*--- find end of collection ---*/
			for (collection_end_p = pattern_p;; )
			{

				/*--- unexpected end-of-pattern? (due to missing close-bracket) ---*/
				if (*collection_end_p == '\0')
					return utlFAILED;

				else if (*collection_end_p == '}')
				{

					/*--- if '}' does not appear right after collection's open-brace, we're done ---*/
					if (collection_end_p != pattern_p)
						break;

					collection_end_p++; /* skip close-brace */

					/*--- is next character escaped? ---*/
					if (*collection_end_p == '\\')
						collection_end_p++;  /* skip escaped character */

					/*--- else does `collection_end_p' point to a possible collection class? ---*/
				}
				else if (*collection_end_p == '[')
				{
					if (strncmp(collection_end_p, "[:alpha:]",   9) == 0) collection_end_p +=  9;
					else if (strncmp(collection_end_p, "[:upper:]",   9) == 0) collection_end_p +=  9;
					else if (strncmp(collection_end_p, "[:lower:]",   9) == 0) collection_end_p +=  9;
					else if (strncmp(collection_end_p, "[:digit:]",   9) == 0) collection_end_p +=  9;
					else if (strncmp(collection_end_p, "[:xdigit:]", 10) == 0) collection_end_p += 10;
					else if (strncmp(collection_end_p, "[:fdigit:]", 10) == 0) collection_end_p += 10;
					else if (strncmp(collection_end_p, "[:alnum:]",   9) == 0) collection_end_p +=  9;
					else if (strncmp(collection_end_p, "[:space:]",   9) == 0) collection_end_p +=  9;
					else if (strncmp(collection_end_p, "[:blank:]",   9) == 0) collection_end_p +=  9;
					else if (strncmp(collection_end_p, "[:newln:]",   9) == 0) collection_end_p +=  9;
					else if (strncmp(collection_end_p, "[:print:]",   9) == 0) collection_end_p +=  9;
					else if (strncmp(collection_end_p, "[:punct:]",   9) == 0) collection_end_p +=  9;
					else if (strncmp(collection_end_p, "[:graph:]",   9) == 0) collection_end_p +=  9;
					else if (strncmp(collection_end_p, "[:cntrl:]",   9) == 0) collection_end_p +=  9;
					else
					{
						collection_end_p++; /* skip open-bracket */

						/*--- is next character escaped? ---*/
						if (*collection_end_p == '\\')
							collection_end_p++;  /* skip escaped character */
					}
				}
				else
				{
					collection_end_p++; /* skip character */

					/*--- is next character escaped? ---*/
					if (*collection_end_p == '\\')
						collection_end_p++;  /* skip escaped character */
				}
			}

			/*--- at this point `pattern_p' points to the first character of the
			      matching/non-matching collection and `collection_end_p' points
			      to the first cahracter following the matching/non-matching
			      collection */

			/*--- short-circuit evaluation if non-matching ---*/
			if (non_match == false)
			{
				/*--- skip all data bytes where the current data byte matches the current
				      pattern and the next byte does not match the next pattern... */
				for (; (*s_p != '\0') && (n < size); )
				{
					bool matched;

					/*--- note where range should start (in case one follows) ---*/
					range_first = *pattern_p;

					/*--- check if collection matches `*s_p'... ---*/
					matched = false;
					for (c_p = pattern_p; c_p < collection_end_p; c_p++)
					{

						/*--- found a range? ---*/
						if (*c_p == '-')
						{

							/*--- if `-' is not the first or last member of the collection ---*/
							if ((c_p > pattern_p) && (c_p < (collection_end_p - 1)))
							{
								char a, b, c;

								c_p++; /* skip range character */

								/*--- compare `*s_p' to range ---*/
								if (ignore_case == true) a = utlToLower(*c_p), b = utlToLower(*c_p), c = utlToLower(range_first);
								else a =           *c_p, b =           *c_p, c =           range_first;

								if ((a >= c) && (a <= b))
								{
									matched = true;
									break;
								}

								range_first = *s_p;

							}
							else
							{
								/*--- compare `*s_p' to `-' ---*/
								if (*s_p == *c_p)
								{
									matched = true;
									break;
								}
							}

							/*--- else does `c_p' point to a possible collection class? ---*/
						}
						else if (*c_p == '[')
						{

							int match;

							match = 0;

							if (strncmp(c_p, "[:alpha:]",   9) == 0) match = utlIsAlpha(*s_p),  c_p += 7;
							else if (strncmp(c_p, "[:upper:]",   9) == 0) match = utlIsUpper(*s_p),  c_p += 7;
							else if (strncmp(c_p, "[:lower:]",   9) == 0) match = utlIsLower(*s_p),  c_p += 7;
							else if (strncmp(c_p, "[:digit:]",   9) == 0) match = utlIsDigit(*s_p),  c_p += 7;
							else if (strncmp(c_p, "[:xdigit:]", 10) == 0) match = utlIsXDigit(*s_p), c_p += 8;
							else if (strncmp(c_p, "[:fdigit:]", 10) == 0) match = utlIsDigit(*s_p) ||
													      (*s_p == '-')   ||
													      (*s_p == '+')   ||
													      (*s_p == '.')   ||
													      (*s_p == 'e')   ||
													      (*s_p == 'E'),   c_p += 8;
							else if (strncmp(c_p, "[:alnum:]",   9) == 0) match = utlIsAlNum(*s_p), c_p += 7;
							else if (strncmp(c_p, "[:space:]",   9) == 0) match = utlIsSpace(*s_p), c_p += 7;
							else if (strncmp(c_p, "[:blank:]",   9) == 0) match = (*s_p == ' ')   ||
													      (*s_p == '\t'),  c_p += 7;
							else if (strncmp(c_p, "[:newln:]",   9) == 0) match = (*s_p == '\f')  ||
													      (*s_p == '\n')  ||
													      (*s_p == '\r'),  c_p += 7;
							else if (strncmp(c_p, "[:print:]",   9) == 0) match = utlIsPrint(*s_p), c_p += 7;
							else if (strncmp(c_p, "[:punct:]",   9) == 0) match = utlIsPunct(*s_p), c_p += 7;
							else if (strncmp(c_p, "[:graph:]",   9) == 0) match = utlIsGraph(*s_p), c_p += 7;
							else if (strncmp(c_p, "[:cntrl:]",   9) == 0) match = utlIsCntrl(*s_p), c_p += 7;
							else
							{
								/*--- compare `*s_p' to `['---*/
								if (*s_p == *c_p)
									match = 1;
							}

							if (match)
							{
								matched = true;
								break;
							}

						}
						else if (*c_p == '\\')
						{
							c_p++; /* skip escape character */

							/*--- compare `*s_p' to `*c_p' ---*/
							if (*s_p == *c_p)
							{
								matched = true;
								break;
							}

						}
						else
						{
							/*--- compare `*s_p' to `*c_p' ---*/
							if (*s_p == *c_p)
							{
								matched = true;
								break;
							}
						}
					}

					/*--- if nothing matched for this char ---*/
					if (((non_match == false) && (matched == false)) ||
					    ((non_match ==  true) && (matched ==  true)))
						break;

					/*--- if there is another pattern directive... ---*/
					if (*(collection_end_p + 1) != '\0')
					{

						/*--- if this char matches the next pattern directive ---*/
						if (utlStrIncrMatch(s_p, 1, collection_end_p + 1, ignore_case, largest_match) > 0)
							break;

						/*--- if the next char (if any) matches the next pattern directive ---*/
						if (n < size)
						{
							int rv;

							if ((rv = utlStrIncrMatch(s_p + 1, 1, collection_end_p + 1, ignore_case, largest_match)) > 0)
								break;

							if (rv == utlTOO_SHORT)
								break;
						}
					}

					s_p++;
					n++;
				}
			}

			/* for zero or more data bytes... */
			for (; (*s_p != '\0') && (n < size); )
			{
				bool matched;

				{
					ssize_t rv;

					if ((rv = utlStrIncrMatch(s_p, size - n, collection_end_p + 1, ignore_case, largest_match)) > 0)
						return n + rv;

					if (rv == utlTOO_SHORT)
						return utlTOO_SHORT;
				}

				/* note where range should start (in case one follows) */
				range_first = *pattern_p;

				/* check if collection matches `*s_p'... */
				matched = false;
				for (c_p = pattern_p; c_p < collection_end_p; c_p++)
				{
					/* found a range? */
					if (*c_p == '-')
					{
						/* if `-' is not the first or last member of the collection */
						if ((c_p > pattern_p) && (c_p < (collection_end_p - 1)))
						{
							char a, b, c;

							c_p++; /* skip range character */

							/* compare `*s_p' to range */
							if (ignore_case == true) a = tolower(*s_p), b = tolower(*c_p), c = tolower(range_first);
							else a =         *s_p, b =         *c_p, c =         range_first;

							if ((a >= c) && (a <= b))
							{
								matched = true;
								break;
							}

							range_first = *s_p;
						}
						else
						{
							/* compare `*s_p' to `-' */
							if (*s_p == *c_p)
							{
								matched = true;
								break;
							}
						}
					}
					/* else does `c_p' point to a possible collection class? */
					else if (*c_p == '[')
					{
						int match;
						match = 0;

						if (strncmp(c_p, "[:alpha:]",   9) == 0) match = utlIsAlpha(*s_p),  c_p += 7;
						else if (strncmp(c_p, "[:upper:]",   9) == 0) match = utlIsUpper(*s_p),  c_p += 7;
						else if (strncmp(c_p, "[:lower:]",   9) == 0) match = utlIsLower(*s_p),  c_p += 7;
						else if (strncmp(c_p, "[:digit:]",   9) == 0) match = utlIsDigit(*s_p),  c_p += 7;
						else if (strncmp(c_p, "[:xdigit:]", 10) == 0) match = utlIsXDigit(*s_p), c_p += 8;
						else if (strncmp(c_p, "[:fdigit:]", 10) == 0) match = utlIsDigit(*s_p) ||
												      (*s_p == '-')   ||
												      (*s_p == '+')   ||
												      (*s_p == '.')   ||
												      (*s_p == 'e')   ||
												      (*s_p == 'E'),   c_p += 8;
						else if (strncmp(c_p, "[:alnum:]",   9) == 0) match = utlIsAlNum(*s_p), c_p += 7;
						else if (strncmp(c_p, "[:space:]",   9) == 0) match = utlIsSpace(*s_p), c_p += 7;
						else if (strncmp(c_p, "[:blank:]",   9) == 0) match = (*s_p == ' ')   ||
												      (*s_p == '\t'),  c_p += 7;
						else if (strncmp(c_p, "[:newln:]",   9) == 0) match = (*s_p == '\f')  ||
												      (*s_p == '\n')  ||
												      (*s_p == '\r'),  c_p += 7;
						else if (strncmp(c_p, "[:print:]",   9) == 0) match = utlIsPrint(*s_p), c_p += 7;
						else if (strncmp(c_p, "[:punct:]",   9) == 0) match = utlIsPunct(*s_p), c_p += 7;
						else if (strncmp(c_p, "[:graph:]",   9) == 0) match = utlIsGraph(*s_p), c_p += 7;
						else if (strncmp(c_p, "[:cntrl:]",   9) == 0) match = utlIsCntrl(*s_p), c_p += 7;
						else
						{
							/*--- compare `*s_p' to `['---*/
							if (*s_p == *c_p)
								match = 1;
						}

						if (match)
						{
							matched = true;
							break;
						}
					}
					else if (*c_p == '\\')
					{
						c_p++; /* skip escape character */

						/*--- compare `*s_p' to `*c_p' ---*/
						if (*s_p == *c_p)
						{
							matched = true;
							break;
						}
					}
					else
					{
						/*--- compare `*s_p' to `*c_p' ---*/
						if (*s_p == *c_p)
						{
							matched = true;
							break;
						}
					}
				}

				/*--- if nothing matched ---*/
				if (((non_match == false) && (matched == false)) ||
				    ((non_match ==  true) && (matched ==  true)))
				{
					/*--- if miss-match was at end ---*/
					if (*(collection_end_p + 1) == '\0')
						return n;

					return utlFAILED;
				}

				s_p++;
				n++;
			}

			/*--- if we've reached the end of `pattern_p' ---*/
			if (*(collection_end_p + 1) == '\0')
				return n;

			/*--- if we've reached the end of `s_p' ---*/
			if ((*s_p == '\0') || (n >= size))
				return utlTOO_SHORT;

			pattern_p = collection_end_p;
			break;

		case '\\':              /* an escaped special character */
			pattern_p++;    /* skip escape character */

			{
				char a, b;

				if (ignore_case == true) a = utlToLower(*pattern_p), b = utlToLower(*s_p);
				else a =           *pattern_p, b =           *s_p;

				/*--- found miss-match? ---*/
				if (a != b)
					return utlFAILED;
			}

			s_p++;
			n++;
			break;

		default: /* an ordinary character */
		{
			char a, b;

			if (ignore_case == true) a = utlToLower(*pattern_p), b = utlToLower(*s_p);
			else a =           *pattern_p, b =           *s_p;

			/*--- found miss-match? ---*/
			if (a != b)
				return utlFAILED;
		}

			s_p++;
			n++;
			break;
		}

		/*--- are there no more characters left in `s_p'? ---*/
		if ((*s_p == '\0') || (n >= size))
		{
			pattern_p++; /* peek at next character in pattern */

			/*--- if we've reached the end of the pattern, or if what's left matches zero characters ---*/
			while (*pattern_p != '\0')
			{
				if (*pattern_p == '*') pattern_p += 1;
				else if (*pattern_p == '#') pattern_p += 1;
				else if (*pattern_p == '{')
				{
					pattern_p++;

					/*--- find end of collection ---*/
					for (collection_end_p = pattern_p;; )
					{

						/*--- unexpected end-of-pattern? (due to missing close-bracket) ---*/
						if (*collection_end_p == '\0')
							return utlFAILED;

						else if (*collection_end_p == '}')
						{
							/*--- if '}' does not appear right after collection's open-brace, we're done ---*/
							if (collection_end_p != pattern_p)
								break;

							collection_end_p++; /* skip close-brace */

							/*--- is next character escaped? ---*/
							if (*collection_end_p == '\\')
								collection_end_p++;  /* skip escaped character */

							/*--- else does `collection_end_p' point to a possible collection class? ---*/
						}
						else if (*collection_end_p == '[')
						{
							if (strncmp(collection_end_p, "[:alpha:]",   9) == 0) collection_end_p +=  9;
							else if (strncmp(collection_end_p, "[:upper:]",   9) == 0) collection_end_p +=  9;
							else if (strncmp(collection_end_p, "[:lower:]",   9) == 0) collection_end_p +=  9;
							else if (strncmp(collection_end_p, "[:digit:]",   9) == 0) collection_end_p +=  9;
							else if (strncmp(collection_end_p, "[:xdigit:]", 10) == 0) collection_end_p += 10;
							else if (strncmp(collection_end_p, "[:fdigit:]", 10) == 0) collection_end_p += 10;
							else if (strncmp(collection_end_p, "[:alnum:]",   9) == 0) collection_end_p +=  9;
							else if (strncmp(collection_end_p, "[:space:]",   9) == 0) collection_end_p +=  9;
							else if (strncmp(collection_end_p, "[:blank:]",   9) == 0) collection_end_p +=  9;
							else if (strncmp(collection_end_p, "[:newln:]",   9) == 0) collection_end_p +=  9;
							else if (strncmp(collection_end_p, "[:print:]",   9) == 0) collection_end_p +=  9;
							else if (strncmp(collection_end_p, "[:punct:]",   9) == 0) collection_end_p +=  9;
							else if (strncmp(collection_end_p, "[:graph:]",   9) == 0) collection_end_p +=  9;
							else if (strncmp(collection_end_p, "[:cntrl:]",   9) == 0) collection_end_p +=  9;
							else
							{
								collection_end_p++; /* skip open-bracket */

								/*--- is next character escaped? ---*/
								if (*collection_end_p == '\\')
									collection_end_p++;  /* skip escaped character */
							}

						}
						else
						{
							collection_end_p++; /* skip character */

							/*--- is next character escaped? ---*/
							if (*collection_end_p == '\\')
								collection_end_p++;  /* skip escaped character */
						}
					}
					pattern_p = collection_end_p + 1;
				}
				else
					break;
			}

			if (*pattern_p == '\0')
				return n;

			return utlTOO_SHORT;
		}
	}

	return n;
}
/*---------------------------------------------------------------------------*
* FUNCTION
*	utlStrMatch(s_p, pattern_p, ignore_case)
* INPUT
*              s_p == the string being checked
*        pattern_p == the pattern to match
*      ignore_case == ignore character case
* OUTPUT
*      none
* RETURNS
*	true for a match, else false.
* DESCRIPTION
*	Determines if `pattern_p' matches `s_p'.
*---------------------------------------------------------------------------*/
bool utlStrMatch(const char *s_p,
		 const char *pattern_p,
		 const bool ignore_case)
{
	UNUSEDPARAM(ignore_case)

	utlAssert(s_p       != NULL);
	utlAssert(pattern_p != NULL);

	if ((unsigned int)utlStrIncrMatch(s_p, strlen(s_p), pattern_p, false, true) == strlen(s_p))
		return true;

	return false;
}

#ifdef utlTEST
/*---------------------------------------------------------------------------*
* FUNCTION
*	strMatchTest()
* INPUT
*	none
* OUTPUT
*	none
* RETURNS
*	"true" for pass, "false" for failure
*---------------------------------------------------------------------------*/
bool strMatchTest(void)
{
	/* a single character */
	if (utlStrIncrMatch("a", 1, "a", false, false) != 1)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(1) failed\n");
		return false;
	}
	if (utlStrIncrMatch("b", 1, "a", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(2) failed\n");
		return false;
	}

	/* multiple characters */
	if (utlStrIncrMatch("abc", 3, "abc", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(3) failed\n");
		return false;
	}
	if (utlStrIncrMatch("abd", 3, "abc", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(4) failed\n");
		return false;
	}

	/* single character wild card */
	if (utlStrIncrMatch("abc", 3, "a?c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(5) failed\n");
		return false;
	}
	if (utlStrIncrMatch("adc", 3, "a?c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(6) failed\n");
		return false;
	}
	if (utlStrIncrMatch("aec", 3, "a?c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(7) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a\nc", 3, "a?c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(8) failed\n");
		return false;
	}

	/* multiple character wild card */
	if (utlStrIncrMatch("ac", 2, "a*c", false, false) != 2)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(9) failed\n");
		return false;
	}
	if (utlStrIncrMatch("abc", 3, "a*c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(10) failed\n");
		return false;
	}
	if (utlStrIncrMatch("abbc", 4, "a*c", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(11) failed\n");
		return false;
	}
	if (utlStrIncrMatch("abddacbc", 8, "a*c", false, false) != 6)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(12a) failed\n");
		return false;
	}
	if (utlStrIncrMatch("abddacbc", 8, "a*c", false, true) != 8)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(12b) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ab\ndc", 5, "a*c", false, false) != 5)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(13) failed\n");
		return false;
	}

	/* escaped special characters */
	if (utlStrIncrMatch("abc", 3, "a\\bc", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(14) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a*c", 3, "a\\*c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(15) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a?c", 3, "a\\?c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(16) failed\n");
		return false;
	}

	/* a simple matching collection */
	if (utlStrIncrMatch("aec", 3, "a[efghi]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(17) failed\n");
		return false;
	}
	if (utlStrIncrMatch("afc", 3, "a[efghi]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(18) failed\n");
		return false;
	}
	if (utlStrIncrMatch("agc", 3, "a[efghi]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(19) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ahc", 3, "a[efghi]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(20) failed\n");
		return false;
	}
	if (utlStrIncrMatch("aic", 3, "a[efghi]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(21) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ajc", 3, "a[efghi]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(22) failed\n");
		return false;
	}

	/* matching collection with range */
	if (utlStrIncrMatch("aec", 3, "a[e-i]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(23) failed\n");
		return false;
	}
	if (utlStrIncrMatch("afc", 3, "a[e-i]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(24) failed\n");
		return false;
	}
	if (utlStrIncrMatch("agc", 3, "a[e-i]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(25) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ahc", 3, "a[e-i]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(26) failed\n");
		return false;
	}
	if (utlStrIncrMatch("aic", 3, "a[e-i]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(27) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ajc", 3, "a[e-i]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(28) failed\n");
		return false;
	}

	/* matching collection with complex range */
	if (utlStrIncrMatch("aec", 3, "a[e-g-i]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(29) failed\n");
		return false;
	}
	if (utlStrIncrMatch("afc", 3, "a[e-g-i]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(30) failed\n");
		return false;
	}
	if (utlStrIncrMatch("agc", 3, "a[e-g-i]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(31) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ahc", 3, "a[e-g-i]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(32) failed\n");
		return false;
	}
	if (utlStrIncrMatch("aic", 3, "a[e-g-i]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(33) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ajc", 3, "a[e-g-i]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(34) failed\n");
		return false;
	}

	/* matching collection with two ranges */
	if (utlStrIncrMatch("aec", 3, "a[e-gg-i]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(35) failed\n");
		return false;
	}
	if (utlStrIncrMatch("afc", 3, "a[e-gg-i]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(36) failed\n");
		return false;
	}
	if (utlStrIncrMatch("agc", 3, "a[e-gg-i]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(37) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ahc", 3, "a[e-gg-i]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(38) failed\n");
		return false;
	}
	if (utlStrIncrMatch("aic", 3, "a[e-gg-i]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(39) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ajc", 3, "a[e-gg-i]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(40) failed\n");
		return false;
	}

	/* non-matching simple collection */
	if (utlStrIncrMatch("aec", 3, "a[!efghi]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(41) failed\n");
		return false;
	}
	if (utlStrIncrMatch("afc", 3, "a[!efghi]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(42) failed\n");
		return false;
	}
	if (utlStrIncrMatch("agc", 3, "a[!efghi]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(43) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ahc", 3, "a[!efghi]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(44) failed\n");
		return false;
	}
	if (utlStrIncrMatch("aic", 3, "a[!efghi]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(45) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ajc", 3, "a[!efghi]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(46) failed\n");
		return false;
	}

	/* non-matching collection with range */
	if (utlStrIncrMatch("aec", 3, "a[!e-i]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(47) failed\n");
		return false;
	}
	if (utlStrIncrMatch("afc", 3, "a[!e-i]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(48) failed\n");
		return false;
	}
	if (utlStrIncrMatch("agc", 3, "a[!e-i]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(49) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ahc", 3, "a[!e-i]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(50) failed\n");
		return false;
	}
	if (utlStrIncrMatch("aic", 3, "a[!e-i]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(51) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ajc", 3, "a[!e-i]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(52) failed\n");
		return false;
	}

	/* non-matching collection with complex range */
	if (utlStrIncrMatch("aec", 3, "a[!e-g-i]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(53) failed\n");
		return false;
	}
	if (utlStrIncrMatch("afc", 3, "a[!e-g-i]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(54) failed\n");
		return false;
	}
	if (utlStrIncrMatch("agc", 3, "a[!e-g-i]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(55) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ahc", 3, "a[!e-g-i]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(56) failed\n");
		return false;
	}
	if (utlStrIncrMatch("aic", 3, "a[!e-g-i]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(57) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ajc", 3, "a[!e-g-i]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(58) failed\n");
		return false;
	}

	/* non-matching collection with two ranges */
	if (utlStrIncrMatch("aec", 3, "a[!e-gg-i]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(59) failed\n");
		return false;
	}
	if (utlStrIncrMatch("afc", 3, "a[!e-gg-i]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(60) failed\n");
		return false;
	}
	if (utlStrIncrMatch("agc", 3, "a[!e-gg-i]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(61) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ahc", 3, "a[!e-gg-i]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(62) failed\n");
		return false;
	}
	if (utlStrIncrMatch("aic", 3, "a[!e-gg-i]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(63) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ajc", 3, "a[!e-gg-i]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(64) failed\n");
		return false;
	}

	/* matching collection with a character class */
	if (utlStrIncrMatch("a4c", 3, "a[[:digit:]]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(65) failed\n");
		return false;
	}
	if (utlStrIncrMatch("abc", 3, "a[[:digit:]]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(66) failed\n");
		return false;
	}

	/* matching collection with an ordinary character and character class */
	if (utlStrIncrMatch("abc", 3, "a[b[:digit:]]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(67) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a4c", 3, "a[b[:digit:]]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(68) failed\n");
		return false;
	}
	if (utlStrIncrMatch("aec", 3, "a[b[:digit:]]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(69) failed\n");
		return false;
	}

	/* matching collection with two character classes */
	if (utlStrIncrMatch("abc", 3, "a[b[:digit:][:space:]]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(70) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a4c", 3, "a[b[:digit:][:space:]]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(71) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a c", 3, "a[b[:digit:][:space:]]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(72) failed\n");
		return false;
	}
	if (utlStrIncrMatch("aec", 3, "a[b[:digit:][:space:]]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(73) failed\n");
		return false;
	}

	/* non-matching collection with character class */
	if (utlStrIncrMatch("a4c", 3, "a[![:digit:]]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(74) failed\n");
		return false;
	}
	if (utlStrIncrMatch("abc", 3, "a[![:digit:]]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(75) failed\n");
		return false;
	}

	/* non-matching collection with ordinary character and character class */
	if (utlStrIncrMatch("abc", 3, "a[!b[:digit:]]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(76) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a4c", 3, "a[!b[:digit:]]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(77) failed\n");
		return false;
	}
	if (utlStrIncrMatch("aec", 3, "a[!b[:digit:]]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(78) failed\n");
		return false;
	}

	/* non-matching collection with two character classes */
	if (utlStrIncrMatch("abc", 3, "a[!b[:digit:][:space:]]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(79) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a4c", 3, "a[!b[:digit:][:space:]]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(80) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a c", 3, "a[!b[:digit:][:space:]]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(81) failed\n");
		return false;
	}
	if (utlStrIncrMatch("aec", 3, "a[!b[:digit:][:space:]]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(82) failed\n");
		return false;
	}

	/* non-matching collection with character class */
	if (utlStrIncrMatch("a]c", 3, "a[]]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(83) failed\n");
		return false;
	}
	if (utlStrIncrMatch("abc", 3, "a[]]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(84) failed\n");
		return false;
	}

	/* matching collection with closing bracket as first member */
	if (utlStrIncrMatch("a]c", 3, "a[!]]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(85) failed\n");
		return false;
	}
	if (utlStrIncrMatch("abc", 3, "a[!]]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(86) failed\n");
		return false;
	}

	/* matching collection with hyphen as last member */
	if (utlStrIncrMatch("abc", 3, "a[b-]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(87) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a-c", 3, "a[b-]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(88) failed\n");
		return false;
	}
	if (utlStrIncrMatch("acc", 3, "a[b-]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(89) failed\n");
		return false;
	}

	/* matching collection with hyphen as first member */
	if (utlStrIncrMatch("abc", 3, "a[-b]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(90) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a-c", 3, "a[-b]c", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(91) failed\n");
		return false;
	}
	if (utlStrIncrMatch("acc", 3, "a[-b]c", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(92) failed\n");
		return false;
	}

	/* wild card at start of pattern */
	if (utlStrIncrMatch("cde", 3, "*cde", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(93) failed\n");
		return false;
	}
	if (utlStrIncrMatch("bcde", 4, "*cde", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(94) failed\n");
		return false;
	}
	if (utlStrIncrMatch("abcde", 5, "*cde", false, false) != 5)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(95) failed\n");
		return false;
	}

	/* wild card at end of pattern */
	if (utlStrIncrMatch("abc", 3, "abc*", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(96) failed\n");
		return false;
	}
	if (utlStrIncrMatch("abcd", 4, "abc*", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(97) failed\n");
		return false;
	}
	if (utlStrIncrMatch("abcde", 5, "abc*", false, false) != 5)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(98) failed\n");
		return false;
	}

	/* a random pattern */
	if (utlStrIncrMatch("abd1f", 5, "a[bc][de][[:digit:]]f*", false, false) != 5)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(99) failed\n");
		return false;
	}
	if (utlStrIncrMatch("acd2f", 5, "a[bc][de][[:digit:]]f*", false, false) != 5)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(100) failed\n");
		return false;
	}
	if (utlStrIncrMatch("abe3f", 5, "a[bc][de][[:digit:]]f*", false, false) != 5)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(101) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ace4f", 5, "a[bc][de][[:digit:]]f*", false, false) != 5)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(102) failed\n");
		return false;
	}
	if (utlStrIncrMatch("abe5fxx", 7, "a[bc][de][[:digit:]]f*", false, false) != 7)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(103) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ade6fxx", 7, "a[bc][de][[:digit:]]f*", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(104) failed\n");
		return false;
	}
	if (utlStrIncrMatch("abeufxx", 7, "a[bc][de][[:digit:]]f*", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(105) failed\n");
		return false;
	}

	/* extra trailing characters */
	if (utlStrIncrMatch("4", 1, "[[:digit:]]", false, false) != 1)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(106) failed\n");
		return false;
	}
	if (utlStrIncrMatch("4a", 2, "[[:digit:]]", false, false) != 1)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(107) failed\n");
		return false;
	}

	/* literal hyphen characters in a collection */
	if (utlStrIncrMatch("-", 1, "[$\\-9]", false, false) != 1)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(108) failed\n");
		return false;
	}
	if (utlStrIncrMatch("-", 1, "[-$9]", false, false) != 1)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(109) failed\n");
		return false;
	}
	if (utlStrIncrMatch("-", 1, "[$9-]", false, false) != 1)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(110) failed\n");
		return false;
	}

	/* white space */
	if (utlStrIncrMatch("ab", 2, "a{[:space:]}b", false, false) != 2)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(111) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a b", 3, "a{[:space:]}b", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(112) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a  b", 4, "a{[:space:]}b", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(113) failed\n");
		return false;
	}

	if (utlStrIncrMatch("ab", 2, "{[:space:]}ab", false, false) != 2)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(114) failed\n");
		return false;
	}
	if (utlStrIncrMatch(" ab", 3, "{[:space:]}ab", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(115) failed\n");
		return false;
	}
	if (utlStrIncrMatch("  ab", 4, "{[:space:]}ab", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(116) failed\n");
		return false;
	}

	if (utlStrIncrMatch("ab", 2, "ab{[:space:]}", false, false) != 2)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(114b) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ab ", 3, "ab{[:space:]}", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(115b) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ab  ", 4, "ab{[:space:]}", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(116b) failed\n");
		return false;
	}

	if (utlStrIncrMatch("a  b", 0, "a{[:space:]}b", false, false) != utlTOO_SHORT)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(117) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a  b", 1, "a{[:space:]}b", false, false) != utlTOO_SHORT)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(118) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a  b", 2, "a{[:space:]}b", false, false) != utlTOO_SHORT)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(119) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a  b", 3, "a{[:space:]}b", false, false) != utlTOO_SHORT)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(120) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a  b", 4, "a{[:space:]}b", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(121) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a  b", 5, "a{[:space:]}b", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(122) failed\n");
		return false;
	}

	if (utlStrIncrMatch("ab  ", 0, "ab{[:space:]}", false, false) != utlTOO_SHORT)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(123) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ab  ", 1, "ab{[:space:]}", false, false) != utlTOO_SHORT)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(124) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ab  ", 2, "ab{[:space:]}", false, false) != 2)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(125) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ab  ", 3, "ab{[:space:]}", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(126) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ab  ", 4, "ab{[:space:]}", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(127) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ab  ", 5, "ab{[:space:]}", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(128) failed\n");
		return false;
	}

	/* space or tab */
	if (utlStrIncrMatch("ab", 2, "a{[:blank:]}b", false, false) != 2)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(129) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a b", 3, "a{[:blank:]}b", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(130) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a  b", 4, "a{[:blank:]}b", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(131) failed\n");
		return false;
	}

	if (utlStrIncrMatch("ab", 2, "{[:blank:]}ab", false, false) != 2)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(132) failed\n");
		return false;
	}
	if (utlStrIncrMatch(" ab", 3, "{[:blank:]}ab", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(133) failed\n");
		return false;
	}
	if (utlStrIncrMatch("  ab", 4, "{[:blank:]}ab", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(134) failed\n");
		return false;
	}

	if (utlStrIncrMatch("ab", 2, "ab{[:blank:]}", false, false) != 2)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(135) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ab ", 3, "ab{[:blank:]}", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(136) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ab  ", 4, "ab{[:blank:]}", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(137) failed\n");
		return false;
	}

	if (utlStrIncrMatch("a  b", 0, "a{[:blank:]}b", false, false) != utlTOO_SHORT)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(138) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a  b", 1, "a{[:blank:]}b", false, false) != utlTOO_SHORT)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(139) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a  b", 2, "a{[:blank:]}b", false, false) != utlTOO_SHORT)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(140) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a  b", 3, "a{[:blank:]}b", false, false) != utlTOO_SHORT)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(141) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a  b", 4, "a{[:blank:]}b", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(142) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a  b", 5, "a{[:blank:]}b", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(143) failed\n");
		return false;
	}

	if (utlStrIncrMatch("ab  ", 0, "ab{[:blank:]}", false, false) != utlTOO_SHORT)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(144) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ab  ", 1, "ab{[:blank:]}", false, false) != utlTOO_SHORT)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(145) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ab  ", 2, "ab{[:blank:]}", false, false) != 2)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(146) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ab  ", 3, "ab{[:blank:]}", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(147) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ab  ", 4, "ab{[:blank:]}", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(148) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ab  ", 5, "ab{[:blank:]}", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(149) failed\n");
		return false;
	}

	/* digits */
	if (utlStrIncrMatch("ab", 2, "a#b", false, false) != 2)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(150) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a1b", 3, "a#b", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(151) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a12b", 4, "a#b", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(152) failed\n");
		return false;
	}

	if (utlStrIncrMatch("ab", 2, "#ab", false, false) != 2)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(153) failed\n");
		return false;
	}
	if (utlStrIncrMatch("1ab", 3, "#ab", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(154) failed\n");
		return false;
	}
	if (utlStrIncrMatch("12ab", 4, "#ab", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(155) failed\n");
		return false;
	}

	if (utlStrIncrMatch("ab", 2, "ab#", false, false) != 2)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(156) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ab1", 3, "ab#", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(157) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ab12", 4, "ab#", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(158) failed\n");
		return false;
	}

	if (utlStrIncrMatch("a12b", 0, "a#b", false, false) != utlTOO_SHORT)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(159) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a12b", 1, "a#b", false, false) != utlTOO_SHORT)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(160) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a12b", 2, "a#b", false, false) != utlTOO_SHORT)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(161) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a12b", 3, "a#b", false, false) != utlTOO_SHORT)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(162) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a12b", 4, "a#b", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(163) failed\n");
		return false;
	}
	if (utlStrIncrMatch("a12b", 5, "a#b", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(163) failed\n");
		return false;
	}

	if (utlStrIncrMatch("ab12", 0, "ab#", false, false) != utlTOO_SHORT)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(164) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ab12", 1, "ab#", false, false) != utlTOO_SHORT)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(165) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ab12", 2, "ab#", false, false) != 2)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(166) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ab12", 3, "ab#", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(167) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ab12", 4, "ab#", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(168) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ab12", 5, "ab#", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(169) failed\n");
		return false;
	}

	/* test repeat matches */
	if (utlStrIncrMatch("a", 6, "a{z}b", false, false) != utlTOO_SHORT)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(170) failed\n");
		return false;
	}
	if (utlStrIncrMatch("ab", 6, "a{z}b", false, false) != 2)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(170) failed\n");
		return false;
	}
	if (utlStrIncrMatch("azb", 6, "a{z}b", false, false) != 3)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(171) failed\n");
		return false;
	}
	if (utlStrIncrMatch("azzb", 6, "a{z}b", false, false) != 4)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(172) failed\n");
		return false;
	}
	if (utlStrIncrMatch("azzzb", 6, "a{z}b", false, false) != 5)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(173) failed\n");
		return false;
	}
	if (utlStrIncrMatch("azzzzb", 6, "a{z}b", false, false) != 6)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(174) failed\n");
		return false;
	}

	if (utlStrIncrMatch("aboo", 6, "ab{z}", false, false) != 2)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(175) failed\n");
		return false;
	}
	if (utlStrIncrMatch("abzzzz", 6, "ab{z}", false, false) != 6)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(176) failed\n");
		return false;
	}

	if (utlStrIncrMatch("ab", 6, "a{!o}b", false, false) != 2)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(177) failed\n");
		return false;
	}
	if (utlStrIncrMatch("azzzzb", 6, "a{!o}b", false, false) != 6)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(178) failed\n");
		return false;
	}

	if (utlStrIncrMatch("abo", 6, "ab{!o}", false, false) != 2)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(179) failed\n");
		return false;
	}
	if (utlStrIncrMatch("abzzzz", 6, "ab{!o}", false, false) != 6)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(180) failed\n");
		return false;
	}

	if (utlStrIncrMatch("abzzzz\n", 8, "ab$", false, false) != 7)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(181) failed\n");
		return false;
	}
	if (utlStrIncrMatch("abzzzz\r", 8, "ab$", false, false) != 7)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(182) failed\n");
		return false;
	}
	if (utlStrIncrMatch("abzzzz\n\r", 9, "ab$", false, false) != 8)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(183) failed\n");
		return false;
	}
	if (utlStrIncrMatch("abzzzz\r\n", 9, "ab$", false, false) != 8)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(184) failed\n");
		return false;
	}

	if (utlStrIncrMatch("abzzzz\r\nhello", 14, "ab$hello", false, false) != 13)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(185) failed\n");
		return false;
	}
	if (utlStrIncrMatch("abzzzz\r\nhello", 14, "ab$$hello", false, false) != 13)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(186) failed\n");
		return false;
	}
	if (utlStrIncrMatch("abzzzz\n\nhello", 14, "ab$$hello", false, false) != 13)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(187) failed\n");
		return false;
	}
	if (utlStrIncrMatch("abzzzz\n\n\nhello", 15, "ab$$hello", false, false) != utlFAILED)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrIncrMatch(188) failed\n");
		return false;
	}


	/*--- test utlStrMatch() -------------------------------------------------*/

	if (utlStrMatch("hello", "hello", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(1) failed\n");
		return false;
	}

	if (utlStrMatch("hello", "goodby", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(2) failed\n");
		return false;
	}

	if (utlStrMatch("hello", "h*", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(3) failed\n");
		return false;
	}

	if (utlStrMatch("hello", "h*llo", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(4) failed\n");
		return false;
	}

	if (utlStrMatch("hello", "h*o", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(5) failed\n");
		return false;
	}

	if (utlStrMatch("hello", "h*O", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(6) failed\n");
		return false;
	}

	/* a single character */
	if (utlStrMatch("a", "a", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: strMatch(1) failed\n");
		return false;
	}
	if (utlStrMatch("b", "a", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: strMatch(2) failed\n");
		return false;
	}

	/* multiple characters */
	if (utlStrMatch("abc", "abc", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(3) failed\n");
		return false;
	}
	if (utlStrMatch("abd", "abc", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(4) failed\n");
		return false;
	}

	/* single character wild card */
	if (utlStrMatch("abc", "a?c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(5) failed\n");
		return false;
	}
	if (utlStrMatch("adc", "a?c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(6) failed\n");
		return false;
	}
	if (utlStrMatch("aec", "a?c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(7) failed\n");
		return false;
	}
	if (utlStrMatch("a\nc", "a?c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(8) failed\n");
		return false;
	}

	/* multiple character wild card */
	if (utlStrMatch("ac", "a*c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(9) failed\n");
		return false;
	}
	if (utlStrMatch("abc", "a*c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(10) failed\n");
		return false;
	}
	if (utlStrMatch("abbc", "a*c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(11) failed\n");
		return false;
	}
	if (utlStrMatch("abddacbc", "a*c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(12) failed\n");
		return false;
	}
	if (utlStrMatch("ab\ndc", "a*c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(13) failed\n");
		return false;
	}

	/* escaped special characters */
	if (utlStrMatch("abc", "a\\bc", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(14) failed\n");
		return false;
	}
	if (utlStrMatch("a*c", "a\\*c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(15) failed\n");
		return false;
	}
	if (utlStrMatch("a?c", "a\\?c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(16) failed\n");
		return false;
	}

	/* a simple matching collection */
	if (utlStrMatch("aec", "a[efghi]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(17) failed\n");
		return false;
	}
	if (utlStrMatch("afc", "a[efghi]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(18) failed\n");
		return false;
	}
	if (utlStrMatch("agc", "a[efghi]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(19) failed\n");
		return false;
	}
	if (utlStrMatch("ahc", "a[efghi]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(20) failed\n");
		return false;
	}
	if (utlStrMatch("aic", "a[efghi]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(21) failed\n");
		return false;
	}
	if (utlStrMatch("ajc", "a[efghi]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(22) failed\n");
		return false;
	}

	/* matching collection with range */
	if (utlStrMatch("aec", "a[e-i]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(23) failed\n");
		return false;
	}
	if (utlStrMatch("afc", "a[e-i]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(24) failed\n");
		return false;
	}
	if (utlStrMatch("agc", "a[e-i]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(25) failed\n");
		return false;
	}
	if (utlStrMatch("ahc", "a[e-i]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(26) failed\n");
		return false;
	}
	if (utlStrMatch("aic", "a[e-i]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(27) failed\n");
		return false;
	}
	if (utlStrMatch("ajc", "a[e-i]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(28) failed\n");
		return false;
	}

	/* matching collection with complex range */
	if (utlStrMatch("aec", "a[e-g-i]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(29) failed\n");
		return false;
	}
	if (utlStrMatch("afc", "a[e-g-i]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(30) failed\n");
		return false;
	}
	if (utlStrMatch("agc", "a[e-g-i]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(31) failed\n");
		return false;
	}
	if (utlStrMatch("ahc", "a[e-g-i]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(32) failed\n");
		return false;
	}
	if (utlStrMatch("aic", "a[e-g-i]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(33) failed\n");
		return false;
	}
	if (utlStrMatch("ajc", "a[e-g-i]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(34) failed\n");
		return false;
	}

	/* matching collection with two ranges */
	if (utlStrMatch("aec", "a[e-gg-i]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(35) failed\n");
		return false;
	}
	if (utlStrMatch("afc", "a[e-gg-i]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(36) failed\n");
		return false;
	}
	if (utlStrMatch("agc", "a[e-gg-i]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(37) failed\n");
		return false;
	}
	if (utlStrMatch("ahc", "a[e-gg-i]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(38) failed\n");
		return false;
	}
	if (utlStrMatch("aic", "a[e-gg-i]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(39) failed\n");
		return false;
	}
	if (utlStrMatch("ajc", "a[e-gg-i]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(40) failed\n");
		return false;
	}

	/* non-matching simple collection */
	if (utlStrMatch("aec", "a[!efghi]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(41) failed\n");
		return false;
	}
	if (utlStrMatch("afc", "a[!efghi]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(42) failed\n");
		return false;
	}
	if (utlStrMatch("agc", "a[!efghi]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(43) failed\n");
		return false;
	}
	if (utlStrMatch("ahc", "a[!efghi]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(44) failed\n");
		return false;
	}
	if (utlStrMatch("aic", "a[!efghi]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(45) failed\n");
		return false;
	}
	if (utlStrMatch("ajc", "a[!efghi]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(46) failed\n");
		return false;
	}

	/* non-matching collection with range */
	if (utlStrMatch("aec", "a[!e-i]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(47) failed\n");
		return false;
	}
	if (utlStrMatch("afc", "a[!e-i]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(48) failed\n");
		return false;
	}
	if (utlStrMatch("agc", "a[!e-i]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(49) failed\n");
		return false;
	}
	if (utlStrMatch("ahc", "a[!e-i]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(50) failed\n");
		return false;
	}
	if (utlStrMatch("aic", "a[!e-i]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(51) failed\n");
		return false;
	}
	if (utlStrMatch("ajc", "a[!e-i]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(52) failed\n");
		return false;
	}

	/* non-matching collection with complex range */
	if (utlStrMatch("aec", "a[!e-g-i]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(53) failed\n");
		return false;
	}
	if (utlStrMatch("afc", "a[!e-g-i]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(54) failed\n");
		return false;
	}
	if (utlStrMatch("agc", "a[!e-g-i]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(55) failed\n");
		return false;
	}
	if (utlStrMatch("ahc", "a[!e-g-i]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(56) failed\n");
		return false;
	}
	if (utlStrMatch("aic", "a[!e-g-i]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(57) failed\n");
		return false;
	}
	if (utlStrMatch("ajc", "a[!e-g-i]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(58) failed\n");
		return false;
	}

	/* non-matching collection with two ranges */
	if (utlStrMatch("aec", "a[!e-gg-i]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(59) failed\n");
		return false;
	}
	if (utlStrMatch("afc", "a[!e-gg-i]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(60) failed\n");
		return false;
	}
	if (utlStrMatch("agc", "a[!e-gg-i]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(61) failed\n");
		return false;
	}
	if (utlStrMatch("ahc", "a[!e-gg-i]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(62) failed\n");
		return false;
	}
	if (utlStrMatch("aic", "a[!e-gg-i]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(63) failed\n");
		return false;
	}
	if (utlStrMatch("ajc", "a[!e-gg-i]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(64) failed\n");
		return false;
	}

	/* matching collection with a character class */
	if (utlStrMatch("a4c", "a[[:digit:]]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(65) failed\n");
		return false;
	}
	if (utlStrMatch("abc", "a[[:digit:]]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(66) failed\n");
		return false;
	}

	/* matching collection with an ordinary character and character class */
	if (utlStrMatch("abc", "a[b[:digit:]]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(67) failed\n");
		return false;
	}
	if (utlStrMatch("a4c", "a[b[:digit:]]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(68) failed\n");
		return false;
	}
	if (utlStrMatch("aec", "a[b[:digit:]]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(69) failed\n");
		return false;
	}

	/* matching collection with two character classes */
	if (utlStrMatch("abc", "a[b[:digit:][:space:]]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(70) failed\n");
		return false;
	}
	if (utlStrMatch("a4c", "a[b[:digit:][:space:]]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(71) failed\n");
		return false;
	}
	if (utlStrMatch("a c", "a[b[:digit:][:space:]]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(72) failed\n");
		return false;
	}
	if (utlStrMatch("aec", "a[b[:digit:][:space:]]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(73) failed\n");
		return false;
	}

	/* non-matching collection with character class */
	if (utlStrMatch("a4c", "a[![:digit:]]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(74) failed\n");
		return false;
	}
	if (utlStrMatch("abc", "a[![:digit:]]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(75) failed\n");
		return false;
	}

	/* non-matching collection with ordinary character and character class */
	if (utlStrMatch("abc", "a[!b[:digit:]]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(76) failed\n");
		return false;
	}
	if (utlStrMatch("a4c", "a[!b[:digit:]]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(77) failed\n");
		return false;
	}
	if (utlStrMatch("aec", "a[!b[:digit:]]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(78) failed\n");
		return false;
	}

	/* non-matching collection with two character classes */
	if (utlStrMatch("abc", "a[!b[:digit:][:space:]]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(79) failed\n");
		return false;
	}
	if (utlStrMatch("a4c", "a[!b[:digit:][:space:]]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(80) failed\n");
		return false;
	}
	if (utlStrMatch("a c", "a[!b[:digit:][:space:]]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(81) failed\n");
		return false;
	}
	if (utlStrMatch("aec", "a[!b[:digit:][:space:]]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(82) failed\n");
		return false;
	}

	/* non-matching collection with character class */
	if (utlStrMatch("a]c", "a[]]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(83) failed\n");
		return false;
	}
	if (utlStrMatch("abc", "a[]]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(84) failed\n");
		return false;
	}

	/* matching collection with closing bracket as first member */
	if (utlStrMatch("a]c", "a[!]]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(85) failed\n");
		return false;
	}
	if (utlStrMatch("abc", "a[!]]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(86) failed\n");
		return false;
	}

	/* matching collection with hyphen as last member */
	if (utlStrMatch("abc", "a[b-]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(87) failed\n");
		return false;
	}
	if (utlStrMatch("a-c", "a[b-]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(88) failed\n");
		return false;
	}
	if (utlStrMatch("acc", "a[b-]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(89) failed\n");
		return false;
	}

	/* matching collection with hyphen as first member */
	if (utlStrMatch("abc", "a[-b]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(90) failed\n");
		return false;
	}
	if (utlStrMatch("a-c", "a[-b]c", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(91) failed\n");
		return false;
	}
	if (utlStrMatch("acc", "a[-b]c", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(92) failed\n");
		return false;
	}

	/* wild card at start of pattern */
	if (utlStrMatch("cde", "*cde", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(93) failed\n");
		return false;
	}
	if (utlStrMatch("bcde", "*cde", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(94) failed\n");
		return false;
	}
	if (utlStrMatch("abcde", "*cde", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(95) failed\n");
		return false;
	}

	/* wild card at end of pattern */
	if (utlStrMatch("abc", "abc*", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(96) failed\n");
		return false;
	}
	if (utlStrMatch("abcd", "abc*", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(97) failed\n");
		return false;
	}
	if (utlStrMatch("abcde", "abc*", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(98) failed\n");
		return false;
	}

	/* a random pattern */
	if (utlStrMatch("abd1f", "a[bc][de][[:digit:]]f*", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(99) failed\n");
		return false;
	}
	if (utlStrMatch("acd2f", "a[bc][de][[:digit:]]f*", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(100) failed\n");
		return false;
	}
	if (utlStrMatch("abe3f", "a[bc][de][[:digit:]]f*", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(101) failed\n");
		return false;
	}
	if (utlStrMatch("ace4f", "a[bc][de][[:digit:]]f*", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(102) failed\n");
		return false;
	}
	if (utlStrMatch("abe5fxx", "a[bc][de][[:digit:]]f*", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(103) failed\n");
		return false;
	}
	if (utlStrMatch("ade6fxx", "a[bc][de][[:digit:]]f*", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(104) failed\n");
		return false;
	}
	if (utlStrMatch("abeufxx", "a[bc][de][[:digit:]]f*", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(105) failed\n");
		return false;
	}

	/* extra trailing characters */
	if (utlStrMatch("4", "[[:digit:]]", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(106) failed\n");
		return false;
	}
	if (utlStrMatch("4a", "[[:digit:]]", false) != false)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(107) failed\n");
		return false;
	}

	/* literal hyphen characters in a collection */
	if (utlStrMatch("-", "[$\\-9]", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(108) failed\n");
		return false;
	}
	if (utlStrMatch("-", "[-$9]", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(109) failed\n");
		return false;
	}
	if (utlStrMatch("-", "[$9-]", false) != true)
	{
		(void)fprintf(stderr, "strMatchTest: utlStrMatch(110) failed\n");
		return false;
	}

	return true;
}
#endif /* utlTEST */

