/* drivers/android/pmem.c
 *
 * Copyright (C) 2007 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/mm.h>
#include <linux/list.h>
#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/android_pmem.h>
#include <linux/mempolicy.h>
#include <linux/oom.h>
#include <linux/sched.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/cacheflush.h>

#define PMEM_MAX_DEVICES 10
#define PMEM_MAX_ORDER 64
#define PMEM_MIN_ALLOC PAGE_SIZE
#define PMEM_MIN_SHIFT PAGE_SHIFT

#define PMEM_DEBUG 1

/* indicates that a refernce to this file has been taken via get_pmem_file,
 * the file should not be released until put_pmem_file is called */
#define PMEM_FLAGS_BUSY 0x1
/* indicates that this is a suballocation of a larger master range */
#define PMEM_FLAGS_CONNECTED 0x1 << 1
/* indicates this is a master and not a sub allocation and that it is mmaped */
#define PMEM_FLAGS_MASTERMAP 0x1 << 2
/* submap and unsubmap flags indicate:
 * 00: subregion has never been mmaped
 * 10: subregion has been mmaped, reference to the mm was taken
 * 11: subretion has ben released, refernece to the mm still held
 * 01: subretion has been released, reference to the mm has been released
 */
#define PMEM_FLAGS_SUBMAP 0x1 << 3
#define PMEM_FLAGS_UNSUBMAP 0x1 << 4


struct pmem_data {
	/* in alloc mode: an index into the bitmap
	 * in no_alloc mode: the size of the allocation */
	int index;
	/* see flags above for descriptions */
	unsigned int flags;
	/* protects this data field, if the mm_mmap sem will be held at the
	 * same time as this sem, the mm sem must be taken first (as this is
	 * the order for vma_open and vma_close ops */
	struct rw_semaphore sem;
	/* info about the mmaping process */
	struct vm_area_struct *vma;
	/* task struct of the mapping process */
	struct task_struct *task;
	/* process id of the mapping process */
	pid_t pid;
	/* file descriptor of the master */
	int master_fd;
	/* file struct of the master */
	struct file *master_file;
	/* a list of currently available regions if this is a suballocation */
	struct list_head region_list;
	/* a linked list of data so we can access them for debugging */
	struct list_head list;
#ifdef PMEM_DEBUG
	int ref;
#endif
};

struct pmem_bits {
	unsigned allocated:1;		/* 1 if allocated, 0 if free */
	unsigned compound:1;		/* 1 if the next index belongs to
					   a compound allocation, 0 if no */ 
	unsigned order:6;		/* size of the region in pmem space */
};

struct pmem_bitmap {
	/* number of entries in the pmem space */
	unsigned long num_entries;
	/* the bitmap for the region indicating which entries are allocated
	 * and which are free */
	struct pmem_bits *bits;
	/* pmem_sem protects the bitmap array
	 * a write lock should be held when modifying entries in bitmap
	 * a read lock should be held when reading data from bits or
	 * dereferencing a pointer into bitmap
	 *
	 * pmem_data->sem protects the pmem data of a particular file
	 * Many of the function that require the pmem_data->sem have a non-
	 * locking version for when the caller is already holding that sem.
	 *
	 * IF YOU TAKE BOTH LOCKS TAKE THEM IN THIS ORDER:
	 * down(pmem_data->sem) => down(bitmap->sem)
	 */
	struct rw_semaphore sem;
};

struct pmem_region_node {
	struct pmem_region region;
	struct list_head list;
};

struct pmem_info {
	struct miscdevice mdev;
	/* physical start address of the pmem space */
	unsigned long base;
	/* total size of the pmem space */
	unsigned long size;
	/* pfn of the garbage page in memory */
	unsigned long garbage_pfn;
	/* index of the garbage page in the pmem space */
	int garbage_index;
	/* the bitmap for the region indicating which entries are allocated
	 * and which are free */
	struct pmem_bitmap *bitmap;
	/* indicates the region should not be managed with an allocator */
	unsigned no_allocator;
	/* indicates maps of this region should be cached, if a mix of
	 * cached and uncached is desired, set this and open the device with
	 * O_SYNC to get an uncached region */
	unsigned cached;
	unsigned buffered;
	/* in no_allocator mode the first mapper gets the whole space and sets
	 * this flag */
	unsigned allocated;
	/* for debugging, creates a list of pmem file structs, the
	 * data_list_sem should be taken before pmem_data->sem if both are
	 * needed */
	struct semaphore data_list_sem;
	struct list_head data_list;
	struct device *dev;

	long (*ioctl)(struct file *, unsigned int, unsigned long);
	int (*release)(struct inode *, struct file *);
};

static DEFINE_MUTEX(pmem_oom_lock);
static struct pmem_info pmem[PMEM_MAX_DEVICES];
static int id_count;
static int debug_bitmap = 0, ctl_killer = 1;

#define PMEM_IS_FREE(id, index) !(pmem[id].bitmap->bits[index].allocated)
#define PMEM_ORDER(id, index) pmem[id].bitmap->bits[index].order
#define PMEM_BUDDY_INDEX(id, index) (index ^ (1 << PMEM_ORDER(id, index)))
#define PMEM_NEXT_INDEX(id, index) (index + (1 << PMEM_ORDER(id, index)))
#define PMEM_OFFSET(index) (index * PMEM_MIN_ALLOC)
#define PMEM_START_ADDR(id, index) (PMEM_OFFSET(index) + pmem[id].base)
#define PMEM_LEN(id, index) ((1 << PMEM_ORDER(id, index)) * PMEM_MIN_ALLOC)
#define PMEM_END_ADDR(id, index) (PMEM_START_ADDR(id, index) + \
	PMEM_LEN(id, index))
#define PMEM_REVOKED(data) (data->flags & PMEM_FLAGS_REVOKED)
#define PMEM_IS_PAGE_ALIGNED(addr) (!((addr) & (~PAGE_MASK)))
#define PMEM_IS_SUBMAP(data) ((data->flags & PMEM_FLAGS_SUBMAP) && \
	(!(data->flags & PMEM_FLAGS_UNSUBMAP)))

static int pmem_release(struct inode *, struct file *);
static int pmem_mmap(struct file *, struct vm_area_struct *);
static int pmem_open(struct inode *, struct file *);
static long pmem_ioctl(struct file *, unsigned int, unsigned long);

struct file_operations pmem_fops = {
	.release = pmem_release,
	.mmap = pmem_mmap,
	.open = pmem_open,
	.unlocked_ioctl = pmem_ioctl,
};

static int get_id(struct file *file)
{
	return MINOR(file->f_dentry->d_inode->i_rdev);
}

int is_pmem_file(struct file *file)
{
	int id;

	if (unlikely(!file || !file->f_dentry || !file->f_dentry->d_inode))
		return 0;
	id = get_id(file);
	if (unlikely(id >= PMEM_MAX_DEVICES))
		return 0;
	if (unlikely(file->f_dentry->d_inode->i_rdev !=
	     MKDEV(MISC_MAJOR, pmem[id].mdev.minor)))
		return 0;
	return 1;
}
EXPORT_SYMBOL(is_pmem_file);

