#include <NVMProxy.h>

#if !defined (BIONIC)
extern "C" {
BOOL NVM_Init(void);
}
#endif /* BIONIC */

BOOL NVM_Init(void)
{
	CNVMProxy* proxy;

	proxy = new CNVMProxy();
	return proxy->Init();
}
