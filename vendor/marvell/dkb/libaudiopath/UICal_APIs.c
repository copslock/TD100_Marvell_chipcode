
#include "MarvellAmixer.h"
#include "UICal_APIs.h"

//for memset
#include <stdio.h>
#include <string.h>

//for log10
#include <math.h>




/******************************************************************************
  Local variable
*******************************************************************************/
static unsigned short gEcMode; //0:ES, 1:HF, 2:SP

//For different EcMode(ES/HF/SP) there are different parameters
//These values should be updated in UICal_MSA_Report_Parse()
static AVC_ts AVC_Es;
static AVC_ts AVC_Hf;
static AVC_ts AVC_Sp;

static EC_ts EC_Es;
static EC_ts EC_Hf;
static EC_ts EC_Sp;


//voiceControl is the on/off status of each module
//It is a 32bits 'bitmap', it will be updated in UICal_MSA_Report_Parse()
static unsigned int voiceControl=0;



/****************************************************************
    UI elements
    These elements should be displayed on UI
*****************************************************************/
unsigned short UI_ECMODE;   //value=gEcMode
unsigned short UI_ResidualEC_Status = MSA_MODULE_OFF;		//default:off
UICal_EC UI_EC;
UICal_NS UI_NS_TX;
UICal_NS UI_NS_RX;
UICal_EQ UI_EQ_TX;
UICal_EQ UI_EQ_RX;
UICal_AGC UI_AGC;
UICal_VolMute_TX UI_VOLMUTE_TX;
UICal_VolMute_RX UI_VOLMUTE_RX;
UICal_AVC UI_AVC;





void UICal_MSA_voiceControl_Parse()
{
    UICAL_PRINT("-----------MSA Audio Enhancement Modules On/Off Status-----------\n");

    if(voiceControl & EC_ENABLE)
    {
        UICAL_PRINT("EC: On\n");
        UI_EC.Status = MSA_MODULE_ON;
    }
    else
    {
        UICAL_PRINT("EC: Off\n");
        UI_EC.Status = MSA_MODULE_OFF;
    }

    if(voiceControl & RES_ENABLE)
    {
        UICAL_PRINT("ResidualEC: On\n");
        UI_ResidualEC_Status = MSA_MODULE_ON;
    }
    else
    {
        UICAL_PRINT("ResidualEC: Off\n");
        UI_ResidualEC_Status = MSA_MODULE_OFF;
    }

    if(voiceControl & TX_NS_ENABLE)
    {
        UICAL_PRINT("TX_NS: On\n");
        UI_NS_TX.Status = MSA_MODULE_ON;
    }
    else
    {
        UICAL_PRINT("TX_NS: Off\n");
        UI_NS_TX.Status = MSA_MODULE_OFF;
    }

    if(voiceControl & RX_NS_ENABLE)
    {
        UICAL_PRINT("RX_NS: On\n");
        UI_NS_RX.Status = MSA_MODULE_ON;
    }
    else
    {
        UICAL_PRINT("RX_NS: Off\n");
        UI_NS_RX.Status = MSA_MODULE_OFF;
    }

    if(voiceControl & TX_EQU_ENABLE)
    {
        UICAL_PRINT("TX_EQ: On\n");
        UI_EQ_TX.Status = MSA_MODULE_ON;
    }
    else
    {
        UICAL_PRINT("TX_EQ: Off\n");
        UI_EQ_TX.Status = MSA_MODULE_OFF;
    }

    if(voiceControl & RX_EQU_ENABLE)
    {
        UICAL_PRINT("RX_EQ: On\n");
        UI_EQ_RX.Status = MSA_MODULE_ON;
    }
    else
    {
        UICAL_PRINT("RX_EQ: Off\n");
        UI_EQ_RX.Status = MSA_MODULE_OFF;
    }

    if(voiceControl & TX_AGC_ENABLE)
    {
        UICAL_PRINT("TX_AGC: On\n");
        UI_AGC.Status = MSA_MODULE_ON;
    }
    else
    {
        UICAL_PRINT("TX_AGC: Off\n");
        UI_AGC.Status = MSA_MODULE_OFF;
    }

    if(voiceControl & RX_AVC_ENABLE)
    {
        UICAL_PRINT("RX_AVC: On\n");
        UI_AVC.Status = MSA_MODULE_ON;
    }
    else
    {
        UICAL_PRINT("RX_AVC: Off\n");
        UI_AVC.Status = MSA_MODULE_OFF;
    }

    if(voiceControl & TX_VOLUME_ENABLE)
    {
        UICAL_PRINT("TX_VOLUME: On\n");
        UI_VOLMUTE_TX.Status = MSA_MODULE_ON;
    }
    else
    {
        UICAL_PRINT("TX_VOLUME: Off\n");
        UI_VOLMUTE_TX.Status = MSA_MODULE_OFF;
    }

    if(voiceControl & RX_VOLUME_ENABLE)
    {
        UICAL_PRINT("RX_VOLUME: On\n");
        UI_VOLMUTE_RX.Status = MSA_MODULE_ON;
    }
    else
    {
        UICAL_PRINT("RX_VOLUME: Off\n");
        UI_VOLMUTE_RX.Status = MSA_MODULE_OFF;
    }

    UICAL_PRINT("\n");
}


/******************************************************************************
    When UI Cal is launched, UICal_MSA_Reports_Request(0x35ff) should be called.
    This will trigger MSA to report all parameters needed by UI Cal.

    Then UICal_MSA_Report_Parse() should be used to parse each report.
*******************************************************************************/



/******************************************************************************
 Request Status Report

There are one parameter:
1) unsigned short modulesBitmap
    0x05ff means request the report of these modules:
	EC_ID,
	TX_NS_ID,
	TX_EQ_ID,
	TX_AGC_ID,
	TX_VOLUME_AND_MUTE_ID,
	RX_NS_ID,
	RX_EQ_ID,
	RX_AVC_ID,
	RX_VOLUME_AND_MUTE_ID,
	////RX_TX_ENERGY,                    ////Not parse, not displayed
	VOICE_CONTROL_AND_VOCODER_CONTROL_ID,
	////MODEM_INFO_ID,                  ////Not parse, not displayed
	////TX_CTM_ID,                      ////Not parse, not displayed
	////RX_CTM_ID,                      ////Not parse, not displayed
	////EC_FIR_ID,                          ////Not parse, not displayed
	////VOLUME_EC_THRESHOLD_ID ////Not parse, not displayed
*******************************************************************************/

void UICal_MSA_Reports_Request(unsigned short modulesBitmap)
{
    unsigned short cmd[MSA_DATA_LENGTH];
    memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

    cmd[0]=VOICE_MODULE_DEBUG;  //opcode
    cmd[1]=0x4;                                 //length:count in char

    //modules Bitmap
    cmd[2]=modulesBitmap;
    cmd[3]=0x200;

    //Send cmd to MSA...
    UICal_send_MSA_request(cmd);
}