static int has_allocation(struct file *file)
{
	struct pmem_data *data;
	/* check is_pmem_file first if not accessed via pmem_file_ops */
	int id = get_id(file);

	if (unlikely(!file->private_data
		|| file->private_data == &pmem[id].mdev))
		return 0;
	data = (struct pmem_data *)file->private_data;
	if (unlikely(data->index < 0))
		return 0;
	return 1;
}

static int is_master_owner(struct file *file)
{
	struct file *master_file;
	struct pmem_data *data;
	int put_needed, ret = 0;

	if (!is_pmem_file(file) || !has_allocation(file))
		return 0;
	data = (struct pmem_data *)file->private_data;
	if (PMEM_FLAGS_MASTERMAP & data->flags)
		return 1;
	master_file = fget_light(data->master_fd, &put_needed);
	if (master_file && data->master_file == master_file)
		ret = 1;
	else {
		printk(KERN_ERR "pmem: master_file(0x%x) is NULL or not "
			"aligned\n",(unsigned int)master_file);
		return 0;
	}
	fput_light(master_file, put_needed);
	return ret;
}

static int dump_bitmap(int id)
{
	int col = 0, i, n = 0;
	char *buf;

	if (!debug_bitmap)
		return 0;

	down_read(&pmem[id].bitmap->sem);

	buf = kzalloc(PAGE_SIZE, GFP_KERNEL);
	if (buf == NULL)
		goto out;

	n += sprintf(buf + n, "\n[Used:Compound:Order]\n");
	for (i = 0; i < pmem[id].bitmap->num_entries;) {
		n += sprintf(buf + n, "[%d:%d:%d] ",
				pmem[id].bitmap->bits[i].allocated,
				pmem[id].bitmap->bits[i].compound,
				pmem[id].bitmap->bits[i].order);
		i += 1 << pmem[id].bitmap->bits[i].order;
		if (++col == 8) {
			col = 0;
			n += sprintf(buf + n, "\n");
		}
	}
	n += sprintf(buf + n, "\n");

	dev_dbg(pmem[id].dev, "%s", buf);
	kfree(buf);
out:
	up_read(&pmem[id].bitmap->sem);
	return n;
}

static int __pmem_free(int id, int index)
{
	/* caller should hold the write lock on pmem_sem! */
	int buddy, curr;
	int compound;

	if (pmem[id].no_allocator) {
		pmem[id].allocated = 0;
		return 0;
	}

	/* clean up the bitmap */
	curr = index;
	do {
		compound = pmem[id].bitmap->bits[curr].compound;
		pmem[id].bitmap->bits[curr].allocated = 0;
		pmem[id].bitmap->bits[curr].compound = 0;
		curr = PMEM_NEXT_INDEX(id, curr);
		dev_dbg(pmem[id].dev, "free addr:0x%x order:%d, compound:%d\n",
			curr, PMEM_ORDER(id, curr), compound);
	} while (compound);

	/*
	 * find a slots buddy Buddy# = Slot# ^ (1 << order)
	 * if the buddy is also free merge them
	 * repeat until the buddy is not free or end of the bitmap is reached
	 * or next block is not free (since compound page is introduced, next
	 * block may be less than current block)
	 */
	curr = index;
	do {
		buddy = PMEM_BUDDY_INDEX(id, curr);
		if (PMEM_IS_FREE(id, buddy) &&
				PMEM_ORDER(id, buddy) == PMEM_ORDER(id, curr)) {
			PMEM_ORDER(id, buddy)++;
			PMEM_ORDER(id, curr)++;
			curr = min(buddy, curr);
		} else {
			curr = PMEM_NEXT_INDEX(id, curr);
			if (!PMEM_IS_FREE(id, curr))
				break;
		}
	} while (curr < pmem[id].bitmap->num_entries);

	return 0;
}

static int pmem_free(int id, int index)
{
	int ret;
	down_write(&pmem[id].bitmap->sem);
	ret = __pmem_free(id, index);
	up_write(&pmem[id].bitmap->sem);

	dump_bitmap(id);
	return ret;
}

static void pmem_revoke(struct file *file, struct pmem_data *data);

static int pmem_release(struct inode *inode, struct file *file)
{
	struct pmem_data *data = (struct pmem_data *)file->private_data;
	struct pmem_region_node *region_node;
	struct list_head *elt, *elt2;
	int id = get_id(file), ret = 0;


	down(&pmem[id].data_list_sem);
	/* if this file is a master, revoke all the memory in the connected
	 *  files */
	if (PMEM_FLAGS_MASTERMAP & data->flags) {
		struct pmem_data *sub_data;
		list_for_each(elt, &pmem[id].data_list) {
			sub_data = list_entry(elt, struct pmem_data, list);
			down_read(&sub_data->sem);
			if (PMEM_IS_SUBMAP(sub_data) &&
			    file == sub_data->master_file) {
				up_read(&sub_data->sem);
				pmem_revoke(file, sub_data);
			}  else
				up_read(&sub_data->sem);
		}
	}
	list_del(&data->list);
	up(&pmem[id].data_list_sem);


	down_write(&data->sem);

	/* if its not a connected file and it has an allocation, free it */
	if (!(PMEM_FLAGS_CONNECTED & data->flags) && has_allocation(file)) {
		ret = pmem_free(id, data->index);
	}

	/* if this file is a submap (mapped, connected file), downref the
	 * task struct */
	if (PMEM_FLAGS_SUBMAP & data->flags)
		if (data->task) {
			put_task_struct(data->task);
			data->task = NULL;
		}

	file->private_data = NULL;

	list_for_each_safe(elt, elt2, &data->region_list) {
		region_node = list_entry(elt, struct pmem_region_node, list);
		list_del(elt);
		kfree(region_node);
	}
	BUG_ON(!list_empty(&data->region_list));

	up_write(&data->sem);
	kfree(data);
	if (pmem[id].release)
		ret = pmem[id].release(inode, file);

	return ret;
}

static int pmem_open(struct inode *inode, struct file *file)
{
	struct pmem_data *data;
	int id = get_id(file);
	int ret = 0;

	dev_dbg(pmem[id].dev, "current %u file %p(%ld)\n",
		current->pid, file, file_count(file));
	/* setup file->private_data to indicate its unmapped */
	/*  you can only open a pmem device one time */
	if (file->private_data != NULL && file->private_data != &pmem[id].mdev)
		return -1;
	data = kzalloc(sizeof(struct pmem_data), GFP_KERNEL);
	if (!data) {
		printk("pmem: unable to allocate memory for pmem metadata.");
		return -1;
	}
	data->index = -1;
	INIT_LIST_HEAD(&data->region_list);
	init_rwsem(&data->sem);

	file->private_data = data;
	INIT_LIST_HEAD(&data->list);

	down(&pmem[id].data_list_sem);
	list_add(&data->list, &pmem[id].data_list);
	up(&pmem[id].data_list_sem);
	return ret;
}

