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
#define LOG_TAG	"FMRadio"
#include "FMRingBuffer.h"

namespace android{
using namespace android;

FMRingBuffer::FMRingBuffer():mSize(65535){
    mHead = mWriteOff = 0;
    mExitBlock = false;
    mBuf = (char *)malloc(mSize);
    mFrameSize = 0;

    pthread_mutex_init(&mMutex, NULL);
    pthread_cond_init(&mCond, NULL);
}

FMRingBuffer::FMRingBuffer(int size){
    if (size > 0){
        mSize = size;
    } else {
        mSize = 65535;
    }
    mBuf = (char *)malloc(mSize);

    mExitBlock = false;
    mFrameSize = 0;
    pthread_mutex_init(&mMutex, NULL);
    pthread_cond_init(&mCond, NULL);
}

FMRingBuffer::~FMRingBuffer(){
    if (mBuf){
        free(mBuf);
        mBuf = NULL;
    }

    pthread_mutex_destroy(&mMutex);
    pthread_cond_destroy(&mCond);
}

/*
 *init - initialize for reader or wirter
 *   behavior: For reader: insert a element to list mReadOffs<tid, read Off> for reader
 *                  For writer: reset flag mExitBlock
 *
*/
void FMRingBuffer::init(const char *identity){
    if (!strncmp(identity, "read", strlen("read"))){
        pthread_mutex_lock(&mMutex);
        mReadOffs.add(gettid(), mHead);
        pthread_mutex_unlock(&mMutex);
    } else if (!strncmp (identity, "write", strlen("write"))){
        pthread_mutex_lock(&mMutex);
        mExitBlock = false;
        mHead = mWriteOff = 0;
        pthread_mutex_unlock(&mMutex);
    } else {
        LOGE("unrecognized identity: %s", identity);
    }

    return;
}

/*
 *uninit - uninit for reader or wirter
 *   behavior: For reader: remove element from list mReadOffs for reader
 *                  For wirter: break all blocked readers who are waiting for data
*/
void FMRingBuffer::uninit(const char *identity){
    if (!strncmp(identity, "read", strlen("read"))){
        pthread_mutex_lock(&mMutex);
        mReadOffs.removeItem(gettid());
        pthread_mutex_unlock(&mMutex);
    } else if (!strncmp (identity, "write", strlen("write"))){
        /* break all blocked readers */
        pthread_mutex_lock(&mMutex);
        mExitBlock = true;
        /* wake up all blocked readers */
        pthread_cond_broadcast(&mCond);
        pthread_mutex_unlock(&mMutex);
    } else {
        LOGE("unrecognized identity: %s", identity);
    }

    return;
}

/*
 * clock_interval - is a < c < b in mode-space? In other words, does the line from a to b
 * cross c?
 *
*/
inline int FMRingBuffer::clock_interval(int a, int b, int c){
    if (b < a){
        if (a < c || b >= c)
            return 1;
    } else {
        if (a < c && b >= c)
            return 1;
    }

    return 0;
}

/*
 * fix_up_readers - walk the list of all readers and "fix up" any who were lapped by writer;
 * also do this for the dafault "start head".
 * We do this by "pulling forward" the readers and start head to the first byte after the
 * new write head.
 *
 * the caller needs to hold mMutex
*/
void FMRingBuffer::fix_up_readers(int size){
    int oldWriteOff = mWriteOff;
    int newWriteOff = offset(oldWriteOff + size);
    int updateOff = 0;


    if (clock_interval(oldWriteOff, newWriteOff, mHead)){
        int iDis = distance(mHead, newWriteOff);

        if (iDis%mFrameSize == 0){
            updateOff = newWriteOff;
        } else {
            updateOff = offset(newWriteOff + (mFrameSize - iDis%mFrameSize));
        }

        mHead = updateOff;
    }

    for (unsigned int i = 0; i < mReadOffs.size(); i++){
        if (clock_interval(oldWriteOff, newWriteOff, mReadOffs.valueAt(i))){
            LOGW("FMRingBuffer overflow.\n");
            mReadOffs.editValueAt(i) = updateOff;
        }
    }

    return;
}


/*
 * get_data_len - Grabs the length of the data avaliable for read starting from readoff.
 *
 * Caller needs hold mMutex.
*/
int FMRingBuffer::get_data_len(){
    int size;
    int readOff;
    int tid = gettid();

    readOff = mReadOffs.valueFor(tid);
    if (mWriteOff > mReadOffs.valueFor(tid)){
        size = mWriteOff - readOff;
    } else if (mWriteOff == mReadOffs.valueFor(tid)){
        size = 0;
    } else {
        size = mSize - (readOff - mWriteOff);
    }
    return size;
}

/*
 * do_read_data - reads from mBuf to
 *                             destination buf. Returns bytes read on success
 *
 * Caller needs hold mMutex.
*/
int FMRingBuffer::do_read_data(char *buf, int size){
    int len;
    int readOff;
    int tid = gettid();

    readOff = mReadOffs.valueFor(tid);
    len = min(size, (mSize - readOff));
    memcpy(buf, mBuf + readOff, len);

    if (size != len){
        memcpy(buf + len, mBuf, size - len);
    }

    /* update read offset */
    mReadOffs.editValueFor(tid) = offset(readOff + size);

    return size;
}

/*
 * do_write_data - write min(size, bufSize) bytes from src buffer to mBuf
 *
 * Caller needs hold mMutex.
 * Returns bytes written on success, negative error code on failure
*/
int FMRingBuffer::do_write_data(char *buf, int size){
    int len;
    int sizeWritten = min(size, mSize);

    len = min(sizeWritten, mSize - mWriteOff);
    memcpy(mBuf + mWriteOff, buf, len);

    if (sizeWritten != len)
        memcpy(mBuf, buf + len, sizeWritten - len);

    mWriteOff = offset(mWriteOff + sizeWritten);
    return sizeWritten;
}

/*
 ** read -- the ringbuffer read() method
 ** Behavior:
 **          - unblock mode support, set isBlock = false
 **          - in block mode, if there are no data to read, block until data is written to
 ** Return:
 **          0, work in block mode, it means no data available and
 **                                                read thread is request to break the block
 **                                                by setting mExitBlock to true.
 **         -1, work in unblock mode, it means no data available.
 **         >0, read successfully, return read buffer size.
 */
int FMRingBuffer::read(char *buf, int count, bool isBlock){
    int ret;

    while (1){
        /* Check data available */
        pthread_mutex_lock(&mMutex);
        if (get_data_len()){
            ret = do_read_data(buf, min(count, get_data_len()));
            pthread_mutex_unlock(&mMutex);
            return ret;
        }

        /* No data available && unblock mode,  return -1*/
        if (isBlock == false){
            pthread_mutex_unlock(&mMutex);
            return -1;
        }

        /* No data avalialbe && block mode && request exit, return 0 */
        if (mExitBlock == true){
            pthread_mutex_unlock(&mMutex);
            return 0;
        }

        /* Block waiting for new data */
        while ((0 == get_data_len()) && !mExitBlock){
            pthread_cond_wait(&mCond, &mMutex);
        }
        pthread_mutex_unlock(&mMutex);
    }
}

/*
 * write - write() method, return 0 once write fail, return written size on success.
 */
int FMRingBuffer::write(char *buf, int size){
    if (size <= 0){
        return 0;
    }

    /*
        ** As the policy is: writer wouldn't wait the ring buffer being read out,
        ** it just cover the unread buffer. Make sure the buffer written size not
        ** surpass half of ring buffer to make the policy fairer to readers.
        */
    int sizeWritten = min(size, mSize/2);

    pthread_mutex_lock(&mMutex);
    fix_up_readers(sizeWritten);
    do_write_data(buf, sizeWritten);
    /* wake up all blocked readers */
    pthread_cond_broadcast(&mCond);
    pthread_mutex_unlock(&mMutex);

    return sizeWritten;
}

/*
 * setFrameSize - set framesize used to update reader offset.
 */
bool FMRingBuffer::setFrameSize(int size){
    if (size > mSize)
        return false;

    mFrameSize = size;
    return true;
}

}
