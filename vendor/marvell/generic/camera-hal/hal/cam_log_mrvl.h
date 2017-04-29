#ifndef ANDROID_HARDWARE_CAMERA_LOG_MRVL_H
#define ANDROID_HARDWARE_CAMERA_LOG_MRVL_H

#include <utils/Log.h>

#define DEBUG_TIME
#ifdef DEBUG_TIME
#include <utils/Timers.h>
static nsecs_t told,delta;
static int lineold;
#define TIMETAG do{\
    TRACE_D("************************"); \
    delta=systemTime()-told; \
    TRACE_D("*line:%d - %d,\t dt=%f ms,  file=%s",lineold, __LINE__, double(delta/1000000), __FILE__); \
    lineold=int(__LINE__); \
    told=systemTime(); \
    TRACE_D("************************");\
    }while(0)
#else
#define TIMETAG do{}while(0)
#endif
/*
 * local debug info
 */
#define FUNC_TAG    do{LOGD("-%s", __FUNCTION__);}while(0)
#define FUNC_TAG_E  do{LOGD("-%s e", __FUNCTION__);}while(0)
#define FUNC_TAG_X  do{LOGD("-%s x", __FUNCTION__);}while(0)

#define TRACE_E(...)\
    do \
{\
    LOGE("ERROR: %s:%s:%d",__FUNCTION__,__FILE__,__LINE__); \
    LOGE(__VA_ARGS__); \
} while(0);

#define TRACE_D(...)\
    do \
{\
    LOGD(__VA_ARGS__); \
} while(0);

#define TRACE_V(...)\
    do \
{\
    LOGV(__VA_ARGS__); \
} while(0);


//debug macro

/*
 * used to check rgb565 baselayer preview, which is supposed to be supported by surface by default.
 * notes: we only support yuv420p->rgb565 conversion, so yuv40p output should be used when enabling this macro,
 * and cameraservice should be changed accordingly.
 */
//#define DEBUG_BASE_RGB565

#endif