static unsigned long pmem_order(unsigned long len)
{
	int i;

	len = (len + PMEM_MIN_ALLOC - 1)/PMEM_MIN_ALLOC;
	len--;
	for (i = 0; i < sizeof(len)*8; i++)
		if (len >> i == 0)
			break;
	return i;
}

static int pmem_out_of_memory(int);

static int __pmem_allocate(int id, unsigned long len)
{
	/* caller should hold the write lock on pmem_sem! */
	/* return the corresponding pdata[] entry */
	int curr = 0;
	int end = pmem[id].bitmap->num_entries;
	int best_fit, compound_fit;
	int pages = 0;
	unsigned long order = pmem_order(len);

	if (pmem[id].no_allocator) {
		dev_dbg(pmem[id].dev, "no allocator");
		if ((len > pmem[id].size) || pmem[id].allocated)
			return -1;
		pmem[id].allocated = 1;
		return len;
	}

	if (order > PMEM_MAX_ORDER)
		return -1;

	best_fit = -1;
	compound_fit = -1;
	curr = 0;
	/*
	 * look through the bitmap:
	 * 	if you find a free slot of the correct order use it
	 *	otherwise,
	 *	1. find continuous free slots whose summation of order/len
	 *	   meets the requirement.
	 * 	2. use the best fit (smallest with size > order) slot
	 */
	while (curr < end) {
		if (PMEM_IS_FREE(id, curr)) {
			unsigned long curr_order = PMEM_ORDER(id, curr);
			if (curr_order == (unsigned char)order) {
				/* set the not free bit and clear others */
				best_fit = curr;
				break;
			}
			if (curr_order > (unsigned char)order) {
				if (best_fit < 0 ||
				    curr_order < PMEM_ORDER(id, best_fit))
					best_fit = curr;
				compound_fit = -1;
			} else {
				if (compound_fit < 0) {
					pages = ALIGN(len, PMEM_MIN_ALLOC)
						>> PMEM_MIN_SHIFT;
					compound_fit = curr;
				}
				pages = pages - (1 << curr_order);
				if (pages <= 0) {
					best_fit = compound_fit;
					break;
				}
			}
		} else
			compound_fit = -1;
		curr = PMEM_NEXT_INDEX(id, curr);
	}

	/*
	 *if best_fit < 0, there are no suitable slots,
	 * return an error
	 */
	if (best_fit < 0) {
		dev_err(pmem[id].dev, "pmem: no space left to allocate!\n");
		return -EAGAIN;
	}

	/*
	 * now partition the best fit:
	 * 1. order > length
	 * 	split the slot into 2 buddies of order - 1
	 * 2. order & length
	 *	mark allocated
	 *	mark compound if there remains un-allocation
	 * 	go ahead with the next index
	 */
	curr = best_fit;
	pages = ALIGN(len, PMEM_MIN_ALLOC) >> PMEM_MIN_SHIFT;
	while (pages > 0) {
		while (pages < (1 << PMEM_ORDER(id, curr))) {
			int buddy;
			PMEM_ORDER(id, curr) -= 1;
			buddy = PMEM_BUDDY_INDEX(id, curr);
			PMEM_ORDER(id, buddy) = PMEM_ORDER(id, curr);
		}
		dev_dbg(pmem[id].dev, "pages:%d, allocated order:%d, "
			"request order:%lu\n",
			pages, PMEM_ORDER(id, curr), pmem_order(len));
		pages = pages - (1 << PMEM_ORDER(id, curr));
		pmem[id].bitmap->bits[curr].allocated = 1;
		pmem[id].bitmap->bits[curr].compound = (pages > 0) ? 1 : 0;
		curr = PMEM_NEXT_INDEX(id, curr);
	}
	return best_fit;
}

static int pmem_allocate(int id, unsigned long len)
{
	int ret, min_adj = 8;
	down_write(&pmem[id].bitmap->sem);
	ret = __pmem_allocate(id, len);
	up_write(&pmem[id].bitmap->sem);

	if (!ctl_killer)
		goto out;

	/* min_adj -- [0 forgound], [1 visible] */
	while ((ret == -EAGAIN) && (--min_adj > 1)) {
		pmem_out_of_memory(min_adj);
		msleep(20);
		dev_dbg(pmem[id].dev, "Try to re-allocate %lu bytes memory\n",
			len);

		down_write(&pmem[id].bitmap->sem);
		ret = __pmem_allocate(id, len);
		up_write(&pmem[id].bitmap->sem);
	}
	if (ret == -EAGAIN)
		dev_warn(pmem[id].dev, "Failed to re-allocate %lu bytes "
			"memory\n", len);

out:
	dump_bitmap(id);
	return ret;
}

static pgprot_t phys_mem_access_prot(struct file *file, pgprot_t vma_prot)
{
	int id = get_id(file);
#ifdef pgprot_noncached
	if ((pmem[id].cached == 0 && pmem[id].buffered == 0) ||
	    file->f_flags & O_SYNC)
		return pgprot_noncached(vma_prot);
#endif
#ifdef pgprot_writecombine
	if (pmem[id].cached == 0 && pmem[id].buffered != 0)
		return pgprot_writecombine(vma_prot);
#endif
	return vma_prot;
}

static unsigned long pmem_start_addr(int id, struct pmem_data *data)
{
	if (pmem[id].no_allocator)
		return PMEM_START_ADDR(id, 0);
	else
		return PMEM_START_ADDR(id, data->index);

}

static unsigned long pmem_len(int id, struct pmem_data *data)
{
	if (pmem[id].no_allocator) {
		if(data->index >= 0) {
			return data->index;
		}
		else {
			dev_err(pmem[id].dev, "pmem: data->index has "
				"error=%d\n",data->index);
			return 0;
		}
	}
	else {
		unsigned long len = 0;
		int next = data->index, curr;
		do {
			curr = next;
			next = PMEM_NEXT_INDEX(id, curr);
			len += PMEM_LEN(id, curr);
		} while (pmem[id].bitmap->bits[curr].compound);
		return len;
	}
}

static void *pmem_start_vaddr(int id, struct pmem_data *data)
{
	struct file *master_file;
	int put_needed;

	if ((!data->vma) && (PMEM_FLAGS_CONNECTED && data->flags)) {
		/* check master file memory mapping instead */
		master_file = fget_light(data->master_fd, &put_needed);
		if (master_file && data->master_file == master_file)
			data = (struct pmem_data *) master_file->private_data;
		fput_light(master_file, put_needed);
	}

	BUG_ON(!data || !data->vma);
	return data->vma->vm_start;
}

int get_pmem_area(int minor, struct pmem_region *index,
		  unsigned long *start, void **vstart)
{
	int ret;
	if (!((minor < PMEM_MAX_DEVICES) && index && start))
		return -EINVAL;
	ret = pmem_allocate(minor, index->len);
	if (ret < 0)
		return ret;
	*start = PMEM_START_ADDR(minor, ret);
	/* FIXME: don't care vstart */
	index->offset = ret;
	return 0;
}
EXPORT_SYMBOL(get_pmem_area);

