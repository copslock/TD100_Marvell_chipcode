#define MAX_TEST_THREAD 10
struct _thread_param
{
	int recv_speed; //1 full speed; 0 slow speed
	int send_speed;
	int cycle;      //cycle of 2k packets, by default 1
	int port;
	int sock;
};

struct _test_param
{
	int th_num;
	struct _thread_param thread_param[MAX_TEST_THREAD];
};


