#include <nativehelper/JNIHelp.h>
#include <nativehelper/jni.h>

#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <utils/Log.h>

#include <linux/capability.h>
#include <linux/prctl.h>
#include <private/android_filesystem_config.h>

#include <termios.h>
#include <fcntl.h>

#define LOG_TAG "LibUsbSetting"
#define MAX_CHARS 256

static jint run(JNIEnv * env, jobject jobj, jstring cmdLine)
{
    const char *namestr = env->GetStringUTFChars(cmdLine, NULL);
    LOGD("cmdLine is: %s", namestr);
    char savedCmdLine[MAX_CHARS];
    strncpy(savedCmdLine, namestr, MAX_CHARS - 1);
    system(savedCmdLine);
    return 1;
}

static char atcmd_request[256];

static const char *classPathName = "com/marvell/usbsetting/UsbSetting";

static JNINativeMethod methods[] = {
    {"run", "(Ljava/lang/String;)I", (void *)run},
};

static int
registerNativeMethods(JNIEnv * env, const char *className,
                      JNINativeMethod * gMethods, int numMethods)
{
    jclass clazz;

    clazz = env->FindClass(className);
    if (clazz == NULL) {
        fprintf(stderr,
                "usbsetting registration unable to find class '%s'\n",
                className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        fprintf(stderr, "Register usbsetting failed for '%s'\n", className);
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

static int registerNatives(JNIEnv * env)
{
    LOGD("registerUsbSetting");
    return jniRegisterNativeMethods(env, classPathName,
                                    methods, NELEM(methods));
}

__attribute__ ((visibility("default")))
jint JNI_OnLoad(JavaVM * vm, void *reserved)
{
    JNIEnv *env = NULL;
    jint result = -1;

    if (vm->GetEnv((void **)&env, JNI_VERSION_1_4) != JNI_OK) {
        fprintf(stderr, "ERROR: GetEnv failed\n");
        goto bail;
    }
    assert(env != NULL);

    printf("In mgmain JNI_OnLoad\n");

    if (registerNatives(env) < 0) {
        fprintf(stderr, "ERROR: Exif usbsetting registration failed\n");
        goto bail;
    }

    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

bail:
    return result;
}
