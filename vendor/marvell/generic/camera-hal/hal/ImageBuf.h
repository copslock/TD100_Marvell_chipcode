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

#ifndef ANDROID_HARDWARE_CAMERA_IMAGEBUF_H
#define ANDROID_HARDWARE_CAMERA_IMAGEBUF_H

//for cameraparameters
#if PLATFORM_SDK_VERSION >= 8
#include <camera/CameraHardwareInterface.h>
#else
#include <ui/CameraHardwareInterface.h>
#endif
#include <binder/MemoryBase.h>
#include <binder/MemoryHeapBase.h>
#include <utils/threads.h>
#include <binder/MemoryHeapPmem.h>
#include <linux/android_pmem.h>
#include <cam_log_mrvl.h>

//Notes: we only refer to CAM_ImageBuffer & CAM_ImageBufferReq from CameraEngine.h,
//no other structure should be envolved.
#include <CameraEngine.h>
/*
// image buffer header
typedef struct
{
CAM_ImageFormat         eFormat;
CAM_Int32s              iWidth;
CAM_Int32s              iHeight;
CAM_Int32s              iStep[3];       // Given by external before enqueue
CAM_Int32s              iAllocLen[3];   // Given by external before enqueue
CAM_Int32s              iFilledLen[3];
CAM_Int32s              iOffset[3];     // Given by external before enqueue
CAM_Int8u               *pBuffer[3];    // Given by external before enqueue, virtual address
CAM_Int32u              iPhyAddr[3];    // Given by external before enqueue, physical address of pBuffer[]
CAM_Tick                iTick;
CAM_Int32u              iFlag;          // Given by external before enqueue
CAM_Int32s              iUserIndex;     // Given by external before enqueue
void                    *pUserData;     // Given by external before enqueue
CAM_Int32s              iPrivateIndex;
void                    *pPrivateData;

CAM_Bool                bEnableShotInfo;
CAM_ShotInfo            stShotInfo;
} CAM_ImageBuffer;

// buffer requirement
typedef struct
{
CAM_ImageFormat         eFormat;
CAM_Int32s              iWidth;         // image width in pixels
CAM_Int32s              iHeight;        // image height in pixels
CAM_Int32s              iMinStep[3];    // image buffer's row stride in bytes
CAM_Int32s              iMinBufLen[3];
CAM_Int32s              iMinBufCount;
CAM_Int32s              iAlignment[3];  // alignment of each buffer start address
CAM_Int32s              iRowAlign[3];   // alignment of each row

CAM_Int32u              iFlagOptimal;
CAM_Int32u              iFlagAcceptable;
} CAM_ImageBufferReq;
*/

#define MAX_BUFCNT (20)

namespace android {

    //TODO:add time out strategy.
    class BufStatus{
        public:
            virtual ~BufStatus(){}
            BufStatus(){
                for(int i=0;i<MAX_BUFCNT;i++){
                    previewref[i]=0;
                    stillref[i]=0;
                    videoref[i]=0;
                    driverref[i]=0;
                }
            }
            void setFree(int i){
                previewref[i]=0;
                stillref[i]=0;
                videoref[i]=0;
                driverref[i]=0;
            }
            //buf setatus
            void toPreview(int i){
                if(++previewref[i] > MAX_BUFCNT){
                    previewref[i] = 0;
                    TRACE_E("%s:bufidx=%d, reach max ref, reset!",__FUNCTION__,i);
                }
            }
            void toStill(int i){
                if(++stillref[i] > MAX_BUFCNT){
                    stillref[i] = 0;
                    TRACE_E("%s:bufidx=%d, reach max ref, reset!",__FUNCTION__,i);
                }
            }
            void toDriver(int i){
                if(++driverref[i] > MAX_BUFCNT){
                    driverref[i] = 0;
                    TRACE_E("%s:bufidx=%d, reach max ref, reset!",__FUNCTION__,i);
                }
            }
            void toVideo(int i){
                if(++videoref[i] > MAX_BUFCNT){
                    videoref[i] = 0;
                    TRACE_E("%s:bufidx=%d, reach max ref, reset!",__FUNCTION__,i);
                }
            }
            void fromPreview(int i){
                previewref[i] = 0;
                /*
                   if(--previewref[i] < 0){
                   previewref[i] = 0;
                   TRACE_E("%s:bufidx=%d,less than 0, sth. wrong!",__FUNCTION__,i);
                   }
                   */
            }
            void fromStill(int i){
                stillref[i] = 0;
                /*
                   if(--stillref[i] < 0){
                   stillref[i] = 0;
                   TRACE_E("%s:bufidx=%d,less than 0, sth. wrong!",__FUNCTION__,i);
                   }
                   */
            }
            void fromDriver(int i){
                driverref[i] = 0;
                /*
                   if(--driverref[i] < 0){
                   driverref[i] = 0;
                   TRACE_E("%s:bufidx=%d,less than 0, sth. wrong!",__FUNCTION__,i);
                   }
                   */
            }
            void fromVideo(int i){
                videoref[i] = 0;
                /*
                   if(--videoref[i] < 0){
                   videoref[i] = 0;
                   TRACE_E("%s:bufidx=%d,less than 0, sth. wrong!",__FUNCTION__,i);
                   }
                   */
            }
            bool isFree(int i){
                if(previewref[i] == 0 && videoref[i] == 0 && stillref[i] == 0 &&
                        driverref[i] == 0){
                    return true;
                }
                else{
                    return false;
                }
            }
            void getRef(int i, int* preview, int* still, int* video, int* driver){
                *preview = previewref[i];
                *still = stillref[i];
                *video = videoref[i];
                *driver = driverref[i];
            }
            void getStatus(int i, int* previewcnt,
                    int* stillcnt, int* videocnt, int* drivercnt){
                *previewcnt = previewref[i];
                *stillcnt = stillref[i];
                *videocnt = videoref[i];
                *drivercnt = driverref[i];
            }

