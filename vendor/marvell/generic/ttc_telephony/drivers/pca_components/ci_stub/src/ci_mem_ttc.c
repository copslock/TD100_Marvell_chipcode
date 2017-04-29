#include "ci_stub_ttc.h"
#include "ci_trace.h"
#include "ci_cc.h"
#include "ci_ss.h"
#include "ci_mm.h"
#include "ci_pb.h"
#include "ci_sim.h"
#include "ci_msg.h"
#include "ci_ps.h"
#include "ci_dat.h"
#include "ci_dev.h"

extern CiServiceGroupID findCiSgId(CiServiceHandle handle);

static const UINT16 CiShellPrimReqSize[CI_SH_NUMOPERS] =
{
	0,                                              /* CI_SH_OPER_REGISTERSH - no request for this primitive   */
	0,                                              /* CI_SH_OPER_DEREGISTERSH - no request for this primitive */
	sizeof(CiShOperGetVersionReq),                  /* CI_SH_OPER_GETVERSION,                         */
	sizeof(CiShOperRegisterSgReq),                  /* CI_SH_OPER_REGISTERSG,                         */
	sizeof(CiShOperDeregisterSgReq),                /* CI_SH_OPER_DEREGISTERSG,                       */
	sizeof(CiShOperRegisterDefaultSgReq),           /* CI_SH_OPER_REGISTERDEFSG,                      */
	sizeof(CiShOperDeregisterDefaultSgReq),         /* CI_SH_OPER_DEREGISTERDEFSG,                    */
	sizeof(CiShQueryExtendedReq),                   /* CI_SH_QUERYEXTENDED,                           */
	sizeof(CiShQueryExtendedServicePrimReq),        /* CI_SH_QUERYEXTENDEDSERVICEPRIM,                */
	sizeof(CiShQueryExtendedServiceGroupReq)        /* CI_SH_QUERYEXTENDEDSERVICEGROUP                */
};

static const UINT16 CiShellPrimCnfSize[CI_SH_NUMOPERS] =
{
	sizeof(CiShOperRegisterShCnf),                  /* CI_SH_OPER_REGISTERSH*/
	sizeof(CiShOperDeregisterShCnf),                /* CI_SH_OPER_DEREGISTERSH*/
	sizeof(CiShOperGetVersionCnf),                  /* CI_SH_OPER_GETVERSION,                         */
	sizeof(CiShOperRegisterSgCnf),                  /* CI_SH_OPER_REGISTERSG,                         */
	sizeof(CiShOperDeregisterSgCnf),                /* CI_SH_OPER_DEREGISTERSG,                       */
	sizeof(CiShOperRegisterDefaultSgCnf),           /* CI_SH_OPER_REGISTERDEFSG,                      */
	sizeof(CiShOperDeregisterDefaultSgCnf),         /* CI_SH_OPER_DEREGISTERDEFSG,                    */
	sizeof(CiShQueryExtendedCnf),                   /* CI_SH_QUERYEXTENDED,                           */
	sizeof(CiShQueryExtendedServicePrimCnf),        /* CI_SH_QUERYEXTENDEDSERVICEPRIM,                */
	sizeof(CiShQueryExtendedServiceGroupCnf)        /* CI_SH_QUERYEXTENDEDSERVICEGROUP                */
};

static const UINT16 CiCcPrimSize[CI_CC_NUM_PRIM + 1] =
{
	0,                                              /* NOT IN USE */
	0,                                              /*     CI_CC_PRIM_GET_NUMBERTYPE_REQ = 1,         */
	sizeof(CiCcPrimGetNumberTypeCnf),               /*     CI_CC_PRIM_GET_NUMBERTYPE_CNF,             */
	sizeof(CiCcPrimSetNumberTypeReq),               /*     CI_CC_PRIM_SET_NUMBERTYPE_REQ,             */
	sizeof(CiCcPrimSetNumberTypeCnf),               /*     CI_CC_PRIM_SET_NUMBERTYPE_CNF,             */
	0,                                              /*     CI_CC_PRIM_GET_SUPPORTED_CALLMODES_REQ,    */
	sizeof(CiCcPrimGetSupportedCallModesCnf),       /*     CI_CC_PRIM_GET_SUPPORTED_CALLMODES_CNF,    */
	0,                                              /*     CI_CC_PRIM_GET_CALLMODE_REQ,               */
	sizeof(CiCcPrimGetCallModeCnf),                 /*     CI_CC_PRIM_GET_CALLMODE_CNF,               */
	sizeof(CiCcPrimSetCallModeReq),                 /*     CI_CC_PRIM_SET_CALLMODE_REQ,               */
	sizeof(CiCcPrimSetCallModeCnf),                 /*     CI_CC_PRIM_SET_CALLMODE_CNF,               */
	0,                                              /*     CI_CC_PRIM_GET_SUPPORTED_DATA_BSTYPES_REQ, */
	sizeof(CiCcPrimGetSupportedDataBsTypesCnf),     /*     CI_CC_PRIM_GET_SUPPORTED_DATA_BSTYPES_CNF, */
	0,                                              /*     CI_CC_PRIM_GET_DATA_BSTYPE_REQ,            */
	sizeof(CiCcPrimGetDataBsTypeCnf),               /*     CI_CC_PRIM_GET_DATA_BSTYPE_CNF,            */
	sizeof(CiCcPrimSetDataBsTypeReq),               /*     CI_CC_PRIM_SET_DATA_BSTYPE_REQ,            */
	sizeof(CiCcPrimSetDataBsTypeCnf),               /*     CI_CC_PRIM_SET_DATA_BSTYPE_CNF,            */
	0,                                              /*     CI_CC_PRIM_GET_AUTOANSWER_ACTIVE_REQ,      */
	sizeof(CiCcPrimGetAutoAnswerActiveCnf),         /*     CI_CC_PRIM_GET_AUTOANSWER_ACTIVE_CNF,      */
	sizeof(CiCcPrimSetAutoAnswerActiveReq),         /*     CI_CC_PRIM_SET_AUTOANSWER_ACTIVE_REQ,      */
	sizeof(CiCcPrimSetAutoAnswerActiveCnf),         /*     CI_CC_PRIM_SET_AUTOANSWER_ACTIVE_CNF,      */
	0,                                              /*     CI_CC_PRIM_LIST_CURRENT_CALLS_REQ,         */
	sizeof(CiCcPrimListCurrentCallsCnf),            /*     CI_CC_PRIM_LIST_CURRENT_CALLS_CNF,         */
	sizeof(CiCcPrimGetCallInfoReq),                 /*     CI_CC_PRIM_GET_CALLINFO_REQ,               */
	sizeof(CiCcPrimGetCallInfoCnf),                 /*     CI_CC_PRIM_GET_CALLINFO_CNF,               */
	sizeof(CiCcPrimMakeCallReq),                    /*     CI_CC_PRIM_MAKE_CALL_REQ,                  */
	sizeof(CiCcPrimMakeCallCnf),                    /*     CI_CC_PRIM_MAKE_CALL_CNF,                  */
	sizeof(CiCcPrimCallProceedingInd),              /*     CI_CC_PRIM_CALL_PROCEEDING_IND,            */
	sizeof(CiCcPrimMoCallFailedInd),                /*     CI_CC_PRIM_MO_CALL_FAILED_IND,             */
	sizeof(CiCcPrimAlertingInd),                    /*     CI_CC_PRIM_ALERTING_IND,                   */
	sizeof(CiCcPrimConnectInd),                     /*     CI_CC_PRIM_CONNECT_IND,                    */
	sizeof(CiCcPrimDisconnectInd),                  /*     CI_CC_PRIM_DISCONNECT_IND,                 */
	sizeof(CiCcPrimIncomingCallInd),                /*     CI_CC_PRIM_INCOMING_CALL_IND,              */
	sizeof(CiCcPrimCallWaitingInd),                 /*     CI_CC_PRIM_CALL_WAITING_IND,               */
	sizeof(CiCcPrimHeldCallInd),                    /*     CI_CC_PRIM_HELD_CALL_IND,                  */
	sizeof(CiCcPrimAnswerCallReq),                  /*     CI_CC_PRIM_ANSWER_CALL_REQ,                */
	sizeof(CiCcPrimAnswerCallCnf),                  /*     CI_CC_PRIM_ANSWER_CALL_CNF,                */
	sizeof(CiCcPrimRefuseCallReq),                  /*     CI_CC_PRIM_REFUSE_CALL_REQ,                */
	sizeof(CiCcPrimRefuseCallCnf),                  /*     CI_CC_PRIM_REFUSE_CALL_CNF,                */
	sizeof(CiCcPrimMtCallFailedInd),                /*     CI_CC_PRIM_MT_CALL_FAILED_IND,             */
	sizeof(CiCcPrimHoldCallReq),                    /*     CI_CC_PRIM_HOLD_CALL_REQ,                  */
	sizeof(CiCcPrimHoldCallCnf),                    /*     CI_CC_PRIM_HOLD_CALL_CNF,                  */
	sizeof(CiCcPrimRetrieveCallReq),                /*     CI_CC_PRIM_RETRIEVE_CALL_REQ,              */
	sizeof(CiCcPrimRetrieveCallCnf),                /*     CI_CC_PRIM_RETRIEVE_CALL_CNF,              */
	sizeof(CiCcPrimSwitchActiveHeldReq),            /*     CI_CC_PRIM_SWITCH_ACTIVE_HELD_REQ,         */
	sizeof(CiCcPrimSwitchActiveHeldCnf),            /*     CI_CC_PRIM_SWITCH_ACTIVE_HELD_CNF,         */
	sizeof(CiCcPrimCallDeflectReq),                 /*     CI_CC_PRIM_CALL_DEFLECT_REQ,               */
	sizeof(CiCcPrimCallDeflectCnf),                 /*     CI_CC_PRIM_CALL_DEFLECT_CNF,               */
	sizeof(CiCcPrimExplicitCallTransferReq),        /*     CI_CC_PRIM_EXPLICIT_CALL_TRANSFER_REQ,     */
	sizeof(CiCcPrimExplicitCallTransferCnf),        /*     CI_CC_PRIM_EXPLICIT_CALL_TRANSFER_CNF,     */
	sizeof(CiCcPrimReleaseCallReq),                 /*     CI_CC_PRIM_RELEASE_CALL_REQ,               */
	sizeof(CiCcPrimReleaseCallCnf),                 /*     CI_CC_PRIM_RELEASE_CALL_CNF,               */
	sizeof(CiCcPrimReleaseAllCallsReq),             /*     CI_CC_PRIM_RELEASE_ALL_CALLS_REQ,          */
	sizeof(CiCcPrimReleaseAllCallsCnf),             /*     CI_CC_PRIM_RELEASE_ALL_CALLS_CNF,          */
	sizeof(CiCcPrimSwitchCallModeReq),              /*     CI_CC_PRIM_SWITCH_CALLMODE_REQ,            */
	sizeof(CiCcPrimSwitchCallModeCnf),              /*     CI_CC_PRIM_SWITCH_CALLMODE_CNF,            */
	sizeof(CiCcPrimEstablishMptyReq),               /*     CI_CC_PRIM_ESTABLISH_MPTY_REQ,             */
	sizeof(CiCcPrimEstablishMptyCnf),               /*     CI_CC_PRIM_ESTABLISH_MPTY_CNF,             */
	sizeof(CiCcPrimAddToMptyReq),                   /*     CI_CC_PRIM_ADD_TO_MPTY_REQ,                */
	sizeof(CiCcPrimAddToMptyCnf),                   /*     CI_CC_PRIM_ADD_TO_MPTY_CNF,                */
	sizeof(CiCcPrimHoldMptyReq),                    /*     CI_CC_PRIM_HOLD_MPTY_REQ,                  */
	sizeof(CiCcPrimHoldMptyCnf),                    /*     CI_CC_PRIM_HOLD_MPTY_CNF,                  */
	sizeof(CiCcPrimRetrieveMptyReq),                /*     CI_CC_PRIM_RETRIEVE_MPTY_REQ,              */
	sizeof(CiCcPrimRetrieveMptyCnf),                /*     CI_CC_PRIM_RETRIEVE_MPTY_CNF,              */
	sizeof(CiCcPrimSplitFromMptyReq),               /*     CI_CC_PRIM_SPLIT_FROM_MPTY_REQ,            */
	sizeof(CiCcPrimSplitFromMptyCnf),               /*     CI_CC_PRIM_SPLIT_FROM_MPTY_CNF,            */
	sizeof(CiCcPrimShuttleMptyReq),                 /*     CI_CC_PRIM_SHUTTLE_MPTY_REQ,               */
	sizeof(CiCcPrimShuttleMptyCnf),                 /*     CI_CC_PRIM_SHUTTLE_MPTY_CNF,               */
	sizeof(CiCcPrimReleaseMptyReq),                 /*     CI_CC_PRIM_RELEASE_MPTY_REQ,               */
	sizeof(CiCcPrimReleaseMptyCnf),                 /*     CI_CC_PRIM_RELEASE_MPTY_CNF,               */
	sizeof(CiCcPrimStartDtmfReq),                   /*     CI_CC_PRIM_START_DTMF_REQ,                 */
	sizeof(CiCcPrimStartDtmfCnf),                   /*     CI_CC_PRIM_START_DTMF_CNF,                 */
	sizeof(CiCcPrimStopDtmfReq),                    /*     CI_CC_PRIM_STOP_DTMF_REQ,                  */
	sizeof(CiCcPrimStopDtmfCnf),                    /*     CI_CC_PRIM_STOP_DTMF_CNF,                  */
	0,                                              /*     CI_CC_PRIM_GET_DTMF_PACING_REQ,            */
	sizeof(CiCcPrimGetDtmfPacingCnf),               /*     CI_CC_PRIM_GET_DTMF_PACING_CNF,            */
	sizeof(CiCcPrimSetDtmfPacingReq),               /*     CI_CC_PRIM_SET_DTMF_PACING_REQ,            */
	sizeof(CiCcPrimSetDtmfPacingCnf),               /*     CI_CC_PRIM_SET_DTMF_PACING_CNF,            */
	sizeof(CiCcPrimSendDtmfStringReq),              /*     CI_CC_PRIM_SEND_DTMF_STRING_REQ,           */
	sizeof(CiCcPrimSendDtmfStringCnf),              /*     CI_CC_PRIM_SEND_DTMF_STRING_CNF,           */
	sizeof(CiCcPrimClipInfoInd),                    /*     CI_CC_PRIM_CLIP_INFO_IND,                  */
	sizeof(CiCcPrimColpInfoInd),                    /*     CI_CC_PRIM_COLP_INFO_IND,                  */
	sizeof(CiCcPrimCcmUpdateInd),                   /*     CI_CC_PRIM_CCM_UPDATE_IND,                 */
	0,                                              /*     CI_CC_PRIM_GET_CCM_VALUE_REQ,              */
	sizeof(CiCcPrimGetCcmValueCnf),                 /*     CI_CC_PRIM_GET_CCM_VALUE_CNF,              */
	0,                                              /*     CI_CC_PRIM_AOC_WARNING_IND,                */
	sizeof(CiCcPrimSsiNotifyInd),                   /*     CI_CC_PRIM_SSI_NOTIFY_IND,                 */
	sizeof(CiCcPrimSsuNotifyInd),                   /*     CI_CC_PRIM_SSU_NOTIFY_IND,                 */
	0,                                              /*     CI_CC_PRIM_LOCALCB_NOTIFY_IND,             */
	0,                                              /*     CI_CC_PRIM_GET_ACM_VALUE_REQ,              */
	sizeof(CiCcPrimGetAcmValueCnf),                 /*     CI_CC_PRIM_GET_ACM_VALUE_CNF,              */
	0,                                              /*     CI_CC_PRIM_RESET_ACM_VALUE_REQ,            */
	sizeof(CiCcPrimResetAcmValueCnf),               /*     CI_CC_PRIM_RESET_ACM_VALUE_CNF,            */
	0,                                              /*     CI_CC_PRIM_GET_ACMMAX_VALUE_REQ,           */
	sizeof(CiCcPrimGetAcmMaxValueCnf),              /*     CI_CC_PRIM_GET_ACMMAX_VALUE_CNF,           */
	sizeof(CiCcPrimSetAcmMaxValueReq),              /*     CI_CC_PRIM_SET_ACMMAX_VALUE_REQ,           */
	sizeof(CiCcPrimSetAcmMaxValueCnf),              /*     CI_CC_PRIM_SET_ACMMAX_VALUE_CNF,           */
	0,                                              /*     CI_CC_PRIM_GET_PUCT_INFO_REQ,              */
	sizeof(CiCcPrimGetPuctInfoCnf),                 /*     CI_CC_PRIM_GET_PUCT_INFO_CNF,              */
	sizeof(CiCcPrimSetPuctInfoReq),                 /*     CI_CC_PRIM_SET_PUCT_INFO_REQ,              */
	sizeof(CiCcPrimSetPuctInfoCnf),                 /*     CI_CC_PRIM_SET_PUCT_INFO_CNF,              */
	0,                                              /*     CI_CC_PRIM_GET_BASIC_CALLMODES_REQ,        */
	sizeof(CiCcPrimGetBasicCallModesCnf),           /*     CI_CC_PRIM_GET_BASIC_CALLMODES_CNF,        */
	0,                                              /*     CI_CC_PRIM_GET_CALLOPTIONS_REQ,            */
	sizeof(CiCcPrimGetCallOptionsCnf),              /*     CI_CC_PRIM_GET_CALLOPTIONS_CNF,            */
	0,                                              /*     CI_CC_PRIM_GET_DATACOMP_CAP_REQ,           */
	sizeof(CiCcPrimGetDataCompCapCnf),              /*     CI_CC_PRIM_GET_DATACOMP_CAP_CNF,           */
	0,                                              /*     CI_CC_PRIM_GET_DATACOMP_REQ,               */
	sizeof(CiCcPrimGetDataCompCnf),                 /*     CI_CC_PRIM_GET_DATACOMP_CNF,               */
	sizeof(CiCcPrimSetDataCompReq),                 /*     CI_CC_PRIM_SET_DATACOMP_REQ,               */
	sizeof(CiCcPrimSetDataCompCnf),                 /*     CI_CC_PRIM_SET_DATACOMP_CNF,               */
	0,                                              /*     CI_CC_PRIM_GET_RLP_CAP_REQ,                */
	sizeof(CiCcPrimGetRlpCapCnf),                   /*     CI_CC_PRIM_GET_RLP_CAP_CNF,                */
	sizeof(CiCcPrimGetRlpCfgReq),                   /*     CI_CC_PRIM_GET_RLP_CFG_REQ,                */
	sizeof(CiCcPrimGetRlpCfgCnf),                   /*     CI_CC_PRIM_GET_RLP_CFG_CNF,                */
	sizeof(CiCcPrimSetRlpCfgReq),                   /*     CI_CC_PRIM_SET_RLP_CFG_REQ,                */
	sizeof(CiCcPrimSetRlpCfgCnf),                   /*     CI_CC_PRIM_SET_RLP_CFG_CNF,                */
	sizeof(CiCcPrimDataServiceNegInd),              /*     CI_CC_PRIM_DATA_SERVICENEG_IND,            */
	sizeof(CiCcPrimEnableDataServiceNegIndReq),     /*     CI_CC_PRIM_ENABLE_DATA_SERVICENEG_IND_REQ, */
	sizeof(CiCcPrimEnableDataServiceNegIndCnf),     /*     CI_CC_PRIM_ENABLE_DATA_SERVICENEG_IND_CNF, */
	sizeof(CiCcPrimSetUDUBReq),                     /*     CI_CC_PRIM_SET_UDUB_REQ,                   */
	sizeof(CiCcPrimSetUDUBCnf),                     /*     CI_CC_PRIM_SET_UDUB_CNF,                   */
	sizeof(CiCcPrimGetSupportedCallManOpsReq),      /*     CI_CC_PRIM_GET_SUPPORTED_CALLMAN_OPS_REQ,  */
	sizeof(CiCcPrimGetSupportedCallManOpsCnf),      /*     CI_CC_PRIM_GET_SUPPORTED_CALLMAN_OPS_CNF,  */
	sizeof(CiCcPrimManipulateCallsReq),             /*     CI_CC_PRIM_MANIPULATE_CALLS_REQ,           */
	sizeof(CiCcPrimManipulateCallsCnf),             /*     CI_CC_PRIM_MANIPULATE_CALLS_CNF,           */
	sizeof(CiCcPrimListCurrentCallsInd),            /*     CI_CC_PRIM_LIST_CURRENT_CALLS_IND,         */

	sizeof(CiCcPrimCallDiagnosticInd),              /*     CI_CC_PRIM_CALL_DIAGNOSTIC_IND,            */
	sizeof(CiCcPrimDtmfEventInd),                   /*     CI_CC_PRIM_DTMF_EVENT_IND,                 */
	sizeof(CiCcPrimClearBlackListReq),              /*     CI_CC_PRIM_CLEAR_BLACK_LIST_REQ,           */
	sizeof(CiCcPrimClearBlackListCnf),              /*     CI_CC_PRIM_CLEAR_BLACK_LIST_CNF,           */
	sizeof(CiCcPrimSetCtmStatusReq),                /*     CI_CC_PRIM_SET_CTM_STATUS_REQ,           */
	sizeof(CiCcPrimSetCtmStatusCnf),                /*     CI_CC_PRIM_SET_CTM_STATUS_CNF,           */
	sizeof(CiCcPrimCTMNegReportInd),                /*     CI_CC_PRIM_CTM_NEG_REPORT_IND,           */
	sizeof(CiCcPrimCdipInfoInd),                     /*     Michal Bukai - CI_CC_PRIM_CDIP_INFO_IND,    */
#ifdef AT_CUSTOMER_HTC
	sizeof(CiCcPrimSetFwListReq),                   /*CI_CC_PRIM_SET_FW_LIST_REQ*/
	sizeof(CiCcPrimSetFwListCnf)                   /*CI_CC_PRIM_SET_FW_LIST_CNF*/
#endif
};

