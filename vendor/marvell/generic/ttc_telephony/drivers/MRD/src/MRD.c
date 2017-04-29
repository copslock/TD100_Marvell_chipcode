// (C) Copyright 2006 Marvell International Ltd.
// All Rights Reserved
//

/*------------------------------------------------------------------------------
 *  ----------------------------------------------------------------------------
 *
 *  Filename: MRD.c
 *
 *  Description: The APIs to handle MRD file
 *
 *  History:
 *   June, 2010 - Shuki Zanyovka
 *
 *  Notes:
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>

#include <limits.h> // for PATH_MAX
#include "utilities.h"
#include "pxa_dbg.h"
#include <unistd.h> // sync
#include <time.h> // usleep

#ifdef _MSC_VER  //all MS compilers define this (version)
     #define snprintf _snprintf
#endif

#if !defined(ASSERT)
#define ASSERT(x)
#endif //ASSERT

#include "MRD.h"
#include "MRD_structure.h"

#define MAX_KERNEL_CMDLINE_SZ 1024

typedef int (*EraseBlockFunc_t)(char *dev, unsigned int flash_offset);

static BOOL initialized = FALSE;
static unsigned char *MRD_file_buffer;
static char g_mrd_mtd_partition[PATH_MAX];
static char *g_raw_block_read_cmd = "";
static char *g_raw_block_write_cmd = "";
static UINT32 g_block_sz;
static UINT32 g_mrd_block_offset;
EraseBlockFunc_t g_erase_block_func;

static MRDErrorCodes MRDRawBlockWrite(void);

static MRDErrorCodes MRDRawBlockRead(char *filename);

//static MRDErrorCodes MRDTemporaryFileRemove(void);

static MRDErrorCodes MRDParseHeader(unsigned char *MRD_file_buffer);

static UINT32 MRDInternalFileOffsetGet(unsigned char *MRD_file_buffer,
										char *MRD_filename,
										UINT32 mrd_file_type,
										UINT32 *file_size);

static UINT32 MRDInternalFileHeaderOffsetGet(unsigned char *MRD_file_buffer,
												char *MRD_filename,
												UINT32 mrd_file_type,
												UINT32 *file_size);

static void MRDInitNewHeader(MRD_header_t *pMRD_hdr);

static UINT32 MRDChecksumCalc(UINT32 *pMRD_start,UINT32 *pMRD_end);

//static MRDErrorCodes MRDIsValidFileHeader(MRD_entry_header_t *pMRD_file_header);

static UINT32 MRDFileHeaderChecksumCalc(MRD_entry_header_t *pMRD_file_header);

static int flashEraseBlockMTD(char *dev, unsigned int flash_offset);
static int flashEraseBlockMMC(char *dev, unsigned int flash_offset);

/* We need the GLOBAL mutex which is not supported by BIONIC
 * Implement the "pseudo-mutex" by Exclusive-file open.
 * There is NO blocking but error so apply the usleep()
 */
#define MRD_TMP_MUTEXFILE			"/tmp/MRD_mutex.bin"
static int mutex_mrd = -1;

static mrd_mutex_lock(void)
{
	do {
		mutex_mrd = open(MRD_TMP_MUTEXFILE, O_CREAT | O_EXCL);
		if (mutex_mrd >= 0)
			break;
		usleep(100000);
	} while (mutex_mrd < 0);
}

static mrd_mutex_unlock(void)
{
	if (mutex_mrd >= 0) {
		close(mutex_mrd);
		mutex_mrd = -1;
		unlink(MRD_TMP_MUTEXFILE);
	}
}

/***********************************************************************
* Function: flashEraseBlockMTD	 				                       *
************************************************************************
* Description: Erase a MTD block									   *
*              				                                           *
* Parameters: none                                                     *
*              														   *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static int flashEraseBlockMTD(char *dev, unsigned int flash_offset)
{
	mtd_info_t meminfo;
	int fd;
	erase_info_t erase;
	int ret_val = 0;

	fd = open(dev, O_RDWR);
	if (fd == -1) {
		ret_val = -1;
		goto clean_up;
	}

	if (ioctl(fd, MEMGETINFO, &meminfo) != 0) {
		ret_val = -2;
		goto clean_up;
	}

	/* Initialize to write size */
	g_block_sz = meminfo.writesize;

	erase.start = flash_offset;
	erase.length = meminfo.erasesize;
	if (ioctl(fd, MEMERASE, &erase) != 0) {
		ret_val = -3;
		goto clean_up;
	}

clean_up:
	if (fd != -1)
		close(fd);
	if (ret_val != 0)
		DPRINTF("flashEraseBlockMTD(): Error - ret_val=%d\n", ret_val);

	return ret_val;
}

