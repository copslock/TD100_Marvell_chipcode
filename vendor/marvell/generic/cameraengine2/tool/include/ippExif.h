/******************************************************************************
//(C) Copyright [2011] Marvell International Ltd.
//All Rights Reserved
******************************************************************************/

#ifndef _IPP_EXIF_H_
#define _IPP_EXIF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ippdefs.h"

#define MAX_IMAGE_NAME         256
#define MAX_MAKE_NAME          256
#define MAX_MODEL_NAME         256
#define MAX_ISO_SPEED_RATE     256
#define MAX_SOFTWARE_LEN       256
#define MAX_MAPDATUM_LEN       256
#define TIME_DEFAULT_LEN       20

/* simply helping macro to help user to assign exif info */

#define exif_fourcc(a,b,c,d)\
	( (Ipp32s)(a) | ( (Ipp32s)(b) << 8 ) | ( (Ipp32s)(c) << 16 ) | ( (Ipp32s)(d) << 24 ) )

/*
// convert decimals to Rational type 
// 0xFFFFFFFF usually has special meaning 
*/
#define  ToRational( num, stRational)\
{\
	Ipp32u i = 0;\
	if ( num >= 0 )\
	{\
		if ( num < 1 )\
		{\
			stRational.denominal = (~i) >> 1;\
			stRational.numerator = (Ipp32u)( num * stRational.denominal );\
		}\
		else\
		{\
			stRational.numerator = (~i) >> 1;\
			stRational.denominal = (Ipp32u)( stRational.numerator/num );\
		}\
	}\
}

/* convert decimals num,to SRational type */
#define  ToSRational( num, stSRational)\
{\
	Ipp32u i = 0;\
	if ( abs(num) < 1 )\
	{\
		stSRational.idenominal = (~i) >> 1;\
		stSRational.inumerator = (Ipp32s)( num * stSRational.idenominal );\
	}\
	else\
	{\
		stSRational.inumerator = (~i) >> 1;\
		stSRational.idenominal = (Ipp32s)( stSRational.inumerator / num );\
	}\
}

/* convert decimalism num to sexagesimal type  used for GPS data convertion, eg 39.20567->39 degree 12'22''*/
#define Decimalto60( num, pRational )\
{\
	pRational[0].numerator = (Ipp32u)num;\
	pRational[0].denominal = 1;\
	pRational[1].numerator = (Ipp32u)(( num - pRational[0].numerator ) * 60 );\
	pRational[1].denominal = 1;\
	pRational[2].numerator = (Ipp32u)((( num - pRational[0].numerator ) * 60 - pRational[1].numerator ) * 60 );\
	pRational[2].denominal = 1;\
}

/* convert to time data format defined in Exif spec 2.2  "YYYY:MM:DD HH:MM:SS" */
#define ConvertTime( year, month, day, hour, minite, second, date )\
	{\
		int MonthValid[] = { 0, 31, -1, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };\
		memset( date, 0x20, 20 );\
		if ( year >= 0 )\
		{\
			date[0] = '0' + year / 1000;\
			date[1] = '0' + year / 100 % 10;\
			date[2] = '0' + year / 10 % 10;\
			date[3] = '0' + year % 10;\
			if ( ( year % 4 == 0 && year % 100 != 0 ) || year % 400 == 0 )\
			{\
				MonthValid[2] = 29;\
			}\
			else\
			{\
				MonthValid[2] = 28;\
			}\
		}\
		date[4] = ':';\
		if( 1 <= month && month <= 12 )\
		{\
			date[5] = '0' + month / 10;\
			date[6] = '0' + month % 10;\
		}\
		date[7] = ':';\
		if ( year < 0 || month > 12 || month < 1)\
		{\
			MonthValid[2] = 29;\
		}\
		if ( 1<= day && day <= MonthValid[month] )\
		{\
			date[8] = '0' + day / 10;\
			date[9] = '0' + day % 10;\
		}\
		if ( 0 <= hour && hour <= 23 )\
		{\
			date[11] = '0' + hour / 10;\
			date[12] = '0' + hour % 10;\
		}\
		date[13] = ':';\
		if ( 0 <= minite && minite <= 59 )\
		{\
			date[14] = '0' + minite / 10;\
			date[15] = '0' + minite % 10;\
		}\
			date[16] = ':';\
		if ( 0 <= second && second <= 59 )\
		{\
			date[17] = '0' + second / 10;\
			date[18] = '0' + second % 10;\
		}\
		date[19] = '\0';\
		}