static const UINT16 CiSsPrimSize[CI_SS_NUM_PRIM + 1] =
{
	0,                                                                                                              /* NOT IN USE */
	0,                                                                                                              /* CI_SS_PRIM_GET_CLIP_STATUS_REQ                         */
	sizeof(CiSsPrimGetClipStatusCnf),                                                                               /* CI_SS_PRIM_GET_CLIP_STATUS_CNF                         */
	sizeof(CiSsPrimSetClipOptionReq),                                                                               /* CI_SS_PRIM_SET_CLIP_OPTION_REQ                         */
	sizeof(CiSsPrimSetClipOptionCnf),                                                                               /* CI_SS_PRIM_SET_CLIP_OPTION_CNF                         */
	0,                                                                                                              /* CI_SS_PRIM_GET_CLIR_STATUS_REQ                         */
	sizeof(CiSsPrimGetClirStatusCnf),                                                                               /* CI_SS_PRIM_GET_CLIR_STATUS_CNF                         */
	sizeof(CiSsPrimSetClirOptionReq),                                                                               /* CI_SS_PRIM_SET_CLIR_OPTION_REQ                         */
	sizeof(CiSsPrimSetClirOptionCnf),                                                                               /* CI_SS_PRIM_SET_CLIR_OPTION_CNF                         */
	0,                                                                                                              /* CI_SS_PRIM_GET_COLP_STATUS_REQ                         */
	sizeof(CiSsPrimGetColpStatusCnf),                                                                               /* CI_SS_PRIM_GET_COLP_STATUS_CNF                         */
	sizeof(CiSsPrimSetColpOptionReq),                                                                               /* CI_SS_PRIM_SET_COLP_OPTION_REQ                         */
	sizeof(CiSsPrimSetColpOptionCnf),                                                                               /* CI_SS_PRIM_SET_COLP_OPTION_CNF                         */
	0,                                                                                                              /* CI_SS_PRIM_GET_CUG_CONFIG_REQ                          */
	sizeof(CiSsPrimGetCugConfigCnf),                                                                                /* CI_SS_PRIM_GET_CUG_CONFIG_CNF                          */
	sizeof(CiSsPrimSetCugConfigReq),                                                                                /* CI_SS_PRIM_SET_CUG_CONFIG_REQ                          */
	sizeof(CiSsPrimSetCugConfigCnf),                                                                                /* CI_SS_PRIM_SET_CUG_CONFIG_CNF                          */
	0,                                          /* CI_SS_PRIM_GET_CNAP_STATUS_REQ                          */       //Michal Bukai - CNAP support
	sizeof(CiSsPrimGetCnapStatusCnf),            /* CI_SS_PRIM_GET_CNAP_STATUS_CNF                          */      //Michal Bukai - CNAP support
	sizeof(CiSsPrimSetCnapOptionReq),            /* CI_SS_PRIM_SET_CNAP_OPTION_REQ                          */      //Michal Bukai - CNAP support
	sizeof(CiSsPrimSetCnapOptionCnf),            /* CI_SS_PRIM_SET_CNAP_OPTION_CNF                          */      //Michal Bukai - CNAP support
	sizeof(CiSsPrimRegisterCfInfoReq),                                                                              /* CI_SS_PRIM_REGISTER_CF_INFO_REQ                        */
	sizeof(CiSsPrimRegisterCfInfoCnf),                                                                              /* CI_SS_PRIM_REGISTER_CF_INFO_CNF                        */
	sizeof(CiSsPrimEraseCfInfoReq),                                                                                 /* CI_SS_PRIM_ERASE_CF_INFO_REQ                           */
	sizeof(CiSsPrimEraseCfInfoCnf),                                                                                 /* CI_SS_PRIM_ERASE_CF_INFO_CNF                           */
	0,                                                                                                              /* CI_SS_PRIM_GET_CF_REASONS_REQ                          */
	sizeof(CiSsPrimGetCfReasonsCnf),                                                                                /* CI_SS_PRIM_GET_CF_REASONS_CNF                          */
	sizeof(CiSsPrimSetCfActivationReq),                                                                             /* CI_SS_PRIM_SET_CF_ACTIVATION_REQ                       */
	sizeof(CiSsPrimSetCfActivationCnf),                                                                             /* CI_SS_PRIM_SET_CF_ACTIVATION_CNF                       */
	sizeof(CiSsPrimInterrogateCfInfoReq),                                                                           /* CI_SS_PRIM_INTERROGATE_CF_INFO_REQ                     */
	sizeof(CiSsPrimInterrogateCfInfoCnf),                                                                           /* CI_SS_PRIM_INTERROGATE_CF_INFO_CNF                     */
	sizeof(CiSsPrimSetCwActivationReq),                                                                             /* CI_SS_PRIM_SET_CW_ACTIVATION_REQ                       */
	sizeof(CiSsPrimSetCwActivationCnf),                                                                             /* CI_SS_PRIM_SET_CW_ACTIVATION_CNF                       */
	0,                                                                                                              /* CI_SS_PRIM_GET_CW_OPTION_REQ                           */
	sizeof(CiSsPrimGetCwOptionCnf),                                                                                 /* CI_SS_PRIM_GET_CW_OPTION_CNF                           */
	sizeof(CiSsPrimSetCwOptionReq),                                                                                 /* CI_SS_PRIM_SET_CW_OPTION_REQ                           */
	sizeof(CiSsPrimSetCwOptionCnf),                                                                                 /* CI_SS_PRIM_SET_CW_OPTION_CNF                           */
	sizeof(CiSsPrimGetCwActiveStatusReq),                                                                           /* CI_SS_PRIM_GET_CW_ACTIVE_STATUS_REQ                    */
	sizeof(CiSsPrimGetCwActiveStatusCnf),                                                                           /* CI_SS_PRIM_GET_CW_ACTIVE_STATUS_CNF                    */
	0,                                                                                                              /* CI_SS_PRIM_GET_USSD_ENABLE_REQ                         */
	sizeof(CiSsPrimGetUssdEnableCnf),                                                                               /* CI_SS_PRIM_GET_USSD_ENABLE_CNF                         */
	sizeof(CiSsPrimSetUssdEnableReq),                                                                               /* CI_SS_PRIM_SET_USSD_ENABLE_REQ                         */
	sizeof(CiSsPrimSetUssdEnableCnf),                                                                               /* CI_SS_PRIM_SET_USSD_ENABLE_CNF                         */
	sizeof(CiSsPrimReceivedUssdInfoInd),                                                                            /* CI_SS_PRIM_RECEIVED_USSD_INFO_IND                      */
	sizeof(CiSsPrimReceivedUssdInfoRsp),                                                                            /* CI_SS_PRIM_RECEIVED_USSD_INFO_RSP                      */
	sizeof(CiSsPrimStartUssdSessionReq),                                                                            /* CI_SS_PRIM_START_USSD_SESSION_REQ                      */
	sizeof(CiSsPrimStartUssdSessionCnf),                                                                            /* CI_SS_PRIM_START_USSD_SESSION_CNF                      */
	0,                                                                                                              /* CI_SS_PRIM_ABORT_USSD_SESSION_REQ                      */
	sizeof(CiSsPrimAbortUssdSessionCnf),                                                                            /* CI_SS_PRIM_ABORT_USSD_SESSION_CNF                      */
	0,                                                                                                              /* CI_SS_PRIM_GET_CCM_OPTION_REQ                          */
	sizeof(CiSsPrimGetCcmOptionCnf),                                                                                /* CI_SS_PRIM_GET_CCM_OPTION_CNF                          */
	sizeof(CiSsPrimSetCcmOptionReq),                                                                                /* CI_SS_PRIM_SET_CCM_OPTION_REQ                          */
	sizeof(CiSsPrimSetCcmOptionCnf),                                                                                /* CI_SS_PRIM_SET_CCM_OPTION_CNF                          */
	0,                                                                                                              /* CI_SS_PRIM_GET_AOC_WARNING_ENABLE_REQ                  */
	sizeof(CiSsPrimGetAocWarningEnableCnf),                                                                         /* CI_SS_PRIM_GET_AOC_WARNING_ENABLE_CNF                  */
	sizeof(CiSsPrimSetAocWarningEnableReq),                                                                         /* CI_SS_PRIM_SET_AOC_WARNING_ENABLE_REQ                  */
	sizeof(CiSsPrimSetAocWarningEnableCnf),                                                                         /* CI_SS_PRIM_SET_AOC_WARNING_ENABLE_CNF                  */
	0,                                                                                                              /* CI_SS_PRIM_GET_SS_NOTIFY_OPTIONS_REQ                   */
	sizeof(CiSsPrimGetSsNotifyOptionsCnf),                                                                          /* CI_SS_PRIM_GET_SS_NOTIFY_OPTIONS_CNF                   */
	sizeof(CiSsPrimSetSsNotifyOptionsReq),                                                                          /* CI_SS_PRIM_SET_SS_NOTIFY_OPTIONS_REQ                   */
	sizeof(CiSsPrimSetSsNotifyOptionsCnf),                                                                          /* CI_SS_PRIM_SET_SS_NOTIFY_OPTIONS_CNF                   */
	0,                                                                                                              /* CI_SS_PRIM_GET_LOCALCB_LOCKS_REQ                       */
	sizeof(CiSsPrimGetLocalCbLocksCnf),                                                                             /* CI_SS_PRIM_GET_LOCALCB_LOCKS_CNF                       */
	sizeof(CiSsPrimGetLocalCbLockActiveReq),                                                                        /* CI_SS_PRIM_GET_LOCALCB_LOCK_ACTIVE_REQ                 */
	sizeof(CiSsPrimGetLocalCbLockActiveCnf),                                                                        /* CI_SS_PRIM_GET_LOCALCB_LOCK_ACTIVE_CNF                 */
	sizeof(CiSsPrimSetLocalCbLockActiveReq),                                                                        /* CI_SS_PRIM_SET_LOCALCB_LOCK_ACTIVE_REQ                 */
	sizeof(CiSsPrimSetLocalCbLockActiveCnf),                                                                        /* CI_SS_PRIM_SET_LOCALCB_LOCK_ACTIVE_CNF                 */
	sizeof(CiSsPrimSetLocalCbNotifyOptionReq),                                                                      /* CI_SS_PRIM_SET_LOCALCB_NOTIFY_OPTION_REQ               */
	sizeof(CiSsPrimSetLocalCbNotifyOptionCnf),                                                                      /* CI_SS_PRIM_SET_LOCALCB_NOTIFY_OPTION_CNF               */
	sizeof(CiSsPrimChangeCbPasswordReq),                                                                            /* CI_SS_PRIM_CHANGE_CB_PASSWORD_REQ                      */
	sizeof(CiSsPrimChangeCbPasswordCnf),                                                                            /* CI_SS_PRIM_CHANGE_CB_PASSWORD_CNF                      */
	sizeof(CiSsPrimGetCbStatusReq),                                                                                 /* CI_SS_PRIM_GET_CB_STATUS_REQ                           */
	sizeof(CiSsPrimGetCbStatusCnf),                                                                                 /* CI_SS_PRIM_GET_CB_STATUS_CNF                           */
	sizeof(CiSsPrimSetCbActivationReq),                                                                             /* CI_SS_PRIM_SET_CB_ACTIVATE_REQ                         */
	sizeof(CiSsPrimSetCbActivateCnf),                                                                               /* CI_SS_PRIM_SET_CB_ACTIVATE_CNF                         */
	0,                                                                                                              /* CI_SS_PRIM_GET_CB_TYPES_REQ                            */
	sizeof(CiSsPrimGetCbTypesCnf),                                                                                  /* CI_SS_PRIM_GET_CB_TYPES_CNF                            */
	0,                                                                                                              /* CI_SS_PRIM_GET_BASIC_SVC_CLASSES_REQ                   */
	sizeof(CiSsPrimGetBasicSvcClassesCnf),                                                                          /* CI_SS_PRIM_GET_BASIC_SVC_CLASSES_CNF                   */
	sizeof(CiSsPrimGetActiveCwClassesReq),                                                                          /* CI_SS_PRIM_GET_ACTIVE_CW_CLASSES_REQ                   */
	sizeof(CiSsPrimGetActiveCwClassesCnf),                                                                          /* CI_SS_PRIM_GET_ACTIVE_CW_CLASSES_CNF                   */

	sizeof(CiSsPrimGetCbMapStatusReq),                                                                              /* CI_SS_PRIM_GET_CB_MAP_STATUS_REQ                       */
	sizeof(CiSsPrimGetCbMapStatusCnf),                                                                              /* CI_SS_PRIM_GET_CB_MAP_STATUS_CNF                       */

#if defined(CCI_POSITION_LOCATION)
	sizeof(CiSsPrimPrivacyCtrlRegReq),      /* CI_SS_PRIM_PRIVACY_CTRL_REG_REQ                        */
	sizeof(CiSsPrimPrivacyCtrlRegCnf),      /* CI_SS_PRIM_PRIVACY_CTRL_REG_CNF                        */
	sizeof(CiSsPrimLocationInd),            /* CI_SS_PRIM_LOCATION_IND                                */
	sizeof(CiSsPrimLocationVerifyRsp),      /* CI_SS_PRIM_LOCATION_VERIFY_RSP                         */
	sizeof(CiSsPrimGetLocationReq),         /* CI_SS_PRIM_GET_LOCATION_REQ                            */
	sizeof(CiSsPrimGetLocationCnf),         /* CI_SS_PRIM_GET_LOCATION_CNF                            */
	0,                                      /* CI_SS_PRIM_GET_LCS_NWSTATE_REQ                         */
	sizeof(CiSsPrimGetLcsNwstateCnf),       /* CI_SS_PRIM_GET_LCS_NWSTATE_CNF                         */
	sizeof(CiSsPrimLcsNwstateCfgIndReq),    /* CI_SS_PRIM_LCS_NWSTATE_CFG_IND_REQ                     */
	sizeof(CiSsPrimLcsNwstateCfgIndCnf),    /* CI_SS_PRIM_LCS_NWSTATE_CFG_IND_CNF                     */
	sizeof(CiSsPrimLcsNwstateInd),          /* CI_SS_PRIM_LCS_NWSTATE_IND                             */
#endif /* CCI_POSITION_LOCATION */

	sizeof(CiSsPrimServiceRequestCompleteInd),      /* CI_SS_PRIM_SERVICE_REQUEST_COMPLETE_IND                */

	0,                                              /* CI_SS_PRIM_GET_COLR_STATUS_REQ                         */
	sizeof(CiSsPrimGetColrStatusCnf),               /* CI_SS_PRIM_GET_COLR_STATUS_CNF                         */
	0,                                              /* Michal Bukai - CI_SS_PRIM_GET_CDIP_STATUS_REQ          */
	sizeof(CiSsPrimGetCdipStatusCnf),               /* Michal Bukai - CI_SS_PRIM_GET_CDIP_STATUS_CNF          */
	sizeof(CiSsPrimSetCdipOptionReq),               /* Michal Bukai - CI_SS_PRIM_SET_CDIP_OPTION_REQ          */
	sizeof(CiSsPrimSetCdipOptionCnf)                /* Michal Bukai - CI_SS_PRIM_SET_CDIP_OPTION_CNF          */
};

