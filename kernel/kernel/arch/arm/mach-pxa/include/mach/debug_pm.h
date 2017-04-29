
#ifndef DEBUG_PM_H
#define DEBUG_PM_H

enum pxa9xx_force_lpm {
	PXA9xx_Force_None,
	PXA9xx_Force_D2,
	PXA9xx_Force_D1,
	PXA9xx_Force_CGM,
	PXA9xx_Force_count
};

/* for debugfs*/
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/seq_file.h>

#ifdef CONFIG_DEBUG_FS
#define LPM_NAMES_LEN 20
extern const char pxa9xx_force_lpm_names__[][LPM_NAMES_LEN];
#endif
extern enum pxa9xx_force_lpm ForceLPM;
extern enum pxa9xx_force_lpm LastForceLPM;
extern unsigned int ForceLPMWakeup;
extern int RepeatMode;

extern uint32_t profilerRecommendationPP;
extern uint32_t profilerRecommendationEnable;
void pxa_9xx_power_init_debugfs(void);
void pxa_9xx_power_cleanup_debugfs(void);
#endif
