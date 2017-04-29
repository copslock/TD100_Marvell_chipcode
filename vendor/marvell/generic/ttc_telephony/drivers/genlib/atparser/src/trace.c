/*****************************************************************************
* Utility Library
*
* DESCRIPTION
*      Tracing utility functions.  Allows diagnostic operations to be conditionally
*      executed.
*
* EXAMPLE USAGE
*
*      #define MY_TRACE_TAG "myTag"
*
*      {
*      int  trace_peg;
*      char *message[10];
*
*          if (utlSetProcessName("my process name") != utlSUCCESS)
*              <handle error here>
*
*
*          --- specify what to trace as a glob pattern ---
*          if (utlAddTraceCriteria("*") != utlSUCCESS)
*              <handle error here>
*
*
*          trace_peg = 0;
*
*          utlTrace(MY_TRACE_TAG,
*                   trace_peg++;
*                  );
*
*          utlTrace(MY_TRACE_TAG,
*                   utlPrintTrace("my simple trace message\n");
*                  );
*
*          <initialize message here>
*
*          utlTrace(MY_TRACE_TAG,
*                   int i;
*
*                   utlPrintTrace("more complex trace message:\n");
*                   for (i = 0; i < utlNumberOf(message); i++)
*                       utlPrintTrace("  message[%d]: %s\n", i, message[i]);
*                  );
*
*
*          if (utlDumpTracePegs(stderr, "*") != utlSUCCESS)
*              <handle error here>
*
*          if (utlDeleteTraceCriteria("*") != utlSUCCESS)
*              <handle error here>
*      }
*
*****************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <utlTypes.h>
#include <utlError.h>
#include <utlMalloc.h>
#include <utlMutex.h>
#include <utlSemaphore.h>
#include <utlStrMatch.h>

#include <utlTrace.h>


/*--- Configuration ---------------------------------------------------------*/

/*--- amount to increase size of trace criterion database by when too small ---*/
#define utlTRACE_CRITERION_SIZE_INC ((utlTraceCriterionSize_T)4)

/*--- amount to increase size of trace tags database by when too small ---*/
#define utlTRACE_SYMBOLS_SIZE_INC ((utlTraceTagsSize_T)32)

/*--- maximum length of a trace prefix string ---*/
#define utlTRACE_MAX_PREFIX_LEN ((size_t)100)


/*--- Data Types ------------------------------------------------------------*/

#if defined(utlDEBUG) ||  defined(utlTEST)
/*--- Trace criterion database ---*/
typedef struct utlTraceCriteria_S {
	char *pattern_p;
}             utlTraceCriteria_T, *utlTraceCriteria_P;
typedef const utlTraceCriteria_T  *utlTraceCriteria_P2c;

typedef size_t utlTraceCriterionSize_T;

typedef enum {
	utlTRACING_DISABLED = 0,
	utlTRACING_ENABLED  = 1
} utlTraceState_T, *utlTraceState_P;

typedef long utlTracePeg_T;

typedef struct utlTraceTag_S {
	const char      *name_p;
	utlTraceState_T state;
	utlTracePeg_T hits;
}             utlTraceTag_T, *utlTraceTag_P;
typedef const utlTraceTag_T  *utlTraceTag_P2c;

typedef size_t utlTraceTagsSize_T;


/*--- Local Data --------------------------------------------------------*/

static utlSemaphore_T trace_semaphore = utlNULL_SEMAPHORE;  /* protection for static trace data */
static char proc_name[20]   = "<unknown>";

/*--- Trace criterion database ---*/
static utlTraceCriteria_P trace_criterion_p     = NULL;
static utlTraceCriterionSize_T trace_criterion_count = 0;
static utlTraceCriterionSize_T trace_criterion_size  = 0;

