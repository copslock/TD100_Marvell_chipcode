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
#include <sys/socket.h>
#include <sys/un.h>
#include <dirent.h>
#include <hardware_legacy/power.h>
#include <sys/reboot.h>
#include <cutils/properties.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <com_marvell_powersetting_PowerFreq.h>
#include "cpufreq.h"
#include "cpufreqd_remote.h"


#define LOG_TAG "PowerSettingJni"

#define CMD_SIZE 4096
#define RTS_SIZE 4096
#define OP_NUM 100
#define WAKELOCK_NUM 100
#define IDLE_WAKELOCK_LEN 10

#define CPU0 0

#define OPS "/sys/devices/system/cpu/cpu0/ops"
#define CUR_OP "/sys/devices/system/cpu/cpu0/cur_op"

#define MIN_FREQ "/sys/devices/system/cpu/cpu0/min_freq"
#define DISABLE_CPUFREQ "/sys/devices/system/cpu/cpu0/disable_cpufreq"
#define IDLE_WAKELOCK "/sys/devices/system/cpu/cpu0/idle_wake_lock"
#define DISABLE_IDLE "/sys/devices/system/cpu/cpu0/disable_idle"

#define WAKELOCKS "/proc/wakelocks"

#define MAKE_COMMAND(cmd, arg)  ((cmd << CMD_SHIFT) | arg)
#define REMOTE_CMD(c)		(c >> CMD_SHIFT)
#define REMOTE_ARG(c)		(c & ARG_MASK)

using namespace android;

struct _wakelock{
	char name[256];
	long long count;
	long long expire_count;
	long long wake_count;
	long long active_since;
	long long total_time;
	long long sleep_time;
	long long max_time;
	long long last_change;
};

struct _wakelock wakelocks[WAKELOCK_NUM];


int catEntry(const char* entry, char * rts)
{
	FILE * fd= fopen(entry, "rb");
	char tmp[RTS_SIZE];
        if(fd==NULL){
                LOGE("cannot open %s\n",entry);
                return -1;
	}
        if(fgets(rts,RTS_SIZE,fd) <= 0) {
                LOGE("Nothing is read out");
                return -1;
	}
        
	fclose(fd);
	return 0;
}

int echoEntry(const char *entry, const char *data)
{
	int fd= open(entry, O_WRONLY);
        if(fd < 0){
                LOGE("cannot open %s\n",entry);
                return -1;
        }
        if(write(fd, data, strlen(data)) <= 0) {
                LOGE("Nothing is written");
                return -1;
        }
        
	close(fd);
	return 0;
}

int igetActiveWakelocks()
{
        char rts[RTS_SIZE];
	char *p;
        
	FILE * fd= fopen(WAKELOCKS, "rb");
        if(fd==NULL){
                LOGE("cannot open %s\n",WAKELOCKS);
                return -1;
        }else{
                LOGD("opened file %s", WAKELOCKS);
        }

	int i = 0;
	memset(rts, 0, RTS_SIZE);
        while(fgets(rts,RTS_SIZE,fd)) {
                if(strncmp(rts, "\"", 1) == 0){
			//name count expire_count wake_count active_since total_time sleep_time max_time last_change
			
			p = strtok(rts, "\t");
			if(p != NULL){
				strcpy(wakelocks[i].name, p);
			}
                        
			p = strtok(NULL, "\t");
			if(p != NULL){
				wakelocks[i].count = atoll(p);
			}

			p = strtok(NULL, "\t");
			if(p != NULL){
				wakelocks[i].expire_count = atoll(p);
			}

			p = strtok(NULL, "\t");
			if(p != NULL){
				wakelocks[i].wake_count = atoll(p);
			}

			p = strtok(NULL, "\t");
			if(p != NULL){
				wakelocks[i].active_since = atoll(p);
			}

			p = strtok(NULL, "\t");
			if(p != NULL){
				wakelocks[i].total_time = atoll(p);
			}
			
			p = strtok(NULL, "\t");
			if(p != NULL){
				wakelocks[i].sleep_time = atoll(p);
			}

			p = strtok(NULL, "\t");
			if(p != NULL){
				wakelocks[i].max_time = atoll(p);
			}

			p = strtok(NULL, "\t");
			if(p != NULL){
				wakelocks[i].last_change = atoll(p);
			}
		
			i++;
		}//end of if
		memset(rts, 0, RTS_SIZE);
        }

	strcpy(wakelocks[i].name, "last");

        fclose(fd);
        return 0;

}

