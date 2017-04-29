/** @file mathUtility.c
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */
#include "Clss_os.h"
#include "mathUtility.h"  

#ifndef _LINUX_
double round(double x)
{

	if(fabs(floor(x)-x) > fabs(ceil(x)-x) )
		return (double) ceil(x);
	else
		return (double) floor(x);

}
#else
double round (double x)
{
	int a1, a2;
	if (x>=0)
	{
		a1 = (int) x;
		a2 = a1 + 1;
	}
	else
	{
		a2 = (int) x;
		a1 = a2 + 1;
	}

	if((x-a1) > (a2 - x))
		return a2;
	else
		return a1;
	
}
#endif //#ifndef _LINUX_


MATHCALL int clip(double *x, double max, double min)
{
	if(min > max) return -1;
	
	if((*x) > max) (*x)=max;
	if((*x) < min) (*x)=min;
	
	return 0; 
}
MATHCALL int clipInt(int *x, int max, int min) 
{
	if(min > max) return -1;
	
	if((*x) > max) (*x)=max;
	if((*x) < min) (*x)=min;
	
	return 0; 
}

MATHCALL int clipChar(char *x, int max, int min)
{
	if(min > max) return -1;
	
	if((*x) > max) (*x)=max;
	if((*x) < min) (*x)=min;
	
	return 0; 
}


#ifndef _OAMP_
void InterExterPChars(int NumOfData,	const char IndexArray[], 
					const char Data1[], const char Data2[], 
					const char Data3[], const char Data4[], 
					int NumOfListData,	const char IndexList[], 
					char List1[],		char List2[], 
					char List3[],		char List4[])
#else
void InterExterPChars(int NumOfData,	const char *IndexArray, 
					const char *Data1, const char *Data2, 
					const char *Data3, const char *Data4, 
					int NumOfListData,	const char *IndexList, 
					char *List1,		char *List2, 
					char *List3,		char *List4)
#endif
{
	int indexList=0, indexData=0;
	double slope=0.0;


	for (indexList=0; indexList < NumOfListData; indexList++)
	{
		for (indexData=0; indexData < NumOfData; indexData++)
		{

			if(IndexList[indexList] < IndexArray[0])
			{
				slope=1.0*(IndexArray[0]-IndexList[indexList])/(IndexArray[0]-IndexArray[1]); 
				if(List1 && Data1) List1[indexList]=(  char)round(Data1[0]-(Data1[0]-Data1[1])*slope);
				if(List2 && Data2) List2[indexList]=(  char)round(Data2[0]-(Data2[0]-Data2[1])*slope);
				if(List3 && Data3) List3[indexList]=(  char)round(Data3[0]-(Data3[0]-Data3[1])*slope);
				if(List4 && Data4) List4[indexList]=(  char)round(Data4[0]-(Data3[0]-Data4[1])*slope);
			break;
			}

			if(IndexList[indexList] > IndexArray[NumOfData-1])
			{
				slope=1.0*(IndexArray[NumOfData-1]-IndexList[indexList])/(IndexArray[NumOfData-1]-IndexArray[NumOfData-2]); 
				if(List1 && Data1) List1[indexList]=(  char)round(Data1[NumOfData-1]-(Data1[NumOfData-1]-Data1[NumOfData-2])*slope);
				if(List2 && Data2) List2[indexList]=(  char)round(Data2[NumOfData-1]-(Data2[NumOfData-1]-Data2[NumOfData-2])*slope);
				if(List3 && Data3) List3[indexList]=(  char)round(Data3[NumOfData-1]-(Data3[NumOfData-1]-Data3[NumOfData-2])*slope);
				if(List4 && Data4) List4[indexList]=(  char)round(Data4[NumOfData-1]-(Data4[NumOfData-1]-Data4[NumOfData-2])*slope);
				break;
			}
			
			if (IndexList[indexList] == IndexArray[indexData])
			{
				if(List1 && Data1) List1[indexList]=Data1[indexData];
				if(List2 && Data2) List2[indexList]=Data2[indexData];
				if(List3 && Data3) List3[indexList]=Data3[indexData];
				if(List4 && Data4) List4[indexList]=Data4[indexData];
				break;
			}

			
			if (IndexList[indexList] > IndexArray[indexData-1] && 
				IndexList[indexList] < IndexArray[indexData] )
			{
				slope=1.0*(IndexArray[indexData-1]-IndexList[indexList])/(IndexArray[indexData-1]-IndexArray[indexData]); 
 		 		if(List1 && Data1) List1[indexList]=(  char)round(Data1[indexData-1]-(Data1[indexData-1]-Data1[indexData])*slope);
				if(List2 && Data2) List2[indexList]=(  char)round(Data2[indexData-1]-(Data2[indexData-1]-Data2[indexData])*slope);
				if(List3 && Data3) List3[indexList]=(  char)round(Data3[indexData-1]-(Data3[indexData-1]-Data3[indexData])*slope);
				if(List4 && Data4) List4[indexList]=(  char)round(Data4[indexData-1]-(Data4[indexData-1]-Data4[indexData])*slope);
				break;
			}

		}

	}

}


