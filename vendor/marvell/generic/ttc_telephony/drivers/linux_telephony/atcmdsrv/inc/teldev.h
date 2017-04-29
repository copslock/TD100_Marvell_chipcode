/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: teldev.h
 *
 *  Description: Telephony Definitions for AT Command for CI_SG_ID_DEV.
 *
 *  History:
 *   Jan 17, 2008 - Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/
#ifndef TELATCI_DEV_H
#define TELATCI_DEV_H

#include "ci_api_types.h"
#include "ci_api.h"
#include "teldef.h"
#include "telconfig.h"
#include "telatparamdef.h"

#include "ci_dev.h"

#define MAX_UMTS_NEIGHBORING_CELLS CI_DEV_MAX_UMTS_NEIGHBORING_CELLS
#define MAX_GSM_NEIGHBORING_CELLS CI_DEV_MAX_GSM_NEIGHBORING_CELLS

#define MODEM_FLAG_MASK (0xFFF)
#define PXA920_MODEM_FLAG (0x920)
#define PXA910_MODEM_FLAG (0x910)
enum {
	UNKNOWN_MODEM,
	PXA910_MODEM,
	PXA920_MODEM,
};
typedef UINT8 Boolean;


/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
				2G (GSM) Engineering Mode Structures
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

/* Engineering Mode: Mode Type */
//ICAT EXPORTED ENUM
typedef enum ENGMODE_STATE
{
	EM_IDLE = 0,
	EM_DEDICATED,

	/* This must be the last entry */
	NUM_EM_STATES
} _EngModeState;

typedef UINT8 EngModeState;

/* Engineering Mode: Network Type */
typedef enum ENGMODE_NETWORK
{
	EM_NETWORK_GSM = 0,
	EM_NETWORK_UMTS,

	/* This must be the last entry */
	NUM_EM_NETWORKS
} _EngModeNetwork;

typedef UINT8 EngModeNetwork;



/* ===================================================================================
	     Engineering Mode: 2G (GSM) Serving Cell Info structure
   ===================================================================================*/
//ICAT EXPORTED STRUCT
typedef struct GSMServingCellInfo_struct
{
	UINT8 rxSigLevel;       /* Receive Signal Level */
	UINT8 rxSigLevelFull;   /* Receive Signal Level accessed over all TDMA frames */
	UINT8 rxSigLevelSub;    /* Receive Signal Level accessed over subset of TDMA frames*/
	UINT8 rxQualityFull;    /* Receive Quality accessed over all TDMA frames */
	UINT8 rxQualitySub;     /* Receive Quality accessed over subset of TDMA frames */
	UINT8 rac;              /* Routing Area code */
	UINT8 bsic;             /* Base Transceiver Station Identity Code */
	UINT8 nom;              /* Network operation mode */
	UINT8 nco;              /* Network Control Order  */
	UINT8 bs_pa_mfrms;      /* Number of mutiframes between paging messages sent */

	UINT16 mcc;             /* Mobile Country Code */
	UINT16 mnc;             /* Mobile Network Code */
	UINT16 lac;             /* Location Area Code */
	UINT16 ci;              /* Cell Identity */
	UINT16 arfcn;           /* Absolute Radio Frequency Channel Number */

	INT16 C1;               /* Path loss criterion parameter #1 */
	INT16 C2;               /* Path loss criterion parameter #2 */
	INT16 C31;              /* GPRS Signal Level Threshold Criterion Parameter */
	INT16 C32;              /* GPRS Cell Ranking Criterion Parameter */

	UINT16 t3212;           /* Periodic LA Update Timer (T3212) in minutes */
	UINT16 t3312;           /* Periodic RA Update Timer (T3312) in minutes */

	Boolean pbcchSupport;   /* Support of PBCCH */

	UINT8 res1U8[3];

/*  TBD: splitPagingCycle, T3212, T3312, cipheringStatus, cipheringAlgorithm */

} GSMServingCellInfo;

/* ===================================================================================
		Engineering Mode: 2G (GSM) Neighboring Cell Info structure
   ===================================================================================*/
//ICAT EXPORTED STRUCT
typedef struct GSMNeighboringCellInfo_struct {
	UINT8 rxSigLevel;       /* Receive Signal Level */
	UINT8 bsic;             /* Base Transceiver Station Identity Code */
	UINT8 rac;              /* Routing Area code */

	UINT8 res1U8;

	UINT16 mcc;     /* Mobile Country Code */
	UINT16 mnc;     /* Mobile Network Code */
	UINT16 lac;     /* Location Area Code */
	UINT16 ci;      /* Cell Identity */
	UINT16 arfcn;   /* Absolute Radio Frequency Channel Number */

	INT16 C1;       /* Path loss criterion parameter #1 */
	INT16 C2;       /* Path loss criterion parameter #2 */
	INT16 C31;      /* GPRS Signal Level Threshold Criterion Parameter */
	INT16 C32;      /* GPRS Cell Ranking Criterion Parameter */

	UINT8 res2U8[2];
} GSMNeighboringCellInfo;