/***********************************************************************
* Function: flashEraseBlockMMC	 				                       *
************************************************************************
* Description: Erase a MMC block									   *
*              				                                           *
* Parameters: none                                                     *
*              														   *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static int flashEraseBlockMMC(char *dev, unsigned int flash_offset)
{
	UNUSEDPARAM(dev);
	UNUSEDPARAM(flash_offset);
	return 0;
}

/***********************************************************************
* Function: MRDFlashTypeInit	 				                       *
************************************************************************
* Description: Initializes the MRD module							   *
*              				                                           *
* Parameters: none                                                     *
*              														   *
*                                                                      *
* Return value: success or not                                         *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
MRDErrorCodes MRDFlashTypeInit(void)
{
	switch(get_flash_type())
	{
	case FLASH_TYPE_NAND:
		{
			int mtd_number = mtd_name_to_number(MRD_PARTITION_NAME_MTD);
			if(mtd_number < 0)
				return MRD_FLASH_TYPE_ERROR;
			sprintf(g_mrd_mtd_partition, MRD_PARTITION_PATTERN_MTD, mtd_number);
			g_raw_block_read_cmd = MRD_RAW_BLOCK_READ_CMD_MTD;
			g_raw_block_write_cmd = MRD_RAW_BLOCK_WRITE_CMD_MTD;
			g_block_sz = BLOCK_SIZE_MTD;
			g_mrd_block_offset = MRD_BLOCK_OFFSET_MTD;
			g_erase_block_func = flashEraseBlockMTD;
		}
		break;

	case FLASH_TYPE_EMMC:
		{
			sprintf(g_mrd_mtd_partition, MRD_PARTITION_PATTERN_MMC, MRD_PARTITION_NUM_MMC);
			g_raw_block_read_cmd = MRD_RAW_BLOCK_READ_CMD_MMC;
			g_raw_block_write_cmd = MRD_RAW_BLOCK_WRITE_CMD_MMC;
			g_block_sz = BLOCK_SIZE_MMC;
			g_mrd_block_offset = MRD_BLOCK_OFFSET_MMC;
			g_erase_block_func = flashEraseBlockMMC;
		}
		break;

	default:
		return MRD_FLASH_TYPE_ERROR;
	}

	return MRD_NO_ERROR;
}


/***********************************************************************
* Function: MRDInit								                       *
************************************************************************
* Description: Initializes the MRD module							   *
*              				                                           *
* Parameters: none                                                     *
*              														   *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
MRDErrorCodes MRDInit(void)
{
	MRDErrorCodes ret_val;
	MRD_header_t mrd_header;
	MRD_entry_header_t MRD_end_hdr;

	if (FALSE != initialized)
		return MRD_ALREADY_INITIALIZED_ERROR;

	mrd_mutex_lock();

	ret_val = MRDFlashTypeInit();
	if (ret_val != MRD_NO_ERROR) {
		mrd_mutex_unlock();
		return ret_val;
	}

	/* Allocate MRD file buffer */
	MRD_file_buffer = malloc(MRD_IMAGE_TOTAL_SIZE);
	if (MRD_file_buffer == NULL) {
		mrd_mutex_unlock();
		return MRD_NO_MEMORY_ERROR;
	}
	/* Read MRD block from flash */
	ret_val = MRDRawBlockRead(MRD_TMP_FILENAME);
	if (ret_val != MRD_NO_ERROR) {
		mrd_mutex_unlock();
		return ret_val;
	}
	/* Parse MRD header - is valid MRD? */
	ret_val = MRDParseHeader(MRD_file_buffer);

	if (ret_val == MRD_NO_ERROR) {
		DPRINTF("MRD read from flash successfully\n");
		initialized = TRUE;
	} else {
		/* No MRD signature in header? assume file is empty */
		if (ret_val == MRD_INVALID_HEADER_ERROR) {
			/* Shuki: TODO - Add a journal entry here !!!! */
			/* Create a new header */
			DPRINTF("Error: MRD read from flash has an invalid header\n");
			DPRINTF("Creating a new MRD header...\n");
			MRDInitNewHeader(&mrd_header);
			memset(MRD_file_buffer, 0x00, MRD_IMAGE_TOTAL_SIZE);
			memcpy(MRD_file_buffer, &mrd_header, sizeof(MRD_header_t));
			DPRINTF("Done.\n");

			DPRINTF("Creating a new MRD end file...\n");
			memset(&MRD_end_hdr, 0x00, sizeof(MRD_entry_header_t));
			MRD_end_hdr.entry_type = END_BUF_STMP;
			MRD_end_hdr.header_checksum = MRDFileHeaderChecksumCalc(&MRD_end_hdr);
			memcpy(((UINT8 *)MRD_file_buffer + sizeof(mrd_header)),
				   &MRD_end_hdr,
				   sizeof(MRD_entry_header_t));

			DPRINTF("Done.\n");

			/* return no error to the user */
			ret_val = MRD_NO_ERROR;
			initialized = TRUE;
		} else {
			free(MRD_file_buffer);
			mrd_mutex_unlock();
		}
	}

	return ret_val;
}