#ifndef _OAMP_
void Sort(int NumOfData, int IndexArray[], int Data1[], int Data2[], int Data3[], int Data4[])
#else
void Sort(int NumOfData, int *IndexArray, int *Data1, int *Data2, int *Data3, int *Data4)
#endif
{
	int indexSmall=0, indexData=0;
	int tempInt=0; 

	for (indexData=0; indexData < NumOfData; indexData++)
	{
		for (indexSmall=0; indexSmall < NumOfData; indexSmall++)
			if(IndexArray[indexData] < IndexArray[indexSmall])
			{
				// swape data and index
				tempInt = IndexArray[indexData];
				IndexArray[indexData] = IndexArray[indexSmall];
				IndexArray[indexSmall] = tempInt;

				if(Data1)
				{
					tempInt = Data1[indexData];
					Data1[indexData] = Data1[indexSmall];
					Data1[indexSmall] = tempInt;
				}
				if(Data2)
				{
					tempInt = Data2[indexData];
					Data2[indexData] = Data2[indexSmall];
					Data2[indexSmall] = tempInt;
				}
				if(Data3)
				{
					tempInt = Data3[indexData];
					Data3[indexData] = Data3[indexSmall];
					Data3[indexSmall] = tempInt;
				}
				if(Data4)
				{
					tempInt = Data4[indexData];
					Data4[indexData] = Data4[indexSmall];
					Data4[indexSmall] = tempInt;
				}
			}
	}

}


void SortD(int NumOfData, int IndexArray[], int Data1[], int Data2[], int Data3[], 
				int Data4[], int Data5[], int Data6[])
{
	int indexSmall=0, indexData=0;
	int tempInt=0; 

	for (indexData=0; indexData < NumOfData; indexData++)
	{
		for (indexSmall=0; indexSmall < NumOfData; indexSmall++)
		{
			if(IndexArray[indexData] > IndexArray[indexSmall])
			{
				// swape data and index
				tempInt = IndexArray[indexData];
				IndexArray[indexData] = IndexArray[indexSmall];
				IndexArray[indexSmall] = tempInt;

				if(Data1)
				{
					tempInt = Data1[indexData];
					Data1[indexData] = Data1[indexSmall];
					Data1[indexSmall] = tempInt;
				}
				if(Data2)
				{
					tempInt = Data2[indexData];
					Data2[indexData] = Data2[indexSmall];
					Data2[indexSmall] = tempInt;
				}
				if(Data3)
				{
					tempInt = Data3[indexData];
					Data3[indexData] = Data3[indexSmall];
					Data3[indexSmall] = tempInt;
				}
				if(Data4)
				{
					tempInt = Data4[indexData];
					Data4[indexData] = Data4[indexSmall];
					Data4[indexSmall] = tempInt;
				}
				if(Data5)
				{
					tempInt = Data5[indexData];
					Data5[indexData] = Data5[indexSmall];
					Data5[indexSmall] = tempInt;
				}
				if(Data6)
				{
					tempInt = Data6[indexData];
					Data6[indexData] = Data6[indexSmall];
					Data6[indexSmall] = tempInt;
				}
			}
		}
	}

}

