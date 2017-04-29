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

#include <stdint.h>
#include <sys/types.h>

#include <binder/Parcel.h>
#include <binder/IMemory.h>
#include "IFMRadioService.h"

#include <utils/Errors.h>  // for status_t
#include "IFMRadioCallback.h"
namespace android {

enum {
    ISFMTX = IBinder::FIRST_CALL_TRANSACTION,
    ISFMRX,
    POWERON,
    POWEROFF,
    ENABLETX,
    DISABLETX,
    ENABLERX,
    DISABLERX,
    SUSPEND,
    RESUME,
    SCAN_ALL,
    STOP_SCAN,
    SET_CHANNEL,
    GET_CHANNEL,
    GET_RSSI,
    GET_VOLUME,
    SET_VOLUME,
    SET_MUTE,
    SET_BAND,
    SCAN_NEXT,
    SCAN_PREV,
    SET_SPEAKER_ON,
    SET_TX_MUTE,
    SET_POWER_MODE,
    SET_TX_MONO_STEREO,
    START_RECORDING,
    STOP_RECORDING,
    GET_RECORD_FORMAT,
    REGISTER_CALLBACK,
    UNREGISTER_CALLBACK,
};

class BpFMRadioService: public BpInterface<IFMRadioService>
{
    public:
        BpFMRadioService(const sp<IBinder>& impl)
            : BpInterface<IFMRadioService>(impl)
        {
        }

        virtual bool isFMTx(){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            remote()->transact(ISFMTX, data, &reply);
            return reply.readInt32();
        }

