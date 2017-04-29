#ifdef CONFIG_DEBUG_FS

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <mach/debug_pm.h>
#include <asm/system.h>
#include <asm/io.h>
#include <mach/hardware.h>
#include <mach/pxa9xx_pm_logger.h>
#include <mach/pxa9xx_pm_parser.h>

/*
 * Debug fs
 */
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/seq_file.h>

#define PXA9XX__POWER_DEBUG_NAME "PM"
#define USER_BUF_SIZE 50

static ssize_t PXA9xx_pm_logger_read(struct file *file,
							char __user *userbuf,
							size_t count,
							loff_t *ppos);
static ssize_t PXA9xx_pm_logger_write(struct file *file,
							const char __user *ubuf,
							size_t count,
							loff_t *ppos);

static ssize_t PXA9xx_DVFM_ForceLPM_seq_read(struct file *file,
					char __user *userbuf,
					size_t count, loff_t *ppos);

static int PXA9xx_DVFM_ForceLPM_seq_write(struct file *file,
					const char __user *ubuf,
					size_t count, loff_t *ppos);

static ssize_t PXA9xx_DVFM_profilerRecommendation_seq_read(struct file *file,
					char __user *userbuf,
					size_t count, loff_t *ppos);
static int PXA9xx_DVFM_profilerRecommendation_write(struct file *file,
					const char __user *ubuf,
					size_t count, loff_t *ppos);
static int pxa_9xx_power_MeasureCoreFreq_open(struct inode *inode,
					struct file *file);


/* The exec names of the enum defined in debug_pm.h */
const char pxa9xx_force_lpm_names__[][LPM_NAMES_LEN] = {
	"PXA9xx_Force_None",
	"PXA9xx_Force_D2",
	"PXA9xx_Force_D1",
	"PXA9xx_Force_CGM"
};

static struct	dentry *dbgfs_root,
				*pmLogger_file,
				*ForceLPM_file,
				*MeasureCoreFreq_file,
				*profilerRecommendation_file;
uint32_t ForceLPMWakeups_tmp;
uint32_t ForceLPM_tmp;
uint32_t profilerRecommendationPP = 6;
uint32_t profilerRecommendationEnable;

static const struct file_operations PXA9xx_file_op_pmLogger = { \
	.owner		= THIS_MODULE, \
	.read		= PXA9xx_pm_logger_read, \
	.write		 = PXA9xx_pm_logger_write, \
};

static const struct file_operations PXA9xx_file_op_profilerRecommendation = { \
	.owner		= THIS_MODULE, \
	.read		= PXA9xx_DVFM_profilerRecommendation_seq_read, \
	.write		 = PXA9xx_DVFM_profilerRecommendation_write, \
};

static const struct file_operations PXA9xx_file_op_ForceLPM = {
	.owner = THIS_MODULE,
	.read = PXA9xx_DVFM_ForceLPM_seq_read,
	.write = PXA9xx_DVFM_ForceLPM_seq_write,
};


static const struct file_operations PXA9xx_file_op_MeasureCoreFreq = {
	.owner = THIS_MODULE,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
	.open = pxa_9xx_power_MeasureCoreFreq_open,
};

static ssize_t PXA9xx_pm_logger_read(struct file *file,
							char __user *userbuf,
							size_t count,
							loff_t *ppos)
{
	/* print function usage */
	printk(KERN_INFO "\
PM Logger Usage: \n\
0 - print comm log\n\
1 - print apps log\n\
2 - clear buffer\n\
3 - enable apps logger\n\
4 - disable apps logger\n\
8-128 - change buffer size in cells to the value given\n");

	return 0;
}

static ssize_t PXA9xx_pm_logger_write(struct file *file,
							const char __user *ubuf,
							size_t count,
							loff_t *ppos)

{
	unsigned int pmlogger_val;
	char buf[USER_BUF_SIZE] = {0};
	int pos = 0;
	int min_size;

	/* copy user's input to kernel space */
	min_size = min_t(char, sizeof(buf)-1, count);
	if (copy_from_user(buf, ubuf, min_size))
		return -EFAULT;
	pos += sscanf(buf, "%d", &pmlogger_val);

	/* 0 - display comm log */
	if (pmlogger_val == 0)
		pm_parser_display_log(0);

	/* 1 - display apps log */
	else if (pmlogger_val == 1)
		pm_parser_display_log(1);

	/* 2 - clear buffer */
	else if (pmlogger_val == 2) {
		printk(KERN_INFO "\nClearing buffer\n");
		pm_logger_app_clear();
	}

	/* 3 - start logger */
	else if (pmlogger_val == 3) {
		printk(KERN_INFO "\nEnable logger\n");
		pm_logger_app_start(1);
	}

	/* 4 - stop logger */
	else if (pmlogger_val == 4) {
		printk(KERN_INFO "\nDisable logger\n");
		pm_logger_app_start(0);
	}

	/* 8-128 - change buffer size */
	else {
		printk(KERN_INFO "\nChanging buffer size\n");
		pm_logger_app_change_buffSize(pmlogger_val);
	}

	return count;
}

