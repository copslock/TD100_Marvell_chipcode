/*
 *                   SiRF Technology, Inc. GPS Software
 *
 *    Copyright (c) 2005-2009 by SiRF Technology, Inc. All rights reserved.
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
 *  FILENAME:       LSM_SUPLInit.h
 *
 *  DESCRIPTION:    This header file includes all types and data structures for LSM SUPL INIT callback.
 *
 *  NOTES:          Only single session is supported for now.
 *
 ***************************************************************************
 *
 *  Keywords for Perforce. Do not modify.
 *
 *  $File: //customs/customer/Samsung_Volga/SDK/sirf/SiRFLPL/include/LSM_SUPLInit.h $
 *
 *  $DateTime: 2010/06/21 13:32:13 $
 *
 *  $Revision: #1 $
 *
 ***************************************************************************
*/

/** 
 * @file
 * @brief This header file includes all types and data structures for LSM SUPL INIT callback.
 * @addtogroup LSM
 * @{
 */

/** @struct LSM_AGPS_QoP
 *  @brief The structure contains the QoP for SUPL_INIT messages. */
typedef struct 
{
    /** This indicates what information is present in the structure */
    struct 
    {
        /** This indicates that Vertical Accuracy is present */
        unsigned veraccPresent : 1;
        /** This indicates that Max Loc Age is present */
        unsigned maxLocAgePresent : 1;
        /** This indicates that Delay is present */
        unsigned delayPresent : 1;
    } m;
    /** Horizontal Accuracy value */
    LSM_UINT8 horacc;
    /** Vertical Accuracy value */
    LSM_UINT8 veracc;
    /** Max Loc Age value */
    LSM_UINT16 maxLocAge;
    /** Delay value */
    LSM_UINT8 delay;
} LSM_AGPS_QoP;

/**
* Positioning Method Types (for LSM_AGPS_SUPL_INIT structure)
*/
typedef enum 
{
    LSM_AGPS_agpsSETassisted = 0,
    LSM_AGPS_agpsSETbased = 1,
    LSM_AGPS_agpsSETassistedpref = 2,
    LSM_AGPS_agpsSETbasedpref = 3,
    LSM_AGPS_autonomousGPS = 4,
    LSM_AGPS_aFLT = 5,
    LSM_AGPS_eCID = 6,
    LSM_AGPS_eOTD = 7,
    LSM_AGPS_oTDOA = 8,
    LSM_AGPS_noPosition = 9
} LSM_AGPS_PosMethod;

/**
* Notification Types (for LSM_AGPS_SUPL_INIT structure)
*/
typedef enum 
{
    LSM_AGPS_noNotificationNoVerification = 0,
    LSM_AGPS_notificationOnly = 1,
    LSM_AGPS_notificationAndVerficationAllowedNA = 2,
    LSM_AGPS_notificationAndVerficationDeniedNA = 3,
    LSM_AGPS_privacyOverride = 4
} LSM_AGPS_NotificationType;

/**
* Requestor ID and Client ID Types (for AGPS_SUPL_INIT structure)
*/
typedef enum 
{
    LSM_AGPS_logicalName = 0,
    LSM_AGPS_e_mailAddress = 1,
    LSM_AGPS_msisdn = 2,
    LSM_AGPS_url = 3,
    LSM_AGPS_sipUrl = 4,
    LSM_AGPS_min = 5,
    LSM_AGPS_mdn = 6
} LSM_AGPS_FormatIndicator;

/** @struct LSM_AGPS_Notification
 *  @brief The structure contains the notification for SUPL_INIT messages. */
typedef struct
{
    /** @brief It indicates what information is present in the structure */
    struct 
    {
        /** This indicates that Encoding Type is present */
        unsigned encodingTypePresent : 1;
        /** This indicates that Requestor ID is present */
        unsigned requestorIdPresent : 1;
        /** This indicates that Requestor ID Type is present */
        unsigned requestorIdTypePresent : 1;
        /** This indicates that Client Name is present */
        unsigned clientNamePresent : 1;
        /** This indicates that Client Name Type is present */
        unsigned clientNameTypePresent : 1;
    } m;
    /** Notification type */
    LSM_UINT32  notificationType;
    /** encoding type */
    LSM_UINT32 encodingType;
    /** Requestor ID Length */
    tSIRF_UINT32 requestorIdLength;
    /** Requestor ID */
    LSM_UINT8* requestorId;
    /** Requestor ID Type */
    LSM_UINT32  requestorIdType;
    /** Client Name Length */
    tSIRF_UINT32 clientNameLength;
    /** Client Name */
    LSM_UINT8* clientName;
    /** Client Name Type */
    LSM_UINT32  clientNameType;
} LSM_AGPS_Notification;


/** @struct LSM_AGPS_SUPL_INIT
 *  @brief This structure is to be used by the SuplInitListener. */
typedef struct
{
    /** This indicates what fields are present in the message*/
    struct 
    {
        /** This indicates that Notification is present */
        unsigned notificationPresent : 1;
        /** This indicates that QoP is present */
        unsigned qoPPresent : 1;
    } m;
    /** Positioning Method */
    LSM_UINT32              posMethod;
    /** Notification structure */
    LSM_AGPS_Notification   notification;
    /** QoP structure */
    LSM_AGPS_QoP            qoP;
} LSM_AGPS_SUPL_INIT;

/** @enum eLSM_UserResponse
 *  @brief The user response to SUPL NI position request. */
typedef enum 
{
   /** User accepts the request */
    LSM_AGPS_ACCEPT,
    /** User rejects the request */
    LSM_AGPS_REJECT,
    /** User ignores the request */
    LSM_AGPS_IGNORE
} eLSM_UserResponse;

/** @struct LSM_AGPS_UserResponse
 *  @brief structure contains the user response to be used by SuplInitListener. */
typedef struct
{
   /** User response */
    eLSM_UserResponse   UserResponse;
} LSM_AGPS_UserResponse;


/** @struct LSM_SUPL_INIT
 *  @brief structure contains the SUPL INIT Message bytes and length. */
typedef struct
{
    /*SUPL INIT packet bytes*/
    LSM_UINT8*  message;
    LSM_UINT16  size;
} LSM_SUPL_INIT;


  /**
   * @fn           LSM_BOOL LSM_DecodeSuplInit(LSM_SUPL_INIT* otaMessage,
                                               LSM_AGPS_SUPL_INIT* pSuplInitData);
   * @brief        AGPS driver calls this API to decode the SUPL INIT packet.
   * @param[in]    otaMessage: SUPL INIT message in LSM_SUPL_INIT structure format.
   * @param[out]   pSuplInitData: Pointer to the structure filled after decoding SUPL INIT.
   * @return       LSM_TRUE: LSM successfully decodes the SUPL INIT packet.
   *            @n LSM_FALSE: LSM cannot decode the SUPL INIT packet.
   */
#ifdef DLL
   DLL_EXPORT
#endif /*DLL*/
   LSM_BOOL LSM_DecodeSuplInit(LSM_SUPL_INIT* otaMessage,
                               LSM_AGPS_SUPL_INIT* pSuplInitData);

/** 
 * @}
 * End of file.
 */
