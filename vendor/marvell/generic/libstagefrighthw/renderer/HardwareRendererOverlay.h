/***************************************************************************************** 
 * Copyright (c) 2009, Marvell International Ltd. 
 * All Rights Reserved.
 *****************************************************************************************/

#ifndef HARDWARE_RENDERER_OVERLAY_H_

#define HARDWARE_RENDERER_OVERLAY_H_

#include <media/stagefright/VideoRenderer.h>
#include <surfaceflinger/ISurface.h>
#include <ui/Overlay.h>
#include <utils/RefBase.h>
#include <OMX_Component.h>

#include <media/stagefright/MediaBuffer.h>
#define BUF_COUNT 5

#define PERF

namespace android {

class ISurface;
class MemoryHeapBase;

/**
 * Each rendering data be tracked by
 * this struct.
 */
class HardwareRendererOverlay : public VideoRenderer {
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
            HardwareRendererOverlay *renderer;
            void *data;
            size_t data_size;
            // use below hook to do more cleanup work, i.e. free bmm mem.
            void (*hook)(void *priv);
            void *hook_priv;
    };
public:
    static HardwareRendererOverlay * CreateInstance( const char *componentName,
            OMX_COLOR_FORMATTYPE colorFormat,
            const sp<ISurface> &surface,
            size_t displayWidth, size_t displayHeight,
            size_t decodedWidth, size_t decodedHeight,
            int32_t rotationDegrees);

    HardwareRendererOverlay(
            const char *componentName,
            OMX_COLOR_FORMATTYPE colorFormat,
            const sp<ISurface> &surface,
            size_t displayWidth, size_t displayHeight,
            size_t decodedWidth, size_t decodedHeight,
            int32_t rotationDegrees);

    bool initCheck();

    virtual ~HardwareRendererOverlay();

    virtual void render(
        const void *data, size_t size, void *platformPrivate, sp<MemoryHeapBase> memoryHeap=NULL);
    //called back by overlay-hal per buffer display done
    static void displayDone( void *renderBuf );
    void getDisplayInfo(OMX_COLOR_FORMATTYPE &ColorFormat, size_t &DisplayWidth, size_t &DisplayHeight, size_t &DecodedWidth,size_t &DecodedHeight);
    void Reconfig(OMX_COLOR_FORMATTYPE ColorFormat, size_t DisplayWidth, size_t DisplayHeight, size_t DecodedWidth, size_t DecodedHeight);

private:
    const char *mComponentName;
    OMX_COLOR_FORMATTYPE mColorFormat;
    sp<ISurface> mISurface;
    size_t mDisplayWidth, mDisplayHeight;
    size_t mDecodedWidth, mDecodedHeight;
    int32_t mRotationDegrees;
	sp<OverlayRef> mOverlayRef;
	sp<Overlay>    mOverlay;
    int mNumberOfFramesAlreadySent;
    //data being displayed
    void *virtDataInDisplay;
    void *physDataInDisplay;

    HardwareRendererOverlay(const HardwareRendererOverlay &);
    HardwareRendererOverlay &operator=(const HardwareRendererOverlay &);
    int hasMrvlExtension();
#ifdef PERF
    long long time_tot_base;
    int frame_num_base;
    struct timespec t_start_base;
    struct timespec t_stop_base;
    bool bStartPerf;
#endif
};

}  // namespace android

#endif  // HARDWARE_RENDERER_OVERLAY_H_
