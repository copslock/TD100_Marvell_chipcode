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
#define LOG_TAG "sensorHAL"
#include <sensors_hal.h>
#include <errno.h>
#include <linux/input.h>
#include <dirent.h>
#include <fcntl.h>
#include <assert.h>
#include <stdlib.h>

#define ABSOLUTE_SUB_GT(a,b,c) (((a)-(b)) > c || ((b) - (a)) > c)

static int
chomp(char *buf, int len)
{
    if (buf == NULL || len < 0) {
        return -1;
    }
    while (len > 0 && (buf[len - 1] == '\n' || buf[len -1] == '\r')) {
        buf[len - 1] = '\0';
        len--;
    }
    return 0;
}

/**
* Fetch class path according to the device name.
* @return 0 if found, negative value for fail.
*/
static int
get_class_path(char *classpath, const char *name, int len)
{
    DIR *dir;
    struct dirent *de;
    char name_path[PATH_MAX];
    char dev_name[NAME_MAX];
    int fd;
    int nRet;
    int flag_found = 0;

    assert(name);

    if (!(dir = opendir(DEVICE_CONTROL_PATH))){
        LOGE("Directory %s open failed.", DEVICE_CONTROL_PATH);
        return -1;
    }

    while((de = readdir(dir))){
        if (strncmp(de->d_name, "input", strlen("input")) != 0){
            continue;
        }

        snprintf(name_path, sizeof(name_path), "%s/%s/name", 
        DEVICE_CONTROL_PATH, de->d_name);

        if ((fd = open(name_path, O_RDONLY)) < 0){
            LOGE("File %s open failed.", name_path);
            continue;
        }

        if ((nRet = read(fd, dev_name, sizeof(dev_name))) <= 0){
            close(fd);
            continue;
        }
        dev_name[nRet - 1] = '\0';
        chomp(dev_name, strlen(dev_name));

        if (strcmp(dev_name, name) == 0){
            /* 
                    ** found the device name under 
                    ** "DEVICE_CONTROL_PATH/inputxx/name" 
                    */
            snprintf(classpath, len, "%s/%s/", DEVICE_CONTROL_PATH, de->d_name);	
            LOGE("Class path %s is for device %s.\n", classpath, name);			
            flag_found = 1;
            close(fd);
            break;

        }

        close(fd);
    }

    closedir(dir);

    if (flag_found){
        return 0;
    }
    else{
        classpath[0] = '\0';
        return -1;
    }	
}

static int 
write_to_file(char *filename, char *value, int size)
{
    int fd = -1;

    assert(filename);
    assert(value);

    if ((fd = open(filename, O_WRONLY)) < 0){
        return -1;
    }

    if (write(fd, value, size) < 0){
        close(fd);
        return -1;
    }

    close(fd);

    return 0;	
}

static int64_t
timeval_to_nanosec(struct timeval *tv)
{
    return (int64_t)((int64_t)tv->tv_sec*1000000000
            + (int64_t)tv->tv_usec*1000);
}

/*
** Based on sensor information list from sensorlib,  
** we can fill g_sensor_list and g_sensor_hal.
** @return 0 for success, other values for fail. 
*/ 
int
fill_sensor_list(const sensor_info_t *sensor_info_list, int sensor_info_size,
                struct sensor_t *sensor, int *sensor_size, 
                struct sensor_hal_fd *sensor_hal)
{
    int i,j;

    /* Count valid number */
    for (i = 0, j = 0; i < sensor_info_size; i++){
        if (sensor_info_list[i].type > 0 &&
        sensor_info_list[i].type < ID_MAX){
        j++;	
        }
    }        
        
    *sensor_size = j;

