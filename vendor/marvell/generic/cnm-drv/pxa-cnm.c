/*
 *  pxa-cnm main driver for chip&media codad8x codec
 *
 *  Copyright (C) 2009, Marvell International Ltd.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/device.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/uio.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/fb.h>
#include <linux/dma-mapping.h>
#include <linux/clk.h>
#include <linux/miscdevice.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/page.h>
#include <asm/mman.h>
#include <asm/pgtable.h>
#include <asm/uaccess.h>
#include <asm/cacheflush.h>
#include <mach/cputype.h>

#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 39))
#define CNMKERN_USE_UNLOCKEDIOCTL
#define CNMKERN_QOS_INTF_UPDATE
#endif

//#define CNMPOWER_SETJTAG	//remove JTAG operation, we dnn't support A0 any more, 20110504
#ifdef CNMPOWER_SETJTAG
#include <mach/regs-apbc.h>
#endif
#include <mach/regs-apmu.h>

#define CNMFREQ_SETTO208
//#define CNMPOWER_SOFTWARE_ONOFF
#define CNMPXA921CHECK_ENABLED
#define CNMPXA918CHECK_ENABLED

#if defined(CONFIG_DVFM)
#include <mach/dvfm.h>		//for .32 kernel
static int dvfm_dev_idx;
static void set_dvfm_constraint(void)
{
	/* Disable 78M, 156M, 312M */
	dvfm_disable_op_name("78MHz", dvfm_dev_idx);
	dvfm_disable_op_name("156MHz", dvfm_dev_idx);
	dvfm_disable_op_name("208MHz", dvfm_dev_idx);
	dvfm_disable_op_name("312MHz", dvfm_dev_idx);
}
static void unset_dvfm_constraint(void)
{
	/* Enable 78M, 156M, 312M*/
	dvfm_enable_op_name("78MHz", dvfm_dev_idx);
	dvfm_enable_op_name("156MHz", dvfm_dev_idx);
	dvfm_enable_op_name("208MHz", dvfm_dev_idx);
	dvfm_enable_op_name("312MHz", dvfm_dev_idx);
}
#else
#include <linux/pm_qos_params.h>	//on .35 kernel, we removed DVFM framework and make use of PM QoS as power constraints framework.
static struct pm_qos_request_list *pxa_cnm_qos_req_min = NULL;
#ifdef CNMKERN_QOS_INTF_UPDATE
static struct pm_qos_request_list req;
#endif
#endif

#if defined(CONFIG_DVFM) && defined(CONFIG_PXA910_DVFM_STATS)	//below APIs only for performance profile, not implement them is allowed
static __inline void vpuprofile_init(void)
{
	start_profile(1,1,2);
}
//in busybegin(), will calculate the ticks passed during last idle interval
static __inline void vpuprofile_busybegin(void)
{
	start_profile(1,0,1);
}
//in idlebegin(), will calculate the ticks passed during last busy interval
static __inline void vpuprofile_idlebegin(void)
{
	start_profile(1,1,1);
}
#else
static __inline void vpuprofile_init(void) {return;}
static __inline void vpuprofile_busybegin(void) {return;}
static __inline void vpuprofile_idlebegin(void) {return;}
#endif


#define BIT_INT_CLEAR		(0xC)
#define BIT_BUSY_FLAG		(0x160)
#define BIT_INT_ENABLE		(0x170)
#define BIT_INT_REASON		(0x174)

////////////////////////////////////////////////////////////
/////
//  // DX8_CLK_RES definition:
    // bit[1:0]: {func_reset, axi_reset}
	// bit [5]: dx8_clk_sel: 1-208M, 0-312M
    // bit[4:3]: {func_clk_en, axi_clk_en}
    // bit[10:6]:  dx8_clk_div
    // bit[18:16]: {pwr_on2, pwr_on1, isob}
    // bit[19]: hw_mode
#define DX8_AXI_RST   0x1
#define DX8_FUNC_RST  0x2
#define DX8_RST_OFF       0x3	//(DX8_AXI_RST | DX8_FUNC_RST)
#define DX8_FUNC_CLK  0x10
#define DX8_AXI_CLK   0x8
#define DX8_CLK_DIV   0x40
#define DX8_CLK_SEL_208   0x20
#ifdef CNMFREQ_SETTO208
#define DX8_CLK_EN    0x78		//(DX8_AXI_CLK | DX8_CLK_SEL_208 | DX8_FUNC_CLK | DX8_CLK_DIV)	//For DX8_CLK_EN, because bit[5] == 1, bit[10:6] = 1, therefore, Dx8 clock equal to 208/1 = 208M
#else
#define DX8_CLK_EN    0x58		//(DX8_AXI_CLK | DX8_FUNC_CLK | DX8_CLK_DIV)	//For DX8_CLK_EN, because bit[5] == 0, bit[10:6] = 1, therefore, Dx8 clock equal to 312M/1 = 312M
#endif
#define DX8_HW_MODE   0x80000
#define DX8_ON2       0x40000
#define DX8_ON1       0x20000
#define DX8_PWR_ON    0x60000	//(DX8_ON2 | DX8_ON1)
#define DX8_ISO       0x10000


#undef CNM_DEBUG
#ifdef CNM_DEBUG
#define DBG_CNM(x)	do{x;}while(0)
#else
#define DBG_CNM(x)
#endif
#ifdef CONFIG_PROC_FS
#define	CNM_PROC_FILE	"driver/cnm"
static struct proc_dir_entry *cnm_proc_file;
//static int index;
static int pxa_cnm_power(int on);

static ssize_t cnm_proc_read(struct file *filp,
		char *buffer, size_t length, loff_t *offset)
{
	//this function is useless in fact, let it be empty function is ok
#if 0
	unsigned int reg_val;

	reg_val = __raw_readl(APMU_DX8_CLK_RES_CTRL);
	printk(KERN_INFO "register 0x%x\n", reg_val);
#endif
	return 0;
}