static utlTraceTag_P trace_tags_p     = NULL;
static utlTraceTagsSize_T trace_tags_count = 0;
static utlTraceTagsSize_T trace_tags_size  = 0;
#endif /* defined(utlDEBUG) ||  defined(utlTEST) */

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlSetProcessName(name_p)
* INPUT
*      name_p == name of the process
* OUTPUT
*      none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Sets the process name to `name_p'.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlSetProcessName(const char *name_p)
{
#if defined(utlDEBUG) ||  defined(utlTEST)
	size_t n;

	utlAssert(name_p != NULL);

	/*--- semaphore initialization ---*/
	if (!utlIsSemaphoreInitialized(&trace_semaphore))
	{
		if (utlInitSemaphore(&trace_semaphore, utlSEMAPHORE_ATTR_SHARING_ENABLE |
				     utlSEMAPHORE_ATTR_NESTING_ENABLE) != utlSUCCESS)
			return utlFAILED;
	}

	/*--- acquire write-access ---*/
	if (utlAcquireExclusiveAccess(&trace_semaphore) != utlSUCCESS)
		return utlFAILED;

	n = utlNumberOf(proc_name) - 1;
	strncpy(proc_name, name_p, n);
	proc_name[n] = '\0';

	/*--- release write-access ---*/
	if (utlReleaseExclusiveAccess(&trace_semaphore) != utlSUCCESS)
		return utlFAILED;

#endif  /* defined(utlDEBUG) ||  defined(utlTEST) */

	return utlSUCCESS;
}
#if defined(utlDEBUG) ||  defined(utlTEST)

/*---------------------------------------------------------------------------*
* FUNCTION
*	evaluateTraceState(name_p)
* INPUT
*      name_p == name of the tag to evaluage
* OUTPUT
*      none
* RETURNS
*      utlTRACING_ENABLED or utlTRACING_DISABLED
* DESCRIPTION
*	Evaluates `name_p' to determine if it should be traced or not.
*---------------------------------------------------------------------------*/
static utlTraceState_T evaluateTraceState(const char *name_p)
{
	utlTraceCriteria_P criteria_p;
	utlTraceCriteria_P terminal_criteria_p;

	utlAssert(name_p != NULL);

	/*--- search for matching trace criteria ---*/
	terminal_criteria_p = trace_criterion_p + trace_criterion_count;
	for (criteria_p = trace_criterion_p; criteria_p != terminal_criteria_p; criteria_p++)
		if (utlStrMatch(name_p, criteria_p->pattern_p, false) == true)
			return utlTRACING_ENABLED;

	return utlTRACING_DISABLED;
}
#endif /* defined(utlDEBUG) ||  defined(utlTEST) */

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlAddTraceCriteria(pattern_p)
* INPUT
*      pattern_p == pattern that specifies a new trace criteria
* OUTPUT
*      none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Adds `pattern_p' to the trace criterion database, and then re-evaluates
*      all existing trace tags.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlAddTraceCriteria(const char *pattern_p)
{
#if defined(utlDEBUG) ||  defined(utlTEST)
	utlTraceCriteria_P criteria_p;

	utlAssert(pattern_p != NULL);

	/*--- semaphore initialization ---*/
	if (!utlIsSemaphoreInitialized(&trace_semaphore))
	{
		if (utlInitSemaphore(&trace_semaphore, utlSEMAPHORE_ATTR_SHARING_ENABLE |
				     utlSEMAPHORE_ATTR_NESTING_ENABLE) != utlSUCCESS)
			return utlFAILED;
	}

	/*--- acquire write-access ---*/
	if (utlAcquireExclusiveAccess(&trace_semaphore) != utlSUCCESS)
		return utlFAILED;

	/*--- existing trace criterion database too small? ---*/
	if (trace_criterion_count >= trace_criterion_size)
	{
		utlTraceCriteria_P new_trace_criterion_p;
		utlTraceCriterionSize_T new_trace_criterion_size;

		/*--- increase size of existing trace criterion database... ---*/
		new_trace_criterion_size = trace_tags_size + utlTRACE_CRITERION_SIZE_INC;
		if ((new_trace_criterion_p = utlRealloc(trace_criterion_p,
							sizeof(utlTraceCriteria_T) *
							new_trace_criterion_size)) == NULL)
		{
			/*--- clean ---*/
			(void)utlReleaseExclusiveAccess(&trace_semaphore);

			return utlFAILED;
		}

		trace_criterion_p    = new_trace_criterion_p;
		trace_criterion_size = new_trace_criterion_size;
	}

	/*--- add `pattern' to end of trace criterion database ---*/
	criteria_p = trace_criterion_p + trace_criterion_count;
	if ((criteria_p->pattern_p = utlMalloc((strlen(pattern_p) + 1) * sizeof(char))) == NULL)
	{
		/*--- clean ---*/
		(void)utlReleaseExclusiveAccess(&trace_semaphore);

		return utlFAILED;
	}
	(void)strcpy(criteria_p->pattern_p, pattern_p);
	trace_criterion_count++;

	/*--- re-evaluate the trace state of all existing trace tags ---*/
	{
		utlTraceTag_P tag_p;
		utlTraceTag_P terminal_tag_p;

		terminal_tag_p = trace_tags_p + trace_tags_count;
		for (tag_p = trace_tags_p; tag_p != terminal_tag_p; tag_p++)
			tag_p->state = evaluateTraceState(tag_p->name_p);
	}

	/*--- release write-access ---*/
	if (utlReleaseExclusiveAccess(&trace_semaphore) != utlSUCCESS)
		return utlFAILED;

