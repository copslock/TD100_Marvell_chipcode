#ifndef CAMERA_EXIF_HELPER_HEADER
#define CAMERA_EXIF_HELPER_HEADER

#define   _ALIGN_TO(x,iAlign) ( (((Ipp32s)(x)) + (iAlign) - 1) & (~((iAlign) - 1)) )

namespace android {

    typedef enum
    {
        // Low->high  R,G,B
        ippExif_IMGFMT_RGB888  = 2000,

        ippExif_IMGFMT_YCC420P = 3000,
        ippExif_IMGFMT_YCC422P,

        ippExif_IMGFMT_JPEG    = 4000,
    }Exif_ImageFormat;

    sp<MemoryBase> ExifGenerator(const CAM_ImageBuffer* const pImgBuf_still, const CameraSetting& setting);

};

#endif
