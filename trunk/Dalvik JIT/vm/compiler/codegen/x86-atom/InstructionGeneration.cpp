/*
 * Copyright (C) 2010-2011 Intel Corporation
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

#include "Dalvik.h"
#include "InstructionGeneration.h"
#include "libdex/DexOpcodes.h"
#include "compiler/Compiler.h"
#include "compiler/CompilerIR.h"
#include "interp/Jit.h"
#include "libdex/DexFile.h"
#include "Lower.h"
#include "NcgAot.h"
#include "compiler/codegen/CompilerCodegen.h"

#define P_GPR_1 PhysicalReg_EBX
#define P_GPR_2 PhysicalReg_ECX
/**
 * @brief Generate a Null check
 * @param cUnit the CompilationUnit
 * @param mir the MIR instruction
 */
void genHoistedNullCheck (CompilationUnit *cUnit, MIR *mir)
{
    /*
     * NOTE: these synthesized blocks don't have ssa names assigned
     * for Dalvik registers.  However, because they dominate the following
     * blocks we can simply use the Dalvik name w/ subscript 0 as the
     * ssa name.
     */
    /* Assign array in virtual register to P_GPR_1 */
    get_virtual_reg(mir->dalvikInsn.vA, OpndSize_32, P_GPR_1, true);
    export_pc();
    compare_imm_reg(OpndSize_32, 0, P_GPR_1, true);
    condJumpToBasicBlock(stream, Condition_E, cUnit->exceptionBlockId);
}

/**
 * @brief Generate a Bound check:
      vA arrayReg
      arg[0] -> determines whether it is a constant or a register
      arg[1] -> register or constant

      is idx < 0 || idx >= array.length ?
 * @param cUnit the CompilationUnit
 * @param mir the MIR instruction
 */
void genHoistedBoundCheck (CompilationUnit *cUnit, MIR *mir)
{
    /* Assign array in virtual register to P_GPR_1 */
    get_virtual_reg(mir->dalvikInsn.vA, OpndSize_32, P_GPR_1, true);

    if (mir->dalvikInsn.arg[0] == MIR_BOUND_CHECK_REG)
    {
        //Ok let us get the index in P_GPR_2
        get_virtual_reg(mir->dalvikInsn.arg[1], OpndSize_32, P_GPR_2, true);
    }
    else
    {
        //Move the constant to P_GPR_2
        move_imm_to_reg(OpndSize_32, mir->dalvikInsn.arg[1], P_GPR_2, true);
    }
    export_pc();

    //Compare array length with index value
    compare_mem_reg(OpndSize_32, offArrayObject_length, P_GPR_1, true, P_GPR_2, true);
    //Jump to exception block if array.length <= index
    condJumpToBasicBlock(stream, Condition_LE, cUnit->exceptionBlockId);

    //Now, compare to 0
    compare_imm_reg(OpndSize_32, 0, P_GPR_2, true);
    //Jump to exception if index < 0
    condJumpToBasicBlock(stream, Condition_L, cUnit->exceptionBlockId);
}

//use O0 code generator for hoisted checks outside of the loop
/*
 * vA = arrayReg;
 * vB = idxReg;
 * vC = endConditionReg;
 * arg[0] = maxC
 * arg[1] = minC
 * arg[2] = loopBranchConditionCode
 */
void genHoistedChecksForCountUpLoop(CompilationUnit *cUnit, MIR *mir)
{
    /*
     * NOTE: these synthesized blocks don't have ssa names assigned
     * for Dalvik registers.  However, because they dominate the following
     * blocks we can simply use the Dalvik name w/ subscript 0 as the
     * ssa name.
     */
    DecodedInstruction *dInsn = &mir->dalvikInsn;
    const int maxC = dInsn->arg[0];

    //First do the null check
    genHoistedNullCheck (cUnit, mir);

    /* assign index in virtual register to P_GPR_2 */
    get_virtual_reg(mir->dalvikInsn.vC, OpndSize_32, P_GPR_2, true);

    int delta = maxC;
    /*
     * If the loop end condition is ">=" instead of ">", then the largest value
     * of the index is "endCondition - 1".
     */
    if (dInsn->arg[2] == OP_IF_GE) {
        delta--;
    }

    if (delta < 0) { //+delta
        //if P_GPR_2 is mapped to a VR, we can't do this
        alu_binary_imm_reg(OpndSize_32, sub_opc, -delta, P_GPR_2, true);
    } else if(delta > 0) {
        alu_binary_imm_reg(OpndSize_32, add_opc, delta, P_GPR_2, true);
    }
    compare_mem_reg(OpndSize_32, offArrayObject_length, P_GPR_1, true, P_GPR_2, true);
    condJumpToBasicBlock(stream, Condition_NC, cUnit->exceptionBlockId);
}

