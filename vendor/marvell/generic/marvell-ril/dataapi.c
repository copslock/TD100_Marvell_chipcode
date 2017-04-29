/******************************************************************************
 * *(C) Copyright 2008 Marvell International Ltd.
 * * All Rights Reserved
 * ******************************************************************************/

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <cutils/properties.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in6.h>
#include <sys/ioctl.h>

#include "atchannel.h"
#include "marvell-ril.h"

#if PLATFORM_SDK_VERSION >= 8
#define PPP0_PID_FILE   "/mnt/asec/ppp0.pid"
#else
#define PPP0_PID_FILE    "/sqlite_stmt_journals/ppp0.pid"
#endif

#ifdef DKB_CP
#define LOCK_FILE_PATH "/var/lock/LCK..cidatatty1"
#define DEVICE_FILE_PATH "/dev/cidatatty1"
#elif defined BROWNSTONE_CP
#define LOCK_FILE_PATH "/var/lock/LCK..ttyACM0"
#define DEVICE_FILE_PATH "/dev/ttyACM0"
#endif

#define CCINET_MAJOR 241
#define CCINET_IOC_MAGIC CCINET_MAJOR
#define CCINET_IP_ENABLE  _IOW(CCINET_IOC_MAGIC, 1, int)
#define CCINET_IP_DISABLE  _IOW(CCINET_IOC_MAGIC, 2, int)
#define TIOPPPON _IOW('T', 208, int)
#define TIOPPPOFF _IOW('T', 209, int)

extern struct psd_channel_decription ppp_channel;
#if defined(DKB_WUKONG)
extern struct psd_channel_decription wukong_pdp_channel[];
#endif

#ifndef _LINUX_IPV6_H
/*
 *    This is in linux/include/net/ipv6.h.
 */

struct in6_ifreq {
    struct in6_addr ifr6_addr;
    __u32 ifr6_prefixlen;
    unsigned int ifr6_ifindex;
};
#endif


int ifc_init(void);
int ifc_close(void);
int ifc_set_addr(const char *name, unsigned addr);
int ifc_set_mask(const char *name, in_addr_t mask);

int ifc_create_default_route(const char *name, unsigned addr);
int ifc_remove_default_route(const char *name);
int ifc_up(const char *name);
int ifc_down(const char *name);
int ifc_get_info(const char *name, unsigned *addr, unsigned *mask, unsigned *flags);

static int cinetdevfd = 0;
static int cidatadevfd = 0;


int is_ppp_enabled()
{
	char value[PROPERTY_VALUE_MAX];
	property_get("marvell.ril.ppp.enabled", value, "1");
	return atoi(value);
}

