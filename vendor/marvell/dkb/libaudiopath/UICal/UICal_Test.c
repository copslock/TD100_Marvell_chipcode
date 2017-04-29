#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "../UICal_APIs.h"


static char buf[512];


void AudioUICal_Test_ShowParams()
{
	UICal_MSA_Report_Parse();
}


/************************************************************
    moduleIDs is 16bits 'bitmap' of each module:
	EC_ID,
	TX_NS_ID,
	TX_EQ_ID,
	TX_AGC_ID,
	TX_VOLUME_AND_MUTE_ID,
	RX_NS_ID,
	RX_EQ_ID,
	RX_AVC_ID,
	RX_VOLUME_AND_MUTE_ID,
	RX_TX_ENERGY,                    ////Not parse, not displayed
	VOICE_CONTROL_AND_VOCODER_CONTROL_ID,
	MODEM_INFO_ID,                  ////Not parse, not displayed
	TX_CTM_ID,                      ////Not parse, not displayed
	RX_CTM_ID,                      ////Not parse, not displayed
	EC_FIR_ID,                      ////Not parse, not displayed
	VOLUME_EC_THRESHOLD_ID          ////Not parse, not displayed
************************************************************/

void AudioUICal_Test_Get()
{
    unsigned short modulesBitmap ;
    memset(buf, 0, 512);

    UICAL_PRINT("AudioUICal_Test_Get: enter modules Bitmap (hexadecimal, unsigned short type):\n");
    read(0, buf, 512);
    modulesBitmap = strtol(buf, NULL, 16);
    UICAL_PRINT("AudioUICal_Test_Get: modules Bitmap is 0x%x\n", modulesBitmap );

    if((modulesBitmap & 0x0001)==0x0001)
        UICAL_PRINT("AudioUICal_Test_Get: Request EC(bit 0)\n");

    if((modulesBitmap & 0x0002)==0x0002)
        UICAL_PRINT("AudioUICal_Test_Get: Request TX_NS(bit 1)\n");

    if((modulesBitmap & 0x0004)==0x0004)
        UICAL_PRINT("AudioUICal_Test_Get: Request TX_EQ(bit 2)\n");

    if((modulesBitmap & 0x0008)==0x0008)
        UICAL_PRINT("AudioUICal_Test_Get: Request TX_AGC(bit 3)\n");

    if((modulesBitmap & 0x0010)==0x0010)
        UICAL_PRINT("AudioUICal_Test_Get: Request TX_VOLUME_AND_MUTE(bit 4)\n");

    if((modulesBitmap & 0x0020)==0x0020)
        UICAL_PRINT("AudioUICal_Test_Get: Request RX_NS(bit 5)\n");

    if((modulesBitmap & 0x0040)==0x0040)
        UICAL_PRINT("AudioUICal_Test_Get: Request RX_EQ(bit 6)\n");

    if((modulesBitmap & 0x0080)==0x0080)
        UICAL_PRINT("AudioUICal_Test_Get: Request RX_AVC(bit 7)\n");

    if((modulesBitmap & 0x0100)==0x0100)
        UICAL_PRINT("AudioUICal_Test_Get: Request RX_VOLUME_AND_MUTE(bit 8)\n");

    if((modulesBitmap & 0x0200)==0x0200)
        UICAL_PRINT("AudioUICal_Test_Get: Request RX_TX_ENERGY(bit 9)\n");

    if((modulesBitmap & 0x0400)==0x0400)
        UICAL_PRINT("AudioUICal_Test_Get: Request VOICE_CONTROL_AND_VOCODER_CONTROL(bit 10)\n");

    if((modulesBitmap & 0x0800)==0x0800)
        UICAL_PRINT("AudioUICal_Test_Get: Request MODEM_INFO(bit 11)\n");

    if((modulesBitmap & 0x1000)==0x1000)
        UICAL_PRINT("AudioUICal_Test_Get: Request TX_CTM(bit 12)\n");

    if((modulesBitmap & 0x2000)==0x2000)
        UICAL_PRINT("AudioUICal_Test_Get: Request RX_CTM(bit 13)\n");

    if((modulesBitmap & 0x4000)==0x4000)
        UICAL_PRINT("AudioUICal_Test_Get: Request EC_FIR(bit 14)\n");

    if((modulesBitmap & 0x8000)==0x8000)
        UICAL_PRINT("AudioUICal_Test_Get: Request VOLUME_EC_THRESHOLD(bit 15)\n");

    UICAL_PRINT("\n");
    UICal_MSA_Reports_Request(modulesBitmap);

    sleep(1);
    AudioUICal_Test_ShowParams();
}


