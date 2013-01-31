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


/*! \file GlueOpt.cpp
    \brief This file updates gen & kill for a bytecode
*/
#ifndef WITH_JIT
#include "libdex/DexOpcodes.h"
#include "libdex/DexFile.h"
#include "Lower.h"
#ifdef NCG_O1
#include "AnalysisO1.h"

//glue optimization
//  GEN: set of glue variables that are used
//  KILL: (resetGlue)
//  IN: set of glue variables that are in memory at the entry point
//  ALL glue variables: pDvmDex, pResFields, pResStrings
//  a glue variable is in register for the GEN set, when a glue variable

//for bytecodes with both gen & kill, first gen then kill
void killAllGlueVariables(int* killSet) {
  killSet[0] = RES_CLASS;
  killSet[1] = RES_METHOD;
  killSet[2] = RES_FIELD;
  killSet[3] = RES_STRING;
  killSet[4] = GLUE_DVMDEX;
  killSet[5] = GLUE_METHOD_CLASS;
  killSet[6] = GLUE_METHOD;
  num_regs_kill = 7;
}
//RES_CLASS: get_res_classes
//RES_METHOD: get_res_method
//RES_FIELD: get_res_fields
//RES_STRING: get_res_string
//GLUE_DVMDEX: get_glue_dvmdex
//GLUE_METHOD_CLASS: get_glue_method_class
//GLUE_METHOD: get_glue_method
void getGlueInfo(int* genSet, int* killSet) {
    u2 inst_op = INST_INST(inst);
    u2 vA;
    num_regs_gen = 0;
    num_regs_kill = 0;

    switch (inst_op) {

    case OP_CONST_STRING:
    case OP_CONST_STRING_JUMBO:
      genSet[0] = RES_STRING;
      num_regs_gen = 1;
      break;
    case OP_CONST_CLASS:
      genSet[0] = RES_CLASS;
      num_regs_gen = 1;
      break;

    case OP_CHECK_CAST:
    case OP_INSTANCE_OF:
    case OP_NEW_ARRAY:
    case OP_NEW_INSTANCE:
    case OP_FILLED_NEW_ARRAY: //update return value
    case OP_FILLED_NEW_ARRAY_RANGE: //use "length" virtual registers
      genSet[0] = RES_CLASS;
      num_regs_gen = 1;
      break;
#if 0
    case OP_THROW_VERIFICATION_ERROR:
      genSet[0] = GLUE_METHOD;
      num_regs_gen = 1;
      break;
#endif
    case OP_IGET: //iget_iput_common_nohelper
    case OP_IGET_WIDE:
    case OP_IGET_OBJECT:
    case OP_IGET_BOOLEAN:
    case OP_IGET_BYTE:
    case OP_IGET_CHAR:
    case OP_IGET_SHORT:
    case OP_IPUT:
    case OP_IPUT_WIDE:
    case OP_IPUT_OBJECT:
    case OP_IPUT_BOOLEAN:
    case OP_IPUT_BYTE:
    case OP_IPUT_CHAR:
    case OP_IPUT_SHORT:
    case OP_SGET:
    case OP_SGET_WIDE:
    case OP_SGET_OBJECT:
    case OP_SGET_BOOLEAN:
    case OP_SGET_BYTE:
    case OP_SGET_CHAR:
    case OP_SGET_SHORT:
    case OP_SPUT:
    case OP_SPUT_WIDE:
    case OP_SPUT_OBJECT:
    case OP_SPUT_BOOLEAN:
    case OP_SPUT_BYTE:
    case OP_SPUT_CHAR:
    case OP_SPUT_SHORT:
      genSet[0] = RES_FIELD;
      num_regs_gen = 1;
      break;

    case OP_INVOKE_VIRTUAL: //nullcheck
    case OP_INVOKE_VIRTUAL_RANGE: //nullcheck
    case OP_INVOKE_DIRECT: //nullcheck
    case OP_INVOKE_DIRECT_RANGE: //nullcheck
    case OP_INVOKE_STATIC:
    case OP_INVOKE_STATIC_RANGE:
      genSet[0] = RES_METHOD;
      num_regs_gen = 1;
      killAllGlueVariables(killSet);
      break;
    case OP_INVOKE_SUPER:
    case OP_INVOKE_SUPER_RANGE:
      genSet[0] = RES_METHOD;
      genSet[1] = GLUE_METHOD_CLASS;
      num_regs_gen = 2;
      killAllGlueVariables(killSet);
      break; //methods & method_class
    case OP_INVOKE_INTERFACE: //nullcheck
    case OP_INVOKE_INTERFACE_RANGE: //nullcheck
      genSet[0] = GLUE_DVMDEX;
      genSet[1] = GLUE_METHOD;
      num_regs_gen = 2;
      killAllGlueVariables(killSet);
      break; //use get_glue_dvmdex & get_glue_method
    case OP_INVOKE_VIRTUAL_QUICK: //nullcheck
    case OP_INVOKE_VIRTUAL_QUICK_RANGE: //nullcheck
      killAllGlueVariables(killSet);
      break;
    case OP_INVOKE_SUPER_QUICK: //nullcheck TODO
    case OP_INVOKE_SUPER_QUICK_RANGE: //nullcheck TODO
      genSet[0] = GLUE_METHOD_CLASS;
      num_regs_gen = 1;
      killAllGlueVariables(killSet);
      break;//get_glue_method_class
    default:
      break;
    }
    return;
}
#endif
#endif
