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
 *  FILENAME:       LSM_APIs.h
 *
 *  DESCRIPTION:    LSM is a middleware between the lower layer SiRF location protocol
 *                  library (LPL) and upper layer software entity including AGPS
 *                  driver/service/deamon, control plane agent (CPA), and WAP/SMS
 *                  related module.
 *
 *                  This header file includes all LSM APIs.
 *
 *  NOTES:          Only single session is supported for now.
 *
 ***************************************************************************
 *
 *  Keywords for Perforce. Do not modify.
 *
 *  $File: //customs/customer/Samsung_Volga/SDK/sirf/SiRFLPL/include/LSM_APIs.h $
 *
 *  $DateTime: 2010/06/28 22:14:04 $
 *
 *  $Revision: #2 $
 *
 ***************************************************************************
*/
/**
 * @mainpage Location Session Manager (LSM)
 * @par Introduction to LSM
 * @n LSM is a middleware between SiRF location protocol library (LPL) and an AGPS
   application/driver/service/deamon. It reduces the number of LPL APIs to
   facilitate the LPL integration. It decides which positioning request will
   be serviced when concurrent requests arrive. The positioning request to
   LSM may come from the control plane agent (CPA), the user application/service
   driver/deamon, or the WAP/SMS module. LSM's APIs matches WINCE and Windows
   PC driver and service framework, and Linux user space device driver framework
   so it can be easily ported to and integrated with the platforms of those OS.
   Because of the small number of APIs, LSM can be easily ported to other OS as
   well.
 * @n
 * @warning
 * @n Concurrent requests are not supported for now. Only an emergent request
   such as E911/112 has a higher priority to stop a normal priority running
   session even it is not completed. A normal request will be dropped when there
   is another running LPL session.
 * @defgroup LSM "Location Session Manager"
 * @addtogroup LSM
 * @file
 * @brief This header file includes all LSM APIs.
 * @{
 */

#ifndef _LSM_APIS_H_
#define _LSM_APIS_H_

#include "LSM_Types.h"

#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus*/

   /* *******************************-**********************************
    *                APIs For All Types Of LPL Sessions                *
    ********************************************************************/
   /**
   * @fn           LSM_CHAR* LSM_GetLSMVersion(tSIRF_VOID)
   * @brief        To get LSM + LPL version string. This is an optional API.
   * @return       The version string including LSM and LPL version information.
   */
#ifdef DLL
   DLL_EXPORT
#endif /*DLL*/
   LSM_CHAR* LSM_GetLSMVersion(tSIRF_VOID);

   /**
   * @fn           LSM_CHAR* LSM_GetGPSVersion(tSIRF_VOID)
   * @brief        To get SiRFNav + SLC + GPS tracker string. This is an optional API.
   * @return       The version string including SiRFNav, SLC, and tracker version information.
   */
#ifdef DLL
   DLL_EXPORT
#endif /*DLL*/
   LSM_CHAR* LSM_GetGPSVersion(tSIRF_VOID);

   /**
   * @fn           LSM_BOOL LSM_Init(LSM_commonCfgData *pCommonCfgData)
   * @brief        AGPS driver/service/deamon calls this API to initialize LSM and LPL
   *               when the device boots up. This is a mandatory API for all types of LPL sessions.
   * @param[in]    pCommonCfgData pointer to the data structure containing
   *               LPL configuration data for all LPL sessions.
   * @return       LSM_TRUE: success; @n LSM_FALSE: failure.
   * @warning      This function should be called only once when the device boots up.
   *               It's incorrect to call it per session.
   */
#ifdef DLL
   DLL_EXPORT
#endif /*DLL*/
   LSM_BOOL LSM_Init(LSM_commonCfgData *pCommonCfgData);

   /**
   * @fn           LSM_BOOL LSM_Deinit(tSIRF_VOID)
   * @brief        AGPS driver/service/deamon calls this API to deinitialize LSM/LPL before
   *               turning off the device. This is a mandatory API for all types of LPL sessions.
   * @return       LSM_TRUE: success; @n LSM_FALSE: failure.
   * @warning      This function should be called only once before the device is turned off.
   *               It's incorrect to call it per session.
   */
#ifdef DLL
   DLL_EXPORT
