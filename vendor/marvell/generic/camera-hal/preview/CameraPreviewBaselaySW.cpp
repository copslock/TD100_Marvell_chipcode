//#define LOG_NDEBUG 0
#define LOG_TAG "CameraPreviewBaselaySW"
#include "cam_log_mrvl.h"

#include "CameraPreviewBaselaySW.h"
#include "CameraHardware.h"
#include "ippIP.h"

extern "C" IppStatus ippiYUV420ToBGR565_8u16u_P3C3R(const Ipp8u* const pSrc[3], int srcStep[3], Ipp16u* pDst, int dstStep, IppiSize roiSize);

namespace android {

    CameraPreviewBaselaySW::CameraPreviewBaselaySW(wp<CameraHardwareInterface> camera)
        : mRecordingOn(false),
        mPreviewCallback(0),
        mPreviewCallbackCookie(0),
        mImageBuf(NULL)
    {
        mCameraHardware = camera;
#ifdef DEBUG_BASE_RGB565
        mRGB565ImageBuf.clear();
        mRGB565ImageBuf= new ImageBuf();
#endif
        updateBaseBuffer();
    }

    CameraPreviewBaselaySW::~CameraPreviewBaselaySW()
    {
        TRACE_D("%s", __FUNCTION__);
#ifdef DEBUG_BASE_RGB565
        mRGB565ImageBuf.clear();
#endif
    }

    status_t CameraPreviewBaselaySW::display(const sp<ImageBuf>& imagebuf_src, int i)
    {
        sp<CameraHardware> hardware = getInstance(mCameraHardware);
        if (hardware == 0) {
            TRACE_E("CameraHardware instace already destroyed,promote failed!");
            return UNKNOWN_ERROR;
        }

        TRACE_V("display(): display buffer #%d", i);

#ifdef DEBUG_BASE_RGB565
        //use rgb565 baselaydisplay to debug graphic path.
        //always convert yuv420p to rgb565.
        sp<ImageBuf>    ImageBuf_base=hardware->mPreviewImageBuf_base;
        yuv420ToRGB565(ImageBuf_base, 0, imagebuf_src, i);

        ImageBuf_base->flush(0);
        mPreviewCallback(CAMERA_MSG_PREVIEW_FRAME, ImageBuf_base->getIMemory(0), mPreviewCallbackCookie);
        TRACE_V("display(): return buffer #%d", i);
        hardware->releasePreviewBuffer(i);
        return NO_ERROR;
#endif

        /*
         * 1>yuv420p/uyvy: use preview buffer directly, callback without conversion.
         * 2>yuv420sp: memcpy to special baseheap buffer, surface do not support this
         * format temporarily, convert to yuv420p before callback.
         * 3>rgb565: memcpy to special baseheap buffer, yuv->rgb color conversion
         * before callback.
         */
        const char* fmt = imagebuf_src->getImageFormat(i);
        if(strcmp(fmt, CameraParameters::PIXEL_FORMAT_YUV420P)==0 ||
                strcmp(fmt,CameraParameters::PIXEL_FORMAT_YUV420SP)==0 ||
                strcmp(fmt, CameraParameters::PIXEL_FORMAT_YUV422I_UYVY)==0){
            mPreviewCallback(CAMERA_MSG_PREVIEW_FRAME, imagebuf_src->getIMemory(i), mPreviewCallbackCookie);
            TRACE_V("display(): return buffer #%d", i);
            hardware->releasePreviewBuffer(i);
            return NO_ERROR;
        }

        ssize_t offset = 0;
        size_t size = 0;

        //FIXME:
        sp<ImageBuf> imagebuf_dst = hardware->mPreviewImageBuf_base;

        //dst buffer vaddr
        unsigned char* vaddr_dst[3];
        imagebuf_dst->getVirAddr(0,vaddr_dst+0,vaddr_dst+1,vaddr_dst+2);
        TRACE_V("dst vaddr: %x,%x,%x",vaddr_dst[0],vaddr_dst[1],vaddr_dst[2]);

        unsigned char* vaddr_src[3];
        imagebuf_src->getVirAddr(i,vaddr_src+0,vaddr_src+1,vaddr_src+2);
        TRACE_V("src vaddr: %x,%x,%x",vaddr_src[0],vaddr_src[1],vaddr_src[2]);

        int step[3];
        imagebuf_src->getStep(i,step+0,step+1,step+2);

        int width,height;
        imagebuf_src->getImageSize(i, &width, &height);

        if(strcmp(fmt, CameraParameters::PIXEL_FORMAT_YUV420SP)==0)
        {
            //convert from 420sp to 420p, so that the surface of our GC can support.
            int ysize=width * height;
            int	 ret;
            Ipp8u *pSrc[3], *pDst[3];
            int srcStep[3], dstStep[3];
            IppiSize  roiSize;

            //
            pSrc[0] = (Ipp8u*)vaddr_src[0];
            pSrc[1] = (Ipp8u*)vaddr_src[1];
            pSrc[2] = (Ipp8u*)vaddr_src[2];

            pDst[0] = (Ipp8u*)(vaddr_dst[0]);
            //NOTES:as we have only YCbCr420SP->YUV420P conversion, so we just swap the buffer ptr.
            //FIXME: need to consider the step here?
            pDst[2] = pDst[0]+ysize;
            pDst[1] = pDst[2]+(ysize>>2);

            srcStep[0] = step[0];
            srcStep[1] = step[1];

            //FIXME: need to define step conception in Overlay
            dstStep[0] = srcStep[0];
            dstStep[1] = dstStep[2] = srcStep[1]>>1;
            roiSize.height = height;
            roiSize.width = width;
            ret = ippiYCbCr420SPToYCbCr420P_8u_P2P3R( (const Ipp8u**)pSrc, srcStep, pDst, dstStep, roiSize );
            if( ret != ippStsNoErr ){
                TRACE_E("ippiYCbCr420SPToYCbCr420P_8u_P2P3R return error");
                return -EINVAL;
            }
        }
        else if(strcmp(fmt,CameraParameters::PIXEL_FORMAT_RGB565)==0){
            int len=width * height * 2;
            memcpy(vaddr_dst[0], vaddr_src[0], len);
        }
        else{
            TRACE_E("Known display format.");
            return UNKNOWN_ERROR;
        }

        imagebuf_dst->flush(0);
        mPreviewCallback(CAMERA_MSG_PREVIEW_FRAME, imagebuf_dst->getIMemory(0), mPreviewCallbackCookie);
        TRACE_V("display(): return buffer #%d", i);
        hardware->releasePreviewBuffer(i);
        return NO_ERROR;
    }

