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

#ifndef ANDROID_IFMRADIOCALLBACK_H
#define ANDROID_IFMRADIOCALLBACK_H

#include <utils/Errors.h>  // for status_t
#include <binder/IInterface.h>
#include <binder/Parcel.h>

namespace android {


class IFMRadioCallback: public IInterface
{
public:
    DECLARE_META_INTERFACE(FMRadioCallback);
    virtual void scan_finshed() = 0;
    virtual void found_channel(int channel) = 0;
    virtual void mono_changed(int mono) = 0;
    virtual void get_rdsps_name(char *name) = 0;
    virtual void get_cur_rssi(int rssi) = 0;
    virtual void state_changed(int mode) = 0;
    virtual void recording_stop(int reason) = 0;
};

// ----------------------------------------------------------------------------

class BnFMRadioCallback: public BnInterface<IFMRadioCallback>
{
public:
    virtual status_t    onTransact( uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags = 0);
};

}; // namespace android

#endif // ANDROID_IFMRADIOCALLBACK_H


