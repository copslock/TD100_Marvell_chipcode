/*
 *                   SiRF Technology, Inc. GPS Software
 *
 *    Copyright (c) 2007-2009 by SiRF Technology, Inc. All rights reserved.
 *
 *    This Software is protected by United States copyright laws and
 *    international treaties.  You may not reverse engineer, decompile
 *    or disassemble this Software.
 *
 *    WARNING:
 *    This Software contains SiRF Technology Inc.’s confidential and
 *    proprietary information. UNAUTHORIZED COPYING, USE, DISTRIBUTION,
 *    PUBLICATION, TRANSFER, SALE, RENTAL OR DISCLOSURE IS PROHIBITED
 *    AND MAY RESULT IN SERIOUS LEGAL CONSEQUENCES.  Do not copy this
 *    Software without SiRF Technology, Inc.’s  express written
 *    permission.   Use of any portion of the contents of this Software
 *    is subject to and restricted by your signed written agreement with
 *    SiRF Technology, Inc.
 *
 *  MODULE:         LSM (Location Session Manager)
 *
 *  FILENAME:       LSM_Types.h
 *
 *  DESCRIPTION:    This header file includes all LSM types and data structures other than those for 
 *                  SUPL INIT callback.
 *
 *  NOTES:          Only single session is supported for now.
 *
 ***************************************************************************
 *
 *  Keywords for Perforce. Do not modify.
 *
 *  $File: //customs/customer/Samsung_Volga/SDK/sirf/SiRFLPL/include/LSM_Types.h $
 *
 *  $DateTime: 2010/06/21 13:32:13 $
 *
 *  $Revision: #1 $
 *
 ***************************************************************************
*/
/**
 * @file
 * @brief This header file includes all LSM types and data structures other than those for LSM SUPL INIT callback.
 * @addtogroup LSM
 * @{
 */
#ifndef _LSM_TYPES_H_
#define _LSM_TYPES_H_

#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus*/

   /* ********************************-**********************************
    *                              TYPES                               *
    ********************************************************************/
#include "sirf_types.h"
#if (defined(SIRF_AGPS) && (SIRF_HOST_SDK_VERSION >= (400)))
#include "sirf_errors.h"
#endif /* SIRF_AGPS && SIRF_HOST_SDK_VERSION >= (400) */


   typedef tSIRF_BOOL      LSM_BOOL;

   typedef tSIRF_CHAR      LSM_CHAR;

   typedef tSIRF_UINT8     LSM_UINT8;

   typedef tSIRF_INT16     LSM_SINT16;
   typedef tSIRF_UINT16    LSM_UINT16;

   typedef unsigned int    LSM_UINT;

   typedef tSIRF_INT32     LSM_SINT32;
   typedef tSIRF_UINT32    LSM_UINT32;

   typedef tSIRF_UINT64    LSM_UINT64;

   typedef tSIRF_DOUBLE    LSM_DOUBLE;
   typedef tSIRF_FLOAT     LSM_FLOAT;

#include "LSM_SUPLInit.h"
   /* ********************************-**********************************
    *                             DEFINES                              *
    ********************************************************************/
   /** @def LSM_TRUE
    *  @brief true */
#define LSM_TRUE            1
   /** @def LSM_FALSE
    *  @brief false */
#define LSM_FALSE           0
   /** @def SLC_VER_LEN
    *  @brief SiRFLoc Firmware version string length */
#define SLC_VER_LEN         160
   /** @def LSM_IP_ADDR_LENGTH
    *  @brief IP address length */
#define LSM_IP_ADDR_LENGTH  256
   /** @def SET_ID_LENGTH
    *  @brief SET ID length */
#define SET_ID_LENGTH       8
   /** @def LSM_MAX_CHAN_NUM
    *  @brief maximum GPS channels */
#define LSM_MAX_CHAN_NUM    12
   /** @def AI3_REV_NUM_16
    *  @brief AI3 ICD1.6 */
#define AI3_REV_NUM_16      0x16
   /** @def AI3_REV_NUM_19
    *  @brief AI3 ICD1.9 */
#define AI3_REV_NUM_19      0x19
   /** @def AI3_REV_NUM_20
    *  @brief AI3 ICD2.0 */
#define AI3_REV_NUM_20      0x20
   /** @def AI3_REV_NUM_21
    *  @brief AI3 ICD2.1 */
#define AI3_REV_NUM_21      0x21
   /** @def AI3_REV_NUM_22
    *  @brief AI3 ICD2.2 */
#define AI3_REV_NUM_22      0x22
   /** @def F_REV_NUM_19
    *  @brief F ICD1.9 */
#define F_REV_NUM_19        0x19
   /** @def F_REV_NUM_20
    *  @brief F ICD2.0 */
#define F_REV_NUM_20        0x20
   /** @def F_REV_NUM_21
    *  @brief F ICD2.1 */
#define F_REV_NUM_21        0x21

#define LSM_MAX_FQDN_LEN    256
    /** @def LSM_MAX_NI_MSRMENTS
    *   @brief maximum Number Of Network Measurements for NI */
#define LSM_MAX_NI_MSRMENTS  10
    /** @def LSM_MAX_NI_MSRMENTS
    *   @brief maximum Number Of Cells For NI */
#define LSM_MAX_NI_CELLS      16


