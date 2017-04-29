/*
 * PXA910 CP related
 *
 * This software program is licensed subject to the GNU General Public License
 * (GPL).Version 2,June 1991, available at http://www.fsf.org/copyleft/gpl.html

 * (C) Copyright 2007 Marvell International Ltd.
 * All Rights Reserved
 */
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/sysdev.h>
#include <linux/spinlock.h>
#include <linux/notifier.h>
#include <linux/string.h>
#include <linux/kobject.h>
#include <linux/list.h>
#include <linux/notifier.h>
#include <linux/relay.h>
#include <linux/debugfs.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <linux/mtd/super.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/file.h>
#include <mach/cputype.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 28)
#include <mach/hardware.h>
#else
#include <asm/arch/hardware.h>
#endif
#include <asm/atomic.h>
#include <asm/io.h>

#include "watchdog.h"
#include "pxa910_cp.h"
extern struct sysdev_class cpu_sysdev_class;
//extern void micco_dump_regs(void);
//#define CP_LOW_VECTOR
//#define CP_LOOP_TEST
//#define AP_LOW_VECTOR
//#define AP_LOOP_TEST
#define CP_RESET_MSA
//#define CP_HOLD_AP
#define CP_AP_BUSY
//#define CP_RESET_AP
//#define PRELOAD_UBOOT
//#define RELEASE_MSA
//#define PRELOAD_CP

#ifdef CP_LOW_VECTOR
#define ARBEL_LOW_VECTOR_ADDR (0)
static void *arbel_low_vector_addr;
#endif

#if defined(CP_LOOP_TEST) || defined(AP_LOOP_TEST)
#define ARBEL_LOOP_ADDR (0x07a00000 - 0x1000)
static void *arbel_loop_addr;
#endif

#ifdef PRELOAD_UBOOT
#define UBOOT_LOAD_ADDR (0x7900000)
//#define UBOOT_LOAD_ADDR (0xD1000000)
#define UBOOT_FLASH_ADDR 0x40000
static void *uboot_load_addr;
#endif

static void *arbel_addr;
static void *m05_bin_addr;

struct cp_buffer {
	char    *addr;
	int len;
};

#define ARBEL_BIN_ADDR LOAD_SEAGULL_ADDR
#define M05_BIN_ADDR LOAD_MSA_ADDR
#define RELIABLE_BIN_ADDR LOAD_RDATA_ADDR

#define ARBEL_BIN_SIZE LOAD_SEAGULL_SIZE
#define M05_BIN_SIZE 0x100000 //1M
#define RELIABLE_DATA_SIZE 0x20000 //128k

//#define M05_DDR_SIZE   0x00088000
//#define M05_FLASH_SIZE 0x00078000
#define M05_DDR_SIZE   0x00078000       //0x00088000
#define M05_FLASH_SIZE 0x00088000       //0x00078000


#define RELIABLE_DATA_MTD 3
#define ARBEL_BIN_MTD  4
#define M05_BIN_MTD 5


#if 0
#define SW_BRANCH_ADDR  (&__REG_PXA910(0xD4282C24))
#define PMUM_APRR       (&__REG_PXA910(0xD4051020))
#define PMUM_CPRR       (&__REG_PXA910(0xD4050020))
#define SEAGULL_CPU_CONF (&__REG_PXA910(0xD4282C04))
#define MONHAWK_CPU_CONF (&__REG_PXA910(0xD4282C08))
#define FAB_CP_ADDR_REMAP (&__REG_PXA910(0xD4200200))
#endif

#define SW_BRANCH_ADDR          0xfe282C24
#define PMUM_APRR               0xfe051020
#define PMUM_CPRR               0xfe050020
#define SEAGULL_CPU_CONF        0xfe282C04
#define MONHAWK_CPU_CONF        0xfe282C08
#define FAB_CP_ADDR_REMAP       0xfe200200
#define STIKY_CLR0				0xfe0318f8
#define STIKY_CLR1				0xfe0318fc

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 28)
static ssize_t cp_show(struct sys_device *sys_dev, struct sysdev_attribute *sys_attr, char *buf)
#else
static ssize_t cp_show(struct sys_device *sys_dev, char *buf)
#endif
{
	int len;
	int cp_enable;

	cp_enable = readl(PMUM_APRR) & 0x1;
	cp_enable = !cp_enable;
	len = sprintf(buf, "%d\n", cp_enable);

	return len;
}

