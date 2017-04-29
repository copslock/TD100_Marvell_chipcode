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
	GCU_FILL_DATA fillData;
	GCU_BLEND_DATA blendData;

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

    pSrcSurface = _gcuLoadRGBSurfaceFromFile(pContextA, "football.bmp");
	
    pDstSurface = _gcuCreateBuffer(pContextA, width , height, GCU_FORMAT_RGB565, &dstVirtAddr, &dstPhysAddr);

    if(pSrcSurface && pDstSurface)
    {
        GCU_SURFACE_DATA surfaceData;
        gcuQuerySurfaceInfo(pContextA, pSrcSurface, &surfaceData);
        width = surfaceData.width;
        height = surfaceData.height;
        
        while(i < 1)
    	{
    	    /* fill dst surface with green */
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

            /* blend blit */
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
            blendData.srcGlobalAlpha = 15;
            blendData.dstGlobalAlpha = 0xff;

            gcuBlend(pContextA, &blendData);

            gcuFinish(pContextA);
    		_gcuDumpSurface(pContextA, pDstSurface, "blend_pixel");
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
