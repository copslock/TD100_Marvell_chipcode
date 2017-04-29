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

#define LOG_TAG "BatteryCalibration"
#define LOG_NDEBUG 0

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <utils/Log.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include "FileMonitorSystem.h"


using namespace android;

#define PATH_MAX 4096
#define MEMBER_MAX 64
#define CMD_MAX 128

#define NVM_ROOT_DIR "/data/Linux/Marvell/NVM"
#define BATTERY_NVM "BatteryCalData.nvm"
#define SANREMO_CALI "/sys/devices/platform/pxa2xx-i2c.0/i2c-0/0-0034/88pm860x-battery.0/calibration"

typedef struct NVM_Header
{
    unsigned int  StructSize; // (UINT32) the size of the user structure below
    unsigned int  NumOfStruct; // >1 in case of array of structures (default is 1).
    char    StructName[64];// the name of the user structure below
    char    Date[32];  // date updated by the ICAT when the file is saved.
    char    Time[32];  // time updated by the ICAT when the file is saved.
    char    Version[64];   // this is the field the SW Eng. Will update for a new structure.
    char    HW_ID[32];     // this field signifies the board number.
    char    CustomVersion[32]; // calibration  version signifies the calibration SW version.

}NVMStructHeader;

typedef struct
{
         unsigned char d4;
         unsigned char d5;
         unsigned char d7;
         signed short offset_ibat;
         signed short offset_vbat;
         unsigned short slope_low;
         unsigned short slope_high;
}CaliReturnStruct;


class BatteryCalibrationCallback : public FileMonitorCallback {
public:
    NVMStructHeader header;
    CaliReturnStruct caliData; 
    
    BatteryCalibrationCallback(const String8& name)
        : FileMonitorCallback(name)
    {
    }

    void reloadFile()
    {
        //reload file content...
	char path[PATH_MAX];
        LOGD("Reloading file content for battery calibration");
			
	snprintf(path, sizeof(path), "%s/%s", NVM_ROOT_DIR, BATTERY_NVM); 
	FILE *nvm = fopen(path, "rb");
	if (nvm == NULL){
        	LOGD("Reloading file %s failed", path);
		return;
	}
	fillNVMHeader(nvm);
	fillPayload(nvm);
	doCalibration();	
    }

private:
	void doCalibration(){
	
		LOGD("DoCalibration: set data");	

		char cmd[CMD_MAX];	
		snprintf(cmd, sizeof(cmd), "d4value %d", caliData.d4);
		setCaliData(cmd);
 
		snprintf(cmd, sizeof(cmd), "d5value %d", caliData.d5);
		setCaliData(cmd);
 
		snprintf(cmd, sizeof(cmd), "d7value %d", caliData.d7);
		setCaliData(cmd);
 
		snprintf(cmd, sizeof(cmd), "ibat_offset %d", caliData.offset_ibat);
		setCaliData(cmd);
 
		snprintf(cmd, sizeof(cmd), "vbat_offset %d", caliData.offset_vbat);
		setCaliData(cmd);
 
		snprintf(cmd, sizeof(cmd), "vbat_slope_low %d", caliData.slope_low);
		setCaliData(cmd);
 
		snprintf(cmd, sizeof(cmd), "ibat_slope_high %d", caliData.slope_high);
		setCaliData(cmd);
 
	}


	int setCaliData(const char* data){

		int fd = open(SANREMO_CALI, O_WRONLY);
		if(fd < 0){
                	LOGE("cannot open %s\n",SANREMO_CALI);
                	return -1;
		}
		if(write(fd, data, strlen(data)) <= 0) {
			LOGE("Nothing is written");
			return -1;
		}

		close(fd);
		return 0;
	}

	void fillPayload(FILE *nvm){
		LOGD("Filling NVM payload");
		
		readCharFromFile((char*)&(caliData.d4), 1, nvm);	
		readCharFromFile((char*)&(caliData.d5), 1, nvm);	
		readCharFromFile((char*)&(caliData.d7), 1, nvm);
		skipCharFromFile(1, nvm);

		caliData.offset_ibat = readShortFromFile(nvm);
		caliData.offset_vbat = readShortFromFile(nvm);
		caliData.slope_low = readShortFromFile(nvm);
		caliData.slope_high = readShortFromFile(nvm);

		LOGD("Cali Data is %x %x %x %d %d %d %d", caliData.d4, caliData.d5, caliData.d7, caliData.offset_ibat, caliData.offset_vbat, caliData.slope_low, caliData.slope_high);
	
	}

	void fillNVMHeader(FILE *nvm){
		LOGD("Filling NVM header");

		header.StructSize = readIntFromFile(nvm);
//		fscanf(nvm, "%d", &(header.StructSize));
		
		header.NumOfStruct = readIntFromFile(nvm);

		readCharFromFile(header.StructName, 64, nvm);
		readCharFromFile(header.Date, 32, nvm);
		readCharFromFile(header.Time, 32, nvm);
		readCharFromFile(header.Version, 64, nvm);
		readCharFromFile(header.HW_ID, 32, nvm);
		readCharFromFile(header.CustomVersion, 32, nvm);
		
		LOGD("header is %d %d %s %s %s %s %s %s", header.StructSize, header.NumOfStruct, header.StructName, header.Date, header.Time, header.Version, header.HW_ID, header.CustomVersion);
	}

	int readIntFromFile(FILE* file){
		char tmp[4];
		fread(tmp, 4, 1, file);
		return ((tmp[3] << 24) + (tmp[2] << 16) + (tmp[1] << 8) + tmp[0]);
	}

	int readShortFromFile(FILE* file){
		char tmp[2];
		fread(tmp, 2, 1, file);
		return ( (tmp[1] << 8) + tmp[0]);
	}

	void readCharFromFile(char *buf, int size, FILE *file){
		fread(buf, size, 1, file);
	}
	
	void skipCharFromFile(int size, FILE *file){
		char tmp[MEMBER_MAX];
		fread(tmp, size, 1, file);
	}
};

int main(int argc, char** argv)
{
	char path[PATH_MAX];
	
	if (argc == 2)
	{
		snprintf(path, sizeof(path), "%s", argv[1]); 
	}else if(argc == 1){
		
		char* nvmRootDir = getenv("NVM_ROOT_DIR");
		LOGD("NVM_ROOT_DIR is %s", nvmRootDir);
			
		snprintf(path, sizeof(path), "%s", BATTERY_NVM); 
	}

    FileMonitorSystem::registerCallback(new BatteryCalibrationCallback(String8(path)));

    ProcessState::self()->startThreadPool();
    IPCThreadState::self()->joinThreadPool();

    return 0;
}


