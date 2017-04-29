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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*     File name:      diag_tx  .h                                                 *
*     Programmer:     Shiri Dolev                                                 *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*       Create Date:  November, 2003                                              *
*                                                                                 *
*       Description: DIAG Tx Definitions - Handle the transmit reports functions. *
*                                                                                 *
*       Notes:                                                                    *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if !defined (_DIAG_TX_H_)
#define _DIAG_TX_H_

#include "osa.h"
#include "diag_osif.h"

typedef enum
{
	Static_Memory_Allocation,
	Pool_Memory_Allocation_Trace,
	Pool_Memory_Allocation_Signal
} Allocation_Type;

#define MAX_BUFFER_SIZE (16*1024)

struct diag_target_buffer
{
	enum {INSIDE_PACKET, OUT_OF_PACKET} state;
	unsigned int chunk_size;
	unsigned int packet_len;
	char chunk[MAX_BUFFER_SIZE];
};

/*-- Public Functions Prototypes ---------------------------------------*/
DIAG_EXPORT void diagSendPDUSignal( void *txPduBuffPtr, UINT16 Length );
void DiagSendFullFN(void);
void setL2Header(UINT8 *pHeader, UINT16 *pduLen);
UINT16 GetOnePacket(UINT8 *RawHeader, UINT8 * NewHeader, UINT16 pduLen, BOOL *Wraped);
int diag_disc_flush(struct diag_target_buffer* target_buff);
int diag_disc_rx(struct diag_target_buffer* target_buff, char * buff, int size);

/*----------------------------------------------------------------------*/
 #endif  /* _DIAG_TX_H_ */

