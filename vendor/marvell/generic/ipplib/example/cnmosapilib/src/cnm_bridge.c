#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <dlfcn.h>
#include <malloc.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <string.h>		//to include memcpy
#include <unistd.h>		//to include usleep
#include "cnm_bridge.h"

//#define CNM_KERNEL_USEBMM		//2011.07.14, from now on, we won't use bmm in kernel driver.
//#define CNM_USERSPACE_USEBMM	//2011.05.19, currently, we use bmm in kernel driver but use pmem in user space by default.

#if defined(CNM_KERNEL_USEBMM) || defined(CNM_USERSPACE_USEBMM)
#include "bmm_lib.h"
#endif
#if !defined(CNM_USERSPACE_USEBMM)	//pmem only used in user space, won't be used in kernel driver
#include "pmem_helper_lib.h"
#endif

typedef struct{
	unsigned int    off;
	void            *value;
}VPU_INFO_T;

#define CNM_IOCTL_MAGIC 'c'
#define VPU_READREG             _IOW(CNM_IOCTL_MAGIC, 1, unsigned int)
#define VPU_WRITEREG            _IOW(CNM_IOCTL_MAGIC, 2, unsigned int)
#define VPU_WAITFORTIMEOUT      _IOW(CNM_IOCTL_MAGIC, 3, unsigned int)
#define VPU_CLOCK_CHANGE        _IOW(CNM_IOCTL_MAGIC, 4, unsigned int)
#define VPU_LOCK				_IOW(CNM_IOCTL_MAGIC, 5, unsigned int)
#define VPU_UNLOCK				_IOW(CNM_IOCTL_MAGIC, 6, unsigned int)
#define VPU_GET_RESERVEDMEM		_IOW(CNM_IOCTL_MAGIC, 7, unsigned int)
#define VPU_GETSET_INFOMATION	_IOW(CNM_IOCTL_MAGIC, 8, unsigned int)
#define VPU_DVFMPROFILING		_IOW(CNM_IOCTL_MAGIC, 9, unsigned int)
#define VPU_QUERYHWLIMIT		_IOW(CNM_IOCTL_MAGIC, 10, unsigned int)

#if 0
#define assist_echo(...)	printf(__VA_ARGS__)
#else
#define assist_echo(...)
#endif

//following vars are critical region, codec will call cnm_os_driver_vpulock/cnm_os_driver_vpuunlock to protect following vars between different threads
static unsigned int g_firmwarebuf_viraddr = 0;
static unsigned int g_firmwarebuf_phyaddr;
static int g_firmwarebuf_sz;
static int g_firmwarebuf_refcnt = 0;

static __inline int open_cnmfd(int* p_cnmfd)
{
	int fd_cnm = open("/dev/cnm", O_RDWR);
	if(fd_cnm < 0) {
		printf("open /dev/cnm fail!\n");
		return -1;
	}
	*p_cnmfd = fd_cnm;
	return 0;
}

static __inline int close_cnmfd(int fd_cnm)
{
	if(fd_cnm >= 0) {
		close(fd_cnm);
		return 0;
	}else{
		return -1;
	}
}

int cnm_os_driver_init(int* p_driverid)
{
	if( open_cnmfd(p_driverid) == 0 ) {
		return 0;
	}else{
		return -1;
	}
}

int cnm_os_driver_clean(int driverid)
{
	if( close_cnmfd(driverid) == 0 ) {
		return 0;
	}else{
		return -1;
	}
}

//cnm_os_driver_vpulock and cnm_os_driver_vpuunlock only for multi-instance, not use them currently
//after timeout_ms millisecond, if still couldn't get lock, return non-zero
int cnm_os_driver_vpulock(int driverid, int timeout_ms)
{
	assist_echo("]]]] %s called\n", __FUNCTION__);
	int ret;
	VPU_INFO_T ioctl_par;
	ioctl_par.off = timeout_ms;
	ioctl_par.value = &ret;
	if (ioctl(driverid, VPU_LOCK, &ioctl_par)) {
		printf ("%s: ioctl error!!!\n", __FUNCTION__);
		exit(1);
	}
	return ret;
}