static ssize_t PXA9xx_DVFM_ForceLPM_seq_read(struct file *file,
					char __user *userbuf,
					size_t count, loff_t *ppos)
{
	/* Consider buff size when modifiy this function */
	char buf[512] = { 0 };
	int ret, sum;
	int nameIndex = 0;

	/* Safe version of sprintf of that doesn't suffer from buffer
	 * overruns */
	ret = snprintf(buf, sizeof(buf) - 1,
		     "type [LPM to Force],[0x peripherals for wakeup] <r>\n\n");
	if (-1 == ret)
		return ret;
	sum = ret;

	for (; nameIndex < PXA9xx_Force_count; nameIndex++) {
		ret = snprintf(buf + sum, sizeof(buf) - 1,
			     "Enter %d to select %s\n", nameIndex,
			     pxa9xx_force_lpm_names__[nameIndex]);

		if (-1 == ret)
			return ret;
		sum += ret;
	}

	ret = snprintf(buf + sum, sizeof(buf) - 1,
			"\n<r> - Repeat mode\n\nForceLPM state = ");
	if (-1 == ret)
		return ret;
	sum += ret;

	if (ForceLPM == 1) {
		if (RepeatMode == 1)
			ret = snprintf(buf + sum, sizeof(buf) - 1,
				"Repeat mode\n");
		else
			ret = snprintf(buf + sum, sizeof(buf) - 1,
				"Singleshot mode\n");
		if (-1 == ret)
			return ret;
		sum += ret;
		ret = snprintf(buf + sum, sizeof(buf) - 1,
			"ForceLPMWakeup = 0x%x\n\n", ForceLPMWakeup);
	} else
		ret = snprintf(buf + sum, sizeof(buf) - 1,
			"Not active\n\n");

	if (-1 == ret)
		return ret;
	sum += ret;

	return simple_read_from_buffer(userbuf, count, ppos, buf, sum);
}

static int PXA9xx_DVFM_ForceLPM_seq_write(struct file *file,
					  const char __user *ubuf,
					  size_t count, loff_t *ppos)
{
	char buf[USER_BUF_SIZE] = { 0 };
	int pos = 0;
	int min_size;
	char ch;

	min_size = min_t(char, sizeof(buf) - 1, count);

	if (copy_from_user(buf, ubuf, min_size))
		return -EFAULT;

	pos += sscanf(buf, "%d, %x %c",
		&ForceLPM_tmp, &ForceLPMWakeups_tmp, &ch);

	if (ch == 'r')
		RepeatMode = 1;
	else
		RepeatMode = 0;

	if (ForceLPM_tmp < PXA9xx_Force_count) {
		ForceLPM = (enum pxa9xx_force_lpm) ForceLPM_tmp;
		LastForceLPM = PXA9xx_Force_None;
		ForceLPMWakeup = ForceLPMWakeups_tmp;
	} else
		printk(KERN_WARNING "\n%d is not a valid state\n",
		       ForceLPM_tmp);
	return count;
}


static ssize_t PXA9xx_DVFM_profilerRecommendation_seq_read(struct file *file,
				char __user *userbuf, size_t count, loff_t *ppos)
{
	/*Consider buff size when modifiy this function*/
	char buf[512] = {0};

	int ret, sum;

	/*Safe version of sprintf of that doesn't suffer from buffer overruns*/
	ret = snprintf(buf, sizeof(buf) - 1,
				"type [Active 0/1],[ppIndex to recommend]\n");
	if (-1 == ret)
		return ret;
	sum = ret;

	ret = snprintf(buf + sum, sizeof(buf) - sum - 1, \
		"profilerRecommendationEnable = %d\nprofilerRecommendationPP = %d\n\n",\
		profilerRecommendationEnable, profilerRecommendationPP);
	if (-1 == ret)
		return ret;
	sum += ret;
	return simple_read_from_buffer(userbuf, count, ppos, buf, sum);
}

