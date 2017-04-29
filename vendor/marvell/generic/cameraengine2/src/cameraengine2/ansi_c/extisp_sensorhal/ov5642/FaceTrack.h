/*============================================================================
 Name		: FaceTrack.h
 Version	 : 1.2
 Copyright   : 
 Description : 
        A simple wrapper to use the Face Tracking algorithm 
 History    : 
            V1.0    first draft 	
            V1.2    add setting parameter in FaceTrackOpen()			
 ============================================================================*/

#ifndef FACETRACK_H
#define FACETRACK_H


#define FACEPROCESSBUFSIZE (16*161*121+160*120) /*pre-allocated buffer for image process*/


#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int TUint32;
typedef unsigned int TUint;
typedef unsigned char TUint8;
typedef int 	TInt;

/*=========================================================================
  Type definition of point
=========================================================================*/
typedef struct TPoint {
	int iX;
	int iY;
} TPoint;

/*=========================================================================
  Type definition of rectangle
=========================================================================*/
typedef struct TRect {
	TPoint iTl;
	TPoint iBr;
}TRect;

/*=========================================================================
  Type definition of callback function
=========================================================================*/
typedef unsigned int (*app_callback)(unsigned int);


/*=========================================================================
  TrackInFrame return value 
=========================================================================*/
enum {
    StatusSuccess=0,
    StatusFailure,
	  StatusNoFace,
	  StatusErrCallback,
	  StatusErrNotReady
};

/*=========================================================================
  Function
	int FaceTrackOpen(unsigned char *pProcessBuf, app_callback callback);

  Description
	Ininialize parameters and allocate buffers for face tracking algorithm..

  Parameters
	pProcessBuf: the pointer to a pre-allocate process buffer.
	callback: the function pointer of callback     
	setting :	face detectionconfigure 
            [3:0]  faces detect number, max is 9
            [4]    face detect orietation, 0:  three directions( upside, left and down), 1: eight directions
            [5]    result sort order, 0: by face size, 1: by distance to center  
                              
  Return
	0: success
	1: failure
=========================================================================*/
int FaceTrackOpen(unsigned char *pProcessBuf, app_callback callback, unsigned int setting);

/*=========================================================================
  Function
	void FaceTrackClose(void);

  Description
	un-init, free allocated memory.

  Parameters
	NULL

  Return
    NULL
=========================================================================*/
void FaceTrackClose(void);

/*=========================================================================
  Function
	int TrackInFrame(TUint8 *pData, TUint32 width, TUint32 height);

  Description
	Run face tracking algorithm, detect face positions and change the global 
        variables iFaceNum and iFaceArea

  Parameters
	pData: the pointer to the face detection image buffer(Y format).
	width: image buffer width (fixed to 160)
	height: image buffer height (fixed to 120)

  Return
    StatusSuccess=0,
    StatusFailure,
	StatusNoFace,
	StatusErrCallback, 
	StatusErrNotReady
=========================================================================*/
int TrackInFrame(TUint8 *pData, TUint32 width, TUint32 height);

extern 	TInt iFaceNum;           // detected face number           
extern	TRect iFaceArea[];       // detected face position data (160*120 size)

//auto draw face position, default  is closed
extern unsigned int autoDrawFace ;

int test_add(int a, int b);


#ifdef __cplusplus
}
#endif

#endif // FACETRACK_H
