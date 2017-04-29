/*****************************************************************************
* Utility Library
*
* DESCRIPTION
*      Time utility functions.  These wrapper functions provide the
*      following benefits:
*
*         - Report failures using utlError() so you don't need to every time
*           you call gettimeofday() and strftime().
*
*         - Provides a standardized string format for dates and times.
*
*         - Wraps the system call "gettimeofday()" such that if it is interrupted
*           due to receipt of a signal, that it is propertly restarted.
*
* EXAMPLE USAGE
*
*        {
*        utlAbsoluteTime_T time;
*        char              buf[50];
*
*            if (utlCurrentTime(&time) != utlSUCCESS)
*                <handle error here>
*
*            if (utlFormatTime(buf, utlNumberOf(buf), &time) != utlSUCCESS)
*                <handle error here>
*
*            if (utlFormatDate(buf, utlNumberOf(buf), &time) != utlSUCCESS)
*                <handle error here>
*
*            if (utlFormatDateTime(buf, utlNumberOf(buf), &time) != utlSUCCESS)
*                <handle error here>
*        }
*
*****************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <utlTypes.h>
#include <utlError.h>
#include <utlTime.h>

#define UNUSEDPARAM(param) (void)param;
/*---------------------------------------------------------------------------*
* FUNCTION
*	utlCurrentTime(time_p)
* INPUT
*      time_p == pointer to an empty utlAbsoluteTime_T structure
* OUTPUT
*      none
* RETURNS
*      utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Fetches the current system time since the system epoch.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlCurrentTime(const utlAbsoluteTime_P time_p)
{

	utlAssert(time_p != NULL);

#ifdef USE_POSIX_API
	{
		struct timespec curr_time;

		/*--- fetch current time (since epoch) ---*/
		for (;; )
		{
			if (clock_gettime(CLOCK_REALTIME, &curr_time) == 0)
				break;

			if (errno != EINTR)
			{
				utlError("Call to %s() failed, errno=%d", "clock_gettime", errno);
				return utlFAILED;
			}
		}

		time_p->seconds     = curr_time.tv_sec;
		time_p->nanoseconds = curr_time.tv_nsec;
	}
#else
	{
		struct timeval curr_time;

		/*--- fetch current time (since epoch) ---*/
		for (;; )
		{
			if (gettimeofday(&curr_time, NULL) == 0)
				break;

			if (errno != EINTR)
			{
				utlError("Call to %s() failed, errno=%d", "gettimeofday", errno);
				return utlFAILED;
			}
		}

		time_p->seconds     = curr_time.tv_sec;
		time_p->nanoseconds = curr_time.tv_usec * 1000;
	}
