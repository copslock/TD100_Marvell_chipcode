/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: eeh_api.c
 *
 *  Description: The APIs to process error & exception of system.
 *
 *  History:
 *   April, 2008 - Rovin Yu Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/

/******************************************************************************
*    Include files
******************************************************************************/

#include <sys/ioctl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/mman.h>
#if defined (BIONIC)
#include <sys/reboot.h> // reboot, RB_AUTOBOOT
#endif

#include "osa.h"
#include "pxa_dbg.h"
#include "EE_Postmortem.h"
#include "EEHandler.h"
#include "EEHandler_int.h"
#include "eeh.h"
#include "eeh_assert_notify.h"
#include "msocket_api.h"
#include "NVMServer_api.h"



#define EEH_TASK_PRIORITY   100
#define EEH_STACK_SIZE      2048

#define EEH_DEVICE "/dev/seh"

#define TIOPPPOFF _IOW('T', 209, int)

static UINT8 EehTaskStack[EEH_STACK_SIZE];
static OSTaskRef EehTaskRef   = NULL;

int sehdrv_fd = 0;

static UINT32 iSilentRestsCount = 1;
static int currentAssert = EEH_NONE_ASSERT;

int gDumpHugeMemory = 0;
int b_launch_diag = 0;

void SaveSilentResetStatistics(UINT32 msgId);
EEH_STATUS EehInsertCommtoReset(void);
EEH_STATUS EehReleaseCommFromReset(void);
EEH_STATUS EehCopyCommImageFromFlash(void);
static int  EehRamdump(void);
static void EehSaveErrorInfo(unsigned err, char *str, unsigned char *context);
EEH_STATUS EehDisableCPUFreq(void);
EEH_STATUS EehEnableCPUFreq(void);


static void EehSaveCommon(const char* buf)
{      
	UNUSEDPARAM(buf);
//	char *nvm_root_dir = NULL;
	
//To save more useful logs

//    EehSaveThreadInfo(NULL);

//	EehSaveNvmBackup(nvm_root_dir);
}

static void EehFinalAction(UINT32 action)
{
	sync(); /* prepare for potential system reset */
	switch (action)
	{
	case EE_STALL:		   ////do nothing - BSP_ARGS_EE_NO_RESET
	{
		ERRMSG("[ERROR Handler] ************  No Reset - STALL **************** .\r\n");
		//When CP is asserted, AP should mark the msocket link as down
		msocket_linkdown();
		break;
	}
	case EE_RESET_COMM_SILENT:
	{
		ERRMSG("[ERROR Handler] ************  Silent Reset **************** .\r\n");
	
		//Save silent reset statictics
		//SaveSilentResetStatistics(EEH_WDT_INT_MSG);
	
		time_t tm;
		time(&tm);
		ERRMSG("%s: Fail recovery start at %s", __FUNCTION__, ctime(&tm));
	
		unlink("/tmp/atcmdsrv_ok");
	
		//Disable CPUFreq
		if ( EehDisableCPUFreq() != EEH_SUCCESS )
		{
			ERRMSG("[ERROR Handler] ************  Cannot disable CPUFreq now ******************* .\r\n");
			break;
		}

		//Insert Comm subsystem into Reset
		if ( EehInsertCommtoReset() != EEH_SUCCESS )
		{
			ERRMSG("[ERROR Handler] ************  Cannot reset CP now ******************* .\r\n");
			break;
		}
	
		msocket_linkdown();
	
		//Copy CP&DSP image from Flash to DDR
	#if defined (EE_COPY_COMM_IMAGE_FROM_FLASH)
		EehCopyCommImageFromFlash();
	#endif
	
		//Release Comm subsystem from Reset
		EehReleaseCommFromReset();
	
		msocket_linkup();

		//Enable CPUFreq
		if ( EehEnableCPUFreq() != EEH_SUCCESS )
		{
			ERRMSG("[ERROR Handler] ************  Cannot enable CPUFreq now ******************* .\r\n");
			break;
		}

		break;
	}
	case EE_RESET_GPIO_RESET:
	{
		ERRMSG("[ERROR Handler] ************   GPIO Reset !!! **************** .\r\n");
	
		sync();
#if defined (BIONIC)
		reboot(RB_AUTOBOOT);
#else
		if (system("reboot"))
			ERRMSG("[ERROR Handler] GPIO Reset failed\n");
#endif
		break;
	}
	case EE_RAMDUMP:
	{
		ERRMSG("[ERROR Handler] ************  RAMDUMP Emulate PANIC **************** .\r\n");
	
		EehRamdump(); /*noreturn*/
		break;
	}
	default:
	{
		ERRMSG("[ERROR Handler] ************  Other Reset - unknow id:%d  *************** .\r\n", eeConfiguration.finalAction);
		break;
	}
	}

}
/************************************************************************************
 *
 * EehTask
 *
 * Description: Error handler task function
 *
 * Parameters:
 *
 * Returns:     None
 *
 * Notes:
 *
 **********************************************************************************/