static ssize_t cnm_proc_write(struct file *filp,
		const char *buff, size_t len, loff_t *off)
{
	//this function is useless in fact, let it be empty function is ok
#if 0
	char messages[256];
	unsigned int reg_val;

	if (copy_from_user(messages, buff, len))
		return -EFAULT;

	reg_val = (unsigned int)simple_strtoul(messages, NULL, 16);
	printk(KERN_INFO "\nready to write %x\n", reg_val);
	__raw_writel(reg_val, APMU_DX8_CLK_RES_CTRL);
#endif
	return len;
}

static struct file_operations cnm_proc_ops = {
	.read = cnm_proc_read,
	.write = cnm_proc_write,
};

static void create_cnm_proc_file(void)
{
	cnm_proc_file = create_proc_entry(CNM_PROC_FILE, 0644, NULL);
	if (cnm_proc_file) {
		//for 32 kernel
		//cnm_proc_file->owner = THIS_MODULE;
		cnm_proc_file->proc_fops = &cnm_proc_ops;
	} else
		printk(KERN_INFO "proc file create failed!\n");
}
#endif

struct vpu_info {
	unsigned int	off;
	void		*value;
};

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

struct pxa_cnm {
	struct clk              *clk;
	void __iomem		*mmio_base;
	//int			vpu_downloaded;
	int			current_act;
	int			filehandle_ins_num;
	struct mutex		lock;		//used to protect open/close
	unsigned int		status;		//record interrupt reason
};

#define KERN_CNM_MEM_ALLOC_METHOD_BMM				0	//we have three method to allocate c&m firmware memory. User space code also are different for those three method because user space need to query the virtual address of the firmware memory.
#define KERN_CNM_MEM_ALLOC_METHOD_KMALLOC			1
#define KERN_CNM_MEM_ALLOC_METHOD_DMACOHERENT		2
#define KERN_CNM_MEM_ALLOC_METHOD_DEDICATED			3
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 0, 0))
#define KERN_CNM_MEM_ALLOC_METHOD					KERN_CNM_MEM_ALLOC_METHOD_DEDICATED//KERN_CNM_MEM_ALLOC_METHOD_KMALLOC//KERN_CNM_MEM_ALLOC_METHOD_DMACOHERENT
#else
#define KERN_CNM_MEM_ALLOC_METHOD					KERN_CNM_MEM_ALLOC_METHOD_KMALLOC
#endif

#define KERN_CNM_MEM_DEDICATED_DEFAULT_ADDR			0x08000000
#define KERN_CNM_MEM_RESERVED_FOR_FIRMWARE_DEFAULT_SZ (2 * 1024 * 1024)		//reserved c&m firmware memory size, which is to hold c&m firmware, the memory should be non-cacheable & non-buffered
static void* reservedmem_viraddr = 0;
static void *reservedmem = 0;
static __inline long get_cnm_firmware_mem_sz(void)
{
	return KERN_CNM_MEM_RESERVED_FOR_FIRMWARE_DEFAULT_SZ;
}
static __inline long get_cnm_firmware_mem_dedicated_addr(void)
{
	return KERN_CNM_MEM_DEDICATED_DEFAULT_ADDR;
}

static int is_cnmfirmware_downloaded = 0;		//There is two steps to start up firmware, 1: download firmware, 2: lood firmware boot code and run it. is_cnmfirmware_downloaded is only used to indicate the 1st step. Once power off, the 1st step needn't re-do, but the 2nd step need re-do. If PC register of CNM is 0, means need to do 2nd step.
static int is_cnm_power_on = 0;
static int is_cnm_clock_on = 0;
static struct semaphore sema;
static struct completion cmd_complete;
static struct pxa_cnm pxa_cnm;
#define VPU_CODEC_TYPEID_ENC_H264		0x1		//definition should be same as user space lib
#define VPU_CODEC_TYPEID_ENC_MPEG4		0x2
#define VPU_CODEC_TYPEID_ENC_H263		0x4
#define VPU_CODEC_TYPEID_DEC_H264		0x10000
#define VPU_CODEC_TYPEID_DEC_MPEG4		0x20000
#define VPU_CODEC_TYPEID_DEC_H263		0x40000
#define VPU_CODEC_TYPEID_DEC_MPEG2		0x80000
#define VPU_CODEC_TYPEID_DEC_VC1		0x100000
//use following data structure to assicate fd and codec instance, one fd corresponding to one codec instance
struct Cnm_fd_codecinst {
	int bOccupied;
	int bGotSemaphore;
	int vpuslotidx;	//it also act as the item idx in cnm_fd_codecinst_arr[]
	int bSlotIsUsing;
	unsigned int codecTypeId;
	unsigned int reserved;
};
#define MAX_NUM_VPUSLOT				4			//cnm vpu only has 4 slot
#define CNM_FD_CODECINST_ARRAY_SZ	(MAX_NUM_VPUSLOT+1)
static struct Cnm_fd_codecinst cnm_fd_codecinst_arr[CNM_FD_CODECINST_ARRAY_SZ];	//in fact, 4 is enough because vpu only has 4 slot, so at the same time, at most 4 codec instance could co-exist. The 5th fd could be used for special usage when MAX_NUM_CNMFILEHANDLE is defined as 5 in future.
#define MAX_NUM_CNMFILEHANDLE	4

static __inline void VpuWriteReg(unsigned int off, unsigned int value)
{
	__raw_writel((value), pxa_cnm.mmio_base + (off));
}

static __inline unsigned int VpuReadReg(unsigned int off)
{
	return __raw_readl(pxa_cnm.mmio_base + (off));
}

//active_pxacnm is used to prepare/shutdown whole hardware, not for one instance
static int active_pxacnm(int on)
{
	int i;
	int rt;
	if(on == 1) {
		//active cnm
		//power on
		if(! is_cnm_power_on) {
			rt = pxa_cnm_power(1);
			if(rt != 0) {
				return rt;
			}
		}

		//init completion and sema
		init_completion(&cmd_complete);
		sema_init(&sema, 1);
	}else{
		//deactive cnm
		//power off
		if(is_cnm_power_on) {
			rt = pxa_cnm_power(0);
			if(rt != 0) {
				return rt;
			}
		}
	}

	pxa_cnm.status = 0;
	is_cnmfirmware_downloaded = 0;

	//clear fd_codecinst
	for(i=0;i<CNM_FD_CODECINST_ARRAY_SZ;i++) {
		cnm_fd_codecinst_arr[i].bOccupied = 0;
		cnm_fd_codecinst_arr[i].bSlotIsUsing = 0;
		cnm_fd_codecinst_arr[i].codecTypeId = 0;
	}
	return 0;
}

