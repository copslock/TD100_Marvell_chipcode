/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/


#ifndef _MEP__H_
#define _MEP__H_

/**************************************************************************************/
/**************************************************************************************/

//#ifndef ON_PC
//#include "utils.h"
//#else
//#include "gbl_types.h"
//#endif

/**************************************************************************************/
/**************************************************************************************/

#define MEP_64_BITS						2

#define MEP_TC_DEFAULT_VALUE  			10
#define MEP_PASSWORD_LEN				16
#define MEP_SIGNATURE_SIZE				20			// 20 Bytes means 160 bits
#define UDP_SIGNATURE_SIZE				20			// 20 Bytes means 160 bits


#define UDP_SI_DEFAULT_VALUE  			0
#define UDP_ASL_DEFAULT_VALUE  			0
#define UDP_PSW_DEFAULT_VALUE  			0
#define UDP_PSW_LEN_DEFAULT_VALUE		4

#define UDP_MAX_ASL						10
#define UDP_MAX_ASL_ICCID				10			// in bytes
#define UDP_MAX_ASL_DATE				8			// in bytes
#define UDP_ASL_ICCID_LEN				10
#define MEP_NUM_MEP_SUBSET_DIGITS       1
#define MEP_MAX_IMSI_LENGTH  			8
#define MEP_NUM_MEP_NETWORKS     		3

/**************************************************************************************/
/**************************************************************************************/


typedef enum
{
	MEP_LSB,
	MEP_MSB
} MEP_SIGNIFICANT_BITs;


typedef enum
{
	MEP_FALSE,
	MEP_TRUE
} MEP_BOOL;



typedef enum
{
	MEP_DB_FIX,
	MEP_DB_MEP,
	MEP_DB_UDP
} MEP_DB_TYPE;


typedef enum
{
	MEP_UDP_RC_OK,
	MEP_UDP_RC_NOT_OK,
	MEP_UDP_RC_NOT_SUPPORT,
	MEP_UDP_RC_FILE_INTEGRITY_FAILED,
	MEP_UDP_RC_FILE_CONFIDENTIALITY_FAILED
#ifdef MEP_GLOBAL_VAR
	,
	MEP_UDP_RC_MESSAGE_QUEUE_IS_FULL
#endif
} MEP_UDP_RC;


typedef enum
{
	MEP_FIX_RC 			= 0x2000,                // These three enum's will be OR operation with the above
	MEP_CHANGEABLE_RC 	= 0x4000,                // MEP and UDP enum's to identify the source of problem.
	UDP_CHANGEABLE_RC 	= 0x8000
} MEP_RC_GENERAL;



typedef enum
{
	MEP_CAT_NP,                      // Network personalization
	MEP_CAT_NSP,                     // Network subset personalization
	MEP_CAT_SP,                      // Service provider
	MEP_CAT_CP,                      // Corporate personalization
	MEP_CAT_SIM_LOCK,                // SIM/USIM personalization
	MEP_MAX_CAT
} MEP_CAT;


typedef enum
{
	MEP_CODE_ID_NP,                      // Network personalization
	MEP_CODE_ID_NSP,                     // Network subset personalization
	MEP_CODE_ID_SP,                      // Service provider
	MEP_CODE_ID_CP,                      // Corporate personalization
	MEP_CODE_ID_SIM_LOCK,                // SIM/USIM personalization
	MEP_MAX_CODE_ID_CAT
} MEP_CODE_ID;


typedef enum
{
	UDP_CAT_SD,                      // SIM Detection
	UDP_CAT_USD,                     // Unauthorized SIM Detection
	UDP_MAX_CAT
} UDP_CAT;



typedef enum
{
	MEP_SI_OFF,
	MEP_SI_ON,
	MEP_SI_DISABLE
} MEP_SI;


typedef enum
{
	UDP_SI_OFF,
	UDP_SI_ON,
} UDP_SI;



typedef enum
{
	MEP_ENCRYPT,
	MEP_DECRYPT
} MEP_ENC_DEC;


/**************************************************************************************/
/**************************************************************************************/

typedef UINT8  	MEP_BCD;    			/* 0-9 and 15 (for unused) */


