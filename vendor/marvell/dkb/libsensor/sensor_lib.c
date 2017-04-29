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

#include <linux/input.h>
#include <hardware/sensors.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <cutils/log.h>

typedef struct sensor_info_t{
    const char* name;		/* sensor name */
    const char* vendor;		/* vendor name */	
    int version;		/* version number */
    int type;			/* sensor type */	 
    float max_range;		/* max range */	
    float resolution;		/* resolution */
    float power;		/* power */	
    float (*convert) (float orig, int *evt_type); /*convert function */
    int minDelay;               /* minimum delay allowed between events in microseconds. */
}sensor_info_t;

typedef struct proximity_map{
	float code;
	float distance;
}proximity_map;

static proximity_map proximity_convert_map[] = {
	{.code = 1.0f, .distance = 10.0f},
	{.code = 2.0f, .distance = 8.0f},
	{.code = 3.0f, .distance = 6.0f},
	{.code = 4.0f, .distance = 5.0f},
	{.code = 6.0f, .distance = 4.0f},
	{.code = 10.0f, .distance = 3.0f},
	{.code = 16.0f, .distance = 2.4f},
	{.code = 32.0f, .distance = 1.7f},
	{.code = 96.0f,	.distance = 1.0f},
	{.code = 256.0f, .distance = 0.6f}	
};

typedef float (*convert) (float orig, int *evt_type); /*convert function */

float 
convert_accelerometor(float orig, int *evt_type)
{
	if ((*evt_type == ABS_X) || (*evt_type == ABS_Y) || (*evt_type == ABS_Z)){
		/* return orig * GRAVITY_EARTH / (1632.0f)*32*64/9806550; */
		return orig / 100.0;
		
	}
	return orig;
}

float 
convert_proximity(float orig, int *evt_type)
{
	unsigned int i;

	for(i = 0; i < sizeof(proximity_convert_map)/sizeof(proximity_convert_map[0])-1; i++){
		if ((orig < proximity_convert_map[i+1].code) &&
				(orig >= proximity_convert_map[i].code)){
				break;
		}
	}

	return proximity_convert_map[i].distance;
}

sensor_info_t sensors[] = {
	{
		.name = "accelerometer",			/* sensor name */
		.vendor = "ST",				/* vendor name */
		.version = 1,					/* version */
		.type = SENSOR_TYPE_ACCELEROMETER,	/* sensor type */
		.max_range = 2.0f * GRAVITY_EARTH,			/* max range */
		.resolution = GRAVITY_EARTH/2048,		/* resolution */
		.power = 0,						/* TODO: power */
		.minDelay = 20,
		.convert = &(convert_accelerometor),/* convert function */
	},
    {
        .name = "sfh7773 light sensor",
        .vendor = "freecall",
        .version = 1,
        .type = SENSOR_TYPE_LIGHT,
        .max_range = 65535,
        .resolution = 1,
        .power = 0,
    },
    {
        .name = "sfh7773 proximity sensor",
        .vendor = "freecall",
        .version = 1,
        .type = SENSOR_TYPE_PROXIMITY,
        .max_range = 255,
        .power = 0,
/*        .convert = &(convert_proximity),*/
    },
	{
		.name = "l3g4200d_gyr",
		.vendor = "ST",
		.version = 1,
		.type = SENSOR_TYPE_GYROSCOPE,
		.max_range = 65535,
		.power = 0,

	},
	{
		.name = "mag3110",
		.vendor = "freescale",
		.version = 1,
		.type = SENSOR_TYPE_MAGNETIC_FIELD,
		.max_range = 65535,
		.power = 0,
	},
};


int	
get_sensor_info_list(sensor_info_t **sensor_info_list)
{
	int num_sensor = 0;

	assert(sensor_info_list);

	num_sensor = sizeof(sensors)/sizeof(sensor_info_t);
	*sensor_info_list = sensors;

	return num_sensor;
}


