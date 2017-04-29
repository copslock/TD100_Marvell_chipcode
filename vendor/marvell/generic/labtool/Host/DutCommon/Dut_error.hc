#ifndef DUT_ERROR_HC
#define DUT_ERROR_HC

#define ERROR_NONE					(0x00)
#define ERROR_ERROR					(0x01)
#define ERROR_READY_TIMEOUT			(0x02)
#define ERROR_FRAMEEXPIRE			(0x03)
#define ERROR_DATANOTEXIST			(0x05)
#if defined(_W8787_) || defined(_W8782_)
#define ERROR_DATANOTAVAILABLE		ERROR_DATANOTEXIST	
#endif
#define ERROR_MEMORY				(0x07)
#define ERROR_FILENOTEXIST			(0x08)
#define ERROR_UNSUPPORTEDDEVICETYPE	(0x09)
#define ERROR_FILEACCESS			(0x0A)


#define ERROR_NOT_A_FEATURE			(0xBA)
#define ERROR_NOT_IMPLEMENTED		(0xBB)

#define ERROR_INPUT_INVALID			(-1)
#define ERROR_MISMATCH				(-2)
#define ERROR_SPACENOTAVAILABLE		(-3)
#define ERROR_CHECKSUM				(-4)
#define ERROR_DATAMAINNOTSET		(-5)
#define ERROR_TOOMANYLEVELS			(-6)


#define ERROR_LINK_NOTFOUND			(0x98)
#define ERROR_LINK_NOLINK			(0x99)

#endif //#ifndef DUT_ERROR_HC
