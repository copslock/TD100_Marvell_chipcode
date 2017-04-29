/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/* ===========================================================================
   File        : ci_pb.h
   Description : Data types file for the PB Service Group

   Notes       : See "Cellular Interface Phonebook Service Group" API document
	      for a description of the PB Service Group primitives.

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

   Unless otherwise agreed by Intel in writing, you may not remove or alter this notice or any other notice embedded
   in Materials by Intel or Intel's suppliers or licensors in any way.
   =========================================================================== */

#if !defined( _CI_PB_H_ )
#define _CI_PB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ci_api_types.h"
#include "ci_cfg.h"

/* Need to check later on for make sure definition is correct */
#define CIPB_MAX_NAME_LENGTH 32
/** \addtogroup  SpecificSGRelated
 * @{ */
#define CI_PB_VER_MAJOR 3
#define CI_PB_VER_MINOR 0

//ICAT EXPORTED ENUM
/** Summary of primitives */
typedef enum CI_PB_PRIM {
	CI_PB_PRIM_GET_SUPPORTED_PHONEBOOKS_REQ = 1, /**< \brief Requests a list of supported phonebooks \details   */
	CI_PB_PRIM_GET_SUPPORTED_PHONEBOOKS_CNF, /**< \brief Confirms the request to get a list of supported phonebooks
						 * \details The "list of supported phonebooks" is an array of integer "Phonebook ID" numbers, as defined by the enumerated type CiPhonebookId. */
	CI_PB_PRIM_GET_PHONEBOOK_INFO_REQ,      /**< \brief Requests information for one of the supported phonebooks
						 * \details The caller may indicate the currently selected phonebook, or may indicate any of the supported phonebooks.*/
	CI_PB_PRIM_GET_PHONEBOOK_INFO_CNF,      /**< \brief Confirms the request and returns information for a specified phonebook
						 * \details The returned PbId field indicates the actual phonebook ID,
						 * even if current phonebook information (CI_PHONEBOOK_CURRENT) is requested.  */
	CI_PB_PRIM_SELECT_PHONEBOOK_REQ,        /**< \brief Requests to select one of the supported phonebooks as the current phonebook
						 * \details If selected, the phonebook becomes the current phonebook for subsequent read and (if allowed) write operations. */
	CI_PB_PRIM_SELECT_PHONEBOOK_CNF,                /**< \brief Confirms the request to select one of the supported phonebooks as the current phonebook \details   */
	CI_PB_PRIM_READ_FIRST_PHONEBOOK_ENTRY_REQ,      /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_PB_PRIM_READ_FIRST_PHONEBOOK_ENTRY_CNF,      /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_PB_PRIM_READ_NEXT_PHONEBOOK_ENTRY_REQ,       /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_PB_PRIM_READ_NEXT_PHONEBOOK_ENTRY_CNF,       /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_PB_PRIM_FIND_FIRST_PHONEBOOK_ENTRY_REQ,      /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_PB_PRIM_FIND_FIRST_PHONEBOOK_ENTRY_CNF,      /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_PB_PRIM_FIND_NEXT_PHONEBOOK_ENTRY_REQ,       /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_PB_PRIM_FIND_NEXT_PHONEBOOK_ENTRY_CNF,       /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_PB_PRIM_ADD_PHONEBOOK_ENTRY_REQ,     /**< \brief Requests to add a new entry to the current phonebook
						 * \details The new entry is added to the end of the current phonebook, if there is room.
						 * If write access is not allowed for the currently selected phonebook,
						 * this request returns an error confirmation indicating "invalid operation".
						 * The current phonebook is selected by CI_PB_SELECT_PHONEBOOK_REQ.   */
	CI_PB_PRIM_ADD_PHONEBOOK_ENTRY_CNF,     /**< \brief Confirms the addition of a new entry to the current phonebook
						 * \details If the request is successful (a new entry is added to the current phonebook),
						 * the Index parameter indicates the location (index) of the new entry.
						 * The phonebook information is updated to show that a new entry has been added.
						 *    If the Result field indicates an error, the phonebook entry is not added.
						 *    If the phonebook is on the SIM and requires password verification for write access, the result code indicates a SIM error if
						 *    verification has not already been established. */
	CI_PB_PRIM_DELETE_PHONEBOOK_ENTRY_REQ,  /**< \brief Requests the indicated entry to be deleted from the current phonebook
						 * \details If write access is not allowed for the currently selected phonebook,
						 * this request returns an error indicating "invalid operation".
						 *  The current phonebook is selected by CI_PB_SELECT_PHONEBOOK_REQ.  */
	CI_PB_PRIM_DELETE_PHONEBOOK_ENTRY_CNF,  /**< \brief Confirms the deletion of an entry from the current phonebook
						 * \details If the Result field indicates an error, the phonebook entry is not deleted.
						 *  If the phonebook is on the SIM and requires password verification for write access, the result code indicates a SIM error if
						 *  verification has not already been established.*/
	CI_PB_PRIM_REPLACE_PHONEBOOK_ENTRY_REQ, /**< \brief Requests the indicated entry in the current phonebook to be replaced (overwritten) with new data
						 * \details The indicated entry is to be replaced (overwritten) with the supplied information.
						 *   If write access is not allowed for the currently selected phonebook, this request returns an error indicating
						 *   "invalid operation".
						 *   The current phonebook is selected by CI_PB_SELECT_PHONEBOOK_REQ.  */
	CI_PB_PRIM_REPLACE_PHONEBOOK_ENTRY_CNF, /**< \brief Confirms the replace (overwrite) of the requested entry in the current phonebook
						 * \details If the Result field indicates an error, the phonebook entry remains unmodified (not replaced).
						 *  If the phonebook is on the SIM and requires password verification for write access, the result code indicates a SIM error if
						 *  verification has not already been established. */
	CI_PB_PRIM_READ_PHONEBOOK_ENTRY_REQ,    /**< \brief Requests to reads an entry from the current phonebook
						 * \details Caller uses this request to read individual entries from the current phonebook.
						 * The current phonebook is selected by CI_PB_SELECT_PHONEBOOK_REQ.  */
	CI_PB_PRIM_READ_PHONEBOOK_ENTRY_CNF,    /**< \brief Confirms the request and returns the requested entry from the current phonebook
						 * \details If the Result field indicates an error condition, the returned index and phonebook entry are invalid.
						 *   If the phonebook is on the SIM and requires password verification for read access, the result code indicates a SIM error if
						 *   verification has not already been established.
						 */
	CI_PB_PRIM_GET_IDENTITY_REQ,            /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_PB_PRIM_GET_IDENTITY_CNF,            /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_PB_PRIM_PHONEBOOK_READY_IND,         /**< \brief Indicates that the SIM and/or NVRAM phonebooks are 'ready to use'
						 * \details SAC sends this notification after receiving a notification signal from the protocol stack
						 * indicating that its 'list management' component has been initialized (SIM and NVRAM files
						 * have been validated/read/cached/etc.) and is ready for processing call requests.  */
	CI_PB_PRIM_GET_ALPHA_NAME_REQ,          /**< \brief Requests the alphanumeric name string associated with the specified phone number
						 * \details SAC sends a request to the protocol stack to find the number and name in its ADR/ADN phonebooks.  */
	CI_PB_PRIM_GET_ALPHA_NAME_CNF,          /**< \brief Confirms the request to get the alphanumeric name string associated with the specified phone number
						 * \details If the requested number is not found, the alphanumeric name string is not provided (Name.Length = 0).
						 * Note that in this case the result code indicates success (Result = CIRC_PB_SUCCESS). */

	CI_PB_PRIM_READ_ENH_PBK_ENTRY_REQ,              /**< \brief Requests to read an enhanced information element from an entry in the current phonebook
							 * \details The enhanced information elements are: email address 0, email address 1, additional number 0,
							 * additional number 1, associated group index, and second name.
							 * Only one of the enhanced information elements can be read at a time.
							 * The phonebook must be selected by the user before this primitive can be used. */

	CI_PB_PRIM_READ_ENH_PBK_ENTRY_CNF,      /**< \brief Confirms the request and returns the requested enhanced phonebook information element
						 * \details The confirmation structure contains all possible enhanced phonebook information elements,
						 *   but only the information element requested is valid. */
	CI_PB_PRIM_EDIT_ENH_PBK_ENTRY_REQ,      /**< \brief Requests to edit one enhanced phonebook information element from an entry in the current phonebook
						 * \details
						 * The phonebook must be selected by the user before this primitive can be used.
						 * This primitive can be used to edit one information element at a time. */
	CI_PB_PRIM_EDIT_ENH_PBK_ENTRY_CNF,      /**< \brief Confirms the request to edit one enhanced phonebook information element
						 * \details
						 */

	CI_PB_PRIM_READ_CATEGORY_NAME_REQ,    /**< \brief Requests to read a category name or a list of all the defined category names in the current phonebook (SIM file: EFAAS)
					       * \details The phonebook must be selected by the user before this primitive can be used.
					       * The user can enter a valid index (between '1' and '10') to read a specific category name or enter an
					       * index value of '0' to read all category names.
					       * The category index associated with an additional number in a phonebook entry is provided in the ANR enhanced phonebook information element.
					       */
	CI_PB_PRIM_READ_CATEGORY_NAME_CNF, /**< \brief Confirms the request and returns the requested category name(s) */
	CI_PB_PRIM_EDIT_CATEGORY_NAME_REQ, /**< \brief Requests to edit a category name in the current phonebook
				     * \details The phonebook must be selected by the user before this primitive can be used.
				     * If the edit operation is 'add', the index is not relevant since the PS adds the new category name in the next available entry.
				     * If the edit operation is 'delete', the category name is not relevant. */
	CI_PB_PRIM_EDIT_CATEGORY_NAME_CNF, /**< \brief Confirms the request to edit a category name in the current phonebook */

	CI_PB_PRIM_READ_GROUP_NAME_REQ, /**< \brief Requests to read a group name or a list of all the defined group names in the current phonebook (SIM file: EFGAS)
				  * \details The phonebook must be selected by the user before this primitive can be used.
				  * The user can enter a valid index (between '1' and '10') to read a specific group name or enter
				  * index value '0' to read all category names.
				  * The group index associated with a PB entry is one of the enhanced information elements that can be read using CI_PB_PRIM_READ_ENHANCED_PHONEBOOK_ENTRY_REQ.
				  */
	CI_PB_PRIM_READ_GROUP_NAME_CNF, /**< \brief Confirms the request and returns the requested group name(s)
				  */
	CI_PB_PRIM_EDIT_GROUP_NAME_REQ, /**< \brief Requests to edit a group name in the current phonebook (SIM file: EFGAS)
				  * \details The phonebook must be selected by the user before this primitive can be used.
				  * If the edit operation is 'add', the index is not relevant since the PS adds the new group name in the next available entry.
				  * If the edit operation is 'delete', the group name is not relevant. */


	CI_PB_PRIM_EDIT_GROUP_NAME_CNF, /**< \brief Confirms the request to edit a group name in the current phonebook (SIM file: EFGAS)
				  */
	/* ADD NEW COMMON PRIMITIVES HERE, BEFORE 'CI_PB_PRIM_LAST_COMMON_PRIM' */

	/* END OF COMMON PRIMITIVES LIST */
	CI_PB_PRIM_LAST_COMMON_PRIM

	/* the  customer specific extension primitives will be added starting from
	 * CI_PB_PRIM_firstCustPrim = CI_PB_PRIM_LAST_COMMON_PRIM as the first identifier.
	 * The actual primitive names and IDs are defined in the associated
	 * 'ci_pb_cust_xxx.h' file.
	 */

	/* DO NOT ADD ANY MORE PRIMITIVES HERE */

} _CiPbPrim;

