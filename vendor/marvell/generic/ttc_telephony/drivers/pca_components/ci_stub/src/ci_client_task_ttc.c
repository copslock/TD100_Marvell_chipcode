/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/******************************************************************************
**
**
**  FILENAME:       ci_client_task.c
**
**  PURPOSE:        CI client stub task

   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code ("Material") are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.

   Unless otherwise agreed by Intel in writing, you may not remove or alter this notice or any other notice embedded
   in Materials by Intel or Intel’s suppliers or licensors in any way.
******************************************************************************/
#include "ci_task.h"
#include "ci_syswrap.h"

#include "ci_api.h"
#include "ci_stub_ttc.h"
//#include "ci_mem_ttc.h"
#include "ci_trace.h"

#include "osa.h"

#include "string.h"

//#include "ci_dat.h"
//#include "ci_opt_dat_types.h"
#include <msocket_api.h>
#include <sys/socket.h>

#include "common_datastub.h"
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>

#define ciInitDoneMask 0x01

BOOL gDlLinkDownFlag = TRUE;
OSAMsgQRef ciClientStubMsgQ;
BOOL initTaskStop = 0;

/*-------------------------------------------------------------------------*
* Internal Global Variables
*-------------------------------------------------------------------------*/
static UINT8 ciClientStubTaskStack[CI_STUB_STACK_SIZE];
static OSTaskRef ciClientStubTaskRef   = NULL;
static UINT8 ciInitTaskStack[CI_STUB_STACK_SIZE];
static OSTaskRef ciInitTaskRef   = NULL;
char gCiSvcNameString[]                     = "CIService";
static ciClientStubInitStatus gCiInitStatus = CI_STUB_INIT_NOT_STARTED;
int cistubsockfd;
static CiClientStubStatusInd gStatusIndCb = NULL;
extern int cidata_fd;
OSASemaRef semaRef = 0;  /* for Semaphore */

OSASemaRef initTaskRef = 0;
extern int gTimeoutNum;
extern pthread_mutex_t gTimeoutLock;
void ciSendMsgToServer(int procId, unsigned char *msg, int msglen)
{
	int ret;
	ShmApiMsg *pShmArgs;

	CCI_ASSERT(msg != NULL);

	pShmArgs = (ShmApiMsg *)msg;
	pShmArgs->svcId = CiStubSvc_Id;
	pShmArgs->msglen = msglen;
	pShmArgs->procId = procId;

	ret = msend(cistubsockfd, msg, msglen + SHM_HEADER_SIZE, 0);
	//    CCI_ASSERT(ret >= 0);

}
void ciInitTask(void *input)
{
	int ciInitTryTimes = 400;
	ShmApiMsg startmsg;
	int isFirstTime = (int)input;
	DPRINTF("ciInitTask: isFirstTime:%d!\n", isFirstTime);
	if (isFirstTime)
		ioctl(cidata_fd, CCIDATASTUB_START, 0);
	else
		ioctl(cidata_fd, CCIDATASTUB_LINKSTATUS, &gDlLinkDownFlag);

	while ((ciInitTryTimes > 0) && !initTaskStop)
	{
		//init handshake send CiStubStartReq_MSG and wait for server ack CiStubStartCnf_MSG
		DPRINTF("ciInitTask: send hand shake messaget for %d times!\n", ciInitTryTimes);
		if (gCiInitStatus == CI_STUB_INIT_COMPLETE)
		{

			DPRINTF("ciInitTask: handshake done!\n");
			CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT_TASK, __LINE__, CCI_TRACE_INFO, "ciClientStubInit done!");
			gDlLinkDownFlag = FALSE;
			if (isFirstTime == TRUE)
			{
				gStatusIndCb(CISW_INIT_SUCCESS);
				// ioctl(cidata_fd, CCIDATASTUB_START, 0);
			}
			else
			{
				gStatusIndCb(CISW_INTERLINK_UP);
				// ioctl(cidata_fd, CCIDATASTUB_LINKSTATUS, &gDlLinkDownFlag);
			}
			OSASemaphoreRelease(initTaskRef);
			return;

		}
		ciSendMsgToServer(CiStubRequestStartProcId, (unsigned char *)&startmsg, 0);

		CCI_TRACE1(CI_CLIENT_STUB, CI_CLIENT_TASK, __LINE__, CCI_TRACE_DEBUG, "ciInitTask ciInitTryTimes: %d!", ciInitTryTimes);

		ciInitTryTimes--;
		OSATaskSleep(100);

	}
	OSASemaphoreRelease(initTaskRef);
}