typedef struct
{
  UINT8      	signature[MEP_SIGNATURE_SIZE];
} MEP_SIGNATURE;

//ICAT EXPORTED STRUCT
typedef struct
{
	UINT8		number[MEP_PASSWORD_LEN];
	UINT8		length;
} MEP_PASSWORD;

//ICAT EXPORTED STRUCT
typedef struct
{
	UINT16		year;
	UINT8		month;
	UINT8		day;
} MEP_DATE;


typedef UINT16 MEP_MNC;
typedef UINT16 MEP_MCC;
typedef UINT16 MEP_AccessTechnologyId;
typedef UINT8 MEP_Boolean;


typedef struct
{
	MEP_MCC		mcc;                 	// Mobile Country Code
	MEP_MNC		mnc;                 	// Mobile Network Code
	UINT16		AccessTechnology;
} MEP_PLMN;                          	// Public Land Mobile Network


typedef struct
{
	MEP_PLMN    	plmn;
	MEP_Boolean     mncThreeDigitsDecoding;  // Is three digit encoding used
} MEP_SIMPLMN;


typedef struct
{
  MEP_SIMPLMN  	networkIds[MEP_NUM_MEP_NETWORKS];
} MEP_CODE_PLMN_LIST;


typedef struct
{
  MEP_BCD		networkSubsetId[MEP_NUM_MEP_SUBSET_DIGITS];     /* Bits 0-3 = IMSI digit 6 ,  Bits 4-7 = IMSI digit 7 */
} MEP_CODE_NS;


typedef struct
{
  UINT8      	serviceproviderId;
} MEP_CODE_SP;


typedef struct
{
  UINT8      	corporateId;
} MEP_CODE_CP;



typedef struct
{
    UINT8        length;
    UINT8        contents[MEP_MAX_IMSI_LENGTH];
} MEP_IMSI;


typedef struct
{
  MEP_IMSI      simId;							/* store the raw IMSI (whole IMSI) */
} MEP_CODE_SIMUSIM;



/******************************************  Mep Fix Data  ********************************************/

typedef struct
{
	MEP_CODE_PLMN_LIST  	PlmnList;
	MEP_CODE_NS 	        NetworkSubset;
	MEP_CODE_SP 			Sp;
	MEP_CODE_CP 			Corporate;
	MEP_CODE_SIMUSIM 		SimUsim;
} MEP_CODE;


typedef struct
{
	MEP_CODE            code;
	UINT8				DefaultSiActivated;
	UINT8				DefaultSiEnabled;
} MEP_CAT_DATA;


typedef struct
{
	MEP_PASSWORD		CatPsw[MEP_MAX_CAT];
	MEP_PASSWORD		UnblockPsw;
	UINT8				TrialLimit;
} MEP_BLOCK_DATA;


typedef struct
{
	MEP_CAT_DATA   		data;                                      // !!! Run F8 And F9 on this field
	MEP_BLOCK_DATA		blocking;                                  // !!! Run F8 And F9 on this field
	UINT8      			signature[MEP_SIGNATURE_SIZE];             // This is the output of the F9 function
	UINT8				DataIsEncrypted;                           // Should be set to FALSE or TRUE -- do not run F8 | F9
} MEP_FIX;


#define MEP_FIX_SIZE	( offsetof(MEP_FIX, signature) )

/******************************************  Mep Changeable Data  *******************************************/


// Regarding the SI field -- it will be manage as bit wise
//                           0 = Not_activate or Disable,  1 = activate or Enable
// bit 0 -- Sim/Usim category
// bit 1 -- Network category
// bit 2 -- Network Subset category
// bit 3 -- Service Provider category
// bit 4 -- Corporate category

//ICAT EXPORTED STRUCT
typedef struct
{
	UINT8				SiActivated;
	UINT8				SiEnabled;
	UINT8				TrialCounter[MEP_MAX_CAT];
	UINT8				padding[15];
	UINT8      			signature[MEP_SIGNATURE_SIZE];
} MEP_CHANGEABLE;

#define MEP_CHANGEABLE_SIZE	( offsetof(MEP_CHANGEABLE, padding ))