/* enumerations, these enumerations are all defined according to EXIF2.2 spec */
typedef enum
{
	ippExifOrientationNormal                    = 1,
	ippExifOrientationHFLIP                     = 2,
	ippExifOrientation180                       = 3,
	ippExifOrientationVFLIP                     = 4,
	ippExifOrientation90L                       = 5,
	ippExifOrientation90R                       = 6,
	ippExifOrientationADFLIP                    = 7,
	ippExifOrientationDFLIP                     = 8,
	ippExifOrientationMax                       = 0x7FFFFFFF,
} IppExifOrientation;

typedef enum
{
	ippExifYCbCrPosCentral                      = 1,
	ippExifYCbCrPosCoSited                      = 2,
	ippExifYCbCrPosMax                          = 0x7FFFFFFF,
} IppExifYCbCrPosition;

typedef enum
{
	ippExifCompressionUnCompressed              = 1,
	ippExifCompressionThumbnailJPEG             = 6,
	ippExifCompressionMax                       = 0x7FFFFFFF,
} IppExifCompression;

typedef enum
{
	ippExifPlanarConfigChunky                   = 1,
	ippExifPlanarConfigPlanar                   = 2,
	ippExifPlanarConfigMax                      = 0x7FFFFFFF,
} IppExifPlanarConfig;

typedef enum
{
	ippExifResolutionUnitInch                   = 2,
	ippExifResolutionUnitCentimeters            = 3,
	ippExifResolutionUnitMax                    = 0x7FFFFFFF,
} IppExifResolutinUnit;

typedef enum
{
	ippExifComponentConfigRGB                   = 0,
	ippExifComponentConfigYCbCr                 = 1,
	ippExifComponentConfigMax                   = 0x7FFFFFFF,
} IppExifComponentConfig;

typedef enum
{
	ippExifWhiteBalanceAuto                     = 0,
	ippExifWhiteBalanceManual                   = 1,
	ippExifWhiteBalanceMax                      = 0x7FFFFFFF,
} IppExifWhiteBalance;

typedef enum
{
	ippExifExposurePragramNoDef               = 0,
	ippExifExposurePragramManual              = 1,
	ippExifExposurePragramNomal               = 2,
	ippExifExposurePragramAperturePrior       = 3,
	ippExifExposurePragramShutterPrior        = 4,
	ippExifExposurePragramCreativeProgram     = 5,
	ippExifExposurePragramActionProgram       = 6,
	ippExifExposurePragramPortraitMode        = 7,
	ippExifExposurePragramLandscapeMode       = 8,
	ippExifExposurePragramMax                 = 0x7FFFFFFF,
} IppExifExposurePragram;

typedef enum
{
	ippExifLightSourceUnKnown                 = 0,
	ippExifLightSourceDayLight                = 1,
	ippExifLightSourceFluorescent             = 2,
	ippExifLightSourceTungesten               = 3,
	ippExifLightSourceFlash                   = 4,
	ippExifLightSourceFineWeather             = 9,
	ippExifLightSourceCloudyWeather           = 10,
	ippExifLightSourceShade                   = 11,
	ippExifLightSourceDayLightFluorescent     = 12,
	ippExifLightSourceDayWhiteFluorescent     = 13,
	ippExifLightSourceCoolLightFluorescent    = 14,
	ippExifLightSourceWhiteFluorescent        = 15,
	ippExifLightSourceStandardLightA          = 17,
	ippExifLightSourceStandardLightB          = 18,
	ippExifLightSourceStandardLightC          = 19,
	ippExifLightSourceD55                     = 20,
	ippExifLightSourceD65                     = 21,
	ippExifLightSourceD75                     = 22,
	ippExifLightSourceD50                     = 23,
	ippExifLightSourceISO                     = 24,
	ippExifLightSourceOther                   = 255,
	ippExifLightSourceMax                     = 0x7FFFFFFF,
} IppExifLightSource;