int put_pmem_area(int minor, struct pmem_region *index, void *vstart)
{
	if (!((minor < PMEM_MAX_DEVICES) && index))
		return -EINVAL;
	/* FIXME: don't care vstart */
	return pmem_free(minor, index->offset);
}
EXPORT_SYMBOL(put_pmem_area);

static int pmem_map_garbage(int id, struct vm_area_struct *vma,
			    struct pmem_data *data, unsigned long offset,
			    unsigned long len)
{
	int i, garbage_pages = len >> PAGE_SHIFT;

	vma->vm_flags |= VM_IO | VM_RESERVED | VM_PFNMAP | VM_SHARED | VM_WRITE;
	for (i = 0; i < garbage_pages; i++) {
		if (vm_insert_pfn(vma, vma->vm_start + offset + (i * PAGE_SIZE),
		    pmem[id].garbage_pfn))
			return -EAGAIN;
	}
	return 0;
}

static int pmem_unmap_pfn_range(int id, struct vm_area_struct *vma,
				struct pmem_data *data, unsigned long offset,
				unsigned long len)
{
	int garbage_pages;
	dev_dbg(pmem[id].dev, "unmap offset %lx len %lx\n", offset, len);

	BUG_ON(!PMEM_IS_PAGE_ALIGNED(len));

	garbage_pages = len >> PAGE_SHIFT;
	zap_page_range(vma, vma->vm_start + offset, len, NULL);
	pmem_map_garbage(id, vma, data, offset, len);
	return 0;
}

static int pmem_map_pfn_range(int id, struct vm_area_struct *vma,
			      struct pmem_data *data, unsigned long offset,
			      unsigned long len)
{
	dev_dbg(pmem[id].dev, "map offset %lx len %lx\n", offset, len);
	BUG_ON(!PMEM_IS_PAGE_ALIGNED(vma->vm_start));
	BUG_ON(!PMEM_IS_PAGE_ALIGNED(vma->vm_end));
	BUG_ON(!PMEM_IS_PAGE_ALIGNED(len));
	BUG_ON(!PMEM_IS_PAGE_ALIGNED(offset));

	if (io_remap_pfn_range(vma, vma->vm_start + offset,
		(pmem_start_addr(id, data) + offset) >> PAGE_SHIFT,
		len, vma->vm_page_prot)) {
		return -EAGAIN;
	}
	return 0;
}

static int pmem_remap_pfn_range(int id, struct vm_area_struct *vma,
			      struct pmem_data *data, unsigned long offset,
			      unsigned long len)
{
	/* hold the mm semp for the vma you are modifying when you call this */
	BUG_ON(!vma);
	zap_page_range(vma, vma->vm_start + offset, len, NULL);
	return pmem_map_pfn_range(id, vma, data, offset, len);
}

static void pmem_vma_open(struct vm_area_struct *vma)
{
	struct file *file = vma->vm_file;
	struct pmem_data *data = file->private_data;
	int id = get_id(file);
	/* this should never be called as we don't support copying pmem
	 * ranges via fork */
	BUG_ON(!has_allocation(file));
	down_write(&data->sem);
	/* remap the garbage pages, forkers don't get access to the data */
	pmem_unmap_pfn_range(id, vma, data, 0, vma->vm_start - vma->vm_end);
	up_write(&data->sem);
}

static void pmem_vma_close(struct vm_area_struct *vma)
{
	struct file *file = vma->vm_file;
	struct pmem_data *data = file->private_data;
	int id = get_id(file);

	dev_dbg(pmem[id].dev, "current %u ppid %u file %p count %ld\n",
		current->pid, current->parent->pid,
		file, file_count(file));
	if (unlikely(!is_pmem_file(file) || !has_allocation(file))) {
		dev_warn(pmem[id].dev, "pmem: something is wrong, you are "
		       "closing a vm backing an allocation that doesn't "
		       "exist!\n");
		return;
	}
	down_write(&data->sem);
	if (data->vma == vma) {
		data->vma = NULL;
		if ((data->flags & PMEM_FLAGS_CONNECTED) &&
		    (data->flags & PMEM_FLAGS_SUBMAP))
			data->flags |= PMEM_FLAGS_UNSUBMAP;
	}
	/* the kernel is going to free this vma now anyway */
	up_write(&data->sem);
}

static struct vm_operations_struct vm_ops = {
	.open = pmem_vma_open,
	.close = pmem_vma_close,
};

static int pmem_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct pmem_data *data;
	unsigned long vma_size =  vma->vm_end - vma->vm_start;
	int ret = 0, id = get_id(file);

	if (vma->vm_pgoff || !PMEM_IS_PAGE_ALIGNED(vma_size)) {
		dev_err(pmem[id].dev, "pmem: mmaps must be at offset zero, "
			"aligned and a multiple of pages_size.\n");
		return -EINVAL;
	}

	data = (struct pmem_data *)file->private_data;
	down_write(&data->sem);
	if (data->flags & PMEM_FLAGS_UNSUBMAP) {
	/* if this file has been mmaped, downref the task struct for re-mmap */
		if (data->flags & PMEM_FLAGS_SUBMAP) {
			put_task_struct(data->task);
			data->task = NULL;
			data->flags &= ~(PMEM_FLAGS_SUBMAP);
		}
		data->flags &= ~(PMEM_FLAGS_UNSUBMAP);
	} else if (data->flags & PMEM_FLAGS_SUBMAP) {
	/* check this file isn't already mmaped */
		dev_err(pmem[id].dev, "pmem: this file is already mmaped. "
			"flags:%x\n", data->flags);
		ret = -EINVAL;
		goto error;
	}
	/* if file->private_data == unalloced, alloc*/
	if (data->index == -1) {
		data->index = pmem_allocate(id, vma->vm_end - vma->vm_start);
	}
	/* either no space was available or an error occured */
	if (!has_allocation(file)) {
		ret = -EINVAL;
		dev_err(pmem[id].dev, "pmem: could not find allocation for "
			"map.\n");
		goto error;
	}

	if (pmem_len(id, data) < vma_size) {
		dev_warn(pmem[id].dev, "pmem: mmap size [%lu] does not match"
		       "size of backing region [%lu].\n", vma_size,
		       pmem_len(id, data));
		ret = -EINVAL;
		goto error;
	}

	vma->vm_pgoff = pmem_start_addr(id, data) >> PAGE_SHIFT;
	vma->vm_page_prot = phys_mem_access_prot(file, vma->vm_page_prot);

	if (data->flags & PMEM_FLAGS_CONNECTED) {
		struct pmem_region_node *region_node;
		struct list_head *elt;
		if (pmem_map_garbage(id, vma, data, 0, vma_size)) {
			dev_info(pmem[id].dev,
				"pmem: mmap failed in kernel!\n");
			ret = -EAGAIN;
			goto error;
		}
		list_for_each(elt, &data->region_list) {
			region_node = list_entry(elt, struct pmem_region_node,
						 list);
			dev_dbg(pmem[id].dev, "remapping file: %p %lx %lx\n",
				file, region_node->region.offset,
				region_node->region.len);
			if (pmem_remap_pfn_range(id, vma, data,
						 region_node->region.offset,
						 region_node->region.len)) {
				ret = -EAGAIN;
				goto error;
			}
		}
		data->flags |= PMEM_FLAGS_SUBMAP;
		get_task_struct(current->group_leader);
		data->task = current->group_leader;
#if PMEM_DEBUG
		data->pid = current->pid;
#endif
		dev_dbg(pmem[id].dev, "submmapped file %p vma %p pid %u\n",
			file, vma, current->pid);
	} else {
		if (pmem_map_pfn_range(id, vma, data, 0, vma_size)) {
			dev_info(pmem[id].dev,
				"pmem: mmap failed in kernel!\n");
			ret = -EAGAIN;
			goto error;
		}
		data->flags |= PMEM_FLAGS_MASTERMAP;
		data->pid = current->pid;
	}
	data->vma = vma;
	vma->vm_ops = &vm_ops;
