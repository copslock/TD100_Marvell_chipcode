#include<signal.h>
#ifndef _DIAG_AL_H_
#define _DIAG_AL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define ICAT_DATA_LENGTH 0x1000
#define DIAG_COM_USB_MAX_PACKET_BYTE_SIZE 0x400
int diag_init(void);
void diag_sig_action(int signum, siginfo_t * info, void *p);
#ifdef __cplusplus
}
#endif
#endif