#endif /*DLL*/
   LSM_BOOL LSM_Deinit(tSIRF_VOID);

   /* ********************************-**********************************
    *                    APIs For SUPL Sesssions Only                   *
    ********************************************************************/
   /**
   * @fn           LSM_BOOL CP_SendCellInfo(LSM_netCellID *pCellInfo, LSM_BOOL cellInfoValid)
   * @brief        CPA calls this API to update GSM or WCDMA cell information.
   * @param[in]    pCellInfo: contains GSM or WCDMA cell info.
   * @param[in]    cellInfoValid: LSM_TRUE: cell info is valid; LSM_FALSE: invalidate cell info.
   * @return       LSM_TRUE: cell info sent successfully; @n LSM_FALSE: failed to send the cell info.
   * @warning      This API is used only when UP-A can push the cellular info to LSM.
   */
#ifdef DLL
   DLL_EXPORT
#endif /*DLL*/
   LSM_BOOL CP_SendCellInfo(LSM_netCellID *pCellInfo, LSM_BOOL cellInfoValid);

   /**
   * @fn           LSM_BOOL CP_SendSETIDInfo(SETID_Info *pSETidInfo, LSM_BOOL isSETIDInfoValid)
   * @brief        CPA calls this API to update SET ID info.
   * @param[in]    pSETidInfo: contains IMSI/T-IMSI or MSISDN number.
   * @param[in]    isSETIDInfoValid: LSM_TRUE: SET ID is valid; LSM_FALSE: invalidate SET ID info.
   * @return       LSM_TRUE: SET ID info sent successfully; @n LSM_FALSE: failed to send the SET ID info.
   * @warning      This API is used only when UP-A can push the SET ID info to LSM.
   */
#ifdef DLL
   DLL_EXPORT
#endif /*DLL*/
   LSM_BOOL CP_SendSETIDInfo(SETID_Info *pSETidInfo, LSM_BOOL isSETIDInfoValid);

   /* ********************************-**********************************
    *           APIs For Control Plane And SUPL NI Sesssions           *
    ********************************************************************/
   /**
   * @fn           LSM_BOOL CP_SendNetMessage(eLSM_OTA_TYPE ota_type,
                                              eLSM_RRC_STATE rrc_state,
                                              eLSM_SESSION_PRIORITY sessionPriority,
                                              LSM_UINT8* p_msg_data,
                                              LSM_UINT32 msg_size)
   * @brief        CPA calls this API to send a downlink control plane message to LPL;
   *               WAP/SMS calls this API to send a SUPL_INIT message to LPL.
   * @param[in]    ota_type: LSM_CP_RRC, LSM_CP_RRLP, or LSM_SUPL_RRLP_OR_RRC.
   * @param[in]    rrc_state: initial RRC state; @n Not used for RRLP and SUPL MT sessions.
   * @param[in]    sessionPriority: NO_EMERGENCY or EMERGENCY; @n Not used for SUPL and standalone sessions.
   * @param[in]    p_msg_data: For LSM_CP_RRC - downlink RRC message including DL-DCCH-Message part;
   *                        @n For LSM_CP_RRLP - downlink RRLP message including the whole RRLP PDU;
   *                        @n For LSM_SUPL_RRLP_OR_RRC - SUPL_INIT message body.
   * @param[in]    msg_size: size of the raw data.
   * @return       LSM_TRUE: success; @n LSM_FALSE: failure.
   * @warning      For RRC DL messages, p_msg_data could also be compiled in form of DL-DCCH-MessageType.
   */
#ifdef DLL
   DLL_EXPORT
#endif /*DLL*/
   LSM_BOOL CP_SendNetMessage(eLSM_OTA_TYPE ota_type,
                              eLSM_RRC_STATE rrc_state,
                              eLSM_SESSION_PRIORITY sessionPriority,
                              LSM_UINT8* p_msg_data,
                              LSM_UINT32 msg_size);

   /**
   * @fn LSM_BOOL CP_SendMeasurementTerminate(eLSM_OTA_TYPE otaType)
   * @brief CPA calls this API to terminate the current CP session;
   *        UPA or GPS service calls this API to terminate the current
   *        SUPL NI session once LSM reports LSM_TO_CLOSE in lplStatusCallback.
   * @param[in] otaType: LSM_CP_RRLP for RRLP or LSM_CP_RRC for RRC.
   * @return LSM_TRUE: request sent successfully;
   *      @n LSM_FALSE: failed to send the request.
   */
