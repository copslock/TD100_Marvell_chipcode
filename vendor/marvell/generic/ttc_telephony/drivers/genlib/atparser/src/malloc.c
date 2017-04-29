/*****************************************************************************
* Utility Library
*
* DESCRIPTION
*	Memory allocation wrapper functions.  These wrapper functions provide
*      the following benefits:
*
*         - Report failures using utlError so you don't need to every time you
*           call malloc/realloc/calloc.
*
*         - Allow reported errors to include information about from where in
*           your source code the calls to malloc/realloc/calloc/free were made.
*           This is useful when trying to track down memory leaks.
*
*         - Arranges for free'ed pointers to be assigned the value "NULL", to
*           better detect when an application attempts to access free'ed memory.
*
*         - Makes it easier to later add test code to check for write-boundary
*           violations, memory leaks, and so forth  (Future feature).
*
* EXAMPLE USAGE
*
*      --- malloc/realloc ---
*      {
*      char *c_p;
*
*          if ((c_p = utlMalloc(100 * sizeof(char))) == NULL)
*              <handle error here>
*
*          <use `c_p' here>
*
*          {
*          char *new_c_p;
*
*              if ((new_c_p = utlRealloc(c_p, 200 * sizeof(char))) == NULL)
*                  <handle error here>
*
*              c_p = new_c_p;
*          }
*
*          <use resize'ed `c_p' here>
*
*          utlFree(c_p);
*      }
*
*      --- malloc/realloc ---
*      {
*      char *c_p;
*
*          if ((c_p = utlCalloc(100, sizeof(char))) == NULL)
*              <handle error here>
*
*          <use `c_p' here>
*
*          utlFree(c_p);
*      }
*
*      Note: Use "utlFreeConst()" if you're trying to free a pointer that has
*            been declared as a "const".  "utlFreeConst()" does not attempt
*            to NULL the just-free'ed pointer.
*
*      Note: According to ANSI-C, if realloc() returns NULL, the block of
*            memory originally passed to realloc() remains allocated and
*            must eventually be free'ed to avoid a memory leak.  Loosing
*            track of this original block is a very common programming
*            error.
*
*****************************************************************************/

#include <stdio.h>

#include <utlTypes.h>
#include <utlError.h>
#include <utlMalloc.h>
#include <utlTrace.h>


/*--- Configuration ---------------------------------------------------------*/

/*--- for tracing heap allocs and frees ---*/
#define utlTRACE_HEAP "heap"


