/*
 * "This software program is available to you under a choice of one of two 
 * licenses.  You may choose to be licensed under either the GNU General Public 
 * License (GPL) Version 2, June 1991, available at 
 * http://www.fsf.org/copyleft/gpl.html, or the BSD License, the text of
 * which follows:
 * 
 * Copyright (c) 1996-2005, Intel Corporation. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 * 
 * Redistributions of source code must retain the above copyright notice, this 
 * list of conditions and the following disclaimer. 
 * 
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or 
 * other materials provided with the distribution. 
 * 
 * Neither the name of the Intel Corporation ("Intel") nor the names of its 
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 */

/** 
**  FILENAME:       pmu.h
**
**	CORE STEPPING:  
**
**  PURPOSE: contains all PMU specific macros, typedefs, and prototypes.
**           Declares no storage.
**                  
**
******************************************************************************/

#ifndef __PMU_H__
#define __PMU_H__

#include <asm/types.h>
#include <asm/arch/pxa-regs.h>
#include <asm/hardware.h>


/* PMU Performance Monitor Control Register (PMNC) */
#define PMU_ID							(0x24u << 24)
#define PMU_COUNTERS_DISALBLE			(1u<<4)
#define PMU_CLOCK_DIVIDER				(1u<<3)
#define PMU_CLOCK_RESET				(1u<<2)
#define PMU_COUNTERS_RESET				(1u<<1)
#define PMU_3_COUNTERS_ENABLE			(1u<<0)
#define PMU_COUNTERS_ENABLE			(1u<<0)

/* INTEN & FLAG Registers bit definition*/
#define PMU_CLOCK_COUNT				(1u<<0)
#define PMU_COUNT_0					(1u<<1)
#define PMU_COUNT_1					(1u<<2)
#define PMU_COUNT_2					(1u<<3)
#define PMU_COUNT_3					(1u<<4)


/*Events combination*/
/*!evtCount0/2:0x7(instruction count), evtCount1/3:0x0(ICache miss)*/
#define PMU_EVTCOUNT_1					(0x0007)
/*!evtCount0/2:0xA(DCache Access), evtCount1/3:0xB(DCache miss)*/
#define PMU_EVTCOUNT_2					(0x0B0A)
/*!evtCount0/2:0x1(ICache cannot deliver), evtCount1/3:0x0(ICache miss)*/
#define PMU_EVTCOUNT_3					(0x0001)
/*!evtCount0/2:0xB(DBufer stall duration), evtCount1/3:0x9(Dbuffer stall)*/
#define PMU_EVTCOUNT_4					(0x090B)
/*!evtCount0/2:0x2(data stall), evtCount1/3:0xC(DCache writeback)*/
#define PMU_EVTCOUNT_5					(0x0C02)
/*!evtCount0/2:0x7(instruction count), evtCount1/3:0x3(ITLB miss)*/
#define PMU_EVTCOUNT_6					(0x0307)
/*!evtCount0/2:0xA(DCache Access), evtCount/31:0x4(DTLB miss)*/
#define PMU_EVTCOUNT_7					(0x040A)

enum 
{
    PMU_PMNC = 0,
    PMU_CCNT,
    PMU_PMN0,
    PMU_PMN1,
	PMU_PMN2,
	PMU_PMN3,
	PMU_INTEN,
	PMU_FLAG,
	PMU_EVTSEL
};

/**
  PMU and PML Event
*/
enum
{
	PMU_EVENT_INVALIDATE=0xFFFFFFFFu,
	PMU_EVENT_L1_INSTRUCTION_MISS=0x0u,			/*!< L1 Instruction cache miss requires fetch from external memory */

	PMU_EVENT_L1_INSTRUCTION_NOT_DELIVER,	/*!< L1 Instruction cache cannot deliver an instruction. this  indicate
												 an instruction cache or TLB miss. This event will occur eveyr cycle in 
												 which the condition is present */
	
