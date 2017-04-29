/*--------------------------------------------------------------------------------------------------------------------
 *    (C) Copyright 2011 Marvell Technology Group Ltd. All Rights Reserved.
 *-------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
 *    MARVELL CONFIDENTIAL
 *    Copyright 2011 Marvell Corporation All Rights Reserved.
 *    The source code contained or described herein and all documents related to the source code are owned
 *    by MArvell Corporation or its suppliers or licensors. Title to the Material remains with Marvell Corporation or
 *    its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
 *    Marvell or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
 *    treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
 *    transmitted, distributed, or disclosed in any way without Marvell's prior express written permission.
 *
 *    No license under any patent, copyright, trade secret or other intellectual property right is granted to or
 *    conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
 *    estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
 *    MArvell in writing.
 *-------------------------------------------------------------------------------------------------------------------*/

/******************************************************************************
 *               MODULE IMPLEMENTATION FILE
 *******************************************************************************
 * Title:
 *
 * Filename: diag_aud_eq.h
 *
 * Authors: Nenghua Cao
 *
 * Description: In ACAT, in order to open EQ files in AP folder, We must export the definitions in AP diag code.
 *
 * Last Updated :
 *
 * Notes:
 * ******************************************************************************/

#ifndef _DIAG_AUDIO_EQ_H_
#define _DIAG_AUDIO_EQ_H_

#ifdef __cplusplus
extern "C"{
#endif


//ICAT EXPORTED ENUM
typedef enum
{
        ON = 0,
        OFF,

        ON_OFF_ENUM_32_BIT = 0x7FFFFFFF  /* 32bit enum compiling enforcement */
} On_Off;

//ICAT EXPORTED ENUM
typedef enum
{
        SPEAKER_PHONE_IIR1,
        HANDSET,
        HEADSET,
        CAR_KIT,
        BT_HEADSET,
        SPEAKER_PHONE_IIR2,       //Non voice call filter

    BUILTIN_MIC_IIR_1,
    BUILTIN_MIC_IIR_2,
    MAX_APPLIANCE_VALUE,

    APPLIANCE_TYPE_ENUM_32_BIT = 0x7FFFFFFF  /* 32bit enum compiling enforcement */
} Appliance;

//ICAT EXPORTED ENUM
typedef enum
{
        MP3,
        AAC,
        OGG,
        MP4,
        AVI,

        CONTAINER_TYPE_ENUM_32_BIT = 0x7FFFFFFF  /* 32bit enum compiling enforcement */
} Container_Type;

//ICAT EXPORTED ENUM
typedef enum
{
        SYSTEM,
        RINGTONE,
        MUSIC,
        ALARM,

        STREAM_TYPE_ENUM_32_BIT = 0x7FFFFFFF  /* 32bit enum compiling enforcement */
} Stream_Type;


//ICAT EXPORTED STRUCT
typedef struct
{
        unsigned short  nSampleRate;  //change from NOofTaps. ;
        signed short    params[6];
        Appliance       appliance;
        On_Off          onOff;
        unsigned char   description[32];
} AUDIO_HiFi_EQ_Params;

//ICAT EXPORTED STRUCT
typedef struct
{
    Container_Type      containerType;
    Stream_Type         streamType;
    unsigned short      volumeOffset;
} AUDIO_HiFi_Volume_Params;

//ICAT EXPORTED STRUCT
typedef struct
{
    unsigned short      noOfTaps;
    short                       params[6];
    unsigned long       nSampleRate;
    Appliance           appliance;
    unsigned long       nBand;
    On_Off              OnOff;

    unsigned char  description[32];
} AUDIO_HiFi_MultiEQ_Params;

//ICAT EXPORTED STRUCT
typedef struct{
        int             nOffset; //Q16
        int             nThreshold;  //Q16
        int             nCompressFlag; //0: expand 1:compress
        int             nRatio; //Q16
}ae_drc_gain_curve_t;

#define MAX_DRC_SECTIONS 5

//DRC

//ICAT EXPORTED STRUCT
typedef struct
{
        //Totally (nNumOfDrcSections-1) threshold, compose nNumOfDrcSections sections,

        //eg: 3 threshold, 4 sections

        //[-100, nThreshold0] [nThreshold0, nThreshold1] [nThreshold1, nThreshold2] [nThreshold2, 0]

        ae_drc_gain_curve_t     drcGainMap[MAX_DRC_SECTIONS];
        unsigned int                    nNumOfDrcSections;
        unsigned int            nAttackTime;   //ms
        unsigned int            nReleaseTime;  //ms
}AUDIO_HIFI_DRC_Params;



#ifdef __cplusplus
}
#endif
#endif  /* _DIAG_AUD_EQ_H_ */
