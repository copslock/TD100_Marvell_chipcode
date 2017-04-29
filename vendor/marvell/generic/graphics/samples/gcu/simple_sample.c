#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#ifdef WINCE
#include <windows.h>
#endif

#include "gcu.h"
                          
void _loadYUV(const char* filename, void* pData, unsigned int size)
{
    FILE*  fp = NULL;
   
    fp = fopen(filename, "rb");
    if(fp)
    {
        fread(pData, 1, size, fp);
        fclose(fp);          
    }
    else
    {
        printf("fail to load YUV data from %s\n",filename);
    }
}

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
	GCUFence   pFence[2];
	int current = 0;
	int next = 1;
	
	GCUSurface pSrcSurface1 = NULL;
	GCUSurface pSrcSurface2 = NULL;
	GCUSurface pDstSurface = NULL;
	GCUSurface pDstSurface2 = NULL;
	GCUSurface pTestSrcSurface[6], pTestDstSurface;

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
    GCU_BLEND_DATA blendData;

    GCU_SURFACE_DATA     surfaceData;

    unsigned int        width       = 352;
    unsigned int        height      = 288;
    GCU_ALLOC_INFO  	preAllocInfos[1];

	float vFilter[9] = {1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f};

    GCU_ROTATION aRot[] = { GCU_ROTATION_0,
                            GCU_ROTATION_90,
                            GCU_ROTATION_180,
                            GCU_ROTATION_270, 
                            GCU_ROTATION_FLIP_H,
                            GCU_ROTATION_FLIP_V
                           };
    GCU_FORMAT eSurFormat[6] = {
        GCU_FORMAT_ARGB8888,
        GCU_FORMAT_RGB565,
        GCU_FORMAT_XRGB8888, 
        GCU_FORMAT_ARGB1555,
        GCU_FORMAT_XRGB1555,
        GCU_FORMAT_L8
    };
    GCUuint arrayColor[6] = {
        0xFF000000,
        0xFFFF0000,
        0xFF00FF00,
        0xFF0000FF,
        0xFFFF00FF,
        0xFF00FFFF
    };
    GCUuint i , j, k, iwidth = 128, iheight = 128;
    GCUenum eRes;
    struct timeval tpstart,tpend;
    double timeuse;
    
	printf("test begin\n");

	i =  0;
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

	pFence[0] = gcuCreateFence(pContextA);
	pFence[1] = gcuCreateFence(pContextA);
	
    //pSrcSurface1 = _gcuLoadRGBSurfaceFromFile(pContextA, "shun.bmp");

    pSrcSurface1 = _gcuCreateBuffer(pContextA, 1280, 720, GCU_FORMAT_UYVY, &dstVirtAddr, &dstPhysAddr);
    gcuQuerySurfaceInfo(pContextA, pSrcSurface1, &surfaceData);
	width = surfaceData.width;
	height = surfaceData.height;
    
    pDstSurface = _gcuCreateBuffer(pContextA, 640, 480, GCU_FORMAT_RGB565, &dstVirtAddr, &dstPhysAddr);
    
    if(pSrcSurface1)
    {
        while(i < 10)
        {
           /*
    		memset(&fillData, 0, sizeof(fillData));
    		fillData.pSurface = pDstSurface;
    		fillData.color = 0xff00ff00;
    		fillData.bSolidColor = GCU_FALSE;
    		srcRect.left = 0;
    		srcRect.top = 0;
    		srcRect.right = 560;
    		srcRect.bottom = 560;
    		fillData.pRect = &srcRect;
    		fillData.pPattern = pSrcSurface1;
    		gcuFill(pContextA, &fillData);*/
    		            
    		memset(&bltData, 0, sizeof(blendData));
    		bltData.pSrcSurface = pSrcSurface1;
    		bltData.pDstSurface = pDstSurface;
    		srcRect.left = 0;
    		srcRect.top = 0;
    		srcRect.right = srcRect.left + width/2;
    		srcRect.bottom = srcRect.top + height/2;
    		bltData.pSrcRect = NULL;
    		dstRect.left = width/2;
    		dstRect.top = height/2;
    		dstRect.right = dstRect.left + width/2;
    		dstRect.bottom = dstRect.top + height/2;
    		bltData.pDstRect = NULL;
    		bltData.rotation = GCU_ROTATION_0;

            gcuBlit(pContextA, &bltData);
            /*
            memset(&blendData, 0, sizeof(blendData));
    		blendData.pSrcSurface = pSrcSurface1;
    		blendData.pDstSurface = pDstSurface;
    		blendData.pSrcRect = NULL;
    		dstRect.left = width + 16;
    		dstRect.top = 0;
    		dstRect.right = dstRect.left + width + 16;
    		dstRect.bottom = dstRect.top + height + 16;
    		blendData.pDstRect = &dstRect;
    		blendData.rotation = GCU_ROTATION_270;

    		blendData.blendMode = GCU_BLEND_SRC_OVER;
    		blendData.srcGlobalAlpha = 0x88;
    		blendData.dstGlobalAlpha = 0xff;
            gcuBlend(pContextA, &blendData);
            

            
            /*
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
            */
            

    		//gcuSet(pContextA, GCU_FILTER_MODE, GCU_BLUR_FILTER);
    		//gcuSet(pContextA, GCU_QUALITY, GCU_QUALITY_NORMAL);
    		//gcuSetFilter(pContextA, GCU_V_USER_FILTER, 9, vFilter);
    		//gcuFilterBlit(pContextA, &bltData);
    		

            //gcuSet(pContextA, GCU_BLEND_MODE, GCU_BLEND_SRC_OVER);
            //gcuSet(pContextA, GCU_GLOBAL_SRC_ALPHA, 128);
            //gcuSet(pContextA, GCU_GLOBAL_DST_ALPHA, 127);
            //gcuBlit(pContextA, &bltData);
    		//gcuBlend(pContextA, &blndData);

            //gcuWaitFence(pContextA, pFence[current], GCU_INFINITE);

            gettimeofday(&tpstart,NULL);
            gcuSendFence(pContextA, pFence[0]);
    		gcuFlush(pContextA);
    		gcuWaitFence(pContextA, pFence[0], GCU_INFINITE);
    		gettimeofday(&tpend,NULL);
    		timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
    		timeuse/=1000000;
    		printf("processor time is %lf s\n",timeuse);
    		
    		current = 1 - current;
    		next = 1 - next;
    		//gcuFlush(pContextA);
    		//_gcuDumpSurface(pContextA, pSrcSurface1, "simple_sample");
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

    gcuDestroyFence(pContextA, pFence[0]);
    gcuDestroyFence(pContextA, pFence[1]);
	gcuDestroyContext(pContextA);
	gcuTerminate();
  	return 0;
}