/* specify the number of default common PB primitives */
#define CI_PB_NUM_COMMON_PRIM ( CI_PB_PRIM_LAST_COMMON_PRIM - 1 )
/**@}*/

/* Michal Bukai & -BT  AT&T Smart Card support */

/** \brief  Phonebook group definitions */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CiPbGroupsTg {
	CI_PB_GROUP_PHONEBOOK_ENTRY = 0,                                /**< Phone book  */
	CI_PB_GROUP_ALPHA_NAME,                                         /**<  Alpha name*/
	CI_PB_GROUP_ENHANCED_PHONEBOOK_ENTRY,                           /**< Enhanced phone book*/
	CI_PB_GROUP_CATEGORY_NAME,                                      /**< Category name - EFAAS*/
	CI_PB_GROUP_GROUP_NAME,                                         /**< Read group names - EFGAS */
	CI_PB_GROUP_MAX
} _CiPbGroups;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Phonebook groups
 * \sa CIRC_PB */
/** \remarks Common Data Section */
typedef UINT8 CiPbGroupsType;

/**@}*/
/** \brief Phonebook return codes. */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIRC_PB {
	CIRC_PB_SUCCESS = 0,                    /**< Request completed successfully */
	CIRC_PB_FAIL,                           /**< CME 100 - General failure (catch-all)  */
	CIRC_PB_NO_PHONEBOOK,                   /**< CME 100 - No phonebook has been selected */
	CIRC_PB_BAD_PHONEBOOK_ID,               /**< CME 100 Phonebook ID is invalid or unknown */
	CIRC_PB_SIM_FAILURE,                    /**< CME 13 - SIM error while accessing phonebook */
	CIRC_PB_NO_MORE_ENTRIES,                /**< CME 20 - No more phonebook entries for request  */
	CIRC_PB_NO_MORE_ROOM,                   /**< CME 20 - No more room in phonebook  */
	CIRC_PB_DATA_UNAVAILABLE,               /**< CME 100 - Phonebook data unavailable */
	CIRC_PB_INVALID_OPERATION,              /**< CME 3 - Phonebook operation not allowed  */
	CIRC_PB_BAD_DIALSTRING,                 /**< CME 27 - Invalid phone number */
	CIRC_PB_FDN_OPER_NOT_ALLOWED,           /**< CME 3 - Phonebook operation not allowed */

	/* This one must always be last in the list! */

	CIRC_PB_NUM_RESCODES                    /**< Number of result codes defined  */

} _CiPbResultCode;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Result code
 * \sa CIRC_PB */
