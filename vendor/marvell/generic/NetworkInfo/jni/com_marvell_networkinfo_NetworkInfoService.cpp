/*
 * * (C) Copyright 2010 Marvell International Ltd.
 * * All Rights Reserved
 * *
 * * MARVELL CONFIDENTIAL
 * * Copyright 2008 ~ 2010 Marvell International Ltd All Rights Reserved.
 * * The source code contained or described herein and all documents related to
 * * the source code ("Material") are owned by Marvell International Ltd or its
 * * suppliers or licensors. Title to the Material remains with Marvell International Ltd
 * * or its suppliers and licensors. The Material contains trade secrets and
 * * proprietary and confidential information of Marvell or its suppliers and
 * * licensors. The Material is protected by worldwide copyright and trade secret
 * * laws and treaty provisions. No part of the Material may be used, copied,
 * * reproduced, modified, published, uploaded, posted, transmitted, distributed,
 * * or disclosed in any way without Marvell's prior express written permission.
 * *
 * * No license under any patent, copyright, trade secret or other intellectual
 * * property right is granted to or conferred upon you by disclosure or delivery
 * * of the Materials, either expressly, by implication, inducement, estoppel or
 * * otherwise. Any license under such intellectual property rights must be
 * * express and approved by Marvell in writing.
 * *
 * */

#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"
#include <utils/misc.h>

#include <utils/Log.h>
#include <utils/threads.h>

#include <hardware_legacy/power.h>
#include <sys/reboot.h>
#include <cutils/properties.h>

#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "atchannel.h"
#include "at_tok.h"
#include "misc.h"
#include <termios.h>
#include "ee_dev.h"

#include <com_marvell_networkinfo_NetworkInfoService.h>

using namespace android;

static int s_closed = 0;
pthread_t s_tid_mainloop;

static void *mainLoop(void *param);

/*************************************************************/
/**************       data for Engineering mode       *****************/
static int s_td_supported = 0;
static DevEngModeState s_mode = CI_DEV_EM_INVALID_STATE;        /**< Current Mode (Idle/Dedicated). \sa CiDevEngModeState */
static DevEngModeNetwork s_network = CI_DEV_EM_NETWORK_UMTS;    /**< Network Type (GSM/UMTS). \sa CiDevEngModeNetwork */

//3G common data  +EEMUMTSSVC:
DevUmtsServingCellMeasurements s_UMTS_CellMeas;
DevTDServingCellMeasurements s_TD_CellMeas;
DevUmtsServingCellParameters s_UMTS_CellParam;
DevTDServingCellParameters s_TD_CellParam;
DevUmtsUeOperationStatus s_UeStatus;

static int s_intraFreq_num = 0;
static int s_interFreq_num = 0;

static int s_init_success = 0;
static int s_umts_svc_ind = 0;
static int s_gsm_svc_ind = 0; 

static pthread_mutex_t s_state_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t s_state_cond = PTHREAD_COND_INITIALIZER;

//3G inter/Intra cell data  +EEMUMTSINTER:/+EEMUMTSINTRA:
DevUmtsFddNeighborInfo s_UMTS_intraFreq[ DEV_MAX_UMTS_NEIGHBORING_CELLS ];
DevUmtsFddNeighborInfo s_UMTS_interFreq[ DEV_MAX_UMTS_NEIGHBORING_CELLS ];

DevUmtsTddNeighborInfo s_TD_intraFreq[ DEV_MAX_UMTS_NEIGHBORING_CELLS ];
DevUmtsTddNeighborInfo s_TD_interFreq[ DEV_MAX_UMTS_NEIGHBORING_CELLS ];


//GSM common data +EEMGINFOSVC
DevGsmServingCellInfo s_GSM_svcCellInfo;

//GPRS common data +EEMGINFOPS
DevGPRSPTMInfo s_GPRSPTMInfo;

static int s_NCCellNumber = 0;
DevGsmNeighboringCellInfo s_GSM_nbCellInfo[ DEV_MAX_GSM_NEIGHBORING_CELLS ];

static int s_rejectCause_num = 0;
DevRejectCause s_RejectHistory[DEV_MAX_REJECTCAUSE_HISTORY];


#define AT_COMMAND_PORT "/dev/citty8"

#define DATABITS       CS8
#define BAUD          B115200
#define STOPBITS        0
#define PARITYON        0
#define PARITY            0

struct NetworkInfo{
	jfieldID item_2g_sub0_arfcn;
	jfieldID item_2g_sub0_rl_timeout_v;
	jfieldID item_2g_sub0_rxlev;
	jfieldID item_2g_sub0_c1;
	jfieldID item_2g_sub0_c2;
	jfieldID item_2g_sub1_ar;
	jfieldID item_2g_sub1_rxl;
	jfieldID item_2g_sub1_c1;
	jfieldID item_2g_sub1_c2;
	jfieldID item_2g_sub2_ar;
	jfieldID item_2g_sub2_rxl;
	jfieldID item_2g_sub2_c1;
	jfieldID item_2g_sub2_c31;
	jfieldID item_2g_sub2_c32;
	jfieldID item_2g_sub3_rxlevel_full;
	jfieldID item_2g_sub3_rxlevel_sub;
	jfieldID item_2g_sub3_rxqual_full;
	jfieldID item_2g_sub3_rxqual_sub;
	jfieldID item_2g_sub3_tx_power_level;
	jfieldID item_2g_sub4_amr_ch;
	jfieldID item_2g_sub4_channel_mode;
	jfieldID item_3g_sub0_rcc_state;
	jfieldID item_3g_sub0_s_freq;
	//jfieldID item_3g_sub0_s_freq;
	jfieldID item_3g_sub0_s_psc;
	jfieldID item_3g_sub0_s_ecio;
	jfieldID item_3g_sub0_s_rscp;
	//jfieldID item_nw_sub0_plmn;
	jfieldID item_nw_sub0_mnc;
	jfieldID item_nw_sub0_mcc;
	jfieldID item_nw_sub0_lac;
	jfieldID item_nw_sub0_rac;
	jfieldID item_nw_sub0_hplmn;
	jfieldID item_nw_sub1_mm_state;
	jfieldID item_nw_sub1_mm_reject;
	jfieldID item_nw_sub1_gmm_state;
	jfieldID item_nw_sub1_attach_reject;
	jfieldID item_nw_sub1_rau_reject;
	jfieldID item_nw_sub2_ciphering;
	jfieldID item_nw_sub2_UEA1;
	jfieldID item_nw_sub2_GEA1;
	jfieldID item_nw_sub2_GEA2;
	jfieldID item_nw_sub2_A5_1;
	jfieldID item_nw_sub2_A5_2;
	jfieldID item_nw_sub2_A5_3;
	jfieldID item_gsm_edge_support;
	jfieldID item_gsm_ci;
	jfieldID item_gsm_bsic;
	jfieldID item_td_pid;
	jfieldID item_3g_tmsi;
	jfieldID item_2g_t3212;
	jfieldID item_3g_t3212;
};
static struct NetworkInfo networkInfo;

//inform java level that data is updated
void update_ee_data(JNIEnv* env)
{
	jclass clazz = env->FindClass("com/marvell/networkinfo/NetworkInfoService");
	jmethodID med = env->GetStaticMethodID(clazz, "update_ee_data_callback","()V");
	env->CallStaticVoidMethod(clazz, med);
}

static void setPthreadState()
{
	pthread_mutex_lock(&s_state_mutex);
	if (s_closed > 0) {
		pthread_cond_broadcast (&s_state_cond);
	}
	pthread_mutex_unlock(&s_state_mutex);
}

/* Called on command or reader thread */
static void onATReaderClosed()
{
	at_close();
	s_closed = 1;
	setPthreadState();
}

/* Called on command thread */
static void onATTimeout()
{
	at_close();
	s_closed = 1;
	setPthreadState();
}

static void waitForClose()
{
	pthread_mutex_lock(&s_state_mutex);
	while (s_closed == 0) {
		pthread_cond_wait(&s_state_cond, &s_state_mutex);
	}
	pthread_mutex_unlock(&s_state_mutex);
}


/**
 * Called by atchannel when an unsolicited line appears
 * This is called on atchannel's reader thread. AT commands may
 * not be issued here
 */