/* ===================================================================================
	 2G (GSM)  Engineering Mode Information Main structure
   ===================================================================================*/
//ICAT EXPORTED STRUCT
typedef struct EngModeGSMData_struct {

	/* Serving Cell information */
	GSMServingCellInfo svcCellInfo;

	/* Neighboring Cell information */
	UINT8 numNCells;                        /* 0..CI_DEV_MAX_NEIGHBORING_CELLS */
	UINT8 res2U8[3];                        // padding
	GSMNeighboringCellInfo nbCellInfo[ MAX_GSM_NEIGHBORING_CELLS ];
} EngModeGSMData;


/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
				3G (UMTS) Engineering Mode Information structure
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/


/* ===================================================================================
   Engineering Mode: 3G (UMTS) Serving Cell Measurements structure
   ===================================================================================*/
//ICAT EXPORTED STRUCT
typedef struct UMTSServingCellMeasurements_struct
{
	INT16 cpichRSCP;        /* CPICH Received Signal Code Power */
	INT16 utraRssi;         /* UTRA Carrier RSSI */
	INT16 cpichEcN0;        /* CPICH Ec/N0 */
	INT16 sQual;            /* Cell Selection Quality (Squal) */
	INT16 sRxLev;           /* Cell Selection Rx Level (Srxlev) */
	INT16 txPower;          /* UE Transmitted Power */

/*  TBD: Transport Channel BLER */

} UMTSServingCellMeasurements;


/* ===================================================================================
	Engineering Mode: 3G (UMTS) Serving Cell PLMN/Cell Parameters structure
   ===================================================================================*/
//ICAT EXPORTED STRUCT
typedef struct UMTSServingCellParameters_struct
{
	UINT8 rac;              /* Routing Area Code */
	UINT8 nom;              /* Network Operation Mode */

	UINT16 mcc;             /* Mobile Country Code */
	UINT16 mnc;             /* Mobile Network Code */
	UINT16 lac;             /* Location Area Code */
	UINT16 ci;              /* Cell Identity */
	UINT16 uraId;           /* URA Identity */
	UINT16 psc;             /* Primary Scrambling Code */
	UINT16 arfcn;           /* Absolute Radio Frequency Channel Number */

	UINT16 t3212;           /* Periodic LA Update Timer (T3212) in minutes */
	UINT16 t3312;           /* Periodic RA Update Timer (T3312) in minutes */

	Boolean hcsUsed;        /* Hierarchical Cell Structure used? */
	Boolean attDetAllowed;  /* Attach-Detach allowed? */
	Boolean cipherStatus;   /* Ciphering Status = On? */


	UINT8 res1U8;                   /* (padding) */
	UINT16 csDrxCycleLen;           /* CS Domain DRX Cycle Length */
	UINT16 psDrxCycleLen;           /* PS Domain DRX Cycle Length */
	UINT16 utranDrxCycleLen;        /* UTRAN DRX Cycle Length */

	UINT8 res2U8[2];                /* (padding) */

/*  TBD: cipherAlgorithm */

} UMTSServingCellParameters;

/* ===================================================================================
	Engineering Mode: 3G (UMTS) UE Operation Status structure
   ===================================================================================*/
//ICAT EXPORTED STRUCT
typedef struct UMTSUeOperationStatus_struct
{
	UINT8 rrcState;                         /* RRC State */
	UINT8 numLinks;                         /* Number of Radio Links */

	UINT16 srncId;                          /* U-RNTI: SRNC Identifier */
	UINT32 sRnti;                           /* U-RNTI: S-RNTI */
} UMTSUeOperationStatus;


/* ===================================================================================
	Engineering Mode 3G (UMTS) Serving Cell Information structure
   ===================================================================================*/
//ICAT EXPORTED STRUCT
typedef struct UMTSServingCellInfo_struct
{
	Boolean sCellMeasPresent;
	Boolean sCellParamPresent;
	Boolean ueOpStatusPresent;
	UINT8 res1U8;
	UMTSServingCellMeasurements sCellMeas;  /* Measurements */
	UMTSServingCellParameters sCellParam;   /* PLMN/Cell Parameters */
	UMTSUeOperationStatus ueOpStatus;       /* UE Operation Status */
} UMTSServingCellInfo;




/* ===================================================================================
   Engineering Mode 3G (UMTS) Intra-Frequency/Inter-Frequency FDD Cell Information structure
   ===================================================================================*/
