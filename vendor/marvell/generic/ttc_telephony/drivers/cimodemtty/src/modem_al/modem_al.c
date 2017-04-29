#include "modem_al.h"
#include "pxa_dbg.h"
#include <pthread.h>
#include <sched.h>
#include "linux_types.h"
#include "osa.h"
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <linux/netlink.h>
#include <linux/version.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include "teldef.h"
#include "unistd.h"
#include "termios.h"
#if defined (BIONIC)
#include <sys/socket.h>
#endif

#define MODEMPRINT DPRINTF
#define MODEMASSERT ASSERT

#define MODEM_TTY_USB_DEVICE "/dev/ttymodem0"
#define USB_DRV_CONNECTED    "/sys/devices/platform/pxa-u2o/connected"
#define USB_DRV_COMPOSITE    "/sys/devices/platform/pxa-u2o/composite"
#define MODEM_COMPO_KEY      "acm"
#define PXA_GADGET_CABLE_CONNECTED '1'

#define USB_ANDROID_COMPOSITE    "/sys/devices/virtual/android_usb/android0/functions"
#define MODEM_ANDROID_COMPO_KEY      "marvell_modem"

pthread_t modemUsbRxTask;
//pthread_t modemProcAtChanTask;
int modemNlFd;
static int hPort = -1;
extern int bTerminated[NUM_OF_TEL_ATP];
extern TelAtpCtrl gAtpCtrl[NUM_OF_TEL_ATP];
extern void ProcessAtChanThread(TelAtParserID sAtpIndex);
extern BOOL tcInitAtParser( TelAtParserID * sAtpIndex);
extern BOOL tcOpenExtSerialPort(int *fd, char * serial_path, int flag );
static void initUsbConnection();
static void ReceiveDataFromDev(void) __attribute__((noreturn));
static UINT8 taskStack[OSA_STACK_SIZE];
char modemPath[255] = "/dev/ttymodem0";
extern int notifyAtParser(TelAtParserID sAtpIndex, unsigned char cmd);
extern pthread_mutex_t modem_thread_mutex;
extern pthread_cond_t modem_thread_cond;
extern int is_modem_thread_running;

static void openNetlink(void)
{
	struct sockaddr_nl snl;

	memset(&snl, 0, sizeof(snl));
	snl.nl_family = AF_NETLINK;
	snl.nl_pid = getpid();
	snl.nl_groups = 1;

	modemNlFd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
	if (modemNlFd < 0)
	{
		MODEMPRINT("openNetlink socket create error.\r\n");
		return;
	}

	if (bind(modemNlFd, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl)) < 0)
	{
		MODEMPRINT("openNetlink socket bind error.\r\n");
		close(modemNlFd);
	}
	return;

}
static const char *searchNetlink(const char *key, const char *buf, size_t len)
{
	size_t curlen;
	size_t keylen = strlen(key);
	const char *cur = buf;

	while (cur < buf + len - keylen)
	{
		curlen = strlen(cur);
		if (curlen == 0)
			break;
		if (!strncmp(key, cur, keylen) && cur[keylen] == '=')
			return cur + keylen + 1;
		cur += curlen + 1;
	}

	return NULL;
}

int AcmIsUSBCableConnected()
{
	FILE *fp_conn = NULL, *fp_comp = NULL;
	char connect = 0, composite_cfg[50];

	fp_conn = fopen(USB_DRV_CONNECTED, "rb");
	fp_comp = fopen(USB_DRV_COMPOSITE, "rb");

	if (fp_conn && fp_comp &&
	    (fread(&connect,1,1,fp_conn) == 1)  && (connect == (PXA_GADGET_CABLE_CONNECTED)) &&
	    (fread(&composite_cfg, 1, 40, fp_comp) && strstr(composite_cfg, MODEM_COMPO_KEY)))
	{
		fclose(fp_conn);
		fclose(fp_comp);
		MODEMPRINT(" ++++ Modem USB connected \n");
		return 1;    //connected
	}

	if(fp_conn)
	{
		fclose(fp_conn);
		fp_conn = NULL;
	}

	if(fp_comp)
	{
		fclose(fp_comp);
		fp_comp = NULL;
	}

	MODEMPRINT(" ---- Modem USB not connected \n");
	return 0;
}