static void onUnsolicited (const char *s, const char *smsPdu)
{
	char *line = NULL, *response, *plmn = NULL, *linesave = NULL;
	int err;

	LOGI("onUnsolicited:%s", s);

	if(strStartsWith(s, "+EEMUMTSINTERRAT:"))
	{

	}
	else if(strStartsWith(s, "+EEMUMTSINTER:") || strStartsWith(s, "+EEMUMTSINTRA:"))
	{
		//UMTS neighboring cell info
		int loop, i;
		int psc; //Primary Scrambling Code (as described in TS 25.331) in 9 bits in UMTS FDD ,
		int cellparaId; // cellParameterId in UMTS TDD
		int rscp;//Level index of CPICH Received Signal Code Power in UMTS FDD, PCCPCH Received Signal Code Power in UMTS TDD
		DevUmtsFddNeighborInfo * UmtsCellInfo = NULL;
		DevUmtsTddNeighborInfo * TDCellInfo = NULL;
		//TDD case
		//+EEMUMTSINTER: 0, -826, 0, -792, 1120, 0, 65534, 0, 10071, 71
		//+EEMUMTSINTER: index, pccpchRSCP, utraRssi, sRxLev,mcc, mnc, lac, ci, arfcn, cellParameterId
		//FDD case 
		//+EEMUMTSINTER: 0, -32768, 0, -32768, -144, -760, 65535, 65535, 65534, 0, 10663, 440
		//+EEMUMTSINTER: index, cpichRSCP, utraRssi, cpichEcN0, sQual, sRxLev,mcc, mnc, lac, ci, arfcn, psc
		if(strStartsWith(s, "+EEMUMTSINTER:"))
		{
			if(s_interFreq_num >= DEV_MAX_UMTS_NEIGHBORING_CELLS)
			{
				LOGI("onUnsolicited: too much EEMUMTSINTER reported");
				return;
			}
			if(s_td_supported)
				TDCellInfo = &s_TD_interFreq[s_interFreq_num++];
			else
				UmtsCellInfo = &s_UMTS_interFreq[s_interFreq_num++];
		}else if(strStartsWith(s, "+EEMUMTSINTRA:"))
		{
			if(s_intraFreq_num >= DEV_MAX_UMTS_NEIGHBORING_CELLS)
			{
				LOGI("onUnsolicited: too much EEMUMTSINTRA reported");
				return;
			}
			if(s_td_supported)
				TDCellInfo = &s_TD_intraFreq[s_intraFreq_num++];
			else
				UmtsCellInfo = &s_UMTS_intraFreq[s_intraFreq_num++];
		}else
		{
			return;
		}
		if(s_td_supported)
		{
			line = strdup(s);
			linesave = line;
			err = at_tok_start(&line);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&loop);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&TDCellInfo->pccpchRSCP);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&TDCellInfo->utraRssi);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&TDCellInfo->sRxLev);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&TDCellInfo->mcc);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&TDCellInfo->mnc);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&TDCellInfo->lac);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&TDCellInfo->ci);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&TDCellInfo->arfcn);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&TDCellInfo->cellParameterId);
			if (err < 0) goto error;
		}
		else
		{
			line = strdup(s);
			linesave = line;
			err = at_tok_start(&line);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&loop);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&UmtsCellInfo->cpichRSCP);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&UmtsCellInfo->utraRssi);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&UmtsCellInfo->cpichEcN0);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&UmtsCellInfo->sQual);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&UmtsCellInfo->sRxLev);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&UmtsCellInfo->mcc);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&UmtsCellInfo->mnc);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&UmtsCellInfo->lac);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&UmtsCellInfo->ci);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&UmtsCellInfo->arfcn);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&UmtsCellInfo->psc);
			if (err < 0) goto error;
		}
	}else if(strStartsWith(s, "+EEMGINFONC:"))
	{
		//GSM neighboring cell info
		// +EEMGINFONC: 2, 0, 0, 6334, 0, 0,41, 55, 29, 29, 516, 0, 29
		//+EEMGINFONC: nc_num, mcc, mnc, lac, rac, ci,rx_lv, bsic, C1, C2, arfcn, C31, C32
		int loop, i;
		DevGsmNeighboringCellInfo *nbCellInfo;
		if(s_NCCellNumber < DEV_MAX_GSM_NEIGHBORING_CELLS)
		{
			nbCellInfo = &s_GSM_nbCellInfo[s_NCCellNumber++];
			line = strdup(s);
			linesave = line;
			err = at_tok_start(&line);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&loop);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&nbCellInfo->mcc);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&nbCellInfo->mnc);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&nbCellInfo->lac);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&nbCellInfo->rac);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&nbCellInfo->ci);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&nbCellInfo->rxSigLevel);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&nbCellInfo->bsic);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&nbCellInfo->C1);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&nbCellInfo->C2);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&nbCellInfo->arfcn);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&nbCellInfo->C31);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&nbCellInfo->C32);
			if (err < 0) goto error;
		}
		else
		{
			LOGI("too much GSM cell info ");
		}
	}else if(strStartsWith(s, "+EEMUMTSSVC:"))
	{
		int loop;
		char sCMeasPresent, sCParamPresent, ueOpStatusPresent;
		//for FDD
		//+EEMUMTSSVC: -4- Mode, sCMeasPresent, sCParamPresent, ueOpStatusPresent
		// -6- cpichRSCP, utraRssi, cpichEcN0, sQual, sRxLev, txPower
		//-6-rac, nom, mcc, mnc, lac, ci
		//-7-uraId, psc, arfcn, t3212, t3312, hcsUsed, attDetAllowed
		//-5- csDrxCycleLen, psDrxCycleLen, utranDrxCycleLen, HSDPASupport, HSUPASupport
		//-4- rrcState, numLinks, srncId, sRnti
		//-6- algPresent, cipherAlg, cipherOn, algPresent, cipherAlg, cipherOn
		//-8- HSDPAActive, HSUPAActive, MccLastRegisteredNetwork, MncLastRegisteredNetwork, TMSI, PTMSI, IsSingleMmRejectCause, IsSingleGmmRejectCause
		// -9- MMRejectCause, GMMRejectCause, mmState, gmmState, gprsReadyState, readyTimerValueInSecs, NumActivePDPContext, ULThroughput, DLThroughput

		//For TDD
		//+EEMUMTSSVC: -4- Mode, sCMeasPresent, sCParamPresent, ueOpStatusPresent
		//-4- pccpchRSCP, utraRssi, sRxLev, txPower	
		//-6-rac, nom, mcc, mnc, lac, ci
		//-7-uraId, cellParameterId, arfcn, t3212, t3312, hcsUsed, attDetAllowed
		//-5- csDrxCycleLen, psDrxCycleLen, utranDrxCycleLen, HSDPASupport, HSUPASupport
		//-4- rrcState, numLinks, srncId, sRnti
		//-6- algPresent, cipherAlg, cipherOn, algPresent, cipherAlg, cipherOn
		//-8- HSDPAActive, HSUPAActive, MccLastRegisteredNetwork, MncLastRegisteredNetwork, TMSI, PTMSI, IsSingleMmRejectCause, IsSingleGmmRejectCause
		// -9- MMRejectCause, GMMRejectCause, mmState, gmmState, gprsReadyState, readyTimerValueInSecs, NumActivePDPContext, ULThroughput, DLThroughput

		//first indication in 3G, reset EE info structure
		s_NCCellNumber = 0;
		s_intraFreq_num = 0;
		s_interFreq_num = 0;
		s_umts_svc_ind = 1;

		if(s_td_supported)
		{
			line = strdup(s);
			linesave = line;
			err = at_tok_start(&line);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&loop);
			if (err < 0) goto error;
			err = at_tok_nextbool(&line,&sCMeasPresent);
			if (err < 0) goto error;
			err = at_tok_nextbool(&line,&sCParamPresent);
			if (err < 0) goto error;
			err = at_tok_nextbool(&line,&ueOpStatusPresent);
			if (err < 0) goto error;
			if(sCMeasPresent > 0)
			{
				//-4- pccpchRSCP, utraRssi, sRxLev, txPower 
				err = at_tok_nextint(&line,&s_TD_CellMeas.pccpchRSCP);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_TD_CellMeas.utraRssi);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_TD_CellMeas.sRxLev);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_TD_CellMeas.txPower);
				if (err < 0) goto error;
			}
			if(sCParamPresent > 0)
			{
				//-6-rac, nom, mcc, mnc, lac, ci
				err = at_tok_nextint(&line,&s_TD_CellParam.rac);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_TD_CellParam.nom);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_TD_CellParam.mcc);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_TD_CellParam.mnc);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_TD_CellParam.lac);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_TD_CellParam.ci);
				if (err < 0) goto error;
				//-7-uraId, cellParameterId, arfcn, t3212, t3312, hcsUsed, attDetAllowed
				err = at_tok_nextint(&line,&s_TD_CellParam.uraId);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_TD_CellParam.cellParameterId);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_TD_CellParam.arfcn);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_TD_CellParam.t3212);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_TD_CellParam.t3312);
				if (err < 0) goto error;
				err = at_tok_nextbool(&line,&s_TD_CellParam.hcsUsed);
				if (err < 0) goto error;
				err = at_tok_nextbool(&line,&s_TD_CellParam.attDetAllowed);
				if (err < 0) goto error;
				//-5- csDrxCycleLen, psDrxCycleLen, utranDrxCycleLen, HSDPASupport, HSUPASupport
				err = at_tok_nextint(&line,&s_TD_CellParam.csDrxCycleLen);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_TD_CellParam.psDrxCycleLen);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_TD_CellParam.utranDrxCycleLen);
				if (err < 0) goto error;
				err = at_tok_nextbool(&line,&s_TD_CellParam.HSDPASupport);
				if (err < 0) goto error;
				err = at_tok_nextbool(&line,&s_TD_CellParam.HSUPASupport);
				if (err < 0) goto error;
			}
		}
		else
		{
			line = strdup(s);
			linesave = line;
			err = at_tok_start(&line);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&loop);
			if (err < 0) goto error;
			err = at_tok_nextbool(&line,&sCMeasPresent);
			if (err < 0) goto error;
			err = at_tok_nextbool(&line,&sCParamPresent);
			if (err < 0) goto error;
			err = at_tok_nextbool(&line,&ueOpStatusPresent);
			if (err < 0) goto error;
			if(sCMeasPresent > 0)
			{
				//-6- cpichRSCP, utraRssi, cpichEcN0, sQual, sRxLev, txPower 
				err = at_tok_nextint(&line,&s_UMTS_CellMeas.cpichRSCP);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_UMTS_CellMeas.utraRssi);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_UMTS_CellMeas.cpichEcN0);
				if(err < 0) goto  error;
				err = at_tok_nextint(&line,&s_UMTS_CellMeas.sQual);
				if(err < 0) goto  error;
				err = at_tok_nextint(&line,&s_UMTS_CellMeas.sRxLev);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_UMTS_CellMeas.txPower);
				if (err < 0) goto error;
			}
			if(sCParamPresent > 0)
			{
				//-6-rac, nom, mcc, mnc, lac, ci
				err = at_tok_nextint(&line,&s_UMTS_CellParam.rac);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_UMTS_CellParam.nom);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_UMTS_CellParam.mcc);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_UMTS_CellParam.mnc);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_UMTS_CellParam.lac);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_UMTS_CellParam.ci);
				if (err < 0) goto error;
				//-7-uraId, psc, arfcn, t3212, t3312, hcsUsed, attDetAllowed
				err = at_tok_nextint(&line,&s_UMTS_CellParam.uraId);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_UMTS_CellParam.psc);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_UMTS_CellParam.arfcn);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_UMTS_CellParam.t3212);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_UMTS_CellParam.t3312);
				if (err < 0) goto error;
				err = at_tok_nextbool(&line,&s_UMTS_CellParam.hcsUsed);
				if (err < 0) goto error;
				err = at_tok_nextbool(&line,&s_UMTS_CellParam.attDetAllowed);
				if (err < 0) goto error;
				//-5- csDrxCycleLen, psDrxCycleLen, utranDrxCycleLen, HSDPASupport, HSUPASupport
				err = at_tok_nextint(&line,&s_UMTS_CellParam.csDrxCycleLen);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_UMTS_CellParam.psDrxCycleLen);
				if (err < 0) goto error;
				err = at_tok_nextint(&line,&s_UMTS_CellParam.utranDrxCycleLen);
				if (err < 0) goto error;
				err = at_tok_nextbool(&line,&s_UMTS_CellParam.HSDPASupport);
				if (err < 0) goto error;
				err = at_tok_nextbool(&line,&s_UMTS_CellParam.HSUPASupport);
				if (err < 0) goto error;
			}
		}
		if(ueOpStatusPresent > 0)
		{
			//-4- rrcState, numLinks, srncId, sRnti
			err = at_tok_nextint(&line,&s_UeStatus.rrcState);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&s_UeStatus.numLinks);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&s_UeStatus.srncId);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&s_UeStatus.sRnti);
			if (err < 0) goto error;
			//-6- algPresent, cipherAlg, cipherOn, algPresent, cipherAlg, cipherOn
			err = at_tok_nextbool(&line,&s_UeStatus.csCipherInfo.algPresent);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&s_UeStatus.csCipherInfo.cipherAlg);
			if (err < 0) goto error;
			err = at_tok_nextbool(&line,&s_UeStatus.csCipherInfo.cipherOn);
			if (err < 0) goto error;
			err = at_tok_nextbool(&line,&s_UeStatus.psCipherInfo.algPresent);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&s_UeStatus.psCipherInfo.cipherAlg);
			if (err < 0) goto error;
			err = at_tok_nextbool(&line,&s_UeStatus.psCipherInfo.cipherOn);
			if (err < 0) goto error;
			//-8- HSDPAActive, HSUPAActive, MccLastRegisteredNetwork, MncLastRegisteredNetwork, TMSI, PTMSI, IsSingleMmRejectCause, IsSingleGmmRejectCause
			err = at_tok_nextbool(&line,&s_UeStatus.HSDPAActive);
			if (err < 0) goto error;
			err = at_tok_nextbool(&line,&s_UeStatus.HSUPAActive);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&s_UeStatus.MccLastRegisteredNetwork);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&s_UeStatus.MncLastRegisteredNetwork);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&s_UeStatus.TMSI);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&s_UeStatus.PTMSI);
			if (err < 0) goto error;
			err = at_tok_nextbool(&line,&s_UeStatus.IsSingleMmRejectCause);
			if (err < 0) goto error;
			err = at_tok_nextbool(&line,&s_UeStatus.IsSingleGmmRejectCause);
			if (err < 0) goto error;
			// -9- MMRejectCause, GMMRejectCause, mmState, gmmState, gprsReadyState, readyTimerValueInSecs, NumActivePDPContext, ULThroughput, DLThroughput
			err = at_tok_nextint(&line,&s_UeStatus.MMRejectCause);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&s_UeStatus.GMMRejectCause);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&s_UeStatus.mmState);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&s_UeStatus.gmmState);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&s_UeStatus.gprsReadyState);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&s_UeStatus.readyTimerValueInSecs);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&s_UeStatus.NumActivePDPContext);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&s_UeStatus.ULThroughput);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&s_UeStatus.DLThroughput);
			if (err < 0) goto error;
		}
	}else if(strStartsWith(s, "+EEMGINFOSVC:"))
	{
		//GSM network informations, +EEMGINFOSVC:
		//-6- mcc, mnc, lac, ci, nom, nco
		// -5- bsic, C1, C2, TA, TxPwr
		// -5- RxSig, RxSigFull, RxSigSub, RxQualFull, RxQualSub
		// -7- ARFCB_tch, hopping_chnl, chnl_type, TS, PacketIdle, rac, arfcn
		// -7- bs_pa_mfrms, C31, C32, t3212, t3312, pbcch_support, EDGE_support
		// -6- ncc_permitted, rl_timeout, ho_count, ho_succ, chnl_access_count, chnl_access_succ_count
		s_gsm_svc_ind = 1; 
		line = strdup(s);
		linesave = line;
		err = at_tok_start(&line);
		if (err < 0) goto error;
		//-6- mcc, mnc, lac, ci, nom, nco
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.mcc);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.mnc);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.lac);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.ci);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.nom);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.nco);
		if (err < 0) goto error;
		// -5- bsic, C1, C2, TA, TxPwr
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.bsic);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.C1);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.C2);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.timingAdv);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.TxPowerLevel);
		if (err < 0) goto error;
		// -5- RxSig, RxSigFull, RxSigSub, RxQualFull, RxQualSub
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.rxSigLevel);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.rxSigLevelFull);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.rxSigLevelSub);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.rxQualityFull);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.rxQualitySub);
		if (err < 0) goto error;
		// -7- ARFCB_tch, hopping_chnl, chnl_type, TS, PacketIdle, rac, arfcn
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.arfcnTch);
		if (err < 0) goto error;
		err = at_tok_nextbool(&line,&s_GSM_svcCellInfo.hoppingChannel);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.ChType);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.timeSlot);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.isInPacketIdle);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.rac);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.arfcn);
		if (err < 0) goto error;
		// -7- bs_pa_mfrms, C31, C32, t3212, t3312, pbcch_support, EDGE_support
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.bs_pa_mfrms);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.C31);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.C32);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.t3212);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.t3312);
		if (err < 0) goto error;
		err = at_tok_nextbool(&line,&s_GSM_svcCellInfo.pbcchSupport);
		if (err < 0) goto error;
		err = at_tok_nextbool(&line,&s_GSM_svcCellInfo.EGPRSSupport);
		if (err < 0) goto error;
		// -6- ncc_permitted, rl_timeout, ho_count, ho_succ, chnl_access_count, chnl_access_succ_count
		err = at_tok_nextbool(&line,&s_GSM_svcCellInfo.nccPermitted);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.RadioLinkTimeout);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.hoCount);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.hoSuccessCount);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.chanAssCount);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GSM_svcCellInfo.chanAssSuccessCount);
		if (err < 0) goto error;

		LOGI("+EEMGINFOSVC_2g 2G item0: %d %d %d %d %d ", s_GSM_svcCellInfo.arfcn, 
				s_GSM_svcCellInfo.RadioLinkTimeout, s_GSM_svcCellInfo.rxSigLevel, s_GSM_svcCellInfo.C1, s_GSM_svcCellInfo.C2 );
	}else if(strStartsWith(s, "+EEMGINFOPS"))
	{
		int loop;
		//GPRS status +EEMGINFOPS
		// -5- PS_attached, attach_type, service_type, tx_power, c_value
		// -6- ul_ts, dl_ts, ul_cs, dl_cs, ul_modulation, dl_modulation
		// -6- gmsk_cv_bep, 8psk_cv_bep, gmsk_mean_bep, 8psk_mean_bep, EDGE_bep_period, single_gmm_rej_cause
		// -6- pdp_active_num, mac_mode, network_control, network_mode, EDGE_slq_measurement_mode, edge_status
		line = strdup(s);
		linesave = line;
		err = at_tok_start(&line);
		if (err < 0) goto error;
		// -5- PS_attached, attach_type, service_type, tx_power, c_value
		err = at_tok_nextbool(&line,&s_GPRSPTMInfo.GPRSAttached);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GPRSPTMInfo.GPRSAttachType);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GPRSPTMInfo.gprsServiceType);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GPRSPTMInfo.txPower);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GPRSPTMInfo.cValue);
		if (err < 0) goto error;
		// -6- ul_ts, dl_ts, ul_cs, dl_cs, ul_modulation, dl_modulation
		err = at_tok_nextint(&line,&s_GPRSPTMInfo.ulTimeSlot);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GPRSPTMInfo.dlTimeSlot);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GPRSPTMInfo.CodingSchemeUL);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GPRSPTMInfo.CodingSchemeDL);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GPRSPTMInfo.ulMod);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GPRSPTMInfo.dlMod);
		if (err < 0) goto error;
		// -6- gmsk_cv_bep, 8psk_cv_bep, gmsk_mean_bep, 8psk_mean_bep, EDGE_bep_period, single_gmm_rej_cause
		err = at_tok_nextint(&line,&loop);//&s_GPRSPTMInfo.egprsBep.gmskCvBep);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&loop);//&s_GPRSPTMInfo.egprsBep.eightPskCvBep);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&loop);//&s_GPRSPTMInfo.egprsBep.gmskMeanBep);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&loop);//&s_GPRSPTMInfo.egprsBep.eightPskMeanBep);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GPRSPTMInfo.EGPRSBEPPeriod);
		if (err < 0) goto error;
		err = at_tok_nextbool(&line,&s_GPRSPTMInfo.IsSingleGmmRejectCause);
		if (err < 0) goto error;
		// -6- pdp_active_num, mac_mode, network_control, network_mode, EDGE_slq_measurement_mode, edge_status
		err = at_tok_nextint(&line,&s_GPRSPTMInfo.NumActivePDPContext);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GPRSPTMInfo.MacMode);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GPRSPTMInfo.NetworkControl);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GPRSPTMInfo.NetworkMode);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&s_GPRSPTMInfo.EGPRSLQMeasurementMode);
		if (err < 0) goto error;
		err = at_tok_nextint(&line,&loop);//&s_GPRSPTMInfo.egprsBep.status);
		if (err < 0) goto error;
	}else if(strStartsWith(s, "+EEMGINBFTM"))
	{
		//here is the last indication for EE mode info, so we update java data:
		//TODO: need notify Java application to update screen
	}else if(strStartsWith(s, "+EEMGINFOBASIC"))
	{
		//first indication in 2G, reset EE info structure
		s_NCCellNumber = 0;
		s_intraFreq_num = 0;
		s_interFreq_num = 0;
	} else if (strStartsWith(s, "*REJCAUSE")) {
		DevRejectCause *mRejectCause;
		if(s_rejectCause_num < DEV_MAX_REJECTCAUSE_HISTORY) {
			mRejectCause = &s_RejectHistory[s_rejectCause_num];
			line = strdup(s);
			linesave = line;
			err = at_tok_start(&line);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&mRejectCause->mmCause);
			if (err < 0) goto error;
			err = at_tok_nextint(&line,&mRejectCause->gmmCause);
			if (err < 0) goto error;
			s_rejectCause_num++;
		} else {
			LOGI("too much Reject Cause info ");
		}
	} else {
		if(linesave != NULL) free(linesave);
		LOGI("unknown unsolicited indication");
		return;
	}

	/* Free allocated memory and return */
	if (linesave != NULL) free(linesave);
		return;

