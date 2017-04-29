/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.

 *(C) Copyright 2007 Marvell International Ltd.
 * All Rights Reserved
 */

#ifndef __EEH_IOCTL__
#define __EEH_IOCTL__

#include "linux_types.h"

/* The retun value of the EEH API finction calls. */
typedef UINT32 EEH_STATUS;

/* Error code */
#define EEH_NO_ERROR                                                    0
#define EEH_SUCCESS                             EEH_NO_ERROR
#define EEH_ERROR                                                               (EEH_NO_ERROR - 1)
#define EEH_ERROR_INIT                                                  (EEH_NO_ERROR - 2)
#define EEH_MSG_DESC_LEN						512

#define SEH_IOC_MAGIC 'Y'
#define SEH_IOCTL_API  _IOW(SEH_IOC_MAGIC, 1, int)
#define SEH_IOCTL_TEST _IOW(SEH_IOC_MAGIC, 2, int)
#define SEH_IOCTL_APP_ASSERT            _IOW(SEH_IOC_MAGIC, 4, int)
#define SEH_IOCTL_EMULATE_PANIC			_IOW(SEH_IOC_MAGIC, 13, int)
#define SEH_IOCTL_SET_ERROR_INFO		_IOW(SEH_IOC_MAGIC, 14, int)
#define SEH_IOCTL_CP_SILENT_RESET    _IOW(SEH_IOC_MAGIC, 15, int)

typedef enum _EehApiId
{
	_EehInit    = 0,
	_EehDeInit,
	_EehInsertComm2Reset,
	_EehReleaseCommFromReset,
	_EehCopyCommImageFromFlash,
	_EehDisableCPUFreq,
	_EehEnableCPUFreq
}EehApiId;

typedef struct _EehApiParams
{
	int eehApiId;
	UINT32 status;
	void *params;
} EehApiParams;

typedef enum _EehMsgInd
{
	EEH_INVALID_MSG     = 0,
	EEH_WDT_INT_MSG,
	EEH_AP_ASSERT_MSG,
	EEH_CP_SILENT_RESET_MSG,
}EehMsgInd;
typedef enum _EehAssertType
{
	EEH_AP_ASSERT =0,
	EEH_CP_EXCEPTION =1,
	EEH_AP_EXCEPTION=2,
	EEH_CP_ASSERT,
	EEH_NONE_ASSERT,
}EehAssertType;

typedef struct _EehInsertComm2ResetParam
{
	UINT32 AssertType;
}EehInsertComm2ResetParam;

typedef struct _EehMsgStruct
{
	UINT32 msgId;
	char msgDesc[EEH_MSG_DESC_LEN];   //for save AP assert description
}EehMsgStruct;

typedef struct _EehAppAssertParam
{
	char msgDesc[EEH_MSG_DESC_LEN];
}EehAppAssertParam;

typedef struct _EehCpSilentResetParam
{
	char msgDesc[EEH_MSG_DESC_LEN];
}EehCpSilentResetParam;

/* Communicate the error info to SEH (for RAMDUMP and m.b. more) */
typedef struct _EehErrorInfo
{
	unsigned err;
	char	 *str;
	unsigned char *regs;
} EehErrorInfo;

/* err: SEH converts these into what defined in kernel ramdump_defs.h file */
#define ERR_EEH_CP 0
#define ERR_EEH_AP 1

#endif


