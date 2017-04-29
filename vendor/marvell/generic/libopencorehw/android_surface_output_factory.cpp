#include "android_surface_output_ov.h"
#include "android_surface_output_gcx00.h"
#include "android_surface_output_3d.h"
#include <cutils/properties.h>

// valid value of PROP_VIDEO_ACCELERATE_HW
// overlay : video out put use overlay
// gc : video out put with gc
#define PROP_VIDEO_ACCELERATE_HW "video.accelerate.hw"
#define VIDEO_ACCELERATE_HW_OVERLAY "overlay"
#define VIDEO_ACCELERATE_HW_GC      "gc"
#define VIDEO_ACCELERATE_HW_3D      "3d"
// factory function for playerdriver linkage
extern "C" __attribute__ ((visibility("default")))
AndroidSurfaceOutput* createVideoMio()
{
    AndroidSurfaceOutput *ret = NULL;
    char value[PROPERTY_VALUE_MAX];
    property_get( PROP_VIDEO_ACCELERATE_HW, value, "unknown" );
    if( strcasecmp(value, VIDEO_ACCELERATE_HW_OVERLAY) == 0 ) {
        ret = new AndroidSurfaceOutputOverlay();
    } else if( strcasecmp(value, VIDEO_ACCELERATE_HW_GC) == 0 ) {
        ret = new AndroidSurfaceOutputGCx00();
    } else if( strcasecmp(value, VIDEO_ACCELERATE_HW_3D) == 0 ) {
	ret = new AndroidSurfaceOutput3D();
    }
    return ret;
}