	PMU_EVENT_STALL_DATA_DEPENDENCY,		/*!< Stall due to a data dependency. This event will occur every cycle in which 
												 the condition is present */

	PMU_EVENT_INSTRUCTION_TLB_MISS,			/*!< Instruction TLB miss*/
	
	PMU_EVENT_DATA_TLB_MISS,				/*!< Data TLB miss*/

	PMU_EVENT_BRANCH_RETIRED,				/*!< Branch instruction retired, branch may or many not have changed program flow.
												 (Counts only B and BL instruction, in both ARM and Thumb mode)*/

	PMU_EVENT_BRANCH_MISPREDICTED,			/*!< Branch mispredicted. Counts only B and BL instructions, in both ARM and Thumb mode*/

	PMU_EVENT_INSTRUCTION_RETIRED,			/*!< Instruction retired. This event will occur every cycle in which 
												 the condition is present*/
	
	PMU_EVENT_L1_DATA_STALL,				/*!< L1 Data cache buffer full stall. This event will occur every cycle in which 
												 the condition is present. */

	PMU_EVENT_L1_DATA_STALL_C,				/*!< L1 Data cache buffer full stall. This event occur for each contiguous sequence of 
												 this type of stall*/

	PMU_EVENT_L1_DATA_ACCESS,				/*!< L1 Data cache access, not including Cache Operations. All data accesses are 
												 treated as cacheable accessses and are counted here even if the cache is not enabled*/
	
	PMU_EVENT_L1_DATA_MISS,					/*!< L1 Data cache miss, not including Cache Operations. All data accesses are treated as 
												 cachedable accesses and are counted as misses if the data cache is not enable*/
	
	PMU_EVENT_L1_DATA_WRITE_BACK,			/*!< L1 data cache write-back. This event occures once for each line that is written back
												 from the cache*/

	PMU_EVENT_SOFTWARE_CHANGED_PC,			/*!< Software changed the PC(b bx bl blx and eor sub rsb add adc sbc rsc orr mov bic mvn ldm pop)
											   will be counted. The count does not increment when an exception occurs and the PC changed to 
											   the exception address(e.g.. IRQ, FIR, SWI,...)*/

	PMU_EVENT_BRANCH_RETIRED_ALL,			/*!< Branch instruction retired, branch may or may noot have chanaged program flow.(Count
											    ALL branch instructions, indirect as well as direct*/

	PMU_EVENT_INSTRUCTION_CYCLE_RETIRED,	/*!< Instruction issue cycle of retired instruction. This event is a count of the number of 
												core cycle each instruction requires to issue*/

	PMU_EVENT_ALL_CHANGED_PC=0x18,				/*!< All change to the PC. (includes software changes and exceptions*/

	PMU_EVENT_PIPE_FLUSH_BRANCH,			/*!< Pipe line flush due to branch mispredict or exception*/

	PMU_EVENT_BACKEND_STALL,				/*!< The core could not issue an instruction due to a backed stall. This event will occur every 
											     cycle in which the condition is present*/
	
	PMU_EVENT_MULTIPLIER,					/*!< Multiplier in use. This event will occur every cycle in which the multiplier is active*/

	PMU_EVENT_MULTIPLIER_STALL_PIPE,		/*!< Multiplier stalled the instruction pipelien due to resource stall. This event will occur
												 every cycle in which the condition is present*/

	PMU_EVENT_COPROCESSOR_STALL_PIPE,		/*!< Coprocessor stalled the instruction pipeline. This event will occur every cycle in which the 
												 condition is present*/

	PMU_EVENT_DATA_CACHE_STALL_PIPE,		/*!< Data cache stalled the instruction pipeline. This event will occur every cycle in which the 
												 condition is present*/

	PMU_EVENT_L2_REQUEST=0x20,				/*!< Unified L2 Cache request, not including cache operations. This event includes table walks, 
												 data and instruction reqeusts*/

