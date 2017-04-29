#if !defined (MRD_STRUCTURE_H)
#define MRD_STRUCTURE_H

#if defined(WIN32)
#include <windows.h>
#endif//WIN32


#define VALID_BUFFER_STAMP	0xCAFECAFE
#define END_BUF_STMP		0xDADADADA

#define RD_OLD_FORMAT_VER	0x00001017
#define RD_FORMAT_VER		0x00001020
#define MRD_MAX_FILENAME			0x74


#define IMEI_DATA_SIZE						32
#define MRD_MAX_DATA_LEN					0x14000 //80*1024
#define MRD_MAX_FILE_LEN					0x20000//(128*1024)

#if 0
#define PREAMBLE		0x2b02d300
#define ELBMAERP		0x00d3022b
#define AUTH_PREAMBLE	0xCAFE

#define MRD_DEF_START_ADDRESS				_T("0x00100000")
#define BINDED_IMEI							_T("bnd_imei")
#define BINDED_MEP							_T("bnd_mep")
#define BND									_T("bnd_")
#define SN_FILE								_T("sn.nvm")
#define TRST_MRD_FILE						_T("reliabledata_trusted.mrd")
#define NON_TRST_MRD_FILE					_T("reliabledata_non_trusted.mrd")
#define MRD_FILE_DIR						_T("ReliableData")


#define MEP_V1_FILE_SIZE					167
#define MEP_V2_FILE_SIZE					279
#endif//0

#define IMEI_VERSION						1
#define SN_VERSION							1
#define NVM_VERSION							1

typedef struct {
	UINT32	validBufferStamp;
	UINT32	bufCheckSum;
} MRD_id_t;

typedef struct {
	MRD_id_t MRD_id;
	UINT32	version;
	UINT32	image_size;
	UINT32	update_counter;
} MRD_header_t, *PMRD_header_t;

typedef struct MRD_entry_header_t {
	UINT32			entry_type;
	UINT32			entry_size;
	char  			fileName[MRD_MAX_FILENAME];
	UINT32			version;
	UINT32			timestamp; // in C time
	UINT32			header_checksum;
} MRD_entry_header_t, *PMRD_entry_header_t;

#endif// MRD_STRUCTURE_H

