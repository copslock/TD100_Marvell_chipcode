/*--------------------------------------------------------------------------------------------------------------------
(C) Copyright 2010 Marvell DSPC Ltd. All Rights Reserved.
-------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************
*                      M O D U L E     B O D Y                       							*
***********************************************************************
* Title: Test														*
*																*
* Filename: testport.c													*
*																*
* Target, platform: 													*
*																*
* Authors:															*
*																*
* Description: handles the interaction with other core over							*
*					shared memory. Specific implementation					*
*					for the internal-interface							*
* Notes:															*
*																*
*																*
*																*/
/************* General include definitions *************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include "testport.h"
#include "linux_types.h"
#include "msocket_api.h"
#include "osa.h"

/* data */
/*-------------------------------------------------------------------------*
* Internal Global Variables
*-------------------------------------------------------------------------*/
static UINT8 testportSendTaskStack[CI_TESTPORT_STACK_SIZE];
static OSTaskRef testportSendTaskRef   = NULL;
static UINT8 testportRecvTaskStack[CI_TESTPORT_STACK_SIZE];
static OSTaskRef testportRecvTaskRef   = NULL;
UINT8 TestPortRecvMsg[MAX_TEST_PORT_MSG_LEN];
int testportsockfd;

/*export from msocket.h, indicate entering the test mode and no other ports could send msg down*/
static BOOL inTestMode = FALSE;

static TestConfigData testConfigData;
static TestResultData testResultData;
static TestPortStatus gTestPortStatus = TEST_PORT_NOT_STARTED;

static const UINT32 TestPortHandShakeReq = TEST_HANDSHAKE_REQ;
static const UINT32 TestPortHandShakeConf = TEST_HANDSHAKE_CONF;
static const UINT32 TestPortDataReq = TEST_DATA_REQ;
static struct itimerval t;



/* functions */
void testportSendMsg(int procId, unsigned char *msg, int msglen);
void testportEventHandler (UINT8*  rxmsg);
static void testportSendTask(void *input);
static void testportRecvTask(void *input);
void generateReport();
void initTimer();
void timeout_alarm_handler();
int recv_startTimer();
int recv_stopTimer();


void initTimer()
{
	t.it_interval.tv_usec = 0;
	t.it_interval.tv_sec = 0;
	t.it_value.tv_usec = 0;
	t.it_value.tv_sec = 0;
	signal(SIGALRM, timeout_alarm_handler);
	return;
}

int recv_stopTimer()
{
	t.it_value.tv_sec = 0;
	if( setitimer( ITIMER_REAL, &t, NULL) < 0 ){
        TESTPORTDBG("settimer error!!\n");
        return -1;
    }
	return 0;
}

int recv_startTimer()
{
	t.it_value.tv_sec = 4;
	if( setitimer( ITIMER_REAL, &t, NULL) < 0 ){
        TESTPORTDBG("settimer error!!\n");
        return -1;
    }
	return 0;
}

void timeout_alarm_handler()
{
	
	/*
	  * recv task timeout, assume no packet from CP any more
	  */
	if(testConfigData.testMode == TEST_MODE_THROUGHPUT_SINGLETRIP || gTestPortStatus == TEST_PORT_HANDSHAKETIMEOUT)
		return;
	else if(testResultData.recvPacketCount == 0)
	{
		TESTPORTERR("NO loopback packet from CP!\n");
		return;
	}
	
	/*
	if (testportSendTaskRef)
		OSATaskDelete(testportSendTaskRef);
	*/
	if(gettimeofday(&(testResultData.recvEndTime), NULL) != 0)
	{
		TESTPORTERR("get recv end time error\n");
		inTestMode = FALSE;
		return;
	}

	/*report the test result of round-trip throughput test or reliability test*/
	if(inTestMode == TRUE)
		generateReport();
	inTestMode = FALSE;
	return;
}


void testportSendMsg(int procId, unsigned char *msg, int msglen)
{
	int ret;
	ShmApiMsg *pShmArgs;

	TESTPORT_ASSERT(msg != NULL);
	
	pShmArgs = (ShmApiMsg *)msg;
	pShmArgs->svcId = TestPortSvc_Id;
	pShmArgs->msglen = msglen;
	pShmArgs->procId = procId;

	ret = msend(testportsockfd, msg, msglen, 0);
	if(ret<0) testResultData.dropPacketCount++;
	return;
}