static void EehTask(void* arg)
{
	UNUSEDPARAM(arg)

	DBGMSG("%s: Error Hander Task is running\n", __FUNCTION__);

	for (;; )
	{
		EehMsgStruct msg;
		UINT32 msgId;
		int received = -1;
		static int iCount = 0;
		static int iCountApReq = 0;
		BOOL isCPAssert = TRUE;
		received = read(sehdrv_fd, &msg, sizeof(EehMsgStruct));
		if ( received == sizeof(EehMsgStruct))
		{
			msgId = msg.msgId;
			switch (msgId)
			{
			case EEH_WDT_INT_MSG:
			{
				ERRMSG("EehTask: receiving EEH_WDT_INT_MSG\n");
				eeReadConfiguration(&eeConfiguration);
				NVMS_FileCloseAll(NVM_CLIENT_COMM);
				if (currentAssert != EEH_NONE_ASSERT)
				{
					/* There is assert recovery procedure ongoing, we don't need to recover it again */
					ERRMSG("%s: there is assert recovery procedure ongoing. Reject for this new EEH_WDT_INT_MSG\n", __FUNCTION__);
					return;
				}

				iCount++;

				commImageTableInit();

				if (getCommImageBaseAddr() != INVALID_ADDRESS)
				{
					char eeBuffer[512];
					char eeString[512+60];
					/* Print out the CP Error Buffer Description Info */
					isCPAssert = GetComEEDescbuf(eeBuffer, sizeof(eeBuffer));

					EehSaveCommon(eeBuffer);
					EehSaveErrorInfo(ERR_EEH_CP, eeBuffer, 0);

					if(eeConfiguration.finalAction == EE_RESET_COMM_SILENT)
						sprintf(eeString, "No.%d CP Assert, Cause:%s", iCount, eeBuffer);
					else
						sprintf(eeString, "non silent CP Assert, Cause:%s", eeBuffer);
					
					eeh_draw_panic_text(eeString, strlen(eeString), (int)EEH_CP_EXCEPTION);

					if(eeConfiguration.finalAction != EE_RESET_COMM_SILENT)
					{
						/* Save Comm Post Morten Files */
						SaveComPostmortem();

						/* Output DTCM, DSP & Ram Log */
						Linux_EELOG_shareReadFWrite();
						sync();
					}

					
				}

				//Sometimes the WDT interrupt is triggerred by CP dead loop, not by CP Asserts.
				//In this case, we need to dump the whole CP DDR for analysis
				if(eeConfiguration.finalAction != EE_RESET_COMM_SILENT)
				{
					if (gDumpHugeMemory || !isCPAssert)
					{
						EE_SaveComDDR_RW(1);
						sync();
					}
				#if defined (TD_SUPPORT)
					EE_SaveComDDR_DiagOffline(1);
					sync();
				#endif
				}

				commImageTableFree();

				EehFinalAction(eeConfiguration.finalAction);

				break;
			}
			
			case EEH_AP_ASSERT_MSG:
				ERRMSG("EehTask: receiving EEH_AP_ASSERT_MSG\n");
				eeReadConfiguration(&eeConfiguration);

				EehSaveCommon(msg.msgDesc);
				EehSaveErrorInfo(ERR_EEH_AP, msg.msgDesc, 0);
				
				eeh_draw_panic_text(msg.msgDesc, strlen(msg.msgDesc), (int)EEH_AP_EXCEPTION);
			
				EehFinalAction(eeConfiguration.finalAction);

				break;
				
			case EEH_CP_SILENT_RESET_MSG:
			{
				ERRMSG("EehTask: receiving EEH_CP_SILENT_RESET_MSG\n");
				eeReadConfiguration(&eeConfiguration);
				NVMS_FileCloseAll(NVM_CLIENT_COMM);
				iCountApReq++;
				commImageTableInit();

				if (getCommImageBaseAddr() != INVALID_ADDRESS)
				{
					char eeString[512+60];

					EehSaveCommon(msg.msgDesc);
					EehSaveErrorInfo(ERR_EEH_CP, msg.msgDesc, 0);
					if(eeConfiguration.finalAction == EE_RESET_COMM_SILENT)
						sprintf(eeString, "No.%d CP Reset on AP request, Cause:%s", iCountApReq, msg.msgDesc);
					else
						sprintf(eeString, "No.%d non CP Reset on AP request, Cause:%s", iCountApReq, msg.msgDesc);
					eeh_draw_panic_text(eeString, strlen(eeString), (int)EEH_CP_EXCEPTION);
				}
				//dump the whole CP DDR for analysis
				if(eeConfiguration.finalAction != EE_RESET_COMM_SILENT)
				{
					if (gDumpHugeMemory)
					{
						EE_SaveComDDR_RW(1);
						sync();
					}
				#if defined (TD_SUPPORT)
					EE_SaveComDDR_DiagOffline(1);
					sync();
				#endif
				}

				commImageTableFree();

				EehFinalAction(eeConfiguration.finalAction);

				break;
			}

			default:
				ERRMSG("%s: Invalid msg Id: %d\n", __FUNCTION__, msgId);
				break;
			}
		}
		else
		{
			DBGMSG("[%d]: %s\n", errno, strerror(errno));
			if (errno == EINTR)
				continue;
			else
			{
				ERRMSG("%s:Exit Error Handler Task!\n", __FUNCTION__);
				break;
			}
		}
	}

}


