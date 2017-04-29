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
	GCU_FILTER_BLT_DATA filterData;
	GCU_SURFACE_DATA surfaceData;

    unsigned int        width       = 0;
    unsigned int        height      = 0;

	int i = 0;
	float vFilter[9] = {0.1f,0.1f,0.1f,0.1f,0.1f,0.1f,0.1f,0.1f,0.1f};

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

    
    pDstSurface = _gcuCreateBuffer(pContextA, surfaceData.width * 2, surfaceData.height, GCU_FORMAT_ARGB8888, &dstVirtAddr, &dstPhysAddr);
    
    if(pSrcSurface && pDstSurface)
    {
        GCU_SURFACE_DATA surfaceData;
        gcuQuerySurfaceInfo(pContextA, pSrcSurface, &surfaceData);
        width = surfaceData.width;
        height = surfaceData.height;
        
        while(i < 1)
    	{
    	    /* deinterlace */
    	    memset(&filterData, 0, sizeof(filterData));
    		filterData.pSrcSurface = pSrcSurface;
    		filterData.pDstSurface = pDstSurface;
    		srcRect.left = 0;
    		srcRect.top = 0;
    		srcRect.right = width;
    		srcRect.bottom = height;
    		filterData.pSrcRect = &srcRect;
    		dstRect.left = 0;
    		dstRect.top = 0;
    		dstRect.right = width;
    		dstRect.bottom = height;
    		filterData.pDstRect = &dstRect;
    		filterData.rotation = GCU_ROTATION_90;
    		filterData.filterType = GCU_H_USER_FILTER;

    	    gcuSetFilter(pContextA, GCU_H_USER_FILTER, 9, vFilter);
    	    gcuFilterBlit(pContextA, &filterData);

            /* blur */
    	    memset(&filterData, 0, sizeof(filterData));
    		filterData.pSrcSurface = pSrcSurface;
    		filterData.pDstSurface = pDstSurface;
    		srcRect.left = 0;
    		srcRect.top = 0;
    		srcRect.right = width;
    		srcRect.bottom = height;
    		filterData.pSrcRect = &srcRect;
    		dstRect.left = width;
    		dstRect.top = 0;
    		dstRect.right = width * 2;
    		dstRect.bottom = height;
    		filterData.pDstRect = &dstRect;
    		filterData.rotation = GCU_ROTATION_90;
            filterData.filterType = GCU_BLUR_FILTER;
    		
    		gcuFilterBlit(pContextA, &filterData);

    		gcuFinish(pContextA);
            _gcuDumpSurface(pContextA, pDstSurface, "filter_blit");
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