/*******************************************************************************
    Parse the report file from the start to end

    Will update contents in:
        unsigned short gEcMode; //0:ES, 1:HF, 2:SP

        UICal_EC UI_EC;
        UICal_NS UI_NS_TX;
        UICal_NS UI_NS_RX;
        UICal_EQ UI_EQ_TX;
        UICal_EQ UI_EQ_RX;
        UICal_AGC UI_AGC;
        UICal_VolMute_TX UI_VOLMUTE_TX;
        UICal_VolMute_RX UI_VOLMUTE_RX;
        UICal_AVC UI_AVC;
********************************************************************************/
void UICal_MSA_Report_Parse()
{
    unsigned short opcode;
    unsigned short length;
    unsigned short moduleID;
    unsigned short VoiceDebugFrequent;
    unsigned short data[MSA_DATA_LENGTH];

    FILE *statusFile = NULL;
    statusFile = fopen(MSA_status_filename, "rb");

    while ((statusFile != NULL) && (fread(data, sizeof(unsigned short), MSA_DATA_LENGTH, statusFile) == MSA_DATA_LENGTH))
    {
        opcode=data[0];
        length=data[1];
        moduleID=data[2];
        VoiceDebugFrequent=data[3];

        //All MSA report has opcode=0x49
        //length must be greater than 2
        if((opcode!=0x49)||(length<2))
	{
	  fclose(statusFile);
          return;
	}

        switch(moduleID)
        {
            case EC_ID:
        //U16 opcode=0x0049
        //U16 length
        //U16 module ID(0: EC_ID)

        if(VoiceDebugFrequent==OneShotMode)
        {
            UI_EC.sampleDelay= data[12];
            UI_EC.ecLmsBurst = data[13];

            gEcMode = data[4]; //0:ES, 1:HF, 2:SP

            //Update the UI element
            UI_ECMODE = gEcMode;

            UI_EC.ResMode= data[5];  //ResMode: ATTN(0) / DTX(1)

            EC_Es.AttenuationFactor1= data[6];
            EC_Hf.AttenuationFactor1= data[7];
            EC_Sp.AttenuationFactor1= data[8];
            EC_Es.EchoToNoiseThresFactor= data[9];
            EC_Hf.EchoToNoiseThresFactor= data[10];
            EC_Sp.EchoToNoiseThresFactor= data[11];

            //Update with new value
            if(gEcMode == ES)
            {
                UI_EC.attenuationFactor1 = EC_Es.AttenuationFactor1;
                UI_EC.EchoToNoiseThresFactor = EC_Es.EchoToNoiseThresFactor;
            }
            else if(gEcMode == HF)
            {
                UI_EC.attenuationFactor1 = EC_Hf.AttenuationFactor1;
                UI_EC.EchoToNoiseThresFactor  = EC_Hf.EchoToNoiseThresFactor;
            }
            else if(gEcMode == SP)
            {
                UI_EC.attenuationFactor1 = EC_Sp.AttenuationFactor1;
                UI_EC.EchoToNoiseThresFactor = EC_Sp.EchoToNoiseThresFactor;
            }
        	UICAL_PRINT("EC_TX_Get, ResMode: 0x%x, sampleDelay: 0x%x, ecLmsBurst: 0x%x, attenuationFactor1: 0x%x, EchoToNoiseThresFactor: 0x%x\n", gEcMode,UI_EC.sampleDelay,UI_EC.ecLmsBurst,UI_EC.attenuationFactor1,UI_EC.EchoToNoiseThresFactor);
        }
        break;

           case TX_NS_ID:
        //U16 opcode=0x0049
        //U16 length
        //U16 module (1: TX_NS_ID)

        if(VoiceDebugFrequent==OneShotMode)
        {
            UI_NS_TX.nsMaxSupress = data[4];
            UI_NS_TX.nsMaxNoise = data[5];

            UICAL_PRINT("NS_TX_Get, nsMaxSupress: 0x%x, nsMaxNoise: 0x%x\n", UI_NS_TX.nsMaxSupress,UI_NS_TX.nsMaxNoise);
        }
        break;

           case TX_EQ_ID:
        //U16 opcode=0x0049
        //U16 length
        //U16 module (2: TX_EQ_ID)
        //U16 VoiceDebugFrequent
        //U16 EQ length

        UI_EQ_TX.Eq_Length = data[4];
        if(UI_EQ_TX.Eq_Length == 15) //EQ length
        {
            //This filter is Symmetrical Coefficients filter,  only half of it is needed(data[5]~data[12])
            UI_EQ_TX.EqTap[0] = data[5];
            UI_EQ_TX.EqTap[1] = data[6];
            UI_EQ_TX.EqTap[2] = data[7];
            UI_EQ_TX.EqTap[3] = data[8];
            UI_EQ_TX.EqTap[4] = data[9];
            UI_EQ_TX.EqTap[5] = data[10];
            UI_EQ_TX.EqTap[6] = data[11];
            UI_EQ_TX.EqTap[7] = data[12];

            UICAL_PRINT("EQ_TX_Get: 15Taps, Tap1: 0x%x, Tap2: 0x%x, Tap3: 0x%x, Tap4: 0x%x, Tap5: 0x%x, Tap6: 0x%x, Tap7: 0x%x, Tap8: 0x%x\n", data[5], data[6], data[7], data[8], data[9], data[10], data[11], data[12]);
        }
        else
        {
            //This filter is Symmetrical Coefficients filter,  only half of it is needed(data[5]~data[20])
            UI_EQ_TX.EqTap[0] = data[5];
            UI_EQ_TX.EqTap[1] = data[6];
            UI_EQ_TX.EqTap[2] = data[7];
            UI_EQ_TX.EqTap[3] = data[8];
            UI_EQ_TX.EqTap[4] = data[9];
            UI_EQ_TX.EqTap[5] = data[10];
            UI_EQ_TX.EqTap[6] = data[11];
            UI_EQ_TX.EqTap[7] = data[12];

            UI_EQ_TX.EqTap[8] = data[13];
            UI_EQ_TX.EqTap[9] = data[14];
            UI_EQ_TX.EqTap[10] = data[15];
            UI_EQ_TX.EqTap[11] = data[16];
            UI_EQ_TX.EqTap[12] = data[17];
            UI_EQ_TX.EqTap[13] = data[18];
            UI_EQ_TX.EqTap[14] = data[19];
            UI_EQ_TX.EqTap[15] = data[20];
	    UICAL_PRINT("EQ_TX_Get: 31Taps, Tap1: 0x%x, Tap2: 0x%x, Tap3: 0x%x, Tap4: 0x%x, Tap5: 0x%x, Tap6: 0x%x, Tap7: 0x%x, Tap8: 0x%x, Tap9: 0x%x, Tap10: 0x%x, Tap11: 0x%x, Tap12: 0x%x, Tap13: 0x%x, Tap14: 0x%x, Tap15: 0x%x, Tap16: 0x%x\n", data[5], data[6], data[7], data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15], data[16], data[17], data[18], data[19], data[20]); 
	}
        break;

            case TX_AGC_ID:
        //U16 opcode=0x0049
        //U16 length
        //U16 module (3:TX_AGC_ID)

        if(VoiceDebugFrequent==OneShotMode)
        {
            UI_AGC.MaxAgcGain = data[4];
            UI_AGC.Level1 = data[5];

            AVC_Es.MinEnvelopeTx = data[6];
            AVC_Hf.MinEnvelopeTx = data[7];
            AVC_Sp.MinEnvelopeTx = data[8];

            if(gEcMode == ES)
            {
                UI_AGC.MinEnvelope	= AVC_Es.MinEnvelopeTx;
            }
            else if(gEcMode == HF)
            {
                UI_AGC.MinEnvelope	= AVC_Hf.MinEnvelopeTx;
            }
            else if(gEcMode == SP)
            {
                UI_AGC.MinEnvelope	= AVC_Sp.MinEnvelopeTx;
            }

            UI_AGC.initPeriod = data[9];
            UICAL_PRINT("AGC_TX_Get, MaxAgcGain: 0x%x, Level1: 0x%x, MinEnvelope: 0x%x, initPeriod: 0x%x\n", UI_AGC.MaxAgcGain,UI_AGC.Level1,UI_AGC.MinEnvelope,UI_AGC.initPeriod);
        }
        break;

            case TX_VOLUME_AND_MUTE_ID:
        //U16 opcode=0x0049
        //U16 length
        //U16 module (4: TX_VOLUME_AND_MUTE_ID)

        if(VoiceDebugFrequent==OneShotMode)
        {
            float ftmp;
            ftmp = (float)(data[4])/Q13;
            if (ftmp < 1)
                UI_VOLMUTE_TX.VolumeIndex = (short)(20*log10(ftmp) - 0.5) + 37;
            else
                UI_VOLMUTE_TX.VolumeIndex = (short)(20*log10(ftmp) + 0.5) + 37;

            UI_VOLMUTE_TX.muteType = data[5];	//MSA just return 0 or 1(mute off/on), If mute on,  TX is always DC

            /**************************************************
           !!!!!!!!Mark!!!!!!!!
           Currently these three parameters are not returned from MSA.
           It need to upgrade MSA code.
          ***************************************************/
            if(data[1] > 8) //More than 8 bytes ==>New MSA code
            {//New MSA code which report more data
                /*EC_Es.ECRefThresExp = data[6];
                EC_Es.PeakInThreshExp = data[7];
                EC_Es.RESRefThresExp = data[8];

                EC_Hf.ECRefThresExp = data[9];
                EC_Hf.PeakInThreshExp = data[10];
                EC_Hf.RESRefThresExp = data[11];

                EC_Sp.ECRefThresExp = data[12];
                EC_Sp.PeakInThreshExp = data[13];
                EC_Sp.RESRefThresExp = data[14];

                if(gEcMode == ES)
                {
                    UI_VOLMUTE_TX.ECRefThresExp = EC_Es.ECRefThresExp;
                    UI_VOLMUTE_TX.PeakInThreshExp = EC_Es.PeakInThreshExp;
                }
                else if(gEcMode == HF)
                {
                    UI_VOLMUTE_TX.ECRefThresExp = EC_Hf.ECRefThresExp;
                    UI_VOLMUTE_TX.PeakInThreshExp = EC_Hf.PeakInThreshExp;
                }
                else if(gEcMode == SP)
                {
                    UI_VOLMUTE_TX.ECRefThresExp = EC_Sp.ECRefThresExp;
                    UI_VOLMUTE_TX.PeakInThreshExp = EC_Sp.PeakInThreshExp;
                }*/

		  //modified by Hongxing
		  EC_Es.ECRefThresExp = data[6];
                EC_Es.PeakInThreshExp = data[7];

                EC_Hf.ECRefThresExp = data[8];
                EC_Hf.PeakInThreshExp = data[9];

                EC_Sp.ECRefThresExp = data[10];
                EC_Sp.PeakInThreshExp = data[11];

		  UI_VOLMUTE_TX.RESRefThresExp = data[12];

                if(gEcMode == ES)
                {
                    UI_VOLMUTE_TX.ECRefThresExp = EC_Es.ECRefThresExp;
                    UI_VOLMUTE_TX.PeakInThreshExp = EC_Es.PeakInThreshExp;
                }
                else if(gEcMode == HF)
                {
                    UI_VOLMUTE_TX.ECRefThresExp = EC_Hf.ECRefThresExp;
                    UI_VOLMUTE_TX.PeakInThreshExp = EC_Hf.PeakInThreshExp;
                }
                else if(gEcMode == SP)
                {
                    UI_VOLMUTE_TX.ECRefThresExp = EC_Sp.ECRefThresExp;
                    UI_VOLMUTE_TX.PeakInThreshExp = EC_Sp.PeakInThreshExp;
                }
				
            }
            else
            {//Old MSA code
                UI_VOLMUTE_TX.ECRefThresExp = INVALID_MSA_PARAMETER;
                UI_VOLMUTE_TX.PeakInThreshExp = INVALID_MSA_PARAMETER;
                UI_VOLMUTE_TX.RESRefThresExp = INVALID_MSA_PARAMETER;
            }
	    UICAL_PRINT("TX_VOLUME_AND_MUTE_Get, muteType: 0x%x, VolumeIndex: 0x%x, ECRefThresExp: 0x%x, RESRefThresExp: 0x%x, PeakInThreshExp: 0x%x\n", UI_VOLMUTE_TX.muteType,UI_VOLMUTE_TX.VolumeIndex, UI_VOLMUTE_TX.ECRefThresExp,UI_VOLMUTE_TX.RESRefThresExp, UI_VOLMUTE_TX.PeakInThreshExp);
        }
        break;

            case RX_NS_ID:
        //U16 opcode=0x0049
        //U16 length
        //U16 module (5: RX_NS_ID)

        if(VoiceDebugFrequent==OneShotMode)
        {
            UI_NS_RX.nsMaxSupress = data[4];
            UI_NS_RX.nsMaxNoise = data[5];
            UICAL_PRINT("NS_RX_Get, nsMaxSupress: 0x%x, nsMaxNoise: 0x%x\n", UI_NS_RX.nsMaxSupress,UI_NS_RX.nsMaxNoise);
        }
        break;

            case RX_EQ_ID:
        //U16 opcode=0x0049
        //U16 length
        //U16 module (6: RX_EQ_ID)
        //U16 VoiceDebugFrequent
        //U16 EQ Length

        UI_EQ_RX.Eq_Length = data[4];
        if(UI_EQ_RX.Eq_Length == 15) //EQ length
        {
            //This filter is Symmetrical Coefficients filter,  only half of it is needed(data[5]~data[12])
            UI_EQ_RX.EqTap[0] = data[5];
            UI_EQ_RX.EqTap[1] = data[6];
            UI_EQ_RX.EqTap[2] = data[7];
            UI_EQ_RX.EqTap[3] = data[8];
            UI_EQ_RX.EqTap[4] = data[9];
            UI_EQ_RX.EqTap[5] = data[10];
            UI_EQ_RX.EqTap[6] = data[11];
            UI_EQ_RX.EqTap[7] = data[12];

            UICAL_PRINT("EQ_RX_Get:15Taps, Tap1: 0x%x, Tap2: 0x%x, Tap3: 0x%x, Tap4: 0x%x, Tap5: 0x%x, Tap6: 0x%x, Tap7: 0x%x, Tap8: 0x%x\n", data[5], data[6], data[7], data[8], data[9], data[10], data[11], data[12]);
        }
        else
        {
            //This filter is Symmetrical Coefficients filter,  only half of it is needed(data[5]~data[20])
            UI_EQ_RX.EqTap[0] = data[5];
            UI_EQ_RX.EqTap[1] = data[6];
            UI_EQ_RX.EqTap[2] = data[7];
            UI_EQ_RX.EqTap[3] = data[8];
            UI_EQ_RX.EqTap[4] = data[9];
            UI_EQ_RX.EqTap[5] = data[10];
            UI_EQ_RX.EqTap[6] = data[11];
            UI_EQ_RX.EqTap[7] = data[12];

            UI_EQ_RX.EqTap[8] = data[13];
            UI_EQ_RX.EqTap[9] = data[14];
            UI_EQ_RX.EqTap[10] = data[15];
            UI_EQ_RX.EqTap[11] = data[16];
            UI_EQ_RX.EqTap[12] = data[17];
            UI_EQ_RX.EqTap[13] = data[18];
            UI_EQ_RX.EqTap[14] = data[19];
            UI_EQ_RX.EqTap[15] = data[20];

            UICAL_PRINT("EQ_RX_Get:31Taps, Tap1: 0x%x, Tap2: 0x%x, Tap3: 0x%x, Tap4: 0x%x, Tap5: 0x%x, Tap6: 0x%x, Tap7: 0x%x, Tap8: 0x%x, Tap9: 0x%x, Tap10: 0x%x, Tap11: 0x%x, Tap12: 0x%x, Tap13: 0x%x, Tap14: 0x%x, Tap15: 0x%x, Tap16: 0x%x\n", data[5], data[6], data[7], data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15], data[16], data[17], data[18], data[19], data[20]); 
        }
        break;

            case RX_AVC_ID:
        //U16 opcode=0x0049
        //U16 length
        //U16 module (7:RX_AVC_ID)

        if(VoiceDebugFrequent==OneShotMode)
        {
            UI_AVC.MaxAgcGain = data[4];
            UI_AVC.Level1 = data[5];

            AVC_Es.MinEnvelopeRx = data[6];
            AVC_Es.LowAmbientNoise = data[10];
            AVC_Hf.MinEnvelopeRx = data[7];
            AVC_Hf.LowAmbientNoise = data[11];
            AVC_Sp.MinEnvelopeRx = data[8];
            AVC_Sp.LowAmbientNoise = data[12];

            if(gEcMode == ES)
            {
                UI_AVC.MinEnvelope	= AVC_Es.MinEnvelopeRx;
                UI_AVC.LowAmbientNoise = AVC_Es.LowAmbientNoise;
            }
            else if(gEcMode == HF)
            {
                UI_AVC.MinEnvelope = AVC_Hf.MinEnvelopeRx;
                UI_AVC.LowAmbientNoise = AVC_Hf.LowAmbientNoise;
            }
            else if(gEcMode == SP)
            {
                UI_AVC.MinEnvelope = AVC_Sp.MinEnvelopeRx;
                UI_AVC.LowAmbientNoise = AVC_Sp.LowAmbientNoise;
            }

            UI_AVC.DrcGainFactor = data[9];
            UI_AVC.MaxDrcGain = data[13];
            UI_AVC.AdditionalGain = data[14];
            UI_AVC.initPeriod = data[15];
            UICAL_PRINT("AVC_RX_Get, MaxAgcGain: 0x%x, Level1: 0x%x, MinEnvelope: 0x%x, DrcGainFactor: 0x%x, LowAmbientNoise: 0x%x, MaxDrcGain: 0x%x, AdditionalGain: 0x%x, initPeriod: 0x%x\n", UI_AVC.MaxAgcGain,UI_AVC.Level1,AVC_Es.MinEnvelopeRx,UI_AVC.DrcGainFactor,UI_AVC.LowAmbientNoise,UI_AVC.MaxDrcGain,UI_AVC.AdditionalGain,UI_AVC.initPeriod);
        }
        break;

            case RX_VOLUME_AND_MUTE_ID:
        //U16 opcode=0x0049
        //U16 length
        //U16 module (8: RX_VOLUME_AND_MUTE_ID)

        if(VoiceDebugFrequent==OneShotMode)
        {
            float ftmp;
            ftmp = (float)(data[4])/Q13;
            if (ftmp < 1)
                UI_VOLMUTE_RX.VolumeIndex = (short)(20*log10(ftmp) - 0.5) + 37;
            else
                UI_VOLMUTE_RX.VolumeIndex = (short)(20*log10(ftmp) + 0.5) + 37;

            UI_VOLMUTE_RX.muteType = data[5];   //MSA just return 0 or 1(mute off/on)

            /**************************************************
           !!!!!!!!Mark!!!!!!!!
           Side Tone in MSA is disable.
          ***************************************************/
            #if 0
          unsigned short SideToneVolIndex;
          ftmp = (float)(cmd[6])/Q13;
          if (ftmp < 1)
              SideToneVolIndex = (short)(20*log10(ftmp) - 0.5) + 37;
          else
              SideToneVolIndex = (short)(20*log10(ftmp) + 0.5) + 37;
           #endif

            /**************************************************
           !!!!!!!!Mark!!!!!!!!
           Currently MSA do not return muteType(DC/RN).
           It need to upgrade MSA code.
          ***************************************************/
            if(data[1] > 10)    //More than 10 bytes==>New MSA code
            {//New MSA code which report more data
                if(data[5]==1)  //mute on
                {
                    /**************************************************
                   New MSA  code will fill data[7] with:
                   0: DC (Quiet)
                   1: RN (Random Noise)

                   UI_VOLMUTE_RX.muteType value range:
                    0:OFF
                    1:ON_DC
                    2:ON_RN

                    So, add 1 here.
                  ***************************************************/
                    UI_VOLMUTE_RX.muteType = data[7]+1;
                }
            }

            UICAL_PRINT("RX_VOLUME_AND_MUTE_Get, muteType: 0x%x, VolumeIndex: 0x%x\n", UI_VOLMUTE_RX.muteType,UI_VOLMUTE_RX.VolumeIndex);
        }
        break;

           case VOICE_CONTROL_AND_VOCODER_CONTROL_ID:
      //U16 opcode=0x0049
      //U16 length
      //U16 module (0xa: VOICE_CONTROL_AND_VOCODER_CONTROL_ID)
      //U16 VoiceDebugFrequent
      voiceControl=(data[4]<<16) + data[5];

      /**************************************************
       !!!!!!!!Mark!!!!!!!!
       Below parameters are not used by UI cal
      ***************************************************/
       #if 0
      unsigned short	    DtxState;
      unsigned short	    VocoderTypeRx, VocoderTypeTx;
      unsigned short	    VocoderRateRx, VocoderRateTx;

      DtxState = cmd[6];
      VocoderTypeTx = cmd[7];
      VocoderTypeRx = VocoderTypeTx;
      VocoderRateTx = cmd[8];
      VocoderRateRx = VocoderRateTx;
       #endif

       UICAL_PRINT("VOICE_CONTROL_AND_VOCODER_CONTROL_Get, voiceControl : 0x%x\n", voiceControl);     //32bits

       //Parse modules enable/disable status
        UICal_MSA_voiceControl_Parse();
      break;

           default:
           /*
      UI cal do not use these:

           RX_TX_ENERGY,
           MODEM_INFO_ID,
           TX_CTM_ID,
           RX_CTM_ID,
           EC_FIR_ID,
           VOLUME_EC_THRESHOLD_ID
           */
      break;
       }
    }

    fclose(statusFile);

}