            void showStatus(int i){
                if (isFree(i)){
                    TRACE_D("idx=%d, free",i);
                }
                else{
                    TRACE_D("bufidx=%d, driver=%d, previewref=%d, stillref=%d, videoref=%d",
                            i, driverref[i], previewref[i], stillref[i],videoref[i]);
                }
            }

        private:
            //buffer reference cnt
            int previewref[MAX_BUFCNT];
            int stillref[MAX_BUFCNT];
            int videoref[MAX_BUFCNT];
            int driverref[MAX_BUFCNT];
    };//class BufStatus

    class ImageBuf: virtual public RefBase{
        public:
            class EngBufReq;
            class EngBuf;
            class EngBufReq: private CAM_ImageBufferReq{
                public:
                    EngBufReq(const CAM_ImageBufferReq& engbufreq):CAM_ImageBufferReq(engbufreq){
                    }
                    EngBufReq(){
                        eFormat=CAM_IMGFMT_LIMIT;
                        iWidth=0;
                        iHeight=0;
                        iMinBufCount=0;
                    }
                    status_t	getSize(int* w, int* h) const;
                    CAM_ImageFormat		getFormat() const;
                    void	showInfo() const;
                    status_t	getAlignment(int* align0, int* align1, int* align2) const;
                    status_t	getMinStep(int* step0, int* step1, int* step2) const;
                    int		getMinBufCount() const;
                    status_t	getMinBufLen(int* buflen0, int* buflen1, int* buflen2) const;
                    static EngBufReq getBufReq(const char* imageformat, Size size, int bufcnt);
            };

            class EngBuf: private CAM_ImageBuffer{
                public:
                    EngBuf(){
                        /*
                           eFormat=CAM_IMGFMT_LIMIT;
                           iWidth=0;
                           iHeight=0;
                           for(int i=0;i<3;i++){
                           iStep[i]=0;
                           iAllocLen[i]=0;
                           iFilledLen[i]=0;
                           iOffset[i]=0;
                           pBuffer[i]=0;
                           iPhyAddr[i]=0;
                           }
                           iTick=0;
                           iFlag=0;
                           iUserIndex=0;
                           pUserData=0;
                           iPrivateIndex=0;
                           pPrivateData=0;

                           bEnableShotInfo=CAM_FALSE;
                           */
                        CAM_ImageBuffer* parent = this;
                        memset(parent, 0, sizeof(CAM_ImageBuffer));
                    }

                    status_t	getSize(int* w, int* h) const;
                    status_t	getStep(int* step0, int* step1, int* step2) const;
                    status_t	getFilledLen(int* len0, int* len1, int* len2) const;
                    status_t	getAllocLen(int* len0, int* len1, int* len2) const;
                    status_t	getVAddr(unsigned char** vaddr0, unsigned char** vaddr1, unsigned char** vaddr2) const;
                    status_t	getSetp(int* step0, int* step1, int* step2)const;
                    const char*	getImageFormat() const;

                    status_t	getPAddr(unsigned long* paddr0, unsigned long* paddr1, unsigned long* paddr2) const;
                    int		getMinBufCount() const;
                    void	showInfo() const;

