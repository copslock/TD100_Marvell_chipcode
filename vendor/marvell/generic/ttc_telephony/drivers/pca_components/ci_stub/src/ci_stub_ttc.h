#ifndef _CI_STUB_TTC_H_
#define _CI_STUB_TTC_H_

#include "ci_api_types.h"
#include "shm_share.h"
#include "ci_sim.h"

//#define CISTUB_PORT 1 //ttc
//#define CIDATASTUB_PORT 3 //ttc

#define CiNullProcId                                                    0x00
#define CiShRegisterReqProcId                   0x01
#define CiShDeregisterReqProcId         0x02
#define CiShRequestProcId                                       0x03

#define CiRequestProcId                                         0x04
#define CiRespondProcId                                         0x05
#define CiStubRequestStartProcId                 0x06
#define CiDataStubRequestStartProcId                 0x07
#define CiDataStubReqDataProcId                 0x08

#define CiShConfirmCbId                                 0x0001
#define CiConfirmDefCbId                                0x0002
#define CiIndicateDefCbId                               0x0003
#define CiConfirmCbId                                           0x0004
#define CiIndicateCbId                                  0x0005
#define CiStubConfirmStartProcId                0x0006
#define CiDatStubConfirmStartProcId   0x0007
#define CiDataStubIndDataProcId         0x0008


#define MAX_CI_STUB_RXMSG_LEN  2048
#define CI_DAT_INTERNAL_BUFFER 0xA

typedef enum _ciClientStubInitStatus
{
	CI_STUB_INIT_NOT_STARTED,
	CI_STUB_INIT_PENDING_CNF,
	CI_STUB_INIT_COMPLETE,

	CI_STUB_INIT_INVALID

} ciClientStubInitStatus;

#define SHM_PROC_MSGHDR_PTR(ptr)   ((ptr) + sizeof(ShmApiMsg))


#if 0
typedef struct CiShRegisterReqArgs {
	//CiShConfirm ciShConfirm;
	//CiShOpaqueHandle opShHandle;
	//CiShFreeReqMem ciShFreeReqMem;
	CiShOpaqueHandle ciShOpaqueHandle;
}CiShRegisterReqArgs;

typedef struct CiShDeregisterReqArgs {
	CiShHandle handle;
	CiShOpaqueHandle opShHandle;
}CiShDeregisterReqArgs;

typedef struct _CiShRequestArgs {
	CiShHandle handle;
	CiShOper oper;
	CiShRequestHandle opHandle;
}CiShRequestArgs;
typedef struct _CiRequestArgs {
	CiServiceHandle handle;
	CiPrimitiveID primId;
	CiRequestHandle reqHandle;
}CiRequestArgs;

typedef struct _CiShConfirmArgs {
	CiShOpaqueHandle opShHandle;
	CiShOper oper;
	CiShRequestHandle opHandle;
}CiShConfirmArgs;


typedef struct _CiConfirmArgs {
	CiSgOpaqueHandle opSgCnfHandle;
	CiServiceGroupID id;
	CiPrimitiveID primId;
	CiRequestHandle reqHandle;
}CiConfirmArgs;

typedef struct _CiIndicateArgs {
	CiSgOpaqueHandle opSgIndHandle;
	CiServiceGroupID id;
	CiPrimitiveID primId;
	CiIndicationHandle indHandle;
}CiIndicateArgs;

typedef struct CiRespondArgs {
	CiServiceHandle handle;
	CiPrimitiveID primId;
	CiIndicationHandle indHandle;
}CiRespondArgs;
#endif
typedef struct CiShRegisterReqArgs CiShRegisterReqArgs;
struct CiShRegisterReqArgs {
	CiShConfirm ciShConfirm;
	CiShOpaqueHandle opShHandle;
	CiShFreeReqMem ciShFreeReqMem;
	CiShOpaqueHandle ciShOpaqueHandle;
};

typedef struct CiShDeregisterReqArgs CiShDeregisterReqArgs;
struct CiShDeregisterReqArgs {
	CiShHandle handle;
	CiShOpaqueHandle opShHandle;
};

typedef struct CiShRequestArgs CiShRequestArgs;
struct CiShRequestArgs {
	CiShHandle handle;
	CiShOper oper;
	void* reqParas;
	CiShRequestHandle opHandle;
};


typedef struct CiRequestArgs CiRequestArgs;
struct CiRequestArgs {
	CiServiceHandle handle;
	CiPrimitiveID primId;
	CiRequestHandle reqHandle;
	void* paras;
};

typedef struct CiRespondArgs CiRespondArgs;
struct CiRespondArgs {
	CiServiceHandle handle;
	CiPrimitiveID primId;
	CiIndicationHandle indHandle;
	void* paras;
};

typedef struct CiShConfirmArgs CiShConfirmArgs;
struct CiShConfirmArgs {
	CiShOpaqueHandle opShHandle;
	CiShOper oper;
	void* cnfParas;
	CiShRequestHandle opHandle;
};

typedef struct CiConfirmArgs CiConfirmArgs;
struct CiConfirmArgs {
	CiSgOpaqueHandle opSgCnfHandle;
	CiServiceGroupID id;
	CiPrimitiveID primId;
	CiRequestHandle reqHandle;
	void* paras;
};

typedef struct CiIndicateArgs CiIndicateArgs;
struct CiIndicateArgs {
	CiSgOpaqueHandle opSgIndHandle;
	CiServiceGroupID id;
	CiPrimitiveID primId;
	CiIndicationHandle indHandle;
	void* paras;
};

typedef struct CrsmCciAction_struct CrsmCciAction;
struct CrsmCciAction_struct{
    UINT16  cci_confirm;
    UINT32 crsm_reshandle;
    UINT32  svshandle;
    void* ci_request;
    CiSimPrimGenericCmdReq GenSimCmd_crsm;
    struct CrsmCciAction_struct *next;
};

void clientCiShConfirmCallback(CiShConfirmArgs* pArg,  void *paras);
void clientCiDefConfirmCallback(CiConfirmArgs* pArg, void *paras);
void clientCiConfirmCallback(CiConfirmArgs* pArg, void *paras);
void clientCiConfirmCallback_transfer(CiConfirmArgs* pArg, void *paras,CrsmCciAction *tem);
void clientCiIndicateCallback(CiIndicateArgs* pArg, void *paras);
void clientCiDefIndicateCallback(CiIndicateArgs* pArg, void *paras);
CiReturnCode  ciShRegisterReq_1(CiShConfirm ciShConfirm,
				CiShOpaqueHandle opShHandle);
CiReturnCode  ciShDeregisterReq_1(CiShHandle handle, CiShOpaqueHandle opShHandle);
CiReturnCode  ciShRequest_1(CiShHandle handle,
			    CiShOper oper,
			    void* reqParas,
			    CiShRequestHandle opHandle);
CiReturnCode ciRequest_Transfer(CiServiceHandle svchandle,  CiPrimitiveID primId, CiRequestHandle reqHandle,void* paras);
CiReturnCode  ciRequest_1(CiServiceHandle handle,
			  CiPrimitiveID primId,
			  CiRequestHandle reqHandle,
			  void* paras);
CiReturnCode ciRespond_1(CiServiceHandle handle,
			 CiPrimitiveID primId,
			 CiIndicationHandle indHandle,
			 void* paras);

void CiSetSvgGroupHandle(UINT8 svgId, UINT32 shcnfhandler);

void clearCciActionlist(void);







#endif /* _CI_STUB_TTC_H_ */
