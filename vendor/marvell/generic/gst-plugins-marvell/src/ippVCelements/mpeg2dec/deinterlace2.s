@/*
@Copyright (c) 2009, Marvell International Ltd.
@All Rights Reserved.
@
@ * This library is free software; you can redistribute it and/or
@ * modify it under the terms of the GNU Library General Public
@ * License as published by the Free Software Foundation; either
@ * version 2 of the License, or (at your option) any later version.
@ *
@ * This library is distributed in the hope that it will be useful,
@ * but WITHOUT ANY WARRANTY; without even the implied warranty of
@ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
@ * Library General Public License for more details.
@ *
@ * You should have received a copy of the GNU Library General Public
@ * License along with this library; if not, write to the
@ * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
@ * Boston, MA 02111-1307, USA.
@ */

@//////////////////////////////////////////////////////////////////////////////
@//   void _Deinterlace2(
@//            unsigned char *dst, int dst_wrap,
@//            const unsigned char *src, int src_wrap,
@//            int width, int height)
@//////////////////////////////////////////////////////////////////////////////


    .global _Deinterlace2

    .equ Deinterlace_LocalSize, 0

    .equ Deinterlace_REG_SAVE_SIZE, 10*4
    .equ Deinterlace_Width, (0 + Deinterlace_REG_SAVE_SIZE+Deinterlace_LocalSize)
    .equ Deinterlace_Height, (4 + Deinterlace_REG_SAVE_SIZE+Deinterlace_LocalSize)

@ ro: dst
@ r1: dst_wrap
@ r2: src
@ r3: src_wrap
@ r11: width
@ r12: height


@ r7: dst backup
@ r8: src backup
@ r9: width backup

@ wr0: src_m2
@ wr1: src_m1
@ wr2: src_0
@ wr3: src_p1
@ wr4: src_p2

@ src and dst are both 8-byte aligned
@ assume width is multiple of 8
@ filter ( [-1, 4, 2, 4, -1] + 4 ) >> 3
@ wcgr0: filter shift param 2
@ wcgr1: filter shift param 4
@ wcgr2: filter shift 3
@ wr15: filter shift 4

@///////////////////////////////////////////
@ src_m2 ----->
@ src_m2 ----->
@ src_0  ----->
@ src_p2 ----->
@ src_p2 ----->
@        ----->
@ external loop:
@   counter: height/2
@   process 8 pixels in horizontal directory
@ internal loop:
@   counter: width/8
@   process each row in vertical directory
@ reduce cache miss
@ as each load is in the same line 
@ but may bring more memory load,
@ most pixels are loaded twice
@////////////////////////////////////////////

