/*****************************************************************************
* Utility Library
*
*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <utlTypes.h>
#include <utlError.h>
#include <utlEventHandler.h>
#include <utlTrace.h>
#include <utlTimer.h>

/*---------------------------------------------------------------------------*/
#ifdef utlTEST
void runUnitTests(void)
{
	printf("run unit tests\n");

	printf("       errorTest = %s\n", (       errorTest() == true) ? "passed" : "FAILED");
	printf("     convertTest = %s\n", (     convertTest() == true) ? "passed" : "FAILED");
	printf("      mallocTest = %s\n", (      mallocTest() == true) ? "passed" : "FAILED");
	printf("  linkedListTest = %s\n", (  linkedListTest() == true) ? "passed" : "FAILED");
	printf("     vStringTest = %s\n", (     vStringTest() == true) ? "passed" : "FAILED");
	printf("    strMatchTest = %s\n", (    strMatchTest() == true) ? "passed" : "FAILED");
	printf("       traceTest = %s\n", (       traceTest() == true) ? "passed" : "FAILED");
	printf("        timeTest = %s\n", (        timeTest() == true) ? "passed" : "FAILED");
	printf("stateMachineTest = %s\n", (stateMachineTest() == true) ? "passed" : "FAILED");
	printf("       timerTest = %s\n", (       timerTest() == true) ? "passed" : "FAILED");
	printf("eventHandlerTest = %s\n", (eventHandlerTest() == true) ? "passed" : "FAILED");
	printf("       mutexTest = %s\n", (       mutexTest() == true) ? "passed" : "FAILED");
	printf("   semaphoreTest = %s\n", (   semaphoreTest() == true) ? "passed" : "FAILED");
	printf("    atParserTest = %s\n", (    atParserTest() == true) ? "passed" : "FAILED");
}
#endif /* utlTEST */
/*---------------------------------------------------------------------------*/
utlReturnCode_T myTimeoutHandler(const utlTimerId_T id,
				 const unsigned long timeout_count,
				 void                *arg_p,
				 const utlAbsoluteTime_P2c curr_time_p)
{
	char buf[40];

	utlFormatDateTime(buf, sizeof(buf), curr_time_p);
	fprintf(stderr, "timeout at %s\n", buf);
	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*/
utlReturnCode_T myFdHandler(const utlEventHandlerType_T handler_type,
			    const utlEventHandlerType_T event_type,
			    const int fd,
			    const utlRelativeTime_P2c period_p,
			    void                  *arg_p)
{
	char buf[40];
	size_t n;

	if (handler_type != utlEVENT_HANDLER_TYPE_READ)
		return utlFAILED;
	if (event_type != utlEVENT_HANDLER_TYPE_READ)
		return utlFAILED;

	/*--- read input ---*/
	if ((n = read(fd, buf, sizeof(buf))) < 0)
	{
		utlError("Call to %s() failed, %s", "read", strerror(errno));
		return utlFAILED;
	}

	/*--- write output ---*/
	{
		char    *buf_p;
		ssize_t rv;

		buf_p = buf;
		for (; n > (size_t)0; buf_p += rv, n -= rv)
			if ((rv = write(1, buf_p, n)) < (ssize_t)0)
			{
				utlError("Call to %s() failed, %s", "write", strerror(errno));
				return utlFAILED;
			}
	}

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	/*--- parse command parameters ---*/
	{
	}

	/*--- initialize ---*/
	{
		utlSetProcessName(argv[0]);
		utlInitTimers();
	}

	/*--- tracing ---*/
	{
		/* utlAddTraceCriteria("*"); */
		/* utlAddTraceCriteria("AT parsers"); */
		/* utlAddTraceCriteria("dispatcher"); */
		/* utlAddTraceCriteria("events"); */
		/* utlAddTraceCriteria("heap"); */
		/* utlAddTraceCriteria("queues"); */
		/* utlAddTraceCriteria("state machines"); */
		/* utlAddTraceCriteria("timers"); */
	}

#ifdef utlTEST
	/*--- Testing ---*/
	{
		runUnitTests();
		return utlSUCCESS;
	}
#endif

	/*--- Example: start a periodic timer ---*/
	{
		utlRelativeTime_T period;
		utlTimerId_T timer_id;

		period.seconds     = 1;
		period.nanoseconds = 0;

		if ((timer_id = utlStartTimer(&period, utlTIMER_INFINITY, &myTimeoutHandler, NULL)) == utlFAILED)
		{
			utlError("Call to %s() failed", "utlStartTimer");
			return utlFAILED;
		}
	}

	/*--- Example: fd read handler for <stdin> ---*/
	{
		utlEventHandlerId_T handler_id;

		if ((handler_id = utlSetFdEventHandler(utlEVENT_HANDLER_TYPE_READ,
						       utlEVENT_HANDLER_PRIORITY_MEDIUM,
						       0, myFdHandler, NULL)) == utlFAILED)
		{
			utlError("Call to %s() failed", "utlSetFdEventHandler");
			return utlFAILED;
		}
	}

	/*--- main event loop ---*/
	if (utlEventLoop(true) != utlSUCCESS)
	{
		utlError("Event loop reports failure");
		return utlFAILED;
	}

	utlError("Process %s exiting.", argv[0]);
	return utlSUCCESS;
}