void low_vector_section(void)
{
	__asm__ __volatile__ (
		".align 5\n"
		".global vector_start;\n"
		"vector_start:\n"
#ifdef CP_RESET_AP
		"b	cpu_type\n"
		"sub pc, pc, #8;\n"
		"sub pc, pc, #8;\n"
		"sub pc, pc, #8;\n"
		"sub pc, pc, #8;\n"
		"sub pc, pc, #8;\n"
		"sub pc, pc, #8;\n"
		"sub pc, pc, #8;\n"
		"sub pc, pc, #8;\n"
		"\n"
		"cpu_type:\n"
		"/* if it is arm926?*/\n"
		"mrc p15, 0, r0, c0, c0, 0;\n"
		"mov r1, #0xff0;\n"
		"orr r1, r1, #0xf000;\n"
		"and r0, r0, r1;\n"
		"mov r1, #0x00000260;\n"
		"orr r1, r1, #0x00009000;\n"
		"cmp  r0, r1;\n"
		"beq  arm926;\n"
		"\n"
		"/* arm946: reset AP and then branch */\n"
		"arm946:\n"
#ifdef CP_LOOP_TEST
		"sub pc, pc, #8\n"
#endif
		"ldr r0, reg_mpmu_cprr;\n"
		"ldr r1, [r0, #0];\n"
		"orr r1, r1, #0x2;\n"
		"str r1, [r0, #0];\n"
		"ldr r1, [r0, #0];\n"
		"ldr r2, count\n"
		"loop:\n"
		"subs r2, r2, #1\n"
		"bne loop\n"
		"bic r1, r1, #0x2;\n"
		"str r1, [r0, #0];\n"
		"ldr r1, [r0, #0];\n"
		"\n"
		"/* branch to address in SW_BRANCH_REG */\n"
		"ldr r0, reg_sw_branch_reg;\n"
		"ldr pc, [r0, #0];\n"
		"arm926:\n"
#ifdef AP_LOOP_TEST
		"sub pc, pc, #8\n"
#endif
		"ldr r0, ap_entry;\n"
		"mov pc, r0\n"
		"reg_mpmu_cprr:\n"
		".long 0xD4050020\n"
		"reg_sw_branch_reg:\n"
		".long 0xD4282C24\n"
		"ap_entry:\n"
#ifdef PRELOAD_UBOOT
		".long 0x7900000\n"
#else
		".long 0xffff0000\n"
#endif
		"count:\n"
		".long 3120000\n"
#else
		"sub pc, pc, #0x4;\n"
		"ldr r0, reg_sw_branch_reg;\n"
		"ldr pc, [r0, #0];\n"
		"reg_sw_branch_reg:\n"
		".long 0xD4282C24\n"
#endif
		".global vector_end;\n"
		"vector_end:\n"
		::
	);
}

/* Set current operating point */
#define VECTOR_SAVE_SIZE 128
extern int vector_start, vector_end;

void cp_loadfromNAND(void)
{
	struct mtd_info *mtd;
	size_t retlen;
	int i; char buff[32];
	/* Load CP Arbel and MSA image from the flash */
	mtd = get_mtd_device(NULL, ARBEL_BIN_MTD);
	printk("Load Arbel image from Flash!\n");
	mtd->read(mtd, 0x0, ARBEL_BIN_SIZE, &retlen, arbel_addr);
	memcpy(buff, (char*)arbel_addr, 32 * sizeof(char));
	for (i = 0; i < 32; i++)
	{
		printk("%02x", buff[i]);
	}
	printk("\n\n\n");
	mtd = get_mtd_device(NULL, M05_BIN_MTD);
	printk("Load MSA image from Flash!\n");
	mtd->read(mtd, 0x0, M05_BIN_SIZE, &retlen, m05_bin_addr);
}