/* The following are defined in SUPL 1.0 specification */
#define LSM_MAX_TIME_SLOT 14
#define LSM_MAX_CELL_MEAS 32
#define LSM_MAX_FREQ 8

   /* ********************************-**********************************
    *                         DATA STRUCTURES                          *
    ********************************************************************/
   /* ------------------ For control plane use only ------------------ */
   /** @enum eLSM_RRC_STATE
    *  @brief For RRC state machine */
   typedef enum
   {
      /** RRC DCH state */
      LSM_RRC_STATE_CELL_DCH,
      /** RRC FACH state */
      LSM_RRC_STATE_CELL_FACH,
      /** RRC PCH state */
      LSM_RRC_STATE_CELL_PCH,
      /** RRC URA_PCH state */
      LSM_RRC_STATE_URA_PCH,
      /** RRC IDL state */
      LSM_RRC_STATE_IDLE,
      /** Invalid RRC state */
      LSM_RRC_INVALID_STATE
   }  eLSM_RRC_STATE;

   /** @enum eLSM_OTA_TYPE
    *  @brief For OTA message type */
   typedef enum
   {
      /** OTA message uses SUPL protocol */
      LSM_SUPL_RRLP_OR_RRC,
      /** OTA message uses RRLP protocol */
      LSM_CP_RRLP,
      /** OTA message uses RRC protocol */
      LSM_CP_RRC,
      /** OTA message uses RRC SIB15 protocol - not supported */
      LSM_RRC_SIB15,
      /** OTA message AGPS assist request protocol - not supported */
      LSM_SS_AGPS_ASSIST_REQ,
      /** OTA Message SGEE data */
      LSM_EE_PUSH,
      /** Invalid OTA type */
      LSM_INVALID_OTA_TYPE = (~0)
   }  eLSM_OTA_TYPE;

   /** @enum eLSM_SESSION_PRIORITY
    *  @brief positioning request priority */
   typedef enum
   {
      /** Normal positioning request */
      NO_EMERGENCY = 0x00,
      /** Emergent positioning request such as E911/112 */
      EMERGENCY = 0x01
   }  eLSM_SESSION_PRIORITY;

   /** @enum eLSM_UL_RRC_MSG_TYPE
    *  @brief Uplink RRC message types. */
   typedef enum
   {
      /** UL RRC message includeds MeasurementReport, which could be MeasuredResults/UE-Positioning-MeasuredResults or EventResults. */
      LSM_RRC_MEAS_REPORT = 0,
      /** UL RRC MeasurementControlFailure is included. */
      LSM_RRC_MEAS_FAILURE,
      /** UL RRCStatus is included  */
      LSM_RRC_STATUS
   }  eLSM_UL_RRC_MSG_TYPE;

   /** @struct LSM_lplOtaParams
    *  @brief For Uplink (UL) OTA message status */
   typedef struct
   {
      eLSM_UL_RRC_MSG_TYPE rrc_msg_type;
      struct
      {   /** valid if rrc_msg_type is LSM_RRC_MEAS_REPORT */
         LSM_UINT8 measurement_identity;
         /** 0: MeasuredResults or UE-Positioning-MeasuredResults, which is compile time decided inside LPL.
             1: EventResults is reported. */
         LSM_UINT8 measurement_or_event;
      }  meas_report;
      /** 1: final UL response; 0: more UL message to come. */
      LSM_BOOL is_final_response;
   }  LSM_lplOtaParams;

   /** @typedef LSM_BOOL (*LSM_SendNetMessage)(eLSM_OTA_TYPE ota_type,
                                               LSM_lplOtaParams *p_ota_params,
                                               LSM_UINT8* p_msg_data,
                                               LSM_UINT32 msg_size)
    *  @brief A callback with this type is registered through LSM_Init. LSM calls it when there is an UL CP message.
    *  @param[out] ota_type: OTA protocol type.
    *  @param[out] p_ota_params: pointer to the detailed info about the UL message.
    *  @param[out] p_msg_data: For LSM_CP_RRLP - Uplink RRLP message including the whole RRLP PDU;
    *                       @n For LSM_CP_RRC - Uplink RRC messages is defined by eLSM_UL_RRC_MSG_TYPE.
    *  @param[out] msg_size: UL message length in bytes.
    *  @return     LSM_TRUE: CPA successfully sent the UL CP message to the network;
    *           @n LSM_FALSE: CPA failed to send the UL CP message to the network.
    *  @warning  The UL RRC messages from LSM normally include LSM_RRC_MEAS_REPORT type of messages because
    *            LSM assumes the RRC stack takes care of MeasurementControlFailure and RRCStatus messages.
    *            However during the integration stage, the DL RRC messages may include errors and LPL needs
    *            to report them to CPA for debugging purpose.
    */
   typedef LSM_BOOL (*LSM_SendNetMessage)(eLSM_OTA_TYPE ota_type,
                                          LSM_lplOtaParams *p_ota_params,
                                          LSM_UINT8* p_msg_data,
                                          LSM_UINT32 msg_size);
   /** @struct LSM_MessageListener
    * @brief CPA calls CP_SetMessageListeners to pass an LSM_MessageListener data structure that includes LSM_SendNetMessage. */
   typedef struct
   {
      LSM_SendNetMessage reportMessageCb;
   }  LSM_MessageListener;

   /* --------------- For SUPL SI/standalone sessions ---------------- */
   /** @enum eLSM_EVENT
    *  @brief LSM events reported in LsmCallbackData */
   typedef enum
   {
      /** The running SUPL SI/standalone session needs to be closed by LSM application. */
      LSM_TO_CLOSE  = 0x00000001,
      /** LPL is busy with a SUPL MT or CP session */
      LSM_LPL_BUSY        = 0x00000002,
      /** LPL is transferred from busy to idle */
      LSM_LPL_IDLE        = 0x00000004,
      /** LPL got the ref location */
      LSM_LPL_REF_LOC     = 0x00000008,
      /** LPL got a fix with the requested QoS */
      LSM_LPL_LOC_W_QOS   = 0x00000010,
      /** LPL met the number of positioning request with QoS */
      LSM_LPL_MET_NUM_FIX = 0x00000020
   }  eLSM_EVENT;

   /** @struct LSM_GPSSatelliteInfo
    *  @brief SV information */
   typedef struct
   {
      /** GPS satellite id. The range is from 1 to 32. */
      LSM_UINT8 satelliteId;
      /** SV signal-to-noise ratio in dBHz. The range is from 0 to 90. */
      LSM_UINT8 signalStrength;
      /** SV azimuth in degress. The range is from 0 to 359. */
      LSM_UINT16 azimuth;
      /** SV elevation angle in degress. The range is from 0 to 90. */
      LSM_UINT8 elevationAngle;
   }  LSM_GPSSatelliteInfo;

   /** @struct LSM_LOC_REPORT
    *  @brief LSM reporting data. */
   typedef struct
   {
      /** struct ReportDataBitMap */
      struct
      {
         /** is positionID set? */
         LSM_UINT isPosIdSet: 1;
         /** is timeStamp set? */
         LSM_UINT isTimestampSet: 1;
         /** is heading set? */
         LSM_UINT isHeadingSet: 1;
         /** is Coordinate set */
         LSM_UINT isCoordinateSet: 1;
         /** is Velocity set? */
         LSM_UINT isVelocitySet: 1;
         /** is GPSInfo set? */
         LSM_UINT isGPSInfoSet: 1;
         /** is TTFFinfo set? */
         LSM_UINT isTTFFinfoSet: 1;
      }  ReportDataBitMap;
      /** number of position reported so far */
      LSM_UINT32 positionID;
      /** timestampe associated with the report */
      LSM_UINT64 timeStamp;
      /** navigation heading in degree. 0: true north; the angle increases towards east */
      LSM_UINT16 heading;
      /** @struct Coordinate
       *  @brief Position info. Only valid when eLSM_EVENT is LSM_LPL_LOC_W_QOS or LSM_LPL_REF_LOC */
      struct
      {
         /** LSM_TRUE: 3D; LSM_FALSE: 2D. */
         LSM_BOOL position3D;
         /** latitude in degree */
         LSM_DOUBLE latitude;
         /** longitude in degree */
         LSM_DOUBLE longitude;
         /** altitude in meter */
         LSM_FLOAT altitude;
         /** horizontal uncertainty error (in meter) represents the standard deviation of a 2-D error ellipse.*/
         LSM_UINT16 horUncert;
         /** vertical uncertainty error (in meter) represents the standard deviation of a 1-D error curve */
         LSM_UINT16 verticalUncert;
      }  Coordinate;
      /** @struct Velocity
       *  @brief velocity. info Only available when eLSM_EVENT is LSM_LPL_LOC_W_QOS or LSM_LPL_REF_LOC */
      struct
      {
         /** horizontal velocity in m/s */
         LSM_UINT16 horizontalVelocity;
         /** horizontal velocity uncertainty in m/s */
         LSM_UINT16 horizontalVelocityUncert;
         /** vertical velocity in m/s */
         LSM_CHAR verticalVelocity;
         /** vertical velocity uncertainty in m/s */
         LSM_UINT16 verticalVelocityUncert;
      }  Velocity;
      /** @struct GPSInfo
       *  @brief SV tracked or used */
      struct
      {
         /** The number of SVs tracked or the number of SV measurements. */
         LSM_UINT8 numSVs;
         /** Detailed SV info */
         LSM_GPSSatelliteInfo SVInfo[LSM_MAX_CHAN_NUM];
      }  GPSInfo;
      /** @struct TTFFinfo
       *  @brief time to first fix (TTFF) data for SUPL and standalone sessions */
      struct
      {
         /** suplAidingTime (in second) is the time from sending SUPL-START or receiving
             SUPL-INIT to receiving the last SUPL-POS message with aiding data. If there
             is no aiding, aidingTime = 0. */
         LSM_UINT16 suplAidingTime;
         /** ttff (in second) is the time from sending GPS_RESET to getting the first fix
             from GPS. If there is no fix from GPS, ttff = 0. */
         LSM_UINT16 ttff;
         /** suplMsaPositionTime (in second) is the time from sending SUPL-START or
             SUPL-POSINIT to receiving SUPL-END with position. If not in MSA position
             method, msaPositionTime = 0; */
         LSM_UINT16 suplMsaPositionTime;
         /** Here is how actualTTFF (in second) gets computed for SUPL sessions:
             1. In MSA Mode:
                The time difference between LPL getting SUPL-END with position or the first
                fix from GPS, whichever is earlier, and LPL sending GPS_REST or SUPL-START/
                SUPL-POSINIT, whichever is earlier.
             2. In MSB Mode:
                The time difference between LPL getting the first fix from GPS, and when LPL
                sending GPS_REST or SUPL-START/SUPL-POSINIT, whichever is earlier.
             3. In Standalone or Autonomous Mode:
                The time difference between when LPL getting the first fix from GPS, and when LPL
                sending GPS_RESET. */
         LSM_UINT16 suplActualTTFF;
      }  TTFFinfo;
   }  LSM_LOC_REPORT;

   /** @struct LsmCallbackData
    *  @brief LSM callback data structure */
   typedef struct
   {
      /** LSM events. Multiple events could be reported in the same callback call. */
      eLSM_EVENT eLsmEvent;
      /** Add more data members late if LSM caller needs more info */
      LSM_LOC_REPORT locReport;
   }  LsmCallbackData;

   /**
   * @typedef tSIRF_VOID (*LSM_StatusListener) (LsmCallbackData *pLsmCallbackData)
   * @brief This type of the function is registered in LSM_Init and will be called
            by LSM to notify the LSM_Start caller about the SUPL SI/standalone
            session status.
   * @param[out] pLsmCallbackData: a pointer to the LSM reporting data.
   */
   typedef tSIRF_VOID (*LSM_StatusListener) (LsmCallbackData *pLsmCallbackData);
   /* ---------------- For all types of LPL sessions ----------------- */
   /** @enum eLSM_SESSION_TYPE
    *  @brief LSM session type. */
   typedef enum
   {
      /** Standalone GPS session. */
      LSM_STANDALONE = 0,
      /** SUPL AGPS SET initiated (SI) session */
      LSM_SUPL_SI_SESSION,
      /** SUPL AGPS network initiated (NI) session */
      LSM_SUPL_NI_SESSION,
      /** 3G control plane AGPS session. */
      LSM_CP_RRC_SESSION,
      /** 2G control plane AGPS session. */
      LSM_CP_RRLP_SESSION, 
      /** EE Push Session */
      LSM_EE_PUSH_SESSION
   }  eLSM_SESSION_TYPE;

   /**
    * @enum eLSM_LOGGING_TYPE
    * @brief LPL brief/detail log types
    */
   typedef enum
   {
      /** No LPL logging */
      LSM_NO_LOGGING = 0,
      /** log LPL brief or/and detail log per session */
      LSM_SINGLE_SESSION_LOGGING = 1,
      /** concatenate LPL brief or/and detail logs */
      LSM_ALL_SESSION_LOGGING = 2,
   }  eLSM_LOGGING_TYPE;

   /** @enum eLSM_NETWORK_TYPE
    *  @brief network type */
   typedef enum
   {
      /** The device is in GMS network. */
      LSM_GSM,
      /** The device is in WCDMA network. */
      LSM_WCDMA,
      /** Invalid network type */
      LSM_INVALID_NT_TYPE = (~0)
   }  eLSM_NETWORK_TYPE;
   
   /** @enum eLSM_AIDING_TYPE
   *  @brief Aiding type */
   typedef enum
   {
      LSM_NO_AIDING                   = 0x00,
      /** CLM Aiding */
      LSM_LOCAL_AIDING                = 0x01,
      /** SUPL Server Aiding */
      LSM_NETWORK_AIDING              = 0x02,
      /** Local Aiding preferred */
      LSM_LOCAL_AIDING_PREFERED       = 0x03 /* Not in use for now */
   }  eLSM_AIDING_TYPE;

