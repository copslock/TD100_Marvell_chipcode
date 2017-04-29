/** @file DutWlanIQCal.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

static BYTE bb288=0, bb28c=0, bb28b=0;
static BYTE bb4f=0, rf155=0;
static BYTE chipRev = 0;

#define CHIP_REV_T0 0x00
#define CHIP_REV_W0 0x30
#define CHIP_REV_W1 0x31
#define CHIP_REV_U0 0x10
#define CHIP_REV_A0 0x40

void DutWlanApiClssHeader InitB4IQCal()
{
	int err = 0;
	DWORD rdBack1;
	BYTE rdBack;

	//get chip rev 
	PeekRegDword(0x80002018, &rdBack1, 0);
	chipRev = (BYTE)rdBack1&0xFF;

	//set bit 28 to 1
	err = PeekRegDword(0x8000a924, &rdBack1, 0);
	rdBack1 |= 0x10000000;
	err = PokeRegDword(0x8000a924, rdBack1, 0);

	//set bit 12 to 0  RxPE
	err = PeekRegDword(0x8000a924, &rdBack1, 0);
	rdBack1 &= 0xffffefff;
	err = PokeRegDword(0x8000a924, rdBack1, 0);

	//PE1/PE2 not with MAC
	err = PeekRegDword(0x80002060, &rdBack1, 0);
	rdBack1 |= 0x3000;
	err = PokeRegDword(0x80002060, rdBack1, 0);

	//Allow radio to control PE1/PE2
	WriteRfReg(0x6,0x38, 0);

	//Disable ROS cal at DAC4
	WriteRfReg(0x184, 0xBF, 0);
	WriteRfReg(0x188, 0xBF, 0);

	//TOS gain for TVGA
	WriteRfReg(0x1c8, 0x3, 0);

	//PPDA PPA settling time
	WriteRfReg(0x1a0, 0x00, 0);

	PeekRegDword(0x80002068, &rdBack1, 0);
	rdBack1 |= 0x20;
	PokeRegDword(0x80002068, rdBack1, 0);

	//configuring for gain commands
	//non cumulative mode
	ReadRfReg(0x7a, &rdBack, 0);
	rdBack &= 0xfe;
	WriteRfReg(0x7a, rdBack, 0);

	// latching of pkdetects in the radio
	ReadRfReg(0x7a, &rdBack, 0);
	rdBack |= 0x08;
	WriteRfReg(0x7a, rdBack, 0);

	ReadBBReg(0xE0, &rdBack, 0);
	rdBack |= 0x80;
	WriteBBReg(0xE0, rdBack, 0);

	//set BB registers
	ReadBBReg(0x288, &bb288, 0);
	ReadBBReg(0x28b, &bb28b, 0);
	ReadBBReg(0x28C, &bb28c, 0);

	WriteBBReg(0x288, 0x00, 0);
	WriteBBReg(0x28b, 0x00, 0);
	WriteBBReg(0x28C, 0xC0, 0);

	ReadBBReg(0x4F, &bb4f, 0);
	ReadRfReg(0x155, &rf155, 0);
	return;
}

int DutWlanApiClssHeader rfu_wait_for_signal(int addr, int sigWait)
{
    int waitTime = 0;
    BYTE regVal;
	int errStatus = ERROR_NONE;

    ReadRfReg(addr, &regVal, 0);
    while ( !(regVal & sigWait) )
    {
	  Sleep(10);
	  waitTime++;

        if (waitTime > 60)
        {
			DebugLogRite("rfu_wait_for_signal(0x%X:0x%X) timeout!!!!!!!!!\n", addr, sigWait);
			return ERROR_READY_TIMEOUT;
        }
		else
			ReadRfReg(addr, &regVal, 0);
    }
    
    return errStatus;
}

#define HAL_RF_REG_TR_MR3		0x5
#define HAL_RF_REG_MSRE1		0x7
#define HAL_RF_REG_CAL_MR1		0x47
#define HAL_RF_REG_MCU_CHIP_STATUS1 0xB2
#define HAL_RF_REG_MCU_CHIP_STATUS2 0xC4

#define HAL_BBP_REG_DEV_REG0	0x8
#define HAL_BBP_REG_TX_REG5		0x2D
#define HAL_BBP_REG_TX_REG6		0x2E
#define HAL_BBP_REG_CCA_REG0	0x5A
#define HAL_BBP_REG_DC_REG8		0x10B
#define HAL_BBP_REG_AGC_REG30	0x204
#define HAL_BBP_REG_MDI_REG6	0x286
#define HAL_BBP_REG_RX_REG44	0x385

#define HAL_BBP_REG_RFCAL_REG0	0x3B5
#define HAL_BBP_REG_RFCAL_REG1	0x3B6
#define HAL_BBP_REG_RFCAL_REG2	0x3B7
#define HAL_BBP_REG_RFCAL_REG3	0x3B8
#define HAL_BBP_REG_RFCAL_REG5	0x3BA
#define HAL_BBP_REG_RFCAL_REG6	0x3BB
#define HAL_BBP_REG_RFCAL_REG7	0x3BC
#define HAL_BBP_REG_RFCAL_REG8	0x3BD
#define HAL_BBP_REG_RFCAL_REG9	0x3BE
#define HAL_BBP_REG_RFCAL_REG10	0x3BF
#define HAL_BBP_REG_RFCAL_REG11	0x3C0
#define HAL_BBP_REG_RFCAL_REG12	0x3C1
#define HAL_BBP_REG_RFCAL_REG13	0x3C2
#define HAL_BBP_REG_RFCAL_REG14	0x3C3
#define HAL_BBP_REG_RFCAL_REG15	0x3C4
#define HAL_BBP_REG_RFCAL_REG16	0x3C5
#define HAL_BBP_REG_RFCAL_REG19	0x3C8

int DutWlanApiClssHeader rfu_wait_for_intr(int intrWait)
{
    int status = ERROR_NONE;
	BYTE temp1;

    status = rfu_wait_for_signal(HAL_RF_REG_MCU_CHIP_STATUS1, intrWait);
    if (!status)
    {
		ReadRfReg(HAL_RF_REG_MCU_CHIP_STATUS1,&temp1, 0);
		temp1 = temp1 | 0x01;
		WriteRfReg(HAL_RF_REG_MCU_CHIP_STATUS1, temp1, 0);
    }
    
    return status;
}

#define ROUND_FACTOR		0x10
#define HAL_RF_REG_LO_CONFIG1 	 (0x29)
#define HAL_RF_REG_LO_CONFIG2 	 (0x2A)
#define HAL_RF_REG_LO_CONFIG3 	 (0x2B)
#define HAL_RF_REG_LO_CONFIG4 	 (0x2C)
#define HAL_RF_REG_LO_CONFIG5 	 (0x2D)
int DutWlanApiClssHeader LO_program(int centerFreq, /* in MHz */
               int RefClockHKHz,   /* In 100 kHz */
               int isDiv2)
{
    int bandG = 0;
    int valPR;
    double valDIN;
    double divTop;
    int valDiv, roundVal;
    int temp = 0;
    int regVal;
	int errStatus = 0;
	unsigned long lineNumb = 0;

    /* Determine if this is A band or G Band */
    if (centerFreq <= 3000)
    {
        bandG = 1; /* 1 - G; 0 - A */
    }
    
    /*
    ** For div2
    ** PR = 2 * ROUND(DIV)          
    ** DIV(A) = fc/fref*(1/3)   
    ** DIV(G) = (fc/fref)*(2/3)
    **
    ** Otherwise for div23
    ** PR = ROUND(DIV)          
    ** DIV(A) = fc/fref*(2/3)   
    ** DIV(G) = (fc/fref)*(4/3)
    **
    ** Note that Center Freq are expressed in MHz, e.g. 2412MHz.
    ** Generally the Ref Clock is also expressed in MHz, e.g. 40MHz
    ** However to accomodate 38.4 and still keep integers, the ref
    ** clock values will be passed in in 100KHz values, e.g. 384 (100KHz)
    */
    divTop = ROUND_FACTOR * (bandG + 1) * (10 * centerFreq);

    if (!isDiv2)
    {
        divTop *= 2;
    }
    
    valDiv = (int) divTop / RefClockHKHz / 3;
    roundVal = ROUND_FACTOR * (valDiv / ROUND_FACTOR); /* integer */
 
    if ((valDiv - roundVal) * 2 >= ROUND_FACTOR)
    {
        roundVal += ROUND_FACTOR;                 /* round */
    }
    
    valPR = roundVal / ROUND_FACTOR;

    /*
    ** 2097152 is 0x200000 
    ** DIN = ROUND[2097152*(DIV-ROUND(DIV))
    */
    valDIN = (0x200000 * (divTop - (valPR * ROUND_FACTOR * RefClockHKHz * 3)));
    valDIN /= (RefClockHKHz * 3);

    /* Round */
    if (valDIN >= 0)
    {
        valDIN = (valDIN + ROUND_FACTOR/2) / ROUND_FACTOR;
    }
    else
    {
        valDIN = (valDIN - ROUND_FACTOR/2) / ROUND_FACTOR;
    }

    if (valDIN != 0)
    {
        valDIN++;
    }

    if (isDiv2)
    {
        valPR *= 2;
    }

    /* 29[7:0]= PR[7:0] */
    regVal = valPR & 0xff;
   	WriteRfReg(HAL_RF_REG_LO_CONFIG1, regVal, 0);  
 
    /*
    ** 2a[7] = PR[8]
    ** 2a[6] = 0
    ** 2a[5:4] = 11 if 40  <= PR <  72
    **           10 if 72  <= PR <  136
    **           01 if 136 <= PR <= 504
    */ 
    if (40 <= valPR && valPR < 72)
    {
        temp = 3;
    }
    else if (72 <= valPR && valPR < 136)
    {
        temp = 2;
    }
    else if (136 <= valPR && valPR <= 504)
    {
        temp = 1;
    }
    else
    {
        /* out of range error */
        temp = 0;
        return -1;
    }
    
    regVal = ((valPR & 0x100) >> 1) | (temp <<4);
    WriteRfReg(HAL_RF_REG_LO_CONFIG2, regVal, 0); //*****

    /* 2b[5:0] = DIN[21:16] */
    regVal = (((int)valDIN & 0x3f0000) >> 16);
    WriteRfReg(HAL_RF_REG_LO_CONFIG3, regVal, 0); //*****

    /* 2c[7:0] = DIN[15:8] */
    regVal = ((int)valDIN & 0xff00) >> 8;
    WriteRfReg(HAL_RF_REG_LO_CONFIG4, regVal, 0); //*****

    /* 2d[7:0] = DIN[7:0] */
    regVal = (int)valDIN & 0xff;
    WriteRfReg(HAL_RF_REG_LO_CONFIG5, regVal, 0); //*****

	
	return errStatus;
}