void cp_loadreliabledatafromNAND(void)
{
	struct mtd_info *mtd;
	size_t retlen;
	void *data_addr;
	int i;
	char *buff;

	data_addr = ioremap_nocache(RELIABLE_BIN_ADDR, RELIABLE_DATA_SIZE);

	/* Load IMEI image from the flash */
	mtd = get_mtd_device(NULL, RELIABLE_DATA_MTD);
	printk("Load reliable data image from Flash!\n");
	mtd->read(mtd, 0x0, RELIABLE_DATA_SIZE, &retlen, data_addr);
	buff = (char*)data_addr;
	for (i = 0; i < 32; i++)
	{
		printk("%02x", *buff++);
	}
	printk("\n");
	iounmap(data_addr);

}
void cp_loadfromEMMC(void)
{
	struct file *f;
	mm_segment_t oldfs;
	char device_name[128];

	oldfs = get_fs();
	set_fs (KERNEL_DS);

	sprintf(device_name, "/dev/block/mmcblk0p%d",ARBEL_BIN_MTD);
	f = filp_open(device_name, O_RDONLY, 0);
	if(!IS_ERR(f))
	{
		vfs_read(f, arbel_addr, ARBEL_BIN_SIZE, &f->f_pos);
		filp_close(f,NULL);
	}

	sprintf(device_name, "/dev/block/mmcblk0p%d",M05_BIN_MTD);
	f = filp_open(device_name, O_RDONLY, 0);
	if(!IS_ERR(f))
	{
		vfs_read(f, m05_bin_addr, M05_BIN_SIZE, &f->f_pos);
		filp_close(f,NULL);
	}

	set_fs(oldfs);
}
void cp_loadreliabledatafromEMMC(void)
{
	void *data_addr;
	int i;
	char *buff;
	struct file *f;
	mm_segment_t oldfs;
	char device_name[128];

	oldfs = get_fs();
	set_fs (KERNEL_DS);

	data_addr = ioremap_nocache(RELIABLE_BIN_ADDR, RELIABLE_DATA_SIZE);

	/* Load IMEI image from the flash */
	sprintf(device_name, "/dev/block/mmcblk0p%d",RELIABLE_DATA_MTD);
	f = filp_open(device_name, O_RDONLY, 0);
	if(!IS_ERR(f))
	{
		vfs_read(f, data_addr, RELIABLE_DATA_SIZE, &f->f_pos);
		filp_close(f,NULL);
	}
	set_fs(oldfs);

	buff = (char*)data_addr;
	for (i = 0; i < 32; i++)
	{
		printk("%02x", *buff++);
	}
	printk("\n");
	iounmap(data_addr);
}

void cp_loadreliabledatafromflash(void)
{
	if(cpu_is_pxa921())
		cp_loadreliabledatafromEMMC();
	else
		cp_loadreliabledatafromNAND();
}

void cp_loadfromflash(void)
{
	if(cpu_is_pxa921())
		cp_loadfromEMMC();
	else
		cp_loadfromNAND();
}


void checkloadinfo(void)
{
	void *data_addr;
	char *buff;
	int i;

	/* Check CP Arbel image in DDR */
	printk("Check loading Arbel image: ");
	buff = (char*) arbel_addr;
	for (i = 0; i < 32; i++)
	{
		printk("%02x", *buff++);
	}
	printk("\n\n\n");

	/* Check ReliableData image in DDR */
	printk("Check loading ReliableData image: ");
	data_addr = ioremap_nocache(RELIABLE_BIN_ADDR, RELIABLE_DATA_SIZE);
	buff = (char*)data_addr;
	for (i = 0; i < 32; i++)
	{
		printk("%02x", *buff++);
	}
	printk("\n");
	iounmap(data_addr);
}