static int AbnormalStopCnmVPU(void)
{
	int i;
	printk("process killed abnormal, AbnormalStopCnmVPU() called.\n");
	//currently, we couldn't do other thing except waiting for CNM VPU stop by itself
	for(i=0;i<6;i++) {
		schedule_timeout(msecs_to_jiffies(5));
		if(0 == VpuReadReg(BIT_BUSY_FLAG)) {
			return 0;
		}
	}
	return -1;
}

static int pxa_cnm_open(struct inode *inode, struct file *file)
{
	int ret = 0;
	int i;
	printk(KERN_INFO "Enter pxa_cnm_open(%p, %p)\n", inode, file);
	mutex_lock(&pxa_cnm.lock);
	if (pxa_cnm.filehandle_ins_num < MAX_NUM_CNMFILEHANDLE) {
		pxa_cnm.filehandle_ins_num ++;
	}else {
		printk(KERN_ERR "Current CNM cannot accept more file handle instance!!\n");
		ret = -EACCES;
		goto out;
	}

	/* open the hardware power, do some init/reset work*/
	if(pxa_cnm.filehandle_ins_num == 1) {
		if( 0 != active_pxacnm(1) ) {
			pxa_cnm.filehandle_ins_num = 0;
			ret = -EACCES;
			goto out;
		}
	}
	printk(KERN_DEBUG "Current CNM Get %d file handle instance!!\n", pxa_cnm.filehandle_ins_num);
	if(pxa_cnm.filehandle_ins_num > MAX_NUM_VPUSLOT) {
		//file->private_data = &(cnm_fd_codecinst_arr[MAX_NUM_VPUSLOT]);	//In future, the 5th, 6th and ... file handle are all refer to the 5th fd_codecinst. The 5th fd_codecinst won't refer to any real codec instance, but only be used to do some trick, like capture some information.
	}else{
		for(i=0;i<MAX_NUM_VPUSLOT;i++) {
			if(cnm_fd_codecinst_arr[i].bOccupied == 0) {
				file->private_data = &(cnm_fd_codecinst_arr[i]);
				cnm_fd_codecinst_arr[i].bOccupied = 1;
				cnm_fd_codecinst_arr[i].bGotSemaphore = 0;
				break;
			}
		}
		if(i == MAX_NUM_VPUSLOT) {
			pxa_cnm.filehandle_ins_num--;
			printk(KERN_ERR "Couldn't found any idle seat for new cnm fd!!\n");	
			ret = -EACCES;
			goto out;
		}
	}
	

out:
	mutex_unlock(&pxa_cnm.lock);
	printk(KERN_INFO "Leave pxa_cnm_open(%p, %p)\n", inode, file);
	return ret;
}


static int pxa_cnm_close(struct inode *inode, struct file *file)
{
	int ret = 0;
	struct Cnm_fd_codecinst* pinst;
	printk(KERN_INFO "Enter pxa_cnm_close(%p, %p)\n", inode, file);
	pinst = (struct Cnm_fd_codecinst*)(file->private_data);
	mutex_lock(&pxa_cnm.lock);

	//if this codec instance got semaphore, it means this codec instance want VPU to do something, and other codec instance will wait. If process is killed before VPU finished working for this codec instance and instance release semaphore, we need do something. 
	if(pinst->bGotSemaphore) {
		if(is_cnm_clock_on) {
			if(VpuReadReg(BIT_BUSY_FLAG) != 0) {
				AbnormalStopCnmVPU();
			}
		}
		DBG_CNM(printk("need unlock at fd close\n"));
		up(&sema);	//release semaphore
		pinst->bGotSemaphore = 0;
	}

	if(pinst->bSlotIsUsing) {
		//If cnm has decoded/encoded some frame, the slot could be directly reuse without call instance codec API to close this instance. It's verified by C&M supply.
		pinst->bSlotIsUsing = 0;
	}

	pinst->bOccupied = 0;
	pinst->codecTypeId = 0;

	file->private_data = NULL;
	
	if(pxa_cnm.filehandle_ins_num > 0) {
		pxa_cnm.filehandle_ins_num--;
		if(pxa_cnm.filehandle_ins_num == 0) {
			if(0 != active_pxacnm(0)) {
				ret = -EFAULT;
			}
		}
	}
	mutex_unlock(&pxa_cnm.lock);
	printk(KERN_DEBUG "Current CNM left %d file handle instances, ret %d!!\n", pxa_cnm.filehandle_ins_num, ret);
	printk(KERN_INFO "Leave pxa_cnm_close(%p, %p)\n", inode, file);
	return ret;
}

#if KERN_CNM_MEM_ALLOC_METHOD != KERN_CNM_MEM_ALLOC_METHOD_BMM
static int pxa_cnm_mmap(struct file *file, struct vm_area_struct *vma)
{
	int ret;
	printk(KERN_INFO "%s() is called\n", __FUNCTION__);
	if(reservedmem_viraddr == 0 || reservedmem == 0) {
		return -ENOMEM;
	}
	if(vma->vm_end - vma->vm_start > get_cnm_firmware_mem_sz()) {
		return -EINVAL;
	}
#if KERN_CNM_MEM_ALLOC_METHOD == KERN_CNM_MEM_ALLOC_METHOD_DMACOHERENT
	ret = dma_mmap_coherent(NULL, vma, reservedmem_viraddr, reservedmem, vma->vm_end - vma->vm_start);
#elif (KERN_CNM_MEM_ALLOC_METHOD == KERN_CNM_MEM_ALLOC_METHOD_KMALLOC) || (KERN_CNM_MEM_ALLOC_METHOD == KERN_CNM_MEM_ALLOC_METHOD_DEDICATED)
	vma->vm_flags |= VM_IO | VM_RESERVED;
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	ret = remap_pfn_range(vma,
			       vma->vm_start,
			       (unsigned long)reservedmem >> PAGE_SHIFT,
			       vma->vm_end - vma->vm_start,
			       vma->vm_page_prot);
#endif
	printk(KERN_INFO "%s() will return %d", __FUNCTION__, ret);
	return ret;
}
#endif