/** \remarks Common Data Section */
typedef UINT16 CiPbResultCode;
/**@}*/



/** \brief Phonebook ID definitions */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CI_PHONEBOOK_ID {

	CI_PHONEBOOK_DC = 1,            /**< ME dialed calls list */
	CI_PHONEBOOK_EN,                /**< SIM card emergency number  */
	CI_PHONEBOOK_FD,                /**< SIM card fixed dialing numbers list  */
	CI_PHONEBOOK_LD,                /**< SIM card last dialed numbers list  */
	CI_PHONEBOOK_MC,                /**< Missed (unanswered) calls list */
	CI_PHONEBOOK_ME,                /**< ME phonebook */
	CI_PHONEBOOK_MT,                /**< Combined ME and SIM phonebook  */
	CI_PHONEBOOK_ON,                /**< SIM card (or ME) own numbers list  */
	CI_PHONEBOOK_RC,                /**< Received calls (last numbers received) */
	CI_PHONEBOOK_SM,                /**< SIM phonebook  */
	CI_PHONEBOOK_AP,                /**< App Phonebook  */
	CI_PHONEBOOK_MBDN,              /**< SIM CPHS MBDN    */
	CI_PHONEBOOK_MN,                /**< SIM CPHS MN     */

	/* << Define additional Phonebook IDs here >> */
	CI_PHONEBOOK_NEXT               /**< REMOVE FROM API */
	/* Next available (unused) Phonebook ID */

} _CiPhonebookId;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Phonebook ID.
 * \sa CI_PHONEBOOK_ID */
