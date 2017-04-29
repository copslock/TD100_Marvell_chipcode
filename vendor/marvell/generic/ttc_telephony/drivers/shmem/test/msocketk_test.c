#include "typedef.h"
#include "osa.h"
#include "msocket.h"
#include "shm.h"
#include "msocketk_test.h"

#ifdef _TAVOR_HARBELL_
	#define MSOCKET_TESTCP
#else
	#ifndef TEST_USER
		#define MSOCKET_TESTAP
	#endif
#endif

#ifdef TEST_USER
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
//#include <asm-arm/arch-pxa/pxa930_acipc_test.h>
#include <pxa930_acipc_test.h>
#endif

#ifdef MSOCKET_TESTCP
 #include "osa.h"
 #include "stdio.h"
 #include "string.h"
 #include "osa_types.h"
 #include "acipc.h"
#endif

#ifdef MSOCKET_TESTAP
#include <linux/kthread.h>
//#include <asm-arm/arch-pxa/pxa930_acipc.h>
#include <pxa930_acipc.h>
extern acipc_return_code ACIPCDataSend(acipc_events user_event, acipc_data data);
#endif

#define ACIPC_MSOCKET_TEST ACIPC_SPARE_1

#define TEST_MTU (PACKETLEN - MHEADER_SIZE - 1)
//#define TEST_MTU 100

#ifdef TEST_USER
	#undef TESTPRINTF
	#define TESTPRINTF printf

	#define ACIPC_DEV "/dev/acipc"
#endif

//typedef void* OSTaskRef;
OSTaskRef SendTaskRef[MAX_TEST_THREAD];
static UINT8 SendTaskStack[MAX_TEST_THREAD][2048];
OSTaskRef RecvTaskRef[MAX_TEST_THREAD];
static UINT8 RecvTaskStack[MAX_TEST_THREAD][2048];
int t_send_result[MAX_TEST_THREAD];
int t_recv_result[MAX_TEST_THREAD];
struct _test_param t_param;
int gCPTest_Start = 0;

int prepare_buffer(char **buffer, int len, int Port)
{
	int i;
	int data;

#ifdef MSOCKET_TESTCP
	(*buffer) = malloc(len);
#else
	#ifdef TEST_USER
	(*buffer) = malloc(len);
	#else
	(*buffer) = kmalloc(len, GFP_KERNEL);
	#endif
#endif

	if ((*buffer) == NULL)
	{
		TESTPRINTF("buffer alloc failed\r\n");
		return -1;
	}
	for (i = 0; i < len; i++)
	{
		*((*buffer) + i) = ((len + i + Port) % 256); //exceed what a byte can hold
	} //each lenth has different content, in the same packet, every byte is different

	return 0;
}

int send_thread(void *data)
{
	int sock = 0, Port;
	char* send_buffer = NULL;
	int i, j, ret = 0, flag = 0, cycle;
	struct _thread_param* p_send_param;

	do {
		p_send_param = (struct _thread_param *)data;
		Port = p_send_param->port;
		cycle = p_send_param->cycle;

		TESTPRINTF("enter send thread %d\r\n", Port);

		sock = msocket(Port);

		if ( sock < 0 )
		{
			TESTPRINTF("socket error\r\n");
			ret = -1;
			break;
		}

		for (j = 0; j < cycle; j++)
		{
			TESTPRINTF("********Start Cycle %d*********\r\n", j);
			for ( i = 1; i <= TEST_MTU; i++)
			{

				ret = prepare_buffer(&send_buffer, i, Port);
				if (ret < 0)
				{
					TESTPRINTF("prepare buffer error\r\n");
					ret = -1;
					break;
				}

				/*TESTPRINTF("packet %d buffer content: ", i);
				   for (j = 0; j < i; j++){
					TESTPRINTF("%d ", *(send_buffer + j));
				   }
				   TESTPRINTF("**********packet %d finished*****\r\n", i);*/
				ret = msend( sock, send_buffer, i, 0 );
				if (ret <= 0)
				{
					TESTPRINTF("Port %d send packet %d error\r\n", Port, i);
					flag = 1;
					continue;
				}
				else
				{
					TESTPRINTF("Port %d send packet %d passed\r\n", Port, i);
				}

				if (send_buffer)
				{
			#ifdef MSOCKET_TESTCP
					free(send_buffer);
			#else
				#ifdef TEST_USER
					free(send_buffer);
				#else
					kfree(send_buffer);
				#endif
			#endif

					send_buffer = NULL;
				}

				if (p_send_param->send_speed == 0)
				{
					//send in slow speed
					OSATaskSleep(20);
				}
			} //end of for i
		} //end of for j
	} while (0);
	if (send_buffer)
	{
		#ifdef MSOCKET_TESTCP
		free(send_buffer);
		#else
			#ifdef TEST_USER
		free(send_buffer);
			#else
		kfree(send_buffer);
			#endif
		#endif
		send_buffer = NULL;
	}
	if (sock)
	{
		mclose( sock );
	}

	if (ret < 0 || flag == 1)
	{
		t_send_result[Port] = 1; //fail
	}
	else
	{
		t_send_result[Port] = 2; //succeed
	}
	return ret;
}