void SortCharEx(int NumOfData, unsigned char IndexArray[], 
			  char Data1[], char Data2[], char Data3[], char Data4[],
			  char Data5[], char Data6[], char Data7[], char Data8[])
{
	int indexSmall=0, indexData=0;
	char tempChar=0; 

	if(NumOfData <=1) return ;

	for (indexData=0; indexData < NumOfData; indexData++)
	{
		for (indexSmall=0; indexSmall < NumOfData; indexSmall++)
			if(IndexArray[indexData] < IndexArray[indexSmall])
			{
				// swape data and index
				tempChar = IndexArray[indexData];
				IndexArray[indexData] = IndexArray[indexSmall];
				IndexArray[indexSmall] = tempChar;

				if(Data1)
				{
					tempChar = Data1[indexData];
					Data1[indexData] = Data1[indexSmall];
					Data1[indexSmall] = tempChar;
				}
				if(Data2)
				{
					tempChar = Data2[indexData];
					Data2[indexData] = Data2[indexSmall];
					Data2[indexSmall] = tempChar;
				}
				if(Data3)
				{
					tempChar = Data3[indexData];
					Data3[indexData] = Data3[indexSmall];
					Data3[indexSmall] = tempChar;
				}
				if(Data4)				
				{
					tempChar = Data4[indexData];
					Data4[indexData] = Data4[indexSmall];
					Data4[indexSmall] = tempChar;
				}
				if(Data5)
				{
					tempChar = Data5[indexData];
					Data5[indexData] = Data5[indexSmall];
					Data5[indexSmall] = tempChar;
				}
				if(Data6)
				{					
					tempChar = Data6[indexData];
					Data6[indexData] = Data6[indexSmall];
					Data6[indexSmall] = tempChar;
				}
				if(Data7)
				{
					tempChar = Data7[indexData];
					Data7[indexData] = Data7[indexSmall];
					Data7[indexSmall] = tempChar;
				}
				if(Data8)
				{
					tempChar = Data8[indexData];
					Data8[indexData] = Data8[indexSmall];
					Data8[indexSmall] = tempChar;
				}
			}
	}

}