	PMU_EVENT_L2_MISS=0x23,					/*!< Unified L2 cache miss, not including cache operations*/

	PMU_EVENT_ADDRESS_BUS=0x40,				/*!< Address bus transcation*/

	PMU_EVENT_SELF_INITIATED_ADDRESS,		/*!< Self initiated(Core Generated) address bus transaction*/

	PMU_EVENT_BUS_CLOCK=0x43,				/*!< Bus clock. This event occurs onece for each bus cycle*/

	PMU_EVENT_SELF_INITIATED_DATA=0x47,			/*!< Data bus transaction. This event occurs once for each data bus cycle*/

	PMU_EVENT_BUS_TRANSACTION,				/*!< Data bus transaction. This event occures once for each data bus cycle*/


	PMU_EVENT_ASSP_0=0x80,
	PMU_EVENT_ASSP_1,
	PMU_EVENT_ASSP_2,
	PMU_EVENT_ASSP_3,
	PMU_EVENT_ASSP_4,
	PMU_EVENT_ASSP_5,
	PMU_EVENT_ASSP_6,
	PMU_EVENT_ASSP_7,
	
	PMU_EVENT_POWER_SAVING=0xFF,			/*!< Power Saving event. This event deactivates the corresponding PMU event counter*/

	MONAHANS_EVENT_MASK=0x80000000,
	MONAHANS_EVENT_CORE_INSTRUCTION_FETCH=MONAHANS_EVENT_MASK,  
												/*!< Core is performing a new instruction fetch. e.g. an L2 cache miss.*/
	MONAHANS_EVENT_CORE_DATA_FETCH,				/*!< Core is performing a new data fetch*/
	MONAHANS_EVENT_CORE_READ,					/*!< Core read request count*/
	MONAHANS_EVENT_LCD_READ,					/*!< LCD read request cout*/
	MONAHANS_EVENT_DMA_READ,					/*!< DMA read request count*/
	MONAHANS_EVENT_CAMERA_READ,					/*!< Camera interface read request cout*/
	MONAHANS_EVENT_USB20_READ,					/*!< USB 2.0 read request count*/
	MONAHANS_EVENT_2D_READ,						/*!< 2D grahpic read request count*/
	MONAHANS_EVENT_USB11_READ,					/*!< USB1.1 host read reqeust count*/
	MONAHANS_EVENT_PX1_UNITIZATION,				/*!< PX1 bus unitization. the number of cycles durring which the PX1 bus is occupied*/
	MONAHANS_EVENT_PX2_UNITIZATION,				/*!< PX2(sidecar) bus unitization. the number of cycles durring which the PX2 bus is occupied*/
	MONAHANS_EVENT_DMC_NOT_EMPTY=MONAHANS_EVENT_MASK|14,			
												/*!< Dynamic memory queue for Mandris occupied. the number of cycles when the DMC queue is not empty*/
	MONAHANS_EVENT_DMC_2,						/*!< Dynamic memory queue for Mandris occupied by more than 1 request. the number of cycles when the DMC
													 queue has 2 or more requests*/

	MONAHANS_EVENT_DMC_3,						/*!< Dynamic memory queue for Mandris occupied by more than 2 request. the number of cycles when the DMC
													 queue has 3 or more requests*/

	MONAHANS_EVENT_DMC_FULL,					/*!< Dynamic memory queue for Mandris occupied by more than 3 request. the number of cycles when the DMC
													 queue is full*/										
	
	MONAHANS_EVENT_SMC_NOT_EMPTY,				/*!< Static memory queue for Mandris occupied. the number of cycles when the SMC queue is not empty*/
	MONAHANS_EVENT_SMC_2,						/*!< Static memory queue for Mandris occupied by more than 1 request. the number of cycles when the SMC
													 queue has 2 or more requests*/

	MONAHANS_EVENT_SMC_3,						/*!< Static memory queue for Mandris occupied by more than 2 request. the number of cycles when the SMC
													 queue has 3 or more requests*/

