/*
 * Copyright (C) 2011 Marvell International Ltd., All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <unistd.h>
#include <android_runtime/ActivityManager.h>
#include <binder/IBinder.h>
#include <binder/IServiceManager.h>
#include <binder/Parcel.h>
#include <utils/String8.h>


using namespace android;

static const int BROADCAST_INTENT_TRANSACTION = IBinder::FIRST_CALL_TRANSACTION+13;

static const int URI_TYPE_ID = 1;
static const int NULL_TYPE_ID = 0;

static const int VAL_NULL = -1;
static const int VAL_STRING = 0;
static const int VAL_INTEGER = 1;

static const int START_SUCCESS = 0;

extern "C" int send_dhcp_intent(char * actionDhcp,char * macAddr,char * ipAddr)
{

    sp<IServiceManager> sm = defaultServiceManager();
    sp<IBinder> am = sm->checkService(String16("activity"));

    Parcel data, reply;
    data.writeInterfaceToken(String16("android.app.IActivityManager"));

    data.writeStrongBinder(NULL); /* caller */

    /* intent */
    data.writeString16(String16("android.net.conn.TETHER_LEASE_CHANGED")); /* action */
    data.writeInt32(NULL_TYPE_ID); /* Uri - type */
    data.writeString16(NULL, 0); /* type */
    data.writeInt32(0); /* flags */

    data.writeString16(NULL, 0); /* package name - DONUT ONLY, NOT IN CUPCAKE. */
    data.writeString16(NULL, 0); /* ComponentName - package */
    data.writeInt32(0); /* Categories - size */
    data.writeInt32(0);

    { /* Extras */
        data.writeInt32(-1); /* dummy, will hold length */
        int oldPos = data.dataPosition();
        data.writeInt32(0x4C444E42); // 'B' 'N' 'D' 'L'
        { /* writeMapInternal */
            data.writeInt32(4); /* writeMapInternal - size */

            data.writeInt32(VAL_STRING);
            data.writeString16(String16("dhcpEvent"));
            data.writeInt32(VAL_STRING);
            data.writeString16(String16(actionDhcp));

            data.writeInt32(VAL_STRING);
            data.writeString16(String16("dhcpMac"));
            data.writeInt32(VAL_STRING);
            data.writeString16(String16(macAddr));

            data.writeInt32(VAL_STRING);
            data.writeString16(String16("dhcpIp"));
            data.writeInt32(VAL_STRING);
            data.writeString16(String16(ipAddr));

        }
        int newPos = data.dataPosition();
        data.setDataPosition(oldPos - 4);
        data.writeInt32(newPos - oldPos); /* length */
        data.setDataPosition(newPos);
    }

    data.writeString16(NULL, 0); /* resolvedType */

    data.writeInt32(-1); /* grantedUriPermissions */
    data.writeInt32(0); /* grantedMode */

    data.writeStrongBinder(NULL); /* resultTo */
    data.writeString16(NULL, 0); /* resultWho */
    data.writeInt32(-1); /* requestCode */
    data.writeInt32(false); /* onlyIfNeeded */
    data.writeInt32(false); /* debug */

    status_t ret = am->transact(BROADCAST_INTENT_TRANSACTION, data, &reply);

    if (ret < START_SUCCESS) return -1;

    return 0;
}
