/** @file hci_opcodes.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

/**
 *@component    CNH202028 HCI 
 *@{*//**
 *\addtogroup   hci_common
 *@{*/
/**
 *@file         hci_opcodes.h 
 *
 *\brief        This header file contains HCI OpCodes and Event Codes.
 *
 *@version      _r2_r3_r4/1 
 *@date         Tue Aug  3 16:01:11 2004 
 *
 */

#ifndef HCI_OPCODES_H
#define HCI_OPCODES_H

/*====================  Include files  =======================================*/

//#include "core_types.h"
typedef unsigned char uint8;
typedef unsigned short uint16;

/*====================  Configurations =======================================*/

/*====================  Constants  ===========================================*/

/* OGF of OpCode (upper 6 bits): */
#define HCI_CMD_GROUP_LINK_CONTROL                   ((uint8)0x01)
#define HCI_CMD_GROUP_LINK_POLICY                    ((uint8)0x02)
#define HCI_CMD_GROUP_HC_BASEBAND                    ((uint8)0x03)
#define HCI_CMD_GROUP_INFORMATIONAL                  ((uint8)0x04)
#define HCI_CMD_GROUP_STATUS                         ((uint8)0x05)
#define HCI_CMD_GROUP_TESTING                        ((uint8)0x06)
#define HCI_CMD_GROUP_VENDOR_SPECIFIC                ((uint8)0x3F)

/* OCF of OpCode (lower 10 bits) for Link Control Commands: */
#define HCI_CMD_LC_INQUIRY                           ((uint16)0x0001)
#define HCI_CMD_LC_INQUIRY_CANCEL                    ((uint16)0x0002)
#define HCI_CMD_LC_PERIODIC_INQ_MODE                 ((uint16)0x0003)
#define HCI_CMD_LC_EXIT_PERIODIC_INQ_MODE            ((uint16)0x0004)
#define HCI_CMD_LC_CREATE_CONNECTION                 ((uint16)0x0005)
#define HCI_CMD_LC_DISCONNECT                        ((uint16)0x0006)
#define HCI_CMD_LC_ADD_SCO_CONN                      ((uint16)0x0007)
#define HCI_CMD_LC_CREATE_CONNECTION_CANCEL          ((uint16)0x0008)
#define HCI_CMD_LC_ACCEPT_CONNECTION_REQ             ((uint16)0x0009)
#define HCI_CMD_LC_REJECT_CONNECTION_REQ             ((uint16)0x000A)
#define HCI_CMD_LC_LINK_KEY_REQ_REPLY                ((uint16)0x000B)
#define HCI_CMD_LC_LINK_KEY_REQ_NEG_REPLY            ((uint16)0x000C)
#define HCI_CMD_LC_PIN_CODE_REQ_REPLY                ((uint16)0x000D)
#define HCI_CMD_LC_PIN_CODE_REQ_NEG_REPLY            ((uint16)0x000E)
#define HCI_CMD_LC_CHANGE_CONNECTION_PACKET_TYPE     ((uint16)0x000F)
/* 0x0010 Not in use */
#define HCI_CMD_LC_AUTHENTICATION_REQ                ((uint16)0x0011)
/* 0x0012 Not in use */
#define HCI_CMD_LC_SET_CONN_ENCRYPTION               ((uint16)0x0013)
/* 0x0014 Not in use */
#define HCI_CMD_LC_CHANGE_CONN_LINK_KEY              ((uint16)0x0015)
/* 0x0016 Not in use */
#define HCI_CMD_LC_MASTER_LINK_KEY                   ((uint16)0x0017)
/* 0x0018 Not in use */
#define HCI_CMD_LC_REMOTE_NAME_REQ                   ((uint16)0x0019)
#define HCI_CMD_LC_REMOTE_NAME_REQUEST_CANCEL        ((uint16)0x001A)
#define HCI_CMD_LC_READ_REMOTE_FEATURES              ((uint16)0x001B)
#define HCI_CMD_LC_READ_REMOTE_EXTENDED_FEATURES     ((uint16)0x001C)
#define HCI_CMD_LC_READ_REMOTE_VERSION_INFO          ((uint16)0x001D)
#define HCI_CMD_LC_READ_CLOCK_OFFSET                 ((uint16)0x001F)
#define HCI_CMD_LC_READ_LMP_HANDLE                   ((uint16)0x0020)