#ifdef LPL_SIF
   /** @struct LSM_agpsAidingCapabilities
   *  @brief Aiding Capabilities CGEE/SGEE/NAVBIT */
   typedef struct
   {
      /** 0: no NAV Bit Aiding; 1: NAV Bit aiding enable */
      LSM_UINT  LPL_NAV_BITS_AIDING  : 1;
      /** 0: SGEE Disable; 1: CGEE Enable */
      LSM_UINT  LPL_SGEE_AIDING      : 1;
      /** 0: CGEE Disable; 1: CGEE Enable */
      LSM_UINT  LPL_CGEE_AIDING      : 1;

   }  LSM_agpsAidingCapabilities;
#endif /*LPL_SIF*/

   /** @enum eLSM_PREFERRED_LOC_METHOD
    *  @brief Preferred location methods. */
   typedef enum
   {
      /** MSB location method is preferred in SUPL session */
      LSM_PREFERRED_LOC_METHOD_MSB,
      /** MSA location method is preferred in SUPL session */
      LSM_PREFERRED_LOC_METHOD_MSA,
      /** no preference for location method. */
      LSM_PREFERRED_LOC_METHOD_NONE
   }  eLSM_PREFERRED_LOC_METHOD;

   /** @enum eLSM_SET_IDENTIFICATION
    *  @brief SET ID types. */
   typedef enum
   {
      /** Indicates that IMSI is to be used */
      LSM_IMSI,
      /** Indicates that MSISDN is to be used */
      LSM_MSISDN,
      /** Invalid SET ID type */
      LSM_INVALID_SETID_TYPE
   }  eLSM_SET_IDENTIFICATION;

   /** @enum eLSM_RESET_TYPE
    *  @brief GPS reset types. */
   typedef enum
   {
      /** Send Factory Reset command GPS when starting a new session. */
      LSM_RESET_TYPE_FACTORY_RESET,
      /** Send Cold Reset command GPS when starting a new session. */
      LSM_RESET_TYPE_COLD_RESET,
      /** Send Hot Reset command GPS when starting a new session. */
      LSM_RESET_TYPE_HOT_RESET,
      /** Send Warm Reset command GPS when starting a new session. */
      LSM_RESET_TYPE_WARM_RESET
   }  eLSM_RESET_TYPE;

   /** @enum eLSM_TIME_TRANSFER
    *  @brief Time ading transfer types. */
   typedef enum
   {
      /** Indicates that Time Transfer is Not Available */
      LSM_TIME_TRANSFER_NOT_AVAILABLE,
      /** Indicates that Coarse Time Transfer is being used */
      LSM_TIME_TRANSFER_COARSE,
      /** Indicates that Precise Time Transfer is being used */
      LSM_TIME_TRANSFER_PRECISE
   }  eLSM_TIME_TRANSFER;

   /** @enum eLSM_FREQUENCY_TRANSFER
    *  @brief Frequency ading transfer types. */
   typedef enum
   {
      /** Frequency Transfer is Not Available on the device. */
      LSM_FREQUENCY_TRANSFER_NOT_AVAILABLE,
      /** Frequency Transfer is available on the device and the Non Counter Method is being used. */
      LSM_FREQUENCY_TRANSFER_METHOD_NON_COUNTER,
      /** Frequency Transfer is available on the device and the Counter Method is being used. */
      LSM_FREQUENCY_TRANSFER_METHOD_COUNTER
   }  eLSM_FREQUENCY_TRANSFER;

   /** @enum eLSM_RTC_AVAILABILITY
    *  @brief RTC source type. */
   typedef enum
   {
      /** RTC is not available to the GPS Receiver on the device. */
      LSM_RTC_NOT_AVAILABLE,
      /** Internal RTC is available on the device. */
      LSM_RTC_INTERNAL,
      /** External RTC is available on the device. */
      LSM_RTC_EXTERNAL
   }  eLSM_RTC_AVAILABILITY;

   /** @enum eLSM_LOCATION_METHOD
    *  @brief Location method types. */
   typedef enum
   {
      /** The SUPL Autonomous Location Method is to be used */
      LSM_LOCATION_METHOD_AUTONOMOUS,
      /** The MSB Location Method is to be used. */
      LSM_LOCATION_METHOD_MS_BASED,
      /** The MSA Location Method is to be used. */
      LSM_LOCATION_METHOD_MS_ASSISTED,
      /** MSAB Location method is selected. The final Location method
          will be determined by the handshaking between SET and SLC. */
      LSM_LOCATION_METHOD_COMBO,
      /** The Standalone Location Method is to be used. */
      LSM_LOCATION_METHOD_STANDALONE,
      /** The Enhanced Cell ID Location Method is to be used. */
      LSM_LOCATION_METHOD_ECID
   }  eLSM_LOCATION_METHOD;

   /** @struct LSM_LOGFiles
    *  @brief LPL log file names. */
   typedef struct
   {
      /** LPL brief log file name */
      LSM_CHAR* briefLogFileName;
      /** LPL detailed log file name */
      LSM_CHAR* detailedLogFileName;
      /** AGPS Session log file name */
      LSM_CHAR* agpsLogFileName;
      /** LPL cached position log file name */
      LSM_CHAR* previousLocationFileName;
      /** LPL SSB/STAT log file name */
      LSM_CHAR* slcLogFileName;
   }  LSM_LOGFiles;

   /** @struct LSM_loggingInformation
    *  @brief LPL logging info. */
   typedef struct
   {
      /** LPL logging type for the running session. */
      eLSM_LOGGING_TYPE loggingtype;
      /** LPL logging file names for the running session. */
      LSM_LOGFiles logfiles;
   }  LSM_loggingInformation;

   /** @struct LSM_netGSMCellID
    *  @brief GMS cell info. */
   typedef struct
   {
      /** Mobile contry code. */
      LSM_UINT16 mcc;
      /** Mobile network code. */
      LSM_UINT16 mnc;
      /** Local area code. */
      LSM_UINT16 lac;
      /** Cell ID. */
      LSM_UINT16 cid;
   }  LSM_netGSMCellID;

   /** @struct LSM_netWCDMACellID
    *  @brief WCDMA cell info. */
   typedef struct
   {
      /** Mobile contry code. */
      LSM_UINT16 mcc;
      /** Mobile network code. */
      LSM_UINT16 mnc;
      /** Unique cell ID. */
      LSM_UINT32 ucid;
   }  LSM_netWCDMACellID;

   /** @struct LSM_netCellID
    *  @brief 2G/3G network cell ID data structure. */
   typedef struct
   {
      /** Networt type. */
      eLSM_NETWORK_TYPE eNetworkType;
      /**  @brief union structure for network cell info. */
      union
      {
         /** GMS cell. */
         LSM_netGSMCellID gsm_cellid;
         /** WCDMA cell. */
         LSM_netWCDMACellID wcdma_cellid;
      }  m;
   }  LSM_netCellID;

   /** @struct SETID_Info
    *  @brief It MUST be configured for SUPL sessions. */
   typedef struct
   {
      /* SET ID */
      eLSM_SET_IDENTIFICATION SETidType;
      /** SET ID string. */
      LSM_UINT8 SETidValue[SET_ID_LENGTH];
   }  SETID_Info;

   /** @struct LSM_SlcICDSetting
    *  @brief SiRFLoc (SLC) firmware ICD version numbers. */
   typedef struct
   {
      /** AI3 ICD version number. */
      LSM_UINT8 AI3ICD;
      /** F ICD version number. */
      LSM_UINT8 FICD;
   }  LSM_SlcICDSetting;

   /** @struct LSM_NetworkIFCfg
    *  @brief network interface info data structure. */
   typedef struct
   {
      /** SSL/TSL over TCP/IP network connection. LSM_TRUE: secure connection; LPL_FALSE: non-secure connection. */
      LSM_BOOL bSecure;
      /** @struct SuplServerInfo
       *  @brief SUPL server IP address and port info data structure. */
      struct
      {
         /** SUPL server IP address. */
         LSM_CHAR ipAddr[LSM_IP_ADDR_LENGTH];
         /** SUPL server port number. Default is 7275. */
         LSM_UINT16 port;
      }  SuplServerInfo;
      /** @struct BindAddress
       *  @brief The binding IP address data structure. */
      struct
      {
         /** To bind an network interface to an IP address and a port. LSM_TRUE: binding; LSM_FALSE: no binding. */
         LSM_BOOL bBind;
         /** Bound IP address. */
         LSM_CHAR ipAddr[LSM_IP_ADDR_LENGTH];
         /** Bound port. */
         LSM_UINT16 port;
      }  BindAddress;
   }  LSM_NetworkIFCfg;

   /** @struct LSM_ApproxLoc
    *  @brief Approximate position data structure. */
   typedef struct
   {
      /** latitude in degree. */
      LSM_DOUBLE lat;
      /** longitude in degree. */
      LSM_DOUBLE lon;
      /** altitude in meter. */
      LSM_SINT16 alt;
      /** vertical error in meter. */
      LSM_SINT16 verErr;
      /** horizontal error in meter. */
      LSM_UINT32 horErr;
   }  LSM_ApproxLoc;

   /**
    * @struct LSM_GSM_Measurements
    * Structure definition for the GSM measurements.
    * This structure should be filled with the measurements.
    */
   typedef struct
   {
      LSM_UINT32 firstMeasurementTimeStamp;
      LSM_UINT8  numMeasurements;
      struct _measurements
      {
         LSM_UINT16 deltaTime;
         LSM_UINT16 mcc;
         LSM_UINT16 mnc;
         LSM_UINT16 lac;
         LSM_UINT16 cid;
         LSM_UINT8  ta;
         LSM_UINT8  numCells;
         struct _cellInfo
         {
            LSM_UINT16 arfcn;
            LSM_UINT8  bsic;
            LSM_UINT8  rxlev;
         }  cellInfo[LSM_MAX_NI_CELLS];
      }   measurements [LSM_MAX_NI_MSRMENTS];
   } LSM_GSM_Measurements;

   /**
    * @struct LSM_FrequencyInfoFDD
    * Structure definition for the WCDMA Frequency information for FDD.
    * This structure should be filled with the FDD frequency information.
    */
   typedef struct
   {
      LSM_UINT8  uarfcn_ULPresent;
      LSM_UINT16 uarfcn_UL;
      LSM_UINT16 uarfcn_DL;
   } LSM_FrequencyInfoFDD;

   /**
    * @struct LSM_FrequencyInfoTDD
    * Structure definition for the WCDMA Frequency information for TDD.
    * This structure should be filled with the TDD frequency information.
    */
   typedef struct
   {
      LSM_UINT16 uarfcn_Nt;
   } LSM_FrequencyInfoTDD;

   /**
    * @enum LSM_FrequencyInfoType
    * Enum definition for supported frequency types.
    */
   typedef enum
   {
      LSM_FREQUENCY_INFO_FDD = 1,
      LSM_FREQUENCY_INFO_TDD = 2
   } LSM_FrequencyInfoType;

   /**
    * @struct LSM_FrequencyInfo
    * Structure definition for the WCDMA Frequency information.
    * This structure should be filled with the frequency information.
    */
   typedef struct
   {
      LSM_FrequencyInfoType type;
      union
      {
         LSM_FrequencyInfoFDD fdd;
         LSM_FrequencyInfoTDD tdd;
      } modeSpecificInfo;
   } LSM_FrequencyInfo;

   /**
    * @enum LSM_FrequencyInfoType
    * Enum definition for supported types of cell measurements.
    */
   typedef enum
   {
      LSM_CELL_MEASURED_RESULTS_FDD = 1,
      LSM_CELL_MEASURED_RESULTS_TDD = 2
   } LSM_CellMeasuredResultsType;

   /**
    * @struct LSM_CellMeasuredResults_fdd
    * Structure definition for the WCDMA Cell Measurements for FDD.
    * This structure should be filled with the cell measurements.
    */
   typedef struct
   {
      LSM_UINT8  cpich_Ec_N0Present;
      LSM_UINT8  cpich_RSCPPresent;
      LSM_UINT8  pathlossPresent;
      LSM_UINT16 primaryScramblingCode;
      LSM_UINT8  cpich_Ec_N0;
      LSM_UINT8  cpich_RSCP;
      LSM_UINT8  pathloss;
   } LSM_CellMeasuredResults_fdd;

   /**
    * @struct LSM_TimeslotISCP
    * Structure definition for the WCDMA time slot information for TDD.
    * This structure should be filled with the time slot information.
    */
   typedef struct
   {
      LSM_UINT32 n;
      LSM_UINT8  elem[LSM_MAX_TIME_SLOT];
   } LSM_TimeslotISCP;

   /**
    * @struct LSM_CellMeasuredResults_tdd
    * Structure definition for the WCDMA Cell Measurements for TDD.
    * This structure should be filled with the cell measurements.
    */
   typedef struct
   {
      LSM_UINT8  proposedTGSNPresent;
      LSM_UINT8  primaryCCPCH_RSCPPresent;
      LSM_UINT8  pathlossPresent;
      LSM_UINT8  timeslotISCP_ListPresent;
      LSM_UINT8  cellParametersID;
      LSM_UINT8  proposedTGSN;
      LSM_UINT8  primaryCCPCH_RSCP;
      LSM_UINT8  pathloss;
      LSM_TimeslotISCP timeslotISCP;
   } LSM_CellMeasuredResults_tdd;

   /**
    * @struct LSM_CellMeasuredResults
    * Structure definition for the WCDMA Cell Measurements.
    * This structure should be filled with the cell measurements.
    */
   typedef struct
   {
      LSM_UINT8  cellIdentityPresent;
      LSM_UINT32 cellIdentity;
      LSM_CellMeasuredResultsType type;
      union
      {
         LSM_CellMeasuredResults_fdd fdd;
         LSM_CellMeasuredResults_tdd tdd;
      } modeSpecificInfo;
   } LSM_CellMeasuredResults;

   /**
    * @struct LSM_WCDMA_MeasuredResults
    * Structure definition for the WCDMA Cell Measured Results.
    * This structure should be filled with the cell measurements.
    */
   typedef struct
   {
      LSM_UINT8  frequencyInfoPresent;
      LSM_UINT8  utra_CarrierRSSIPresent;
      LSM_UINT8  cellMeasuredResultsPresent;
      LSM_FrequencyInfo frequencyInfo;
      LSM_UINT8 utra_CarrierRSSI;
      LSM_CellMeasuredResults cellMeasuredResults[LSM_MAX_CELL_MEAS];
   } LSM_WCDMA_MeasuredResults;

   /**
    * @struct LSM_WCDMA_Measurements
    * Structure definition for the WCDMA measurements.
    * This structure should be filled with the measurements.
    */
   typedef struct
   {
      LSM_UINT8  frequencyInfoPresent;
      LSM_UINT8  primaryScramblingCodePresent;
      LSM_UINT8  measuredResultsPresent;
      LSM_UINT16 mcc;
      LSM_UINT16 mnc;
      LSM_UINT32 uc;
      LSM_FrequencyInfo frequencyInfo;
      LSM_UINT16 primaryScramblingCode;
      LSM_WCDMA_MeasuredResults measuredResults[LSM_MAX_FREQ];
   } LSM_WCDMA_Measurements;

   /**
    * @struct LSM_Network_Measurements
    * This structure incorporates all possible types of Network measurements.
    */
   typedef struct
   {
      eLSM_NETWORK_TYPE  networkType;

      union
      {
         LSM_GSM_Measurements   gsmMeasurements;
         LSM_WCDMA_Measurements wcdmaMeasurements;
      } m;

   } LSM_Network_Measurements;

   /** @typedef LSM_NWMeasListener
    *  @brief Network Measurements callback function type.
    *  @param[out] mesurements: Network Measurements. */
   typedef tSIRF_VOID (*LSM_NWMeasListener) (LSM_Network_Measurements* mesurements);

   /** @typedef LSM_NMEA_Listener
    *  @brief NMEA callback function type.
    *  @param[out] type: NMEA message type;
    *  @param[out] message: NMEA message;
    *  @param[out] msgLen: NMEA message length. */
   typedef tSIRF_VOID (*LSM_NMEA_Listener)   (LSM_UINT8 type, LSM_UINT8* message, LSM_UINT32 msgLen);

   /** @typedef LSM_SSBListener
    *  @brief SSB/STAT callback function type.
    *  @param[out] message: SSB/STAT message;
    *  @param[out] msgLen: SSB/STAT message length. */
   typedef tSIRF_VOID (*LSM_SSBListener)     (LSM_UINT8* message, LSM_UINT32 msgLen);

   /** @typedef LSM_SerialListener
    *  @brief SLC message callback function type.
    *  @param[out] message: SLC messages.
    *  @param[out] msgLen: SLC message length. */
   typedef tSIRF_VOID (*LSM_SerialListener)  (LSM_UINT8* message, LSM_UINT32 msgLen);

   /** @typedef LSM_DebugListener
    *  @brief LPL brief debug callback function type.
    *  @param[out] message: LPL brief debug message;
    *  @param[out] msgLen: LPL brief debug message length. */
   typedef tSIRF_VOID (*LSM_DebugListener)   (LSM_CHAR* pMsg);

   typedef tSIRF_VOID (*LSM_SuplInitListener)(LSM_AGPS_SUPL_INIT* pSuplInitData,
                        LSM_AGPS_UserResponse* pUserResponse,
                        tSIRF_VOID* pUserData);

   /** @struct LSM_SETcapabilities
    *  @brief SET positioning capabiloity. */
   typedef struct
   {
      /** The MSB location method is supported by the SET. */
      LSM_UINT8 msBased;
      /** The MSA location method is supported by the SET. */
      LSM_UINT8 msAssisted;
      /** The SUPL autonomous location method is supported by the SET. */
      LSM_UINT8 autonomous;
      /** The extended cell ID location method is supported by the SET. */
      LSM_UINT8 eCID;
   }  LSM_SETcapabilities;

   /** @enum eLSM_GPSHW_UNIT
    *  @brief GPS types. */
   typedef enum
   {
      /** SiRF LT/LP types of GPS. */
      LSM_EUART_PORT,
      /** SiRF GPS chip on other device through bluetooth connection. */
      LSM_EBLUETOOTH_PORT,
      /** SiRF GPS chip on other device through TCP/IP connection. */
      LSM_ETCPIP,
      /** SIRF 3tw types of GPS. */
      LSM_ETRACKER,
   }  eLSM_GPSHW_UNIT;

   /** @struct LSM_serialConfig
    *  @brief GPS serial port info. */
   typedef struct
   {
      /** 0: the app opens the serial port; 1: LPL opens it. */
      LSM_BOOL needToOpenComport;
      /** Serial port name. */
      LSM_CHAR m_SerialPort[20];
      /** Serial port baud rate. */
      LSM_SINT32 m_BaudRate;

      /** 1: LPL opens the virtual comport; 0: The application opens the virtual comport. */
      LSM_BOOL needToOpenVirtualPort;
      /** The virtual comport name. */
      LSM_CHAR m_NMEASerialPort[20];
      /** 1: LPL outputs NMEA to the virtual comport; 0: no NMEA to the virtual comport. */
      LSM_BOOL bUseNMEA;                          /*is virtual port used? */
   }  LSM_serialConfig;

   /** @struct LSM_bluetoothConfig
    *  @brief Bluetooth (BT) port configuration info. */
   typedef struct
   {
      /** BT address in hexadecimal big-endian format string. */
      LSM_UINT16 m_BTAddress[50];
      /** BT port number. */
      LSM_UINT16 BTPort;
   }  LSM_bluetoothConfig;

   /** @struct LSM_TCPIPConfig
    *  @brief TCP/IP interface configuration info. */
   typedef struct
   {
      /** The server IP address. */
      LSM_CHAR serverAddress[LSM_MAX_FQDN_LEN];
      /** The server port number. */
      LSM_UINT16 serverPort;
      /** Bound IP address. */
      LSM_CHAR bindServerIP[LSM_MAX_FQDN_LEN];
      /** Bound port number. */
      LSM_UINT16 bindServerPort;
      /** Binding or not. */
      LSM_BOOL isBind;
   }  LSM_TCPIPConfig;

   /** @struct LSM_trackerCfg
    *  @brief 3tw tracker info. */
   typedef struct
   {
      /** Tracker serial port name. */
      LSM_CHAR m_SerialPort[20];
      /** Tracker serial port baud rate. */
      LSM_SINT32 m_BaudRate;
      /** Tracker start mode. */
      LSM_UINT32 gpsStartMode;
      /** Tracker clock offset. */
      LSM_UINT32 gpsClockOffset;
   }  LSM_trackerCfg;

   /** @struct LSM_GPS_UNIT
    *  @brief SiRF GPS info. */
   typedef struct
   {
      /** SiRF GPS type. */
      eLSM_GPSHW_UNIT configType;
      /** @union DeviceConfig
       *  @brief union structure for GPS device info. */
      union
      {
         /** If configType = LSM_EUART_PORT, LSM uses serialConfig. */
         LSM_serialConfig serialConfig;
         /** If configType = LSM_EBLUETOOTH_PORT, LSM uses btConfig. */
         LSM_bluetoothConfig btConfig;
         /** If configType = LSM_ETCPIP, LSM uses tcpipConfig. */
         LSM_TCPIPConfig tcpipConfig;
         /** If configType = LSM_ETRACKER, LSM uses trackerCfg. */
         LSM_trackerCfg trackerCfg;
      }  DeviceConfig;
   }  LSM_GPS_UNIT;

   /** @struct LSM_memConfig
    *  @brief LPL heap memory info. */
   typedef struct
   {
      /** The size of the heap memory for LPL. */
      LSM_SINT32 memSize;
      /** The pointer to the allocated memory location for LPL heap memory. NULL if the default location is used. */
      LSM_CHAR* pMem;
   }  LSM_memConfig;

   /** @enum LSM_CGPSCCtrl
    *  @brief Disable or enable Cooperative GPS power save and control (CGPSC) feature. */
   typedef enum
   {
      /** Diable CGPSC */
      LSM_DISABLE_CGPSC,
      /** Eanble CGPSC. */
      LSM_ENABLE_CGPSC
   }  LSM_CGPSCCtrl;

   /** @enum eLSM_GPS_PWR_CTRL_CAP
    *  @brief Indicate CGPSC capability. */
   typedef enum
   {
      /** LPL cannot control GPS power. */
      LSM_NO_GPS_POWER_CONTROL,
      /** LPL can control the GPS power. */
      LSM_HAS_GPS_POWER_CONTROL
   }  eLSM_GPS_PWR_CTRL_CAP;

   /** @struct LSM_powerControl
    *  @brief The GPS power control info for SiRF LP/LT GPS chip, on which the chip hibernation and wake-up
              are through external GPIO. */
   typedef struct
   {
      /** CGPSC capability info. */
      eLSM_GPS_PWR_CTRL_CAP gpsPwrCtrlCap;
      /** Disable or enable CGPSC. */
      LSM_CGPSCCtrl cgpsCtrl;
   }  LSM_powerControl;

   /** @struct LSM_InitData
    *  @brief LSM initialization data. */
   typedef struct
   {
      /** GPS type. */
      LSM_GPS_UNIT gpsDevice;
      /** LPL heap memory info. */
      LSM_memConfig lpl_mem;
      /** LPL power control info. */
      LSM_powerControl lplPowerControl;
   }  LSM_InitData;

   /** @struct LSM_commonCfgData
    *  @brief LSM common configuration info. */
   typedef struct
   {
      /** MUST be set. For LPL GM_Init function call. */
      LSM_InitData lsmInitData;
      /** MUST be set. LSM status callback which combined the LPL progCallback and locCallback. */
      LSM_StatusListener lplStatusCallback;
      /** @struct CommonCfgBitMap
       *  @brief It's optional to configure the data entries corresponding to the following bit map. */
      struct
      {
         /** 0: no LPL logging; 1: LPL logging is enable and the log file names are needed in logInfo. */
         LSM_UINT isLogInfoSet: 1;
         /** 0: no SUPL_INIT callback; 1: LSM will call suplInitCallback is it set. (not enabled yet) */
         LSM_UINT isSuplInitCallbackSet: 1;
         /** 0: use LPL default capability; 1: use capability info in PosMethodCapabilities. */
         LSM_UINT isCapSet: 1;
         /** 0: use LPL default SET ID; 1: use SET ID info in SETidInfo. */
         LSM_UINT isSETidInfoSet: 1;
         /** 0: use LPL default reset type if isResetCmdRcvd = 0;
             1: use reset type in gpsResetType if isResetCmdRcvd = 0.
             If isResetCmdRcvd = 1, always do "COLD START" or "FACTORY default" if it's available. */
         LSM_UINT isResetTypeSet: 1;
         /** 0: use LPL default; 1: use time data in timeTransferParam. */
         LSM_UINT isTimeTransferSet: 1;
         /** 0: use LPL default; 1: use RTC data in FreqParam. */
         LSM_UINT isFreqAidingSet: 1;
         /** 0: use LPL default; use RTC data in RTCType. */
         LSM_UINT isRTCTypeSet: 1;
         /** 0: no NW measurement will be available, LPL uses cell ID info;
             1: LPL will call getNwMeasCallback to get new network measurements. */
         LSM_UINT isNWMeasCallbackSet: 1;
         /** 0: NMEA is disabled; 1: NMEA is enabled. NMEA should be always enabled even it's optional here. */
         LSM_UINT isNMEAenabled: 1;
         /** 0: disable SSB data; 1: enable SSB data. */
         LSM_UINT isSSBenabled: 1;
         /** 0: disable SSB STAT data; 1: enable SSB STAT data. */
         LSM_UINT isSTATenable: 1;
         /** 0: disable LPL debug callback; 1: enable LPL debug callback. */
         LSM_UINT isDbgListenerSet: 1;
         /** 0: disable SLC message callback; 1: enable SLC message callback. */
         LSM_UINT isSerialListenerSet: 1;
         /** 0: LSM will use the network parameters in commonNetworkCfg;
             1: LSM will use the network parameters in commonNetworkCfg if isSessionNTWKParamSet is not set. */
         LSM_UINT isCommonNTWKParamSet: 1;
         /** 0: use default in LPL; 1: UT timers use the parameters in UTTimerParam. */
         LSM_UINT isUTTimerCfgSet : 1;
         /** 0: auto-detect SLC AI3 and F ICD version numbers. */
         LSM_UINT isSLCICDSet : 1;
         /** 0: LPL default re-aiding data will be used. 1: re-aiding info in reAidingParam will be used. */
         LSM_UINT isReAidingParamSet : 1;
         /** 0: no APM; 1: use APM parameters in apmParam. */
         LSM_UINT isApmParamSet: 1;
         /** 0: no Aiding Type Set, Use Default= Network Aiding; 1: use AidingType param in Aiding Type */
         LSM_UINT isAidingTypeParamSet: 1;
         /** 0: no event except LSM_CLOSE event is sent to apps. 1: LSM is communicating with the demo 
         application and can display fixes and events on the application through lsmCallback.  */
         LSM_UINT reportEvents2App4NIsessions: 1;
#ifdef LPL_SIF
         /** 0: no SIF; 1: SIF is available. (not implemented yet) */
         LSM_UINT isCLMCfgDataSet: 1;
#endif /*LPL_SIF*/
      }  CommonCfgBitMap;
      /** It's optional to configure logInfo. When needed, isLogInfoSet needs to be set. */
      LSM_loggingInformation logInfo;
      /** @struct SuplInitCallbackInfo
       *  @brief LPL calls suplInitCallback when notification is present in SUPL NI message. */
      struct
      {
         /** Callback for the decoded SUPL_INIT message. */
         LSM_SuplInitListener suplInitCallback;
         /** the pointer to the data structure by user */
         tSIRF_VOID *pUserData;
      }  SuplInitCallbackInfo;
      /** @struct PosMethodCapabilities
       *  @brief It's optional to configure PosMethodCapabilities. */
      struct
      {
         eLSM_PREFERRED_LOC_METHOD prefLocMethod;/* preferred location method */
         LSM_SETcapabilities cap;
      }  PosMethodCapabilities;
      /** SET (SUPL Enabled Terminal) ID - SETidInfo MUST be configured for SUPL sessions. */
      SETID_Info SETidInfo;
      /** GPS reset type. */
      eLSM_RESET_TYPE gpsResetType;
      /** Aiding Type */
      eLSM_AIDING_TYPE aidingType;
      /** @struct timeTransferParam
       *  @brief Only the precise time aiding is available, timeTransferParam needs to be configured. */
      struct
      {
         /** Time tranfer type */
         eLSM_TIME_TRANSFER timeTransferType;
         /** Time transfer precision. */
         LSM_SINT32 timeTransferPrecision;
         /** Time transfer skew. */
         LSM_SINT32 timeTransferSkew;
      }  timeTransferParam;
      /** @struct FreqParam
       *  @brief Only when frequency aiding is avaiable, FreqParam needs to be configured. */
      struct
      {
         /** Frequency transfer method */
         eLSM_FREQUENCY_TRANSFER freqXferMethod;
         /** Transferred frequency norminal value. */
         LSM_DOUBLE frequencyNorminalValue;
      }  FreqParam;
      /** RTC availability. */
      eLSM_RTC_AVAILABILITY RTCType;
      /** Network measurement listener. */
      LSM_NWMeasListener getNwMeasCallback;
      /* GPS NMEA data callback function. */
      LSM_NMEA_Listener nmeaCallback;
      /** SSB/STAT callback function - for debug only. */
      LSM_SSBListener ssbCallback;
      /** SLC message callback functions - for debug only. */
      LSM_SerialListener serialCallback;
      /** LPL brief debug message callback function - for debug only. */
      LSM_DebugListener dbgCallback;
      /** It's mandatory to configure commonNetworkCfg for SUPL sessions. */
      LSM_NetworkIFCfg commonNetworkCfg;
      /** @struct UTTimerParam
          @brief It's optional to configure the timeout values for SUPL UT timers. */
      struct
      {
         /** UT timer1. */
         LSM_UINT8 t1;
         /** UT timer2. */
         LSM_UINT8 t2;
         /** UT timer3. */
         LSM_UINT8 t3;
      }  UTTimerParam;
      LSM_SlcICDSetting ICDFromUser;
      /** @struct reAidingParam
       *  @brief It's optional to configure these parameters for smart reaiding feature. */
      struct
      {
         LSM_UINT8 SVLimit;
         LSM_UINT16 reAidingTimeIntervalLimit;
      }  reAidingParam;
      /** @struct apmParam
       *  @brief It's optional to configure apmParam. */
      struct
      {
         /** enableApm - LSM_TRUE: enable APM; LSM_FALSE: disable APM */
         LSM_BOOL enableApm;
         /** Power duty cycle info. */
         LSM_UINT8 powerDutyCycle;
         /** Time duty cycle info. */
         LSM_UINT8 timeDutyPriority;
      }  apmParam;
#ifdef LPL_SIF
      /** @struct clmCfgData
       *  @brief It's optional to configure clmCfgData.  */
      struct
      {
         /** The device aiding capability. */
         LSM_agpsAidingCapabilities aidingCap;
         /* Update SGEE file */
         LSM_BOOL updateEEFile;
      }   clmCfgData;
#endif /*LPL_SIF*/
   }  LSM_commonCfgData;

   /** @struct LSM_QoS
       @brief To specify the desired quality of service. */
   typedef struct
   {
      /** The requested horizontal error in meters. */
      LSM_UINT32 horizontalAccuracy;
      /** The requested vertical error in meters. */
      LSM_UINT32 verticalAccuracy;
      /** The requested response time for the first position fix in seconds. */
      LSM_UINT8 maxResponseTime;
      /** The maximum limit of the age of the cached position. */
      LSM_UINT16 maxLocationAge;
   }  LSM_QoS;

   /** @struct LSM_SIsessionCfgData
    *  @brief LPL SUPL SI and standalone session configuration settings. */
   typedef struct
   {
      /** @struct SessionCfgBitMap
       *  @brief If a bit is set in SessionCfgBitMap, LSM will use the corresponding data to call an LPL API. */
      struct
      {
         /** 0: approximate location is not available; 1: approxLoc has the approximate location. */
         LSM_UINT isApproxLocInfoSet: 1;
         /** 0: GMS cell info is not available; 1: gsmInfo has GMS cell info. */
         LSM_UINT isGSMInfoSet: 1;
         /** 0: WCDMA cell info is not available; 1: wcdmaInfo has WCDMA cell info. */
         LSM_UINT isWCDMAInfoSet: 1;
         /** 0: LSM will use the network parameters in commonNetworkCfg;
             1: LSM will use the network parameters in sessionNetworkCfg. */
         LSM_UINT isSessionNTWKParamSet: 1;
         /** 0: Use default GPS reset type; 1: gpsResetType has GPS reset types. */
         LSM_UINT isResetTypeSet: 1;
         /** 0: Use default SUPL/RRLP; 1: Not supported now. */
         LSM_UINT isAssistProtocolSet: 1;
         /** 0: no Aiding Type Set, Use Default= Network Aiding; 1: use AidingType param in Aiding Type */
         LSM_UINT isAidingTypeParamSet: 1;
         /** 0: not set, keep value from Init Configuration, 1: set, use prefLocationMethod value. */
         LSM_UINT isPrefLocationMethodSet: 1;
      }  SessionCfgBitMap;
      /** The approximate location provided by the AGPS application/driver/service/deamon. */
      LSM_ApproxLoc approxLoc;
      /** It's mandatory to configure gsmInfo for SUPL over 2G network. */
      LSM_netGSMCellID gsmInfo;
      /** It's mandatory to configure wcdmaInfo for SUPL over 3G network. */
      LSM_netWCDMACellID wcdmaInfo;
      /** It's optional to include the network configuration data in LSM_SIsessionCfgData. */
      LSM_NetworkIFCfg sessionNetworkCfg;
      /** If isAssistProtocolSet = 1, assistProtocol needs to be configured. */
      eLSM_OTA_TYPE assistProtocol;
      /** LSM_FALSE: no QoS in SUPL_START; LSM_TRUE: QoS will be sent in SUPL_START. */
      LSM_BOOL isQoSInSUPLStart;
      /** If isResetTypeSet = 1, gpsResetType needs to be configured. */
      eLSM_RESET_TYPE gpsResetType;
      /** If isQoSInSUPLStart = LSM_TRUE, QoS needs to be configured. */
      LSM_QoS QoS;
      /** Number of desired fixes. 0: infinite number of fixes. */
      LSM_UINT8 numFixes;
      /** Fix interval in second. */
      LSM_UINT8 timeInterval;
      /** Aiding Type */
      eLSM_AIDING_TYPE aidingType;
      /** 0: no Network connection; 1: Network connection available */
      LSM_BOOL isNWConnectionAvailable;
      /** Preferred location method, it is used to override the default value set in Init Configuration. */
      eLSM_PREFERRED_LOC_METHOD prefLocationMethod;

   }  LSM_SIsessionCfgData;

   /** @struct LSM_OTA_MSG
    *  @brief OTA message data structure. */
   typedef struct
   {
      /** OTA message body. */
      LSM_UINT8* pbBody;
      /** OTA mesage length. */
      LSM_UINT16 cbBodyLength;
   }  LSM_OTA_MSG;

   /** @struct LSM_CPCfgData
    *  @brief OTA message data structure for control plane and SUPL NI session. */
   typedef struct
   {
      /** OTA message type. */
      eLSM_OTA_TYPE ota_type;
      /** RRC state. */
      eLSM_RRC_STATE rrc_state;
      /** OTA message priority. */
      eLSM_SESSION_PRIORITY sessionPriority;
      /** OTA message. */
      LSM_OTA_MSG otaMsg;
   }  LSM_CPCfgData;

   /** @struct LSM_SessionID
    *  @brief LSM session ID. */
   typedef struct
   {
      /** id starts from 1. */
      LSM_UINT32 id;
      /** session type. */
      LSM_UINT8 type;
   }  LSM_SessionID;

   /*@}*/


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif  /* _LSM_TYPES_H_ */

/**
 * @}
 * End of file.
 */
