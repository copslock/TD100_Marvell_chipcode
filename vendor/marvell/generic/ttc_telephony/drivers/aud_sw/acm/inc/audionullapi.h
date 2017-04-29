/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (�Material�) are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel�s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/*******************************************************************************
*               MODULE HEADER FILE
********************************************************************************
* Title: audioNullapi
*
* Filename: audioNullapi.h
*
* Target, platform: Common Platform, SW platform
*
* Authors: Isar Ariel
*
* Description:
*
* Last Updated:
*
* Notes:
*******************************************************************************/
#ifndef _AUDIO_NULL_API_H_
#define _AUDIO_NULL_API_H_

#include "acmtypes.h"

extern ACM_ComponentHandle HWNullHandle;

typedef enum
{
	NULL_SIMPLE = 0,
	NULL_TOTAL_VALID_PATHS //Must the be last one
} NullPaths;

#endif /*_AUDIO_NULL_API_H_*/