static const UINT16 CiMmPrimSize[CI_MM_NUM_PRIM + 1] =
{
	0,                                                                              /* NOT IN USE */
	0,                                                                              /* CI_MM_PRIM_GET_NUM_SUBSCRIBER_NUMBERS_REQ      */
	sizeof(CiMmPrimGetNumSubscriberNumbersCnf),                                     /* CI_MM_PRIM_GET_NUM_SUBSCRIBER_NUMBERS_CNF      */
	sizeof(CiMmPrimGetSubscriberInfoReq),                                           /* CI_MM_PRIM_GET_SUBSCRIBER_INFO_REQ             */
	sizeof(CiMmPrimGetSubscriberInfoCnf),                                           /* CI_MM_PRIM_GET_SUBSCRIBER_INFO_CNF             */
	0,                                                                              /* CI_MM_PRIM_GET_SUPPORTED_REGRESULT_OPTIONS_REQ */
	sizeof(CiMmPrimGetSupportedRegResultOptionsCnf),                                /* CI_MM_PRIM_GET_SUPPORTED_REGRESULT_OPTIONS_CNF */
	0,                                                                              /* CI_MM_PRIM_GET_REGRESULT_OPTION_REQ            */
	sizeof(CiMmPrimGetRegResultOptionCnf),                                          /* CI_MM_PRIM_GET_REGRESULT_OPTION_CNF            */
	sizeof(CiMmPrimSetRegResultOptionReq),                                          /* CI_MM_PRIM_SET_REGRESULT_OPTION_REQ            */
	sizeof(CiMmPrimSetRegResultOptionCnf),                                          /* CI_MM_PRIM_SET_REGRESULT_OPTION_CNF            */
	sizeof(CiMmPrimRegResultInd),                                                   /* CI_MM_PRIM_REGRESULT_IND                       */
	0,                                                                              /* CI_MM_PRIM_GET_REGRESULT_INFO_REQ              */
	sizeof(CiMmPrimGetRegResultInfoCnf),                                            /* CI_MM_PRIM_GET_REGRESULT_INFO_CNF              */
	0,                                                                              /* CI_MM_PRIM_GET_SUPPORTED_ID_FORMATS_REQ        */
	sizeof(CiMmPrimGetSupportedIdFormatsCnf),                                       /* CI_MM_PRIM_GET_SUPPORTED_ID_FORMATS_CNF        */
	0,                                                                              /* CI_MM_PRIM_GET_ID_FORMAT_REQ                   */
	sizeof(CiMmPrimGetIdFormatCnf),                                                 /* CI_MM_PRIM_GET_ID_FORMAT_CNF                   */
	sizeof(CiMmPrimSetIdFormatReq),                                                 /* CI_MM_PRIM_SET_ID_FORMAT_REQ                   */
	sizeof(CiMmPrimSetIdFormatCnf),                                                 /* CI_MM_PRIM_SET_ID_FORMAT_CNF                   */
	0,                                                                              /* CI_MM_PRIM_GET_NUM_NETWORK_OPERATORS_REQ       */
	sizeof(CiMmPrimGetNumNetworkOperatorsCnf),                                      /* CI_MM_PRIM_GET_NUM_NETWORK_OPERATORS_CNF       */
	sizeof(CiMmPrimGetNetworkOperatorInfoReq),                                      /* CI_MM_PRIM_GET_NETWORK_OPERATOR_INFO_REQ       */
	sizeof(CiMmPrimGetNetworkOperatorInfoCnf),                                      /* CI_MM_PRIM_GET_NETWORK_OPERATOR_INFO_CNF       */
	0,                                                                              /* CI_MM_PRIM_GET_NUM_PREFERRED_OPERATORS_REQ     */
	sizeof(CiMmPrimGetNumPreferredOperatorsCnf),                                    /* CI_MM_PRIM_GET_NUM_PREFERRED_OPERATORS_CNF     */
	sizeof(CiMmPrimGetPreferredOperatorInfoReq),                                    /* CI_MM_PRIM_GET_PREFERRED_OPERATOR_INFO_REQ     */
	sizeof(CiMmPrimGetPreferredOperatorInfoCnf),                                    /* CI_MM_PRIM_GET_PREFERRED_OPERATOR_INFO_CNF     */
	sizeof(CiMmPrimAddPreferredOperatorReq),                                        /* CI_MM_PRIM_ADD_PREFERRED_OPERATOR_REQ          */
	sizeof(CiMmPrimAddPreferredOperatorCnf),                                        /* CI_MM_PRIM_ADD_PREFERRED_OPERATOR_CNF          */
	sizeof(CiMmPrimDeletePreferredOperatorReq),                                     /* CI_MM_PRIM_DELETE_PREFERRED_OPERATOR_REQ       */
	sizeof(CiMmPrimDeletePreferredOperatorCnf),                                     /* CI_MM_PRIM_DELETE_PREFERRED_OPERATOR_CNF       */
	0,                                                                              /* CI_MM_PRIM_GET_CURRENT_OPERATOR_INFO_REQ       */
	sizeof(CiMmPrimGetCurrentOperatorInfoCnf),                                      /* CI_MM_PRIM_GET_CURRENT_OPERATOR_INFO_CNF       */
	0,                                                                              /* CI_MM_PRIM_AUTO_REGISTER_REQ                   */
	sizeof(CiMmPrimAutoRegisterCnf),                                                /* CI_MM_PRIM_AUTO_REGISTER_CNF                   */
	sizeof(CiMmPrimManualRegisterReq),                                              /* CI_MM_PRIM_MANUAL_REGISTER_REQ                 */
	sizeof(CiMmPrimManualRegisterCnf),                                              /* CI_MM_PRIM_MANUAL_REGISTER_CNF                 */
	0,                                                                              /* CI_MM_PRIM_DEREGISTER_REQ                      */
	sizeof(CiMmPrimDeregisterCnf),                                                  /* CI_MM_PRIM_DEREGISTER_CNF                      */
	0,                                                                              /* CI_MM_PRIM_GET_SIGQUALITY_IND_CONFIG_REQ       */
	sizeof(CiMmPrimGetSigQualityIndConfigCnf),                                      /* CI_MM_PRIM_GET_SIGQUALITY_IND_CONFIG_CNF       */
	sizeof(CiMmPrimSetSigQualityIndConfigReq),                                      /* CI_MM_PRIM_SET_SIGQUALITY_IND_CONFIG_REQ       */
	sizeof(CiMmPrimSetSigQualityIndConfigCnf),                                      /* CI_MM_PRIM_SET_SIGQUALITY_IND_CONFIG_CNF       */
	sizeof(CiMmPrimSigQualityInfoInd),                                              /* CI_MM_PRIM_SIGQUALITY_INFO_IND                 */

	sizeof(CiMmPrimEnableNetworkModeIndReq),                                        /* CI_MM_PRIM_ENABLE_NETWORK_MODE_IND_REQ         */
	sizeof(CiMmPrimEnableNetworkModeIndCnf),                                        /* CI_MM_PRIM_ENABLE_NETWORK_MODE_IND_CNF         */
	sizeof(CiMmPrimNetworkModeInd),                                                 /* CI_MM_PRIM_NETWORK_MODE_IND                    */
	0,                                                                              /* CI_MM_PRIM_GET_NITZ_INFO_REQ                   */
	sizeof(CiMmPrimGetNitzInfoCnf),                                                 /* CI_MM_PRIM_GET_NITZ_INFO_CNF                   */

	sizeof(CiMmPrimNitzInfoInd),                                                    /* CI_MM_PRIM_NITZ_INFO_IND                  */
	sizeof(CiMmPrimCipheringStatusInd),                                             /* CI_MM_PRIM_CIPHERING_STATUS_IND                */
/*Tal Porat/Michal Bukai - Network Selection*/
	sizeof(CiMmPrimAirInterfaceRejectCauseInd),                                     /* CI_MM_PRIM_AIR_INTERFACE_REJECT_CAUSE_IND  */
	/* Michal Bukai - Selection of preferred PLMN list +CPLS - START */
	sizeof(CiMmPrimSelectPreferredPlmnListReq),                                     /*CI_MM_PRIM_SELECT_PREFERRED_PLMN_LIST_REQ*/
	sizeof(CiMmPrimSelectPreferredPlmnListCnf),                                     /*CI_MM_PRIM_SELECT_PREFERRED_PLMN_LIST_CNF*/
	0,                                                                              /*CI_MM_PRIM_GET_PREFERRED_PLMN_LIST_REQ*/
	sizeof(CiMmPrimGetPreferredPlmnListCnf),                                        /*CI_MM_PRIM_GET_PREFERRED_PLMN_LIST_CNF*/
	/* Michal Bukai - Selection of preferred PLMN list +CPLS - END */
	sizeof(CiMmPrimBandIndInd),                                                     /*CI_MM_PRIM_BANDIND_IND */
	sizeof(CiMmPrimSetBandIndReq),                                                  /*CI_MM_PRIM_SET_BANDIND_REQ */
	sizeof(CiMmPrimSetBandIndCnf),                                                  /*CI_MM_PRIM_SET_BANDIND_CNF */
	0,                                                                              /*CI_MM_PRIM_GET_BANDIND_REQ */
	sizeof(CiMmPrimGetBandIndCnf),                                                  /*CI_MM_PRIM_GET_BANDIND_CNF */
    sizeof(CiMmPrimServiceRestrictionsInd),          /* CI_MM_PRIM_SERVICE_RESTRICTIONS_IND  */

    sizeof(CiMmPrimCellLockReq),					/*CI_MM_PRIM_CELL_LOCK_REQ*/
	sizeof(CiMmPrimCellLockCnf),					/*CI_MM_PRIM_CELL_LOCK_CNF*/
	sizeof(CiMmPrimCellLockInd),					/*CI_MM_PRIM_CELL_LOCK_IND*/
};

