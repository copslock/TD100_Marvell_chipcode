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

#include "acm.h"
#include "acmtypes.h"
#include "acm_device.h"
#include "acmplatform.h"

const ACM_DeviceHandle deviceTableMicco_TTCEVB[] =
{
	/*   device                 format           component             path                    pathDirection */
	{ ACM_MAIN_SPEAKER,	  ACM_MSA_PCM, MICCO_COMPONENT,	    MICCO_IN_TO_OUT,	ACM_PATH_NOT_CONNECTED }
	,{ ACM_MAIN_SPEAKER,	   ACM_MSA_PCM, MICCO_COMPONENT,     MICCO_PCM_TO_BEAR,	 ACM_PATH_OUT		}

	,{ ACM_OUTPUT_DEVICE_23,   ACM_MSA_PCM, MICCO_COMPONENT,     MICCO_IN_TO_OUT,	 ACM_PATH_NOT_CONNECTED }
	,{ ACM_OUTPUT_DEVICE_23,   ACM_MSA_PCM, MICCO_COMPONENT,     MICCO_PCM_TO_BEAR,	 ACM_PATH_OUT		}

	,{ ACM_AUX_SPEAKER,	   ACM_MSA_PCM, MICCO_COMPONENT,     MICCO_PCM_TO_MONO,	 ACM_PATH_OUT		}

	,{ ACM_HEADSET_SPEAKER,	   ACM_MSA_PCM, MICCO_COMPONENT,     MICCO_IN_TO_OUT,	 ACM_PATH_NOT_CONNECTED }
	,{ ACM_HEADSET_SPEAKER,	   ACM_MSA_PCM, MICCO_COMPONENT,     MICCO_PCM_TO_SPKR,	 ACM_PATH_OUT		}

	,{ ACM_MONO_LEFT_SPEAKER,  ACM_MSA_PCM, MICCO_COMPONENT,     MICCO_IN_TO_OUT,	 ACM_PATH_NOT_CONNECTED }
	,{ ACM_MONO_LEFT_SPEAKER,  ACM_MSA_PCM, MICCO_COMPONENT,     MICCO_PCM_TO_SPKRL, ACM_PATH_OUT		}

	,{ ACM_MONO_RIGHT_SPEAKER, ACM_MSA_PCM, MICCO_COMPONENT,     MICCO_IN_TO_OUT,	 ACM_PATH_NOT_CONNECTED }
	,{ ACM_MONO_RIGHT_SPEAKER, ACM_MSA_PCM, MICCO_COMPONENT,     MICCO_PCM_TO_SPKRR, ACM_PATH_OUT		}

	,{ ACM_MAIN_SPEAKER,	   ACM_I2S,	MICCO_COMPONENT,     MICCO_I2S_TO_BEAR,	 ACM_PATH_NOT_CONNECTED }
	,{ ACM_AUX_SPEAKER,	   ACM_I2S,	MICCO_COMPONENT,     MICCO_I2S_TO_MONO,	 ACM_PATH_NOT_CONNECTED }
	,{ ACM_HEADSET_SPEAKER,	   ACM_I2S,	MICCO_COMPONENT,     MICCO_I2S_TO_SPKR,	 ACM_PATH_NOT_CONNECTED }
	,{ ACM_MONO_LEFT_SPEAKER,  ACM_I2S,	MICCO_COMPONENT,     MICCO_I2S_TO_SPKRL, ACM_PATH_NOT_CONNECTED }
	,{ ACM_MONO_RIGHT_SPEAKER, ACM_I2S,	MICCO_COMPONENT,     MICCO_I2S_TO_SPKRR, ACM_PATH_NOT_CONNECTED }


	,{ ACM_MIC,		   ACM_MSA_PCM, MICCO_COMPONENT,     MICCO_MIC1_TO_PCM,	 ACM_PATH_IN		}
	,{ ACM_MIC,		   ACM_I2S,	MICCO_COMPONENT,     MICCO_MIC1_TO_PCM,	 ACM_PATH_IN		}

	,{ ACM_AUX_MIC,		   ACM_MSA_PCM, MICCO_COMPONENT,     MICCO_MIC1_TO_PCM,	 ACM_PATH_IN		}
	,{ ACM_AUX_MIC,		   ACM_I2S,	MICCO_COMPONENT,     MICCO_MIC1_TO_PCM,	 ACM_PATH_IN		}

	,{ ACM_HEADSET_MIC,	   ACM_MSA_PCM, MICCO_COMPONENT,     MICCO_MIC2_TO_PCM,	 ACM_PATH_IN		}
	,{ ACM_HEADSET_MIC,	   ACM_I2S,	MICCO_COMPONENT,     MICCO_MIC2_TO_PCM,	 ACM_PATH_IN		}



	,{ ACM_BLUETOOTH_SPEAKER,  ACM_MSA_PCM, BLUETOOTH_COMPONENT, BLUETOOTH_OUT,	 ACM_PATH_OUT		}
	,{ ACM_BLUETOOTH_MIC,	   ACM_MSA_PCM, BLUETOOTH_COMPONENT, BLUETOOTH_IN,	 ACM_PATH_IN		}


	/* Must be the last one! */
	,{ ACM_NOT_CONNECTED,	   0,		NULL_COMPONENT,	     0,			 ACM_PATH_NOT_CONNECTED }
};