/*************************************************
 * EehInit
 *
 * Descrption:
 *
 * Initialize the error handler device and make first contact with the error handler device.
 *
 * Notes:
 *
 *
 **************************************************/
EEH_STATUS EehInit (void)
{
	EehApiParams ApiParams;
	INT32 ret;
	OS_STATUS status;

	sehdrv_fd = open(EEH_DEVICE, O_RDWR | O_SYNC);
	if ( sehdrv_fd == -1)
	{
		ERRMSG("%s: open seh driver error...\n", __FUNCTION__);
		return EEH_ERROR_INIT;
	}

	eeInitConfiguration(); //create EE file in case not present

	status = OSATaskCreate(&EehTaskRef, EehTaskStack, EEH_STACK_SIZE, EEH_TASK_PRIORITY, (CHAR *)"EehTask", EehTask, NULL);
	if (status != OS_SUCCESS)
	{
		ERRMSG("%s: Error to creat the task\n", __FUNCTION__);
		return EEH_ERROR;
	}

	ApiParams.eehApiId                  = _EehInit;
	ApiParams.params                    = NULL;

	DBGMSG("%s: _EehInit\n", __FUNCTION__);

	ret = ioctl(sehdrv_fd, SEH_IOCTL_API,  &ApiParams);
	if (ret)
	{
		ERRMSG("%s: error=%d\n", __FUNCTION__, errno);
		return EEH_ERROR;
	}

	return EEH_SUCCESS;
}

