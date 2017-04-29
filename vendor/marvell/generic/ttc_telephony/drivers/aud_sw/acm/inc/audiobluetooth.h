/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (?Material?) are owned
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

// audioBluetooth.h
#include "acmtypes.h"


extern ACM_ComponentHandle HWBlueToothHandle;

//ICAT EXPORTED ENUM
typedef enum
{
	BT_FAIL = 0,
	BT_OK,
	BT_NO_FILE
} BT_FDI_READ_FILE_STATUS;

typedef enum
{
	BLUETOOTH_OUT = 0,
	BLUETOOTH_IN,
	BLUETOOTH_APP_OUT,
	BLUETOOTH_APP_IN,
	NUM_OF_BLUETOOTH_PATHS
} BlueToothPaths;


/*----------- Global function prototypes -------------------------------------*/
void AppblueToothDeviceEnable(void);
void CommunicationsblueToothDeviceEnable(void);