#define HCI_CMD_LC_SETUP_SYNCHRONOUS_CONN            ((uint16)0x0028)
#define HCI_CMD_LC_ACCEPT_SYNCHRONOUS_CONNECTION_REQ ((uint16)0x0029)
#define HCI_CMD_LC_REJECT_SYNCHRONOUS_CONNECTION_REQ ((uint16)0x002A)

/* OCF of OpCode (lower 10 bits) for Link Policy Commands: */
#define HCI_CMD_LP_HOLD_MODE                         ((uint16)0x0001)
/* 0x0002 Not in use */
#define HCI_CMD_LP_SNIFF_MODE                        ((uint16)0x0003)
#define HCI_CMD_LP_EXIT_SNIFF_MODE                   ((uint16)0x0004)
#define HCI_CMD_LP_PARK_MODE                         ((uint16)0x0005)
#define HCI_CMD_LP_EXIT_PARK_MODE                    ((uint16)0x0006)
#define HCI_CMD_LP_QOS_SETUP                         ((uint16)0x0007)
#define HCI_CMD_LP_FLOW_SPECIFICATION                ((uint16)0x0010)
/* 0x0008 Not in use */
#define HCI_CMD_LP_ROLE_DISCOVERY                      ((uint16)0x0009)
#define HCI_CMD_LP_SWITCH_ROLE                         ((uint16)0x000B)
#define HCI_CMD_LP_READ_LP_SETTINGS                    ((uint16)0x000C)
#define HCI_CMD_LP_WRITE_LP_SETTINGS                   ((uint16)0x000D)
#define HCI_CMD_LP_READ_DEFAULT_LINK_POLICY_SETTINGS   ((uint16)0x000E)
#define HCI_CMD_LP_WRITE_DEFAULT_LINK_POLICY_SETTINGS  ((uint16)0x000F)

#if defined(SUPPORT_BT30) && defined(SUPPORT_BT30_ATOMIC_ENCRYPTION)
#define HCI_CMD_LP_ATOMIC_ENCRYPTION_STOP             ((uint16)0x0080)
#define HCI_CMD_LP_ATOMIC_ENCRYPTION_START            ((uint16)0x0081)
#endif

