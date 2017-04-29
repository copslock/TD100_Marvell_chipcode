/** @file mathUtility.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include <math.h>

#ifdef __cplusplus
#define MATHCALL extern "C" 
#else
#define MATHCALL extern  
#endif

#ifndef _LINUX_
MATHCALL double round(double x);
#else 
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif //#ifndef _LINUX_


MATHCALL double maxArray32(int n, double *a); 

MATHCALL int clip(double *x, double max, double min);
MATHCALL int clipInt(int *x, int max, int min);
MATHCALL int clipChar(char *x, int max, int min);


MATHCALL void InterExterPChars(int NumOfData, const   char IndexArray[], 
				 const   char Data1[], const   char Data2[], 
				 const   char Data3[], const   char Data4[], 
				int NumOfListData, const   char IndexList[], 
				  char List1[],   char List2[], 
				  char List3[],   char List4[]);

MATHCALL void InterExterPIntChars(int NumOfData, const   int IndexArray[], 
				 const   char Data1[], const   char Data2[], 
				 const   char Data3[], const   char Data4[], 
				int NumOfListData, const   int IndexList[], 
				  char List1[],   char List2[], 
				  char List3[],   char List4[]);

#ifndef _OAMP_
MATHCALL void Sort(int NumOfData, int IndexArray[], 
				   int Data1[], int Data2[], int Data3[], int Data4[]);
MATHCALL void SortD(int NumOfData, int IndexArray[], 
				   int Data1[], int Data2[], int Data3[], 
				   int Data4[], int Data5[], int Data6[]);

MATHCALL void SortCharEx(int NumOfData, unsigned char IndexArray[], 
			  char Data1[], char Data2[], char Data3[], char Data4[],
			  char Data5[], char Data6[], char Data7[], char Data8[]);

MATHCALL void InterExterPChars(int NumOfData, const   char IndexArray[], 
				 const   char Data1[], const   char Data2[], 
				 const   char Data3[], const   char Data4[], 
				int NumOfListData, const   char IndexList[], 
				  char List1[],   char List2[], 
				  char List3[],   char List4[]);
#else
MATHCALL void Sort(int NumOfData, int *IndexArray, 
				   int *Data1, int *Data2, int *Data3, int *Data4);
#endif

/*
MATHCALL void floatInterExterP(int NumOfData, const int IndexArray[], const double FreqArray[],
				 const int Data1[], const int Data2[], const int Data3[], const int Data4[], 
				int NumOfListData, int IndexList[], double FreqListArray[],
				unsigned char List1[], unsigned char List2[], unsigned char List3[], unsigned char List4[]);
*/

#ifndef _OAMP_
MATHCALL void InterExterPChars(int NumOfData, const   char IndexArray[], 
				 const   char Data1[], const   char Data2[], 
				 const   char Data3[], const   char Data4[], 
				int NumOfListData, const   char IndexList[], 
				  char List1[],   char List2[], 
				  char List3[],   char List4[]);
#else
MATHCALL void InterExterPChars(int NumOfData, const   char *IndexArray, 
				 const   char *Data1, const   char *Data2, 
				 const   char *Data3, const   char *Data4, 
				int NumOfListData, const   char *IndexList, 
				  char *List1,   char *List2, 
				  char *List3,   char *List4);
#endif


MATHCALL int Poly2PeakEstmation(int NumData, double x[], double y[], 
								double *estX, double *estY, double *errR2 );
MATHCALL int Poly2PeakEstmationInt(int NumData, int x[], double y[], 
								double *estX, double *estY, double *errR2 );

MATHCALL void dB2Linear(int NumData, double x[]);

MATHCALL void Linear2dB(int NumData, double x[]);

MATHCALL int AverageInt(int NumData, int  Data[]);

MATHCALL double AverageDouble(int NumData, double  Data[]);
