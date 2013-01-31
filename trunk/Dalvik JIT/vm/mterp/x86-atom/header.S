/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
 * 32-bit x86 definitions and declarations.
 */

/*
386 ABI general notes:

Caller save set:
   eax, edx, ecx, st(0)-st(7)
Callee save set:
   ebx, esi, edi, ebp
Return regs:
   32-bit in eax
   64-bit in edx:eax (low-order 32 in eax)
   fp on top of fp stack st(0)

Parameters passed on stack, pushed right-to-left.  On entry to target, first
parm is at 4(%esp).  Traditional entry code is:

functEntry:
    push    %ebp             # save old frame pointer
    mov     %ebp,%esp        # establish new frame pointer
    sub     FrameSize,%esp   # Allocate storage for spill, locals & outs

Once past the prologue, arguments are referenced at ((argno + 2)*4)(%ebp)

Stack must be 16-byte aligned to support SSE in native code.

If we're not doing variable stack allocation (alloca), the frame pointer can be
eliminated and all arg references adjusted to be esp relative.

Mterp notes:

Some key interpreter variables will be assigned to registers.  Note that each
will also have an associated spill location (mostly useful for those assigned
to callee save registers).

  nick     reg   purpose
  rPC      esi   interpreted program counter, used for fetching instructions
  rFP      edi   interpreted frame pointer, used for accessing locals and args
  rINSTw   bx    first 16-bit code of current instruction
  rINSTbl  bl    opcode portion of instruction word
  rINSTbh  bh    high byte of inst word, usually contains src/tgt reg names
  rIBASE   edx   base of instruction handler table

Notes:
   o High order 16 bits of ebx must be zero on entry to handler
   o rPC, rFP, rINSTw/rINSTbl valid on handler entry and exit
   o eax and ecx are scratch, rINSTw/ebx sometimes scratch

*/

# Many data elements that were accessed
# by rGLUE in HC are now accessed through the
# this pointer to the thread struct
#define rSELF    8(%ebp)
# For HC x86-atom this was also %esi
#define rPC      %esi
# For HC x86-atom this was also %edi
#define rFP      %edi
# For HC x86-atom this was also %ebx
#define rINST    %ebx
# For HC x86-atom this was also %bx
#define rINSTw   %bx
# For HC x86-atom this was also %bh
#define rINSTbh  %bh
# For HC x86-atom this was also %bl
#define rINSTbl  %bl
# For HC x86-atom this handler table base was
# variable and not tied exclusively to %edx
#define rIBASE   %edx


/* Frame diagram while executing dvmMterpStdRun, high to low addresses */
#define IN_ARG0        (  8)
#define CALLER_RP      (  4)
#define PREV_FP        (  0)
/* Spill offsets relative to %ebp */
#define EDI_SPILL      ( -4)
#define ESI_SPILL      ( -8)
#define EBX_SPILL      (-12)
#define rPC_SPILL      (-16)
#define rFP_SPILL      (-20)
#define rINST_SPILL    (-24)
#define rIBASE_SPILL   (-28)
#define TMP_SPILL1     (-32)
#define TMP_SPILL2     (-36)
#define TMP_SPILL3     (-20)
#define LOCAL0_OFFSET  (-44)
#define LOCAL1_OFFSET  (-48)
#define LOCAL2_OFFSET  (-52)
/* Out Arg offsets, relative to %esp */
#define OUT_ARG4       ( 16)
#define OUT_ARG3       ( 12)
#define OUT_ARG2       (  8)
#define OUT_ARG1       (  4)
#define OUT_ARG0       (  0)  /* <- dvmMterpStdRun esp */
#if defined(WITH_JIT)
/* for spill region: increase size by 48 (to keep 16-byte alignment) */
/* 76 + 48 = 124 */
#define JIT_SPILL      (-56)
#define FRAME_SIZE     124
#else
#define FRAME_SIZE     76
#endif

#define SPILL(reg) movl reg##,reg##_SPILL(%ebp)
#define UNSPILL(reg) movl reg##_SPILL(%ebp),reg
#define SPILL_TMP1(reg) movl reg,TMP_SPILL1(%ebp)
#define UNSPILL_TMP1(reg) movl TMP_SPILL1(%ebp),reg
#define SPILL_TMP2(reg) movl reg,TMP_SPILL2(%ebp)
#define UNSPILL_TMP2(reg) movl TMP_SPILL2(%ebp),reg
#define SPILL_TMP3(reg) movl reg,TMP_SPILL3(%ebp)
#define UNSPILL_TMP3(reg) movl TMP_SPILL3(%ebp),reg