int cnm_os_driver_vpuunlock(int driverid)
{
	assist_echo("]]]] %s called\n", __FUNCTION__);
	VPU_INFO_T ioctl_par;
	if (ioctl(driverid, VPU_UNLOCK, &ioctl_par)) {
		printf ("%s: ioctl error!!!\n", __FUNCTION__);
		exit(1);
	}
	return 0;
}

void VpuWriteReg(int driverid, unsigned int off, unsigned int value)
{
	VPU_INFO_T vpu_info_element;
	vpu_info_element.off = off;
	vpu_info_element.value = (void*)value;

	if (ioctl(driverid, VPU_WRITEREG, &vpu_info_element)) {
		printf ("%s: ioctl error!!!\n", __FUNCTION__);
		exit(1);
	}
	return;
}

unsigned int VpuReadReg(int driverid, unsigned int off)
{
	VPU_INFO_T vpu_info_element;
	unsigned int regvalue;

	vpu_info_element.off = off;
	vpu_info_element.value = &regvalue;	//after ioctl, register value will be stored in regvalue
	if (ioctl(driverid, VPU_READREG, &vpu_info_element)) {
		printf ("%s: ioctl error!!!\n", __FUNCTION__);
		exit(1);
	}
	return regvalue;
}

//VpuWriteMem is used to access the firmware buffer acquired in VpuDevice_Get()
//paddr is physical address, for physical continuous memory, physical address and virtual address are linear
void VpuWriteMem(int driverid, unsigned int paddr, unsigned int value)
{
	void *vaddr = (void*)(g_firmwarebuf_viraddr +  (paddr - g_firmwarebuf_phyaddr));

    //printf("vadd 0x%.8x, phy 0x%.8x\n", vaddr, paddr);
	*(unsigned int *)vaddr = value;
	return;
}

//VpuReadMem is used to access the firmware buffer acquired in VpuDevice_Get()
//paddr is physical address, for physical continuous memory, physical address and virtual address are linear
unsigned int VpuReadMem(int driverid, unsigned int paddr)
{
	void *vaddr = (void*)(g_firmwarebuf_viraddr +  (paddr - g_firmwarebuf_phyaddr));

    //printf("vadd 0x%.8x, phy 0x%.8x\n", vaddr, paddr);
	return *(unsigned int *)vaddr;
}


//please refer kernel driver source code to see the detail of VpuWaitInterrupt()
//timeout unit is millisecond.
//if timeout==0, kernel driver will clear last interrupt and prepare for next interrupt,
//otherwise, kernel driver will wait interrupt until time out, timeout unit is millisecond
//Host software common process:
//1): VpuWaitInterrupt(0);	//clear interrupt and inform kernel driver to prepare for next interrupt
//2): issue some vpu command;
//3): VpuWaitInterrupt(xxx);	//wait interrupt
//return value:	if timeout == 0, return value is last interrupt reason
//				if timeout != 0, return value is interrupt reason, if timeout, return value is 0xffffffff
unsigned int VpuWaitInterrupt(int driverid, unsigned int timeout)
{
	VPU_INFO_T vpu_info_element;
	//0 means to clear last interrupt and prepare next interrupt; other value indicate timeout value in ms
	vpu_info_element.off = timeout;
	
	vpu_info_element.value = &timeout;	//after ioctl, interrupt reason will be stored in timeout
	if (ioctl(driverid, VPU_WAITFORTIMEOUT, &vpu_info_element)) {
		printf ("%s: ioctl error!!!\n", __FUNCTION__);
		exit(1);
	}
	return timeout;
}