// for kernel 3.0
int AcmIsInUSBComposite()
{
	FILE *fp_comp = NULL;
	char composite_cfg[50];

	fp_comp = fopen(USB_ANDROID_COMPOSITE, "rb");

	if (fp_comp &&
	    (fread(&composite_cfg, 1, 40, fp_comp) && strstr(composite_cfg, MODEM_ANDROID_COMPO_KEY)))
	{
		fclose(fp_comp);
		MODEMPRINT(" ++++ Modem USB connected \n");
		return 1;    //connected
	}

	if(fp_comp)
	{
		fclose(fp_comp);
		fp_comp = NULL;
	}

	MODEMPRINT(" ---- Modem USB not connected \n");
	return 0;
}

static void ReceiveDataFromDev(void)
{
	ssize_t dwBytesTransferred = -1;
	unsigned char *transferBuffer;
	fd_set readfds;
	struct timeval tv;
	int fdcount;
	const char *keys;
	const char *subsys;
	const char *driver;
	const char *action;
	const char *usb_state;
	int UsbStatus = -1; // -1: invalid, 0: usb out, 1: usb in
	int AcmInComposite = 0; // 0: acm not in usb composite, 1: acm is in composite

	transferBuffer = malloc(MODEM_DATA_LENGTH);
	while (1)
	{
		FD_ZERO(&readfds);
		if (modemNlFd > 0)
		{
			FD_SET(modemNlFd, &readfds);
		}

		tv.tv_sec = 10; //timeout is needed to allow FD_SET updates
		tv.tv_usec = 0;
		fdcount = select(modemNlFd + 1, &readfds, NULL, NULL, &tv);
		if (fdcount < 0)
		{
			MODEMPRINT("select modemNlFd error\n");
			continue;
		}
		else if (fdcount == 0)
		{
			//		printf("select modemNlFd timeout\n");
			continue;
		}
		else
		{
			if (modemNlFd > 0 && FD_ISSET(modemNlFd, &readfds))
			{
				MODEMPRINT("got Netlink Data\n");
				dwBytesTransferred = recv(modemNlFd, transferBuffer, MODEM_COM_USB_MAX_PACKET_BYTE_SIZE, 0);
				if (dwBytesTransferred <= 0)
					continue;

				/* search udev netlink keys from env */
				keys = (char *)(transferBuffer + strlen((char *)transferBuffer) + 1);
				subsys = searchNetlink("SUBSYSTEM", keys, dwBytesTransferred);
				driver = searchNetlink("DRIVER", keys, dwBytesTransferred);
				action = searchNetlink("ACTION", keys, dwBytesTransferred);
				usb_state = searchNetlink("USB_STATE", keys, dwBytesTransferred);

				UsbStatus = -1;
				AcmInComposite = 0;

				if (subsys && driver && action) // for kernel .35 or before
				{
					if ((!strcmp(subsys, "platform")) &&
						!(strcmp(driver, "pxa27x-udc") /* Tavor P USB 1.1 */
						 && strcmp(driver, "pxa3xx-u2d") /* Tavor P USB 2.0 */
						 && strcmp(driver, "pxa-u2o")
						 && strcmp(driver, "pxa9xx-u2o"))) /* Tavor PV USB 2.0 */
					{
						if (!strcmp(action, "add"))
						{
							UsbStatus = 1;
							AcmInComposite = AcmIsUSBCableConnected();
						}
						else if (!strcmp(action, "remove"))
						{
							UsbStatus = 0;
						}
					}
					else
					{
						UsbStatus = -1;
					}
				} else if (subsys && usb_state && action) // for kernel 3.0
				{
					if ((!strcmp(subsys, "android_usb")) &&
						!(strcmp(action, "change")))
					{
						if (!strcmp(usb_state, "CONFIGURED"))
						{
							UsbStatus = 1;
							AcmInComposite = AcmIsInUSBComposite();
						}
						else if (!strcmp(usb_state, "DISCONNECTED"))
						{
							UsbStatus = 0;
						}
					}
					else
					{
						UsbStatus = -1;
					}
				}
				else
				{
					/* wrong event */
					//MODEMPRINT("Modem: Wrong event\r\n");
				}


				/* plug in or out? */
				if (UsbStatus == 1)
				{
					MODEMPRINT("MODEM RX Netlink add \r\n");
					/* FIXME: detect /dev/ttygserial to avoid endless loop */
					if (access(MODEM_TTY_USB_DEVICE, F_OK) < 0) //existence test
					{
						MODEMPRINT("MODEM over FS: TTY device error\r\n");
					}

					pthread_mutex_lock(&modem_thread_mutex);
					if(is_modem_thread_running)
					{
						pthread_mutex_unlock(&modem_thread_mutex);
						notifyAtParser(TEL_MODEM_AT_CMD_ATP, TEL_ATP_CMD_EXIT_CHAN);
						pthread_mutex_lock(&modem_thread_mutex);
						while(is_modem_thread_running)
							pthread_cond_wait(&modem_thread_cond, &modem_thread_mutex);
						pthread_mutex_unlock(&modem_thread_mutex);
					}
					else
					{
						pthread_mutex_unlock(&modem_thread_mutex);
					}

					if (AcmInComposite)
					{
						bTerminated[TEL_MODEM_AT_CMD_ATP] = 0;
						initUsbConnection();
					}
					else
					{
						MODEMPRINT("MODEM is not in USB composite, Do Nothing\r\n");
					}
				}
				else if (UsbStatus == 0)
				{
					MODEMPRINT("MODEM RX Netlink remove\r\n");

					pthread_mutex_lock(&modem_thread_mutex);
					if(is_modem_thread_running)
					{
						pthread_mutex_unlock(&modem_thread_mutex);
						notifyAtParser(TEL_MODEM_AT_CMD_ATP, TEL_ATP_CMD_EXIT_CHAN);
						pthread_mutex_lock(&modem_thread_mutex);
						while(is_modem_thread_running)
							pthread_cond_wait(&modem_thread_cond, &modem_thread_mutex);
						pthread_mutex_unlock(&modem_thread_mutex);
						MODEMPRINT("MODEM resource is cleaned!\r\n");
					}
					else
					{
						pthread_mutex_unlock(&modem_thread_mutex);
						MODEMPRINT("MODEM has been removed, Do Nothing\r\n");
					}
				}

			}

		}
	}


}
void initUsbConnection()
{
	BOOL ret;
	OS_STATUS osaStatus;

	ret = tcOpenExtSerialPort(&gAtpCtrl[TEL_MODEM_AT_CMD_ATP].iFd, modemPath, O_RDWR | O_NOCTTY | O_NDELAY);

#if 0
	pthread_attr_t tattr;
	int ret;
	struct sched_param param;
	/* initialized with default attributes */
	ret = pthread_attr_init(&tattr);
	if (ret != 0 )
	{
		return -1;
	}
	ret = pthread_attr_setstacksize(&tattr, OSA_STACK_SIZE);
	if (ret != 0 )
	{
		return -1;
	}
	/* safe to get existing scheduling param */
	ret = pthread_attr_getschedparam(&tattr, &param);
	if (ret != 0 )
	{
		return -1;
	}
	/* set the priority; others are unchanged */
	param.sched_priority = MODEM_TASK_PRIORITY;
	/* setting the new scheduling param */
	ret = pthread_attr_setschedparam(&tattr, &param);
	if (ret != 0 )
	{
		return -1;
	}
	ret = pthread_create(&modemProcAtChanTask, &tattr, (void*)ProcessAtChanThread, NULL);
	if (ret != 0 )
	{
		MODEMPRINT("pthread_create modemProcAtChanTask error");
		return -1;
	}
#endif

	if (ret == TRUE)
	{
		hPort = gAtpCtrl[TEL_MODEM_AT_CMD_ATP].iFd;
		MODEMPRINT("Modem: hPort is %d\n", hPort);
	}
	else
	{
		MODEMPRINT("failed to init USB connection\n");
		return;
	}

	tcInitAtParser(&gAtpCtrl[TEL_MODEM_AT_CMD_ATP].index);

	osaStatus  = OSATaskCreate(&gAtpCtrl[TEL_MODEM_AT_CMD_ATP].taskRef,
				   (void *)taskStack, OSA_STACK_SIZE, MODEM_TASK_PRIORITY,
				   (CHAR *)"MODEM_READ_TASK", (void*)ProcessAtChanThread, (void *)TEL_MODEM_AT_CMD_ATP);
	if ( osaStatus != OS_SUCCESS)
	{
		DPRINTF("OSATaskCreate error!\n");
		return;
	}

	gAtpCtrl[TEL_MODEM_AT_CMD_ATP].bEnable = TRUE;
	MODEMPRINT( "MODEM: Initialized USB connection %s.\r\n", MODEM_TTY_USB_DEVICE);
}

int modem_init()
{
	MODEMPRINT("%s enter\n", __FUNCTION__);

	//initUsbConnection();

	openNetlink();
	if ( pthread_create( &modemUsbRxTask, NULL, (void *)ReceiveDataFromDev, NULL) != 0 )
	{
		MODEMPRINT("pthread_create modemUsbRxTask error");
		return -1;
	}
	return 0;
}
