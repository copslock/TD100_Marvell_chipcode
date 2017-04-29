#ifndef PROD_API_H_
#define PROD_API_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define PROD_SOCKET_NAME "/dev/socket/prod_sock"

#define CODE_SWITCH_TO_AT_CMD_SRV 0x00000001UL
#define CODE_SWITCH_TO_DIAG       0x00000002UL

#define TURN_OFF_AT_CMD_SRV                   0x00000011UL
#define TURN_OFF_AT_CMD_SRV_AND_START_DIAG    0x00000022UL


typedef void (*PROD_PROCESS_FP)(const unsigned char *, int len);

typedef struct prod_ctrl_tag{
	int fd;
	PROD_PROCESS_FP fp;
}PROD_CTRL;

void create_prod_conn_task(PROD_CTRL *pc);

#ifdef __cplusplus
}
#endif

#endif