static int cpufreqd_dirs(const struct dirent *d) {
	return (strncmp(d->d_name, "cpufreqd-", 9) == 0);
}

int openConnectionToCpufreqd(){

	int sock;
	struct dirent **namelist = NULL;
	struct sockaddr_un sck;
	struct stat st;
	time_t last_mtime = 0;
	long int n = 0;
	char *endptr = NULL;
	char buf[108];
	
	sck.sun_family = AF_UNIX;
	sck.sun_path[0] = '\0';
	/* get path */
	n = scandir("/tmp", &namelist, &cpufreqd_dirs, NULL);
	if (n > 0) { 
		while (n--) {
			snprintf(buf, 108, "/tmp/%s", namelist[n]->d_name);
			free(namelist[n]);

			if (stat(buf, &st) != 0) {
				fprintf(stderr, "%s: %s\n", buf, strerror(errno));
				continue;
			}

			if (last_mtime == 0 || last_mtime < st.st_mtime) {
				last_mtime = st.st_mtime;
				snprintf(sck.sun_path, 108,"%s/cpufreqd", buf);
			}

		}
		free(namelist);

	} else {
		fprintf(stderr, "No cpufreqd socket found\n");
		return ENOENT;
	}

	if (!sck.sun_path[0]) {
		fprintf(stderr, "No cpufreqd socket found\n");
		return ENOENT;
	}

	LOGD("socket I'll try to connect: %s\n", sck.sun_path);

	
	if ((sock = socket(PF_UNIX, SOCK_STREAM, 0)) == -1) {
		LOGE("socket() failed %s", strerror(errno));
		return errno;
	}
	
	if (connect(sock, (struct sockaddr *)&sck, sizeof(sck)) == -1) {
		LOGE("connect() failed %s", strerror(errno));
		close(sock);
		return errno;
	}

	LOGD("connection to socket %s done\n", sck.sun_path);
	return sock;

}

int writeCmdtoCpufreqd(int sock, int cmd){

	if (write(sock, &cmd, 4) != 4){
		LOGE("write() failed %s", strerror(errno));
		return -1;
	}

	LOGD("writeCmdtoCpufreqd %s done", cmd);
	return 0;
}

int closeConnectionToCpufreqd(int sock){
	close(sock);

	LOGD("closeConnectionToCpufreqd");
	return 0;
}


static jint
setCpufreqdManual(JNIEnv *env, jobject clazz)
{
	unsigned int cmd = MAKE_COMMAND(CMD_SET_MODE, MODE_MANUAL); 	

	int sock = openConnectionToCpufreqd();	
	if(sock < 0){
		LOGE("connect to cpufreqd socket error");
		return -1;
	}	
	int ret = writeCmdtoCpufreqd(sock, cmd);
	if(ret < 0){
		LOGE("write to cpufreqd socket failed");
		return -1;
	}
	closeConnectionToCpufreqd(sock);

	return 0;
}

static jint
setCpufreqdAuto(JNIEnv *env, jobject clazz)
{
	unsigned int cmd = MAKE_COMMAND(CMD_SET_MODE, MODE_DYNAMIC); 	
	
	int sock = openConnectionToCpufreqd();
	if(sock < 0){
		LOGE("connect to cpufreqd sock error");
		return -1;
	}	
	int ret = writeCmdtoCpufreqd(sock, cmd);	
	if(ret < 0){
		LOGE("write to cpufreqd socket failed");
		return -1;
	}
	
	closeConnectionToCpufreqd(sock);

	return 0;
}