error:
	up_write(&data->sem);
	return ret;
}

/* the following are the api for accessing pmem regions by other drivers
 * from inside the kernel */
int get_pmem_user_addr(struct file *file, unsigned long *start,
		   unsigned long *len)
{
	struct pmem_data *data;
	int id = get_id(file);
	if (!is_pmem_file(file) || !has_allocation(file)) {
		dev_info(pmem[id].dev,
			"pmem: requested pmem data from invalid file.\n");
		return -1;
	}
	data = (struct pmem_data *)file->private_data;
	down_read(&data->sem);
	if (data->vma) {
		*start = data->vma->vm_start;
		*len = data->vma->vm_end - data->vma->vm_start;
	} else {
		*start = 0;
		*len = 0;
	}
	up_read(&data->sem);
	return 0;
}
EXPORT_SYMBOL(get_pmem_user_addr);

int get_pmem_addr(struct file *file, unsigned long *start, void **vstart,
		  unsigned long *len)
{
	struct pmem_data *data;
	int id = get_id(file);

	if (!is_pmem_file(file) || !has_allocation(file)) {
		return -1;
	}

	data = (struct pmem_data *)file->private_data;
	if (data->index == -1) {
		dev_info(pmem[id].dev, "pmem: requested pmem data from file "
			"with no allocation.\n");
		return -1;
	}

	down_write(&data->sem);
	*len = pmem_len(id, data);
	*start = pmem_start_addr(id, data);
	*vstart = pmem_start_vaddr(id, data);
#ifdef PMEM_DEBUG
	data->ref++;
#endif
	up_write(&data->sem);
	return 0;
}

int get_pmem_file(int fd, unsigned long *start, void **vstart,
		  unsigned long *len, struct file **filp)
{
	struct file *file;

	file = fget(fd);
	if (unlikely(file == NULL)) {
		printk(KERN_INFO "pmem: requested data from file descriptor "
		       "that doesn't exist.");
		return -1;
	}

	if (get_pmem_addr(file, start, vstart, len))
		goto end;

	if (filp)
		*filp = file;
	return 0;
end:
	fput(file);
	return -1;
}

int put_pmem_addr(struct file *file)
{
	struct pmem_data *data;
	int id;

	if (!is_pmem_file(file))
		return -1;
	id = get_id(file);
	data = (struct pmem_data *)file->private_data;
#if PMEM_DEBUG
	down_write(&data->sem);
	if (data->ref == 0) {
		printk("pmem: pmem_put > pmem_get %s (pid %d)\n",
		       pmem[id].mdev.name, data->pid);
		BUG();
	}
	data->ref--;
	up_write(&data->sem);
#endif
	return 0;
}

void put_pmem_file(struct file *file)
{
	if (put_pmem_addr(file))
		return;
	fput(file);
}

void sync_pmem_area(void *vaddr, unsigned long addr, unsigned long len,
		    unsigned int cmd, enum dma_data_direction dir)
{
	switch (cmd) {
	case PMEM_CACHE_FLUSH:
		dmac_flush_range(vaddr, vaddr + len);
		outer_flush_range(addr, addr + len);
		break;
	case PMEM_MAP_REGION:
		dmac_map_area(vaddr, len, dir);
		if (dir == DMA_FROM_DEVICE)
			outer_inv_range(addr, addr + len);
		else
			outer_clean_range(addr, addr + len);
		break;
	case PMEM_UNMAP_REGION:
		if (dir != DMA_TO_DEVICE)
			outer_inv_range(addr, addr + len);
		dmac_unmap_area(vaddr, len, dir);
	}
}
EXPORT_SYMBOL(sync_pmem_area);

void sync_pmem_file(struct file *file, unsigned long offset, unsigned long len,
		    unsigned int cmd, enum dma_data_direction dir)
{
	struct pmem_data *data;
	int id;
	void *vaddr;
	unsigned long addr, plen;
	struct pmem_region_node *region_node;
	struct list_head *elt;

	if (!is_pmem_file(file) || !has_allocation(file)) {
		return;
	}

	id = get_id(file);
	data = (struct pmem_data *)file->private_data;
	if (!pmem[id].cached || file->f_flags & O_SYNC)
		return;

	get_pmem_addr(file, &addr, &vaddr, &plen);
	down_read(&data->sem);
	/*
	if this isn't a submmapped file, don't flush the whole thing. we do have
	some limitations in application, which push the kernel to flush/invalidate/clean
	part of the whole area.
	*/
	if (unlikely(!(data->flags & PMEM_FLAGS_CONNECTED))) {
		if ((offset+len) <= plen)
			sync_pmem_area(vaddr+offset, addr+offset, len, cmd, dir);
		goto end;
	}

	/*sync the region of the file we are drawing */
	list_for_each(elt, &data->region_list) {
		region_node = list_entry(elt, struct pmem_region_node, list);
		if ((offset >= region_node->region.offset) &&
		    ((offset + len) <= (region_node->region.offset +
			region_node->region.len))) {
			sync_pmem_area(vaddr + region_node->region.offset,
				       addr + region_node->region.offset,
				       region_node->region.len, cmd, dir);
			break;
		}
	}
end:
	up_read(&data->sem);
	put_pmem_addr(file);
}
EXPORT_SYMBOL(sync_pmem_file);

void flush_pmem_file(struct file *file, unsigned long offset, unsigned long len)
{
	sync_pmem_file(file, offset, len, PMEM_CACHE_FLUSH, 0);
}
EXPORT_SYMBOL(flush_pmem_file);