BOOL checkParameters(int packetSize, int packetCount, int testMode){
	
	if( packetSize < MIN_TEST_PORT_PACKET_SIZE || (UINT32)packetSize > MAX_TEST_PORT_PACKET_SIZE)	return FALSE;
	if( packetCount < MIN_TEST_PORT_PACKET_COUNT || packetCount > MAX_TEST_PORT_PACKET_COUNT) return FALSE;
	if( testMode < MIN_TEST_PORT_TEST_MODE || testMode > MAX_TEST_PORT_TEST_MODE) return FALSE;

	return TRUE;
}

TPSysWrapReturnCode testportInit(int packetSize, int packetCount, int testMode)
{
	TESTPORTDBG("init!\n");
	OS_STATUS status = OS_FAIL;

	if(!checkParameters(packetSize, packetCount, testMode))
	{
		TESTPORTERR("invalid parameters!\n");
		return TPSW_INVALID_PARAS;
	}

	testportsockfd = msocket(TEST_PORT);
	if(testportsockfd<=0)
	{
		TESTPORTERR("open msocket port error!\n");
		return TPSW_OPEN_MSOCKET_ERROR;
	}
	
	testConfigData.packetSize= packetSize; /* 0 to (2048-16) */
	testConfigData.packetCount= packetCount; /* 1 to 10000000 */
	testConfigData.testMode= testMode; /* 0 to 2 */

	gTestPortStatus = TEST_PORT_NOT_STARTED;

	/*
	  * indicate msocket to enter test Mode
	  * if using flight mode, then no change in msocket layer
	  */

	initTimer();
	
	OSAInitialize();

	if(testportSendTaskRef == NULL)
	{
		status = OSATaskCreate(&testportSendTaskRef,
				       &testportSendTaskStack[0],
				       CI_TESTPORT_STACK_SIZE,
				       CI_TESTPORT_TASK_PRIORITY,
				       (CHAR*)"testportSendTask",
				       (void*)testportSendTask,
				       NULL);

		if (status != OS_SUCCESS)
		{
			TESTPORTERR("testportSendTask create failed\n");
			return TPSW_OPEN_MSOCKET_ERROR;
		}
	}

	if(testportRecvTaskRef == NULL)
	{
		status = OSATaskCreate(&testportRecvTaskRef,
				       (void *)testportRecvTaskStack,
				       CI_TESTPORT_STACK_SIZE,
				       CI_TESTPORT_TASK_PRIORITY,
				       (CHAR*)"testportRecvTask",
				       testportRecvTask,
				       NULL);		
		if (status != OS_SUCCESS)
		{
			TESTPORTERR("testportRecvTask create failed\n");
			/* delete already started sending task */
			if(testportSendTaskRef) OSATaskDelete(testportSendTaskRef);
			return TPSW_OPEN_MSOCKET_ERROR;
		}
	}

	TESTPORTDBG("inited!\n");
	return TPSW_SUCCESS;
}


