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

/*********************************************************************
*                      M O D U L E     B O D Y                       *
**********************************************************************
* Title: Diag                                                        *
*                                                                    *
* Filename: diag_comm_L2.c                                           *
*                                                                    *
* Target, platform: Common Platform, SW platform                     *
*                                                                    *
* Authors:Boaz Sommer                                                *
*                                                                    *
* Description: Provide layer 2 of communication                      *
*                                                                    *
* Notes:                                                             *
*                                                                    */
/************* General include definitions ****************************/
#include "global_types.h"

//#include "ICAT_config.h"
#include "diag_config.h"

#include "diag_osif.h" // changing place
#include INCLUDE_ASSERT_OR_DUMMY

#include "diag.h"
//#include "diag_ram.h"
#include "diag_buff.h"
#include "diag_comm.h"
#include "diag_comm_L2.h"
#include "diag_comm_L4.h"
//#include "diag_mem.h"

#if defined (OSA_LINUX)
#define DIAG_SSP_L2_RX_NO_TIMEOUTS
#endif

typedef enum
{
    SM_WAIT_STX,
    SM_WAIT_PID,
    SM_WAIT_LEN,
    SM_WAIT_DATA,
    SM_WAIT_CHKSUM,
    SM_WAIT_ETX
}L2RxDataStateMachineEnum;


static  L2HeaderStruct  L2Header;
static  L2FooterStruct  L2Footer;
static  L2DataStruct    L2Data;

static  UINT8           EtxExpectBytes;

static  L2RxDataStateMachineEnum    L2RxDataStateMachine;

static  void    diagCommL2ReportBadRxMessage    (void);
static  void    diagCommL2ReportRxCommunicationHalt (void);

static  UINT32          diagSppRxNumberOfBadMessages = 0;

#if !defined(OSA_LINUX)
//TBDIY this mode is not reentrant and in Linux we might have reentrancy due to multiple interfaces or commands within the code in addition to ACAT commands.
//This may lead to a memory leak or a double free.
#define DIAG_COMM_L2_DEBUG
#endif

#if defined (DIAG_COMM_L2_DEBUG)
static  L2DataStruct    LastL2Data  =   {0,0,0};
#endif

/*-----------------1/19/2006 12:04------------------
 * Added time for cases where message from ICAT are
 * cut before finish, and no other messages are availalbe
 * --------------------------------------------------*/
#if defined (_DIAG_USE_VIRTIO_)
#define  DIAG_SSP_L2_RX_NO_TIMEOUTS
#endif

#if !defined (DIAG_SSP_L2_RX_NO_TIMEOUTS)
#define TAVOR_SLOW_CLOCK_RATE       32768

#if defined (OSA_LINUX)
	#define DIAG_SSP_L2_RX_TIMEOUT      (TAVOR_SLOW_CLOCK_RATE) //multiply bu 5?
#else
	#define DIAG_SSP_L2_RX_TIMEOUT      TAVOR_SLOW_CLOCK_RATE
#endif

void    diagCommSspL2TimerExpired   (UINT32 num);


OSTimerRef  diagCommSspL2TimerRef;

#endif

#define DiagAlignMalloc malloc
#define DiagAlignFree free

static  void    ResetStxEtx(void)
{
    L2Header.Stx    =   0;
    L2Footer.Etx    =   0;
}


void    diagCommL2ResetRxStateMachine   (void)
{
    ResetStxEtx ();

    L2RxDataStateMachine    =   SM_WAIT_STX;

    if (L2Data.Buffer)
	{
        DiagAlignFree(L2Data.Buffer);
		L2Data.Buffer=0;
	}

    memset  (&L2Data , 0 , sizeof (L2DataStruct));
    memset  (&L2Header , 0 , sizeof(L2HeaderStruct));
}


void    diagCommL2Init  (void)
{
    ResetStxEtx ();
    L2RxDataStateMachine    =   SM_WAIT_STX;

#if !defined (DIAG_SSP_L2_RX_NO_TIMEOUTS)
    OSATimerCreate(&diagCommSspL2TimerRef);
	OSATimerStart(diagCommSspL2TimerRef, DIAG_SSP_L2_RX_TIMEOUT, 0 , diagCommSspL2TimerExpired,0);

#endif
}