const ACM_DeviceHandle deviceTableLevante_TTCDKB[] =
{
	/* device                  format          component               path                            pathDirection */
	{ ACM_MAIN_SPEAKER,	 ACM_MSA_PCM, LEVANTE_COMPONENT,   LEVANTE_IN_L_TO_OUT,		  ACM_PATH_OUT		 }
	,{ ACM_MAIN_SPEAKER,	  ACM_MSA_PCM, LEVANTE_COMPONENT,   LEVANTE_PCM_L_TO_EAR,	   ACM_PATH_NOT_CONNECTED }
	,{ ACM_MIC,		  ACM_MSA_PCM, LEVANTE_COMPONENT,   LEVANTE_AnaMIC1_TO_PCM_L,	   ACM_PATH_IN		  }

	,{ ACM_AUX_SPEAKER,	  ACM_MSA_PCM, LEVANTE_COMPONENT,   LEVANTE_PCM_L_TO_SPKR,	   ACM_PATH_OUT		  }
	,{ ACM_AUX_MIC,		  ACM_MSA_PCM, LEVANTE_COMPONENT,   LEVANTE_AnaMIC1_LOUD_TO_PCM_L, ACM_PATH_IN		  }

	,{ ACM_HEADSET_SPEAKER,	  ACM_MSA_PCM, LEVANTE_COMPONENT,   LEVANTE_IN_L_TO_OUT,	   ACM_PATH_OUT		  }
	,{ ACM_HEADSET_SPEAKER,	  ACM_MSA_PCM, LEVANTE_COMPONENT,   LEVANTE_PCM_L_TO_HS,	   ACM_PATH_NOT_CONNECTED }
	,{ ACM_HEADSET_MIC,	  ACM_MSA_PCM, LEVANTE_COMPONENT,   LEVANTE_AnaMIC2_TO_PCM_L,	   ACM_PATH_IN		  }

	,{ ACM_MAIN_SPEAKER,	  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_I2S_TO_EAR,		   ACM_PATH_OUT		  }
	,{ ACM_MIC,		  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_AnaMIC1_TO_PCM_I2S_L,  ACM_PATH_IN		  }
	,{ ACM_MIC,		  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_AnaMIC1_TO_PCM_I2S_R,  ACM_PATH_IN		  }
	,{ ACM_MIC_EC,		  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_AnaMIC1_TO_PCM_I2S_L,  ACM_PATH_IN		  }
	,{ ACM_MIC_EC,		  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_AnaMIC1_TO_PCM_I2S_R,  ACM_PATH_IN		  }
	,{ ACM_MIC_EC,		  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_DAC1_DAC2_IN_EC_LOOP,  ACM_PATH_IN		  }

	,{ ACM_AUX_SPEAKER,	  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_I2S_TO_SPKR,	   ACM_PATH_OUT		  }
	,{ ACM_AUX_MIC,		  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_AnaMIC1_LOUD_TO_PCM_I2S_L, ACM_PATH_IN	  }
	,{ ACM_AUX_MIC,		  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_AnaMIC1_LOUD_TO_PCM_I2S_R, ACM_PATH_IN	  }
	,{ ACM_AUX_MIC_EC,	  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_AnaMIC1_LOUD_TO_PCM_I2S_L, ACM_PATH_IN	  }
	,{ ACM_AUX_MIC_EC,	  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_AnaMIC1_LOUD_TO_PCM_I2S_R, ACM_PATH_IN	  }
	,{ ACM_AUX_MIC_EC,	  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_DAC1_DAC2_IN_EC_LOOP,  ACM_PATH_IN		  }

	,{ ACM_HEADSET_SPEAKER,	  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_I2S_TO_HS,		   ACM_PATH_NOT_CONNECTED }
	,{ ACM_HEADSET_MIC,	  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_AnaMIC2_TO_PCM_I2S_L,   ACM_PATH_IN		  }
	,{ ACM_HEADSET_MIC,	  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_AnaMIC2_TO_PCM_I2S_R,   ACM_PATH_IN		  }
	,{ ACM_HEADSET_MIC_EC,	  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_AnaMIC2_TO_PCM_I2S_L,   ACM_PATH_IN		  }
	,{ ACM_HEADSET_MIC_EC,	  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_AnaMIC2_TO_PCM_I2S_R,   ACM_PATH_IN		  }
	,{ ACM_HEADSET_MIC_EC,	  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_DAC1_DAC2_IN_EC_LOOP,  ACM_PATH_IN		  }

	,{ ACM_BLUETOOTH_SPEAKER, ACM_MSA_PCM, BLUETOOTH_COMPONENT, BLUETOOTH_OUT,		   ACM_PATH_OUT		  }
	,{ ACM_BLUETOOTH_MIC,	  ACM_MSA_PCM, BLUETOOTH_COMPONENT, BLUETOOTH_IN,		   ACM_PATH_IN		  }

	,{ ACM_BLUETOOTH9_SPEAKER, ACM_MSA_PCM, BLUETOOTH_COMPONENT, BLUETOOTH_OUT,		   ACM_PATH_OUT		  }
	,{ ACM_BLUETOOTH9_MIC,	   ACM_MSA_PCM, BLUETOOTH_COMPONENT, BLUETOOTH_IN,		   ACM_PATH_IN		  }

	,{ ACM_HEADSET_SPEAKER,	  ACM_AUX_FM,  LEVANTE_COMPONENT,   LEVANTE_AUX2_TO_HS1,	   ACM_PATH_OUT		  }
	,{ ACM_HEADSET_SPEAKER,	  ACM_AUX_FM,  LEVANTE_COMPONENT,   LEVANTE_AUX1_TO_HS2,	   ACM_PATH_OUT		  }

	,{ ACM_AUX_SPEAKER,	  ACM_AUX_FM,  LEVANTE_COMPONENT,   LEVANTE_AUX1_TO_SIDETONE_L,	   ACM_PATH_IN		  }
	,{ ACM_AUX_SPEAKER,	  ACM_AUX_FM,  LEVANTE_COMPONENT,   LEVANTE_AUX2_TO_SIDETONE_R,	   ACM_PATH_IN		  }
	,{ ACM_AUX_SPEAKER,	  ACM_AUX_FM,  LEVANTE_COMPONENT,   LEVANTE_SIDETONE_IN_TO_OUT,	   ACM_PATH_OUT		  }
	,{ ACM_AUX_SPEAKER,	  ACM_AUX_FM,  LEVANTE_COMPONENT,   LEVANTE_SIDETONE_TO_SPKR,	   ACM_PATH_OUT		  }

	,{ ACM_MAIN_SPEAKER,	  ACM_AUX_FM,  LEVANTE_COMPONENT,   LEVANTE_AUX1_TO_I2S_L,	   ACM_PATH_OUT		  }
	,{ ACM_MAIN_SPEAKER,	  ACM_AUX_FM,  LEVANTE_COMPONENT,   LEVANTE_AUX2_TO_I2S_R,	   ACM_PATH_OUT		  }
	,{ ACM_MAIN_SPEAKER,	  ACM_AUX_FM,  LEVANTE_COMPONENT,   LEVANTE_I2S_INT_LOOP,	   ACM_PATH_OUT		  }
	,{ ACM_MAIN_SPEAKER,	  ACM_AUX_FM,  LEVANTE_COMPONENT,   LEVANTE_I2S_TO_EAR,		   ACM_PATH_OUT		  }

	,{ ACM_BLUETOOTH_SPEAKER, ACM_AUX_FM,  LEVANTE_COMPONENT,   LEVANTE_AUX1_TO_PCM_I2S_L,	   ACM_PATH_OUT		  }
	,{ ACM_BLUETOOTH_SPEAKER, ACM_AUX_FM,  LEVANTE_COMPONENT,   LEVANTE_AUX2_TO_PCM_I2S_R,	   ACM_PATH_OUT		  }

	,{ ACM_AUX_MIC,		  ACM_AUX_FM,  LEVANTE_COMPONENT,   LEVANTE_AUX1_TO_PCM_I2S_L,	   ACM_PATH_IN		  }
	,{ ACM_AUX_MIC,		  ACM_AUX_FM,  LEVANTE_COMPONENT,   LEVANTE_AUX2_TO_PCM_I2S_R,	   ACM_PATH_IN		  }

	,{ ACM_MIC_LOOP_SPEAKER,  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_AnaMIC1_LOUD_TO_I2S_L, ACM_PATH_IN		  }
	,{ ACM_MIC_LOOP_SPEAKER,  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_AnaMIC1_LOUD_TO_I2S_R, ACM_PATH_IN		  }
	,{ ACM_MIC_LOOP_SPEAKER,  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_I2S_INT_LOOP,	   ACM_PATH_IN		  }
	,{ ACM_MIC_LOOP_SPEAKER,  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_I2S_TO_SPKR,	   ACM_PATH_IN		  }

	,{ ACM_MIC_LOOP_EARPIECE, ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_AnaMIC1_TO_I2S_L,	   ACM_PATH_IN		  }
	,{ ACM_MIC_LOOP_EARPIECE, ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_AnaMIC1_TO_I2S_R,	   ACM_PATH_IN		  }
	,{ ACM_MIC_LOOP_EARPIECE, ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_I2S_INT_LOOP,	   ACM_PATH_IN		  }
	,{ ACM_MIC_LOOP_EARPIECE, ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_I2S_TO_EAR,		   ACM_PATH_IN		  }

	,{ ACM_HEADSET_MIC_LOOP,  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_AnaMIC2_TO_I2S_L,	   ACM_PATH_IN		  }
	,{ ACM_HEADSET_MIC_LOOP,  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_AnaMIC2_TO_I2S_R,	   ACM_PATH_IN		  }
	,{ ACM_HEADSET_MIC_LOOP,  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_I2S_INT_LOOP,	   ACM_PATH_IN		  }
	,{ ACM_HEADSET_MIC_LOOP,  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_I2S_TO_HS,	           ACM_PATH_IN		  }

	//Jackie,2011-0222
	//Loop include codec and MSA
	,{ACM_MAIN_SPEAKER__LOOP,    ACM_MSA_PCM, LEVANTE_COMPONENT,   LEVANTE_PCM_L_TO_EAR_LOOP,          ACM_PATH_OUT   }
	,{ACM_MIC__LOOP,             ACM_MSA_PCM, LEVANTE_COMPONENT,   LEVANTE_AnaMIC1_TO_PCM_L_LOOP,      ACM_PATH_IN    }

	,{ACM_HEADSET_SPEAKER__LOOP, ACM_MSA_PCM, LEVANTE_COMPONENT,   LEVANTE_PCM_L_TO_HS_LOOP,           ACM_PATH_OUT   }
	,{ACM_HEADSET_MIC__LOOP,     ACM_MSA_PCM, LEVANTE_COMPONENT,   LEVANTE_AnaMIC2_TO_PCM_L_LOOP,      ACM_PATH_IN    }

	,{ACM_AUX_SPEAKER__LOOP,     ACM_MSA_PCM, LEVANTE_COMPONENT,   LEVANTE_PCM_L_TO_SPKR_LOOP,         ACM_PATH_OUT   }
	,{ACM_AUX_MIC__LOOP,         ACM_MSA_PCM, LEVANTE_COMPONENT,   LEVANTE_AnaMIC1_LOUD_TO_PCM_L_LOOP, ACM_PATH_IN    }

	/*nenghua, force speaker*/
	,{ ACM_FORCE_SPEAKER,	  ACM_I2S,     LEVANTE_COMPONENT,   LEVANTE_I2S_TO_SPKR_HS,	   ACM_PATH_OUT		  }

	/* Must be the last one! */
	,{ ACM_NOT_CONNECTED,	  0,	       NULL_COMPONENT,	    0,				   ACM_PATH_NOT_CONNECTED }
};

