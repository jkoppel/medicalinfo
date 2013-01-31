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



#ifndef _DALVIK_TRANSLATOR
#define _DALVIK_TRANSLATOR

#include "Dalvik.h"
#include "enc_wrapper.h"

int ncgClass(ClassObject* clazz,  const DexClassDef* pClassDef, int classIdx, int fd, int* stats);
/* initialization for NCG O0 & O1 */
void initNCG(const char* curFileName, DvmDex *pDvmDex);
/* initialization for trace-based JIT */
void initJIT(const char* curFileName, DvmDex *pDvmDex);
void freeNCG();
int clearNCG(int fd);
int createCFGOfClass(ClassObject* clazz);

#endif
