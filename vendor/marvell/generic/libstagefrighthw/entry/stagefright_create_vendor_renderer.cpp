#include "HardwareRendererOverlay.h"
#include "HardwareRendererGCxx.h"
#include "DrmHardwareRendererOverlay.h"
#include <cutils/properties.h>
#include <media/stagefright/VideoRenderer.h>

// valid value of PROP_VIDEO_ACCELERATE_HW
// overlay : video out put use overlay
// gc : video out put with gc
#define PROP_VIDEO_ACCELERATE_HW "video.accelerate.hw"
#define VIDEO_ACCELERATE_HW_OVERLAY "overlay"
#define VIDEO_ACCELERATE_HW_GC      "old_gc"
#define VIDEO_ACCELERATE_HW_DIRECT_GC      "gc"
#define VIDEO_ACCELERATE_HW_3D      "3d"

#ifdef _MARVELL_DRM_PLAYER
extern "C" int DrmPlaySetVideoRenderer(void *pHandle);
#endif


android::VideoRenderer *createRendererWithRotation(
        const android::sp<android::ISurface> &surface,
        const char *componentName,
        OMX_COLOR_FORMATTYPE colorFormat,
        size_t displayWidth, size_t displayHeight,
        size_t decodedWidth, size_t decodedHeight,
        int32_t rotationDegrees)
{
    android::VideoRenderer * ret = NULL;
    char value[PROPERTY_VALUE_MAX];
    property_get( PROP_VIDEO_ACCELERATE_HW, value, "unknown" );
    if( strcasecmp(value, VIDEO_ACCELERATE_HW_OVERLAY) == 0 ) {
        LOGD("****  Create Vendor Renderer, Ov  ***");

        /* drm.play input an invalid color format */        
        if (strstr(componentName, "drm.play")) {
#ifdef _MARVELL_DRM_PLAYER
            colorFormat = OMX_COLOR_FormatCbYCrY;
            ret = android::DrmHardwareRendererOverlay::CreateInstance( componentName,
                            colorFormat, surface, displayWidth, displayHeight,
                            decodedWidth, decodedHeight );
            LOGD("XXXXXXXXXXXXXXXxx create DRM render XXXXXXXXXXXXXXXXXXXXXXXXXXx");
#endif
        }
        else {
            ret = android::HardwareRendererOverlay::CreateInstance( componentName,
                            colorFormat, surface, displayWidth, displayHeight,
                            decodedWidth, decodedHeight, rotationDegrees );

        }
    } else if( strcasecmp(value, VIDEO_ACCELERATE_HW_GC) == 0 ) {
	 ret = android::HardwareRendererGC::CreateInstance( 
                            colorFormat, surface, displayWidth, displayHeight,
                            decodedWidth, decodedHeight, rotationDegrees );
    } else if( strcasecmp(value, VIDEO_ACCELERATE_HW_DIRECT_GC) == 0 ) {
        if ((displayWidth != decodedWidth) || (displayHeight != decodedHeight) ||
            (decodedWidth & 15) || (decodedHeight & 3)){
            LOGD("****  Create Vendor Renderer, GC, for unaligned stream ***");
            ret = android::HardwareRendererGC::CreateInstance( 
                            colorFormat, surface, displayWidth, displayHeight,
                            decodedWidth, decodedHeight,rotationDegrees);
        }
        else {
            LOGD("****  Create Vendor Renderer, GC Direct  ***");
            ret = android::HardwareRendererDirectGC::CreateInstance(
                            colorFormat, surface, displayWidth, displayHeight,
                            decodedWidth, decodedHeight,rotationDegrees);
        }
    } else if( strcasecmp(value, VIDEO_ACCELERATE_HW_3D) == 0 ) {
        //ret = new AndroidSurfaceOutput3D();
    }
#ifdef _MARVELL_DRM_PLAYER
    if (strstr(componentName,"drm.play")){        
        LOGD("set video render for drm.play");        
        DrmPlaySetVideoRenderer(ret);    
    }
#endif
    return ret;

}

android::VideoRenderer *createRenderer(
        const android::sp<android::ISurface> &surface,
        const char *componentName,
        OMX_COLOR_FORMATTYPE colorFormat,
        size_t displayWidth, size_t displayHeight,
        size_t decodedWidth, size_t decodedHeight) {
    return createRendererWithRotation( surface, componentName,
                                       colorFormat,
                                       displayWidth, displayHeight,
                                       decodedWidth, decodedHeight, 0 );
}