    /* Fill each element */
    for (i = 0, j = 0; i < sensor_info_size; i++, j++) {
        switch(sensor_info_list[i].type){
            case SENSOR_TYPE_ACCELEROMETER:
                sensor[j].handle = ID_ACCELERATION;
                break;
            case SENSOR_TYPE_MAGNETIC_FIELD:
                sensor[j].handle = ID_MAGNETIC;
                break;
            case SENSOR_TYPE_ORIENTATION:   
                sensor[j].handle = ID_ORIENTATION;
                break;
            case SENSOR_TYPE_GYROSCOPE:     
                sensor[j].handle = ID_GYROSCOPE;
                break;
            case SENSOR_TYPE_LIGHT:         
                sensor[j].handle = ID_LIGHT;
                break;
            case SENSOR_TYPE_PRESSURE:      
                sensor[j].handle = ID_PRESSURE;
                break;
            case SENSOR_TYPE_TEMPERATURE:   
                sensor[j].handle = ID_TEMPERATURE;
                break;
            case SENSOR_TYPE_PROXIMITY:
                sensor[j].handle = ID_PROXIMITY;
                break;
            default:
                sensor[j].handle = ID_MAX;
                break;	
        }

        /* Not supported type, just ignore. */
        if (sensor[j].handle == ID_MAX){
            j--;
            continue;
        }	

        /* Struct sensor_t */
        sensor[j].name = sensor_info_list[i].name;
        sensor[j].vendor = sensor_info_list[i].vendor;
        sensor[j].type = sensor_info_list[i].type;    
        sensor[j].maxRange = sensor_info_list[i].max_range;
        sensor[j].resolution = sensor_info_list[i].resolution;
        sensor[j].power = sensor_info_list[i].power;
        sensor[j].version = sensor_info_list[i].version;

        /* Struct sensor_hal_fd */
        sensor_hal->sensors[j].name = sensor_info_list[i].name;
        sensor_hal->sensors[j].enable = 0;	
        sensor_hal->sensors[j].fd = -1;
        sensor_hal->sensors[j].handle = sensor[j].handle;
        sensor_hal->sensors[j].type = sensor[j].type;
        sensor_hal->sensors[j].convert = sensor_info_list[i].convert;	
        sensor_hal->sensors[j].resolution = sensor_info_list[i].resolution;
        sensor_hal->sensors[j].minDelay = sensor_info_list[i].minDelay;
        sensor_hal->sensors[j].isChanged = FALSE;

        get_class_path(sensor_hal->sensors[j].classpath, 
                        sensor_hal->sensors[j].name, 
                        sizeof(sensor_hal->sensors[j].classpath));

        /* Struct sensors_event_t */
        sensor_hal->sensors[j].data.version = sizeof(struct sensors_event_t);
        sensor_hal->sensors[j].data.type = sensor[j].type;
        sensor_hal->sensors[j].data.sensor = sensor[j].handle;
    }

    sensor_hal->pending_flag = 0;	
    sensor_hal->sensor_num = *sensor_size;

    return 0;
}

/**
* Enumerate all available sensors from sensor_lib. 
* The list is returned in "sensor_list".
* @return number of sensors in the list
*/
int
get_sensor_list_info(struct sensor_t *sensor_list,
								struct sensor_hal_fd *sensor_hal)
{
    sensor_info_t *sensor_info_list = NULL;
    int sensor_info_list_size = 0;
    int sensor_list_size = 0;
    sensor_info_list_size = get_sensor_info_list(&sensor_info_list);

    /* fill sensor_list with elements from sensor_info_list*/
    if (fill_sensor_list(sensor_info_list, sensor_info_list_size,
                            sensor_list, &sensor_list_size, sensor_hal) != 0)
    {
        sensor_list_size = 0;
    }

    sensor_info_list = NULL;

    return sensor_list_size;
}

/* 
** Open the device according to its name, register its path by the way.
** @return the fd number. -1 for fail
*/
int
open_input_device(const char *name, int flag, char *dev_path, int len)
{
    DIR *dir;
    struct dirent *de;
    char dev_name[NAME_MAX];
    int fd = -1;

    assert(name);

    if (!(dir = opendir(INPUT_DIR))){
        LOGE("Directory %s open failed.", INPUT_DIR);
        return -1;
    }

    while((de = readdir(dir))){
        if(de->d_name[0] == '.' &&
        (de->d_name[1] == '\0' ||
        (de->d_name[1] == '.' && de->d_name[2] == '\0')))
            continue;

        snprintf(dev_path, len, "%s/%s", INPUT_DIR, de->d_name);

        if ((fd = open(dev_path, flag)) < 0){
            continue;
        }

        if (ioctl(fd, EVIOCGNAME(sizeof(dev_name) - 1), 
                                        &dev_name) < 1) {
            dev_name[0] = '\0';
        }

        if (strcmp(name, dev_name) == 0) {
            break;
        }

        close(fd);
        fd = -1;
    }

    closedir(dir);
    return fd;	
}

sensor_fd*
find_sensor_on_type(int handle, struct sensor_hal_fd *sensor_hal)
{
    int i;
    sensor_fd *sensor = NULL;

    for (i = 0; i < sensor_hal->sensor_num; i++){
        if (sensor_hal->sensors[i].handle == handle){
            sensor = &(sensor_hal->sensors[i]);
            break;
        }
    }

    return sensor;
}

