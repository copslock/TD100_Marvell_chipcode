/******************************************************************************
//(C) Copyright [2008] Marvell International Ltd.  
// All Rights Reserved
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <sys/time.h>

#include "gcu.h"
#include "ippIE.h"
#include "misc.h"
#include "bmm_lib.h"

#include "ippLV.h"

unsigned int _getTickCount()
{
	struct timeval tv;
	struct timezone tz;
	unsigned int tickcount = 0;

	gettimeofday(&tv,&tz);
	tickcount = (tv.tv_sec*1000000 + tv.tv_usec);

	return tickcount;
}

const char sFormat[][32] = {
	"FORMAT_YUV420PLANAR",
	"FORMAT_YUYVPACKED",
	"FORMAT_UYVYPACKED",
	"FORMAT_BGRA",
	"FORMAT_BGRA5551",
	"FORMAT_BGR565",
};

const char sDeinterlace[][32] = {
	"DEINT_OFF",
	"DEINT_9TAP",
	"DEINT_VT",
};


/******************************************************************************
// Name:			 DisplayLibVersion
//
// Description:		 Display library build information on the text console
//
// Input Arguments:  None.
//
// Output Arguments: None				
//
// Returns:			 None				
*******************************************************************************/
void DisplayLibVersion()
{
	char libversion[128]={'\0'};
	IppCodecStatus ret;
	ret = GetLibVersion_IppIE(libversion,sizeof(libversion));
	if(0 == ret){
		IPP_Printf("\n*****************************************************************\n");
	    IPP_Printf("This library is built from %s\n",libversion);
		IPP_Printf("*****************************************************************\n");
	}else{
		IPP_Printf("\n*****************************************************************\n");
		IPP_Printf("Can't find this library version information\n");
		IPP_Printf("*****************************************************************\n");
	}
}