/** \remarks Common Data Section */
typedef UINT8 CiPhonebookId;
/**@}*/



#define CI_NUM_DEFINED_PHONEBOOKS ( (CiPhonebookId)( CI_PHONEBOOK_NEXT - 1 ) )
#define CI_PHONEBOOK_CURRENT      ( (CiPhonebookId)0 )          /* Current selected phonebook */
#define CI_PHONEBOOK_NONE         ( (CiPhonebookId) - 1 )       /* No phonebook selected yet  */

/** \brief Phonebook entry content structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiPhonebookEntry_struct {
	CiAddressInfo Number;           /**< Phone number \sa CCI API Ref Manual */
	CiNameInfo Name;                /**< Name \sa CCI API Ref Manual */
	CiBoolean isHidden;             /**< TRUE if phonebook entry is hidden \sa CCI API Ref Manual  */
}CiPhonebookEntry;

/** \brief Phonebook information  structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiPhonebookInfo_struct {
	CiPhonebookId PbId;                                     /**< Phonebook ID \sa CiPhonebookId  */
	CiBoolean NeedPassword;                                 /**< TRUE if phonebook requires a password \sa CCI API Ref Manual  */
	CiBoolean writeAllowed;                                 /**< TRUE if phonebook write access is allowed \sa CCI API Ref Manual */
	UINT16 UsedEntries;                                     /**< Number of entries used (if known)  */
	UINT16 TotalEntries;                                    /**< Total number of entries (if known) */
	UINT8 MaxAlphaIdLength;                                 /**< Maximum length of alphanumeric name  */
}CiPhonebookInfo;

/* -------------------------- PB Service Group Primitives ------------------------------ */

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_GET_SUPPORTED_PHONEBOOKS_REQ"> */
typedef CiEmptyPrim CiPbPrimGetSupportedPhonebooksReq;

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_GET_SUPPORTED_PHONEBOOKS_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiPbPrimGetSupportedPhonebooksCnf_struct {
	CiPbResultCode Result;                                                                                  /**< Result code \sa CiPbResultCode */
	UINT8 NumPb;                                                                                            /**< The number of supported phonebooks [1..CI_NUM_SUPPORTED_PHONEBOOKS]*/
	CiPhonebookId SuppPb[CI_NUM_SUPPORTED_PHONEBOOKS];                                                      /**< A list of supported phonebooks \sa CiPhonebookId*/
} CiPbPrimGetSupportedPhonebooksCnf;

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_GET_PHONEBOOK_INFO_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiPbPrimGetPhonebookInfoReq_struct {
	UINT8 PbId;                                     /**< Requested phonebook ID or current phonebook [range: 1..CI_NUM_SUPPORTED_PHONEBOOKS or CI_PHONEBOOK_CURRENT] \sa CI_PHONEBOOK_ID  */
} CiPbPrimGetPhonebookInfoReq;

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_GET_PHONEBOOK_INFO_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiPbPrimGetPhonebookInfoCnf_struct {
	CiPbResultCode Result;                          /**< Result code \sa CiPbResultCode */
#ifndef CCI_PB_CONTIGUOUS
	CiPhonebookInfo       *Info;                    /**< Phonebook information \sa CiPhonebookInfo_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *               tempNoncontPtr;           /* remove when both COMM and APP side use contiguous flags */
#endif
	CiPhonebookInfo info;                           /**< Phonebook information \sa CiPhonebookInfo_struct */
#endif

} CiPbPrimGetPhonebookInfoCnf;

/*<NOTINUSE>*/
/** <paramref name="CI_PB_PRIM_SELECT_PHONEBOOK_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiPbPrimSelectPhonebookReq_struct {
	CiPhonebookId PbId;                                     /**< Phonebook ID for the phonebook to be selected [1..CI_NUM_SUPPORTED_PHONEBOOKS] \sa CiPhonebookId */
#ifndef CCI_PB_CONTIGUOUS
	CiPassword                    *Password;                /**< Not in use */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *                       tempNoncontPtr;           /* remove when both COMM and APP side use contiguous flags */
#endif
	CiPassword password;                                    /**< Not in use */
#endif

} CiPbPrimSelectPhonebookReq;

/*<INUSE>*/
/** <paramref name="CI_PB_PRIM_SELECT_PHONEBOOK_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiPbPrimSelectPhonebookCnf_struct {
	CiPbResultCode Result;          /**< Result code \sa CiPbResultCode */
} CiPbPrimSelectPhonebookCnf;

typedef CiEmptyPrim CiPbPrimReadFirstPhonebookEntryReq;

//ICAT EXPORTED STRUCT
typedef struct CiPbPrimReadFirstPhonebookEntryCnf_struct {
	CiPbResultCode Result;          /**< Result code \sa CiPbResultCode */
	UINT16 Index;
#ifndef CCI_PB_CONTIGUOUS
	CiPhonebookEntry  *Entry;
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr; /* remove when both COMM and APP side use contiguous flags */
#endif
	CiPhonebookEntry entry;
#endif

} CiPbPrimReadFirstPhonebookEntryCnf;