/*
 * vA = arrayReg;
 * vB = idxReg;
 * vC = endConditionReg;
 * arg[0] = maxC
 * arg[1] = minC
 * arg[2] = loopBranchConditionCode
 */
void genHoistedChecksForCountDownLoop(CompilationUnit *cUnit, MIR *mir)
{
    DecodedInstruction *dInsn = &mir->dalvikInsn;
    const int maxC = dInsn->arg[0];

    //First do the null check
    genHoistedNullCheck (cUnit, mir);

    /* assign index in virtual register to P_GPR_2 */
    get_virtual_reg(mir->dalvikInsn.vB, OpndSize_32, P_GPR_2, true);

    if (maxC < 0) {
        //if P_GPR_2 is mapped to a VR, we can't do this
        alu_binary_imm_reg(OpndSize_32, sub_opc, -maxC, P_GPR_2, true);
    } else if(maxC > 0) {
        alu_binary_imm_reg(OpndSize_32, add_opc, maxC, P_GPR_2, true);
    }
    compare_mem_reg(OpndSize_32, offArrayObject_length, P_GPR_1, true, P_GPR_2, true);
    condJumpToBasicBlock(stream, Condition_NC, cUnit->exceptionBlockId);

}

#undef P_GPR_1
#undef P_GPR_2

/*
 * vA = idxReg;
 * vB = minC;
 */
#define P_GPR_1 PhysicalReg_ECX
void genHoistedLowerBoundCheck(CompilationUnit *cUnit, MIR *mir)
{
    DecodedInstruction *dInsn = &mir->dalvikInsn;
    const int minC = dInsn->vB;
    get_virtual_reg(mir->dalvikInsn.vA, OpndSize_32, P_GPR_1, true); //array
    export_pc();
    compare_imm_reg(OpndSize_32, -minC, P_GPR_1, true);
    condJumpToBasicBlock(stream, Condition_C, cUnit->exceptionBlockId);
}
#undef P_GPR_1

#ifdef WITH_JIT_INLINING_PHASE2
void genValidationForPredictedInline(CompilationUnit *cUnit, MIR *mir)
{
    CallsiteInfo *callsiteInfo = mir->meta.callsiteInfo;
    if(gDvm.executionMode == kExecutionModeNcgO0) {
        get_virtual_reg(mir->dalvikInsn.vC, OpndSize_32, PhysicalReg_EBX, true);
        move_imm_to_reg(OpndSize_32, (int) callsiteInfo->clazz, PhysicalReg_ECX, true);
        compare_imm_reg(OpndSize_32, 0, PhysicalReg_EBX, true);
        export_pc(); //use %edx
        conditional_jump_global_API(, Condition_E, "common_errNullObject", false);
        move_mem_to_reg(OpndSize_32, offObject_clazz, PhysicalReg_EBX, true, PhysicalReg_EAX, true);
        compare_reg_reg(PhysicalReg_ECX, true, PhysicalReg_EAX, true);
    } else {
        get_virtual_reg(mir->dalvikInsn.vC, OpndSize_32, 5, false);
        move_imm_to_reg(OpndSize_32, (int) callsiteInfo->clazz, 4, false);
        nullCheck(5, false, 1, mir->dalvikInsn.vC);
        move_mem_to_reg(OpndSize_32, offObject_clazz, 5, false, 6, false);
        compare_reg_reg(4, false, 6, false);
    }

    //immdiate will be updated later in genLandingPadForMispredictedCallee
    streamMisPred = stream;
    callsiteInfo->misPredBranchOver = (LIR*)conditional_jump_int(Condition_NE, 0, OpndSize_8);
}
#endif
