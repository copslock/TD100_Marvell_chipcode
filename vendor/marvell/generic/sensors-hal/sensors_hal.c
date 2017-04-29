/*
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
#define LOG_TAG "sensorHAL"

#include <sensors_hal.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <unistd.h>
#include <cutils/properties.h>

static struct sensor_t g_sensor_list[MAX_SENSOR];	/* sensor list */
static int g_sensor_list_size = -1; 				/* sizeof g_sensor_list */		
static struct sensor_hal_fd g_sensor_hal;

#define ACTIVATE_SENSOR(sensor, enable) \
do{\
    retval = activate_sensor(sensor, enable);\
    if (retval < 0){\
        LOGE("Failed to activate sensor %s.", sensor->name);\
        return retval;\
    }\
    LOGE("Successfully to %s sensor %s.", sensor->name, enable ? "activate" : "deactivate");\
}while(0) 

/*
** Return sensor_info list.
*/
static int
sensors_get_list(struct sensors_module_t *module,
		struct sensor_t const** sensor_list)
{

    LOGD("Enter sensors_get_list.\n");

    if (g_sensor_list_size < 0){
        g_sensor_list_size = get_sensor_list_info(g_sensor_list, &g_sensor_hal);
        LOGI("g_sensor_list_size is %d.\n", g_sensor_list_size);
    }

    *sensor_list = g_sensor_list;

    {
        int i = 0;
        LOGD("Sensor num %d", g_sensor_list_size);
        for (i = 0; i < g_sensor_list_size; i++){		
            LOGD("sensor name from libsensor: %s.\n", (*sensor_list)[i].name);
        }
    }

    return g_sensor_list_size;	
}

/*
** Activate the device. 
*/
static int
set_activate(struct sensors_poll_device_t *dev, int handle, int enable)
{
    int i;
    struct sensor_fd *sensor = NULL;
    struct sensor_fd *sensor_acc = NULL;
    struct sensor_fd *sensor_mag = NULL;
    struct sensor_fd *sensor_ori = NULL;
    struct sensor_fd *sensor_gyro = NULL;
    int retval = 0;
    int cywee_enable = 0;
    char propBuf[PROPERTY_VALUE_MAX];

    property_get("persist.cywee_sensor.enable", propBuf, "");
    if (propBuf[0] == '1') {
        cywee_enable = 1;
    }

    assert(dev);

    LOGD("Enter control_activate. active = %d, handle is %d, pid = %d\n", enable, handle, getpid());
    if (!(sensor = find_sensor_on_type(handle, &g_sensor_hal))){
        LOGD("Failed to find the sensor to activate, handle is %d.\n", handle);	
        return -1;
    }

    switch (handle){
        case ID_ORIENTATION:
        case ID_MAGNETIC:

            /* Should activate magnetic and acceleration sensor */		
            sensor_acc = find_sensor_on_type(SENSOR_TYPE_ACCELEROMETER, &g_sensor_hal);
            sensor_mag = find_sensor_on_type(SENSOR_TYPE_MAGNETIC_FIELD, &g_sensor_hal);

            if (cywee_enable == 1){
                sensor_gyro = find_sensor_on_type(SENSOR_TYPE_GYROSCOPE, &g_sensor_hal);
                if (!sensor_acc || !sensor_mag || !sensor_gyro){
                   return -1;
                }
            } else {
                if (!sensor_acc || !sensor_mag){
                   return -1;
                }
            }

            if (!sensor_acc->enable){
                ACTIVATE_SENSOR(sensor_acc, enable);
            }
            if (!sensor_mag->enable){
                ACTIVATE_SENSOR(sensor_mag, enable);                
            }

            if (cywee_enable == 1){
                if (!sensor_gyro->enable){
                    ACTIVATE_SENSOR(sensor_gyro, enable);
                }
            }
            sensor->enable = enable;
            ACTIVATE_SENSOR(sensor, enable);            
            break;

        case ID_ACCELERATION:
            /* if Orientation or Magnetic sensor is activated, nothing need to do */
            sensor_ori = find_sensor_on_type(SENSOR_TYPE_ORIENTATION, &g_sensor_hal);
            sensor_mag = find_sensor_on_type(SENSOR_TYPE_MAGNETIC_FIELD, &g_sensor_hal);

            sensor->enable = enable;
            if (((!sensor_ori) || (sensor_ori && !sensor_ori->enable)) ||
                ((!sensor_mag) || (sensor_mag && !sensor_mag->enable)) ){
                ACTIVATE_SENSOR(sensor, enable);
            }
            break;

        default:
            ACTIVATE_SENSOR(sensor, enable);                       
            break;
    }

    return retval;
}

/*
** Set delay to sensor
*/
static int
set_delay(struct sensors_poll_device_t *dev, int handle, int64_t ns)
{
    int i;
    int ret = 0;
    sensor_fd *sensor = NULL;

    assert(dev);
	
    /* Delay the sensor */
    sensor = find_sensor_on_type(handle, &g_sensor_hal);

    if (!sensor){
        LOGD("Failed to set interval for lacking activate any sensor.");
        return -1;
    }

    if (set_interval_sensor(sensor, ns/1000000) < 0){
        LOGD("Failed to set interval to %s.\n", sensor->name);          
        return -1;
    }		
    LOGD("Successfully set interval to %s.\n", sensor->name);

    return ret;
}