void cp_releasecp(void)
{
	int value;

#ifdef CP_LOW_VECTOR
	int i;
	unsigned long flags;
	unsigned int vector_size;
	volatile void *dst_vector;
	volatile void *src_vector, *src_vector_end;
	unsigned int save[VECTOR_SAVE_SIZE];
#endif
#ifdef CP_AP_BUSY
	unsigned int timeout = 100;
#endif
#if defined(PRELOAD_UBOOT)
	struct mtd_info *mtd;
	size_t retlen;
#endif

#ifdef CP_LOW_VECTOR
	/* let CP boot from lower vector so to skip bootrom */
	value = readl(SEAGULL_CPU_CONF);
	value &= ~0x1;
	writel(value, SEAGULL_CPU_CONF);
	printk("%s: SEAGULL_CPU_CONF = %08x\n", __func__,
	       __raw_readl(SEAGULL_CPU_CONF));
	writel(0xffffffff, STIKY_CLR0);
	writel(0xffffffff, STIKY_CLR1);

	/* put loop instructions at CP address 0x0 */
	local_irq_save(flags);
	src_vector = &vector_start;
	src_vector_end = &vector_end;
	dst_vector = arbel_low_vector_addr;
	vector_size = src_vector_end - src_vector;
	memcpy(save, (const void *)dst_vector, vector_size);
	printk("vector_start:0x%08x, vector_end:0x%08x\n",
	       (unsigned int)src_vector, (unsigned int)src_vector_end);
#ifdef CP_RESET_AP
	/* let AP boot from lower vector so to skip bootrom */
	value = readl(MONHAWK_CPU_CONF);
#ifdef AP_LOW_VECTOR
	value &= ~0x1;
#else
	value |= 0x1;
#endif
	writel(value, MONHAWK_CPU_CONF);
#endif
	memcpy((void *)dst_vector, (const void *)src_vector, vector_size);
	for (i = 0; i < vector_size; i += 4)
	{
		printk("0x%08x\n", *(unsigned int*)(dst_vector + i));
	}
#endif
	/* mmap DDR 0x70000000 to MSA 0xD0000000 */
	/* writel(0x70, FAB_CP_ADDR_REMAP); */
	/* writel(0x00, FAB_CP_ADDR_REMAP);*/

#ifdef PRELOAD_UBOOT
	/* preload uboot to DDR */
	mtd  = get_mtd_device(NULL, 0);
	printk("mtd->name:%s\n", mtd->name);
	mtd->read(mtd, UBOOT_FLASH_ADDR, 0x40000, &retlen, uboot_load_addr);
#endif
	/* where to branch */
#if defined(CP_LOOP_TEST)
	*(unsigned int*)arbel_loop_addr = 0xe24ff008; /* sub pc, pc, #8 */
#endif

#ifdef CP_LOOP_TEST
	writel(ARBEL_LOOP_ADDR, SW_BRANCH_ADDR);
#else
	writel(ARBEL_BIN_ADDR, SW_BRANCH_ADDR);
#endif
#ifdef RELEASE_MSA
	/* release CP */
	printk("release msa...");
	value = readl(PMUM_CPRR);
	value &= ~0x4;
	writel(value, PMUM_CPRR);
	printk("done!\n");
#endif

	printk("release cp...\n");
	value = readl(PMUM_APRR);
	printk("the PMUM_APRR value is 0x%08x\n", PMUM_APRR);
	printk("the value is 0x%08x\n", value);
	value &= ~0x41; //Both bit6 and bit0 is set as 0
	writel(value, PMUM_APRR);
	//while (1);
	printk("done!\n");
#ifdef CP_HOLD_AP
	printk("hold ap...");
	value = readl(PMUM_CPRR);
	value |= (1 << 1);
	writel(value, PMUM_CPRR);
	printk("done!\n");
#endif
#ifdef CP_AP_BUSY
	printk("keep ap busy wait...");
	while (timeout--)
		mdelay(1);
	printk("done\n");
#endif
#ifdef CP_LOW_VECTOR
	mdelay(100);
	memcpy((void *)dst_vector, save, vector_size);
	local_irq_restore(flags);
#endif

}
void cp_holdcp(void)
{
	int value;
	watchDogCountStop();
	printk("hold cp...");
	value = readl(PMUM_APRR);
	value |= 0x1;
	writel(value, PMUM_APRR);
	printk("done!\n");

#ifdef CP_RESET_MSA
	printk("hold msa...");
	value = readl(PMUM_CPRR);
	value |= 0x4;    //////0x4
	writel(value, PMUM_CPRR);
	printk("done!\n");
#endif

}
struct cp_load_table_head * get_cp_load_table(void)
{
	struct cp_load_table_head * ptable;
	if(!arbel_addr)
	{
		printk("Can not find CP table\n");
		return NULL;
	}

	ptable = (struct cp_load_table_head *)(arbel_addr + 4);
	return ptable;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 28)
static ssize_t cp_store(struct sys_device *sys_dev, struct sysdev_attribute *attr,
			const char *buf, size_t len)