//2011.08, bfang1, found current c&m firmware h264 encoder couldn't support power off during encoding(encoder output bitstream will different). Therefore, only allow power off when there is no h264 encoder.
#define NO_CNM_H264ENC_LIVING					( ((cnm_fd_codecinst_arr[0].codecTypeId|cnm_fd_codecinst_arr[1].codecTypeId|cnm_fd_codecinst_arr[2].codecTypeId|cnm_fd_codecinst_arr[3].codecTypeId)&VPU_CODEC_TYPEID_ENC_H264) == 0 )
//#define could_power_off_at_codecliving()		(NO_CNM_H264ENC_LIVING)
#define could_power_off_at_codecliving()		(pxa_cnm.filehandle_ins_num<=1 || NO_CNM_H264ENC_LIVING)	//Sometimes, we could accept h264 encoder output isn't unique. Therefore, if there is only 1 instance and the instance is h264 encoder, we allow power off though the output of h264 encoder probably isn't unique.

#ifndef CNMKERN_USE_UNLOCKEDIOCTL
static int pxa_cnm_ioctl(struct inode *inode, struct file *file, u_int cmd, u_long arg)
#else
static long pxa_cnm_ioctl(struct file *file, u_int cmd, u_long arg)
#endif
{
	void __user *argp = (void __user *)arg;
	struct vpu_info vpu_info;
	unsigned int value;
	int ret;
	struct Cnm_fd_codecinst* pinst = (struct Cnm_fd_codecinst*)(file->private_data);

	if (copy_from_user(&vpu_info, argp, sizeof(struct vpu_info)))
		return -EFAULT;

	switch (cmd) {
	case VPU_READREG:
	{
		if(is_cnm_clock_on == 0) {
			return -EFAULT;		//read reg only valid when clock is on
		}
		value = VpuReadReg(vpu_info.off);
		if (copy_to_user(vpu_info.value, &value, sizeof(unsigned int)))
			return -EFAULT;
		break;
	}
	case VPU_WRITEREG:
	{
		if(is_cnm_clock_on == 0) {
			return -EFAULT;		//write reg only valid when clock is on
		}
		VpuWriteReg(vpu_info.off, (unsigned int)(vpu_info.value));
		break;
	}
	case VPU_WAITFORTIMEOUT:
	{
		if(vpu_info.off == 0) {	//use off to indicate the different purpose, if off==0, means clear interrupt, otherwise, off is timeout value in ms
			//clear last interrupt and prepare for next interrupt
			value = pxa_cnm.status;
			pxa_cnm.status = 0;
			init_completion(&cmd_complete);		//Probably could use INIT_COMPLETION, some article said it is fast re-init.
		}else{
			//wait interrupt
			value = msecs_to_jiffies(vpu_info.off);
			ret = wait_for_completion_timeout(&cmd_complete, value);
			if(!ret) {
				value = ~0;
			}else{
				value = pxa_cnm.status;
			}
		}

		if(copy_to_user(vpu_info.value, &value, sizeof(unsigned int))) {
			return -EFAULT;
		}
		break;
	}
	case VPU_CLOCK_CHANGE:
	{
		if(vpu_info.off == 0) {	//use off to indicate the different purpose
			//read APMU_DX8_CLK_RES_CTRL
			value = __raw_readl(APMU_DX8_CLK_RES_CTRL);
			DBG_CNM(printk("read apmu Dx8 clk res value %x\n", value));
			if (copy_to_user(vpu_info.value, &value, sizeof(unsigned int))) {
				return -EFAULT;
			}
		}else{
			unsigned int clk_opt0 = (((unsigned int)(vpu_info.value)&DX8_FUNC_CLK) != 0);
			unsigned int clk_opt1 = (((unsigned int)(vpu_info.value)&DX8_AXI_CLK) != 0);
			if(clk_opt0 != clk_opt1) {
				return -EFAULT;	//those two clock option should be same
			}

#define DISABLE_IRQ_ATCLOCKONOFF
#ifdef DISABLE_IRQ_ATCLOCKONOFF	//Before close clock, mask irq. It's not must to have, just for safe. In fact, c&m shouldn't produce interrupt during clock on/off, because when application want to open/close c&m clock, the c&m should be in idle state.
			if(clk_opt0 == 0) {
				if(is_cnm_clock_on) {	//only when clock on, c&m register is accessable.
					//mask irq
					VpuWriteReg(BIT_INT_ENABLE, 0);		//disable c&m irq
				}
			}
#endif

			//write APMU_DX8_CLK_RES_CTRL
			DBG_CNM(printk("write apmu Dx8 clk res value to %x\n", (unsigned int)(vpu_info.value)));
			__raw_writel((unsigned int)(vpu_info.value), APMU_DX8_CLK_RES_CTRL);
			if(clk_opt0) {
				is_cnm_clock_on = 1;
			}else{
				is_cnm_clock_on = 0;
			}

#ifdef DISABLE_IRQ_ATCLOCKONOFF //After open clock, un-mask irq. It's not must to have, just for safe.
			if(clk_opt0) {
				//un-mask irq
				VpuWriteReg(BIT_INT_ENABLE, (1<<3) | (1<<14) | (1<<15));	//bit 3 is PIC_RUN_INT, bit 14 is DECSTM_EMPTY_INT, bit 15 is ENCSTM_FULL_INT. Currently, we only care about those 3 interrupts, it's consilient with c&m application.
			}
#endif
		}
		break;
	}
	case VPU_LOCK:
		{
			DBG_CNM(printk("cnm lock timeout val ms %d\n", (unsigned int)(vpu_info.off)));
			//(printk("cnm lock timeout val ms %d\n", (unsigned int)(vpu_info.off)));
			if(pinst->bGotSemaphore == 0) {
				value = down_timeout(&sema, msecs_to_jiffies(vpu_info.off));
				if(value == 0) {
					pinst->bGotSemaphore = 1;
				}
			}else{
				value = 0;	//has been locked
			}
			//printk("down_timeout ret %d\n", value);
			if (copy_to_user(vpu_info.value, &value, sizeof(unsigned int))) {
				return -EFAULT;
			}
		}
		break;
	case VPU_UNLOCK:
		{
			if(pinst->bGotSemaphore) {
				up(&sema);
				pinst->bGotSemaphore = 0;
			}
			DBG_CNM(printk("cnm unlock\n"));
			//(printk("cnm unlock\n"));
		}
		break;
	case VPU_GET_RESERVEDMEM:
		{
			int sz = get_cnm_firmware_mem_sz();
			DBG_CNM(printk("VPU_GET_RESERVEDMEM, %x, %d\n", reservedmem, sz));
			if(copy_to_user(vpu_info.value, &reservedmem, sizeof(void *))) {
				return -EFAULT;
			}
			if(copy_to_user((void*)vpu_info.off, &sz, sizeof(sz))) {
				return -EFAULT;
			}
		}
		break;
	case VPU_GETSET_INFOMATION:
		{
			DBG_CNM(printk("VPU_GETSET_INFOMATION par %d\n", (unsigned int)(vpu_info.off)));
			if(vpu_info.off == 0) {	//use off to indicate the different purpose
				//0 get is_cnmfirmware_downloaded
				if(copy_to_user(vpu_info.value, &is_cnmfirmware_downloaded, sizeof(is_cnmfirmware_downloaded))) {
					return -EFAULT;
				}
			}else if(vpu_info.off == 1) {
				//1 set is_cnmfirmware_downloaded
				is_cnmfirmware_downloaded = 1;
			}else if(vpu_info.off == 2){
				//2 get vpu slot
				int idx;
				if(pinst->bSlotIsUsing == 0 && pinst->vpuslotidx < MAX_NUM_VPUSLOT){
					idx = pinst->vpuslotidx;
				}else{
					idx = -1;
				}
				if(copy_to_user(vpu_info.value, &idx, sizeof(idx))) {
					return -EFAULT;
				}
				pinst->bSlotIsUsing = 1;
			}else if(vpu_info.off == 3){
				//3 release vpu slot
				if(pinst->vpuslotidx < MAX_NUM_VPUSLOT) {
					pinst->bSlotIsUsing = 0;
				}
			}else if(vpu_info.off == 4){
				//4 power off VPU
				mutex_lock(&pxa_cnm.lock);
				if(is_cnm_power_on && could_power_off_at_codecliving()) {
					if(0 != pxa_cnm_power(0)) {
						mutex_unlock(&pxa_cnm.lock);
						return -EFAULT;
					}
				}
				mutex_unlock(&pxa_cnm.lock);
			}else if(vpu_info.off == 5){
				//5 power on VPU
				mutex_lock(&pxa_cnm.lock);
				if(! is_cnm_power_on) {
					if(0 != pxa_cnm_power(1)) {
						mutex_unlock(&pxa_cnm.lock);
						return -EFAULT;
					}
				}
				mutex_unlock(&pxa_cnm.lock);
			}else if(vpu_info.off == 6){
				//6 inform codec type
				pinst->codecTypeId = (unsigned int)(vpu_info.value);
			}else{
				return -EFAULT;
			}
		}
		break;
	case VPU_DVFMPROFILING:
		{
			if(vpu_info.off == 0) {
				vpuprofile_busybegin();
			}else{
				vpuprofile_idlebegin();
			}
		}
		break;
	case VPU_QUERYHWLIMIT:
		//2011.07.27, currently, this ioctl is only for some special platform, like pxa918, which forbid c&m to decode 720p stream. For common platform, let limit = 0 is ok.
		{
			int limit = 0;
#ifdef CNMPXA918CHECK_ENABLED
			if(cpu_is_pxa918()) {
				unsigned long w,h;
				w = (unsigned long)(vpu_info.off) >> 16;
				h = (unsigned long)(vpu_info.off) & 0xffff;
				if((w >= 1280 && h >= 720) || (w >=720 && h >= 1280)) {	//We tried 848x848 mpeg2 video on pxa918, it's ok. It only couldn't decode 1280x720 and bigger resolution.
					limit = 1;
				}
			}
#endif
			if(copy_to_user(vpu_info.value, &limit, sizeof(limit))) {
				return -EFAULT;
			}
		}
		break;
	default:
	{
		return -EFAULT;
	}
	}

	return 0;
}