/***********************************************************************
* Function: MRDClose							                       *
************************************************************************
* Description: Shuts down the MRD module							   *
*              				                                           *
* Parameters: none                                                     *
*              														   *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
MRDErrorCodes MRDClose(void)
{
	if (!initialized)
		return MRD_NOT_INITIALIZED_ERROR;

	if (MRD_file_buffer != NULL)
		free(MRD_file_buffer);

	MRD_file_buffer = NULL;

	initialized = FALSE;
	mrd_mutex_unlock();

	return MRD_NO_ERROR;
}


/***********************************************************************
* Function: MRDFileAdd							                       *
************************************************************************
* Description: Adds a file to the MRD image							   *
*              				                                           *
* Parameters: MRD_filename - Internal MRD filename                     *
*             flags - Flags for the file in the MRD					   *
*             input_filename - Filename + path of the file to be added *
* 							   to the MRD							   *
*            				                                           *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
MRDErrorCodes MRDFileAdd(char *MRD_filename,
						 UINT32 flags,
						 UINT32 mrd_file_type,
						 UINT32 file_format_version,
						 UINT32 timestamp,
						 char *input_filename)
{
	UNUSEDPARAM(flags);
	UINT32 file_offset;
	UINT32 new_file_sz;
	UINT32 file_size;
	UINT32 pad_size;
	FILE *file;
	void *new_mrd_file_ptr;
	int ret_val;
	MRD_entry_header_t *pMRD_file_hdr;
	MRD_entry_header_t MRD_end_hdr;
	MRD_header_t *pMRD_header;
	UINT32 new_total_mrd_image_size;

	if (!initialized)
		return MRD_NOT_INITIALIZED_ERROR;


	/* does file exist in MRD? */
	file_offset = MRDInternalFileOffsetGet(MRD_file_buffer,
											MRD_filename,
											mrd_file_type,
											&file_size);
	if (file_offset != 0)
		return MRD_FILE_ALREADY_EXISTS_ERROR;

	/* Get internal file offset of the end of the MRD image */
	file_offset = MRDInternalFileHeaderOffsetGet(MRD_file_buffer,
													NULL,
													END_BUF_STMP,
													&file_size);																					
	if (file_offset == 0)
		return MRD_INVALID_END_OF_FILE_ERROR;

	pMRD_file_hdr = (MRD_entry_header_t	*)((UINT32)MRD_file_buffer + file_offset);
	new_mrd_file_ptr = (void *)((UINT32)pMRD_file_hdr + sizeof(MRD_entry_header_t));

	file = fopen(input_filename,"rb");
	if (file == NULL)
		return MRD_FILE_NOT_FOUND_ERROR;

	/* get file size */
	if( fseek(file, 0, SEEK_END) != 0)
	{
		fclose(file);
		return MRD_ERROR;
	}
	if( (new_file_sz = ftell(file)) == (UINT32)-1)
	{
		fclose(file);
		return MRD_ERROR;
	}
	if( fseek(file, 0, SEEK_SET) != 0)
	{
		fclose(file);
		return MRD_ERROR;
	}

	if (NULL == strncpy(pMRD_file_hdr->fileName, MRD_filename, MRD_MAX_FILENAME)) {
		fclose(file);
		return MRD_FILENAME_TOO_LONG;
	}

	// Shuki: TODO - Add assert if there is memory overrun of MAX MRD file size!!!
	pad_size = sizeof(UINT32);
	pad_size -= (new_file_sz & 0x3);// & 0x3;
	pad_size &= 0x3;
    
	pMRD_file_hdr->entry_size = new_file_sz + pad_size;
	pMRD_file_hdr->entry_size |= (pad_size << 24);

	new_total_mrd_image_size = file_offset + sizeof(MRD_entry_header_t) + new_file_sz;
	if (new_total_mrd_image_size >= MRD_MAX_FILE_LEN)
	{
		fclose(file);
		return MRD_NO_SPACE_ERROR;
	}
    DPRINTF("new total mrd image size is: %d\n",new_total_mrd_image_size);
	/* Update main MRD header */
	pMRD_header = (MRD_header_t *)MRD_file_buffer;
	pMRD_header->image_size = new_total_mrd_image_size;
	pMRD_header->update_counter++;

	pMRD_file_hdr->entry_type = mrd_file_type;
	pMRD_file_hdr->timestamp = timestamp;
	pMRD_file_hdr->version = file_format_version;

	/* calculate checksum */
	pMRD_file_hdr->header_checksum = MRDFileHeaderChecksumCalc(pMRD_file_hdr);
    
	/* read into MRD file buffer */
	ret_val = fread(new_mrd_file_ptr, 1, new_file_sz, file);
	if (ret_val == 0)
	{
		fclose(file);
		return MRD_FILE_READ_ERROR;
	}

	fclose(file);
    
    
	/* Create a new MRD end header */
	memset(&MRD_end_hdr, 0x00, sizeof(MRD_entry_header_t));
	MRD_end_hdr.entry_type = END_BUF_STMP;
	MRD_end_hdr.header_checksum = MRDFileHeaderChecksumCalc(&MRD_end_hdr);
     
	/* Copy into MRD image */
	//new_mrd_file_ptr = (void *)((UINT32)new_mrd_file_ptr + new_file_sz);
	new_mrd_file_ptr = (void *)((UINT32)new_mrd_file_ptr + (pMRD_file_hdr->entry_size & 0xFFFFFF));

	memcpy(new_mrd_file_ptr, &MRD_end_hdr, sizeof(MRD_entry_header_t));   
    
	/* Update MRD image on flash */
	return MRDRawBlockWrite();
}


/***********************************************************************
* Function: MRDFileRead							                       *
************************************************************************
* Description: Adds a file to the MRD image							   *
*              				                                           *
* Parameters: MRD_filename - Internal MRD filename                     *
*             flags - Flags of the file in the MRD					   *
*             input_filename - Filename + path of the output file in   *
* 							   OS file system						   *
*            				                                           *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
MRDErrorCodes MRDFileRead(char *MRD_filename,
						  UINT32 mrd_file_type,
						  UINT32 *flags,
						  UINT32 *file_format_version,
						  UINT32 *timestamp,
						  char *output_filename)
{
	UNUSEDPARAM(flags);
	UINT32 file_offset;
	UINT32 file_size;
	UINT32 ret_val;
	FILE *file;
	MRD_entry_header_t *pMRD_file_hdr = NULL;

	if (!initialized)
		return MRD_NOT_INITIALIZED_ERROR;

	/* Get internal file offset */
	file_offset = MRDInternalFileOffsetGet(MRD_file_buffer,
											MRD_filename,
											mrd_file_type,
											&file_size);
	if (file_offset == 0x0)
		return MRD_FILE_NOT_FOUND_ERROR;
   
    /* return the version and timestamp string to caller*/
   	pMRD_file_hdr = (MRD_entry_header_t	*)((UINT32)MRD_file_buffer + (file_offset - sizeof(MRD_entry_header_t)));
	*file_format_version = pMRD_file_hdr->version;
	*timestamp = pMRD_file_hdr->timestamp;

	/* check output_filename, if equals to NULL, just ignore it, so we can get
	 * file version and timestamp without copying */
	if(output_filename == NULL)
		return MRD_NO_ERROR;

	/* Write file to file-system */
	file = fopen(output_filename, "wb");
	if (file == NULL)
		return MRD_FILE_NOT_FOUND_ERROR;

	ret_val = fwrite((unsigned char *)MRD_file_buffer + file_offset,
						1,
						file_size,
						file);
	if (ret_val != file_size)
	{
		fclose(file);
		return MRD_FILE_WRITE_ERROR;
	}

	fclose(file);

	return MRD_NO_ERROR;
}