/******************************************  Udp Changeable Data  *******************************************/
//ICAT EXPORTED STRUCT
typedef struct
{
	UINT8				iccid[UDP_MAX_ASL_ICCID];
	MEP_DATE			date;
} UDP_ASL_DATA;

//ICAT EXPORTED STRUCT
typedef struct
{
	UDP_ASL_DATA		data[UDP_MAX_ASL];
	UINT8				NumAslEntries;
} UDP_ASL;


//ICAT EXPORTED STRUCT
typedef struct
{
	MEP_PASSWORD		psw;
	UDP_ASL				asl;
	UINT8				si;
	UINT8				padding[15];
	UINT8      			signature[UDP_SIGNATURE_SIZE];
} UDP_CHANGEABLE;

#define UDP_CHANGEABLE_SIZE	( offsetof(UDP_CHANGEABLE, padding ))


/*********************************************  Mep Main Database   *******************************************/

typedef struct
{
	MEP_FIX					MepFix;
	MEP_CHANGEABLE			MepChangeable;
	UDP_CHANGEABLE     		UdpChangeable;
	MEP_UDP_RC				MepErrorCode;
	MEP_UDP_RC	 			UdpErrorCode;
} MEP_DATA_BASE;



/**************************************************************************************/
/**************************************************************************************/

// Platform API

MEP_UDP_RC MEPInit ( MEP_FIX   *MepDdrBuffaddr );
MEP_UDP_RC UdpInit ( void );
MEP_UDP_RC MEPPhase1Iinit ( void );
MEP_UDP_RC MEPPhase2Iinit ( void );

/**************************************************************************************/
/**************************************************************************************/


////////////////////////////
// callbacks definition
////////////////////////////

typedef  void  (*MEP_Callback)(MEP_UDP_RC*);


typedef union
{
    void    (*MEP_PutTC_Callback)(MEP_UDP_RC*) ;
    void    (*MEP_PutSI_Callback)(MEP_UDP_RC*) ;
    void    (*UDP_PutSI_Callback)(MEP_UDP_RC*) ;
    void    (*UDP_PutASL_Callback)(MEP_UDP_RC*) ;
    void    (*UDP_PutPassword_Callback)(MEP_UDP_RC*) ;
} MEP_UDP_CallBackFuncS;


/**************************************************************************************/
/**************************************************************************************/

// L1 API

MEP_UDP_RC MEP_GetCK ( MEP_CAT   category , MEP_PASSWORD   *ck );
MEP_UDP_RC MEP_GetGC ( MEP_CODE_ID   codeId , MEP_CODE   *gc );
MEP_UDP_RC MEP_GetTC ( MEP_CAT   category , UINT8   *tc );
MEP_UDP_RC MEP_GetSI ( UINT8   *SiActivated , UINT8   *SiEnabled );
MEP_UDP_RC MEP_GetTL ( UINT8    *tl );
MEP_UDP_RC ME_GetUPW ( MEP_PASSWORD     *Upsw );

MEP_UDP_RC MEP_PutTC ( UINT8   *tc , MEP_CAT     category , MEP_UDP_CallBackFuncS 	*CallBackFunc );
MEP_UDP_RC MEP_PutSI ( UINT8   *SiActivated , UINT8   *SiEnabled , MEP_UDP_CallBackFuncS 	*CallBackFunc );


MEP_UDP_RC UDP_GetSI ( UINT8   *SiActivated );
MEP_UDP_RC UDP_GetASL ( UDP_ASL   *asl );
MEP_UDP_RC UDP_GetPassword ( MEP_PASSWORD     *psw );


MEP_UDP_RC UDP_PutSI ( UINT8   *SiActivated , MEP_UDP_CallBackFuncS 	*CallBackFunc );
MEP_UDP_RC UDP_PutASL ( UDP_ASL   *asl , MEP_UDP_CallBackFuncS 	*CallBackFunc );
MEP_UDP_RC UDP_PutPassword ( MEP_PASSWORD     *psw , MEP_UDP_CallBackFuncS 	*CallBackFunc );

/**************************************************************************************/
/**************************************************************************************/
#endif //_MEP__H_