extern void setDNS(char* dns, char* ifname)
{
	unsigned char *buf, *packetEnd, *ipcpEnd;
	int len = strlen(dns);
	int buf_len = len / 2;
	buf = alloca(buf_len);

	int i = 0;
	unsigned int tmp;
	char tmpbuf[3];
	char primaryDNS[INET6_ADDRSTRLEN] = { '\0' };
	char secondaryDNS[INET6_ADDRSTRLEN] = { '\0' };
	char proper_name[32] = { '\0' };
	int dnsipv6_count = 0;

	while (i < len)
	{
		memcpy(tmpbuf, &dns[i], 2);
		tmpbuf[2] = '\0';
		sscanf(tmpbuf, "%x", &tmp);
		buf[i / 2] = tmp;
		i += 2;
	}
	packetEnd = buf + buf_len;
	LOGD("Protocol configuration options length:%d", buf_len);
	while (buf < packetEnd)
	{

		unsigned short type = buf[0] << 8 | buf[1];
		unsigned char len = buf[2]; //  this length field includes only what follows it
		switch (type)
		{
		// we are currently interested only on one type
		// but will specify some more for fute
		case 0x23C0:
			// PAP - not used  - ignore
			buf += len + 3;
			break;

		case 0x8021:
			// IPCP option for IP configuration - we are looking for DNS parameters.
			// it seem that this option may appear more than once!
			LOGD("parse DNS");
			ipcpEnd = buf + len + 3;
			buf += 3;
			// Ido : I guess that this must be a Nak because of the conf-request structure ???
			if (*buf == 0x03)
			{
				LOGV("parse DNS 1:buf:%d %d %d %d", buf[1], buf[2], buf[3], buf[4]);
				// Config-Nak found, advance to IPCP data start
				buf += 4;
				// parse any configuration
				while (buf < ipcpEnd)
				{
					LOGV("parse DNS 11:buf:%d, buf1:%d", *buf, buf[1]);
					if (*buf == 129)
					{
						// Primary DNS address
						buf += 2;
						LOGI("Primary DNS %d.%d.%d.%d\n", buf[0], buf[1], buf[2], buf[3]);
						sprintf(primaryDNS, "%d.%d.%d.%d", buf[0], buf[1], buf[2], buf[3]);
						buf += 4;
						continue;
					}
					if (*buf == 131)
					{
						// Secondary DNS address
						buf += 2;
						LOGI("secondary DNS %d.%d.%d.%d\r\n", buf[0], buf[1], buf[2], buf[3]);
						sprintf(secondaryDNS, "%d.%d.%d.%d", buf[0], buf[1], buf[2], buf[3]);
						//pNode->directIpAddress.ipv4.inSecondaryDNS=((buf[0]<<24) | (buf[1]<<16) | (buf[2]<<8) | buf[3]);
						buf += 4;
						continue;
					}
					// buf[0] includes the ipcp type, buf[1] the length of this field includes the whole TLV
					buf += buf[1];
				}
			}
			else
			{
				LOGW("parse DNS 2");
				buf += len;
			}
			break;
		case 0x0003:
		{
			// TS 24.008, 10.5.6.3
			// additional parameter: DNS Server IPv6 Address
			buf += 3;
			char *target_buf = NULL;
			if (len == 16) {
				dnsipv6_count++;
				if (dnsipv6_count == 1)	target_buf = primaryDNS;
				else if (dnsipv6_count == 2) target_buf = secondaryDNS;
			} else {
				LOGW("invalid length of DNS Server ipv6 address");
			}
			if (target_buf) {
				inet_ntop(AF_INET6, buf, target_buf, INET6_ADDRSTRLEN);
				LOGI("[DNS%d] %s", dnsipv6_count, target_buf);
			}
			buf += len;
			break;
		}
		default:
			LOGW("parse default");
			buf += len + 3;
			break;
		}
	}

	if ((primaryDNS[0] != '\0') && strcmp(primaryDNS, "0.0.0.0"))
	{
		sprintf(proper_name, "net.%s.dns1", ifname);
		property_set(proper_name, primaryDNS);
	}
	if ((secondaryDNS[0] != '\0') && strcmp(secondaryDNS, "0.0.0.0"))
	{
		sprintf(proper_name, "net.%s.dns2", ifname);
		property_set(proper_name, secondaryDNS);
	}
	//LOGI("---cid:%d,type:%s\n,apn:%s\n,address:%s\n",cid,type,apn,address);
}

static int configureInterface4(char* address, char* ifname)
{
	char gw[20];
	int ret;
	unsigned addr, netmask, gateway, dns1, dns2;
	int tmp1, tmp2, tmp3, tmp4;
	char proper_name[32] = { '\0' };

	sscanf(address, "%d.%d.%d.%d", &tmp1, &tmp2, &tmp3, &tmp4);
	sprintf(gw, "%d.%d.%d.%d", tmp1, tmp2, tmp3, tmp4 ^ 0xFF);
	LOGI("gw:%s", gw);

	sprintf(proper_name, "net.%s.gw", ifname);
	property_set(proper_name, gw);

	sscanf(address, "%d.%d.%d.%d", &tmp1, &tmp2, &tmp3, &tmp4);
	addr = tmp1 | (tmp2 << 8) | (tmp3 << 16) | (tmp4 << 24);
	sscanf(gw, "%d.%d.%d.%d", &tmp1, &tmp2, &tmp3, &tmp4);
	gateway = tmp1 | (tmp2 << 8) | (tmp3 << 16) | (tmp4 << 24);

	ifc_init();
	if (ifc_up(ifname))
	{
		LOGW("failed to turn on interface");
		ifc_close();
		return -1;
	}
	if (ifc_set_addr(ifname, addr))
	{
		LOGW("failed to set ipaddr");
		ifc_close();
		return -1;
	}

	inet_pton(AF_INET, "255.255.255.0", &netmask);
	if (ifc_set_mask(ifname, netmask)) {
		LOGW("failed to set netmask: %s", strerror(errno));
		ifc_close();
		return -1;
	}
#if 0 //telephony framework will create route
	if (ifc_create_default_route(ifname, gateway))
	{
		LOGW("failed to set default route");
		ifc_close();
		return -1;
	}
#endif

	ifc_close();
	return 0;
}