static void testportSendTask(void *input)
{
	UNUSEDPARAM(input)

	int ciTryTimes = 3;
	unsigned char *pShmBuf, *pdata;
	int shmBufLen;
	int _packetSize, _packetCount, _testMode;
	TestPacketHead * testPacketHead;
	int seq = 0;

	TESTPORTDBG("test port send task start!\n");

	_packetSize = testConfigData.packetSize;
	_packetCount = testConfigData.packetCount;
	_testMode = testConfigData.testMode;
	
	/* handshake with CP */
	while (ciTryTimes > 0)
	{
		if (gTestPortStatus == TEST_PORT_INIT_COMPLETE)
			break;
		msend(testportsockfd, (UINT8 *)&TestPortHandShakeReq, 4, 0);
		ciTryTimes--;
		OSATaskSleep(1000);	/* sleep 1000 ticks */
	}

	if(ciTryTimes == 0)
	{
		inTestMode = FALSE;
		gTestPortStatus = TEST_PORT_HANDSHAKETIMEOUT;
		if(testportRecvTaskRef) OSATaskDelete(testportRecvTaskRef);
		TESTPORTERR("handshake timeout!\n");
		return;
	}

	gTestPortStatus = TEST_PORT_INTESTING;

	testResultData.dropPacketCount = 0;

	TESTPORTDBG("Sending... \n");

	/* 
	  * start sending test data according to configurations
	  */

	if(gettimeofday(&(testResultData.sendStartTime), NULL) != 0)
	{
		inTestMode = FALSE;
		TESTPORTERR("get send start time error\n");
		return;
	}

	shmBufLen = SHM_HEADER_SIZE + TEST_PACKET_HEADER_SIZE + _packetSize;
	while(_packetCount>0)
	{

		pShmBuf = malloc(shmBufLen);
		memset(pShmBuf,'\0',shmBufLen);
		testPacketHead = (TestPacketHead *)(pShmBuf + SHM_HEADER_SIZE);
		pdata = (unsigned char *)(pShmBuf + SHM_HEADER_SIZE + TEST_PACKET_HEADER_SIZE);
	
		if(_testMode == TEST_MODE_THROUGHPUT_SINGLETRIP) testPacketHead->isloopback = 0;
		else testPacketHead->isloopback = 1; /* THROUGHPUT_ROUNDTRIP or RELIABILITY test */
	
		testPacketHead->dataLength = _packetSize;
		testPacketHead->seq = seq;
	
		while(_packetSize>0)
		{
			*pdata = 'A';
			pdata++;
			_packetSize--;
		}

		testportSendMsg(TestPortDataReq, pShmBuf, shmBufLen);
		seq++;
		_packetCount--;

		free(pShmBuf);
		_packetSize = testConfigData.packetSize;
	
	}

	if(gettimeofday(&(testResultData.sendEndTime), NULL) != 0)
	{
		inTestMode = FALSE;
		TESTPORTERR("get send end time error\n");
		return;
	}

	if(_testMode == TEST_MODE_THROUGHPUT_SINGLETRIP)
	{
		/*report test results of single-trip throughput test only*/
		generateReport();
		inTestMode = FALSE;
		if(testportRecvTaskRef) OSATaskDelete(testportRecvTaskRef);
	}
	else
	{
		/* start timer to wait receive all the CP response */
		recv_startTimer();
	}

	TESTPORTDBG("testportSendTask end!\n");

	return;	
}

static void testportRecvTask(void *input)
{
	UNUSEDPARAM(input)

	int ret;

	TESTPORTDBG("test port receive task start!\n");
	
	while (inTestMode)
	{
		/* wating loopback msg from CP */
		ret = mrecv(testportsockfd, TestPortRecvMsg, MAX_TEST_PORT_MSG_LEN, 0);

		if (ret == 0)
			continue;
		else if (ret < 0)
			break;

		testportEventHandler(TestPortRecvMsg);
		memset(TestPortRecvMsg,'\0',MAX_TEST_PORT_MSG_LEN);
	} /* end while */

	TESTPORTDBG("testportRecvTask end!\n");
	return;
}

