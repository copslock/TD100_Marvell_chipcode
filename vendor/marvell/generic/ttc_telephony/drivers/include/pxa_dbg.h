/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************/
#ifndef __XSCALE_SOURCE_DEBUG_HEADER__
#define __XSCALE_SOURCE_DEBUG_HEADER__
#ifndef ACI_LNX_KERNEL
#include <stdio.h>
#include <syslog.h>
#endif

#ifndef ACI_LNX_KERNEL

#ifdef __cplusplus
extern "C" {
#endif
extern int eeh_draw_panic_text(const char *panic_text, size_t panic_len, int do_timer);
extern void eeh_log(int level, const char *format, ...);
#ifdef __cplusplus
}
#endif

    #define     ASSERT(a)       do { if (!(a)) { \
					     char buffer[512]; \
					     sprintf(buffer, "ASSERT FAIL AT FILE %s FUNC %s LINE %d", __FILE__, __FUNCTION__, __LINE__); \
					     eeh_draw_panic_text(buffer, strlen(buffer), 0); } \
} while (0)
#else
    #undef ASSERT
    #define     ASSERT(a) do { if (!(a)) { \
				       while (1) { \
					       char buffer[512]; \
					       sprintf(buffer, "ASSERT FAIL AT FILE %s FUNC %s LINE %d\n", __FILE__, __FUNCTION__, __LINE__); \
				       } } \
} while (0)
#endif


#ifdef  ACI_KERNEL_DEBUG

#ifndef ACI_LNX_KERNEL
	#define ACIKMSG(fmt, args ...)    eeh_log(LOG_DEBUG, "ACIU: " fmt, ## args)
#else  //ACI_LNX_KERNEL

	#define ACIKMSG(fmt, args ...)    printk(KERN_DEBUG "ACIK: " fmt, ## args)
#endif

#else //ACI_KERNEL_DEBUG
     #define ACIKMSG(fmt, args ...)
#endif


#ifndef ACI_LNX_KERNEL
	#define DPRINTF(fmt, args ...)    eeh_log(LOG_NOTICE, fmt, ## args)
	#define ERRMSG(fmt, args ...)     eeh_log(LOG_ERR, fmt, ## args)
	#define DBGMSG(fmt, args ...)     eeh_log(LOG_DEBUG, fmt, ## args)
	#define F_ENTER()       DBGMSG("Enter %s\n", __FUNCTION__);
	#define F_LEAVE()       DBGMSG("Leave %s\n", __FUNCTION__);
#else
	#define DPRINTF(fmt, args ...)    printk(KERN_NOTICE fmt, ## args)
    #define     ERRMSG(fmt, args ...)     printk(KERN_ERR fmt, ## args)
    #define DBGMSG(fmt, args ...) printk(KERN_DEBUG fmt, ## args)
	#define F_ENTER()       DBGMSG("Enter %s\n", __FUNCTION__);
	#define F_LEAVE()       DBGMSG("Leave %s\n", __FUNCTION__);
#endif

#endif