/* OCF of OpCode (lower 10 bits) for Controller & Baseband Commands: */
#define HCI_CMD_HC_SET_EVENT_MASK                    ((uint16)0x0001)
#define HCI_CMD_HC_RESET                             ((uint16)0x0003)
#define HCI_CMD_HC_SET_EVENT_FILTER                  ((uint16)0x0005)
#define HCI_CMD_HC_FLUSH                             ((uint16)0x0008)
#define HCI_CMD_HC_READ_PIN_TYPE                     ((uint16)0x0009)
#define HCI_CMD_HC_WRITE_PIN_TYPE                    ((uint16)0x000A)
/* #define HCI_CMD_HC_CREATE_NEW_UNIT_KEY               ((uint16)0x000B) */
#define HCI_CMD_HC_READ_STORED_LINK_KEY              ((uint16)0x000D)
#define HCI_CMD_HC_WRITE_STORED_LINK_KEY             ((uint16)0x0011)
#define HCI_CMD_HC_DELETE_STORED_LINK_KEY            ((uint16)0x0012)
#define HCI_CMD_HC_CHANGE_LOCAL_NAME                 ((uint16)0x0013)
#define HCI_CMD_HC_READ_LOCAL_NAME                   ((uint16)0x0014)
#define HCI_CMD_HC_READ_CONN_ACCEPT_TO               ((uint16)0x0015)
#define HCI_CMD_HC_WRITE_CONN_ACCEPT_TO              ((uint16)0x0016)
#define HCI_CMD_HC_READ_PAGE_TO                      ((uint16)0x0017)
#define HCI_CMD_HC_WRITE_PAGE_TO                     ((uint16)0x0018)
#define HCI_CMD_HC_READ_SCAN_ENABLE                  ((uint16)0x0019)
#define HCI_CMD_HC_WRITE_SCAN_ENABLE                 ((uint16)0x001A)
#define HCI_CMD_HC_READ_PAGE_SCAN_ACTIVITY           ((uint16)0x001B)
#define HCI_CMD_HC_WRITE_PAGE_SCAN_ACTIVITY          ((uint16)0x001C)
#define HCI_CMD_HC_READ_INQ_SCAN_ACTIVITY            ((uint16)0x001D)
#define HCI_CMD_HC_WRITE_INQ_SCAN_ACTIVITY           ((uint16)0x001E)
#define HCI_CMD_HC_READ_AUTH_ENABLE                  ((uint16)0x001F)
#define HCI_CMD_HC_WRITE_AUTH_ENABLE                 ((uint16)0x0020)
#define HCI_CMD_HC_READ_ENCRYPT_MODE                 ((uint16)0x0021)
#define HCI_CMD_HC_WRITE_ENCRYPT_MODE                ((uint16)0x0022)
#define HCI_CMD_HC_READ_COD                          ((uint16)0x0023)
#define HCI_CMD_HC_WRITE_COD                         ((uint16)0x0024)
#define HCI_CMD_HC_READ_VOICE_SETTING                ((uint16)0x0025)
#define HCI_CMD_HC_WRITE_VOICE_SETTING               ((uint16)0x0026)
#define HCI_CMD_HC_READ_AUTOMATIC_FLUSH_TIMEOUT      ((uint16)0x0027)
#define HCI_CMD_HC_WRITE_AUTOMATIC_FLUSH_TIMEOUT     ((uint16)0x0028)
#define HCI_CMD_HC_READ_NUM_BROADCAST_RETRANS        ((uint16)0x0029)
#define HCI_CMD_HC_WRITE_NUM_BROADCAST_RETRANS       ((uint16)0x002A)
#define HCI_CMD_HC_READ_HOLD_MODE_ACTIVITY           ((uint16)0x002B)
#define HCI_CMD_HC_WRITE_HOLD_MODE_ACTIVITY          ((uint16)0x002C)
#define HCI_CMD_HC_READ_TRANSMIT_POWER_LEVEL         ((uint16)0x002D)
#define HCI_CMD_HC_READ_SYNCHRONOUS_FLOW_CONTROL_ENABLE  ((uint16)0x002E)
#define HCI_CMD_HC_WRITE_SYNCHRONOUS_FLOW_CONTROL_ENABLE ((uint16)0x002F)
#define HCI_CMD_HC_SET_HC_TO_HOST_FLOW_CTRL          ((uint16)0x0031)
#define HCI_CMD_HC_HOST_BUFFER_SIZE                  ((uint16)0x0033)
#define HCI_CMD_HC_HOST_NUMBER_OF_COMPLETED_PACKETS  ((uint16)0x0035)
#define HCI_CMD_HC_READ_LINK_SUPERVISION_TO          ((uint16)0x0036)
#define HCI_CMD_HC_WRITE_LINK_SUPERVISION_TO         ((uint16)0x0037)
#define HCI_CMD_HC_READ_NUM_OF_SUPPORTED_IAC         ((uint16)0x0038)
#define HCI_CMD_HC_READ_CURRENT_IAC_LAP              ((uint16)0x0039)
#define HCI_CMD_HC_WRITE_CURRENT_IAC_LAP             ((uint16)0x003A)
#define HCI_CMD_HC_READ_PAGE_SCAN_PERIOD_MODE        ((uint16)0x003B)
#define HCI_CMD_HC_WRITE_PAGE_SCAN_PERIOD_MODE       ((uint16)0x003C)
#define HCI_CMD_HC_READ_PAGE_SCAN_MODE               ((uint16)0x003D)
#define HCI_CMD_HC_WRITE_PAGE_SCAN_MODE              ((uint16)0x003E)
#define HCI_CMD_HC_SET_AFH_CHANNEL_CLASSIFICATION    ((uint16)0x003F)
#define HCI_CMD_HC_READ_INQUIRY_SCAN_TYPE            ((uint16)0x0042)
#define HCI_CMD_HC_WRITE_INQUIRY_SCAN_TYPE           ((uint16)0x0043)
#define HCI_CMD_HC_READ_INQUIRY_MODE                 ((uint16)0x0044)
#define HCI_CMD_HC_WRITE_INQUIRY_MODE                ((uint16)0x0045)
#define HCI_CMD_HC_READ_PAGE_SCAN_TYPE               ((uint16)0x0046)
#define HCI_CMD_HC_WRITE_PAGE_SCAN_TYPE              ((uint16)0x0047)
#define HCI_CMD_HC_READ_AFH_CHANNEL_ASSESSMENT_MODE  ((uint16)0x0048)
#define HCI_CMD_HC_WRITE_AFH_CHANNEL_ASSESSMENT_MODE ((uint16)0x0049)
#define HCI_CMD_HC_READ_ANONYMITY_MODE               ((uint16)0x004A)
#define HCI_CMD_HC_WRITE_ANONYMITY_MODE              ((uint16)0x004B)
#define HCI_CMD_HC_READ_ALIAS_AUTHENTICATION         ((uint16)0x004C)
#define HCI_CMD_HC_WRITE_ALIAS_AUTHENTICATION        ((uint16)0x004D)
#define HCI_CMD_HC_READ_ANONYMOUS_ADDRESS_CHANGE     ((uint16)0x004E)
#define HCI_CMD_HC_WRITE_ANONYMOUS_ADDRESS_CHANGE    ((uint16)0x004F)
#define HCI_CMD_HC_RESET_FIXED_ADDRESS_ATTEMPTS_COUNTER ((uint16)0x0050)