error:
	if (linesave != NULL) free(linesave);
	LOGE("%s: Error parameter in ind msg: %s", __FUNCTION__, s);
	return;
}

static int EnterEEmode()
{
		int err;
		ATResponse *p_response = NULL;
		char *line, *result;
		//s_rejectCause_num set to 0 when enter EE mode, because ATCmdServer will save 10 time records
		s_rejectCause_num = 0;
		if(s_closed) return -1;
		//Query the CP version firstly, otherwise if any Init AT error, will cause CP version missing.
		err = at_send_command_sync("AT+CGMR", SINGLELINE, "+CGMR:", NULL, &p_response);
		if (err < 0 || p_response->success == 0) goto error;
		line = p_response->p_intermediates->line;
		err = at_tok_start(&line);
		if (err < 0) goto error;
		err = at_tok_nextstr(&line, &result);
		if (err < 0) goto error;
		LOGD("Baseband version is %s", result);
		LOGD("init is %d, umst_svc is %d, gsm_svc is %d", s_init_success, s_umts_svc_ind, s_gsm_svc_ind);	
		if (strStartsWith(result, "TTC"))
		{
			s_td_supported = 0;
		}
		else //set TD as default
		{
			s_td_supported = 1;
		}
		at_response_free(p_response);
		//Set query mode
		err = at_send_command_sync("AT+EEMOPT=1", NO_RESULT, NULL, NULL, &p_response);
		if (err < 0 || p_response->success == 0) goto error;
		at_response_free(p_response);
		//Set query Rejcause 
		//If ATCmdServer not support *REJCAUSE, please don't do this command, otherwise cause initialization fail.
		err = at_send_command_sync("AT*REJCAUSE=2", NO_RESULT, NULL, NULL, &p_response);
		if (err < 0 || p_response->success == 0) goto error;
		at_response_free(p_response);
		LOGD("Enter EEmode, s_rejectCause_num is %d" , s_rejectCause_num);
		LOGD("EE init OK!");	
		//kick off first query
		//Query EE mode info
		err = at_send_command_sync("AT+EEMGINFO?", SINGLELINE, "+EEMGINFO :", NULL, &p_response);
		if (err < 0 || p_response->success == 0) goto error;
		//check network type UMTS or GSM
		line = p_response->p_intermediates->line;
		err = at_tok_start(&line);
		if (err < 0) goto error;
		err = at_tok_nextint(&line, &s_mode);
		if (err < 0) goto error;
		LOGV("EngModeinfo mode:%d", s_mode);
		err = at_tok_nextint(&line, &s_network);
		if (err < 0) goto error;
		LOGV("EngModeinfo network:%d", s_network);
		at_response_free(p_response);
		return 0;

error:
	at_response_free(p_response);
	return -1;
}