	MONAHANS_EVENT_SMC_FULL,					/*!< Static memory queue for Mandris occupied by more than 3 request. the number of cycles when the SMC
													 queue is full*/				
	
	MONAHANS_EVENT_ISRAM_NOT_EMPTY=MONAHANS_EVENT_MASK|26,		/*!< Internal SRAM queue for Mandris occupied. the number of cycles when the ISRAM queue is not empty*/
	MONAHANS_EVENT_ISRAM_2,						/*!< Internal SRAM queue for Mandris occupied by more than 1 request. the number of cycles when the ISRAM
													 queue has 2 or more requests*/

	MONAHANS_EVENT_ISRAM_3,						/*!< Internal SRAM queue for Mandris occupied by more than 2 request. the number of cycles when the ISRAM
													 queue has 3 or more requests*/

	MONAHANS_EVENT_ISRAM_FULL,					/*!< Internal SRAM queue for Mandris occupied by more than 3 request. the number of cycles when the ISRAM
													 queue is full*/				
	MONAHANS_EVENT_EXMEM,						/*!< the number of cycles when external memory controller bus is occupied */
	MONAHANS_EVENT_DFC,						/*!< the number of cycles when external data flash bus is occupies */
	MONAHANS_EVENT_CORE_WRITE=MONAHANS_EVENT_MASK|36,	
												/*!< Core write request count*/
	MONAHANS_EVENT_DMA_WRITE,					/*!< DMA write request count*/
	MONAHANS_EVENT_CAMERA_WRITE,				/*!< Camera interface write request cout*/
	MONAHANS_EVENT_USB20_WRITE,					/*!< USB 2.0 write request count*/
	MONAHANS_EVENT_2D_WRITE,					/*!< 2D grahpic write request count*/
	MONAHANS_EVENT_USB11_WRITE,					/*!< USB1.1 host write reqeust count*/

	MONAHANS_EVENT_PX1_REQUEST,					/*!< PX1 bus reqeust. length of time that at least one bus request is asserted on PX bus 1*/
	MONAHANS_EVENT_PX2_REQUEST,					/*!< PX2 bus reqeust.  length of time that at least one bus request is asserted on PX bus 2*/

	MONAHANS_EVENT_PX1_RETRIES,					/*!< PX1 bus retries. number of retries on PX bus 1*/
	MONAHANS_EVENT_PX2_RETRIES,					/*!< PX2 bus retries. number of retries on PX bus 2*/

	
	MONAHANS_EVENT_TEMPERATURE_1,				/*!< Temperature leve 1. time the part has spent in temperature range 1*/
	MONAHANS_EVENT_TEMPERATURE_2,				/*!< Temperature leve 1. time the part has spent in temperature range 2*/
	MONAHANS_EVENT_TEMPERATURE_3,				/*!< Temperature leve 1. time the part has spent in temperature range 3*/
	MONAHANS_EVENT_TEMPERATURE_4,				/*!< Temperature leve 1. time the part has spent in temperature range 4*/

	MONAHANS_EVENT_CORE_LATENCY_1,				/*!< Core read/write latency measurement. amount of time when core have more than 1 read/write
												  request outstanding*/

	MONAHANS_EVENT_CORE_LATENCY_2,				/*!< Core read/write latency measurement. amount of time when core have more than 2 read/write
												  request outstanding*/
	MONAHANS_EVENT_CORE_LATENCY_3,				/*!< Core read/write latency measurement. amount of time when core have more than 3 read/write
												  request outstanding*/
	MONAHANS_EVENT_CORE_LATENCY_4,				/*!< Core read/write latency measurement. amount of time when core have more than 4 read/write
												  request outstanding*/