#else
static ssize_t cp_store(struct sys_device *sys_dev, const char *buf,
			size_t len)
#endif
{
	int cp_enable;

	cp_enable = simple_strtol(buf, NULL, 0);
	printk("buf={%s}, cp_enable=%d\n", buf, cp_enable);

	if (cp_enable)
	{
#ifdef PRELOAD_CP
		cp_loadfromflash();
		cp_loadreliabledatafromflash();
#else
		checkloadinfo();
#endif
		cp_releasecp();

	}
	else
	{
		cp_holdcp();
	}

	return len;
}
SYSDEV_ATTR(cp, 0644, cp_show, cp_store);

static struct attribute *cp_attr[] = {
	&attr_cp.attr,
};

static int cp_add(struct sys_device *sys_dev)
{
	int i, n;
	int ret;

	n = ARRAY_SIZE(cp_attr);
	for (i = 0; i < n; i++)
	{
		ret = sysfs_create_file(&(sys_dev->kobj), cp_attr[i]);
		if (ret)
			return -EIO;
	}
	return 0;
}

static int cp_rm(struct sys_device *sys_dev)
{
	int i, n;

	n = ARRAY_SIZE(cp_attr);
	for (i = 0; i < n; i++)
	{
		sysfs_remove_file(&(sys_dev->kobj), cp_attr[i]);
	}
	return 0;
}

static struct sysdev_driver cp_sysdev_driver = {
	.add		= cp_add,
	.remove		= cp_rm,
};

static int
cp_release(struct inode *inode, struct file *file)
{
	struct cp_buffer *cp_buffer = file->private_data;

	kfree(cp_buffer);

	return 0;
}

static ssize_t
cp_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	struct cp_buffer *cp_buffer = file->private_data;
	char *src = cp_buffer->addr;
	int len = cp_buffer->len;
	int err = 0;
	int pos = *ppos;

	if (pos > len)
		return -EFBIG;

	if (count > len)
	{
		err = -EFBIG;
		count = len;
	}

	if (count + pos > len)
	{
		if (!err)
			err = -ENOSPC;
		count = len - pos;
	}

	if (copy_to_user(buf, src + pos, count))
	{
		printk(KERN_WARNING "%s: failed to copy data from user space\n",
		       __func__);
		return -EIO;
	}
	*ppos += count;

	return count;
}

static ssize_t
cp_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	struct cp_buffer *cp_buffer = file->private_data;
	char *dst = cp_buffer->addr;
	int len = cp_buffer->len;
	int pos = *ppos;
	int err = 0;
	char *temp_buff;

	printk("%s: write: 0x%08x-0x%08x\n", __func__, (unsigned int)dst + pos, (unsigned int)dst + pos + count - 1);
	if (pos > len)
		return -EFBIG;

	if (count > len)
	{
		err = -EFBIG;
		count = len;
	}

	if (count + pos > len)
	{
		if (!err)
			err = -ENOSPC;
		count = len - pos;
	}
	temp_buff = dst + pos;

	printk("pos is 0x%08x, the len is 0x%08x, the count is %d\n\n", pos, len, count);
	if (copy_from_user(temp_buff, buf, count))
	{
		printk(KERN_WARNING "%s: failed to copy data from user space\n",
		       __func__);
		return -EIO;
	}

	*ppos += count;

	return count;
}

static struct file_operations cp_fops = {
	.owner		= THIS_MODULE,
//	.open           = cp_open,
	.read		= cp_read,
	.release	= cp_release,
	.write		= cp_write,
};

static struct miscdevice arbel_bin_miscdev = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= "arbel_bin",
	.fops		= &cp_fops,
};

static struct miscdevice m05_ddr_miscdev = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= "m05_ddr",
	.fops		= &cp_fops,
};

static struct miscdevice m05_flash_miscdev = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= "m05_flash",
	.fops		= &cp_fops,
};