typedef enum Chan_BW

{
      CHANWIDTH10MHZ,
      CHANWIDTH20MHZ,
      CHANWIDTH40MHZ_ABOVE, // secondary channel is upper channel
      CHANWIDTH40MHZ_BELOW, // secondary channel is lower channel
      CHANWIDTH5MHZ,
}  Chan_BW_t;   // will change to Chan_BW_e

int resolve_chnlidx (int aMode, int chan, int chanBW, int *chnlidx)
{
	int chanIdxToUse;

   if(aMode == 0) 
   { /* 2.4 GHz */
		if(chanBW == CHANWIDTH40MHZ_ABOVE) 
		{
            if(chan <= 7) 
			{
                chanIdxToUse = chan + 1;
            } 
			else 
			{
                chanIdxToUse = 8;
            }
        } 
		else if(chanBW == CHANWIDTH40MHZ_BELOW) 
		{
			if(chan >= 5) 
			{
                chanIdxToUse = chan - 3;
            } 
			 else 
			{
                chanIdxToUse = 2;
            }
        } 
		else 
		{                  //20MHz
            chanIdxToUse = chan - 1;
        }
    } 
    else 
    {                     /* 5 GHz */
        if((chanBW == CHANWIDTH40MHZ_ABOVE) || (chanBW == CHANWIDTH40MHZ_BELOW)) {
            switch(chan) {
            case 36:
            case 40:
                chanIdxToUse = 38;
                break;
            case 44:
            case 48:
                chanIdxToUse = 46;
                break;
            case 52:
            case 56:
                chanIdxToUse = 54;
                break;
            case 60:
            case 64:
                chanIdxToUse = 62;
                break;
            case 149:
            case 153:
                chanIdxToUse = 151;
                break;
            case 157:
            case 161:
                chanIdxToUse = 159;
                break;
            case 100:
            case 104:
                chanIdxToUse = 102;
                break;
            case 108:
            case 112:
                chanIdxToUse = 110;
                break;
            case 116:
            case 120:
                chanIdxToUse = 118;
                break;
            case 124:
            case 128:
                chanIdxToUse = 126;
                break;
            case 132:
            case 136:
                chanIdxToUse = 134;
                break;
            default: //52/56
                chanIdxToUse = 54;
                break;
            }
        } else {
            chanIdxToUse = chan;
        }
    }
	*chnlidx = chanIdxToUse;
	return (0);
}
/*
int rf_get_center_freq(int aMode, int chnlidx)
{

    if (aMode == 1)
		return rf_11a_freq_mhz[chan2idx11a[chnlidx]];
	else    
		return rf_11g_freq_mhz[chnlidx];
}
*/
static int			bwMode40MHz = 0;
static int			RefClk = 6;
const int hal_RefClock[] = {
    120,     // REF_CLOCK_12  0x00
    130,     // REF_CLOCK_13  0x01
    192,     // REF_CLOCK_192 0x02
    200,     // REF_CLOCK_20  0x03
    240,     // REF_CLOCK_24  0x04
    260,     // REF_CLOCK_26  0x05
    384,     // REF_CLOCK_384 0x06
    400,     // REF_CLOCK_40  0x07
    440,     // REF_CLOCK_44  0x08
    520,     // REF_CLOCK_52  0x09
};