static int getInterfaceAddr6(const char* ifname, char* ipaddress);
static int configureInterface6(struct in6_addr addr, char* ifname)
{
	int ret = -1;
	ifc_init();

	if (ifc_up(ifname))
	{
		LOGW("failed to turn on interface");
		goto exit;
	}

	//Wait to get the gloabal address
	int count;
	for (count = 20 ;count > 0; count--) {
		sleep(1);
		char ipaddress[INET6_ADDRSTRLEN];
		ret = getInterfaceAddr6(ifname, ipaddress);
		if ( ret ==0 ) goto exit;
	}

	ifc_down(ifname);
exit:
	ifc_close();
	return ret;
}

int configureInterface(char* address, char* ifname)
{
	int ret = 0;
	struct addrinfo hints, *addr_ai;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;  /* Allow IPv4 or IPv6 */
	hints.ai_flags = AI_NUMERICHOST;

	ret = getaddrinfo(address, NULL, &hints, &addr_ai);

	if (ret != 0) {
		LOGE("getaddrinfo failed: invalid address %s", address);
		return -1;
	}

	if (addr_ai->ai_family == AF_INET6) {
		struct sockaddr_in6* sock_addr = (struct sockaddr_in6*)addr_ai->ai_addr;
		ret = configureInterface6(sock_addr->sin6_addr, ifname);
	} else if (addr_ai->ai_family == AF_INET) {
		ret = configureInterface4(address, ifname);
	} else {
		LOGE("configureInterface: getaddrinfo returned un supported address family %d", addr_ai->ai_family);
		ret = -1;
	}
	freeaddrinfo(addr_ai);
	return ret;
}

void deconfigureInterface(char* ifname)
{
	char proper_name[32] = { '\0' };

	sprintf(proper_name, "net.%s.gw", ifname);
	property_set(proper_name, "");
	sprintf(proper_name, "net.%s.dns1", ifname);
	property_set(proper_name, "");
	sprintf(proper_name, "net.%s.dns2", ifname);
	property_set(proper_name, "");
	ifc_init();

#if 0 //telephony framework will create route
	if (ifc_remove_default_route(ifname))
	{
		LOGW("failed to remove default route");
		ifc_close();
		return;
	}
#endif

	if (ifc_down(ifname))
	{
		LOGW("failed to turn off interface");
		ifc_close();
		return;
	}
	ifc_close();

}

void closeCiNetDev()
{
	if (cinetdevfd <= 0)
		return;

	close(cinetdevfd);
	cinetdevfd = -1;

}
static int CID_MUX_FD[MAX_DATA_CALLS];
#define CINET_IOC_MAGIC 'C'
#define CINET_IOCTL_ADDIF  _IOW(CINET_IOC_MAGIC, 1, int)
void enableInterface(int cid)
{
	if (cid < 1)
	{
		LOGE("%s: invalid cid:%d\n", __FUNCTION__, cid);
		return;
	}
#if defined(DKB_WUKONG)
	if(modem_mode == MUX_MODE)
	{
		int fd = 0;
		int ret;
		int displ_num = 8;

		fd = open(wukong_pdp_channel[cid].ttyName, O_RDWR);
		LOGE("%s: open device %s ret:%d\n", __FUNCTION__, wukong_pdp_channel[cid].ttyName,fd);
		ret = ioctl(fd, TIOCSETD, &displ_num);
		if(ret < 0)
			LOGE("%s: ioctl device %s ret:%d, error:%s\n", __FUNCTION__, wukong_pdp_channel[cid].ttyName,ret,strerror(errno));

		ret = ioctl(fd, CINET_IOCTL_ADDIF, cid - 1);
		if(ret < 0)
			LOGE("%s: add cinet device %s ret:%d, error:%s\n", __FUNCTION__, wukong_pdp_channel[cid].ttyName,ret,strerror(errno));
		CID_MUX_FD[cid - 1] = fd;
		return;
	}
#elif defined(DKB_CP)
	if (cinetdevfd <= 0)
	{
		LOGD("open ccichar");
		cinetdevfd = open("/dev/ccichar", O_RDWR);
	}

	if (cinetdevfd <= 0)
	{
		LOGW("open ccichar fail");
		return;
	}
	else
		LOGI("open ccichar success");
	ioctl(cinetdevfd, CCINET_IP_ENABLE, cid - 1);
#endif
}

