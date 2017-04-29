/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.

 *(C) Copyright 2008 Marvell International Ltd.
 * All Rights Reserved
 */

#include <linux/errno.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/wait.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/serial.h>
#include <asm/uaccess.h>

#include <linux/tty_ldisc.h>
#include <linux/module.h>

#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/errno.h>

#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/mman.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/pgtable.h>

//#define HWMAP_DEBUG

#define HWMAP_MAJOR 237

#define DRIVER_VERSION "v1.0"
#define DRIVER_AUTHOR "Marvell"
#define DRIVER_DESC "Driver for HW access via physical address"

/* Module information */
MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE("GPL");

static void hwmap_vma_open(struct vm_area_struct *vma)
{
#if defined(HWMAP_DEBUG)
  printk("HWMAP OPEN 0x%lx -> 0x%lx (0x%lx)\n",vma->vm_start, vma->vm_pgoff<<PAGE_SHIFT, vma->vm_page_prot);
#endif
}
static void hwmap_vma_close(struct vm_area_struct *vma)
{
#if defined(HWMAP_DEBUG)
  printk("HWMAP CLOSE 0x%lx -> 0x%lx\n",vma->vm_start, vma->vm_pgoff<<PAGE_SHIFT);
#endif
}

// These are mostly for debug: do nothing useful otherwise
static struct vm_operations_struct vm_ops =
  {
    .open=hwmap_vma_open,
    .close=hwmap_vma_close
  };

/* device memory map method */
/*
 vma->vm_end, vma->vm_start : specify the user space process address range assigned when mmap has been called;
 vma->vm_pgoff - is the physical address supplied by user to mmap in the last argument (off)
             However, mmap restricts the offset, so we pass this shifted 12 bits right.
 */
int hwmap_mmap(struct file *file, struct vm_area_struct *vma)
{
    unsigned long size = vma->vm_end - vma->vm_start;
    unsigned long pa=vma->vm_pgoff;
            
    /* we do not want to have this area swapped out, lock it */
    vma->vm_flags |= (VM_RESERVED|VM_IO);
    vma->vm_page_prot=pgprot_noncached(vma->vm_page_prot); // see linux/drivers/char/mem.c

   // TBD: without this vm_page_prot=0x53 and write seem to not reach the destination:
   // - no fault on write
   // - read immediately (same user process) return the new value
   // - read after a second by another user process instance return original value
   // Why PROT_WRITE specified by mmap caller does not take effect?
   // MAP_PRIVATE used by app results in copy-on-write behaviour, which is irrelevant for this application
   //vma->vm_page_prot|=L_PTE_WRITE; 
    
    if(io_remap_pfn_range(vma,
           vma->vm_start,
           pa,/* physical page index */
           size,
	   vma->vm_page_prot))
    {
            printk("remap page range failed\n");
            return -ENXIO;
    }
    vma->vm_ops=&vm_ops;
    hwmap_vma_open(vma);
    return(0);
}

static const struct file_operations hwmap_fops = {
	.owner =	THIS_MODULE,
	.mmap =		hwmap_mmap
};


static int __init
hwmap_init(void)
{
    int err = 0;
    err = register_chrdev (HWMAP_MAJOR, "hwmap", &hwmap_fops);
    if (err)
    {
        printk(KERN_ERR "%s can't register device\n",__FUNCTION__);
        return -ENOMEM;
    }
    return 0;
}

static void __exit
hwmap_exit(void)
{
    unregister_chrdev (HWMAP_MAJOR, "hwmap"); 
    printk(KERN_DEBUG "%s unregistering chrdev done\n",__FUNCTION__);
}

#ifdef MODULE
module_init(hwmap_init);
module_exit(hwmap_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("HW memory mapped access");
#else
subsys_initcall(hwmap_init);
#endif
