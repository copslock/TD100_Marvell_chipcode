/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (?Material? are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel?s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: telatci.c
 *
 *  Authors:  Vincent Yeung
 *
 *  Description: Telephony AT2CI utilities functions
 *
 *  Notes:
 *
 ******************************************************************************/

#include "ci_api_types.h"
#include "ci_api.h"
#include "teldef.h"
#include "telconfig.h"
#include "telatparamdef.h"
#include "telcontroller.h"
#include "teldev.h"     //dev service definition.
#include "teldat.h"     // dat service definition.

/********************************************************************
**  Defines
********************************************************************/
#if 1 || defined (INTEL_2CHIP_PLAT_BVD)
#include "ci_cc.h"
#include "ci_ps.h"
#include "ci_dev.h"
#include "ci_mm.h"
#include "ci_dat.h"
#include "ci_ss.h"
#include "ci_msg.h"
#include "ci_sim.h"
#include "ci_pb.h"
//#include "ci_stub.h"
#include "ci_stub_ttc.h"

#include "ci_syswrap.h"

#include "cc_api.h"
#include "msg_api.h"
#include "ss_api.h"
#include "pb_api.h"
#include "mm_api.h"
#include "ps_api.h"
#include "teldat.h"
#include "sim_api.h"
#include "dev_api.h"
#include "eeh_ioctl.h"

#include "osa.h"
#include <common_datastub.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#define Sleep(X)        OSATaskSleep(X)

#define ATCI_SAC_SG_HANDLE_BASE     (0x10000000L)
#define ATCI_SAC_SH_HANDLE_BASE     (0x01000000L)




#define CLIENT(opShHandle)          (CiShHandle)(opShHandle + ATCI_SAC_SH_HANDLE_BASE)
#define CLIENTID                    1


#define CI_SG_ID_DFLT               0

#define OPSGCNFHANDLE_OFFSET        10
#define OPSGFREEREQHANDLE_OFFSET    20
#define OPSGFREERSPHANDLE_OFFSET    30
#define OPSGINDHANDLE_OFFSET        40

#define DIAG_UART_BSPFLAG           2

/*********************************************************************
**  Global Variables
*********************************************************************/
CiShHandle gAtciShellHandle = 0;                                        /* shell handle after registration */
CiShHandle gAtciShReqHandle = 0;                                        /* shell request handle */

CiServiceHandle gAtciSvgHandle[CI_SG_NUMIDS + 1];

/* Mem Free function pointer for CI Server Stub */
static CiSgOpaqueHandle gAtciSvgFreeCnfMemHandle[CI_SG_NUMIDS + 1];
static CiSgOpaqueHandle gAtciSvgFreeIndMemHandle[CI_SG_NUMIDS + 1];
static CiSgFreeCnfMem gAtciSvgFreeCnfMemFuncTable[CI_SG_NUMIDS + 1];
static CiSgFreeIndMem gAtciSvgFreeIndMemFuncTable[CI_SG_NUMIDS + 1];

static CiShFreeCnfMem gAtciShFreeCnfMemFunc;
static CiShOpaqueHandle gAtciShFreeCnfMemHandle;


extern int cidata_fd;

extern TelAtpCtrl gAtpCtrl[];
extern utlAtParser_P aParser_p[];

UINT32 gServiceGroupRegistered = CI_SG_ID_CC;

extern UINT8 g_uintErrorCodePresentationMode;
extern int get_bspflag_from_kernel_cmdline(void);


/**********************************************************************
**  External Variables
**********************************************************************/

/**********************************************************************
**  Function Prototypes
***********************************************************************/
void initATCIContext(void);

static void initATCI(void);

static void ciRegisterATCIClientApp(void);

static void atciShConfirmFunc( CiShOpaqueHandle opShHandle, CiShOper oper,
			       void* cnfParas, CiShRequestHandle opHandle );

static void atciDfltCnfFunc(CiSgOpaqueHandle opSgOpaqueHandle,
			    CiServiceGroupID svgId,
			    CiPrimitiveID primId,
			    CiIndicationHandle indHandle,
			    void*               pParam);

static void atciDfltIndFunc(CiSgOpaqueHandle opSgOpaqueHandle,
			    CiServiceGroupID svgId,
			    CiPrimitiveID primId,
			    CiIndicationHandle indHandle,
			    void*               pParam);

static void atciDfltCnfIndFunc(CiSgOpaqueHandle opSgOpaqueHandle,
			       CiServiceGroupID svgId,
			       CiPrimitiveID primId,
			       CiIndicationHandle indHandle,
			       void*               pParam);

static void atciRegDefSvg(void);

static void atciRegTheSvg( CiServiceGroupID svgId );

static void dfltCnf(CiSgOpaqueHandle opSgCnfHandle,
		    CiServiceGroupID svgId,
		    CiPrimitiveID primId,
		    CiRequestHandle reqHandle,
		    void*            paras);



static void dfltInd(CiSgOpaqueHandle opSgIndHandle,
		    CiServiceGroupID svgId,
		    CiPrimitiveID primId,
		    CiIndicationHandle indHandle,
		    void*               paras);




/**********************************************************
**  IMPLEMENTATIONS
***********************************************************/



static void (*atciConfirmFunc[])(CiSgOpaqueHandle opSgCnfHandle,
				 CiServiceGroupID id,
				 CiPrimitiveID primId,
				 CiRequestHandle reqHandle,
				 void*            paras) =
{
	NULL,
	ccCnf,          /* CC Confirmation function */
	ssCnf,          /* SS Confirmation function */
	mmCnf,          /* MM Confirmation function */
	pbCnf,          /* PB Confirmation function */
	simCnf,         /* SIM Confirmation function */
	msgCnf,         /* Msg Confirmation function */
	psCnf,          /* PS Confirmation function */
	datCnf,         /* DAT Confirmation function */
	devCnf,         /* DEV Confirmation function */
	dfltCnf,        /* HSCSD Confirmation function */
	dfltCnf,        /* DEB Confirmation function */
	dfltCnf,        /* ATPI Confirmation function */
	dfltCnf,        /* PL Confirmation function */
	dfltCnf         /* OAM Confirmation function */
};


static void (*atciIndFunc[])(CiSgOpaqueHandle opSgIndHandle,
			     CiServiceGroupID id,
			     CiPrimitiveID primId,
			     CiIndicationHandle indHandle,
			     void*               paras) =
{
	NULL,
	ccInd,          /* CC Indications function */
	ssInd,          /* SS Indications function */
	mmInd,          /* MM Indications function */
	pbInd,          /* PB Indications function */
	simInd,         /* SIM Indications function */
	msgInd,         /* MSG Indications function */
	psInd,          /* PS Indications function */
	datInd,         /* DAT Indications function */
	devInd,         /* DEV Indications function */
	dfltInd,        /* HSCSD Indications function */
	dfltInd,        /* DEB Indications function */
	dfltInd,        /* ATPI Indications function */
	dfltInd,        /* PL Indications function */
	dfltInd         /* OAM Indications function */
};


/*******************************************************************
 */
static BOOL handlerDFLT (CiPrimitiveID primId, void*  pParam)
{
	UNUSEDPARAM(primId)
	UNUSEDPARAM(pParam)
	static UINT32 sacAppDFLTcntr = 0;

	F_ENTER();

	sacAppDFLTcntr++;
	/*DevParam(0, primId, pParam);*/
	return TRUE;
}


/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

/******************************************************************
 *  ciRegisterClientApp - client registration
 *
 */
static void ciRegisterClientApp(CiServiceGroupID dummy)    //YANM
{
	UNUSEDPARAM(dummy)

	static BOOL atciInit = FALSE;

	if ( atciInit == TRUE )
	{
		ERRMSG( "ATCI is re-initialized.\r\n");
	}
	else
	{
		atciInit = TRUE;
	}

	/* init the ATCI */
	initATCI();

	/* register the ATCI as a client of SAC */
	ciRegisterATCIClientApp();
	//}
}

/**************************************************
 *  F@: initATCI - GLOBAL API for GCF AT-command
 *
 */
static void initATCI (void)
{
	CiServiceGroupID svgId;

	gAtciShellHandle = ATCI_SAC_SH_INVALID_HANDLE;
	gAtciShReqHandle = 0;
	gAtciShFreeCnfMemFunc = NULL;
	gAtciShFreeCnfMemHandle = ATCI_SAC_SH_INVALID_HANDLE;

	/*  initialize current registration status for each service groups.	*/
	for ( svgId = 0; svgId <= CI_SG_NUMIDS; svgId++ )
	{
		gAtciSvgHandle[svgId] = ATCI_SAC_SG_INVALID_HANDLE;
		gAtciSvgFreeCnfMemHandle[svgId] = ATCI_SAC_SG_INVALID_HANDLE;
		gAtciSvgFreeIndMemHandle[svgId] = ATCI_SAC_SG_INVALID_HANDLE;
		gAtciSvgFreeCnfMemFuncTable[svgId] = NULL;
		gAtciSvgFreeIndMemFuncTable[svgId] = NULL;
	}

	/* init the ATCI context variables */
	initATCIContext();
}

/**************************************************
 *  F@: initATCIContext - GLOBAL API for GCF AT-command
 *
 */
void initATCIContext (void)
{
	initCCContext();

	initPSContext();
}


/******************************************************************
 *  ciRegisterATCIClientApp - shell registration
 *
 */
static void ciRegisterATCIClientApp(void)
{
	CiReturnCode rc;

	/*
	**  Perform Shell Registration for this ATCI client.
	*/

	rc = ciShRegisterReq( atciShConfirmFunc,        /* CI Shell Confirm function */
			      CLIENT(CLIENTID),         /* Opaque Shell Handle */
			      NULL,                     /* CI Shell Free Request Memory function */
			      CLIENT(CLIENTID) );       /* Opaque Free Shell Handle */
	/*
	**  Check the return code to see if the shell was registered.
	*/
	switch ( rc )
	{
	case CIRC_SH_NULL_CONFIRM_CB:
	case CIRC_SH_NULL_FREEREQMEM_CB:
	case CIRC_SUCCESS:
		break;
	case CIRC_INTERLINK_FAIL:
		ERRMSG( "ERROR - Shell Registration: CIRC_INTERLINK_FAIL\r\n");
		return;
		break;
	default:
		ERRMSG( "ERROR - Shell Registration: unknown\r\n");
		return;
		break;
	}

}

/************************************************************************************
 * Function: atciDfltCnfFunc
 *
 * Parameters:
 *
 * Returned value:
 *
 * Description:    Default Confirmation function
 */
static void atciDfltCnfFunc (CiSgOpaqueHandle opSgOpaqueHandle,
			     CiServiceGroupID svgId,
			     CiPrimitiveID primId,
			     CiIndicationHandle indHandle,
			     void*               pParam)
{
	atciDfltCnfIndFunc( opSgOpaqueHandle, svgId, primId, indHandle, pParam);

	/* free up the confirmation memory */
	atciSvgFreeCnfMem( svgId, primId, pParam );

	return;

}

/************************************************************************************
 * Function: atciDfltIndFunc
 *
 * Parameters:
 *
 * Returned value:
 *
 * Description:    Default Confirmation function
 */
static void atciDfltIndFunc (CiSgOpaqueHandle opSgOpaqueHandle,
			     CiServiceGroupID svgId,
			     CiPrimitiveID primId,
			     CiIndicationHandle indHandle,
			     void*               pParam)
{
	atciDfltCnfIndFunc( opSgOpaqueHandle, svgId, primId, indHandle, pParam);

	/* free up the indication memory */
	atciSvgFreeIndMem( svgId, primId, pParam );

	return;

}

/************************************************************************************
 * Function: atciDfltCnfIndFunc
 *
 * Parameters:
 *
 * Returned value:
 *
 * Description:    Default Confirmation/Indication function
 */
static void atciDfltCnfIndFunc (CiSgOpaqueHandle opSgOpaqueHandle,
				CiServiceGroupID svgId,
				CiPrimitiveID primId,
				CiIndicationHandle indHandle,
				void*               pParam)
{
	UNUSEDPARAM(opSgOpaqueHandle)
	UNUSEDPARAM(indHandle)
	BOOL handled;
//	ERRMSG("atciDfltCnfIndFunc: svgId %d.\n", svgId);
	switch (svgId)
	{
	case CI_SG_ID_DFLT:
	case CI_SG_ID_CC:
	case CI_SG_ID_SS:
	case CI_SG_ID_MM:
	case CI_SG_ID_PB:
	case CI_SG_ID_SIM:
	case CI_SG_ID_MSG:
	case CI_SG_ID_PS:
	case CI_SG_ID_DAT:
	case CI_SG_ID_DEV:
		handled = handlerDFLT(primId, pParam);
		break;
	default:
		ERRMSG( "ERROR - indication for invalid service group.\r\n");
		break;
	}
}

/************************************************************************************
 * Function: atciShConfirmFunc
 *
 * Parameters:
 *
 * Returned value:
 *
 * Description:    Confirmation to a Shell registration request
 */
static void atciShConfirmFunc( CiShOpaqueHandle opShHandle, CiShOper oper,
			       void* cnfParas, CiShRequestHandle opHandle )
{
	UNUSEDPARAM(opShHandle)
	UNUSEDPARAM(opHandle)
	F_ENTER();

	DBGMSG("atciShConfirmFunc: oper=%d\n", oper);
	switch (oper)
	{
	case CI_SH_OPER_REGISTERSH:
	{
		CiShOperRegisterShCnf* pShCnf = (CiShOperRegisterShCnf *)cnfParas;

		if (pShCnf->rc == CIRC_SUCCESS)
		{
			/* save the shell handle & free memory functions */
			gAtciShellHandle = pShCnf->handle;

			/* request to register all service groups */
			//	atciReqSvgReg();
			atciRegDefSvg();
		}

		break;
	}
	case CI_SH_OPER_DEREGISTERSH:
	{
		break;
	}
	case CI_SH_OPER_GETVERSION:
	{
		break;
	}
	case CI_SH_OPER_REGISTERSG:
	{
		CiShOperRegisterSgCnf* pShCnf = (CiShOperRegisterSgCnf *)cnfParas;
		CiServiceGroupID svgId = pShCnf->id;



		if (pShCnf->rc == CIRC_SUCCESS)
		{

			gAtciSvgHandle[svgId] = pShCnf->handle;
			CiSetSvgGroupHandle(svgId, pShCnf->handle);
			if (svgId == CI_SG_ID_DAT)
				ioctl(cidata_fd, CCIDATASTUB_DATASVGHANDLE, gAtciSvgHandle[svgId]);
			DPRINTF("======Svg %d registered======\n\n", svgId);

		}
		else
		{
			DPRINTF("Svg %d not implemented.\n\n", svgId);
		}
			#if 1
		if (svgId < CI_SG_NUMIDS)
			atciRegTheSvg(++svgId);
			#endif
		gServiceGroupRegistered++;
		/* Check whether all the service group are registered */
		if (gServiceGroupRegistered > CI_SG_NUMIDS)
		{
			FILE *fd;
			fd = fopen("/tmp/atcmdsrv_ok", "w");
			if (fd)
				fclose(fd);

			{
				int i;
				UINT32 atHandle;

				/* Start to get CP and MSA revision ID */
				ciRequest(gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_GET_REVISION_ID_REQ, IND_REQ_HANDLE, NULL);
				
				/* Notify all the TEs that we are ready */
				for (i = 0; i < NUM_OF_TEL_ATP; i++)
				{
					if (gAtpCtrl[i].bEnable)
					{
						atHandle = AT_RESV_HANDLE(i, 2);
						ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
					}
				}
				/* if android.bsp=2 in kernel cmdline, it means diag over uart is used, CFUN=0 */
				if (DIAG_UART_BSPFLAG !=  get_bspflag_from_kernel_cmdline())
				{
					/* Currently CP is power off by default: in test mode, we should use CFUN=1 to power on CP */
					if (bLocalTest)
					{
						DEV_SetFunc(IND_REQ_HANDLE, 1, 0, FALSE, 0, 0);
					}
				}
			}

		}
		break;
	}
	case CI_SH_OPER_DEREGISTERSG:
	{
		break;
	}
	case CI_SH_OPER_REGISTERDEFSG:
	{
		CiShOperRegisterDefaultSgCnf* pShCnf = (CiShOperRegisterDefaultSgCnf *)cnfParas;

		if (pShCnf->rc == CIRC_SUCCESS)
		{
			gAtciSvgHandle[CI_SG_ID_DFLT] = pShCnf->defHandle;



			atciRegTheSvg(CI_SG_ID_CC);

		}
		break;
	}
	case CI_SH_OPER_DEREGISTERDEFSG:
	{
		break;
	}
	case CI_SH_QUERYEXTENDED:
	{
		break;
	}
	case CI_SH_QUERYEXTENDEDSERVICEPRIM:
	{
		break;
	}
	case CI_SH_QUERYEXTENDEDSERVICEGROUP:
	{
		break;
	}
	default:
	{
		break;
	}
	}

	/* free up the shell confirmation memory */
//	atciShFreeCnfMem( oper, cnfParas );

	F_LEAVE();

	return;
}
#if 0
/******************************************************************
 *  atciSvgRegister - service group registration
 *
 */
static void atciReqSvgReg(void)
{
	CiServiceGroupID svgId;

	F_ENTER();

	ERRMSG("atciReqSvgReg\n");

	atciRegDefSvg();

	/*  Register the CI client for each of the existing service groups.	*/
	for ( svgId = CI_SG_ID_CC; svgId <= CI_SG_NUMIDS; svgId++ )
	{
		atciRegTheSvg(svgId);
	}
	F_LEAVE();
}
#endif
/******************************************************************
 *  atciRegDefSvg - request default service group registration
 *
 */
static void atciRegDefSvg(void)
{
	static CiShOperRegisterDefaultSgReq gDefSvgRegBuf;

	CiReturnCode rc;

	/*  Register a default service group.  */
	gDefSvgRegBuf.opSgDefCnfHandle = OPSGCNFHANDLE_OFFSET;
	gDefSvgRegBuf.opSgDefIndHandle = OPSGINDHANDLE_OFFSET;
	gDefSvgRegBuf.ciCnfDef         = atciDfltCnfFunc;
	gDefSvgRegBuf.ciIndDef         = atciDfltIndFunc;

	rc = ciShRequest( gAtciShellHandle,             /* Shell Handle */
			  CI_SH_OPER_REGISTERDEFSG,     /* Operation */
			  &gDefSvgRegBuf,               /* parameters */
			  gAtciShReqHandle++ );         /* Opaque handle */

	Sleep(500);
//	Sleep(2000);
	/* Check the return code to see if the default service group was registered.*/
//	ERRMSG("atciRegDefSvg: rc=%d\n", rc);
	switch ( rc )
	{
	case CIRC_SUCCESS:
		DBGMSG("%s: CIRC_SUCCESS\r\n", __FUNCTION__);
		break;
	case CIRC_SH_INVALID_HANDLE:
		ERRMSG( "ERROR: CIRC_SH_INVALID_HANDLE\r\n");
		break;
	case CIRC_SH_INVALID_OPER:
		ERRMSG( "ERROR: CIRC_SH_INVALID_OPER\r\n");
		break;
	case CIRC_SH_NULL_REQPARAS:
		ERRMSG( "ERROR: CIRC_SH_NULL_REQPARAS\r\n");
		break;
	case CIRC_INTERLINK_FAIL:
		ERRMSG( "ERROR: CIRC_INTERLINK_FAIL\r\n");
		break;
	default:
		ERRMSG( "ERROR: %d\r\n", rc);
		break;
	}
}
//for freeing req and rsp messages
#if 1
void ciSgFreeMem (CiSgOpaqueHandle opSgFreeHandle,
		  CiServiceGroupID id,
		  CiPrimitiveID primId,
		  void* pPara)
{

	UNUSEDPARAM(opSgFreeHandle)
	DBGMSG("ciSgFreeMem, id=%d, primId=%d\n", id, primId);
//    cimem_CiSgFreeMem(opSgFreeHandle, id, primId, pPara);

	/* Release the memory */
	if (pPara != NULL)
		free(pPara);

}
#endif

/******************************************************************
 *  atciRegTheSvg - request service group registration
 *
 */
static void atciRegTheSvg(CiServiceGroupID svgId)
{
	static CiShOperRegisterSgReq gAtciSvgRegBuf[CI_SG_NUMIDS];

	CiReturnCode rc;
	CiShOperRegisterSgReq    *pCiShOperRegisterSgReq;

	pCiShOperRegisterSgReq = &gAtciSvgRegBuf[svgId - 1];

	pCiShOperRegisterSgReq->id                = svgId;
	pCiShOperRegisterSgReq->ciConfirm         = atciConfirmFunc[svgId];
	pCiShOperRegisterSgReq->ciIndicate        = atciIndFunc[svgId];
	pCiShOperRegisterSgReq->opSgCnfHandle     = svgId + OPSGCNFHANDLE_OFFSET;
	pCiShOperRegisterSgReq->opSgIndHandle     = svgId + OPSGINDHANDLE_OFFSET;
	pCiShOperRegisterSgReq->ciSgFreeReqMem    = ciSgFreeMem; //xinli
	pCiShOperRegisterSgReq->ciSgFreeRspMem    = NULL; //ciSgFreeMem;//NULL;
	pCiShOperRegisterSgReq->opSgFreeReqHandle = (CiSgOpaqueHandle)NULL;
	pCiShOperRegisterSgReq->opSgFreeRspHandle = (CiSgOpaqueHandle)NULL;

	DBGMSG("atciRegTheSvg: svgId %d \n", svgId);
	rc = ciShRequest( gAtciShellHandle,             /* Shell Handle */
			  CI_SH_OPER_REGISTERSG,        /* Operation */
			  pCiShOperRegisterSgReq,       /* parameters */
			  gAtciShReqHandle++ );         /* Opaque handle */
	/*
	**  Check the return code to see if the specific service group was registered.
	*/
	switch ( rc )
	{
	case CIRC_SUCCESS:
		break;
	case CIRC_SH_INVALID_HANDLE:
		ERRMSG("ERROR - CIRC_SH_INVALID_HANDLE\r\n");
		break;
	case CIRC_SH_INVALID_OPER:
		ERRMSG("ERROR - CIRC_SH_INVALID_OPER\r\n");
		break;
	case CIRC_SH_NULL_REQPARAS:
		ERRMSG("ERROR - CIRC_SH_NULL_REQPARAS\r\n");
		break;
	case CIRC_INTERLINK_FAIL:
		ERRMSG("ERROR - CIRC_INTERLINK_FAIL\r\n");
		break;
	default:
		ERRMSG("ERROR - unknown\r\n");
		break;
	}
}

/******************************************************************
 *  atciSvgSetFreeMemFunc
 *
 * service group Free memory call-back that is used by Wrapper
 * to free memory allocated for the confirmation/indication primitive
 */
#if 0
static void atciSvgSetFreeMemFunc( CiServiceGroupID svgId,
				   CiSgOpaqueHandle cnfHandle,
				   CiSgOpaqueHandle indHandle,
				   CiSgFreeCnfMem ciSgFreeCnfMem,
				   CiSgFreeIndMem ciSgFreeIndMem )
{
	if (ciSgFreeCnfMem != NULL)
	{
		gAtciSvgFreeCnfMemHandle[svgId] = cnfHandle;
		gAtciSvgFreeCnfMemFuncTable[svgId] = ciSgFreeCnfMem;
	}

	if (ciSgFreeIndMem != NULL)
	{
		gAtciSvgFreeIndMemHandle[svgId] = indHandle;
		gAtciSvgFreeIndMemFuncTable[svgId] = ciSgFreeIndMem;
	}
}
#endif


/******************************************************************
 *  atciShFreeCnfMem - free shell confirmation params memory function
 *
 */
#if 0
static void atciShFreeCnfMem( CiShOper oper, void* cnfParas )
{
	if ( gAtciShFreeCnfMemFunc != NULL)
	{
		gAtciShFreeCnfMemFunc( gAtciShFreeCnfMemHandle, oper, cnfParas );
	}
}
#endif


/******************************************************************
 *  atciSvgFreeCnfMem - free confirmation params memory function
 *
 */
void atciSvgFreeCnfMem( CiServiceGroupID svgId,
			CiPrimitiveID primId, void* paras )
{
	UNUSEDPARAM(svgId)
	UNUSEDPARAM(primId)
	UNUSEDPARAM(paras)
#if 0
	if (gAtciSvgFreeCnfMemFuncTable[svgId] != NULL)
	{
		gAtciSvgFreeCnfMemFuncTable[svgId](gAtciSvgFreeCnfMemHandle[svgId], svgId, primId, paras);
	}
#endif
}


/******************************************************************
 *  atciSvgFreeIndMem - free indication params memory function
 *
 */
void atciSvgFreeIndMem( CiServiceGroupID svgId,
			CiPrimitiveID primId, void* paras )
{
	UNUSEDPARAM(svgId)
	UNUSEDPARAM(primId)
	UNUSEDPARAM(paras)
#if 0
	F_ENTER();
	if (gAtciSvgFreeIndMemFuncTable[svgId] != NULL)
	{
		gAtciSvgFreeIndMemFuncTable[svgId](gAtciSvgFreeIndMemHandle[svgId], svgId, primId, paras);
	}
	F_LEAVE();
#endif
}

/************************************************************************************
*
* Default confirmation handler
*
************************************************************************************/
static void dfltCnf(CiSgOpaqueHandle opSgCnfHandle,
		    CiServiceGroupID svgId,
		    CiPrimitiveID primId,
		    CiRequestHandle reqHandle,
		    void*             paras)
{
	UNUSEDPARAM(opSgCnfHandle)
	UNUSEDPARAM(reqHandle)

	if (svgId == CI_SG_ID_DFLT)
	{
		handlerDFLT(primId, paras);
	}

	/* free up the confirmation memory */
//	atciSvgFreeCnfMem( svgId, primId, paras );

	return;
}

/************************************************************************************
 *
 * Default CI notification handler
 *
 *************************************************************************************/

static void dfltInd(CiSgOpaqueHandle opSgOpaqueHandle,
		    CiServiceGroupID svgId,
		    CiPrimitiveID primId,
		    CiIndicationHandle indHandle,
		    void*               pParam)
{
	UNUSEDPARAM(opSgOpaqueHandle)
	UNUSEDPARAM(indHandle)

	if (svgId == CI_SG_ID_DFLT)
	{
		handlerDFLT(primId, pParam);
	}

	/* free up the indication memory */
//	atciSvgFreeIndMem( svgId, primId, pParam );

	return;
}

void CiInitCallBack( CiSysWrapStatus status )
{

	switch (status)
	{
	case CISW_INIT_SUCCESS:
		DPRINTF("CISW_INIT_SUCCESS\n");
		ciRegisterClientApp(0);
		break;
	case CISW_INIT_FAIL:
		ERRMSG("CISW_INIT_FAIL\n");
		//	ciClientStubInit((CiClientStubStatusInd)CiInitCallBack);
		break;
	case CISW_INTERLINK_DOWN:
		ERRMSG("CISW_INTERLINK_DOWN\n");
		gServiceGroupRegistered = CI_SG_ID_CC;
		/* It should release the ongoing CS call locally */
		resetAllCalls();
		resetPsParas();
		resetMmParas();
		break;
	case CISW_INTERLINK_UP:
		DPRINTF("CISW_INTERLINK_UP\n");
		resetCidList();
		resetMsgParas();
		resetPbParas();
		resetSimParas();
		resetSsParas();
		resetDevParas();
		ciRegisterClientApp(0);
		break;
	case CISW_INTERLINK_TEST_IND:
	{
		FILE *fd;
		fd = fopen("/tmp/link_test_ok", "w");
		if (fd)
			fclose(fd);
		break;
	}
	default:
		ERRMSG("Unknown status!\n");
		break;
	}
	return;
}


int handle_CMEE_code(UINT32 atHandle, UINT16 errCode){

	char cmeStr[80];
	int ret = 0;

	if(g_uintErrorCodePresentationMode == 0){
		sprintf(cmeStr, "ERROR\r\n");
		ret = utlAtCommandResponse(atHandle, utlAT_COMMAND_RESPONSE_TYPE_ABORT, cmeStr);
		return ret;
	}

	switch(errCode){
	case CME_PHONE_FAILURE: 						  //phone failure
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 0\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: phone failure\r\n");
		break;
	case CME_NO_CONNECTION: 						  //no connection to phone
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 1\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: no connection to phone\r\n");
		break;
	case CME_PHONE_ADP_LINK_RSVD:					  //phone adaptor link reserved
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 2\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: phone adaptor link reserved\r\n");
		break;
	case CME_OPERATION_NOT_ALLOWED: 				  //operation not allowed
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 3\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: operation not allowed\r\n");
		break;
	case CME_OPERATION_NOT_SUPPORTED:					  //operation not supported
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 4\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: Operation not supported\r\n");
		break;
	case CME_PH_SIM_PIN_REQUIRED:					  //PH SIM PIN required
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 5\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: PH SIM PIN required\r\n");
		break;
	case CME_PH_FSIM_PIN_REQUIRED:					  //PH-FSIM PIN required
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 6\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: PH-FSIM PIN required\r\n");
		break;
	case CME_PH_FSIM_PUK_REQUIRED:					  //PH-FSIM PUK required
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 7\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: PH-FSIM PUK required\r\n");
		break;
	case CME_NO_SIM:								 //SIM not inserted
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 10\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: SIM not inserted\r\n");
		break;
	case CME_SIM_PIN_REQUIRED:							 //SIM PIN required
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 11\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: SIM PIN required\r\n");
		break;
	case CME_SIM_PUK_REQUIRED:							 //SIM PUK required
		if(g_uintErrorCodePresentationMode == 1)
		sprintf(cmeStr, "+CME ERROR: 12\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: SIM PUK required\r\n");
		break;
	case CME_SIM_FAILURE:							 //SIM failure
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 13\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: SIM failure\r\n");
		break;
	case CME_SIM_BUSY:									 //SIM busy
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 14\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: SIM busy\r\n");
		break;
	case CME_SIM_WRONG: 								 //SIM wrong
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 15\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: SIM wrong\r\n");
		break;
	case CME_INCORRECT_PASSWD:							 //incorrect password
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 16\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: incorrect password\r\n");
		break;
	case CME_SIM_PIN2_REQUIRED: 						 //SIM PIN2 required
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 17\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: SIM PIN2 required\r\n");
		break;
	case CME_SIM_PUK2_REQUIRED: 						 //SIM PUK2 required
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 18\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: SIM PUK2 required\r\n");
		break;
	case CME_MEMORY_FULL:							 //memory full
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 20\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: memory full\r\n");
		break;
	case CME_INVALID_INDEX: 						 //invalid index
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 21\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: invalid index\r\n");
		break;
	case CME_NOT_FOUND: 								 //not found
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 22\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: not found\r\n");
		break;
	case CME_MEMORY_FAILURE:						 //memory failure
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 23\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: memory failure\r\n");
		break;
	case CME_TEXT_STRING_TOO_LONG:					 //text string too long
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 24\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: text string too long\r\n");
		break;
	case CME_INVALID_CHAR_IN_STRING:				 //invalid characters in text string
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 25\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: invalid characters in text string\r\n");
		break;
	case CME_DAIL_STRING_TOO_LONG:					 //dial string too long
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 26\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: dial string too long\r\n");
		break;
	case CME_INVALID_CHAR_IN_DIAL_STRING:			 //invalid characters in dial string
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 27\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: invalid characters in dial string\r\n");
		break;
	case CME_NO_NW_SERVICE: 						 //no network service
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 30\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: no network service\r\n");
		break;
	case CME_NW_TIMEOUT:								 //network timeout
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 31\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: network timeout\r\n");
		break;
	case CME_NW_NOT_ALLOWED:						 //network not allowed - emergency calls only
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 32\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: network not allowed - emergency calls only\r\n");
		break;
	case CME_NW_PIN_REQUIRED:							 //network personalization PIN required
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 40\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: network personalization PIN required\r\n");
		break;
	case CME_NW_PUK_REQUIRED:							 //network personalization PUK required
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 41\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: network personalization PUK required\r\n");
		break;
	case CME_NW_SUB_PIN_REQUIRED:					 //network subset personalization PIN required
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 42\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: network subset personalization PIN required\r\n");
		break;
	case CME_NW_SUB_PUK_REQUIRED:					 //network subset personalization PUK required
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 43\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: network subset personalization PUK required\r\n");
		break;
	case CME_SP_PIN_REQUIRED:							 //service provider personalization PIN required
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 44\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: service provider personalization PIN required\r\n");
		break;
	case CME_SP_PUK_REQUIRED:							 //service provider personalization PUK required
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 45\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: service provider personalization PUK required\r\n");
		break;
	case CME_CP_PIN_REQUIRED:							 //corporate personalization PIN required
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 46\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: corporate personalization PIN required\r\n");
		break;
	case CME_CP_PUK_REQUIRED:							 //corporate personalization PUK required
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 47\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: corporate personalization PUK required\r\n");
		break;
	case CME_HD_KEY_REQUIRED:							 //hidden key required
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 48\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: hidden key required\r\n");
		break;
	case CME_INVALID_PARAM: 				   //Invalid Param
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 50\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: Incorrect parameters\r\n");
		break;
	case CME_ILLEGAL_MS:								//Illegal MS (#3)
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 103\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: Illegal MS (#3)\r\n");
		break;
	case CME_ILLEGAL_ME:								//Illegal ME (#6)
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 106\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: Illegal ME (#6)\r\n");
		break;
	case CME_GPRS_NOT_ALLOWED:							//GPRS services not allowed (#7)
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 107\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: GPRS services not allowed (#7)\r\n");
		break;
	case CME_PLMN_NOT_ALLOWED:							//PLMN not allowed (#11)
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 111\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: PLMN not allowed (#11)\r\n");
		break;
	case CME_LA_NOT_ALLOWED:						//Location area not allowed (#12)
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 112\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
					   sprintf(cmeStr, "+CME ERROR: Location area not allowed (#12)\r\n");
		break;
	case CME_ROAMING_NOT_ALLOWED:					//Roaming not allowed in this location area (#13)
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 113\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: Roaming not allowed in this location area (#13)\r\n");
		break;
	case CME_SERVICE_OP_NOT_SUPPORTED:					//service option not supported (#32)
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 132\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: service option not supported (#32)\r\n");
		break;
	case CME_SERVICE_OP_NOT_SUBSCRIBED: 				//requested service option not subscribed (#33)
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 133\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: requested service option not subscribed (#33)\r\n");
		break;
	case CME_SERVICE_OP_OUT_OF_ORDER:					//service option temporarily out of order (#34)
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 134\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: service option temporarily out of order (#34)\r\n");
		break;
	case CME_PDP_AUTH_FAILURE:							//PDP authentication failure
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 149\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: PDP authentication failure\r\n");
		break;
	case CME_INVALID_MOBILE_CLASS:					//invalid mobile class
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 150\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: invalid mobile class\r\n");
		break;
	case CME_UNSPECIFIED_GPRS_ERR:					 //unspecified GPRS error
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 148\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: unspecified GPRS error\r\n");
		break;
	case CME_COMMMAND_TIMEOUT_ERR:					 //AT command timeout error
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 151\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: AT command timeout\r\n");
		break;
#ifdef AT_LABTOOL_CMNDS
	case CME_WIFI_RETURN_ERROR:							// WIFI labtool return error
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: %d\r\n", CME_WIFI_RETURN_ERROR);
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: Wi-Fi labtool return error\r\n");
		break;
	case CME_BT_RETURN_ERROR:							// BT labtool return error
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: %d\r\n", CME_BT_RETURN_ERROR);
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: BT labtool return error\r\n");
		break;
	case CME_FM_RETURN_ERROR:							// FM labtool return error
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: %d\r\n", CME_FM_RETURN_ERROR);
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: FM labtool return error\r\n");
		break;
#endif
#ifdef AT_PRODUCTION_CMNDS
	case CME_FILE_EXIST_ERROR:                        //MRD file already exist error
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: %d\r\n", CME_FILE_EXIST_ERROR);
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: file already exist error\r\n");
		break;
	case CME_FILE_WITH_SAME_VERSION_EXIST_ERROR:      //MRD file with same version already exist error
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: %d\r\n", CME_FILE_WITH_SAME_VERSION_EXIST_ERROR);
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: file with same version already exist error\r\n");
		break;
	case CME_FILE_WITH_NEWER_VERSION_EXIST_ERROR:     //MRD file with newer version already exist error
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: %d\r\n", CME_FILE_WITH_NEWER_VERSION_EXIST_ERROR);
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: file with newer version already exist error\r\n");
		break;
#endif
	case CME_UNKNOWN:									//unknown error
	default:
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CME ERROR: 100\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CME ERROR: unknown error\r\n");
		break;
	}

	ret = utlAtCommandResponse(atHandle, utlAT_COMMAND_RESPONSE_TYPE_ABORT, cmeStr);
	return ret;
}

int handle_CMSE_code(UINT32 atHandle, UINT16 errCode){

	char cmeStr[80];
	int ret = 0;

	if(g_uintErrorCodePresentationMode == 0){
		sprintf(cmeStr, "ERROR\r\n");
		ret = utlAtCommandResponse(atHandle, utlAT_COMMAND_RESPONSE_TYPE_ABORT, cmeStr);
		return ret;
	}

	switch(errCode){
	case CMS_ME_FAILURE:                                                                    //ME failure
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CMS ERROR: 300\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CMS ERROR: ME failure\r\n");
		break;
	case CMS_SMS_SERVICE_RESV:                                                              //SMS service of ME reserved
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CMS ERROR: 301\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CMS ERROR: SMS service of ME reserved\r\n");
		break;
	case CMS_OPERATION_NOT_ALLOWED:                                                 //operation not allowed
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CMS ERROR: 302\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CMS ERROR: operation not allowed\r\n");
		break;
	case CMS_OPERATION_NOT_SUPPORTED:                                               //operation not supported
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CMS ERROR: 303\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CMS ERROR: operation not supported\r\n");
		break;
	case CMS_INVALID_PDU_MODE_PARA:                                                 //invalid PDU mode parameter
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CMS ERROR: 304\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CMS ERROR: invalid PDU mode parameter\r\n");
		break;
	case CMS_INVALID_TEXT_MODE_PARA:                                                //invalid text mode parameter
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CMS ERROR: 305\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CMS ERROR: invalid text mode parameter\r\n");
		break;
	case CMS_NO_SIM:                                                                                //(U)SIM not inserted
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CMS ERROR: 310\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CMS ERROR: (U)SIM not inserted\r\n");
		break;
	case CMS_SIM_PIN_REQUIRED:                                                              //(U)SIM PIN required
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CMS ERROR: 311\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CMS ERROR: (U)SIM PIN required\r\n");
		break;
	case CMS_PH_SIM_PIN_REQUIRED:                                                   //PH-(U)SIM PIN required
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CMS ERROR: 312\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CMS ERROR: PH-(U)SIM PIN required\r\n");
		break;
	case CMS_SIM_FAILURE:                                                                   //(U)SIM failure
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CMS ERROR: 313\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CMS ERROR: (U)SIM failure\r\n");
		break;
	case CMS_SIM_BUSY:                                                                              //(U)SIM busy
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CMS ERROR: 314\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CMS ERROR: (U)SIM busy\r\n");
		break;
	case CMS_SIM_WRONG:                                                                     //(U)SIM wrong
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CMS ERROR: 315\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CMS ERROR: (U)SIM wrong\r\n");
		break;
	case CMS_SIM_PUK_REQUIRED:                                                              //(U)SIM PUK required
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CMS ERROR: 316\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CMS ERROR: (U)SIM PUK required\r\n");
		break;
	case CMS_SIM_PIN2_REQUIRED:                                                     //(U)SIM PIN2 required
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CMS ERROR: 317\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CMS ERROR: (U)SIM PIN2 required\r\n");
		break;
	case CMS_SIM_PUK2_REQUIRED:                                                     //(U)SIM PUK2 required
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CMS ERROR: 318\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CMS ERROR: (U)SIM PUK2 required\r\n");
		break;
	case CMS_MEMORY_FAILURE:                                                                //memory failure
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CMS ERROR: 320\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CMS ERROR: memory failure\r\n");
		break;
	case CMS_INVALID_MEMORY_INDEX:                                                  //invalid memory index
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CMS ERROR: 321\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CMS ERROR: invalid memory index\r\n");
		break;
	case CMS_MEMORY_FULL:                                                                   //memory full
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CMS ERROR: 322\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CMS ERROR: memory full\r\n");
		break;
	case CMS_SMSC_ADDR_UNKNOWN:                                                     //SMSC address unknown
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CMS ERROR: 330\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CMS ERROR: SMSC address unknown\r\n");
		break;
	case CMS_NO_NW_SERVICE:                                                                 //no network service
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CMS ERROR: 331\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CMS ERROR: no network service\r\n");
		break;
	case CMS_NW_TIMEOUT:                                                                    //network timeout
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CMS ERROR: 332\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CMS ERROR: network timeout\r\n");
		break;
	case CMS_NO_CNMA_ACK_EXPECTED:                                                  //no +CNMA acknowledgement expected
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CMS ERROR: 340\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CMS ERROR: no +CNMA acknowledgement expected\r\n");
		break;
	case CMS_UNKNOWN_ERROR:                                                                 //unknown error
	default:
		if(g_uintErrorCodePresentationMode == 1)
			sprintf(cmeStr, "+CMS ERROR: 500\r\n");
		else if(g_uintErrorCodePresentationMode == 2)
			sprintf(cmeStr, "+CMS ERROR: unknown error\r\n");
		break;
	}

	ret = utlAtCommandResponse(atHandle, utlAT_COMMAND_RESPONSE_TYPE_ABORT, cmeStr);
	return ret;
}

/*****************************************************************************************
 *  FUNCTION: atRespStr - return the result code and response to the terminal
 *
 * NOTES:
 *	1. If the resultCode is ATCI_RESULT_CODE_NULL and respString is not NULL, only return respString to terminal
 *	2. If the resultCode is not ATCI_RESULT_CODE_NULL and respString is NULL, only return resultCode to terminal
 *	3. If the resultCode is not ATCI_RESULT_CODE_NULL and respString is not NULL, first return respString to terminal
 *		then return the result code to terminal
 *	4. Only when resultCode is ATCI_RESULT_CODE_CME_ERROR,  the errCode is valid
 *
 * RETURNS:
 *	0 is successful, not 0 for error
 ******************************************************************************************/

int atRespStr(UINT32 reqHandle, UINT8 resultCode, UINT16 errCode, char *respString)
{
	UINT32 atHandle = GET_AT_HANDLE(reqHandle);
	TelAtParserID sAtpIndex;
	int i, ret = 0;

	DBGMSG("%s: athandle = %d, resultCode = %d, errCode = %d, respString = %s\n",
	       __FUNCTION__, atHandle, resultCode, errCode, respString);

	sAtpIndex = GET_ATP_INDEX(atHandle);
	if(sAtpIndex >= NUM_OF_TEL_ATP)
	{
		ERRMSG("%s: invalid atHandle %d. \n", __FUNCTION__, atHandle);
		return -1;
	}

	/* send the response */
	if (respString != NULL)
	{
		if (strlen(respString) != (size_t)0)
		{
			char strbuf[1024*2];
			//Append "\r\n" to the string tail if it doesn't include
			if ((respString[strlen(respString) - 2] != '\r') || (respString[strlen(respString) - 1] != '\n'))
				sprintf(strbuf, "\r\n%s\r\n", respString);
			else
				sprintf(strbuf, "\r\n%s", respString);

			switch ( atHandle )
			{
			/* Indication, send to every port */
			case IND_REQ_HANDLE:
				/* Send ind msg only to IND_MSG_CHANNEL in ril */
				for ( i = RIL_IND_MSG_CHANNEL; i < NUM_OF_TEL_ATP; i++)
				{
					if (!gAtpCtrl[i].bEnable)
					{
						/* do nothing */
					}
					else
					{
						if(aParser_p[gAtpCtrl[i].index]->parameters.suppress_results == false)
							tcWriteParser(i, strbuf, strlen(strbuf));
					}
				}
				break;
			/* send the response to the specific port */
			default:
				sAtpIndex = GET_ATP_INDEX(atHandle);
				if(sAtpIndex < NUM_OF_TEL_ATP && 
					aParser_p[gAtpCtrl[sAtpIndex].index]->parameters.suppress_results == false)
					tcWriteParser(sAtpIndex, strbuf, strlen(strbuf));
				break;
			}
		}
	}

	switch (resultCode)
	{
	case ATCI_RESULT_CODE_OK:
		ret = utlAtCommandResponse(atHandle, utlAT_COMMAND_RESPONSE_TYPE_INFO_TEXT,  "");
		break;

	case ATCI_RESULT_CODE_ERROR:
		ret = utlAtCommandResponse(atHandle, utlAT_COMMAND_RESPONSE_TYPE_ERROR,  "");
		break;

	case ATCI_RESULT_CODE_CME_ERROR:
	{
		ret = handle_CMEE_code(atHandle, errCode);
		break;
	}

	case ATCI_RESULT_CODE_CMS_ERROR:
	{
		ret = handle_CMSE_code(atHandle, errCode);
		break;
	}
	case ATCI_RESULT_CODE_SUPPRESS:
		ret = utlAtCommandResponse(atHandle, utlAT_COMMAND_RESPONSE_TYPE_CUSTOM,  "");
		break;
	default:
		/* Do nothing */
		break;
	}

	DBGMSG("%s: ret = %d\n", __FUNCTION__, ret);
	return ret;

}

/*****************************************************************************************
 *  FUNCTION: HANDLE_RETURN_VALUE
 *
 *  NOTES: AT Parser will return "ERROR" to the terminal if the result is utlFAILED; Otherwise, the user should give
 *			the response to the terminal specifically.
 *
 *  RETURNS:
 *	utlSUCCESS is successful, utlFAILED for error
 ******************************************************************************************/

RETURNCODE_T HANDLE_RETURN_VALUE(UINT32 ret)
{
	DBGMSG("%s: ret:%d!\n", __FUNCTION__, ret);
	return ((ret == CIRC_SUCCESS) ? utlSUCCESS : utlFAILED);
}

#endif /*INTEL_2CHIP_PLAT_BVD*/