/***********************************************************************
* Function: MRDFileExist 						                       *
************************************************************************
* Description: Checks if a file exists in MRD						   *
*              				                                           *
* Parameters: MRD_filename - Internal MRD filename                     *
*             mrd_file_type - MRD file type							   *
*            				                                           *
* Return value: if the file exists in MRD: MRD_NO_ERROR                *
*               if file does not exist: MRD_FILE_NOT_FOUND_ERROR       *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
MRDErrorCodes MRDFileExist(char *MRD_filename, UINT32 mrd_file_type)
{
	UINT32 file_size;
	UINT32 file_offset;

	if (!initialized)
		return MRD_NOT_INITIALIZED_ERROR;

	/* Get internal file offset */
	file_offset = MRDInternalFileOffsetGet(MRD_file_buffer,
											MRD_filename,
											mrd_file_type,
											&file_size);
	if (file_offset == 0x0)
		return MRD_FILE_NOT_FOUND_ERROR;

	return MRD_NO_ERROR;
}

/***********************************************************************
* Function: MRDFileRemove						                       *
************************************************************************
* Description: Removes a file from the MRD 							   *
*              				                                           *
* Parameters: MRD_filename - Internal MRD filename                     *
*             MRD_file_type	- MRD file type                            *
*              				                                           *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
MRDErrorCodes MRDFileRemove(char *MRD_filename, UINT32 MRD_file_type)
{
	UINT32 file_header_offset;
	UINT32 file_size;
	UINT32 next_file_header_offset;
	UINT32 pad_size;

	if (!initialized)
		return MRD_NOT_INITIALIZED_ERROR;

	/* Search for the MRD file name + type in the MRD */
	file_header_offset = MRDInternalFileHeaderOffsetGet(MRD_file_buffer,
														MRD_filename,
														MRD_file_type,
														&file_size);
	if (file_header_offset == 0)
		return MRD_FILE_NOT_FOUND_ERROR;


	/* File found in MRD, let's remove it... */
	/* Calculate pad size */
	pad_size = (sizeof(UINT32) - (file_size & 0x3)) & 0x3;

	/* Calculate next file header offset */
	next_file_header_offset = file_header_offset;
	next_file_header_offset += sizeof(MRD_entry_header_t);
	next_file_header_offset += file_size;
	next_file_header_offset += pad_size;

	/* Destroy the file data (just in case) */
	memset(&MRD_file_buffer[file_header_offset],
		   0x00,
		   (file_size + pad_size + sizeof(MRD_entry_header_t)));

	/* override the file and effectively remove it */
	memcpy(&MRD_file_buffer[file_header_offset],
		   &MRD_file_buffer[next_file_header_offset],
		   MRD_MAX_FILE_LEN - next_file_header_offset);

	/* Update MRD image on flash */
	return MRDRawBlockWrite();
}


/***********************************************************************
* Function: MRDFileDirFirst						                       *
************************************************************************
* Description: MRD directory read - FIRST							   *
*              				                                           *
* Parameters: MRD_filename - Internal MRD filename                     *
* 			  flags - flags of the file in the MRD					   *
*            				                                           *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
MRDErrorCodes MRDFileDirFirst(char *MRD_filename,
  							  UINT32 *flags,
							  UINT32 *file_format_version,
							  UINT32 *timestamp)
{
	UNUSEDPARAM(MRD_filename);
	UNUSEDPARAM(flags);
	UNUSEDPARAM(file_format_version);
	UNUSEDPARAM(timestamp);
	if (!initialized)
		return MRD_NOT_INITIALIZED_ERROR;

	return MRD_NO_ERROR;
}


/***********************************************************************
* Function: MRDFileDirNext						                       *
************************************************************************
* Description: MRD directory read - NEXT							   *
*              				                                           *
* Parameters: MRD_filename - Internal MRD filename                     *
* 			  flags - flags of the file in the MRD					   *
*            				                                           *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
MRDErrorCodes MRDFileDirNext(char *MRD_filename,
							 UINT32 *flags,
							  UINT32 *file_format_version,
							  UINT32 *timestamp)
{
	UNUSEDPARAM(MRD_filename);
	UNUSEDPARAM(flags);
	UNUSEDPARAM(file_format_version);
	UNUSEDPARAM(timestamp);
	if (!initialized)
		return MRD_NOT_INITIALIZED_ERROR;

	return MRD_NO_ERROR;
}


#if !defined(UNIT_UNDER_TEST_VC)
/***********************************************************************
* Function: MRDRawBlockWrite					                       *
************************************************************************
* Description: Writes a raw MRD block to flash	 					   *
*              				                                           *
* Parameters: none                                                     *
*              														   *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static MRDErrorCodes MRDRawBlockWrite(void)
{
	int len;
	MRDErrorCodes ret_val;
	char shell_cmd[MRD_FILENAME_AND_PATH_MAX_SZ];
	FILE *file;
	UINT32 crc;
	UINT32 *pMRD_end;
	MRD_header_t *pMRD_hdr;
	UINT32 num_blocks_skip;
	UINT32 num_blocks_to_read;
	int ret;
	UINT32 tmp_crc=0;
	char local_buf[200];
	unsigned char *temp_MRD_file_buffer=NULL;
	int try_num = 0;

	/* Allocate temp MRD file buffer */
	temp_MRD_file_buffer = malloc(MRD_IMAGE_TOTAL_SIZE);
	if (temp_MRD_file_buffer == NULL)
		return MRD_NO_MEMORY_ERROR;

	/* Update checksum */
	pMRD_end = (UINT32 *)((UINT32)MRD_file_buffer + MRD_MAX_FILE_LEN);
	crc = MRDChecksumCalc((UINT32 *)MRD_file_buffer, pMRD_end);

	pMRD_hdr = (MRD_header_t *)MRD_file_buffer;
	pMRD_hdr->MRD_id.bufCheckSum = crc;

	tmp_crc = crc;

	/* Write MRD file snapshot from DDR to the RAM disk */
	file = fopen(MRD_TMP_FILENAME, "wb");
	if (NULL == file)
	{
		ret_val = MRD_FILE_NOT_FOUND_ERROR;
		goto ERROR_EXIT;
	}

	len = fwrite(MRD_file_buffer, 1, MRD_IMAGE_TOTAL_SIZE, file);
	fclose(file);

	if (MRD_MAX_FILE_LEN != len)
	{
		ret_val = MRD_FILE_WRITE_ERROR;
		goto ERROR_EXIT;
	}

	num_blocks_skip = g_mrd_block_offset / g_block_sz;
	num_blocks_to_read = (MRD_IMAGE_TOTAL_SIZE / g_block_sz);

