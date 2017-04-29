/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/******************************************************************************
*                MODULE HEADER FILE
*******************************************************************************
*  COPYRIGHT (C) 2001 Intel Corporation.
*
*  This file and the software in it is furnished under
*  license and may only be used or copied in accordance with the terms of the
*  license. The information in this file is furnished for informational use
*  only, is subject to change without notice, and should not be construed as
*  a commitment by Intel Corporation. Intel Corporation assumes no
*  responsibility or liability for any errors or inaccuracies that may appear
*  in this document or any software that may be provided in association with
*  this document.
*  Except as permitted by such license, no part of this document may be
*  reproduced, stored in a retrieval system, or transmitted in any form or by
*  any means without the express written consent of Intel Corporation.
*
*  Title: rtc
*
*  Filename: rtc.h
*
*  Target, subsystem: Common Platform, HAL
*
*  Authors:  Avi Erami, Eilam Ben-Dror
*
*  Description:  rtc header file.
*
*  Last Modified: 24.07.2001
*
*  Notes:
******************************************************************************/
#ifndef _RTC_H_
#define _RTC_H_


#include <time.h>
#include "global_types.h"



/*------------ Global type definitions --------------------------------------*/

/*
	RTC_CalendarTime structure is used in all API that receive/retrieve the date and the time.
	It may be converted into the ANSI time.h tm structure by using the RTCToANSIConvert service.
 */
//ICAT EXPORTED STRUCT
typedef struct
{
	UINT8 second;   //  Seconds after minute: 0 - 59
	UINT8 minute;   //  Minutes after hour: 0 - 59
	UINT8 hour;     //  Hours after midnight: 0 - 23
	UINT8 day;      //  Day of month: 1 - 31
	UINT8 month;    //  Month of year: 1 - 12
	UINT16 year;    //  Calendar year: e.g 2001
}RTC_CalendarTime;


/*
	RTC_Trim structure is used in the RTCConfigure and RTCConfigurationGet services. It specifies
	the calibrated frequency of the Hz clock.
 */
typedef struct
{
	UINT16 clkDividerCount;         //	Number of 32KHz clock cycles +1, per Hz clock cycle.
	UINT16 trimDeleteCount;         //	Number of 32KHz clock cycles to delete when trimming begins.
	BOOL locked;                    //	Indicates whether the value within the trim register can be changed.
}RTC_Trim;


//The handle is received when binding an ISR to the RTC periodic interrupt, and is used for unbinding.
typedef UINT8 RTC_Handle;


//Return Code Definitions:
typedef enum
{
	RTC_RC_OK = 1,

	RTC_RC_MAKETIME_FAILURE = -100,
	RTC_RC_INVALID_PARAMETERS,
	RTC_RC_ALARM_ALREADY_SET,
	RTC_RC_ALARM_TIME_ALREADY_PASSED,
	RTC_RC_NO_ALARM_SET,
	RTC_RC_NO_FREE_HANDLE,
	RTC_RC_INVALID_HANDLE,
	RTC_RC_NO_ISR_BOUND,
	RTC_RC_HW_FAILURE,
	RTC_RC_TRIM_LOCKED,
	RTC_RC_INVALID_BACKUP_REGISTER,
	RTC_RC_BAD_TRIM_PARAMETER,
	RTC_RC_FAIL
}RTC_ReturnCode;


typedef enum
{
	RTC_BACKUP_REG_0        = 0,
	RTC_BACKUP_REG_1        = 1,
	RTC_BACKUP_REG_2        = 2,
	RTC_BACKUP_REG_3        = 3,
	RTC_BACKUP_REG_4        = 4,
	RTC_BACKUP_MAX_REGS = 5
} RTC_BACKUP_REG;

typedef enum
{
	RTC_ALARM_NEGATE = 0x0,
	RTC_ALARM_ASSERT = 0x1
} RTC_ALARM_SIGNAL;


// This is a prototype for an ISR that may be bound to the RTC interrupts
typedef void (*RTC_ISR)(void);

// This is a prototype for a callback function that may be bound to the RTC On Time Set notify service
typedef void (*RTCOnTimeSetNotifyCallback)(RTC_CalendarTime* oldTime, RTC_CalendarTime* newTime);

/*---------------------------------------------------------------------------*/

/*---------- Global variable declarations -----------------------------------*/

/*---------- Global constant declarations -----------------------------------*/




/*---------- Exported function prototypes -------------------------------------*/


void            RTCReset(void);
void            RTCUnreset(void);
void            RTCTrimReset(void);
void            RTCTrimUnreset(void);


RTC_ReturnCode  RTCFromANSIConvert(const struct tm * ANSIStruct, RTC_CalendarTime *calendarTime);
RTC_ReturnCode  RTCToANSIConvert(RTC_CalendarTime *dateAndTime, struct tm * ANSIStruct);
RTC_ReturnCode  RTCDateAndTimeSet(RTC_CalendarTime *dateAndTime);
RTC_ReturnCode  RTCCurrentDateAndTimeGet(RTC_CalendarTime * dateAndTime);
RTC_ReturnCode  RTCAlarmBind(RTC_ISR isr);
RTC_ReturnCode  RTCAlarmSet(RTC_CalendarTime *dateAndTime, RTC_ISR isr);
RTC_ReturnCode  RTCAlarmGet(RTC_CalendarTime *dateAndTime);
RTC_ReturnCode  RTCAlarmReset(void);
RTC_ReturnCode  RTCPeriodicBind(RTC_ISR isr, RTC_Handle *handle);
RTC_ReturnCode  RTCPeriodicUnbind(RTC_Handle handle);
RTC_ReturnCode  RTCHwCheck(void);
RTC_ReturnCode  RTCPhase1Init(void);
RTC_ReturnCode  RTCPhase2Init(void);
RTC_ReturnCode  RTCConfigure(RTC_Trim * trim);
RTC_ReturnCode  RTCConfigurationGet(RTC_Trim * trim);
BOOL            RTCPowerUpAlarmCheck(void);
BOOL            RTCIsAlarmSet(void);
BOOL            RTCHasAlarmInterrupted(void);
SwVersion       RTCVersionGet(void);

RTC_ReturnCode  RTCControlAlarmSignalRead( RTC_ALARM_SIGNAL       *alarm_signal );
RTC_ReturnCode  RTCControlAlarmSignalWrite( RTC_ALARM_SIGNAL alarm_signal );
RTC_ReturnCode  RTCBackupRegisterWrite( RTC_BACKUP_REG backup_reg_num, UINT32 reg_value );
RTC_ReturnCode  RTCBackupRegisterRead( RTC_BACKUP_REG backup_reg_num, UINT32    *reg_value );

RTC_ReturnCode  RTCNotifyOnTimeSetBind(RTCOnTimeSetNotifyCallback OnTimeSetCallback);
RTC_ReturnCode  RTCNotifyOnTimeSetUnBind(RTCOnTimeSetNotifyCallback OnTimeSetCallback);
RTC_ReturnCode RTCNotifyUsers(RTC_CalendarTime *oldRTC, RTC_CalendarTime *newRTC);
void TimeGetTest( void );


//Functions that use time_t type:
time_t          time(time_t * retrievedTime);
RTC_ReturnCode  timeSet(time_t newTime);
RTC_ReturnCode  alarmSet(time_t alarmTime);
time_t          alarmGet(time_t * retrievedTime);

time_t read_rtc(time_t *rtc);
int write_rtc(struct tm *tm);
void calib_rtc(void);


#endif // _RTC_H_

