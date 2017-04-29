#ifndef __PRM_H
#define __PRM_H

#include <linux/interrupt.h>
#include <asm/arch/irqs.h>
#include <asm/arch/cpu-freq-voltage-mhn.h>
#include <asm/arch/pmu.h>

#define MAX_GROUPS 	2
#define MAX_CLIENTS	16

typedef enum {
    /* tag the loweset priority*/
    PRI_LOWEST = 0,
    /*define the possible priorities here*/
	PRI_IPMC = PRI_LOWEST,
    PRI_PROFILER,
    PRI_VTUNE,
    /*tag the highest priority*/
    MAX_PRIORITIES,
    PRI_HIGHEST = MAX_PRIORITIES -1,
}prm_priority;

struct prm_group;
struct prm_resource;
struct prm_resource_state;

typedef enum {
	PRM_RES_APPROPRIATED,
	PRM_RES_READY,
}prm_event;

typedef enum {
	PRM_CCNT = 0,
	PRM_PMN0,
	PRM_PMN1,
	PRM_PMN2,
	PRM_PMN3,
	PRM_VCC0,
	PRM_VCC1,
	PRM_IDLE_PROFILER,
	PRM_COP,
	RESOURCE_NUM,
}prm_resource_id;

typedef void (*clientcallback)(prm_event, unsigned int, void *);
typedef irqreturn_t (*isrhandler)(int , void * , struct pt_regs *);

/* The gourp includes a set of resources. If one of the set of resources is 
   appropriated, the other resources will not available for access. But the 
   resources are still allocated by the client. So the group is defined as 
  "a set of resources that all can be accessed or all can not be accessed. */
struct prm_group {
    unsigned int id;
	unsigned int appropriated_cnt; 	/* appropriated resources count */
	unsigned int member_cnt;	/* total resources count in the group */
	struct list_head resources;	/* list for all the resources in the group */
	struct proc_dir_entry *dir;
};

struct prm_client {
	unsigned int id;	/* client id */
	unsigned int pid;	/* process id for the client */
	prm_priority priority;	/* priority for the client.(LOW or HIGH) */
	char *name;		/* name of the client */	
	unsigned int group_cnt;	/* How many groups in the client */
	struct prm_group *groups[MAX_GROUPS]; /* support MAXGROUP groups, some may be NULL */
	void *client_data;
	clientcallback notify;	/* notifier for resource appropriate and ready */
	isrhandler handler;
	void *dev_id;
	struct proc_dir_entry *dir;
};

/* resources state */
struct prm_resource_state {
	struct prm_client *allocate;	/* which client allocate the resources. In every priority, there can be only one client allocate the resource*/
	struct prm_group *group;	/* which group it belongs to */
	int active;
	struct prm_resource *resource;
	struct list_head entry;	/* used by prm_group->resources for link the resources into the group */
	struct proc_dir_entry *dir;
};

struct prm_resource {
	struct prm_client *access;	/* Only one client can access it */
	prm_resource_id id;
	struct prm_resource_state priority[MAX_PRIORITIES];
	struct proc_dir_entry *dir;
};

int prm_open_session(prm_priority , char * , clientcallback , void * );
int prm_close_session(unsigned int );
int prm_allocate_resource(unsigned int , prm_resource_id , unsigned int );
int prm_free_resources(unsigned int , unsigned int );
int prm_commit_resources(unsigned int , unsigned int );
int pmu_read_register(unsigned int , int , unsigned int * );
int pmu_write_register(unsigned int , int , unsigned int );
int pmu_set_event(unsigned int , unsigned int , int * , int );
int pmu_enable_event_counting(unsigned int );
int pmu_disable_event_counting(unsigned int );
int pmu_enable_event_interrupt(unsigned int , int );
int pmu_disable_event_interrupt(unsigned int , int );
int pmu_register_isr(unsigned int , isrhandler , void * );
int pmu_unregister_isr(unsigned int );
int cop_get_num_of_cops(void);
int cop_get_cop(unsigned int , unsigned int , struct mhn_fv_info *);
int cop_set_cop(unsigned int , unsigned int , int mode);
int cop_get_def_cop(unsigned int , unsigned int *, struct mhn_fv_info *);
int cop_set_def_cop(unsigned int );
int cop_get_cur_cop(unsigned int , unsigned int *, struct mhn_fv_info *);

#endif