void testportEventHandler (UINT8*  rxmsg)
{
	/* handshanke conf msg from CP	*/
	if(*((UINT32*)rxmsg) == TestPortHandShakeConf)
	{
		if(gTestPortStatus == TEST_PORT_NOT_STARTED)
			gTestPortStatus = TEST_PORT_INIT_COMPLETE;
		if( testConfigData.testMode != TEST_MODE_THROUGHPUT_SINGLETRIP)
		{
			testResultData.recvPacketCount = 0;
			if(gettimeofday(&(testResultData.recvStartTime), NULL) != 0)
			{
				if (testportSendTaskRef)
					OSATaskDelete(testportSendTaskRef);
				inTestMode = FALSE;
				TESTPORTERR("get recv start time error\n");
				return;
			}
		}
		return;		
	}

	/* data msg */
	ShmApiMsg *shmmsg = (ShmApiMsg *)rxmsg;

	TESTPORT_ASSERT(shmmsg->svcId == TestPortSvc_Id);

	/* 
	  *  TEST_PORT_NOT_STARTED (on recving msg TEST_HANDSHAKE_CONF)
	  *  ==> TEST_PORT_INIT_COMPLETE (on testportSendTask start send packets)
	  *  ==> TEST_PORT_INTESTING (on finish the test)
	  *  ==> TEST_PORT_NOT_STARTED
	  *  
	  */

	switch(shmmsg->procId)
	{
		case TEST_DATA_REQ:
			if(gTestPortStatus == TEST_PORT_INTESTING)
				testResultData.recvPacketCount++;
			break;
		case MsocketLinkdownProcId:
			TESTPORTERR("Receive MsocketLinkdownProcId!");
			if (testportSendTaskRef)
				OSATaskDelete(testportSendTaskRef);
			inTestMode = FALSE;
			break;
		case MsocketLinkupProcId:
			TESTPORTERR("Receive MsocketLinkupProcId!");
			break;
		default:
			TESTPORTERR("Receive unexpected message!\n");
			break;
	}
	
}

void generateReport()
{
	int _packetSize, _packetCount, _testMode;
	struct timeval sendTimeDiff, recvTimeDiff;
	float sendSec, recvSec;
	float throughput_send,throughput_recv, reliability, droprate;

	_packetSize = testConfigData.packetSize;
	_packetCount = testConfigData.packetCount;
	_testMode = testConfigData.testMode;

	sendTimeDiff.tv_sec = testResultData.sendEndTime.tv_sec - testResultData.sendStartTime.tv_sec;
	if(testResultData.sendEndTime.tv_usec >=  testResultData.sendStartTime.tv_usec)
	   sendTimeDiff.tv_usec = testResultData.sendEndTime.tv_usec - testResultData.sendStartTime.tv_usec;
	else
	{	
		sendTimeDiff.tv_usec = testResultData.sendEndTime.tv_usec + 1000000 - testResultData.sendStartTime.tv_usec;
		sendTimeDiff.tv_sec -= 1;
	}
	sendSec = sendTimeDiff.tv_sec+(float)sendTimeDiff.tv_usec/1000000;
	droprate = (float)testResultData.dropPacketCount/testConfigData.packetCount;
	printf("SendStart time at %d sec %d usec; SendEnd time at %d sec %d usec\n", 
			(int)testResultData.sendStartTime.tv_sec, (int)testResultData.sendStartTime.tv_usec,
			(int)testResultData.sendEndTime.tv_sec, (int)testResultData.sendEndTime.tv_usec);

	switch (_testMode)
	{
		case TEST_MODE_THROUGHPUT_SINGLETRIP:
			throughput_send = ((_packetCount-testResultData.dropPacketCount)/sendSec)*(_packetSize+SHM_HEADER_SIZE+TEST_PACKET_HEADER_SIZE)*8;

			printf("\r\n\r\n[TEST PORT test report]\r\n");
 			printf("[single-trip throughput test]\r\n");
 			printf("* send %d packets(size = %d bytes) in %f seconds\r\n", _packetCount, _packetSize+SHM_HEADER_SIZE+TEST_PACKET_HEADER_SIZE, sendSec);
			printf("* drop %d packets, drop rate: %f%%\r\n",testResultData.dropPacketCount,droprate*100);
 			printf("* throughput: %fMbps\r\n",throughput_send/(1024*1024));
			printf("\r\n");
			break;
		case TEST_MODE_THROUGHPUT_ROUNDTRIP:
			throughput_send = ((_packetCount-testResultData.dropPacketCount)/sendSec)*(_packetSize+SHM_HEADER_SIZE+TEST_PACKET_HEADER_SIZE)*8;
			recvTimeDiff.tv_sec = testResultData.recvEndTime.tv_sec - testResultData.recvStartTime.tv_sec;
			if(testResultData.recvEndTime.tv_usec >= testResultData.recvStartTime.tv_usec)
				recvTimeDiff.tv_usec = testResultData.recvEndTime.tv_usec - testResultData.recvStartTime.tv_usec;
			else
			{
				recvTimeDiff.tv_usec = testResultData.recvEndTime.tv_usec + 1000000 - testResultData.recvStartTime.tv_usec;
				recvTimeDiff.tv_sec -= 1;
			}
//			recvSec = recvTimeDiff.tv_sec+(float)recvTimeDiff.tv_usec/1000000;
			recvSec = sendSec; // We think it is reasonable
			throughput_recv = (testResultData.recvPacketCount*(_packetSize+SHM_HEADER_SIZE+TEST_PACKET_HEADER_SIZE)*8)/recvSec; /* -4, remove the timeout waiting period*/
			
//			printf("RecvStart time at %d sec %d usec; RecvEnd time at %d sec %d usec\n", 
//				testResultData.recvStartTime.tv_sec, testResultData.recvStartTime.tv_usec,
//				testResultData.recvEndTime.tv_sec, testResultData.recvEndTime.tv_usec);

			printf("\r\n\r\n[TEST PORT test report]\r\n");
			printf("[round-trip throughput test]\r\n");
			printf("* send %d packets(size = %d bytes) in %f seconds\r\n", _packetCount, _packetSize+SHM_HEADER_SIZE+TEST_PACKET_HEADER_SIZE, sendSec);
			printf("* drop %d packets, drop rate: %f%%\r\n",testResultData.dropPacketCount,droprate*100);
			printf("* recv %d packets in %f seconds\r\n", testResultData.recvPacketCount, recvSec);
			printf("* send throughput: %fMbps\r\n",throughput_send/(1024*1024));
			printf("* recv throughput: %fMbps\r\n",throughput_recv/(1024*1024));
			printf("\r\n");
			break;
		case TEST_MODE_RELIABILITY:
			reliability= (float)testResultData.recvPacketCount/(_packetCount-testResultData.dropPacketCount); /* keep out the dropped packet*/
			
			printf("\r\n\r\n[TEST PORT test report]\r\n");
			printf("[reliability test]\r\n");
			printf("* send %d packets\r\n", _packetCount);
			printf("* drop %d packets, drop rate: %f%%\r\n",testResultData.dropPacketCount,droprate*100);
			printf("* recv %d packets\r\n", testResultData.recvPacketCount);
			printf("* reliability: %f%% \r\n", reliability*100);
			break;
		default:
			break;
	}
	
	return;
}