static jint
setProfile(JNIEnv *env, jobject clazz, jstring freq)
{
	//Get the native string from javaString
	const char *nativeString = env->GetStringUTFChars(freq, 0);   
	char buf[4096] = {0}, name[256] = {0}, policy[255] = {0};
	char *in;
	int min, max, active, n;
	
	LOGD("Trying to set freq to %s", nativeString);
	
	unsigned int cmd = MAKE_COMMAND(CMD_LIST_PROFILES, 0); 	
	
	int sock = openConnectionToCpufreqd();	
	if(sock < 0){
		LOGE("connect to cpufreqd socket error");
		return -1;
	}	
	
	int ret = writeCmdtoCpufreqd(sock, cmd);	
	if(ret < 0){
		LOGE("write to cpufreqd socket failed");
		return -1;
	}

	n = 0;
	while (read(sock, buf, 4096)) {
		int is_active = 0;
		in = buf;
		while (sscanf(in, "%d/%[^/]/%d/%d/%[^\n]\n", &active, name, &min, &max, policy) == 5){ 
			LOGD("active %d, name %s, min %d, max %d, policy %s", active, name, min, max, policy);
			n++;
			in = strchr(in, '\n') + 1;

			if(strstr(name, "Manual") != 0 && strstr(name, nativeString) != 0){
				break;	
			}
			
		}

	}
	
	closeConnectionToCpufreqd(sock);
	
	sock = openConnectionToCpufreqd();	
	if(sock < 0){
		LOGE("connect to cpufreqd socket error");
		return -1;
	}	
	LOGD("choose profile %d", n);
	cmd = MAKE_COMMAND(CMD_SET_PROFILE, n); 	
	ret = writeCmdtoCpufreqd(sock, cmd);	
	if(ret < 0){
		LOGE("write to cpufreqd socket failed");
		return -1;
	}

	closeConnectionToCpufreqd(sock);

	env->ReleaseStringUTFChars(freq, nativeString); 
	return 0;
}

static jint
setScalingMinFreq(JNIEnv *env, jobject clazz, jint freq)
{
	LOGD("set scaling min freq to %d", freq);
	if (cpufreq_modify_policy_min(CPU0, freq) != 0){
		LOGD("set min cpu freq to %d failed: %s", freq, strerror(errno));
		return -1;
	}
	
	return 0;
}


static jint
getHwMinFreq(JNIEnv *env, jobject clazz)
{
	LOGD("get min freq");
	unsigned long minFreq, maxFreq;
	if (cpufreq_get_hardware_limits(CPU0, &minFreq, &maxFreq) != 0){
		LOGD("get min freq failed: %s", strerror(errno));
		return -1;
	}
	
	return (int)minFreq;
}


static jint
setGovernor(JNIEnv *env, jobject clazz, jstring governor)
{

	//Get the native string from javaString
	const char *nativeString = env->GetStringUTFChars(governor, 0);   
	LOGD("set governor to %s", nativeString);

	if (cpufreq_modify_policy_governor(CPU0, (char *)nativeString) != 0){
		LOGD("set governor to %s failed with %s", nativeString, strerror(errno));
		return -1;
	}
	env->ReleaseStringUTFChars(governor, nativeString); 
	return 0;
}

static jint
startPolicy(JNIEnv *env, jobject clazz)
{

	//Get the native string from javaString
	LOGD("start policy");
       
	property_set("persist.service.pp.enable", "1");
	return 0;
}

static jint
stopPolicy(JNIEnv *env, jobject clazz)
{

	//Get the native string from javaString
	LOGD("stop policy");
       
	property_set("persist.service.pp.enable", "0");
	return 0;
}

static jint
isPolicyStarted(JNIEnv *env, jobject clazz)
{
	char status[8];
	property_get("persist.service.pp.enable", status, "2");

	int start = atoi(status);
	return start;
}