static const UINT16 CiPbPrimSize[CI_PB_NUM_PRIM + 1] =
{
	0,                                              /* NOT IN USE */
	0,                                              /* CI_PB_PRIM_GET_SUPPORTED_PHONEBOOKS_REQ            */
	sizeof(CiPbPrimGetSupportedPhonebooksCnf),      /* CI_PB_PRIM_GET_SUPPORTED_PHONEBOOKS_CNF            */
	sizeof(CiPbPrimGetPhonebookInfoReq),            /* CI_PB_PRIM_GET_PHONEBOOK_INFO_REQ                  */
	sizeof(CiPbPrimGetPhonebookInfoCnf),            /* CI_PB_PRIM_GET_PHONEBOOK_INFO_CNF                  */
	sizeof(CiPbPrimSelectPhonebookReq),             /* CI_PB_PRIM_SELECT_PHONEBOOK_REQ                    */
	sizeof(CiPbPrimSelectPhonebookCnf),             /* CI_PB_PRIM_SELECT_PHONEBOOK_CNF                    */
	0,                                              /* CI_PB_PRIM_READ_FIRST_PHONEBOOK_ENTRY_REQ          */
	sizeof(CiPbPrimReadFirstPhonebookEntryCnf),     /* CI_PB_PRIM_READ_FIRST_PHONEBOOK_ENTRY_CNF          */
	0,                                              /* CI_PB_PRIM_READ_NEXT_PHONEBOOK_ENTRY_REQ           */
	sizeof(CiPbPrimReadNextPhonebookEntryCnf),      /* CI_PB_PRIM_READ_NEXT_PHONEBOOK_ENTRY_CNF           */
	sizeof(CiPbPrimFindFirstPhonebookEntryReq),     /* CI_PB_PRIM_FIND_FIRST_PHONEBOOK_ENTRY_REQ          */
	sizeof(CiPbPrimFindFirstPhonebookEntryCnf),     /* CI_PB_PRIM_FIND_FIRST_PHONEBOOK_ENTRY_CNF          */
	sizeof(CiPbPrimFindNextPhonebookEntryReq),      /* CI_PB_PRIM_FIND_NEXT_PHONEBOOK_ENTRY_REQ           */
	sizeof(CiPbPrimFindNextPhonebookEntryCnf),      /* CI_PB_PRIM_FIND_NEXT_PHONEBOOK_ENTRY_CNF           */
	sizeof(CiPbPrimAddPhonebookEntryReq),           /* CI_PB_PRIM_ADD_PHONEBOOK_ENTRY_REQ                 */
	sizeof(CiPbPrimAddPhonebookEntryCnf),           /* CI_PB_PRIM_ADD_PHONEBOOK_ENTRY_CNF                 */
	sizeof(CiPbPrimDeletePhonebookEntryReq),        /* CI_PB_PRIM_DELETE_PHONEBOOK_ENTRY_REQ              */
	sizeof(CiPbPrimDeletePhonebookEntryCnf),        /* CI_PB_PRIM_DELETE_PHONEBOOK_ENTRY_CNF              */
	sizeof(CiPbPrimReplacePhonebookEntryReq),       /* CI_PB_PRIM_REPLACE_PHONEBOOK_ENTRY_REQ             */
	sizeof(CiPbPrimReplacePhonebookEntryCnf),       /* CI_PB_PRIM_REPLACE_PHONEBOOK_ENTRY_CNF             */
	sizeof(CiPbPrimReadPhonebookEntryReq),          /* CI_PB_PRIM_READ_PHONEBOOK_ENTRY_REQ                */
	sizeof(CiPbPrimReadPhonebookEntryCnf),          /* CI_PB_PRIM_READ_PHONEBOOK_ENTRY_CNF                */
	0,                                              /* CI_PB_PRIM_GET_IDENTITY_REQ                        */
	sizeof(CiPbPrimGetIdentityCnf),                 /* CI_PB_PRIM_GET_IDENTITY_CNF                        */
	sizeof(CiPbPrimPhoneBookReadyInd),              /* CI_PB_PRIM_PHONEBOOK_READY_IND                     */
	sizeof(CiPbPrimGetAlphaNameReq),                /* CI_PB_PRIM_GET_ALPHA_NAME_REQ                      */
	sizeof(CiPbPrimGetAlphaNameCnf),                /* CI_PB_PRIM_GET_ALPHA_NAME_CNF                      */
	sizeof(CiPbPrimReadEnhPbkEntryReq),             /* CI_PB_PRIM_READ_ENH_PBK_ENTRY_REQ */
	sizeof(CiPbPrimReadEnhPbkEntryCnf),             /* CI_PB_PRIM_READ_ENH_PBK_ENTRY_CNF */
	sizeof(CiPbPrimEditEnhPbkEntryReq),             /* CI_PB_PRIM_EDIT_ENH_PBK_ENTRY_REQ */
	sizeof(CiPbPrimEditEnhPbkEntryCnf),             /* CI_PB_PRIM_EDIT_ENH_PBK_ENTRY_CNF */
	sizeof(CiPbPrimReadCategoryNameReq),            /*CI_PB_PRIM_READ_CATEGORY_NAME_REQ*/
	sizeof(CiPbPrimReadCategoryNameCnf),            /*CI_PB_PRIM_READ_CATEGORY_NAME_CNF*/
	sizeof(CiPbPrimEditCategoryNameReq),            /*CI_PB_PRIM_EDIT_CATEGORY_NAME_REQ*/
	sizeof(CiPbPrimEditCategoryNameCnf),            /*CI_PB_PRIM_EDIT_CATEGORY_NAME_CNF*/
	sizeof(CiPbPrimReadGroupNameReq),               /*CI_PB_PRIM_READ_GROUP_NAME_REQ*/
	sizeof(CiPbPrimReadGroupNameCnf),               /*CI_PB_PRIM_READ_GROUP_NAME_CNF*/
	sizeof(CiPbPrimEditGroupNameReq),               /*CI_PB_PRIM_EDIT_GROUP_NAME_REQ*/
	sizeof(CiPbPrimEditGroupNameCnf)                /*CI_PB_PRIM_EDIT_GROUP_NAME_CNF */

};