TRY_AGAIN:
	DPRINTF("MRD: Writing to the MRD TRY #%d\n", try_num);
	/*#define MRD_RAW_BLOCK_READ_WRITE_CMD	"dd if=%s of=%s bs=%d count=%d skip=%d"*/
	len = snprintf(shell_cmd,
					MRD_FILENAME_AND_PATH_MAX_SZ,
					g_raw_block_write_cmd,
					MRD_TMP_FILENAME,
					g_mrd_mtd_partition,
					g_block_sz,
					num_blocks_to_read,
					num_blocks_skip);

	if (len > MRD_FILENAME_AND_PATH_MAX_SZ)
	{
		ret_val = MRD_NO_ERROR;
		goto ERROR_EXIT;
	}

	/* initiate command */
	DPRINTF("MRD: Writing to the MRD using the following commandline:\n%s\n", shell_cmd);
	ret_val = system(shell_cmd);
	DPRINTF("MRD: Write return val %d\n", ret_val);
	sync();
	len=0;
	file = fopen("/tmp/MRD_tmp_retval.bin", "r");
	if (NULL == file)
	{
		ret_val = MRD_VERIFY_ERROR1;
		DPRINTF("MRD: Error1 #%d\n", ret_val);
	}
	else
	{
		len = fread(local_buf,1,sizeof(local_buf),file);
		fclose(file);
	}
	if(len <=0)
	{
		ret_val = MRD_VERIFY_ERROR2;
		DPRINTF("MRD: Error2 #%d\n", ret_val);
	}
	else
	{
		if(local_buf[0] == '0')
		{
			ret_val=MRD_NO_ERROR;
			DPRINTF("MRD: Error3 #%d\n", ret_val);
		}
		else //dd operation failed
		{
			ret_val=MRD_VERIFY_ERROR2;
			DPRINTF("MRD: Error4 #%d\n", ret_val);
		}
	}

	memcpy(temp_MRD_file_buffer, MRD_file_buffer, MRD_IMAGE_TOTAL_SIZE);
	memset((char*)MRD_file_buffer, 0x00, MRD_IMAGE_TOTAL_SIZE);

	ret_val = MRDRawBlockRead(MRD_TMP_FILENAME_VERIFY);
	if(ret_val != 0)
	{
		ret_val= MRD_VERIFY_ERROR3;
		DPRINTF("MRD: Error5 #%d\n", ret_val);
	}

	crc = MRDChecksumCalc((UINT32 *)MRD_file_buffer, pMRD_end);
	if(crc != tmp_crc)
	{
		sprintf(shell_cmd, "cp %s %s ; cp %s %s ; dmesg > /data/MRD_log.txt",
			MRD_TMP_FILENAME,
			MRD_DEBUG_FILENAME,
			MRD_TMP_FILENAME_VERIFY,
			MRD_DEBUG_FILENAME_VERIFY);
		system(shell_cmd);
		ret_val=MRD_VERIFY_ERROR4;
		DPRINTF("MRD: Error6 #%d\n", ret_val);
		// Restore from backup copy
		memcpy(MRD_file_buffer, temp_MRD_file_buffer, MRD_IMAGE_TOTAL_SIZE);
		if (try_num++ > 3) goto ERROR_EXIT;
		//add erase operation in case of error
		DPRINTF("MRD: erase operation in case of error\n");
		ret = g_erase_block_func(g_mrd_mtd_partition, 0x00);
		DPRINTF("MRD: erase operation retval-%d\n",ret);
		goto TRY_AGAIN;
	}
	else
		ret_val=MRD_NO_ERROR;

	/* Remove the temporary MRD.bin file !!! */
	//sprintf(shell_cmd, "rm %s", MRD_TMP_FILENAME);
	//system(shell_cmd);