static int LeaveEEmode()
{
	if(s_closed) return -1;
	s_init_success = 0;
	s_umts_svc_ind = 0;
	s_gsm_svc_ind = 0;
	at_send_command("AT+EEMOPT=0", NULL);
	return 0;
}

void Java_com_marvell_networkinfo_NetworkInfoService_start_ee_mode(JNIEnv* env, jobject obj)
{
	//mainLoop start readerLoop will exit immediately. so don't need create a theread.
	mainLoop(NULL);
}

static void *mainLoop(void *param)
{
	int fd, ret, i;
	LOGD("mainLoop entered");	
	at_set_on_reader_closed(onATReaderClosed);
	at_set_on_timeout(onATTimeout);
	//open tty port to send/receive AT command
	struct termios newtio; //place for old and new port settings for serial port
	fd = open(AT_COMMAND_PORT, O_RDWR);
	fcntl(fd, F_SETFL, 0);
	//fcntl(fd, F_SETFL, O_NONBLOCK);
	newtio.c_cflag = BAUD | CRTSCTS | DATABITS | STOPBITS | PARITYON | PARITY | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	newtio.c_lflag =  ECHOE | ECHO | ICANON;		//ICANON;
	newtio.c_lflag =  0;							//ICANON;
	newtio.c_cc[VMIN] = 1;
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VERASE] = 0x8;
	newtio.c_cc[VEOL] = 0xD;
	newtio.c_cc[VINTR]	  = 0;		/* Ctrl-c */
	newtio.c_cc[VQUIT]	  = 0;		/* Ctrl-\ */
	newtio.c_cc[VERASE]   = 0;		/* del */
	newtio.c_cc[VKILL]	  = 0;		/* @ */
	newtio.c_cc[VEOF]	  = 4;		/* Ctrl-d */
	newtio.c_cc[VTIME]	  = 0;		/* inter-character timer unused */
	newtio.c_cc[VMIN]	  = 1;		/* blocking read until 1 character arrives */
	newtio.c_cc[VSWTC]	  = 0;		/* '\0' */
	newtio.c_cc[VSTART]   = 0;		/* Ctrl-q */
	newtio.c_cc[VSTOP]	  = 0;		/* Ctrl-s */
	newtio.c_cc[VSUSP]	  = 0;		/* Ctrl-z */
	newtio.c_cc[VEOL]	  = 0;		/* '\0' */
	newtio.c_cc[VREPRINT] = 0;		/* Ctrl-r */
	newtio.c_cc[VDISCARD] = 0;		/* Ctrl-u */
	newtio.c_cc[VWERASE]  = 0;		/* Ctrl-w */
	newtio.c_cc[VLNEXT]   = 0;		/* Ctrl-v */
	newtio.c_cc[VEOL2]	  = 0;		/* '\0' */
	newtio.c_cc[VMIN] = 1;
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VERASE] = 0x8;
	newtio.c_cc[VEOL] = 0xD;
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &newtio);
	if (fd >= 0)
	{
		s_closed = 0;
		ret = at_open(fd, onUnsolicited);
		if(ret < 0)
		{
			LOGE("at_open fail");
			close(fd);
			return 0;
		}
		LOGI("EE mode AT channel open successfully, ttyName:%s", AT_COMMAND_PORT );
	}
	else
	{
		LOGE("EE mode AT channel open error, ttyName:%s, try again", AT_COMMAND_PORT );
		return 0;
	}

	ret = EnterEEmode();

	if(ret < 0)
	{
		//TODO: close port and kill all thread
		LOGE("EE mode, initialization failed");
		s_init_success = 0;
		at_close(); // thread will die after port close
		s_closed = 1;
	} else {
		s_init_success = 1;
	}

	return 0;

}

