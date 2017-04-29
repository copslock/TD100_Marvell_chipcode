/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (?Material? are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel?s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/******************************************************************************
*               MODULE IMPLEMENTATION FILE
*******************************************************************************
* Title: audioNull
*
* Filename: audioNull.c
*
* Target, platform: Common Platform, SW platform
*
* Authors: Isar Ariel
*
* Description: Null component (template and last component)
*
* Last Updated:
*
* Notes:
******************************************************************************/
#include "audionull.h"
#include "acmtypes.h"

static char voidFunc(void);
ACM_ComponentHandle HWNullHandle = { (ACMComponentInit_t)voidFunc,
				     (ACMEnablePath_t)voidFunc,
				     (ACMDisablePath_t)voidFunc,
				     (ACMVolumeSetPath_t)voidFunc,
				     (ACMMutePath_t)voidFunc,
				     (ACMGetPathsStatus_t)voidFunc,
				     (ACMGetPathAnalogGain_t)voidFunc,
				     (ACMSetPathAnalogGain_t)voidFunc,
				     (ACMGetActivePathNum_t) voidFunc,
				     (ACMSetLoopbackTestStatus_t) voidFunc,
				     (ACMGetLoopbackTestStatus_t) voidFunc
};


static char voidFunc(void)
{
	return 0;
}