ERROR_EXIT:
	free(temp_MRD_file_buffer);
	DPRINTF("MRD: Exit with Error #%d\n", ret_val);
	return ret_val;
}

/***********************************************************************
* Function: MRDRawBlockRead						                       *
************************************************************************
* Description: Writes a raw MRD block to flash	 					   *
*              				                                           *
* Parameters: none                                                     *
*              														   *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static MRDErrorCodes MRDRawBlockRead(char *filename)
{
	int len;
	int ret_val;
	char shell_cmd[MRD_FILENAME_AND_PATH_MAX_SZ];
	UINT32 num_blocks_skip;
	UINT32 num_blocks_to_read;
	FILE *file;
	char local_buf[200];

	if (filename == NULL)
		return MRD_FILE_NOT_FOUND_ERROR;

	num_blocks_skip = g_mrd_block_offset / g_block_sz;
	num_blocks_to_read = MRD_IMAGE_TOTAL_SIZE / g_block_sz;
	DPRINTF("num_blocks_skip=%d, num_blocks_to_read=%d, g_block_sz=%d\n", num_blocks_skip, num_blocks_to_read, g_block_sz);
	/*#define MRD_RAW_BLOCK_READ_WRITE_CMD	"dd if=%s of=%s bs=%d count=%d skip=%d"*/
	len = snprintf(shell_cmd,
					MRD_FILENAME_AND_PATH_MAX_SZ,
					g_raw_block_read_cmd,
					g_mrd_mtd_partition,
					filename,
					g_block_sz,
					num_blocks_to_read,
					num_blocks_skip);

	if (len > MRD_FILENAME_AND_PATH_MAX_SZ)
		return MRD_ERROR;

	/* initiate command */
	DPRINTF("MRD: reading from the MRD using the following commandline:\n%s\n", shell_cmd);
	ret_val = system(shell_cmd);

	/* the access permission of the file generated by dd command is 1100 ---xr----T
	 * in order to read it, we need to change it to 700
	 */
	chmod(filename, S_IRWXU);

	sync();

	file = fopen("/tmp/MRD_tmp_retval.bin", "r");
	if (NULL == file)
		return MRD_FILE_NOT_FOUND_ERROR;

	len = fread(local_buf,1,sizeof(local_buf),file);
	fclose(file);
	if(len <=0)
		ret_val = MRD_VERIFY_ERROR5;
	else {
		if(local_buf[0] == '0')
			ret_val=MRD_NO_ERROR;
		else
			ret_val=MRD_VERIFY_ERROR6;
	}

	/* read file into buffer */
	file = fopen(filename, "rb");
	if (NULL == file) {
		return MRD_VERIFY_ERROR7;
	}

	len = fread(MRD_file_buffer, 1, MRD_IMAGE_TOTAL_SIZE, file);
	if (len == 0) {
		fclose(file);
		return MRD_FILE_READ_ERROR;
	}

	fclose(file);

	/* Remove the temporary MRD.bin file !!! */
	//sprintf(shell_cmd, "rm %s", filename);
	//system(shell_cmd);

	return (ret_val == 0) ? MRD_NO_ERROR : MRD_ERROR;
}

#if 0
/***********************************************************************
* Function: MRDTemporaryFileRemove				                       *
************************************************************************
* Description: Removes the temporary file used for MRD				   *
*              				                                           *
* Parameters: none                                                     *
*              														   *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static MRDErrorCodes MRDFileRemove(char )
{
	char rm_command_str[sizeof(
}
#endif//0
#else // UNIT_UNDER_TEST_VC
/***********************************************************************
* Function: MRDRawBlockWrite					                       *
************************************************************************
* Description: Writes a raw MRD block to flash	 					   *
*              				                                           *
* Parameters: none                                                     *
*              														   *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static MRDErrorCodes MRDRawBlockWrite(void)
{
#if 0
	if (filename == NULL)
	{
		return MRD_FILE_NOT_FOUND_ERROR;
	}
#endif//0
	return MRD_NO_ERROR;
}

/***********************************************************************
* Function: MRDRawBlockRead						                       *
************************************************************************
* Description: Writes a raw MRD block to flash	 					   *
*              				                                           *
* Parameters: none                                                     *
*              														   *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static MRDErrorCodes MRDRawBlockRead(char *filename)
{
	int size_read;
	FILE *file = NULL;

	/* initiate command */
	DPRINTF("MRD: reading MRD image\n");
	file = fopen(filename, "rb");
	if (NULL != file) {
		size_read = fread(MRD_file_buffer, 1, MRD_MAX_FILE_LEN, file);
	} else {
		return MRD_FILE_NOT_FOUND_ERROR;
	}
	fclose(file);
	DPRINTF("Done\n");

	return (size_read != 0) ? MRD_NO_ERROR : MRD_ERROR;
}
#endif// UNIT_UNDER_TEST_VC