/******************************************************************************
 NS(Noise Suppression)

 There are two parameters:
 1) unsigned short nsMaxSupress
 2) unsigned short nsMaxNoise
*******************************************************************************/

void UICal_MSA_NS_TX_Set(unsigned short nsMaxSupress, unsigned short nsMaxNoise)
{
    unsigned short cmd[MSA_DATA_LENGTH];
    memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

    cmd[0]=NS_CONTROL;   //opcode
    cmd[1]=0x6;                  //length:count in char

    cmd[2]=TX_PATH;                 //path:TX
    cmd[3]=nsMaxSupress;
    cmd[4]=nsMaxNoise;

    //Send cmd to MSA...
    UICal_send_MSA_setting(cmd);
}


void UICal_MSA_NS_RX_Set(unsigned short nsMaxSupress, unsigned short nsMaxNoise)
{
    unsigned short cmd[MSA_DATA_LENGTH];
    memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

    cmd[0]=NS_CONTROL;   //opcode
    cmd[1]=0x6;                  //length:count in char

    cmd[2]=RX_PATH;                 //path:RX
    cmd[3]=nsMaxSupress;
    cmd[4]=nsMaxNoise;

     //Send cmd to MSA...
    UICal_send_MSA_setting(cmd);
}


void UICal_MSA_NS_TX_TurnOnOff(unsigned short On)
{
    unsigned short cmd[MSA_DATA_LENGTH];
    memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

    if(On == MSA_MODULE_ON)
    {
        voiceControl |= (TX_NS_ENABLE);
    }
    else
    {
        voiceControl &= ~(TX_NS_ENABLE);
    }

    cmd[0]=VOICE_CONTROL;   //opcode
    cmd[1]=0x4;                  //length:count in char

    cmd[2] = (voiceControl & 0xffff0000) >> 16;
    cmd[3] = (voiceControl & 0x0000ffff);

    //Send cmd to MSA...
    UICal_send_MSA_setting(cmd);
}


