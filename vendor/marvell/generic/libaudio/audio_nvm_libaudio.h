/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
INTEL CONFIDENTIAL
Copyright 2006 Intel Corporation All Rights Reserved.
The source code contained or described herein and all documents related to the source code ("Material") are owned
by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

No license under any patent, copyright, trade secret or other intellectual property right is granted to or
conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
Intel in writing.
-------------------------------------------------------------------------------------------------------------------*/

#include "IPPEqWrapper.h"


/************************      H i F i     E Q U A L I Z E R     P A R A M E R E R S   ***************************************/

const AUDIO_HiFi_EQ_Params hifiEqParamsTable[] =
{
    { 6, {16343, -31417, 15364, 14, -31417, 15323}, SPEAKER_PHONE, ON,  {"HiFi Eq Sp-defaults"} },
    { 6, {     0,      0,     0,  0,      0,     0}, HANDSET,       OFF, {"HiFi Eq Ep-defaults"} },
    {6, {14440, -28879, 14440, 14, -29523, 13504}, SPEAKER_PHONE_HP, OFF,  {"HiFi Eq HP Sp-defaults"} }
};


/************************      H i F i     V O L U M E     P A R A M E R E R S      ******************************************/

const AUDIO_HiFi_Volume_Params hifiVolumeParamsTable[] =
{
    { MP3, RINGTONE,  2 },
    { MP3, MUSIC,     5 },
    { MP3, ALARM,    10 },
    { AAC, RINGTONE,  3 },
    { AAC, MUSIC,     6 },
    { AAC, ALARM,    10 },
    { OGG, SYSTEM,    1 },
    { OGG, RINGTONE,  4 },
    { OGG, MUSIC,     7 },
    { OGG, ALARM,    12 },
    { MP4, MUSIC,     8 },
    { AVI, MUSIC,     9 }
};

