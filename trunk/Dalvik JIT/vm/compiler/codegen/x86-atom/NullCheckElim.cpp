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


/*! \file NullCheckElim.cpp
    \brief This file updates gen & kill for a bytecode
*/
#ifndef WITH_JIT
#include "libdex/DexOpcodes.h"
#include "libdex/DexFile.h"
#include "Lower.h"
#ifdef NCG_O1
#include "AnalysisO1.h"
//null check elimination
//  GEN: set of variables on which nullCheck is called during lowering
//  KILL: set of variables that are updated
//  IN: set of variable on which nullCheck has been called at the entry point
//  IN(X) = union(OUT(Y)) for all predecessors Y of X
//  OUT(X) = GEN(X) + (IN(X) - KILL(X))
//  if(old_OUT != OUT(X)) change = 1
//  null_check_visitor: returns GEN & KILL for each bytecode

void getNullCheckInfo(int* genSet, int* killSet) {
    u2 inst_op = INST_INST(inst);
    u2 vA, vB, vref, vD;
    num_regs_gen = 0;
    num_regs_kill = 0;

    switch (inst_op) {
    case OP_MOVE:
    case OP_MOVE_OBJECT:
    case OP_MOVE_FROM16:
    case OP_MOVE_OBJECT_FROM16:
    case OP_MOVE_16:
    case OP_MOVE_OBJECT_16:
        if(inst_op == OP_MOVE || inst_op == OP_MOVE_OBJECT) {
            vA = INST_A(inst);
        }
        else if(inst_op == OP_MOVE_FROM16 || inst_op == OP_MOVE_OBJECT_FROM16) {
            vA = INST_AA(inst);
        }
        else if(inst_op == OP_MOVE_16 || inst_op == OP_MOVE_OBJECT_16) {
            vA = FETCH(1);
        }
	killSet[0] = vA;
	num_regs_kill = 1;
        break;
    case OP_MOVE_WIDE:
    case OP_MOVE_WIDE_FROM16:
    case OP_MOVE_WIDE_16:
        if(inst_op == OP_MOVE_WIDE) {
            vA = INST_A(inst);
        }
        else if(inst_op == OP_MOVE_WIDE_FROM16) {
            vA = INST_AA(inst);
        }
        else if(inst_op == OP_MOVE_WIDE_16) {
            vA = FETCH(1);
        }
	killSet[0] = vA;
	killSet[1] = vA+1;
	num_regs_kill = 2;
        break;
    case OP_MOVE_RESULT: //access memory
    case OP_MOVE_RESULT_OBJECT:
        vA = INST_AA(inst);
	killSet[0] = vA;
	num_regs_kill = 1;
        break;
    case OP_MOVE_RESULT_WIDE: //note: 2 destinations
        vA = INST_AA(inst);
	killSet[0] = vA;
	killSet[1] = vA+1;
	num_regs_kill = 2;
        break;
    case OP_MOVE_EXCEPTION: //access memory
        vA = INST_AA(inst);
	killSet[0] = vA;
	num_regs_kill = 1;
        break;

    case OP_CONST_4:
        vA = INST_A(inst);
	killSet[0] = vA;
	num_regs_kill = 1;
        break;
    case OP_CONST_16:
    case OP_CONST:
    case OP_CONST_HIGH16:
        vA = INST_AA(inst);
	killSet[0] = vA;
	num_regs_kill = 1;
        break;
    case OP_CONST_WIDE_16:
    case OP_CONST_WIDE_32:
    case OP_CONST_WIDE:
    case OP_CONST_WIDE_HIGH16:
        vA = INST_AA(inst);
	killSet[0] = vA;
	killSet[1] = vA+1;
	num_regs_kill = 2;
        break;
    case OP_CONST_STRING:
    case OP_CONST_STRING_JUMBO:
    case OP_CONST_CLASS:
        vA = INST_AA(inst);
	killSet[0] = vA;
	num_regs_kill = 1;
        break;

    case OP_MONITOR_ENTER: //nullCheck on vA
    case OP_MONITOR_EXIT: //nullCheck
        vA = INST_AA(inst);
	genSet[0] = vA;
	num_regs_gen = 1;
        break;
    case OP_ARRAY_LENGTH: //nullCheck
        vA = INST_A(inst);
	vB = INST_B(inst);
	killSet[0] = vA;
	num_regs_kill = 1;
	genSet[0] = vB;
	num_regs_gen = 1;
        break;
    case OP_INSTANCE_OF:
    case OP_NEW_ARRAY:
        vA = INST_A(inst);
	killSet[0] = vA;
	num_regs_kill = 1;
        break;
    case OP_NEW_INSTANCE:
        vA = INST_AA(inst);
	killSet[0] = vA;
	num_regs_kill = 1;
        break;
    case OP_THROW: //update glue->exception nullcheck TODO
        vA = INST_AA(inst);
	genSet[0] = vA;
	num_regs_gen = 1;
        break;

    case OP_CMPL_FLOAT: //move 32 bits from memory to lower part of XMM register
    case OP_CMPG_FLOAT:
        vA = INST_AA(inst);
	killSet[0] = vA;
	num_regs_kill = 1;
        break;
    case OP_CMPL_DOUBLE: //move 64 bits from memory to lower part of XMM register
    case OP_CMPG_DOUBLE:
    case OP_CMP_LONG: //load v1, v1+1, v2, v2+1 to gpr
        vA = INST_AA(inst);
	killSet[0] = vA;
	num_regs_kill = 1;
        break;

    case OP_AGET: //aget_common_nohelper
    case OP_AGET_WIDE:
    case OP_AGET_OBJECT:
    case OP_AGET_BOOLEAN: //movez 8
    case OP_AGET_BYTE: //moves 8
    case OP_AGET_CHAR: //movez 16
    case OP_AGET_SHORT: //moves 16
        vA = INST_AA(inst);
	vref = FETCH(1) & 0xff;
        if(inst_op == OP_AGET_WIDE) {
	    killSet[0] = vA;
	    killSet[1] = vA+1;
	    num_regs_kill = 2;
        } else {
	    killSet[0] = vA;
	    num_regs_kill = 1;
        }
	genSet[0] = vref;
	num_regs_gen = 1;
        break;
    case OP_APUT://aput_common_nohelper
    case OP_APUT_WIDE:
    case OP_APUT_OBJECT: //nullcheck TODO
    case OP_APUT_BOOLEAN:
    case OP_APUT_BYTE:
    case OP_APUT_CHAR:
    case OP_APUT_SHORT:
        vref = FETCH(1) & 0xff;
	genSet[0] = vref;
	num_regs_gen = 1;
        break;

    case OP_IGET: //iget_iput_common_nohelper
    case OP_IGET_WIDE:
    case OP_IGET_OBJECT:
    case OP_IGET_BOOLEAN:
    case OP_IGET_BYTE:
    case OP_IGET_CHAR:
    case OP_IGET_SHORT:
    case OP_IGET_QUICK: //nullCheck
    case OP_IGET_WIDE_QUICK://nullCheck
    case OP_IGET_OBJECT_QUICK://nullCheck
        vA = INST_A(inst);
        vB = INST_B(inst);
        if(inst_op == OP_IGET_WIDE || inst_op == OP_IGET_WIDE_QUICK) {
	    killSet[0] = vA;
	    killSet[1] = vA+1;
	    num_regs_kill = 2;
        } else {
	    killSet[0] = vA;
	    num_regs_kill = 1;
        }
	genSet[0] = vB;
	num_regs_gen = 1;
        break;
    case OP_IPUT:
    case OP_IPUT_WIDE:
    case OP_IPUT_OBJECT:
    case OP_IPUT_BOOLEAN:
    case OP_IPUT_BYTE:
    case OP_IPUT_CHAR:
    case OP_IPUT_SHORT:
    case OP_IPUT_QUICK: //nullCheck
    case OP_IPUT_WIDE_QUICK://nullCheck
    case OP_IPUT_OBJECT_QUICK://nullCheck
        vB = INST_B(inst);
	genSet[0] = vB;
	num_regs_gen = 1;
        break;
    case OP_SGET:
    case OP_SGET_WIDE:
    case OP_SGET_OBJECT:
    case OP_SGET_BOOLEAN:
    case OP_SGET_BYTE:
    case OP_SGET_CHAR:
    case OP_SGET_SHORT:
        vA = INST_AA(inst);
        if(inst_op == OP_SGET_WIDE) {
	    killSet[0] = vA;
	    killSet[1] = vA+1;
	    num_regs_kill = 2;
        } else {
	    killSet[0] = vA;
	    num_regs_kill = 1;
        }
        break;

    case OP_INVOKE_VIRTUAL: //nullcheck
    case OP_INVOKE_DIRECT: //nullcheck
    case OP_INVOKE_INTERFACE: //nullcheck
    case OP_INVOKE_VIRTUAL_QUICK: //nullcheck
    case OP_INVOKE_SUPER_QUICK: //nullcheck TODO
      vD = FETCH(2) & 0xf;
      genSet[0] = vD;
      num_regs_gen = 1;
      break;
    case OP_INVOKE_VIRTUAL_RANGE: //nullcheck
    case OP_INVOKE_DIRECT_RANGE: //nullcheck
    case OP_INVOKE_INTERFACE_RANGE: //nullcheck
    case OP_INVOKE_VIRTUAL_QUICK_RANGE: //nullcheck
    case OP_INVOKE_SUPER_QUICK_RANGE: //nullcheck TODO
      vD = FETCH(2);
      genSet[0] = vD;
      num_regs_gen = 1;
      break;

    case OP_NEG_INT:
    case OP_NOT_INT:
    case OP_NEG_FLOAT:
        vA = INST_A(inst); //destination
	killSet[0] = vA;
	num_regs_kill = 1;
        break;
    case OP_NEG_LONG:
    case OP_NOT_LONG:
    case OP_NEG_DOUBLE:
        vA = INST_A(inst); //destination
	killSet[0] = vA;
	killSet[1] = vA+1;
	num_regs_kill = 2;
        break;
    case OP_INT_TO_LONG: //hard-coded registers
        vA = INST_A(inst); //destination
	killSet[0] = vA;
	killSet[1] = vA+1;
	num_regs_kill = 2;
        break;
    case OP_INT_TO_FLOAT:
    case OP_INT_TO_DOUBLE: //32 to 64
    case OP_LONG_TO_FLOAT: //64 to 32
    case OP_LONG_TO_DOUBLE:
    case OP_FLOAT_TO_DOUBLE:
        vA = INST_A(inst); //destination
        if(inst_op == OP_INT_TO_DOUBLE || inst_op == OP_LONG_TO_DOUBLE || inst_op == OP_FLOAT_TO_DOUBLE) {
	  killSet[0] = vA;
	  killSet[1] = vA+1;
	  num_regs_kill = 2;
	}
        else {
	  killSet[0] = vA;
	  num_regs_kill = 1;
	}
        break;
    case OP_LONG_TO_INT:
    case OP_FLOAT_TO_INT:
    case OP_DOUBLE_TO_INT: //for reaching-def analysis
        vA = INST_A(inst); //destination
	killSet[0] = vA;
	num_regs_kill = 1;
        break;
    case OP_FLOAT_TO_LONG:
    case OP_DOUBLE_TO_LONG:
        vA = INST_A(inst); //destination
	killSet[0] = vA;
	killSet[1] = vA+1;
	num_regs_kill = 2;
        break;
    case OP_INT_TO_BYTE:
    case OP_INT_TO_CHAR:
    case OP_INT_TO_SHORT:
        vA = INST_A(inst); //destination
	killSet[0] = vA;
	num_regs_kill = 1;
        break;

    case OP_ADD_INT:
    case OP_SUB_INT:
    case OP_MUL_INT:
    case OP_AND_INT:
    case OP_OR_INT:
    case OP_XOR_INT:
    case OP_DIV_INT:
    case OP_REM_INT:
    case OP_SHL_INT:
    case OP_SHR_INT:
    case OP_USHR_INT:
        vA = INST_AA(inst);
	killSet[0] = vA;
	num_regs_kill = 1;
        break;
    case OP_ADD_LONG:
    case OP_SUB_LONG:
    case OP_AND_LONG:
    case OP_OR_LONG:
    case OP_XOR_LONG:
    case OP_MUL_LONG: //used int
    case OP_DIV_LONG: //v1: xmm v2,vA:
    case OP_REM_LONG:
    case OP_SHL_LONG: //v2: 32, move_ss; v1,vA: xmm CHECK
    case OP_SHR_LONG: //v2: 32, move_ss; v1,vA: xmm CHECK
    case OP_USHR_LONG: //v2: move_ss; v1,vA: move_sd
        vA = INST_AA(inst);
	killSet[0] = vA;
	killSet[1] = vA+1;
	num_regs_kill = 2;
        break;
    case OP_ADD_FLOAT: //move_ss
    case OP_SUB_FLOAT:
    case OP_MUL_FLOAT:
    case OP_DIV_FLOAT: //fp_stack
    case OP_REM_FLOAT: //32 bit GPR, fp_stack for output
        vA = INST_AA(inst);
	killSet[0] = vA;
	num_regs_kill = 1;
        break;
    case OP_ADD_DOUBLE: //move_sd
    case OP_SUB_DOUBLE:
    case OP_MUL_DOUBLE:
    case OP_DIV_DOUBLE: //fp_stack
    case OP_REM_DOUBLE: //64 bit XMM, fp_stack for output
        vA = INST_AA(inst);
	killSet[0] = vA;
	killSet[1] = vA+1;
	num_regs_kill = 2;
        break;

    case OP_ADD_INT_2ADDR:
    case OP_SUB_INT_2ADDR:
    case OP_MUL_INT_2ADDR:
    case OP_AND_INT_2ADDR:
    case OP_OR_INT_2ADDR:
    case OP_XOR_INT_2ADDR:
    case OP_DIV_INT_2ADDR:
    case OP_REM_INT_2ADDR:
    case OP_SHL_INT_2ADDR:
    case OP_SHR_INT_2ADDR:
    case OP_USHR_INT_2ADDR:
        vA = INST_A(inst);
	killSet[0] = vA;
	num_regs_kill = 1;
        break;
    case OP_ADD_LONG_2ADDR:
    case OP_SUB_LONG_2ADDR:
    case OP_AND_LONG_2ADDR:
    case OP_OR_LONG_2ADDR:
    case OP_XOR_LONG_2ADDR:
    case OP_MUL_LONG_2ADDR:
    case OP_DIV_LONG_2ADDR: //vA used as xmm, then updated as gps
    case OP_REM_LONG_2ADDR:
    case OP_SHL_LONG_2ADDR:
    case OP_SHR_LONG_2ADDR:
    case OP_USHR_LONG_2ADDR:
        vA = INST_A(inst);
	killSet[0] = vA;
	killSet[1] = vA+1;
	num_regs_kill = 2;
        break;
    case OP_ADD_FLOAT_2ADDR:
    case OP_SUB_FLOAT_2ADDR:
    case OP_MUL_FLOAT_2ADDR:
    case OP_DIV_FLOAT_2ADDR:
    case OP_REM_FLOAT_2ADDR: //load vA as GPR, store from fs
        vA = INST_A(inst);
	killSet[0] = vA;
	num_regs_kill = 1;
        break;
    case OP_ADD_DOUBLE_2ADDR:
    case OP_SUB_DOUBLE_2ADDR:
    case OP_MUL_DOUBLE_2ADDR:
    case OP_DIV_DOUBLE_2ADDR:
    case OP_REM_DOUBLE_2ADDR: //load to xmm, store from fs
        vA = INST_A(inst);
	killSet[0] = vA;
	killSet[1] = vA+1;
	num_regs_kill = 2;
        break;

    case OP_ADD_INT_LIT16:
    case OP_RSUB_INT:
    case OP_MUL_INT_LIT16:
    case OP_AND_INT_LIT16:
    case OP_OR_INT_LIT16:
    case OP_XOR_INT_LIT16:
    case OP_DIV_INT_LIT16:
    case OP_REM_INT_LIT16:
        vA = INST_A(inst);
	killSet[0] = vA;
	num_regs_kill = 1;
        break;
    case OP_ADD_INT_LIT8:
    case OP_RSUB_INT_LIT8:
    case OP_MUL_INT_LIT8:
    case OP_AND_INT_LIT8:
    case OP_OR_INT_LIT8:
    case OP_XOR_INT_LIT8:
    case OP_SHL_INT_LIT8:
    case OP_SHR_INT_LIT8:
    case OP_USHR_INT_LIT8:
    case OP_DIV_INT_LIT8:
    case OP_REM_INT_LIT8:
        vA = INST_AA(inst);
	killSet[0] = vA;
	num_regs_kill = 1;
        break;
    default:
        break;
    }
    return;
}
#endif
#endif