_Deinterlace2:
    stmfd	sp!, {r4-r12, lr}

    ldr r11, [sp, #Deinterlace_Width]    @ width
    ldr r12, [sp, #Deinterlace_Height]   @ height

    mov r5, #1
    mov r6, #2
    mov r14, #3

    @ BAC 
    cmp r11, #8  @ if width <8, then do nothing. application should handle this(e.g. with memcopy)
    blt _FuncRt

    cmp r12, #6  @ if height <6, then do nothing. application should handle thise.g. with memcopy)
    blt _FuncRt

    tmcr wcgr0, r5
    tmcr wcgr1, r6
    tmcr wcgr2, r14
    mov r6, #4
    tbcsth wr15, r6

    mov r12, r12, lsr #1   @ height/2, height process unit size
    mov r11, r11, lsr #3   @ width/8, 
                           @ 8 is the size of a unit width process

    @ backup src, dst, width
    mov r7, r0
    mov r8, r2
    mov r9, r11    

@ copy the top three lines 
    mov r4, #3
    bic r6, r11, #7  @ multiple of 64, width/64
    and r5, r11, #7  @ width%64
   
_Deinterlace_LoopCopy:
    cmp r6, #0
    beq _Deinterlace_Less8Unit
    
    wldrd wr0, [r2], #8
    wldrd wr1, [r2], #8
    wldrd wr2, [r2], #8
    wldrd wr3, [r2], #8

    wldrd wr4, [r2], #8
    wldrd wr5, [r2], #8
    wldrd wr6, [r2], #8
    wldrd wr7, [r2], #8

    wstrd wr0, [r0], #8
    wstrd wr1, [r0], #8
    wstrd wr2, [r0], #8
    wstrd wr3, [r0], #8

    wstrd wr4, [r0], #8
    wstrd wr5, [r0], #8
    wstrd wr6, [r0], #8
    wstrd wr7, [r0], #8

    subs r6, r6, #8
    bne _Deinterlace_LoopCopy

_Deinterlace_Less8Unit:
    cmp r5, #0
    beq _Deinterlace_NoLess8Unit

    wldrd wr0, [r2], #8
    wstrd wr0, [r0], #8
    subs r5, r5, #1
    bne _Deinterlace_Less8Unit

_Deinterlace_NoLess8Unit:
    subs r4, r4, #1
    add r0, r7, r1
    add r2, r8, r3
    add r7, r7, r1
    add r8, r8, r3
    bic r6, r11, #7
    and r5, r11, #7
    bne _Deinterlace_LoopCopy

    @ right now,
    @ r7 pointer to dst+3*dst_wrap
    @ r8 pointer to src+3*src_wrap

    sub r12, r12, #2       @ the top and bottom two lines are only copied
                           @ height process unit size is (height-4)/2

@ intermedia lines
    add r4, r1, r1     @ r4: 2*dst_wrap
    sub r8, r8, r3, lsl #1  @ update src addr

_Deinterlace_LoopCol:
    @ restore src, dst, width
    mov r2, r8   @ r2:  src_m2 addr
    mov r0, r7 
    mov r11, r9

    add r5, r2, r3     @ r5:  src_m1 addr
    add r6, r5, r3     @ r6:  src_0 addr
    add r10, r6, r3    @ r10: src_p1 addr
    add r14, r10, r3   @ r14: src_p2 addr

    mov r8, r6 @ update next src addr

_Deinterlace_LoopRow:
    wldrd wr0, [r2], #8    @ src_m2
    wldrd wr1, [r5], #8    @ src_m1
    wldrd wr2, [r6], #8    @ src_m0
    wldrd wr3, [r10], #8   @ src_p1
    wldrd wr4, [r14], #8   @ src_p2


    @ copy the line of first field, src_p1
    wstrd wr3, [r0, r1]


@ filter the second field line
    @ low 4 bytes
    wunpckelub wr5, wr1  @ wr5: src_m1
    wunpckelub wr6, wr3  @ wr6: src_p1
    wunpckelub wr7, wr2  @ wr7: src_0
        
    waddh wr10, wr5, wr6  @ wr10: src_m1+src_p1
    wsllhg wr7, wr7, wcgr0 @ wr7: 2*src_0
    wsllhg wr10, wr10, wcgr1 @ wr10: 4*(src_m1+src_p1)

    wunpckelub wr8, wr0  @ wr8: src_m2
    wunpckelub wr9, wr4  @ wr9: src_p2

    waddh wr10, wr10, wr7    @ wr10: 4*(src_m1+src_p1) + 2*src_0
    waddh wr8, wr8, wr9      @ wr8: src_m2+src_p2
    wsubh wr10, wr10, wr8    @ wr10: 4*(src_m1+src_p1) + 2*src_0 - (src_m2+src_p2)
    waddh wr10, wr10, wr15   @ wr10: 4*(src_m1+src_p1) + 2*src_0 - (src_m2+src_p2) + 4
    wsrahg wr10, wr10, wcgr2 @ wr10: ( 4*(src_m1+src_p1) + 2*src_0 - (src_m2+src_p2) + 4)>>3

    @ high 4 bytes
    wunpckehub wr5, wr1  @ wr5: src_m1
    wunpckehub wr6, wr3  @ wr6: src_p1
    wunpckehub wr7, wr2  @ wr7: src_0
        
    waddh wr5, wr5, wr6    @ wr5: src_m1+src_p1
    wsllhg wr7, wr7, wcgr0 @ wr7: 2*src_0
    wsllhg wr5, wr5, wcgr1 @ wr5: 4*(src_m1+src_p1)

    wunpckehub wr8, wr0  @ wr8: src_m2
    wunpckehub wr9, wr4  @ wr9: src_p2

    waddh wr5, wr5, wr7    @ wr5: 4*(src_m1+src_p1) + 2*src_0
    waddh wr8, wr8, wr9    @ wr8: src_m2+src_p2
    wsubh wr5, wr5, wr8    @ wr5: 4*(src_m1+src_p1) + 2*src_0 - (src_m2+src_p2)
    waddh wr5, wr5, wr15   @ wr5: 4*(src_m1+src_p1) + 2*src_0 - (src_m2+src_p2) + 4
    wsrahg wr5, wr5, wcgr2 @ wr5: ( 4*(src_m1+src_p1) + 2*src_0 - (src_m2+src_p2) + 4)>>3

    wpackhus wr5, wr10, wr5 @ pack low and high bytes

    @ store the filterd line
    wstrd wr5, [r0]
    @ store the bottom line
    cmp r12, #1
    @wstrdeq wr4, [r0, r4]
    bne NextIterate
    wstrd wr4, [r0, r4]
    
NextIterate:
    subs r11, r11, #1
    add r0, r0, #8
    bne _Deinterlace_LoopRow

    add r7, r7, r4     @ update dst addr   

    subs r12, r12, #1
    bne _Deinterlace_LoopCol

_FuncRt:
    ldmfd	sp!, {r4-r12, pc}

