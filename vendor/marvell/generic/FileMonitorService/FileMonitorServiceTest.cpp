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

#define LOG_TAG "FileMonitorServiceTest"
#define LOG_NDEBUG 0

#include <unistd.h>
#include <utils/Log.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include "FileMonitorSystem.h"

using namespace android;

class MyFileMonitorCallback : public FileMonitorCallback {
public:
    MyFileMonitorCallback(const String8& name)
        : FileMonitorCallback(name)
    {
    }

    void reloadFile()
    {
        //reload file content...
        LOGD("Reloading file content...");
    }
};

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        LOGE("Usage: %s <monitor file name>", argv[0]);
        return -1;
    }

    FileMonitorSystem::registerCallback(new MyFileMonitorCallback(String8(argv[1])));

    ProcessState::self()->startThreadPool();
    IPCThreadState::self()->joinThreadPool();

    return 0;
}