static struct file_operations pxa_cnm_fops = {
	.owner          = THIS_MODULE,
	.open           = pxa_cnm_open,
	.release        = pxa_cnm_close,
#ifndef CNMKERN_USE_UNLOCKEDIOCTL
	.ioctl          = pxa_cnm_ioctl,
#else
	.unlocked_ioctl = pxa_cnm_ioctl,
#endif
#if KERN_CNM_MEM_ALLOC_METHOD != KERN_CNM_MEM_ALLOC_METHOD_BMM
	.mmap			= pxa_cnm_mmap,
#endif
};

static struct miscdevice pxa_cnm_miscdev = {
	.minor  = MISC_DYNAMIC_MINOR,
	.name   = "cnm",
	.fops   = &pxa_cnm_fops,
	.this_device = NULL,
};

static irqreturn_t pxa_cnm_irq (int irq, void *devid)
{
	unsigned int status;
	status = VpuReadReg(BIT_INT_REASON);
	pxa_cnm.status |= status;
	DBG_CNM(printk("##cnm irq %x, busy %x\n", status, VpuReadReg(BIT_BUSY_FLAG)));
	VpuWriteReg(BIT_INT_REASON, 0);
	VpuWriteReg(BIT_INT_CLEAR, 0x1);
	complete(&cmd_complete);
	return IRQ_HANDLED;
}