void copy_to_java_2g(JNIEnv* env,jobject obj)
{

	//2G ¨C item0 Serving Cell Information
	env->SetIntField(obj, networkInfo.item_2g_sub0_arfcn,s_GSM_svcCellInfo.arfcn);
	env->SetIntField(obj,networkInfo.item_2g_sub0_rl_timeout_v,s_GSM_svcCellInfo.RadioLinkTimeout);
	env->SetIntField(obj, networkInfo.item_2g_sub0_rxlev,s_GSM_svcCellInfo.rxSigLevel);
	env->SetIntField(obj,networkInfo.item_2g_sub0_c1,s_GSM_svcCellInfo.C1);
	env->SetIntField(obj, networkInfo.item_2g_sub0_c2,s_GSM_svcCellInfo.C2);
	env->SetIntField(obj,networkInfo.item_gsm_edge_support, s_GSM_svcCellInfo.EGPRSSupport);

	env->SetIntField(obj, networkInfo.item_gsm_ci, s_GSM_svcCellInfo.ci);
	env->SetIntField(obj, networkInfo.item_gsm_bsic, s_GSM_svcCellInfo.bsic);
	env->SetIntField(obj, networkInfo.item_2g_t3212, s_GSM_svcCellInfo.t3212);
	LOGI("copy_to_java_2g 2G item0: %d %d %d %d %d  %d %d %d", s_GSM_svcCellInfo.arfcn, 
			s_GSM_svcCellInfo.RadioLinkTimeout, s_GSM_svcCellInfo.rxSigLevel, s_GSM_svcCellInfo.C1, s_GSM_svcCellInfo.C2, s_GSM_svcCellInfo.EGPRSSupport, s_GSM_svcCellInfo.ci, s_GSM_svcCellInfo.bsic);

	//2G ITEM 1 : Neighboring Cell Information array
	/* networkInfo.item_2g_sub1_ar = env->GetFieldID(clazz, "item_2g_sub1_ar", "S");
	  networkInfo.item_2g_sub1_rxl = env->GetFieldID(clazz, "item_2g_sub1_rxl", "S");
	  networkInfo.item_2g_sub1_c1 = env->GetFieldID(clazz, "item_2g_sub1_c1", "S");
	  networkInfo.item_2g_sub1_c2 = env->GetFieldID(clazz, "item_2g_sub1_c2", "S");
	  */

	//2G ITEM 2 : Neighboring Cell information in GPRS array
	/*
	 networkInfo.item_2g_sub2_ar = env->GetFieldID(clazz, "item_2g_sub2_ar", "S");
	 networkInfo.item_2g_sub2_rxl = env->GetFieldID(clazz, "item_2g_sub2_rxl", "S");
	 networkInfo.item_2g_sub2_c1 = env->GetFieldID(clazz, "item_2g_sub2_c1", "S");
	 networkInfo.item_2g_sub2_c31 = env->GetFieldID(clazz, "item_2g_sub2_c31", "S");
	 networkInfo.item_2g_sub2_c32 = env->GetFieldID(clazz, "item_2g_sub2_c32", "S");
	 */

	//2G ITEM 3 : RX/TX Power Information
	env->SetIntField(obj, networkInfo.item_2g_sub3_rxlevel_full,s_GSM_svcCellInfo.rxSigLevelFull);
	env->SetIntField(obj,networkInfo.item_2g_sub3_rxlevel_sub,s_GSM_svcCellInfo.rxSigLevelSub);
	env->SetIntField(obj, networkInfo.item_2g_sub3_rxqual_full,s_GSM_svcCellInfo.rxQualityFull);
	env->SetIntField(obj,networkInfo.item_2g_sub3_rxqual_sub,s_GSM_svcCellInfo.rxQualitySub);
	env->SetIntField(obj, networkInfo.item_2g_sub3_tx_power_level,s_GSM_svcCellInfo.TxPowerLevel);

	LOGI("copy_to_java_2g 2G ITEM 3: %d %d %d %d %d ", s_GSM_svcCellInfo.rxSigLevelFull, 
			s_GSM_svcCellInfo.rxSigLevelSub, s_GSM_svcCellInfo.rxQualityFull, s_GSM_svcCellInfo.rxQualitySub, s_GSM_svcCellInfo.TxPowerLevel );

	//2G ITEM 4 : Current State Information
	env->SetIntField(obj,networkInfo.item_2g_sub4_amr_ch,s_GSM_svcCellInfo.arfcnTch);
	env->SetIntField(obj, networkInfo.item_2g_sub4_channel_mode,s_GSM_svcCellInfo.ChType);

	LOGI("copy_to_java_2g 2G ITEM 4: %d %d ", s_GSM_svcCellInfo.arfcnTch, s_GSM_svcCellInfo.ChType);
}