int check_buffer(char *buffer, int len, int Port)
{
	int i, ret = 0;

	for (i = 0; i < len; i++)
	{
		if (*(buffer + i) != ((len + i + Port) % 256))
		{
			TESTPRINTF("Port %d the %d byte in packet %d Error:%d instead of %d\r\n", Port, i, len, *(buffer + i), (len + i + Port) % 256);
			ret = -1;
			break;
		}
	}
	return ret;
}



int recv_thread(void *data)
{
	int sock = 0, Port;
	int i, j, ret = 0, flag = 0, cycle;
	char *recv_buffer = NULL;
	struct _thread_param* p_recv_param;

	do {


		p_recv_param = (struct _thread_param *)data;
		Port = p_recv_param->port;
		cycle = p_recv_param->cycle;

		TESTPRINTF("enter recv thread %d\r\n", Port);

#ifdef MSOCKET_TESTCP
		recv_buffer = malloc(TEST_MTU);
#else
	#ifdef TEST_USER
		recv_buffer = malloc(TEST_MTU);
	#else
		recv_buffer = kmalloc(TEST_MTU, GFP_KERNEL);
	#endif
#endif
		if (recv_buffer == NULL)
		{
			TESTPRINTF("recv buffer malloc failed\r\n");
			ret = -1;
			break;
		}

		sock = msocket( Port );

		if ( sock < 0 )
		{
			TESTPRINTF("socket error");
			ret = -1;
			break;
		}
		for (j = 0; j < cycle; j++)
		{
			TESTPRINTF("********Start Cycle %d*********\r\n", j);
			for ( i = 1; i <= TEST_MTU; i++)
			{
				memset(recv_buffer, 0, TEST_MTU);
				//TESTPRINTF("Port %d recv %d\n", Port, i);
				ret = mrecv(sock, recv_buffer, i, 0); //len should be larger than arrived data length
				if (ret <= 0)
				{
					TESTPRINTF("Port %d receive error for packet %d status %d\r\n", Port, i, ret);
					flag = 1;
					continue;
				}
/*		TESTPRINTF("Received packet %d buffer content: ", i);
		for (j = 0; j < i; j++){
			TESTPRINTF("%d ", *(recv_buffer + j));
		}
		TESTPRINTF("**********packet %d finished*****\r\n", i);
 */
				ret = check_buffer(recv_buffer, i, Port);
				if (ret == 0 )
				{
					TESTPRINTF("Port %d Received Packet %d passed\r\n", Port, i);
				}
				else
				{
					flag = 1; //fail
					TESTPRINTF("Port %d Received Packet %d failed\r\n", Port, i);
				}

				if (p_recv_param->recv_speed == 0)
				{
					//recv in slow speed
					OSATaskSleep(20);
				}
			} //end of for i
		} //end of for j

	} while (0);

	if (recv_buffer)
	{
		#ifdef MSOCKET_TESTCP
		free(recv_buffer);
		#else
			#ifdef TEST_USER
		free(recv_buffer);
			#else
		kfree(recv_buffer);
			#endif
		#endif

		recv_buffer = NULL;
	}
	if (sock)
	{
		mclose( sock );
	}

	if (ret < 0 || flag == 1)
	{
		t_recv_result[Port] = 1; //fail
	}
	else
	{
		t_recv_result[Port] = 2; //succeed
	}
	return ret;
}