void UICal_MSA_NS_RX_TurnOnOff(unsigned short On)
{
    unsigned short cmd[MSA_DATA_LENGTH];
    memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

    if(On == MSA_MODULE_ON)
    {
        voiceControl |= (RX_NS_ENABLE);
    }
    else
    {
        voiceControl &= ~(RX_NS_ENABLE);
    }

    cmd[0]=VOICE_CONTROL;   //opcode
    cmd[1]=0x4;                  //length:count in char

    cmd[2] = (voiceControl & 0xffff0000) >> 16;
    cmd[3] = (voiceControl & 0x0000ffff);


    //Send cmd to MSA...
    UICal_send_MSA_setting(cmd);
}



/******************************************************************************
 EQ(Equalizer)

 There are one parameter:
 1) UICal_EQ * Eq

*******************************************************************************/

void UICal_MSA_EQ_TX_Set(UICal_EQ *Eq)
{
    unsigned short cmd[MSA_DATA_LENGTH];
    memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

    cmd[0]=EQ_CONTROL;   //opcode

    if(Eq->Eq_Length==15)
    {
        //cmd[1]=(EQ_FILTER_15_TAPS_ORDER+1)*sizeof(unsigned short);                  //length:count in char
        cmd[1] = ((EQ_FILTER_15_TAPS_ORDER+1)/2 + 1)*sizeof(unsigned short);       //length:count in char, modified by Hongxing
        cmd[2]=TX_PATH_15_TAPS;

        //This filter is Symmetrical Coefficients filter,  only half of it is needed(cmd[3]~cmd[10])
        cmd[3] = Eq->EqTap[0];
        cmd[4] = Eq->EqTap[1];
        cmd[5] = Eq->EqTap[2];
        cmd[6] = Eq->EqTap[3];
        cmd[7] = Eq->EqTap[4];
        cmd[8] = Eq->EqTap[5];
        cmd[9] = Eq->EqTap[6];

        cmd[10] = Eq->EqTap[7];

        /*cmd[11] = cmd[9];
        cmd[12] = cmd[8];
        cmd[13] = cmd[7];
        cmd[14] = cmd[6];
        cmd[15] = cmd[5];
        cmd[16] = cmd[4];
        cmd[17] = cmd[3];*/ 		//modified by Hongxing
    }
    else if(Eq->Eq_Length==31)
    {
        //cmd[1]=(EQ_FILTER_31_TAPS_ORDER+1)*sizeof(unsigned short);                  //length:count in char
        cmd[1]=((EQ_FILTER_31_TAPS_ORDER+1)/2+ 1)*sizeof(unsigned short) ;         //length:count in char, modified by Hongxing
        cmd[2]=TX_PATH_31_TAPS;

        //This filter is Symmetrical Coefficients filter,  only half of it is needed(cmd[3]~cmd[18])
        cmd[3] = Eq->EqTap[0];
        cmd[4] = Eq->EqTap[1];
        cmd[5] = Eq->EqTap[2];
        cmd[6] = Eq->EqTap[3];
        cmd[7] = Eq->EqTap[4];
        cmd[8] = Eq->EqTap[5];
        cmd[9] = Eq->EqTap[6];
        cmd[10] = Eq->EqTap[7];
        cmd[11] = Eq->EqTap[8];
        cmd[12] = Eq->EqTap[9];
        cmd[13] = Eq->EqTap[10];
        cmd[14] = Eq->EqTap[11];
        cmd[15] = Eq->EqTap[12];
        cmd[16] = Eq->EqTap[13];
        cmd[17] = Eq->EqTap[14];

        cmd[18] = Eq->EqTap[15];

        /*cmd[19] = cmd[17];
        cmd[20] = cmd[16];
        cmd[21] = cmd[15];
        cmd[22] = cmd[14];
        cmd[23] = cmd[13];
        cmd[24] = cmd[12];
        cmd[25] = cmd[11];
        cmd[26] = cmd[10];
        cmd[27] = cmd[9];
        cmd[28] = cmd[8];
        cmd[29] = cmd[7];
        cmd[30] = cmd[6];
        cmd[31] = cmd[5];
        cmd[32] = cmd[4];
        cmd[33] = cmd[3];*/	//modified by Hongxing
    }

    //Send cmd to MSA...
    if((Eq->Eq_Length==15)||(Eq->Eq_Length==31))
        UICal_send_MSA_setting(cmd);
}