UINT32	CalcLongMessageLength	(L2HeaderStruct *l2_header)
{
    return	(DIAG_SSP_GET_LONG_MESSAGE_LENGTH(l2_header->ProtocolID,l2_header->Length));
}

// This API process all messages in given buffer and send them to hte rx-task (or spaciel
// handshake). 
void    diagCommL2ReceiveL1Packet   (UINT8   *buff , UINT32  length , BOOL isRecursive, COMDEV_NAME rxif)
{
    UINT32  buff_index;
    UINT8   *L2DataBufferSave;
#if defined (DIAG_SSP_L2_RECOVER_LOST_DATA_IN_BUFFER)
    UINT32  L2DataBufferLengthSave;
#endif

#if !defined (DIAG_SSP_L2_RX_NO_TIMEOUTS)

    OSATimerStop(diagCommSspL2TimerRef);
#endif

    for (buff_index = 0 ; buff_index < length ; buff_index++)
    {
        switch  (L2RxDataStateMachine)
        {
            // Waiting for start of L2 message (i.e. 0x7E7E)
            case SM_WAIT_STX:
                L2Header.Stx <<= 8;
                L2Header.Stx |= buff[buff_index];
                if  (L2Header.Stx == L2_STX)
                    L2RxDataStateMachine    =   SM_WAIT_PID;
                break;

            // Waiting for L2 protocol ID field
            case SM_WAIT_PID:
//#if !defined (DIAG_SSP_USE_CHKSUM)
                // unexpected PID field - "bad" message.
                if  (buff[buff_index] == L2_STX_LOW)
                {
					// still StartOfTransmit - lets allow it, we still wait for PID 

                    //diagCommL2ReportBadRxMessage    ();
                    //ResetStxEtx();
                    //buff_index--;   // will be increased by for loop
                    //L2RxDataStateMachine    =   SM_WAIT_STX;
                }
                else
//#endif
                {
                    L2Header.ProtocolID =   buff[buff_index];
                    L2RxDataStateMachine    =   SM_WAIT_LEN;
                }
                break;
            // Waiting for L2 length field
            case SM_WAIT_LEN:
                L2Header.Length =   buff[buff_index];
                L2Data.Length   =   L2Header.Length;
                if  (L2Header.ProtocolID & PID_LONG)
                    L2Data.Length   =   CalcLongMessageLength   (&L2Header);

                if  (L2Data.Length == 0)
                {
                    ResetStxEtx();
                    L2RxDataStateMachine    =   SM_WAIT_STX;
                    break;
                }
                L2RxDataStateMachine    =   SM_WAIT_DATA;

                L2Data.Buffer   =   DiagAlignMalloc (L2Data.Length);
                DIAG_ASSERT  (L2Data.Buffer != NULL);
                L2Data.Index    =   0;
                break;

            // Waiting for data items, up to length bytes.
            case SM_WAIT_DATA:
                L2Data.Buffer   [L2Data.Index++]    =   buff[buff_index];
                if  (L2Data.Index == L2Data.Length)
                {
#if defined (DIAG_SSP_USE_CHKSUM)
                    if  (L2Header.ProtocolID & PID_CHKSUM)
                        L2RxDataStateMachine    =   SM_WAIT_CHKSUM;
                    else
#endif
                        EtxExpectBytes  =   L2_ETX_SIZEOF;
                        L2RxDataStateMachine    =   SM_WAIT_ETX;
                }
                break;

            // Waiting for checksum. Currently not supported
            case SM_WAIT_CHKSUM:

                EtxExpectBytes  =   L2_ETX_SIZEOF;
                L2RxDataStateMachine    =   SM_WAIT_ETX;
                break;

            // Waiting for end of L2 message indicator (i.e. 0x8181)
            case SM_WAIT_ETX:
                L2Footer.Etx <<= 8;
                L2Footer.Etx |= buff[buff_index];

                // Found end of message
                if  (L2Footer.Etx == L2_ETX)
                {
                    if  ((L2Header.ProtocolID & (PID_L4_BIT | PID_LONG)) == PID_L4_BIT)
                        diagCommL4ReceiveL2Packet   (L2Data.Buffer , L2Data.Index, rxif);
                    else
                        diagCommL4ReceiveL2FullMessage  (L2Data.Buffer , L2Data.Index, rxif);

#if defined (DIAG_COMM_L2_DEBUG)
                    // save last Rx message length
                    LastL2Data.Index    =   L2Data.Index;
                    LastL2Data.Length   =   L2Data.Length;

                    // Free last memory buffer
                    if  ((LastL2Data.Buffer != NULL) && (!isRecursive))
					{
						DiagAlignFree   (LastL2Data.Buffer);
					}

                    // point to last received message
                    LastL2Data.Buffer   =   L2Data.Buffer;
#else
                    if (!isRecursive)
					{
						if (L2Data.Buffer)
						{
							DiagAlignFree(L2Data.Buffer);
							L2Data.Buffer=0;
						}
					}
#endif

                    // reset L2Data
                    memset  (&L2Data , 0 , sizeof (L2DataStruct));
                    ResetStxEtx();
                    L2RxDataStateMachine    =   SM_WAIT_STX;
                }
                // Not ETX yet
                else
                {

                    EtxExpectBytes--;
                    // EtxExpectBytes is sizeof ETX. If no ETX after EtxExpectBytes - error in message.
                    if  (EtxExpectBytes == 0)
                    {


                        // Report error
                        diagCommL2ReportBadRxMessage    ();

                        // save buffer pointer
                        L2DataBufferSave        =   L2Data.Buffer;

#if defined (DIAG_SSP_L2_RECOVER_LOST_DATA_IN_BUFFER)
                        L2DataBufferLengthSave  =   L2Data.Index;
#endif

                        // reset state machine
                        ResetStxEtx();
                        L2RxDataStateMachine    =   SM_WAIT_STX;

                        // reset L2Data
                        memset  (&L2Data , 0 , sizeof (L2DataStruct));
#if defined (DIAG_SSP_L2_RECOVER_LOST_DATA_IN_BUFFER)
                        // if overlapped message is in the length field.
                        if  (L2Header.Length == L2_STX_LOW)
                        {
                            L2Header.Stx    =   L2_STX_LOW;
                        }

                        // call diagCommL2ReceiveL1Packet recursivly to search for next start of message
                        diagCommL2ReceiveL1Packet   (L2DataBufferSave,L2DataBufferLengthSave,TRUE, rxif);

                        // free buffer pointer only on top level call to this function
                        if  (!isRecursive)
#endif
						{
                            DiagAlignFree(L2DataBufferSave);
							L2DataBufferSave=0;
						}

                    }
                }
                break;
        }
    }

#if !defined (DIAG_SSP_L2_RX_NO_TIMEOUTS)
    if  (L2RxDataStateMachine != SM_WAIT_STX)
    {
        OSATimerStart(diagCommSspL2TimerRef, DIAG_SSP_L2_RX_TIMEOUT, 0 , diagCommSspL2TimerExpired,0);
    }
#endif
}


