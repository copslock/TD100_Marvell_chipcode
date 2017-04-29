#define IMEI_NUMBER_SIZE                 16                                 // 16 bytes
#define USER_DATA_SIZE                   16                                 // 16 bytes
#define IMEI_SUB_KEY_SIZE                26                                 // Longs == 104 bytes
#define IMEI_USER_KEY_SIZE            	 26                                 // Longs == 104 bytes

typedef enum
{
	IMEI_IS_NOT_ENC = 0,
	IMEI_IS_ENC
} IMEI_DEC_ENC;

typedef struct
{
    char   digits[16];
} IMEIData;

typedef struct
{
	unsigned char               DeycrptNumber[IMEI_NUMBER_SIZE];
    unsigned char               UserData[USER_DATA_SIZE];
    unsigned char               EncryptNumber[IMEI_NUMBER_SIZE];
    unsigned long               UserKey[IMEI_USER_KEY_SIZE];              // 104 bytes
    unsigned long               SubKey[IMEI_SUB_KEY_SIZE];             // 104 bytes
	unsigned char               ImeiEncrypt;                     /* this will be set to 'IMEI_IS_ENC' in case the EncryptNumber
																  *  is valid */
} IMEIExtendedData;

typedef struct
{
// CD = Check Digit , SV = Software Version

// IMEI(14 digits)+ CD(1 digit), SV(2 digits)
    char   imei_digits[16];
	char   sv_digits[3];
} IMEIStr;