int start_recv(void *data)
{
	int sock = 0, Port;
	int i, j, ret = 0, flag = 0, cycle;
	char *recv_buffer = NULL;
	struct _thread_param * p_recv_param;

	do {
		p_recv_param = (struct _thread_param *)data;
		Port = p_recv_param->port;
		sock = p_recv_param->sock;
		cycle = p_recv_param->cycle;

#ifdef MSOCKET_TESTCP
		recv_buffer = malloc(TEST_MTU);
#else
	#ifdef TEST_USER
		recv_buffer = malloc(TEST_MTU);
	#else
		recv_buffer = kmalloc(TEST_MTU, GFP_KERNEL);
	#endif
#endif
		if (recv_buffer == NULL)
		{
			TESTPRINTF("recv buffer malloc failed\r\n");
			ret = -1;
			break;
		}
		for (j = 0; j < cycle; j++)
		{
			TESTPRINTF("********Start Cycle %d*********\r\n", j);
			for ( i = 1; i <= TEST_MTU; i++)
			{
				memset(recv_buffer, 0, TEST_MTU);
				//TESTPRINTF("Port %d recv %d\n", Port, i);
				ret = mrecv(sock, recv_buffer, i, 0); //len should be larger than arrived data length
				if (ret <= 0)
				{
					TESTPRINTF("Port %d receive error for packet %d status %d\r\n", Port, i, ret);
					flag = 1;
					continue;
				}
/*		TESTPRINTF("Received packet %d buffer content: ", i);
		for (j = 0; j < i; j++){
			TESTPRINTF("%d ", *(recv_buffer + j));
		}
		TESTPRINTF("**********packet %d finished*****\r\n", i);
 */
				ret = check_buffer(recv_buffer, i, Port);
				if (ret == 0 )
				{
					TESTPRINTF("Port %d Received Packet %d passed\r\n", Port, i);
				}
				else
				{
					flag = 1; //fail
					TESTPRINTF("Port %d Received Packet %d failed\r\n", Port, i);
				}

				if (p_recv_param->recv_speed == 0)
				{
					//recv in slow speed
					OSATaskSleep(20);
				}
			} //end of for i
		} //end of for j

	} while (0);

	if (recv_buffer)
	{
		#ifdef MSOCKET_TESTCP
		free(recv_buffer);
		#else
			#ifdef TEST_USER
		free(recv_buffer);
			#else
		kfree(recv_buffer);
			#endif
		#endif

		recv_buffer = NULL;
	}

	if (ret < 0 || flag == 1)
	{
		t_recv_result[Port] = 1; //fail
	}
	else
	{
		t_recv_result[Port] = 2; //succeed
	}
	return ret;


}


int create_task(OSTaskRef *taskRef, void* stackPtr,
		UINT32 stackSize, UINT8 priority, CHAR *taskName,
		void (*taskStart)(void*), void *argv)
{
#ifdef TEST_USER
/*	pid_t pid;

	if((pid=fork()) < 0){
		perror("fork error");
		return -1;
	}else if(pid == 0){

		(*taskStart)(argv);
	}
 */
	int rc;
	printf("use pthread_create\n");
	rc = pthread_create((pthread_t *)taskRef, NULL, taskStart, argv);
	if (rc)
	{
		perror("create thread fail");
		return -1;
	}

	return 0;
#endif

#ifdef MSOCKET_TESTCP
#if 0
	NU_RTN_STATUS status;
	NU_TASK *task = (NU_TASK *)taskRef;

	TESTPRINTF("entered create_task %x\r\n", taskStart);
	/* Create the Nucleus task */
	status = NU_Create_Task(task, taskName,
				(void (*)(UNSIGNED, void*))taskStart,
				(UINT32)argv, argv,     /* argc, argv */
				stackPtr, stackSize,
				priority, 0,            /* NO TIMESLICING */
				NU_PREEMPT,
				NU_START);

	TESTPRINTF("status is %d\r\n", status);
	if (status != NU_SUCCESS)
	{
		return status;
	}
#endif

	int status;
	TESTPRINTF("osataskcreate\r\n");
	status = OSATaskCreate(taskRef, stackPtr, stackSize, priority, taskName, taskStart, argv);
	return 0;
#endif

#ifdef MSOCKET_TESTAP

//	int status;
//	TESTPRINTF("osataskcreate\r\n");
//	status = OSATaskCreate(taskRef, stackPtr, stackSize, priority, taskName, taskStart, argv);

	kthread_run(taskStart, argv, (const char *)taskName);
#endif


}