void usage(){	
	printf("Usage: msocket_testport -S packet payload size -C packet count -M test mode\n");
	printf("       packet payload size  : (%d-%d)\n",MIN_TEST_PORT_PACKET_SIZE,MAX_TEST_PORT_PACKET_SIZE);
	printf("       packet count : (%d-%d)\n",MIN_TEST_PORT_PACKET_COUNT,MAX_TEST_PORT_PACKET_COUNT);
	printf("       test mode    : (%d-%d)\n",MIN_TEST_PORT_TEST_MODE,MAX_TEST_PORT_TEST_MODE);
}

int main(int argc, char *argv[]){
	int i;
	char * cp, opt;
	int packetSize, packetCount, testMode;
	int ret = 0;
	packetSize = packetCount = testMode = -1;
	if (argc != 7)
	{
		usage();
		return -1;
	}

	for (i = 0; i < argc;i++)
	{
		cp = argv[i];
		if (*cp == '-' )
		{
			opt = *(++cp);

			switch (opt)
			{
			case 's':
			case 'S':
				i++;
				packetSize = atoi(argv[i]);
				break;
			case 'c':
			case 'C':
				i++;
				packetCount = atoi(argv[i]);
				break;
			case 'm':
			case 'M':
				i++;
				testMode = atoi(argv[i]);
				break;
			default:
				printf("parameter error!\n");
				usage();
				return -1;
			}
		}
	}

	if(!checkParameters(packetSize, packetCount, testMode))
	{
		usage(); 
		return -1;
	}

	printf("Start test procdure!\n");
	inTestMode = TRUE;
	testportInit(packetSize, packetCount, testMode);
	while(inTestMode){OSATaskSleep(1000*5);}
	if(ret != TPSW_OPEN_MSOCKET_ERROR && ret != TPSW_INVALID_PARAS)
		mclose(testportsockfd);
	printf("End test procdure!\n");
	return 0;

}