static int pmem_connect(unsigned long connect, struct file *file)
{
	struct pmem_data *data = (struct pmem_data *)file->private_data;
	struct pmem_data *src_data;
	struct file *src_file;
	int ret = 0, put_needed, id = get_id(file);

	down_write(&data->sem);
	/* retrieve the src file and check it is a pmem file with an alloc */
	src_file = fget_light(connect, &put_needed);
	dev_dbg(pmem[id].dev, "connect %p to %p\n", file, src_file);
	if (!src_file) {
		dev_info(pmem[id].dev, "pmem: src file not found!\n");
		ret = -EINVAL;
		goto err_no_file;
	}
	if (unlikely(!is_pmem_file(src_file) || !has_allocation(src_file))) {
		dev_info(pmem[id].dev, "pmem: src file is not a pmem file or "
			"has no alloc!\n");
		ret = -EINVAL;
		goto err_bad_file;
	}
	src_data = (struct pmem_data *)src_file->private_data;

	if (has_allocation(file) && (data->index != src_data->index)) {
		dev_info(pmem[id].dev, "pmem: file is already mapped but "
			"doesn't match this src_file!\n");
		ret = -EINVAL;
		goto err_bad_file;
	}
	data->index = src_data->index;
	data->flags |= PMEM_FLAGS_CONNECTED;
	data->master_fd = connect;
	data->master_file = src_file;

err_bad_file:
	fput_light(src_file, put_needed);
err_no_file:
	up_write(&data->sem);
	return ret;
}

static void pmem_unlock_data_and_mm(struct pmem_data *data,
				    struct mm_struct *mm)
{
	up_write(&data->sem);
	if (mm != NULL) {
		up_write(&mm->mmap_sem);
		mmput(mm);
	}
}

static int pmem_lock_data_and_mm(struct file *file, struct pmem_data *data,
				 struct mm_struct **locked_mm)
{
	int ret = 0;
	int id = get_id(file);
	struct mm_struct *mm = NULL;
	*locked_mm = NULL;
lock_mm:
	down_read(&data->sem);
	if (PMEM_IS_SUBMAP(data)) {
		mm = get_task_mm(data->task);
		if (!mm) {
			dev_info(pmem[id].dev,
				"pmem: can't remap task is gone!\n");
			up_read(&data->sem);
			return -1;
		}
	}
	up_read(&data->sem);

	if (mm)
		down_write(&mm->mmap_sem);

	down_write(&data->sem);
	/* check that the file didn't get mmaped before we could take the
	 * data sem, this should be safe b/c you can only submap each file
	 * once */
	if (PMEM_IS_SUBMAP(data) && !mm) {
		pmem_unlock_data_and_mm(data, mm);
		goto lock_mm;
	}
	/* now check that vma.mm is still there, it could have been
	 * deleted by vma_close before we could get the data->sem */
	if ((data->flags & PMEM_FLAGS_UNSUBMAP) && (mm != NULL)) {
		/* might as well release this */
		if (data->flags & PMEM_FLAGS_SUBMAP) {
			put_task_struct(data->task);
			data->task = NULL;
			/* lower the submap flag to show the mm is gone */
			data->flags &= ~(PMEM_FLAGS_SUBMAP);
		}
		pmem_unlock_data_and_mm(data, mm);
		return -1;
	}
	*locked_mm = mm;
	return ret;
}

int pmem_remap(struct pmem_region *region, struct file *file,
		      unsigned operation)
{
	int ret;
	struct pmem_region_node *region_node;
	struct mm_struct *mm = NULL;
	struct list_head *elt, *elt2;
	int id = get_id(file);
	struct pmem_data *data = (struct pmem_data *)file->private_data;

	/* pmem region must be aligned on a page boundry */
	if (unlikely(!PMEM_IS_PAGE_ALIGNED(region->offset) ||
		 !PMEM_IS_PAGE_ALIGNED(region->len))) {
		dev_info(pmem[id].dev, "pmem: request for unaligned pmem "
			"suballocation %lx %lx\n", region->offset, region->len);
		return -EINVAL;
	}

	/* if userspace requests a region of len 0, there's nothing to do */
	if (region->len == 0)
		return 0;

	/* lock the mm and data */
	ret = pmem_lock_data_and_mm(file, data, &mm);
	if (ret)
		return 0;

	/* only the owner of the master file can remap the client fds
	 * that back in it */
	if (!is_master_owner(file)) {
		dev_info(pmem[id].dev, "pmem: remap requested from "
			"non-master process\n");
		ret = -EINVAL;
		goto err;
	}

	/* check that the requested range is within the src allocation */
	if (unlikely((region->offset > pmem_len(id, data)) ||
		     (region->len > pmem_len(id, data)) ||
		     (region->offset + region->len > pmem_len(id, data)))) {
		dev_info(pmem[id].dev, "pmem: suballoc doesn't fit "
			"in src_file!\n");
		ret = -EINVAL;
		goto err;
	}

	if (operation == PMEM_MAP) {
		region_node = kmalloc(sizeof(struct pmem_region_node),
			      GFP_KERNEL);
		if (!region_node) {
			ret = -ENOMEM;
			dev_info(pmem[id].dev, "No space to allocate "
				"metadata!");
			goto err;
		}
		region_node->region = *region;
		list_add(&region_node->list, &data->region_list);
	} else if (operation == PMEM_UNMAP) {
		int found = 0;
		list_for_each_safe(elt, elt2, &data->region_list) {
			region_node = list_entry(elt, struct pmem_region_node,
				      list);
			if (region->len == 0 ||
			    (region_node->region.offset == region->offset &&
			    region_node->region.len == region->len)) {
				list_del(elt);
				kfree(region_node);
				found = 1;
			}
		}
		if (!found) {
			dev_info(pmem[id].dev, "pmem: Unmap region does "
				"not map any mapped region!");
			ret = -EINVAL;
			goto err;
		}
	}

	if (data->vma && PMEM_IS_SUBMAP(data)) {
		if (operation == PMEM_MAP)
			ret = pmem_remap_pfn_range(id, data->vma, data,
						   region->offset, region->len);
		else if (operation == PMEM_UNMAP)
			ret = pmem_unmap_pfn_range(id, data->vma, data,
						   region->offset, region->len);
	}

err:
	pmem_unlock_data_and_mm(data, mm);
	return ret;
}

static void pmem_revoke(struct file *file, struct pmem_data *data)
{
	struct pmem_region_node *region_node;
	struct list_head *elt, *elt2;
	struct mm_struct *mm = NULL;
	int id = get_id(file);
	int ret = 0;

	data->master_file = NULL;
	ret = pmem_lock_data_and_mm(file, data, &mm);
	/* if lock_data_and_mm fails either the task that mapped the fd, or
	 * the vma that mapped it have already gone away, nothing more
	 * needs to be done */
	if (ret)
		return;
	/* unmap everything */
	/* delete the regions and region list nothing is mapped any more */
	if (data->vma)
		list_for_each_safe(elt, elt2, &data->region_list) {
			region_node = list_entry(elt, struct pmem_region_node,
						 list);
			pmem_unmap_pfn_range(id, data->vma, data,
					     region_node->region.offset,
					     region_node->region.len);
			list_del(elt);
			kfree(region_node);
	}
	/* delete the master file */
	pmem_unlock_data_and_mm(data, mm);
}