static int
cp_open(struct inode *inode, struct file *file)
{
	struct inode *d_inode = file->f_path.dentry->d_inode;
	int minor = iminor(d_inode);
	struct cp_buffer *cp_buffer;

	cp_buffer = kmalloc(sizeof(struct cp_buffer), GFP_KERNEL);
	printk("********************cp_buffer is 0x%08x \n\n", (unsigned int)cp_buffer);
	if (!cp_buffer)
		return -EIO;

	cp_buffer->addr = (minor == arbel_bin_miscdev.minor) ? arbel_addr :
			  (minor == m05_ddr_miscdev.minor) ? m05_bin_addr + M05_FLASH_SIZE :
			  (minor == m05_flash_miscdev.minor) ? m05_bin_addr : 0;
	cp_buffer->len = (minor == arbel_bin_miscdev.minor) ? ARBEL_BIN_SIZE :
			 (minor == m05_ddr_miscdev.minor) ? M05_DDR_SIZE :
			 (minor == m05_flash_miscdev.minor) ? M05_FLASH_SIZE : 0;

	printk("********************cp_buffer len is 0x%08x \n\n", cp_buffer->len);
	printk("********************cp_buffer addr is 0x%08x \n\n", (unsigned int)cp_buffer->addr);
	file->private_data = cp_buffer;

	return 0;
}


static int __init cp_init(void)
{
	int ret;

#ifdef CP_LOW_VECTOR
	arbel_low_vector_addr = ioremap_nocache(ARBEL_LOW_VECTOR_ADDR, 0x1000);
#endif
#if defined(CP_LOOP_TEST)
	arbel_loop_addr = ioremap_nocache(ARBEL_LOOP_ADDR, 0x1000);
#endif
#ifdef PRELOAD_UBOOT
	uboot_load_addr  = ioremap_nocache(UBOOT_LOAD_ADDR, 0x40000);
#endif

	arbel_addr = ioremap_nocache(ARBEL_BIN_ADDR, ARBEL_BIN_SIZE);
	m05_bin_addr = ioremap_nocache(M05_BIN_ADDR, M05_BIN_SIZE);
	cp_fops.open = cp_open;
	ret = misc_register(&arbel_bin_miscdev);
	if (ret)
	{
		printk(KERN_WARNING "%s: failed to register arbel_bin\n", __func__);
		goto err1;
	}
	ret = misc_register(&m05_ddr_miscdev);
	if (ret)
	{
		printk(KERN_WARNING "%s: failed to register m05_ddr\n", __func__);
		goto err2;
	}
	ret = misc_register(&m05_flash_miscdev);
	if (ret)
	{
		printk(KERN_WARNING "%s: failed to register m05_flash\n", __func__);
		goto err3;
	}
	ret = sysdev_driver_register(&cpu_sysdev_class, &cp_sysdev_driver);
	if (ret)
	{
		printk(KERN_WARNING "%s: failed to register cp_sysdev\n", __func__);
		goto err4;
	}
	return 0;

 err4:
	misc_deregister(&m05_flash_miscdev);
 err3:
	misc_deregister(&m05_ddr_miscdev);
 err2:
	misc_deregister(&arbel_bin_miscdev);
 err1:
	iounmap(arbel_addr);
	iounmap(m05_bin_addr);
#ifdef CP_LOW_VECTOR
	iounmap(arbel_low_vector_addr);
#endif
#ifdef CP_LOOP_TEST
	iounmap(arbel_loop_addr);
#endif

	return -EIO;
}

static void cp_exit(void)
{
	iounmap(arbel_addr);
	iounmap(m05_bin_addr);
#ifdef CP_LOW_VECTOR
	iounmap(arbel_low_vector_addr);
#endif
#ifdef CP_LOOP_TEST
	iounmap(arbel_loop_addr);
#endif
	misc_deregister(&arbel_bin_miscdev);
	misc_deregister(&m05_ddr_miscdev);
	misc_deregister(&m05_flash_miscdev);

	return sysdev_driver_unregister(&cpu_sysdev_class, &cp_sysdev_driver);
}

module_init(cp_init);
module_exit(cp_exit);

EXPORT_SYMBOL(cp_loadfromflash);
EXPORT_SYMBOL(cp_loadreliabledatafromflash);
EXPORT_SYMBOL(cp_releasecp);
EXPORT_SYMBOL(cp_holdcp);
EXPORT_SYMBOL(get_cp_load_table);

MODULE_DESCRIPTION("PXA910 CP Related Operation");
MODULE_LICENSE("GPL");
