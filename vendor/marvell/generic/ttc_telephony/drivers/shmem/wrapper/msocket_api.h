#ifndef __MSOCKET_H__
#define __MSOCKET_H__

#define MSOCKET_IOC_MAGIC       0xD0
#define MSOCKET_IOC_BIND 	_IO (MSOCKET_IOC_MAGIC, 1)
#define MSOCKET_IOC_UP 		_IO (MSOCKET_IOC_MAGIC, 2)
#define MSOCKET_IOC_DOWN 	_IO (MSOCKET_IOC_MAGIC, 3)
#define MSOCKET_IOC_PMIC_QUERY	_IOR(MSOCKET_IOC_MAGIC, 4, int)


/* Allow the use in C++ code.  */
#ifdef __cplusplus
extern "C" {
#endif

extern int msocket(int port);
extern int mclose(int sock);
extern int msend(int sock, const void *buf, int len, int flags);
extern int mrecv(int sock, void *buf, int len, int flags);
extern int msocket_linkdown(void);
extern int msocket_linkup(void);

#ifdef __cplusplus
}
#endif /* C++ */

#endif /* __MSOCKET_H__ */