int wait_task(void *th, int th_num, int type)
{
#ifdef TEST_USER
/*	int child_status;
	waitpid(0, &child_status, 0);

	if (WIFEXITED(child_status)){
		printf("+++child process terminate normally\n");
	}else {
		printf("+++child process terminate abnormally\n");
	}
 */
	//	pthread_exit(NULL);

	int ret;
	void *thread_return;


	ret = pthread_join(*((pthread_t *)th), &thread_return);
	if (ret == 0)
	{
		TESTPRINTF("pthread_join successfully\r\n");
	}
	else
	{
		TESTPRINTF("pthread_join failed with ret %d\r\n", ret);
	}
#endif

#ifdef MSOCKET_TESTCP
//could not find any function on Nucleus that can do wait and reap
	OSA_STATUS status;

	if (type == 2)
	{
		TESTPRINTF("waittask recv: t_result %d %d\n", th_num, t_recv_result[th_num]);
		while (t_recv_result[th_num] == 0)
		{
			OSATaskSleep(50);
		}
	}
	else if (type == 1)
	{
		TESTPRINTF("waittask send: t_result %d %d\n", th_num, t_send_result[th_num]);
		while (t_send_result[th_num] == 0)
		{
			OSATaskSleep(50);
		}
	}
	else if (type == 0)
	{
		TESTPRINTF("waittask send_recv: t_result %d %d %d\n", th_num, t_send_result[th_num], t_recv_result[th_num]);
		while (t_send_result[th_num] == 0 || t_recv_result[th_num] == 0)
		{
			OSATaskSleep(50);
		}
	}


	OSATaskSleep(50);
	status = OSATaskDelete(*((OSTaskRef *)th));
	if (status == OS_SUCCESS)
	{
		TESTPRINTF("TaskDelete successfully\r\n");
	}
	else
	{
		TESTPRINTF("TaskDelete failed with status %d\r\n", status);
	}
#endif
}


int send_recv_thread(void *data)
{
	int sock = 0, Port;
	char* send_buffer = NULL;
	int i, j, ret = 0, flag = 0, cycle;
	struct _thread_param* p_send_param;
	int status;

	do {
		p_send_param = (struct _thread_param *)data;
		Port = p_send_param->port;
		cycle =  p_send_param->cycle;

		TESTPRINTF("enter send_recv thread %d\r\n", Port);

		sock = msocket(Port);

		if ( sock < 0 )
		{
			TESTPRINTF("socket error\r\n");
			ret = -1;
			break;
		}

		p_send_param->sock = sock;

		TESTPRINTF("call start_recv\r\n");

		status = create_task(&RecvTaskRef[Port], (void *)RecvTaskStack[Port], 2048, 100, "MRECV Task", start_recv, (void *)p_send_param);

	#ifndef MSOCKET_TESTCP
		OSATaskSleep(1000);
	#endif

		for (j = 0; j < cycle; j++)
		{
			TESTPRINTF("********Start Cycle %d*********\r\n", j);
			for ( i = 1; i <= TEST_MTU; i++)
			{

				ret = prepare_buffer(&send_buffer, i, Port);
				if (ret < 0)
				{
					TESTPRINTF("prepare buffer error\r\n");
					ret = -1;
					break;
				}

				/*TESTPRINTF("packet %d buffer content: ", i);
				   for (j = 0; j < i; j++){
					TESTPRINTF("%d ", *(send_buffer + j));
				   }
				   TESTPRINTF("**********packet %d finished*****\r\n", i);*/
				ret = msend( sock, send_buffer, i, 0 );
				if (ret <= 0)
				{
					TESTPRINTF("Port %d send packet %d error\r\n", Port, i);
					flag = 1;
					continue;
				}
				else
				{
					TESTPRINTF("Port %d send packet %d passed\r\n", Port, i);
				}

				if (send_buffer)
				{
			#ifdef MSOCKET_TESTCP
					free(send_buffer);
			#else
				#ifdef TEST_USER
					free(send_buffer);
				#else
					kfree(send_buffer);
				#endif
			#endif

					send_buffer = NULL;
				}

				if (p_send_param->send_speed == 0)
				{
					//send in slow speed
					OSATaskSleep(20);
				}
			} //end of for i
		} //end of for j
	} while (0);
	if (send_buffer)
	{
		#ifdef MSOCKET_TESTCP
		free(send_buffer);
		#else
			#ifdef TEST_USER
		free(send_buffer);
			#else
		kfree(send_buffer);
			#endif
		#endif
		send_buffer = NULL;
	}

	if (ret < 0 || flag == 1)
	{
		t_send_result[Port] = 1; //fail
	}
	else
	{
		t_send_result[Port] = 2; //succeed
	}

	TESTPRINTF("wait for recv to end\r\n");
	wait_task(&RecvTaskRef[Port], Port, 2); //recv

	if (sock)
	{
		mclose( sock );
	}

	return ret;
}