    status_t CameraPreviewBaselaySW::startRecording()
    {
        mRecordingOn = true;
        return NO_ERROR;
    }

    status_t CameraPreviewBaselaySW::stopRecording()
    {
        mRecordingOn = false;
        return NO_ERROR;
    }

    status_t CameraPreviewBaselaySW::setPreviewCallback(data_callback cb,
            void *user,
            const sp<ImageBuf> imagebuf)
    {
        mPreviewCallback = cb;
        mPreviewCallbackCookie = user;
        mImageBuf = imagebuf;

        return updateBaseBuffer();
    }

    status_t CameraPreviewBaselaySW::updateBaseBuffer()
    {
        sp<CameraHardware> hardware = getInstance(mCameraHardware);
        if (hardware == 0) {
            TRACE_E("CameraHardware instace already destroyed,promote failed!");
            return UNKNOWN_ERROR;
        }

        int width, height;
        CameraParameters params(hardware->getParameters());
        params.getPreviewSize(&width, &height);

#ifdef DEBUG_BASE_RGB565
        if(mRGB565ImageBuf == NULL){
            TRACE_E("mRGB565ImageBuf is invalid.");
        }
        mRGB565ImageBuf.clear();
        mRGB565ImageBuf = new ImageBuf(
                CameraParameters::PIXEL_FORMAT_RGB565,
                Size(width,height),
                1,
                0
                );//use ashmem for baselayer preview
#endif

        TRACE_D("%s:update preview buffer (%dx%d)", __FUNCTION__, width, height);
        return NO_ERROR;
    }


    status_t CameraPreviewBaselaySW::yuv420ToRGB565(sp<ImageBuf>& buffer_dst, int j, const sp<ImageBuf>& imagebuf_src, int i)
    {
        //source buffer info
        int width,height;
        imagebuf_src->getImageSize(i, &width, &height);

        Ipp8u *pSrc[3];
        imagebuf_src->getVirAddr(i,(unsigned char **)(pSrc+0),(unsigned char **)(pSrc+1),(unsigned char **)(pSrc+2));

        int srcStep[3] = {width, width>>1, width>>1};
        IppiSize roiSize = {width, height};

        //dest buffer info
        unsigned char* vaddr[3];
        mRGB565ImageBuf->getVirAddr(j,vaddr+0,vaddr+1,vaddr+2);

        int step[3];
        mRGB565ImageBuf->getStep(j,step+0,step+1,step+2);

        //color conversion
        ippiYUV420ToBGR565_8u16u_P3C3R(pSrc, srcStep, (Ipp16u*)(vaddr[0]), step[0], roiSize);

        //
        Ipp8u *pDest[3];
        buffer_dst->getVirAddr(j,(unsigned char **)(pDest+0),(unsigned char **)(pDest+1),(unsigned char **)(pDest+2));
        memcpy(pDest[0], vaddr[0], width*height*2);

        return NO_ERROR;

    }
}; // namespace android