static int PXA9xx_DVFM_profilerRecommendation_write(struct file *file,
			const char __user *ubuf, size_t count, loff_t *ppos)
{
	char buf[USER_BUF_SIZE] = {0};
	int pos = 0;
	int min_size;

	min_size = min_t(char, sizeof(buf)-1, count);

	if (copy_from_user(buf, ubuf, min_size))
		return -EFAULT;

	pos += sscanf(buf, "%d,%d", &profilerRecommendationEnable,
					&profilerRecommendationPP);
	return count;
}
static int MeasureCoreFreq(struct seq_file *seq, void *unused)
{
	unsigned long start32kHz, startCore = 0, endCore = 0xffffffff;
	unsigned long flags, cycleEnable;
	unsigned long end32Khz;

	local_irq_save(flags);

	/*
	   The correct form to use single_open with printouts is with seq_printf
	   function, and not printk, however,   in this case all the printouts
	   will appear togther on the returning to user-space, which not
	   satisfies MeasureCoreFreq printouts.
	 */
	printk(KERN_WARNING "\nMake sure the core is running on single PP!\n"
			"Starting...\n");

	/*The folowing commands are to make sure the CCNT counter is enable */

	/*Read count enable set */
	__asm__ __volatile__("mrc p15, 0, %0, c9, c12, 1;\n" :
			"=r"(cycleEnable) : );
	cycleEnable |= 0x80000000;	/*Enable cycle counter */
	/*write to Count enable set */
	__asm__ __volatile__("mcr p15, 0, %0, c9, c12, 1;\n" :
			: "r"(cycleEnable));
	/*Read Performance Monitor Control */
	__asm__ __volatile__("mrc p15, 0, %0, c9, c12, 0;\n" :
			"=r"(cycleEnable) : );
	cycleEnable |= 0x9;	/*Enable all counters and 64 prescaler */
	/*write Performance Monitor Control */
	__asm__ __volatile__("mcr p15, 0, %0, c9, c12, 0;\n" :
			: "r"(cycleEnable));

	/*Read oscr4 (32.768KHz) */
	start32kHz = __raw_readl((void *) &(__REG(0x40A00040)));

	/*Read cycle counter */
	__asm__ __volatile__("mrc p15,0,%0,c9,c13,0;\n" :
			"=r"(startCore) : );

	do {
		end32Khz = __raw_readl((void *) &(__REG(0x40A00040)));
		/* 1Sec = 32768 = 0x8000 counts for this counter */
	} while ((end32Khz - start32kHz) < 0x8000);
	/*Read cycle counter again */
	__asm__ __volatile__("mrc p15,0,%0,c9,c13,0;\n" :
			"=r"(endCore) : );

	printk(KERN_WARNING "Done.\n\nCore frequency = %lu MHz\n",
	       ((endCore - startCore) * 64 / 1000000) + 1);

	local_irq_restore(flags);
	return 0;
}



static int pxa_9xx_power_MeasureCoreFreq_open(struct inode *inode,
					      struct file *file)
{
	return single_open(file, MeasureCoreFreq, NULL);
}



void pxa_9xx_power_init_debugfs(void)
{
	int errRet = 0;
	dbgfs_root = debugfs_create_dir(PXA9XX__POWER_DEBUG_NAME, NULL);
	if (!(IS_ERR(dbgfs_root) || !dbgfs_root)) {

		pmLogger_file = debugfs_create_file("pmLogger", 0600,
			dbgfs_root, NULL,  &PXA9xx_file_op_pmLogger);
		if (pmLogger_file)
			printk(KERN_WARNING "%s success\n", __func__);
		else
			errRet = -EINVAL;

		ForceLPM_file = debugfs_create_file("ForceLPM", 0600,
				dbgfs_root, NULL, &PXA9xx_file_op_ForceLPM);

		if (ForceLPM_file)
			printk(KERN_WARNING "%s success\n", __func__);
		else
			errRet = -EINVAL;

		MeasureCoreFreq_file = debugfs_create_file("MeasureCoreFreq",
				0400, dbgfs_root, NULL,
				&PXA9xx_file_op_MeasureCoreFreq);
		if (MeasureCoreFreq_file)
			printk(KERN_WARNING "%s success\n", __func__);
		else
			errRet = -EINVAL;

		profilerRecommendation_file =
			debugfs_create_file("profilerRecommendation", 0600,
					dbgfs_root,	NULL,
					&PXA9xx_file_op_profilerRecommendation);

		if (profilerRecommendation_file)	{
			printk(KERN_WARNING "%s success\n", __func__);
		} else
			errRet = -EINVAL;
		if (errRet) {
			debugfs_remove_recursive(dbgfs_root);
			printk(KERN_ERR "%s Failed\n", __func__);
		}
	} else
		pr_err("pxa9xx_power: debugfs is not available\n");
	return;
}


void __exit pxa_9xx_power_cleanup_debugfs(void)
{
	debugfs_remove_recursive(dbgfs_root);
}
#else
inline void pxa_9xx_power_init_debugfs(void)
{
}

inline void pxa_9xx_power_cleanup_debugfs(void)
{
}
#endif
