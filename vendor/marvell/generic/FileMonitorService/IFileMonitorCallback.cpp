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

#define LOG_TAG	"FileMonitorCallback"
#define LOG_NDEBUG 0

#include <stdint.h>
#include <sys/types.h>
#include <utils/Log.h>
#include <binder/Parcel.h>
#include <IFileMonitorCallback.h>

#include <utils/Errors.h>  // for status_t

namespace android {

enum {
    RELOAD_FILE = IBinder::FIRST_CALL_TRANSACTION,
    GET_FILE_NAME
};

class BpFileMonitorCallback: public BpInterface<IFileMonitorCallback>
{
    public:
        BpFileMonitorCallback(const sp<IBinder>& impl)
            : BpInterface<IFileMonitorCallback>(impl)
        {
        }

        virtual void reloadFile()
        {
            Parcel data, reply;
            data.writeInterfaceToken(IFileMonitorCallback::getInterfaceDescriptor());
            remote()->transact(RELOAD_FILE, data, &reply);
            return;
        }

        virtual String8 getFileName()
        {
            Parcel data, reply;
            data.writeInterfaceToken(IFileMonitorCallback::getInterfaceDescriptor());
            remote()->transact(GET_FILE_NAME, data, &reply);
            return reply.readString8();
        }
};

IMPLEMENT_META_INTERFACE(FileMonitorCallback, "com.marvell.IFileMonitorCallback");

// ----------------------------------------------------------------------

status_t BnFileMonitorCallback::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch(code) {
        case RELOAD_FILE:
            CHECK_INTERFACE(IFileMonitorCallback, data, reply);
            reloadFile();
            return NO_ERROR;
        case GET_FILE_NAME:
            CHECK_INTERFACE(IFileMonitorCallback, data, reply);
            reply->writeString8(getFileName());
            return NO_ERROR;
        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}

}