typedef CiEmptyPrim CiPbPrimReadNextPhonebookEntryReq;

//ICAT EXPORTED STRUCT
typedef struct CiPbPrimReadNextPhonebookEntryCnf_struct {
	CiPbResultCode Result;          /**< Result code \sa CiPbResultCode */
	UINT16 Index;
#ifndef CCI_PB_CONTIGUOUS
	CiPhonebookEntry  *Entry;
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr; /* remove when both COMM and APP side use contiguous flags */
#endif
	CiPhonebookEntry entry;
#endif

} CiPbPrimReadNextPhonebookEntryCnf;

//ICAT EXPORTED STRUCT
typedef struct CiPbPrimFindFirstPhonebookEntryReq_struct {
	INT8 FindText[CIPB_MAX_NAME_LENGTH + 1]; /* Allow space for null terminator */
}CiPbPrimFindFirstPhonebookEntryReq;

//ICAT EXPORTED STRUCT
typedef struct CiPbPrimFindFirstPhonebookEntryCnf_struct {
	CiPbResultCode Result;          /**< Result code \sa CiPbResultCode */
	UINT16 Index;
#ifndef CCI_PB_CONTIGUOUS
	CiPhonebookEntry  *Entry;
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr; /* remove when both COMM and APP side use contiguous flags */
#endif
	CiPhonebookEntry entry;
#endif

} CiPbPrimFindFirstPhonebookEntryCnf;

//ICAT EXPORTED STRUCT
typedef struct CiPbPrimFindNextPhonebookEntryReq_struct {
	INT8 FindText[ CIPB_MAX_NAME_LENGTH + 1 ]; /* Allow space for null terminator */
}CiPbPrimFindNextPhonebookEntryReq;

//ICAT EXPORTED STRUCT
typedef struct CiPbPrimFindNextPhonebookEntryCnf_struct {
	CiPbResultCode Result;          /**< Result code \sa CiPbResultCode */
	UINT16 Index;
#ifndef CCI_PB_CONTIGUOUS
	CiPhonebookEntry  *Entry;
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr; /* remove when both COMM and APP side use contiguous flags */
#endif
	CiPhonebookEntry entry;
#endif

} CiPbPrimFindNextPhonebookEntryCnf;

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_ADD_PHONEBOOK_ENTRY_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiPbPrimAddPhonebookEntryReq_struct {
#ifndef CCI_PB_CONTIGUOUS
	CiPhonebookEntry      *Entry;           /**< New phonebook entry contents \sa CiPhonebookEntry_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *                       tempNoncontPtr;           /* Remove when both COMM and APP side use contiguous flags */
#endif
	CiPhonebookEntry entry;                                 /**< New phonebook entry contents \sa CiPhonebookEntry_struct */
#endif

}CiPbPrimAddPhonebookEntryReq;

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_ADD_PHONEBOOK_ENTRY_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiPbPrimAddPhonebookEntryCnf_struct {
	CiPbResultCode Result;                                          /**< Result code \sa CiPbResultCode */
	UINT16 Index;                                                   /**< Location (index) of new entry */
#ifndef CCI_PB_CONTIGUOUS
	CiPhonebookInfo               *Info;                            /**< Updated phonebook information \sa CiPhonebookInfo_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8                                         *tempNoncontPtr;  /* Remove when both COMM and APP side use contiguous flags */
#endif
	CiPhonebookInfo info;                                           /**< Updated phonebook information \sa CiPhonebookInfo_struct */
#endif

} CiPbPrimAddPhonebookEntryCnf;

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_DELETE_PHONEBOOK_ENTRY_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiPbPrimDeletePhonebookEntryReq_struct {
	UINT16 Index;   /**< Entry number (index) [1..number of entries in phonebook] */
}CiPbPrimDeletePhonebookEntryReq;

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_DELETE_PHONEBOOK_ENTRY_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiPbPrimDeletePhonebookEntryCnf_struct {
	CiPbResultCode Result;          /**< Result code \sa CiPbResultCode */
#ifndef CCI_PB_CONTIGUOUS
	CiPhonebookInfo   *Info;        /**< Updated phonebook information \sa CiPhonebookInfo_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr;        /* remove when both COMM and APP side use contiguous flags */
#endif
	CiPhonebookInfo info;                   /**< Updated phonebook information \sa CiPhonebookInfo_struct */
#endif
} CiPbPrimDeletePhonebookEntryCnf;

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_REPLACE_PHONEBOOK_ENTRY_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiPbPrimReplacePhonebookEntryReq_struct {
	UINT16 Index;                                   /**< Entry number (index) [1..number of entries in phonebook] */
#ifndef CCI_PB_CONTIGUOUS
	CiPhonebookEntry      *Entry;                   /**< New phonebook entry data \sa CiPhonebookEntry_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8                          *tempNoncontPtr; /* remove when both COMM and APP side use contiguous flags */
#endif
	CiPhonebookEntry entry;                         /**< New phonebook entry data \sa CiPhonebookEntry_struct */
#endif

}CiPbPrimReplacePhonebookEntryReq;

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_REPLACE_PHONEBOOK_ENTRY_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiPbPrimReplacePhonebookEntryCnf_struct {
	CiPbResultCode Result;                  /**< Result code \sa CiPbResultCode */
	UINT16 Index;                           /**< Entry number (index) [1..number of entries in phonebook] */
#ifndef CCI_PB_CONTIGUOUS
	CiPhonebookInfo       *Info;            /**< Updated phonebook information \sa CiPhonebookInfo_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr;        /* remove when both COMM and APP side use contiguous flags */
#endif
	CiPhonebookInfo info;                   /**< Updated phonebook information \sa CiPhonebookInfo_struct */
#endif
} CiPbPrimReplacePhonebookEntryCnf;

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_READ_PHONEBOOK_ENTRY_REQ"> */
/**  -
 *
 */