/***********************************************************************
* Function: MRDParseHeader						                       *
************************************************************************
* Description: Adds a file to the MRD image							   *
*              				                                           *
* Parameters: MRD_filename - Internal MRD filename                     *
*            				                                           *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static MRDErrorCodes MRDParseHeader(unsigned char *MRD_file_buffer)
{
	UINT32				image_checksum;
	MRD_header_t		*pMRD_hdr;
	UINT8				*pMrd_start_addr = NULL;
	UINT8				*pMrd_end_addr = NULL;

	pMRD_hdr = (MRD_header_t *)MRD_file_buffer;

	DPRINTF("MRD header\n");
	DPRINTF("==========\n");
	DPRINTF("validBufferStamp=0x%x\n", pMRD_hdr->MRD_id.validBufferStamp);
	DPRINTF("bufCheckSum=0x%x\n", pMRD_hdr->MRD_id.bufCheckSum);
	//DPRINTF("reserve=0x%x\n", pMRD_hdr->reserve);
	//DPRINTF("reserve2=0x%x\n", pMRD_hdr->reserve2);
	DPRINTF("version=0x%x\n", pMRD_hdr->version);

	if (VALID_BUFFER_STAMP != pMRD_hdr->MRD_id.validBufferStamp)
		return MRD_INVALID_HEADER_ERROR;

	DPRINTF("MRD: Calculating checksum\n");

	pMrd_start_addr = (UINT8 *)MRD_file_buffer;
	pMrd_end_addr = (UINT8 *)((UINT8 *)MRD_file_buffer + MRD_MAX_FILE_LEN);
	image_checksum = MRDChecksumCalc((UINT32 *)pMrd_start_addr,
									 (UINT32 *)pMrd_end_addr);

	DPRINTF("MRD: Done. Calculated checksum=0x%x\n",image_checksum);

	if (image_checksum != pMRD_hdr->MRD_id.bufCheckSum) {
		DPRINTF("MRD ERROR: Calculated checksum does not match image checksum\n");
		DPRINTF("Calculated checksum=0x%x\n", image_checksum);
		DPRINTF("Image checksum=0x%x\n", pMRD_hdr->MRD_id.bufCheckSum);
		return MRD_INVALID_CHECKSUM_ERROR;
	}

	return MRD_NO_ERROR;
}


/***********************************************************************
* Function: MRDInitNewHeader					                       *
************************************************************************
* Description: Initializes a new MRD header							   *
*              				                                           *
* Parameters: pMRD_hdr - The ptr to the header to initialize           *
*            				                                           *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static void MRDInitNewHeader(MRD_header_t *pMRD_hdr)
{
	pMRD_hdr->MRD_id.validBufferStamp = VALID_BUFFER_STAMP;
	pMRD_hdr->MRD_id.bufCheckSum = 0x0; // left uninitialized
	pMRD_hdr->image_size = 0;
	pMRD_hdr->update_counter = 1;
	pMRD_hdr->version = RD_FORMAT_VER;
}


/***********************************************************************
* Function: MRDChecksumCalc						                       *
************************************************************************
* Description: Initializes a new MRD header							   *
*              				                                           *
* Parameters: pMRD_start - ptr to MRD in memory						   *
*            				                                           *
*             pMRD_end - ptr to the end of MRD in memory               *
*            				                                           *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static UINT32 MRDChecksumCalc(UINT32 *pBuf, UINT32 *pRD_DDR_end)
{
#define MOD_ADLER 65521
#define MOD_LOOPS 4000
	UINT32 		check_sum = 0;
   	UINT8	   *pRAMbuf = (UINT8 *)pBuf;
   	UINT32	 	a = 1 , b = 0 , index = 0;


   	pRAMbuf +=8;//skip the valid stamp & checksum field

  	do
    {
		size_t i = 0;

		for(i = 0; i < sizeof(UINT32); ++i)
		{
			//Adler32 Checksum
			a += (UINT32)(*pRAMbuf++);
			index++;
			b += a;
			/*
			   Algorithm is defined as mod MOD_ADLER at every modification to a and b.
			   For efficiency reasons this mod can be done once in a while to keep (a) and (b) in range (32-bit signed)
			   The result is invariant of MOD_LOOPS value
			   a<5550*256; b<(5551*256/2)*5550=3943430400 (31-bit overflow, negative)
			   As standard libraries are not present, ModOfTwoNumbers function is called directly.
			   ModOfTwoNumbers function accepts signed 32-bit int.
			   a<4000*256; b<(4001*256/2)*4000=2048512000 (fit into 31-bit)
			 */

			if ( index == MOD_LOOPS )
			{
				index = 0;
				a %= MOD_ADLER;
				b %= MOD_ADLER;
			}
		}

		// use aligned access
  	} while( *(UINT32 *)pRAMbuf != (UINT32)END_BUF_STMP && (pRAMbuf < (UINT8 *)pRD_DDR_end ));

  	if ( index )
  	{
	    a %= MOD_ADLER;
	   	b %= MOD_ADLER;
  	}

  	check_sum = (( b << 16 ) | a);

  	return (check_sum);
}


