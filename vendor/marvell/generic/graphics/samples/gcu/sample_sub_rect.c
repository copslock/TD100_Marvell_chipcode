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
	
	GCUSurface pSrcSurface = NULL;
	GCUSurface pDstSurface = NULL;

	GCU_RECT srcRect;
	GCU_RECT dstRect;

    GCUVirtualAddr      dstVirtAddr = 0;
    GCUPhysicalAddr     dstPhysAddr = 0;

	GCU_INIT_DATA initData;
	GCU_CONTEXT_DATA contextData;
	GCU_BLT_DATA bltData;
	GCU_SURFACE_DATA     surfaceData;

    unsigned int        width       = 0;
    unsigned int        height      = 0;

	int i = 0;

    // Init GCU library     
    memset(&initData, 0, sizeof(initData)); 	
  	gcuInitialize(&initData);

    // Create Context
	memset(&contextData, 0, sizeof(contextData));
	pContextA = gcuCreateContext(&contextData);
	if(pContextA == NULL)
	{
		result = gcuGetError();
		exit(0);
	}

    pSrcSurface = _gcuLoadRGBSurfaceFromFile(pContextA, "shun.bmp");
    gcuQuerySurfaceInfo(pContextA, pSrcSurface, &surfaceData);
	width = surfaceData.width;
	height = surfaceData.height;
	
    pDstSurface = _gcuCreateBuffer(pContextA, width * 2, height, GCU_FORMAT_ARGB8888, &dstVirtAddr, &dstPhysAddr);

    if(pSrcSurface && pDstSurface)
    {
        GCU_SURFACE_DATA surfaceData;
        gcuQuerySurfaceInfo(pContextA, pSrcSurface, &surfaceData);
        width = surfaceData.width;
        height = surfaceData.height;
        
        while(i < 1)
    	{
    	    /* normal blit */
    	    memset(&bltData, 0, sizeof(bltData));
    		bltData.pSrcSurface = pSrcSurface;
    		bltData.pDstSurface = pDstSurface;
    		srcRect.left = 0;
    		srcRect.top = 0;
    		srcRect.right = width;
    		srcRect.bottom = height;
    		bltData.pSrcRect = &srcRect;
    		dstRect.left = 0;
    		dstRect.top = 0;
    		dstRect.right = width;
    		dstRect.bottom = height;
    		bltData.pDstRect = &dstRect;
    		bltData.rotation = GCU_ROTATION_0;
            gcuBlit(pContextA, &bltData);

            /* sub rect blit */
            memset(&bltData, 0, sizeof(bltData));
    		bltData.pSrcSurface = pSrcSurface;
    		bltData.pDstSurface = pDstSurface;
    		srcRect.left = width / 4;
    		srcRect.top = 0;
    		srcRect.right = width / 4 * 3;
    		srcRect.bottom = height / 2;
    		bltData.pSrcRect = &srcRect;
    		dstRect.left = width ;
    		dstRect.top = 0;
    		dstRect.right = width * 2;
    		dstRect.bottom = height;
    		bltData.pDstRect = &dstRect;
    		bltData.rotation = GCU_ROTATION_0;
            gcuBlit(pContextA, &bltData);

            gcuFinish(pContextA);
    		_gcuDumpSurface(pContextA, pDstSurface, "sub_rect");
    		i++;
    	}
    }

    if(pSrcSurface)
	{
	    _gcuDestroyBuffer(pContextA, pSrcSurface);
    }

    if(pDstSurface)
    {
        _gcuDestroyBuffer(pContextA, pDstSurface);
    }
	gcuDestroyContext(pContextA);
	gcuTerminate();
    return 0;
}