static const UINT16 CiSimPrimSize[CI_SIM_NUM_PRIM + 1] =
{
	0,                                                                                              /* NOT IN USE */
	sizeof(CiSimPrimExecCmdReq),                                                                    /* CI_SIM_PRIM_EXECCMD_REQ                        */
	sizeof(CiSimPrimExecCmdCnf),                                                                    /* CI_SIM_PRIM_EXECCMD_CNF                        */
	sizeof(CiSimPrimDeviceInd),                                                                     /* CI_SIM_PRIM_DEVICE_IND                         */
	sizeof(CiSimPrimPersonalizeMEReq),                                                              /* CI_SIM_PRIM_PERSONALIZEME_REQ                  */
	sizeof(CiSimPrimPersonalizeMECnf),                                                              /* CI_SIM_PRIM_PERSONALIZEME_CNF                  */
	sizeof(CiSimPrimOperChvReq),                                                                    /* CI_SIM_PRIM_OPERCHV_REQ                        */
	sizeof(CiSimPrimOperChvCnf),                                                                    /* CI_SIM_PRIM_OPERCHV_CNF                        */
	sizeof(CiSimPrimDownloadProfileReq),                                                            /* CI_SIM_PRIM_DOWNLOADPROFILE_REQ                */
	sizeof(CiSimPrimDownloadProfileCnf),                                                            /* CI_SIM_PRIM_DOWNLOADPROFILE_CNF                */
	0,                                                                                              /* CI_SIM_PRIM_ENDATSESSION_IND                   */
	sizeof(CiSimPrimProactiveCmdInd),                                                               /* CI_SIM_PRIM_PROACTIVE_CMD_IND                  */
	sizeof(CiSimPrimProactiveCmdRsp),                                                               /* CI_SIM_PRIM_PROACTIVE_CMD_RSP                  */
	sizeof(CiSimPrimEnvelopeCmdReq),                                                                /* CI_SIM_PRIM_ENVELOPE_CMD_REQ                   */
	sizeof(CiSimPrimEnvelopeCmdCnf),                                                                /* CI_SIM_PRIM_ENVELOPE_CMD_CNF                   */
	0,                                                                                              /* CI_SIM_PRIM_GET_SUBSCRIBER_ID_REQ              */
	sizeof(CiSimPrimGetSubscriberIdCnf),                                                            /* CI_SIM_PRIM_GET_SUBSCRIBER_ID_CNF              */
	0,                                                                                              /* CI_SIM_PRIM_GET_PIN_STATE_REQ                  */
	sizeof(CiSimPrimGetPinStateCnf),                                                                /* CI_SIM_PRIM_GET_PIN_STATE_CNF                  */
	0,                                                                                              /* CI_SIM_PRIM_GET_TERMINALPROFILE_REQ            */
	sizeof(CiSimPrimGetTerminalProfileCnf),                                                         /* CI_SIM_PRIM_GET_TERMINALPROFILE_CNF            */
	sizeof(CiSimPrimEnableSimatIndsReq),                                                            /* CI_SIM_PRIM_ENABLE_SIMAT_INDS_REQ              */
	sizeof(CiSimPrimEnableSimatIndsCnf),                                                            /* CI_SIM_PRIM_ENABLE_SIMAT_INDS_CNF              */
	sizeof(CiSimPrimLockFacilityReq),                                                               /* CI_SIM_PRIM_LOCK_FACILITY_REQ                  */
	sizeof(CiSimPrimLockFacilityCnf),                                                               /* CI_SIM_PRIM_LOCK_FACILITY_CNF                  */
	0,                                                                                              /* CI_SIM_PRIM_GET_FACILITY_CAP_REQ               */
	sizeof(CiSimPrimGetFacilityCapCnf),                                                             /* CI_SIM_PRIM_GET_FACILITY_CAP_CNF               */
	0,                                                                                              /* CI_SIM_PRIM_GET_SIMAT_NOTIFY_CAP_REQ           */
	sizeof(CiSimPrimGetSimatNotifyCapCnf),                                                          /* CI_SIM_PRIM_GET_SIMAT_NOTIFY_CAP_CNF           */
	sizeof(CiSimPrimGetCallSetupAckInd),                                                            /* CI_SIM_PRIM_GET_CALL_SETUP_ACK_IND             */
	sizeof(CiSimPrimGetCallSetupAckRsp),                                                            /* CI_SIM_PRIM_GET_CALL_SETUP_ACK_RSP             */

    sizeof(CiSimPrimGetServiceProviderNameReq),         /* CI_SIM_PRIM_GET_SERVICE_PROVIDER_NAME_REQ      */
	sizeof(CiSimPrimGetServiceProviderNameCnf),                                                     /* CI_SIM_PRIM_GET_SERVICE_PROVIDER_NAME_CNF      */

	0,                                                                                              /* CI_SIM_PRIM_GET_MESSAGE_WAITING_INFO_REQ       */
	sizeof(CiSimPrimGetMessageWaitingInfoCnf),                                                      /* CI_SIM_PRIM_GET_MESSAGE_WAITING_INFO_CNF       */
	sizeof(CiSimPrimSetMessageWaitingInfoReq),                                                      /* CI_SIM_PRIM_SET_MESSAGE_WAITING_INFO_REQ       */
	sizeof(CiSimPrimGetSimServiceTableCnf),                                                         /* CI_SIM_PRIM_SET_MESSAGE_WAITING_INFO_CNF       */

	0,                                                                                              /* CI_SIM_PRIM_GET_SIM_SERVICE_TABLE_REQ          */
	sizeof(CiSimPrimGetSimServiceTableCnf),                                                         /* CI_SIM_PRIM_GET_SIM_SERVICE_TABLE_CNF          */

	0,                                                                                              /* CI_SIM_PRIM_GET_CUSTOMER_SERVICE_PROFILE_REQ   */
	sizeof(CiSimPrimGetCustomerServiceProfileCnf),                                                  /* CI_SIM_PRIM_GET_CUSTOMER_SERVICE_PROFILE_CNF   */

	sizeof(CiSimPrimSimatDisplayInfoInd),                                                           /* CI_SIM_PRIM_SIMAT_DISPLAY_INFO_IND             */

	0,                                                                                              /* CI_SIM_PRIM_GET_DEFAULT_LANGUAGE_REQ           */
	sizeof(CiSimPrimGetDefaultLanguageCnf),                                                         /* CI_SIM_PRIM_GET_DEFAULT_LANGUAGE_CNF           */

	sizeof(CiSimPrimGenericCmdReq),                                                                 /* CI_SIM_PRIM_GENERIC_CMD_REQ                    */
	sizeof(CiSimPrimGenericCmdCnf),                                                                 /* CI_SIM_PRIM_GENERIC_CMD_CNF                    */

	sizeof(CiSimPrimCardInd),                                                                       /* CI_SIM_PRIM_CARD_IND                           */

	sizeof(CiSimPrimIsEmergencyNumberReq),                                                          /* CI_SIM_PRIM_IS_EMERGENCY_NUMBER_REQ            */
	sizeof(CiSimPrimIsEmergencyNumberCnf),                                                          /* CI_SIM_PRIM_IS_EMERGENCY_NUMBER_CNF            */

	sizeof(CiSimPrimSimOwnedInd),                                                                   /* CI_SIM_PRIM_SIM_OWNED_IND                      */
	sizeof(CiSimPrimSimChangedInd),                                                                 /* CI_SIM_PRIM_SIM_CHANGED_IND                    */
	0,                                                                                              /* CI_SIM_PRIM_DEVICE_STATUS_REQ       */
	sizeof(CiSimPrimDeviceStatusCnf),                                                               /* CI_SIM_PRIM_DEVICE_STATUS_CNF       */
/**< Michal Bukai & VADIM SimLock - MEP UDP support - START */
	sizeof(CiSimPrimReadMEPCodesReq),                                                               /* CI_SIM_PRIM_READ_MEP_CODES_REQ */
	sizeof(CiSimPrimReadMEPCodesCnf),                                                               /* CI_SIM_PRIM_READ_MEP_CODES_CNF */
	sizeof(CiSimPrimUDPLockReq),                                                                    /* CI_SIM_PRIM_UDP_LOCK_REQ */
	sizeof(CiSimPrimUDPLockCnf),                                                                    /* CI_SIM_PRIM_UDP_LOCK_CNF */
	sizeof(CiSimPrimUDPChangePasswordReq),                                                          /* CI_SIM_PRIM_UDP_CHANGE_PASSWORD_REQ */
	sizeof(CiSimPrimUDPChangePasswordCnf),                                                          /* CI_SIM_PRIM_UDP_CHANGE_PASSWORD_CNF */
	sizeof(CiSimPrimUDPASLReq),                                                                     /* CI_SIM_PRIM_UDP_ASL_REQ */
	sizeof(CiSimPrimUDPASLCnf),                                                                     /* CI_SIM_PRIM_UDP_ASL_CNF */
/**< Michal Bukai & VADIM SimLock - MEP UDP support - END */
/**< Michal Bukai - Virtual SIM support - START */
	0,                                                                                      /* CI_SIM_PRIM_SET_VSIM_REQ */
	sizeof(CiSimPrimSetVSimCnf),                                                            /* CI_SIM_PRIM_SET_VSIM_CNF */
	0,                                                                                      /* CI_SIM_PRIM_GET_VSIM_REQ */
	sizeof(CiSimPrimGetVSimCnf),                                                            /* CI_SIM_PRIM_GET_VSIM_CNF */
/**< Michal Bukai - Virtual SIM support - END */
/*Michal Bukai - OTA support for AT&T - START*/
	0,                                                                                      /*CI_SIM_PRIM_CHECK_MMI_STATE_IND*/
	sizeof(CiSimPrimCheckMMIStateRsp),                                                       /*CI_SIM_PRIM_CHECK_MMI_STATE_RSP*/
/*Michal Bukai - OTA support for AT&T - END*/
/*Michal Bukai - additional SIMAT primitives - START*/
/*new add five IND */
	sizeof(CiSimPrimSIMATCcStatusInd),
	sizeof(CiSimPrimSIMATSendCallSetupRspInd),
	sizeof(CiSimPrimSIMATSendSsUssdRspInd),
	sizeof(CiSimPrimSIMATSmControlStatusInd),
	sizeof(CiSimPrimSIMATSendSmRspInd)
/*Michal Bukai - additional SIMAT primitives - END*/

};

const UINT16 CiMsgPrimSize[CI_MSG_NUM_PRIM + 1] =
{
	0,                                              /* NOT IN USE */
	0,                                              /* CI_MSG_PRIM_GET_SUPPORTED_SERVICES_REQ                 */
	sizeof(CiMsgPrimGetSupportedServicesCnf),       /* CI_MSG_PRIM_GET_SUPPORTED_SERVICES_CNF,                */
	sizeof(CiMsgPrimSelectServiceReq),              /* CI_MSG_PRIM_SELECT_SERVICE_REQ,                        */
	sizeof(CiMsgPrimSelectServiceCnf),              /* CI_MSG_PRIM_SELECT_SERVICE_CNF,                        */
	0,                                              /* CI_MSG_PRIM_GET_CURRENT_SERVICE_INFO_REQ,              */
	sizeof(CiMsgPrimGetCurrentServiceInfoCnf),      /* CI_MSG_PRIM_GET_CURRENT_SERVICE_INFO_CNF,              */
	0,                                              /* CI_MSG_PRIM_GET_SUPPORTED_STORAGES_REQ,                */
	sizeof(CiMsgPrimGetSupportedStoragesCnf),       /* CI_MSG_PRIM_GET_SUPPORTED_STORAGES_CNF,                */
	sizeof(CiMsgPrimSelectStorageReq),              /* CI_MSG_PRIM_SELECT_STORAGE_REQ,                        */
	sizeof(CiMsgPrimSelectStorageCnf),              /* CI_MSG_PRIM_SELECT_STORAGE_CNF,                        */
	0,                                              /* CI_MSG_PRIM_GET_CURRENT_STORAGE_INFO_REQ,              */
	sizeof(CiMsgPrimGetCurrentStorageInfoCnf),      /* CI_MSG_PRIM_GET_CURRENT_STORAGE_INFO_CNF,              */
	sizeof(CiMsgPrimReadMessageReq),                /* CI_MSG_PRIM_READ_MESSAGE_REQ,                          */
	sizeof(CiMsgPrimReadMessageCnf),                /* CI_MSG_PRIM_READ_MESSAGE_CNF,                          */
	sizeof(CiMsgPrimDeleteMessageReq),              /* CI_MSG_PRIM_DELETE_MESSAGE_REQ,                        */
	sizeof(CiMsgPrimDeleteMessageCnf),              /* CI_MSG_PRIM_DELETE_MESSAGE_CNF,                        */
	sizeof(CiMsgPrimSendMessageReq),                /* CI_MSG_PRIM_SEND_MESSAGE_REQ,                          */
	sizeof(CiMsgPrimSendMessageCnf),                /* CI_MSG_PRIM_SEND_MESSAGE_CNF,                          */
	sizeof(CiMsgPrimWriteMessageReq),               /* CI_MSG_PRIM_WRITE_MESSAGE_REQ,                         */
	sizeof(CiMsgPrimWriteMessageCnf),               /* CI_MSG_PRIM_WRITE_MESSAGE_CNF,                         */
	sizeof(CiMsgPrimSendCommandReq),                /* CI_MSG_PRIM_SEND_COMMAND_REQ,                          */
	sizeof(CiMsgPrimSendCommandCnf),                /* CI_MSG_PRIM_SEND_COMMAND_CNF,                          */
	sizeof(CiMsgPrimSendStoredMessageReq),          /* CI_MSG_PRIM_SEND_STORED_MESSAGE_REQ,                   */
	sizeof(CiMsgPrimSendStoredMessageCnf),          /* CI_MSG_PRIM_SEND_STORED_MESSAGE_CNF,                   */
	sizeof(CiMsgPrimConfigMsgIndReq),               /* CI_MSG_PRIM_CONFIG_MSG_IND_REQ,                        */
	sizeof(CiMsgPrimConfigMsgIndCnf),               /* CI_MSG_PRIM_CONFIG_MSG_IND_CNF,                        */
	sizeof(CiMsgPrimNewMsgIndexInd),                /* CI_MSG_PRIM_NEWMSG_INDEX_IND,                          */
	sizeof(CiMsgPrimNewMsgInd),                     /* CI_MSG_PRIM_NEWMSG_IND,                                */
	sizeof(CiMsgPrimNewMsgRsp),                     /* CI_MSG_PRIM_NEWMSG_RSP,                                */
	0,                                              /* CI_MSG_PRIM_GET_SMSC_ADDR_REQ,                         */
	sizeof(CiMsgPrimGetSmscAddrCnf),                /* CI_MSG_PRIM_GET_SMSC_ADDR_CNF,                         */
	sizeof(CiMsgPrimSetSmscAddrReq),                /* CI_MSG_PRIM_SET_SMSC_ADDR_REQ,                         */
	sizeof(CiMsgPrimSetSmscAddrCnf),                /* CI_MSG_PRIM_SET_SMSC_ADDR_CNF,                         */
	0,                                              /* CI_MSG_PRIM_GET_MOSMS_SERVICE_CAP_REQ,                 */
	sizeof(CiMsgPrimGetMoSmsServiceCapCnf),         /* CI_MSG_PRIM_GET_MOSMS_SERVICE_CAP_CNF,                 */
	0,                                              /* CI_MSG_PRIM_GET_MOSMS_SERVICE_REQ,                     */
	sizeof(CiMsgPrimGetMoSmsServiceCnf),            /* CI_MSG_PRIM_GET_MOSMS_SERVICE_CNF,                     */
	sizeof(CiMsgPrimSetMoSmsServiceReq),            /* CI_MSG_PRIM_SET_MOSMS_SERVICE_REQ,                     */
	sizeof(CiMsgPrimSetMoSmsServiceCnf),            /* CI_MSG_PRIM_SET_MOSMS_SERVICE_CNF,                     */
	0,                                              /* CI_MSG_PRIM_GET_CBM_TYPES_CAP_REQ,                     */
	sizeof(CiMsgPrimGetCbmTypesCapCnf),             /* CI_MSG_PRIM_GET_CBM_TYPES_CAP_CNF,                     */
	0,                                              /* CI_MSG_PRIM_GET_CBM_TYPES_REQ,                         */
	sizeof(CiMsgPrimGetCbmTypesCnf),                /* CI_MSG_PRIM_GET_CBM_TYPES_CNF,                         */
	sizeof(CiMsgPrimSetCbmTypesReq),                /* CI_MSG_PRIM_SET_CBM_TYPES_REQ,                         */
	sizeof(CiMsgPrimSetCbmTypesCnf),                /* CI_MSG_PRIM_SET_CBM_TYPES_CNF,                         */
	sizeof(CiMsgPrimSelectStoragesReq),             /* CI_MSG_PRIM_SELECT_STORAGES_REQ,                       */
	sizeof(CiMsgPrimSelectStoragesCnf),             /* CI_MSG_PRIM_SELECT_STORAGES_CNF,                       */

	sizeof(CiMsgPrimWriteMsgWithIndexReq),          /* CI_MSG_PRIM_WRITE_MSG_WITH_INDEX_REQ,                  */
	sizeof(CiMsgPrimWriteMsgWithIndexCnf),          /* CI_MSG_PRIM_WRITE_MSG_WITH_INDEX_CNF,                  */

	sizeof(CiMsgPrimMessageWaitingInd),             /* CI_MSG_PRIM_MESSAGE_WAITING_IND,                       */
	sizeof(CiMsgPrimStorageStatusInd),              /* CI_MSG_PRIM_STORAGE_STATUS_IND,                        */

	/*Michal Bukai - SMS Memory Full Feature - start*/
#ifdef AT_CUSTOMER_HTC
	sizeof(CiMsgPrimResetMemcapFullReq),                                                    /* CI_MSG_PRIM_RESET_MEMCAP_FULL_REQ */
#else
	0,                                                                                      /* CI_MSG_PRIM_RESET_MEMCAP_FULL_REQ */
#endif
	sizeof(CiMsgPrimResetMemcapFullCnf),                                                     /* CI_MSG_PRIM_RESET_MEMCAP_FULL_CNF */
	/*Michal Bukai - SMS Memory Full Feature - end*/

	sizeof(CiMsgPrimSetRecordStatusReq),             /*CI_MSG_PRIM_SET_RECORD_STATUS_REQ*/
	sizeof(CiMsgPrimSetRecordStatusCnf),             /*CI_MSG_PRIM_SET_RECORD_STATUS_CNF*/
#ifdef AT_CUSTOMER_HTC
	sizeof(CiMsgPrimSmsRejectCauseInd),               /*CI_MSG_PRIM_SMS_REJECT_CAUSE_IND*/
#endif
	sizeof(CiMsgPrimLockSmsStatusReq),               /*CI_MSG_PRIM_LOCK_SMS_STATUS_REQ*/
	sizeof(CiMsgPrimLockSmsStatusCnf),               /*CI_MSG_PRIM_LOCK_SMS_STATUS_CNF*/
};

