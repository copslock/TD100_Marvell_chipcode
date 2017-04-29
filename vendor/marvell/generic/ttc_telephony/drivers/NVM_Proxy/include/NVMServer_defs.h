// (C) Copyright 2006 Marvell International Ltd.
// All Rights Reserved
//
/******************************************************************************
**
** INTEL CONFIDENTIAL
** Copyright 2000-2005 Intel Corporation All Rights Reserved.
**
** The source code contained or described herein and all documents
** related to the source code (Material) are owned by Intel Corporation
** or its suppliers or licensors.  Title to the Material remains with
** Intel Corporation or its suppliers and licensors. The Material contains
** trade secrets and proprietary and confidential information of Intel
** or its suppliers and licensors. The Material is protected by worldwide
** copyright and trade secret laws and treaty provisions. No part of the
** Material may be used, copied, reproduced, modified, published, uploaded,
** posted, transmitted, distributed, or disclosed in any way without Intel’s
** prior express written permission.
**
** No license under any patent, copyright, trade secret or other intellectual
** property right is granted to or conferred upon you by disclosure or
** delivery of the Materials, either expressly, by implication, inducement,
** estoppel or otherwise. Any license under such intellectual property rights
** must be express and approved by Intel in writing.
**
*****************************************************************************/
#pragma once

#include "global_types.h"
#include "pxa_dbg.h"

#ifdef  DEBUG_NVM_PROXY
#define DEBUGMSG                DPRINTF
#else
#define DEBUGMSG        DBGMSG
#endif

#define RETAILMSG       DPRINTF

//////////////////////////////////////////////////////////////////////////////

#define NVM_CLIENT_COMMS _T("Comm")
#define NVM_CLIENT_BT _T("Bluethooth")


#define NVM_CLIENT_ID DWORD
#define VALID_NVM_CLIENT_ID_TEMPLETE 0xfafb

//HKEY_LOCAL_MACHINE
#define TRUSTED_CLIENT_KEY  TEXT("Drivers\\BuiltIn\\NVMServer\\Clients\\")
//registry value\keys name
#define READ_ONLY_PATH_VALUE_NAME _T("ReadOnlyPath")
#define READ_WRITE_PATH_VALUE_NAME _T("ReadWritePath")
#define TRANSLATION_TABLE_PATH_KEY_NAME _T("TransTable")
#define TRANSLATION_TABLE_SIZE_VALUE_NAME _T("Size")
#define TRANSLATION_TABLE_EX_VALUE_NAME _T("TransTableEx")


#define IOCTL_NVM_REGISTER              FSCTL_USER(1)
#define IOCTL_NVM_DE_REGISTER   FSCTL_USER(2)
#define IOCTL_NVM_FILE_READ             FSCTL_USER(3)
#define IOCTL_NVM_FILE_GET_SIZE FSCTL_USER(4)
#define IOCTL_NVM_FILE_SEEK             FSCTL_USER(5)
#define IOCTL_NVM_FILE_OPEN             FSCTL_USER(6)
#define IOCTL_NVM_FILE_CLOSE    FSCTL_USER(7)
#define IOCTL_NVM_FILE_CLOSE_ALL        FSCTL_USER(8)
#define IOCTL_NVM_FILE_FLUSH    FSCTL_USER(9)
#define IOCTL_NVM_FILE_WRITE    FSCTL_USER(10)
#define IOCTL_NVM_FILE_DELETE   FSCTL_USER(11)
#define IOCTL_NVM_FIND_FIRST_FILE       FSCTL_USER(12)
#define IOCTL_NVM_FIND_NEXT_FILE        FSCTL_USER(13)
#define IOCTL_NVM_FIND_CLOSE    FSCTL_USER(14)
#define IOCTL_NVM_CLOSE_ALL_FILES       FSCTL_USER(15)




//FSCTL_USER(FILE_DEVICE_FILE_SYSTEM, 0x1, METHOD_BUFFERED, FILE_ANY_ACCESS)





//Entry format is as followed
// <fs id>,<nvm file name>,<fs file name>

// NVM configuration
#define NVM_ROOT_DIR    "NVM_ROOT_DIR"
#define NVM_IPC_ACM     "NVM_IPC_ACM"
#define NVM_IPC_EEH "NVM_IPC_EEH"
#define NVM_CLIENT_COMM_NAME   ((char *)"COMM")
#define NVM_ACAT_CLIENT_NAME   ((char *)"ACAT")
#define NVM_CLIENT_BT           _T("Bluethooth")
#define NVM_CLIENT_COMM         0x01
#define NVM_CLIENT_ACAT         0x03


#define NVM_FILE_NAME_LENGTH 64                                 //NVM File Name length
#define FS_FILE_NAME_LENGTH FILENAME_MAX - NVM_FILE_NAME_LENGTH //File System File length, we substract NVM_FILE_NAME_LENGTH so we will not exceed MAX_PATH

//Translation Table Structure and Defines
typedef struct
{
	short fs_id;
	TCHAR szNVMfileName[NVM_FILE_NAME_LENGTH];
	TCHAR szFSfileName[FS_FILE_NAME_LENGTH];

}TRANS_TABLE_ENTRY;
#define TRANS_TABLE_FSID_FIELD_INDEX 1
#define TRANS_TABLE_NVM_FILE_FIELD_INDEX 2
#define TRANS_TABLE_FS_FIELD_INDEX 3
#define TRANS_TABLE_ENTRY_FIELD_COUNT 3

//default path structure and defines
typedef struct
{
	short fs_id;
	TCHAR szFSDefaultPath[FILENAME_MAX];

}DEFAULT_PATH_ENTRY, *PDEFAULT_PATH_ENTRY;


#define DEFAULT_PATH_FSID_FIELD_INDEX 1
#define DEFAULT_PATH_FS_FIELD_INDEX 2
#define DEFAULT_PATH_FIELD_COUNT 2



#ifdef DEBUG
#ifndef ZONE_ERROR
#define ZONE_ERROR           DEBUGZONE(0)
#endif
#define ZONE_WARN            DEBUGZONE(1)
#ifndef ZONE_INIT
#define ZONE_INIT            DEBUGZONE(2)
#endif

#define ZONE_READ        DEBUGZONE(3)
#define ZONE_WRITE          DEBUGZONE(4)
#define ZONE_OPEN          DEBUGZONE(5)
#define ZONE_DELETE                      DEBUGZONE(6)
#define ZONE_FIND                        DEBUGZONE(7)
#define ZONE_SEEK            DEBUGZONE(8)
#define ZONE_CLOSE           DEBUGZONE(9)
#define ZONE_API             DEBUGZONE(10)
#define ZONE_INFO            DEBUGZONE(11)
#else
#ifndef ZONE_ERROR
#define ZONE_ERROR           1
#endif
#define ZONE_WARN            0
#ifndef ZONE_INIT
#define ZONE_INIT            1
#endif
#define ZONE_READ        0
#define ZONE_WRITE           0
#define ZONE_OPEN          0
#define ZONE_DELETE                      0
#define ZONE_FIND                        0
#define ZONE_SEEK                0
#define ZONE_CLOSE              0
#define ZONE_API                0
#define ZONE_INFO               0

#endif