void copy_to_java_3g(JNIEnv* env, jobject obj)
{
	if(s_td_supported)
	{
		//3G item0: RRC State and Serving Cell Information 
		env->SetIntField(obj, networkInfo.item_3g_sub0_rcc_state,s_UeStatus.rrcState);
		env->SetIntField(obj,networkInfo.item_3g_sub0_s_freq,s_TD_CellParam.arfcn);
		env->SetIntField(obj, networkInfo.item_3g_sub0_s_psc,-1);
		env->SetIntField(obj,networkInfo.item_3g_sub0_s_ecio,-1);
		env->SetIntField(obj, networkInfo.item_3g_sub0_s_rscp,s_TD_CellMeas.pccpchRSCP);
		//3G Item 1: Intra frequency neighbor cell information array
		//3G Item 2: Inter Frequency neighbor cell information array
	}
	else
	{
		//3G item0: RRC State and Serving Cell Information 
		env->SetIntField(obj, networkInfo.item_3g_sub0_rcc_state,s_UeStatus.rrcState);
		env->SetIntField(obj,networkInfo.item_3g_sub0_s_freq,s_UMTS_CellParam.arfcn);
		env->SetIntField(obj, networkInfo.item_3g_sub0_s_psc,s_UMTS_CellParam.psc);
		env->SetIntField(obj,networkInfo.item_3g_sub0_s_ecio,s_UMTS_CellMeas.cpichEcN0);
		env->SetIntField(obj, networkInfo.item_3g_sub0_s_rscp,s_UMTS_CellMeas.cpichRSCP);
	}
}

void copy_to_java_network(JNIEnv* env, jobject obj)
{
	if(s_network == CI_DEV_EM_NETWORK_GSM)
	{
		//NM ITEM 0 : General information
		//env->SetIntField(obj, networkInfo.item_nw_sub0_plmn,s_GSM_svcCellInfo.mnc);
		env->SetIntField(obj, networkInfo.item_nw_sub0_mnc,s_GSM_svcCellInfo.mnc);
		env->SetIntField(obj, networkInfo.item_nw_sub0_mcc,s_GSM_svcCellInfo.mcc);
		env->SetIntField(obj,networkInfo.item_nw_sub0_lac,s_GSM_svcCellInfo.lac);
		env->SetIntField(obj, networkInfo.item_nw_sub0_rac,s_GSM_svcCellInfo.rac);
		env->SetIntField(obj,networkInfo.item_nw_sub0_hplmn,-1);
		//NM ITEM 1 : Network Management information (GSM no these areas)
		env->SetIntField(obj, networkInfo.item_nw_sub1_mm_state,-1);
		env->SetIntField(obj,networkInfo.item_nw_sub1_mm_reject,-1);
		env->SetIntField(obj, networkInfo.item_nw_sub1_gmm_state,-1);
		env->SetIntField(obj,networkInfo.item_nw_sub1_attach_reject,-1);
		env->SetIntField(obj,networkInfo.item_nw_sub1_rau_reject,-1);
		//NM ITEM 2 : Ciphering Information (GSM not support)
		if(s_mode == CI_DEV_EM_GSM_IDLE_STATE)  //Shows the Ciphering value which the UE supports
		{
			env->SetIntField(obj, networkInfo.item_nw_sub2_ciphering,-1);
			env->SetIntField(obj,networkInfo.item_nw_sub2_UEA1,-1);
			env->SetIntField(obj, networkInfo.item_nw_sub2_GEA1,-1);
			env->SetIntField(obj,networkInfo.item_nw_sub2_GEA2,-1);
			env->SetIntField(obj,networkInfo.item_nw_sub2_A5_1,-1);
			env->SetIntField(obj,networkInfo.item_nw_sub2_A5_2,-1);
			env->SetIntField(obj,networkInfo.item_nw_sub2_A5_3,-1);
		}
		else //Shows the Ciphering value that the N/W sent
		{
			env->SetIntField(obj, networkInfo.item_nw_sub2_ciphering,-1);
			env->SetIntField(obj,networkInfo.item_nw_sub2_UEA1,-1);
			env->SetIntField(obj, networkInfo.item_nw_sub2_GEA1,-1);
			env->SetIntField(obj,networkInfo.item_nw_sub2_GEA2,-1);
			env->SetIntField(obj,networkInfo.item_nw_sub2_A5_1,-1);
			env->SetIntField(obj,networkInfo.item_nw_sub2_A5_2,-1);
			env->SetIntField(obj,networkInfo.item_nw_sub2_A5_3,-1);
		}
	}
	else if(s_network == CI_DEV_EM_NETWORK_UMTS)
	{
		//NM ITEM 0 : General information
		if(s_td_supported) 
		{
			//env->SetIntField(obj, networkInfo.item_nw_sub0_plmn,s_TD_CellParam.mnc);
			env->SetIntField(obj, networkInfo.item_nw_sub0_mnc,s_TD_CellParam.mnc);
			env->SetIntField(obj, networkInfo.item_nw_sub0_mcc,s_TD_CellParam.mcc);
			env->SetIntField(obj,networkInfo.item_nw_sub0_lac,s_TD_CellParam.lac);
			env->SetIntField(obj, networkInfo.item_nw_sub0_rac,s_TD_CellParam.rac);
			env->SetIntField(obj,networkInfo.item_nw_sub0_hplmn,-1);
			env->SetIntField(obj, networkInfo.item_td_pid,s_TD_CellParam.cellParameterId);
			env->SetIntField(obj, networkInfo.item_3g_t3212, s_TD_CellParam.t3212);
		}
		else
		{
			//umts
			//env->SetIntField(obj, networkInfo.item_nw_sub0_plmn,s_UMTS_CellParam.mnc);
			env->SetIntField(obj, networkInfo.item_nw_sub0_mnc,s_UMTS_CellParam.mnc);
			env->SetIntField(obj, networkInfo.item_nw_sub0_mcc,s_UMTS_CellParam.mcc);
			env->SetIntField(obj,networkInfo.item_nw_sub0_lac,s_UMTS_CellParam.lac);
			env->SetIntField(obj, networkInfo.item_nw_sub0_rac,s_UMTS_CellParam.rac);
			env->SetIntField(obj,networkInfo.item_nw_sub0_hplmn,-1);
			env->SetIntField(obj, networkInfo.item_3g_t3212, s_UMTS_CellParam.t3212);
		}
		//NM ITEM 1 : Network Management information 
		env->SetIntField(obj, networkInfo.item_nw_sub1_mm_state,s_UeStatus.mmState);
		env->SetIntField(obj,networkInfo.item_nw_sub1_mm_reject,s_UeStatus.MMRejectCause);
		env->SetIntField(obj, networkInfo.item_nw_sub1_gmm_state,s_UeStatus.gmmState);
		env->SetIntField(obj,networkInfo.item_nw_sub1_attach_reject,s_UeStatus.GMMRejectCause);
		env->SetIntField(obj,networkInfo.item_nw_sub1_rau_reject,s_UeStatus.GMMRejectCause);
		//NM ITEM 2 : Ciphering Information because the network is umts, don't judge GSM state
		env->SetIntField(obj, networkInfo.item_nw_sub2_ciphering,s_UeStatus.csCipherInfo.cipherOn);
		env->SetIntField(obj,networkInfo.item_nw_sub2_UEA1,s_UeStatus.csCipherInfo.cipherAlg);
		env->SetIntField(obj, networkInfo.item_nw_sub2_GEA1,-1);
		env->SetIntField(obj,networkInfo.item_nw_sub2_GEA2,-1);
		env->SetIntField(obj,networkInfo.item_nw_sub2_A5_1,-1);
		env->SetIntField(obj,networkInfo.item_nw_sub2_A5_2,-1);
		env->SetIntField(obj,networkInfo.item_nw_sub2_A5_3,-1);
		env->SetIntField(obj, networkInfo.item_3g_tmsi, s_UeStatus.TMSI);
	}
}