int DutWlanApiClssHeader rf_set_channel(int aMode, int chnl)
{
	int errStatus = 0;
	int center_freq;
	int chnlidx;

	//ALTERNATE 1
	//resolve the chnlidx to match array
	resolve_chnlidx(aMode, chnl, bwMode40MHz,&chnlidx);
	//Compute and set the Channel Frequency						
	center_freq = 2437;	//rf_get_center_freq(aMode,chnlidx);

	//ALTERNATE 2
	//Compute and set the Channel Frequency						
	//	errChk(wshMimoTx (errLog, parm->Tx_RadioBW_Mode, parm->Tx_ChnlNum, (int) parm->Tx_ext_Chnloffset, 
	//				parm->Tx_ModBW, parm->Tx_ModScheme, &chnlfrequency, parm->band, 0);

	//Riddhi: Based on recommendation from Li Lin 7/28/09
	if(aMode == 0)
	{
		WriteRfReg(0x08,0x51, 0);
		WriteRfReg(0x09,0x14, 0);
		WriteRfReg(0x13,0x10, 0);
		WriteRfReg(0x2E,0x0F, 0);
		WriteRfReg(0x32,0x08, 0);
		WriteRfReg(0x33,0x35, 0);
		WriteRfReg(0x34,0xC7, 0);
		WriteRfReg(0x35,0x04, 0);
		WriteRfReg(0x70,0x50, 0);
		//PPAD_bandsel, TMIX_bandsel
		//WriteRfReg(0x1a2,0x24, 0);
	}


//Resume LAter

	LO_program(center_freq,
               hal_RefClock[RefClk],
               FALSE);

	return errStatus;
}

int DutWlanApiClssHeader lo_leak_cal(int *pLO_I, int *pLO_Q)
{
	int errStatus = 0;
	unsigned long lineNumb = 0;
	DWORD rdBack=0;
	BYTE temp;
	// Rf sends Q channel by default for Lo leakage cal.
	ReadRfReg(0x48, &temp, 0);
	temp |= 0x40;
	WriteRfReg(0x48, temp, 0);

	//We had to back off DAC op because of IM3 tone at the mixer op tx_reg5 was set to 0x30.
	// Trying to make mixer more linear. So set tx_reg5 back to 80 and set the 0x1aa
	ReadRfReg(0x1AA, &temp, 0);
	temp &= 0xE7;
	temp |= 0x18;
	WriteRfReg(0x1AA, temp, 0);
	
	//2G gain settings finalised as of 9/21/09
	//2G gain settings board 49
	WriteRfReg(0x1af, 0x0f, 0);
	WriteBBReg(0x4F, 0xA1, 0);
	WriteRfReg(0x155, 0xFF, 0);
	WriteRfReg(0x156, 0x08, 0);
//	WriteRfReg(0x157, 0xD2, 0);
//	WriteRfReg(0x158, 0x0C, 0);
	//Freeze Rx DVGA gains to 0
	WriteBBReg(0x17B, 0x00, 0);
	WriteBBReg(0x179, 0xE0, 0);
//	WriteBBReg(0x1B0, 0x3B, 0);
	//PPAD 12dB
	//Digital BB gain set to -4 db in steps of 0.5dB (bits 7 to 2 in 2s complement) and Tx VGA1 gain set to 
	WriteRfReg(0x1B1, 0x01, 0);

	//for different chip rev
    if (chipRev == CHIP_REV_T0)
    {
    	WriteRfReg(0x157, 0xD2, 0);
	    WriteRfReg(0x1B0, 0x3B, 0);
    	WriteRfReg(0x158, 0x18, 0);
    }
    else
	{
		//Mixer at m-18, PPAD at 12dB and RX VGA at 3dB
		WriteRfReg(0x157, 0xCE, 0);
		WriteRfReg(0x1B0, 0x3F, 0);
		WriteRfReg(0x158, 0x0B, 0);
	}
  
    if (chipRev == CHIP_REV_A0)
    {
		WriteRfReg(0x1ab, 0x02, 0);
		WriteRfReg(0x1ac, 0x01, 0);

		//PPAD bias lowered for A0
		ReadRfReg(0x1a7, &temp, 0);
		temp &= 0xFC;
		WriteRfReg(0x1a7, temp, 0);
    }
	
	cal_init();
	WriteBBReg(HAL_BBP_REG_TX_REG5, 0x40, 0);
    
	//** To run:
    //reg_wr3(rfcal_reg0, 0x00);//do not enable any iq cal
    WriteBBReg(HAL_BBP_REG_RFCAL_REG0, 0x00, 0);
    //reg_wr3(rfcal_reg1, 0x2a);//en_lo_cal, wait_time = 10; cal_time =10 **
    WriteBBReg(HAL_BBP_REG_RFCAL_REG1, 0x2A, 0);
    //reg_wr3(rfcal_reg3, 0x18);//rf_cal_freq_delta_en, iq_cal_agc_mode 
    WriteBBReg(HAL_BBP_REG_RFCAL_REG3, 0x18, 0);

	//wait 2us.
	Sleep((unsigned long)0.002);

    //reg_wr3(rfcal_reg2, 0x80);//start rf calibration
    WriteBBReg(HAL_BBP_REG_RFCAL_REG2, 0x80, 0);
	ReadBBReg(HAL_BBP_REG_RFCAL_REG3, &temp, 0);
    //Wait till rfcal_reg3 bit7 rf_cal_done = 1
    while ( (temp & 0x80) == 0) 
    {
        //hal_WaitInUs(2); //?
		ReadBBReg(HAL_BBP_REG_RFCAL_REG3,&temp, 0);
	    Sleep((unsigned long)0.002);
	}

 	//** To read result
	//*pLO_I = BBPHal_ReadReg(HAL_BBP_REG_RFCAL_REG13);
    //*pLO_Q = BBPHal_ReadReg(HAL_BBP_REG_RFCAL_REG14);
	ReadBBReg(HAL_BBP_REG_RFCAL_REG13,&temp, 0);
	*pLO_I = temp;
	ReadBBReg(HAL_BBP_REG_RFCAL_REG14,&temp, 0);
	*pLO_Q = temp;
   
	//** restore
    //reg_wr3(rfcal_reg0, 0);
    WriteBBReg(HAL_BBP_REG_RFCAL_REG0,0, 0);
    //reg_wr3(rfcal_reg1, 0);
    WriteBBReg(HAL_BBP_REG_RFCAL_REG1,0, 0);
    //reg_wr3(rfcal_reg3, 0);
    WriteBBReg(HAL_BBP_REG_RFCAL_REG2,0, 0);
	
	return errStatus;
}