/*
** Poll all sensors, if data's pending, read the pending values.
** otherwize, just poll to wait for new data.
** @return the number of events read on success, or -errno in case of an error.
** This function should never return 0 (no event).
*/
static int
data_poll(struct sensors_poll_device_t *dev, sensors_event_t *value, int maxCount)
{
    fd_set fds;
    int i;
    int ret;
    int valid_fd_num;
    int fd_poll;
    int select_dim;

    assert(dev);
    assert(value);
    
    /* read pending values */
    if (g_sensor_hal.pending_flag){
        return read_pending_values(value, &g_sensor_hal, maxCount);
    }
    
    /* poll */
    while(1){
        FD_ZERO(&fds);
        select_dim = -1;
        valid_fd_num = 0;
        /* get all valid file describers of sensors */
        for (i = 0; i < g_sensor_hal.sensor_num; i++){
            if (g_sensor_hal.sensors[i].fd < 0){
                continue;
            }
            fd_poll = g_sensor_hal.sensors[i].fd;
            valid_fd_num += 1;
            FD_SET(fd_poll, &fds);
            if (select_dim < fd_poll + 1) {
                select_dim = fd_poll + 1;
            }
        }

        if (valid_fd_num <= 0){
            LOGD("Exit data_poll for valid_fd_num <= 0.\n");	
            return -1;
        }        

        /* call the select */
        if ((ret = select(select_dim, &fds, NULL, NULL, 0)) < 0){
            LOGE("Exit data_poll select fail.\n");
            return -1;
        }

        /* get all values from sensor describers who has data */
        for (i = 0; i < g_sensor_hal.sensor_num; i++){
            fd_poll = g_sensor_hal.sensors[i].fd;
            if (!FD_ISSET(fd_poll, &fds)){
                continue;        		
            }

            ret = read_events(fd_poll, &(g_sensor_hal.sensors[i].data),
                                        g_sensor_hal.sensors[i].convert,
                                        g_sensor_hal.sensors[i].resolution,
                                        &(g_sensor_hal.sensors[i].isChanged));

            if (ret > 0){
                /* having event, set the bit */
                g_sensor_hal.pending_flag |= 
                        (1 << g_sensor_hal.sensors[i].handle);
            }
            else if (ret == -EWOULDBLOCK) {
                LOGD("Exit data_poll, read_events fail.\n");        	
                return ret;
            }
        }

        /* return values */
        if (g_sensor_hal.pending_flag){
            return read_pending_values(value, &g_sensor_hal, maxCount);
        }
    }
}

/*
** Close the sensor control device
*/
static int
device_close(struct hw_device_t *device)
{
    struct sensors_poll_device_t *dev_poll =
        (struct sensors_poll_device_t *)device;
    int i = 0;
    sensor_fd *sensor;

    LOGD("Enter device_close.\n");

    /* release control device structure */
    if (dev_poll){
        free(dev_poll);
        dev_poll = NULL;
    }

    /* close all sensor fds. */
	for (i = 0; i < g_sensor_hal.sensor_num; i++){
		sensor = &(g_sensor_hal.sensors[i]);
		if ((!sensor) || (sensor->fd < 0)){
			continue;
		}
		close(sensor->fd);
		sensor->fd = -1;
	}

    return 0;
}

static int
device_open(const struct hw_module_t* module, const char* name,
                    struct hw_device_t** device)
{
    int i;
    int status = 0;

    LOGD("Enter device_open. name is %s\n", name);

    if (!strcmp(name, SENSORS_HARDWARE_POLL)) {
        struct sensors_poll_device_t *dev_poll;
        if ((dev_poll = malloc(sizeof(struct sensors_poll_device_t))) == 0) {
            return -ENOMEM;
        }
        memset(dev_poll, 0, sizeof(*dev_poll));

        dev_poll->common.tag     = HARDWARE_DEVICE_TAG;
        dev_poll->common.version = 0;
        dev_poll->common.close   = device_close;
        dev_poll->common.module  = (struct hw_module_t*)module;
        dev_poll->activate       = set_activate;
        dev_poll->poll           = data_poll;
        dev_poll->setDelay      = set_delay;
        *device             = &dev_poll->common;

        g_sensor_list_size = get_sensor_list_info(g_sensor_list, &g_sensor_hal);
        LOGI("valid sensor number from libsensor is %d.\n", g_sensor_list_size);

        if (g_sensor_list_size <= 0){
            status = -EINVAL;
        }

        /* open input devices, will be closed by caller. */
        for (i = 0; i < g_sensor_list_size; i++){
            g_sensor_hal.sensors[i].fd = open_input_device(
                g_sensor_hal.sensors[i].name, 
                O_RDONLY,
                g_sensor_hal.sensors[i].devpath, 
                sizeof(g_sensor_hal.sensors[i].devpath));
            LOGI("Open sensor %s, devpath %s, fd = %d",
            g_sensor_hal.sensors[i].name, g_sensor_hal.sensors[i].devpath,
            g_sensor_hal.sensors[i].fd);
        }
    }

    return status; 
}

static struct hw_module_methods_t sensors_module_methods = {
    .open =  device_open,
}; 

/*
** The Sensors Hardware Module
*/
const struct sensors_module_t HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .version_major = 1,
        .version_minor = 0,
        .id = SENSORS_HARDWARE_MODULE_ID,
        .name = "Sensors Module",
        .author = "Marvell APSE",
        .methods = &sensors_module_methods,
    },
    .get_sensors_list = sensors_get_list,
};