                    status_t	clearBuf();
                    status_t	setSize(int w, int h);
                    status_t	setFormat(CAM_ImageFormat format);
                    status_t	setPAddr(unsigned long paddr0, unsigned long paddr1, unsigned long paddr2);
                    status_t	setVAddr(unsigned char* const vaddr0,
                            unsigned char* const vaddr1, unsigned char* const vaddr2);
                    status_t	setFilledLen(int len0, int len1, int len2);
                    status_t	setStep(int step0, int step1, int step2);
                    status_t	setAllocLen(int len0,int len1,int len2);

                    status_t	setOffset(int offset0,int offset1,int offset2);
                    status_t	setUserIndex(int index);
                    status_t	setUserData(void* userdata);
                    status_t	setPrivateIndex(int index);
                    status_t	setPrivateData(void* privatedata);
                    status_t	setTick(long long tick);
                    status_t	setEnShotInfo(bool enshot);
                    status_t	setFlag(unsigned int flag);
                    CAM_ImageBuffer* getImageBuffer();
            };

        public:
            virtual ~ImageBuf();
            ImageBuf(const EngBufReq& bufreq, int bufcnt, int shared=0,  const char* BufNode="/dev/pmem");
            ImageBuf(const char* imageformat, Size size, int bufcnt, int shared=0, const char* BufNode="/dev/pmem");

            //utility to query buffer requirement for sepcified format & size.
            static ImageBuf::EngBufReq getBufReq(const char* imageformat, Size size, int bufcnt);

            int	    getBufIndex(const sp<IMemory>& mem);
            int	    getBufCnt(){return mBufCnt;}
            sp<IMemoryHeap> getBufHeap();
            sp<IMemoryHeap> getDupBufHeap();

            CAM_ImageBuffer* getEngBuf(int index);
            sp<IMemory>	getIMemory(int index);
            status_t flush(int index, dma_data_direction dir=DMA_TO_DEVICE);
            status_t getPhyAddr(int index, unsigned long* paddr0,
                    unsigned long* paddr1, unsigned long* paddr2);

            status_t getVirAddr(int index, unsigned char** vaddr0,
                    unsigned char** vaddr1, unsigned char** vaddr2);

            status_t getAllocLen(int index, int* len0, int* len1, int* len2);

            const char* getImageFormat(int index=0);

            status_t getImageSize(int index, int* w, int* h);

            status_t getStep(int index, int* step0, int* step1, int* step2);
            status_t getFilledLen(int index, int* len0, int* len1, int* len2);

            void setFree(int i){ bufstatus.setFree(i);}
            void toPreview(int i){bufstatus.toPreview(i);}
            void toStill(int i){bufstatus.toStill(i);}
            void toDriver(int i){bufstatus.toDriver(i);}
            void toVideo(int i){bufstatus.toVideo(i);}
            void fromPreview(int i){bufstatus.fromPreview(i);}
            void fromStill(int i){bufstatus.fromStill(i);}
            void fromDriver(int i){bufstatus.fromDriver(i);}
            void fromVideo(int i){bufstatus.fromVideo(i);}
            bool isFree(int i){return bufstatus.isFree(i);}
            void getRef(int i, int* preview, int* still, int* video, int* driver){
                bufstatus.getRef(i, preview, still, video, driver);
            }
            void getStatus(int i, int* previewcnt,
                    int* stillcnt, int* videocnt, int* drivercnt){
                bufstatus.getStatus(i, previewcnt,
                        stillcnt, videocnt, drivercnt);
            }
            void showStatus(int i){
                bufstatus.showStatus(i);
            }
            void showStatus(){
                TRACE_D("*****buf status*****");
                for(int i=0; i<mBufCnt; i++){
                    bufstatus.showStatus(i);
                }
            }

        private:
            status_t checkIndex(int index);

            status_t AllocImage(const EngBufReq& bufreq, int bufcnt, int shared=0, const char* BufNode="/dev/pmem");
            status_t AllocImage(const char* imageformat, Size size, int bufcnt, int shared=0, const char* BufNode="/dev/pmem");

            EngBuf	mEngBuf[MAX_BUFCNT];
            EngBufReq	mEngBufReq;

            int mBufCnt;

            size_t mAlignSize;//buffer start addr & buf length align
            int mBufLength;//actually used buf len
            int mBufLength_align;//allocated buf len

            //heap info (for all mem)
            sp<MemoryHeapBase>  mBufHeap;
            sp<MemoryHeapBase>  mDupBufHeap;
            int mFD_PMEM;
            String8 mBufNode;

            sp<IMemory>     mBuffers[MAX_BUFCNT];
            sp<IMemory>     mDupBuffers[MAX_BUFCNT];

            BufStatus bufstatus;

            int mShared;
    };//class ImageBuf

}; // namespace android

#endif
