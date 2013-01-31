/*
 * Copyright (C) 2009 The Android Open Source Project
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

#ifndef DALVIK_VM_LOOP_H_
#define DALVIK_VM_LOOP_H_

#include "Dalvik.h"
#include "CompilerInternals.h"

typedef struct LoopAnalysis {
    BitVector *isIndVarV;               // length == numSSAReg
    GrowableList *ivList;               // induction variables
    GrowableList *arrayAccessInfo;      // hoisted checks for array accesses
    int numBasicIV;                     // number of basic induction variables
    int ssaBIV;                         // basic IV in SSA name
    bool isCountUpLoop;                 // count up or down loop
    Opcode loopBranchOpcode;            // OP_IF_XXX for the loop back branch
    int loopBranchMIROffset;            // MIR offset in method for loop back branch
    int endConditionReg;                // vB in "vA op vB"
    LIR *branchToBody;                  // branch over to the body from entry
    LIR *branchToPCR;                   // branch over to the PCR cell
    bool bodyIsClean;                   // loop body cannot throw any exceptions

#ifdef ARCH_IA32
    void *passData;                     // Pass data
    bool isSimpleCountedLoop;           // Is the loop simple counted
#endif
} LoopAnalysis;

bool dvmCompilerFilterLoopBlocks(CompilationUnit *cUnit);

/*
 * An unexecuted code path may contain unresolved fields or classes. Before we
 * have a quiet resolver we simply bail out of the loop compilation mode.
 */
#define BAIL_LOOP_COMPILATION() if (cUnit->jitMode == kJitLoop) {       \
                                    cUnit->quitLoopMode = true;         \
                                    return false;                       \
                                }

#ifdef ARCH_IA32
//Externalizing certain Loop.cpp functionalities
/**
 * @brief Whether or not the loop is a simple counted loop
 * @param cUnit the CompilationUnit
 * @return whether or not the loop is a simple counted loop
 */
bool dvmCompilerSimpleCountedLoop (CompilationUnit *cUnit);

/**
 * @brief Handle the detection of Induction Variable Array Accesses
 * @param cUnit the CompilationUnit
 * @return whether or not the optimization has been performed
 */
bool dvmCompilerIVArrayAccess (CompilationUnit *cUnit);

/**
 * @brief Hoist the access checks that are detected by the dvmCompielrIVArrayAccess function
 * @param cUnit the CompilationUnit
 */
void dvmCompilerHoistIVArrayAccess (CompilationUnit *cUnit);

/**
 * @brief Dump the Induction Variable list, only if DEBUG_LOOP is defined in Loop.cpp
 * @param cUnit the CompilationUnit
 */
void dvmCompilerDumpIVList (CompilationUnit *cUnit);

/**
 * @brief Dump the detected constants, only if DEBUG_LOOP is defined in Loop.cpp
 * @param cUnit the CompilationUnit
 */
void dvmCompilerDumpConstants (CompilationUnit *cUnit);

/**
 * @brief Dump the hoisted checks, only if DEBUG_LOOP is defined in Loop.cpp
 * @param cUnit the CompilationUnit
 */
void dvmCompilerDumpHoistedChecks (CompilationUnit *cUnit);
#endif
#endif  // DALVIK_VM_LOOP_H_
