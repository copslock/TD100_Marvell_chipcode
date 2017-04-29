/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

#ifndef _HIFIEQCALIBRATION_H_
#define _HIFIEQCALIBRATION_H_

#include <FileMonitorSystem.h>

namespace android {
class HifiEqCalibrationCallback : public FileMonitorCallback {
public:
    HifiEqCalibrationCallback(const String8& name);
    void reloadFile();
};
}
#endif  /* HIFIEQCALIBRATION_H_*/
