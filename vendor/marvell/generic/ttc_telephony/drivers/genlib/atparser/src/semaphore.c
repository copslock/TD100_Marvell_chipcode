/*****************************************************************************
* Utility Library
*
* DESCRIPTION
*	Semaphore wrapper functions.  This semaphore implementation allows
*      both shared access (multiple simulatneous readers), or exclusive
*      access (only one writer) at any given moment.
*
*      Processes/threads wanting exclusive access have priority (a pending
*      process/thread wanting exclusive access blocks all subsequent processes/
*      threads requesting shared access until the process/thread currently having
*      exclusive access is finished).
*
*      When nesting is enabled, if a given process+thread has the semaphore
*      locked for exclusive access, that process+thread can also simulatneously
*      engage a lock (on the same semaphore) for shared access (although access
*      will not be granted to other processes/threads), and also engage additional
*      exclusive-access locks (again, on the same semphore).  These additional
*      locks must of couse be followed by corresponding unlocks before the resource
*      is actually free'ed.
*
*      Current restriction: If a process/thread has the semaphore locked for shared
*      access, it cannot simultaneously lock the semaphore for exclusive access--all
*      shared-access locks must first be released.
*
* EXAMPLE USAGE
*
*      {
*      utlSemaphore_T           semaphore;
*      utlSemaphoreAttributes_T attr;
*
*          attr = utlSEMAPHORE_ATTR_SHARING_ENABLE |
*                 utlSEMAPHORE_ATTR_NESTING_ENABLE;
*          if (utlInitSemaphore(&semaphore, attr) != utlSUCCESS)
*              <handle error here>
*
*          if (utlAcquireSharedAccess(&semaphore) != utlSUCCESS)
*              <handle error here>
*
*          <read protected resource here>
*
*          if (utlReleaseSharedAccess(&semaphore) != utlSUCCESS)
*              <handle error here>
*
*          if (utlAcquireExclusiveAccess(&semaphore) != utlSUCCESS)
*              <handle error here>
*
*          <modify protected resource here>
*
*          if (utlReleaseExclusiveAccess(&semaphore) != utlSUCCESS)
*              <handle error here>
*
*      }
*
*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include <utlTypes.h>
#include <utlError.h>
#include <utlMalloc.h>
#include <utlMutex.h>

#include <utlSemaphore.h>
#include <utlTrace.h>


/*---------------------------------------------------------------------------*
* FUNCTION
*	utlInitSemaphore(semaphore_p, attributes)
* INPUT
*      semaphore_p == pointer to a valid utlSemaphore_T structure
*       attributes == semaphore attributes
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Initializes a shared-resource semaphore.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlInitSemaphore(const utlSemaphore_P semaphore_p,
				 const utlSemaphoreAttributes_T attributes)
{
	int pthread_rv;
	pthread_mutexattr_t attrs;
	bool enable_sharing;
	bool enable_nesting;

	utlAssert(semaphore_p != NULL);

	/* already initialized */
	if (semaphore_p->initialized == true)
	{
		return utlSUCCESS;
	}

	/*--- initialize attributes used by all mutexs ---*/
	{
		int value;

		/*--- set defaults ---*/
		enable_sharing = false;
		enable_nesting = false;

		/*--- configure mutex type ---*/
		{
			if ((pthread_rv = pthread_mutexattr_init(&attrs)) != 0)
			{
				utlError("Call to %s() failed, %s", "pthread_mutexattr_init", strerror(pthread_rv));
				return utlFAILED;
			}

#           if defined __USE_UNIX98 || defined (BIONIC)
			value = PTHREAD_MUTEX_ERRORCHECK;
#           else
			value = PTHREAD_MUTEX_ERRORCHECK_NP;
#           endif

			if ((pthread_rv = pthread_mutexattr_settype(&attrs, value)) != 0)
			{
				utlError("Call to %s() failed, %s", "pthread_mutexattr_settype", strerror(pthread_rv));

				/*--- clean ---*/
				(void)pthread_mutexattr_destroy(&attrs);

				return utlFAILED;
			}
		}

		/*--- configure semaphore sharing flag ---*/
		if (        attributes & utlSEMAPHORE_ATTR_SHARING_MASK)
		{
			switch (attributes & utlSEMAPHORE_ATTR_SHARING_MASK)
			{
			case utlSEMAPHORE_ATTR_SHARING_ENABLE: enable_sharing = true;  break;

			default:
				utlError("Invalid %s attribute 0x%08x", "semaphore-sharing", attributes & utlSEMAPHORE_ATTR_SHARING_MASK);

				/*--- clean ---*/
				(void)pthread_mutexattr_destroy(&attrs);

				return utlFAILED;
			}
		}

		/*--- configure semaphore nesting flag ---*/
		if (        attributes & utlSEMAPHORE_ATTR_NESTING_MASK)
		{
			switch (attributes & utlSEMAPHORE_ATTR_NESTING_MASK)
			{
			case utlSEMAPHORE_ATTR_NESTING_ENABLE: enable_nesting = true;  break;

			default:
				utlError("Invalid %s attribute 0x%08x", "semaphore-nesting", attributes & utlSEMAPHORE_ATTR_NESTING_MASK);

				/*--- clean ---*/
				(void)pthread_mutexattr_destroy(&attrs);

				return utlFAILED;
			}
		}

#if !defined (BIONIC)  /* PHREAD_PROCESS_SHARED not suppored in Bionic */
		/*--- configure semaphore scope ---*/
		if (        attributes & utlSEMAPHORE_ATTR_SCOPE_MASK)
		{
			switch (attributes & utlSEMAPHORE_ATTR_SCOPE_MASK)
			{
			case utlSEMAPHORE_ATTR_SCOPE_PROCESS_PRIVATE: value = PTHREAD_PROCESS_PRIVATE;  break;
			case utlSEMAPHORE_ATTR_SCOPE_PROCESS_SHARED:  value = PTHREAD_PROCESS_SHARED;   break;

			default:
				utlError("Invalid %s attribute 0x%08x", "semaphore-scope", attributes & utlSEMAPHORE_ATTR_SCOPE_MASK);

				/*--- clean ---*/
				(void)pthread_mutexattr_destroy(&attrs);

				return utlFAILED;
			}

			if ((pthread_rv = pthread_mutexattr_setpshared(&attrs, value)) != 0)
			{
				utlError("Call to %s() failed, %s", "pthread_mutexattr_setpshared", strerror(pthread_rv));

				/*--- clean ---*/
				(void)pthread_mutexattr_destroy(&attrs);

				return utlFAILED;
			}
		}
#endif          /* BIONIC */
	}

	/*-----------------------------------------------------------------------*/

	/*--- if someone else is already initializing this semaphore ---*/
	if (semaphore_p->initializing == true)
	{
		/*--- clean ---*/
		(void)pthread_mutexattr_destroy(&attrs);

		return utlSUCCESS;
	}

	/*--- Note: we don't need a strict test-and-set here, since all entities
	      trying to access this semaphore are trying to do the same thing:
	      initialize it */
	semaphore_p->initializing = true;

	/*--- initialize all mutexs ---*/
	{
		if ((pthread_rv = pthread_mutex_init(&(semaphore_p->in_use_mutex), &attrs)) != 0)
		{
			utlError("call to %s() failed, %s", "pthread_mutex_init", strerror(pthread_rv));

			/*--- clean ---*/
			(void)pthread_mutexattr_destroy(&attrs);

			return utlFAILED;
		}
		if ((pthread_rv = pthread_mutex_init(&(semaphore_p->block_sharing_mutex), &attrs)) != 0)
		{
			utlError("call to %s() failed, %s", "pthread_mutex_init", strerror(pthread_rv));

			/*--- clean ---*/
			(void)pthread_mutexattr_destroy(&attrs);
			(void)pthread_mutex_destroy(&(semaphore_p->in_use_mutex));

			return utlFAILED;
		}
		if ((pthread_rv = pthread_mutex_init(&(semaphore_p->num_shared_mutex), &attrs)) != 0)
		{
			utlError("call to %s() failed, %s", "pthread_mutex_init", strerror(pthread_rv));

			/*--- clean ---*/
			(void)pthread_mutexattr_destroy(&attrs);
			(void)pthread_mutex_destroy(&(semaphore_p->block_sharing_mutex));
			(void)pthread_mutex_destroy(&(semaphore_p->in_use_mutex));

			return utlFAILED;
		}
		if ((pthread_rv = pthread_mutex_init(&(semaphore_p->num_exclusive_mutex), &attrs)) != 0)
		{
			utlError("call to %s() failed, %s", "pthread_mutex_init", strerror(pthread_rv));

			/*--- clean ---*/
			(void)pthread_mutexattr_destroy(&attrs);
			(void)pthread_mutex_destroy(&(semaphore_p->num_shared_mutex));
			(void)pthread_mutex_destroy(&(semaphore_p->block_sharing_mutex));
			(void)pthread_mutex_destroy(&(semaphore_p->in_use_mutex));

			return utlFAILED;
		}
	}

	semaphore_p->enable_sharing = enable_sharing;
	semaphore_p->enable_nesting = enable_nesting;
	semaphore_p->num_shared     = 0;        /* not locked for shared access        */
	semaphore_p->num_exclusive  = 0;        /* not locked for exxclusive access    */
	semaphore_p->pid            = 0;        /* no process has the semaphore locked */
	semaphore_p->thread         = 0;        /* no thread  has the semaphore locked */

	semaphore_p->initializing = false;
	semaphore_p->initialized  = true;

	/*-----------------------------------------------------------------------*/

	/*--- destroy mutex attributes ---*/
	if ((pthread_rv = pthread_mutexattr_destroy(&attrs)) != 0)
	{
		utlError("Call to %s() failed, %s", "pthread_mutexattr_destroy", strerror(pthread_rv));

		/*--- clean ---*/
		(void)pthread_mutex_destroy(&(semaphore_p->num_exclusive_mutex));
		(void)pthread_mutex_destroy(&(semaphore_p->num_shared_mutex));
		(void)pthread_mutex_destroy(&(semaphore_p->block_sharing_mutex));
		(void)pthread_mutex_destroy(&(semaphore_p->in_use_mutex));

		return utlFAILED;
	}

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlAcquireSharedAccess(semaphore_p)
* INPUT
*      semaphore_p == pointer to a valid utlSemaphore_T structure
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Blocks waiting for shared access to a semaphore-protected resource.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlAcquireSharedAccess(const utlSemaphore_P semaphore_p)
{
	int pthread_rv;
	bool exclusively_mine;

	utlAssert(semaphore_p != NULL);

	/*--- if semaphore is still initializing ---*/
	if (semaphore_p->initializing == true)
		return utlFAILED;

	utlAssert(semaphore_p->initialized == true);

	/*--- if nesting is enabled ---*/
	if (semaphore_p->enable_nesting == true)
	{

		/*--- wait for exclusive access to `pid' and `thread' fields ---*/
		if ((pthread_rv = pthread_mutex_lock(&(semaphore_p->num_exclusive_mutex))) != 0)
		{
			utlError("call to %s() failed, %s", "pthread_mutex_lock", strerror(pthread_rv));
			return utlFAILED;
		}

		exclusively_mine =               (semaphore_p->pid == getpid()) &&
				   (pthread_equal(semaphore_p->thread, pthread_self()) == 1);

		/*--- release exclusive access to `pid' and `thread' fields ---*/
		if ((pthread_rv = pthread_mutex_unlock(&(semaphore_p->num_exclusive_mutex))) != 0)
		{
			utlError("call to %s() failed, %s", "pthread_mutex_unlock", strerror(pthread_rv));
			return utlFAILED;
		}

	}
	else
		exclusively_mine = false;

	/*--- if nesting is enabled and this process/thread currently has exclusive
	      access, it is allowed to engage shared locks (but access will actually
	      be exclusive to this process/thread until ALL locks are released) */
	if (exclusively_mine == true)
	{

		/*--- wait for exclusive access to `num_shared' field ---*/
		if ((pthread_rv = pthread_mutex_lock(&(semaphore_p->num_shared_mutex))) != 0)
		{
			utlError("call to %s() failed, %s", "pthread_mutex_lock", strerror(pthread_rv));
			return utlFAILED;
		}

		semaphore_p->num_shared++;

		/*--- release exclusive access to `num_shared' field ---*/
		if ((pthread_rv = pthread_mutex_unlock(&(semaphore_p->num_shared_mutex))) != 0)
		{
			utlError("call to %s() failed, %s", "pthread_mutex_unlock", strerror(pthread_rv));
			return utlFAILED;
		}

		return utlSUCCESS;
	}

	/*--- block if sharing is prohibited ---*/
	if ((pthread_rv = pthread_mutex_lock(&(semaphore_p->block_sharing_mutex))) != 0)
	{
		utlError("call to %s() failed, %s", "pthread_mutex_lock", strerror(pthread_rv));
		return utlFAILED;
	}

	/*--- wait for exclusive access to `num_shared' field ---*/
	if ((pthread_rv = pthread_mutex_lock(&(semaphore_p->num_shared_mutex))) != 0)
	{
		utlError("call to %s() failed, %s", "pthread_mutex_lock", strerror(pthread_rv));

		/*--- clean ---*/
		(void)pthread_mutex_unlock(&(semaphore_p->block_sharing_mutex));

		return utlFAILED;
	}

	/*--- are we the very first wanting to share this semaphore-protected resource? ---*/
	if (semaphore_p->num_shared++ == (unsigned int)0)
	{

		/*--- semaphore-protected resource now being accessed for sharing ---*/
		if ((pthread_rv = pthread_mutex_lock(&(semaphore_p->in_use_mutex))) != 0)
		{
			utlError("call to %s() failed, %s", "pthread_mutex_lock", strerror(pthread_rv));

			/*--- clean ---*/
			(void)pthread_mutex_unlock(&(semaphore_p->num_shared_mutex));
			(void)pthread_mutex_unlock(&(semaphore_p->block_sharing_mutex));

			return utlFAILED;
		}
	}

	/*--- release exclusive access to `num_shared' field ---*/
	if ((pthread_rv = pthread_mutex_unlock(&(semaphore_p->num_shared_mutex))) != 0)
	{
		utlError("call to %s() failed, %s", "pthread_mutex_unlock", strerror(pthread_rv));

		/*--- clean ---*/
		(void)pthread_mutex_unlock(&(semaphore_p->block_sharing_mutex));

		return utlFAILED;
	}

	/*--- if sharing is enabled, allow more shared-locks... ---*/
	if (semaphore_p->enable_sharing == true)
		if ((pthread_rv = pthread_mutex_unlock(&(semaphore_p->block_sharing_mutex))) != 0)
		{
			utlError("call to %s() failed, %s", "pthread_mutex_unlock", strerror(pthread_rv));
			return utlFAILED;
		}

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlReleaseSharedAccess(semaphore_p)
* INPUT
*      semaphore_p == pointer to a valid utlSemaphore_T structure
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Signals the end of shared access to the semaphore-protected resource.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlReleaseSharedAccess(const utlSemaphore_P semaphore_p)
{
	int pthread_rv;
	bool exclusively_mine;

	utlAssert(semaphore_p != NULL);

	/*--- if semaphore is still initializing ---*/
	if (semaphore_p->initializing == true)
		return utlFAILED;

	/*--- wait for exclusive access to `pid' and `thread' fields ---*/
	if ((pthread_rv = pthread_mutex_lock(&(semaphore_p->num_exclusive_mutex))) != 0)
	{
		utlError("call to %s() failed, %s", "pthread_mutex_lock", strerror(pthread_rv));
		return utlFAILED;
	}

	exclusively_mine =               (semaphore_p->pid == getpid()) &&
			   (pthread_equal(semaphore_p->thread, pthread_self()) == 1);

	/*--- release exclusive access to `pid' and `thread' fields ---*/
	if ((pthread_rv = pthread_mutex_unlock(&(semaphore_p->num_exclusive_mutex))) != 0)
	{
		utlError("call to %s() failed, %s", "pthread_mutex_unlock", strerror(pthread_rv));
		return utlFAILED;
	}

	/*--- if this process/thread currently has exclusive access ---*/
	if (exclusively_mine == true)
	{

		/*--- wait for exclusive access to `num_shared' field ---*/
		if ((pthread_rv = pthread_mutex_lock(&(semaphore_p->num_shared_mutex))) != 0)
		{
			utlError("call to %s() failed, %s", "pthread_mutex_lock", strerror(pthread_rv));
			return utlFAILED;
		}

		utlAssert(semaphore_p->num_shared > (unsigned int)0);
		semaphore_p->num_shared--;

		/*--- release exclusive access to `num_shared' field ---*/
		if ((pthread_rv = pthread_mutex_unlock(&(semaphore_p->num_shared_mutex))) != 0)
		{
			utlError("call to %s() failed, %s", "pthread_mutex_unlock", strerror(pthread_rv));
			return utlFAILED;
		}

		return utlSUCCESS;
	}

	/*--- wait for exclusive access to `num_shared' field ---*/
	if ((pthread_rv = pthread_mutex_lock(&(semaphore_p->num_shared_mutex))) != 0)
	{
		utlError("call to %s() failed, %s", "pthread_mutex_lock", strerror(pthread_rv));
		return utlFAILED;
	}

	/*--- we the last one out? ---*/
	utlAssert(semaphore_p->num_shared > (unsigned int)0);
	if (--(semaphore_p->num_shared) == (unsigned int)0)
	{

		/*--- semaphore-protected resource now free ---*/
		if ((pthread_rv = pthread_mutex_unlock(&(semaphore_p->in_use_mutex))) != 0)
		{
			utlError("call to %s() failed, %s", "pthread_mutex_unlock", strerror(pthread_rv));

			/*--- clean ---*/
			(void)pthread_mutex_unlock(&(semaphore_p->num_shared_mutex));

			return utlFAILED;
		}
	}

	/*--- release exclusive access to `num_shared' field ---*/
	if ((pthread_rv = pthread_mutex_unlock(&(semaphore_p->num_shared_mutex))) != 0)
	{
		utlError("call to %s() failed, %s", "pthread_mutex_unlock", strerror(pthread_rv));
		return utlFAILED;
	}

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlAcquireExclusiveAccess(semaphore_p)
* INPUT
*      semaphore_p == pointer to a valid utlSemaphore_T structure
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Blocks waiting for exclusive access to a semaphore-protected resource.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlAcquireExclusiveAccess(const utlSemaphore_P semaphore_p)
{
	int pthread_rv;

	utlAssert(semaphore_p != NULL);

	/*--- if semaphore is still initializing ---*/
	if (semaphore_p->initializing == true)
		return utlFAILED;

	if ( (semaphore_p->enable_nesting == true) &&
	     (semaphore_p->pid == getpid()) &&
	     (pthread_equal(semaphore_p->thread, pthread_self()) == 1))
	{
		/* Same thread: Do nothing */
	}
	else
	{
		/*--- semaphore-protected resource now being accessed exclusively by us ---*/
		if ((pthread_rv = pthread_mutex_lock(&(semaphore_p->in_use_mutex))) != 0)
		{
			utlError("call to %s() failed, %s", "pthread_mutex_lock", strerror(pthread_rv));

			return utlFAILED;
		}
	}

	/*--- wait for exclusive access to `num_exclusive', `pid', and `thread' fields ---*/
	if ((pthread_rv = pthread_mutex_lock(&(semaphore_p->num_exclusive_mutex))) != 0)
	{
		utlError("call to %s() failed, %s", "pthread_mutex_lock", strerror(pthread_rv));
		return utlFAILED;
	}

	semaphore_p->num_exclusive++;


	/*--- we the very first writer? ---*/
	if (semaphore_p->num_exclusive == (unsigned int)1)
	{

		/*--- record this process/thread as the exclusive owner ---*/
		semaphore_p->pid    = getpid();
		semaphore_p->thread = pthread_self();

		#if 0
		/*--- block all subsequent sharing ---*/
		if ((pthread_rv = pthread_mutex_lock(&(semaphore_p->block_sharing_mutex))) != 0)
		{
			utlError("call to %s() failed, %s", "pthread_mutex_lock", strerror(pthread_rv));
			return utlFAILED;
		}
	#endif

		/*--- else if nesting is enabled and this thread already has exclusive access ---*/
	}

	/*--- clean ---*/
	(void)pthread_mutex_unlock(&(semaphore_p->num_exclusive_mutex));

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlReleaseExclusiveAccess(semaphore_p)
* INPUT
*      semaphore_p == pointer to a valid utlSemaphore_T structure
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Signals the end of exclusive access to the semaphore-protected resource.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlReleaseExclusiveAccess(const utlSemaphore_P semaphore_p)
{
	int pthread_rv;

	utlAssert(semaphore_p != NULL);

	/*--- if semaphore is still initializing ---*/
	if (semaphore_p->initializing == true)
		return utlFAILED;

	/*--- wait for exclusive access to `num_exclusive', `pid', and `thread' fields ---*/
	if ((pthread_rv = pthread_mutex_lock(&(semaphore_p->num_exclusive_mutex))) != 0)
	{
		utlError("call to %s() failed, %s", "pthread_mutex_lock", strerror(pthread_rv));
		return utlFAILED;
	}

	/*--- no more writers? ---*/
	utlAssert(semaphore_p->num_exclusive > (unsigned int)0);

	semaphore_p->num_exclusive--;
	/*--- clean ---*/
	(void)pthread_mutex_unlock(&(semaphore_p->num_exclusive_mutex));


	if (semaphore_p->num_exclusive == (unsigned int)0)
	{

		/*--- sanity check ---*/
		if ( (semaphore_p->pid != getpid()) ||
		     (pthread_equal(semaphore_p->thread, pthread_self()) != 1))
		{
			utlError("Semaphore 0x%x was not locked for write by this process and thread\n", semaphore_p);

			return utlFAILED;
		}

		/*--- wait for exclusive access to `num_exclusive', `pid', and `thread' fields ---*/
		if ((pthread_rv = pthread_mutex_lock(&(semaphore_p->num_exclusive_mutex))) != 0)
		{
			utlError("call to %s() failed, %s", "pthread_mutex_lock", strerror(pthread_rv));
			return utlFAILED;
		}

		/*--- unmark this process/thread as the current owner ---*/
		semaphore_p->pid    = 0;
		semaphore_p->thread = 0;

		/*--- clean ---*/
		(void)pthread_mutex_unlock(&(semaphore_p->num_exclusive_mutex));

		/*--- unblock sharing ---*/
	#if 0
		if ((pthread_rv = pthread_mutex_unlock(&(semaphore_p->block_sharing_mutex))) != 0)
		{
			utlError("call to %s() failed, %s", "pthread_mutex_unlock", strerror(pthread_rv));

			return utlFAILED;
		}
	#endif

		/*--- semaphore-protected resource now free ---*/
		if ((pthread_rv = pthread_mutex_unlock(&(semaphore_p->in_use_mutex))) != 0)
		{
			utlError("call to %s() failed, %s", "pthread_mutex_unlock", strerror(pthread_rv));
			return utlFAILED;
		}
	}

	return utlSUCCESS;
}

#if defined(utlDEBUG) || defined(utlTEST)

/*---------------------------------------------------------------------------*
 * FUNCTION
 *      utlMutexState(mutex_p)
 * INPUT
 *      mutex_p == pointer to a mutex
 * OUTPUT
 *      none
 * RETURNS
 *      a pointer to a character string
 * DESCRIPTION
 *      Determines if the specified mutex is locked or unlocked.
 *--------------------------------------------------------------------------*/
static char *utlMutexState(pthread_mutex_t *mutex_p)
{
	int pthread_rv;

	utlAssert(mutex_p != NULL);

	if ((pthread_rv = pthread_mutex_trylock(mutex_p)) != 0)
	{

		/*--- was mutex locked? ---*/
		if ((pthread_rv == EBUSY) ||
		    (pthread_rv == EDEADLK))
			return "Locked";

		/*--- we'll assume if trylock returns failure, that the mutex was not locked ---*/
		return "unlocked";
	}

	/*--- pthread_mutex_trylock() leaves the mutex locked, so unlock it ---*/
	if ((pthread_rv = pthread_mutex_unlock(mutex_p)) != 0)
		return "unknown";

	return "unlocked";
}

/*---------------------------------------------------------------------------*
 * FUNCTION
 *      utlVStringDumpSemaphore(v_string_p, label_p, semaphore_p, prefix_p)
 * INPUT
 *       v_string_p == pointer to an initialized utlVString_T structure
 *          label_p == pointer to a null-terminated character string, or NULL
 *      semaphore_p == pointer to a semaphore structure
 *         prefix_p == pointer to a prefix string
 * OUTPUT
 *      *v_string_p == the updated utlVString_T structure
 * RETURNS
 *      nothing
 * DESCRIPTION
 *      Dumps the semaphore specified by `semaphore_p' to `v_string_p'.
 *--------------------------------------------------------------------------*/
void utlVStringDumpSemaphore(const utlVString_P v_string_p,
			     const char           *label_p,
			     const utlSemaphore_P semaphore_p,
			     const char           *prefix_p)
{
	utlAssert(v_string_p  != NULL);
	utlAssert(semaphore_p != NULL);
	utlAssert(prefix_p    != NULL);

	(void)utlVStringPrintF(v_string_p, "%s:\n", prefix_p);

	if (label_p != NULL)
		(void)utlVStringPrintF(v_string_p, "%s:                name = \"%s\"\n", prefix_p, label_p);

	(void)utlVStringPrintF(v_string_p, "%s:         initialized = %s\n", prefix_p, (semaphore_p->initialized == true) ? "yes" : "no");

	if (semaphore_p->initialized == true)
	{

		(void)utlVStringPrintF(v_string_p, "%s:             sharing = %s\n", prefix_p,  (semaphore_p->enable_sharing == true) ? "enabled" : "disabled");
		(void)utlVStringPrintF(v_string_p, "%s:             nesting = %s\n", prefix_p,  (semaphore_p->enable_nesting == true) ? "enabled" : "disabled");
		(void)utlVStringPrintF(v_string_p, "%s:        in-use mutex = %s\n", prefix_p, utlMutexState(&(semaphore_p->in_use_mutex       )));
		(void)utlVStringPrintF(v_string_p, "%s: block-sharing mutex = %s\n", prefix_p, utlMutexState(&(semaphore_p->block_sharing_mutex)));
		(void)utlVStringPrintF(v_string_p, "%s:    num-shared mutex = %s\n", prefix_p, utlMutexState(&(semaphore_p->num_shared_mutex   )));
		(void)utlVStringPrintF(v_string_p, "%s: num-exclusive mutex = %s\n", prefix_p, utlMutexState(&(semaphore_p->num_exclusive_mutex)));
		(void)utlVStringPrintF(v_string_p, "%s:          num shared = %d\n", prefix_p,   semaphore_p->num_shared);
		(void)utlVStringPrintF(v_string_p, "%s:       num exclusive = %d\n", prefix_p,   semaphore_p->num_exclusive);
		(void)utlVStringPrintF(v_string_p, "%s:       exclusive PID = 0x%x\n", prefix_p, semaphore_p->pid);
		(void)utlVStringPrintF(v_string_p, "%s:    exclusive thread = 0x%x\n", prefix_p, semaphore_p->thread);
	}
}

/*---------------------------------------------------------------------------*
 * FUNCTION
 *      utlDumpSemaphore(file_p, label_p, semaphore_p)
 * INPUT
 *           file_p == pointer to an open file
 *          label_p == pointer to a null-terminated character string, or NULL
 *      semaphore_p == pointer to a semphore structure
 * OUTPUT
 *      none
 * RETURNS
 *      nothing
 * DESCRIPTION
 *      Dumps the semaphore specified by `semaphore_p'.
 *--------------------------------------------------------------------------*/
/*void utlDumpSemaphore(      FILE           *file_p,
		      const char           *label_p,
		      const utlSemaphore_P  semaphore_p)
   {
   utlVString_T v_string;

    utlAssert(file_p      != NULL);
    utlAssert(semaphore_p != NULL);

    utlInitVString(&v_string);

    utlVStringDumpSemaphore(&v_string, label_p, semaphore_p, "Semaphore");

    utlVStringPuts(&v_string, file_p);

    utlVStringFree(&v_string);
   }*/

#endif /* defined(utlDEBUG) || defined(utlTEST) */

#ifdef utlTEST
/*---------------------------------------------------------------------------*
* FUNCTION
*	semaphoreTest()
* INPUT
*	none
* OUTPUT
*	none
* RETURNS
*	"true" for pass, "false" for failure
*---------------------------------------------------------------------------*/
bool semaphoreTest(void)
{
	utlSemaphore_T test_semaphore;

	/*--- semaphore ----------------------------------------------------------*/
	if (utlInitSemaphore(&test_semaphore, utlSEMAPHORE_ATTR_SHARING_ENABLE |
			     utlSEMAPHORE_ATTR_NESTING_ENABLE) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlInitSemaphore() failed\n");
		return false;
	}

	/*--- shared ---*/
	if (utlAcquireSharedAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlAcquireSharedAccess(1) failed\n");
		return false;
	}

	if (utlReleaseSharedAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlReleaseSharedAccess(1) failed\n");
		return false;
	}

	/*--- mult-read ---*/
	if (utlAcquireSharedAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlAcquireSharedAccess(2) failed\n");
		return false;
	}

	if (utlAcquireSharedAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlAcquireSharedAccess(3) failed\n");
		return false;
	}

	if (utlReleaseSharedAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlReleaseSharedAccess(2) failed\n");
		return false;
	}

	if (utlReleaseSharedAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlReleaseSharedAccess(3) failed\n");
		return false;
	}

	/*--- write ---*/
	if (utlAcquireExclusiveAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlAcquireExclusiveAccess(1) failed\n");
		return false;
	}

	if (utlReleaseExclusiveAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlReleaseExclusiveAccess(1) failed\n");
		return false;
	}

	/*--- multi-write ---*/
	if (utlAcquireExclusiveAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlAcquireExclusiveAccess(2) failed\n");
		return false;
	}

	if (utlAcquireExclusiveAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlAcquireExclusiveAccess(3) failed\n");
		return false;
	}

	if (utlReleaseExclusiveAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlReleaseExclusiveAccess(2) failed\n");
		return false;
	}

	if (utlReleaseExclusiveAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlReleaseExclusiveAccess(3) failed\n");
		return false;
	}

	/*--- exclusive (1) ---*/
	if (utlAcquireExclusiveAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlAcquireExclusiveAccess(4) failed\n");
		return false;
	}

	if (utlAcquireSharedAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlAcquireSharedAccess(4) failed\n");
		return false;
	}

	if (utlReleaseSharedAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlReleaseSharedAccess(4) failed\n");
		return false;
	}

	if (utlReleaseExclusiveAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlReleaseExclusiveAccess(4) failed\n");
		return false;
	}

	/*--- exclusive (2) ---*/
	if (utlAcquireExclusiveAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlAcquireExclusiveAccess(5) failed\n");
		return false;
	}

	if (utlAcquireSharedAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlAcquireSharedAccess(5) failed\n");
		return false;
	}

	if (utlReleaseExclusiveAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlReleaseExclusiveAccess(5) failed\n");
		return false;
	}

	if (utlReleaseSharedAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlReleaseSharedAccess(5) failed\n");
		return false;
	}

	/*--- exclusive (3) ---*/
	if (utlAcquireExclusiveAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlAcquireExclusiveAccess(6) failed\n");
		return false;
	}

	if (utlAcquireExclusiveAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlAcquireExclusiveAccess(7) failed\n");
		return false;
	}

	if (utlAcquireSharedAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlAcquireSharedAccess(6) failed\n");
		return false;
	}

	if (utlAcquireSharedAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlAcquireSharedAccess(7) failed\n");
		return false;
	}

	if (utlReleaseExclusiveAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlReleaseExclusiveAccess(6) failed\n");
		return false;
	}

	if (utlReleaseSharedAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlReleaseSharedAccess(6) failed\n");
		return false;
	}

	if (utlReleaseExclusiveAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlReleaseExclusiveAccess(7) failed\n");
		return false;
	}

	if (utlReleaseSharedAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlReleaseSharedAccess(7) failed\n");
		return false;
	}

	/*--- exclusive (4) ---*/
	if (utlAcquireExclusiveAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlAcquireExclusiveAccess(8) failed\n");
		return false;
	}

	if (utlAcquireExclusiveAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlAcquireExclusiveAccess(9) failed\n");
		return false;
	}

	if (utlAcquireSharedAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlAcquireSharedAccess(8) failed\n");
		return false;
	}

	if (utlAcquireSharedAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlAcquireSharedAccess(9) failed\n");
		return false;
	}

	if (utlReleaseExclusiveAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlReleaseExclusiveAccess(8) failed\n");
		return false;
	}

	if (utlReleaseSharedAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlReleaseSharedAccess(8) failed\n");
		return false;
	}

	if (utlReleaseExclusiveAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlReleaseExclusiveAccess(9) failed\n");
		return false;
	}

	if (utlReleaseSharedAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlReleaseSharedAccess(9) failed\n");
		return false;
	}

	/*--- exclusive (5) ---*/
	if (utlAcquireExclusiveAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlAcquireExclusiveAccess(10) failed\n");
		return false;
	}

	if (utlAcquireSharedAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlAcquireSharedAccess(10) failed\n");
		return false;
	}

	if (utlAcquireExclusiveAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlAcquireExclusiveAccess(11) failed\n");
		return false;
	}

	if (utlAcquireSharedAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlAcquireSharedAccess(11) failed\n");
		return false;
	}

	if (utlReleaseExclusiveAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlReleaseExclusiveAccess(10) failed\n");
		return false;
	}

	if (utlReleaseExclusiveAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlReleaseExclusiveAccess(11) failed\n");
		return false;
	}

	if (utlReleaseSharedAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlReleaseSharedAccess(10) failed\n");
		return false;
	}

	if (utlReleaseSharedAccess(&test_semaphore) != utlSUCCESS)
	{
		(void)fprintf(stderr, "semaphoreTest: utlReleaseSharedAccess(11) failed\n");
		return false;
	}


	/*--- dump ---*/
	//utlDumpSemaphore(stdout, "test semaphore", &test_semaphore);


	return true;
}
#endif /* utlTEST */

