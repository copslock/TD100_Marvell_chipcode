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
#include "FMRadio.h"
#include "FMEventParser.h"
#include <binder/IServiceManager.h>
extern "C"{
#include <sys/prctl.h>
#include <linux/capability.h>
#include <private/android_filesystem_config.h>
#include <linux/capability.h>
#include "marvell_wireless.h"
#include "hardware_legacy/power.h"
#define FM_WAKELOCK "FMService"
}
//#define STREAM_DUMP_DEBUG
#ifdef STREAM_DUMP_DEBUG
#define FMRECORD_PCM_LOC "/data/fmRecord_debug.pcm"
#define FMPLAY_PCM_LOC "/data/fmPlay_debug.pcm"
#endif

#ifdef BOARD_USES_SANREMO
#include <media/AudioSystem.h>
#endif

// Define for events and commands
#define S_EVENT_SCAN_FINISHED "eventScanFinished"
#define S_EVENT_FOUND_CHANNEL "eventFoundChannel"
#define S_CMD_START "cmdStart"
#define S_CMD_SUSPEND "cmdSuspend"
#define S_CMD_EXIT "cmdExit"
#define S_RADIO_DEVICE "/dev/radio0"

namespace android{
using namespace android;

Mutex FMRadio::mInstanceLock;
FMRadio* FMRadio::mInstance = NULL;
#ifndef FM_NOT_USES_RECORD  //record mode
FMRingBuffer* FMRadio::mFMBuffer = NULL;
#endif
int FMRadio::mfd = -1;

#define report_recording_stop(reason) \
do{\
    AutoMutex _l(mOwner.mCallbackLock);\
    for (int i = 0, n = mOwner.mCallbacks.size(); i < n; ++i) {\
        mOwner.mCallbacks.valueAt(i)->recording_stop(reason);\
    }\
}\
while(0)

class FMRadio::EventThread : public Thread {
private:

    enum poll_fds{
        FD_CMD = 0,  // command pipe
        FD_DEVICE,   // radio device
        FD_NUM
    };
    // command status
    enum cmd_excecuted_status{
        CMD_STATUS_NOT_EXECTED = 0,  // has not been executed
        CMD_STATUS_SUCCESS = 1,  // success
        CMD_STATUS_FAIL = -1,  // fail
    };

    enum thread_status{
        T_STATUS_SUSPEND,   // thread is waiting for commands "start" or "exit"
        T_STATUS_START,     // thread is polling events
        T_STATUS_EXIT,      // thread is down
    }; 

    FMRadio& mOwner;
    int mRadioFd;      // radio device
    int mTStatus;      // thread status, refer to enum STATUS
    Mutex mLock;      // protect cmd excuted
    bool mCmdStatus;     // Cmd excuted successfully?
    Condition mCond;
    struct pollfd mPollFds[FD_NUM]; // polling fds, changed from thread status
    int mPollNum;      // Polling file numbers
    int mCmdPipe[2];   //sending commands to thread from this pipe

    bool prepareStart(){
        if (mRadioFd == -1){
            mRadioFd = dup(FMRadio::mfd);
            if (mRadioFd == -1){
                LOGE("event thread: duplicate Radio device failed.");
                LOGE("event thread: prepare Start failed.");
                return false;
            }            
        }
        // Prepare all poll file descriptors
        mPollFds[FD_CMD].fd = mCmdPipe[0];
        mPollFds[FD_CMD].events = POLLIN;
        mPollFds[FD_CMD].revents = 0;
        mPollFds[FD_DEVICE].fd = mRadioFd;
        mPollFds[FD_DEVICE].events = POLLIN;
        mPollFds[FD_DEVICE].revents = 0;
        mPollNum = FD_NUM;
        LOGI("event thread: prepare Start succeed.");
        return true;
    }

    bool prepareSuspend(){
        // Prepare command pipe
        if (mCmdPipe[0] < 0){
            LOGE("Command pipe was not opened.");
            LOGE("event thread: prepare Suspend failed.");
            return false;
        }

        if (mRadioFd > 0){
            close(mRadioFd);
            mRadioFd = -1;
        }
        mPollFds[FD_CMD].fd = mCmdPipe[0];
        mPollFds[FD_CMD].events = POLLIN;
        mPollFds[FD_CMD].revents = 0;
        mPollNum = 1;
        LOGI("event thread: prepare Suspend succeed.");
        return true;
    }

    bool prepareExit(){
        if (mRadioFd > 0){
            close(mRadioFd);
            mRadioFd = -1;
        }
        LOGI("event thread: prepare Eixt succeed.");
        return true;
    }    

public:
    EventThread(FMRadio& fm): Thread(false), mOwner(fm) {
        // Initialize class members 
        mRadioFd = -1;
        mTStatus = T_STATUS_EXIT;
        mPollFds[FD_CMD].fd = -1;
        mPollFds[FD_CMD].events = POLLIN;
        mPollFds[FD_CMD].revents = 0;
        mPollFds[FD_DEVICE].fd = -1;
        mPollFds[FD_DEVICE].events = POLLIN;
        mPollFds[FD_DEVICE].revents = 0;
        mPollNum = 0;
        mCmdStatus = CMD_STATUS_NOT_EXECTED;

        if (pipe(mCmdPipe) == -1) {
            LOGE("create pipe of command pipe error.");
        }
    }

    ~EventThread(){
        if (mCmdPipe[0])
            close(mCmdPipe[0]);
        if (mCmdPipe[1])
            close(mCmdPipe[1]);
    }

    int getThreadStatus(){
        return mTStatus;
    }
    
    /*
    **  @Description
    ** This API is for sending thread control commands and event commands.
    ** Parameter cmd includes:
    ** "cmdStart", "cmdSuspend", "cmdExit", "eventScanFinished" and 
    ** "eventFoundChannel + frequency".
    ** Parameter expectedStatus is the expected thread status after 
    ** thread control commands successfully exectued. If  cmd is a event 
    ** commands,  expectedStatus should be -1.   
    ** @Return values
    ** true Success
    ** false Fail
    */
    bool sendCmd(const char *cmd, int expectedStatus){
        if (!cmd){
            LOGE("input parameter cmd is NULL.");
            return false;
        }
        LOGI("Main thread: send cmd %s to Event thread", cmd);
        
        if (expectedStatus == -1){
            // event commands, just return
            write(mCmdPipe[1], cmd, strlen(cmd));
            return true;
        }else{
            mLock.lock();
            write(mCmdPipe[1], cmd, strlen(cmd));
        }
        
        mCmdStatus = CMD_STATUS_NOT_EXECTED;
        /*
        ** Wait for thread feedback, mCmdStatus = -1, executed fail.
        ** mCmdStatus  = 1, successful. mCmdStatus = 0, has not been executed.
        */
        LOGI("Main thread: wait signal from Event thread to wake up.");
        while (mCmdStatus == CMD_STATUS_NOT_EXECTED) {
            mCond.wait(mLock);
        }
        mLock.unlock();
        LOGI("Main thread: waked up!.");

        if (mCmdStatus == CMD_STATUS_FAIL){
            // command excuted fail
            LOGE("Command excuted fail.");
            return false;
        }

        assert(mTStatus == expectedStatus);
        
        return true;
    }

    void respondCmdStatus(int value){
        mLock.lock();
        mCmdStatus = value;
        mCond.signal();
        mLock.unlock();
        LOGI("event thread: sent signal to wake up main thread.");
        return;
    }

    bool requireStart(){
        // Check thread status
        if (mTStatus == T_STATUS_START){
            LOGI("Mainthread: Event thread is already started.");
            return true;
        }

        // Start the thread firstly if thread already exit
        if (mTStatus == T_STATUS_EXIT){
            run("FMEvent", ANDROID_PRIORITY_NORMAL);
        }
        
        // Send request start command to Event Thread 
        if (sendCmd(S_CMD_START, T_STATUS_START)){
            LOGI("Mainthread: Event thread start succeed.");
            return true;
        }else{
            LOGE("Mainthread: Event thread start failed.");
            return false;
        }
    }

    bool requireSuspend(){
        // Check thread status
        if (mTStatus == T_STATUS_SUSPEND){
            LOGI("Mainthread: Event thread is already Suspended.");
            return true;
        }

        // Start the thread firstly (thread will be suspend) if thread already exit, 
        if (mTStatus == T_STATUS_EXIT){
            run("FMEvent", ANDROID_PRIORITY_NORMAL);
            return true;
        }
         
        // Send request suspend command to Event Thread
        if (sendCmd(S_CMD_SUSPEND, T_STATUS_SUSPEND)){
            LOGI("Mainthread: Event thread suspend succeed.");
            return true;
        }else{
            LOGE("Mainthread: Event thread suspend failed.");
            return false;
        }
    }

    bool requireExit(){
        // Check thread status
        if (mTStatus == T_STATUS_EXIT){
            LOGI("Mainthread: Event thread is already exit.");
            return true;
        }
        // keep a strong ref on ourself so that we wont get
        // destroyed in the middle of requireExit()
        // Must not delete mEventThread instance since the
        // destroying of strongMe would do that.
        sp <Thread> strongMe = this;
        
        // Send request exit command to Event Thread 
        if (sendCmd(S_CMD_EXIT, T_STATUS_EXIT)){
            LOGI("Mainthread: Event thread exit succeed.");
            return true;
        }else{
            LOGE("Mainthread: Event thread exit failed.");
            return false;
        }
    }
    