int test_send_recv(int type)
{
	int i = 0, j = 0;
	int status;
	int th_num = t_param.th_num;

	TESTPRINTF("th_num %d\r\n", th_num);
	if (type == 1 )         //send

	{
		TESTPRINTF("sleep for 2000 ticks\r\n");
		OSATaskSleep(2000);

		for (i = 1; i <= th_num; i++)
		{
			TESTPRINTF("create send thread for %d %x\r\n", i, send_thread);
			t_param.thread_param[i - 1].port = i;

			status = create_task(&SendTaskRef[i], (void *)SendTaskStack[i], 2048, 100, "MSEND Task", send_thread, (void *)&(t_param.thread_param[i - 1]));

		}

		for (j = 1; j <= th_num; j++)
		{
			wait_task(&SendTaskRef[j], j, type);
		}

		t_send_result[0] = 1;   //send result is used
	}
	else if (type == 2)             //recv

	{
		for (i = 1; i <= th_num; i++)
		{
			TESTPRINTF("create recv thread for %d\n", i);
			t_param.thread_param[i - 1].port = i;

			status = create_task(&RecvTaskRef[i], (void *)RecvTaskStack[i], 2048, 100, "MRECV Task", recv_thread, (void *)&(t_param.thread_param[i - 1]));

		}
		for (j = 1; j <= th_num; j++)
		{
			wait_task(&RecvTaskRef[j], j, type);
		}


		t_recv_result[0] = 1; //recv result is used
	}
	else if (type == 0)
	{
		for (i = 1; i <= th_num; i++)
		{
			TESTPRINTF("create thread for %d\n", i);
			t_param.thread_param[i - 1].port = i;

			status = create_task(&SendTaskRef[i], (void *)SendTaskStack[i], 2048, 100, "MSEND Task", send_recv_thread, (void *)&(t_param.thread_param[i - 1]));


		}

		for (j = 1; j <= th_num; j++)
		{
			wait_task(&SendTaskRef[j], j, type);
		}

		t_recv_result[0] = 1;   //recv result is used
		t_send_result[0] = 1;   //send result is used
		TESTPRINTF("t_recv_result[0] %d, t_send_result[0] %d\r\n", t_recv_result[0], t_send_result[0]);
	}
	else
	{
		TESTPRINTF("Invalid type\n");
		return -1;
	}

	return 0;
}

void msocket_test_init(int case_num)
{
	TESTPRINTF("*******Start Test %d********\r\n", case_num);
	memset(t_send_result, 0, MAX_TEST_THREAD * sizeof(int));
	memset(t_recv_result, 0, MAX_TEST_THREAD * sizeof(int));
}

