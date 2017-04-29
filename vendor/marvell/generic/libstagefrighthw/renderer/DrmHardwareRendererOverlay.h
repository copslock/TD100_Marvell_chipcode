/*****************************************************************************************
 * Copyright (c) 2009, Marvell International Ltd.
 * All Rights Reserved.
 *****************************************************************************************/

#ifndef DRMHARDWARE_RENDERER_OVERLAY_H_

#define DRMHARDWARE_RENDERER_OVERLAY_H_

#include <media/stagefright/VideoRenderer.h>
#include <surfaceflinger/ISurface.h>
#include <ui/Overlay.h>
#include <utils/RefBase.h>
#include <OMX_Component.h>

#include <media/stagefright/MediaBuffer.h>

#define PERF


#define ALIGN16(m)  (((m + 15)/16) * 16)
#define ALIGN4(m)  (((m + 3)/4) * 4)

namespace android {


class ISurface;
/**
 * Each rendering data be tracked by
 * this struct.
 */
class DrmHardwareRendererOverlay : public VideoRenderer {
private:
    class RenderBufferInfo {
        public:
            RenderBufferInfo(){
                renderer = NULL;
                data = NULL;
                data_size = 0;
                hook = NULL;
                hook_priv = NULL;
            }
            DrmHardwareRendererOverlay *renderer;
            void *data;
            size_t data_size;
            // use below hook to do more cleanup work, i.e. free bmm mem.
            void (*hook)(void *priv);
            void *hook_priv;
    };

public:
    static DrmHardwareRendererOverlay * CreateInstance( const char *componentName,
            OMX_COLOR_FORMATTYPE colorFormat,
            const sp<ISurface> &surface,
            size_t displayWidth, size_t displayHeight,
            size_t decodedWidth, size_t decodedHeight);

    DrmHardwareRendererOverlay(
            const char *componentName,
            OMX_COLOR_FORMATTYPE colorFormat,
            const sp<ISurface> &surface,
            size_t displayWidth, size_t displayHeight,
            size_t decodedWidth, size_t decodedHeight);

    bool initCheck();

    bool createOverlay();
    void destroyOverlay();

    virtual ~DrmHardwareRendererOverlay();

    virtual void render(
        const void *data, size_t size, void *platformPrivate, sp<MemoryHeapBase> memoryHeap=NULL);
    //called back by overlay-hal per buffer display done
    static void displayDone( void *renderBuf );
    void getDisplayInfo(OMX_COLOR_FORMATTYPE &ColorFormat, size_t &DisplayWidth, size_t &DisplayHeight, size_t &DecodedWidth,size_t &DecodedHeight);
    void Reconfig(OMX_COLOR_FORMATTYPE ColorFormat, size_t DisplayWidth, size_t DisplayHeight, size_t DecodedWidth, size_t DecodedHeight, bool recreateOvly);

private:
    const char *mComponentName;
    OMX_COLOR_FORMATTYPE mOMXColorFmt;
    sp<ISurface> mISurface;
    size_t mDisplayWidth, mDisplayHeight;
    size_t mDecodedWidth, mDecodedHeight;
	sp<OverlayRef> mOverlayRef;
	sp<Overlay>    mOverlay;
    int mNumberOfFramesAlreadySent;
    //data being displayed
    void *virtDataInDisplay;
    void *physDataInDisplay;

    DrmHardwareRendererOverlay(const DrmHardwareRendererOverlay &);
    DrmHardwareRendererOverlay &operator=(const DrmHardwareRendererOverlay &);
    int hasMrvlExtension();

    int _getOvlyFmtFromOMXFmt( OMX_COLOR_FORMATTYPE omxFmt );

#ifdef PERF
    long time_tot_base;
    int frame_num_base;
    struct timespec t_start_base;
    struct timespec t_stop_base;
    bool bStartPerf;
#endif
};



}  // namespace android

#endif  // DRMHARDWARE_RENDERER_OVERLAY_H_