int DutWlanApiClssHeader coldpowerupcal()
{
	int status = 0;
	DWORD rdBack1;
	BYTE rdBack;

	//park SP3T to WL_TX
	PeekRegDword(0x8000A710, &rdBack1, 0);
	rdBack1 &= ~(0xFF << 10);
    rdBack1 |= (0xBA << 10); 
	PokeRegDword(0x8000A710, rdBack1, 0);

	PeekRegDword(0x8000A7E0, &rdBack1, 0);
	rdBack1 &= ~(0xF << 0);
    rdBack1 |= (0xE << 0); 
	PokeRegDword(0x8000A7E0, rdBack1, 0);

	//rf 0x7 clear bits 1 and 4
	ReadRfReg(HAL_RF_REG_MSRE1, &rdBack, 0);
	rdBack &= 0xED;
	WriteRfReg(HAL_RF_REG_MSRE1, rdBack, 0);

	//set channel to 6
//	status = SetRfChannel(6, 0);
//	rf_set_channel(0,6);
//	DebugLogRite("SetRfChannel: 0x%08X\n", status);
//	if (status) return status;

	//setcal level to 3
	ReadRfReg(0x07, &rdBack, 0);
	rdBack |= (0x3 << 6);
	WriteRfReg(0x07, rdBack, 0);

	//Release MCU soft reset
	ReadRfReg(0x2, &rdBack, 0);
	rdBack |= 0x10;
	WriteRfReg(0x2, rdBack, 0);

	status = rfu_wait_for_intr(0x40);	// RF MCU_CHIP_STATUS1
	if (status) return status;

	//enable rfu wakeup
	ReadRfReg(0x2, &rdBack, 0);
	rdBack |= 0x2;
	WriteRfReg(0x2, rdBack, 0);

	return status;
}

int DutWlanApiClssHeader cal_init()
{
	int errStatus = 0;
	BYTE temp1;
	  
	//reg_wr3(dev_reg0, 0x08); //set to 40Mhz dev 
   	WriteBBReg(HAL_BBP_REG_DEV_REG0, 0x08, 0);
	//Bandwidth set to 20MHz
	//WriteBBReg(HAL_BBP_REG_DEV_REG0, 0x00, 0);
	//Toggle RxpE to reset ADC. ADC should be reset everytime bandwidth is changed to 
	//reset the clocks for the ADC

	
    //reg_wr3_partial(cca_reg0, 7, 7, 1); //disable_cca = 1
    //BBPHal_SetRegBitmap(HAL_BBP_REG_CCA_REG0, BIT7);
	ReadBBReg(HAL_BBP_REG_CCA_REG0, &temp1, 0);
	temp1 = temp1 | 0x80;
	WriteBBReg(HAL_BBP_REG_CCA_REG0,temp1, 0);

    //reg_wr3_partial (agc_reg30, 7, 7, 0); //en_rfu_rdy = 0
    //BBPHal_ClrRegBitmap(HAL_BBP_REG_AGC_REG30, BIT7);
	ReadBBReg(HAL_BBP_REG_AGC_REG30,&temp1, 0);
	temp1 &= 0x7f;
	WriteBBReg(HAL_BBP_REG_AGC_REG30,temp1, 0);

	
	//reg_wr3_partial (pmc_reg1, 1, 1, 1); //dis_rxpe= 0
    //BBPHal_SetRegBitmap(HAL_BBP_REG_PMC_REG1, BIT1);

	ReadBBReg(0x402,&temp1, 0);
	
	ReadBBReg(HAL_BBP_REG_MDI_REG6,&temp1, 0);
	temp1 = temp1 & 0xFB;
	WriteBBReg(HAL_BBP_REG_MDI_REG6,temp1, 0);

	Sleep((unsigned long)0.001);

	ReadBBReg(HAL_BBP_REG_MDI_REG6,&temp1, 0);
	temp1 = temp1 | 0x04;
	WriteBBReg(HAL_BBP_REG_MDI_REG6,temp1, 0);
	
	//reg_wr3(tx_reg5, 0xc0); //scale factor for sin wave 
	//Mayank check DAC o/p for Rc cal tones
    WriteBBReg(HAL_BBP_REG_TX_REG5, 0xC0, 0);
	//WriteBBReg(HAL_BBP_REG_TX_REG5, 0xdc, 0);
    //reg_wr3(tx_reg6, 0x10); //select sin wave 
    WriteBBReg(HAL_BBP_REG_TX_REG6, 0x10, 0);
	
	return errStatus;
}

