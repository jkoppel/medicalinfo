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

#ifndef H_DALVIK_INSTRUCTIONGENERATION
#define H_DALVIK_INSTRUCTIONGENERATION

#include "Dalvik.h"

/**
 * @brief Generate a Null check
 * @param cUnit the CompilationUnit
 * @param mir the MIR instruction
 */
void genHoistedNullCheck (CompilationUnit *cUnit, MIR *mir);

/**
 * @brief Generate a Bound check:
      vA arrayReg
      arg[0] -> determines whether it is a constant or a register
      arg[1] -> register or constant

      is idx < 0 || idx >= array.length ?
 * @param cUnit the CompilationUnit
 * @param mir the MIR instruction
 */
void genHoistedBoundCheck (CompilationUnit *cUnit, MIR *mir);

//use O0 code generator for hoisted checks outside of the loop
/*
 * @brief Generate the null and upper bound check for a count up loop
 * vA = arrayReg;
 * vB = idxReg;
 * vC = endConditionReg;
 * arg[0] = maxC
 * arg[1] = minC
 * arg[2] = loopBranchConditionCode
 * @param cUnit the CompilationUnit
 * @param mir the MIR instruction
 */
void genHoistedChecksForCountUpLoop(CompilationUnit *cUnit, MIR *mir);

/*
 * @brief Generate the null and upper bound check for a count down loop
 * vA = arrayReg;
 * vB = idxReg;
 * vC = endConditionReg;
 * arg[0] = maxC
 * arg[1] = minC
 * arg[2] = loopBranchConditionCode
 * @param cUnit the CompilationUnit
 * @param mir the MIR instruction
 */
void genHoistedChecksForCountDownLoop(CompilationUnit *cUnit, MIR *mir);

/*
 * @brief Generate the lower bound check
 * vA = arrayReg;
 * vB = minimum constant used for the array;
 * @param cUnit the CompilationUnit
 * @param mir the MIR instruction
 */
void genHoistedLowerBoundCheck(CompilationUnit *cUnit, MIR *mir);

/*
 * @brief Generate the validation for a predicted inline
 * vC actual class
 * @param cUnit the CompilationUnit
 * @param mir the MIR instruction
 */
void genValidationForPredictedInline(CompilationUnit *cUnit, MIR *mir);
#endif
