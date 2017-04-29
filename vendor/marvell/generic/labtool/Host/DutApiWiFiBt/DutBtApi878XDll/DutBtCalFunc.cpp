/** @file DutBtCalFunc.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

 

/////////////////////////////////

/*
// not supported by this chip  

int DutBtApiClssHeader SetRfPowerUseCal( DutSharedClssObj  *pObj,
										 double PwrdBm, int  ClassMode, int useFeLoss)
{
	int status=0;
	int channel=0;
	BOOL	LimitPwrAsC2=0;
	BYTE	GainPpa6dBStepMode=1;
	char	PwrIndBm_C2[MAXSAVED_PWRLVL_C2]={0},	PwrIndBm_C1_5[MAXSAVED_PWRLVL_C1_5]={0};
	BYTE	GainPpa_C2[MAXSAVED_PWRLVL_C2]={0},	GainPga_C2[MAXSAVED_PWRLVL_C2]={0},	GainBb_C2[MAXSAVED_PWRLVL_C2]={0};
    BYTE	GainPpa_C1_5[MAXSAVED_PWRLVL_C1_5]={0},	GainPga_C1_5[MAXSAVED_PWRLVL_C1_5]={0},	GainBb_C1_5[MAXSAVED_PWRLVL_C1_5]={0};
	int		PwrUpPwrLvl=0;
 	BYTE	TempSlope_Gain=0, TempRd_Ref=0;
	BYTE	FELoss=0;
	BYTE	Xtal=0, ExtXtalSrc=0;

	BT_PWRGAIN_WORD_VC	*ptrGain=NULL;
	WORD Gain3dBm={0},Gainm6dBm={0};
//	int		GainBoost4Bias=1;
//	BYTE	GainPpa=0,	GainPga=0,	GainBb=0;
	double stepdB=0;
	int index=0;
// 1 get channel Current
// 2 set NoCal mode function and resetBT
// 3 set set bais, set Class, set XTAL
// 4 set channel
// 5 set gains
#ifndef _NO_CLASS_
	#define OBJPTR   (*pObj).
#else	// #ifndef _NO_CLASS_
#define OBJPTR
#endif	// #ifndef _NO_CLASS_

#ifndef _NO_CLASS_
	status = OBJPTR LoadCustomizedSettings(1, this);
#else // #ifndef _NO_CLASS_
	status = OBJPTR LoadCustomizedSettings(1, NULL);
#endif //#ifndef _NO_CLASS_
	if(status) return status;

	ExtXtalSrc = ((CardSpecData*) (OBJPTR pMain))->MiscFlag.ExtXtalSource;

  	status = OBJPTR GetAnnexType43Data( &LimitPwrAsC2,
					   &GainPpa6dBStepMode,
						PwrIndBm_C2,
						GainPpa_C2,
						GainPga_C2,
						GainBb_C2,
						PwrIndBm_C1_5,
						GainPpa_C1_5,
						GainPga_C1_5,
						GainBb_C1_5,
						&PwrUpPwrLvl, 
						&TempSlope_Gain,
						&TempRd_Ref,
						&FELoss,
						&Xtal
						);
	if(status) return status;

	if (PWRCTRL_MRVL_C2 == ClassMode)
	{
 
	SortChar(MAXSAVED_PWRLVL_C2, PwrIndBm_C2, (char*)GainPpa_C2,
					(char*)GainPga_C2,
					(char*)GainBb_C2, NULL, NULL, NULL, NULL, NULL);
// boundary check
	if(PwrdBm>PwrIndBm_C2[MAXSAVED_PWRLVL_C2-1] || PwrdBm<PwrIndBm_C2[0])
		return ERROR_MISMATCH;

 
	status = findStepdB(MAXSAVED_PWRLVL_C2, GainPpa6dBStepMode,
					PwrIndBm_C2,
					GainPpa_C2,
					GainPga_C2,
					GainBb_C2, PwrdBm, NULL, &stepdB);
	if(status) return status;

	ptrGain= (BT_PWRGAIN_WORD_VC*)&Gainm6dBm;
	(*ptrGain).BbGain=GainBb_C2[0];
	(*ptrGain).PgaGain=GainPga_C2[0];
	(*ptrGain).PpaGain=GainPpa_C2[0];

	ptrGain= (BT_PWRGAIN_WORD_VC*)&Gain3dBm;
	(*ptrGain).BbGain=GainBb_C2[MAXSAVED_PWRLVL_C2-1];
	(*ptrGain).PgaGain=GainPga_C2[MAXSAVED_PWRLVL_C2-1];
	(*ptrGain).PpaGain=GainPpa_C2[MAXSAVED_PWRLVL_C2-1];
	}

	if (PWRCTRL_MRVL_C1P5 == ClassMode)
	{
 
	SortChar(MAXSAVED_PWRLVL_C1_5, PwrIndBm_C1_5, (char*)GainPpa_C1_5,
					(char*)GainPga_C1_5,
					(char*)GainBb_C1_5, NULL, NULL, NULL, NULL, NULL);

	if(PwrdBm>PwrIndBm_C1_5[MAXSAVED_PWRLVL_C1_5-1] || PwrdBm<PwrIndBm_C1_5[0])
		return ERROR_MISMATCH;

// which part it belows to? 
//	then do the same as class 2. 
	status = findStepdB(MAXSAVED_PWRLVL_C1_5, GainPpa6dBStepMode,
					PwrIndBm_C1_5,
					GainPpa_C1_5,
					GainPga_C1_5,
					GainBb_C1_5, PwrdBm, &index, &stepdB);
	if(status) return status;

 	ptrGain= (BT_PWRGAIN_WORD_VC*)&Gainm6dBm;
	(*ptrGain).BbGain=GainBb_C1_5[index];
	(*ptrGain).PgaGain=GainPga_C1_5[index];
	(*ptrGain).PpaGain=GainPpa_C1_5[index];

	ptrGain= (BT_PWRGAIN_WORD_VC*)&Gain3dBm;
	(*ptrGain).BbGain=GainBb_C1_5[index+1];
	(*ptrGain).PgaGain=GainPga_C1_5[index+1];
	(*ptrGain).PpaGain=GainPpa_C1_5[index+1];


	}


 
	status = GetBtChannel(&channel, 
									NULL, 
									0);
	if(status) channel = 78;

#ifdef _HCI_PROTOCAL_
	status = SetMrvlTempCompModeHC(2, Gain3dBm, Gainm6dBm, TempRd_Ref, TempSlope_Gain);
	if(status) return status;
#endif //#ifdef _HCI_PROTOCAL_
 	
	if(NULL == OBJPTR pAnnex14[0][0] && NULL == OBJPTR pAnnex15[0][0])
	{
		status = SetBtXtal(ExtXtalSrc, Xtal);
		if(status) return status;
	}
	else
	{
		if(OBJPTR pAnnex14[0][0]) 
			status = SetBtXtal(ExtXtalSrc, (*((ANNEX_TYPE_14_PWRCAL2GHZ_VC *) (OBJPTR pAnnex14[0][0]))).CalXtal);
 		else if (OBJPTR pAnnex15[0][0]) 
			status = SetBtXtal(ExtXtalSrc, (*((ANNEX_TYPE_15_PWRCAL5GHZ_VC *) (OBJPTR pAnnex15[0][0]))).CalXtal);

		if(status) return status;
	}


	status = SetBtChannel(channel, 0);
	if(status) return status;



	// if it is class2, the class mode is 0 and bias mode is 0
	// power level need to be between the 2 class 2 points and linearly interpolated. 

	// if it is class1.5, the class mode is 1 and bias mode is 1. 
	// power level no-higher the highest class1.5 and no-lower than the lowest class1.5
 
	if (PWRCTRL_MRVL_C2 == ClassMode)
	{
	status = SetBtPwrControlClass(PWRCTRL_MRVL_C2|MASK_PWRCTRL_OPENLOOP|MASK_PWRCTRL_EXTRANGE);
	if(status) return status;

	status = SetTxPwrModeHighLow(0);
	if(status) return status; 

	status = SetBtPwrPpaPgaInitValue(GainPpa_C2[0], GainPga_C2[0]); 
	if(status) return status;

	status = SetBbGainInitValue(GainBb_C2[0]);
	if(status) return status;

	status = StepPower(stepdB);
	if(status) return status;

	}

	if (PWRCTRL_MRVL_C1P5 == ClassMode)
	{
	status = SetBtPwrControlClass(PWRCTRL_MRVL_C1P5|MASK_PWRCTRL_OPENLOOP|MASK_PWRCTRL_EXTRANGE);
	if(status) return status;

	status = SetTxPwrModeHighLow(1);
	if(status) return status;

	status = SetBtPwrPpaPgaInitValue(GainPpa_C1_5[index], GainPga_C1_5[index]); 
	if(status) return status;

	status = SetBbGainInitValue(GainBb_C1_5[index]);
	if(status) return status;

	status = StepPower(stepdB);
	if(status) return status;
	}

	if(useFeLoss) 
	{
		status = StepPower(FELoss);
		if(status) return status;
	}
	return status;
}

*/   


