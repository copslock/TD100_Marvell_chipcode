
#ifndef __MUDP_DEMO_H__
#define __MUDP_DEMO_H__

//Tavor L MUDP/MTCP
#define NETLINK_MUDP 20
#define NETLINK_MTCP 21

int mudp_receive( int sock, char *buf, int size );
int mudp_send( int sock, char *buf, int size );


#endif

