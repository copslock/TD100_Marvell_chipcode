/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (“Material”) are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

//
// Common NVM file header
//

#ifndef _NVM_HEADER_H_
#define _NVM_HEADER_H_

#ifndef HERMON_NVM_HEADER_ALREADY_DEFINED
#define HERMON_NVM_HEADER_ALREADY_DEFINED

// NVM Header according to NVM Structure 1.0

//ICAT EXPORTED STRUCT
typedef struct NVM_Header
{
	unsigned long StructSize;       // the size of the user structure below
	unsigned long NumofStructs;     // >1 in case of array of structs (default is 1).
	char StructName[64];            // the name of the user structure below
	char Date[32];                  // date updated by the ICAT when the file is saved. Filled by calibration SW.
	char time[32];                  // time updated by the ICAT when the file is saved. Filled by calibration SW.
	char Version[64];               // user version - this field is updated by the SW eng. Every time they update the UserStruct.
	char HW_ID[32];                 // signifies the board number. Filled by calibration SW.
	char CalibVersion[32];          // signifies the calibration SW version. Filled by calibration SW.
} NVM_Header_ts;

#endif  /*HERMON_NVM_HEADER_ALREADY_DEFINED*/

#endif  //_NVM_HEADER_H_

