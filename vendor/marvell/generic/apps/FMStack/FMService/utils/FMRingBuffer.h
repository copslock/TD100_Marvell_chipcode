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

#ifndef ANDROID_FMRINGBUFFER_H
#define ANDROID_FMRINGBUFFER_H
#include <utils/Log.h>
#include <utils/KeyedVector.h>
#include <pthread.h>

namespace android {

class FMRingBuffer{
private:
    int get_data_len();
    int do_read_data(char *, int);
    int do_write_data(char *, int);
    void fix_up_readers(int);
    int offset(int n){ return n%mSize;}
    int min(int a, int b){ return a > b ? b : a;}
    int distance(int a, int b){ return (a <= b) ? (b - a) : (mSize - a + b);}
    int clock_interval(int a, int b, int c);
    int mSize;
    KeyedVector<int, int> mReadOffs;
    pthread_mutex_t mMutex;
    pthread_cond_t mCond;
    int mWriteOff;
    int mHead;
    char *mBuf;
    bool mExitBlock;
    bool mWriteInit;
    int mFdRecordFile;
    int mFrameSize;

public:
    FMRingBuffer();
    FMRingBuffer(int size);
    ~FMRingBuffer();

    void init(const char* identity);
    void uninit(const char* identity);
    int read(char *buf, int count, bool isBlock);
    int write(char *buf, int count);
    bool setFrameSize(int size);
};

}

#endif
