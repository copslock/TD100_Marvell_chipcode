/*
* (C) Copyright 2010 Marvell International Ltd.
* All Rights Reserved
*
* MARVELL CONFIDENTIAL
* Copyright 2008 ~ 2010 Marvell International Ltd All Rights Reserved.
* The source code contained or described herein and all documents related to
* the source code ("Material") are owned by Marvell International Ltd or its
* suppliers or licensors. Title to the Material remains with Marvell International Ltd
* or its suppliers and licensors. The Material contains trade secrets and
* proprietary and confidential information of Marvell or its suppliers and
* licensors. The Material is protected by worldwide copyright and trade secret
* laws and treaty provisions. No part of the Material may be used, copied,
* reproduced, modified, published, uploaded, posted, transmitted, distributed,
* or disclosed in any way without Marvell's prior express written permission.
*
* No license under any patent, copyright, trade secret or other intellectual
* property right is granted to or conferred upon you by disclosure or delivery
* of the Materials, either expressly, by implication, inducement, estoppel or
* otherwise. Any license under such intellectual property rights must be
* express and approved by Marvell in writing.
*
*/

#ifndef __SENSORS_HAL_H__
#define __SENSORS_HAL_H__

#include <stdio.h>
#include <hardware/sensors.h>
#include <cutils/log.h>
#include <dlfcn.h> 

#define	LIB_SENSOR	"libsensors.so"		
#define INPUT_DIR               "/dev/input"
#define DEVICE_CONTROL_PATH		"/sys/class/input"
#define GET_SENSOR_INFO_FUNC "get_sensor_info_list"
#define FREE_SENSOR_INFO_FUNC "free_sensor_info_list"
#define	VALUE_MAX	128
#define ACTIVE_NAME "active"
#define INTERVAL_NAME	"interval"
#define	STATUS_NAME	"status"
#define	MAX_EVENT_NUM	128
#define ABS_STATUS	(ABS_BRAKE)
#define ABS_WAKE	(ABS_MISC)
#define MAX_SENSOR	8

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

enum {
    ID_ACCELERATION = 1,  // = SENSORS_HANDLE_BASE
    ID_MAGNETIC,
    ID_ORIENTATION,
    ID_GYROSCOPE,
    ID_LIGHT,
    ID_PRESSURE,
    ID_TEMPERATURE,
    ID_PROXIMITY,
    ID_GRAVITY,
    ID_LINEAR_ACCELERATION,
    ID_ROTATION_VECTOR,
    ID_MAX
};

typedef struct sensor_info_t{
    const char* name;	/* sensor name */
    const char* vendor;	/* vendor name */
    int version;		/* version number */
    int type;		/* sensor type */
    float max_range;	/* max range */
    float resolution;	/* resolution */
    float power;		/* power */
    float (*convert) (float, int *); /*convert function */
    int minDelay;        /* minimum delay allowed between events in microseconds. */
}sensor_info_t;

typedef struct sensor_fd{
    const char *name; 
    int type; 
    int handle;
    int fd;
    char classpath[PATH_MAX];
    char devpath[PATH_MAX];
    sensors_event_t data;
    int enable;
    float resolution;
    int minDelay;
    float (*convert) (float, int *);	
    int isChanged;  /* indicate whether sensor value changed */
}sensor_fd;

struct sensor_hal_fd{
    sensor_fd sensors[MAX_SENSOR];
    int	pending_flag;
    int sensor_num;
};

int
read_pending_values(sensors_event_t *value, struct sensor_hal_fd *sensor_hal, int maxCount);

int
fill_sensor_list(const sensor_info_t *sensor_info_list, int sensor_info_size,
                struct sensor_t *sensor, int *sensor_size, 
                struct sensor_hal_fd *sensor_hal);

int
free_sensor_list(struct sensor_t **sensor, int size,
                    struct sensor_hal_fd* sensor_hal);

int
get_sensor_list_info(struct sensor_t *sensor_list,
                        struct sensor_hal_fd *sensor_hal);

int
open_input_device(const char *name, int flag, char *dev_path, int len);

sensor_fd*
find_sensor_on_type(int handle, struct sensor_hal_fd *sensor_hal);

int
activate_sensor(sensor_fd *sensor, int enable);

int
set_interval_sensor(sensor_fd *sensor, int delay);

int
read_events(int, sensors_event_t *, float (*) (float, int *), float, int *);

/* 
** The returned pointer sensor_info_list points to a static array from libsensor, 
** no need to free it.
*/
extern int	
get_sensor_info_list(sensor_info_t **sensor_info_list);

#endif /* __SENSORS_HAL_H__ */
