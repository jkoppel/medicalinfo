/*
* Copyright (C) 2012 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef DALVIK_VM_LOOPOPT_H_
#define DALVIK_VM_LOOPOPT_H_

#include "Dalvik.h"
#include "CompilerInternals.h"

/**
  * @class SPass
  * @brief SPass is the Pass structure for the optimizations
  * The following structure has the different optimization passes that we are going to do
  */
typedef struct sPass
{
    /** @brief Pass name */
    const char *passName;

    /** @brief Type of traversal */
    DataFlowAnalysisMode traversalType;

    /** @brief Specific data for the pass */
    void *data;

    /** @brief Gate for the pass, taking the CompilationUnit and the pass information */
    bool (*gate) (const CompilationUnit *cUnit, const struct sPass *curPass);

    /** @brief Start of the pass function */
    void (*start) (CompilationUnit *cUnit, struct sPass *curPass);

    /** @brief End of the pass function */
    void (*end) (CompilationUnit *cUnit, struct sPass *curPass);

    /** @brief Per basic block work
     *  @return returns whether the BasicBlock has been changed
     */
    bool (*doWork) (CompilationUnit *, BasicBlock *bb);

    /** @brief Flags for clean-up */
    unsigned int flags;
}SPass;

/**
  * @brief The loop optimization driver: calls each pass from the gPasses array
  * @param cUnit the Compilation Unit
  */
void dvmCompilerLoopOptimizationDriver (CompilationUnit *cUnit);

#endif
