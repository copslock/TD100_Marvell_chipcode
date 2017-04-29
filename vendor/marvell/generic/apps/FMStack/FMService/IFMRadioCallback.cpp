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

#define LOG_TAG	"FMRadioCallback"

#include <stdint.h>
#include <sys/types.h>
#include <utils/Log.h>
#include <binder/Parcel.h>
#include <IFMRadioCallback.h>

#include <utils/Errors.h>  // for status_t

namespace android {

enum {
    SCAN_FINISHED = IBinder::FIRST_CALL_TRANSACTION,
    FOUND_CHANNEL,
    MONO_CHANGED,
    GET_CUR_RSSI,
    GET_RDSPS_NAME,
    STATE_CHANGED,
    RECORDING_STOP
};

class BpFMRadioCallback: public BpInterface<IFMRadioCallback>
{
    public:
        BpFMRadioCallback(const sp<IBinder>& impl)
            : BpInterface<IFMRadioCallback>(impl)
        {
        }
        virtual void scan_finshed(){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioCallback::getInterfaceDescriptor());
            remote()->transact(SCAN_FINISHED, data, &reply);
            return;

        }
        virtual void found_channel(int channel){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioCallback::getInterfaceDescriptor());
            data.writeInt32(channel);
            remote()->transact(FOUND_CHANNEL, data, &reply);
            return;

        }
        virtual void mono_changed(int mono){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioCallback::getInterfaceDescriptor());
            data.writeInt32(mono);
            remote()->transact(MONO_CHANGED, data, &reply);
            return;

        }
        virtual void get_rdsps_name(char *name){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioCallback::getInterfaceDescriptor());
            data.writeInt32(strlen(name));
            // Dont write '\0', for some Java Application limitions
            data.write(name, strlen(name));
            remote()->transact(GET_RDSPS_NAME, data, &reply);
            return;
        }
        virtual void get_cur_rssi(int rssi){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioCallback::getInterfaceDescriptor());
            data.writeInt32(rssi);
            remote()->transact(GET_CUR_RSSI, data, &reply);
            return;

        }
        virtual void state_changed(int mode){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioCallback::getInterfaceDescriptor());
            data.writeInt32(mode);
            remote()->transact(STATE_CHANGED, data, &reply);
            return;

        }
        virtual void recording_stop(int reason){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioCallback::getInterfaceDescriptor());
            data.writeInt32(reason);
            remote()->transact(RECORDING_STOP, data, &reply);
            return;

        }

};

IMPLEMENT_META_INTERFACE(FMRadioCallback, "com.marvell.fmmanager.IFMRadioCallback");

// ----------------------------------------------------------------------

status_t BnFMRadioCallback::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch(code) {
        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}

}