jintArray Java_com_marvell_networkinfo_NetworkInfoService_get_reject_history(JNIEnv* env, jobject obj)
{
	jintArray result = NULL;
	jint tmp[256] = {-1};
	LOGI("get_reject_history: s_rejectCause_num %d", s_rejectCause_num);
	if(s_rejectCause_num <= 0 || s_rejectCause_num > DEV_MAX_REJECTCAUSE_HISTORY)
		goto error;
	result = env->NewIntArray(2*s_rejectCause_num);
	for(int i = 0; i < s_rejectCause_num; i++) {
		tmp[i*2] = s_RejectHistory[i].mmCause;
		tmp[i*2+1] = s_RejectHistory[i].gmmCause;
	}
	env->SetIntArrayRegion(result, 0, 2*s_rejectCause_num, tmp);
	return result;
error:
	result = env->NewIntArray(1);
	env->SetIntArrayRegion(result, 0, 1, tmp);
	return result;
}

jboolean Java_com_marvell_networkinfo_NetworkInfoService_lock_cell(JNIEnv* env, jobject obj,jint mode, jint arfcn, jint cell)
{
	int err;
	ATResponse * p_response = NULL;
	char cmdString[64];

	LOGI("lock cell");
	if(mode == 0) {
		sprintf(cmdString, "AT*CELLLOCK=%d", mode);
	} else if (mode == 1) {
		sprintf(cmdString, "AT*CELLLOCK=%d,%d", mode, arfcn);
	} else {
		sprintf(cmdString, "AT*CELLLOCK=%d,%d,%d", mode, arfcn, cell);
	}
	err = at_send_command_sync(cmdString, NO_RESULT, NULL, NULL, &p_response);
	if (err < 0 || p_response->success == 0) goto error;
	at_response_free(p_response);
	return JNI_TRUE;
error:
	at_response_free(p_response);
	return JNI_FALSE;
}

jintArray Java_com_marvell_networkinfo_NetworkInfoService_get_neighboring_cell(JNIEnv* env, jobject obj, jint type)
{
	jintArray result = NULL; 
	jint tmp[256] = {-1}; /* make sure it is large enough! */

	LOGI("get_neighboring_cell: s_NCCellNumber; %d, s_interFreq_num: %d, s_intraFreq_num: %d,  ", s_NCCellNumber, s_interFreq_num, s_intraFreq_num);

	switch(type)
	{
	case 0:
		{
			if(s_NCCellNumber <= 0 || s_NCCellNumber > DEV_MAX_GSM_NEIGHBORING_CELLS) 
				goto error;
			result = env->NewIntArray(4*s_NCCellNumber); 
			for(int i = 0; i < s_NCCellNumber; i++) {
				tmp[i*4] = s_GSM_nbCellInfo[i].arfcn;
				tmp[i*4+1] = s_GSM_nbCellInfo[i].rxSigLevel;
				tmp[i*4+2] = s_GSM_nbCellInfo[i].C1;
				tmp[i*4+3] = s_GSM_nbCellInfo[i].C2;
			}
			env->SetIntArrayRegion(result, 0, 4*s_NCCellNumber, tmp);
			break;
		}
	case  1:
		{
			if(s_NCCellNumber <= 0 || s_NCCellNumber > DEV_MAX_GSM_NEIGHBORING_CELLS) 
				goto error;
			result = env->NewIntArray(5*s_NCCellNumber); 
			for(int i = 0; i < s_NCCellNumber; i++) {
				tmp[i*5] = s_GSM_nbCellInfo[i].arfcn;
				tmp[i*5+1] = s_GSM_nbCellInfo[i].rxSigLevel;
				tmp[i*5+2] = s_GSM_nbCellInfo[i].C1;
				tmp[i*5+3] = s_GSM_nbCellInfo[i].C31;
				tmp[i*5+4] = s_GSM_nbCellInfo[i].C32;
			}
			env->SetIntArrayRegion(result, 0, 5*s_NCCellNumber, tmp);
			break;
		}
	case  2:
		{
			if(s_intraFreq_num <= 0 || s_intraFreq_num > DEV_MAX_UMTS_NEIGHBORING_CELLS) 
				goto error;
			result = env->NewIntArray(4*s_intraFreq_num); 
			for(int i = 0; i < s_intraFreq_num; i++) {
				if(s_td_supported)
				{
					tmp[i*4] = s_TD_intraFreq[i].arfcn;
					tmp[i*4+1] = s_TD_intraFreq[i].cellParameterId;
					tmp[i*4+2] = s_TD_intraFreq[i].pccpchRSCP;
					tmp[i*4+3] = -1;
				}
				else
				{
					tmp[i*4] = s_UMTS_intraFreq[i].arfcn;
					tmp[i*4+1] = s_UMTS_intraFreq[i].psc;
					tmp[i*4+2] = s_UMTS_intraFreq[i].cpichRSCP;
					tmp[i*4+3] = s_UMTS_intraFreq[i].cpichEcN0;
				}
			}
			env->SetIntArrayRegion(result, 0, 4*s_intraFreq_num, tmp);
			break;
		}
	case  3:
		{
			if(s_interFreq_num <= 0 || s_interFreq_num > DEV_MAX_UMTS_NEIGHBORING_CELLS) 
				goto error;
			result = env->NewIntArray(4*s_interFreq_num); 
			for(int i = 0; i < s_interFreq_num; i++) {
				if(s_td_supported)
				{
					tmp[i*4] = s_TD_interFreq[i].arfcn;
					tmp[i*4+1] = s_TD_interFreq[i].cellParameterId;
					tmp[i*4+2] = s_TD_interFreq[i].pccpchRSCP;
					tmp[i*4+3] = -1;
				}
				else
				{
					tmp[i*4] = s_UMTS_interFreq[i].arfcn;
					tmp[i*4+1] = s_UMTS_interFreq[i].psc;
					tmp[i*4+2] = s_UMTS_interFreq[i].cpichRSCP;
					tmp[i*4+3] = s_UMTS_interFreq[i].cpichEcN0;
				}
			}
			env->SetIntArrayRegion(result, 0, 4*s_interFreq_num, tmp);
			break;
		}
	default:
		goto error;
	}
	return result;

error:
	result = env->NewIntArray(1);
	env->SetIntArrayRegion(result, 0, 1, tmp);
	return result;
}


void Java_com_marvell_networkinfo_NetworkInfoService_stop_ee_mode(JNIEnv* env, jobject obj)
{
	int ret = -1;
	if(s_closed) return;

	LeaveEEmode();
	at_close();
	s_closed = 1;
	ret = exit_read_loop_thread(0x00);

	LOGD("stop ee mode, exit read loop %d\n", ret);	
	sleep(1);
	waitForClose();
}



jint Java_com_marvell_networkinfo_NetworkInfoService_query_ee_data(JNIEnv* env, jobject obj)
{
	int err;
	ATResponse *p_response = NULL;
	char *line, *result;

	if(s_closed) return 3;

	if(s_init_success == 0) return 2;

	if((s_umts_svc_ind|s_gsm_svc_ind) != 0) {
		//write back latest informations
		if(s_network == CI_DEV_EM_NETWORK_GSM)
			copy_to_java_2g(env, obj);
		else if(s_network == CI_DEV_EM_NETWORK_UMTS)
			copy_to_java_3g(env, obj);

		copy_to_java_network(env, obj);
	}
	//Query EE mode info
	err = at_send_command_sync("AT+EEMGINFO?", SINGLELINE, "+EEMGINFO :", NULL, &p_response);
	if (err < 0 || p_response->success == 0) goto error;
	//check network type UMTS or GSM
	line = p_response->p_intermediates->line;
	err = at_tok_start(&line);
	if (err < 0) goto error;
	err = at_tok_nextint(&line, &s_mode);
	if (err < 0) goto error;
	LOGV("EngModeinfo mode:%d", s_mode);
	err = at_tok_nextint(&line, &s_network);
	if (err < 0) goto error;
	LOGV("EngModeinfo network:%d", s_network);
	at_response_free(p_response);
	return 0;
	// 
error:
	at_response_free(p_response);
	return 1;
}


