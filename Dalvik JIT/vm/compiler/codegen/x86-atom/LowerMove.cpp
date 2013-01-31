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


/*! \file LowerMove.cpp
    \brief This file lowers the following bytecodes: MOVE_XXX
*/
#include "libdex/DexOpcodes.h"
#include "libdex/DexFile.h"
#include "Lower.h"
#include "enc_wrapper.h"

#define P_GPR_1 PhysicalReg_EBX

/**
 * @brief Generate native code for bytecodes move and
 * move-object
 * @param mir bytecode representation
 * @return value >= 0 when handled
 */
int op_move(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_MOVE
            || mir->dalvikInsn.opcode == OP_MOVE_OBJECT);
    u2 vA = mir->dalvikInsn.vA;
    u2 vB = mir->dalvikInsn.vB;
    get_virtual_reg(vB, OpndSize_32, 1, false/*isPhysical*/);
    set_virtual_reg(vA, OpndSize_32, 1, false);
    return 2;
}

/**
 * @brief Generate native code for bytecodes move/from16
 * and move-object/from16
 * @param mir bytecode representation
 * @return value >= 0 when handled
 */
int op_move_from16(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_MOVE_FROM16
            || mir->dalvikInsn.opcode == OP_MOVE_OBJECT_FROM16);
    u2 vA = mir->dalvikInsn.vA;
    u2 vB = mir->dalvikInsn.vB;
    get_virtual_reg(vB, OpndSize_32, 1, false);
    set_virtual_reg(vA, OpndSize_32, 1, false);
    return 2;
}

/**
 * @brief Generate native code for bytecodes move/16 and
 * move-object/16
 * @param mir bytecode representation
 * @return value >= 0 when handled
 */
int op_move_16(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_MOVE_16
            || mir->dalvikInsn.opcode == OP_MOVE_OBJECT_16);
    u2 vA = mir->dalvikInsn.vA;
    u2 vB = mir->dalvikInsn.vB;
    get_virtual_reg(vB, OpndSize_32, 1, false);
    set_virtual_reg(vA, OpndSize_32, 1, false);
    return 2;
}
#undef P_GPR_1

/**
 * @brief Generate native code for bytecode move-wide
 * @param mir bytecode representation
 * @return value >= 0 when handled
 */
int op_move_wide(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_MOVE_WIDE);
    u2 vA = mir->dalvikInsn.vA;
    u2 vB = mir->dalvikInsn.vB;
    get_virtual_reg(vB, OpndSize_64, 1, false);
    set_virtual_reg(vA, OpndSize_64, 1, false);
    return 2;
}

/**
 * @brief Generate native code for bytecode move-wide/from16
 * @param mir bytecode representation
 * @return value >= 0 when handled
 */
int op_move_wide_from16(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_MOVE_WIDE_FROM16);
    u2 vA = mir->dalvikInsn.vA;
    u2 vB = mir->dalvikInsn.vB;
    get_virtual_reg(vB, OpndSize_64, 1, false);
    set_virtual_reg(vA, OpndSize_64, 1, false);
    return 2;
}

/**
 * @brief Generate native code for bytecode move-wide/16
 * @param mir bytecode representation
 * @return value >= 0 when handled
 */
int op_move_wide_16(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_MOVE_WIDE_16);
    u2 vA = mir->dalvikInsn.vA;
    u2 vB = mir->dalvikInsn.vB;
    get_virtual_reg(vB, OpndSize_64, 1, false);
    set_virtual_reg(vA, OpndSize_64, 1, false);
    return 2;
}

/**
 * @brief Generate native code for bytecodes move-result
 * and move-result-object
 * @param mir bytecode representation
 * @return value >= 0 when handled
 */
int op_move_result(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_MOVE_RESULT
            || mir->dalvikInsn.opcode == OP_MOVE_RESULT_OBJECT);
#ifdef WITH_JIT_INLINING
    /* An inlined move result is effectively no-op */
    if (mir->OptimizationFlags & MIR_INLINED)
        return 0;
#endif
    u2 vA = mir->dalvikInsn.vA;
    scratchRegs[0] = PhysicalReg_SCRATCH_1;
    get_return_value(OpndSize_32, 1, false);
    set_virtual_reg(vA, OpndSize_32, 1, false);
    return 0;
}

/**
 * @brief Generate native code for bytecode move-result-wide
 * @param mir bytecode representation
 * @return value >= 0 when handled
 */
int op_move_result_wide(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_MOVE_RESULT_WIDE);
#ifdef WITH_JIT_INLINING
    /* An inlined move result is effectively no-op */
    if (mir->OptimizationFlags & MIR_INLINED)
        return 0;
#endif
    u2 vA = mir->dalvikInsn.vA;
    scratchRegs[0] = PhysicalReg_SCRATCH_1;
    get_return_value(OpndSize_64, 1, false);
    set_virtual_reg(vA, OpndSize_64, 1, false);
    return 0;
}

#define P_GPR_1 PhysicalReg_EBX
#define P_GPR_2 PhysicalReg_ECX

/**
 * @brief Generate native code for bytecode move-exception
 * @details Updates virtual register with exception from Thread and then
 * clear the exception from Thread.
 * @param mir bytecode representation
 * @return value >= 0 when handled
 */
int op_move_exception(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_MOVE_EXCEPTION);
    u2 vA = mir->dalvikInsn.vA;
    scratchRegs[2] = PhysicalReg_Null; scratchRegs[3] = PhysicalReg_Null;
    scratchRegs[0] = PhysicalReg_SCRATCH_1; scratchRegs[1] = PhysicalReg_Null;
    get_self_pointer(2, false);
    move_mem_to_reg(OpndSize_32, offThread_exception, 2, false, 3, false);
    move_imm_to_mem(OpndSize_32, 0, offThread_exception, 2, false);
    set_virtual_reg(vA, OpndSize_32, 3, false);
    return 0;
}
#undef P_GPR_1
#undef P_GPR_2