int DutWlanApiClssHeader txiq_rxiq1_cal(int *pTX_Alpha, int *pTX_Beta)
{
	int status = 0;
    BYTE temp, capbank;

	// Rf sends Q channel by default for IQ cal.
	ReadRfReg(0x48, &temp, 0);
	temp |= 0x40;
	WriteRfReg(0x48, temp, 0);
	
	//We had to back off DAC op because of IM3 tone at the mixer op tx_reg5 was set to 0x30.
	// Trying to make mixer more linear. So set tx_reg5 back to 80 and set the 0x1aa
	ReadRfReg(0x1AA, &temp, 0);
	temp &= 0xE7;
	temp |= 0x18;
	WriteRfReg(0x1AA, temp, 0);
		
	WriteRfReg(0x1af, 0x0f, 0);
	WriteBBReg(0x4F, 0xA1, 0);
	WriteRfReg(0x155, 0xFF, 0);
	
	//2G gain settings finalised as of 9/21/09
	//2G gain settings board 49
	WriteRfReg(0x156, 0x08, 0);
	WriteRfReg(0x157, 0xD2, 0);
	WriteRfReg(0x158, 0x18, 0);
	
	//Freeze Rx DVGA gains to 0
	WriteBBReg(0x17B, 0x00, 0);
	WriteBBReg(0x179, 0xE0, 0);
	//PPAD 12dB
	WriteRfReg(0x1B0, 0x3F, 0);
	//Digital BB gain set to -4 db in steps of 0.5dB (bits 7 to 2 in 2s complement) and Tx VGA1 gain set to 
	WriteRfReg(0x1B1, 0x01, 0);
		
	//for different chip rev
	if (chipRev == CHIP_REV_T0)
	{
    	WriteRfReg(0x157, 0xD2, 0);
	    WriteRfReg(0x158, 0x18, 0);
    	WriteRfReg(0x1B0, 0x3F, 0);
	}
	else
	{
		//Mixer at m-18, PPAD at 12dB and RX VGA at 3dB
		WriteRfReg(0x157, 0xCE, 0);
		WriteRfReg(0x1B0, 0x3F, 0);
		WriteRfReg(0x158, 0x0B, 0);
	}
	
	if(chipRev == CHIP_REV_A0)
	{
    	WriteRfReg(0x1ab, 0x02, 0);
	    WriteRfReg(0x1ac, 0x01, 0);
		//Mixer at m-18, PPAD at 12dB and RX VGA at 3dB
		ReadRfReg(0x1a7, &temp, 0);
		temp &= 0xFC;
		WriteRfReg(0x1a7, temp, 0);
	}

	cal_init();

	//2G gain setting related. Finalised as of 9/21/09 for 2G.
	WriteBBReg(HAL_BBP_REG_TX_REG5, 0x40, 0);

	//** To run: 
    //reg_wr3(rfcal_reg0, 0x08);//enable iq tx cal **
    WriteBBReg(HAL_BBP_REG_RFCAL_REG0, 0x08, 0);
    //reg_wr3(rfcal_reg1, 0x0a);//wait_time = 10, cal_time = 10 **
	//Default:WriteBBReg(HAL_BBP_REG_RFCAL_REG1, 0x0A, 0);
	WriteBBReg(HAL_BBP_REG_RFCAL_REG1, 0x0F, 0);

    //reg_wr3(rfcal_reg3, 0x18);//rf_cal_freq_delta_en, iq_cal_agc_mode 
    WriteBBReg(HAL_BBP_REG_RFCAL_REG3, 0x18, 0);
	
	ReadRfReg(0x1a2,&capbank, 0);

	for(int i = 0; i<2; i++)
	{
		if (i ==0)
		{
			ReadRfReg(0x1a2, &temp, 0);
			WriteRfReg(0x1a2, temp&0xC7, 0); //Set cap bank to minimum
		}
		else
		{
			ReadRfReg(0x1a2,&temp, 0);
			WriteRfReg(0x1a2,temp|0x38, 0); //Set cap bank to maximum
		}

		Sleep(1);

		//reg_wr3(rfcal_reg2, 0x80);//start rf calibration
		WriteBBReg(HAL_BBP_REG_RFCAL_REG2, 0x80, 0);
	
		ReadBBReg(HAL_BBP_REG_RFCAL_REG3,&temp, 0);
		//Wait till rfcal_reg3 bit7 rf_cal_done = 1
		// while ((BBPHal_ReadReg(HAL_BBP_REG_RFCAL_REG3) & 0x80) == 0) 
		while ( (temp & 0x80) == 0)
		{
			 ReadBBReg(HAL_BBP_REG_RFCAL_REG3,&temp, 0);
			//hal_WaitInUs(2); //??
			Sleep((unsigned long)0.002);
		}

		ReadBBReg(HAL_BBP_REG_RFCAL_REG19,&temp, 0);

		//** To read result
		if(i ==0)
		{
			ReadBBReg(HAL_BBP_REG_RFCAL_REG5, &temp, 0);
			*pTX_Alpha = (signed char)temp;
			ReadBBReg(HAL_BBP_REG_RFCAL_REG6, &temp, 0);
			*pTX_Beta = (signed char)temp;
		}
		else if (i ==1)
		{
			ReadBBReg(HAL_BBP_REG_RFCAL_REG5,&temp, 0);
			*pTX_Alpha += (signed char)temp;
			*pTX_Alpha /= 2;
			ReadBBReg(HAL_BBP_REG_RFCAL_REG6,&temp, 0);
			*pTX_Beta += (signed char)temp;
			*pTX_Beta /= 2;
		}
    		
		WriteBBReg(HAL_BBP_REG_RFCAL_REG2, 0, 0);
	}

	WriteRfReg(0x1a2,capbank, 0);

	WriteBBReg(HAL_BBP_REG_RFCAL_REG0, 0, 0);

    WriteBBReg(HAL_BBP_REG_RFCAL_REG1, 0, 0);
	
	return status;
}

int DutWlanApiClssHeader rxiq1_cal(int *pTX_Alpha, int *pTX_Beta)
{
	int errStatus = 0;
    BYTE temp;
	DWORD rdBack=0;

	//We had to back off DAC op because of IM3 tone at the mixer op tx_reg5 was set to 0x30.
	// Trying to make mixer more linear. So set tx_reg5 back to 80 and set the 0x1aa
	ReadRfReg(0x1AA, &temp, 0);
	temp &= 0xE7;
	temp |= 0x18;
	WriteRfReg(0x1AA, temp, 0);

    if(chipRev == CHIP_REV_A0)
    {
    }
    else
    {
		WriteRfReg(0x1af, 0x0f, 0);
		WriteBBReg(0x4F, 0xA1, 0);
		WriteRfReg(0x155, 0xFF, 0);
		//For TX IQ LPF gain is zero. For Rx IQ LPF gain is zero too.
		//Freeze Rx DVGA gains to 0
		WriteBBReg(0x17B, 0x00, 0);
		WriteBBReg(0x179, 0xE0, 0);
		//Digital BB gain set to -4 db in steps of 0.5dB (bits 7 to 2 in 2s complement) and Tx VGA1 gain set to 
		WriteRfReg(0x1B1, 0x01, 0);
		//2G gain settings end.

		if (chipRev == CHIP_REV_T0)
		{
    		WriteRfReg(0x157, 0xD2, 0);
			WriteRfReg(0x1B0, 0x3F, 0);
    		WriteRfReg(0x158, 0x18, 0);
		}
		else
		{
			//Mixer at m-18, PPAD at 12dB and RX VGA at 3dB
			WriteRfReg(0x157, 0xCE, 0);
			WriteRfReg(0x1B0, 0x3F, 0);
			WriteRfReg(0x158, 0x0B, 0);
		}
    }
    
	cal_init();
	// Clipping scale
	// 2G setting.
	WriteBBReg(HAL_BBP_REG_TX_REG5, 0x40, 0);
		
    //** To run: 
    //reg_wr3(rfcal_reg0, 0x08);//enable iq tx cal **
    WriteBBReg(HAL_BBP_REG_RFCAL_REG0, 0x08, 0);
    //reg_wr3(rfcal_reg1, 0x0a);//wait_time = 10, cal_time = 10 **
//Default:WriteBBReg(HAL_BBP_REG_RFCAL_REG1, 0x0A, 0);
	WriteBBReg(HAL_BBP_REG_RFCAL_REG1, 0x0F, 0);

    //reg_wr3(rfcal_reg3, 0x18);//rf_cal_freq_delta_en, iq_cal_agc_mode 
    WriteBBReg(HAL_BBP_REG_RFCAL_REG3, 0x18, 0);
	
    //wait 2us
    Sleep((unsigned long)0.002);

    //reg_wr3(rfcal_reg2, 0x80);//start rf calibration
    WriteBBReg(HAL_BBP_REG_RFCAL_REG2, 0x80, 0);

	
    ReadBBReg(HAL_BBP_REG_RFCAL_REG3,&temp, 0);
	//Wait till rfcal_reg3 bit7 rf_cal_done = 1
   // while ((BBPHal_ReadReg(HAL_BBP_REG_RFCAL_REG3) & 0x80) == 0) 
    while ( (temp & 0x80) == 0)
	{
         ReadBBReg(HAL_BBP_REG_RFCAL_REG3,&temp, 0);
		//hal_WaitInUs(2); //??
		Sleep((unsigned long)0.002);
    }

	ReadBBReg(HAL_BBP_REG_RFCAL_REG19,&temp, 0);

    //** To read result
     
	//*pTX_Alpha = BBPHal_ReadReg(HAL_BBP_REG_RFCAL_REG5);
    //*pTX_Beta  = BBPHal_ReadReg(HAL_BBP_REG_RFCAL_REG6);
	ReadBBReg(HAL_BBP_REG_RFCAL_REG5,&temp, 0);
	*pTX_Alpha = temp;
	//fprintf(fcal,"Tx Alpha :%x\t", temp);
		
	ReadBBReg(HAL_BBP_REG_RFCAL_REG6,&temp, 0);
	*pTX_Beta = temp;

	//fprintf(fcal,"Tx Beta :%x\n", temp);
    	
    //reg_wr3(rfcal_reg3, 0);
    WriteBBReg(HAL_BBP_REG_RFCAL_REG2, 0, 0);

	WriteBBReg(HAL_BBP_REG_RFCAL_REG0, 0, 0);

    //reg_wr3(rfcal_reg1, 0);
    WriteBBReg(HAL_BBP_REG_RFCAL_REG1, 0, 0);
	//fprintf(fcal,"\n");
	
	return errStatus;
}

