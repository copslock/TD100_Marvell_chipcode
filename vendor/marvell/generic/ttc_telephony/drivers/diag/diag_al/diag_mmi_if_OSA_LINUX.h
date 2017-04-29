/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/*********************************************************************
 *                      M O D U L E     B O D Y                       *
 **********************************************************************
 * Title: Diag                                                        *
 *                                                                    *
 * Filename: diag_mmi_if_OSA_LINUX.h                                     *
 *                                                                    *
 * Target, platform: Common Platform, SW platform                     *
 *                                                                    *
 * Authors: Shuki Zanyovka											 *
 *                                                                    *
 * Description: handles Linux NETLINK event for insertion/removal     *
 *              of MMC/SD cards										 *
 *                                                                    *
 * Notes:                                                             *
 **********************************************************************/

#if !defined (_DIAG_MMI_IF_OSA_LINUX_H_)
#define _DIAG_MMI_IF_OSA_LINUX_H_

#include "osa.h"
#include "global_types.h"
#include "linux_types.h"
//#include "diag_comm.h"
#include "diag_init.h"

typedef enum
{
	MMI_REP_AUTO,                           //diag.cfg is configured to auto start mode
	MMI_REP_ALREADY_START,                  //got start when already in start
	MMI_REP_ALREADY_STOP,                   //got stop when already in stop
	MMI_REP_STOP_DONE,                      //stop process done
	MMI_REP_APPS_FILTER_DB_VER_MATCH,       //the filter file apps DB version match the target
	MMI_REP_APPS_FILTER_DB_VER_MISMATCH, //the filter file apps DB version doesn't match the target
	MMI_REP_COMM_FILTER_DB_VER_MATCH,       //the filter file comm DB version match the target
	MMI_REP_COMM_FILTER_DB_VER_MISMATCH, //the filter file comm DB version doesn't match the target
	MMI_REP_LSEEK,                          //overwriting the comm DB ver in the header
	MMI_REP_WRITE,                          //writing to a file
	MMI_REP_FOPEN,                          //opening the configuration file
	MMI_REP_OPEN,                           //opening a log file
	MMI_REP_FSYNC,                          //fsync() before closing a log file
	MMI_REP_CLOSE,                          //closing a log file
	MMI_REP_FCLOSE,                         //closing the configuration file
	MMI_REP_STAT,                           //non-existent directory for the log file
	MMI_REP_MKDIR                           //create the non-existent directory for the log file
} E_DiagOverFS_MMI_Report;


typedef enum
{
	MMI_IF_MOUNT_EVENT_CARD_ERROR,
	MMI_IF_MOUNT_EVENT_CARD_NOT_DETECTED,
	MMI_IF_MOUNT_EVENT_CARD_DETECTED,
	MMI_IF_MOUNT_EVENT_CARD_MOUNTED
} MMI_IF_Mount_Event_t;

typedef enum
{
	MMI_IF_EVENT_NONE,
	MMI_IF_EVENT_CARD_IN,
	MMI_IF_EVENT_CARD_OUT,
	MMI_IF_EVENT_REPORT_AUTO,
	MMI_IF_EVENT_REPORT_STOP_DONE,
	MMI_IF_EVENT_USER_START,
	MMI_IF_EVENT_USER_STOP
} MMI_IF_Event_t;

typedef enum
{
	MMI_IF_STATE_INIT,
	MMI_IF_STATE_INSERTED_STOPPED,
	MMI_IF_STATE_INSERTED_STARTED,
	MMI_IF_STATE_NOT_INSERTED_STOPPED,
	MMI_IF_STATE_NOT_INSERTED_STARTED,
	MMI_IF_STATE_MOUNTED_STOPPED,
	MMI_IF_STATE_MOUNTED_STARTED,
	MMI_IF_STATE_MOUNTING_MMI_STOPPED,
	MMI_IF_STATE_MOUNTING_MMI_STARTED,
	MMI_IF_STATE_UNMOUNTING_MMI_STOPPED,
	MMI_IF_STATE_UNMOUNTING_MMI_STARTED,
	MMI_IF_STATE_EXCEPTION_MMI_STOPPED,
	MMI_IF_STATE_EXCEPTION_MMI_STARTED
} MMI_IF_state_t;

typedef void (*MMI_STOP_CB_Func)(void);
typedef void (*MMI_START_CB_Func)(const char* rel_path);
BOOL MMI_Init(MMI_START_CB_Func MMI_Start_CB, MMI_STOP_CB_Func MMI_Stop_CB, EActiveConnectionLL_Type LL_Type, DiagBSPFSConfigS* p_FSConfig );

void MMI_Report(E_DiagOverFS_MMI_Report rep, int err_code);
void MMI_process_eehcontrol_event_receive(void);
void  readPath(void);
void MMI_process_eehcontrol_event_send(void);
void openEehwriteSocket(void);
void openEehReadSocket(void);


#endif //!defined (_DIAG_MMI_IF_OSA_LINUX_H_)

