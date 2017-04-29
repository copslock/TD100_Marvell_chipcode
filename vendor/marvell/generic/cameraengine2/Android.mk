# camera-engine lib
include vendor/marvell/generic/cameraengine2/src/cameraengine2/ansi_c/Mdroid_cameraengine.mk
include vendor/marvell/generic/cameraengine2/src/cameraengine2/ansi_c/Mdroid_sensorhal.mk

# camera sample application in /system/bin/MrvlCameraDemo
include vendor/marvell/generic/cameraengine2/example/cameraengine2/Mdroid.mk

# sensor vendor deliverables
include vendor/marvell/generic/cameraengine2/src/cameraengine2/ansi_c/extisp_sensorhal/ov5642/Mdroid.mk

# tool library of camera usage model, including: 
# 1. libippexif to generate exif image from JPEG image
include vendor/marvell/generic/cameraengine2/tool/lib/Mdroid.mk

# tool library sample application, including:
# 1. MrvlExifGenerator in /system/bin
#include vendor/marvell/generic/cameraengine2/tool/example/Mdroid.mk