//please refer kernel driver source code to see the detail of VpuWaitInterrupt()
//If apmu_value==0xffffffff, VpuChangeClock() will return current VPU apmu clk res register value,
//otherwise, VpuChangeClock() will set theVPU apmu clk res register value to apmu_value
//return value: if apmu_value==0xffffffff, return current VPU apmu clk res register value
//				if apmu_value!=0xffffffff, return 0 means OK, other value means fail
unsigned int VpuChangeClock(int driverid, unsigned int apmu_value)
{
	VPU_INFO_T vpu_info_element;
	if(apmu_value == ~0) {
		vpu_info_element.off = 0;	//use off to indicate different purpose, 0 means read register
		vpu_info_element.value = &apmu_value;	//after ioctl, register value will be stored in apmu_value
		if (ioctl(driverid, VPU_CLOCK_CHANGE, &vpu_info_element)) {
			printf ("%s: ioctl error!!!\n", __FUNCTION__);
			exit(1);
		}
		return apmu_value;
	}else{
		vpu_info_element.off = 1;
		vpu_info_element.value = (void*)apmu_value;	//in ioctl, apmu_value will be write to register
		if (ioctl(driverid, VPU_CLOCK_CHANGE, &vpu_info_element)) {
			printf ("%s: ioctl error!!!\n", __FUNCTION__);
			exit(1);
			//return -1;
		}
		return 0;
	}
}

//bOn=1 to power on VPU, bOn=0 to power off VPU.
//succeed return 0, fail return others
int VpuPowerOnOff(int driverid, int bOn)
{
	VPU_INFO_T vpu_info_element;
	vpu_info_element.off = (bOn == 0? 4 : 5);
	if (ioctl(driverid, VPU_GETSET_INFOMATION, &vpu_info_element)) {
		printf ("%s(): power on off %d(0x%x) VPU fail!!!\n", __FUNCTION__, bOn, driverid);
		return -1;
	}
	return 0;
}

static unsigned int _VpuGetFirmwareBufPhyAddr(int driverid, int* pSize)
{
	unsigned int addr;
	int sz;
	VPU_INFO_T ioctl_par;
	ioctl_par.off = (unsigned int)(&sz);
	ioctl_par.value = &addr;

	if (ioctl(driverid, VPU_GET_RESERVEDMEM, &ioctl_par)) {
		printf ("%s: ioctl error!!!\n", __FUNCTION__);
		exit(1);
	}
	*pSize = sz;
	return addr;
}

static int _VpuGetSetFWDownloaded(int driverid, int option)
{
	VPU_INFO_T ioctl_par;
	int ret = 0;
	if(option == 0) {	//get firmware downloaded state
		ioctl_par.off = 0;
		ioctl_par.value = &ret;
	}else if(option == 1){	//inform kernel, firmware is downloaded
		ioctl_par.off = 1;
	}else{
		printf("%s: not support option %d\n", __FUNCTION__, option);
		exit(1);	//not supported option
	}
	if (ioctl(driverid, VPU_GETSET_INFOMATION, &ioctl_par)) {
		printf ("%s: ioctl error!!!\n", __FUNCTION__);
		exit(1);
	}
	return ret;
}


//return 0 means ok, other value means error. VpuDevice_Get() is to get the firmware memory, and copy firmware to this memory.
int VpuDevice_Get(int driverid, unsigned int* pFirmwarePhyAddr, unsigned int* pFirmwareVirAddr, int (*pfun_download)(int, unsigned int))
{
	if(g_firmwarebuf_viraddr == 0) {
		g_firmwarebuf_phyaddr = _VpuGetFirmwareBufPhyAddr(driverid, &g_firmwarebuf_sz);
		if(g_firmwarebuf_phyaddr == 0) {
			printf("_VpuGetFirmwareBufPhyAddr error, ret 0\n");
			return -1;
		}
		printf("_VpuGetFirmwareBufPhyAddr() get firmware mem phy addr %p, sz %d\n", (void*)g_firmwarebuf_phyaddr, g_firmwarebuf_sz);
#ifdef CNM_KERNEL_USEBMM
		g_firmwarebuf_viraddr = (unsigned int)bmm_attach((void*)g_firmwarebuf_phyaddr, g_firmwarebuf_sz);
		printf("bmm_attach() return FW mem vir addr %p\n", (void*)g_firmwarebuf_viraddr);
#else
		g_firmwarebuf_viraddr = (unsigned int)mmap(NULL, g_firmwarebuf_sz, PROT_READ|PROT_WRITE, MAP_SHARED, driverid, 0);
		printf("mmap() return FW mem vir addr %p\n", (void*)g_firmwarebuf_viraddr);
#endif
		
		if(g_firmwarebuf_viraddr == 0 || g_firmwarebuf_viraddr == MAP_FAILED) {
			printf("Get FW mem vir addr in user space fail, in %s\n", __FUNCTION__);
			return -2;
		}
	}
	*pFirmwarePhyAddr = g_firmwarebuf_phyaddr;
	*pFirmwareVirAddr = g_firmwarebuf_viraddr;
	
	g_firmwarebuf_refcnt++;

	if(_VpuGetSetFWDownloaded(driverid, 0) == 0) {
		//Following code download firmware. Copy firmware to firmware memory.
		if(pfun_download(driverid, g_firmwarebuf_phyaddr) != 0) {
			printf("download firmware error in %s\n", __FUNCTION__);
			return -3;
		}
		_VpuGetSetFWDownloaded(driverid, 1);
	}

	return 0;
}

