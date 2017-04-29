/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************
*  Filename: Telmsg.h
*
*  Description: SMS AT commands implementation
*
*  History:
*   Jan 18, 2008 - Creation of file
*   Sep 22, 2008 - Modification for optimization of AT SMS module
*
*  Notes:
*
******************************************************************************/
#ifndef TELMSG_H
#define TELMSG_H

#include "ci_api_types.h"
#include "ci_msg.h"
/*
   typedef enum
   {
	MSGFLAG_MORETOSEND=0x01,
	MSGFLAG_STATUSREPORTRETURNED=0x02,
	MSGFLAG_HEADER=0x04,
	MSGFLAG_REPLYPATH=0x08
   }MSGFLAG;
 */ /*
   //
   // ProtocolID types
   //
 #define MSGPROTOCOL_UNKNOWN                 (0x00)
 #define MSGPROTOCOL_SMETOSME                (0x01)
 #define MSGPROTOCOL_IMPLICIT                (0x02)
 #define MSGPROTOCOL_TELEX                   (0x03)
 #define MSGPROTOCOL_TELEFAX_GROUP3          (0x04)
 #define MSGPROTOCOL_TELEFAX_GROUP4          (0x05)
 #define MSGPROTOCOL_VOICEPHONE              (0x06)
 #define MSGPROTOCOL_ERMES                   (0x07)
 #define MSGPROTOCOL_PAGING                  (0x08)
 #define MSGPROTOCOL_VIDEOTEX                (0x09)
 #define MSGPROTOCOL_TELETEX                 (0x0a)
 #define MSGPROTOCOL_TELETEX_PSPDN           (0x0b)
 #define MSGPROTOCOL_TELETEX_CSPDN           (0x0c)
 #define MSGPROTOCOL_TELETEX_PSTN            (0x0d)
 #define MSGPROTOCOL_TELETEX_ISDN            (0x0e)
 #define MSGPROTOCOL_UCI                     (0x0f)
 #define MSGPROTOCOL_MSGHANDLING             (0x10)
 #define MSGPROTOCOL_X400                    (0x11)
 #define MSGPROTOCOL_EMAIL                   (0x12)
 #define MSGPROTOCOL_SCSPECIFIC1             (0x13)
 #define MSGPROTOCOL_SCSPECIFIC2             (0x14)
 #define MSGPROTOCOL_SCSPECIFIC3             (0x15)
 #define MSGPROTOCOL_SCSPECIFIC4             (0x16)
 #define MSGPROTOCOL_SCSPECIFIC5             (0x17)
 #define MSGPROTOCOL_SCSPECIFIC6             (0x18)
 #define MSGPROTOCOL_SCSPECIFIC7             (0x19)
 #define MSGPROTOCOL_GSMSTATION              (0x1a)
 #define MSGPROTOCOL_SM_TYPE0                (0x1b)
 #define MSGPROTOCOL_RSM_TYPE1               (0x1c)
 #define MSGPROTOCOL_RSM_TYPE2               (0x1d)
 #define MSGPROTOCOL_RSM_TYPE3               (0x1e)
 #define MSGPROTOCOL_RSM_TYPE4               (0x1f)
 #define MSGPROTOCOL_RSM_TYPE5               (0x20)
 #define MSGPROTOCOL_RSM_TYPE6               (0x21)
 #define MSGPROTOCOL_RSM_TYPE7               (0x22)
 #define MSGPROTOCOL_RETURNCALL              (0x23)
 #define MSGPROTOCOL_ME_DOWNLOAD             (0x24)
 #define MSGPROTOCOL_DEPERSONALIZATION       (0x25)
 #define MSGPROTOCOL_SIM_DOWNLOAD            (0x26)
 */

#define DCSTYPE_GENERAL     (0x00)
#define DCSTYPE_MSGWAIT    (0x01)
#define DCSTYPE_MSGCLASS   (0x02)

#define DCSMSGCLASS_0  (0x00)                                   // @constdefine TBD
#define DCSMSGCLASS_1  (0x01)                                   // @constdefine TBD
#define DCSMSGCLASS_2   (0x02)                                  // @constdefine TBD
#define DCSMSGCLASS_3   (0x03)                                  // @constdefine TBD

#define DCSINDICATION_VOICEMAIL                 (0x00)          //voice mail
#define DCSINDICATION_FAX                       (0x01)          // Fax indication
#define DCSINDICATION_EMAIL                     (0x02)          //E-Mail indication
#define DCSINDICATION_OTHER                     (0x03)          //Other indication
/*
   typedef struct
   {
	UINT8     tsYear;
	UINT8     tsMonth;
	UINT8     tsDay;
	UINT8     tsHour;
	UINT8     tsMinute;
	UINT8     tsSecond;
	UINT8     tsTimezone;
	UINT8     tsZoneSign;
   }TIMESTAMP;

   typedef struct {
    UINT32 type;                       // @field DCS type
    UINT32 msgClass;                   // @field message class (Only for RIL_DCSTYPE_GENERAL and RIL_DCSTYPE_MSGCLASS)
    UINT32 alphabet;                   // @field DCS alphabet
    UINT32 indication;                 // @field indication (Only for RIL_DCSTYPE_MSGWAIT)
   } DCS;

   typedef struct
   {
	MSGFLAG bMsgFlag;
	UINT8     srcAddr[ CI_MAX_ADDRESS_LENGTH + 1];
	UINT32 protocolId;
	TIMESTAMP timeStamp;
	DCS bDCS;
   //	UINT32 bHdrLength;          // length of header
       UINT32 bMsgLength;         // length of body
   //     UINT8 hdrBuf[256];  //msg hdr buffer
       UINT8 msgBuf[512]; // msg body buffer
   }SHORTMESSAGE;*/

RETURNCODE_T  ciSelectSmsSrv(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciPrefStorage(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciMsgFormat(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciSrvCenterAddr(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciSetTextModeParam(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciSelectCbMsgType(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciNewMsgInd(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciListMsg(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciReadMsg(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciSendMsg(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciSendStoredMsg(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciStoreMsg(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciDeleteMsg(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciSetMoreMsgToSend(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciSetMoService(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciNewMsgAck(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciCMEMFULL(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);

void ciSendMsgAfterCtrlZ( TelAtParserID sAtpIndex, CHAR *pSmsData, INT32 smsDataLength );
void ciSendMsgAfterEsc( TelAtParserID sAtpIndex );

#endif