typedef enum
{
	ippExifExposureModeAutoExposure           = 0,
	ippExifExposureModeManual                 = 1,
	ippExifExposureModeAutoBracket            = 2,
	ippExifExposureModeMax                    = 0x7FFFFFFF,
} IppExifExposureMode;

typedef enum
{
	ippExifColorSpacesRGB                     = 1,
	ippExifColorSpaceUnCalibrated             = 0xFFFF,
	ippExifColorSpaceMax                      = 0x7FFFFFFF,
}IppExifColorSpace;

typedef enum
{
	ippExifGPSLatituteRefNorth                = 'N',
	ippExifGPSLatituteRefSouth                = 'S',
	ippExifGPSLatituteMax                     = 0x7FFFFFFF,
} IppExifGPSLatituteRef;

typedef enum
{
	ippExifGPSLongtituteRefEast               = 'E',
	ippExifGPSLongtituteRefWest               = 'W',
	ippExifGPSLongtituteMax                   = 0x7FFFFFFF,
} IppExifGPSLongtituteRef;

typedef enum
{
	ippExifPhotometricInterpretationRGB       = 2,
	ippExifPhotometricInterpretationYCC       = 6,
	ippExifPhotometricInterpretationMax       = 0x7FFFFFFF,
} IppExifPhotometricInterpretation;

typedef enum 
{
	ippExifYCCSubSampling422                  = 0,
	ippExifYCCSubSampling420                  = 1,
	ippExifYCCSubSamplingMax                  = 0x7FFFFFFF,
} IppExifYCCSubSampling;

typedef enum 
{
	ippExifExposureProgramNotDefined          = 0,
	ippExifExposureProgramManual              = 1,
	ippExifExposureProgramNormal              = 2,
	ippExifExposureProgramAperturePriority    = 3,
	ippExifExposureProgramShutterPriority     = 4,
	ippExifExposureProgramCreative            = 5,
	ippExifExposureProgramAction              = 6,
	ippExifExposureProgramPortrait            = 7,
	ippExifExposureProgramLandscape           = 8,
	ippExifExposureProgramMax                 = 0x7FFFFFFF,
} IppExifExposureProgram;

typedef enum 
{
	ippExifMeteringModeUnknown                = 0,
	ippExifMeteringModeAverage                = 1,
	ippExifMeteringModeCenterWeightedAverage  = 2,
	ippExifMeteringModeSpot                   = 3,
	ippExifMeteringModeMultiSpot              = 4,
	ippExifMeteringModePattern                = 5,
	ippExifMeteringModePartial                = 6,
	ippExifMeteringModeOther                  = 255,
	ippExifMeteringModeMax                    = 0x7FFFFFFF,
} IppExifMeteringMode;

typedef enum 
{
	ippExifContrastNormal                     = 0,
	ippExifContrastSoft                       = 1,
	ippExifContrastHard                       = 2,
	ippExifContrastMax                        = 0x7FFFFFFF,
} IppExifContrast;

typedef enum 
{
	ippExifSaturationNormal                   = 0,
	ippExifSaturationLow                      = 1,
	ippExifSaturationHigh                     = 2,
	ippExifSaturationMax                      = 0x7FFFFFFF,
} IppExifSaturation;

typedef enum 
{
	ippExifSceneCaptureTypeStandard           = 0,  // default if exist
	ippExifSceneCaptureTypeLandscape          = 1,
	ippExifSceneCaptureTypePortrait           = 2,
	ippExifSceneCaptureTypeNightScene         = 3,
	ippExifSceneCaptureTypeMax                = 0x7FFFFFFF,
} IppExifSceneCaptureType;

