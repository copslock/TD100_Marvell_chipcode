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
	

    unsigned int        width       = 208;
    unsigned int        height      = 200;

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

    pSrcSurface1 = _gcuLoadRGBSurfaceFromFile(pContextA, "shun.bmp");
    
    //pSrcSurface2 = _gcuCreateBuffer(pContextA, 176, 144, GCU_FORMAT_ARGB8888, &srcVirtAddr, &srcPhysAddr);
    pSrcSurface2 = _gcuLoadYUVSurfaceFromFile(pContextA, "I420_176x144.yuv", GCU_FORMAT_I420, 176, 144);
    
    pDstSurface = _gcuCreateBuffer(pContextA, 400, 200, GCU_FORMAT_ARGB8888, &dstVirtAddr, &dstPhysAddr);

    if(pSrcSurface1 && pSrcSurface2 && pDstSurface)
    {
        GCU_SURFACE_DATA surfaceData;
        gcuQuerySurfaceInfo(pContextA, pSrcSurface1, &surfaceData);
        width = surfaceData.width;
        height = surfaceData.height;
        
        while(i < 1)
    	{
    	    memset(&fillData, 0, sizeof(fillData));
    		fillData.pSurface = pDstSurface;
    		fillData.color = 0xffcc00cc;
    		fillData.bSolidColor = GCU_TRUE;
    		srcRect.left = 0;
    		srcRect.top = 0;
    		srcRect.right = 400;
    		srcRect.bottom = 200;
    		fillData.pRect = &srcRect;
    		gcuFill(pContextA, &fillData);

    		/* rgba rotation */
    	    memset(&bltData, 0, sizeof(bltData));
    		bltData.pSrcSurface = pSrcSurface1;
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
    		bltData.rotation = GCU_ROTATION_180;
            gcuBlit(pContextA, &bltData);

            /* yuv rotation */
            memset(&bltData, 0, sizeof(bltData));
    		bltData.pSrcSurface = pSrcSurface2;
    		bltData.pDstSurface = pDstSurface;
    		srcRect.left = 0;
    		srcRect.top = 0;
    		srcRect.right = 176;
    		srcRect.bottom = 144;
    		bltData.pSrcRect = &srcRect;
    		dstRect.left = 200;
    		dstRect.top = 0;
    		dstRect.right = 376;
    		dstRect.bottom = 144;
    		bltData.pDstRect = &dstRect;
    		bltData.rotation = GCU_ROTATION_180;
            gcuBlit(pContextA, &bltData);
                
            gcuFinish(pContextA);
            _gcuDumpSurface(pContextA, pDstSurface, "rotate");
    		i++;
    	}
    }

    if(pSrcSurface1)
	{
	    _gcuDestroyBuffer(pContextA, pSrcSurface1);
	    pSrcSurface1 = NULL;
    }

    if(pSrcSurface2)
    { 
        _gcuDestroyBuffer(pContextA, pSrcSurface2);
        pSrcSurface2 = NULL;
    }

    if(pDstSurface)
    {
        _gcuDestroyBuffer(pContextA, pDstSurface);
        pDstSurface = NULL;
    }
	gcuDestroyContext(pContextA);
	gcuTerminate();
    
    return 0;
}