void disableInterface(int cid)
{
	char ifname[32] = { '\0' };

	sprintf(ifname, "ccinet%d", cid-1);
#if defined (DKB_CP)
	if (cinetdevfd <= 0)
		return;
#endif
	LOGD("disable %s", ifname);

	deconfigureInterface(ifname);
#if defined(DKB_WUKONG)
	if(modem_mode == MUX_MODE)
	{
		if(CID_MUX_FD[cid - 1])
		{
			LOGD("close mux cid %d", cid);
			close(CID_MUX_FD[cid - 1]);
			CID_MUX_FD[cid - 1] = 0;
		}
		return;
	}
#elif defined(DKB_CP)
	ioctl(cinetdevfd, CCINET_IP_DISABLE, cid - 1);
#endif
}


void disableAllCcinetInterfaces()
{
	int cid;

	for (cid=1; cid <= MAX_DATA_CALLS; cid++)
		disableInterface(cid);

	closeCiNetDev();
}

int enablePPPInterface(int cid, const char* user, const char* passwd, char* ipaddress)
{
	char shell[256];
	int ret = -1;
	struct psd_channel_decription * pppchannel= &ppp_channel;
	if (cid < 1)
	{
		LOGE("%s: invalid cid:%d\n", __FUNCTION__, cid);
		return -1;
	}
#if  defined(DKB_WUKONG)
	if(modem_mode != MUX_MODE)
	{
		return -1;
	}
#elif defined(DKB_CP)
		if (cidatadevfd <= 0)
		{
			LOGD("open /dev/cctdatadev1");
			cidatadevfd = open("/dev/cctdatadev1", O_RDWR);
		}

		if (cidatadevfd <= 0)
		{
			LOGE("%s: Error open /dev/cctdatadev1!\n", __FUNCTION__);
			return -1;
		}
		else
			LOGI("open /dev/cctdatadev1 success");
		ioctl(cidatadevfd, TIOPPPON, cid - 1);
#endif

	// Kill PPPD if it exists
	FILE* fp = fopen(PPP0_PID_FILE, "r");
	if ( fp )
	{
		int pid = -1;
		fscanf(fp, "%d", &pid);
		if ( pid != -1 )
		{
			char cmd[256];
			sprintf(cmd, "kill %d", pid);
			ret = system(cmd);
			LOGD("exec cmd: %s and ret is: %d!", cmd, ret);
			//Wait pppd exit
			if ( ret == 0 ) sleep(2);
		}
		fclose(fp);
	}

	unlink(pppchannel->ttyLock);
	sprintf(shell, "pppd unit 0 %s 115200 -detach modem lock defaultroute usepeerdns user %s password %s &",
	pppchannel->ttyName, user && user[0] ? user : "any", passwd && passwd[0] ? passwd : "any");
	ret = system(shell);
	LOGD("Launch: %s and ret is: %d!\n", shell, ret);
	if ( ret == -1)
	{
		LOGE("Launch: %s failed!\n", shell);
		return -1;
	}

	//Wait to get the IP address
	int count = 20;
	unsigned myaddr = 0;
	ret = -1;
	ifc_init();
	while ( count > 0 )
	{
		sleep(1);
		ifc_get_info("ppp0", &myaddr, NULL, NULL);
		if ( myaddr )
		{
			ret = 0;
			sprintf(ipaddress, "%d.%d.%d.%d", myaddr & 0xFF, (myaddr >> 8) & 0xFF, (myaddr >> 16) & 0xFF, (myaddr >> 24) & 0xFF);
			LOGD("ppp0 IP address is: %s!\n", ipaddress);
			break;
		}
		--count;
	}
	ifc_close();
	return ret;
}