void UICal_MSA_EQ_RX_Set(UICal_EQ *Eq)
{
    unsigned short cmd[MSA_DATA_LENGTH];
    memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

    cmd[0]=EQ_CONTROL;   //opcode

    if(Eq->Eq_Length==15)
    {
        //cmd[1]=(EQ_FILTER_15_TAPS_ORDER+1)*sizeof(unsigned short);                  //length:count in char
        cmd[1]=((EQ_FILTER_15_TAPS_ORDER+1)/2+ 1)*sizeof(unsigned short) ;         //length:count in char, modified by Hongxing
        cmd[2]=RX_PATH_15_TAPS;

        //This filter is Symmetrical Coefficients filter,  only half of it is needed(cmd[3]~cmd[10])
        cmd[3] = Eq->EqTap[0];
        cmd[4] = Eq->EqTap[1];
        cmd[5] = Eq->EqTap[2];
        cmd[6] = Eq->EqTap[3];
        cmd[7] = Eq->EqTap[4];
        cmd[8] = Eq->EqTap[5];
        cmd[9] = Eq->EqTap[6];

        cmd[10] = Eq->EqTap[7];

        /*cmd[11] = cmd[9];
        cmd[12] = cmd[8];
        cmd[13] = cmd[7];
        cmd[14] = cmd[6];
        cmd[15] = cmd[5];
        cmd[16] = cmd[4];
        cmd[17] = cmd[3];*/	//modified by Hongxing
    }
    else if(Eq->Eq_Length==31)
    {
        //cmd[1]=(EQ_FILTER_31_TAPS_ORDER+1)*sizeof(unsigned short);                  //length:count in char
        cmd[1]=((EQ_FILTER_31_TAPS_ORDER+1)/2 + 1)*sizeof(unsigned short);         //length:count in char, modified by Hongxing
        cmd[2]=RX_PATH_31_TAPS;

        //This filter is Symmetrical Coefficients filter,  only half of it is needed(cmd[3]~cmd[18])
        cmd[3] = Eq->EqTap[0];
        cmd[4] = Eq->EqTap[1];
        cmd[5] = Eq->EqTap[2];
        cmd[6] = Eq->EqTap[3];
        cmd[7] = Eq->EqTap[4];
        cmd[8] = Eq->EqTap[5];
        cmd[9] = Eq->EqTap[6];
        cmd[10] = Eq->EqTap[7];
        cmd[11] = Eq->EqTap[8];
        cmd[12] = Eq->EqTap[9];
        cmd[13] = Eq->EqTap[10];
        cmd[14] = Eq->EqTap[11];
        cmd[15] = Eq->EqTap[12];
        cmd[16] = Eq->EqTap[13];
        cmd[17] = Eq->EqTap[14];

        cmd[18] = Eq->EqTap[15];

        /*cmd[19] = cmd[17];
        cmd[20] = cmd[16];
        cmd[21] = cmd[15];
        cmd[22] = cmd[14];
        cmd[23] = cmd[13];
        cmd[24] = cmd[12];
        cmd[25] = cmd[11];
        cmd[26] = cmd[10];
        cmd[27] = cmd[9];
        cmd[28] = cmd[8];
        cmd[29] = cmd[7];
        cmd[30] = cmd[6];
        cmd[31] = cmd[5];
        cmd[32] = cmd[4];
        cmd[33] = cmd[3];*/
    }

    //Send cmd to MSA...
    if((Eq->Eq_Length==15)||(Eq->Eq_Length==31))
        UICal_send_MSA_setting(cmd);
}