int DutWlanApiClssHeader rxiq2_cal(int *pRX_Alpha, int *pRX_Beta)
{
	int errStatus = 0;
	BYTE temp;
	DWORD rdBack;
	BYTE	rdBack1;

	PeekRegDword(0x800021a8, &rdBack, 0);
//	DebugLogRite("0x800021a8:0x%08X, 0x800021a8[9:6]:0x%02X\n", rdBack, (rdBack & 0xf<<6) >> 6);

	ReadBBReg(0x390, &rdBack1, 0);
//	DebugLogRite("BB 0x390:0x%02X\t", rdBack1);
	ReadBBReg(0x391, &rdBack1, 0);
//	DebugLogRite("BB 0x391:0x%02X\n", rdBack1);

	ReadCAUReg(0x3D, &rdBack1, 0);
//	DebugLogRite("CAU 0x3D:0x%02X\n", rdBack1);

	WriteCAUReg(0x39, 0xF7, 0);
	WriteCAUReg(0x3A, 0x53, 0);


	ReadRfReg(0x1AA, &temp, 0);
	temp &= 0xE7;
	temp |= 0x18;
	WriteRfReg(0x1AA, temp, 0);

	//We had to back off DAC op because of IM3 tone at the mixer op tx_reg5 was set to 0x30.
	// Trying to make mixer more linear. So set tx_reg5 back to 80 and set the 0x1aa

	ReadRfReg(0x1AA, &temp, 0);
	temp = temp & 0xE7;
	temp  = temp | 0x18;
	WriteRfReg(0x1AA, temp, 0);

    if(chipRev == CHIP_REV_A0)	
    {
    }
    else
    {
	//2G gain settings
		// Original Gain settings
		WriteRfReg(0x1af, 0x0f, 0);
		WriteBBReg(0x4F, 0xA1, 0);
		WriteRfReg(0x155, 0xFF, 0);
		//Set Rx LPF to 0dB gain
		WriteRfReg(0x156, 0x0A, 0);
		//Freeze Rx DVGA gains to 0
		WriteBBReg(0x17B, 0x00, 0);
		WriteBBReg(0x179, 0xE0, 0);
		WriteRfReg(0x1B1, 0x01, 0);
				
		if (chipRev == CHIP_REV_T0)
		{
			WriteRfReg(0x157, 0xD2, 0);
			WriteRfReg(0x158, 0x12, 0);
    		//PPAD 12dB
			WriteRfReg(0x1B0, 0x3B, 0);
		}
		else
		{
			//Mixer at m-18
			WriteRfReg(0x157, 0xCE, 0);
			//VGA 5dB
			WriteRfReg(0x158, 0x0D, 0);
			//PPAD 12dB
			WriteRfReg(0x1B0, 0x3B, 0);
		}
    }
    
	cal_init();
	WriteBBReg(HAL_BBP_REG_TX_REG5, 0x40, 0);
	
//	Sleep(0.01);
	Sleep(5);
    //** To run:
    //reg_wr3(rfcal_reg0, 0x01);//enable iq rx cal **
    WriteBBReg(HAL_BBP_REG_RFCAL_REG0, 0x01, 0);
	//Alpha calculation only.
	//WriteBBReg(HAL_BBP_REG_RFCAL_REG0, 0x81, 0);
    //reg_wr3(rfcal_reg1, 0x0a);//wait_time = 10, cal_time = 10 **
    WriteBBReg(HAL_BBP_REG_RFCAL_REG1, 0x0F, 0);
	//Search alpha beta both
	//WriteBBReg(HAL_BBP_REG_RFCAL_REG1, 0x8F, 0);
    //reg_wr3(rfcal_reg3, 0x18);//rf_cal_freq_delta_en, iq_cal_agc_mode 
    WriteBBReg(HAL_BBP_REG_RFCAL_REG3, 0x18, 0);

	
    //wait 2us
    Sleep((unsigned long)0.002);

	//reg_wr3(rfcal_reg2, 0x80);//start rf calibration
    WriteBBReg(HAL_BBP_REG_RFCAL_REG2, 0x80, 0);
	
	
    ReadBBReg(HAL_BBP_REG_RFCAL_REG3, &temp, 0);
	//Wait till rfcal_reg3 bit7 rf_cal_done = 1
   // while ((BBPHal_ReadReg(HAL_BBP_REG_RFCAL_REG3) & 0x80) == 0) 
	while (temp & 0x80 == 0)
	{
        Sleep((unsigned long)0.002); 
		ReadBBReg(HAL_BBP_REG_RFCAL_REG3, &temp, 0);
	}
    //** To read result
    //*pRX_Alpha = BBPHal_ReadReg(HAL_BBP_REG_RFCAL_REG7);
    //*pRX_Beta  = BBPHal_ReadReg(HAL_BBP_REG_RFCAL_REG8);
	
	ReadBBReg(HAL_BBP_REG_RFCAL_REG7, &temp, 0);
	*pRX_Alpha = temp;

	//fprintf(fcal,"Rx Alpha :%x\t", temp);
	
	ReadBBReg(HAL_BBP_REG_RFCAL_REG8, &temp, 0);
	*pRX_Beta = temp;

	//fprintf(fcal,"Rx Beta :%x\n", temp);

	// Only for debug.
	WriteBBReg(HAL_BBP_REG_RFCAL_REG11,*pRX_Alpha, 0);
	WriteBBReg(HAL_BBP_REG_RFCAL_REG12,*pRX_Beta, 0);
	
	WriteBBReg(HAL_BBP_REG_RFCAL_REG2, 0, 0);

    //restore
    //reg_wr3(rfcal_reg0, 0);
    WriteBBReg(HAL_BBP_REG_RFCAL_REG0, 0, 0);
    //reg_wr3(rfcal_reg1, 0);
    WriteBBReg(HAL_BBP_REG_RFCAL_REG1, 0, 0);
    //reg_wr3(rfcal_reg3, 0);
    WriteBBReg(HAL_BBP_REG_RFCAL_REG2, 0, 0);

	return errStatus;
}

typedef struct
{
int band;
int chnl;
int lo_cal_I;
int lo_cal_Q;
}lo_leaktable;

