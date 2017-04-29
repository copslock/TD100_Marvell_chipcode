// (C) Copyright 2006 Marvell International Ltd.
// All Rights Reserved
//

#include "global_types.h"

/* BackNVM configuration file */
#define BACKNVM_CFG     "BackNVM.cfg"

#ifdef __cplusplus
extern "C" {
#endif

extern void BackNVMPhase1Init(void);
extern void BackNVMPhase2Init(void);
extern BOOL IsBackNVMValid(void);
extern BOOL IsBackNVMCfgValid(void);
extern BOOL StoreBackNVM(void);
extern BOOL LoadBackNVM(void);
extern void CreateDefaultBackNVMCfg();

#ifdef __cplusplus
}
#endif