int msocket_test (int case_num)
{
	int i;
	int ret = 0;

	switch (case_num)
	{
	//cp(full)->ap(full)
	case 1:
		msocket_test_init(case_num);

		t_param.th_num = 1;

		for (i = 0; i < t_param.th_num; i++)
		{
			t_param.thread_param[i].recv_speed = 1;
			t_param.thread_param[i].send_speed = 1;
			t_param.thread_param[i].cycle = 1;
		}
			#ifdef MSOCKET_TESTCP
		test_send_recv(1);
			#else
		test_send_recv(2);
			#endif
		break;
	//cp(full)->ap(slow)
	case 2:
		msocket_test_init(case_num);

		t_param.th_num = 1;

		for (i = 0; i < t_param.th_num; i++)
		{
			t_param.thread_param[i].recv_speed = 0;
			t_param.thread_param[i].send_speed = 1;
			t_param.thread_param[i].cycle = 1;
		}
			#ifdef MSOCKET_TESTCP
		test_send_recv(1);
			#else
		test_send_recv(2);
			#endif
		break;
	//cp(slow)->ap(full)
	case 3:
		msocket_test_init(case_num);

		t_param.th_num = 1;

		for (i = 0; i < t_param.th_num; i++)
		{
			t_param.thread_param[i].recv_speed = 1;
			t_param.thread_param[i].send_speed = 0;
			t_param.thread_param[i].cycle = 1;
		}
			#ifdef MSOCKET_TESTCP
		test_send_recv(1);
			#else
		test_send_recv(2);
			#endif
		break;

	case 4:
		msocket_test_init(case_num);

		t_param.th_num = 1;

		for (i = 0; i < t_param.th_num; i++)
		{
			t_param.thread_param[i].recv_speed = 1;
			t_param.thread_param[i].send_speed = 1;
			t_param.thread_param[i].cycle = 1;
		}
			#ifdef MSOCKET_TESTCP
		test_send_recv(2);
			#else
		test_send_recv(1);
			#endif
		break;
	case 5:
		msocket_test_init(case_num);

		t_param.th_num = 1;

		for (i = 0; i < t_param.th_num; i++)
		{
			t_param.thread_param[i].recv_speed = 0;
			t_param.thread_param[i].send_speed = 1;
			t_param.thread_param[i].cycle = 1;
		}
			#ifdef MSOCKET_TESTCP
		test_send_recv(2);
			#else
		test_send_recv(1);
			#endif
		break;
	case 6:
		msocket_test_init(case_num);

		t_param.th_num = 1;

		for (i = 0; i < t_param.th_num; i++)
		{
			t_param.thread_param[i].recv_speed = 1;
			t_param.thread_param[i].send_speed = 0;
			t_param.thread_param[i].cycle = 1;
		}
			#ifdef MSOCKET_TESTCP
		test_send_recv(2);
			#else
		test_send_recv(1);
			#endif
		break;
	//cp(full)->ap(full)
	case 7:
		msocket_test_init(case_num);

		t_param.th_num = 3;

		for (i = 0; i < t_param.th_num; i++)
		{
			t_param.thread_param[i].recv_speed = 1;
			t_param.thread_param[i].send_speed = 1;
			t_param.thread_param[i].cycle = 1;
		}
			#ifdef MSOCKET_TESTCP
		test_send_recv(1);
			#else
		test_send_recv(2);
			#endif
		break;
	//cp(full)->ap(slow)
	case 8:
		msocket_test_init(case_num);

		t_param.th_num = 3;

		for (i = 0; i < t_param.th_num; i++)
		{
			t_param.thread_param[i].recv_speed = 0;
			t_param.thread_param[i].send_speed = 1;
			t_param.thread_param[i].cycle = 1;
		}
			#ifdef MSOCKET_TESTCP
		test_send_recv(1);
			#else
		test_send_recv(2);
			#endif
		break;
	//cp(slow)->ap(full)
	case 9:
		msocket_test_init(case_num);

		t_param.th_num = 3;

		for (i = 0; i < t_param.th_num; i++)
		{
			t_param.thread_param[i].recv_speed = 1;
			t_param.thread_param[i].send_speed = 0;
			t_param.thread_param[i].cycle = 1;
		}
			#ifdef MSOCKET_TESTCP
		test_send_recv(1);
			#else
		test_send_recv(2);
			#endif
		break;

	//ap(full)->cp(full)
	case 10:
		msocket_test_init(case_num);

		t_param.th_num = 3;

		for (i = 0; i < t_param.th_num; i++)
		{
			t_param.thread_param[i].recv_speed = 1;
			t_param.thread_param[i].send_speed = 1;
			t_param.thread_param[i].cycle = 1;
		}
			#ifdef MSOCKET_TESTCP
		test_send_recv(2);
			#else
		test_send_recv(1);
			#endif
		break;
	//ap(full)->cp(slow)
	case 11:
		msocket_test_init(case_num);

		t_param.th_num = 3;

		for (i = 0; i < t_param.th_num; i++)
		{
			t_param.thread_param[i].recv_speed = 0;
			t_param.thread_param[i].send_speed = 1;
			t_param.thread_param[i].cycle = 1;
		}
			#ifdef MSOCKET_TESTCP
		test_send_recv(2);
			#else
		test_send_recv(1);
			#endif
		break;
	//ap(slow)->cp(full)
	case 12:
		msocket_test_init(case_num);

		t_param.th_num = 3;

		for (i = 0; i < t_param.th_num; i++)
		{
			t_param.thread_param[i].recv_speed = 1;
			t_param.thread_param[i].send_speed = 0;
			t_param.thread_param[i].cycle = 1;
		}
			#ifdef MSOCKET_TESTCP
		test_send_recv(2);
			#else
		test_send_recv(1);
			#endif
		break;
	//bidirectional full speed on one port
	case 13:
		msocket_test_init(case_num);

		t_param.th_num = 1;         //for bidirectional 2 threads will be created one for recv and latter for send

		for (i = 0; i < t_param.th_num; i++)
		{
			t_param.thread_param[i].recv_speed = 1;
			t_param.thread_param[i].send_speed = 1;
			t_param.thread_param[i].cycle = 1;
		}
			#ifdef MSOCKET_TESTCP
		test_send_recv(0);
			#else
		test_send_recv(0);
			#endif
		break;
	//bidirectional recv slow speed on one port
	case 14:
		msocket_test_init(case_num);

		t_param.th_num = 1;         //for bidirectional 2 threads will be created one for recv and latter for send

		for (i = 0; i < t_param.th_num; i++)
		{
			t_param.thread_param[i].recv_speed = 0;
			t_param.thread_param[i].send_speed = 1;
			t_param.thread_param[i].cycle = 1;
		}
			#ifdef MSOCKET_TESTCP
		test_send_recv(0);
			#else
		test_send_recv(0);
			#endif
		break;
	//bidirectional send slow speed on one port
	case 15:
		msocket_test_init(case_num);

		t_param.th_num = 1;         //for bidirectional 2 threads will be created one for recv and latter for send

		for (i = 0; i < t_param.th_num; i++)
		{
			t_param.thread_param[i].recv_speed = 1;
			t_param.thread_param[i].send_speed = 0;
			t_param.thread_param[i].cycle = 1;
		}
			#ifdef MSOCKET_TESTCP
		test_send_recv(0);
			#else
		test_send_recv(0);
			#endif
		break;

	//bidirectional on several ports
	case 16:
		msocket_test_init(case_num);

		t_param.th_num = 3;         //for bidirectional 2*num threads will be created one for recv and latter for send

		t_param.thread_param[0].recv_speed = 1;
		t_param.thread_param[0].send_speed = 1;
		t_param.thread_param[0].cycle = 1;

		t_param.thread_param[1].recv_speed = 1;
		t_param.thread_param[1].send_speed = 0;
		t_param.thread_param[1].cycle = 1;

		t_param.thread_param[2].recv_speed = 0;
		t_param.thread_param[2].send_speed = 1;
		t_param.thread_param[2].cycle = 1;


			#ifdef MSOCKET_TESTCP
		test_send_recv(0);
			#else
		test_send_recv(0);
			#endif
		break;
	//stress 1000 cycles
	//bidirectional full speed on one port
	case 17:
		msocket_test_init(case_num);

		t_param.th_num = 1;         //for bidirectional 2 threads will be created one for recv and latter for send

		for (i = 0; i < t_param.th_num; i++)
		{
			t_param.thread_param[i].recv_speed = 1;
			t_param.thread_param[i].send_speed = 1;
			t_param.thread_param[i].cycle = 1000;
		}
			#ifdef MSOCKET_TESTCP
		test_send_recv(0);
			#else
		test_send_recv(0);
			#endif
		break;
	//bidirectional recv slow speed on one port
	case 18:
		msocket_test_init(case_num);

		t_param.th_num = 1;         //for bidirectional 2 threads will be created one for recv and latter for send

		for (i = 0; i < t_param.th_num; i++)
		{
			t_param.thread_param[i].recv_speed = 0;
			t_param.thread_param[i].send_speed = 1;
			t_param.thread_param[i].cycle = 1000;
		}
			#ifdef MSOCKET_TESTCP
		test_send_recv(0);
			#else
		test_send_recv(0);
			#endif
		break;
	//bidirectional send slow speed on one port
	case 19:
		msocket_test_init(case_num);

		t_param.th_num = 1;         //for bidirectional 2 threads will be created one for recv and latter for send

		for (i = 0; i < t_param.th_num; i++)
		{
			t_param.thread_param[i].recv_speed = 1;
			t_param.thread_param[i].send_speed = 0;
			t_param.thread_param[i].cycle = 1000;
		}
			#ifdef MSOCKET_TESTCP
		test_send_recv(0);
			#else
		test_send_recv(0);
			#endif
		break;

	//bidirectional on several ports
	case 20:
		msocket_test_init(case_num);

		t_param.th_num = 3;         //for bidirectional 2*num threads will be created one for recv and latter for send

		t_param.thread_param[0].recv_speed = 1;
		t_param.thread_param[0].send_speed = 1;
		t_param.thread_param[0].cycle = 1000;

		t_param.thread_param[1].recv_speed = 1;
		t_param.thread_param[1].send_speed = 0;
		t_param.thread_param[1].cycle = 1000;

		t_param.thread_param[2].recv_speed = 0;
		t_param.thread_param[2].send_speed = 1;
		t_param.thread_param[2].cycle = 1000;


			#ifdef MSOCKET_TESTCP
		test_send_recv(0);
			#else
		test_send_recv(0);
			#endif
		break;
	default:
		break;

	}

	if (t_send_result[0] == 1)      //send is used
	{
		for (i = 0; i < t_param.th_num; i++ )
		{

			TESTPRINTF("t_send_result %d %d\n", i + 1, t_send_result[i + 1]);
			while (t_send_result[i + 1] == 0)
			{
				OSATaskSleep(50);
			}
			if (t_send_result[i + 1] == 1)
			{
				ret = 1;
				TESTPRINTF("The Send Test Failed on Port %d\n", i + 1);
			}
		}
	}
	if (t_recv_result[0] == 1)      //recv is used
	{
		for (i = 0; i < t_param.th_num; i++ )
		{

			TESTPRINTF("t_recv_result %d %d\n", i + 1, t_recv_result[i + 1]);
			while (t_recv_result[i + 1] == 0)
			{
				OSATaskSleep(50);
			}
			if (t_recv_result[i + 1] == 1)
			{
				ret = 1;
				TESTPRINTF("The Recv Test Failed on Port %d\n", i + 1);
			}
		}
	}
	if (ret == 0)
	{
		TESTPRINTF("The Test Succeeded\n");
	}
	else
	{
		TESTPRINTF("The Test failed\n");
	}
}
int inform_cp(int case_num)
{
#ifdef TEST_USER
	int fd_acipc;
	struct acipc_ioctl_arg acipc_arg;

	fd_acipc = open(ACIPC_DEV, O_RDWR);
	if (fd_acipc < 0)
	{
		printf("open device failed:%d\n", fd_acipc);
		return -1;
	}

	printf("open device successfully\n");

	acipc_arg.set_event = ACIPC_MSOCKET_TEST;
	acipc_arg.arg = case_num | 0xffff0000;
	if ((ioctl(fd_acipc, ACIPC_SET_EVENT, &acipc_arg)) != 0)
	{
		perror("ioctl error");
	}


	if ((ioctl(fd_acipc, ACIPC_SEND_DATA, &acipc_arg)) != 0)
	{
		perror("ioctl error");
	}
	close(fd_acipc);

#endif
#ifdef MSOCKET_TESTAP
	ACIPCDataSend(ACIPC_MSOCKET_TEST, case_num | 0xffff0000);
#endif
}