int polyfit2nd(int NumData, double x[], double y[], 
			   double *estA, double *estB, double *estC, double *errR2)
{
	int index=0;
	double Sum1=0, SumX=0, SumX2=0, SumX3=0, SumX4=0;
	double SumY=0, SumXY=0, SumX2Y=0;

	double V1=0, V2=0, V3=0;
	double MatrA=0, MatrB=0, MatrC=0, MatrD=0, MatrE=0, MatrF=0, MatrG=0, MatrH=0, MatrI=0;
	double DetM=0; 
	double InvtA=0, InvtB=0, InvtC=0, InvtD=0, InvtE=0, InvtF=0, InvtG=0, InvtH=0, InvtI=0;
	double estAlcl=0, estBlcl=0, estClcl=0, Ylcl=0, R2=0;

	for (index=0; index<NumData; index++)
	{
		Sum1 +=1;
		SumX +=x[index];
		SumX2 +=pow(x[index], 2);
		SumX3 +=pow(x[index], 3);
		SumX4 +=pow(x[index], 4);
		SumY +=y[index];
		SumXY +=(x[index]*y[index]);
		SumX2Y +=(pow(x[index], 2)*y[index]);
	}

// form matrix and vector
//	y=ax2+bx+c
//	SumY	=a*SumX2 + b*SumX	+c*Sum1;
//	SumXY	=a*SumX3 + b*SumX2	+c*SumX;
//	SumX2Y	=a*SumX4 + b*SumX3	+c*SumX2;
//	SumY		SumX2 + SumX	+Sum1		a		| V1	MatrA MatrB MatrC		estA	
//	SumXY	=	SumX3 + SumX2	+SumX	*	b		| V2 =	MatrD MatrE MatrF	*	estB
//	SumX2Y		SumX4 + SumX3	+SumX2		c		| V3	MatrG MatrH MatrI		estC

	V1 = SumY;
	V2 = SumXY;
	V3 = SumX2Y;
	MatrA = MatrE = MatrI = SumX2;
	MatrB = MatrF = SumX;
	MatrC = Sum1;
	MatrD = MatrH = SumX3;
	MatrG = SumX4;

// cal DetM
	DetM = MatrA*(MatrE*MatrI-MatrF*MatrH) 
			- MatrB*(MatrD*MatrI-MatrF*MatrG) 
			+ MatrC*(MatrD*MatrH-MatrE*MatrG);

	if(0==DetM) return -1;

	InvtA = (MatrE*MatrI-MatrF*MatrH)/DetM;
	InvtB = (MatrC*MatrH-MatrB*MatrI)/DetM;
	InvtC = (MatrB*MatrF-MatrC*MatrE)/DetM;
	InvtD = (MatrF*MatrG-MatrD*MatrI)/DetM;
	InvtE = (MatrA*MatrI-MatrC*MatrG)/DetM;
	InvtF =	(MatrC*MatrD-MatrA*MatrF)/DetM;
	InvtG = (MatrD*MatrH-MatrE*MatrG)/DetM;
	InvtH = (MatrB*MatrG-MatrA*MatrH)/DetM;
	InvtI = (MatrA*MatrE-MatrB*MatrD)/DetM;

// estA		InvtA InvtB InvtC		V1 	
// estB =	InvtD InvtE InvtF	*	V2 
// estC		InvtG InvtH InvtI		V3 

	estAlcl = InvtA * V1 +InvtB * V2 +InvtC * V3;
	estBlcl = InvtD * V1 +InvtE * V2 +InvtF * V3;
	estClcl = InvtG * V1 +InvtH * V2 +InvtI * V3;

	R2=0;
 	for (index=0; index<NumData; index++)
	{	
		Ylcl= estAlcl*x[index]*x[index] + estBlcl * x[index] + estClcl;
		R2 += pow((y[index]-Ylcl), 2);
	}
//		printf("R2 + %e\n", R2);

	if(estA) *estA = estAlcl;
	if(estB) *estB = estBlcl;
	if(estC) *estC = estClcl;
	if(errR2) *errR2 = R2; 

	return 0;
}

int Poly2PeakEstmation(int NumData, double x[], double y[], 
					   double *estX, double *estY, double *errR2)
{
	int err=0;
	double coeffA=0, coeffB=0, coeffC=0;
	double PeakX=0, PeakY=0;

	err = polyfit2nd(NumData, x, y, 
			   &coeffA, &coeffB, &coeffC, errR2);
	if(err) return err;
	if(0 == coeffA) return -1;

//	printf("a=%e b=%e c=%e\n", coeffA, coeffB, coeffC);

	PeakX=-coeffB/(2*coeffA);
	PeakY = coeffA*pow(PeakX, 2) + coeffB*PeakX + coeffC;

	if(estX) *estX = PeakX;
	if(estY) *estY = PeakY;
	return 0; 
}

int Poly2PeakEstmationInt(int NumData, int x[], double y[], 
					   double *estX, double *estY, double *errR2)
{	
	int i=0, err=0;
	double *xDouble=0;
	
	xDouble = (double*) malloc(sizeof(double)*NumData);
	for (i=0; i<NumData; i++)
	{
		*(xDouble+i) = x[i];
	}
	err = Poly2PeakEstmation(NumData, xDouble, y, 
					    estX, estY, errR2);
	free(xDouble);

	return err;

}

void dB2Linear(int NumData, double x[])
{
	int index=0;

	for (index=0; index <NumData; index++)
		x[index] = pow(10, x[index]/10);

	return;
}

void Linear2dB(int NumData, double x[])
{
	int index=0;

	for (index=0; index <NumData; index++)
		x[index] = 10*log10(x[index]);

	return;
}


MATHCALL double maxArray32(int n, double *a) 
{
	double b = a[0];
	 int i=0;

	for(i=1; i<n; i++) 
	{
		if(b<a[i]) 
			b=a[i];
	}  
	return b;
}
