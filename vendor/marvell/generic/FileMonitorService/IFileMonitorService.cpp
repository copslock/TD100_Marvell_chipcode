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

#include <binder/Parcel.h>
#include <binder/IMemory.h>
#include "IFileMonitorService.h"
#include "IFileMonitorCallback.h"

#include <utils/Errors.h>  // for status_t
namespace android {

enum {
    REGISTER_CALLBACK = IBinder::FIRST_CALL_TRANSACTION
};

class BpFileMonitorService: public BpInterface<IFileMonitorService>
{
    public:
        BpFileMonitorService(const sp<IBinder>& impl)
            : BpInterface<IFileMonitorService>(impl)
        {
        }

        virtual int registerCallback(const sp<IFileMonitorCallback>& callback){
            Parcel data, reply;
            data.writeInterfaceToken(IFileMonitorService::getInterfaceDescriptor());
            data.writeStrongBinder(callback->asBinder());
            remote()->transact(REGISTER_CALLBACK, data, &reply);
            return 0;     
        }
};

IMPLEMENT_META_INTERFACE(FileMonitorService, "com.marvell.IFileMonitorService");

// ----------------------------------------------------------------------

status_t BnFileMonitorService::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch(code) {
        case REGISTER_CALLBACK: {
            CHECK_INTERFACE(IFileMonitorService, data, reply);
            sp<IFileMonitorCallback> callback = interface_cast<IFileMonitorCallback>(data.readStrongBinder());
            int32_t result = registerCallback(callback);
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;

        } break;

        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}

}