//ICAT EXPORTED STRUCT
typedef struct CiPbPrimReadPhonebookEntryReq_struct {
	UINT16 Index;                   /**< Index (entry number) [1..number of entries] */
} CiPbPrimReadPhonebookEntryReq;

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_READ_PHONEBOOK_ENTRY_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiPbPrimReadPhonebookEntryCnf_struct {
	CiPbResultCode Result;                                  /**< Result code \sa CiPbResultCode */
	UINT16 Index;                                           /**< Index (entry number) [1..number of entries] */
#ifndef CCI_PB_CONTIGUOUS
	CiPhonebookEntry      *Entry;                           /**< Phonebook entry contents \sa CiPhonebookEntry_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8                                 *tempNoncontPtr;  /* remove when both COMM and APP side use contiguous flags */
#endif
	CiPhonebookEntry entry;                                 /**< Phonebook entry contents \sa CiPhonebookEntry_struct */
#endif

} CiPbPrimReadPhonebookEntryCnf;

/*
 * AT&T- Smart Card  ENHANCED_PHONEBOOK_ENTRY ; -BT
 * <CDR-SMCD-912> <CDR-SMCD-913> <CDR-SMCD-914> <CDR-SMCD-918> <CDR-SMCD-917>
 */
#define CI_AAS_LIST_SIZE 10     /* LM_AAS_LIST_SIZE, SIM_MAX_AAS_LIST- have to be increased too */
#define CI_GAS_LIST_SIZE 10     /* LM_GAS_LIST_SIZE, SIM_MAX_GAS_LIST- have to be increased too */
#define NUM_OF_CATEGORY_NAMES CI_AAS_LIST_SIZE
#define NUM_OF_GROUP_NAMES CI_GAS_LIST_SIZE

/** \brief Enhanced phonebook information element type values */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CI_PBInformationElement {
	CI_PB_IE_EMAIL = 0,                     /**< Email address */
	CI_PB_IE_ANR,                           /**< Additional number*/
	CI_PB_IE_GRP,                           /**< Associated group index */
	CI_PB_IE_SN,                            /**< Second name */
	CI_PB_IE_ALIGN_8BIT = 0xff
}_PBInformationElementType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Enhanced phonebook information element type
 * \sa CI_PBInformationElement */
/** \remarks Common Data Section */
typedef UINT8 PBInformationElementType;
/**@}*/


/** \brief Additional number address information */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiAnrAddressInfo_struct
{
	CiAddrNumType typeOfNumber;                     /**< TON (type of number) \sa  CCI API Ref Manual*/
	CiAddrNumPlan numberPlan;                       /**<  NPI (numbering plan identification) \sa CCI API Ref Manual*/
	CiLongAdrInfo AnrDial;                          /**<  Dial number  \sa CCI API Ref Manual*/
}
CiAnrAddressInfo;

/** \brief Additional number information element */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiPbAdditionalNumberEntryType_struct {
	CiAnrAddressInfo number;                /**< Additional number address information \sa CiAnrAddressInfo_struct*/
	UINT8 CategoryIndex;                    /**< Associated category index. The category name can be read using CI_PB_PRIM_READ_CATEGORY_NAME_REQ. */
} CiPbAdditionalNumberEntryType;


/* <INUSE> */
/** <paramref name="CI_PB_PRIM_EDIT_ENH_PBK_ENTRY_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct  CiPbPrimEditEnhPbkEntryReq_struct {
	UINT16 Index;                                                           /**<  Phone book entry index (AdnRecordNumber) */
	CiEditCmdType EditCommand;                                              /**<  Edit command  \sa CCI API Ref Manual*/

	PBInformationElementType PBInformationElement;                          /**< Information element type \sa PBInformationElementType */

	UINT8 emailEntry;                                                       /**< Email entry index (0 or 1). This field is valid only if PBInformationElement has the value CI_PB_IE_EMAIL. */
	CiLongAdrInfo EmailAddress;                                             /**<  Email Address. This field is valid only if PBInformationElement has the value CI_PB_IE_EMAIL. \sa CCI API Ref Manual*/

	UINT8 anrEntry;                                                         /**<  Additional number entry index (0 or 1). This field is valid only if PBInformationElement has the value CI_PB_IE_ANR. */
	CiPbAdditionalNumberEntryType AdditionalNumber;  /**< Additional number. This field is valid only if PBInformationElement has the value CI_PB_IE_ANR. \\sa CiPbAdditionalNumberEntryType_struct*/

	UINT16 GroupAssociationIndex;                                         /**< Associated group index. This field is valid only if PBInformationElement has the value CI_PB_IE_GRP.
									       * The group name can be edited using CI_PB_PRIM_EDIT_GROUP_NAME_REQ.   */
	CiNameInfo SecondName;                                                  /**< Second name. This field is valid only if PBInformationElement has the value CI_PB_IE_SN.  \sa CCI API Ref Manual*/

} CiPbPrimEditEnhPbkEntryReq;

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_EDIT_ENH_PBK__ENTRY_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct  CiPbPrimEditEnhPbkEntryCnf_struct {
	CiPbResultCode Result;                                                          /**< Result code \sa CiPbResultCode */
	UINT16 Index;                                                                   /**<  Phone book entry index (AdnRecordNumber)*/
	PBInformationElementType PBInformationElement;                                  /**< Information element type \sa PBInformationElementType */

	UINT8 emailEntry;                                                               /**< Email entry index (0 or 1). This field is valid only if PBInformationElement has the value CI_PB_IE_EMAIL. */
	UINT8 anrEntry;                                                                 /**<  Additional number entry index (0 or 1). This field is valid only if PBInformationElement has the value CI_PB_IE_ANR. */
} CiPbPrimEditEnhPbkEntryCnf;