#if defined(WITH_JIT)
.macro GET_JIT_PROF_TABLE _self _reg
    movl    offThread_pJitProfTable(\_self),\_reg
.endm
.macro GET_JIT_THRESHOLD _self _reg
    movl    offThread_jitThreshold(\_self),\_reg
.endm
#endif

/* save/restore the PC and/or FP from the self struct */
.macro SAVE_PC_FP_TO_SELF _reg
    movl     rSELF,\_reg
    movl     rPC,offThread_pc(\_reg)
    movl     rFP,offThread_curFrame(\_reg)
.endm

.macro LOAD_PC_FP_FROM_SELF
    movl    rSELF,rFP
    movl    offThread_pc(rFP),rPC
    movl    offThread_curFrame(rFP),rFP
.endm

/* The interpreter assumes a properly aligned stack on entry, and
 * will preserve 16-byte alignment.
 */

/*
 * "export" the PC to the interpreted stack frame, f/b/o future exception
 * objects.  Must be done *before* something throws.
 *
 * In C this is "SAVEAREA_FROM_FP(fp)->xtra.currentPc = pc", i.e.
 * fp - sizeof(StackSaveArea) + offsetof(SaveArea, xtra.currentPc)
 *
 * It's okay to do this more than once.
 */
.macro EXPORT_PC        # This is consistent to the HC x86-atom implementation
    movl     rPC, (-sizeofStackSaveArea + offStackSaveArea_currentPc)(rFP)
.endm

.macro GET_PC        # This is consistent to the HC x86-atom implementation
    movl     (-sizeofStackSaveArea + offStackSaveArea_currentPc)(rFP), rPC
.endm

/*
 * Given a frame pointer, find the stack save area.
 *
 * In C this is "((StackSaveArea*)(_fp) -1)".
 */
.macro SAVEAREA_FROM_FP _reg    # This is consistent with the HC x86-atom implementation
    leal    -sizeofStackSaveArea(rFP), \_reg
.endm

/*
 * Fetch the next instruction from rPC into rINSTw.  Does not advance rPC.
 */
.macro FETCH_INST       # This is consistent with the HC x86-atom implementation
    movzwl    (rPC),rINST
.endm

/*
 * Fetch the opcode byte and zero-extend it into _reg.  Must be used
 * in conjunction with GOTO_NEXT_R
 */
.macro FETCH_INST_R _reg      # In HC x86-atom this was called FFETCH
    movzbl    (rPC),\_reg
.endm

/*
 * Fetch the opcode byte at _count words offset from rPC and zero-extend
 * it into _reg.  Must be used in conjunction with GOTO_NEXT_R
 */
.macro FETCH_INST_OPCODE _count _reg    # In HC x86-atom this wa called FFETCH_ADV
    movzbl  \_count*2(rPC),\_reg
.endm

/*
 * Fetch the nth instruction word from rPC into rINSTw.  Does not advance
 * rPC, and _count is in words
 */
.macro FETCH_INST_WORD _count           # In HC x86-atom using "FETCH rINST"
                                        # would be equivalent
    movzwl  \_count*2(rPC),rINST
.endm

/*
 * Fetch instruction word indexed (used for branching).
 * Index is in instruction word units.
 */
.macro FETCH_INST_INDEXED _reg  # In HC x86-atom there is no translation for this.
    movzwl  (rPC,\_reg,2),rINST
.endm

/*
 * Advance rPC by instruction count
 */
.macro ADVANCE_PC _count        # In HC x86-atom this was done during FETCH_ADV_INST but
                                # that macro also updated rINST
    leal  2*\_count(rPC),rPC
.endm

/*
 * Advance rPC by branch offset in register
 */
.macro ADVANCE_PC_INDEXED _reg  # In HC x86-atom there is no translation for this.
    leal (rPC,\_reg,2),rPC
.endm