static void pmem_get_size(struct pmem_region *region, struct file *file)
{
	struct pmem_data *data = (struct pmem_data *)file->private_data;
	int id = get_id(file);

	if (!has_allocation(file)) {
		region->offset = 0;
		region->len = 0;
		return;
	} else {
		region->offset = pmem_start_addr(id, data);
		region->len = pmem_len(id, data);
	}
	dev_dbg(pmem[id].dev, "offset %lx len %lx\n",
		region->offset, region->len);
}


static long pmem_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct pmem_data *data;
	int id = get_id(file);

	switch (cmd) {
	case PMEM_GET_PHYS:
		{
			struct pmem_region region;
			dev_dbg(pmem[id].dev, "get_phys\n");
			if (!has_allocation(file)) {
				region.offset = 0;
				region.len = 0;
			} else {
				data = (struct pmem_data *)file->private_data;
				region.offset = pmem_start_addr(id, data);
				region.len = pmem_len(id, data);
			}
			dev_dbg(pmem[id].dev, "pmem: request for physical "
				"address of pmem region from process %d.\n",
				current->pid);
			if (copy_to_user((void __user *)arg, &region,
						sizeof(struct pmem_region)))
				return -EFAULT;
			break;
		}
	case PMEM_MAP:
		{
			struct pmem_region region;
			if (copy_from_user(&region, (void __user *)arg,
						sizeof(struct pmem_region)))
				return -EFAULT;
			data = (struct pmem_data *)file->private_data;
			return pmem_remap(&region, file, PMEM_MAP);
		}
		break;
	case PMEM_UNMAP:
		{
			struct pmem_region region;
			if (copy_from_user(&region, (void __user *)arg,
						sizeof(struct pmem_region)))
				return -EFAULT;
			data = (struct pmem_data *)file->private_data;
			return pmem_remap(&region, file, PMEM_UNMAP);
			break;
		}
	case PMEM_GET_SIZE:
		{
			struct pmem_region region;
			dev_dbg(pmem[id].dev, "get_size\n");
			pmem_get_size(&region, file);
			if (copy_to_user((void __user *)arg, &region,
						sizeof(struct pmem_region)))
				return -EFAULT;
			break;
		}
	case PMEM_GET_TOTAL_SIZE:
		{
			struct pmem_region region;
			dev_dbg(pmem[id].dev, "get total size\n");
			region.offset = 0;
			get_id(file);
			region.len = pmem[id].size;
			if (copy_to_user((void __user *)arg, &region,
						sizeof(struct pmem_region)))
				return -EFAULT;
			break;
		}
	case PMEM_ALLOCATE:
		{
			if (has_allocation(file))
				return -EINVAL;
			data = (struct pmem_data *)file->private_data;
			data->index = pmem_allocate(id, arg);
			break;
		}
	case PMEM_CONNECT:
		dev_dbg(pmem[id].dev, "connect\n");
		return pmem_connect(arg, file);
		break;
	case PMEM_CACHE_FLUSH:
		{
			struct pmem_region region;
			if (copy_from_user(&region, (void __user *)arg,
					   sizeof(struct pmem_region)))
				return -EFAULT;
			flush_pmem_file(file, region.offset, region.len);
			break;
		}
	case PMEM_MAP_REGION:
	case PMEM_UNMAP_REGION:
		{
			struct pmem_sync_region sync;
			dev_dbg(pmem[id].dev, "streaming\n");
			if (arch_is_coherent()) return 0;
			if (copy_from_user(&sync, (void __user *)arg,
					   sizeof(struct pmem_sync_region)))
				return -EFAULT;
			sync_pmem_file(file, sync.region.offset,
				       sync.region.len, cmd, sync.dir);
			break;
		}
	default:
		if (pmem[id].ioctl)
			return pmem[id].ioctl(file, cmd, arg);
		return -EINVAL;
	}
	return 0;
}

/*
 * Raise the dying task to the lowest priority of realtime task if necessary.
 */
static void boost_dying_task_prio(struct task_struct *p)
{
	struct sched_param param;

	if (!rt_task(p)) {
		param.sched_priority = 1;
		sched_setscheduler_nocheck(p, SCHED_FIFO, &param);
	}
}

static pid_t pmem_oom_killer(int adj, pid_t *pool, int size)
{
	struct task_struct *p, *selected = NULL;
	struct signal_struct *sig;
	int selected_oom_adj, i, killed = 0;

	read_lock(&tasklist_lock);
	for_each_process(p) {
		task_lock(p);
		/* skip if current process is already marked to be killed */
		for (i = 0; i < size; i++) {
			if (*(pool + i) == p->pid) {
				killed = 1;
				break;
			}
		}
		sig = p->signal;
		if (!p->mm || !sig || p->exit_state || sig->oom_adj < adj
			|| killed) {
			task_unlock(p);
			continue;
		}
		selected = p;
		selected_oom_adj = sig->oom_adj;
		task_unlock(p);
		break;
	}
	if (!selected)
		goto out;
	printk(KERN_INFO "PMEM oom killer: %d (%s), adj %d\n",
		selected->pid, selected->comm, selected_oom_adj);
	/* check flag that is signed by OOM-KILLER */
	if (!test_tsk_thread_flag(selected, TIF_MEMDIE)) {
		selected->rt.time_slice = HZ;
		set_tsk_thread_flag(selected, TIF_MEMDIE);
		force_sig(SIGKILL, selected);
	}
	boost_dying_task_prio(selected);
	read_unlock(&tasklist_lock);
	return selected->pid;
out:
	read_unlock(&tasklist_lock);
	return 0;
}

static int pmem_out_of_memory(int level)
{
	pid_t pool[64];
	int ret = -EAGAIN, min_adj, i;

	mutex_lock(&pmem_oom_lock);
	/* kill all hide and background application in android */
	for (min_adj = OOM_ADJUST_MAX + 1, i = 0; min_adj > level; ) {
		ret = pmem_oom_killer(min_adj, pool, i);
		if (!ret)
			min_adj--;
		else
			pool[i++] = ret;
	}
	mutex_unlock(&pmem_oom_lock);
	pr_info("%d processes are killed\n", i);
	return ret;
}

#if PMEM_DEBUG
static ssize_t debug_open(struct inode *inode, struct file *file)
{
	file->private_data = inode->i_private;
	return 0;
}