#ifdef MSOCKET_TESTCP
static UINT32 acipc_msocket_test_cb(UINT32 status)
{
	UINT32 adata;
	UINT32 case_num;

	TESTPRINTF("acipc handler called\r\n");
	ACIPCDataRead(&adata);

	case_num = adata & 0x0000ffff;

	TESTPRINTF("acipc read case_num %d\r\n", case_num);
	gCPTest_Start = case_num;
}

int msocket_test_acipc_init()
{

	ACIPCEventBind(ACIPC_MSOCKET_TEST, acipc_msocket_test_cb, ACIPC_CB_NORMAL, NULL);
}

int msocket_test_wait_start()
{

	do {
		while (gCPTest_Start == 0)
		{
			OSATaskSleep(50);
		}

		TESTPRINTF("call msocket_test with %d\r\n", gCPTest_Start);
		msocket_test(gCPTest_Start);
		gCPTest_Start = 0;
	} while (1);
}
#endif

#ifdef TEST_USER
int main(int argc, char *argv[])
{
	int case_num;

	if (argc < 2)
	{
		printf("Usage: %s <case_num>\n", argv[0]);
		return -1;
	}

	case_num = atoi(argv[1]);

	TESTPRINTF("AP choose test case %d\r\n", case_num);
	inform_cp(case_num);
	msocket_test(case_num);
}
#endif
