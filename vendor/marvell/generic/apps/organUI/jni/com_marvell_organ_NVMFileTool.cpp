/*
 * * (C) Copyright 2010 Marvell International Ltd.
 * * All Rights Reserved
 * *
 * * MARVELL CONFIDENTIAL
 * * Copyright 2008 ~ 2010 Marvell International Ltd All Rights Reserved.
 * * The source code contained or described herein and all documents related to
 * * the source code ("Material") are owned by Marvell International Ltd or its
 * * suppliers or licensors. Title to the Material remains with Marvell International Ltd
 * * or its suppliers and licensors. The Material contains trade secrets and
 * * proprietary and confidential information of Marvell or its suppliers and
 * * licensors. The Material is protected by worldwide copyright and trade secret
 * * laws and treaty provisions. No part of the Material may be used, copied,
 * * reproduced, modified, published, uploaded, posted, transmitted, distributed,
 * * or disclosed in any way without Marvell's prior express written permission.
 * *
 * * No license under any patent, copyright, trade secret or other intellectual
 * * property right is granted to or conferred upon you by disclosure or delivery
 * * of the Materials, either expressly, by implication, inducement, estoppel or
 * * otherwise. Any license under such intellectual property rights must be
 * * express and approved by Marvell in writing.
 * *
 * */

#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"
#include <utils/misc.h>
#include <utils/Log.h>
#include <utils/threads.h>

#include <hardware_legacy/power.h>
#include <sys/reboot.h>
#include <cutils/properties.h>

//#include <ui/EventHub.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <utils/Log.h>


#include "com_marvell_organ_NVMFileTool.h"

using namespace android;

typedef struct NVM_Header
{
    unsigned long StructSize;       // the size of the user structure below
    unsigned long NumofStructs;     // >1 in case of array of structs (default is 1).
    char StructName[64];            // the name of the user structure below
    char Date[32];                  // date updated by the ICAT when the file is saved. Filled by calibration SW.
    char time[32];                  // time updated by the ICAT when the file is saved. Filled by calibration SW.
    char Version[64];               // user version - this field is updated by the SW eng. Every time they update the UserStruct.
    char HW_ID[32];                 // signifies the board number. Filled by calibration SW.
    char CalibVersion[32];          // signifies the calibration SW version. Filled by calibration SW.
} NVM_Header_ts;

typedef struct HiFi_Organ_Params {
    unsigned short paramType;  //0-playback 1-record 
    unsigned short deviceType; //output or input device 
    unsigned short streamType; //stream type for playback, application style for record 
    unsigned short formatType; //file format for playback, encoding format for record 
    unsigned short volumeIndex;//only valid for playback for record fixed to 0 
    unsigned short dBIndex;    //index to indicate the dB 
} hifiParam;

typedef struct HiFi_Organ_Params_Header {
    unsigned short paramType;  //0-playback 1-record 
    unsigned short deviceType; //output or input device 
    unsigned short streamType; //stream type for playback, application style for record 
    unsigned short formatType; //file format for playback, encoding format for record 
    unsigned short volumeIndex;//only valid for playback for record fixed to 0 
} hifiParamHeader;

static const int MAX_STREAM_VOLUME[] = { 
	-1, // DEFAULT
    5,  // STREAM_VOICE_CALL
    7,  // STREAM_SYSTEM
    7,  // STREAM_RING
    15, // STREAM_MUSIC
    7,  // STREAM_ALARM
    7,  // STREAM_NOTIFICATION
    15, // STREAM_BLUETOOTH_SCO
    7,  // STREAM_ENFORCED_AUDIBLE
    15, // STREAM_DTMF
    15, // STREAM_TTS
    // Organ specific definition
    15, // STREAM_FM_RADIO
};

static const jshort DEFAULT_DB_VALUE_15[] = { 
    0, 27, 33, 40, 47, 53, 60, 67, 73, 80, 87, 93, 100, 107, 113, 120
};

static const jshort DEFAULT_DB_VALUE_7[] = { 
    0, 34, 48, 63, 77, 91, 106, 120
};

static const jshort DEFAULT_DB_VALUE_5[] = { 
    0, 40, 60, 80, 100, 120
};

static const jshort DEFAULT_DB_VALUE_1[] = { 
    120
};

static const char* const organNVMFile = "/data/Linux/Marvell/NVM/audio_hifi_volume.nvm";
static const int maxArraySize = 16;