void UICal_MSA_EQ_TX_TurnOnOff(unsigned short On)
{
    unsigned short cmd[MSA_DATA_LENGTH];
    memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

    if(On == MSA_MODULE_ON)
    {
        voiceControl |= (TX_EQU_ENABLE);
    }
    else
    {
        voiceControl &= ~(TX_EQU_ENABLE);
    }

    cmd[0]=VOICE_CONTROL;   //opcode
    cmd[1]=0x4;                  //length:count in char

    cmd[2] = (voiceControl & 0xffff0000) >> 16;
    cmd[3] = (voiceControl & 0x0000ffff);


    //Send cmd to MSA...
    UICal_send_MSA_setting(cmd);
}


void UICal_MSA_EQ_RX_TurnOnOff(unsigned short On)
{
    unsigned short cmd[MSA_DATA_LENGTH];
    memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

    if(On == MSA_MODULE_ON)
    {
        voiceControl |= (RX_EQU_ENABLE);
    }
    else
    {
        voiceControl &= ~(RX_EQU_ENABLE);
    }

    cmd[0]=VOICE_CONTROL;   //opcode
    cmd[1]=0x4;                  //length:count in char

    cmd[2] = (voiceControl & 0xffff0000) >> 16;
    cmd[3] = (voiceControl & 0x0000ffff);


    //Send cmd to MSA...
    UICal_send_MSA_setting(cmd);
}



/******************************************************************************
 TX:Volume/Mute

 This is only for Tx path.

 The parameters:
 1) unsigned short mute
 2) unsigned short VolumeIndex
 3) unsigned short ECRefThresExp
 4) unsigned short RESRefThresExp
 5) unsigned short PeakInThreshExp
*******************************************************************************/

/*
    muteType:
    0:OFF
    1:ON_DC
    2:ON_RN
*/
void UICal_MSA_VolumeMute_TX_Set(
    unsigned short muteType,
    unsigned short VolumeIndex,
    unsigned short  ECRefThresExp,
    unsigned short RESRefThresExp,
    unsigned short PeakInThreshExp)
{
    unsigned short cmd[MSA_DATA_LENGTH];
    memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

    /*
        Send MUTE_CONTROL
    */
    cmd[0]=MUTE_CONTROL;   //opcode
    //cmd[1]=0x6;                  //length:count in char
    cmd[1] = 0x4;			 //length:count in char, modified by Hongxing

    cmd[2]=TX_MUTE;  //muteSide: TX_MUTE

    if(muteType!=MUTE_OFF1)
    {
        cmd[3]=0x1;     //mute On
        //cmd[4]=muteType-1; //muteType: 0:ON_DC;1:ON_RN, modifeid by Hongxing
    }
    else
    {
        cmd[3]=0x0;     //mute Off
        //cmd[4]=0; //muteType, modifeid by Hongxing
    }

    //Send cmd to MSA...
    UICal_send_MSA_setting(cmd);

    /*
        Send TX_VOLUME
    */
    memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));
    cmd[0]=VOLUME_CONTROL;   //opcode
    cmd[1]=0x4;                  //length:count in char

    cmd[2]=TX_VOLUME;  //TX_VOLUME
    cmd[3]=VolumeIndex;

    //Send cmd to MSA...
    UICal_send_MSA_setting(cmd);

     /*
        Send EC_THRESHOLD
    */
    memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));
    cmd[0]=VOLUME_CONTROL;   //opcode
    cmd[1]=0x8;                  //length:count in char

    cmd[2]=EC_THRESHOLD;  //EC_THRESHOLD
    cmd[3]=ECRefThresExp;
    cmd[4]=PeakInThreshExp;
    cmd[5]=RESRefThresExp;

    //Send cmd to MSA...
    UICal_send_MSA_setting(cmd);
}


void UICal_MSA_VolumeMute_TX_TurnOnOff(unsigned short On)
{
    unsigned short cmd[MSA_DATA_LENGTH];
    memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

    if(On == MSA_MODULE_ON)
    {
        voiceControl |= (TX_VOLUME_ENABLE);
    }
    else
    {
        voiceControl &= ~(TX_VOLUME_ENABLE);
    }

    cmd[0]=VOICE_CONTROL;   //opcode
    cmd[1]=0x4;                  //length:count in char

    cmd[2] = (voiceControl & 0xffff0000) >> 16;
    cmd[3] = (voiceControl & 0x0000ffff);

    //Send cmd to MSA...
    UICal_send_MSA_setting(cmd);
}



/******************************************************************************
 RX:Volume/Mute

 This is only for Rx path.

 The parameters:
 1) unsigned short mute
 2) unsigned short VolumeIndex
*******************************************************************************/