//ICAT EXPORTED STRUCT
typedef struct UMTSFddNeighborInfo_struct
{
	/* Measurements */
	INT16 cpichRSCP;        /* CPICH Received Signal Code Power */
	INT16 utraRssi;         /* UTRA Carrier RSSI */
	INT16 cpichEcN0;        /* CPICH Ec/N0 */
	INT16 sQual;            /* Cell Selection Quality (Squal) */
	INT16 sRxLev;           /* Cell Selection Rx Level (Srxlev) */

	/* PLMN/Cell Parameters */
	UINT16 mcc;             /* Mobile Country Code */
	UINT16 mnc;             /* Mobile Network Code */
	UINT16 lac;             /* Location Area Code */
	UINT16 ci;              /* Cell Identity */
	UINT16 arfcn;           /* Absolute Radio Frequency Channel Number */
} UMTSFddNeighborInfo;

/* ===================================================================================
	Engineering Mode 3G (UMTS) Inter-RAT GSM Cell Information structure
   ===================================================================================*/
//ICAT EXPORTED STRUCT
typedef struct UMTSGsmNeighborInfo_struct
{
	/* Measurements */
	INT16 gsmRssi;          /* GSM Carrier RSSI */
	INT16 rxLev;            /* Cell Selection Rx Level */
	INT16 C1;               /* Path loss criterion parameter #1 */
	INT16 C2;               /* Path loss criterion parameter #2 */

	/* PLMN/Cell Parameters */
	UINT16 mcc;             /* Mobile Country Code */
	UINT16 mnc;             /* Mobile Network Code */
	UINT16 lac;             /* Location Area Code */
	UINT16 ci;              /* Cell Identity */
	UINT16 arfcn;           /* Absolute Radio Frequency Channel Number */

	UINT8 res1U8[2];

} UMTSGsmNeighborInfo;

/* ===================================================================================
	3G Engineering Mode: UMTS Data structure
   ===================================================================================*/
//ICAT EXPORTED STRUCT
typedef struct EngModeUmtsData_struct
{
	/* Serving Cell information */
	UMTSServingCellInfo svcCellInfo;

	/* Neighboring Cell information */
	UINT8 numIntraFreq;                     /* Number of Intra-Frequency FDD Cells */
	UINT8 numInterFreq;                     /* Number of Inter-Frequency FDD Cells */
	UINT8 numInterRAT;                      /* Number of Inter-RAT GSM Cells */

	UINT8 res2U8;                           /* (padding) */

	UMTSFddNeighborInfo intraFreq[ MAX_UMTS_NEIGHBORING_CELLS ];
	UMTSFddNeighborInfo interFreq[ MAX_UMTS_NEIGHBORING_CELLS ];
	UMTSGsmNeighborInfo interRAT[ MAX_UMTS_NEIGHBORING_CELLS ];

} EngModeUmtsData;


//ICAT EXPORTED STRUCT
typedef struct EngModeInfo_struct
{
	EngModeState mode;              /* Current Mode (Idle/Dedicated) */
	EngModeNetwork network;         /* Network Type (GSM/UMTS) */
	UINT8 res1U8[2];                /* (padding) */

	/* UMTS or GSM Data -- depending on Network Type */
	union
	{
		EngModeUmtsData umtsData;       /* UMTS Data */
		EngModeGSMData gsmData;         /* GSM Data */
	} data;

} EngModeInfo;


/* ===================================================================================
				End of Definitions for Engineering Mode
   ===================================================================================*/

RETURNCODE_T  ciFuncSet(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciFuncCpConfig(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciDevStatus(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciCGSN(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciCGED(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciBAND(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciPOWERIND(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);

RETURNCODE_T  ciEEMOPT(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciEEMGINFO(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciLPNWUL(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);

/*Added by Michal Bukai*/
#if 0
RETURNCODE_T  ciCIND(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciCMER(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciCLAC(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p); /* APSE code for AT+CLAC command*/
RETURNCODE_T  ciSilentReset(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
void checkDevRet( CiRequestHandle reqHandle, CiDevRc result ); /*Added by Michal Bukai - support for CMEE command (present eroor code\string) */
RETURNCODE_T  ciCalStatus(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciRST(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
#endif

RETURNCODE_T  ciOFF(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciStarCGSN(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciStarMODEMTYPE(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciStarMODEMRESET(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciStarHTCCTO(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciStarEHSDPA(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciGetHVER(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void*arg_p);

#ifdef AT_PRODUCTION_CMNDS
RETURNCODE_T  ciTDTR(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciGSMTR(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciTGCTRL(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
#endif
#ifdef AT_CUSTOMER_HTC
RETURNCODE_T  ciTPCN(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
#endif

RETURNCODE_T  ciDCTS(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciDEELS(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciDEVEI(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciDNPR(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciDUSR(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);

void devCnf(CiSgOpaqueHandle opSgCnfHandle,
	    CiServiceGroupID svgId,
	    CiPrimitiveID primId,
	    CiRequestHandle reqHandle,
	    void*            paras);

void devInd(CiSgOpaqueHandle opSgIndHandle,
	    CiServiceGroupID svgId,
	    CiPrimitiveID primId,
	    CiIndicationHandle indHandle,
	    void*               paras);

#endif