static const UINT16 CiPsPrimSize[CI_PS_NUM_PRIM + 1] =
{
	0,                                                      /* NOT IN USE */
	sizeof(CiPsPrimSetAttachStateReq),                      /* CI_PS_PRIM_SET_ATTACH_STATE_REQ                    */
	sizeof(CiPsPrimSetAttachStateCnf),                      /* CI_PS_PRIM_SET_ATTACH_STATE_CNF                    */
	0,                                                      /* CI_PS_PRIM_GET_ATTACH_STATE_REQ                    */
	sizeof(CiPsPrimGetAttachStateCnf),                      /* CI_PS_PRIM_GET_ATTACH_STATE_CNF                    */
	sizeof(CiPsPrimDefinePdpCtxReq),                        /* CI_PS_PRIM_DEFINE_PDP_CTX_REQ                      */
	sizeof(CiPsPrimDeletePdpCtxCnf),                        /* CI_PS_PRIM_DEFINE_PDP_CTX_CNF                      */
	sizeof(CiPsPrimDeletePdpCtxReq),                        /* CI_PS_PRIM_DELETE_PDP_CTX_REQ                      */
	sizeof(CiPsPrimDeletePdpCtxCnf),                        /* CI_PS_PRIM_DELETE_PDP_CTX_CNF                      */
	sizeof(CiPsPrimGetPdpCtxReq),                           /* CI_PS_PRIM_GET_PDP_CTX_REQ                         */
	sizeof(CiPsPrimGetPdpCtxCnf),                           /* CI_PS_PRIM_GET_PDP_CTX_CNF                         */
	0,                                                      /* CI_PS_PRIM_GET_PDP_CTX_CAPS_REQ                    */
	sizeof(CiPsPrimGetPdpCtxCapsCnf),                       /* CI_PS_PRIM_GET_PDP_CTX_CAPS_CNF                    */
	sizeof(CiPsPrimSetPdpCtxActStateReq),                   /* CI_PS_PRIM_SET_PDP_CTX_ACT_STATE_REQ               */
	sizeof(CiPsPrimSetPdpCtxActStateCnf),                   /* CI_PS_PRIM_SET_PDP_CTX_ACT_STATE_CNF               */
	0,                                                      /* CI_PS_PRIM_GET_PDP_CTXS_ACT_STATE_REQ              */
	sizeof(CiPsPrimGetPdpCtxsActStateCnf),                  /* CI_PS_PRIM_GET_PDP_CTXS_ACT_STATE_CNF              */
	sizeof(CiPsPrimEnterDataStateReq),                      /* CI_PS_PRIM_ENTER_DATA_STATE_REQ                    */
	sizeof(CiPsPrimEnterDataStateCnf),                      /* CI_PS_PRIM_ENTER_DATA_STATE_CNF                    */
	sizeof(CiPsPrimMtPdpCtxActModifyInd),                   /* CI_PS_PRIM_MT_PDP_CTX_ACT_MODIFY_IND */
	sizeof(CiPsPrimMtPdpCtxActModifyRsp),                   /* CI_PS_PRIM_MT_PDP_CTX_ACT_MODIFY_RSP */
	sizeof(CiPsPrimMtPdpCtxActedInd),                       /* CI_PS_PRIM_MT_PDP_CTX_ACTED_IND                    */
	sizeof(CiPsPrimSetGsmGprsClassReq),                     /* CI_PS_PRIM_SET_GSMGPRS_CLASS_REQ                   */
	sizeof(CiPsPrimSetGsmGprsClassCnf),                     /* CI_PS_PRIM_SET_GSMGPRS_CLASS_CNF                   */
	0,                                                      /* CI_PS_PRIM_GET_GSMGPRS_CLASS_REQ                   */
	sizeof(CiPsPrimGetGsmGprsClassCnf),                     /* CI_PS_PRIM_GET_GSMGPRS_CLASS_CNF                   */
	0,                                                      /* CI_PS_PRIM_GET_GSMGPRS_CLASSES_REQ                 */
	sizeof(CiPsPrimGetGsmGprsClassesCnf),                   /* CI_PS_PRIM_GET_GSMGPRS_CLASSES_CNF                 */
	sizeof(CiPsPrimEnableNwRegIndReq),                      /* CI_PS_PRIM_ENABLE_NW_REG_IND_REQ                   */
	sizeof(CiPsPrimEnableNwRegIndCnf),                      /* CI_PS_PRIM_ENABLE_NW_REG_IND_CNF                   */
	sizeof(CiPsPrimNwRegInd),                               /* CI_PS_PRIM_NW_REG_IND                              */
	sizeof(CiPsPrimSetQosReq),                              /* CI_PS_PRIM_SET_QOS_REQ                             */
	sizeof(CiPsPrimSetQosCnf),                              /* CI_PS_PRIM_SET_QOS_CNF                             */
	sizeof(CiPsPrimDelQosReq),                              /* CI_PS_PRIM_DEL_QOS_REQ                             */
	sizeof(CiPsPrimDelQosCnf),                              /* CI_PS_PRIM_DEL_QOS_CNF                             */
	sizeof(CiPsPrimGetQosReq),                              /* CI_PS_PRIM_GET_QOS_REQ                             */
	sizeof(CiPsPrimGetQosCnf),                              /* CI_PS_PRIM_GET_QOS_CNF                             */
	sizeof(CiPsPrimEnablePoweronAutoAttachReq),             /* CI_PS_PRIM_ENABLE_POWERON_AUTO_ATTACH_REQ          */
	sizeof(CiPsPrimEnablePoweronAutoAttachCnf),             /* CI_PS_PRIM_ENABLE_POWERON_AUTO_ATTACH_CNF          */
	sizeof(CiPsPrimMtPdpCtxRejectedInd),                    /* CI_PS_PRIM_MT_PDP_CTX_REJECTED_IND                 */
	sizeof(CiPsPrimPdpCtxDeactedInd),                       /* CI_PS_PRIM_PDP_CTX_DEACTED_IND                     */
	sizeof(CiPsPrimPdpCtxReactedInd),                       /* CI_PS_PRIM_PDP_CTX_REACTED_IND                     */
	sizeof(CiPsPrimDetachedInd),                            /* CI_PS_PRIM_DETACHED_IND                            */
	sizeof(CiPsPrimGprsClassChangedInd),                    /* CI_PS_PRIM_GPRS_CLASS_CHANGED_IND                  */
	0,                                                      /* CI_PS_PRIM_GET_DEFINED_CID_LIST_REQ                */
	sizeof(CiPsPrimGetDefinedCidListCnf),                   /* CI_PS_PRIM_GET_DEFINED_CID_LIST_CNF                */
	0,                                                      /* CI_PS_PRIM_GET_NW_REG_STATUS_REQ                   */
	sizeof(CiPsPrimGetNwRegStatusCnf),                      /* CI_PS_PRIM_GET_NW_REG_STATUS_CNF                   */
	sizeof(CiPsPrimGetQosCapsReq),                          /* CI_PS_PRIM_GET_QOS_CAPS_REQ                        */
	sizeof(CiPsPrimGetQosCapsCnf),                          /* CI_PS_PRIM_GET_QOS_CAPS_CNF                        */
	sizeof(CiPsPrimEnableEventsReportingReq),               /* CI_PS_PRIM_ENABLE_EVENTS_REPORTING_REQ             */
	sizeof(CiPsPrimEnableEventsReportingCnf),               /* CI_PS_PRIM_ENABLE_EVENTS_REPORTING_CNF             */

	sizeof(CiPsPrimGet3GQosReq),                            /* CI_PS_PRIM_GET_3G_QOS_REQ                          */
	sizeof(CiPsPrimGet3GQosCnf),                            /* CI_PS_PRIM_GET_3G_QOS_CNF                          */
	sizeof(CiPsPrimSet3GQosReq),                            /* CI_PS_PRIM_SET_3G_QOS_REQ                          */
	sizeof(CiPsPrimSet3GQosCnf),                            /* CI_PS_PRIM_SET_3G_QOS_CNF                          */
	sizeof(CiPsPrimDel3GQosReq),                            /* CI_PS_PRIM_DEL_3G_QOS_REQ                          */
	sizeof(CiPsPrimDel3GQosCnf),                            /* CI_PS_PRIM_DEL_3G_QOS_CNF                          */
	sizeof(CiPsPrimGet3GQosCapsReq),                        /* CI_PS_PRIM_GET_3G_QOS_CAPS_REQ                     */
	sizeof(CiPsPrimGet3GQosCapsCnf),                        /* CI_PS_PRIM_GET_3G_QOS_CAPS_CNF                     */

	sizeof(CiPsPrimDefineSecPdpCtxReq),                     /* CI_PS_PRIM_DEFINE_SEC_PDP_CTX_REQ                  */
	sizeof(CiPsPrimDefineSecPdpCtxCnf),                     /* CI_PS_PRIM_DEFINE_SEC_PDP_CTX_CNF                  */
	sizeof(CiPsPrimDeleteSecPdpCtxReq),                     /* CI_PS_PRIM_DELETE_SEC_PDP_CTX_REQ                  */
	sizeof(CiPsPrimDeleteSecPdpCtxCnf),                     /* CI_PS_PRIM_DELETE_SEC_PDP_CTX_CNF                  */
	sizeof(CiPsPrimGetSecPdpCtxReq),                        /* CI_PS_PRIM_GET_SEC_PDP_CTX_REQ                     */
	sizeof(CiPsPrimGetSecPdpCtxCnf),                        /* CI_PS_PRIM_GET_SEC_PDP_CTX_CNF                     */

	sizeof(CiPsPrimDefineTftFilterReq),                     /* CI_PS_PRIM_DEFINE_TFT_FILTER_REQ                   */
	sizeof(CiPsPrimDefineTftFilterCnf),                     /* CI_PS_PRIM_DEFINE_TFT_FILTER_CNF                   */
	sizeof(CiPsPrimDeleteTftReq),                           /* CI_PS_PRIM_DELETE_TFT_REQ                          */
	sizeof(CiPsPrimDeleteTftCnf),                           /* CI_PS_PRIM_DELETE_TFT_CNF                          */
	sizeof(CiPsPrimGetTftReq),                              /* CI_PS_PRIM_GET_TFT_REQ                             */
	sizeof(CiPsPrimGetTftCnf),                              /* CI_PS_PRIM_GET_TFT_CNF                             */

	sizeof(CiPsPrimModifyPdpCtxReq),                        /* CI_PS_PRIM_MODIFY_PDP_CTX_REQ                      */
	sizeof(CiPsPrimModifyPdpCtxCnf),                        /* CI_PS_PRIM_MODIFY_PDP_CTX_CNF                      */
	0,                                                      /* CI_PS_PRIM_GET_ACTIVE_CID_LIST_REQ                 */
	sizeof(CiPsPrimGetActiveCidListCnf),                    /* CI_PS_PRIM_GET_ACTIVE_CID_LIST_CNF                 */

	sizeof(CiPsPrimReportCounterReq),                       /* CI_PS_PRIM_REPORT_COUNTER_REQ                      */
	sizeof(CiPsPrimReportCounterCnf),                       /* CI_PS_PRIM_REPORT_COUNTER_CNF                      */
	sizeof(CiPsPrimResetCounterReq),                        /* CI_PS_PRIM_RESET_COUNTER_REQ                       */
	sizeof(CiPsPrimResetCounterCnf),                        /* CI_PS_PRIM_RESET_COUNTER_CNF                       */
	sizeof(CiPsPrimCounterInd),                             /* CI_PS_PRIM_COUNTER_IND                             */
	sizeof(CiPsPrimSendDataReq),                            /* CI_PS_PRIM_SEND_DATA_REQ                           */
	sizeof(CiPsPrimSendDataCnf),                            /* CI_PS_PRIM_SEND_DATA_CNF                           */

	sizeof(CiPsPrimSetAclReq),                              /* CI_PS_PRIM_SET_ACL_SERVICE_REQ */
	sizeof(CiPsPrimSetAclCnf),                              /* CI_PS_PRIM_SET_ACL_SERVICE_CNF */
	0,                                                      /*CI_PS_PRIM_GET_ACL_SIZE_REQ*/
	sizeof(CiPsPrimGetAclSizeCnf),                          /* CI_PS_PRIM_GET_ACL_SIZE_CNF */
	sizeof(CiPsPrimReadAclEntryReq),                        /* CI_PS_PRIM_READ_ACL_ENTRY_REQ */
	sizeof(CiPsPrimReadAclEntryCnf),                        /* CI_PS_PRIM_READ_ACL_ENTRY_CNF */
	sizeof(CiPsPrimEditAclEntryReq),                        /* CI_PS_PRIM_EDIT_ACL_ENTRY_REQ */
	sizeof(CiPsPrimEditAclEntryCnf),                         /* CI_PS_PRIM_EDIT_ACL_ENTRY_CNF */
/* Michal Bukai ?PDP authentication - Start*/
	sizeof(CiPsPrimAuthenticateReq), /*CI_PS_PRIM_AUTHENTICATE_REQ*/
	sizeof(CiPsPrimAuthenticateCnf),  /*CI_PS_PRIM_AUTHENTICATE_REQ*/
/* Michal Bukai ?PDP authentication - End*/

/* Michal Bukai ?Fast Dormancy - Start*/
	0,                               /*CI_PS_PRIM_FAST_DORMANT_REQ*/
   sizeof(CiPsPrimFastDormantCnf),  /*CI_PS_PRIM_FAST_DORMANT_CNF*/
/* Michal Bukai ?Fast Dormancy - End*/

	0,								 /*CI_PS_PRIM_GET_CURRENT_JOB_REQ*/
	sizeof(CiPsPrimGetCurrentJobCnf), /*CI_PS_PRIM_GET_CURRENT_JOB_CNF*/
#ifdef AT_CUSTOMER_HTC
	sizeof(CiPsPrimSetFastDormancyConfigReq),  /*CI_PS_PRIM_SET_FAST_DORMANCY_CONFIG_REQ*/
	sizeof(CiPsPrimSetFastDormancyConfigCnf),  /*CI_PS_PRIM_SET_FAST_DORMANCY_CONFIG_CNF*/

	sizeof(CiPsPrimPdpActivationRejectCauseInd) /*CI_PS_PRIM_PDP_ACTIVATION_REJECT_CAUSE_IND*/
#endif
};