/* <INUSE> */
/** <paramref name="CI_PB_PRIM_READ_ENH_PBK_ENTRY_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct  CiPbPrimReadEnhPbkEntryReq_struct {
	UINT16 Index;                                                                           /**<  Phone book entry index (ADN record number)*/
	PBInformationElementType PBInformationElement;                                          /**< Information element \sa PBInformationElementType */
	UINT8 emailEntry;                                                                       /**< Email entry index (0 or 1). This field is valid only if PBInformationElement has the value CI_PB_IE_EMAIL. */
	UINT8 anrEntry;                                                                         /**<  Additional number entry index (0 or 1). This field is valid only if PBInformationElement has the value CI_PB_IE_ANR. */
} CiPbPrimReadEnhPbkEntryReq;

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_READ_ENH_PBK_ENTRY_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct  CiPbPrimReadEnhPbkEntryCnf_struct {
	CiPbResultCode Result;                                                          /**< Result code \sa CiPbResultCode */
	UINT16 Index;                                                                   /**<  Phone book entry index (ADN record number)*/
	PBInformationElementType PBInformationElement;                                  /**< Information element type \sa PBInformationElementType */

	UINT8 emailEntry;                                                               /**< Email entry index (0 or 1). This field is valid only if PBInformationElement has the value CI_PB_IE_EMAIL. */
	CiLongAdrInfo EmailAddress;                                                     /**<  Email Address. This field is valid only if PBInformationElement has the value CI_PB_IE_EMAIL. \sa CCI API Ref Manual*/

	UINT8 anrEntry;                                                                 /**<  Additional number entry index (0 or 1). This field is valid only if PBInformationElement has the value CI_PB_IE_ANR. */
	CiPbAdditionalNumberEntryType AdditionalNumber;                                 /**< Additional number. This field is valid only if PBInformationElement has the value CI_PB_IE_ANR. \sa CiPbAdditionalNumberEntryType_struct*/

	UINT16 GroupAssociationIndex;                                           /**< Associated group index. This field is valid only if PBInformationElement has the value CI_PB_IE_GRP.
										 * The group name can be read using CI_PB_PRIM_READ_GROUP_NAME_REQ.   */
	CiNameInfo SecondName;                                                          /**< Second name. This field is valid only if PBInformationElement has the value CI_PB_IE_SN.  \sa CCI API Ref Manual*/

} CiPbPrimReadEnhPbkEntryCnf;

/*
 * AT&T- Smart Card  <CDR-SMCD-941> <CDR-SMCD-916>  PB  CI_PB CategoryName- EFAas -BT2
 */


/* <INUSE> */
/** <paramref name="CI_PB_PRIM_READ_CATEGORY_NAME_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct  CiPbPrimReadCategoryNameReq_struct {
	UINT8 Index;   /**< Category name file index (AAS record number). Index value '0' means read all entries. */
}CiPbPrimReadCategoryNameReq;


/* <INUSE> */
/** <paramref name="CI_PB_PRIM_READ_CATEGORY_NAME_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct  CiPbPrimReadCategoryNameCnf_struct {
	CiPbResultCode Result;                                                  /**< Result code \sa CiPbResultCode */
	UINT8 Index;                                                            /**< Category name file index (AAS record number). Index value '0' means all entries are provided. */
	UINT8 Num;                                                              /**< Number of category names. This field is valid if index = '0'. */
	UINT8 RecordNumber [NUM_OF_CATEGORY_NAMES];                             /**< Category name file record number (Index in EFAAS) */
	CiNameInfo CategoryName[NUM_OF_CATEGORY_NAMES];                         /**< Category name \sa CCI API Ref Manual */
}CiPbPrimReadCategoryNameCnf;

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_EDIT_CATEGORY_NAME_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct  CiPbPrimEditCategoryNameReq_struct {
	UINT8 Index;                                    /**< Category name file index (AAS record number) */
	CiEditCmdType EditCommand;                      /**< Edit command  \sa CCI API Ref Manual*/
	CiNameInfo CategoryName;                        /**< Category name \sa CCI API Ref Manual */
}CiPbPrimEditCategoryNameReq;

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_EDIT_CATEGORY_NAME_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct  CiPbPrimEditCategoryNameCnf_struct {
	CiPbResultCode EditResult;                      /**< Result code \sa CiPbResultCode */
	UINT8 Index;                                    /**< Category name file record number (Index in EFAAS) */
}CiPbPrimEditCategoryNameCnf;



