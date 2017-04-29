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

#ifndef FMEVENTPARSER_H
#define FMEVENTPARSER_H

#define FM_EVENT_RDS_DATA 0x81
#define FM_EVENT_CUR_RSSI 0x82
#define FM_EVENT_MONO_STATUS 0x83
#define FM_RDS_GPTP_0A 0x00
#define FM_RDS_GPTP_0B 0x01
#define RDS_PS_NAME_SIZE 8
namespace android{
class FmEventParser{
public:
    enum{
        PRG_SRV_NAME = 0,
        CUR_RSSI,
        MONO_STATUS
    };
    
    FmEventParser();
    ~FmEventParser();    
    bool parse(char *buf, int size);
    int getType();
    int getRssi();
    int getMonoStatus();
    char *getPsName();
    
private:
    int type;
    int type_old;
    long rssi;
    int rssi_old;
    int mono_status;
    int mono_status_old;
    char *psName;
    char *psName_old;

    int get24(char *);
    int get16(char *);    
#ifdef EVENT_DUMP_DEBUG
    int fd;
#endif    
};

};//end android
#endif //FmEventParser