static JNINativeMethod method_table[] = {
	{"query_ee_data", "()I", (void*)Java_com_marvell_networkinfo_NetworkInfoService_query_ee_data},
	{"start_ee_mode", "()V", (void*)Java_com_marvell_networkinfo_NetworkInfoService_start_ee_mode},
	{"stop_ee_mode", "()V", (void*)Java_com_marvell_networkinfo_NetworkInfoService_stop_ee_mode},
	{"get_neighboring_cell", "(I)[I", (void*)Java_com_marvell_networkinfo_NetworkInfoService_get_neighboring_cell},
	{"lock_cell", "(III)Z", (void*)Java_com_marvell_networkinfo_NetworkInfoService_lock_cell},
	{"get_reject_history", "()[I", (void*)Java_com_marvell_networkinfo_NetworkInfoService_get_reject_history},
};

int register_com_marvell_networkinfo_NetworkInfoService(JNIEnv *env)
{
	jclass clazz = env->FindClass("com/marvell/networkinfo/NetworkInfoService");

	if (clazz == NULL) {
		LOGE("Can't find com/marvell/networkinfo/NetworkInfoService");
		return -1;
	}

	//2G ¨C item0 Serving Cell Information
	networkInfo.item_2g_sub0_arfcn = env->GetFieldID(clazz, "item_2g_sub0_arfcn", "I");
	networkInfo.item_2g_sub0_rl_timeout_v = env->GetFieldID(clazz, "item_2g_sub0_rl_timeout_v", "I");
	networkInfo.item_2g_sub0_rxlev = env->GetFieldID(clazz, "item_2g_sub0_rxlev", "I");
	networkInfo.item_2g_sub0_c1 = env->GetFieldID(clazz, "item_2g_sub0_c1", "I");
	networkInfo.item_2g_sub0_c2 = env->GetFieldID(clazz, "item_2g_sub0_c2", "I");
	networkInfo.item_gsm_edge_support = env->GetFieldID(clazz, "item_gsm_edge_support", "I");
	networkInfo.item_gsm_ci = env->GetFieldID(clazz, "item_gsm_ci", "I");
	networkInfo.item_gsm_bsic = env->GetFieldID(clazz, "item_gsm_bsic", "I");
	//2G ITEM 1 : Neighboring Cell Information array
	//2G ITEM 2 : Neighboring Cell information in GPRS array
	//2G ITEM 3 : RX/TX Power Information
	networkInfo.item_2g_sub3_rxlevel_full = env->GetFieldID(clazz, "item_2g_sub3_rxlevel_full", "I");
	networkInfo.item_2g_sub3_rxlevel_sub = env->GetFieldID(clazz, "item_2g_sub3_rxlevel_sub", "I");
	networkInfo.item_2g_sub3_rxqual_full = env->GetFieldID(clazz, "item_2g_sub3_rxqual_full", "I");
	networkInfo.item_2g_sub3_rxqual_sub = env->GetFieldID(clazz, "item_2g_sub3_rxqual_sub", "I");
	networkInfo.item_2g_sub3_tx_power_level = env->GetFieldID(clazz, "item_2g_sub3_tx_power_level", "I");
	//2G ITEM 4 : Current State Information
	networkInfo.item_2g_sub4_amr_ch = env->GetFieldID(clazz, "item_2g_sub4_amr_ch", "I");
	networkInfo.item_2g_sub4_channel_mode = env->GetFieldID(clazz, "item_2g_sub4_channel_mode", "I");
	networkInfo.item_2g_t3212 = env->GetFieldID(clazz, "item_2g_t3212", "I");
	//3G item0: RRC State and Serving Cell Information 
	networkInfo.item_3g_sub0_rcc_state = env->GetFieldID(clazz, "item_3g_sub0_rcc_state", "I");
	networkInfo.item_3g_sub0_s_freq = env->GetFieldID(clazz, "item_3g_sub0_s_freq", "I");
	networkInfo.item_3g_sub0_s_psc = env->GetFieldID(clazz, "item_3g_sub0_s_psc", "I");
	networkInfo.item_3g_sub0_s_ecio = env->GetFieldID(clazz, "item_3g_sub0_s_ecio", "I");
	networkInfo.item_3g_sub0_s_rscp = env->GetFieldID(clazz, "item_3g_sub0_s_rscp", "I");
	networkInfo.item_3g_tmsi = env->GetFieldID(clazz, "item_3g_tmsi", "I");
	networkInfo.item_3g_t3212 = env->GetFieldID(clazz, "item_3g_t3212", "I");
	//3G Item 1: Intra frequency neighbor cell information array
	//3G Item 2: Inter Frequency neighbor cell information array
	//NM ITEM 0 : General information
	//networkInfo.item_nw_sub0_plmn = env->GetFieldID(clazz, "item_nw_sub0_plmn", "I");
	networkInfo.item_nw_sub0_mnc = env->GetFieldID(clazz, "item_nw_sub0_mnc", "I"); 
	networkInfo.item_nw_sub0_mcc = env->GetFieldID(clazz, "item_nw_sub0_mcc", "I"); 
	networkInfo.item_nw_sub0_lac = env->GetFieldID(clazz, "item_nw_sub0_lac", "I");
	networkInfo.item_nw_sub0_rac = env->GetFieldID(clazz, "item_nw_sub0_rac", "I");
	networkInfo.item_nw_sub0_hplmn = env->GetFieldID(clazz, "item_nw_sub0_hplmn", "I");
	networkInfo.item_td_pid = env->GetFieldID(clazz, "item_td_pid", "I");
	//NM ITEM 1 : Network Management information 
	networkInfo.item_nw_sub1_mm_state = env->GetFieldID(clazz, "item_nw_sub1_mm_state", "I");
	networkInfo.item_nw_sub1_mm_reject = env->GetFieldID(clazz, "item_nw_sub1_mm_reject", "I");
	networkInfo.item_nw_sub1_gmm_state = env->GetFieldID(clazz, "item_nw_sub1_gmm_state", "I");
	networkInfo.item_nw_sub1_attach_reject = env->GetFieldID(clazz, "item_nw_sub1_attach_reject", "I");
	networkInfo.item_nw_sub1_rau_reject = env->GetFieldID(clazz, "item_nw_sub1_rau_reject", "I");
	//NM ITEM 2 : Ciphering Information
	networkInfo.item_nw_sub2_ciphering = env->GetFieldID(clazz, "item_nw_sub2_ciphering", "I");
	networkInfo.item_nw_sub2_UEA1 = env->GetFieldID(clazz, "item_nw_sub2_UEA1", "I");
	networkInfo.item_nw_sub2_GEA1 = env->GetFieldID(clazz, "item_nw_sub2_GEA1", "I");
	networkInfo.item_nw_sub2_GEA2 = env->GetFieldID(clazz, "item_nw_sub2_GEA2", "I");
	networkInfo.item_nw_sub2_A5_1 = env->GetFieldID(clazz, "item_nw_sub2_A5_1", "I");
	networkInfo.item_nw_sub2_A5_2 = env->GetFieldID(clazz, "item_nw_sub2_A5_2", "I");
	networkInfo.item_nw_sub2_A5_3 = env->GetFieldID(clazz, "item_nw_sub2_A5_3", "I");
	//reset EE info structure
	s_NCCellNumber = 0;
	s_intraFreq_num = 0;
	s_interFreq_num = 0;

	return AndroidRuntime::registerNativeMethods(
			env, "com/marvell/networkinfo/NetworkInfoService",
			method_table, NELEM(method_table));
}


/*
 *  * JNI registration.
 *   */

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env = NULL;
	jint result = -1;

	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		LOGE("GetEnv failed");
		goto bail;
	}
	assert(env != NULL);

	if (register_com_marvell_networkinfo_NetworkInfoService(env) < 0) {
		LOGE("Register failed");
		goto bail;
	}

	result = JNI_VERSION_1_4;

bail:
	return result;
}