/*
    muteType:
    0:OFF
    1:ON_DC
    2:ON_RN
*/
void UICal_MSA_VolumeMute_RX_Set(unsigned short muteType, unsigned short VolumeIndex)
{
    unsigned short cmd[MSA_DATA_LENGTH];
    memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

    /*
        Send MUTE_CONTROL
    */
    cmd[0]=MUTE_CONTROL;   //opcode
    cmd[1]=0x6;                  //length:count in char

    cmd[2]=RX_MUTE;  //muteSide: RX_MUTE

    if(muteType!=MUTE_OFF1)
    {
        cmd[3]=0x1;     //mute On
        cmd[4]=muteType-1; //muteType: 0:ON_DC;1:ON_RN
    }
    else
    {
        cmd[3]=0x0;     //mute Off
        cmd[4]=0; //muteType
    }

    //Send cmd to MSA...
    UICal_send_MSA_setting(cmd);


    /*
        Send RX_VOLUME
    */
    memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));
    cmd[0]=VOLUME_CONTROL;   //opcode
    cmd[1]=0x4;                  //length:count in char

    cmd[2]=RX_VOLUME;  //RX_VOLUME
    cmd[3]=VolumeIndex;

    //Send cmd to MSA...
    UICal_send_MSA_setting(cmd);
}


void UICal_MSA_VolumeMute_RX_TurnOnOff(unsigned short On)
{
    unsigned short cmd[MSA_DATA_LENGTH];
    memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

    if(On == MSA_MODULE_ON)
    {
        voiceControl |= (RX_VOLUME_ENABLE);
    }
    else
    {
        voiceControl &= ~(RX_VOLUME_ENABLE);
    }

    cmd[0]=VOICE_CONTROL;   //opcode
    cmd[1]=0x4;                  //length:count in char

    cmd[2] = (voiceControl & 0xffff0000) >> 16;
    cmd[3] = (voiceControl & 0x0000ffff);

    //Send cmd to MSA...
    UICal_send_MSA_setting(cmd);
}



/******************************************************************************
 AGC/AVC

 TX:AGC
 There are parameters:
    unsigned short MaxAgcGain
    unsigned short Level1
    unsigned short  MinEnvelope
    unsigned short initPeriod

RX:AVC
There are parameters:
    unsigned short MaxAgcGain
    unsigned short Level1
    unsigned short  MinEnvelope
    unsigned short DrcGainFactor
    unsigned short LowAmbientNoise
    unsigned short MaxDrcGain
    unsigned short AdditionalGain
    unsigned short initPeriod

*******************************************************************************/

void UICal_MSA_AGC_TX_Set(
    unsigned short MaxAgcGain,
    unsigned short Level1,
    unsigned short  MinEnvelope,
    unsigned short initPeriod)
{
    unsigned short cmd[MSA_DATA_LENGTH];
    memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

    cmd[0]=AVC_CONTROL;   //opcode
    cmd[1]=0xe;                  //length:count in char

    cmd[2]=TX_PATH_UPDATE_INIT_PERIOD;                 //pathMode
    cmd[3] = MaxAgcGain;
    cmd[4] = Level1;
    cmd[5] = AVC_Es.MinEnvelopeTx;
    cmd[6] = AVC_Hf.MinEnvelopeTx;
    cmd[7] = AVC_Sp.MinEnvelopeTx;
    cmd[8] = initPeriod;

    //Update with new value
    if(gEcMode == ES)
    {
        cmd[5] = MinEnvelope;
    }
    else if(gEcMode == HF)
    {
        cmd[6] = MinEnvelope;
    }
    else if(gEcMode == SP)
    {
        cmd[7] = MinEnvelope;
    }

    //Send cmd to MSA...
    UICal_send_MSA_setting(cmd);
}


void UICal_MSA_AVC_RX_Set(
    unsigned short MaxAgcGain,
    unsigned short Level1,
    unsigned short  MinEnvelope,
    unsigned short DrcGainFactor,
    unsigned short LowAmbientNoise,
    unsigned short MaxDrcGain,
    unsigned short AdditionalGain,
    unsigned short initPeriod)
{
    unsigned short cmd[MSA_DATA_LENGTH];
    memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

    cmd[0]=AVC_CONTROL;   //opcode
    cmd[1]=0x1a;                  //length:count in char

    cmd[2]=RX_PATH_UPDATE_INIT_PERIOD;                 //pathMode
    cmd[3] = MaxAgcGain;
    cmd[4] = Level1;

    //Use current value
    cmd[5] = AVC_Es.MinEnvelopeRx;
    cmd[6] = AVC_Hf.MinEnvelopeRx;
    cmd[7] = AVC_Sp.MinEnvelopeRx;

    cmd[8] = DrcGainFactor;

    //Use current value
    cmd[9] = AVC_Es.LowAmbientNoise;
    cmd[10] = AVC_Hf.LowAmbientNoise;
    cmd[11] = AVC_Sp.LowAmbientNoise;

    cmd[12] = MaxDrcGain;
    cmd[13] = AdditionalGain;
    cmd[14] = initPeriod;

    //Update with new value
    if(gEcMode == ES)
    {
        cmd[5] = MinEnvelope;
        cmd[9] = LowAmbientNoise;
    }
    else if(gEcMode == HF)
    {
        cmd[6] = MinEnvelope;
        cmd[10] = LowAmbientNoise;
    }
    else if(gEcMode == SP)
    {
        cmd[7] = MinEnvelope;
        cmd[11] = LowAmbientNoise;
    }

    //Send cmd to MSA...
    UICal_send_MSA_setting(cmd);
}


void UICal_MSA_AGC_TX_TurnOnOff(unsigned short On)
{
    unsigned short cmd[MSA_DATA_LENGTH];
    memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

    if(On == MSA_MODULE_ON)
    {
        voiceControl |= (TX_AGC_ENABLE);
    }
    else
    {
        voiceControl &= ~(TX_AGC_ENABLE);
    }

    cmd[0]=VOICE_CONTROL;   //opcode
    cmd[1]=0x4;                  //length:count in char

    cmd[2] = (voiceControl & 0xffff0000) >> 16;
    cmd[3] = (voiceControl & 0x0000ffff);

    //Send cmd to MSA...
    UICal_send_MSA_setting(cmd);
}


void UICal_MSA_AVC_RX_TurnOnOff(unsigned short On)
{
    unsigned short cmd[MSA_DATA_LENGTH];
    memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

    if(On == MSA_MODULE_ON)
    {
        voiceControl |= (RX_AVC_ENABLE);
    }
    else
    {
        voiceControl &= ~(RX_AVC_ENABLE);
    }

    cmd[0]=VOICE_CONTROL;   //opcode
    cmd[1]=0x4;                  //length:count in char

    cmd[2] = (voiceControl & 0xffff0000) >> 16;
    cmd[3] = (voiceControl & 0x0000ffff);

    //Send cmd to MSA...
    UICal_send_MSA_setting(cmd);
}



/******************************************************************************
 Residual EC
	ResidualEC works for both TX and RX channel
*******************************************************************************/
void UICal_MSA_ResidualEC_TurnOnOff(unsigned short On)
{
    unsigned short cmd[MSA_DATA_LENGTH];
    memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

    //if we turn on RES, EC also should be turned on
    if(On == MSA_MODULE_ON)
    {
        voiceControl |= (EC_ENABLE);
        voiceControl |= (RES_ENABLE);
    }
    else
    {
        voiceControl &= ~(RES_ENABLE);
    }

    cmd[0]=VOICE_CONTROL;   //opcode
    cmd[1]=0x4;                  //length:count in char

    cmd[2] = (voiceControl & 0xffff0000) >> 16;
    cmd[3] = (voiceControl & 0x0000ffff);

    //Send cmd to MSA...
    UICal_send_MSA_setting(cmd);
}



