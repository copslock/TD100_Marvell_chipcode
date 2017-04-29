/*
* (C) Copyright 2010 Marvell International Ltd.
* All Rights Reserved
*
* MARVELL CONFIDENTIAL
* Copyright 2008 ~ 2010 Marvell International Ltd All Rights Reserved.
* The source code contained or described herein and all documents related to
* the source code ("Material") are owned by Marvell International Ltd or its
* suppliers or licensors. Title to the Material remains with Marvell International Ltd
* or its suppliers and licensors. The Material contains trade secrets and
* proprietary and confidential information of Marvell or its suppliers and
* licensors. The Material is protected by worldwide copyright and trade secret
* laws and treaty provisions. No part of the Material may be used, copied,
* reproduced, modified, published, uploaded, posted, transmitted, distributed,
* or disclosed in any way without Marvell's prior express written permission.
*
* No license under any patent, copyright, trade secret or other intellectual
* property right is granted to or conferred upon you by disclosure or delivery
* of the Materials, either expressly, by implication, inducement, estoppel or
* otherwise. Any license under such intellectual property rights must be
* express and approved by Marvell in writing.
*
*/


#ifndef MARVELL_FILEMONITORSYSTEM_H
#define MARVELL_FILEMONITORSYSTEM_H

#include <utils/String8.h>
#include <utils/String16.h>
#include <binder/IServiceManager.h>
#include "IFileMonitorService.h"
#include "IFileMonitorCallback.h"

namespace android{

class FileMonitorCallback : public BnFileMonitorCallback, public IBinder::DeathRecipient {
public:
    FileMonitorCallback(const String8& name) 
        : BnFileMonitorCallback()
    {
        mFileName = name;
    }
    ~FileMonitorCallback(){}
    virtual void binderDied(const wp<IBinder>& who){}
    virtual String8 getFileName() { return mFileName; }
};

class FileMonitorSystem {
public:
    static void registerCallback(const sp<IFileMonitorCallback>& callback)
    {
        sp<IServiceManager> sm = defaultServiceManager();
        sp<IBinder> binder;
        do {
            binder = sm->getService(String16("com.marvell.FileMonitorService"));
            if (binder != 0)
                break;
            LOGW("FileMonitorService not published, waiting...");
            usleep(500000);
        } while(true);

        sp<IFileMonitorService> fms = interface_cast<IFileMonitorService>(binder);
        fms->registerCallback(callback);
    }
private:
    FileMonitorSystem(){}
    ~FileMonitorSystem(){}
};

};//end android
#endif // MARVELL_FILEMONITORSYSTEM_H