int
activate_sensor(sensor_fd *sensor, int enable)
{
    char  filename[PATH_MAX];
    char  value[VALUE_MAX];

    snprintf(value, sizeof(value), "%d\n", enable);
	
    snprintf(filename, sizeof(filename), 
                    "%s/%s", sensor->classpath, ACTIVE_NAME);	
    
    LOGD("Try to activate %s, file is :%s, active is %d", 
                                sensor->name, filename, enable);    
    
    return write_to_file(filename, value, VALUE_MAX);	
}

int
set_interval_sensor(sensor_fd *sensor, int delay)
{
    char  filename[PATH_MAX];
    char  value[VALUE_MAX];

    snprintf(value, sizeof(value), "%d", delay);

    snprintf(filename, sizeof(filename), "%s/%s", 
                            sensor->classpath, INTERVAL_NAME);
    
    LOGD("Try to set interval to %s, file is :%s, delay is %d", 
                                sensor->name, filename, delay);  
    
    return write_to_file(filename, value, strlen(value));	
}

int
read_pending_values(sensors_event_t *value, struct sensor_hal_fd *sensor_hal, int maxCount)
{
    int i, count = 0;
    sensor_fd *sensor = NULL;	

    for (i = 0; (i < sensor_hal->sensor_num) && (count < maxCount); i++){
        sensor = &(sensor_hal->sensors[i]);
        if (sensor_hal->pending_flag & (1 << sensor->handle)){
            sensor_hal->pending_flag &= ~(1 << sensor->handle);
            value[count] = sensor_hal->sensors[i].data;
            count++;
        }
    }

    return count;
}

/* 
** read events from sensor device and fill to sensor_data_t 
** @ret 1 on success, -1 on read nothing or read incomplete event, -EWOULDBLOCK on user request to break poll.
*/
int
read_events(int fd, sensors_event_t *value, float (*convert) (float, int *), float resolution, int *changed)
{
    struct input_event event_buf[MAX_EVENT_NUM];
    int num_event, i, nread, wake_flag = 0, syned = -1;

    assert(fd);
    assert(value);

    if ((nread = read(fd, event_buf, sizeof(event_buf))) < 0) {
        return -1;
    }
    num_event = nread / sizeof(struct input_event);

    for (i = 0; i < num_event; i++) {
        switch (event_buf[i].type) {
            case EV_ABS:
                {
                    float evt_value = 0.0f;
                    int type = 0;

                    type = event_buf[i].code;
                    evt_value = event_buf[i].value;
                    if (convert){
                        evt_value = (*convert)(evt_value, &type);
                    }
                    switch (type) {
                        case ABS_X:
                        case ABS_RX:
                            if (ABSOLUTE_SUB_GT(value->acceleration.v[0],
                                                  evt_value, resolution)) {
                                    value->acceleration.v[0] = evt_value;
                                    *changed = TRUE;
                                }
                            break;
                        case ABS_Y:
                        case ABS_RY:  
                            if (ABSOLUTE_SUB_GT(value->acceleration.v[1],
                                                  evt_value, resolution)) {
                                    value->acceleration.v[1] = evt_value;
                                    *changed = TRUE;
                            }
                            break;
                        case ABS_Z:
                        case ABS_RZ:       
                            if (ABSOLUTE_SUB_GT(value->acceleration.v[2],
                                                  evt_value, resolution)) {
                                    value->acceleration.v[2] = evt_value;
                                    *changed = TRUE;
                            }
                            break;
                        case ABS_STATUS:
                            value->acceleration.status = evt_value;
                            break;
                        case ABS_DISTANCE:
                             if (ABSOLUTE_SUB_GT(value->distance, 
                                                   evt_value, resolution)) {
                                     value->distance = evt_value;
                                     *changed = TRUE;
                            }
                            break;
                        case ABS_PRESSURE:
                             if (ABSOLUTE_SUB_GT(value->distance, 
                                                   evt_value, resolution)) {
                                     value->pressure = evt_value;
                                     *changed = TRUE;
                             }
                            break;
                        case ABS_WAKE:
                            wake_flag = 1;
                            break;
                        default:
                            break;
                    }
                    break;
                }
            case EV_SYN:
                value->timestamp = timeval_to_nanosec(&event_buf[i].time);
                syned = 1;
                break;
                
            default:
                break;
        }
    }

    if (wake_flag) {
        return -EWOULDBLOCK;
    }

    /*
     * Return true only after read a timestamp
     */
    if ((syned == 1) && (*changed == TRUE)){
        *changed = FALSE;
        return 1;
    }

    return -1;
}

