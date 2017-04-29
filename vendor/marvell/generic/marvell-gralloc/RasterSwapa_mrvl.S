@/******************************************************************************
@ *      Copyright (c) 2010 Marvell Corporation. All Rights Reserved.
@ ******************************************************************************/
    .title    "MARVELL_BLEND_PRIMITIVES"

    .code 32

@ Converted from:     AREA RasterConvertWMMX, CODE, READONLY, align=5
.text
.align 5

    .global ARGB8888ToABGR8888_MRVL
    .hidden ARGB8888ToABGR8888_MRVL

    .align 5

ARGB8888ToABGR8888_MRVL:
@
@   void ARGB8888ToABGR8888_MRVL(DWORD *pSrc, SDWORD srcStride, DWORD srcWidth, DWORD srcHeight
@                                DWORD *pDst, SDWORD dstStride)
@
@   ARM Registers Assignment:
@       r0          :   pSrc
@       r1          :   pDst
@       r2          :   srcWidth
@       r3          :   srcHeight
@   WMMX registers Assignment:
@       wR0         :   [0xFF00   0xFF00   0xFF00   0xFF00  ]
@
@  swap from ARGB8888 to ABGR8888
@
    LDR         r12,   [sp]                                     @ r12 = pDst
    TEQ         r0,    #0
    TEQNE       r2,    #0
    TEQNE       r3,    #0
    TEQNE       r12,   #0
    BXEQ        lr

    PLD         [r0]
    STMFD       sp!,   {r4 - r11}
    MUL         r4,    r2,    r3                                @ r4 = width * height
    MOV         r12,   r2

    LDR         r7,    [sp,   #36]                              @ r7 = dstStride
    SUB         r5,    r3,    #1                                @ r5 = height - 1
    WCMPEQH     wR0,   wR1,   wR1                               @ WR0 = ox ffff ffff ffff ffff
    MOV         r5,    r5,    LSL #2                            @ r5 = (height-1)*4
    TST         r1,    r7                                       @ srcStride AND dstStride
    BPL         _ARGB8888ToABGR8888_MRVL_NoRecast               @ if(srcStride>=0 || dstStride>=0)
    LDR         r6,    [sp,   #32]
    MLA         r0,    r1,    r5,    r0                         @ srcStride<0 && dstStride<0
    RSB         r1,    r1,    #0
    MLA         r6,    r7,    r5,    r6
    RSB         r7,    r7,    #0

_ARGB8888ToABGR8888_MRVL_NoRecast:
    LDR         r6,    [sp,   #32]                              @ r6 = pDst
    ORRS        r8,    r1,    r7
    WSLLH       wR0,   wR0,   #8                                @ wR0 = 0x ff00 ff00 ff00 ff00
    BMI         _ARGB8888ToABGR8888_MRVL_NegStride              @ have one negtive stride

    SUB         r8,    r1,    r2                                @ r8 = srcStride - width
    SUB         r9,    r7,    r2                                @ r9 = dstStride - width
    ORRS        r10,   r8,    r9
    MOVEQ       r3,    #1                                       @ if(srcStride=dstStride=width)
    MOVEQ       r2,    r4

    SUBS        r10,   r6,    r0                                @ r10 = pDst - pSrc
    MUL         r11,   r1,    r5                                @ r11 = srcStride*(height-1)*4
    ADD         r11,   r11,   r12,    LSL #2                    @ r11 = srcStride*(height-1)*4+width*4, all the contents

    MUL         r5,    r7,    r5                                @ r5 = dstStride*(height-1)*4
    ADD         r5,    r5,    r12,   LSL #2                     @ r5 = dstStride*(height-1)*4 + width*4
    CMPHS       r11,   r10
    MOV         r1,    r6
    BHI         _ARGB8888ToABGR8888_MRVL_Overlap
    B           _ARGB8888ToABGR8888_MRVL_LineLoop

_ARGB8888ToABGR8888_MRVL_NegStride:
    SUB         r8,    r1,    r2
    SUB         r9,    r7,    r2

    @lineloop
    @ r0  = pSrc
    @ r1  = pDst
    @ r2  = width
    @ r3  = height
    @ r4  = 0x ff00 ff00
    @ wR0 = 0x ff00 ff00 ff00 ff00
_ARGB8888ToABGR8888_MRVL_LineLoop:
    TST         r1,    #7                                       @ test pDst
    PLD         [r0,   #32]
    LDR         r4,    =0xff00ff00
    BEQ         _ARGB8888ToABGR8888_MRVL_Aligned

    LDR         r5,    [r0],  #4                                @ r5 = A R G B
    SUBS        r2,    r2,    #1
    AND         r6,    r5,    r4                                @ r6 = A 0 G 0
    BIC         r7,    r5,    r4                                @ r7 = 0 R 0 B
    ORR         r5,    r6,    r7,    ROR #16
    STR         r5,    [r1],  #4
    BEQ         _ARGB8888ToABGR8888_MRVL_TailEnd

    @ pDst Aligned
_ARGB8888ToABGR8888_MRVL_Aligned:
    SUBS        r2,    r2,    #8
    BLT         _ARGB8888ToABGR8888_MRVL_Tail
    TST         r0,    #7
    BNE         _ARGB8888ToABGR8888_MRVL_SrcNotAligned
    @ pDst and pSrc both aligned
_ARGB8888ToABGR8888_MRVL_SrcAligned:
    WLDRD       wR1,   [r0],  #8                                @ wR1 = A1 R1 G1 B1 A0 R0 G0 B0
    WLDRD       wR2,   [r0],  #8                                @ wR2 = A3 R3 G3 B3 A2 R2 G2 B2
    WLDRD       wR3,   [r0],  #8                                @ wR3 = A5 R5 G5 B5 A4 R4 G4 B4
    WLDRD       wR4,   [r0],  #8                                @ wR4 = A7 R7 G7 B7 A6 R6 G6 B6
    SUBS        r2,    r2,    #8
    PLD         [r0,   #64]

    WAND        wR5,   wR1,   wR0                               @ wR5 = A1 00 G1 00 A0 00 G0 00
    WANDN       wR6,   wR1,   wR0                               @ wR6 = 00 R1 00 B1 00 R0 00 B0
    WSHUFH      wR6,   wR6,   #0xb1                             @ wR6 = 00 B1 00 R1 00 B0 00 R0
    WOR         wR1,   wR5,   wR6                               @ wR1 = A1 B1 G1 R1 A0 R0 G0 B0
    WSTRD       wR1,   [r1],  #8

    WAND        wR5,   wR2,   wR0
    WANDN       wR6,   wR2,   wR0
    WSHUFH      wR6,   wR6,   #0xb1
    WOR         wR2,   wR5,   wR6
    WSTRD       wR2,   [r1],  #8

    WAND        wR5,   wR3,   wR0
    WANDN       wR6,   wR3,   wR0
    WSHUFH      wR6,   wR6,   #0xb1
    WOR         wR3,   wR5,   wR6
    WSTRD       wR3,   [r1],  #8

    WAND        wR5,   wR4,   wR0
    WANDN       wR6,   wR4,   wR0
    WSHUFH      wR6,   wR6,   #0xb1
    WOR         wR4,   wR5,   wR6
    WSTRD       wR4,   [r1],  #8

    BGE         _ARGB8888ToABGR8888_MRVL_SrcAligned
    B           _ARGB8888ToABGR8888_MRVL_Tail

    @ pDst Aligned, pSrc not aligned
_ARGB8888ToABGR8888_MRVL_SrcNotAligned:
    WLDRW       wR1,   [r0],  #4                                @ wR1 = 00 00 00 00 A0 R0 G0 B0
    WLDRD       wR2,   [r0],  #8                                @ wR2 = A2 R2 G2 B2 A1 R1 G1 B1
    WLDRD       wR3,   [r0],  #8                                @ wR3 = A4 R4 G4 B4 A3 R3 G3 B3
    WLDRD       wR4,   [r0],  #8                                @ wR4 = A6 R6 G6 B6 A5 R5 G5 B5
    WLDRW       wR5,   [r0],  #4                                @ wR5 = 00 00 00 00 A7 R7 G7 B7
    PLD         [r0,   #64]

    WUNPCKILW   wR1,   wR1,   wR2                               @ wR1 = A1 R1 G1 B1 A0 R0 G0 B0
    SUBS        r2,    r2,    #8
    WALIGNI     wR2,   wR2,   wR3,   #4                         @ wR2 = A3 R3 G3 B3 A2 R2 G2 B2

    WAND        wR6,   wR1,   wR0                               @ wR6 = A1 00 G1 00 A0 00 G0 00
    WANDN       wR7,   wR1,   wR0                               @ wR7 = 00 R1 00 B1 00 R0 00 B0
    WSHUFH      wR7,   wR7,   #0xb1                             @ wR7 = 00 B1 00 R1 00 B0 00 R0
    WOR         wR1,   wR6,   wR7                               @ wR1 = A1 B1 G1 R1 A0 R0 G0 B0
    WSTRD       wR1,   [r1],  #8

    WAND        wR6,   wR2,   wR0
    WANDN       wR7,   wR2,   wR0
    WSHUFH      wR7,   wR7,   #0xb1
    WOR         wR2,   wR6,   wR7
    WSTRD       wR2,   [r1],  #8

    WALIGNI     wR3,   wR3,   wR4,   #4                         @ wR3 = A5 R5 G5 B5 A4 R4 G4 B4
    WALIGNI     wR4,   wR4,   wR5,   #4                         @ wR4 = A7 R7 G7 B7 A6 R6 G6 B6

    WAND        wR6,   wR3,   wR0
    WANDN       wR7,   wR3,   wR0
    WSHUFH      wR7,   wR7,   #0xb1
    WOR         wR3,   wR6,   wR7
    WSTRD       wR3,   [r1],  #8

    WAND        wR6,   wR4,   wR0
    WANDN       wR7,   wR4,   wR0
    WSHUFH      wR7,   wR7,   #0xb1
    WOR         wR4,   wR6,   wR7
    WSTRD       wR4,   [r1],  #8

    BGE         _ARGB8888ToABGR8888_MRVL_SrcNotAligned

    @ less than 8 pixels, do one-by-one
_ARGB8888ToABGR8888_MRVL_Tail:
    ADDS        r2,    r2,    #8
    BLE         _ARGB8888ToABGR8888_MRVL_TailEnd
_ARGB8888ToABGR8888_MRVL_TailLoop:
    LDR         r5,    [r0],  #4                                @ r5 = A R G B
    SUBS        r2,    r2,    #1
    AND         r6,    r5,    r4                                @ r6 = A 0 G 0
    BIC         r7,    r5,    r4                                @ r7 = 0 R 0 B
    ORR         r5,    r6,    r7,    ROR #16
    STR         r5,    [r1],  #4
    BGT         _ARGB8888ToABGR8888_MRVL_TailLoop

    @ move to next line
_ARGB8888ToABGR8888_MRVL_TailEnd:
    MOV         r2,    r12
    ADD         r0,    r0,    r8,    LSL #2
    SUBS        r3,    r3,    #1
    ADD         r1,    r1,    r9,    LSL #2
    BGT         _ARGB8888ToABGR8888_MRVL_LineLoop
    LDMFD       sp!,   {r4 - r11}
    BX          lr

    @ overlap
    @ r0  = pSrc
    @ r1  = srcStride
    @ r2  = width
    @ r3  = height
    @ r4  = 0x ff00 ff00
    @ wR0 = 0x ff00 ff00 ff00 ff00
_ARGB8888ToABGR8888_MRVL_Overlap:
    LDR         r4,    =0xff00ff00
    ADD         r0,    r0,    r11                               @ pSrc move to tail (r0)
    ADD         r1,    r1,    r5                                @ pDst move to tail (r1)

_ARGB8888ToABGR8888_MRVL_Overlap_LineLoop:
    TST         r1,    #7                                       @ test pDst
    PLD         [r0,   #-32]
    BEQ         _ARGB8888ToABGR8888_MRVL_Overlap_Aligned

    LDR         r5,    [r0,   #-4]!
    SUBS        r2,    r2,    #1
    AND         r6,    r5,    r4
    BIC         r7,    r5,    r4
    ORR         r5,    r6,    r7,   ROR #16
    STR         r5,    [r1,   #-4]!
    BEQ         _ARGB8888ToABGR8888_MRVL_Overlap_TailEnd

_ARGB8888ToABGR8888_MRVL_Overlap_Aligned:
    SUBS        r2,    r2,    #8
    BLT         _ARGB8888ToABGR8888_MRVL_Overlap_Tail
    TST         r0,    #7
    BNE         _ARGB8888ToABGR8888_MRVL_Overlap_SrcNotAligned

    @ pDst and pSrc both aligned
_ARGB8888ToABGR8888_MRVL_Overlap_SrcAligned:
    WLDRD       wR1,   [r0,   #-8]!                             @ wR1 = A1 R1 G1 B1 A0 R0 G0 B0
    WLDRD       wR2,   [r0,   #-8]!                             @ wR2 = A3 R3 G3 B3 A2 R2 G2 B2
    WLDRD       wR3,   [r0,   #-8]!                             @ wR3 = A5 R5 G5 B5 A4 R4 G4 B4
    WLDRD       wR4,   [r0,   #-8]!                             @ wR4 = A7 R7 G7 B7 A6 R6 G6 B6
    SUBS        r2,    r2,    #8
    PLD         [r0,   #-64]


    WAND        wR5,   wR1,   wR0                               @ wR5 = A1 00 G1 00 A0 00 G0 00
    WANDN       wR6,   wR1,   wR0                               @ wR6 = 00 R1 00 B1 00 R0 00 B0
    WSHUFH      wR6,   wR6,   #0xb1                             @ wR6 = 00 B1 00 R1 00 B0 00 R0
    WOR         wR1,   wR5,   wR6                               @ wR1 = A1 B1 G1 R1 A0 R0 G0 B0
    WSTRD       wR1,   [r1,   #-8]!

    WAND        wR5,   wR2,   wR0
    WANDN       wR6,   wR2,   wR0
    WSHUFH      wR6,   wR6,   #0xb1
    WOR         wR2,   wR5,   wR6
    WSTRD       wR2,   [r1,   #-8]!

    WAND        wR5,   wR3,   wR0
    WANDN       wR6,   wR3,   wR0
    WSHUFH      wR6,   wR6,   #0xb1
    WOR         wR3,   wR5,   wR6
    WSTRD       wR3,   [r1,   #-8]!

    WAND        wR5,   wR4,   wR0
    WANDN       wR6,   wR4,   wR0
    WSHUFH      wR6,   wR6,   #0xb1
    WOR         wR4,   wR5,   wR6
    WSTRD       wR4,   [r1,   #-8]!

    BGE         _ARGB8888ToABGR8888_MRVL_Overlap_SrcAligned
    B           _ARGB8888ToABGR8888_MRVL_Overlap_Tail

_ARGB8888ToABGR8888_MRVL_Overlap_SrcNotAligned:
    WLDRW       wR1,   [r0,   #-4]!
    WLDRD       wR2,   [r0,   #-8]!
    WLDRD       wR3,   [r0,   #-8]!
    WLDRD       wR4,   [r0,   #-8]!
    WLDRW       wR5,   [r0,   #-4]!
    PLD         [r0,   #-64]
    SUBS        r2,    r2,    #8

    WALIGNI     wR1,   wR2,   wR1,   #4
    WALIGNI     wR2,   wR3,   wR2,   #4

    WAND        wR6,   wR1,   wR0                               @ wR6 = A1 00 G1 00 A0 00 G0 00
    WANDN       wR7,   wR1,   wR0                               @ wR7 = 00 R1 00 B1 00 R0 00 B0
    WSHUFH      wR7,   wR7,   #0xb1                             @ wR7 = 00 B1 00 R1 00 B0 00 R0
    WOR         wR1,   wR6,   wR7                               @ wR1 = A1 B1 G1 R1 A0 R0 G0 B0
    WSTRD       wR1,   [r1,   #-8]!

    WAND        wR6,   wR2,   wR0
    WANDN       wR7,   wR2,   wR0
    WSHUFH      wR7,   wR7,   #0xb1
    WOR         wR2,   wR6,   wR7
    WSTRD       wR2,   [r1,   #-8]!

    WALIGNI     wR3,   wR4,   wR3,   #4
    WUNPCKILW   wR4,   wR5,   wR4

    WAND        wR6,   wR3,   wR0
    WANDN       wR7,   wR3,   wR0
    WSHUFH      wR7,   wR7,   #0xb1
    WOR         wR3,   wR6,   wR7
    WSTRD       wR3,   [r1,   #-8]!

    WAND        wR6,   wR4,   wR0
    WANDN       wR7,   wR4,   wR0
    WSHUFH      wR7,   wR7,   #0xb1
    WOR         wR4,   wR6,   wR7
    WSTRD       wR4,   [r1,   #-8]!

    BGE         _ARGB8888ToABGR8888_MRVL_Overlap_SrcNotAligned

_ARGB8888ToABGR8888_MRVL_Overlap_Tail:
    ADDS        r2,    r2,    #8
    BLE         _ARGB8888ToABGR8888_MRVL_Overlap_TailEnd

_ARGB8888ToABGR8888_MRVL_Overlap_TailLoop:
    LDR         r5,    [r0,   #-4]!
    SUBS        r2,    r2,    #1
    AND         r6,    r5,    r4
    BIC         r7,    r5,    r4
    ORR         r5,    r6,    r7,  ROR #16
    STR         r5,    [r1,   #-4]!
    BGT         _ARGB8888ToABGR8888_MRVL_Overlap_TailLoop

_ARGB8888ToABGR8888_MRVL_Overlap_TailEnd:
    MOV         r2,    r12
    SUB         r0,    r0,    r8,    LSL #2
    SUBS        r3,    r3,    #1
    SUB         r1,    r1,    r9,    LSL #2
    BGT         _ARGB8888ToABGR8888_MRVL_Overlap_LineLoop
    LDMFD       sp!,   {r4 - r11}
    BX          lr

    @_ARGB8888PreToARGB8888_MRVL