//return 0 means ok, other value means error. VpuDevice_Rel() is to release firmware memory.
int VpuDevice_Rel(int driverid)
{
	g_firmwarebuf_refcnt--;
	if(g_firmwarebuf_refcnt == 0) {
#ifdef CNM_KERNEL_USEBMM
		bmm_detach((void*)g_firmwarebuf_viraddr, g_firmwarebuf_sz);
		printf("bmm_detach() called in %s()\n", __FUNCTION__);
#else
		int rt = munmap((void*)g_firmwarebuf_viraddr, g_firmwarebuf_sz);
		printf("munmap() called, ret %d, in %s()\n", rt, __FUNCTION__);
#endif
		g_firmwarebuf_viraddr = 0;
	}else if(g_firmwarebuf_refcnt < 0) {
		g_firmwarebuf_refcnt = 0;
		return -1;
	}
	return 0;
}



//return value is the slot index, 0~3 means ok, other value means error 
int VpuSlot_Get(int driverid)
{
	int ret;
	VPU_INFO_T ioctl_par;
	ioctl_par.off = 2;
	ioctl_par.value = &ret;

	if (ioctl(driverid, VPU_GETSET_INFOMATION, &ioctl_par)) {
		printf ("%s: ioctl error!!!\n", __FUNCTION__);
		exit(1);
	}
	printf("VpuSlot_Get(%d) return %d\n", driverid, ret);
	return ret;
}

//return 0 means ok, other value means error 
int VpuSlot_Rel(int driverid, int slot_idx)
{
	printf("VpuSlot_Rel(%d,) called %d\n", driverid, slot_idx);
	VPU_INFO_T ioctl_par;
	ioctl_par.off = 3;
	ioctl_par.value = (void*)slot_idx;

	if (ioctl(driverid, VPU_GETSET_INFOMATION, &ioctl_par)) {
		printf ("%s: ioctl error!!!\n", __FUNCTION__);
		exit(1);
	}
	return 0;
}



//NOTE: the unit is 100 usecond, therefore cnm_os_api_sleep(3) will sleep 3*100 usecond
//return 0 means ok, other value means something wrong
int	cnm_os_api_sleep(int useconds_in100)
{
	return usleep(useconds_in100 * 100);
}

//return the virtual address
void* cnm_os_api_vmalloc(unsigned int sz)
{
	return malloc(sz);	//common malloc is OK, which is used to allocate some structure to store codec information in user space
}

//p is the virtual address, and the memory is allocated from cnm_os_api_vmalloc()
void cnm_os_api_vfree(void* p)
{
	free(p);
	return;
}