//#if defined(SUPPORT_BT30) && defined(SUPPORT_BT30_EXTENDED_INQUIRY_RESP)
#define HCI_CMD_HC_WRITE_EXTENDED_INQUIRY_DATA        ((uint16)0x0051)
//#if defined(SUPPORT_BT30) && defined(SUPPORT_BT30_IMPROVED_PIN_ENTRY)
#define HCI_CMD_HC_INPUT_PIN_CODE                     ((uint16)0x0052)
#define HCI_CMD_HC_ENABLE_PAIRING_REVERSE                 ((uint16)0x0053)

/* OCF of OpCode (lower 10 bits) for Informational Parameters: */
#define HCI_CMD_INFO_READ_LOCAL_VERSION_INFO         ((uint16)0x0001)
#define HCI_CMD_INFO_READ_LOCAL_SUPPORTED_COMMANDS   ((uint16)0x0002)
#define HCI_CMD_INFO_READ_LOCAL_FEATURES             ((uint16)0x0003)
#define HCI_CMD_INFO_READ_LOCAL_EXTENDED_FEATURES    ((uint16)0x0004)
#define HCI_CMD_INFO_READ_BUFFER_SIZE                ((uint16)0x0005)
#define HCI_CMD_INFO_READ_COUNTRY_CODE               ((uint16)0x0007)
#define HCI_CMD_INFO_READ_BD_ADDRESS                 ((uint16)0x0009)

/* OCF of OpCode (lower 10 bits) for Status Parameters: */
#define HCI_CMD_ST_READ_FAILED_CONTACT_COUNTER       ((uint16)0x0001)
#define HCI_CMD_ST_RESET_FAILED_CONTACT_COUNTER      ((uint16)0x0002)
#define HCI_CMD_ST_GET_LINK_QUALITY                  ((uint16)0x0003)
#define HCI_CMD_ST_READ_RSSI                         ((uint16)0x0005)
#define HCI_CMD_ST_READ_AFH_CHANNEL_MAP              ((uint16)0x0006)
#define HCI_CMD_ST_READ_CLOCK                        ((uint16)0x0007)

/* OCF of OpCode (lower 10 bits) for Testing Commands: */
#define HCI_CMD_TEST_READ_LOOPBACK_MODE              ((uint16)0x0001)
#define HCI_CMD_TEST_WRITE_LOOPBACK_MODE             ((uint16)0x0002)
#define HCI_CMD_TEST_ENABLE_DEVICE_UNDER_TEST_MODE   ((uint16)0x0003)

/* OCF of OpCode (lower 10 bits) for Ericsson Specific Commands
 * implemented in hci_commander.c: */
