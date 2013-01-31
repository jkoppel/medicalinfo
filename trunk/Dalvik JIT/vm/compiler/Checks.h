/*
 * Copyright (C) 2012 Intel Corporation
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

#ifndef DALVIK_VM_CHECKS_H_
#define DALVIK_VM_CHECKS_H_

#include "Dalvik.h"

//Forward declaration
struct sPass;

/**
 * @brief Remove redundant checks start function
 * @param cUnit the CompilationUnit
 * @param curPass the current pass SPass
 */
void dvmCompilerStartCheckRemoval (CompilationUnit *cUnit, struct sPass *curPass);

/**
 * @brief Remove redundant checks end function
 * @param cUnit the CompilationUnit
 * @param curPass the current pass struct sPass
 */
void dvmCompilerEndCheckRemoval (CompilationUnit *cUnit, struct sPass *curPass);

/**
 * @brief Remove redundant checks
 * @param cUnit the CompilationUnit
 * @param bb the BasicBlock
 * @return returns whether we changed anything in the BasicBlock
 */
bool dvmCompilerCheckRemoval (CompilationUnit *cUnit, BasicBlock *bb);

#endif