void    diagCommSspL2TimerExpired   (UINT32 num)
{
    UINT32  cpsr;
	DIAG_UNUSED_PARAM(num)

    diagCommL2ReportRxCommunicationHalt();

    diag_lock(&cpsr);
    diagCommL2ResetRxStateMachine();
    diag_unlock(cpsr);
}

void    diagCommL2ReportBadRxMessage    (void)
{
    //DIAG_FILTER(DIAG,PROTOCOL,L2_Rx_Error,DIAG_ERROR)
    //diagTextPrintf ("Error in received packet");

    diagSppRxNumberOfBadMessages++;
}

void    diagCommL2ReportRxCommunicationHalt (void)
{
    //DIAG_FILTER(DIAG,PROTOCOL,L2_Rx_Comm_Timout,DIAG_ERROR)
    //diagTextPrintf ("Timeout in received message");

}

//ICAT EXPORTED FUNCTION - Diag,Debug,getDiagSppRxNumberOfBadMessages
void    getDiagSppRxNumberOfBadMessages (void)
{
    //DIAG_FILTER(Diag,Debug,NumberOfBadRxMessages,DIAG_INFORMATION)
   // diagPrintf("diagSppRxNumberOfBadMessages = %ld",diagSppRxNumberOfBadMessages);

}