        virtual bool isFMRx(){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            remote()->transact(ISFMTX, data, &reply);
            return reply.readInt32();
        }
        virtual int powerOn(){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            remote()->transact(POWERON, data, &reply);
            return reply.readInt32();
        }
        virtual int powerOff(){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            remote()->transact(POWEROFF, data, &reply);
            return reply.readInt32();
        }
        virtual int enableTx(){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            remote()->transact(ENABLETX, data, &reply);
            return reply.readInt32();
        }
        virtual int disableTx(){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            remote()->transact(DISABLETX, data, &reply);
            return reply.readInt32();
        }
        virtual int enableRx(){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            remote()->transact(ENABLERX, data, &reply);
            return reply.readInt32();
        }
        virtual int disableRx(){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            remote()->transact(DISABLERX, data, &reply);
            return reply.readInt32();
        }
        virtual int suspend(){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            remote()->transact(SUSPEND, data, &reply);
            return reply.readInt32();
        }
        virtual int resume(){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            remote()->transact(RESUME, data, &reply);
            return reply.readInt32();
        }
        virtual int scan_all(){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            remote()->transact(SCAN_ALL, data, &reply);
            return reply.readInt32();
        }
        virtual int stop_scan(){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            remote()->transact(STOP_SCAN, data, &reply);
            return reply.readInt32();
        }
        virtual int set_channel(unsigned int freq){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            data.writeInt32(freq);
            remote()->transact(SET_CHANNEL, data, &reply);
            return reply.readInt32();
        }
        virtual int get_channel(){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            remote()->transact(GET_CHANNEL, data, &reply);
            return reply.readInt32();
        }
        virtual int get_rssi(){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            remote()->transact(GET_RSSI, data, &reply);
            return reply.readInt32();
        }
        virtual float get_volume(){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            remote()->transact(GET_VOLUME, data, &reply);
            return reply.readInt32();
        }
        virtual int set_volume(int volume){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            data.writeInt32(volume);
            remote()->transact(SET_VOLUME, data, &reply);
            return reply.readInt32();
        }
        virtual bool set_mute(bool flag){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            data.writeInt32(flag);
            remote()->transact(SET_MUTE, data, &reply);
            return reply.readInt32();
        }
        virtual int set_band(int band){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            data.writeInt32(band);
            remote()->transact(SET_BAND, data, &reply);
            return reply.readInt32();
        }
        virtual int scan_next(){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            remote()->transact(SCAN_NEXT, data, &reply);
            return reply.readInt32();
        }
        virtual int scan_prev(){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            remote()->transact(SCAN_PREV, data, &reply);
            return reply.readInt32();
        }
        virtual int setSpeakerOn(bool on){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            data.writeInt32(on);
            remote()->transact(SET_SPEAKER_ON, data, &reply);
            return reply.readInt32();
        }
        virtual bool set_tx_mute(bool flag){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            data.writeInt32(flag);
            remote()->transact(SET_TX_MUTE, data, &reply);
            return reply.readInt32();
        }
        virtual bool set_power_mode(bool flag){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            data.writeInt32(flag);
            remote()->transact(SET_POWER_MODE, data, &reply);
            return reply.readInt32();
        }
        virtual bool set_tx_mono_stereo(bool flag){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            data.writeInt32(flag);
            remote()->transact(SET_TX_MONO_STEREO, data, &reply);
            return reply.readInt32();
        }
        virtual bool start_recording(int fd){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            data.writeInt32(fd);
            remote()->transact(START_RECORDING, data, &reply);
            return true;
        }
        virtual bool stop_recording(){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            remote()->transact(STOP_RECORDING, data, &reply);
            return true;;
        }
        virtual void get_record_format(char **format){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            remote()->transact(GET_RECORD_FORMAT, data, &reply);
            return;
        }
        virtual int registerCallback(const sp<IFMRadioCallback>& callback){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            data.writeStrongBinder(callback->asBinder());
            remote()->transact(REGISTER_CALLBACK, data, &reply);
            return 0;     
        }
        virtual int unregisterCallback(const sp<IFMRadioCallback>& callback){
            Parcel data, reply;
            data.writeInterfaceToken(IFMRadioService::getInterfaceDescriptor());
            data.writeStrongBinder(callback->asBinder());
            remote()->transact(UNREGISTER_CALLBACK, data, &reply);
            return 0;
        }

};

//Notice, here the name should be same as IAdditionService.java -- "com.Addition.IFMRadioService"
IMPLEMENT_META_INTERFACE(FMRadioService, "com.marvell.fmmanager.IFMRadioService");

// ----------------------------------------------------------------------

status_t BnFMRadioService::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch(code) {
        case ISFMTX: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int result = isFMTx();
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case ISFMRX: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int result = isFMRx();
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case POWERON: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int result = powerOn();
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case POWEROFF: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int result = powerOff();
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case ENABLETX: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int result = enableTx();
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case DISABLETX: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int result = disableTx();
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case ENABLERX: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int result = enableRx();
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case DISABLERX: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int result = disableRx();
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case SUSPEND: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int result = suspend();
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case RESUME: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int result = resume();
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case SCAN_ALL: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int result = scan_all();
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case STOP_SCAN: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int result = stop_scan();
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case SET_CHANNEL: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int32_t freq = data.readInt32();

            int result = set_channel(freq);
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case GET_CHANNEL: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int result = get_channel();
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case GET_RSSI: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int result = get_rssi();
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case GET_VOLUME: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int result = get_volume();
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case SET_VOLUME: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int32_t volume = data.readInt32();
            int result = set_volume(volume);
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case SET_MUTE: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int32_t flag = data.readInt32();
            int result = set_mute(flag);
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case SET_BAND: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int32_t band = data.readInt32();
            int result = set_band(band);
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case SCAN_NEXT: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int result = scan_next();
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case SCAN_PREV: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int result = scan_prev();
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case SET_SPEAKER_ON: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int32_t flag = data.readInt32();
            int result = setSpeakerOn(flag);
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case SET_TX_MUTE: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int32_t flag = data.readInt32();
            int result = set_tx_mute(flag);
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case SET_POWER_MODE: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int32_t flag = data.readInt32();
            int result = set_power_mode(flag);
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case SET_TX_MONO_STEREO: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int32_t flag = data.readInt32();
            int result = set_tx_mono_stereo(flag);
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case START_RECORDING: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            data.readInt32();
            int32_t fd = data.readFileDescriptor();
            int32_t result = start_recording(fd);
            if (fd > 0){
                close (fd);
            }
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        }break;
        case STOP_RECORDING: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            int32_t result = stop_recording();
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        }break;
        case GET_RECORD_FORMAT: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            char *format = (char *)malloc(128);
            get_record_format(&format);
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(strlen(format));
            reply->write(format, strlen(format));
            free(format);
            return NO_ERROR;
        }
        case REGISTER_CALLBACK: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            sp<IFMRadioCallback> callback = interface_cast<IFMRadioCallback>(data.readStrongBinder());
            int32_t result = registerCallback(callback);
            // write no exception
            reply->writeInt32(0);
            reply->writeInt32(result);
            return NO_ERROR;
        } break;
        case UNREGISTER_CALLBACK: {
            CHECK_INTERFACE(IFMRadioService, data, reply);
            sp<IFMRadioCallback> callback = interface_cast<IFMRadioCallback>(data.readStrongBinder());
            int32_t result = unregisterCallback(callback);
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