//ICAT EXPORTED FUNCTION - Audio, UICal, Test_Set
void AudioUICal_Test_Set()
{
    unsigned short moduleID, data1, data2, data3, data4, data5, data6, data7, data8;
    unsigned short data9, data10, data11, data12, data13, data14, data15, data16, data17;

    memset(buf, 0, 512);

    UICAL_PRINT("AudioUICal_Test_Set: enter moduleID (hexadecimal, unsigned short type):\n");
    read(0, buf, 512);
    moduleID = strtol(buf, NULL, 16);

    switch(moduleID)
    {
        case TX_AGC_ID:
            UICAL_PRINT("AudioUICal_Test_Set: --------------------TX_AGC--------------------\n");

            UICAL_PRINT("AudioUICal_Test_Set: enter MaxAgcGain (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data1 = strtol(buf, NULL, 16);

            UICAL_PRINT("AudioUICal_Test_Set: enter Level1 (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data2 = strtol(buf, NULL, 16);

            UICAL_PRINT("AudioUICal_Test_Set: enter MinEnvelope (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data3 = strtol(buf, NULL, 16);

            UICAL_PRINT("AudioUICal_Test_Set: enter initPeriod (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data4 = strtol(buf, NULL, 16);

            UICal_MSA_AGC_TX_Set(data1,data2,data3,data4);

            UICAL_PRINT("AGC_TX_Set, MaxAgcGain: 0x%x, Level1: 0x%x, MinEnvelope: 0x%x, initPeriod: 0x%x\n\n", data1,data2,data3,data4);

            break;

        case RX_AVC_ID:
            UICAL_PRINT("AudioUICal_Test_Set: --------------------RX_AVC--------------------\n");

            UICAL_PRINT("AudioUICal_Test_Set: enter MaxAgcGain (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data1 = strtol(buf, NULL, 16);

            UICAL_PRINT("AudioUICal_Test_Set: enter Level1 (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data2 = strtol(buf, NULL, 16);

            UICAL_PRINT("AudioUICal_Test_Set: enter MinEnvelope (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data3 = strtol(buf, NULL, 16);

            UICAL_PRINT("AudioUICal_Test_Set: enter DrcGainFactor (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data4 = strtol(buf, NULL, 16);

            UICAL_PRINT("AudioUICal_Test_Set: enter LowAmbientNoise (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data5 = strtol(buf, NULL, 16);

            UICAL_PRINT("AudioUICal_Test_Set: enter MaxDrcGain (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data6 = strtol(buf, NULL, 16);

            UICAL_PRINT("AudioUICal_Test_Set: enter AdditionalGain (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data7 = strtol(buf, NULL, 16);

            UICAL_PRINT("AudioUICal_Test_Set: enter initPeriod (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data8 = strtol(buf, NULL, 16);

            UICal_MSA_AVC_RX_Set(data1,data2,data3,data4,data5,data6,data7,data8);

            UICAL_PRINT("AVC_RX_Set, MaxAgcGain: 0x%x, Level1: 0x%x, MinEnvelope: 0x%x, DrcGainFactor: 0x%x, LowAmbientNoise: 0x%x, MaxDrcGain: 0x%x, AdditionalGain: 0x%x, initPeriod: 0x%x\n\n", data1,data2,data3,data4,data5,data6,data7,data8);

            break;

        case EC_ID:
            UICAL_PRINT("AudioUICal_Test_Set: --------------------EC--------------------\n");

            UICAL_PRINT("AudioUICal_Test_Set: enter ResMode (hexadecimal, unsigned short type, ATTN(0) / DTX(1)):\n");
            read(0, buf, 512);
            data1 = strtol(buf, NULL, 16);

            UICAL_PRINT("AudioUICal_Test_Set: enter sampleDelay (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data2 = strtol(buf, NULL, 16);

            UICAL_PRINT("AudioUICal_Test_Set: enter ecLmsBurst (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data3 = strtol(buf, NULL, 16);

            UICAL_PRINT("AudioUICal_Test_Set: enter attenuationFactor1 (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data4 = strtol(buf, NULL, 16);

            UICAL_PRINT("AudioUICal_Test_Set: enter EchoToNoiseThresFactor (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data5 = strtol(buf, NULL, 16);

            UICal_MSA_EC_TX_Set(data1,data2,data3,data4,data5);

            UICAL_PRINT("EC_TX_Set, ResMode: 0x%x, sampleDelay: 0x%x, ecLmsBurst: 0x%x, attenuationFactor1: 0x%x, EchoToNoiseThresFactor: 0x%x\n\n", data1,data2,data3,data4,data5); 

            break;

        case RX_EQ_ID:
            UICAL_PRINT("AudioUICal_Test_Set: --------------------RX_EQ--------------------\n");
            {
                UICal_EQ Eq;

                UICAL_PRINT("AudioUICal_Test_Set: enter Eq_Length (hexadecimal, unsigned short type, (f :15 Taps)/(1f :31 Taps):\n");
                read(0, buf, 512);
                data1 = strtol(buf, NULL, 16);

                Eq.Eq_Length = data1;

                if(Eq.Eq_Length==15)
                {
                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap1 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data2 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap2 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data3 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap3 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data4 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap4 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data5 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap5 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data6 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap6 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data7 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap7 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data8 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap8 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data9 = strtol(buf, NULL, 16);

                    Eq.EqTap[0] = data2;
                    Eq.EqTap[1] = data3;
                    Eq.EqTap[2] = data4;
                    Eq.EqTap[3] = data5;
                    Eq.EqTap[4] = data6;
                    Eq.EqTap[5] = data7;
                    Eq.EqTap[6] = data8;
                    Eq.EqTap[7] = data9;

                    UICAL_PRINT("EQ_RX_Set:15Taps, Tap1: 0x%x, Tap2: 0x%x, Tap3: 0x%x, Tap4: 0x%x, Tap5: 0x%x, Tap6: 0x%x, Tap7: 0x%x, Tap8: 0x%x\n\n", data2,data3,data4,data5,data6,data7,data8,data9); 
                }
                else if(Eq.Eq_Length==31)
                {
                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap1 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data2 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap2 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data3 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap3 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data4 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap4 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data5 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap5 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data6 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap6 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data7 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap7 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data8 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap8 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data9 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap9 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data10 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap10 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data11 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap11 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data12 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap12 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data13 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap13 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data14 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap14 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data15 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap15 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data16 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap16 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data17 = strtol(buf, NULL, 16);

                    Eq.EqTap[0] = data2;
                    Eq.EqTap[1] = data3;
                    Eq.EqTap[2] = data4;
                    Eq.EqTap[3] = data5;
                    Eq.EqTap[4] = data6;
                    Eq.EqTap[5] = data7;
                    Eq.EqTap[6] = data8;
                    Eq.EqTap[7] = data9;
                    Eq.EqTap[8] = data10;
                    Eq.EqTap[9] = data11;
                    Eq.EqTap[10] = data12;
                    Eq.EqTap[11] = data13;
                    Eq.EqTap[12] = data14;
                    Eq.EqTap[13] = data15;
                    Eq.EqTap[14] = data16;
                    Eq.EqTap[15] = data17;

                    UICAL_PRINT("EQ_RX_Set:31Taps, Tap1: 0x%x, Tap2: 0x%x, Tap3: 0x%x, Tap4: 0x%x, Tap5: 0x%x, Tap6: 0x%x, Tap7: 0x%x, Tap8: 0x%x, Tap9: 0x%x, Tap10: 0x%x, Tap11: 0x%x, Tap12: 0x%x, Tap13: 0x%x, Tap14: 0x%x, Tap15: 0x%x, Tap16: 0x%x\n\n", data2,data3,data4,data5,data6,data7,data8,data9, data10,data11,data12,data13,data14,data15,data16,data17); 
                }

                if((Eq.Eq_Length==15)||(Eq.Eq_Length==31))
                {
                    UICal_MSA_EQ_RX_Set(&Eq);
                }
                else
                {
                    UICAL_PRINT("EQ length should be f or 1f !!!\n\n");
                }
            }
            break;

        case TX_EQ_ID:
            UICAL_PRINT("AudioUICal_Test_Set: --------------------TX_EQ--------------------\n");
            {
                UICal_EQ Eq;

                UICAL_PRINT("AudioUICal_Test_Set: enter Eq_Length (hexadecimal, unsigned short type, (f :15 Taps)/(1f :31 Taps):\n");
                read(0, buf, 512);
                data1 = strtol(buf, NULL, 16);

                Eq.Eq_Length = data1;

                if(Eq.Eq_Length==15)
                {
                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap1 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data2 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap2 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data3 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap3 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data4 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap4 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data5 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap5 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data6 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap6 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data7 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap7 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data8 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap8 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data9 = strtol(buf, NULL, 16);

                    Eq.EqTap[0] = data2;
                    Eq.EqTap[1] = data3;
                    Eq.EqTap[2] = data4;
                    Eq.EqTap[3] = data5;
                    Eq.EqTap[4] = data6;
                    Eq.EqTap[5] = data7;
                    Eq.EqTap[6] = data8;
                    Eq.EqTap[7] = data9;

                    UICAL_PRINT("EQ_TX_Set:15Taps, Tap1: 0x%x, Tap2: 0x%x, Tap3: 0x%x, Tap4: 0x%x, Tap5: 0x%x, Tap6: 0x%x, Tap7: 0x%x, Tap8: 0x%x\n\n", data2,data3,data4,data5,data6,data7,data8,data9); 
                }
                else if(Eq.Eq_Length==31)
                {
                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap1 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data2 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap2 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data3 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap3 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data4 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap4 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data5 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap5 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data6 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap6 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data7 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap7 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data8 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap8 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data9 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap9 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data10 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap10 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data11 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap11 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data12 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap12 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data13 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap13 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data14 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap14 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data15 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap15 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data16 = strtol(buf, NULL, 16);

                    UICAL_PRINT("AudioUICal_Test_Set: enter EqTap16 (hexadecimal, unsigned short type):\n");
                    read(0, buf, 512);
                    data17 = strtol(buf, NULL, 16);

                    Eq.EqTap[0] = data2;
                    Eq.EqTap[1] = data3;
                    Eq.EqTap[2] = data4;
                    Eq.EqTap[3] = data5;
                    Eq.EqTap[4] = data6;
                    Eq.EqTap[5] = data7;
                    Eq.EqTap[6] = data8;
                    Eq.EqTap[7] = data9;
                    Eq.EqTap[8] = data10;
                    Eq.EqTap[9] = data11;
                    Eq.EqTap[10] = data12;
                    Eq.EqTap[11] = data13;
                    Eq.EqTap[12] = data14;
                    Eq.EqTap[13] = data15;
                    Eq.EqTap[14] = data16;
                    Eq.EqTap[15] = data17;

                    UICAL_PRINT("EQ_TX_Set:31Taps, Tap1: 0x%x, Tap2: 0x%x, Tap3: 0x%x, Tap4: 0x%x, Tap5: 0x%x, Tap6: 0x%x, Tap7: 0x%x, Tap8: 0x%x, Tap9: 0x%x, Tap10: 0x%x, Tap11: 0x%x, Tap12: 0x%x, Tap13: 0x%x, Tap14: 0x%x, Tap15: 0x%x, Tap16: 0x%x\n\n", data2,data3,data4,data5,data6,data7,data8,data9, data10,data11,data12,data13,data14,data15,data16,data17);
                }

                if((Eq.Eq_Length==15)||(Eq.Eq_Length==31))
                {
                    UICal_MSA_EQ_TX_Set(&Eq);
                }
                else
                {
                    UICAL_PRINT("EQ length should be f or 1f !!!\n\n");
                }
            }
            break;

        case RX_NS_ID:
            UICAL_PRINT("AudioUICal_Test_Set: --------------------RX_NS--------------------\n");

            UICAL_PRINT("AudioUICal_Test_Set: enter nsMaxSupress (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data1 = strtol(buf, NULL, 16);

            UICAL_PRINT("AudioUICal_Test_Set: enter nsMaxNoise (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data2 = strtol(buf, NULL, 16);

            UICal_MSA_NS_RX_Set(data1,data2);

            UICAL_PRINT("NS_RX_Set, nsMaxSupress: 0x%x, nsMaxNoise: 0x%x\n\n", data1,data2);

            break;

        case TX_NS_ID:
            UICAL_PRINT("AudioUICal_Test_Set: --------------------TX_NS--------------------\n");

            UICAL_PRINT("AudioUICal_Test_Set: enter nsMaxSupress (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data1 = strtol(buf, NULL, 16);

            UICAL_PRINT("AudioUICal_Test_Set: enter nsMaxNoise (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data2 = strtol(buf, NULL, 16);

            UICal_MSA_NS_TX_Set(data1,data2);

            UICAL_PRINT("NS_TX_Set, nsMaxSupress: 0x%x, nsMaxNoise: 0x%x\n\n", data1,data2);

            break;

        case RX_VOLUME_AND_MUTE_ID:
            UICAL_PRINT("AudioUICal_Test_Set: --------------------RX_VOLUME_AND_MUTE--------------------\n");

            UICAL_PRINT("AudioUICal_Test_Set: enter muteType (hexadecimal, unsigned short type, 0:MUTE OFF, 1:ON_DC(Mute on with Quiet), 2:ON_RN(Mute on with Random Noise)  :\n");
            read(0, buf, 512);
            data1 = strtol(buf, NULL, 16);

            UICAL_PRINT("AudioUICal_Test_Set: enter VolumeIndex (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data2 = strtol(buf, NULL, 16);

            UICal_MSA_VolumeMute_RX_Set(data1,data2);

            UICAL_PRINT("RX_VOLUME_AND_MUTE_Set, muteType: 0x%x, VolumeIndex: 0x%x\n\n", data1,data2);
            break;

        case TX_VOLUME_AND_MUTE_ID:
            UICAL_PRINT("AudioUICal_Test_Set: --------------------TX_VOLUME_AND_MUTE--------------------\n");

            UICAL_PRINT("AudioUICal_Test_Set: enter muteType (hexadecimal, unsigned short type, 0:MUTE OFF, 1:ON_DC(Mute on with Quiet)  :\n");
            read(0, buf, 512);
            data1 = strtol(buf, NULL, 16);

            UICAL_PRINT("AudioUICal_Test_Set: enter VolumeIndex (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data2 = strtol(buf, NULL, 16);

            UICAL_PRINT("AudioUICal_Test_Set: enter ECRefThresExp (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data3 = strtol(buf, NULL, 16);

            UICAL_PRINT("AudioUICal_Test_Set: enter RESRefThresExp (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data4 = strtol(buf, NULL, 16);

            UICAL_PRINT("AudioUICal_Test_Set: enter PeakInThreshExp (hexadecimal, unsigned short type):\n");
            read(0, buf, 512);
            data5 = strtol(buf, NULL, 16);

            UICal_MSA_VolumeMute_TX_Set(data1,data2,data3,data4,data5);

            UICAL_PRINT("TX_VOLUME_AND_MUTE_Set, muteType: 0x%x, VolumeIndex: 0x%x, ECRefThresExp: 0x%x, RESRefThresExp: 0x%x, PeakInThreshExp: 0x%x\n\n ", data1,data2, data3,data4, data5);

            break;

        default :
            UICAL_PRINT("UNKNOWN_ID, 0x%x\n\n", moduleID);

            break;
    }

}


//ICAT EXPORTED FUNCTION - Audio, UICal, Test_TurnOnOff
void AudioUICal_Test_TurnOnOff()
{
    unsigned short moduleID, on;

    memset(buf, 0, 512);

    UICAL_PRINT("AudioUICal_Test_TurnOnOff: enter moduleID (hexadecimal, unsigned short type):\n");
    read(0, buf, 512);
    moduleID = strtol(buf, NULL, 16);

    switch(moduleID)
    {
        case TX_AGC_ID:
            UICAL_PRINT("AudioUICal_Test_TurnOnOff: -----------------TX_AGC-----------------\n");

		    UICAL_PRINT("AudioUICal_Test_TurnOnOff: 0: Turn off, 1: Turn on :\n");
		    read(0, buf, 512);
		    on = strtol(buf, NULL, 16);

            UICal_MSA_AGC_TX_TurnOnOff(on);

            UICAL_PRINT("AGC_TX_TurnOnOff : %s\n", (on==0)?"Off":"On");

            break;

        case RX_AVC_ID:
        	UICAL_PRINT("AudioUICal_Test_TurnOnOff: -----------------RX_AVC-----------------\n");

		    UICAL_PRINT("AudioUICal_Test_TurnOnOff: 0: Turn off, 1: Turn on :\n");
		    read(0, buf, 512);
		    on = strtol(buf, NULL, 16);

            UICal_MSA_AVC_RX_TurnOnOff(on);

            UICAL_PRINT("AVC_RX_TurnOnOff : %s\n", (on==0)?"Off":"On");

            break;

        case EC_ID:
        	UICAL_PRINT("AudioUICal_Test_TurnOnOff: -----------------EC-----------------\n");

		    UICAL_PRINT("AudioUICal_Test_TurnOnOff: 0: Turn off, 1: Turn on :\n");
		    read(0, buf, 512);
		    on = strtol(buf, NULL, 16);

            UICal_MSA_EC_TX_TurnOnOff(on);

            UICAL_PRINT("EC_TX_TurnOnOff : %s\n", (on==0)?"Off":"On");

            break;

        case RX_EQ_ID:
        	UICAL_PRINT("AudioUICal_Test_TurnOnOff: -----------------RX_EQ-----------------\n");

		    UICAL_PRINT("AudioUICal_Test_TurnOnOff: 0: Turn off, 1: Turn on :\n");
		    read(0, buf, 512);
		    on = strtol(buf, NULL, 16);

            UICal_MSA_EQ_RX_TurnOnOff(on);

            UICAL_PRINT("EQ_RX_TurnOnOff : %s\n", (on==0)?"Off":"On");

            break;

        case TX_EQ_ID:
        	UICAL_PRINT("AudioUICal_Test_TurnOnOff: -----------------TX_EQ-----------------\n");

		    UICAL_PRINT("AudioUICal_Test_TurnOnOff: 0: Turn off, 1: Turn on :\n");
		    read(0, buf, 512);
		    on = strtol(buf, NULL, 16);

            UICal_MSA_EQ_TX_TurnOnOff(on);

            UICAL_PRINT("EQ_TX_TurnOnOff : %s\n", (on==0)?"Off":"On");

            break;

        case RX_NS_ID:
        	UICAL_PRINT("AudioUICal_Test_TurnOnOff: -----------------RX_NS-----------------\n");

		    UICAL_PRINT("AudioUICal_Test_TurnOnOff: 0: Turn off, 1: Turn on :\n");
		    read(0, buf, 512);
		    on = strtol(buf, NULL, 16);

            UICal_MSA_NS_RX_TurnOnOff(on);

            UICAL_PRINT("NS_RX_TurnOnOff : %s\n", (on==0)?"Off":"On");

            break;

        case TX_NS_ID:
        	UICAL_PRINT("AudioUICal_Test_TurnOnOff: -----------------TX_NS-----------------\n");

		    UICAL_PRINT("AudioUICal_Test_TurnOnOff: 0: Turn off, 1: Turn on :\n");
		    read(0, buf, 512);
		    on = strtol(buf, NULL, 16);

            UICal_MSA_NS_TX_TurnOnOff(on);

            UICAL_PRINT("NS_TX_TurnOnOff : %s\n", (on==0)?"Off":"On");

            break;

        case RX_VOLUME_AND_MUTE_ID:
        	UICAL_PRINT("AudioUICal_Test_TurnOnOff: -----------------RX_VOLUME_AND_MUTE-----------------\n");

		    UICAL_PRINT("AudioUICal_Test_TurnOnOff: 0: Turn off, 1: Turn on :\n");
		    read(0, buf, 512);
		    on = strtol(buf, NULL, 16);

            UICal_MSA_VolumeMute_RX_TurnOnOff(on);

            UICAL_PRINT("RX_VOLUME_AND_MUTE_TurnOnOff : %s\n", (on==0)?"Off":"On");
            break;

        case TX_VOLUME_AND_MUTE_ID:
        	UICAL_PRINT("AudioUICal_Test_TurnOnOff: -----------------TX_VOLUME_AND_MUTE-----------------\n");

		    UICAL_PRINT("AudioUICal_Test_TurnOnOff: 0: Turn off, 1: Turn on :\n");
		    read(0, buf, 512);
		    on = strtol(buf, NULL, 16);

            UICal_MSA_VolumeMute_TX_TurnOnOff(on);

            UICAL_PRINT("TX_VOLUME_AND_MUTE_TurnOnOff : %s\n", (on==0)?"Off":"On");

            break;


        case ResidualEC_ID:
        	UICAL_PRINT("AudioUICal_Test_TurnOnOff: -----------------ResidualEC-----------------\n");

		    UICAL_PRINT("AudioUICal_Test_TurnOnOff: 0: Turn off, 1: Turn on :\n");
		    read(0, buf, 512);
		    on = strtol(buf, NULL, 16);

            UICal_MSA_ResidualEC_TurnOnOff(on);

            UICAL_PRINT("ResidualEC_TurnOnOff : %s\n", (on==0)?"Off":"On");

            break;

        default :
            UICAL_PRINT("UNKNOWN_ID, 0x%x\n", moduleID);

            break;
    }

}


int main(int argc, char *argv[])
{
	UICAL_PRINT("************ UICal_Test runs 			************\n");

	while(1)
	{
		UICAL_PRINT("\n");
		UICAL_PRINT("************ press 1 to turn on/off		************\n");
		UICAL_PRINT("************ press 2 to set new parameters	************\n");
		UICAL_PRINT("************ press 3 to get parameters		************\n");
		UICAL_PRINT("************ press 0 to exit			************\n");

		memset(buf, 0, 512);
		read(0, buf, 512);

		//exit
		if (strcmp(buf, "0\n") == 0)
			return 0;
		//turn on/off
		else if (strcmp(buf, "1\n") == 0)
			AudioUICal_Test_TurnOnOff();
		//set new parameters
		else if (strcmp(buf, "2\n") == 0)
			AudioUICal_Test_Set();
		//get parameters
		else if (strcmp(buf, "3\n") == 0)
			AudioUICal_Test_Get();
		else
		{
			UICAL_PRINT("invalid input!\n");
			continue;
		}
	}

	return 0;
}