/*Deinterlace Sample code*/
int main(int argc, char *argv[])
{
	FILE *pIn  = NULL;
	FILE *pOut = NULL;
	int  iSrcSize = 0, iDstSize = 0;
	int iSrcWidth = 0, iSrcHeight = 0, iSrcFormat, iDstWidth, iDstHeight, iDstFormat, iDeinterlace;
	unsigned char *pSrc = NULL, *pDst = NULL;
	int  src_stride[3] = {0, 0, 0}, dst_stride[3] = {0, 0, 0};
	int  ret = 0, i = 0;
	unsigned int  total = 0, start_tick = 0;

	Ipp8u*	srcVirtAddr[3] = {NULL, NULL, NULL};
	void*	srcPhysAddr[3] = {NULL, NULL, NULL};
	Ipp8u*	dstVirtAddr[3] = {NULL, NULL, NULL};
	void*	dstPhysAddr[3] = {NULL, NULL, NULL};

	MiscGeneralCallbackTable *pCallbackTable;
	IppDeinterlaceState *pDeinterlaceState;

	GCU_INIT_DATA data = {0};


	printf("------------------------Marvell Deinterlace-------------------------\n");
	printf("                   Supported Operation Modes\n");
	printf("--------------------------------------------------------------------\n");
	printf("\t   Input        Output             Deinterlace     Resize\n"
		"\t1) 420p/422i    RGB(A)xxx(x)       vt              off\n"
		"\t2) 420p/422i    RGB(A)xxx(x)       off/9tap        on\n"
		"\t3) 420p/422i    <same as input>    9tap            off\n"
	);
	printf("--------------------------------------------------------------------\n");
	printf("Usage: %s <input.yuv> <src_w> <src_h> <420p|yuyv|uyvy> <output.yuv|null> <dst_w> <dst_h> <420p|yuyv|uyvy|rgba8888|rgba5551|rgb565> <off|9tap|vt>\n", argv[0]);
	printf("--------------------------------------------------------------------\n");

	DisplayLibVersion();
	
	gcuInitialize(&data);	// gcuInitialize doesn't support multi-threading

	if(argc != 10)
		return -1;

	iSrcWidth =  atoi(argv[2]);
	iSrcHeight = atoi(argv[3]);
	if (strcmp(argv[4], "420p") == 0 || strcmp(argv[4], "420P") == 0)
		iSrcFormat = FORMAT_YUV420PLANAR;
	else if (strcmp(argv[4], "yuyv") == 0 || strcmp(argv[4], "YUYV") == 0)
		iSrcFormat = FORMAT_YUYVPACKED;
	else if (strcmp(argv[4], "uyvy") == 0 || strcmp(argv[4], "UYVY") == 0)
		iSrcFormat = FORMAT_UYVYPACKED;
	else
	{
		printf("%s: Unknown input format!\n", argv[0]);
		return -1;
	}

	iDstWidth =  atoi(argv[6]);
	iDstHeight = atoi(argv[7]);
	if (strcmp(argv[8], "420p") == 0 || strcmp(argv[8], "420P") == 0)
		iDstFormat = FORMAT_YUV420PLANAR;
	else if (strcmp(argv[8], "yuyv") == 0 || strcmp(argv[8], "YUYV") == 0)
		iDstFormat = FORMAT_YUYVPACKED;
	else if (strcmp(argv[8], "uyvy") == 0 || strcmp(argv[8], "UYVY") == 0)
		iDstFormat = FORMAT_UYVYPACKED;
	else if (strcmp(argv[8], "bgra8888") == 0 || strcmp(argv[8], "BGRA8888") == 0)
		iDstFormat = FORMAT_BGRA;
	else if (strcmp(argv[8], "bgra5551") == 0 || strcmp(argv[8], "BGRA5551") == 0)
		iDstFormat = FORMAT_BGRA5551;
	else if (strcmp(argv[8], "bgr565") == 0 || strcmp(argv[8], "BGR565") == 0)
		iDstFormat = FORMAT_BGR565;
	else
	{
		printf("Unknown output format!\n");
		return -1;
	}

	if (strcmp(argv[9], "off") == 0 || strcmp(argv[9], "OFF") == 0)
		iDeinterlace = DEINT_OFF;
	else if (strcmp(argv[9], "9tap") == 0 || strcmp(argv[9], "9TAP") == 0)
		iDeinterlace = DEINT_9TAP;
	else if (strcmp(argv[9], "vt") == 0 || strcmp(argv[9], "VT") == 0)
		iDeinterlace = DEINT_VT;
	else
	{
		printf("Unknown deinterlace mode!\n");
		return -1;
	}

	printf("input       : %s\n", argv[1]);
	printf("width       : %d\n", iSrcWidth);
	printf("height      : %d\n", iSrcHeight);
	printf("format      : %s\n", sFormat[iSrcFormat]);
	printf("output      : %s\n", argv[5]);
	printf("width       : %d\n", iDstWidth);
	printf("height      : %d\n", iDstHeight);
	printf("format      : %s\n", sFormat[iDstFormat]);
	printf("deinteralce : %s\n", sDeinterlace[iDeinterlace]);

	//pIn = fopen("704_480_No31frame.yuv", "rb");
	pIn = fopen(argv[1], "rb");
	if(!pIn)
	{
		printf("%s: failed to open the input file %s\n", argv[0], argv[1]);
		return -1;
	}
	else
		printf("%s: open %s success\n", argv[0], argv[1]);

	if (strcmp(argv[5], "null") == 0 || strcmp(argv[5], "NULL") == 0)
	{
		pOut = NULL;
	}
	else
	{
		pOut = fopen(argv[5], "wb");
		if(!pOut)
		{
			printf("%s: failed to open the output file %s\n", argv[0], argv[5]);
			return -1;
		}
		else
			printf("%s: open %s success\n", argv[0], argv[5]);
	}

	/*mem allocate*/
	miscInitGeneralCallbackTable(&pCallbackTable);

	if (ippStsNoErr != ippiDeinterlaceResizeCsc_InitAlloc(iSrcWidth, iSrcHeight, iSrcFormat,
		iDstWidth, iDstHeight, iDstFormat, iDeinterlace, pCallbackTable, &pDeinterlaceState))
	{
		miscFreeGeneralCallbackTable(&pCallbackTable);
		printf("%s: Failed to initialize the deinterlace! check the operation mode!\n\n", argv[0]);
		return -1;
	}

	if (iSrcFormat == FORMAT_YUV420PLANAR && (iDstFormat == FORMAT_BGRA || iDstFormat == FORMAT_BGRA5551 || iDstFormat == FORMAT_BGR565))
	{
		src_stride[0] = (iSrcWidth + 15) & ~0x0f;
		src_stride[1] = src_stride[0] >> 1;
		src_stride[2] = src_stride[0] >> 1;
		if (iDstFormat == FORMAT_BGRA)
			dst_stride[0] = ((iDstWidth + 15) & ~0x0f) * 4;
		else
			dst_stride[0] = ((iDstWidth + 15) & ~0x0f) * 2;
		iSrcSize = (src_stride[0] * iSrcHeight * 3) >>1;
		iDstSize = dst_stride[0] * iDstHeight;
	}
	else if ((iSrcFormat == FORMAT_YUYVPACKED || iSrcFormat == FORMAT_UYVYPACKED) && (iDstFormat == FORMAT_BGRA || iDstFormat == FORMAT_BGRA5551 || iDstFormat == FORMAT_BGR565))
	{
		src_stride[0] = ((iSrcWidth + 31) & ~0x1f) * 2;
		if (iDstFormat == FORMAT_BGRA)
			dst_stride[0] = ((iDstWidth + 15) & ~0x0f) * 4;
		else
			dst_stride[0] = ((iDstWidth + 15) & ~0x0f) * 2;
		iSrcSize = src_stride[0] * iSrcHeight;
		iDstSize = dst_stride[0] * iDstHeight;
	}
	else if ((iSrcFormat == FORMAT_YUYVPACKED || iSrcFormat == FORMAT_UYVYPACKED) && iDstFormat == iSrcFormat)
	{
		src_stride[0] = ((iSrcWidth + 31) & ~0x1f) * 2;
		dst_stride[0] = ((iDstWidth + 31) & ~0x1f) * 2;
		iSrcSize = src_stride[0] * iSrcHeight;
		iDstSize = dst_stride[0] * iDstHeight;
	}
	else if (iSrcFormat == FORMAT_YUV420PLANAR && iDstFormat == iSrcFormat)
	{
		src_stride[0] = (iSrcWidth + 127) & ~127;
		src_stride[1] = src_stride[0] >> 1;
		src_stride[2] = src_stride[0] >> 1;

		dst_stride[0] = (iDstWidth + 127) & ~127;
		dst_stride[1] = dst_stride[0] >> 1;
		dst_stride[2] = dst_stride[0] >> 1;

		iSrcSize = (src_stride[0] * iSrcHeight * 3) >>1;
		iDstSize = (dst_stride[0] * iDstHeight * 3) >>1;
	}

	srcVirtAddr[0] = (Ipp8u*)bmm_malloc(iSrcSize, BMM_ATTR_DEFAULT);
	dstVirtAddr[0] = (Ipp8u*)bmm_malloc(iDstSize, BMM_ATTR_DEFAULT);
	memset(srcVirtAddr[0], 0, iSrcSize);
	memset(dstVirtAddr[0], 0, iDstSize);
	if((srcVirtAddr == NULL) || (dstVirtAddr == NULL)){
		printf("%s: Error of bmm\n", argv[0]);
		return -1;
	}
	srcPhysAddr[0] = (void*)bmm_get_paddr(srcVirtAddr[0]);
	dstPhysAddr[0] = (void*)bmm_get_paddr(dstVirtAddr[0]);

	if (iSrcFormat == FORMAT_YUV420PLANAR)
	{
		src_stride[1] = src_stride[0] >> 1;
		src_stride[2] = src_stride[0] >> 1;
		srcVirtAddr[1] = srcVirtAddr[0] + src_stride[0] * iSrcHeight;
		srcVirtAddr[2] = srcVirtAddr[1] + src_stride[1] * (iSrcHeight >> 1);
		srcPhysAddr[1] = srcPhysAddr[0] + src_stride[0] * iSrcHeight;
		srcPhysAddr[2] = srcPhysAddr[1] + src_stride[1] * (iSrcHeight >> 1);
	}

	if (iDstFormat == FORMAT_YUV420PLANAR)
	{
		dst_stride[1] = dst_stride[0] >> 1;
		dst_stride[2] = dst_stride[0] >> 1;
		dstVirtAddr[1] = dstVirtAddr[0] + dst_stride[0] * iDstHeight;
		dstVirtAddr[2] = dstVirtAddr[1] + dst_stride[1] * (iDstHeight >> 1);
		dstPhysAddr[1] = dstPhysAddr[0] + dst_stride[0] * iDstHeight;
		dstPhysAddr[2] = dstPhysAddr[1] + dst_stride[1] * (iSrcHeight >> 1);
	}

	pSrc = (unsigned char*)srcVirtAddr[0];
	pDst = (unsigned char*)dstVirtAddr[0];

	if(!pSrc || !pDst){
		return -1;
	}

	while(!feof(pIn) && (i < 200))
	{
//		printf("Frame %d\n", i);
		i++;
		/*load one frame data*/
		if (iSrcFormat == FORMAT_YUV420PLANAR)
		{
			int t;

			for (t=0 ; t<iSrcHeight ; t++)
			{
				ret = fread(srcVirtAddr[0] + t * src_stride[0], 1, iSrcWidth, pIn);
				if(ret != iSrcWidth){
					printf("%s: Warning - fread \n", argv[0]);
				}
			}

			for (t=0 ; t<iSrcHeight/2 ; t++)
			{
				ret = fread(srcVirtAddr[1] + t * src_stride[1], 1, iSrcWidth/2, pIn);
				if(ret != iSrcWidth/2){
					printf("%s: Warning - fread \n", argv[0]);
				}
			}

			for (t=0 ; t<iSrcHeight/2 ; t++)
			{
				ret = fread(srcVirtAddr[2] + t * src_stride[2], 1, iSrcWidth/2, pIn);
				if(ret != iSrcWidth/2){
					printf("%s: Warning - fread \n", argv[0]);
				}
			}
		}
		else // read from UYVY format file
		{
			int t;

			for (t=0 ; t<iSrcHeight ; t++)
			{
				ret = fread(pSrc + t * src_stride[0], 1, iSrcWidth * 2, pIn);

				if(ret != iSrcWidth * 2){
					printf("%s: Warning - fread \n", argv[0]);
					break;
				}
			}
		}

		start_tick = _getTickCount();
		if (ippStsNoErr != ippiDeinterlaceResizeCsc_Process(srcVirtAddr, srcPhysAddr, src_stride, 0,
			dstVirtAddr, dstPhysAddr, dst_stride, 0, NULL, pDeinterlaceState))
		{
			printf("%s: Unexpected error in DeinterlaceResizeCsc_Process!\n", argv[0]);
			exit(-1);
		}
		if (ippStsNoErr != ippiDeinterlaceResizeCsc_Wait(NULL, pDeinterlaceState))
		{
			printf("%s: Unexpected error in DeinterlaceResizeCsc_Wait!\n", argv[0]);
			exit(-1);
		}
		total += (_getTickCount() - start_tick);

#if 1
		/*Save one plane data*/
		if (pOut == NULL)
			;
		else if ((iDstFormat == FORMAT_BGRA || iDstFormat == FORMAT_BGRA5551 || iDstFormat == FORMAT_BGR565))
		{
			ret = fwrite(dstVirtAddr[0], 1, iDstSize, pOut);
			fflush(pOut);
			if(ret != iDstSize){
				printf("%s: Warning - fwrite \n", argv[0]);
			}
		}
		else if ((iDstFormat == FORMAT_YUYVPACKED || iDstFormat == FORMAT_UYVYPACKED))
		{
			int t;

			for (t=0 ; t<iDstHeight ; t++)
			{
				ret = fwrite(dstVirtAddr[0] + t * dst_stride[0], 1, iDstWidth * 2, pOut);
				fflush(pOut);
				if(ret != iDstWidth * 2){
					printf("%s: Warning - fwrite \n", argv[0]);
				}
			}
		}
		else // if (iDstFormat == YUV420Planar)
		{
			int t;

			for (t=0 ; t<iDstHeight ; t++)
			{
				ret = fwrite(dstVirtAddr[0] + t * dst_stride[0], 1, iDstWidth, pOut);
				fflush(pOut);
				if(ret != iDstWidth){
					printf("%s: Warning - fwrite \n", argv[0]);
				}
			}

			for (t=0 ; t<iDstHeight/2 ; t++)
			{
				ret = fwrite(dstVirtAddr[1] + t * dst_stride[1], 1, iDstWidth/2, pOut);
				fflush(pOut);
				if(ret != iDstWidth/2){
					printf("%s: Warning - fwrite \n", argv[0]);
				}
			}

			for (t=0 ; t<iDstHeight/2 ; t++)
			{
				ret = fwrite(dstVirtAddr[2] + t * dst_stride[2], 1, iDstWidth/2, pOut);
				fflush(pOut);
				if(ret != iDstWidth/2){
					printf("%s: Warning - fwrite \n", argv[0]);
				}
			}
		}
#endif
	}

	printf("frame count = %d, elapse = %.2f sec, fps = %.2f\ninput = %.2f MPixel/sec\noutput = %.2f MPixel/sec\n",
		i, total / 1000000.0, 1000000.0 * i / total,
		iSrcWidth * iSrcHeight / 1.024 / 1.024 * i / total,
		iDstWidth * iDstHeight / 1.024 / 1.024 * i / total);

	ippiDeinterlaceResizeCsc_Free(&pDeinterlaceState);
	miscFreeGeneralCallbackTable(&pCallbackTable);

	if(pIn){
		fclose(pIn);
	}
	if(pOut){
		fclose(pOut);
	}

	if(pSrc){
		bmm_free(pSrc);
	}

	if(pDst){
		bmm_free(pDst);
	}

	gcuTerminate();			// gcuTerminate doesn't support multi-threading, hence we should make sure no others will use it before calling it.

	return 0;
}




/*EOF*/