static jint
setCpuFreq(JNIEnv *env, jobject clazz, jint freq)
{
	LOGD("set cpu freq to %d", freq);
	if (cpufreq_set_frequency(CPU0, freq) != 0){
		LOGD("set cpu freq to %d failed: %s", freq, strerror(errno));
		return -1;
	}
	
	return 0;
}

static jint
getCurrentCpuFreq(JNIEnv *env, jobject clazz)
{
	unsigned long curFreq = cpufreq_get_freq_kernel(CPU0);
	LOGD("get long cur freq %ld\n", curFreq);
	return curFreq;
}

static jstring
getCurrentOtherFreqs(JNIEnv *env, jobject clazz, jint cpufreq)
{
	
        char rts[RTS_SIZE];
        char *strFreq;
        int curfreq = 0;
        int op;
        FILE * fd= fopen(CUR_OP, "rb");

        if(fd==NULL){
                LOGE("cannot open %s\n",CUR_OP);
                return NULL;
        }else{
                LOGD("opened file %s", CUR_OP);
        }

        while(fgets(rts,RTS_SIZE,fd)) {
                if(strncmp(rts, "OP name:", 8) == 0){
                        curfreq = atoi((const char*)(rts+8)); //OP name:624MHz
                	if(cpufreq != curfreq*1000){
				LOGD("CUR_OP %d and SCALING_CUR_FREQ %d doesn't match", curfreq, cpufreq);
				continue;
			}
			fgets(rts, RTS_SIZE, fd);
			jstring str = (env)->NewStringUTF(rts);
			fclose(fd);
			return str;
		}else{
                        continue;
                }
        }

        fclose(fd);

	return NULL;
}

static jint
getMinFreq(JNIEnv *env, jobject clazz)
{

	char rts[RTS_SIZE];
	int minFreq = 0;

	catEntry(MIN_FREQ, rts);
	minFreq = atoi(rts);	

	return minFreq;
}

static jint
getCpufreqStatus(JNIEnv *env, jobject clazz)
{
	char rts[RTS_SIZE];
	int status;

	catEntry(DISABLE_CPUFREQ, rts);
	status = atoi(rts);	

	return status;
}

static jint
getIdleStatus(JNIEnv *env, jobject clazz)
{

	char rts[RTS_SIZE];
	catEntry(DISABLE_IDLE, rts);

	int status = atoi(rts);	

	return status;
}

static jint
disableIdle(JNIEnv *env, jobject clazz)
{

	char data[CMD_SIZE];
	LOGD("disable idle");

	sprintf(data, "%d", 1);
	echoEntry(DISABLE_IDLE, data);

	return 0;
}

static jint
enableIdle(JNIEnv *env, jobject clazz)
{

	char data[CMD_SIZE];
	LOGD("enable idle");

	sprintf(data, "%d", 0);
	echoEntry(DISABLE_IDLE, data);

	return 0;
}


static jobjectArray
getIdleWakelocks(JNIEnv *env, jobject clazz)
{
	jobjectArray args = 0;
	jstring str;
	char rts[RTS_SIZE];

        FILE * fd= fopen(IDLE_WAKELOCK, "rb");

        if(fd==NULL){
                LOGE("cannot open %s\n",IDLE_WAKELOCK);
                return NULL;
        }else{
                LOGD("opened file %s", IDLE_WAKELOCK);
        }

	args = (env)->NewObjectArray(IDLE_WAKELOCK_LEN,(env)->FindClass("java/lang/String"),0);
        
	int i = 0;
	while(fgets(rts,RTS_SIZE,fd)) {
		str = (env)->NewStringUTF(rts);
		(env)->SetObjectArrayElement(args, 0, str);
		i++;
	}

        fclose(fd);

	return args;
}