/*************************************************
 * EehDeinit
 *
 * Descrption:
 * This function terminates all services provided by the EEH.
 *
 * Notes:
 *
 **************************************************/
EEH_STATUS EehDeinit (void)
{
	EehApiParams ApiParams;
	INT32 ret;

	ApiParams.eehApiId                      = _EehDeInit;
	ApiParams.params                        = NULL;

	DBGMSG("%s: _EehDeInit\n", __FUNCTION__);

	ret = ioctl(sehdrv_fd, SEH_IOCTL_API,  &ApiParams);
	if (ret)
	{
		ERRMSG("%s: error=%d\n", __FUNCTION__, errno);
		return EEH_ERROR;
	}

	return ApiParams.status;
}

/*************************************************
 * EehInsertCommtoReset
 *
 * Descrption:
 * This function will insert Comm Subsystem to reset.
 *
 * Notes:
 *
 **************************************************/

EEH_STATUS EehInsertCommtoReset(void)
{
	EehApiParams ApiParams;
	INT32 ret;

	ApiParams.eehApiId                                              = _EehInsertComm2Reset;
	ApiParams.params                                                = NULL;

	DBGMSG("%s: _EehInsertComm2Reset\n", __FUNCTION__);

	ret = ioctl(sehdrv_fd, SEH_IOCTL_API,  &ApiParams);
	if (ret)
	{
		ERRMSG("%s: error=%d\n", __FUNCTION__, errno);
		return EEH_ERROR;
	}

	return ApiParams.status;

}

/*************************************************
 * EehReleaseCommFromReset
 *
 * Descrption:
 * This function will release Comm Subsystem from reset.
 *
 * Notes:
 *
 **************************************************/

EEH_STATUS EehReleaseCommFromReset(void)
{
	EehApiParams ApiParams;
	INT32 ret;

	ApiParams.eehApiId                                              = _EehReleaseCommFromReset;
	ApiParams.params                                                = NULL;

	DBGMSG("%s: _EehReleaseCommFromReset\n", __FUNCTION__);

	ret = ioctl(sehdrv_fd, SEH_IOCTL_API,  &ApiParams);
	if (ret)
	{
		ERRMSG("%s: error=%d\n", __FUNCTION__, errno);
		return EEH_ERROR;
	}

	return ApiParams.status;

}

/*************************************************
 * SaveSilentResetStatistics
 *
 * Descrption:
 * This function will save the silent reset statistics.
 *
 * Notes:
 *
 **************************************************/
void SaveSilentResetStatistics(UINT32 msgId)
{
	UNUSEDPARAM(msgId)

	FILE *fd;
	char fullPath[MAX_PATH] = { 0 };
	char fileName[] = "SilentResetStatistic.log";
	char *nvm_root_dir = getenv("NVM_ROOT_DIR");
	time_t timep;

	if (nvm_root_dir != NULL)
	{
		strcpy(fullPath, nvm_root_dir);
		strcat(fullPath, "/");
	}
	strcat(fullPath, fileName);

	if ((fd = fopen(fullPath, "aw")) != 0)
	{
		time(&timep);
		fprintf(fd, "CPP ERROR causes silent reset No.%d : %s", iSilentRestsCount, ctime(&timep));
		fclose(fd);
		iSilentRestsCount++;
	}
}

/*************************************************
 * EehCopyCommImageFromFlash
 *
 * Descrption:
 * This function will copy Comm image from flash to DDR.
 *
 * Notes:
 *
 **************************************************/

