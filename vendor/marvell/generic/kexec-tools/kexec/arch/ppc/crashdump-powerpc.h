#ifndef CRASHDUMP_POWERPC_H
#define CRASHDUMP_POWERPC_H

struct kexec_info;
int load_crashdump_segments(struct kexec_info *info, char *mod_cmdline,
				unsigned long max_addr, unsigned long min_base);
void add_usable_mem_rgns(unsigned long long base, unsigned long long size);

extern struct arch_options_t arch_options;

#ifdef CONFIG_PPC64
#define PAGE_OFFSET	0xC000000000000000UL
#define VMALLOCBASE	0xD000000000000000UL
#define MAXMEM		(-KERNELBASE-VMALLOCBASE)
#else
#define PAGE_OFFSET	0xC0000000
#define MAXMEM		0x30000000 /* Use CONFIG_LOWMEM_SIZE from kernel */
#endif

#define KERNELBASE	PAGE_OFFSET
#define __pa(x)		((unsigned long)(x)-PAGE_OFFSET)

#define COMMAND_LINE_SIZE	512 /* from kernel */
/* Backup Region, First 64K of System RAM. */
#define BACKUP_SRC_START	0x0000
#define BACKUP_SRC_END		0xffff
#define BACKUP_SRC_SIZE		(BACKUP_SRC_END - BACKUP_SRC_START + 1)

#define KDUMP_BACKUP_LIMIT	BACKUP_SRC_SIZE
#define _ALIGN_UP(addr, size)	(((addr)+((size)-1))&(~((size)-1)))
#define _ALIGN_DOWN(addr, size)	((addr)&(~((size)-1)))

extern unsigned long long crash_base;
extern unsigned long long crash_size;
extern unsigned int rtas_base;
extern unsigned int rtas_size;

#endif /* CRASHDUMP_POWERPC_H */