	MONAHANS_EVENT_PX1_IM_1,					/*!<  PX1 to IM read/write latency measurement. Amount of time when PX1 to IM has more than 1 
												   read/write requests outstanding.*/
	MONAHANS_EVENT_PX1_IM_2,					/*!<  PX1 to IM read/write latency measurement. Amount of time when PX1 to IM has more than 2 
												   read/write requests outstanding.*/
	MONAHANS_EVENT_PX1_IM_3,					/*!<  PX1 to IM read/write latency measurement. Amount of time when PX1 to IM has more than 3 
												   read/write requests outstanding.*/
	MONAHANS_EVENT_PX1_IM_4,					/*!<  PX1 to IM read/write latency measurement. Amount of time when PX1 to IM has more than 4 
												   read/write requests outstanding.*/

	MONAHANS_EVENT_PX1_MEM_1,					/*!<  PX1 to DMEM/SMEM read/write latency measurement. Amount of time when PX1 to DMEM/SMEM has more than 1 
												   read/write requests outstanding.*/
	MONAHANS_EVENT_PX1_MEM_2,					/*!<  PX1 to DMEM/SMEM read/write latency measurement. Amount of time when PX1 to DMEM/SMEM has more than 2 
												   read/write requests outstanding.*/
	MONAHANS_EVENT_PX1_MEM_3,					/*!<  PX1 to DMEM/SMEM read/write latency measurement. Amount of time when PX1 to DMEM/SMEM has more than 3 
												   read/write requests outstanding.*/
	MONAHANS_EVENT_PX1_MEM_4,					/*!<  PX1 to DMEM/SMEM read/write latency measurement. Amount of time when PX1 to DMEM/SMEM has more than 4 
												   read/write requests outstanding.*/

	MONAHANS_EVENT_PX2_IM_1,					/*!<  PX2 to IM read/write latency measurement. Amount of time when PX2 to IM has more than 1 
												   read/write requests outstanding.*/
	MONAHANS_EVENT_PX2_IM_2,					/*!<  PX2 to IM read/write latency measurement. Amount of time when PX2 to IM has more than 2 
												   read/write requests outstanding.*/
	MONAHANS_EVENT_PX2_IM_3,					/*!<  PX2 to IM read/write latency measurement. Amount of time when PX2 to IM has more than 3 
												   read/write requests outstanding.*/
	MONAHANS_EVENT_PX2_IM_4,					/*!<  PX2 to IM read/write latency measurement. Amount of time when PX2 to IM has more than 4 
												   read/write requests outstanding.*/

	MONAHANS_EVENT_PX2_MEM_1,					/*!<  PX2 to DMEM/SMEM read/write latency measurement. Amount of time when PX2 to DMEM/SMEM has more than 1 
												   read/write requests outstanding.*/
	MONAHANS_EVENT_PX2_MEM_2,					/*!<  PX2 to DMEM/SMEM read/write latency measurement. Amount of time when PX2 to DMEM/SMEM has more than 2 
												   read/write requests outstanding.*/
	MONAHANS_EVENT_PX2_MEM_3,					/*!<  PX2 to DMEM/SMEM read/write latency measurement. Amount of time when PX2 to DMEM/SMEM has more than 3 
												   read/write requests outstanding.*/
	MONAHANS_EVENT_PX2_MEM_4					/*!<  PX2 to DMEM/SMEM read/write latency measurement. Amount of time when PX2 to DMEM/SMEM has more than 4 
												   read/write requests outstanding.*/

};

#ifdef __cplusplus
extern "C"
{
#endif

/**
	This routine reads the designated PMU register via CoProcessor 14 

@param	 aReg		PMU register number to read define in int
@return				32-bit value read from register
*/
extern unsigned int pmu_read_reg(unsigned int aReg);
/**
	This routine Writes the designated PMU register via CoProcessor 14 

@param	 aReg		PMU register number to read define in int
		 aValue		Value to write to PMU register
@return				
*/
extern void pmu_write_reg(unsigned int aReg, unsigned int aValue);

extern int pmu_select_event(int counter, int type);

#ifdef __cplusplus
}
#endif


#endif //__PMU_H__ 

