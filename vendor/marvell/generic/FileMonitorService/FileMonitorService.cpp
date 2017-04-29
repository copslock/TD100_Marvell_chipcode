/*
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

#define LOG_TAG "FileMonitorService"
#define LOG_NDEBUG 0

#include "FileMonitorService.h"
#include "IFileMonitorCallback.h"
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
extern "C"{
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/inotify.h>
}

namespace android {

static const char* MONITOR_CONF_FILE = "/etc/FileMonitorService.conf";
static const char* DEFAULT_MONITOR_DIR = "/data/Linux/Marvell/NVM";

Mutex FileMonitorService::mInstanceLock;
FileMonitorService* FileMonitorService::mInstance = NULL;

class FileMonitorService::EventThread : public Thread {
private:
    FileMonitorService& mOwner;

public:
    EventThread(FileMonitorService& fms): Thread(false), mOwner(fms)
    {
    }

    ~EventThread()
    {
    }

    virtual void onFirstRef()
    {
        const size_t SIZE = 256;
        char buffer[SIZE];

        snprintf(buffer, SIZE, "FileMonitorServer Thread %p", this);

        run(buffer, ANDROID_PRIORITY_BACKGROUND);
    }
   
    virtual bool threadLoop()
    {
        struct inotify_event *event;
        fd_set fds;
        char buffer[MAX_BUF_SIZE];
        char signalBuffer[3];
        int len;
        int index;
        int ret;
        int maxFd;

        LOGD("FileMonitorService event thread is started.");

        while (!exitPending())
        {
            FD_ZERO(&fds);
            FD_SET(mOwner.mWatchFd, &fds);
            FD_SET(mOwner.mReadFd, &fds);

            maxFd = mOwner.mWatchFd > mOwner.mReadFd
                ? mOwner.mWatchFd : mOwner.mReadFd;
            if ((ret = select(maxFd + 1, &fds, NULL, NULL, NULL)) > 0)
            {
                //timeout or error
                if (ret <= 0)
                {
                    continue;
                }

                if (FD_ISSET(mOwner.mReadFd, &fds))
                {
                    read(mOwner.mReadFd, signalBuffer, sizeof(signalBuffer));
                }

                if (FD_ISSET(mOwner.mWatchFd, &fds))
                {
                    //no available events
                    if ((len = read(mOwner.mWatchFd, buffer, sizeof(buffer))) < 0)
                    {
                        if (errno == EINTR)
                        {
                            continue;
                        }
                        else
                        {
                            LOGE("Fail to read events: %d", errno);
                            break;
                        }
                    }

                    //parse available events
                    index = 0;
                    while (index < len)
                    {
                        event = (struct inotify_event *)(buffer + index);

                        if (event->mask == IN_CLOSE_WRITE || event->mask == IN_MOVED_TO)
                        {
                            AutoMutex _l(mOwner.mCallbackLock);
                            for (int i = 0, n = mOwner.mCallbacks.size(); i < n; ++i) {
                                if (mOwner.mCallbacks.valueAt(i)->getFileName() == String8(event->name)) {
                                    LOGD("Reloading file %s...", event->name);
                                    mOwner.mCallbacks.valueAt(i)->reloadFile();
                                }
                            }
                        }

                        index += sizeof(struct inotify_event) + event->len;
                    }
				}
            }
        }

        close(mOwner.mReadFd);
        close(mOwner.mWriteFd);

        LOGD("FileMonitorService event thread is stopped.");
        IPCThreadState::self()->stopProcess();
        return true;
    }
 
};

//-----------------------------------------------------------------------------

FileMonitorService::FileMonitorService()
    : BnFileMonitorService()
{
    mInstance = this;

    mWatchFd = inotify_init();
    if (mWatchFd < 0)
    {
        LOGE("Fail to initialize inotify fd!");
        return;
    }

    int fd[2] = { 0 };
    if (pipe(fd) == -1)
    {
        LOGE("Fail to initialize notification fd!");
        return;
    }

    mReadFd = fd[0];
    mWriteFd = fd[1];
    
    //Read configure file to get monitor directory list
    initMonitorConfigure();

    mEventThread = new EventThread(*this);
} 

FileMonitorService::~FileMonitorService()
{
    for (int i = 0; i < MAX_MONITOR_NUM; i++)
    {
        if (mWatchDesc[i] >= 0)
        {
            inotify_rm_watch(mWatchFd, mWatchDesc[i]);
        }
    }

    mEventThread->requestExit();
    write(mWriteFd, "EXT", 3);
}

void FileMonitorService::initMonitorConfigure()
{
    memset(mWatchDesc, -1, sizeof(mWatchDesc));

    FILE *conf = fopen(MONITOR_CONF_FILE, "r");
    if (conf == NULL)
    {
        //use default configure
        LOGW("%s does not exist! Monitoring %s by default", MONITOR_CONF_FILE, DEFAULT_MONITOR_DIR);
        mWatchDesc[0] = inotify_add_watch(mWatchFd, DEFAULT_MONITOR_DIR, IN_ALL_EVENTS);
        return;
    }

    const size_t SIZE = 256;
    char line[SIZE];
    int i = 0;
    while (fgets(line, SIZE, conf) != NULL)
    {
        if (line[0] == '#' || line[0] == 0x0a || line[0] == 0x20)
            continue;

        LOGD("Monitor directory: %s", line);

        line[strlen(line) - 1] = '\0'; //eliminate line break
        mWatchDesc[i++] = inotify_add_watch(mWatchFd, line, IN_ALL_EVENTS);
        if (i == MAX_MONITOR_NUM)
        {
            LOGW("FileMonitorService can only monitor %d directories!", MAX_MONITOR_NUM);
            break;
        }
    }

    fclose(conf);
}

void FileMonitorService::instantiate()
{
    defaultServiceManager()->addService(
            String16("com.marvell.FileMonitorService"), new FileMonitorService());
}

FileMonitorService* FileMonitorService::getInstance()
{
    AutoMutex _l(mInstanceLock);
    if (mInstance == NULL) {
        mInstance = new FileMonitorService();
    }

    return mInstance;
}

int FileMonitorService::registerCallback(const sp<IFileMonitorCallback>& callback)
{
    wp<IBinder> who = callback->asBinder();

    LOGD("registerCallback() %p, tid %d, calling tid %d", 
        who.unsafe_get(), gettid(), IPCThreadState::self()->getCallingPid());

    AutoMutex _l(mCallbackLock);
    mCallbacks.add(who.unsafe_get(), callback);
    callback->asBinder()->linkToDeath(this);
    callback->reloadFile();

    //notify EventThread to add new file into monitor list
    write(mWriteFd, "ADD", 3);

    LOGD("Adding %s into watch list...", callback->getFileName().string());

    return 0;
}

void FileMonitorService::binderDied(const wp<IBinder>& who)
{
    LOGD("binderDied() 1 %p, tid %d, calling tid %d", 
        who.unsafe_get(), gettid(), IPCThreadState::self()->getCallingPid());
    AutoMutex _l(mCallbackLock);

    mCallbacks.removeItem(who.unsafe_get());
}

}//end android


