/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/
#define LOG_NDEBUG 0
#define LOG_TAG "HifiEqCalibration"

#include <utils/Log.h>
#include <semaphore.h>

#include <ippdefs.h>
#include <ippSP.h>
#include <codecDef.h>

#include "HifiEqCalibration.h"
#include "IPPEqWrapper.h"

namespace android {
HifiEqCalibrationCallback::HifiEqCalibrationCallback(const String8& name)
    : FileMonitorCallback(name) {
}

void HifiEqCalibrationCallback::reloadFile() {
	reloadFDI();
	EqReset();
}
}; // namespace android
