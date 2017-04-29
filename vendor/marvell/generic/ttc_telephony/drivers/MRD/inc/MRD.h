// (C) Copyright 2006 Marvell International Ltd.
// All Rights Reserved
//

/*------------------------------------------------------------------------------
 *  ----------------------------------------------------------------------------
 *
 *  Filename: MRD.h
 *
 *  Description: The APIs to handle MRD file
 *
 *  History:
 *   June, 2010 - Shuki Zanyovka
 *
 *  Notes:
 *
 ******************************************************************************/

#if !defined (MRD_HEADER_FILE_H)
#define MRD_HEADER_FILE_H

#if defined(_WIN32)
#define UNIT_UNDER_TEST_VC
#define UINT8	unsigned char
#endif// WIN32

#if !defined(UNIT_UNDER_TEST_VC)
#include "global_types.h"
#endif // UNIT_UNDER_TEST_VC

//Type of Reliable Data Components
typedef enum
{
	MRD_IMEI_TYPE  		=	0xCAFE0010,
	MRD_MEP_TYPE		=	0xCAFE0015,
	MRD_CDF_TYPE		=	0xCAFE0020,
	MRD_SN_TYPE			=	0xCAFE0025,
	MRD_GEN_DATA_TYPE	=	0xCAFE0030,
	MRD_ADF_TYPE   		=	0xAAAA0035
} MRD_FILE_TYPES;


typedef enum {
	MRD_NO_ERROR,
	MRD_ERROR,
	MRD_FILE_NOT_FOUND_ERROR,
	MRD_FILE_ALREADY_EXISTS_ERROR,
	MRD_FILE_WRITE_ERROR,
	MRD_FILE_READ_ERROR,
	MRD_NOT_INITIALIZED_ERROR,
	MRD_NO_VALID_STAMP_ERROR,
	MRD_INVALID_HEADER_ERROR,
	MRD_INVALID_FILE_HEADER_ERROR,
	MRD_INVALID_CHECKSUM_ERROR,
	MRD_NO_MEMORY_ERROR,
	MRD_ALREADY_INITIALIZED_ERROR,
	MRD_FILENAME_TOO_LONG,
	MRD_NO_SPACE_ERROR,
	MRD_INVALID_END_OF_FILE_ERROR,
	MRD_FLASH_TYPE_ERROR,
	MRD_VERIFY_ERROR1,
	MRD_VERIFY_ERROR2,
	MRD_VERIFY_ERROR3,
	MRD_VERIFY_ERROR4,
	MRD_VERIFY_ERROR5,
	MRD_VERIFY_ERROR6,
	MRD_VERIFY_ERROR7
} MRDErrorCodes;

/*The flash type */
#define OBM_TYAX_FLASH		1
#define OBM_SIBLEY_FLASH	2
#define OBM_NAND_FLASH		3
#define OBM_ONENAND_FLASH	4
#define OBM_MSYS_DOC_FLASH	5
#define OBM_SDMMC_FLASH     6
#define OBM_UNKNOWN_FLASH	7

#define FLASH_TYPE_KERN_CMDLINE "FLAS="


#define MRD_FILE_FLAG_ENCRYPTED	0x1
#define MAX_MRD_FILENAME_SIZE 128

#define MRD_JOURNAL_FILENAME   			"/data/log/mrd_journal.txt"
//#define MRD_RAW_BLOCK_READ_WRITE_CMD	"dd if=%s of=%s bs=%d count=%d skip=%d"
#define MRD_RAW_BLOCK_READ_CMD_MMC 		"dd if=%s of=%s bs=%d count=%d skip=%d ; echo $? > /tmp/MRD_tmp_retval.bin"
#define MRD_RAW_BLOCK_WRITE_CMD_MMC		"dd if=%s of=%s bs=%d count=%d seek=%d ; echo $? > /tmp/MRD_tmp_retval.bin"
#define BLOCK_SIZE_MMC					(512)

#define MRD_RAW_BLOCK_READ_CMD_MTD 		"dd if=%s of=%s bs=%d count=%d seek=%d ; echo $? > /tmp/MRD_tmp_retval.bin"
#define MRD_RAW_BLOCK_WRITE_CMD_MTD		"dd if=%s of=%s bs=%d count=%d seek=%d ; echo $? > /tmp/MRD_tmp_retval.bin"
#define BLOCK_SIZE_MTD					(1024)

#define MRD_IMAGE_TOTAL_SIZE		   	(128*1024)
#define MRD_FILENAME_AND_PATH_MAX_SZ    512

#define MRD_BLOCK_OFFSET_MMC   			0x0	    		// ************ Temporary - Shuki TO FIX THIS !!!!
#define MRD_BLOCK_OFFSET_MTD   			0x0	    		// ************ Temporary - Shuki TO FIX THIS !!!!

#define MRD_PARTITION_NAME_MTD 			"imei"
#define MRD_PARTITION_NUM_MMC  			3

#define MRD_PARTITION_PATTERN_MTD 		"/dev/block/mtdblock%d"
#define MRD_PARTITION_PATTERN_MMC 		"/dev/block/mmcblk0p%d"


//#define MRD_DD_BLOCK_SZ					(512)


#if !defined(UNIT_UNDER_TEST_VC)
#define MRD_TMP_FILENAME		  	 	"/tmp/MRD.bin"
#define MRD_TMP_FILENAME_VERIFY	  	 	"/tmp/MRD_verify.bin"
#define MRD_DEBUG_FILENAME				"/data/MRD.bin"
#define MRD_DEBUG_FILENAME_VERIFY		"/data/MRD_verify.bin"
#else
#define MRD_TMP_FILENAME				"C:\\Versions\\MRD\\sample\\176148.mrd"
#endif // UNIT_UNDER_TEST_VC

#if !defined(_WIN32)
#define MRD_SLASH						'/'
#else
#define MRD_SLASH						'\\'
#endif// _WIN32


//sprintf(buf, "dd if=%s of=%s bs=1024 count=128 seek=%d", RDA_file_name, RDA_MTD_PARTITION, RDAFileStruct_ptr->block_offset);
//sprintf(buf, "dd of=%s if=%s bs=1024 count=128 skip=%d", RDA_file_name, RDA_MTD_PARTITION, RDAFileStruct_ptr->block_offset);
//sprintf(buf, "dd if=%s of=%s bs=1024 count=128 seek=%d", FFFF_FILE, RDA_MTD_PARTITION, RDAFileStruct_ptr->block_offset);

MRDErrorCodes MRDInit(void);

MRDErrorCodes MRDClose(void);

MRDErrorCodes MRDFileAdd(char *MRD_filename,
						 UINT32 flags,
						 UINT32 mrd_file_type,
						 UINT32 file_format_version,
						 UINT32 timestamp,
						 char *input_filename);

MRDErrorCodes MRDFileRead(char *MRD_filename,
						  UINT32 mrd_file_type,
						  UINT32 *flags,
						  UINT32 *file_format_version,
						  UINT32 *timestamp,
						  char *output_filename);

MRDErrorCodes MRDFileExist(char *MRD_filename, UINT32 mrd_file_type);

MRDErrorCodes MRDFileRemove(char *MRD_filename, UINT32 MRD_file_type);

MRDErrorCodes MRDFileDirFirst(char *MRD_filename,
							  UINT32 *flags,
							  UINT32 *file_format_version,
							  UINT32 *timestamp);

MRDErrorCodes MRDFileDirNext(char *MRD_filename,
							 UINT32 *flags,
 							 UINT32 *file_format_version,
							 UINT32 *timestamp);


#endif //MRD_HEADER_FILE_H