static jobjectArray
getActiveWakelocks(JNIEnv *env, jobject clazz)
{
	jobjectArray args = 0;
	jstring str;
	char wakelock[1024];

	if (igetActiveWakelocks() != 0){
		LOGD("get active wake lock failed");
		return NULL;
	}
	
	int i = 0;
	int count = 0;
	int len = 0;

	for(i=0; i<WAKELOCK_NUM; i++){
        	if(strcmp(wakelocks[i].name, "last") == 0){
			break;
		}
		if(wakelocks[i].active_since != 0){
			len++;
		}
		count++;
	}

	args = (env)->NewObjectArray(len,(env)->FindClass("java/lang/String"),0);

	int j = 0;
	for(i = 0; i < count; i++){
		if(wakelocks[i].active_since != 0){
			str = (env)->NewStringUTF(wakelocks[i].name);
			(env)->SetObjectArrayElement(args, j, str);
			j++;
		}
	}

	return args;
}



static jintArray
getAllEnabledFreqs(JNIEnv *env, jobject clazz)
{
	int enabledFreqs[OP_NUM];
	struct cpufreq_available_frequencies *head;
	struct cpufreq_available_frequencies *freqs;

	head = cpufreq_get_available_frequencies(CPU0);

	int i = 0;
	freqs = head;
	do{
		LOGD("freq is %d, i  is %d\n", freqs->frequency, i);
		enabledFreqs[i] = freqs->frequency;
		if(freqs->next == NULL){
			break;
		}
		freqs = freqs->next;
		i++;
	}while(true);
	
	int len = i + 1;

	LOGD("len is %d\n", len);	
	jintArray args = (env)->NewIntArray(len);
	if(args == NULL){
		return NULL;
	}

	(env)->SetIntArrayRegion(args, 0, len, enabledFreqs);
	
	return args;

}

static JNINativeMethod method_table[] = {
  	//{"setCpuFreq", "(I)I", (void*)setCpuFreq},
  	{"getCurrentCpuFreq", "()I", (void*)getCurrentCpuFreq},
  	{"getCurrentOtherFreqs", "(I)Ljava/lang/String;", (void*)getCurrentOtherFreqs},
  	{"getAllEnabledFreqs", "()[I", (void*)getAllEnabledFreqs},
  	{"getIdleWakelocks", "()[Ljava/lang/String;", (void*)getIdleWakelocks},
  	{"getActiveWakelocks", "()[Ljava/lang/String;", (void*)getActiveWakelocks},
  	{"getMinFreq", "()I", (void*)getMinFreq},
  	{"getCpufreqStatus", "()I", (void*)getCpufreqStatus},
  	{"getIdleStatus", "()I", (void*)getIdleStatus},
  	{"disableIdle", "()I", (void*)disableIdle},
  	{"enableIdle", "()I", (void*)enableIdle},
  	//{"setGovernor", "(Ljava/lang/String;)I", (void*)setGovernor},
  	//{"setScalingMinFreq", "(I)I", (void*)setScalingMinFreq},
  	//{"getHwMinFreq", "()I", (void*)getHwMinFreq},
  	{"setProfile", "(Ljava/lang/String;)I", (void*)setProfile},
  	{"setCpufreqdManual", "()I", (void*)setCpufreqdManual},
  	{"setCpufreqdAuto", "()I", (void*)setCpufreqdAuto},
  	//{"startPolicy", "()I", (void*)startPolicy},
  	//{"stopPolicy", "()I", (void*)stopPolicy},
  	//{"isPolicyStarted", "()I", (void*)isPolicyStarted},
};

int register_com_marvell_powerpolicy_PowerFreq(JNIEnv *env)
{
	
    return AndroidRuntime::registerNativeMethods(
        env, "com/marvell/powersetting/PowerFreq",
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

    if (register_com_marvell_powerpolicy_PowerFreq(env) < 0) {
        LOGE("Register failed");
        goto bail;
    }

    result = JNI_VERSION_1_4;

bail:
    return result;
}