static int pxa_cnm_power(int on)
{
#define PXA_CNM_POWER_ONOFF_TIMEOUT		10000		//in usec
#define PXA_CNM_POWER_ONOFF_INTERVAL	1000		//in usec
	unsigned int regval;
	u32 tmp;
	int timeout = PXA_CNM_POWER_ONOFF_TIMEOUT;

	if(on == 1){
#ifdef CONFIG_DVFM
		set_dvfm_constraint();
#else
#ifdef CNMPXA921CHECK_ENABLED
		if(cpu_is_pxa921()) {
			pm_qos_update_request(pxa_cnm_qos_req_min, 500);
		}else{
			pm_qos_update_request(pxa_cnm_qos_req_min, 624);
		}
#else
		pm_qos_update_request(pxa_cnm_qos_req_min, 624);
#endif
#endif
		/*Enable*/
		/*Power On*/
		printk(KERN_DEBUG "entering pxa_cnm_power(on)\n");

#ifdef CNMPOWER_SETJTAG
		if(cpu_is_pxa910_A0()) {	//for pxa910_A0, need to set those JTAG
			__raw_writel(0x3, APBC_PXA168_SW_JTAG); // release SW JTAG module reset, and enable its clock in APB clock control unit
			__raw_writel(0x0, JTAGSW_CTRL); // set TSRTn=0
			__raw_writel(0x1, JTAGSW_EN); // enable SW JTAG
		}
#endif
#ifdef CNMPOWER_SOFTWARE_ONOFF
		printk(KERN_DEBUG "DX8 SW power on\n");
		/*DX8_CLK check*/
		//printk("Dx8 Read ...APMU_DX8_CLK_RES_CTRL = 0x%.8x\n", __raw_readl(APMU_DX8_CLK_RES_CTRL));		

		// DX8_CLK_RES definition:
		// bit[1:0]: {func_reset, axi_reset}
		// bit[4:3]: {func_clk_en, axi_clk_en}
		// bit[10:6]:  dx8_clk_div
		// bit[18:16]: {pwr_on2, pwr_on1, isob}
		// bit[19]: hw_mode
		/////////////////////////////reference user.c in PMU_application_notes.doc

		// SW turn-on GC/Dx8 power, and enable clock
		//*PMUA_DX8_CLK_RES = DX8_CLK_DIV | DX8_FUNC_CLK | DX8_AXI_CLK;  // clock enable
		//*PMUA_DX8_CLK_RES = DX8_CLK_EN | DX8_ON1;  // on1
		//*PMUA_DX8_CLK_RES =  DX8_CLK_EN | DX8_ON1 | DX8_ON2;  // on2, 200us latency required
		__raw_writel(DX8_CLK_DIV | DX8_FUNC_CLK | DX8_AXI_CLK, APMU_DX8_CLK_RES_CTRL);
		__raw_writel(DX8_CLK_EN | DX8_ON1, APMU_DX8_CLK_RES_CTRL);
		schedule_timeout(20);
		__raw_writel(DX8_CLK_EN | DX8_ON1 | DX8_ON2, APMU_DX8_CLK_RES_CTRL);
		schedule_timeout(20);

		// SW release reset
		//*PMUA_DX8_CLK_RES = DX8_CLK_EN | DX8_PWR_ON | DX8_FUNC_RST;   // resetPin_ first
		//*PMUA_DX8_CLK_RES = DX8_CLK_EN | DX8_PWR_ON | DX8_FUNC_RST | DX8_AXI_RST;   // areset/hreset 48 cycles needed
		__raw_writel(DX8_CLK_EN | DX8_PWR_ON | DX8_FUNC_RST, APMU_DX8_CLK_RES_CTRL);
		__raw_writel(DX8_CLK_EN | DX8_PWR_ON | DX8_FUNC_RST | DX8_AXI_RST, APMU_DX8_CLK_RES_CTRL);
		// SW enable FW
		//*PMUA_DX8_CLK_RES =  DX8_CLK_EN | DX8_PWR_ON | DX8_RST_OFF | DX8_ISO;   // GC is fully functional now
		__raw_writel(DX8_CLK_EN | DX8_PWR_ON | DX8_RST_OFF | DX8_ISO , APMU_DX8_CLK_RES_CTRL);
#else
		printk(KERN_DEBUG "DX8 auto HW power on\n");
		__raw_writel(0x20001fff, APMU_PWR_BLK_TMR_REG);//on1, on2, off timer
		__raw_writel(DX8_HW_MODE|DX8_CLK_EN /*0x80078*/, APMU_DX8_CLK_RES_CTRL); //DX8 hw mode
		tmp = __raw_readl(APMU_PWR_CTRL_REG);
		__raw_writel(tmp | 0x4, APMU_PWR_CTRL_REG); //DX8 auto power-on
		//polling STATUS bit for DX8
		while(!((__raw_readl(APMU_PWR_STATUS_REG)) & 0x4)) {
			udelay(PXA_CNM_POWER_ONOFF_INTERVAL);	//timeout mechanism for power on/off operation
			timeout -= PXA_CNM_POWER_ONOFF_INTERVAL;
			if(timeout < 0) {
				printk("Power on pxa_cnm timeout fail.\n");
				return -1;
			}
		}
#endif
#ifdef CNMPOWER_SETJTAG
		if(cpu_is_pxa910_A0()) {	//for pxa910_A0, need to set those JTAG
			__raw_writel(0x2, JTAGSW_CTRL); // set TSRTn=1
			__raw_writel(0x0, JTAGSW_EN); // disable SW JTAG
		}
#endif

#if 0
		//check result
		//only for software power on
		regval = __raw_readl(APMU_DX8_CLK_RES_CTRL);
		//printk("Dx8 Reset ...APMU_DX8_CLK_RES_CTRL = 0x%.8x\n", regval);
		if(regval != (DX8_CLK_EN | DX8_PWR_ON | DX8_RST_OFF | DX8_ISO)) {
			printk("----error in power on APMU_DX8_CLK_RES, value is 0x%.8x-----\n", regval);
			return -1;
		}else{
			printk("----suc in power on APMU_DX8_CLK_RES, value is 0x%.8x-----\n", regval);
		}
#endif
		is_cnm_power_on = 1;
		is_cnm_clock_on = 1;

		vpuprofile_init();

	}else{
		printk(KERN_DEBUG "entering pxa_cnm_power(off)\n");
#ifdef CNMPOWER_SETJTAG
		if(cpu_is_pxa910_A0()) {	//for pxa910_A0, need to set those JTAG
			__raw_writel(0x3, APBC_PXA168_SW_JTAG); // release SW JTAG module reset, and enable its clock in APB clock control unit
			__raw_writel(0x0, JTAGSW_CTRL); // set TSRTn=0
			__raw_writel(0x1, JTAGSW_EN); // enable SW JTAG
		}
#endif

#ifdef CNMPOWER_SOFTWARE_ONOFF
		printk(KERN_DEBUG "DX8 SW power off\n");
		/////////////////////////////reference user.c in PMU_application_notes.doc
		// now SW turn-off GC power (clock/reset untouched)
		//*PMUA_DX8_CLK_RES = DX8_CLK_EN | DX8_PWR_ON | DX8_RST_OFF;  // shutdown FW
		//*PMUA_DX8_CLK_RES = DX8_CLK_EN | DX8_ON1 | DX8_RST_OFF;   // on2 off
		//*PMUA_DX8_CLK_RES = DX8_CLK_EN | DX8_RST_OFF ;   // on1 off, GC power down now.

		__raw_writel(DX8_CLK_EN | DX8_PWR_ON | DX8_RST_OFF , APMU_DX8_CLK_RES_CTRL);
		__raw_writel(DX8_CLK_EN | DX8_ON1 | DX8_RST_OFF , APMU_DX8_CLK_RES_CTRL);
		__raw_writel(DX8_CLK_EN | DX8_RST_OFF  , APMU_DX8_CLK_RES_CTRL);
#else
		printk(KERN_DEBUG "DX8 auto HW power off\n");
		__raw_writel(0x80000 , APMU_DX8_CLK_RES_CTRL);
		tmp = __raw_readl(APMU_PWR_CTRL_REG);
		__raw_writel(tmp & (~0x4), APMU_PWR_CTRL_REG); //DX8 auto power-off
		//polling STATUS bit for DX8
		while(((__raw_readl(APMU_PWR_STATUS_REG)) & 0x4)) {
			udelay(PXA_CNM_POWER_ONOFF_INTERVAL);
			timeout -= PXA_CNM_POWER_ONOFF_INTERVAL;
			if(timeout < 0) {
				printk("Power off pxa_cnm timeout fail.\n");
				return -1;
			}
		}
#endif
#ifdef CNMPOWER_SETJTAG
		if(cpu_is_pxa910_A0()) {	//for pxa910_A0, need to set those JTAG
			__raw_writel(0x1, JTAGSW_CTRL); // set TSRTn=1. Different from setting after power on, it write 0x1 instead of 0x2.
			__raw_writel(0x0, JTAGSW_EN); // disable SW JTAG
		}
#endif

#if 0
		//check result
		//only for software power off
		regval = __raw_readl(APMU_DX8_CLK_RES_CTRL);
        if( regval != (DX8_CLK_EN | DX8_RST_OFF ) ) {
			printk("----error in power off APMU_DX8_CLK_RES, value is 0x%.8x-----\n", regval);
			return -1;
		}else{
			printk("----suc in power off APMU_DX8_CLK_RES, value is 0x%.8x-----\n", regval);
		}
#endif
		is_cnm_power_on = 0;
		is_cnm_clock_on = 0;

#ifdef CONFIG_DVFM
		unset_dvfm_constraint();
#else
		pm_qos_update_request(pxa_cnm_qos_req_min, PM_QOS_DEFAULT_VALUE);
#endif
	}

	return 0;
}

