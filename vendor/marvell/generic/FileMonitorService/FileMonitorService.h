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


#ifndef MARVELL_FILEMONITORSERVICE_H
#define MARVELL_FILEMONITORSERVICE_H

#include <utils/threads.h>
#include <utils/KeyedVector.h>
#include "IFileMonitorService.h"
#include "IFileMonitorCallback.h"

namespace android{

#define MAX_MONITOR_NUM 32
#define MAX_BUF_SIZE    4096

class FileMonitorService : public BnFileMonitorService, public IBinder::DeathRecipient {
public:
    // exported APIs
    int registerCallback(const sp<IFileMonitorCallback>& callback);    

    // internal stactic functions
    static void instantiate();
    static FileMonitorService* getInstance();
    
    // Protect mCallbacks.
    mutable Mutex   mCallbackLock;      
    KeyedVector<wp<IBinder>, sp<IFileMonitorCallback> >  mCallbacks;    

    // Watch descriptors
    int mWatchFd;
    int mWatchDesc[MAX_MONITOR_NUM];

    // Register notification descriptor
    int mReadFd;
    int mWriteFd;

    virtual void binderDied(const wp<IBinder>& who);    
   
private:
    FileMonitorService();
    ~FileMonitorService();
    void initMonitorConfigure();

    static Mutex mInstanceLock;
    static FileMonitorService* mInstance;
 
    class EventThread;
    sp<EventThread> mEventThread;
};
};//end android
#endif // MARVELL_FILEMONITORSERVICE_H