/*---------------------------------------------------------------------------*
* FUNCTION
*	utlDoFree(utlHERE_DECLARATION, buf_p)
* INPUT
*      utlHERE_DECLARATION == source of free request
*                    buf_p == the buffer to free
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Free's one block of allocated memory.
*---------------------------------------------------------------------------*/
void utlDoFree(utlHERE_DECLARATION,
	       void *buf_p)
{
	utlTrace(utlTRACE_HEAP,
		 utlPrintTrace("%s/%s/line=%d, Free 0x%x\n",
			       utlHERE_FILE,
			       utlHERE_FUNC,
			       utlHERE_LINE,
			       (unsigned int)buf_p);
		 );

	free(buf_p);
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlDoMalloc(utlHERE_DECLARATION, siz)
* INPUT
*      utlHERE_DECLARATION == source of malloc request
*                      siz == number of bytes to allocate
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Allocates one block of memory.
*---------------------------------------------------------------------------*/
void *utlDoMalloc(utlHERE_DECLARATION,
		  const size_t siz)
{
	void *buf_p;

	if ((buf_p = malloc(siz)) == NULL)
	{
		utlError("Call to malloc(%d) failed\n", siz);
		return NULL;
	}

	utlTrace(utlTRACE_HEAP,
		 utlPrintTrace("%s/%s/line=%d, Malloc(%d bytes) 0x%x\n",
			       utlHERE_FILE,
			       utlHERE_FUNC,
			       utlHERE_LINE,
			       siz,
			       (unsigned int)buf_p);
		 );

	return buf_p;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlDoRealloc(utlHERE_DECLARATION, buf_p, siz)
* INPUT
*      utlHERE_DECLARATION == source of realloc request
*                    buf_p == pointer to the allocated block to be resized, or NULL
*                      siz == number of bytes to allocate
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Resizes an allocated block of memory.
*---------------------------------------------------------------------------*/
void *utlDoRealloc(utlHERE_DECLARATION,
		   void   *buf_p,
		   const size_t siz)
{
	void *new_buf_p;

	if ((new_buf_p = realloc(buf_p, siz)) == NULL)
	{
		utlError("Call to realloc(0x%08x, %d) failed\n", (unsigned int)buf_p, siz);
		return NULL;
	}

	utlTrace(utlTRACE_HEAP,
		 utlPrintTrace("%s/%s/line=%d, Relloc(%d bytes) from 0x%x to 0x%x\n",
			       utlHERE_FILE,
			       utlHERE_FUNC,
			       utlHERE_LINE,
			       siz,
			       (unsigned int)buf_p,
			       (unsigned int)new_buf_p);
		 );

	return new_buf_p;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlDoCalloc(utlHERE_DECLARATION, number_of, siz)
* INPUT
*      utlHERE_DECLARATION == source of calloc request
*                number_of == number of blocks to allocate
*                      siz == number of bytes in each block
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Allocates and initializes multiple same-sized contigious blocks of
*      memory.
*---------------------------------------------------------------------------*/
void *utlDoCalloc(utlHERE_DECLARATION,
		  const size_t number_of,
		  const size_t siz)
{
	void *buf_p;

	if ((buf_p = calloc(number_of, siz)) == NULL)
	{
		utlError("Call to calloc(%d, %d) failed\n", number_of, siz);
		return NULL;
	}

	utlTrace(utlTRACE_HEAP,
		 utlPrintTrace("%s/%s/line=%d, Calloc(%d %d-byte blocks) 0x%x\n",
			       utlHERE_FILE,
			       utlHERE_FUNC,
			       utlHERE_LINE,
			       number_of,
			       siz,
			       (unsigned int)buf_p);
		 );

	return buf_p;
}

#ifdef utlTEST
/*---------------------------------------------------------------------------*
* FUNCTION
*	mallocTest()
* INPUT
*	none
* OUTPUT
*	none
* RETURNS
*	"true" for pass, "false" for failure
*---------------------------------------------------------------------------*/
bool mallocTest(void)
{
	void *buf_p;

	/*--- utlMalloc() -------------------------------------------------------*/

	if ((buf_p = utlMalloc(10)) == NULL)
	{
		(void)fprintf(stderr, "memoryTest: utlMalloc() failed\n");
		return false;
	}
	utlFree(buf_p);
	if (buf_p != NULL)
	{
		(void)fprintf(stderr, "memoryTest: utlFree() failed\n");
		return false;
	}

	/*--- utlRealloc() ------------------------------------------------------*/

	if ((buf_p = utlMalloc(10)) == NULL)
	{
		(void)fprintf(stderr, "memoryTest: utlMalloc() failed\n");
		return false;
	}
	if ((buf_p = utlRealloc(buf_p, 20)) == NULL)
	{
		(void)fprintf(stderr, "memoryTest: utlRealloc() failed\n");
		return false;
	}
	utlFree(buf_p);
	if (buf_p != NULL)
	{
		(void)fprintf(stderr, "memoryTest: utlFree() failed\n");
		return false;
	}

	/*--- utlCalloc() -------------------------------------------------------*/

	if ((buf_p = utlCalloc(10, 5)) == NULL)
	{
		(void)fprintf(stderr, "memoryTest: utlCalloc() failed\n");
		return false;
	}
	utlFree(buf_p);
	if (buf_p != NULL)
	{
		(void)fprintf(stderr, "memoryTest: utlFree() failed\n");
		return false;
	}

	return true;
}
#endif /* utlTEST */