//allocate physical continuous memory, and get virtual address & phyical address.
//pMemInfo->iAlign and pMemInfo->iSize are input parameter
//return 0 is ok, other value is fail
int cnm_os_api_pcontmem_alloc(PhyContMemInfo* pMemInfo)
{
	int align = pMemInfo->iAlign;	//for CNM, the alignment requirement won't exceed 32
	int iSize = pMemInfo->iSize;
	unsigned int phyAddr;
	void* p;
#ifndef CNM_USERSPACE_USEBMM
	struct pmem_handle_mrvl* pmemhandle = pmem_malloc(iSize, MARVELL_PMEMDEV_NAME_NONCACHED);
	if(pmemhandle == NULL) {
		return -1;
	}
	if( (unsigned int)pmemhandle->pa & (align-1) ) {	//check whether meets alignment requirement. In fact, pmem allocated memory are always pagesize aligned.
		pmem_free(pmemhandle);
		pMemInfo->virAddr = NULL;
		pMemInfo->phyAddr = 0;
		return -1;
	}
	pMemInfo->cnm_os_api_mem_helper = (void*)pmemhandle;	//pMemInfo->cnm_os_api_mem_helper won't be accessed by C&M codec API. It is only accessed by cnm_os_api_pcontmem_alloc()/cnm_os_api_pcontmem_free(). The usage of it is depended on implementation of cnm_os_api_pcontmem_alloc().
	p = (void*)pmemhandle->va;
	phyAddr = (unsigned int)pmemhandle->pa;
#else
	//p = bmm_malloc(iSize, BMM_ATTR_NONCACHEABLE | BMM_ATTR_NONBUFFERABLE);
	p = bmm_malloc(iSize, BMM_ATTR_NONCACHED);//BMM_ATTR_NONCACHED == BMM_ATTR_NONCACHEABLE | BMM_ATTR_NONBUFFERABLE, for new bmm, suggest to use BMM_ATTR_NONCACHED
	if( (unsigned int)p & (align-1) ) {	//in fact, bmm_malloc() always return 4096 aligned memory
		bmm_free(p);
		pMemInfo->virAddr = NULL;
		pMemInfo->phyAddr = 0;
		return -1;
	}
	phyAddr = (unsigned int)bmm_get_paddr(p);
	if(phyAddr == 0) {
		bmm_free(p);
		pMemInfo->virAddr = NULL;
		pMemInfo->phyAddr = 0;
		return -1;
	}
#endif

	pMemInfo->virAddr = p;
	pMemInfo->phyAddr = phyAddr;
	return 0;
}

//free physical continuous memory which allocated by cnm_os_api_pcontmem_alloc()
//return 0 is ok, other value is fail
int cnm_os_api_pcontmem_free(PhyContMemInfo* pMemInfo)
{
#ifndef CNM_USERSPACE_USEBMM
	if(pMemInfo->cnm_os_api_mem_helper == NULL) {
		return -1;
	}
	pmem_free(pMemInfo->cnm_os_api_mem_helper);
	pMemInfo->cnm_os_api_mem_helper = NULL;
#else
	bmm_free(pMemInfo->virAddr);
#endif
	pMemInfo->virAddr = NULL;
	pMemInfo->phyAddr = 0;
	return 0;
}

//copy physical continuous memory data, pDst and pSrc are all virtual address and allocated from cnm_os_api_pcontmem_alloc(), pDst probably isn't the PhyContMemInfo.virAddr, but (PhyContMemInfo.virAddr + xxx)
//cnm_os_api_pcontmem_common_cpy() is only called in cnm encoder API 
void* cnm_os_api_pcontmem_common_cpy(void* pDst, const void* pSrc, unsigned int sz)
{
	return memcpy(pDst, pSrc, sz);
}

#if 0	//From 2010.11.04, cnm_os_api_dma_malloc()/cnm_os_api_dma_get_pa()/cnm_os_api_dma_free()/cnm_os_api_dma_cpy() are replaced with cnm_os_api_pcontmem_alloc()/cnm_os_api_pcontmem_free()/
//cnm_os_api_dma_xxx APIs are only used by CM encoder currently, allocate physical continuous memory, *ppAddr is virtual address 
void cnm_os_api_dma_malloc(void** ppAddr, int iSize, int align)
{
	//void* p = bmm_malloc(iSize, BMM_ATTR_NONCACHEABLE | BMM_ATTR_NONBUFFERABLE);
	void* p = bmm_malloc(iSize, BMM_ATTR_NONCACHED);//BMM_ATTR_NONCACHED == BMM_ATTR_NONCACHEABLE | BMM_ATTR_NONBUFFERABLE, for new bmm, suggest to use BMM_ATTR_NONCACHED
	if( (unsigned int)p & (align-1) ) {	//in fact, bmm_malloc() always return 4096 aligned memory
		bmm_free(p);
		*ppAddr = (void*)0;
	}
	*ppAddr = p;
	return;
}