#define HCI_CMD_ERICSSON_WRITE_PCM_SETTINGS          ((uint16)0x0007)
#define HCI_CMD_ERICSSON_BER                         ((uint16)0x0015)
#define HCI_CMD_ERICSSON_TX_TEST                     ((uint16)0x0019)
#define HCI_CMD_ERICSSON_TEST_ACL_RECEIVE            ((uint16)0x001A)
#define HCI_CMD_ERICSSON_TEST_ACL_SEND               ((uint16)0x001B)
#define HCI_CMD_ERICSSON_SET_SCO_DATA_PATH           ((uint16)0x001D)
#define HCI_CMD_ERICSSON_SEND_LMP                    ((uint16)33)
/* #define HCI_CMD_ERICSSON_TEST_SCO_RECEIVE            ((uint16)37) */
#define HCI_CMD_ERICSSON_TEST_SCO_SEND               ((uint16)38)
#define HCI_CMD_ERICSSON_WRITE_PCM_SYNC_SETTINGS     ((uint16)40)
#define HCI_CMD_ERICSSON_WRITE_PCM_LINK_SETTINGS     ((uint16)41)
#define HCI_CMD_ERICSSON_WRITE_MAX_POWER             ((uint16)43)
#define HCI_CMD_ERICSSON_READ_MAX_POWER              ((uint16)44)
#define HCI_CMD_ERICSSON_ENHANCED_POWER_CONTROL      ((uint16)45)
#define HCI_CMD_ERICSSON_WRITE_AFH_PARAMETERS        ((uint16)64)
#define HCI_CMD_ERICSSON_WRITE_AFH_ENABLE            ((uint16)65)
#define HCI_CMD_ERICSSON_WRITE_RADIO_MODE            ((uint16)66)
#define HCI_CMD_ERICSSON_WRITE_EVENTS                ((uint16)67)
#define HCI_CMD_ERICSSON_READ_PER                    ((uint16)68)
#define HCI_CMD_ERICSSON_WRITE_AFH                   ((uint16)69)
#define HCI_CMD_ERICSSON_GET_LT_ADDR                 ((uint16)70)
#define HCI_CMD_ERICSSON_SET_FEATUREBIT              ((uint16)71)
#define HCI_CMD_ERICSSON_WRITE_COMPATIBILITY_MODE    ((uint16)72)


// OCF of OpCode (lower 10 bits) for Marvell MFG Specific Commands 
#define HCI_CMD_MRVL_MFG_BT				((uint16)0x0200)	// temp		
#define HCI_CMD_MRVL_ERC_READ 		((uint16)0xC001)	// temp	
#define HCI_CMD_MRVL_ERC_WRITE		((uint16)0xC002)	// temp	
#define HCI_CMD_MRVL_RXTEST				((uint16)0x0018)	// temp	deepak
#define HCI_CMD_MRVL_TXTEST				((uint16)0x0019)	// temp	deepak


#define HCI_CMD_MRVL_SETBTSLEEPMODE				((uint16)0x0023)

#define HCI_CMD_MRVL_BTCOEX_XCOMP_MODE			((uint16)0x004D)
#define HCI_CMD_MRVL_BTCOEX_MARVELL_MODE		((uint16)0x004E)
#define HCI_CMD_MRVL_BTCOEX_MARVELL_MODE2		((uint16)0x004F)

#define HCI_CMD_MRVL_RF_DYN_CLK_CONTROL			((uint16)0x0050)
 
#define HCI_CMD_MRVL_SET_NCO_MODE				((uint16)0x0051)
#define HCI_CMD_MRVL_SET_TXPOWERCONTROL			((uint16)0x0052)
#define HCI_CMD_MRVL_WAKE_METHOD_CONTROL		((uint16)0x0053) 
#define HCI_CMD_MRVL_SET_RADIO_PERF_MODE		((uint16)0x0054)
#define HCI_CMD_MRVL_ARM_DYNCLK_CONTROL			((uint16)0x0056)
#define HCI_CMD_MRVL_BT_HOST_SLEEP_CONFIG		((uint16)0x0059)
#define HCI_CMD_MRVL_BT_HIU_HS_ENABLE			((uint16)0x005A)
#define HCI_CMD_MRVL_BT_MODULE_CONF				((uint16)0x005B)