static const UINT16 CiDatPrimSize[CI_DAT_NUM_PRIM + 1] =
{
	0,                                      /* NOT USED */
	0,                                      /* CI_DAT_PRIM_GET_MAX_PDU_SIZE_REQ                   */
	sizeof(CiDatPrimGetMaxPduSizeCnf),      /* CI_DAT_PRIM_GET_MAX_PDU_SIZE_CNF                   */
	sizeof(CiDatPrimSendReq),               /* CI_DAT_PRIM_SEND_REQ                               */
	sizeof(CiDatPrimSendCnf),               /* CI_DAT_PRIM_SEND_CNF                               */
	sizeof(CiDatPrimRecvInd),               /* CI_DAT_PRIM_RECV_IND                               */
	sizeof(CiDatPrimRecvRsp),               /* CI_DAT_PRIM_RECV_RSP                               */
	sizeof(CiDatPrimOkInd),                 /* CI_DAT_PRIM_OK_IND                                 */
	sizeof(CiDatPrimNokInd),                /* CI_DAT_PRIM_NOK_IND  */
  sizeof(CiDatPrimSendDataOptReq),           /*CI_DAT_PRIM_SEND_DATA_OPT_REQ             */
  sizeof(CiDatPrimSendDataOptCnf),           /*CI_DAT_PRIM_SEND_DATA_OPT_CNF             */
  sizeof(CiDatPrimRecvDataOptInd),             /*CI_DAT_PRIM_RECV_DATA_OPT_IND             */
  sizeof(CiDatPrimRecvDataOptRsp),             /*CI_DAT_PRIM_RECV_DATA_OPT_RSP             */
	sizeof(CiDatPrimSetUlTpIndReq),                                         /* CI_DAT_PRIM_SET_UL_TP_IND_REQ */
	sizeof(CiDatPrimSetUlTpIndCnf),                                         /* CI_DAT_PRIM_SET_UL_TP_IND_CNF */
	sizeof(CiDatPrimUlTpInd)                                                /* CI_DAT_PRIM_UL_TP_IND */
};

static const UINT16 CiDevPrimSize[CI_DEV_NUM_PRIM + 1] =
{
	0,                                              /* NOT IN USE */
	sizeof(CiDevPrimStatusInd),                     /* CI_DEV_PRIM_STATUS_IND                             */
	0,                                              /* CI_DEV_PRIM_GET_MANU_ID_REQ                        */
	sizeof(CiDevPrimGetManuIdCnf),                  /* CI_DEV_PRIM_GET_MANU_ID_CNF                        */
	0,                                              /* CI_DEV_PRIM_GET_MODEL_ID_REQ                       */
	sizeof(CiDevPrimGetModelIdCnf),                 /* CI_DEV_PRIM_GET_MODEL_ID_CNF,                      */
	0,                                              /* CI_DEV_PRIM_GET_REVISION_ID_REQ                    */
	sizeof(CiDevPrimGetRevisionIdCnf),              /* CI_DEV_PRIM_GET_REVISION_ID_CNF                    */
	0,                                              /* CI_DEV_PRIM_GET_SERIALNUM_ID_REQ                   */
	sizeof(CiDevPrimGetSerialNumIdCnf),             /* CI_DEV_PRIM_GET_SERIALNUM_ID_CNF                   */
	sizeof(CiDevPrimSetFuncReq),                    /* CI_DEV_PRIM_SET_FUNC_REQ                           */
	sizeof(CiDevPrimSetFuncCnf),                    /* CI_DEV_PRIM_SET_FUNC_CNF                           */
	0,                                              /* CI_DEV_PRIM_GET_FUNC_REQ                           */
	sizeof(CiDevPrimGetFuncCnf),                    /* CI_DEV_PRIM_GET_FUNC_CNF                           */
	0,                                              /* CI_DEV_PRIM_GET_FUNC_CAP_REQ                       */
	sizeof(CiDevPrimGetFuncCapCnf),                 /* CI_DEV_PRIM_GET_FUNC_CAP_CNF                       */
	sizeof(CiDevPrimSetGsmPowerClassReq),           /* CI_DEV_PRIM_SET_GSM_POWER_CLASS_REQ                */
	sizeof(CiDevPrimSetGsmPowerClassCnf),           /* CI_DEV_PRIM_SET_GSM_POWER_CLASS_CNF                */
	sizeof(CiDevPrimGetGsmPowerClassReq),           /* CI_DEV_PRIM_GET_GSM_POWER_CLASS_REQ                */
	sizeof(CiDevPrimGetGsmPowerClassCnf),           /* CI_DEV_PRIM_GET_GSM_POWER_CLASS_CNF                */
	sizeof(CiDevPrimGetGsmPowerClassCapReq),        /* CI_DEV_PRIM_GET_GSM_POWER_CLASS_CAP_REQ            */
	sizeof(CiDevPrimGetGsmPowerClassCapCnf),        /* CI_DEV_PRIM_GET_GSM_POWER_CLASS_CAP_CNF            */

	sizeof(CiDevPrimPmPowerDownReq),                /* CI_DEV_PRIM_PM_POWER_DOWN_REQ                      */
	sizeof(CiDevPrimPmPowerDownCnf),                /* CI_DEV_PRIM_PM_POWER_DOWN_CNF                      */

	sizeof(CiDevPrimSetEngmodeRepOptReq),           /* CI_DEV_PRIM_SET_ENGMODE_REPORT_OPTION_REQ          */
	sizeof(CiDevPrimSetEngmodeRepOptCnf),           /* CI_DEV_PRIM_SET_ENGMODE_REPORT_OPTION_CNF          */
	0,                                              /* CI_DEV_PRIM_GET_ENGMODE_INFO_REQ                   */
	sizeof(CiDevPrimGetEngmodeInfoCnf),             /* CI_DEV_PRIM_GET_ENGMODE_INFO_CNF                   */
	sizeof(CiDevPrimEngmodeInfoInd),                /* CI_DEV_PRIM_ENGMODE_INFO_IND                       */

	sizeof(CiDevPrimGsmEngmodeInfoInd),             /* CI_DEV_PRIM_GSM_ENGMODE_INFO_IND                   */
	sizeof(CiDevPrimUmtsEngmodeScellInfoInd),       /* CI_DEV_PRIM_UMTS_ENGMODE_SVCCELL_INFO_IND          */
	sizeof(CiDevPrimUmtsEngmodeIntraFreqInfoInd),   /* CI_DEV_PRIM_UMTS_ENGMODE_INTRAFREQ_INFO_IND        */
	sizeof(CiDevPrimUmtsEngmodeInterFreqInfoInd),   /* CI_DEV_PRIM_UMTS_ENGMODE_INTERFREQ_INFO_IND        */
	sizeof(CiDevPrimUmtsEngmodeInterRatInfoInd),    /* CI_DEV_PRIM_UMTS_ENGMODE_INTERRAT_INFO_IND         */
	sizeof(CiDevPrimDoSelfTestReq),                 /* CI_DEV_PRIM_DO_SELF_TEST_REQ                       */
	sizeof(CiDevPrimDoSelfTestCnf),                 /* CI_DEV_PRIM_DO_SELF_TEST_CNF                       */
	sizeof(CiDevPrimDoSelfTestInd),                 /* CI_DEV_PRIM_DO_SELF_TEST_IND                       */
	sizeof(CiDevPrimSetRfsReq),                     /* CI_DEV_PRIM_SET_RFS_REQ                            */
	sizeof(CiDevPrimSetRfsCnf),                     /* CI_DEV_PRIM_SET_RFS_CNF                            */
	sizeof(CiDevPrimGetRfsReq),                     /* CI_DEV_PRIM_GET_RFS_REQ                            */
	sizeof(CiDevPrimGetRfsCnf),                     /* CI_DEV_PRIM_GET_RFS_CNF                            */
	sizeof(CiDevPrimUmtsEngmodeActiveSetInfo),      /* CI_DEV_PRIM_UMTS_ENGMODE_ACTIVE_SET_INFO_IND */
	sizeof(CiDevPrimActivePDPContextEngModeInd),    /* CI_DEV_PRIM_ACTIVE_PDP_CONTEXT_ENGMODE_IND */
	0,                                              /* CI_DEV_PRIM_NETWORK_MONITOR_INFO_IND */
	sizeof(CiDevPrimLpNwulMsgReq),                  /* CI_DEV_PRIM_LP_NWUL_MSG_REQ */
	sizeof(CiDevPrimLpNwulMsgCnf),                  /* CI_DEV_PRIM_LP_NWUL_MSG_CNF */
	sizeof(CiDevPrimLpNwdlMsgInd),                  /* CI_DEV_PRIM_LP_NWDL_MSG_IND */
	sizeof(CiDevPrimLpRrcStateInd),                 /* CI_DEV_PRIM_LP_RRC_STATE_IND */
	sizeof(CiDevPrimLpMeasTerminateInd),            /* CI_DEV_PRIM_LP_MEAS_TERMINATE_IND */
	sizeof(CiDevPrimLpResetStoreUePosInd),          /* CI_DEV_PRIM_LP_RESET_STORED_UE_POS_IND */
	/*Michal Bukai - Silent Reset support - START*/
	0,                                              /*CI_DEV_PRIM_COMM_ASSERT_REQ*/
	/*Michal Bukai - Silent Reset support - END*/
	/*Michal Bukai *BAND support - START*/
	sizeof(CiDevPrimSetBandModeReq),                /*CI_DEV_PRIM_SET_BAND_MODE_REQ*/
	sizeof(CiDevPrimSetBandModeCnf),                /*CI_DEV_PRIM_SET_BAND_MODE_CNF*/
	0,                                              /*CI_DEV_PRIM_GET_BAND_MODE_REQ*/
	sizeof(CiDevPrimGetBandModeCnf),                /*CI_DEV_PRIM_GET_BAND_MODE_CNF*/
	0,                                              /*CI_DEV_PRIM_GET_SUPPORTED_BAND_MODE_REQ*/
	sizeof(CiDevPrimGetSupportedBandModeCnf),       /*CI_DEV_PRIM_GET_SUPPORTED_BAND_MODE_CNF*/
	/*Michal Bukai *BAND support - END*/
	/*Michal Bukai - IMEI support - START*/
	sizeof(CiDevPrimSetSvReq),                                              /*CI_DEV_PRIM_SET_SV_REQ*/
	sizeof(CiDevPrimSetSvCnf),                                              /*CI_DEV_PRIM_SET_SV_CNF*/
	0,                                                                      /*CI_DEV_PRIM_GET_SV_REQ*/
	sizeof(CiDevPrimGetSvCnf),                                               /*CI_DEV_PRIM_GET_SV_CNF*/
	/*Michal Bukai - IMEI support - END*/
	sizeof(CiDevPrimApPowerNotifyReq),				/*CI_DEV_PRIM_AP_POWER_NOTIFY_REQ*/
	sizeof(CiDevPrimApPowerNotifyCnf),				/*CI_DEV_PRIM_AP_POWER_NOTIFY_CNF*/

	sizeof(CiDevPrimSetTdModeTxRxReq),                /*CI_DEV_PRIM_SET_TD_MODE_TX_RX_REQ*/
	sizeof(CiDevPrimSetTdModeTxRxCnf),                /*CI_DEV_PRIM_SET_TD_MODE_TX_RX_CNF*/
	sizeof(CiDevPrimSetTdModeLoopbackReq),            /*CI_DEV_PRIM_SET_TD_MODE_LOOPBACK_REQ*/
	sizeof(CiDevPrimSetTdModeLoopbackCnf),            /*CI_DEV_PRIM_SET_TD_MODE_LOOPBACK_CNF*/
	sizeof(CiDevPrimSetGsmModeTxRxReq),               /*CI_DEV_PRIM_SET_GSM_MODE_TX_RX_REQ*/
	sizeof(CiDevPrimSetGsmModeTxRxCnf),               /*CI_DEV_PRIM_SET_GSM_MODE_TX_RX_CNF*/
	sizeof(CiDevPrimSetGsmControlInterfaceReq),       /*CI_DEV_PRIM_SET_GSM_CONTROL_INTERFACE_REQ*/
    sizeof(CiDevPrimSetGsmControlInterfaceCnf) ,       /*CI_DEV_PRIM_SET_GSM_CONTROL_INTERFACE_CNF*/

    sizeof(CiDevPrimEnableHsdpaReq),                /*CI_DEV_PRIM_ENABLE_HSDPA_REQ*/
    sizeof(CiDevPrimEnableHsdpaCnf),                 /*CI_DEV_PRIM_ENABLE_HSDPA_CNF*/
    0,                                               /*CI_DEV_PRIM_GET_HSDPA_STATUS_REQ*/
    sizeof(CiDevPrimGetHsdpaStatusCnf),                 /*CI_DEV_PRIM_GET_HSDPA_STATUS_CNF*/

	sizeof(CiDevPrimReadRfTemperatureReq),             /*CI_DEV_PRIM_READ_RF_TEMPERATURE_REQ*/
	sizeof(CiDevPrimReadRfTemperatureCnf),              /*CI_DEV_PRIM_READ_RF_TEMPERATURE_CNF*/

#ifdef AT_CUSTOMER_HTC
	sizeof(CiDevPrimSetModeSwitchOptReq),               /*CI_DEV_PRIM_SET_MODE_SWITCH_OPTION_REQ*/
	sizeof(CiDevPrimSetModeSwitchOptCnf),               /*CI_DEV_PRIM_SET_MODE_SWITCH_OPTION_CNF*/
	0,                                                  /*CI_DEV_PRIM_GET_MODE_SWITCH_OPTION_REQ*/
	sizeof(CiDevPrimGetModeSwitchOptCnf),                /*CI_DEV_PRIM_GET_MODE_SWITCH_OPTION_CNF*/

	0                                                    /*CI_DEV_PRIM_RADIO_LINK_FAILURE_IND*/
#endif

	sizeof(CiDevPrimSetNetworkMonitorOptReq),          /*CI_DEV_PRIM_SET_NETWORK_MONITOR_OPTION_REQ*/
	sizeof(CiDevPrimSetNetworkMonitorOptCnf),          /*CI_DEV_PRIM_SET_NETWORK_MONITOR_OPTION_CNF*/
	0,                                                 /*CI_DEV_PRIM_GET_NETWORK_MONITOR_OPTION_REQ*/
	sizeof(CiDevPrimGetNetworkMonitorOptCnf),          /*CI_DEV_PRIM_GET_NETWORK_MONITOR_OPTION_CNF*/

	sizeof(CiDevPrimSetProtocolStatusConfigReq),        /*CI_DEV_PRIM_SET_PROTOCOL_STATUS_CONFIG_REQ*/
	sizeof(CiDevPrimSetProtocolStatusConfigCnf),        /*CI_DEV_PRIM_SET_PROTOCOL_STATUS_CONFIG_CNF*/
	0,                                                  /*CI_DEV_PRIM_GET_PROTOCOL_STATUS_CONFIG_REQ*/
	sizeof(CiDevPrimGetProtocolStatusConfigCnf),        /*CI_DEV_PRIM_GET_PROTOCOL_STATUS_CONFIG_CNF*/
	sizeof(CiDevPrimProtocolStatusChangedInd),          /*CI_DEV_PRIM_PROTOCOL_STATUS_CHANGED_IND*/

	sizeof(CiDevPrimSetEventIndConfigReq),              /*CI_DEV_PRIM_SET_EVENT_IND_CONFIG_REQ*/
	sizeof(CiDevPrimSetEventIndConfigCnf),              /*CI_DEV_PRIM_SET_EVENT_IND_CONFIG_CNF*/
	0,                                                  /*CI_DEV_PRIM_GET_EVENT_IND_CONFIG_REQ*/
	sizeof(CiDevPrimGetEventIndConfigCnf),              /*CI_DEV_PRIM_GET_EVENT_IND_CONFIG_CNF*/
	sizeof(CiDevPrimEventReportInd),                    /*CI_DEV_PRIM_EVENT_REPORT_IND*/

	sizeof(CiDevPrimSetWirelessParamConfigReq),         /*CI_DEV_PRIM_SET_WIRELESS_PARAM_CONFIG_REQ*/
	sizeof(CiDevPrimSetWirelessParamConfigCnf),         /*CI_DEV_PRIM_SET_WIRELESS_PARAM_CONFIG_CNF*/
	0,                                                  /*CI_DEV_PRIM_GET_WIRELESS_PARAM_CONFIG_REQ*/
	sizeof(CiDevPrimGetWirelessParamConfigCnf),         /*CI_DEV_PRIM_GET_WIRELESS_PARAM_CONFIG_CNF*/
	sizeof(CiDevPrimWirelessParamInd),                  /*CI_DEV_PRIM_WIRELESS_PARAM_IND*/

	sizeof(CiDevPrimSetSignalingReportConfigReq),       /*CI_DEV_PRIM_SET_SIGNALING_REPORT_CONFIG_REQ*/
	sizeof(CiDevPrimSetSignalingReportConfigCnf),       /*CI_DEV_PRIM_SET_SIGNALING_REPORT_CONFIG_CNF*/
	0,                                                  /*CI_DEV_PRIM_GET_SIGNALING_REPORT_CONFIG_REQ*/
	sizeof(CiDevPrimGetSignalingReportConfigCnf),       /*CI_DEV_PRIM_GET_SIGNALING_REPORT_CONFIG_CNF*/
	sizeof(CiDevPrimSignalingReportInd)                 /*CI_DEV_PRIM_SIGNALING_REPORT_IND*/
};