    virtual bool threadLoop() {
        int fd_type = -1;
        int size = 0, index = 0;
        int ret = false;
        char buf[256];
        FmEventParser parser;

        prepareSuspend();
        mTStatus = T_STATUS_SUSPEND;  
        LOGI("Event thread is suspended.");

        while(1) {   
            // poll
POLL:       int rt = poll(mPollFds, mPollNum, -1);
            if (rt < 0) {
                LOGE("poll failed[%s]\n", strerror(errno));
                break;
            }
            for (int i = 0; i < mPollNum; i++) {
                if (!(mPollFds[i].revents & POLLIN)) {
                    if (mPollFds[i].revents) {
                        LOGE("revents[%08x]\n", mPollFds[i].revents);
                        return 0;
                    }
                    mPollFds[i].revents = 0;
                    continue;
                }

                mPollFds[i].revents = 0;
                fd_type = i;
                size = read(mPollFds[i].fd, buf, sizeof(buf));
                if (size < 0){
                    LOGE("Event thread read fail, ret %d, strerror: %s", size, strerror(errno));
                    continue;
                }
                buf[size] = 0;                
                switch (fd_type) {
                case FD_CMD:
                    index = 0;
                    while(index < size){   
                        if (strncmp(&buf[index], S_EVENT_SCAN_FINISHED, 
                                        strlen(S_EVENT_SCAN_FINISHED)) == 0){
                            AutoMutex _l(mOwner.mCallbackLock);
                            for (int i = 0, n = mOwner.mCallbacks.size(); i < n; ++i) {
                                mOwner.mCallbacks.valueAt(i)->scan_finshed();
                            } 
                            //mOwner.mCallback->scan_finshed();
                            index += strlen(S_EVENT_SCAN_FINISHED);
                        }
                        else if(strncmp(&buf[index], S_EVENT_FOUND_CHANNEL, 
                                    strlen(S_EVENT_FOUND_CHANNEL)) == 0){
                            char param[64];
                            char temp[64];
                            int freq = 0;
                            sprintf(param, "%s", S_EVENT_FOUND_CHANNEL);
                            strcat(param, " %d");
                            sscanf(buf, param, &freq);
                            AutoMutex _l(mOwner.mCallbackLock);
                            for (int i = 0, n = mOwner.mCallbacks.size(); i < n; ++i) {
                                mOwner.mCallbacks.valueAt(i)->found_channel(freq);
                            }                            
                            //mOwner.mCallback->found_channel(freq);
                            sprintf(temp, "%d", freq);
                            index += strlen(S_EVENT_FOUND_CHANNEL) + 
                                strlen(temp) + 1;
                        }
                        else if (strncmp(buf, S_CMD_START, strlen(S_CMD_START)) == 0){
                            if (prepareStart() == false){
                                //Prepared failed, will exit
                                prepareExit();
                                mTStatus = T_STATUS_EXIT;
                                respondCmdStatus(CMD_STATUS_FAIL);
                                // Should reture false so that thread really exit. refer to threads.h                           
                                return false;
                            } else{
                                //Thread is now start to poll radio device
                                mTStatus = T_STATUS_START;
                                respondCmdStatus(CMD_STATUS_SUCCESS); 
                                goto POLL;  // goto poll
                            }
                        }else if (strncmp(buf, S_CMD_SUSPEND, strlen(S_CMD_SUSPEND)) == 0){
                            if (prepareSuspend() == false){
                                //Prepared failed, will exit
                                prepareExit();
                                mTStatus = T_STATUS_EXIT;
                                respondCmdStatus(CMD_STATUS_FAIL);                         
                                return false;
                            } else{
                                //Thread is now suspend
                                mTStatus = T_STATUS_SUSPEND;
                                respondCmdStatus(CMD_STATUS_SUCCESS);
                                goto POLL;  // goto poll
                            }
                        }else if (strncmp(buf, S_CMD_EXIT, strlen(S_CMD_EXIT)) == 0){
                            prepareExit();
                            mTStatus = T_STATUS_EXIT;                            
                            respondCmdStatus(CMD_STATUS_SUCCESS);
                            return false;
                        }else{
                            // break while loop if command is not recognized
                            break;
                        }                               
                    }
                    break;

                case FD_DEVICE:
                    ret = parser.parse(buf, size);
                    if (ret) {
                        int event_type = parser.getType();
                        switch(event_type){
                            case FmEventParser::MONO_STATUS:
                                {
                                AutoMutex _l(mOwner.mCallbackLock);
                                for (int i = 0, n = mOwner.mCallbacks.size(); i < n; ++i) {
                                    mOwner.mCallbacks.valueAt(i)->mono_changed(parser.getMonoStatus());
                                }
                                }
                                break;

                            case FmEventParser::PRG_SRV_NAME:
                                {
                                AutoMutex _l(mOwner.mCallbackLock);
                                for (int i = 0, n = mOwner.mCallbacks.size(); i < n; ++i) {
                                    mOwner.mCallbacks.valueAt(i)->get_rdsps_name(parser.getPsName());
                                } 
                                }
                                break;

                            case FmEventParser::CUR_RSSI:
                                {
                                AutoMutex _l(mOwner.mCallbackLock);
                                for (int i = 0, n = mOwner.mCallbacks.size(); i < n; ++i) {
                                    mOwner.mCallbacks.valueAt(i)->get_cur_rssi(parser.getRssi());
                                } 
                                }
                                break;
                        }
                    }
                    break;
                }//switch()
            }//for()
        }//while(1)

        return true;
    }

 
};

#ifndef FM_NOT_USES_RECORD  //record mode
class FMRadio::FMMediaRecordThread{
private:
    FMRadio& mOwner;
    int mFd;
    bool mStopByUser;
    pthread_t mThread;
    bool mRunning;
    Mutex mLock;
    Condition       mThreadExitedCondition;
#ifdef STREAM_DUMP_DEBUG
    int mFdRecordFile;
#endif


public:

    FMMediaRecordThread(FMRadio& fm)
    :   mOwner(fm),
        mFd(-1),
        mStopByUser(false),
        mRunning(false)
    {
    }

    ~FMMediaRecordThread(){
    }

    bool start(int fd){
        LOGI("FMRadio::FMMediaRecordThread::start() fd = %d", fd);
        mStopByUser = false;
        mFd = dup(fd);

        if (mRunning){
            LOGE("mediaRecordThread is already running.!");
            return false;
        }

        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        pthread_create(&mThread, &attr, ThreadWrapper, this);
        pthread_attr_destroy(&attr);
        return true;
    }

    bool stop(){
        LOGI("FMRadio::FMMediaRecordThread::stop()");
        mStopByUser = true;
        CHECK(mThread != pthread_self());
        pthread_join(mThread, NULL);
        return true;
    }
private:
    static void *ThreadWrapper(void *me){
        static_cast<FMMediaRecordThread *>(me)->threadEntry();
        return NULL;
    }

    bool threadEntry() {
        IppAACEncoderConfig        encoderConfig;
        IppSound        sound;
        IppBitstream    bitStream;
        unsigned char   *pOutputBuf = NULL; // output buffer
        int iResult;
        MiscGeneralCallbackTable    *pCallBackTable = NULL;
        void *pEncoderState = NULL;
        bool iRet = true;
        unsigned char *pPrevBsCurByte = NULL;
        char *pInputBuf = NULL;
        int iFrameSize = AAC_FRAME_SIZE * 2 * 2;

        sound.sndChannelNum = 2;
        sound.sndLen = iFrameSize;

        /* Initialize ringbuffer */
        FMRadio::mFMBuffer->init("read");

        /* Allocate the input buffer */
        pInputBuf = (char *)malloc(iFrameSize);

        /* Allocate the output buffer */
        bitStream.bsByteLen = iFrameSize;
        IPP_MemMalloc((void **)&pOutputBuf, bitStream.bsByteLen, 4);
        if(NULL == pOutputBuf) {
            LOGE("IPP Error:malloc DstBitStream failed\n");
            report_recording_stop(INTERNAL_ERROR);
            iRet = false;
            goto FAIL1;
        }
        IPP_Memset(pOutputBuf,0,bitStream.bsByteLen);
        bitStream.pBsBuffer = pOutputBuf;
        bitStream.pBsCurByte = pOutputBuf;
        bitStream.bsCurBitOffset = 0;

        /* initialize configuration structure */
        encoderConfig.bitRate = 96000;
        encoderConfig.bsFormat = AAC_SF_MP4ADTS;
        encoderConfig.channelConfiguration = 2;
        encoderConfig.samplingRate = 48000;
        /* Init callback table */
        if (miscInitGeneralCallbackTable(&pCallBackTable) != 0 ) {
            LOGE("IPP Error:init callback table failed\n");
            report_recording_stop(INTERNAL_ERROR);
            iRet = false;
            goto FAIL2;
        }
        /* Initialize AAC LC encoder */
        iResult = EncoderInitAlloc_AAC(&encoderConfig, pCallBackTable, &pEncoderState);
        if(iResult != IPP_STATUS_INIT_OK) {
            LOGE("IPP Error: AAC LC encoder initialization failed !!!\n");
            report_recording_stop(INTERNAL_ERROR);
            iRet = false;
            goto FAIL3;

        }
        // Thread start successfully
        mRunning = true;
        iResult = IPP_STATUS_NOERR;
        while(1){
            // wait for next frame that ready for record
            if (iResult == IPP_STATUS_NOERR || iResult == IPP_STATUS_FRAME_COMPLETE){
                int iRet = 0;
                if (mStopByUser){
                    LOGE("FM FMMediaRecordThread stopped by user.");
                    iRet = false;
                    goto FAIL3;
                }
                // Read until one FrameSize
                int bytesRead = 0;
                while (bytesRead < iFrameSize){
                    bytesRead += FMRadio::mFMBuffer->read(pInputBuf + bytesRead, iFrameSize - bytesRead, true);
                    if (mStopByUser){
                        LOGE("FM FMMediaRecordThread stopped by user.");
                        iRet = false;
                        goto FAIL3;
                    }
                }
                sound.pSndFrame = (Ipp16s*)pInputBuf;
            }
            // Encode
            pPrevBsCurByte = bitStream.pBsCurByte;
            iResult = Encode_AAC(&sound, &bitStream, pEncoderState);

            switch (iResult)
            {
                case IPP_STATUS_FRAME_COMPLETE:
                case IPP_STATUS_NOERR:
                    // encode success, write to file
                    if(mFd > 0) {
                        write(mFd, pPrevBsCurByte, bitStream.pBsCurByte-pPrevBsCurByte);
                    }
                    break;
                case IPP_STATUS_OUTPUT_DATA:
                    // output buffer is full, update pointer
                    bitStream.pBsCurByte = bitStream.pBsBuffer;
                    bitStream.bsCurBitOffset = 0;
                    break;
                default:
                    // Encode error
                    report_recording_stop(INTERNAL_ERROR);
                    iRet = false;
                    LOGE("IPP media record failed.");
                    goto FAIL3;
                    break;
            }
        }
FAIL3:
        EncoderFree_AAC(&pEncoderState);
FAIL2:
        miscFreeGeneralCallbackTable(&pCallBackTable);
FAIL1:
        if (NULL != pOutputBuf){
            IPP_MemFree((void **)&pOutputBuf);
            pOutputBuf = NULL;
        }
        if (NULL != pInputBuf){
            free(pInputBuf);
            pInputBuf = NULL;
        }
        mRunning = false;
        FMRadio::mFMBuffer->uninit("read");
        if (mFd > 0){
            close(mFd);
            mFd = -1;
        }

        return iRet;
    }
};

class FMRadio::FMRecordThread : public Thread {
private:
    // command types
    enum cmd_types{
        CMD_NONE = 0,  // No requirment
        CMD_SUSPEND,   // Require thread to suspend
        CMD_START,     // Require thread to start
        CMD_EXIT,      // Require thread to exit
    };