/******************************************************************************
 EC(Echo Cancellation)

 This if only for TX path

 There are parameters:
 1)  unsigned short ResMode
 2)  unsigned short sampleDelay
 3)  unsigned short ecLmsBurst
 4)  unsigned short attenuationFactor1
 5)  unsigned short EchoToNoiseThresFactor
*******************************************************************************/

void UICal_MSA_EC_TX_Set(
    unsigned short ResMode,
    unsigned short sampleDelay,
    unsigned short ecLmsBurst,
    unsigned short attenuationFactor1,
    unsigned short EchoToNoiseThresFactor )
{
    unsigned short cmd[MSA_DATA_LENGTH];
    memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

    cmd[0]=EC_CONTROL;   //opcode
    cmd[1]=0x14;                  //length:count in char

    cmd[2]=0x9;                 /* the mode is the length, not including the control word (data[0]) */
    cmd[3]=ResMode;                 //ResMode: ATTN(0) / DTX(1)

    //Set boundary to 800
    if(sampleDelay > 800)
        cmd[4] = 800;
    else
        cmd[4] = sampleDelay;

    cmd[5] = ecLmsBurst;

    //Use current value
    cmd[6] = EC_Es.AttenuationFactor1;
    cmd[7] = EC_Hf.AttenuationFactor1;
    cmd[8] = EC_Sp.AttenuationFactor1;
    cmd[9] = EC_Es.EchoToNoiseThresFactor;
    cmd[10] = EC_Hf.EchoToNoiseThresFactor;
    cmd[11] = EC_Sp.EchoToNoiseThresFactor;

    //Update with new value
    if(gEcMode == ES)
    {
        cmd[6] = attenuationFactor1;
        cmd[9] = EchoToNoiseThresFactor;
    }
    else if(gEcMode == HF)
    {
        cmd[7] = attenuationFactor1;
        cmd[10] = EchoToNoiseThresFactor;
    }
    else if(gEcMode == SP)
    {
        cmd[8]  = attenuationFactor1;
        cmd[11] = EchoToNoiseThresFactor;
    }

    //Send cmd to MSA...
    UICal_send_MSA_setting(cmd);
}



void UICal_MSA_EC_TX_TurnOnOff(unsigned short On)
{
    unsigned short cmd[MSA_DATA_LENGTH];
    memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

    //if we turn off EC, RES also should be turned off
    if(On == MSA_MODULE_ON)
    {
        voiceControl |= (EC_ENABLE);
    }
    else
    {
        voiceControl &= ~(EC_ENABLE);
        voiceControl &= ~(RES_ENABLE);
    }

    cmd[0]=VOICE_CONTROL;   //opcode
    cmd[1]=0x4;                  //length:count in char

    cmd[2] = (voiceControl & 0xffff0000) >> 16;
    cmd[3] = (voiceControl & 0x0000ffff);

    //Send cmd to MSA...
    UICal_send_MSA_setting(cmd);
}

/*for audio loopback test*/

static void sendDspCmdAndOrData1()
{
	unsigned short cmd[MSA_DATA_LENGTH];
	memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

	cmd[0]= VOICE_TEST_CONTROL;   //opcode:0x46(VOICE_TEST_CONTROL)
	cmd[1]=0x4; 		 //length: count in char

	cmd[2]=0x0002;
	cmd[3]=0x0300;

	//Send cmd to MSA...
	UICal_send_MSA_setting(cmd);
}


static void sendDspCmdAndOrData2()
{
	unsigned short cmd[MSA_DATA_LENGTH];
	memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

	cmd[0]= VOICE_TEST_CONTROL;   //opcode:0x46(VOICE_TEST_CONTROL)
	cmd[1]=0x4; 		 //length: count in char

	cmd[2]=0x0002;
	cmd[3]=0x0100;

	//Send cmd to MSA...
	UICal_send_MSA_setting(cmd);
}


static void sendDspCmdAndOrData3()
{
	unsigned short cmd[MSA_DATA_LENGTH];
	memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

	cmd[0]= SELF_INVOCATION;   //opcode:0x49(SELF_INVOCATION)
	cmd[1]=0x6; 		 //length: count in char

	cmd[2]=0x0001;
	cmd[3]=0x0000;
	cmd[4]=0x0000;

	//Send cmd to MSA...
	UICal_send_MSA_setting(cmd);
}

static void sendDspCmdAndOrData4()
{
	unsigned short cmd[MSA_DATA_LENGTH];
	memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

	cmd[0]= START_VOICE_PATH;   //opcode:0x40(START_VOICE_PATH)
	cmd[1]=0x0e; 		 //length: count in char

	cmd[2]=0x000c;
	cmd[3]=0xccd0;
	cmd[4]=0x0000;
	cmd[5]=0x0000;
	cmd[6]=0x0007;
	cmd[7]=0x0000;
	cmd[8]=0x0000;

	//Send cmd to MSA...
	UICal_send_MSA_setting(cmd);
}

static void sendDspCmdAndOrData9()
{
	unsigned short cmd[MSA_DATA_LENGTH];
	memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

	cmd[0]= 0x61;   //opcode:0x61
	cmd[1]=0x4; 		 //length: count in char

	cmd[2]=0x000c;
	cmd[3]=0x0000;

	//Send cmd to MSA...
	UICal_send_MSA_setting(cmd);
}

static void sendDspCmdAndOrData5()
{
	unsigned short cmd[MSA_DATA_LENGTH];
	memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

	cmd[0]= VOICE_TEST_CONTROL;   //opcode:0x46(VOICE_TEST_CONTROL)
	cmd[1]=0x02; 		 //length: count in char

	cmd[2]=0x0712;

	//Send cmd to MSA...
	UICal_send_MSA_setting(cmd);
}

static void sendDspCmdAndOrData6()
{
	unsigned short cmd[MSA_DATA_LENGTH];
	memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

	cmd[0]= VOICE_TEST_CONTROL;   //opcode:0x46(VOICE_TEST_CONTROL)
	cmd[1]=0x02; 		 //length: count in char

	cmd[2]=0x0000;

	//Send cmd to MSA...
	UICal_send_MSA_setting(cmd);
}

static void sendDspCmdAndOrData7()
{
	unsigned short cmd[MSA_DATA_LENGTH];
	memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

	cmd[0]= END_VOICE_PATH;   //opcode:0x41(END_VOICE_PATH)
	cmd[1]=0x00; 		 //length: count in char

	//Send cmd to MSA...
	UICal_send_MSA_setting(cmd);
}

static void sendDspCmdAndOrData8()
{
	unsigned short cmd[MSA_DATA_LENGTH];
	memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

	cmd[0]= SELF_INVOCATION;   //opcode:0x49(SELF_INVOCATION)
	cmd[1]=0x02; 		 //length: count in char

	cmd[2]=0x0000;

	//Send cmd to MSA...
	UICal_send_MSA_setting(cmd);
}

void start_loopback()
{
	sendDspCmdAndOrData1();
	sendDspCmdAndOrData2();
	sendDspCmdAndOrData3();
	sendDspCmdAndOrData4();
	sendDspCmdAndOrData9();
	sendDspCmdAndOrData5();
}

void stop_loopback()
{
	sendDspCmdAndOrData6();
	sendDspCmdAndOrData7();
	sendDspCmdAndOrData8();
}

