#include "linux_types.h"

// parameters used for audio stub stask init
#define AUDIO_STUB_STACK_SIZE                   (1024 * 4)  //1024*2 HEZI - Increase stack size
#define MAX_AUDIO_STUB_RXMSG_LEN                2048
#ifdef OSA_NO_PRIORITY_CONVERSION
#define AUDIO_TASK_PRIORITY             71
#else
#define AUDIO_TASK_PRIORITY             98
#endif
#define AUDIO_MESSAGE_Q_SIZE    sizeof(ShmApiMsg) //it need to discuss, whether the size is enough?
#define AUDIO_MESSAGE_Q_MAX             50
// only one audio callback type
#define AUDIO_CB_TYPE                   0
// trace MARCO defines
#define AUDIO_STUB_TRACE(module, level, fmt)
#define AUDIO_STUB_TRACE1(module, level, fmt, val1)
#define AUDIO_STUB_TRACE2(module, level, fmt, val1, val2)

#define UNUSEDPARAM(param) (void)param;

typedef enum
{
	AUDIO_STUB_INIT_NOT_STARTED,
	AUDIO_STUB_INIT_PENDING_CNF,
	AUDIO_STUB_INIT_COMPLETE,

}AudioStubInitStatus;

typedef enum AUDIO_STUB_STATUS {
	AUDIO_STUB_INIT_SUCCESS = 0,
	AUDIO_STUB_INIT_FAIL,
	AUDIO_STUB_INTERLINK_DOWN, //jiangang:to be discussion????
	AUDIO_STUB_INTERLINK_UP,

	AUDIO_STUB_NUM_STATUS
} AudioStubStatus;

typedef struct _AudioStubClientSendReq {
	UINT8 prodId;   // procedure ID
	void    *pData; // pointer to a memory that contains
	// data to be transmitted
	UINT16 size;    // size of the data pointed to by pData
} AudioStubClientSendReq;

typedef struct _AudioStubServerSendReq {
	UINT32 cbId;    // callback ID
	void  *pData;   // pointer to a memory that contains
	// data to be transmitted
	UINT16 size;    // size of the data pointed to by pDataudioStubServerSendReq
} AudioStubServerSendReq;

typedef struct {
	int procId;
	int msglen;
	void *pData;
} StubMsg;

//typedef UINT8 AudioStubStatus;
/* status indication call-back that is going to be called by the stub after initialization */
typedef void (*AudioStubClientStatusInd)(AudioStubStatus status);

void audioStubTask(void *input);
void audioClientStubTask(void *input);
int audioSendMsgToServer(int procId, unsigned char *msg, int msglen);
UINT8 audioClientStubInit(int pStatusIndCb);