/* Event codes for HCI events: */
#define HCI_EVENT_INQUIRY_COMPLETE                   ((uint8)0x01)
#define HCI_EVENT_INQUIRY_RESULT                     ((uint8)0x02)
#define HCI_EVENT_CONN_COMPLETE                      ((uint8)0x03)
#define HCI_EVENT_CONN_REQUEST                       ((uint8)0x04)
#define HCI_EVENT_DISCONNECTION_COMPLETE             ((uint8)0x05)
#define HCI_EVENT_AUTHENTICATION_COMPLETE            ((uint8)0x06)
#define HCI_EVENT_REMOTE_NAME_REQ_COMPLETE           ((uint8)0x07)
#define HCI_EVENT_ENCRYPTION_CHANGE                  ((uint8)0x08)
#define HCI_EVENT_CHANGE_CONN_LINK_KEY_COMPLETE      ((uint8)0x09)
#define HCI_EVENT_MASTER_LINK_KEY_COMPLETE           ((uint8)0x0A)
#define HCI_EVENT_READ_REMOTE_FEATURES_COMPLETE      ((uint8)0x0B)
#define HCI_EVENT_READ_REMOTE_VERSION_INFO_COMPLETE  ((uint8)0x0C)
#define HCI_EVENT_QOS_SETUP_COMPLETE                 ((uint8)0x0D)
#define HCI_EVENT_CMD_COMPLETE                       ((uint8)0x0E)
#define HCI_EVENT_CMD_STATUS                         ((uint8)0x0F)
#define HCI_EVENT_HW_ERROR                           ((uint8)0x10)
#define HCI_EVENT_FLUSH_OCCURRED                     ((uint8)0x11)
#define HCI_EVENT_ROLE_CHANGE                        ((uint8)0x12)
#define HCI_EVENT_NUMBER_OF_COMPLETED_PACKETS        ((uint8)0x13)
#define HCI_EVENT_MODE_CHANGE                        ((uint8)0x14)
#define HCI_EVENT_RETURN_LINK_KEYS                   ((uint8)0x15)
#define HCI_EVENT_PIN_CODE_REQUEST                   ((uint8)0x16)
#define HCI_EVENT_LINK_KEY_REQUEST                   ((uint8)0x17)
#define HCI_EVENT_LINK_KEY_NOTIFICATION              ((uint8)0x18)
#define HCI_EVENT_LOOPBACK_COMMAND                   ((uint8)0x19)
#define HCI_EVENT_DATA_BUFFER_OVERFLOW               ((uint8)0x1A)
#define HCI_EVENT_MAX_SLOTS_CHANGE                   ((uint8)0x1B)
#define HCI_EVENT_READ_CLOCK_OFFSET_COMPLETE         ((uint8)0x1C)
#define HCI_EVENT_CONN_PACKET_TYPE_CHANGED           ((uint8)0x1D)
#define HCI_EVENT_QOS_VIOLATION                      ((uint8)0x1E)
/* 0x1F Not in use */
#define HCI_EVENT_PAGE_SCAN_REPETITION_MODE_CHANGE   ((uint8)0x20)
#define HCI_EVENT_FLOW_SPECIFICATION_COMPLETE        ((uint8)0x21)
#define HCI_EVENT_INQUIRY_RESULT_WITH_RSSI           ((uint8)0x22)
#if defined(SUPPORT_BT30) && defined(SUPPORT_BT30_EXTENDED_INQUIRY_RESP)
#define HCI_EVENT_INQUIRY_RESULT_WITH_RSSIOREIR      ((uint8)0x23)
#endif
#define HCI_EVENT_SYNCHRONOUS_CONNECTION_COMPLETE    ((uint8)0x2C)
#define HCI_EVENT_SYNCHRONOUS_CONNECTION_CHANGED     ((uint8)0x2D)
#define HCI_EVENT_VENDOR_SPECIFIC                    ((uint8)0xFF)

/* Event ID for Ericsson specific events */
#define ERICSSON_LMP_EVENT                           ((uint8)0x10)
#define ERICSSON_HSSI_EVENT                          ((uint8)0x11)


#define HCI_EVENT_MRVL_RXTEST						 ((uint8)0x18)	// temp	deepak

#define HCI_EVENT_MRVL_LOW_POWER_MODE                ((uint8)0x23) 
#define HCI_EVENT_MRVL_BT_MODULE_CONF                ((uint8)0x5B)



enum{
	MODE_CURRENTMODE_ACTIVE,
	MODE_CURRENTMODE_HOLD,
	MODE_CURRENTMODE_SNIFF,
	MODE_CURRENTMODE_PARK
};
/*====================  Parametrized macros  =================================*/

/*====================  Typedefs  ============================================*/

/*====================  Signals  =============================================*/

/*====================  Variables  ===========================================*/

/*====================  Functions  ===========================================*/

#endif /* HCI_OPCODES_H */
/* End of file *//**@}@}*/
