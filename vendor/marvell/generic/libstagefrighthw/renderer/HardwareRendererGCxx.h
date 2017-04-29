#ifndef HARDWARE_RENDERER_GC_H_

#define HARDWARE_RENDERER_GC_H_

#include <media/stagefright/VideoRenderer.h>
#include <utils/RefBase.h>
#include <OMX_Component.h>

#define PERF

namespace android {

class ISurface;
class MemoryHeapBase;

typedef struct {
    void * pSurface;
    uint8_t*     pVirAddr;
    uint32_t     nPhyAddr;
}GCUSurfaceInfo;

class HardwareRendererGC : public VideoRenderer {
public:
    static HardwareRendererGC * CreateInstance(
            OMX_COLOR_FORMATTYPE colorFormat,
            const sp<ISurface> &surface,
            size_t displayWidth, size_t displayHeight,
            size_t decodedWidth, size_t decodedHeight, int32_t rotationDegrees =0 );

    HardwareRendererGC(
            OMX_COLOR_FORMATTYPE colorFormat,
            const sp<ISurface> &surface,
            size_t displayWidth, size_t displayHeight,
            size_t decodedWidth, size_t decodedHeight,
            int32_t rotationDegrees = 0);

    virtual ~HardwareRendererGC();

    status_t initCheck() const;

    virtual void render(
        const void *data, size_t size, void *platformPrivate, sp<MemoryHeapBase> memoryHeap=NULL);

private:
    status_t mInitCheck;
    OMX_COLOR_FORMATTYPE mColorFormat;
    sp<ISurface> mISurface;
    size_t mDisplayWidth, mDisplayHeight;
    size_t mDecodedWidth, mDecodedHeight;
    size_t mFrameSize;
    sp<MemoryHeapBase> mMemoryHeap;
    int mIndex;

    HardwareRendererGC(const HardwareRendererGC &);
    HardwareRendererGC &operator=(const HardwareRendererGC &);
    int gcu_frame_color_convert (uint8_t* aSrcData,uint32_t aSrcPhyAddress, uint8_t* aDstData, uint32_t aDstPhyAddress);
    void *pGcuContext;
    GCUSurfaceInfo SrcSurfaceInfoQ[16];
    GCUSurfaceInfo DstSurfaceInfoQ[16];
    int32_t iGCUDstVideo_W;
    int32_t iGCUDstVideo_H; 
    uint32_t GCUOutputColorFormat;
    sp<MemoryHeapBase> mBackupMemoryHeap;
#ifdef PERF
    long time_tot_base;
    int frame_num_base;
    struct timespec t_start_base;
    struct timespec t_stop_base;
    bool bStartPerf;
#endif
};

class HardwareRendererDirectGC : public VideoRenderer {
public:
    static HardwareRendererDirectGC * CreateInstance(
            OMX_COLOR_FORMATTYPE colorFormat,
            const sp<ISurface> &surface,
            size_t displayWidth, size_t displayHeight,
            size_t decodedWidth, size_t decodedHeight, int32_t rotationDegrees =0 );

    HardwareRendererDirectGC(
            OMX_COLOR_FORMATTYPE colorFormat,
            const sp<ISurface> &surface,
            size_t displayWidth, size_t displayHeight,
            size_t decodedWidth, size_t decodedHeight,
            int32_t rotationDegrees = 0);

    virtual ~HardwareRendererDirectGC();

    status_t initCheck() const;

    virtual void render(
        const void *data, size_t size, void *platformPrivate, sp<MemoryHeapBase> memoryHeap); 

private:
    status_t mInitCheck;
    OMX_COLOR_FORMATTYPE mColorFormat;
    sp<ISurface> mISurface;
    size_t mDisplayWidth, mDisplayHeight;
    size_t mDecodedWidth, mDecodedHeight;
    size_t mFrameSize;
    sp<MemoryHeapBase> mMemoryHeap;
    int mIndex;

    HardwareRendererDirectGC(const HardwareRendererDirectGC &);
    HardwareRendererDirectGC &operator=(const HardwareRendererDirectGC &);
    int32_t iGCUDstVideo_W;
    int32_t iGCUDstVideo_H; 
    uint32_t GCUOutputColorFormat;
    uint32_t orientation;
    bool memoryHeap_registered;
    sp<MemoryHeapBase> mBackupMemoryHeap;
#ifdef PERF
    long time_tot_base;
    int frame_num_base;
    struct timespec t_start_base;
    struct timespec t_stop_base;
    bool bStartPerf;
#endif
};


}  // namespace android

#endif  // HARDWARE_RENDERER_GC_H_