CiSysWrapReturnCode  ciClientStubInit(CiClientStubStatusInd pStatusIndCb)
{
	OS_STATUS status;
	OSA_STATUS		creatingSemaphoreStatus = OS_INVALID_REF;

	CCI_TRACE1(CI_CLIENT_STUB, CI_CLIENT_TASK, __LINE__, CCI_TRACE_API, "ciClientStubInit: The ciClientStubPriority is %d", CI_STUB_TASK_PRIORITY);

#if 0
	/* Create the client stub message queue if not already created */
	if (ciClientStubMsgQ == NULL)
	{
		/* Create Client Stub task and MsgQ */
		/* Ii is assumed that OSA and DL initialization (like as mslMemInit) has done by some system entity */
		status = OSAMsgQCreate(&ciClientStubMsgQ,
				       "ciClienStubMsgQ",
				       CI_STUB_MESSAGE_Q_SIZE,
				       CI_STUB_MESSAGE_Q_MAX,
				       OSA_PRIORITY);

		if (status != OS_SUCCESS)
		{
			CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT_TASK, __LINE__, CCI_TRACE_ERROR, " ciClientStubInit: Client Stub OSA Message Queue Create Failed");
			return CISW_SYS_ERROR;
		}
	}
#endif
	/* Check if the client stub has been started already */
	if (gCiInitStatus != CI_STUB_INIT_NOT_STARTED)
	{
		return CISW_SYS_ERROR;
	}


	creatingSemaphoreStatus = OSASemaphoreCreate(&semaRef, 1, OSA_PRIORITY);
	CCI_ASSERT(creatingSemaphoreStatus == OS_SUCCESS );
	
	creatingSemaphoreStatus = OSASemaphoreCreate(&initTaskRef, 1, OSA_PRIORITY);
	CCI_ASSERT(creatingSemaphoreStatus == OS_SUCCESS );

	gStatusIndCb = pStatusIndCb;

	cistubsockfd = msocket(CISTUB_PORT);
	CCI_ASSERT(cistubsockfd > 0);
	CCI_TRACE1(CI_CLIENT_STUB, CI_CLIENT_TASK, __LINE__, CCI_TRACE_ERROR, " ciClientStubInit: cistubsockfd=0x%x", cistubsockfd);

	if ((cidata_fd = open("/dev/ccidatastub", O_RDWR)) == -1)
	{
		CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT_TASK, __LINE__, CCI_TRACE_ERROR, "Error open ccidatastub");
		CCI_ASSERT(cidata_fd >= 0);
	}

	if (ciClientStubTaskRef == NULL)
	{
		status = OSATaskCreate(&ciClientStubTaskRef,
				       (void *)ciClientStubTaskStack,
				       CI_STUB_STACK_SIZE,
				       CI_STUB_TASK_PRIORITY,
				       (CHAR*)"ciClientStubTask",
				       ciClientStubTask,
				       NULL);

		if (status != OS_SUCCESS)
		{
			CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT_TASK, __LINE__, CCI_TRACE_ERROR, " ciClientStubInit: CCI Stub OSA Task Create Failed");
			return CISW_SYS_ERROR;
		}
	}

	//need delete ciInitTask when init handshaking done
	if (ciInitTaskRef == NULL)
	{
		int isFirstTime = 1;
		OSASemaphoreAcquire(initTaskRef, OSA_SUSPEND);
		status = OSATaskCreate(&ciInitTaskRef,
				       (void *)ciInitTaskStack,
				       CI_STUB_STACK_SIZE,
				       CI_STUB_TASK_PRIORITY,
				       (CHAR*)"ciInitTask",
				       ciInitTask,
				       (void *)isFirstTime);

		if (status != OS_SUCCESS)
		{
			CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT_TASK, __LINE__, CCI_TRACE_ERROR, " ciClientStubInit: CCI Stub OSA Task Create Failed");
			return CISW_SYS_ERROR;
		}
	}

	return CISW_SUCCESS;
}


/************************************************************************************
 *
 * Name:        ciClientStubDeinit
 *
 * Description: Delete client stub task and message queue and reset static parameters
 *
 * Parameters:  None
 *
 * Returns:     None
 *
 * Notes:
 *
 **********************************************************************************/

void ciClientStubDeinit(void)
{

	CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT_TASK, __LINE__, CCI_TRACE_API, "ciClientStubDeinit:");

	OSATaskDelete(ciClientStubTaskRef);
	OSAMsgQDelete(ciClientStubMsgQ);

	/* Reset and clear the task and queue references */
	ciClientStubTaskRef = NULL;
	ciClientStubMsgQ    = NULL;

	/* reset the client stub initialization status */
	gCiInitStatus = CI_STUB_INIT_NOT_STARTED;

	close(cistubsockfd);

	OSASemaphoreDelete(semaRef);
	OSASemaphoreDelete(initTaskRef);
}

