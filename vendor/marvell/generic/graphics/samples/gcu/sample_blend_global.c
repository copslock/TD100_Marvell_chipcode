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

	GCUVirtualAddr      srcVirtAddr = 0;
    GCUPhysicalAddr     srcPhysAddr = 0;
    GCUVirtualAddr      dstVirtAddr = 0;
    GCUPhysicalAddr     dstPhysAddr = 0;

	GCU_INIT_DATA initData;
	GCU_CONTEXT_DATA contextData;
	GCU_FILL_DATA fillData;
	GCU_BLEND_DATA blendData;

    unsigned int        width       = 128;
    unsigned int        height      = 128;

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

    pSrcSurface = _gcuCreateBuffer(pContextA, width, height, GCU_FORMAT_ARGB8888, &srcVirtAddr, &srcPhysAddr);
	pDstSurface = _gcuCreateBuffer(pContextA, width , height, GCU_FORMAT_ARGB8888, &dstVirtAddr, &dstPhysAddr);

	if(pSrcSurface && pDstSurface)
    {
        while(i < 1)
    	{
    	    /* Source Surface fill with red */ 
    	    memset(&fillData, 0, sizeof(fillData));
    		fillData.pSurface = pSrcSurface;
    		fillData.color = 0xffff0000;
    		fillData.bSolidColor = GCU_TRUE;
    		srcRect.left = 0;
    		srcRect.top = 0;
    		srcRect.right = width;
    		srcRect.bottom = height;
    		fillData.pRect = &srcRect;
    		gcuFill(pContextA, &fillData);

            /* Dst Surface fill with blue */
    		memset(&fillData, 0, sizeof(fillData));
    		fillData.pSurface = pDstSurface;
    		fillData.color = 0xff00ff00;
    		fillData.bSolidColor = GCU_TRUE;
    		srcRect.left = 0;
    		srcRect.top = 0;
    		srcRect.right = width;
    		srcRect.bottom = height;
    		fillData.pRect = &srcRect;
    		gcuFill(pContextA, &fillData);

    		memset(&blendData, 0, sizeof(blendData));
            blendData.pSrcSurface = pSrcSurface;
    		blendData.pDstSurface = pDstSurface;
    		srcRect.left = 0;
    		srcRect.top = 0;
    		srcRect.right = width;
    		srcRect.bottom = height;
    		blendData.pSrcRect = &srcRect;
    		dstRect.left = 0;
    		dstRect.top = 0;
    		dstRect.right = width;
    		dstRect.bottom = height;
    		blendData.pDstRect = &dstRect;
    		blendData.rotation = GCU_ROTATION_0;
            blendData.blendMode = GCU_BLEND_SRC_OVER;
            blendData.srcGlobalAlpha = 255;
            blendData.dstGlobalAlpha = 128;
            
    		gcuBlend(pContextA, &blendData);
    		
            gcuFinish(pContextA);
            _gcuDumpSurface(pContextA, pDstSurface, "blend_global");
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