    // command status
    enum cmd_excecuted_status{
        CMD_STATUS_NOT_EXECTED = 0,  // has not been executed
        CMD_STATUS_SUCCESS = 1,  // success
        CMD_STATUS_FAIL = -1,  // fail
    };

    enum thread_status{
        T_STATUS_SUSPEND,   // thread is waiting for commands "start" or "exit"
        T_STATUS_START,     // thread is polling events
        T_STATUS_EXIT,      // thread is down
    }; 

    FMRadio& mOwner;
    AudioRecord *mAudioRecord; //Record FM data    
    Mutex mLockCmdStatus;    //Protect mCmdStatus
    Condition mCondCmdExctd;// Wait for condtion (mCmdStatus!=CMD_NOT_EXECTED)
    Mutex mLockSendCmd;     //Protect mSendCmd
    Condition mCondSendCmd; // Wait for condtion (mSendCmd!=REQUIRE_NONE)
    int mSendCmd;           // cmd to ctrol thread's status, see enum cmd_types{}
    int mCmdStatus;         // Command was executed?
    int mTStatus;           // Current thread's status, see enum thread_status{}
    char *mBuf;
#ifdef STREAM_DUMP_DEBUG
    int mFdRecordFile;
#endif
    bool prepareStart(){
        if (mAudioRecord == NULL){
            mAudioRecord = new AudioRecord(
                    AUDIO_SOURCE_FMRADIO,
                    SAMPLE_RATE_RECORD,
                    AudioSystem::PCM_16_BIT,
                    AudioSystem::CHANNEL_IN_STEREO,
                    CHUNK_SIZE * 10
                    );
            if (mAudioRecord->initCheck() != NO_ERROR){
                LOGE("Record thread: prepare Start failed.");
                return false;
            }
            mAudioRecord->start();
        }
        LOGI("Record thread: prepare Start succeed.");
        return true;       
    }
    
    bool prepareSuspend(){
        if (mAudioRecord){
            mAudioRecord->stop();
            delete mAudioRecord;
            mAudioRecord = NULL;
        }
        LOGI("Record thread: prepare Suspend succeed.");
        return true;
    }

    bool prepareExit(){
        if (mAudioRecord){
            mAudioRecord->stop();
            delete mAudioRecord;
            mAudioRecord = NULL;
        }
        LOGI("Record thread: prepare Exit succeed.");
        return true; 
    }
    /*
    **  @Description
    ** This API is for sending thread control commands and event commands.
    ** Parameter cmd refer to enum cmd_types{}
    ** Parameter expectedStatus is the expected thread status after 
    ** thread control commands successfully exectued. 
    ** @Return values
    ** true Success
    ** false Fail
    */
    bool sendCmd(int cmd, int expectedStatus){
        mCmdStatus = CMD_STATUS_NOT_EXECTED;

        mLockCmdStatus.lock();
        mLockSendCmd.lock();
        mSendCmd = cmd;
        if (mTStatus == T_STATUS_SUSPEND){
            // Wakeup thread if thread is suspended
            mCondSendCmd.signal();
        }
        mLockSendCmd.unlock();
        LOGI("Main thread: wait for signal to wakeup.");
        while (mCmdStatus == CMD_STATUS_NOT_EXECTED) {
            mCondCmdExctd.wait(mLockCmdStatus);
        }
        mLockCmdStatus.unlock();
        LOGI("Main thread: waked up!");

        if (mCmdStatus == CMD_STATUS_FAIL){
            // command excuted fail
            LOGE("Command excuted fail.");
            return false;
        }

        assert(mTStatus == expectedStatus);

        return true;  
    }

    // response with the command status
    void respondCmdStatus(int value){
        mLockCmdStatus.lock();
        mCmdStatus = value;
        mCondCmdExctd.signal();
        mLockCmdStatus.unlock();
        LOGI("Record thread: sent signal to wake up main thread.");
    }

public:

    FMRecordThread(FMRadio& fm): Thread(false), mOwner(fm) {
        mTStatus = T_STATUS_EXIT;
        mSendCmd = CMD_NONE;
        mAudioRecord = NULL;
        mBuf = (char *)malloc(CHUNK_SIZE);
#ifdef STREAM_DUMP_DEBUG
        mFdRecordFile = open(FMRECORD_PCM_LOC, O_WRONLY | O_CREAT | O_TRUNC);
        if (mFdRecordFile < 0)
            LOGE("Error open %s.", FMRECORD_PCM_LOC);
#endif
    };

    ~FMRecordThread(){   
        if (mBuf){
            free(mBuf);
            mBuf = NULL;
        }
#ifdef STREAM_DUMP_DEBUG
        if (mFdRecordFile > 0){
            close(mFdRecordFile);
        }
#endif
    };

    int getThreadStatus(){
        return mTStatus;
    }

    bool requireStart(){
        // Check thread status
        if (mTStatus == T_STATUS_START){
            LOGI("Main Thread: Record thread is already started.");
            return true;
        }
        // Bring up the thread firstly if thread already exit, thread status will be suspend
        if (mTStatus == T_STATUS_EXIT){
            run("FMRecord", ANDROID_PRIORITY_NORMAL);
        }
        // thread is suspend, send start command and wakeup thread
        if (sendCmd(CMD_START, T_STATUS_START)){
            LOGI("Main Thread: Record thread Start succeed.");
            return true;
        }else{
            LOGE("Main Thread: Record thread Start failed.");
            return false;
        }
    }

    bool requireSuspend(){
        // Check thread status
        if (mTStatus == T_STATUS_SUSPEND){
            LOGI("Main Thread: Record thread is already suspended.");
            return true;
        }
        // Bring up thread firstly if thread not up, thread status is suspend
        if (mTStatus == T_STATUS_EXIT){            
            run("FMRecord", ANDROID_PRIORITY_NORMAL);
            return true;
        }
        // thread is start, send suspend command 
        if (sendCmd(CMD_SUSPEND, T_STATUS_SUSPEND)){
            LOGI("Main Thread: Record thread Suspend succeed.");
            return true;
        }else{
            LOGE("Main thread, Record thread Suspend failed.");
            return false;
        }
    }

    bool requireExit(){
        // Check thread status
        if (mTStatus == T_STATUS_EXIT){
            LOGI("Main thread: Record thread is already exit.");
            return true;
        }
        // keep a strong ref on ourself so that we wont get
        // destroyed in the middle of requireExit()
        sp <Thread> strongMe = this;
        if (sendCmd(CMD_EXIT, T_STATUS_EXIT)){
            LOGI("Main thread: Record thread Eixt succeed.");
            return true;
        }else{
            LOGE("Main thread: Record thread Eixt failed.");
            return false;
        }

    }