/*
 * AT&T- Smart Card  <CDR-SMCD-941> PB     CI_PB Read group names ( EFGAS.) -BT3
 */

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_READ_GROUP_NAME_REQ" >*/
//ICAT EXPORTED STRUCT
typedef struct  CiPbPrimReadGroupNameReq_struct {
	UINT8 Index;   /**< Group name file index (GAS record number). Index value '0' means read all entries. */

}CiPbPrimReadGroupNameReq;

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_READ_GROUP_NAME_CNF" >*/
//ICAT EXPORTED STRUCT
typedef struct  CiPbPrimReadGroupNameCnf_struct {
	CiPbResultCode Result;                                  /**< Result code \sa CiPbResultCode */
	UINT8 Index;                                            /**< Group name file index (GAS record number). Index value '0' means read all entries are provided. */
	UINT8 Num;                                              /**< Number of group names. This field is valid if index = '0'.  */
	UINT8 RecordNumber [NUM_OF_GROUP_NAMES];                /**< Group name file record number (Index in EFGAS) */
	CiNameInfo GroupName   [NUM_OF_GROUP_NAMES];            /**< Group name \sa CCI API Ref Manual */
}CiPbPrimReadGroupNameCnf;

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_EDIT_GROUP_NAME_REQ" >*/
//ICAT EXPORTED STRUCT
typedef struct  CiPbPrimEditGroupNameReq_struct {
	UINT8 Index;                                                    /**< Group name file index (GAS record number) */
	CiEditCmdType EditCommand;                                      /**<  Edit command  \sa CCI API Ref Manual*/
	CiNameInfo GroupName;                                           /**< Group name \sa CCI API Ref Manual */
}CiPbPrimEditGroupNameReq;

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_EDIT_GROUP_NAME_CNF" > */
//ICAT EXPORTED STRUCT
typedef struct  CiPbPrimEditGroupNameCnf_struct {
	CiPbResultCode EditResult;                              /**< Result code \sa CiPbResultCode */
	UINT8 Index;                                            /**< Group name file index (GAS record number) */
}CiPbPrimEditGroupNameCnf;
/* Michal Bukai & -BT AT&T Smart Card support - End */
/*****************************************************************/
/* The maximum size of the PhoneBook Identity */
#define CI_MAX_IDENTITY_LENGTH  15

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_GET_IDENTITY_REQ"> */
typedef CiEmptyPrim CiPbPrimGetIdentityReq;

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_GET_IDENTITY_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiPbPrimGetIdentityCnf_struct {
	CiPbResultCode Result;          /**< Result code \sa CiPbResultCode */
	UINT8 Identity[CI_MAX_IDENTITY_LENGTH];
} CiPbPrimGetIdentityCnf;

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_PHONEBOOK_READY_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiPbPrimPhoneBookReadyInd_struct {

	CiBoolean PbReady;                      /**< TRUE if phoneBook is ready to be used; FALSE otherwise \sa CCI API Ref Manual */

} CiPbPrimPhoneBookReadyInd;

/* <INUSE> */
/** <paramref name="CI_PB_PRIM_GET_ALPHA_NAME_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiPbPrimGetAlphaNameReq_struct {
#ifndef CCI_PB_CONTIGUOUS
	CHAR            *Digits;                                        /**< Null-terminated string of the requested number */
#else
	CHAR digits[CI_MAX_ADDRESS_LENGTH];                             /**< Null-terminated string of the requested number */
#endif

} CiPbPrimGetAlphaNameReq;
/** <paramref name="CI_PB_PRIM_GET_ALPHA_NAME_CNF"> */

//ICAT EXPORTED STRUCT
typedef struct CiPbPrimGetAlphaNameCnf_struct {

	CiPbResultCode Result;          /**< Result code \sa CiPbResultCode */
	CiNameInfo Name;                /**< Name tag found for the requested number \sa CCI API Ref Manual */

} CiPbPrimGetAlphaNameCnf;

#ifdef CI_CUSTOM_EXTENSION
/* It is assumed that only one customized set of extension primitives is
 * to be considered at one time. The selection of the particular customized
 * set is done in the 'ci_pb_cust.h' based on compile flags.
 *
 * Note that if no customer extension primitives are defined for this service group,
 * CI_PB_NUM_CUST_PRIM is set to 0 in the "ci_pb_cust.h" file.
 */
#include "ci_pb_cust.h"

#define CI_PB_NUM_PRIM ( CI_PB_NUM_COMMON_PRIM + CI_PB_NUM_CUST_PRIM )

#else

/* if no customer extension is supported, only the default common set is considered */
#define CI_PB_NUM_PRIM CI_PB_NUM_COMMON_PRIM

#endif /* CI_CUSTOM_EXTENSION */

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  /* _CI_PB_H_ */


/*                      end of ci_pb.h
   --------------------------------------------------------------------------- */