#endif  /* defined(utlDEBUG) ||  defined(utlTEST) */

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlDeleteTraceCriteria(pattern_p)
* INPUT
*      pattern_p == pattern that specifies the trace criteria to delete
* OUTPUT
*      none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Delete `pattern_p' from the trace criterion database, and then
*      re-evaluates all existing trace tags.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlDeleteTraceCriteria(const char *pattern_p)
{
#if defined(utlDEBUG) ||  defined(utlTEST)
	utlTraceCriteria_P criteria_p;
	utlTraceCriteria_P prev_criteria_p;
	utlTraceCriteria_P terminal_criteria_p;

	utlAssert(pattern_p != NULL);

	/*--- if semaphore is not initialized ---*/
	if (!utlIsSemaphoreInitialized(&trace_semaphore))
	{
		utlError("Delete Trace failed, trace criteria `%s' not found");

		return utlFAILED;
	}

	/*--- acquire write-access ---*/
	if (utlAcquireExclusiveAccess(&trace_semaphore) != utlSUCCESS)
		return utlFAILED;

	/*--- search for existing trace criteria ---*/
	terminal_criteria_p = trace_criterion_p + trace_criterion_count;
	for (criteria_p = trace_criterion_p; criteria_p != terminal_criteria_p; criteria_p++)
		if (strcmp(pattern_p, criteria_p->pattern_p) == 0)
			break;

	/*--- not found? ---*/
	if (criteria_p == terminal_criteria_p)
	{
		utlError("Delete Trace failed, trace criteria `%s' not found");

		/*--- clean ---*/
		(void)utlReleaseExclusiveAccess(&trace_semaphore);

		return utlFAILED;
	}

	/*--- release deleted criteria's resources ---*/
	utlFree(criteria_p->pattern_p);

	/*--- close the gap ---*/
	for (prev_criteria_p = criteria_p++; criteria_p != terminal_criteria_p; prev_criteria_p = criteria_p++)
		*prev_criteria_p = *criteria_p;
	criteria_p->pattern_p = NULL;

	trace_criterion_count--;

	/*--- re-evaluate the trace state of all existing trace tags ---*/
	{
		utlTraceTag_P tag_p;
		utlTraceTag_P terminal_tag_p;

		terminal_tag_p = trace_tags_p + trace_tags_count;
		for (tag_p = trace_tags_p; tag_p != terminal_tag_p; tag_p++)
			tag_p->state = evaluateTraceState(tag_p->name_p);
	}

	/*--- release write-access ---*/
	if (utlReleaseExclusiveAccess(&trace_semaphore) != utlSUCCESS)
		return utlFAILED;

#endif  /* defined(utlDEBUG) ||  defined(utlTEST) */

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlDumpTracePegs(file_p, pattern_p)
* INPUT
*         file_p == pointer to an open file
*      pattern_p == which trace pegs to print
* OUTPUT
*      none
* RETURNS
*	The number of bytes successfully printed for success, utlFAILED for
*      failure.
* DESCRIPTION
*	Prints the trace pegs whose tag name matches `pattern_p' to `file_p'.
*---------------------------------------------------------------------------*/
int utlDumpTracePegs(      FILE *file_p,
			   const char *pattern_p)
{
#if defined(utlDEBUG) ||  defined(utlTEST)
	utlTraceTag_P tag_p;
	utlTraceTag_P terminal_tag_p;
	int bytes_printed;

	utlAssert(file_p    != NULL);
	utlAssert(pattern_p != NULL);

	bytes_printed = 0;

	/*--- acquire read-access ---*/
	if (utlAcquireSharedAccess(&trace_semaphore) != utlSUCCESS)
		return utlFAILED;

	if (trace_tags_p == NULL)
	{
		/*--- clean ---*/
		(void)utlReleaseSharedAccess(&trace_semaphore);

		return bytes_printed;
	}

	/*--- for each tag in the trace tags database... ---*/
	terminal_tag_p = trace_tags_p + trace_tags_count;
	for (tag_p = trace_tags_p; tag_p != terminal_tag_p; tag_p++)

		/*--- if this tag has been hit and if it matches `pattern'... ---*/
		if ((tag_p->hits > 0) &&
		    (utlStrMatch(tag_p->name_p, pattern_p, false) == true))
		{
			int rc;

			if ((rc = fprintf(file_p, "%s = %ld\n", tag_p->name_p,
					  tag_p->hits)) == -1)
			{
				/*--- clean ---*/
				(void)utlReleaseSharedAccess(&trace_semaphore);

				return utlFAILED;
			}
			bytes_printed += rc;
		}

	/*--- release read-access ---*/
	if (utlReleaseSharedAccess(&trace_semaphore) != utlSUCCESS)
		return utlFAILED;

	return bytes_printed;
#else
	return 0;
#endif  /* defined(utlDEBUG) ||  defined(utlTEST) */
}
#if defined(utlDEBUG) ||  defined(utlTEST)

