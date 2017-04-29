/*****************************************************************************
* Utility Library
*
* Mutex utilities header file
*
*****************************************************************************/

#ifndef _UTL_MUTEX_INCLUDED
#define _UTL_MUTEX_INCLUDED

#include <pthread.h>

#include <utlTypes.h>


/*--- Data Types ------------------------------------------------------------*/

/*--- additional return value for utlTryMutexLock() ---*/
#define utlWAS_LOCKED -2
/* The value assigned to the above macro assumes:
     utlFAILED  != -2
     utlSUCCESS != -2 */
#if (utlFAILED == utlWAS_LOCKED)
"Error: impractical value defined for utlFAILED";
#endif
#if (utlSUCCESS == utlWAS_LOCKED)
"Error: impractical value defined for utlSUCCESS";
#endif

typedef unsigned int utlMutexAttributes_T, *utlMutexAttributes_P;

#define utlMUTEX_ATTR_TYPE_BINARY           ((utlMutexAttributes_T)0x000001)
#define utlMUTEX_ATTR_TYPE_COUNTING         ((utlMutexAttributes_T)0x000002)
#define utlMUTEX_ATTR_TYPE_MASK             ((utlMutexAttributes_T)0x00000f)

#define utlMUTEX_ATTR_SCOPE_PROCESS_PRIVATE ((utlMutexAttributes_T)0x000010)
#define utlMUTEX_ATTR_SCOPE_PROCESS_SHARED  ((utlMutexAttributes_T)0x000020)
#define utlMUTEX_ATTR_SCOPE_MASK            ((utlMutexAttributes_T)0x0000f0)

#define utlMUTEX_ATTR_PRIO_CEILING_MASK     ((utlMutexAttributes_T)0x00ff00)

#define utlMUTEX_ATTR_PROTOCOL_MASK         ((utlMutexAttributes_T)0x0f0000)

typedef struct utlMutex_S {
	bool initializing;
	bool initialized;
	pthread_mutex_t mutex;
} utlMutex_T, *utlMutex_P;

#define utlNULL_MUTEX { false, false, { 0 } }

#define utlIsMutexInitialized(mutex_p) ((mutex_p)->initialized != false)


/*--- Prototypes ------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

extern utlReturnCode_T utlInitMutex(const utlMutex_P mutex_p,
				    const utlMutexAttributes_T attributes);
extern utlReturnCode_T utlLockMutex(const utlMutex_P mutex_p);
extern utlReturnCode_T utlTryMutexLock(const utlMutex_P mutex_p);
extern bool            utlIsMutexLocked(const utlMutex_P mutex_p);
extern utlReturnCode_T utlUnlockMutex(const utlMutex_P mutex_p);
extern utlReturnCode_T _utlDoDestroyMutex(pthread_mutex_t            *mutex_p);
extern utlReturnCode_T utlDiscardMutex(const utlMutex_P mutex_p);

#ifdef __cplusplus
}
#endif

#endif /* _UTL_MUTEX_INCLUDED */