typedef enum 
{
	ippExifSharpnessNormal                    = 0,
	ippExifSharpnessSoft                      = 1,
	ippExifSharpnessHard                      = 2,
	ippExifSharpnessMax                       = 0x7FFFFFFF,
} IppExifSharpness;

typedef enum 
{
	ippExifSDRUnknow                          = 0,
	ippExifSDRMacro                           = 1,
	ippExifSDRCloseView                       = 2,
	ippExifSDRDistantView                     = 3,
	ippExifSubjectDistanceRangeMax            = 0x7FFFFFFF,
} IppExifSubjectDistanceRange;

typedef enum 
{
	ippExifSMNotDefined                       = 1,   // Not defined
	ippExifSMOneSensor                        = 2,   // one-chip color area sensor
	ippExifSMTwoSensor                        = 3,   // two-chip color area sensor
	ippExifSMThreeSensor                      = 4,   // three-chip color area sensor
	ippExifSMColorSequentialAreaSensor        = 5,   // Color Sequential Area Sensor
	ippExifSMTrilinearSensor                  = 7,   // Trilinear Sensor
	ippExifSMColorSequentialLinearSensor      = 8,   // Color Sequential Linear Sensor
	ippExifSensingMethodMax                   = 0x7FFFFFFF,
} IppExifSensingMethod;

typedef enum 
{
	ippExifGainControlNone                    = 0,
	ippExifGainControlLowGainUp               = 1,
	ippExifGainControlHighGainUp              = 2,
	ippExifGainControlLowGainDown             = 3,
	ippExifGainControlHighGainDown            = 4,
	ippExifGainControlMax                     = 0x7FFFFFFF,
} IppExifGainControl;

typedef enum 
{
	ippExifCustomRenderedNormal               = 0,
	ippExifCustomRenderedCustom               = 1,
	ippExifCustomRenderedMax                  = 0x7FFFFFFF,
} IppExifCustomRendered;

typedef enum 
{
	ippExifSceneTypeDirectly                  = 1,  // indicate the image was directly photographed
	ippExifSceneTypeMax                       = 0x7FFFFFFF,
} IppExifSceneType;

typedef enum 
{
	ippExifFileSourceDirectly                  = 3,  // indicate the image was recordered on a DSC
	ippExifFileSourceMax                       = 0x7FFFFFFF,
} IppExifFileSource;

typedef enum 
{
	ippExifFlashNotFired                      = 0x00,   // Flash did not fired
	ippExifFlashFired                         = 0x01,   // Flash fired
	ippExifFlashLightNotDetected              = 0x05,   // Strobe return light not detected (SRLND)
	ippExifFlashLightDetected                 = 0x07,   // strobe return light detected (SRLD)
	ippExifFlashFired_CFMODE                  = 0x09,   // Flash fired, compulsory flash mode (CFMODE)
	ippExifFlashFired_CFMODE_SRLND            = 0x0D,   // Flash fired, compulsory flash mode (CFMODE), return light not detected (SRLND)
	ippExifFlashFired_CFMODE_SRLD             = 0x0F,   // Flash fired, compulsory flash mode (CFMODE), return light detected (SRLD)
	ippExifFlashNotFired_CFMODE               = 0x10,   // Flash did not fired, compulsory flash mode (CFMODE)
	ippExifFlashNotFired_ATMODE               = 0x18,   // Flash did not fired, auto mode (ATMODE)
	ippExifFlashFired_ATMODE                  = 0x19,   // Flash fired, auto mode (ATMODE)
	ippExifFlashNotFired_ATMODE_SRLND         = 0x1D,   // Flash did not fired, auto mode (ATMODE), return light not detected (SRLND)
	ippExifFlashNotFired_ATMODE_SRLD          = 0x1F,   // Flash did not fired, auto mode (ATMODE), return light detected (SRLD)
	ippExifNoFlashFunction                    = 0x20,   // No Flash function
	ippExifFlashFired_RDMODE                  = 0x41,   // Flash fired, red-eye reduction mode (RDMODE)
	ippExifFlashFired_RDMODE_SRLND            = 0x45,   // Flash fired, red-eye reduction mode (RDMODE), return light not detected (SRLND)
	ippExifFlashFired_RDMODE_SRLD             = 0x47,   // Flash fired, red-eye reduction mode (RDMODE), return light detected (SRLD)
	ippExifFlashFired_CFMODE_RDMODE           = 0x49,   // Flash fired, compulsory flash mode (CFMODE), red-eye reduction mode (RDMODE)
	ippExifFlashFired_CFMODE_RDMODE_SRLND     = 0x4D,   // Flash fired, compulsory flash mode (CFMODE), red-eye reduction mode (RDMODE), return light not detected (SRLND)
	ippExifFlashFired_CFMODE_RDMODE_SRLD      = 0x4F,   // Flash fired, compulsory flash mode (CFMODE), red-eye reduction mode (RDMODE), return light detected (SRLD)
	ippExifFlashFired_ATMODE_RDMODE           = 0x59,   // Flash fired, auto mode (ATMODE), red-eye reduction mode (RDMODE)
	ippExifFlashFired_ATMODE_RDMODE_SRLND     = 0x5D,   // Flash fired, auto mode (ATMODE), red-eye reduction mode (RDMODE), return light not detected (SRLND)
	ippExifFlashFired_ATMODE_RDMODE_SRLD      = 0x5F,   // Flash fired, auto mode (ATMODE), red-eye reduction mode (RDMODE), return light detected (SRLD)
	ippExifFlashMax                           = 0x7FFFFFFF,
} IppExifFlash;