EEH_STATUS EehCopyCommImageFromFlash(void)
{
	EehApiParams ApiParams;
	INT32 ret;

	ApiParams.eehApiId                                              = _EehCopyCommImageFromFlash;
	ApiParams.params                                                = NULL;

	DBGMSG("%s: _EehCopyCommImageFromFlash\n", __FUNCTION__);

	ret = ioctl(sehdrv_fd, SEH_IOCTL_API,  &ApiParams);
	if (ret)
	{
		ERRMSG("%s: error=%d\n", __FUNCTION__, errno);
		return EEH_ERROR;
	}

	return ApiParams.status;
}


#define RAMFILE_HEADER_SIZE 32 /* should match kernel definition for struct ramfile_desc*/
#define RAMFILE_MAX_LEN 0x400000
static int EehPushRamfile(int dfd, const char* file)
{
	char *vpa = 0;
	FILE* ffd = 0;
	int len, l;
	struct stat fst;
	int ret = -1;
	
	if ((ffd=fopen(file, "rb"))==NULL) { 
		fprintf(stderr, "RAMFILE: cannot open %s\n", file);
		goto bail;
	}
	if (fstat(fileno(ffd), &fst)) {
		fprintf(stderr, "RAMFILE: cannot stat %s\n", file);
		goto bail;
	}
	
	len = fst.st_size;
	if ((len<=0) || (len>RAMFILE_MAX_LEN)) {
		fprintf(stderr, "RAMFILE: bad length %d file %s\n", len, file);
		goto bail;
	}
	
	if ((vpa=(char *)mmap(0, len + RAMFILE_HEADER_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, dfd, 0))==MAP_FAILED) {
		fprintf(stderr, "RAMFILE: failed to allocate %d bytes for file %s\n", len, file);
		goto bail;
	}
	printf("RAMFILE vpa=%.8x\n", (unsigned)vpa);

	if ((l = fread((void*)(vpa+RAMFILE_HEADER_SIZE), 1, len, ffd)) != len) {
		fprintf(stderr, "RAMFILE: failed to read %d bytes from %s, ret=%d\n", len, file, l);
		goto bail;
	}

	fprintf(stderr, "RAMFILE: pushed %d bytes from %s\n", len, file);
	/* OK, data is in memory */
	ret = 0;
bail:
	if (ffd)
		fclose(ffd);
	if (vpa)
		munmap(vpa, len);
	return ret;
}

