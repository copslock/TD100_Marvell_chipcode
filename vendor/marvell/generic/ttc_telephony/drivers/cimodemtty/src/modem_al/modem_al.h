#ifndef _MODEM_AL_H_
#define _MODEM_AL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define MODEM_DATA_LENGTH 0x1000
#define MODEM_COM_USB_MAX_PACKET_BYTE_SIZE 0x400

int modem_init();

#ifdef __cplusplus
}
#endif

#endif