void disablePPPInterface(int cid)
{
#if defined(DKB_CP)
	if (cidatadevfd <= 0)
		return;
	LOGD("%s: close /dev/cctdatadev1!\n", __FUNCTION__);
	ioctl(cidatadevfd, TIOPPPOFF, cid - 1);
	close(cidatadevfd);
#endif

	// Kill PPPD if it exists
	FILE* fp = fopen(PPP0_PID_FILE, "r");
	if ( fp )
	{
		int ret;
		int pid = -1;
		fscanf(fp, "%d", &pid);
		if ( pid != -1 )
		{
			char cmd[256];
			sprintf(cmd, "kill %d", pid);
			ret = system(cmd);
			LOGD("exec cmd: %s and ret is: %d!", cmd, ret);
		}
		fclose(fp);
	}
	sleep(3); //Wait pppd exit
#if defined(DKB_CP)
	cidatadevfd = -1;
#endif
}

void disableAllInterfaces()
{
	if (is_ppp_enabled())
		disablePPPInterface(1);
	else
		disableAllCcinetInterfaces();

}

static int getInterfaceAddr4(const char* ifname, char* ipaddress)
{
	int ret = -1;
	unsigned myaddr = 0;

	ifc_init();
	ifc_get_info(ifname, &myaddr, NULL, NULL);
	if ( myaddr )
	{
		ret = 0;
		sprintf(ipaddress, "%d.%d.%d.%d", myaddr & 0xFF, (myaddr >> 8) & 0xFF, (myaddr >> 16) & 0xFF, (myaddr >> 24) & 0xFF);
		LOGD("%s IP address is: %s!\n", ifname, ipaddress);
	}
	ifc_close();
	return ret;
}

// get global ipv6 address
static int getInterfaceAddr6(const char* ifname, char* ipaddress)
{
	const int IPV6_ADDR_SCOPE_MASK = 0x00f0U;
	int ret = -1;
	char devname[20];
	int plen, scope, dad_status, if_idx;
	char addr6p[8][5];

	FILE *f = fopen("/proc/net/if_inet6", "r");
	if (f == NULL) {
		LOGE("Cannot open file: /proc/net/if_inet6");
		goto exit;
	}
	while (fscanf(f, "%4s%4s%4s%4s%4s%4s%4s%4s %08x %02x %02x %02x %20s\n",
			addr6p[0], addr6p[1], addr6p[2], addr6p[3], addr6p[4],
			addr6p[5], addr6p[6], addr6p[7], &if_idx, &plen, &scope,
			&dad_status, devname) != EOF) {
		if (!strcmp(devname, ifname)) {
			sprintf(ipaddress, "%s:%s:%s:%s:%s:%s:%s:%s",
					addr6p[0], addr6p[1], addr6p[2], addr6p[3],
					addr6p[4], addr6p[5], addr6p[6], addr6p[7]);
			if ((scope & IPV6_ADDR_SCOPE_MASK) == 0) {
				LOGV("%s Global inet6 addr: %s/%d", ifname, addr6, plen);
				// set gateway
				struct in6_addr addr;
				if(inet_pton(AF_INET6, ipaddress, &addr) != 1)
				{
					LOGE("inet_pton failed");
					goto exit1;
				}
				addr.s6_addr[15] ^= 0xFF;
				char gw_addr_str[INET6_ADDRSTRLEN + 1];
				if(inet_ntop(AF_INET6, addr.s6_addr, gw_addr_str, sizeof(gw_addr_str)) == NULL)
				{
					LOGE("inet_ntop failed");
					goto exit1;
				}
				char proper_name[PROPERTY_KEY_MAX];
				snprintf(proper_name, sizeof(proper_name), "net.%s.gw", ifname);
				property_set(proper_name, gw_addr_str);
				ret = 0;
				break;
			}
		}
	}
exit1:
	fclose(f);
exit:
	return ret;
}

int getInterfaceAddr(int af, const char* ifname, char* ipaddress)
{
	switch (af) {
	case AF_INET:
		return getInterfaceAddr4(ifname, ipaddress);
	case AF_INET6:
		return getInterfaceAddr6(ifname, ipaddress);
	default:
		errno = EAFNOSUPPORT;
		return (-1);
	}
}