    virtual bool threadLoop() {
        int buffer_index = 0;
        prepareSuspend();
        mTStatus = T_STATUS_SUSPEND;
        int ret = true;
        LOGI("Record thread suspend.");
        while(1) {
SUSPEND:        if (mTStatus == T_STATUS_SUSPEND){
                    // Suspend waiting for CMD_EXIT or CMD_START
                    mLockSendCmd.lock();
                    while ((mSendCmd != CMD_EXIT) && (mSendCmd != CMD_START)) {
                        mCondSendCmd.wait(mLockSendCmd);
                    } 
                    mLockSendCmd.unlock();
                }
                mLockSendCmd.lock();
                switch(mSendCmd){
                case CMD_EXIT:
                    mSendCmd = CMD_NONE;
                    mLockSendCmd.unlock();
                    prepareExit();
                    mTStatus = T_STATUS_EXIT;
                    respondCmdStatus(CMD_STATUS_SUCCESS);
                    // Should reture false so that thread really exit. refer to threads.h
                    ret = false;
                    goto EXIT;
                case CMD_SUSPEND:
                    mSendCmd = CMD_NONE;
                    mLockSendCmd.unlock();
                    prepareSuspend();
                    mTStatus = T_STATUS_SUSPEND;
                    respondCmdStatus(CMD_STATUS_SUCCESS);
                    FMRadio::mFMBuffer->uninit("write");
                    goto SUSPEND;
                case CMD_START:
                    mSendCmd = CMD_NONE;
                    mLockSendCmd.unlock();
                    if (prepareStart() == false){
                        prepareExit();
                        mTStatus = T_STATUS_EXIT;                        
                        respondCmdStatus(CMD_STATUS_FAIL);
                        ret = false;
                        goto EXIT;
                    }
                    mTStatus = T_STATUS_START; 
                    respondCmdStatus(CMD_STATUS_SUCCESS);
                    FMRadio::mFMBuffer->init("write");
                    break;  
                default:
                    mLockSendCmd.unlock();
                    break;              
                }
                int bytesRead = mAudioRecord->read(mBuf, CHUNK_SIZE);
#ifdef STREAM_DUMP_DEBUG
                if (mFdRecordFile > 0)
                    write(mFdRecordFile, mBuf, bytesRead);
#endif
                int bytesWritten = 0;
                while (bytesWritten < bytesRead){
                    bytesWritten +=
                        FMRadio::mFMBuffer->write(mBuf + bytesWritten, bytesRead - bytesWritten);
                }
        }
EXIT:
        FMRadio::mFMBuffer->uninit("write");
        return ret;
    };

};

class FMRadio::FMPlayerThread : public Thread {
private:
    // command types
    enum cmd_types{
        CMD_NONE = 0,  // No requirment
        CMD_SUSPEND,   // Require thread to suspend
        CMD_START,     // Require thread to start
        CMD_EXIT,      // Require thread to exit
    };

    // command status
    enum cmd_excecuted_status{
        CMD_STATUS_NOT_EXECTED = 0,  // has not been executed
        CMD_STATUS_SUCCESS = 1,  // success
        CMD_STATUS_FAIL = -1,  // fail
    };

    enum thread_status{
        T_STATUS_SUSPEND,   // thread is waiting for commands "start" or "exit"
        T_STATUS_START,     // thread is polling events
        T_STATUS_EXIT,      // thread is down
    }; 

    FMRadio& mOwner;
    AudioTrack *mAudioTrack; //Record FM data      
    Mutex mLockCmdStatus;    //Protect mCmdStatus
    Condition mCondCmdExctd;// Wait for condtion (mCmdStatus!=CMD_NOT_EXECTED)
    Mutex mLockSendCmd;     //Protect mSendCmd
    Condition mCondSendCmd; // Wait for condtion (mSendCmd!=REQUIRE_NONE)
    int mSendCmd;           // cmd to ctrol thread's status, see enum cmd_types{}
    int mCmdStatus;         // Command was executed?
    int mTStatus;   
    bool mIsMute;
    char *mBuf;
#ifdef STREAM_DUMP_DEBUG
    int mFdPlayFile;
#endif

    bool prepareStart(){
        if (mAudioTrack == NULL){
            mAudioTrack = new AudioTrack(
                AudioSystem::MUSIC,
                SAMPLE_RATE_RECORD,
                AudioSystem::PCM_16_BIT, 
                AudioSystem::CHANNEL_OUT_STEREO,
                0
                );
            if (mAudioTrack->initCheck() != NO_ERROR){
                LOGE("Player thread: prepare Start failed.");
                return false;
            }     
            mAudioTrack->start();
            if (mIsMute == true){
                mAudioTrack->mute(true);
            }
        }
        LOGI("Player thread: prepare Start succeed.");
        return true;       
    }

    bool prepareSuspend(){
        if (mAudioTrack){
            mAudioTrack->stop();
            delete mAudioTrack;
            mAudioTrack = NULL;
        }
        LOGI("Player thread: prepare Suspend succeed.");
        return true;
    }

    bool prepareExit(){
        if (mAudioTrack){
            mAudioTrack->stop();
            delete mAudioTrack;
            mAudioTrack = NULL;
        }
        LOGI("Player thread: prepare Exit succeed.");
        return true; 
    }

    /*
    **  @Description
    ** This API is for sending thread control commands and event commands.
    ** Parameter cmd refer to enum cmd_types{}
    ** Parameter expectedStatus is the expected thread status after 
    ** thread control commands successfully exectued. 
    ** @Return values
    ** true Success
    ** false Fail
    */
    bool sendCmd(int cmd, int expectedStatus){
        mCmdStatus = CMD_STATUS_NOT_EXECTED;
        mLockCmdStatus.lock();
        mLockSendCmd.lock();
        mSendCmd = cmd;
        if (mTStatus == T_STATUS_SUSPEND){
            // Wakeup thread if thread is suspended
            mCondSendCmd.signal();
        }
        mLockSendCmd.unlock();
        LOGI("Main thread: Waiting for signal to wake up.");
        while (mCmdStatus == CMD_STATUS_NOT_EXECTED) {
            mCondCmdExctd.wait(mLockCmdStatus);
        }
        mLockCmdStatus.unlock();
        LOGI("Main thread: waked up!!");
        if (mCmdStatus == CMD_STATUS_FAIL){
            // command excuted fail
            LOGE("Command excuted fail.");
            return false;
        }

        assert(mTStatus == expectedStatus);

        return true;  
    }

    // response with the command status
    void respondCmdStatus(int value){
        mLockCmdStatus.lock();
        mCmdStatus = value;
        mCondCmdExctd.signal();
        mLockCmdStatus.unlock();
        LOGI("Player thread: Sent signal to wake up Main Thread.");
    }

public:

    FMPlayerThread(FMRadio& fm): Thread(false), mOwner(fm) {
        mTStatus = T_STATUS_EXIT;
        mSendCmd = CMD_NONE;
        mAudioTrack = NULL;
        mIsMute = false;
        mBuf = (char *)malloc(CHUNK_SIZE);
#ifdef STREAM_DUMP_DEBUG
        mFdPlayFile = open(FMPLAY_PCM_LOC, O_WRONLY | O_CREAT | O_TRUNC);
        if (mFdPlayFile < 0)
            LOGE("Error open %s.", FMPLAY_PCM_LOC);
#endif
    };

    ~FMPlayerThread(){
        if (mBuf){
            free(mBuf);
            mBuf = NULL;
        }
#ifdef STREAM_DUMP_DEBUG
        if (mFdPlayFile > 0){
            close (mFdPlayFile);
            mFdPlayFile = -1;
        }
#endif
    };

    int getThreadStatus(){
        return mTStatus;
    }

    bool requireStart(){
        // Check thread status
        if (mTStatus == T_STATUS_START){
            LOGI("Main Thread: Player thread is already started.");
            return true;
        }
        // Bring up the thread firstly if thread already exit, thread status will be suspend
        if (mTStatus == T_STATUS_EXIT){
            run("FMPlayer", ANDROID_PRIORITY_NORMAL);
        }
        // thread is suspend, send start command and wakeup thread
        if (sendCmd(CMD_START, T_STATUS_START)){
            LOGI("Main Thread: player thread Start succeed.");
            return true;
        }else{
            LOGE("Main Thread: player thread Start failed.");
            return false;
        }

    }

    bool requireSuspend(){
        // Check thread status
        if (mTStatus == T_STATUS_SUSPEND){
            LOGI("Main Thread: player thread is already suspended.");
            return true;
        }
        // Bring up thread firstly if thread not up, thread status is suspend
        if (mTStatus == T_STATUS_EXIT){            
            run("FMPlayer", ANDROID_PRIORITY_NORMAL);
            return true;
        }
        // thread is start, send suspend command 
        if (sendCmd(CMD_SUSPEND, T_STATUS_SUSPEND)){
            LOGI("Main Thread: player thread Suspend succeed.");
            return true;
        }else{
            LOGE("Main Thread: player thread Suspend failed.");
            return false;
        }

    }

    bool requireExit(){
        // Check thread status
        if (mTStatus == T_STATUS_EXIT){
            LOGI("Main Thread: Player thread is already exit.");
            return true;
        }
        // keep a strong ref on ourself so that we wont get
        // destroyed in the middle of requestExitAndWait()
        sp <Thread> strongMe = this;
        if (sendCmd(CMD_EXIT, T_STATUS_EXIT)){
            LOGI("Main Thread: Player thread Exit succeed.");
            return true;
        }else{
            LOGE("Main Thread: Player thread Exit failed.");
            return false;
        }
    }

    float get_volume(float *volume){
        if (!mAudioTrack){
            LOGW("AudioTrack was not initialized.");
            return 0;
        }
        mAudioTrack->getVolume(volume, volume);
        return *volume;
    };

    void set_volume(float volume) {
        if (!mAudioTrack){
            LOGW("AudioTrack was not initialized.");
            return;
        }

        mAudioTrack->setVolume(volume, volume);       
    };

    bool set_mute(bool flag) {
        if (!mAudioTrack){
            LOGW("AudioTrack was not initialized.");
            return false;
        }
        mAudioTrack->mute(flag); 
        mIsMute = mAudioTrack->muted();
        return mIsMute;
    };