#define CNMDRVVERSTRING	"cnm driver ver 201111xx"
static void echoCNMDriverVer(void)
{
	const char ver[]=CNMDRVVERSTRING;
	printk("%s\n", ver);
	return;
}

static int pxa_cnm_probe(struct platform_device *pdev)
{
	struct resource *r;
	struct pxa_cnm *cnm = &pxa_cnm;
	int irq, ret;
	int i;
	echoCNMDriverVer();
	printk("begin pxa_cnm_probe().\n");

	platform_set_drvdata(pdev, cnm);

	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		dev_err(&pdev->dev, "no IRQ resource defined\n");
		goto out;
	}

	ret = request_irq(irq, pxa_cnm_irq, 0, "cnm", NULL);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to request IRQ\n");
		goto fail_free_irq;
	}

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (r == NULL)
		goto fail_free_irq;

	r = request_mem_region(r->start, r->end - r->start + 1, pdev->name);
	if (r == NULL) {
		dev_err(&pdev->dev, "failed to request memory resource\n");
		goto fail_free_irq;
	}

	cnm->mmio_base = ioremap(r->start, r->end - r->start + 1);
	if(cnm->mmio_base == NULL)
		goto fail_free_res;

	ret = misc_register(&pxa_cnm_miscdev);
	if (ret < 0) {
		dev_err(&pdev->dev,
				"unable to register device node /dev/cnm\n");
		goto fail_free_res;
	}

	mutex_init(&pxa_cnm.lock);
	//init instance array idx
	for(i=0;i<CNM_FD_CODECINST_ARRAY_SZ;i++) {
		cnm_fd_codecinst_arr[i].vpuslotidx = i;
	}

#if KERN_CNM_MEM_ALLOC_METHOD != KERN_CNM_MEM_ALLOC_METHOD_BMM
#if KERN_CNM_MEM_ALLOC_METHOD == KERN_CNM_MEM_ALLOC_METHOD_DMACOHERENT
	reservedmem_viraddr = dma_alloc_coherent(&pdev->dev, get_cnm_firmware_mem_sz(), (&reservedmem), GFP_KERNEL);
#elif KERN_CNM_MEM_ALLOC_METHOD == KERN_CNM_MEM_ALLOC_METHOD_KMALLOC
	reservedmem_viraddr = kmalloc(get_cnm_firmware_mem_sz(), GFP_KERNEL);
	reservedmem = (void*)virt_to_phys(reservedmem_viraddr);