typedef enum 
{
	ippExifInteropbR98                         = 0,
	ippExifInteropbTHM                         = 1,
	ippExifInteropbIndexMax                    = 0x7FFFFFFF,
} IppDCFInteropbIndex;   // interoperability index, indicate indentification of the interoperability rule

typedef enum 
{
	ippDCFInteropbVersion1                    = 0, // version 1.0
	ippDCFInteropbVersionMax                  = 0x7FFFFFFF,
} IppDCFInteropbVersion; // interoperability version

typedef enum
{
	ippExifAltitudeRefSeaLevel                  = 0,  // sea level, altitude is above sea level
	ippExifAltitudeRefSeaLevelNeg               = 1,  // sea level, altitude is below sea level
	ippExifAltitudeRefMax                       = 0x7FFFFFFF,
} IppExifAltitudeRef;

typedef enum
{
	ippExifCharacterCodeAscii                 = 0,
	ippExifCharacterCodeJis                   = 1,
	ippExifCharacterCodeUnicode               = 2,
	ippExifCharacterCodeUndefined             = 3,
	ippExifCharacterCodemax                   = 0x7FFFFFFF,
} IppExifCharacterCode;

/* struct definition */
typedef struct
{
	Ipp32u  numerator;
	Ipp32u  denominal;
} IppExif32uRational;

typedef struct
{
	Ipp32s  inumerator;
	Ipp32s  idenominal;
} IppExif32SRational;

typedef struct
{
	/* image description should be just name, not including the extension */
	Ipp8u                           ucImageDescription[MAX_IMAGE_NAME];
	Ipp8u                           ucMake[MAX_MAKE_NAME];
	Ipp8u                           ucModel[MAX_MODEL_NAME];
	Ipp8u                           ucDataTime[TIME_DEFAULT_LEN];
	Ipp8u                           ucSoftware[MAX_SOFTWARE_LEN];
	IppExifOrientation              eOrientation;
	IppExif32uRational              urXResolution;
	IppExif32uRational              urYResolution;
	IppExifResolutinUnit            eResolutionUnit;
	IppExifYCbCrPosition            eYCbCrPosition;
} IppExifTiffInfo;

