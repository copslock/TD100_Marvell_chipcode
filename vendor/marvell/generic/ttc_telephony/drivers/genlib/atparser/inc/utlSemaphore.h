/*****************************************************************************
* Utility Library
*
* Semaphore utilities header file
*
*****************************************************************************/

#ifndef _UTL_SEMAPHORE_INCLUDED
#define _UTL_SEMAPHORE_INCLUDED

#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include <utlTypes.h>
#include <utlMutex.h>
#include <utlVString.h>


/*--- Data Types ------------------------------------------------------------*/

typedef unsigned int utlSemaphoreAttributes_T, *utlSemaphoreAttributes_P;

#define utlSEMAPHORE_ATTR_SHARING_ENABLE        ((utlSemaphoreAttributes_T)0x0001)
#define utlSEMAPHORE_ATTR_SHARING_MASK          ((utlSemaphoreAttributes_T)0x000f)

#define utlSEMAPHORE_ATTR_NESTING_ENABLE        ((utlSemaphoreAttributes_T)0x0010)
#define utlSEMAPHORE_ATTR_NESTING_MASK          ((utlSemaphoreAttributes_T)0x00f0)

#define utlSEMAPHORE_ATTR_SCOPE_PROCESS_PRIVATE ((utlSemaphoreAttributes_T)0x0100)
#define utlSEMAPHORE_ATTR_SCOPE_PROCESS_SHARED  ((utlSemaphoreAttributes_T)0x0200)
#define utlSEMAPHORE_ATTR_SCOPE_MASK            ((utlSemaphoreAttributes_T)0x0f00)

typedef struct utlSemaphore_S {
	bool initializing;
	bool initialized;
	bool enable_sharing;                    /* allow sharing where possible?     */
	bool enable_nesting;                    /* allow nesting where possible?     */
	pthread_mutex_t in_use_mutex;           /* mutex for read or write access    */
	pthread_mutex_t block_sharing_mutex;    /* mutex to block subsequent sharing */
	pthread_mutex_t num_shared_mutex;       /* mutex for `num_shared' access     */
	pthread_mutex_t num_exclusive_mutex;    /* mutex for `num_exclusive' access  */
	unsigned int num_shared;                /* current number of shared locks    */
	unsigned int num_exclusive;             /* current number of exclusive locks */
	pid_t pid;                              /* PID of exclusive owner process    */
	pthread_t thread;                       /* ID  of exclusive owner thread     */
} utlSemaphore_T, *utlSemaphore_P;

#define utlNULL_SEMAPHORE { false, false, false, false, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, 0, 0, 0, 0 }


#define utlIsSemaphoreInitialized(semaphore_p) ((semaphore_p)->initialized != false)

/*--- Prototypes ------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

extern utlReturnCode_T utlInitSemaphore(const utlSemaphore_P semaphore_p,
					const utlSemaphoreAttributes_T attributes);
extern utlReturnCode_T utlAcquireSharedAccess(const utlSemaphore_P semaphore_p);
extern utlReturnCode_T utlReleaseSharedAccess(const utlSemaphore_P semaphore_p);
extern utlReturnCode_T utlAcquireExclusiveAccess(const utlSemaphore_P semaphore_p);
extern utlReturnCode_T utlReleaseExclusiveAccess(const utlSemaphore_P semaphore_p);
extern utlReturnCode_T utlDiscardSemaphore(const utlSemaphore_P semaphore_p);

#if defined(utlDEBUG) || defined(utlTEST)
extern void            utlVStringDumpSemaphore(const utlVString_P v_string_p,
					       const char                     *label_p,
					       const utlSemaphore_P semaphore_p,
					       const char                     *prefix_p);
extern void            utlDumpSemaphore(      FILE                     *file_p,
					      const char                     *label_p,
					      const utlSemaphore_P semaphore_p);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _UTL_SEMAPHORE_INCLUDED */

