#ifndef __CNM_BRIDGE_H__
#define __CNM_BRIDGE_H__

#ifdef __cplusplus
extern "C" {
#endif


//cnm_os_api_xxx APIs are OS related 
int	cnm_os_api_sleep(int useconds_in100);	//NOTE: the unit is 100 usecond, therefore cnm_os_api_sleep(3) will sleep 3*100 usecond
void* cnm_os_api_vmalloc(unsigned int sz);	//common allocate, just used to allocate some structure which store some state information of codec, return virtual address
void cnm_os_api_vfree(void* p);			//common free


//From 2010.11.04, cnm_os_api_dma_malloc()/cnm_os_api_dma_get_pa()/cnm_os_api_dma_free()/cnm_os_api_dma_cpy() are replaced with cnm_os_api_pcontmem_alloc()/cnm_os_api_pcontmem_free()/cnm_os_api_pcontmem_common_cpy()
#if 0
//cnm_os_api_dma_malloc() allocate physical continuous memory, *ppAddr is virtual address 
void cnm_os_api_dma_malloc(void** ppAddr,int iSize, int align);
/*void* cnm_os_api_dma_get_va(void* phyAddr);	//transfer phyical address to virtual address. From 2010.10.21, this function isn't needed any more.*/
void* cnm_os_api_dma_get_pa(void* virAddr);	//transfer virtual address to phyical address
void cnm_os_api_dma_free(void** ppAddr);	//free cnm_os_api_dma_malloc() allocated memory, *ppAddr is virtual address
void* cnm_os_api_dma_cpy(void* pDst, const void* pSrc, unsigned int sz);	//pDst, pSrc all are virtual address
#endif
typedef struct{
	void* virAddr;	//virtual address
	unsigned int phyAddr;	//physical address
	int iSize;		//size
	int	iAlign;		//alignment requirement
	void* reserved0;	//for future use
	void* cnm_os_api_mem_helper;	//cnm_os_api_mem_helper is only accessed by cnm_os_api_pcontmem_alloc() and cnm_os_api_pcontmem_free(). It's a helper for cnm_os_api_pcontmem_alloc() implementor, won't accessed by C&M codec API. The usage of it is depended on implementation of cnm_os_api_pcontmem_alloc().
	void* pExtData0;	//pExtData0~3 won't be accessed in C&M codec API, only will accessed by application or cnm_os_api_pcontmem_alloc()/cnm_os_api_pcontmem_free()
	void* pExtData1;
	void* pExtData2;
	void* pExtData3;
}PhyContMemInfo;
int cnm_os_api_pcontmem_alloc(PhyContMemInfo* pMemInfo);
int cnm_os_api_pcontmem_free(PhyContMemInfo* pMemInfo);
void* cnm_os_api_pcontmem_common_cpy(void* pDst, const void* pSrc, unsigned int sz);



//for power setting, suspend and resume operation. NOTE: 2011.07.28. In fact, below 3 APIs haven't been used, implement them as returning 0 directly is OK.
int cnm_os_api_blocksuspend(void);		//during VPU running, OS shouldn't enter suspend state. Call this API prevents OS from entering suspend
int cnm_os_api_unblocksuspend(void);		//Call this API allow OS to enter suspend state
int cnm_os_api_disablePP(int reserved);


//VpuXXX APIs are cnm device related, they are called by libcnmhal
void VpuWriteReg(int driverid, unsigned int off, unsigned int value);
unsigned int VpuReadReg(int driverid, unsigned int off);
void VpuWriteMem(int driverid, unsigned int paddr, unsigned int value);
unsigned int VpuReadMem(int driverid, unsigned int paddr);
unsigned int VpuWaitInterrupt(int driverid, unsigned int timeout);
unsigned int VpuChangeClock(int driverid, unsigned int apmu_value);
int VpuPowerOnOff(int driverid, int bOn);
int VpuDevice_Get(int driverid, unsigned int* pFirmwarePhyAddr, unsigned int* pFirmwareVirAddr, int (*pfun_download)(int, unsigned int));
int VpuDevice_Rel(int driverid);
int VpuSlot_Get(int driverid);
int VpuSlot_Rel(int driverid, int slot_idx);

int cnm_os_driver_init(int* p_driverid);
int cnm_os_driver_clean(int driverid);
int cnm_os_driver_vpulock(int driverid, int timeout_ms);
int cnm_os_driver_vpuunlock(int driverid);

//VpuProfile(int driverid, int bRun) is only for performance profiling, you could implement it as an empty function.
int VpuProfile(int driverid, int bRun);

//VpuQueryDecResolutionForbidden(int driverid, int w, int h) is only for some special platform like pxa918, which forbid c&m to decode 720p stream. For common platform, let this API return 0 is ok.
int VpuQueryDecResolutionForbidden(int driverid, int w, int h);

//VpuInformCodecType() is used to inform kernel driver the codec type.
#define VPU_CODEC_TYPEID_ENC_H264		0x1		//definition should be same as kernel driver
#define VPU_CODEC_TYPEID_ENC_MPEG4		0x2
#define VPU_CODEC_TYPEID_ENC_H263		0x4
#define VPU_CODEC_TYPEID_DEC_H264		0x10000
#define VPU_CODEC_TYPEID_DEC_MPEG4		0x20000
#define VPU_CODEC_TYPEID_DEC_H263		0x40000
#define VPU_CODEC_TYPEID_DEC_MPEG2		0x80000
#define VPU_CODEC_TYPEID_DEC_VC1		0x100000
int VpuInformCodecType(int driverid, int codecTypeId);

#ifdef __cplusplus
}
#endif

#endif