typedef struct
{
	Ipp8s                           cExifVersion[4];
	Ipp8u                           ucDateTimeOriginal[TIME_DEFAULT_LEN];
	Ipp8u                           ucDateTimeDigitized[TIME_DEFAULT_LEN];
	IppExifComponentConfig          eComponentConfiguration;
	IppExif32uRational              urFocalLength;
	Ipp8s                           cFlashPixelVersion[4];
	IppExifColorSpace               eColorSpace;
	/*
	// uiPixelXDimention is valide image width, for data padding is necessary in horizion,
	//                    it may be different with that recorded in SOF
	// uiPixelYDimention is valide image height, for data padding is unnecessary in vertical,
	//                    it is in fact the same as that recorded in SOF
	//  
	*/
	Ipp32u                          uiPixelXDimention;
	Ipp32u                          uiPixelYDimention;
	IppExifExposureMode             eExposureMode;
	IppExifWhiteBalance             eWhiteBalance;
	IppExif32uRational              urExposureTime;
	IppExif32uRational              urApertureValue;
	IppExif32uRational              urMaxApertureValue;  // smallest F number
	IppExifExposureProgram          eExposureProgram;
	IppExif32SRational              srExposureBiaValue;  // [-99.99 99.99]
	IppExifMeteringMode             eMeteringMode;
	Ipp16u                          *pISOSpeedRating;
	Ipp32u                          uiISOSpeedRatingCnt;
	IppExifContrast                 eContrast;
	IppExifSaturation               eSaturation;
	IppExifSharpness                eSharpness;
	IppExif32SRational              srBrightnessValue;  // [-99.99 99.99], indicate unkonwn with numerator 0xFFFFFFFF
	IppExif32uRational              urFNumber;
	IppExifLightSource              eLightSource;
	IppExifSceneCaptureType         eSceneCaptureType;
	IppExif32SRational              srShutterSpeed;
	IppExifFlash                    eFlash;
	Ipp8u                           *pMakerNote;
	Ipp32u                          uiMakerNoteLen;
	IppExifSceneType                eSceneType;
	IppExifFileSource               eFileSource;
	IppExif32uRational              urDigitalZoomRatio;
	Ipp16u                          usFocalLenIn35mmFilm;
	IppExifSensingMethod            eSensingMethod;
	IppExifCustomRendered           eCustomRendered;
	IppExifSubjectDistanceRange     eSubjectDistanceRange;
} IppExifPrivateTagInfo;

typedef struct
{
	IppExifGPSLatituteRef           eGPSLatituteRef;
	IppExifGPSLongtituteRef         eGPSLongtituteRef;
	IppExif32uRational              urGPSLatitute[3];
	IppExif32uRational              urGPSLongtitute[3];
	IppExifAltitudeRef              eGPSAltitudeRef;
	IppExif32uRational              urGPSAltitude;
	IppExif32uRational              urGPSTimeStamp[3];
	/*
	// GPSProcessMethod need to identify the charactor code used, default using ascii
	// a character string records the name of process method
	// for the len is pointed and undefined type, null terminal is not neccessary
	*/
	Ipp8u                           *pGPSProcessMethod;
	Ipp32u                          uiGPSProcessMethodLen;
	IppExifCharacterCode            eGPSProcessMethodCharacterCode;
	Ipp8u                           ucGPSDateStamp[11];
	Ipp8u                           ucGPSMapDatum[MAX_MAPDATUM_LEN];
} IppExifGPSInfo;


typedef struct
{
	Ipp32u                         uiHeight;
	Ipp32u                         uiWidth;
	Ipp16u                         usBitsPerSample[3];
	Ipp16u                         usSamplePerPixel;
	IppExifPlanarConfig            ePlanarConfig;
	IppExifYCCSubSampling          eYCCSubSampling;
} IppRawThumbAttribute;

typedef struct
{
	IppExifCompression                eCompression;
	IppExifPhotometricInterpretation  ePhotometricInterpretation;
	IppExif32uRational                urXResolution;
	IppExif32uRational                urYResolution;
	IppExifResolutinUnit              eResolutionUnit;
	IppRawThumbAttribute              stRawThumbAttribute;
	Ipp8u                             *pData;
	Ipp32s                            iDataLen;
	IppExifOrientation                eOrientation;
} IppExifThumbnailInfo;