static ssize_t debug_read(struct file *file, char __user *buf, size_t count,
			  loff_t *ppos)
{
	struct list_head *elt, *elt2;
	struct pmem_data *data;
	struct pmem_region_node *region_node;
	int id = (int)file->private_data;
	const int debug_bufmax = 4096;
	static char buffer[4096];
	int n = 0;

	dev_dbg(pmem[id].dev, "debug open\n");
	n += scnprintf(buffer + n, debug_bufmax,
			"----control interface-----\n");
	n += scnprintf(buffer + n, debug_bufmax,
			"echo [num] > [this node]\n");
	n += scnprintf(buffer + n, debug_bufmax,
			"num:0 -- Disable bitmap dumping.\n");
	n += scnprintf(buffer + n, debug_bufmax,
			"num:1 -- Enable bitmap dumping.\n");
	n += scnprintf(buffer + n, debug_bufmax,
			"num:2 -- Disable OOM killer in PMEM.\n");
	n += scnprintf(buffer + n, debug_bufmax,
			"num:3 -- Enable OOM killer in PMEM.\n");
	n += scnprintf(buffer + n, debug_bufmax,
			"\n\n----dump interface----\n");
	n += scnprintf(buffer + n, debug_bufmax,
		      "pid #: mapped regions (offset, len) (offset,len)...\n");

	down(&pmem[id].data_list_sem);
	list_for_each(elt, &pmem[id].data_list) {
		data = list_entry(elt, struct pmem_data, list);
		down_read(&data->sem);
		n += scnprintf(buffer + n, debug_bufmax - n, "pid %u:",
				data->pid);
		list_for_each(elt2, &data->region_list) {
			region_node = list_entry(elt2, struct pmem_region_node,
				      list);
			n += scnprintf(buffer + n, debug_bufmax - n,
					"(%lx,%lx) ",
					region_node->region.offset,
					region_node->region.len);
		}
		n += scnprintf(buffer + n, debug_bufmax - n, "\n");
		up_read(&data->sem);
	}
	up(&pmem[id].data_list_sem);

	n++;
	buffer[n] = 0;
	return simple_read_from_buffer(buf, count, ppos, buffer, n);
}

static ssize_t debug_write(struct file *file, const char __user *ubuf,
			   size_t count, loff_t *ppos)
{
	char buf[16];
	int ret;
	long i;

	memset(buf, 0, sizeof(buf));
	if (copy_from_user(&buf, ubuf, min_t(size_t, sizeof(buf) - 1, count)))
		return -EFAULT;
	ret = strict_strtol(buf, 10, &i);
	if (ret)
		return ret;
	switch (i) {
	case 0:
		debug_bitmap = 0;
		break;
	case 1:
		debug_bitmap = 1;
		break;
	case 2:
		ctl_killer = 0;
		break;
	case 3:
		ctl_killer = 1;
		break;
	}
	return count;
}

static struct file_operations debug_fops = {
	.write = debug_write,
	.read = debug_read,
	.open = debug_open,
};
#endif

#if 0
static struct miscdevice pmem_dev = {
	.name = "pmem",
	.fops = &pmem_fops,
};
#endif

int pmem_setup(struct android_pmem_platform_data *pdata, struct device *dev,
	       long (*ioctl)(struct file *, unsigned int, unsigned long),
	       int (*release)(struct inode *, struct file *))
{
	int err = 0;
	int i, index = 0;
	int id = id_count;
	id_count++;

	pmem[id].dev = dev;
	pmem[id].no_allocator = pdata->no_allocator;
	pmem[id].cached = pdata->cached;
	pmem[id].buffered = pdata->buffered;
	pmem[id].ioctl = ioctl;
	pmem[id].release = release;
	init_MUTEX(&pmem[id].data_list_sem);
	INIT_LIST_HEAD(&pmem[id].data_list);
	pmem[id].mdev.name = pdata->name;
	pmem[id].mdev.minor = id;
	pmem[id].mdev.fops = &pmem_fops;
	dev_info(pmem[id].dev, "%s: %d %d init\n", pdata->name,
		pdata->cached, pdata->buffered);
	if (!pdata->size) {
		int master = pdata->start;
		if (pdata->no_allocator != 0 ||
		    master >= id || master < 0 ||
		    pmem[master].size == 0 ||
		    pmem[master].no_allocator != 0) {
			dev_err(pmem[id].dev, "Invalid pmem parameters!\n");
			goto err_cant_register_device;
		}
		pmem[id].base = pmem[master].base;
		pmem[id].size = pmem[master].size;
		pmem[id].bitmap = pmem[master].bitmap;
	}

	err = misc_register(&pmem[id].mdev);
	if (err) {
		dev_err(pmem[id].dev, "Unable to register pmem driver!\n");
		goto err_cant_register_device;
	}
	if (pdata->size) {
		pmem[id].base = pdata->start;
		pmem[id].size = pdata->size;
		pmem[id].bitmap = kzalloc(sizeof(struct pmem_bitmap),
					  GFP_KERNEL);
		if (!pmem[id].bitmap)
			goto err_no_mem_for_metadata;

		pmem[id].bitmap->num_entries = pmem[id].size / PMEM_MIN_ALLOC;
		pmem[id].bitmap->bits = kzalloc(pmem[id].bitmap->num_entries *
						sizeof(struct pmem_bits),
						GFP_KERNEL);
		if (!pmem[id].bitmap->bits)
			goto err_no_mem_for_bitmap;

		for (i = sizeof(pmem[id].bitmap->num_entries) * 8 - 1;
		     i >= 0; i--) {
			if ((pmem[id].bitmap->num_entries) & 1<<i) {
				PMEM_ORDER(id, index) = i;
				index = PMEM_NEXT_INDEX(id, index);
			}
		}

		init_rwsem(&pmem[id].bitmap->sem);
	}

	pmem[id].garbage_pfn = page_to_pfn(alloc_page(GFP_KERNEL));
	if (pmem[id].no_allocator)
		pmem[id].allocated = 0;

#if PMEM_DEBUG
	/* configure debugfs node as both read and write */
	debugfs_create_file(pdata->name, S_IFREG | S_IRUGO | S_IWUGO, NULL,
			    (void *)id, &debug_fops);
#endif
	return 0;
err_no_mem_for_bitmap:
	kfree(pmem[id].bitmap);
err_no_mem_for_metadata:
	misc_deregister(&pmem[id].mdev);
err_cant_register_device:
	return -1;
}

static int pmem_probe(struct platform_device *pdev)
{
	struct android_pmem_platform_data *pdata;

	if (!pdev || !pdev->dev.platform_data) {
		printk(KERN_ALERT "Unable to probe pmem!\n");
		return -1;
	}
	pdata = pdev->dev.platform_data;
	return pmem_setup(pdata, &pdev->dev, NULL, NULL);
}


static int pmem_remove(struct platform_device *pdev)
{
	int id = pdev->id;
	__free_page(pfn_to_page(pmem[id].garbage_pfn));
	misc_deregister(&pmem[id].mdev);
	return 0;
}

static struct platform_driver pmem_driver = {
	.probe = pmem_probe,
	.remove = pmem_remove,
	.driver = { .name = "android_pmem" }
};


static int __init pmem_init(void)
{
	return platform_driver_register(&pmem_driver);
}

static void __exit pmem_exit(void)
{
	platform_driver_unregister(&pmem_driver);
}

module_init(pmem_init);
module_exit(pmem_exit);

