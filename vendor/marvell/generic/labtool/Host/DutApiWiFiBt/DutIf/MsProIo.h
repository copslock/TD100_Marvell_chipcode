/** @file MsProIo.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#define MRV_CMD_PORT_ADDR	0x100
#define MRV_DATA_PORT_ADDR	0x102
#define MRV_IO_PORT_ADDR	0x104


#ifdef MSPROIODLL //_USRDLL
#ifdef __cplusplus
extern "C" { 
#endif
#define DUT_DRV  __declspec(dllexport)  
DUT_DRV int DeviceInit(int *RamTestResult);
DUT_DRV int Query( unsigned long addr, unsigned char *data, unsigned int LenInByte);
DUT_DRV int DeviceClose(void);
#ifdef __cplusplus
}
#endif
 

#else
#ifdef __cplusplus
extern "C" { 
#endif
#define DUT_DRV   __declspec(dllimport)
 
DUT_DRV int DeviceInit(int *pRamTestResult);
DUT_DRV int Query( unsigned long addr, unsigned char *data, unsigned long LenInByte);
DUT_DRV int DeviceClose(void);
DUT_DRV void setTimeout(int timeOut);
DUT_DRV void getTimeout(int *timeOut);

#ifdef __cplusplus
}
#endif

#endif