#elif KERN_CNM_MEM_ALLOC_METHOD == KERN_CNM_MEM_ALLOC_METHOD_DEDICATED
	reservedmem = (void*)get_cnm_firmware_mem_dedicated_addr();	//Always use fixed address. In future, pxa910.c should provide an interface to get this address.
	reservedmem_viraddr = (void*)1;	//If we want to access this memory in kernel drvier, should use ioremap() to get the kernel virtual address.
#endif
	printk("c&m firmware memory allocated, kernel vir addr: %p, phy addr: %p, method %d\n", reservedmem_viraddr, reservedmem, KERN_CNM_MEM_ALLOC_METHOD);
#endif

#ifdef CONFIG_PROC_FS
	create_cnm_proc_file();
#endif
	//init_completion(&cmd_complete);	//will init in active_pxacnm()
	//sema_init(&sema, 1);	//will init in active_pxacnm()
	printk("end of pxa_cnm_probe() suc.\n");
	return 0;

fail_free_res:
	release_mem_region(r->start, r->end - r->start + 1);
fail_free_irq:
	free_irq(irq, &pxa_cnm);
out:
	printk("end of pxa_cnm_probe() fail.\n");
	return -ENODEV;
}

static int pxa_cnm_remove(struct platform_device *pdev)
{
	struct resource *r;
	int irq;
	int ret = 0;
	echoCNMDriverVer();
	printk("begin of pxa_cnm_remove().\n");
	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	irq = platform_get_irq(pdev, 0);

	remove_proc_entry(CNM_PROC_FILE, NULL);
	misc_deregister(&pxa_cnm_miscdev);
	iounmap(pxa_cnm.mmio_base);
	release_mem_region(r->start, resource_size(r));
	free_irq(irq, NULL);

	if(is_cnm_power_on == 1) {
		if(0 != pxa_cnm_power(0)) {
			printk("power off pxa_cnm in pxa_cnm_remove() fail.\n");
			ret = -EFAULT;
		}
	}

#if KERN_CNM_MEM_ALLOC_METHOD != KERN_CNM_MEM_ALLOC_METHOD_BMM
	printk("will free c&m firmware memory, kernel vir addr: %p, phy addr: %p\n, method %d", reservedmem_viraddr, reservedmem, KERN_CNM_MEM_ALLOC_METHOD);
	if(reservedmem_viraddr && reservedmem) {
#if KERN_CNM_MEM_ALLOC_METHOD == KERN_CNM_MEM_ALLOC_METHOD_DMACOHERENT
		dma_free_coherent(&pdev->dev, get_cnm_firmware_mem_sz(), reservedmem_viraddr, reservedmem);
#elif KERN_CNM_MEM_ALLOC_METHOD == KERN_CNM_MEM_ALLOC_METHOD_KMALLOC
		kfree(reservedmem_viraddr);
#elif KERN_CNM_MEM_ALLOC_METHOD == KERN_CNM_MEM_ALLOC_METHOD_DEDICATED
		//do nothing
#endif
	}
	reservedmem_viraddr = 0;
	reservedmem = 0;
	printk("c&m firmware memory is freed\n");
#endif

	printk("end of pxa_cnm_remove(), ret %d\n", ret);
	return ret;
}

#define pxa_cnm_suspend      NULL
#define pxa_cnm_resume       NULL

static struct platform_driver cnm_driver = {
	.driver         = {
		.name   = "pxa-cnm",
	},
	.probe          = pxa_cnm_probe,
	.remove         = pxa_cnm_remove,
	.suspend        = pxa_cnm_suspend,
	.resume         = pxa_cnm_resume,
};

#if KERN_CNM_MEM_ALLOC_METHOD == KERN_CNM_MEM_ALLOC_METHOD_BMM
extern unsigned long bmm_malloc_kernel(size_t size, unsigned long attr);
extern void bmm_free_kernel(unsigned long paddr);
#define BMM_ATTR_NONCACHED       (1 << 1)
#endif
static int __init pxa_cnm_init(void)
{
	int ret;
	printk("begin of pxa_cnm_init().\n");

#ifdef CONFIG_DVFM
	dvfm_register("CNM", &dvfm_dev_idx);
#else
#ifdef CNMKERN_QOS_INTF_UPDATE
	pm_qos_add_request(&req, PM_QOS_CPUFREQ_MIN, PM_QOS_DEFAULT_VALUE);
	pxa_cnm_qos_req_min = &req;
#else
	pxa_cnm_qos_req_min = pm_qos_add_request(PM_QOS_CPUFREQ_MIN,
			PM_QOS_DEFAULT_VALUE);
#endif /* end of CNMKERN_QOS_INTF_UPDATE */
#endif

#if KERN_CNM_MEM_ALLOC_METHOD == KERN_CNM_MEM_ALLOC_METHOD_BMM
	reservedmem = (void*)bmm_malloc_kernel(get_cnm_firmware_mem_sz(), BMM_ATTR_NONCACHED);
	printk("c&m firmware memory allocated, phy addr: %p, method %d\n", reservedmem, KERN_CNM_MEM_ALLOC_METHOD);
#endif

	ret = platform_driver_register(&cnm_driver);
	printk("end of pxa_cnm_init(), ret %d\n", ret);
	return ret;
}

static void __exit pxa_cnm_exit(void)
{

	printk("begin of pxa_cnm_exit().\n");

#ifdef CONFIG_DVFM
	dvfm_unregister("CNM", &dvfm_dev_idx);
#else
	if(pxa_cnm_qos_req_min)
	{
		pm_qos_remove_request(pxa_cnm_qos_req_min);
		pxa_cnm_qos_req_min = NULL;
	};
#endif

#if KERN_CNM_MEM_ALLOC_METHOD == KERN_CNM_MEM_ALLOC_METHOD_BMM
	printk("will free c&m firmware memory, phy addr: %p\n, method %d", reservedmem, KERN_CNM_MEM_ALLOC_METHOD);
	bmm_free_kernel((unsigned long)reservedmem);
	reservedmem = 0;
#endif

	platform_driver_unregister(&cnm_driver);
	printk("end of pxa_cnm_exit().\n");
}

module_init(pxa_cnm_init);
module_exit(pxa_cnm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lei Wen (leiwen@marvell.com)");
MODULE_DESCRIPTION("Chip and Media CodaDx8 codec Driver");