typedef struct
{  
	/* if the file content is equivalent to ExifR98 version 1.0 , the eInterOpbIndex is ippExifInterOpbR98
		and the eInterOpbVersion is ippDCFInterOpbVersion1 */
	IppDCFInteropbIndex               eInteropbIndex;
	IppDCFInteropbVersion             eInteropbVersion;
} IppDCFInteropbInfo;

typedef struct 
{
	Ipp8u                          ucDelJFIFFlag; // 1:remove JFIF tag, 0: keep JFIF tag, default 1
	IppExifTiffInfo                stTiffInfo;
	IppExifPrivateTagInfo          stExifPrivateTagInfo;
	IppExifGPSInfo                 stGPSInfo;
	IppDCFInteropbInfo             stDCFInteropbInfo;
	IppExifThumbnailInfo           stThumbnailInfo;
} IppExifInfo;

typedef struct
{
	Ipp32s   iMinBufLen;
} IppExifBufReq;

typedef struct
{
	Ipp8u    *pBuffer;
	Ipp32s   iAllocLen;
	Ipp32s   iFilledLen;
} IppExifImgBuf;


/* 
* this is the default value of all EXIF related field defined by libippexif,
* users MUST use this const value to initialize IppExifInfo, and modify 
* coorresponding field as you need. ANY usage of libippexif lib without 
* using the IPPExif_Default_ExifInfo to initialize IppExifInfo structure will
* lead to UNEXPECTED result.
*/
extern const IppExifInfo IPPExif_Default_ExifInfo;

/* APIs */
/*****************************************
* name : ippExifWrite
* Function description:
	write exif information to jpeg image
* Input argument:
	pSrcImg   :pointer to jpeg image buffer structure;
	pExifInfo :pointer to exif info structure;
* Output argument:
	pDstImg   :pointer to dest image buffer stucture;
* Return: 
	ippStsDataTypeErr: bad input JPEG image, do not have SOI(FFD8) tag.
	ippStsNotSupportedModeErr: Exif info in App1 is over 64K, currently do not support extending to App2.
	ippStsBadArgErr: bad arg error, assert failed because of bad arg.
	ippStsOutOfRangeErr: input info is not coincidence with Exif spec 2.2.
	ippStsNoMemErr: overlap is occurs between src buffer and dst buffer while reserve JFIF info.need memory to save tmp info.
	ippStsNoErr: no error, success.
* Note:   N/A
******************************************/
IppStatus ippExifWrite( IppExifImgBuf *pSrcImg, IppExifImgBuf *pDstImg,  const IppExifInfo *pExifInfo );

/*****************************************
* Name: ippExifGetBufReq
* Function description:
	calculate the minimum required buffer length(s) according to exif info;
* Input argument:
	pExifInfo :pointer to exif info structure;
* Output argument:
	pBufReq   :record buffer size;
* Return:
	ippStsDataTypeErr: bad input JPEG image, do not have SOI(FFD8) tag.
	ippStsNotSupportedModeErr: Exif info in App1 is over 64K, currently do not support extending to App2.
	ippStsBadArgErr: bad arg error, assert failed because of bad arg.
	ippStsOutOfRangeErr: input info is not coincidence with Exif spec 2.2.
	ippStsNoMemErr: overlap is occurs between src buffer and dst buffer while reserve JFIF info.need memory to save tmp info.
	ippStsErr: unkown error.
	ippStsNoErr: no error, success.
* Note:   N/A
******************************************/
IppStatus ippExifGetBufReq( const IppExifInfo *pExifInfo,  IppExifBufReq *pBufReq );

/*****************************************
* Name: ippExifGetLibVersion
* Function description:
	Get the lib build version. eg: This library is built from TRUNK_IPPONE_BUILD_Jun 15 2011
* Input argument:
	pDst :pointer to dst buffer;
	iSize:allocated len of dst buffer;
* Return:
	ippStsBadArgErr: bad arg.
	ippStsNoErr: no error, success.
* Note:   N/A
******************************************/
IppStatus ippExifGetLibVersion( Ipp8u *pDst, Ipp32s iSize );

#ifdef __cplusplus
}
#endif

#endif  // _IPP_EXIF_H_