jshortArray com_marvell_organ_NVMFileTool_read(JNIEnv *env, jobject obj, jshort paramType, jshort deviceType, jshort streamType, jshort formatType)
{
	FILE * fp;
	NVM_Header_ts header;
	hifiParam param;
	const jshort * defaultDBTable;
	jshortArray resultArray = NULL;
	jshort* resultArrayPtr = NULL;
	int arraySize;
	int i;

	if (paramType == 1) { //record
		defaultDBTable = DEFAULT_DB_VALUE_1;
		arraySize = 1;
	} else {
		switch (MAX_STREAM_VOLUME[streamType==15 ? 1 : streamType]) {
		case 15:
			defaultDBTable = DEFAULT_DB_VALUE_15;
			arraySize = 16;
			break;
		case 7:
			defaultDBTable = DEFAULT_DB_VALUE_7;
			arraySize = 8;
			break;
		case 5:
			defaultDBTable = DEFAULT_DB_VALUE_5;
			arraySize = 6;
			break;
		default:
			defaultDBTable = DEFAULT_DB_VALUE_1;
			arraySize = 1;
			break;
		}
	}

	resultArray = env->NewShortArray(arraySize);
	if (resultArray != NULL) {
		resultArrayPtr = env->GetShortArrayElements(resultArray, NULL);
	} else {
		return NULL;
	}

	memcpy(resultArrayPtr, defaultDBTable, arraySize * sizeof(jshort));
	
	fp = fopen(organNVMFile, "rb");
	if (fp == NULL) {
		return resultArray;
	}
	
	if (fread(&header, sizeof(NVM_Header_ts), 1, fp) != 1) {
		fclose(fp);
		return resultArray;
	}
	
	for (i = 0; i < (int)header.NumofStructs; i++) {
		if (fread(&param, header.StructSize, 1, fp) != 1) {
			break;
		}
		
		if (param.paramType == paramType
			&& param.deviceType == deviceType
			&& param.streamType == streamType
			&& param.formatType == formatType) {
				resultArrayPtr[param.volumeIndex] = param.dBIndex;
			}
	}
		
	fclose(fp);
	return resultArray;
}

void com_marvell_organ_NVMFileTool_write(JNIEnv *env, jobject obj, jshort paramType, jshort deviceType, jshort streamType, jshort formatType, jshortArray step)
{
	FILE * fp;
	NVM_Header_ts header;
	hifiParamHeader param;
	jshort dBValue;
	jshort * stepPtr;
	jshort dBIndex[maxArraySize];
	int arraySize;
    int appendCount;
    unsigned long entryCount;
	int i;
	
	stepPtr = env->GetShortArrayElements(step, NULL);
	if (stepPtr == NULL) {
		return;
	}
	
	if (paramType == 1) { //record
		arraySize = 1;
	} else {
		arraySize = MAX_STREAM_VOLUME[streamType==15 ? 1 : streamType] + 1;
	}

	memcpy(dBIndex, stepPtr, arraySize * sizeof(jshort));
	
	fp = fopen(organNVMFile, "rb+");
	if (fp == NULL) {
        //create a new one
		fp = fopen(organNVMFile, "wb");
        if (fp == NULL) {
            return;
        }

        memset(&header, 0, sizeof(header));
        header.StructSize = sizeof(hifiParam);
        strcpy(header.StructName, "hifiParam");
        fwrite(&header, sizeof(header), 1, fp);
        fclose(fp);

        fp = fopen(organNVMFile, "r+");
        if (fp == NULL) {
            return;
        }
	}
	
	if (fread(&header, sizeof(NVM_Header_ts), 1, fp) != 1) {
		fclose(fp);
		return;
	}
	
	for (i = 0; i < (int)header.NumofStructs; i++) {
		if (fread(&param, sizeof(param), 1, fp) != 1) {
			fclose(fp);
			return;
		}
		
		if (param.paramType == paramType
			&& param.deviceType == deviceType
			&& param.streamType == streamType
			&& param.formatType == formatType) {
            fseek(fp, 0L, SEEK_CUR);//read can not followed by write
			fwrite(&dBIndex[param.volumeIndex], sizeof(jshort), 1, fp);
            fseek(fp, 0L, SEEK_CUR);//write can not followed by read
			dBIndex[param.volumeIndex] = -1;
		} else {
			fread(&dBValue, sizeof(jshort), 1, fp);
		}
	}
	
	//handle newly added parameters
    fseek(fp, 0L, SEEK_CUR);
    appendCount = 0;
	for (i = 0; i < arraySize; i++) {
		if (dBIndex[i] != -1) {
			param.paramType = paramType;
			param.deviceType = deviceType;
			param.streamType = streamType;
			param.formatType = formatType;
			param.volumeIndex = i;
			fwrite(&param, sizeof(param), 1, fp);
			fwrite(&dBIndex[i], sizeof(jshort), 1, fp);
            appendCount++;
		}
	}

    //update file header
    fseek(fp, sizeof(unsigned long), SEEK_SET);
    entryCount = header.NumofStructs + appendCount;
    fwrite(&entryCount, sizeof(unsigned long), 1, fp);

	fclose(fp);
}


static JNINativeMethod method_table[] = {
  	{"read", "(SSSS)[S", (void*)com_marvell_organ_NVMFileTool_read},
  	{"write", "(SSSS[S)V", (void*)com_marvell_organ_NVMFileTool_write},
};

int register_com_marvell_organ_NVMFileTool(JNIEnv *env)
{
    return AndroidRuntime::registerNativeMethods(
        env, "com/marvell/organ/NVMFileTool",
        method_table, NELEM(method_table));
}


/*
 *  * JNI registration.
 *   */

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;


    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        LOGE("GetEnv failed");
        goto bail;
    }
    assert(env != NULL);

    if (register_com_marvell_organ_NVMFileTool(env) < 0) {
        LOGE("Register failed");
        goto bail;
    }

    result = JNI_VERSION_1_4;

bail:
    return result;
}


