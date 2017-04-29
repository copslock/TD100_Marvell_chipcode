#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WINCE
#include <windows.h>
#endif

#include "gcu.h"
                          

#ifdef WINCE
int WINAPI WinMain (
    HINSTANCE   hInstance, 
    HINSTANCE   hPrevInstance, 
    LPTSTR      lpCmdLine, 
    int         nCmdShow)
#else
int main(int argc, char** argv)
#endif 
{
    GCUenum result;
	
	GCUContext pContextA;

	GCUSurface pSrcSurface1 = NULL;
    GCUSurface pSrcSurface2 = NULL;
	GCUSurface pDstSurface = NULL;

	GCU_RECT srcRect;
	GCU_RECT dstRect;

	GCUVirtualAddr      srcVirtAddr = 0;
    GCUPhysicalAddr     srcPhysAddr = 0;
    GCUVirtualAddr      dstVirtAddr = 0;
    GCUPhysicalAddr     dstPhysAddr = 0;

	GCU_INIT_DATA initData;
	GCU_CONTEXT_DATA contextData;
	GCU_FILL_DATA fillData;
	GCU_BLT_DATA bltData;

	int i = 0;

    // Init GCU library     
    memset(&initData, 0, sizeof(initData)); 
    initData.debug = 1;
  	gcuInitialize(&initData);

    // Create Context
	memset(&contextData, 0, sizeof(contextData));
	pContextA = gcuCreateContext(&contextData);
	if(pContextA == NULL)
	{
		result = gcuGetError();
		exit(0);
	}

    /* YU12, 4:2:0 */
    //pSrcSurface1 = _gcuCreateBuffer(pContextA, 176, 144, GCU_FORMAT_ARGB8888, &srcVirtAddr, &srcPhysAddr);
    //pSrcSurface1 = _gcuCreatePreAllocYV12(pContextA, 176, 144, srcVirtAddr, srcPhysAddr, "I420_176x144.yuv");
    pSrcSurface1 = _gcuLoadYUVSurfaceFromFile(pContextA, "YV12_176x144.yuv", GCU_FORMAT_YV12, 176, 144);

    /* UYVY, 4:2:2 */
    //pSrcSurface2 = _gcuCreateVideoBuffer(pContextA, 352, 288, GCU_FORMAT_ARGB8888, &srcVirtAddr, &srcPhysAddr);
    pSrcSurface2 = _gcuLoadYUVSurfaceFromFile(pContextA, "akiyo_352x288.uyvy", GCU_FORMAT_UYVY, 352, 288);
    pDstSurface = _gcuCreateBuffer(pContextA, 704, 352, GCU_FORMAT_ARGB8888, &dstVirtAddr, &dstPhysAddr);

    if(pSrcSurface1 && pSrcSurface2 && pDstSurface)
    {
        while(i < 1)
    	{
    	    memset(&fillData, 0, sizeof(fillData));
    		fillData.pSurface = pDstSurface;
    		fillData.color = 0xffffffff;
    		fillData.bSolidColor = GCU_TRUE;
    		dstRect.left = 0;
    		dstRect.top = 0;
    		dstRect.right = 704;
    		dstRect.bottom = 352;
    		fillData.pRect = &dstRect;
    		gcuFill(pContextA, &fillData);

    		memset(&bltData, 0, sizeof(bltData));
    		bltData.pSrcSurface = pSrcSurface1;
    		bltData.pDstSurface = pDstSurface;
    		srcRect.left = 0;
    		srcRect.top = 0;
    		srcRect.right = 176;
    		srcRect.bottom = 144;
    		bltData.pSrcRect = &srcRect;
    		dstRect.left = 0;
    		dstRect.top = 0;
    		dstRect.right = 144;
    		dstRect.bottom = 176;
    		bltData.pDstRect = &dstRect;
    		bltData.rotation = GCU_ROTATION_90;
    		gcuBlit(pContextA, &bltData);

    		memset(&bltData, 0, sizeof(bltData));
    		bltData.pSrcSurface = pSrcSurface2;
    		bltData.pDstSurface = pDstSurface;
    		srcRect.left = 0;
    		srcRect.top = 0;
    		srcRect.right = 352;
    		srcRect.bottom = 288;
    		bltData.pSrcRect = &srcRect;
    		dstRect.left = 352;
    		dstRect.top = 0;
    		dstRect.right = 640;
    		dstRect.bottom = 352;
    		bltData.pDstRect = &dstRect;
    		bltData.rotation = GCU_ROTATION_90;
    		gcuBlit(pContextA, &bltData);

    		
            gcuFinish(pContextA);
            _gcuDumpSurface(pContextA, pDstSurface, "yuv2rgb");
    		i++;
    	}
    }

    if(pSrcSurface1)
	{
	    _gcuDestroyBuffer(pContextA, pSrcSurface1);
    }
    
    if(pSrcSurface2)
	{
	    _gcuDestroyBuffer(pContextA, pSrcSurface2);
    }

    if(pDstSurface)
    {
        _gcuDestroyBuffer(pContextA, pDstSurface);
    }
	gcuDestroyContext(pContextA);
	gcuTerminate();
	return 0;
}
