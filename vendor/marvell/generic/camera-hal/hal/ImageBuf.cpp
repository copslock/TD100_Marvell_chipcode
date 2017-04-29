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

#include <linux/ioctl.h>
#include "cam_log_mrvl.h"

#include "ImageBuf.h"
#define LOG_TAG "ImageBuf"

namespace android {

#define ARRAY_SET(dest,src) \
    do{\
        dest[0]=src##0; \
        dest[1]=src##1; \
        dest[2]=src##2; \
    }while(0);

#define ARRAY_GET(dest,src) \
    do{\
        *(dest##0)=src[0]; \
        *(dest##1)=src[1]; \
        *(dest##2)=src[2]; \
    }while(0);

    ImageBuf::~ImageBuf()
    {
        //heap info (for all mem)
        for(int i=0;i<MAX_BUFCNT;i++){
            mBuffers[i].clear();
        }
        mBufHeap.clear();
        for(int i=0;i<MAX_BUFCNT;i++){
            mDupBuffers[i].clear();
        }
        mDupBufHeap.clear();
    }

    ImageBuf::ImageBuf(const char* imageformat, Size size, int bufcnt, int shared, const char* BufNode){
        mAlignSize = 0;
        mBufCnt = 0;
        mBufLength = 0;
        mBufLength_align = 0;
        mShared = shared;
        mFD_PMEM = 0;

        status_t res = AllocImage(imageformat, size, bufcnt, shared, BufNode);
        if(res != NO_ERROR){
            TRACE_E("Fatal Error: Fail to alloc buffer");
        }
    }

    ImageBuf::ImageBuf(const EngBufReq& bufreq, int bufcnt, int shared,  const char* BufNode){
        mAlignSize = 0;
        mBufCnt = 0;
        mBufLength = 0;
        mBufLength_align = 0;
        mShared = shared;
        mFD_PMEM = 0;

        status_t res = AllocImage(bufreq, bufcnt, shared, BufNode);
        if(res != NO_ERROR){
            TRACE_E("Fatal Error: Fail to alloc buffer");
        }
    }

    status_t ImageBuf::AllocImage(const EngBufReq& bufreq, int bufcnt, int shared,  const char* BufNode){
        mEngBufReq = bufreq;

        /*
         * manually de-refer the buffer before next allocation,
         * to avoid out-of-memory issue
         */
        mBufHeap.clear();
        for(int i=0;i<MAX_BUFCNT;i++){
            mBuffers[i].clear();
            /*
             * reset all buffer status
             */
            setFree(i);
        }
        for(int i=0;i<MAX_BUFCNT;i++){
            mDupBuffers[i].clear();
        }
        mDupBufHeap.clear();

        //check input variables
        int mincnt = bufreq.getMinBufCount();
        if(bufcnt < mincnt || bufcnt > MAX_BUFCNT){
            TRACE_E("Invalid bufcnt, required=%d, min=%d, max=%d",
                    bufcnt, mincnt, MAX_BUFCNT);
            return BAD_VALUE;
        }
        mBufCnt = bufcnt;

        int w,h;
        bufreq.getSize(&w,&h);
        if(0>=w || 0>=w){
            TRACE_E("Invalid size: %dx%d", w, h);
            return BAD_VALUE;
        }

        int align[3];
        bufreq.getAlignment(align+0,align+1,align+2);
        int engalign=align[0]; //TODO: only consider first plan start addr align
        int androidalign = (int)getpagesize();
        mAlignSize = androidalign>engalign? androidalign:engalign;//start addr align & buf length align

        //for android required IMemory interface, so no hole is allowed between plannar data,
        //so here we simply assume continuous buffer
        int minbuflen[3];
        bufreq.getMinBufLen(minbuflen+0,minbuflen+1,minbuflen+2);
        mBufLength = minbuflen[0] + minbuflen[1] + minbuflen[2];
        //consider buf length align & buf start addr align
        mBufLength_align = ((mBufLength + mAlignSize-1) & ~(mAlignSize-1)) + (mAlignSize);

        bufreq.showInfo();

        /*
         * Make a new pmem heap that can be shared across processes.
         * Two more buffers are reserved for color space conversion and
         * resize/rotate.
         */
        struct pmem_region region;
        mBufNode = String8(BufNode);
        if (0 != strcmp(BufNode, ""))
        {
            //v4l2 user-pointer requires pagealignment for DMA buf offset(and size)
            TRACE_D("%s:alloc imagebuf from %s",__FUNCTION__,BufNode);
            sp<MemoryHeapBase> base = new MemoryHeapBase(BufNode,
                    mBufLength_align * (mBufCnt));
            if (base->heapID() < 0) {
                TRACE_E("Failed to allocate imagebuf from %s", BufNode);
            }

            //TODO:consider non-pmem buffer
            mBufHeap = new MemoryHeapPmem(base, 0);
            mFD_PMEM = mBufHeap->getHeapID();

            if (ioctl(mFD_PMEM, PMEM_GET_PHYS, &region)) {
                TRACE_E("Failed to get imagebuf physical address");
            }

            if(mShared) {
                mDupBufHeap = new MemoryHeapPmem(base, 0);
                if (ioctl(mDupBufHeap->getHeapID(), PMEM_GET_PHYS, &region)) {
                    TRACE_E("Failed to get imagebuf physical address");
                }
            }
            /*
             * Make an IMemory for each streaming buffer so that we can reuse
             * them in callbacks.
             */
            for (int i = 0; i < mBufCnt; i++) {
                ssize_t offset = 0;
                size_t size = 0;
                if (mShared) {
                    mDupBuffers[i] = (static_cast<MemoryHeapPmem *>(mDupBufHeap.get()))->
                        mapMemory(i * mBufLength_align, mBufLength);
                }
                mBuffers[i] = (static_cast<MemoryHeapPmem *>(mBufHeap.get()))->
                    mapMemory(i * mBufLength_align, mBufLength);
                mBuffers[i]->getMemory(&offset, &size);

                /*
                 * as we assume android will not use discrete yuv buffers for camera case,
                 * we may ignore below entries for simplicity.
                 */
                //paddr
                int minbuflen[3];
                bufreq.getMinBufLen(minbuflen+0,minbuflen+1,minbuflen+2);

                unsigned long paddr[3];
                paddr[0] = (unsigned long)(region.offset + i * mBufLength_align);
                paddr[1] = paddr[0] + minbuflen[0];
                paddr[2] = paddr[1] + minbuflen[1];
                mEngBuf[i].setPAddr(paddr[0],paddr[1],paddr[2]);
                //vaddr
                unsigned char* vaddr[3];
                vaddr[0] = (unsigned char*)((unsigned char*)mBufHeap->base() + offset);
                vaddr[1] = (unsigned char*)((unsigned char*)(vaddr[0]) + minbuflen[0]);
                vaddr[2] = (unsigned char*)((unsigned char*)(vaddr[1]) + minbuflen[1]);
                mEngBuf[i].setVAddr(vaddr[0],vaddr[1],vaddr[2]);
            }
        }
        else{
            mFD_PMEM = 0;

            sp<MemoryHeapBase> base = new MemoryHeapBase( mBufLength_align * mBufCnt);
            mBufHeap = base;
            for (int i = 0; i < mBufCnt; i++) {
                ssize_t offset = 0;
                size_t size = 0;
                mBuffers[i] = new MemoryBase(base, i*mBufLength_align , mBufLength);
                //paddr
                mEngBuf[i].setPAddr(0,0,0);//no phyaddr available for ashmem
                //vaddr
                mBuffers[i]->getMemory(&offset, &size);

                int minbuflen[3];
                bufreq.getMinBufLen(minbuflen+0,minbuflen+1,minbuflen+2);

                unsigned char* vaddr[3];
                vaddr[0] = (unsigned char*)((unsigned char*)mBufHeap->base() + offset);//XXX
                vaddr[1] = (unsigned char*)((unsigned char*)(vaddr[0]) + minbuflen[0]);
                vaddr[2] = (unsigned char*)((unsigned char*)(vaddr[1]) + minbuflen[1]);
                mEngBuf[i].setVAddr(vaddr[0],vaddr[1],vaddr[2]);
            }
        }
        for (int i = 0; i < mBufCnt; i++) {
            //init other members
            mEngBuf[i].setFormat(bufreq.getFormat());

            int width,height;
            bufreq.getSize(&width, &height);
            mEngBuf[i].setSize(width, height);

            int step[3];
            bufreq.getMinStep(step+0, step+1, step+2);//TODO:check step
            mEngBuf[i].setStep(step[0],step[1],step[2]);

            int len[3];
            bufreq.getMinBufLen(len+0,len+1,len+2);
            mEngBuf[i].setAllocLen(len[0],len[1],len[2]);

            mEngBuf[i].setOffset(0,0,0);
            mEngBuf[i].setUserIndex(i);
            mEngBuf[i].setUserData(NULL);
            mEngBuf[i].setPrivateIndex(0);
            mEngBuf[i].setPrivateData(NULL);
            mEngBuf[i].setTick(0);
            mEngBuf[i].setEnShotInfo(CAM_TRUE);

            mEngBuf[i].setFlag(
                    BUF_FLAG_PHYSICALCONTIGUOUS |
                    BUF_FLAG_L1NONCACHEABLE |
                    BUF_FLAG_L2NONCACHEABLE |
                    BUF_FLAG_UNBUFFERABLE |
                    BUF_FLAG_YUVBACKTOBACK |
                    BUF_FLAG_FORBIDPADDING);

            mEngBuf[i].clearBuf();
            mEngBuf[i].showInfo();
        }
        return NO_ERROR;
    }

    ImageBuf::EngBufReq ImageBuf::getBufReq(const char* imageformat, Size size, int bufcnt){
        //construct internal image buf requirements
        EngBufReq bufreq = EngBufReq::getBufReq(imageformat, size, bufcnt);

        return bufreq;
    }

    status_t ImageBuf::AllocImage(const char* imageformat, Size size, int bufcnt, int shared, const char* BufNode){
        EngBufReq bufreq = getBufReq(imageformat, size, bufcnt);

        status_t res = AllocImage(bufreq, bufcnt, shared, BufNode);
        if(NO_ERROR != res){
            return res;
        }
        for(int i=0;i<bufcnt;i++){
            //FIXME: hardcoding filled length
            int len[3];
            mEngBuf[i].getAllocLen(len+0,len+1,len+2);
            mEngBuf[i].setFilledLen(len[0],len[1],len[2]);
        }
        return res;
    }

    status_t ImageBuf::checkIndex(int index){
        if(index > mBufCnt || index < 0){
            TRACE_E("invalid buf index, required %d, available [0-%d]",
                    index,mBufCnt);
            return BAD_VALUE;
        }
        return NO_ERROR;
    }

    sp<IMemory>	ImageBuf::getIMemory(int index)
    {
        if(NO_ERROR != checkIndex(index)){
            return NULL;
        }
        return mBuffers[index];
    }

    status_t ImageBuf::flush(int index, dma_data_direction dir)
    {
        /*
           DMA_BIDIRECTIONAL;
           DMA_TO_DEVICE;
           DMA_FROM_DEVICE;
           */
        if(NO_ERROR != checkIndex(index)){
            return NULL;
        }

        if(mFD_PMEM <= 0 || mBufNode == "/dev/pmem_adsp"){
            TRACE_V("%s:Invalid pmem node, return without flush",__FUNCTION__);
            return NO_ERROR;
        }
        //const char* fmt = mEngBuf[index].getImageFormat();
        const char* fmt = getImageFormat(index);
        if(strcmp(fmt, CameraParameters::PIXEL_FORMAT_YUV422I_UYVY) == 0 ||
                strcmp(fmt, CameraParameters::PIXEL_FORMAT_YUV420P) == 0 ||
                strcmp(fmt, CameraParameters::PIXEL_FORMAT_YUV422P) == 0){
            TRACE_V("%s:The image format don't need flush, return without flush",__FUNCTION__);
            return NO_ERROR;
        }

        ssize_t offset = 0;
        size_t size = 0;
        mBuffers[index]->getMemory(&offset, &size);

        struct pmem_sync_region psr;
        int ret;
        psr.region.offset = offset;
        psr.region.len = size;
        psr.dir = dir;
        TRACE_V("%s: region index=%d, offset=%ld, len=%d, dir=%d",__FUNCTION__,index,offset,size,dir);

        if (psr.dir == DMA_BIDIRECTIONAL) {
            ret = ioctl(mFD_PMEM, PMEM_CACHE_FLUSH, (unsigned long)&psr.region);
            if( ret < 0 ) {
                TRACE_E("PMEM_CACHE_FLUSH failed!");
                return INVALID_OPERATION;
            }
        }
        else{
            ret = ioctl(mFD_PMEM, PMEM_MAP_REGION, (unsigned long)&psr);
            if( ret < 0 ) {
                TRACE_E("PMEM_MAP_REGION failed!");
                return INVALID_OPERATION;
            }
        }
        return NO_ERROR;
    }

    int ImageBuf::getBufIndex(const sp<IMemory>& mem)
    {
        ssize_t offset = 0;
        size_t size = 0;
        int index;
#if 0
        //loop internal buffer
        const void* inptr = mem->pointer();
        for(int i=0;i<mBufCnt;i++){
            const void* localptr = mBuffers[i]->pointer();
            if( localptr == inptr){
                return i;
            }
        }
        TRACE_E("invalid buf index, required %d, available [0-%d]",
                index,mBufCnt);
        return -1;
#else
        //use imemory api
        mem->getMemory(&offset, &size);
        index = offset / mBufLength_align;
        if(NO_ERROR != checkIndex(index)){
            return -1;
        }
        return index;
#endif
    }

    sp<IMemoryHeap> ImageBuf::getBufHeap()
    {
        return mBufHeap;
    }

    sp<IMemoryHeap> ImageBuf::getDupBufHeap()
    {
        if(!mShared){
            TRACE_E("Not shared buf heap");
            return NULL;
        }
        return mDupBufHeap;
    }

    status_t ImageBuf::getPhyAddr(int index, unsigned long* paddr0,
            unsigned long* paddr1, unsigned long* paddr2){
        if(NO_ERROR != checkIndex(index)){
            return -1;
        }
        mEngBuf[index].getPAddr(paddr0,paddr1,paddr2);
        return NO_ERROR;
    }

    status_t ImageBuf::getVirAddr(int index, unsigned char** vaddr0,
            unsigned char** vaddr1, unsigned char** vaddr2){
        if(NO_ERROR != checkIndex(index)){
            return -1;
        }
        mEngBuf[index].getVAddr(vaddr0,vaddr1,vaddr2);
        return NO_ERROR;
    }

    status_t ImageBuf::getAllocLen(int index, int* len0,
            int* len1, int* len2){
        if(NO_ERROR != checkIndex(index)){
            return -1;
        }
        status_t ret = mEngBuf[index].getAllocLen(len0,len1,len2);
        return ret;
    }

    const char* ImageBuf::getImageFormat(int index){
        if(NO_ERROR != checkIndex(index)){
            return "";
        }
        const char* fmt = mEngBuf[index].getImageFormat();
        return fmt;
    }

    status_t ImageBuf::getImageSize(int index, int* w, int* h){
        if(NO_ERROR != checkIndex(index)){
            return -1;
        }
        mEngBuf[index].getSize(w,h);
        return NO_ERROR;
    }

    status_t ImageBuf::getStep(int index, int* step0, int* step1, int* step2){
        if(NO_ERROR != checkIndex(index)){
            return -1;
        }
        mEngBuf[index].getStep(step0,step1,step2);
        return NO_ERROR;
    }

    status_t ImageBuf::getFilledLen(int index, int* len0, int* len1, int* len2){
        if(NO_ERROR != checkIndex(index)){
            return -1;
        }
        mEngBuf[index].getFilledLen(len0,len1,len2);
        return NO_ERROR;
    }

    //TODO: actually this function broke the encapsulation of class ImageBuf.
    CAM_ImageBuffer* ImageBuf::getEngBuf(int index){
        if(NO_ERROR != checkIndex(index)){
            return NULL;
        }
        //CAM_ImageBuffer& engbuf = reinterpret_cast<CAM_ImageBuffer>(mEngBuf[index]);
        //CAM_ImageBuffer& engbuf = dynamic_cast<CAM_ImageBuffer&>(mEngBuf[index]);
        CAM_ImageBuffer* engbuf = mEngBuf[index].getImageBuffer();
        if(NULL == engbuf){
            TRACE_E("Fail to get engbuf");
            return NULL;
        }
        return engbuf;
    }

    /*******************************************************************************/
    status_t ImageBuf::EngBuf::setVAddr(unsigned char* const vaddr0,
            unsigned char* const vaddr1, unsigned char* const vaddr2){
        ARRAY_SET(pBuffer,vaddr);
        return NO_ERROR;
    }

    status_t ImageBuf::EngBuf::setPAddr(unsigned long paddr0,
            unsigned long paddr1, unsigned long paddr2){
        ARRAY_SET(iPhyAddr,paddr);
        return NO_ERROR;
    }

    status_t ImageBuf::EngBuf::getAllocLen(int* len0, int* len1, int* len2) const{
        ARRAY_GET(len,iAllocLen);
        return NO_ERROR;
    }

    status_t ImageBuf::EngBuf::setFilledLen(int len0, int len1, int len2){
        ARRAY_SET(iFilledLen,len);
        return NO_ERROR;
    }

    status_t ImageBuf::EngBuf::getPAddr(unsigned long* paddr0, unsigned long* paddr1, unsigned long* paddr2)const{
        ARRAY_GET(paddr,iPhyAddr);
        return NO_ERROR;
    }

    status_t ImageBuf::EngBuf::getVAddr(unsigned char** vaddr0, unsigned char** vaddr1, unsigned char** vaddr2)const{
        ARRAY_GET(vaddr,pBuffer);
        return NO_ERROR;
    }

    status_t ImageBuf::EngBuf::getSize(int* w, int* h)const{
        *w = iWidth;
        *h = iHeight;
        return NO_ERROR;
    }

    status_t ImageBuf::EngBuf::getSetp(int* step0, int* step1, int* step2)const{
        ARRAY_GET(step,iStep);
        return NO_ERROR;
    }

    status_t ImageBuf::EngBuf::getFilledLen(int* len0, int* len1, int* len2)const{
        ARRAY_GET(len,iFilledLen);
        return NO_ERROR;
    }
    status_t	ImageBuf::EngBuf::getStep(int* step0, int* step1, int* step2) const{
        ARRAY_GET(step,iStep);
        return NO_ERROR;
    }

    void ImageBuf::EngBuf::showInfo()const{
        TRACE_D("EngBuf: eFormat=%d,iWidth=%d,iHeight=%d,iStep=%d,%d,%d,iAllocLen=%d,%d,%d,iFilledLen=%d,%d,%d,iOffset=%d,%d,%d,pBuffer=%p,%p,%p,iPhyAddr=%x,%x,%x,iFlag=%x,iUserIndex=%d,pUserData=%p,iPrivateIndex=%d,pPrivateData=%p,bEnableShotInfo=%d",
                eFormat,iWidth,iHeight,iStep[0],iStep[1],iStep[2],iAllocLen[0],iAllocLen[1],iAllocLen[2],iFilledLen[0],iFilledLen[1],iFilledLen[2],iOffset[0],iOffset[1],iOffset[2],pBuffer[0],pBuffer[1],pBuffer[2],iPhyAddr[0],iPhyAddr[1],iPhyAddr[2],iFlag,iUserIndex,pUserData,iPrivateIndex,pPrivateData,bEnableShotInfo);
        return;
    }

    status_t ImageBuf::EngBuf::clearBuf(){
        //attention to buffer range.
        for(int i=0;i<3;i++){
            if(iAllocLen[i] < 0 || iAllocLen[i] >= 10*1024*1024){
                TRACE_E("ERROR! AllocLen[%d]=%d Invalid",i,iAllocLen[i]);
                return BAD_VALUE;
            }
        }
        memset((void*)pBuffer[0], 0, iAllocLen[0]);
        memset((void*)pBuffer[1], 0, iAllocLen[1]);
        memset((void*)pBuffer[2], 0, iAllocLen[2]);
        return NO_ERROR;
    }

    status_t	ImageBuf::EngBuf::setFormat(CAM_ImageFormat format){
        eFormat = format;
        return NO_ERROR;
    }

    status_t	ImageBuf::EngBuf::setSize(int w, int h){
        iWidth = w;
        iHeight = h;
        return NO_ERROR;
    }

    status_t	ImageBuf::EngBuf::setStep(int step0, int step1, int step2){
        ARRAY_SET(iStep,step);
        return NO_ERROR;
    }
    status_t	ImageBuf::EngBuf::setAllocLen(int len0,int len1,int len2){
        ARRAY_SET(iAllocLen,len);
        return NO_ERROR;
    }

    status_t	ImageBuf::EngBuf::setOffset(int offset0,int offset1,int offset2){
        ARRAY_SET(iOffset,offset);
        return NO_ERROR;
    }
    status_t	ImageBuf::EngBuf::setUserIndex(int index){
        iUserIndex = index;
        return NO_ERROR;
    }
    status_t	ImageBuf::EngBuf::setUserData(void* userdata){
        pUserData = userdata;
        return NO_ERROR;
    }
    status_t	ImageBuf::EngBuf::setPrivateIndex(int index){
        iPrivateIndex = index;
        return NO_ERROR;
    }
    status_t	ImageBuf::EngBuf::setPrivateData(void* privatedata){
        pPrivateData = privatedata;
        return NO_ERROR;
    }
    status_t	ImageBuf::EngBuf::setTick(long long tick){
        iTick = tick;
        return NO_ERROR;
    }
    status_t	ImageBuf::EngBuf::setEnShotInfo(bool enshot){
        bEnableShotInfo = enshot;
        return NO_ERROR;
    }
    status_t	ImageBuf::EngBuf::setFlag(unsigned int flag){
        iFlag=flag;
        return NO_ERROR;
    }
    CAM_ImageBuffer* ImageBuf::EngBuf::getImageBuffer(){
        return dynamic_cast<CAM_ImageBuffer*>(this);
    }
    const char* ImageBuf::EngBuf::getImageFormat()const{
        const char* fmt="";
        switch (eFormat){
            case CAM_IMGFMT_RGB565:
                fmt = (CameraParameters::PIXEL_FORMAT_RGB565);
                break;
            case CAM_IMGFMT_YCbCr420P:
                fmt = (CameraParameters::PIXEL_FORMAT_YUV420P);
                break;
            case CAM_IMGFMT_YCrCb420SP:
                fmt = (CameraParameters::PIXEL_FORMAT_YUV420SP); //NV21
                break;
                /*
                   case CAM_IMGFMT_YCbCr420SP:
                   fmt = (CameraParameters::PIXEL_FORMAT_YUV420SP); //NV12
                   break;
                   */
            case CAM_IMGFMT_CbYCrY:
                fmt = (CameraParameters::PIXEL_FORMAT_YUV422I_UYVY);
                break;
            case CAM_IMGFMT_YCbCr422P:
                fmt = (CameraParameters::PIXEL_FORMAT_YUV422P);
                break;
            case CAM_IMGFMT_JPEG:
                fmt = (CameraParameters::PIXEL_FORMAT_JPEG);
                break;
            default:
                TRACE_E("Invalid format, %d", eFormat);
        }
        return fmt;
    }

    /************************************************************************/

    ImageBuf::EngBufReq ImageBuf::EngBufReq::getBufReq(const char* imageformat,Size size, int bufcnt){
        //check image format & init image basic info
        const char* fmt_str = imageformat;
        EngBufReq bufreq;
        if(strcmp(fmt_str,CameraParameters::PIXEL_FORMAT_YUV420P)==0){
            bufreq.eFormat = CAM_IMGFMT_YCbCr420P;
            bufreq.iMinStep[0] =	size.width;
            bufreq.iMinStep[1] =	size.width / 2;
            bufreq.iMinStep[2] =	size.width / 2;
            bufreq.iMinBufLen[0] =	size.width * size.height;
            bufreq.iMinBufLen[1] =	bufreq.iMinBufLen[0]/4;
            bufreq.iMinBufLen[2] =	bufreq.iMinBufLen[0]/4;
        }
        else if(strcmp(fmt_str, CameraParameters::PIXEL_FORMAT_YUV420SP)==0){
            bufreq.eFormat = CAM_IMGFMT_YCrCb420SP;
            bufreq.iMinStep[0] =	size.width;
            bufreq.iMinStep[1] =	size.width;
            bufreq.iMinStep[2] =	0;
            bufreq.iMinBufLen[0] =	size.width * size.height;
            bufreq.iMinBufLen[1] =	bufreq.iMinBufLen[0]/2;
            bufreq.iMinBufLen[2] =	0;
        }
        else if(strcmp(fmt_str, CameraParameters::PIXEL_FORMAT_YUV422I_UYVY)==0){
            bufreq.eFormat = CAM_IMGFMT_CbYCrY;
            bufreq.iMinStep[0] =	size.width * 2;
            bufreq.iMinStep[1] =	0;
            bufreq.iMinStep[2] =	0;
            bufreq.iMinBufLen[0] =	size.width * size.height * 2;
            bufreq.iMinBufLen[1] =	0;
            bufreq.iMinBufLen[2] =	0;
        }
        else if(strcmp(fmt_str, CameraParameters::PIXEL_FORMAT_YUV422P)==0){
            bufreq.eFormat = CAM_IMGFMT_YCbCr422P;
            bufreq.iMinStep[0] =	size.width;
            bufreq.iMinStep[1] =	size.width/2;
            bufreq.iMinStep[2] =	size.width/2;
            bufreq.iMinBufLen[0] =	size.width * size.height;
            bufreq.iMinBufLen[1] =	bufreq.iMinBufLen[0]/2;
            bufreq.iMinBufLen[2] =	bufreq.iMinBufLen[0]/2;
        }
        else if(strcmp(fmt_str, CameraParameters::PIXEL_FORMAT_RGB565)==0){
            bufreq.eFormat = CAM_IMGFMT_RGB565;
            bufreq.iMinStep[0] =	size.width * 2;
            bufreq.iMinStep[1] =	0;
            bufreq.iMinStep[2] =	0;
            bufreq.iMinBufLen[0] =	size.width * size.height * 2;
            bufreq.iMinBufLen[1] =	0;
            bufreq.iMinBufLen[2] =	0;
        }
        else if(strcmp(fmt_str, CameraParameters::PIXEL_FORMAT_JPEG)==0){
            bufreq.eFormat = CAM_IMGFMT_JPEG;
            bufreq.iMinStep[0] =	0;
            bufreq.iMinStep[1] =	0;
            bufreq.iMinStep[2] =	0;
            bufreq.iMinBufLen[0] =	size.width * size.height / 2;//TODO:experience value
            bufreq.iMinBufLen[1] =	0;
            bufreq.iMinBufLen[2] =	0;
        }
        else{
            TRACE_E("Invalid imageformat: %s", fmt_str);
        }

        //init other parameters
        {
            bufreq.iWidth =	size.width;
            bufreq.iHeight =	size.height;
            bufreq.iMinBufCount = bufcnt;//TODO
        }
        return bufreq;
    }


    int ImageBuf::EngBufReq::getMinBufCount()const{
        return iMinBufCount;
    }

    status_t ImageBuf::EngBufReq::getSize(int* w, int* h)const{
        *w = iWidth;
        *h = iHeight;
        return NO_ERROR;
    }

    status_t ImageBuf::EngBufReq::getAlignment(int* align0, int* align1, int* align2)const{
        ARRAY_GET(align,iAlignment);
        return NO_ERROR;
    }

    status_t ImageBuf::EngBufReq::getMinBufLen(int* buflen0, int* buflen1, int* buflen2)const{
        ARRAY_GET(buflen,iMinBufLen);
        return NO_ERROR;
    }

    CAM_ImageFormat ImageBuf::EngBufReq::getFormat()const{
        return eFormat;
    }

    status_t	ImageBuf::EngBufReq::getMinStep(int* step0, int* step1, int* step2)const{
        ARRAY_GET(step,iMinStep);
        return NO_ERROR;
    }

    void ImageBuf::EngBufReq::showInfo()const{
        TRACE_D("EngBufReq: eFormat=%d,iWidth=%d,iHeight=%d,iMinStep=%d,%d,%d,iMinBufLen=%d,%d,%d,iMinBufCount=%d,iAlignment=%d,%d,%d,iRowAlign=%d,%d,%d,iFlagOptimal=%d,iFlagAcceptable=%d",
                eFormat, iWidth, iHeight, iMinStep[0], iMinStep[1], iMinStep[2], iMinBufLen[0], iMinBufLen[1], iMinBufLen[2], iMinBufCount, iAlignment[0], iAlignment[1], iAlignment[2], iRowAlign[0], iRowAlign[1], iRowAlign[2], iFlagOptimal, iFlagAcceptable);
        return;
    }

}; //namespace android
