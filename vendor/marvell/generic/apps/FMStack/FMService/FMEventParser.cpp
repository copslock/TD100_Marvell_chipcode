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

//#define EVENT_DUMP_DEBUG
 
#define LOG_TAG	"FMRadio"
#include "FMEventParser.h"
#include <utils/Log.h>
#include <fcntl.h>

#ifdef EVENT_DUMP_DEBUG
#define EVENT_PCM_LOC "/data/fmEvent_debug"
#endif
namespace android{

FmEventParser::FmEventParser(){
#ifdef EVENT_DUMP_DEBUG
    fd = open(EVENT_PCM_LOC, O_WRONLY | O_CREAT | O_TRUNC);
    if (fd < 0)
        LOGE("Error open %s.", EVENT_PCM_LOC);
#endif
    psName = (char *)malloc(RDS_PS_NAME_SIZE + 1);
    psName_old = (char *)malloc(RDS_PS_NAME_SIZE + 1);
    *psName = *psName_old = 0;
    type = type_old = -1;
    rssi = rssi_old = -1;
    mono_status = mono_status_old = -1;
}

FmEventParser::~FmEventParser(){
#ifdef EVENT_DUMP_DEBUG
    if (fd >0)
        close(fd);
#endif
    if (psName)
        free(psName);
    if (psName_old)
        free(psName_old);
}

int FmEventParser::getType(){
    type_old = type;
    return type;
}

int FmEventParser::getRssi(){
    rssi_old = rssi;
    return rssi;
}

int FmEventParser::getMonoStatus(){
    mono_status_old = mono_status;
    return mono_status;
}

char* FmEventParser::getPsName(){
    memcpy(psName_old, psName, RDS_PS_NAME_SIZE + 1);
    return psName;
}

/*
** @true, Got an event different from last one reported.
** @false, Not received a complete/recognized event or no change.
*/
bool FmEventParser::parse(char* buf, int size){
    int event;
    int index = 0;    
	int b1,b2,b3,b4,c,d,rds_filled = 0;
    int ret = false;
    int grouptype = 0;

    if (size < 4){
        return false;
    }
    
#ifdef EVENT_DUMP_DEBUG
    if (fd > 0){
        write(fd, buf, size);
    }
#endif    

    event = buf[index++];

    switch(event){
        case FM_EVENT_CUR_RSSI:
            rssi = get24(&buf[index]);
            if (rssi != rssi_old){
                type = CUR_RSSI;
                ret = true;
            }
            break;
        case FM_EVENT_MONO_STATUS:
            mono_status = get24(&buf[index]);
            if (mono_status != mono_status_old){
                type = MONO_STATUS;
                ret = true;
            }
            break;        
        case FM_EVENT_RDS_DATA:              
            do{
                b1 = get16(&buf[index]); index += 2;
                b2 = get16(&buf[index]); index += 2;
                b3 = get16(&buf[index]); index += 2;
                b4 = get16(&buf[index]); index += 2;    

                /* get group type */
                grouptype =  (b2 & 0xf800) >> 11;		

                switch(grouptype){
                    case FM_RDS_GPTP_0A:
                    case FM_RDS_GPTP_0B:
                        /* get c[0,1] */    
                        c = b2 & 0x0003;	
                        rds_filled |= 1 << c;	
                        /*
                        ** get Program Service Name, ignore illegal characters
                        ** which value is smaller than 0x20 in ASCII table
                        */
                        psName[(c << 1)] = ((b4>>8) < 0x20) ? 0x20 : (b4>>8);
                        psName[(c << 1) + 1] = ((b4&0xff) < 0x20) ? 0x20 : (b4&0xff);
                        if ((c == 3) && (rds_filled == 15)){
                            rds_filled = 0;
                            psName[8]=0;

                            if (strncmp(psName, psName_old, 9)){
                                ret = true;
                                type = PRG_SRV_NAME;
                            }
                        }
                        break;                        
                }
            }while (index < size);
            break;
    }

    return ret;
    
}

int FmEventParser::get24(char *buf){
    int index = 0;

    int h, m, l;

    l = buf[index++];
    m = buf[index++];
    h = buf[index];
    
    return (h << 16) + (m << 8) + l;
}    

int FmEventParser::get16(char *buf){
    int index = 0;

    int h, l;

    l = buf[index++];
    h = buf[index];
    
    return (h << 8) + l;
}    
}//end android
