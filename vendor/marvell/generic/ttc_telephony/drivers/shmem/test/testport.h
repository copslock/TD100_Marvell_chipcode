#include "linux_types.h"
#include <sys/time.h>
#include "shm_share.h"


#define     TESTPORT_ASSERT(a) do { if (!(a)) { \
		printf("TEST PORT: ASSERT FAIL AT FILE %s FUNC %s LINE %d\r\n", __FILE__, __FUNCTION__, __LINE__); \
		while(1); }\
		} while (0)

#define TESTPORTDBG(fmt, args ...)		printf("TEST PORT: " fmt, ## args) 
//#define TESTPORTDBG(fmt, args ...)	NULL

#define TESTPORTERR(a)     printf("TEST PORT : ERROR AT FILE %s FUNC %s LINE %d as %s", __FILE__, __FUNCTION__, __LINE__, (char *)a)

#define CI_TESTPORT_STACK_SIZE					4096
#define CI_TESTPORT_TASK_PRIORITY				132 //same as ci stub task
#define MAX_TEST_PORT_MSG_LEN 					2048-16 //MAX-MHEADER_SIZE

#define MAX_TEST_PORT_PACKET_SIZE				(MAX_TEST_PORT_MSG_LEN-SHM_HEADER_SIZE-TEST_PACKET_HEADER_SIZE)
#define MIN_TEST_PORT_PACKET_SIZE				0
#define MAX_TEST_PORT_PACKET_COUNT				10000000
#define MIN_TEST_PORT_PACKET_COUNT				1
#define MAX_TEST_PORT_TEST_MODE					2
#define MIN_TEST_PORT_TEST_MODE					0

#define TEST_HANDSHAKE_REQ	 0x6d727631
#define TEST_HANDSHAKE_CONF  0x6d727632
#define TEST_DATA_REQ		0x6d727633

#define TEST_PACKET_HEADER_SIZE sizeof(TestPacketHead)

typedef enum _TestPortStatus
{
	TEST_PORT_NOT_STARTED,
	TEST_PORT_INIT_COMPLETE,
	TEST_PORT_INTESTING,
	TEST_PORT_TIMEOUT,
	TEST_PORT_HANDSHAKETIMEOUT,
	TEST_PORT_INVALID
} TestPortStatus;

typedef enum _TesMode
{
	TEST_MODE_THROUGHPUT_SINGLETRIP,
	TEST_MODE_THROUGHPUT_ROUNDTRIP,
	TEST_MODE_RELIABILITY,
	TEST_MODE_INVALID
} TestMode;

typedef enum _TPSysWrapReturnCode {
	TPSW_SUCCESS=0,
	TPSW_FAIL,
	TPSW_INVALID_PARAS,
	TPSW_OPEN_MSOCKET_ERROR,
	TPSW_CREAT_TASK_ERROR,
	TPSW_MAX
} TPSysWrapReturnCode;

typedef struct TestPacketHead_struct{
	int isloopback;
	int seq;
	int dataLength;
} TestPacketHead;

typedef struct TestConfigData_struct{
	int packetSize;
	int packetCount;
	int testMode;
} TestConfigData;

typedef struct TestResultData_struct{
	struct timeval sendStartTime;
	struct timeval sendEndTime;
	struct timeval recvStartTime;
	struct timeval recvEndTime;
	int recvPacketCount;
	int dropPacketCount;
} TestResultData;

/* function */
BOOL checkParameters(int packetSize, int packetCount, int testMode);
TPSysWrapReturnCode testportInit(int packetSize, int packetCount, int testMode); //entry