/***********************************************************************
* Function: MRDInternalFileHeaderOffsetGet		                       *
************************************************************************
* Description: Get file offset of MRD_filename in MRD				   *
*              				                                           *
* Parameters: MRD_filename - internal MRD filename					   *
*             MRD_file_buffer - MRD buffer	                           *
*            				                                           *
* Return value: offset - the file MRD header offset from MRD base,     *
*						 if the file was not found in MRD, offset=0	   *
*				file_size - the raw file size inside the MRD		   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static UINT32 MRDInternalFileHeaderOffsetGet(unsigned char *MRD_file_buffer,
												char *MRD_filename,
												UINT32 mrd_file_type,
												UINT32 *file_size)
{
	MRD_entry_header_t *pMRD_file_hdr;
	unsigned char *ptr;
	BOOL valid_file;
	BOOL file_found;
	UINT32 offset;
	UINT32 cnt;
	UINT32 pad_size;
	UINT32 compare_result;

	/* skip MRD image header */
	ptr = (unsigned char *)MRD_file_buffer + sizeof(MRD_header_t);

	DPRINTF("Trying to find file_type=0x%x inside MRD image\n", mrd_file_type);

	/* scan until reaching the end of MRD */
	offset = 0;
	valid_file = TRUE;
	file_found = FALSE;
	cnt = 0;
	do {
		pMRD_file_hdr = (MRD_entry_header_t	*)ptr;
        #if 0
		if (MRD_NO_ERROR != MRDIsValidFileHeader(pMRD_file_hdr)) {
		    DPRINTF("not MRD valid file header!\n");
			valid_file = FALSE;
			break;
		}
        #endif
		if (pMRD_file_hdr->entry_type == END_BUF_STMP)
		{ 
			valid_file = FALSE;
        }
		/* Calculate pad size */
		//pad_size = sizeof(UINT32) - (pMRD_file_hdr->entry_size.file_size % sizeof(UINT32));
		// pad_size = (sizeof(UINT32) - (pMRD_file_hdr->entry_size & 0x03)) & 0x3;
		// 8 MSB field is padding size
		pad_size = (pMRD_file_hdr->entry_size >> 24) & 0x3;

		/* Support searching for tail header */
		if (MRD_filename != NULL)
		{
		    if(mrd_file_type == MRD_IMEI_TYPE||mrd_file_type == MRD_MEP_TYPE)
		    {
			    compare_result =0;
			}
			else
			{
			    compare_result = strcmp(MRD_filename, pMRD_file_hdr->fileName);
			}
		}
		else
			compare_result = 0;

		if ( (mrd_file_type == pMRD_file_hdr->entry_type) &&
			 (0 == compare_result) )
		{   
			offset = ((UINT32)ptr - (UINT32)MRD_file_buffer);
			*file_size = (pMRD_file_hdr->entry_size & 0xFFFFFF);
			*file_size -= pad_size; // file_size is raw file size
			file_found = TRUE;
		}

		if (valid_file) {
			DPRINTF("Found MRD file entry:\n");
			DPRINTF("====================\n");
			DPRINTF("entry_size=0x%x\n", pMRD_file_hdr->entry_size);
			//DPRINTF("entry_size.pad_size=0x%x\n", pMRD_file_hdr->entry_size.pad_size);
			DPRINTF("entry_type=0x%x\n", pMRD_file_hdr->entry_type);
			DPRINTF("fileName=%s\n", pMRD_file_hdr->fileName);
			DPRINTF("timestamp=0x%x\n", pMRD_file_hdr->timestamp);
			DPRINTF("header_checksum=0x%x\n", pMRD_file_hdr->header_checksum);
			DPRINTF("version=0x%x\n\n", pMRD_file_hdr->version);
		}

		ptr += sizeof(MRD_entry_header_t);
		ptr += (pMRD_file_hdr->entry_size & 0xFFFFFF); //.file_size;
		//ptr += pad_size;
	} while ( (valid_file != FALSE) &&
			  (file_found == FALSE) &&
			  ((UINT32)ptr < ((UINT32)MRD_file_buffer + MRD_MAX_FILE_LEN)) );

	return offset;
}


/***********************************************************************
* Function: MRDFileOffsetGet					                       *
************************************************************************
* Description: Get file offset of MRD_filename in MRD				   *
*              				                                           *
* Parameters: MRD_filename - internal MRD filename					   *
*             MRD_file_buffer - MRD buffer	                           *
*            				                                           *
* Return value: offset - the file offset from MRD base,                *
*						 if the file was not found in MRD, offset=0	   *
*				file_size - the raw file size inside the MRD		   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static UINT32 MRDInternalFileOffsetGet(unsigned char *MRD_file_buffer,
										char *MRD_filename,
										UINT32 mrd_file_type,
										UINT32 *file_size)
{
	MRD_entry_header_t *pMRD_file_hdr;
	UINT32 offset;

	offset = MRDInternalFileHeaderOffsetGet(MRD_file_buffer,
											MRD_filename,
											mrd_file_type,
											file_size);
	/* if no error, continue */
	if (offset != 0) {
		pMRD_file_hdr = (MRD_entry_header_t	*)((UINT32)MRD_file_buffer + offset);

		/* Calculate pad size */
		//pad_size = ((sizeof(UINT32) - (pMRD_file_hdr->entry_size.file_size & 0x3)) & 0x3);
		offset += sizeof(MRD_entry_header_t);
		//offset += pad_size;
	}

	return offset;
}

/***********************************************************************
* Function: MRDFileHeaderChecksumCalc  				                   *
************************************************************************
* Description: Calculates the MRD file header checksum				   *
*              				                                           *
* Parameters: pMRD_file_header - the file header to check			   *
*            				                                           *
* Return value: Calculated checksum
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static UINT32 MRDFileHeaderChecksumCalc(MRD_entry_header_t *pMRD_file_header)
{
	UINT32 *ptr;
	UINT32 *pEnd = &(pMRD_file_header->header_checksum);
	UINT32 checksum;

	checksum = 0;
	for (ptr = (UINT32 *)pMRD_file_header; ptr < pEnd; ptr++)
	{
		checksum ^= *ptr;
		checksum += 1;
	}

	return checksum;
}
#if 0
/***********************************************************************
* Function: MRDIsValidFileHeader   				                       *
************************************************************************
* Description: Checks if the MRD header is valid					   *
*              				                                           *
* Parameters: pMRD_file_header - the file header to check			   *
*            				                                           *
* Return value: MRD_NO_ERROR - valid header							   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static MRDErrorCodes MRDIsValidFileHeader(MRD_entry_header_t *pMRD_file_header)
{
	UINT32 checksum;

	checksum = MRDFileHeaderChecksumCalc(pMRD_file_header);
	if (checksum != pMRD_file_header->header_checksum)
		return MRD_INVALID_FILE_HEADER_ERROR;

	return MRD_NO_ERROR;
}
#endif