/*---------------------------------------------------------------------------*
* FUNCTION
*	newTraceTag(name_p, no_realloc)
* INPUT
*           name_p == name of the new tag
*       no_realloc == avoid reallocs?
* OUTPUT
*      none
* RETURNS
*	a pointer to the new trace tag for success, NULL for failure
* DESCRIPTION
*	Adds the trace tag `name' to the trace tag database.
*---------------------------------------------------------------------------*/
static utlTraceTag_P newTraceTag(const char *name_p,
				 const bool no_realloc)
{
	utlTraceTag_P tag_p;
	utlTraceTag_P terminal_tag_p;

	utlAssert(name_p != NULL);

	/*--- is this trace tag already defined? ---*/
	if (trace_tags_p != NULL)
	{
		terminal_tag_p = trace_tags_p + trace_tags_count;
		for (tag_p = trace_tags_p; tag_p != terminal_tag_p; tag_p++)
			if (strcmp(name_p, tag_p->name_p) == 0)
				return tag_p;  /* do nothing */
	}

	/*--- existing tags database too small? ---*/
	if (trace_tags_count >= trace_tags_size)
	{
		utlTraceTag_P new_trace_tags_p;
		utlTraceTagsSize_T new_trace_tags_size;

		/*--- avoid realloc? ---*/
		if (no_realloc == true)
			return NULL;

		/*--- increase size of existing tags database... ---*/
		new_trace_tags_size = trace_tags_size + utlTRACE_SYMBOLS_SIZE_INC;
		if ((new_trace_tags_p = utlRealloc(trace_tags_p,
						   sizeof(utlTraceTag_T) *
						   new_trace_tags_size)) == NULL)
			return NULL;

		trace_tags_p    = new_trace_tags_p;
		trace_tags_size = new_trace_tags_size;
	}

	tag_p = trace_tags_p + trace_tags_count;

	/*--- add new trace tag ---*/
	tag_p->name_p = name_p;
	tag_p->state  = evaluateTraceState(name_p);
	tag_p->hits   = 0;

	trace_tags_count++;

	return tag_p;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	findTraceTag(name_p)
* INPUT
*      name_p == name of the trace tag to find
* OUTPUT
*      none
* RETURNS
*	a pointer to a tags database entry for success, NULL for failure
* DESCRIPTION
*	Searchs the trace tags database for the tag specified by `name'.
*---------------------------------------------------------------------------*/
static utlTraceTag_P findTraceTag(const char *name_p)
{
	utlTraceTag_P2c tag_p;
	utlTraceTag_P2c terminal_tag_p;

	utlAssert(name_p != NULL);

	if (trace_tags_p == NULL)
		return NULL;

	/*--- linear search for a matching trace tags entry ---*/
	terminal_tag_p = trace_tags_p + trace_tags_count;
	for (tag_p = trace_tags_p; tag_p != terminal_tag_p; tag_p++)

		/*--- found a matching trace tags entry? ---*/
		if (strcmp(name_p, tag_p->name_p) == 0)
			return (utlTraceTag_P)tag_p;

	/*--- specified trace tag not found ---*/
	return NULL;
}
#endif /* defined(utlDEBUG) ||  defined(utlTEST) */

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlFormatTracePrefix(buf_p, n, tag_p)
* INPUT
*      buf_p == pointer to a character buffer with room for `n' characters
*          n == size of buffer pointed to by `buf' (in characters)
*      tag_p == pointer to a filled utlTraceTag_P structure
* OUTPUT
*      none
* RETURNS
*	nothing
* DESCRIPTION
*	Composes a formatted prefix string for trace messages.
*
*      Generated string:
*
*         <proc name>: TRACE(<tag name>)
*
*---------------------------------------------------------------------------*/
static void utlFormatTracePrefix(      char          *buf_p,
				       size_t n
#if defined(utlDEBUG) ||  defined(utlTEST)
				       , const utlTraceTag_P tag_p
#endif
				       )
{
#if defined(utlDEBUG) ||  defined(utlTEST)
	size_t len;

	utlAssert(buf_p != NULL);
	utlAssert(n > (size_t)0);

	memset(buf_p, 0, n);

	/*--- insert process name (if there's room) ---*/
	if (n > (size_t)2)
	{
		(void)strncpy(buf_p, proc_name, n - 2);
		(void)strncat(buf_p, ":",       n - 1);
	}

	len = strlen(buf_p);

	/*--- insert keyword and trace tag (if there's room) ---*/
	if ((n - len) >= sizeof(" TRACE()"))
	{

		/*--- add keyword ---*/
		(void)strncpy(buf_p + len, " TRACE(", n - len - 1);

		len = strlen(buf_p);

		/*--- add trace-tag name (if specified) ---*/
		if ((tag_p         != NULL) &&
		    (tag_p->name_p != NULL)) (void)strncpy(buf_p + len, tag_p->name_p, n - len - 2);
		else (void)strncpy(buf_p + len, "",            n - len - 2);

		(void)strcat(buf_p, ")");
	}
#else
	(void)strncpy(buf_p, "", n);
#endif  /* defined(utlDEBUG) ||  defined(utlTEST) */
}

#if defined(utlDEBUG) ||  defined(utlTEST)

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlDoPrintTrace(tag_index, format_p, ...)
* INPUT
*      tag_index == a trace tag index
*       format_p == pointer to a trace-message format string
* OUTPUT
*      none
* RETURNS
*	nothing
* DESCRIPTION
*	Emits a trace message.
*---------------------------------------------------------------------------*/
void utlDoPrintTrace(      int tag_index,
			   const char *format_p,
			   ...)
{
#if defined(utlDEBUG) ||  defined(utlTEST)
	va_list va_arg_p;
	utlTraceTag_P tag_p;
	char trace_prefix[utlTRACE_MAX_PREFIX_LEN];

	utlAssert(format_p != NULL);

	/*--- if tag was previously found and looks valid... ---*/
	if ((tag_index >= 0) && (tag_index < trace_tags_count))
	{

		/*--- acquire read-access ---*/
		if (utlAcquireSharedAccess(&trace_semaphore) != utlSUCCESS)
			return;

		utlAssert( trace_tags_p != NULL);
		tag_p = trace_tags_p + tag_index;

		utlFormatTracePrefix(trace_prefix, utlTRACE_MAX_PREFIX_LEN, tag_p);

		va_start(va_arg_p, format_p);
		(void)fprintf(stderr, "%s ", trace_prefix);
		(void)vfprintf(stderr, format_p, va_arg_p);
		va_end(va_arg_p);

		/*--- release read-access ---*/
		(void)utlReleaseSharedAccess(&trace_semaphore);

	}
	else
	{
		va_start(va_arg_p, format_p);
		(void)vfprintf(stderr, format_p, va_arg_p);
		va_end(va_arg_p);
	}
#endif  /* defined(utlDEBUG) ||  defined(utlTEST) */
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlDoTraceOn(tag_index_p, name_p)
* INPUT
*       tag_index_p == pointer to a trace tag, or a NULL pointer
*            name_p == name of a trace tag
* OUTPUT
*      none
* RETURNS
*	"true" if `tag' should be traced, else "false" is returned.
* DESCRIPTION
*	Returns the trace state for the trace tag specified by `name_p'.
*      This function assumes that the allocated array pointed to by
*      `trace_tags_p' is only ever appended to.
*---------------------------------------------------------------------------*/
bool utlDoTraceOn(      int  *tag_index_p,
			const char *name_p)
{
	static bool new_tag_active = false;

	utlTraceTag_P tag_p;
	bool no_realloc;
	bool rv;

	utlAssert(tag_index_p != NULL);
	utlAssert(name_p      != NULL);

	/*--- if semaphore is uninitialized, tracing is off ---*/
	if (!utlIsSemaphoreInitialized(&trace_semaphore))
		return false;

	/*--- acquire read-access ---*/
	if (utlAcquireSharedAccess(&trace_semaphore) != utlSUCCESS)
		return false;

	/*--- if tag was previously found... ---*/
	if (*tag_index_p >= 0)
	{

		/*--- by definition tracing is turned off for corrupt tags ---*/
		if ((trace_tags_p == NULL) || (*tag_index_p >= trace_tags_count))
		{

			/*--- release read-access ---*/
			(void)utlReleaseSharedAccess(&trace_semaphore);

			return false;
		}

		tag_p =  trace_tags_p + *tag_index_p;
		tag_p->hits++;
		rv = tag_p->state == utlTRACING_ENABLED;

		/*--- release read-access ---*/
		if (utlReleaseSharedAccess(&trace_semaphore) != utlSUCCESS)
			return false;

		return rv;
	}

	/*--- search the trace database for `name' ---*/
	tag_p = findTraceTag(name_p);

	/*--- if `name_p' is already defined in the trace database ---*/
	if (tag_p != NULL)
	{

		/*--- arrange for the next access to this tag to be much faster ---*/
		*tag_index_p = tag_p - trace_tags_p;

		tag_p->hits++;
		rv = tag_p->state == utlTRACING_ENABLED;

		/*--- release read-access ---*/
		if (utlReleaseSharedAccess(&trace_semaphore) != utlSUCCESS)
			return false;

		return rv;
	}

	/*--- release read-access ---*/
	if (utlReleaseSharedAccess(&trace_semaphore) != utlSUCCESS)
		return false;

	/*--- acquire write-access ---*/
	if (utlAcquireExclusiveAccess(&trace_semaphore) != utlSUCCESS)
		return false;

	/*--- note if we've recursed ---*/
	if (new_tag_active == true) no_realloc = true;
	else no_realloc = false;

	/*--- `name_p' does not exist in the trace database, so add it ---*/
	new_tag_active = true;
	tag_p = newTraceTag(name_p, no_realloc);
	new_tag_active = false;

	/*--- if `name_p' is now defined in the trace database ---*/
	if (tag_p != NULL)
	{

		/*--- arrange for the next access to this tag to be faster ---*/
		*tag_index_p = tag_p - trace_tags_p;

		tag_p->hits++;
		rv = tag_p->state == utlTRACING_ENABLED;

		/*--- else if while inside newTraceTag() we tried to call utlRealloc()
		   while we were trying to trace a utlRealloc() trace message,
		   we'll determine if tracing is turned on but not add the result to
		   the trace database (since that would lead to infinite recursion). */
	}
	else if (no_realloc == true)
	{
		rv = evaluateTraceState(name_p) == utlTRACING_ENABLED;

	}
	else
		rv = false;

	/*--- release write-access ---*/
	if (utlReleaseExclusiveAccess(&trace_semaphore) != utlSUCCESS)
		return false;

	return rv;
}
#endif /* defined(utlDEBUG) ||  defined(utlTEST) */

#ifdef utlTEST
/*---------------------------------------------------------------------------*
* FUNCTION
*	traceTest()
* INPUT
*	none
* OUTPUT
*	none
* RETURNS
*	"true" for pass, "false" for failure
*---------------------------------------------------------------------------*/
bool traceTest(void)
{

	char saved_proc_name[20];
	utlTraceCriteria_P saved_trace_criterion_p;
	utlTraceCriterionSize_T saved_trace_criterion_count;
	utlTraceCriterionSize_T saved_trace_criterion_size;
	utlTraceTag_P saved_trace_tags_p;
	utlTraceTagsSize_T saved_trace_tags_count;
	utlTraceTagsSize_T saved_trace_tags_size;

	utlTraceTag_P one_p;
	utlTraceTag_P two_p;
	utlTraceTag_P three_p;
	utlTraceTag_P find_one_p;
	utlTraceTag_P find_two_p;
	utlTraceTag_P find_three_p;
	int trace_one_peg;
	int trace_two_peg;
	int trace_three_peg;
	char buf[100];
	utlTraceTag_T tag;

	/*--- save trace context ---*/
	strcpy(saved_proc_name, proc_name);
	saved_trace_criterion_p     = trace_criterion_p;
	saved_trace_criterion_count = trace_criterion_count;
	saved_trace_criterion_size  = trace_criterion_size;
	saved_trace_tags_p          = trace_tags_p;
	saved_trace_tags_count      = trace_tags_count;
	saved_trace_tags_size       = trace_tags_size;

	/*--- trace setup ---*/
	utlSetProcessName("myProcName");

	trace_criterion_p     = NULL;
	trace_criterion_count = 0;
	trace_criterion_size  = 0;
	trace_tags_p          = NULL;
	trace_tags_count      = 0;
	trace_tags_size       = 0;


	/*--- test newTraceTag() ------------------------------------------------*/

	if ((one_p = newTraceTag("one", false)) == NULL)
	{
		(void)fprintf(stderr, "traceTest: newTraceTag(1) failed\n");
		return false;
	}
	if ((strcmp(one_p->name_p, "one") != 0) ||
	    (one_p->state       != utlTRACING_DISABLED) ||
	    (one_p->hits        != 0) ||
	    (trace_tags_count != 1))
	{
		(void)fprintf(stderr, "traceTest: newTraceTag(2) failed\n");
		return false;
	}

	if ((two_p = newTraceTag("two", false)) == NULL)
	{
		(void)fprintf(stderr, "traceTest: newTraceTag(3) failed\n");
		return false;
	}
	if ((strcmp(two_p->name_p, "two") != 0) ||
	    (two_p->state       != utlTRACING_DISABLED) ||
	    (two_p->hits        != 0) ||
	    (trace_tags_count != 2))
	{
		(void)fprintf(stderr, "traceTest: newTraceTag(4) failed\n");
		return false;
	}

	(void)utlAddTraceCriteria("three");
	if ((three_p = newTraceTag("three", false)) == NULL)
	{
		(void)fprintf(stderr, "traceTest: newTraceTag(5) failed\n");
		return false;
	}
	if ((strcmp(three_p->name_p, "three") != 0) ||
	    (three_p->state     != utlTRACING_ENABLED) ||
	    (three_p->hits      != 0) ||
	    (trace_tags_count != 3))
	{
		(void)fprintf(stderr, "traceTest: newTraceTag(6) failed\n");
		return false;
	}


	/*-- test utlAddTraceCriteria() -----------------------------------------*/

	if (utlAddTraceCriteria("five") != utlSUCCESS)
	{
		(void)fprintf(stderr, "traceTest: utlAddTraceCriteria(1) failed\n");
		return false;
	}
	if (trace_criterion_count != 2)
	{
		(void)fprintf(stderr, "traceTest: utlAddTraceCriteria(2) failed\n");
		return false;
	}

	if (utlAddTraceCriteria("one") != utlSUCCESS)
	{
		(void)fprintf(stderr, "traceTest: utlAddTraceCriteria(3) failed\n");
		return false;
	}
	one_p = findTraceTag("one");
	if ((one_p->state            != utlTRACING_ENABLED) ||
	    (trace_criterion_count != 3))
	{
		(void)fprintf(stderr, "traceTest: utlAddTraceCriteria(4) failed\n");
		return false;
	}


	/*-- test utlDeleteTraceCriteria() --------------------------------------*/

	if (utlAddTraceCriteria("delfive") != utlSUCCESS)
	{
		(void)fprintf(stderr, "traceTest: utlDeleteTraceCriteria(1) failed\n");
		return false;
	}
	if (trace_criterion_count != 4)
	{
		(void)fprintf(stderr, "traceTest: utlDeleteTraceCriteria(2) failed\n");
		return false;
	}

	if (utlDeleteTraceCriteria("delfive") != utlSUCCESS)
	{
		(void)fprintf(stderr, "traceTest: utlDeleteTraceCriteria(3) failed\n");
		return false;
	}
	one_p = findTraceTag("one");
	if ((one_p->state          != utlTRACING_ENABLED) ||
	    (trace_criterion_count != 3))
	{
		(void)fprintf(stderr, "traceTest: utlDeleteTraceCriteria(4) failed\n");
		return false;
	}


	/*--- test findTraceTag() -----------------------------------------------*/

	if ((find_one_p = findTraceTag("one")) == NULL)
	{
		(void)fprintf(stderr, "traceTest: findTraceTag(1) failed\n");
		return false;
	}
	if (strcmp(find_one_p->name_p, "one") != 0)
	{
		(void)fprintf(stderr, "traceTest: findTraceTag(2) failed\n");
		return false;
	}

	if ((find_two_p = findTraceTag("two")) == NULL)
	{
		(void)fprintf(stderr, "traceTest: findTraceTag(3) failed\n");
		return false;
	}
	if (strcmp(find_two_p->name_p, "two") != 0)
	{
		(void)fprintf(stderr, "traceTest: findTraceTag(4) failed\n");
		return false;
	}

	if ((find_three_p = findTraceTag("three")) == NULL)
	{
		(void)fprintf(stderr, "traceTest: findTraceTag(5) failed\n");
		return false;
	}
	if (strcmp(find_three_p->name_p, "three") != 0)
	{
		(void)fprintf(stderr, "traceTest: findTraceTag(6) failed\n");
		return false;
	}


	/*--- test utlDoTraceOn() and utlTrace() --------------------------------*/

	/* setup for tests */
	trace_one_peg   = 0;
	trace_two_peg   = 0;
	trace_three_peg = 0;

	utlTrace("one", trace_one_peg++; );
	utlTrace("one", trace_one_peg++; );
	if (trace_one_peg != 2)
	{
		(void)fprintf(stderr, "traceTest: utlTrace(1) failed\n");
		return false;
	}

	utlTrace("two", trace_two_peg++; );
	utlTrace("two", trace_two_peg++; );
	if (trace_two_peg != 0)
	{
		(void)fprintf(stderr, "traceTest: utlTrace(2) failed\n");
		return false;
	}

	utlTrace("three", trace_three_peg++; );
	utlTrace("three", trace_three_peg++; );
	if (trace_three_peg != 2)
	{
		(void)fprintf(stderr, "traceTest: utlTrace(3) failed\n");
		return false;
	}


	/*--- test utlDumpTracePegs() -------------------------------------------*/

	if (utlDumpTracePegs(stderr, "*") < 0)
	{
		(void)fprintf(stderr, "traceTest: utlDumpTracePegs(3) failed\n");
		return false;
	}


	/*--- test utlFormatTracePrefix() ---------------------------------------*/

	tag.name_p = "myTag";
	tag.state  = utlTRACING_ENABLED;
	tag.hits   = 0;

	utlFormatTracePrefix(buf, utlNumberOf(buf), &tag);
	if ((strstr(buf, "TRACE") == NULL) ||
	    (strstr(buf, "myTag") == NULL))
	{
		(void)fprintf(stderr, "traceTest: utlFormatTracePrefix(1) failed\n");
		return false;
	}

#define FORMAT_TRACE_TEST(i, n, s) {   \
		utlFormatTracePrefix(buf, n, &tag); \
		if (strcmp(buf, s) != 0) {	     \
			(void)fprintf(stderr, "traceTest: utlFormatTracePrefix(%d) failed\n", i); \
			return false; \
		} \
}

	FORMAT_TRACE_TEST( 2,  1, "");
	FORMAT_TRACE_TEST( 3,  2, "");
	FORMAT_TRACE_TEST( 4,  3, "m:");
	FORMAT_TRACE_TEST( 5,  4, "my:");
	FORMAT_TRACE_TEST( 6,  5, "myP:");
	FORMAT_TRACE_TEST( 7,  6, "myPr:");
	FORMAT_TRACE_TEST( 8,  7, "myPro:");
	FORMAT_TRACE_TEST( 9,  8, "myProc:");
	FORMAT_TRACE_TEST(10,  9, "myProcN:");
	FORMAT_TRACE_TEST(11, 10, "myProcNa:");
	FORMAT_TRACE_TEST(12, 11, "myProcNam:");
	FORMAT_TRACE_TEST(13, 12, "myProcName:");
	FORMAT_TRACE_TEST(14, 13, "myProcName:");
	FORMAT_TRACE_TEST(15, 14, "myProcName:");
	FORMAT_TRACE_TEST(16, 15, "myProcName:");
	FORMAT_TRACE_TEST(17, 16, "myProcName:");
	FORMAT_TRACE_TEST(18, 17, "myProcName:");
	FORMAT_TRACE_TEST(19, 18, "myProcName:");
	FORMAT_TRACE_TEST(20, 19, "myProcName:");
	FORMAT_TRACE_TEST(21, 20, "myProcName: TRACE()");
	FORMAT_TRACE_TEST(22, 21, "myProcName: TRACE(m)");
	FORMAT_TRACE_TEST(23, 22, "myProcName: TRACE(my)");
	FORMAT_TRACE_TEST(24, 23, "myProcName: TRACE(myT)");
	FORMAT_TRACE_TEST(25, 24, "myProcName: TRACE(myTa)");
	FORMAT_TRACE_TEST(26, 25, "myProcName: TRACE(myTag)");
	FORMAT_TRACE_TEST(27, 26, "myProcName: TRACE(myTag)");


/*--- test utlDoPrintTrace() --------------------------------------------*/

	utlAddTraceCriteria("print_one");
	utlAddTraceCriteria("print_two");
	utlAddTraceCriteria("print_three");
	utlTrace("print_one",   utlPrintTrace("message one\n"); );
	utlTrace("print_two",   utlPrintTrace("message two\n"); );
	utlTrace("print_three", utlPrintTrace("message three\n"); );


/*--- cleanup -----------------------------------------------------------*/

/*--- cleanup from previous tests ---*/
	if (trace_criterion_p != NULL)
	{
		utlTraceCriteria_P criteria_p;
		utlTraceCriteria_P terminal_criteria_p;

		terminal_criteria_p = trace_criterion_p + trace_criterion_count;
		for (criteria_p = trace_criterion_p; criteria_p != terminal_criteria_p; criteria_p++)
			utlFree(criteria_p->pattern_p);

		utlFree(trace_criterion_p);
	}
	trace_criterion_p     = NULL;
	trace_criterion_count = 0;
	trace_criterion_size  = 0;

	if (trace_tags_p != NULL)
		utlFree(trace_tags_p);

	trace_tags_p     = NULL;
	trace_tags_count = 0;
	trace_tags_size  = 0;

	/*--- restore trace context ---*/
	trace_criterion_p     = saved_trace_criterion_p;
	trace_criterion_count = saved_trace_criterion_count;
	trace_criterion_size  = saved_trace_criterion_size;
	trace_tags_p          = saved_trace_tags_p;
	trace_tags_count      = saved_trace_tags_count;
	trace_tags_size       = saved_trace_tags_size;

	return true;
}
#endif /* utlTEST */