#endif

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlFormatTime(buf_p, n, time_p)
* INPUT
*       buf_p == pointer to an empty character buffer
*           n == number of characters that `buf_p' can hold
*      time_p == pointer to an empty utlAbsoluteTime_T structure
* OUTPUT
*      buf_p == the formatted time
* RETURNS
*      utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Generates a Formated time string.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlFormatTime(      char                *buf_p,
				    const size_t n,
				    const utlAbsoluteTime_P2c time_p)
{
	time_t seconds;
	size_t length;

	seconds = time_p->seconds;

	/*--- convert seconds-since-epoch into a tm structure, then a time string ---*/
	if (strftime(buf_p, n, "%H:%M:%S", gmtime(&seconds)) <= 0)
	{
		utlError("Call to %s() failed", "strftime");
		return utlFAILED;
	}

	length =  strlen(buf_p);

	/*--- enough room for fractional seconds? ---*/
	if (length < n)
	{
		char frac[20];

		(void)sprintf(frac, ".%09d UTC", time_p->nanoseconds);

		strncat(buf_p, frac, n - length);

		length += strlen(frac);

		if (length >= n)
			*(buf_p + n - 1) = '\0';
	}

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlFormatDate(buf_p, n, time_p)
* INPUT
*       buf_p == pointer to an empty character buffer
*           n == number of characters that `buf_p' can hold
*      time_p == pointer to an empty utlAbsoluteTime_T structure
* OUTPUT
*      buf_p == the formatted time
* RETURNS
*      utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Generates a Formated date string.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlFormatDate(      char                *buf_p,
				    const size_t n,
				    const utlAbsoluteTime_P2c time_p)
{
	time_t seconds;
	size_t length;

	seconds = time_p->seconds;

	/*--- convert seconds-since-epoch into a tm structure, then a date string ---*/
	if (strftime(buf_p, n, "%G-%m-%d", gmtime(&seconds)) <= 0)
	{
		utlError("Call to %s() failed", "strftime");
		return utlFAILED;
	}

	length =  strlen(buf_p);

	/*--- enough room for timezone info? ---*/
	if (length < n)
	{
		strncat(buf_p, " UTC", n - length);

		length += strlen(" UTC");

		if (length >= n)
			*(buf_p + n - 1) = '\0';
	}

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlFormatDateTime(buf_p, n, time_p)
* INPUT
*       buf_p == pointer to an empty character buffer
*           n == number of characters that `buf_p' can hold
*      time_p == pointer to an empty utlAbsoluteTime_T structure
* OUTPUT
*      buf_p == the formatted time
* RETURNS
*      utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Generates a Formated date/time string.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlFormatDateTime(      char                *buf_p,
					const size_t n,
					const utlAbsoluteTime_P2c time_p)
{
	time_t seconds;
	size_t length;

	seconds = time_p->seconds;

	/*--- convert seconds-since-epoch into a tm structure, then a time string ---*/
	if (strftime(buf_p, n, "%G-%m-%d %H:%M:%S", gmtime(&seconds)) <= 0)
	{
		utlError("Call to %s() failed", "strftime");
		return utlFAILED;
	}

	length =  strlen(buf_p);

	/*--- enough room for fractional seconds and timezone info? ---*/
	if (length < n)
	{
		char frac[20];

		(void)sprintf(frac, ".%09d UTC", time_p->nanoseconds);

		strncat(buf_p, frac, n - length);

		length += strlen(frac);

		if (length >= n)
			*(buf_p + n - 1) = '\0';
	}

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlPause(period_p)
* INPUT
*      period_p == pointer to an empty utlRelativeTime_T structure
* OUTPUT
*      buf_p == the formatted time
* RETURNS
*      utlSUCCESS for succes, utlFAILED for failure
* DESCRIPTION
*	Pauses execution for the specified period of time.  Note that this
*      call blocks for the duration of the time-out.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlPause(const utlRelativeTime_P2c period_p)
{
	utlAssert(period_p != NULL);

#ifdef USE_POSIX_API
	{
		struct timespec timeout;

		timeout.tv_sec  = period_p->seconds;
		timeout.tv_nsec = period_p->nanoseconds;

		/*--- uninterrupted sleep ---*/
		for (;; )
		{
			struct timespec time_left;

			time_left.tv_sec  = 0;
			time_left.tv_nsec = 0;

			if (nanosleep(&timeout, &time_left) == 0)
				break;

			if (errno != EINTR)
			{
				utlError("Call to %s() failed, errno=%d", "nanosleep", errno);
				return utlFAILED;
			}

			timeout = time_left;
		}
	}
#else
	{
		struct timeval timeout;

		timeout.tv_sec  = period_p->seconds;
		timeout.tv_usec = period_p->nanoseconds / 1000;

		if (select(0, NULL, NULL, NULL, &timeout) == -1)
		{
			utlError("call to select failed: %s", strerror(errno));
			return utlFAILED;
		}
	}
#endif

	return utlSUCCESS;
}

#ifdef utlTEST
/*---------------------------------------------------------------------------*
* FUNCTION
*	timeTest()
* INPUT
*	none
* OUTPUT
*	none
* RETURNS
*	"true" for pass, "false" for failure
*---------------------------------------------------------------------------*/
bool timeTest(void)
{
	utlAbsoluteTime_T time;
	char buf[50];

	/*--- test utlCurrentTime() ---------------------------------------------*/

	if (utlCurrentTime(&time) != utlSUCCESS)
	{
		(void)fprintf(stderr, "timeTest: utlCurrentTime() failed\n");
		return false;
	}

	/*--- test utlFormatTime() ---------------------------------------------*/

	if (utlFormatTime(buf, sizeof(buf), &time) != utlSUCCESS)
	{
		(void)fprintf(stderr, "timeTest: utlFormatTime() failed\n");
		return false;
	}
	printf("time: %s\n", buf);

	/*--- test utlFormatDate() ---------------------------------------------*/

	if (utlFormatDate(buf, sizeof(buf), &time) != utlSUCCESS)
	{
		(void)fprintf(stderr, "timeTest: utlFormatTime() failed\n");
		return false;
	}
	printf("date: %s\n", buf);

	/*--- test utlFormatDateTime() -----------------------------------------*/

	if (utlFormatDateTime(buf, sizeof(buf), &time) != utlSUCCESS)
	{
		(void)fprintf(stderr, "timeTest: utlFormatTime() failed\n");
		return false;
	}
	printf("date & time: %s\n", buf);

	return true;
}
#endif /* utlTEST */