#define RAMFILE_TMP "ramfiletmp.tgz" /* temp tgz filename */
#define LISTFILE_TMP "/tmp/sdl-list"
static int EehRamdumpPushLogfiles(int fd)
{
	char tmpfilename[MAX_PATH];
	char command_buf[MAX_PATH+50];
	int ret	= -1;
	char *nvm_root_dir = getenv("NVM_ROOT_DIR");

	if (nvm_root_dir == NULL)
		return ret;
	
	sprintf(tmpfilename, "%s/%s", nvm_root_dir, RAMFILE_TMP);
	sprintf(command_buf, "busybox tar cvz -f %s -C %s -T %s/%s",
		tmpfilename, nvm_root_dir, "/marvell/tel", "ramdump_loglist");
	system(command_buf); /* create tgz containing mandatory files */
	ret = EehPushRamfile(fd, tmpfilename);
	unlink(tmpfilename);
	return ret;
}
#if 0
static int EehRamdumpPushSdlfiles(int fd)
{
	char tmpfilename[MAX_PATH];
	char command_buf[MAX_PATH+50];
	int ret = -1;
	FILE* lfd = 0; /* list file */
	char s[MAX_PATH];
	struct stat fst;
	const char* list_file = LISTFILE_TMP;
	char *nvm_root_dir = getenv("NVM_ROOT_DIR");

	if (nvm_root_dir == NULL)
		return ret;

	sprintf(tmpfilename, "%s/%s", nvm_root_dir, RAMFILE_TMP);
	/* Get a list of all SDL files in reverse order: starting from the most recent one */
	/* WORKAROUND: /marvell is hard-coded in file .../diag/diag_comm/src/diag_comm_EXTif_OSA_LINUX.c */
	sprintf(command_buf, "busybox ls -1t %s/marvell/*.sdl > %s", nvm_root_dir, list_file);
	system(command_buf);
	if ((lfd=fopen(LISTFILE_TMP, "rt"))==NULL) { 
		fprintf(stderr, "RAMDUMP failed to create %s\n", list_file);
		goto bail;
	}
	
	while(fgets(s, sizeof(s), lfd)) {
		/* Must strip end-of-line chars, otherwise filename won't be valid */
		char *s_end = strchr(s,'\n');
		if (s_end)
			*s_end = 0;
		/* List contains full path-names. Use tar -C path basename, so no path appears in tgz */
		if (stat(s, &fst)) {
			fprintf(stderr, "RAMDUMP cannot stat %s\n", s);
			continue; /* cannot stat this file - skip */
		}
		
		sprintf(command_buf, "busybox tar cvz -f %s -C %s %s",
			tmpfilename, dirname(s), basename(s));
		system(command_buf); /* create tgz containing this SDL file; tgz also saves the filename */
		if (EehPushRamfile(fd, tmpfilename))
			break; /* first failure (no memory) - stop the process */
	}
	ret = 0;
bail:
	if (lfd)
		fclose(lfd); /* in tmpfs, leave it there */
	unlink(tmpfilename);
	return ret;
}
#endif
static int EehRamdump(void)
{
	int fd;
	int ret;
	fd = open("/dev/ramfile", O_RDWR | O_SYNC);
	if (fd < 0) {
 		ERRMSG("%s: failed to open ramfile driver, error=%d\n", __FUNCTION__, errno);
	} else {
		/* Push mandatory files */
		EehRamdumpPushLogfiles(fd);
		
		/* Push DIAG log files */
		//disabled because in current code Diag log has been put to SDCARD, need not ramdump
//		EehRamdumpPushSdlfiles(fd);
		close(fd);
	 }

	/* Emulate panic that triggers ramdump */
	sync();
	ret = ioctl(sehdrv_fd, SEH_IOCTL_EMULATE_PANIC,  NULL);
	if (ret) {
		ERRMSG("%s: error=%d\n", __FUNCTION__, errno);
		return -1;
	}

	return 0;
}

/* Communicate the error info to SEH (for RAMDUMP and m.b. more) */
static void EehSaveErrorInfo(unsigned err, char *str, unsigned char *context)
{
	EehErrorInfo info;
	info.err = err;
	info.str = str;
	info.regs = context;
	ioctl(sehdrv_fd, SEH_IOCTL_SET_ERROR_INFO,  &info);
}

/* EehDisableCPUFreq and EehEnableCPUFreq must used in pair, first disable then enable */
EEH_STATUS EehDisableCPUFreq(void)
{
	EehApiParams ApiParams;
	INT32 ret;

	ApiParams.eehApiId                                              = _EehDisableCPUFreq;
	ApiParams.params                                                = NULL;

	DBGMSG("%s: _EehDisableCPUFreq\n", __FUNCTION__);

	ret = ioctl(sehdrv_fd, SEH_IOCTL_API,  &ApiParams);
	if (ret)
	{
		ERRMSG("%s: error=%d\n", __FUNCTION__, errno);
		return EEH_ERROR;
	}

	return ApiParams.status;

}

EEH_STATUS EehEnableCPUFreq(void)
{
	EehApiParams ApiParams;
	INT32 ret;

	ApiParams.eehApiId                                              = _EehEnableCPUFreq;
	ApiParams.params                                                = NULL;

	DBGMSG("%s: _EehEnableCPUFreq\n", __FUNCTION__);

	ret = ioctl(sehdrv_fd, SEH_IOCTL_API,  &ApiParams);
	if (ret)
	{
		ERRMSG("%s: error=%d\n", __FUNCTION__, errno);
		return EEH_ERROR;
	}

	return ApiParams.status;
}