const UINT16* CiPrimSize[CI_SG_NUMIDS + 1] =
{
	NULL,         /* not a valid SG */
	CiCcPrimSize,
	CiSsPrimSize,
	CiMmPrimSize,
	CiPbPrimSize,
	CiSimPrimSize,
	CiMsgPrimSize,
	CiPsPrimSize,
	CiDatPrimSize,
	CiDevPrimSize,
	NULL,           /* HSCSD */
	NULL,           /* DEB */
	NULL,           /* ATPI */
	NULL,           /* PL */
	NULL            /* OAM */
};

static const UINT16 CiErrPrimSize[CI_ERR_NUM_PRIM] =
{
	0,                                              /* CI_ERR_PRIM_HASNOSUPPORT_CNF = 0xF000,             */
	sizeof(CiErrPrimHasInvalidParasCnf),            /* CI_ERR_PRIM_HASINVALIDPARAS_CNF,                   */
	0,                                              /* CI_ERR_PRIM_ISINVALIDREQUEST_CNF,                  */
	0,                                              /* CI_ERR_PRIM_SIMNOTREADY_CNF,                       */
	sizeof(CiErrPrimAccessDeniedCnf),               /* CI_ERR_PRIM_ACCESSDENIED_CNF,                      */
	sizeof(CiErrPrimInterlinkDownInd),              /* CI_ERR_PRIM_INTERLINKDOWN_IND,                     */
	sizeof(CiErrPrimInterlinkDownRsp)               /* CI_ERR_PRIM_INTERLINKDOWN_RSP,                     */
};

UINT32 cimem_GetCiShReqDataSize(CiShOper oper)
{
	UINT32 dataSize = 0;

	if (oper < CI_SH_NUMOPERS)
	{
		dataSize = CiShellPrimReqSize[oper];
	}

	return dataSize;
}

UINT32 cimem_GetDatPrimDataSize(CiPrimitiveID primId, void *paras)
{
#ifndef CCI_DAT_CONTIGUOUS
	CiDatPdu *pCiDatPdu;
#endif
	CiDatPrimSendReq *pCiDatPrimSendReq;
	CiDatPrimRecvInd *pCiDatPrimRecvInd;
	CiDatPrimRecvDataOptInd       *pCiDatPrimRecvDatOptInd;
	UINT32 primDataSize = 0;

	switch (primId)
	{
	case CI_DAT_PRIM_GET_MAX_PDU_SIZE_REQ:
		primDataSize = sizeof(CiDatPrimGetMaxPduSizeReq);
		break;

	case CI_DAT_PRIM_GET_MAX_PDU_SIZE_CNF:
		primDataSize = sizeof(CiDatPrimGetMaxPduSizeCnf);
		break;

	case CI_DAT_PRIM_SEND_CNF:
		primDataSize = sizeof(CiDatPrimSendCnf);
		break;

	case CI_DAT_PRIM_SEND_REQ:
		pCiDatPrimSendReq = (CiDatPrimSendReq *)paras;
		primDataSize = sizeof(CiDatPrimSendReq);
#ifndef CCI_DAT_CONTIGUOUS
		primDataSize += pCiDatPrimSendReq->pSendPdu->len;
#else
		primDataSize += pCiDatPrimSendReq->sendPdu.len;
#endif
		break;

	case CI_DAT_PRIM_RECV_IND:
		pCiDatPrimRecvInd = (CiDatPrimRecvInd *)paras;
		primDataSize = sizeof(CiDatPrimRecvInd);
#ifndef CCI_DAT_CONTIGUOUS
		pCiDatPdu = pCiDatPrimRecvInd->pRecvPdu;
		if ( pCiDatPdu )
		{
			primDataSize += sizeof(CiDatPdu) + pCiDatPdu->len;
		}
#else
		primDataSize += pCiDatPrimRecvInd->recvPdu.len;
#endif
		break;
	case CI_DAT_PRIM_RECV_DATA_OPT_IND:
		pCiDatPrimRecvDatOptInd = (CiDatPrimRecvDataOptInd *)paras;
		primDataSize = sizeof(CiDatPrimRecvDataOptInd);
		primDataSize += pCiDatPrimRecvDatOptInd->recvPdu.len;
		break;

	case CI_DAT_PRIM_RECV_RSP:
		primDataSize = sizeof(CiDatPrimRecvRsp);
		break;

	case CI_DAT_PRIM_OK_IND:
		primDataSize = sizeof(CiDatPrimOkInd);
		break;

	case CI_DAT_PRIM_NOK_IND:
		primDataSize = sizeof(CiDatPrimNokInd);
		break;

	case CI_DAT_PRIM_SET_UL_TP_IND_REQ:
		primDataSize = sizeof(CiDatPrimSetUlTpIndReq);
		break;

	case CI_DAT_PRIM_SET_UL_TP_IND_CNF:
		primDataSize = sizeof(CiDatPrimSetUlTpIndCnf);
		break;

	case CI_DAT_PRIM_UL_TP_IND:
		primDataSize = sizeof(CiDatPrimUlTpInd);
		break;

	default:
		CCI_TRACE1(CI_STUB, CI_DAT_MEM, __LINE__, CCI_TRACE_ERROR, "Wrong Prim ID=%d in imem_GetDatPrimDataSize()", primId);
		break;
	}
	return (primDataSize);
}


UINT32 cimem_GetCiPrimDataSize(CiServiceHandle handle, CiPrimitiveID primId, void *paras)
{
	UINT32 primDataSize = 0;
	CiServiceGroupID id;

	/* Let's check the Error primitive ID that is common to all service group */
	switch (primId)
	{
	/* Error Primitives */
	case CI_ERR_PRIM_HASNOSUPPORT_CNF:
	case CI_ERR_PRIM_ISINVALIDREQUEST_CNF:
	case CI_ERR_PRIM_SIMNOTREADY_CNF:
	case CI_ERR_PRIM_HASINVALIDPARAS_CNF:
	case CI_ERR_PRIM_ACCESSDENIED_CNF:
	case CI_ERR_PRIM_INTERLINKDOWN_IND:
	case CI_ERR_PRIM_INTERLINKDOWN_RSP:
	case CI_ERR_PRIM_INTERLINKUP_IND:
		primDataSize = CiErrPrimSize[primId - 0xF000];
		return (primDataSize);

	default:
		break; /* no match it will fall down to find out correct MDR funtion */
	}

#ifdef CI_STUB_CLIENT_INCLUDE
	id = findCiSgId( handle );
#else
	id = (CiServiceGroupID)handle;
#endif

	switch (id)
	{
	case CI_SG_ID_CC:
		if (primId <= CI_CC_NUM_PRIM)
		{
			primDataSize = CiPrimSize[id][primId];
		}

		break;

	case CI_SG_ID_MM:
		if (primId <= CI_MM_NUM_PRIM)
		{
			primDataSize = CiPrimSize[id][primId];
		}
		break;

	case CI_SG_ID_SIM:
		if (primId <= CI_SIM_NUM_PRIM)
		{
			primDataSize = CiPrimSize[id][primId];
		}
		break;

	case CI_SG_ID_DEV:
		if ( primId <= CI_DEV_NUM_PRIM )
		{
			primDataSize = CiPrimSize[id][primId];
		}
		break;

	case CI_SG_ID_DAT:
		primDataSize = cimem_GetDatPrimDataSize(primId, paras);
		break;

	case CI_SG_ID_MSG:
		if (primId <= CI_MSG_NUM_PRIM)
		{
			primDataSize = CiPrimSize[id][primId];
		}
		break;

	case CI_SG_ID_PS:
		if (primId <= CI_PS_NUM_PRIM)
		{
			primDataSize = CiPrimSize[id][primId];
		}
		break;

	case CI_SG_ID_SS:
		if (primId <= CI_SS_NUM_PRIM)
		{
			primDataSize = CiPrimSize[id][primId];
		}
		break;

	case CI_SG_ID_PB:
		if (primId <= CI_PB_NUM_PRIM)
		{
			primDataSize = CiPrimSize[id][primId];
		}
		break;
	case CI_SG_ID_HSCSD:
	case CI_SG_ID_DEB:
	case CI_SG_ID_ATPI:
	case CI_SG_ID_PL:
	case CI_SG_ID_OAM:
	default:
		break;
	}
	return (primDataSize);
}

UINT32  cimem_GetCiShCnfDataSize(CiShOper oper, void* cnfParas)
{
	UNUSEDPARAM(cnfParas)

	UINT32 dataSize = 0;
	if (oper < CI_SH_NUMOPERS)
	{
		dataSize = CiShellPrimCnfSize[oper];
	}

	return dataSize;
}