#define CH_COUNT			100
//static lo_leaktable lo_table[CH_COUNT];
static int lo_ctr = 0;

int DutWlanApiClssHeader rfu_powerup_cal(int aMode, int chnl, 
										 int *TxAmp, int *TxPhase, 
										 int *RxAmp, int *RxPhase)
{
	int status = 0;
	BYTE rdBack;
	int rfu_cal_lo_i;
	int rfu_cal_lo_q;
	int txamp, txphase;
	BOOL bCalRx=!((RxAmp==NULL)||(RxPhase==NULL));

	// RF register 0x1b4 TxMixer gain (4/15/2010) change to 0x01
	WriteRfReg(0x1B4, 0x01, 0);
	//Rf 0x1A7= PPAD bias; (4/15/2010) change to 0x82
	WriteRfReg(0x1A7, 0x82, 0);

	// set PE1/PE2 to standby through the radio.
	WriteRfReg(0x06,0x3A, 0);

	//RF 0xC4[0] should be 1.
    //hal_rfu_wait_for_signal(HAL_RF_REG_MCU_CHIP_STATUS2, BIT0); // RF 0xC4 
                                                   // [0] RC_CAL_INT_ST
 // ===== RC CAL ============================================
	// enable RC CAL -> clear RC_CAL INTR 0xC4.0
	status = rfu_wait_for_signal(HAL_RF_REG_MCU_CHIP_STATUS2, 0x01);
	if(status) return status;

	//RF 0x47 [4] RCCAL_EN
	ReadRfReg(HAL_RF_REG_CAL_MR1, &rdBack, 0);
	rdBack |= 0x10;
	WriteRfReg(HAL_RF_REG_CAL_MR1, rdBack, 0);

    // disable RC CAL.
	//clear bits Rf 0x47[4] RCCAL_EN
	ReadRfReg(HAL_RF_REG_CAL_MR1, &rdBack, 0);
	rdBack &= 0xEF;
	WriteRfReg(HAL_RF_REG_CAL_MR1, rdBack, 0);

    // ===== LO_LEAK CAL ========================================
    // wait for LO_LEAK_CAL INTR
 //   hal_rfu_wait_for_signal(HAL_RF_REG_MCU_CHIP_STATUS2, BIT1); // RF 0xC4
                                                 // [1] LO_LEAK_CAL_INT_ST
	// enable RC CAL -> clear RC_CAL INTR 0xC4.0
	status = rfu_wait_for_signal(HAL_RF_REG_MCU_CHIP_STATUS2, 0x02);
	if(status) return status;

	// * enable LO CAL -> clear LO_LEAK_CAL INTR 0xC4.1
	//Set RF 0x47 [3] TXIQ_LOLEAK_EN
	ReadRfReg(HAL_RF_REG_CAL_MR1, &rdBack, 0);
	rdBack |= 0x08;
	WriteRfReg(HAL_RF_REG_CAL_MR1, rdBack, 0);

    //only required by A0
	if(chipRev == CHIP_REV_A0)
    {
        // apply BBUD LO LEAK CAL *****
	    lo_leak_cal(&rfu_cal_lo_i, &rfu_cal_lo_q);
    	WriteBBReg(HAL_BBP_REG_RFCAL_REG15, rfu_cal_lo_i, 0);
	    WriteBBReg(HAL_BBP_REG_RFCAL_REG16, rfu_cal_lo_q, 0);
    }
    
	//Update the LO_Cal table structure
//	lo_table[lo_ctr].band = aMode;
//	lo_table[lo_ctr].chnl = chnl;
//	lo_table[lo_ctr].lo_cal_I = rfu_cal_lo_i;
//	lo_table[lo_ctr].lo_cal_Q = rfu_cal_lo_q;
//	lo_ctr++;

    // ===== IQ CAL ========================================
    // ## enable IQ CAL #####
	//Set bit 0x05 [6] IQ_CAL_EN
	ReadRfReg(HAL_RF_REG_TR_MR3, &rdBack, 0);
	rdBack |= 0x40;
	WriteRfReg(HAL_RF_REG_TR_MR3, rdBack, 0);

   // * disable LO CAL
	// Clear bit RF 0x47 [3] TXIQ_LOLEAK_EN
	ReadRfReg(HAL_RF_REG_CAL_MR1, &rdBack, 0);
	rdBack &= 0xF7;
	WriteRfReg(HAL_RF_REG_CAL_MR1, rdBack, 0);

	// ===== IQ CAL ==============================================
    // wait for IQ_CAL state
    //hal_rfu_wait_for_signal(HAL_RF_REG_MCU_CHIP_STATUS1, BIT1); //RF 0xB2
    // [1] IQ_CAL_INT_ST
    // enable RC CAL -> clear RC_CAL INTR 0xC4.0
	status = rfu_wait_for_signal(HAL_RF_REG_MCU_CHIP_STATUS1, 0x02);
	if (status) return status;

	// * enable TXIQ CAL -> clear IQ CAL intr (0xB2.1)
	// Set bit RF 0x47 [3] TXIQ_LOLEAK_EN
	ReadRfReg(HAL_RF_REG_CAL_MR1, &rdBack, 0);
	rdBack |= 0x08;
	WriteRfReg(HAL_RF_REG_CAL_MR1, rdBack, 0);

    // apply BBUD TXIQ CAL *****
	txiq_rxiq1_cal(TxAmp, TxPhase);
//	DebugLogRite("txiq_rxiq1_cal: TxAmp=0x%X\t TxPhase=0x%X\n", *TxAmp, *TxPhase);

	WriteBBReg(HAL_BBP_REG_RFCAL_REG9, *TxAmp, 0);
	WriteBBReg(HAL_BBP_REG_RFCAL_REG10, *TxPhase, 0);

    // ** enable RXIQ1 CAL
	// Set bit RF 0x47 [2] RXIQ1_EN
	ReadRfReg(HAL_RF_REG_CAL_MR1,&rdBack, 0);
	rdBack |= 0x04;
	WriteRfReg(HAL_RF_REG_CAL_MR1,rdBack, 0);

    // * disable TXIQ CAL
	// Clear RF 0x47 [3] TXIQ_LOLEAK_EN
	ReadRfReg(HAL_RF_REG_CAL_MR1,&rdBack, 0);
	rdBack &= 0xF7;
	WriteRfReg(HAL_RF_REG_CAL_MR1,rdBack, 0);

	if (bCalRx)
	{
        //only required by A0
	    if(chipRev == CHIP_REV_A0)
        {
            // apply BBUD LO LEAK CAL *****
    	    lo_leak_cal(&rfu_cal_lo_i, &rfu_cal_lo_q);
        	WriteBBReg(HAL_BBP_REG_RFCAL_REG15, rfu_cal_lo_i, 0);
	        WriteBBReg(HAL_BBP_REG_RFCAL_REG16, rfu_cal_lo_q, 0);
        }
    
		// apply BBUD RXIQ1 CAL *****
		rxiq1_cal(&txamp, &txphase);
		//errChk(sdRobin_rxiq1_softcal(sdRobinHandle,0x05,0x0a, 0);
	//	DebugLogRite("rxiq1_cal: TxAmp=0x%X\t TxPhase=0x%X\n", txamp, txphase);
			
		WriteBBReg(HAL_BBP_REG_RFCAL_REG9, txamp, 0);
		WriteBBReg(HAL_BBP_REG_RFCAL_REG10, txphase, 0);
	}
	
    // *** enable RXIQ2 CAL
	// set bit  RF 0x47 [1] RXIQ2_EN
	ReadRfReg(HAL_RF_REG_CAL_MR1,&rdBack, 0);
	rdBack |= 0x02;
	WriteRfReg(HAL_RF_REG_CAL_MR1,rdBack, 0);
    // ** disable RXIQ1 CAL
	// Clear bit RF 0x47 [2] TXIQ_LOLEAK_EN
	ReadRfReg(HAL_RF_REG_CAL_MR1,&rdBack, 0);
	rdBack &= 0xFB;
	WriteRfReg(HAL_RF_REG_CAL_MR1,rdBack, 0);
	
     
	if (bCalRx)
	{
		// apply BBUD RXIQ2 CAL *****
		rxiq2_cal(RxAmp, RxPhase);

	//	DebugLogRite("rxiq2_cal: RxAmp=0x%X\t RxPhase=0x%X\n", *RxAmp, *RxPhase);

		WriteBBReg(HAL_BBP_REG_RFCAL_REG11, *RxAmp, 0);
		WriteBBReg(HAL_BBP_REG_RFCAL_REG12, *RxPhase, 0);

		WriteBBReg(HAL_BBP_REG_RFCAL_REG9, *TxAmp, 0);
		WriteBBReg(HAL_BBP_REG_RFCAL_REG10, *TxPhase, 0);
	}

	// *** disable RXIQ2 CAL
	//clear bit RF 0x47 [1] RXIQ2_EN
	ReadRfReg(HAL_RF_REG_CAL_MR1,&rdBack, 0);
	rdBack &= 0xFD;
	WriteRfReg(HAL_RF_REG_CAL_MR1,rdBack, 0);
	
    // ## disable IQ CAL #####
	// clear bit RF 0x05 [6] IQ_CAL_EN
	ReadRfReg(HAL_RF_REG_TR_MR3,&rdBack, 0);
	rdBack &= 0xBF;
	WriteRfReg(HAL_RF_REG_TR_MR3,rdBack, 0);
	
	return status;
}