#ifdef DLL
   DLL_EXPORT
#endif /*DLL*/
   LSM_BOOL CP_SendMeasurementTerminate(eLSM_OTA_TYPE otaType);

   /* ********************************-**********************************
    *                   APIs For Control Plane Only                    *
    ********************************************************************/
   /**
   * @fn           LSM_BOOL CP_SetMessageListeners(LSM_MessageListener listener)
   * @brief        CPA calls this API to register the call back for up-link control plane messages.
   * @param[in]    listener: contains the callback function.
   * @return       LSM_TRUE: successfully registered; @n LSM_FALSE: failure to register.
   * @warning      This function MUST be called by CPA for control plane sessions.
   */
#ifdef DLL
   DLL_EXPORT
#endif /*DLL*/
   LSM_BOOL CP_SetMessageListeners(LSM_MessageListener listener);

   /**
   * @fn           LSM_BOOL CP_SendRRCStateEvent(eLSM_RRC_STATE rrc_state)
   * @brief        CPA calls this API to update RRC state to LPL.
   * @param[in]    rrc_state: contains the updated RRC state.
   * @return       LSM_TRUE: success; @n LSM_FALSE: failure.
   */
#ifdef DLL
   DLL_EXPORT
#endif /*DLL*/
   LSM_BOOL CP_SendRRCStateEvent(eLSM_RRC_STATE rrc_state);

   /**
   * @fn LSM_BOOL CP_NotfiyE911Dialed(tSIRF_VOID)
   * @brief CPA calls this API to notify LSM to close a non-emergency LPL session.
   * @return LSM_TRUE: there is no running LPL session any more;
   *      @n LSM_FALSE: the running session is an emergent session and cannot be closed.
   *
   */
#ifdef DLL
   DLL_EXPORT
#endif /*DLL*/
   LSM_BOOL CP_NotfiyE911Dialed(tSIRF_VOID);

   /**
   * @fn LSM_BOOL CP_Reset(tSIRF_VOID)
   * @brief Call this API when CPA receives "reset UE/MS stored positioning info" request from the network stack.
   * @return LSM_TRUE: successfully reset; @n LSM_FALSE: failed.
   * @warning This API is for CP testing only and won't be used in the real network.
   */
#ifdef DLL
   DLL_EXPORT
#endif /*DLL*/
   LSM_BOOL CP_Reset(tSIRF_VOID);

   /* ********************************-**********************************
    *         APIs For SUPL SI And Standalone LPL Sessions Only        *
    ********************************************************************/
   /**
   * @fn           LSM_UINT32 LSM_Start(LSM_SIsessionCfgData *pSISessionCfgData)
   * @brief        For SUPL SI and standalone sessions, AGPS driver needs to call this API to
   *               request a new LPL session.
   * @param[in]    pSISessionCfgData pointer to the data structure containing
   *               SET initialized (SI) LPL session configuration data.
   * @return       sessions ID: @n non-zero: LSM successfully started an LPL session
   *               per request; @n zero: LSM cannot start an LPL session per request,
   *               and the caller needs to ignore the session.
   */
#ifdef DLL
   DLL_EXPORT
#endif /*DLL*/
   LSM_UINT32 LSM_Start(LSM_SIsessionCfgData *pSISessionCfgData);

   /**
   * @fn           LSM_BOOL LSM_Stop(LSM_UINT32 sessionID, LSM_BOOL isUrgent)
   * @brief        AGPS driver calls this API to stop an LPL session.
   * @param[in]    sessionID: an LPL session ID returned by LSM_Start.
   * @param[in]    isUrgent: LSM_TRUE: shutdown the LPL session; @n LSM_FALSE: close the LPL session.
   * @return       LSM_TRUE: LSM successfully stops the LPL session;
   *            @n LSM_FALSE: LSM cannot stop the LPL session.
   */
#ifdef DLL
   DLL_EXPORT
#endif /*DLL*/
   LSM_BOOL LSM_Stop(LSM_UINT32 sessionID, LSM_BOOL isUrgent);

#ifdef DLL
    DLL_EXPORT
#endif
LSM_BOOL LSM_SetPrefLocationMethod(eLSM_PREFERRED_LOC_METHOD prefLocmethod);
#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif  /* _LSM_APIS_H_ */

/**
 * @}
 * End of file.
 */