/* process message received from ci stub server */
static void ciStubEventHandler (UINT8*  rxmsg)
{
	ShmApiMsg                          *pCiStubClientMsg;
	CiShConfirmArgs *pShConfirmArgs;
	CiConfirmArgs  *pConfirmArgs;
	CiIndicateArgs *pIndicateArgs;
	OS_STATUS status;
	int isFirstTime;

	pCiStubClientMsg = (ShmApiMsg *)rxmsg;

	// CCI_TRACE2(CI_CLIENT_STUB, CI_CLIENT_TASK, __LINE__, CCI_TRACE_INFO," ciStubEventHandler, msgid=%d, svcId=%d",pCiStubClientMsg->procId,pCiStubClientMsg->svcId);
	CCI_ASSERT(pCiStubClientMsg->svcId == CiStubSvc_Id);
	switch (pCiStubClientMsg->procId)
	{
	case CiStubConfirmStartProcId:
		gCiInitStatus = CI_STUB_INIT_COMPLETE;
		break;

	case CiShConfirmCbId:
		pShConfirmArgs = (CiShConfirmArgs*)(rxmsg + SHM_HEADER_SIZE);
		//route the args to user registered call back
		clientCiShConfirmCallback(pShConfirmArgs, rxmsg + SHM_HEADER_SIZE + sizeof(CiShConfirmArgs));
		break;
	case CiConfirmDefCbId:
		pConfirmArgs = (CiConfirmArgs*)(rxmsg + SHM_HEADER_SIZE);
		clientCiDefConfirmCallback(pConfirmArgs, rxmsg + SHM_HEADER_SIZE + sizeof(CiConfirmArgs));
		break;
	case CiConfirmCbId:
		pthread_mutex_lock(&gTimeoutLock);
		gTimeoutNum = 0;
		pthread_mutex_unlock(&gTimeoutLock);
		pConfirmArgs = (CiConfirmArgs*)(rxmsg + SHM_HEADER_SIZE);
		clientCiConfirmCallback(pConfirmArgs, rxmsg + SHM_HEADER_SIZE + sizeof(CiConfirmArgs));
		break;
	case CiIndicateCbId:
		pIndicateArgs = (CiIndicateArgs *)(rxmsg + SHM_HEADER_SIZE);
		clientCiIndicateCallback(pIndicateArgs, rxmsg + SHM_HEADER_SIZE + sizeof(CiIndicateArgs));
		break;
	case CiIndicateDefCbId:
		pIndicateArgs = (CiIndicateArgs *)(rxmsg + SHM_HEADER_SIZE);
		clientCiDefIndicateCallback(pIndicateArgs, rxmsg + SHM_HEADER_SIZE + sizeof(CiIndicateArgs));
		break;
	case MsocketLinkdownProcId:

		CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT_TASK, __LINE__, CCI_TRACE_INFO, "Recv MsocketLinkdownProcId!");
		gDlLinkDownFlag = TRUE;
		gCiInitStatus = CI_STUB_INIT_NOT_STARTED;
		initTaskStop = 1;
		OSASemaphoreAcquire(initTaskRef,OSA_SUSPEND);
		gStatusIndCb(CISW_INTERLINK_DOWN);
		ioctl(cidata_fd, CCIDATASTUB_LINKSTATUS, &gDlLinkDownFlag);
		clearCciActionlist();
		OSASemaphoreRelease(initTaskRef);
		break;
	case MsocketLinkupProcId:
		pthread_mutex_lock(&gTimeoutLock);
		gTimeoutNum = 0;
		pthread_mutex_unlock(&gTimeoutLock);
		CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT_TASK, __LINE__, CCI_TRACE_INFO, "Recv MsocketLinkupProcId!");
		//Start ciInitTask to makehandshaking with CP
		if (ciInitTaskRef)
			OSATaskDelete(ciInitTaskRef);
		isFirstTime = 0;
		OSASemaphoreAcquire(initTaskRef, OSA_SUSPEND);
		initTaskStop = 0;
		status = OSATaskCreate(&ciInitTaskRef,
				       (void *)ciInitTaskStack,
				       CI_STUB_STACK_SIZE,
				       CI_STUB_TASK_PRIORITY,
				       (CHAR*)"ciInitTask",
				       ciInitTask,
				       (void *)isFirstTime);

		if (status != OS_SUCCESS)
		{
			CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT_TASK, __LINE__, CCI_TRACE_ERROR, " ciStubEventHandler: CCI Stub OSA Task Create Failed");
		}

		break;

	default:
		CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT_TASK, __LINE__, CCI_TRACE_ERROR, " Recved: Unexpected message");
		CCI_ASSERT(0); //TBD: Unexpected message
		break;
	} // end switch
}

/************************************************************************************
 *
 * Name:        ciClientStubTask
 *
 * Description: Client stub task function
 *
 * Parameters:
 *
 * Returns:     None
 *
 * Notes:
 *
 **********************************************************************************/
UINT8 ciStubRecvMsg[MAX_CI_STUB_RXMSG_LEN];
void ciClientStubTask(void *input)
{
	UNUSEDPARAM(input)

	// UINT8        ciStubRecvMsg[MAX_CI_STUB_RXMSG_LEN];
	int ret;

	CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT_TASK, __LINE__, CCI_TRACE_API, " Running CiStubClientTask");

	while (TRUE)
	{
		/* wating CI client event */
		ret = mrecv(cistubsockfd, ciStubRecvMsg, MAX_CI_STUB_RXMSG_LEN, 0);
		if (ret <= 0)
			continue;

		CCI_ASSERT(ret > 0);

		ciStubEventHandler(ciStubRecvMsg);
	} /* end while */
}