int DutWlanApiClssHeader IQCal_restore()
{
	int status = ERROR_NONE;
	DWORD rdBack1=0;
	BYTE rdBack;

	//set cal level to 3
	ReadRfReg(0x7, &rdBack, 0);
	rdBack &= ~(0x3 << 6);                         // [7:6] CalLevel
    rdBack |= (0x1 << 6);
	WriteRfReg(0x7, rdBack, 0);

	//Restore SP3T to normal operation
    //MAC_REG 0x710[17:10] = 0xAB (force BtTxSwGnt only)
    PeekRegDword(0x8000A710, &rdBack1, 0);
    rdBack1 &= 0xFFFC03FF;
	PokeRegDword(0x8000A710, rdBack1, 0);
    

    //MAC_REG 0x7E0[3:0] = 0xE (force btu TR timing signals only)
    PeekRegDword(0x8000A7E0, &rdBack1, 0);
    rdBack1 &= 0xFFFFFF00;
    PokeRegDword(0x8000A7E0, rdBack1, 0);

	//restore
	WriteBBReg(0x288, bb288, 0);
	WriteBBReg(0x28b, bb28b, 0);
	WriteBBReg(0x28C, bb28c, 0);

	//restore
	//Allow MAC to control PE1/PE2
	PeekRegDword(0x80002060, &rdBack1, 0);
	rdBack1 &= 0xCFFFCFFF;
	PokeRegDword(0x80002060, rdBack1, 0);
	
	//Disallow radio to control PE1/PE2.
	WriteRfReg(0x06,0x00, 0);

	//Control RxPE through register write : bit 28
	PeekRegDword(0x8000A924, &rdBack1, 0);
	rdBack1 &= 0xefffffff;
	PokeRegDword(0x8000A924, rdBack1, 0);
	
	//MAC in Rx mode
	PeekRegDword(0x8000a300,&rdBack1, 0);
	rdBack1 |= 0x00000008;
	PokeRegDword(0x8000a300,rdBack1, 0);

	//need to be restored to original values
	WriteBBReg(0x4F, bb4f, 0);
	WriteRfReg(0x155, rf155, 0);

	//bbu
	WriteBBReg(0x8, 0x0, 0);
	WriteBBReg(0x2d, 0x1, 0);
	WriteBBReg(0x2e, 0x0, 0);
	WriteBBReg(0x5a, 0x2a, 0);
	WriteBBReg(0x17b, 0x1f, 0);
	WriteBBReg(0x204, 0xc2, 0);
	WriteBBReg(0x286, 0x0, 0);
	WriteBBReg(0x352, 0x0, 0);
	WriteBBReg(0x3b6, 0xa, 0);
	WriteBBReg(0x3b8, 0x0, 0);
	WriteBBReg(0x3c4, 0x0, 0);
	WriteBBReg(0x3c5, 0x0, 0);

	WriteRfReg(0x7, 0x62, 0);
	WriteRfReg(0x48, 0x80, 0);
	WriteRfReg(0x156, 0x3c, 0);
	WriteRfReg(0x157, 0xda, 0);
	WriteRfReg(0x158, 0x2a, 0);
	WriteRfReg(0x184, 0x3f, 0);
	WriteRfReg(0x188, 0x3f, 0);
	WriteRfReg(0x1aa, 0x12, 0);
	WriteRfReg(0x1af, 0x00, 0);
	WriteRfReg(0x1b0, 0x7b, 0);
	WriteRfReg(0x1c8, 0xff, 0);

	return status;
}

int DutWlanApiClssHeader PerformTxRxIQCal_Host(
									DWORD Avg,
									DWORD *pTxAmp, DWORD *pTxPhase, 
									DWORD *pRxAmp, DWORD *pRxPhase)
{
	int err = 0;

	InitB4IQCal();

	err = coldpowerupcal();
	if (err) return err;
	
	//G band, channel 6
	err = rfu_powerup_cal(0, 6, (int*)pTxAmp, (int*)pTxPhase, (int*)pRxAmp, (int*)pRxPhase);

	IQCal_restore();

	return err;
}

int DutWlanApiClssHeader PerformTxIQCal_Host(DWORD Avg,
										DWORD *pTxAmp, DWORD *pTxPhase )
{ 
	int err = 0;

	InitB4IQCal();

	err = coldpowerupcal();
	if (err) return err;
	
	//G band, channel 6
	err = rfu_powerup_cal(0, 6, (int*)pTxAmp, (int*)pTxPhase, NULL, NULL);

	IQCal_restore();

	return err; 
}