/* From 2010.10.21, this function isn't needed any more.
//transfer phyical address to virtual address, the memory must be allcoated from cnm_os_api_dma_malloc()
void* cnm_os_api_dma_get_va(void* phyAddr)
{
	return bmm_get_vaddr(phyAddr);
}
*/

//transfer virtual address to phyical address, the memory must be allcoated from cnm_os_api_dma_malloc()
void* cnm_os_api_dma_get_pa(void* virAddr)
{
	return bmm_get_paddr(virAddr);
}

//free cnm_os_api_dma_malloc() allocated memory, *ppAddr is virtual address
void cnm_os_api_dma_free(void** ppAddr)
{
	bmm_free(*ppAddr);
	*ppAddr = (void*)0;
	return;
}

void* cnm_os_api_dma_cpy(void* pDst, const void* pSrc, unsigned int sz)	//pDst, pSrc all are virtual address
{
	return memcpy(pDst, pSrc, sz);
}
#endif

//for power setting, suspend and resume operation. NOTE: 2011.07.28. In fact, below 3 APIs haven't been used, implement them as returning 0 directly is OK.
//during VPU running, OS shouldn't enter suspend state. Call this API prevents OS from entering suspend
int cnm_os_api_blocksuspend(void)
{
	//not implement
	return 0;
}

//Call this API allow OS to enter suspend state
int cnm_os_api_unblocksuspend(void)
{
	//not implement
	return 0;
}

int cnm_os_api_disablePP(int reserved)
{
	//not implement
	return 0;
}


//VpuProfile(int driverid, int bRun) is only for performance profiling, you could implement it as an empty function.
//bRun=1 means VPU is going to start working, bRun=0 means VPU is going to start idling.
//succeed return 0, fail return others
int VpuProfile(int driverid, int bRun)
{
#if 1
	VPU_INFO_T vpu_info_element;
	vpu_info_element.off = (bRun? 0 : 1);
	if (ioctl(driverid, VPU_DVFMPROFILING, &vpu_info_element)) {
		printf ("%s: ioctl error!!!\n", __FUNCTION__);
		exit(1);
		return -1;
	}
	return 0;
#else
	return 0;
#endif
}

//VpuQueryDecResolutionForbidden(int driverid, int w, int h) is only for some special platform like pxa918, which forbid c&m to decode 720p stream. For common platform, let this API return 0 is ok.
//return 0 means resolution (width, height) is allowed, return 1 means forbidden, return other value means some error happened.
//w and h should be in range [1, 65535]
int VpuQueryDecResolutionForbidden(int driverid, int width, int height)
{
#if 1
	VPU_INFO_T vpu_info_element;
	int forbid = 0;
	vpu_info_element.off = (width<<16) | (height&0xffff);
	vpu_info_element.value = &forbid;	//kernel driver will fill return value in forbid
	if (ioctl(driverid, VPU_QUERYHWLIMIT, &vpu_info_element)) {
		printf ("%s: ioctl error!!!\n", __FUNCTION__);
		exit(1);
		return -1;
	}
	return forbid;
#else
	return 0;
#endif
}

//VpuInformCodecType() is used to inform kernel driver the codec type.
int VpuInformCodecType(int driverid, int codecTypeId)
{
	printf("%s(%d, %d) called\n", __FUNCTION__, driverid, codecTypeId);
	VPU_INFO_T ioctl_par;
	ioctl_par.off = 6;
	ioctl_par.value = (void*)codecTypeId;

	if (ioctl(driverid, VPU_GETSET_INFOMATION, &ioctl_par)) {
		printf ("%s: ioctl error!!!\n", __FUNCTION__);
		exit(1);
		return -1;
	}
	return 0;
}