    virtual bool threadLoop() {
        prepareSuspend();
        mTStatus = T_STATUS_SUSPEND;
        LOGI("Player thread is suspend.");
	    int ret = true;
        int bytesRead = 0;

	    while (true) {
SUSPEND:    if (mTStatus == T_STATUS_SUSPEND){
                // Suspend waiting for CMD_EXIT or CMD_START
                mLockSendCmd.lock();
                while ((mSendCmd != CMD_EXIT) && (mSendCmd != CMD_START)) {
                    mCondSendCmd.wait(mLockSendCmd);
                } 
                mLockSendCmd.unlock();
            }
            mLockSendCmd.lock();
            switch(mSendCmd){
                case CMD_EXIT:
                    mSendCmd = CMD_NONE;
                    mLockSendCmd.unlock();
                    prepareExit();
                    mTStatus = T_STATUS_EXIT;
                    respondCmdStatus(CMD_STATUS_SUCCESS);
                    // Should reture false so that thread really exit. refer to threads.h
                    ret = false;
                    FMRadio::mFMBuffer->uninit("read");
                    goto EXIT;
                case CMD_SUSPEND:
                    mSendCmd = CMD_NONE;
                    mLockSendCmd.unlock();
                    prepareSuspend();
                    mTStatus = T_STATUS_SUSPEND;
                    LOGI("Player thread suspend.");
                    respondCmdStatus(CMD_STATUS_SUCCESS);
                    FMRadio::mFMBuffer->uninit("read");
                    goto SUSPEND;
                case CMD_START:
                    mSendCmd = CMD_NONE;
                    mLockSendCmd.unlock();
                    if (prepareStart() == false){
                        prepareExit();
                        mTStatus = T_STATUS_EXIT;
                        respondCmdStatus(CMD_STATUS_FAIL);
                        ret = false;
                        goto EXIT;
                    }
                    mTStatus = T_STATUS_START; 
                    respondCmdStatus(CMD_STATUS_SUCCESS);
                    FMRadio::mFMBuffer->init("read");
                    break;
                default:
                    mLockSendCmd.unlock();
                    break;                 
            }
            while((bytesRead = FMRadio::mFMBuffer->read(mBuf, CHUNK_SIZE, true)) == 0){
                mLockSendCmd.lock();
                if ((mSendCmd == CMD_EXIT) || (mSendCmd == CMD_SUSPEND)){
                    mLockSendCmd.unlock();
                    goto SUSPEND;
                }
                mLockSendCmd.unlock();
            }
#ifdef STREAM_DUMP_DEBUG
            if (mFdPlayFile > 0){
                write(mFdPlayFile, mBuf, bytesRead);
            }
#endif
            mAudioTrack->write(mBuf, bytesRead);
	    } 
EXIT:
        FMRadio::mFMBuffer->uninit("read");
        return true;
    };

};
#else //playback mode
class FMRadio::RecordListener:public MediaRecorderListener
{
    FMRadio& mOwner;
public:
    RecordListener(FMRadio& fm): mOwner(fm) {}
    void notify(int msg, int arg1, int arg2){
        switch(msg){
        case MEDIA_RECORDER_EVENT_ERROR:
            report_recording_stop(INTERNAL_ERROR);
            break;
        case MEDIA_RECORDER_EVENT_INFO:
            switch(arg1){
            case MEDIA_RECORDER_INFO_MAX_FILESIZE_REACHED:
                report_recording_stop(MAX_FILESIZE_REACHED);
                break;
            default:
                report_recording_stop(UNKONW_ERROR);
                break;
            }
        default:
            report_recording_stop(UNKONW_ERROR);
            break;
        }
    }
};
#endif
FMRadio::FMRadio() {
    isPowerOn = false;
    isTx = false;
    isRx = false;
    isScanning = false;
    isSpeaker = false;
    mMinFrequency = 87500;
    mMaxFrequency = 108100;
    mfd = -1;
    mInstance = this;
    isDefferUnmuted = false;
#ifdef FM_NOT_USES_RECORD
    isMutedByUser = false;
#endif
    struct sigaction act;
#ifdef ANDROID_SET_AID_AND_CAP
    android_set_aid_and_cap();
    print_process_info();
#endif

    act.sa_sigaction = signal_action;
    act.sa_flags = SA_SIGINFO;

    sigaction(SIGKILL, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
#ifndef FM_NOT_USES_RECORD //record mode
    mFMBuffer = new FMRingBuffer(CHUNK_SIZE * 100);
    mFMBuffer->setFrameSize(4);
    mFMRecordThread = new FMRecordThread(*this);
    mFMPlayerThread = new FMPlayerThread(*this);
    mFMMediaRecordThread = new FMMediaRecordThread(*this);
#else //playback mode
    mRecordListener = new RecordListener(*this);
#endif    
    mEventThread = new EventThread(*this);
} 

FMRadio::~FMRadio() {
#ifndef FM_NOT_USES_RECORD    
    mFMRecordThread->requireExit();
    mFMPlayerThread->requireExit();
    if (mFMBuffer){
        delete mFMBuffer;
        mFMBuffer = NULL;
    }
#endif
    mEventThread->requireExit();


}
void FMRadio::instantiate() {
    defaultServiceManager()->addService(
            String16("FMRadioService"), new FMRadio());
}

FMRadio* FMRadio::getInstance() {
    AutoMutex _l(mInstanceLock);
    if (mInstance == NULL) {
        mInstance = new FMRadio();
    }
    return mInstance;
}

bool FMRadio::isFMTx() {
    return isTx;
}

bool FMRadio::isFMRx() {
    return isRx;
}

int FMRadio::powerOn() {
    AutoMutex _l(mLock);

    if (isPowerOn) {
        return 0;
    } else {
        if (fm_enable() != 0) {
            LOGE("Enable fm failed.");
            return -1;
        }

        isPowerOn = true;
        return 0;
    }
}

int FMRadio::powerOff() {
    AutoMutex _l(mLock);

    if (isPowerOn) {
        if (fm_disable() != 0) {
            LOGE("Disable fm failed.");
            return -1;
        } else {
            isPowerOn = false;
            return 0;
        }
    } else {
        return 0;
    }
}

int FMRadio::enableTx() {
    AutoMutex _l(mLock);

    struct v4l2_capability cap;
    struct v4l2_tuner tuner;
    int ret = 0;
    int dd;
    int i;

    if (isTx) {
        return 0;
    }

    if (isRx) {
        LOGI("FM Receiver is enabled. Disable it first.");
        mLock.unlock();
        disableRx();
        mLock.lock();
        if (!isRx) {
            LOGI("Disable FM Receiver successfully.");
            AutoMutex _l(mCallbackLock);
            for (int i = 0, n = mCallbacks.size(); i < n; ++i) {
                mCallbacks.valueAt(i)->state_changed(0);
            }
        } else {
            LOGE("Failed to disable FM Receiver.");
            return -1;
        }
    }

    if (!isPowerOn){
        LOGE("FM isn't Power-On.");
        return -1;
    }

    AudioSystem::setDeviceConnectionState(AudioSystem::DEVICE_OUT_FM_TRANSMITTER,
                                          AudioSystem::DEVICE_STATE_AVAILABLE, "");

    for(i = 0; i < 20; i++) {
        mfd = open(S_RADIO_DEVICE, O_RDWR);
        if (mfd > 0) {
            break;
        }
        sleep(1);
    }
    LOGI("Opened radio device.");

    if (mfd < 0) {
        LOGE ("Can not open %s.", S_RADIO_DEVICE);
        return -1;
    }

    // Select fm transmitter mode
    if (radio_set_ctrl(MRVL8787_CID_BASE + MRVL8787_CID_MODE, 1) < 0) {
        LOGE("FM Transmitter configuration failed");
        goto FAIL;
    }

    // Query Radio device capabilities.
    if (ioctl(mfd, VIDIOC_QUERYCAP, &(cap))<0){
        LOGE("ioctl VIDIOC_QUERYCAP failed");
        goto FAIL;
    }

    if ((cap.capabilities & V4L2_CAP_RADIO) && (cap.capabilities & V4L2_CAP_TUNER)) {
        LOGI("V4L2_CAP_RADIO and V4L2_CAP_TUNER set\n");
    } else {
        LOGE("V4L2_CAP_RADIO && V4L2_CAP_TUNER flags not set by ioctl VIDIOC_QUERYCAP");
        goto FAIL;
    }

    tuner.index = 0;
    if (-1 == ioctl(mfd, VIDIOC_G_TUNER, &(tuner))) {
        LOGE("ioctl VIDIOC_G_TUNER failed");
        goto FAIL;

    }

    /* check type of radio*/
    if (tuner.type != V4L2_TUNER_RADIO) {
        LOGE("Error: mTuner.type != V4L2_TUNER_RADIO ");
        goto FAIL;
    }
    /*check capability : a radio tuner only the V4L2_TUNER_CAP_LOW and V4L2_TUNER_CAP_STEREO flags can be set*/
    if (tuner.capability & ~(V4L2_TUNER_CAP_LOW | V4L2_TUNER_CAP_STEREO)) {
        LOGE("Error : mTuner.capability ");
        goto FAIL;
    }

    isTx = true;

    return 0;

FAIL:
    if (mfd > 0){
        close(mfd);
        mfd = -1;
    }
    return -1;
}

int FMRadio::disableTx() {
    AutoMutex _l(mLock);

    int ret = 0;
    if (isTx != true){
        return 0;
    }

    if(mfd > 0){
        close(mfd);
        mfd = -1;
    }
    LOGI("radio device was closed.");

    AudioSystem::setDeviceConnectionState(AudioSystem::DEVICE_OUT_FM_TRANSMITTER,
                                          AudioSystem::DEVICE_STATE_UNAVAILABLE, "");

    isTx = false;
    return ret;
}

int FMRadio::enableRx() {
    AutoMutex _l(mLock);

    struct v4l2_capability cap;
    struct v4l2_tuner tuner;  
    int ret = 0;
    int dd;
    int i;

    if (isRx) {
    	return 0;
    }

    if (isTx) {
       LOGI("FM Transmitter is enabled. Disable it first.");
       mLock.unlock();
       disableTx();
       mLock.lock();
       if (!isTx) {
            LOGI("Disable FM Transmitter successfully.");
            AutoMutex _l(mCallbackLock);
            for (int i = 0, n = mCallbacks.size(); i < n; ++i) {
                mCallbacks.valueAt(i)->state_changed(1);
            }
       } else {
           LOGE("Failed to disable FM Transmitter.");
           return -1;
       }
    }

    if (!isPowerOn){
        LOGE("FM isn't Power-On.");
        return -1;
    }

    for(i = 0; i < 20; i++) {
        mfd = open(S_RADIO_DEVICE, O_RDWR);
        if (mfd > 0) {
            break;
        }
        sleep(1);
    }
    LOGI("Opened radio device.");

    if (mfd < 0) {
        LOGE ("Can not open %s.", S_RADIO_DEVICE);
        return -1;
    }

    // Select FM Radio mode
    if (radio_set_ctrl(MRVL8787_CID_BASE + MRVL8787_CID_MODE, 0) < 0) {
        LOGE("FM Radio configuration failed");
        goto FAIL;
    }

    // Query Radio device capabilities.
    if (ioctl(mfd, VIDIOC_QUERYCAP, &(cap))<0){ 
		LOGE("ioctl VIDIOC_QUERYCAP failed");    
        goto FAIL;
    }

	if ((cap.capabilities & V4L2_CAP_RADIO) && (cap.capabilities & V4L2_CAP_TUNER)) {
		LOGI("V4L2_CAP_RADIO and V4L2_CAP_TUNER set\n");
	} else {
		LOGE("V4L2_CAP_RADIO && V4L2_CAP_TUNER flags not set by ioctl VIDIOC_QUERYCAP");
		goto FAIL;
	}    

	tuner.index = 0;
	if (-1 == ioctl(mfd, VIDIOC_G_TUNER, &(tuner))) {
		LOGE("ioctl VIDIOC_G_TUNER failed");
		goto FAIL;

	}
	
	/* check type of radio*/
	if (tuner.type != V4L2_TUNER_RADIO) {
		LOGE("Error: mTuner.type != V4L2_TUNER_RADIO ");
		goto FAIL;
	}
	/*check capability : a radio tuner only the V4L2_TUNER_CAP_LOW and V4L2_TUNER_CAP_STEREO flags can be set*/
	if (tuner.capability & ~(V4L2_TUNER_CAP_LOW | V4L2_TUNER_CAP_STEREO)) {
		LOGE("Error : mTuner.capability ");
		goto FAIL;
	}
    /* Walkaround: make sure FM is muted until first set channel */
    if (-1 == radio_set_ctrl(MRVL8787_CID_BASE + MRVL8787_AUDIO_MUTE, true)){
            LOGE("Error : ioctl MRVL8787_AUDIO_MUTE failed.");
                goto FAIL;
    }
    isDefferUnmuted = true;

 	if ((mEventThread->requireStart() == false) 
#ifndef FM_NOT_USES_RECORD        
        || (mFMRecordThread->requireStart()== false) ||
        (mFMPlayerThread->requireStart() == false)
#endif
    ){
		LOGE("FM Start threads failed");
		goto FAIL;
    }
#ifdef FM_NOT_USES_RECORD
    if (!isSpeaker){
	    AudioSystem::setDeviceConnectionState(AudioSystem::DEVICE_OUT_FM_HEADPHONE,
                            AudioSystem::DEVICE_STATE_AVAILABLE, "");
    } else {
	    AudioSystem::setDeviceConnectionState(AudioSystem::DEVICE_OUT_FM_SPEAKER,
                            AudioSystem::DEVICE_STATE_AVAILABLE, "");
    }
#else
    if (isSpeaker){
        AudioSystem::setForceUse(AudioSystem::FOR_MEDIA, AudioSystem::FORCE_SPEAKER);
    }

    /* Need capture PARTIAL_WAKE_LOCK */
    if (acquire_wake_lock(PARTIAL_WAKE_LOCK, FM_WAKELOCK) > 0) {
        LOGI("enable() acquire a wakelock success");
    } else {
        LOGE("enable() acquire a wakelock failed");
    }

#endif
    isRx = true;

    return 0;

FAIL:
    if (mfd > 0){
        close(mfd);
        mfd = -1;
    } 
	return -1;
}

int FMRadio::disableRx() {
    AutoMutex _l(mLock);
    stop_recording_locked();

    int ret = 0;
    if (isRx != true){
        return 0;
    }
#ifdef FM_NOT_USES_RECORD
    if (!isSpeaker){
        AudioSystem::setDeviceConnectionState(AudioSystem::DEVICE_OUT_FM_HEADPHONE,
                                    AudioSystem::DEVICE_STATE_UNAVAILABLE, "");
    } else {
        AudioSystem::setDeviceConnectionState(AudioSystem::DEVICE_OUT_FM_SPEAKER,
                                    AudioSystem::DEVICE_STATE_UNAVAILABLE, "");
    }
#else
    mFMRecordThread->requireSuspend();  
    mFMPlayerThread->requireSuspend();
    if (isSpeaker){
        AudioSystem::setForceUse(AudioSystem::FOR_MEDIA, AudioSystem::FORCE_NONE);
    }
#endif    
    mEventThread->requireSuspend();
    if(mfd > 0){
        close(mfd);
        mfd = -1;
    }
    LOGI("radio device was closed.");

#ifndef FM_NOT_USES_RECORD
    release_wake_lock(FM_WAKELOCK);
#endif

    isRx = false;
    return ret;
}

// Suspend FM audio
int FMRadio::suspend() {
    AutoMutex _l(mLock);

    if (isRx) {
#ifdef FM_NOT_USES_RECORD
        if (!isSpeaker){
            AudioSystem::setDeviceConnectionState(AudioSystem::DEVICE_OUT_FM_HEADPHONE,
                                        AudioSystem::DEVICE_STATE_UNAVAILABLE, "");
        } else {
            AudioSystem::setDeviceConnectionState(AudioSystem::DEVICE_OUT_FM_SPEAKER,
                                        AudioSystem::DEVICE_STATE_UNAVAILABLE, "");
        }
#else
        /* Force none when FM is suspened so that it would affect other APPs. */
        if (isSpeaker){
            AudioSystem::setForceUse(AudioSystem::FOR_MEDIA, AudioSystem::FORCE_NONE);
        }
        mFMRecordThread->requireSuspend();
        mFMPlayerThread->requireSuspend();
#endif
    } else if (isTx) {
        AudioSystem::setDeviceConnectionState(AudioSystem::DEVICE_OUT_FM_TRANSMITTER,
                                              AudioSystem::DEVICE_STATE_UNAVAILABLE, "");
    } else {
        return -1;
    }

    return 0;
}

// Resume FM audio
int FMRadio::resume() {
    AutoMutex _l(mLock);

    if (isRx) {
#ifdef FM_NOT_USES_RECORD
        if (!isSpeaker){
            AudioSystem::setDeviceConnectionState(AudioSystem::DEVICE_OUT_FM_HEADPHONE,
                                AudioSystem::DEVICE_STATE_AVAILABLE, "");
        } else {
            AudioSystem::setDeviceConnectionState(AudioSystem::DEVICE_OUT_FM_SPEAKER,
                                AudioSystem::DEVICE_STATE_AVAILABLE, "");
        }
#else
        if ((mFMPlayerThread->requireStart()== false) ||
            (mFMRecordThread->requireStart() == false))
        {
            LOGE("FM Start threads failed");
            return -1;
        }
        if (isSpeaker){
            AudioSystem::setForceUse(AudioSystem::FOR_MEDIA, AudioSystem::FORCE_SPEAKER);
        }
#endif
    } else if (isTx) {
        AudioSystem::setDeviceConnectionState(AudioSystem::DEVICE_OUT_FM_TRANSMITTER,
                                              AudioSystem::DEVICE_STATE_AVAILABLE, "");
    } else {
        return -1;
    }

    return 0;
}

int FMRadio::scan_all() {

    AutoMutex _l(mLock);
	struct v4l2_frequency radio_freq;
    unsigned int frequency, old_frequency;
    frequency = 0;
    old_frequency = 0;
	memset(&radio_freq, 0, sizeof(struct v4l2_frequency));
    radio_freq.tuner = 0;
    radio_freq.type = V4L2_TUNER_RADIO;
          
    if (!isRx || isScanning) {
        isScanning = false;
	    return -1;
    } 
    isScanning = true;
    /* set auto search mode = auto(0) */
    if (radio_set_ctrl(MRVL8787_CID_BASE + MRVL8787_SET_SEARCH_MODE, 0) < 0){
        isScanning = false;        
        return -1;
    }    
    /* set search direction = forward */    
    if (radio_set_ctrl(MRVL8787_CID_BASE + MRVL8787_SET_SEARCH_DIRECTION, 0) 
                                    < 0){
        isScanning = false;                                    
        return -1;
    }   
    for (frequency = mMinFrequency; frequency < mMaxFrequency && isScanning;){
        radio_freq.frequency = frequency;        
    	if (ioctl(mfd, VIDIOC_S_FREQUENCY, &radio_freq) < 0)
    	{
    	    isScanning = false;
            return -1;
    	}
    	if (ioctl(mfd, VIDIOC_G_FREQUENCY, &radio_freq) < 0)
    	{
    	    isScanning = false;
            return -1;
    	}   
        old_frequency = frequency;
        frequency = radio_freq.frequency;
        LOGI("Scan_all searched %d.\n", frequency);
        if (frequency <= old_frequency){
            /* call scan finished handler */
            mEventThread->sendCmd(S_EVENT_SCAN_FINISHED, -1);
            break;
        }
        char temp[64];
        sprintf(temp, "%s %d", S_EVENT_FOUND_CHANNEL, frequency);
        mEventThread->sendCmd(temp, -1);
        frequency += 100;
    }
    isScanning = false;

    return 0;
}

//stop_scan actually not work in stub, since scan_all will not return unless scan finished
int FMRadio::stop_scan() {
    // Only let one thread to stop search at one time
    AutoMutex _l(mLock_stopScan);
    if (!isRx || !isScanning) {
	    return -1;
    } else {
	    isScanning = false;
        radio_set_ctrl(MRVL8787_CID_BASE + MRVL8787_STOP_SEARCH, 0);
    }

    return 0;
}

int FMRadio::set_channel(unsigned int freq) {

    AutoMutex _l(mLock);
	struct v4l2_frequency radio_freq;  
    int ret = 0;     
    if (!isRx && !isTx) {
        return -1;
    } 
    /* set auto search mode = manual(1) */
    if (radio_set_ctrl(MRVL8787_CID_BASE + MRVL8787_SET_SEARCH_MODE, 1) < 0){      
        return -1;
    }    

	memset(&radio_freq, 0, sizeof(struct v4l2_frequency));
	radio_freq.tuner = 0;
	radio_freq.type = V4L2_TUNER_RADIO;
    radio_freq.frequency = freq;

	if (-1 == ioctl(mfd, VIDIOC_S_FREQUENCY, &radio_freq)) {
		LOGE("Failed to set freq to %dhz!\n", freq );
                return -1;
	}
#ifdef FM_NOT_USES_RECORD
    if (isDefferUnmuted && isMutedByUser == false){
#else
    if (isDefferUnmuted){
#endif
        /* Walkaround: make sure FM being muted until first set channel
         * */
        radio_set_ctrl(MRVL8787_CID_BASE + MRVL8787_AUDIO_MUTE, false);
        isDefferUnmuted = false;
    }
#ifdef BOARD_USES_SANREMO
    char temp[64];
    sprintf(temp, "FM_CHANNEL=%d", freq);
    AudioSystem::setParameters(0, String8(temp));
#endif
    return ret;    

}

int FMRadio::get_channel() {

    AutoMutex _l(mLock);
	struct v4l2_frequency radio_freq;  
    int ret = 0;      
    if (!isRx && !isTx) {
        return -1;
    } 

	memset(&radio_freq, 0, sizeof(struct v4l2_frequency));
	radio_freq.tuner = 0;
	radio_freq.type = V4L2_TUNER_RADIO;

	if (-1 == ioctl(mfd, VIDIOC_G_FREQUENCY, &radio_freq)) {
		ret = -1;
		LOGE("Failed to get current chanel.\n");
	}    

    return radio_freq.frequency; 
}

int FMRadio::get_rssi() {    

    AutoMutex _l(mLock); 
    if (!isRx) {
        return -1;
    } 

    return radio_get_ctrl(MRVL8787_CID_BASE + MRVL8787_GET_CURRENT_RSSI);         
}

float FMRadio::get_volume() {

    AutoMutex _l(mLock);
    float volume;     
    if (!isRx) {
        return -1;
    } 
#ifdef FM_NOT_USES_RECORD
    return radio_get_ctrl(MRVL8787_CID_BASE + MRVL8787_AUDIO_VOLUME);        
#else
    mFMPlayerThread->get_volume(&volume);  
#endif
    return volume;   
}

int FMRadio::set_volume(int volume) {  

    AutoMutex _l(mLock); 
    if (!isRx) {
        return -1;
    } 
#ifdef FM_NOT_USES_RECORD   
    return radio_set_ctrl(MRVL8787_CID_BASE + MRVL8787_AUDIO_VOLUME, volume);   
#else
    mFMPlayerThread->set_volume(volume);  
#endif
    return 0;
}

bool FMRadio::set_mute(bool flag) {
    AutoMutex _l(mLock); 
    if (!isRx) {
        return -1;
    }     

#ifdef FM_NOT_USES_RECORD  
    if (radio_set_ctrl(MRVL8787_CID_BASE + MRVL8787_AUDIO_MUTE, flag) == 0){
        isMutedByUser = flag;
    } else {
        return false;
    }
#else
    mFMPlayerThread->set_mute(flag); 
#endif
    return true; 
}

int FMRadio::set_band(int band) {    

    AutoMutex _l(mLock);  
	if (!isRx && !isTx) {
        return -1;
    } 

    return radio_set_ctrl(MRVL8787_CID_BASE + MRVL8787_SET_FM_BAND, band);    
}

int FMRadio::scan_next() {

    AutoMutex _l(mLock);
	struct v4l2_frequency radio_freq;  
    int ret = 0;   
    unsigned int frequency = 0;     
    if (!isRx) {
        return -1;
    } 
    /* set auto search mode = auto(0) */
    if (radio_set_ctrl(MRVL8787_CID_BASE + MRVL8787_SET_SEARCH_MODE, 0) < 0){     
        return -1;
    }    
    /* set search direction = forward */    
    if (radio_set_ctrl(MRVL8787_CID_BASE + MRVL8787_SET_SEARCH_DIRECTION, 0) 
                                    < 0){                                  
        return -1;
    }      
	memset(&radio_freq, 0, sizeof(struct v4l2_frequency));
	radio_freq.tuner = 0;
	radio_freq.type = V4L2_TUNER_RADIO;
    
    if (ioctl(mfd, VIDIOC_G_FREQUENCY, &radio_freq) < 0)
    {
        return -1;
    }   
    frequency = radio_freq.frequency +100;
    frequency = (frequency >= mMaxFrequency) ? mMinFrequency:frequency;
    radio_freq.frequency = frequency;      
	if (ioctl(mfd, VIDIOC_S_FREQUENCY, &radio_freq) < 0) {
		LOGE("Failed to scan_next!\n");
                return -1;
	}
    
	if (ioctl(mfd, VIDIOC_G_FREQUENCY, &radio_freq) < 0) {
		LOGE("Scan_next failed to get current channel!\n");
                return -1;
	}    
    
    frequency = radio_freq.frequency;
    LOGI("scan_next stopped at channel %d", frequency);
    char temp[64];
    sprintf(temp, "%s %d", S_EVENT_FOUND_CHANNEL, frequency);
    mEventThread->sendCmd(temp, -1);
    mEventThread->sendCmd(S_EVENT_SCAN_FINISHED, -1);

#ifdef BOARD_USES_SANREMO
    sprintf(temp, "FM_CHANNEL=%d", frequency);
    AudioSystem::setParameters(0, String8(temp));
#endif

    return ret;    
}

int FMRadio::scan_prev(){

    AutoMutex _l(mLock);
	struct v4l2_frequency radio_freq;  
    int ret = 0;   
    unsigned int frequency = 0;    
    if (!isRx) {
        return -1;
    } 
    /* set auto search mode = auto(0) */
    if (radio_set_ctrl(MRVL8787_CID_BASE + MRVL8787_SET_SEARCH_MODE, 0) < 0){     
        return -1;
    }    
    /* set search direction = backward */    
    if (radio_set_ctrl(MRVL8787_CID_BASE + MRVL8787_SET_SEARCH_DIRECTION, 1) 
                                    < 0){                                  
        return -1;
    }      
	memset(&radio_freq, 0, sizeof(struct v4l2_frequency));
	radio_freq.tuner = 0;
	radio_freq.type = V4L2_TUNER_RADIO;
    
    if (ioctl(mfd, VIDIOC_G_FREQUENCY, &radio_freq) < 0)
    {
        return -1;
    }   
    frequency = radio_freq.frequency - 100;
    frequency = (frequency <= mMinFrequency) ? mMaxFrequency:frequency;
    radio_freq.frequency = frequency;      
	if (ioctl(mfd, VIDIOC_S_FREQUENCY, &radio_freq) < 0) {
		LOGE("Failed to scan_prev!\n");
		return -1;
	}
    
	if (ioctl(mfd, VIDIOC_G_FREQUENCY, &radio_freq) < 0) {
		LOGE("Scan_prev failed to get current channel!!\n");
                return -1;
	}
    
    frequency = radio_freq.frequency;
    LOGI("scan_prev stopped at channel %d", frequency);
    char temp[64];
    sprintf(temp, "%s %d", S_EVENT_FOUND_CHANNEL, frequency);
    mEventThread->sendCmd(temp, -1);
    mEventThread->sendCmd(S_EVENT_SCAN_FINISHED, -1);
#ifdef BOARD_USES_SANREMO
    sprintf(temp, "FM_CHANNEL=%d", frequency);
    AudioSystem::setParameters(0, String8(temp));
#endif

    return ret;    

}

int FMRadio::setSpeakerOn(bool on){
    AutoMutex _l(mLock);
#ifndef FM_NOT_USES_RECORD       
    if (on == true) {
        AudioSystem::setForceUse(AudioSystem::FOR_MEDIA, AudioSystem::FORCE_SPEAKER);
        isSpeaker = true;
    } else {
        AudioSystem::setForceUse(AudioSystem::FOR_MEDIA, AudioSystem::FORCE_NONE);
        isSpeaker = false;
    }

    /* Only restart FMPlayer thread when it is really running. */
    if (mFMPlayerThread->getThreadStatus() == T_STATUS_START){
        mFMPlayerThread->requireSuspend();
        mFMPlayerThread->requireStart();
    }
#else
    if (on == true) {
        AudioSystem::setDeviceConnectionState(AudioSystem::DEVICE_OUT_FM_HEADPHONE, AudioSystem::DEVICE_STATE_UNAVAILABLE, "");                
        AudioSystem::setDeviceConnectionState(AudioSystem::DEVICE_OUT_FM_SPEAKER, AudioSystem::DEVICE_STATE_AVAILABLE, "");
        isSpeaker = true;
    } else {
        AudioSystem::setDeviceConnectionState(AudioSystem::DEVICE_OUT_FM_SPEAKER, AudioSystem::DEVICE_STATE_UNAVAILABLE, "");      
        AudioSystem::setDeviceConnectionState(AudioSystem::DEVICE_OUT_FM_HEADPHONE, AudioSystem::DEVICE_STATE_AVAILABLE, "");
        isSpeaker = false;
    }
#endif

    return 0;
}

bool FMRadio::set_tx_mute(bool flag) {
    AutoMutex _l(mLock);
    if (!isTx) {
        return -1;
    }

    return radio_set_ctrl(MRVL8787_CID_BASE + MRVL8787_SET_MUTE, flag);
}

bool FMRadio::set_power_mode(bool flag) {
    AutoMutex _l(mLock);
    if (!isTx) {
        return -1;
    }

    return radio_set_ctrl(MRVL8787_CID_BASE + MRVL8787_SET_POWER_MODE, flag);
}

bool FMRadio::set_tx_mono_stereo(bool flag) {
    AutoMutex _l(mLock);
    if (!isTx) {
        return -1;
    }

    return radio_set_ctrl(MRVL8787_CID_BASE + MRVL8787_SET_TX_MONO_STEREO, flag);
}

#ifdef FM_NOT_USES_RECORD /* Playback mode */
/* Must be callsed with lock mLock */
bool FMRadio::start_recording_locked(int fd){
    LOGI("Prepare recording, fd = %d", fd);

    /* FM power on ? */
    if (!isPowerOn){
        LOGE("start_recording() FAILED for power off.");
        goto FAIL;
    }

    mRecorder = new MediaRecorder();
    mRecorder->init();
    mRecorder->setAudioSource(AUDIO_SOURCE_FMRADIO);

    mRecorder->setOutputFormat(OUTPUT_FORMAT_THREE_GPP);
    mRecorder->setParameters(String8("audio-param-sampling-rate=48000"));
    // TODO: we should set channels as stereo once AAC encoder support stereo stream
    mRecorder->setParameters(String8("audio-param-number-of-channels=2"));
    // Set max file size as 1 Gega bytes
    mRecorder->setParameters(String8("max-filesize=1073741824"));
    mRecorder->setParameters(String8("audio-param-encoding-bitrate=96000"));
    mRecorder->setAudioEncoder(AUDIO_ENCODER_AAC);

    mRecorder->setOutputFile(fd, 0, 0);

    if ((mRecorder->prepare() != OK) || (mRecorder->start() != OK)){
        LOGE("start_recording() FAIL with internal error.");
        goto FAIL;
    }
    mRecorder->setListener(mRecordListener);
    LOGI("Start Recording...");

    /* Need capture PARTIAL_WAKE_LOCK */
    if (acquire_wake_lock(PARTIAL_WAKE_LOCK, FM_WAKELOCK) > 0) {
        LOGI("start_recording() acquire a wakelock success");
    } else {
        LOGE("start_recording() acquire a wakelock failed");
    }

    return true;
FAIL:
    if (mRecorder != NULL){
        mRecorder->reset();
        mRecorder->release();
        mRecorder = NULL;
    }

    return false;
}

/* Must be callsed with lock mLock */
bool FMRadio::stop_recording_locked(){

    if (mRecorder == NULL){
        return true;
    }

    LOGE("Stop Recording...");
    mRecorder->setListener(NULL);

    mRecorder->stop();
    mRecorder->release();
    mRecorder = NULL;

    release_wake_lock(FM_WAKELOCK);

    return true;
}

#else /* record mode */
/* Must be callsed with lock mLock */
bool FMRadio::start_recording_locked(int fd){
    mFMMediaRecordThread->start(fd);
    return true;
}

/* Must be callsed with lock mLock */
bool FMRadio::stop_recording_locked(){
    mFMMediaRecordThread->stop();
    return true;
}
#endif

bool FMRadio::start_recording(int fd){
    AutoMutex _l(mLock);
    stop_recording_locked();
    return start_recording_locked(fd);
}

bool FMRadio::stop_recording(){
    AutoMutex _l(mLock);
    return stop_recording_locked();
}

void FMRadio::get_record_format(char **format){
#ifdef FM_NOT_USES_RECORD /* Playback mode */
    strcpy(*format, "3gpp");
#else
    strcpy(*format, "aac");
#endif
    return;
}

int FMRadio::radio_set_ctrl(int ctrl_type, int value){
    int ret = 0;
	struct v4l2_control ctrl;    

    ctrl.value = value;
    ctrl.id    = ctrl_type;
    if (ioctl (mfd,  VIDIOC_S_CTRL, &ctrl) < 0 ) { 
        ret = -1; 
        LOGE("set_ctrl failed"); 
    }

    return ret;
}

int FMRadio::radio_get_ctrl(int ctrl_type){
	struct v4l2_control ctrl;
    int ret = 0;

    ctrl.id = ctrl_type; 
    if( ioctl (mfd, VIDIOC_G_CTRL, &ctrl) == -1 ) { 
        ret = -1; 
        LOGE("set_band failed");
    }
    
    return  (ret == 0) ? ctrl.value: ret;    
}    


int FMRadio::registerCallback(const sp<IFMRadioCallback>& callback){
    wp<IBinder> who = callback->asBinder();

    LOGI("registerCallback() %p, tid %d, calling tid %d",
        who.unsafe_get(), gettid(), IPCThreadState::self()->getCallingPid());

    AutoMutex _l(mCallbackLock);
    mCallbacks.add(who.unsafe_get(), callback);
    callback->asBinder()->linkToDeath(this);

    return 0;
}

int FMRadio::unregisterCallback(const sp<IFMRadioCallback>& callback){
    wp<IBinder> who = callback->asBinder();

    LOGI("unregisterCallback() %p, tid %d, calling tid %d",
        who.unsafe_get(), gettid(), IPCThreadState::self()->getCallingPid());

    AutoMutex _l(mCallbackLock);
    mCallbacks.removeItem(who.unsafe_get());

    return 0;
}

void FMRadio::binderDied(const wp<IBinder>& who) {
    LOGW("binderDied() 1 %p, tid %d, calling tid %d",
        who.unsafe_get(), gettid(), IPCThreadState::self()->getCallingPid());
    AutoMutex _l(mCallbackLock);

    mCallbacks.removeItem(who.unsafe_get());
    if (mCallbacks.size() == 0){
        if (isRx) disableRx();
        if (isTx) disableTx();
    }

    powerOff();
}

void FMRadio::signal_action(int signum, siginfo_t *info, void *p){
    LOGW("Received signal %d, now existing..", signum);
    if (mInstance){
        if (mInstance->isTx) mInstance->disableTx();
        if (mInstance->isRx) mInstance->disableRx();
        mInstance->powerOff();
    }
    raise(SIGKILL); 
}

#ifdef ANDROID_SET_AID_AND_CAP
void FMRadio::android_set_aid_and_cap() {
    int ret = -1;
    prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0);

    gid_t groups[] = {1010};
    if ((ret = setgroups(sizeof(groups)/sizeof(groups[0]), groups)) == -1){
        LOGE("capset failed, ret:%d, strerror:%s", ret, strerror(errno));
        return;
    }

    if ((ret = setuid(AID_SYSTEM)) != 0){
        LOGE("setgid failed, ret:%d, strerror:%s", ret, strerror(errno));
        return;
    }
    if ((ret = setuid(AID_SYSTEM)) != 0){
        LOGE("setuid failed, ret:%d, strerror:%s", ret, strerror(errno));
        return;
    }

    struct __user_cap_header_struct header;
    struct __user_cap_data_struct cap;
    header.version = _LINUX_CAPABILITY_VERSION;
    header.pid = 0;

    cap.effective = cap.permitted = 1 << CAP_NET_RAW |
    1 << CAP_NET_ADMIN |
    1 << CAP_NET_BIND_SERVICE |
    1 << CAP_DAC_OVERRIDE;   /* Permission to access wake_lock file */

    cap.inheritable = 0;
    if ((ret = capset(&header, &cap)) != 0){
        LOGE("capset failed, ret:%d, strerror:%s", ret, strerror(errno));
        return;
    }
    return;     
}
void FMRadio::print_process_info(){
    int uid, gid;
    int ret = -1;
    int max = -1;
    int n;
    gid_t list[64];
    struct __user_cap_header_struct header;
    struct __user_cap_data_struct cap;
    header.version = _LINUX_CAPABILITY_VERSION;
    header.pid = 0;

    uid = getuid();
    gid = getgid();
    if ((ret = capget(&header, &cap)) != 0){
        LOGE("capget failed, ret:%d, strerror:%s", ret, strerror(errno));
        return;
    }
    LOGI("Current uid = %d, gid = %d, cap.effective = 0x%x, cap.permitted = 0x%x",
    uid, gid, cap.effective, cap.permitted);

    max = getgroups(64, list);
    if (max < 0) max = 0;

    if (max) {
        LOGI(" groups=");
        LOGI("%d", list[0]);
        for(n = 1; n < max; n++) {
            LOGI("%d", list[n]);
        }
    }

    return;
}
#endif

}//end android