.macro GOTO_NEXT                # In HC x86-atom this is partly implemented in
                                # FINISH_A. Essentially jumps to the next instruction
                                # given rINST holds the next opcode. Also prepares
                                # rINST.
     movzx   rINSTbl,%eax
     movzbl  rINSTbh,rINST
     jmp     *(rIBASE,%eax,4)
.endm

   /*
    * Version of GOTO_NEXT that assumes _reg preloaded with opcode.
    * Should be paired with FETCH_INST_R
    */
.macro GOTO_NEXT_R _reg         # In HC x86-atom this is partly implemented in
                                # FINISH_RB. Essentially jumps to the next instruction
                                # given rINST holds the next opcode. Also prepares
                                # rINST.
     movzbl  1(rPC),rINST
     jmp     *(rIBASE,\_reg,4)
.endm

   /*
    * Jumbo version of GOTO_NEXT that assumes _reg preloaded with table
    * offset of the jumbo instruction, which is the top half of the extended
    * opcode + 0x100.  Loads rINST with BBBB field, similar to GOTO_NEXT_R
    */
.macro GOTO_NEXT_JUMBO_R _reg   # No equivalent in HC
     movzwl  6(rPC),rINST
     jmp     *(rIBASE,\_reg,4)
.endm

/*
 * Get/set the 32-bit value from a Dalvik register.
 */
.macro GET_VREG_R _reg _vreg    # Equivalent to GET_VREG for HC x86-atom
    movl     (rFP,\_vreg,4),\_reg
.endm

.macro SET_VREG _reg _vreg      # Equivalent in HC x86-atom
    movl     \_reg,(rFP,\_vreg,4)
.endm

.macro GET_VREG_WORD _reg _vreg _offset # No equivalent in HC x86-atom
    movl     4*(\_offset)(rFP,\_vreg,4),\_reg
.endm

.macro SET_VREG_WORD _reg _vreg _offset # No equivalent in HC x86-atom
    movl     \_reg,4*(\_offset)(rFP,\_vreg,4)
.endm

#ifdef PROFILE_OPCODE

/* Profile opcodes  */
        .type   fiprint, %function
  fiprint:
        pushl   %eax
        pushl   %ecx
        pushl   %edx
        pushl   (rPC)
        call    fastinterptouch
        popl    %edx
        popl    %edx
        popl    %ecx
        popl    %eax
        ret
#endif

#define sReg0 LOCAL0_OFFSET(%ebp)
#define sReg1 LOCAL1_OFFSET(%ebp)
#define sReg2 LOCAL2_OFFSET(%ebp)

   /*
    * x86-atom JIT Helpers
    */

    .macro dumpSwitch _regData _regScratch1 _regScratch2
#ifdef DEBUG_CALL_STACK3
    movl        rGLUE, \_regScratch2
    //use %ebx, %ecx, update numSwitch & switchPoints
    movl        offGlue_numSwitch(\_regScratch2), \_regScratch1
    lea         (\_regScratch2, \_regScratch1, 4), \_regScratch2
    //numSwitch*4 + offGlue_switchPoints + _regGlue
    movl        \_regData, offGlue_switchPoints(\_regScratch2)
    //update offGlue_numSwitch(%edx) (+1)&0xff
    addl        $$1, \_regScratch1
    movl        rGLUE, \_regScratch2
    and         $$0x00ff, \_regScratch1
    movl        \_regScratch1, offGlue_numSwitch(\_regScratch2)
#endif
    .endm

   /*
    * Hard coded helper values.
    */

.balign 16

.LdoubNeg:
    .quad       0x8000000000000000

.L64bits:
    .quad       0xFFFFFFFFFFFFFFFF

.LshiftMask2:
    .quad       0x0000000000000000
.LshiftMask:
    .quad       0x000000000000003F

.Lvalue64:
    .quad       0x0000000000000040

.LvaluePosInfLong:
    .quad       0x7FFFFFFFFFFFFFFF

.LvalueNegInfLong:
    .quad       0x8000000000000000

.LvalueNanLong:
    .quad       0x0000000000000000

.LintMin:
.long   0x80000000

.LintMax:
.long   0x7FFFFFFF


/*
 * This is a #include, not a %include, because we want the C pre-processor
 * to expand the macros into assembler assignment statements.
 */
#include "../common/asm-constants.h"

#if defined(WITH_JIT)
#include "../common/jit-config.h"
#endif
