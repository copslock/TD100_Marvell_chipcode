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

#ifndef ANDROID_IFMRADIOSERVICE_H
#define ANDROID_IFMRADIOSERVICE_H

#include <utils/Errors.h>  // for status_t
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include "IFMRadioCallback.h"

namespace android {

class IFMRadioService: public IInterface
{
public:
    DECLARE_META_INTERFACE(FMRadioService);
    virtual bool isFMTx() = 0;
    virtual bool isFMRx() = 0;
    virtual int powerOn() = 0;
    virtual int powerOff() = 0;
    virtual int enableTx() = 0;
    virtual int disableTx() = 0;
    virtual int enableRx() = 0;
    virtual int disableRx() = 0;
    virtual int suspend() = 0;
    virtual int resume() = 0;
    virtual int scan_all() = 0;
    virtual int stop_scan() = 0;
    virtual int set_channel(unsigned int freq) = 0;
    virtual int get_channel() = 0;
    virtual int get_rssi() = 0;
    virtual float get_volume() = 0;
    virtual int set_volume(int volume) = 0;
    virtual bool set_mute(bool flag) = 0;
    virtual int set_band(int band) = 0;
    virtual int scan_next() = 0;
    virtual int scan_prev() = 0;
    virtual int setSpeakerOn(bool on) = 0;
    virtual bool set_tx_mute(bool flag) = 0;
    virtual bool set_power_mode(bool flag) = 0;
    virtual bool set_tx_mono_stereo(bool flag) = 0;
    virtual bool start_recording(int fd) = 0;
    virtual bool stop_recording() = 0;
    virtual void get_record_format(char **format) = 0;
    virtual int registerCallback(const sp<IFMRadioCallback>& callback) = 0;
    virtual int unregisterCallback(const sp<IFMRadioCallback>& callback) = 0;

};

// ----------------------------------------------------------------------------

class BnFMRadioService: public BnInterface<IFMRadioService>
{
public:
    virtual status_t    onTransact( uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags = 0);
};

}; // namespace android

#endif // ANDROID_IADDITIONSERVICE_H


