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


/*! \file Lower.cpp
    \brief This file implements the high-level wrapper for lowering

*/

//#include "uthash.h"
#include "libdex/DexOpcodes.h"
#include "libdex/DexFile.h"
#include <math.h>
#include <sys/mman.h>
#include "Translator.h"
#include "Lower.h"
#include "enc_wrapper.h"
#include "vm/mterp/Mterp.h"
#include "NcgHelper.h"
#include "libdex/DexCatch.h"
#if defined(WITH_JIT)
#include "compiler/CompilerIR.h"
#endif
#if defined VTUNE_DALVIK
#include "compiler/JitProfiling.h"
#endif

//statistics for optimization
int num_removed_nullCheck;

PhysicalReg scratchRegs[4];

LowOp* ops[BUFFER_SIZE];
LowOp* op;
u2* rPC; //PC pointer to bytecode
int offsetPC/*offset in bytecode*/, offsetNCG/*byte offset in native code*/;
int ncg_rPC;
//! map from PC in bytecode to PC in native code
int mapFromBCtoNCG[BYTECODE_SIZE_PER_METHOD]; //initially mapped to -1
char* streamStart = NULL; //start of the Pure CodeItem?, not include the global symbols
char* streamCode = NULL; //start of the Pure CodeItem?, not include the global symbols
char* streamMethodStart; //start of the method
char* stream; //current stream pointer
int lowOpTimeStamp = 0;
Method* currentMethod = NULL;
#if defined(WITH_JIT)
int currentExceptionBlockIdx = -1;
LowOpBlockLabel* traceLabelList = NULL;
BasicBlock* traceCurrentBB = NULL;
JitMode traceMode = kJitTrace;
bool branchInLoop = false;
#endif

int common_invokeArgsDone(ArgsDoneType, bool);

//data section of .ia32:
char globalData[128];

char strClassCastException[] = "Ljava/lang/ClassCastException;";
char strInstantiationError[] = "Ljava/lang/InstantiationError;";
char strInternalError[] = "Ljava/lang/InternalError;";
char strFilledNewArrayNotImpl[] = "filled-new-array only implemented for 'int'";
char strArithmeticException[] = "Ljava/lang/ArithmeticException;";
char strArrayIndexException[] = "Ljava/lang/ArrayIndexOutOfBoundsException;";
char strArrayStoreException[] = "Ljava/lang/ArrayStoreException;";
char strDivideByZero[] = "divide by zero";
char strNegativeArraySizeException[] = "Ljava/lang/NegativeArraySizeException;";
char strNoSuchMethodError[] = "Ljava/lang/NoSuchMethodError;";
char strNullPointerException[] = "Ljava/lang/NullPointerException;";
char strStringIndexOutOfBoundsException[] = "Ljava/lang/StringIndexOutOfBoundsException;";

int LstrClassCastExceptionPtr, LstrInstantiationErrorPtr, LstrInternalError, LstrFilledNewArrayNotImpl;
int LstrArithmeticException, LstrArrayIndexException, LstrArrayStoreException, LstrStringIndexOutOfBoundsException;
int LstrDivideByZero, LstrNegativeArraySizeException, LstrNoSuchMethodError, LstrNullPointerException;
int LdoubNeg, LvaluePosInfLong, LvalueNegInfLong, LvalueNanLong, LshiftMask, Lvalue64, L64bits, LintMax, LintMin;

void initConstDataSec() {
    char* tmpPtr = globalData;

    LdoubNeg = (int)tmpPtr;
    *((u4*)tmpPtr) = 0x00000000;
    tmpPtr += sizeof(u4);
    *((u4*)tmpPtr) = 0x80000000;
    tmpPtr += sizeof(u4);

    LvaluePosInfLong = (int)tmpPtr;
    *((u4*)tmpPtr) = 0xFFFFFFFF;
    tmpPtr += sizeof(u4);
    *((u4*)tmpPtr) = 0x7FFFFFFF;
    tmpPtr += sizeof(u4);

    LvalueNegInfLong = (int)tmpPtr;
    *((u4*)tmpPtr) = 0x00000000;
    tmpPtr += sizeof(u4);
    *((u4*)tmpPtr) = 0x80000000;
    tmpPtr += sizeof(u4);

    LvalueNanLong = (int)tmpPtr;
    *((u4*)tmpPtr) = 0;
    tmpPtr += sizeof(u4);
    *((u4*)tmpPtr) = 0;
    tmpPtr += sizeof(u4);

    LshiftMask = (int)tmpPtr;
    *((u4*)tmpPtr) = 0x3f;
    tmpPtr += sizeof(u4);
    *((u4*)tmpPtr) = 0;
    tmpPtr += sizeof(u4);

    Lvalue64 = (int)tmpPtr;
    *((u4*)tmpPtr) = 0x40;
    tmpPtr += sizeof(u4);
    *((u4*)tmpPtr) = 0;
    tmpPtr += sizeof(u4);

    L64bits = (int)tmpPtr;
    *((u4*)tmpPtr) = 0xFFFFFFFF;
    tmpPtr += sizeof(u4);
    *((u4*)tmpPtr) = 0xFFFFFFFF;
    tmpPtr += sizeof(u4);

    LintMin = (int)tmpPtr;
    *((u4*)tmpPtr) = 0x80000000;
    tmpPtr += sizeof(u4);

    LintMax = (int)tmpPtr;
    *((u4*)tmpPtr) = 0x7FFFFFFF;
    tmpPtr += sizeof(u4);

    LstrClassCastExceptionPtr = (int)strClassCastException;
    LstrInstantiationErrorPtr = (int)strInstantiationError;
    LstrInternalError = (int)strInternalError;
    LstrFilledNewArrayNotImpl = (int)strFilledNewArrayNotImpl;
    LstrArithmeticException = (int)strArithmeticException;
    LstrArrayIndexException = (int)strArrayIndexException;
    LstrArrayStoreException = (int)strArrayStoreException;
    LstrDivideByZero = (int)strDivideByZero;
    LstrNegativeArraySizeException = (int)strNegativeArraySizeException;
    LstrNoSuchMethodError = (int)strNoSuchMethodError;
    LstrNullPointerException = (int)strNullPointerException;
    LstrStringIndexOutOfBoundsException = (int)strStringIndexOutOfBoundsException;
}

//declarations of functions used in this file
int spill_reg(int reg, bool isPhysical);
int unspill_reg(int reg, bool isPhysical);

int const_string_resolve();
int sget_sput_resolve();
int new_instance_needinit();
int new_instance_abstract();
int invoke_virtual_resolve();
int invoke_direct_resolve();
int invoke_static_resolve();
int filled_new_array_notimpl();
int resolve_class2(
                   int startLR/*logical register index*/, bool isPhysical, int indexReg/*const pool index*/,
                   bool indexPhysical,
                   int thirdArg);
int resolve_method2(
                    int startLR/*logical register index*/, bool isPhysical, int indexReg/*const pool index*/,
                    bool indexPhysical,
                    int thirdArg/*VIRTUAL*/);
int resolve_inst_field2(
                        int startLR/*logical register index*/, bool isPhysical,
                        int indexReg/*const pool index*/,
                        bool indexPhysical);
int resolve_static_field2(
                          int startLR/*logical register index*/, bool isPhysical,
                          int indexReg/*const pool index*/,
                          bool indexPhysical);

int invokeMethodNoRange_1_helper();
int invokeMethodNoRange_2_helper();
int invokeMethodNoRange_3_helper();
int invokeMethodNoRange_4_helper();
int invokeMethodNoRange_5_helper();
int invokeMethodRange_helper();

int invoke_virtual_helper();
int invoke_virtual_quick_helper();
int invoke_static_helper();
int invoke_direct_helper();
int new_instance_helper();
int sget_sput_helper(int flag);
int aput_obj_helper();
int aget_helper(int flag);
int aput_helper(int flag);
int monitor_enter_helper();
int monitor_exit_helper();
int throw_helper();
int const_string_helper();
int array_length_helper();
int invoke_super_helper();
int invoke_interface_helper();
int iget_iput_helper(int flag);
int check_cast_helper(bool instance);
int new_array_helper();

int common_returnFromMethod();

/*!
\brief dump helper functions

*/
int performCGWorklist() {
    filled_new_array_notimpl();
    freeShortMap();
    const_string_resolve();
    freeShortMap();

    resolve_class2(PhysicalReg_EAX, true, PhysicalReg_EAX, true, 0);
    freeShortMap();
    resolve_method2(PhysicalReg_EAX, true, PhysicalReg_EAX, true, METHOD_VIRTUAL);
    freeShortMap();
    resolve_method2(PhysicalReg_EAX, true, PhysicalReg_EAX, true, METHOD_DIRECT);
    freeShortMap();
    resolve_method2(PhysicalReg_EAX, true, PhysicalReg_EAX, true, METHOD_STATIC);
    freeShortMap();
#ifdef INC_NCG_O0
    if(gDvm.helper_switch[0])
        common_returnFromMethod();
    freeShortMap();
    if(gDvm.helper_switch[1])
        invoke_virtual_helper();
    freeShortMap();
    if(gDvm.helper_switch[2])
        invoke_static_helper();
    freeShortMap();
    if(gDvm.helper_switch[3])
        invoke_virtual_quick_helper();
    freeShortMap();
    if(gDvm.helper_switch[4])
        new_instance_helper();
    freeShortMap();
    if(gDvm.helper_switch[5]) {
        sget_sput_helper(SGET);
        freeShortMap();
        sget_sput_helper(SGET_WIDE);
        freeShortMap();
        sget_sput_helper(SPUT);
        freeShortMap();
        sget_sput_helper(SPUT_WIDE);
        freeShortMap();
    }
    if(gDvm.helper_switch[6])
        aput_obj_helper();
    freeShortMap();
    if(gDvm.helper_switch[7]) {
        aget_helper(AGET);
        freeShortMap();
        aget_helper(AGET_WIDE);
        freeShortMap();
        aget_helper(AGET_CHAR);
        freeShortMap();
        aget_helper(AGET_SHORT);
        freeShortMap();
        aget_helper(AGET_BOOLEAN);
        freeShortMap();
        aget_helper(AGET_BYTE);
        freeShortMap();
    }
    if(gDvm.helper_switch[18]) {
        aput_helper(APUT);
        freeShortMap();
        aput_helper(APUT_WIDE);
        freeShortMap();
        aput_helper(APUT_CHAR);
        freeShortMap();
        aput_helper(APUT_SHORT);
        freeShortMap();
        aput_helper(APUT_BOOLEAN);
        freeShortMap();
        aput_helper(APUT_BYTE);
        freeShortMap();
    }
    if(gDvm.helper_switch[8]) {
        check_cast_helper(false);
        freeShortMap();
        check_cast_helper(true);
        freeShortMap();
    }
    if(gDvm.helper_switch[9]) {
        iget_iput_helper(IGET);
        freeShortMap();
        iget_iput_helper(IGET_WIDE);
        freeShortMap();
        iget_iput_helper(IPUT);
        freeShortMap();
        iget_iput_helper(IPUT_WIDE);
        freeShortMap();
    }
    if(gDvm.helper_switch[10])
        invoke_direct_helper();
    freeShortMap();
    if(gDvm.helper_switch[11]) {
        monitor_enter_helper();
        freeShortMap();
        monitor_exit_helper();
        freeShortMap();
    }

    if(gDvm.helper_switch[12])
        throw_helper();
    freeShortMap();
    if(gDvm.helper_switch[13])
        const_string_helper();
    freeShortMap();
    if(gDvm.helper_switch[14])
        array_length_helper();
    freeShortMap();
    if(gDvm.helper_switch[15])
        invoke_super_helper();
    freeShortMap();
    if(gDvm.helper_switch[16])
        invoke_interface_helper();
    freeShortMap();
    if(gDvm.helper_switch[17])
        new_array_helper();
    freeShortMap();
    invokeMethodRange_helper();
    freeShortMap();
    invokeMethodNoRange_1_helper();
    freeShortMap();
    invokeMethodNoRange_2_helper();
    freeShortMap();
    invokeMethodNoRange_3_helper();
    freeShortMap();
    invokeMethodNoRange_4_helper();
    freeShortMap();
    invokeMethodNoRange_5_helper();
    freeShortMap();
#endif

    resolve_inst_field2(PhysicalReg_EAX, true, PhysicalReg_EAX, true);
    freeShortMap();
    resolve_static_field2(PhysicalReg_EAX, true, PhysicalReg_EAX, true);
    freeShortMap();
    throw_exception_message(PhysicalReg_ECX, PhysicalReg_EAX, true, PhysicalReg_Null, true);
    freeShortMap();
    throw_exception(PhysicalReg_ECX, PhysicalReg_EAX, PhysicalReg_Null, true);
    freeShortMap();
    new_instance_needinit();
    freeShortMap();
    return 0;
}

int aput_object_count;
int common_periodicChecks_entry();
int common_periodicChecks4();

#if 0 /* Commented out because it is dead code. If re-enabling, this needs to be updated
         to take MIR as parameter */
/*!
\brief for debugging purpose, dump the sequence of native code for each bytecode

*/
int ncgMethodFake(Method* method) {
    //to measure code size expansion, no need to patch up labels
    methodDataWorklist = NULL;
    globalShortWorklist = NULL;
    globalNCGWorklist = NULL;
    streamMethodStart = stream;

    //initialize mapFromBCtoNCG
    memset(&mapFromBCtoNCG[0], -1, BYTECODE_SIZE_PER_METHOD * sizeof(mapFromBCtoNCG[0]));
    unsigned int i;
    u2* rStart = (u2*)malloc(5*sizeof(u2));
    if(rStart == NULL) {
        LOGE("Memory allocation failed");
        return -1;
    }
    rPC = rStart;
    method->insns = rStart;
    for(i = 0; i < 5; i++) *rPC++ = 0;
    for(i = 0; i < 256; i++) {
        rPC = rStart;
        //modify the opcode
        char* tmp = (char*)rStart;
        *tmp++ = i;
        *tmp = i;
        inst = FETCH(0);
        char* tmpStart = stream;
        lowerByteCode(method); //use inst, rPC, method, modify rPC
        int size_in_u2 = rPC - rStart;
        if(stream - tmpStart  > 0)
            LOGI("LOWER bytecode %x size in u2: %d ncg size in byte: %d\n", i, size_in_u2, stream - tmpStart);
    }
    exit(0);
}
#endif

/*!
\brief NCG O0 for a method

*/
#ifdef INC_NCG_O0
int codeGenO0(Method* method) {
    u2 insnsSize = dvmGetMethodInsnsSize(method);
    u2* insnEnd = rPC + insnsSize;
    while(rPC < insnEnd) {
        inst = FETCH(0);
        globalShortMap = NULL;

        if(stream - globalDvmNcg->streamStart >= globalDvmNcg->streamEnd - globalDvmNcg->streamStart) {
            LOGI("ERROR ERROR: exceeds code size\n");
            exit(-1);
        }

        int retCode = lowerByteCode(method); //update rPC
        freeShortMap();
        if(retCode < 0) {
            return retCode;
        }
        //update insnEnd when handling bytecodes with data section
        DataWorklist* ptr = methodDataWorklist;
        if(ptr != NULL) {
            int endPC = ptr->offsetPC + ptr->relativePC;
            insnEnd = (u2*)method->insns + endPC;
        }
        if (FREE_METHOD_CODE_BUFFER_SIZE() < MAX_BINARY_SIZE_PER_BYTECODE)
            return -1;
    }
    return 0;
}
#endif

/*!
\brief code generation for a method

Depending on the global flag "gDvm.executionMode", either NCG O0 or NCG O1 will be invoked
*/
#ifdef INC_NCG_O0
int codeGen(Method* method) {
    rPC = (u2*)method->insns;
    streamMethodStart = stream;
    //initialize mapFromBCtoNCG
    memset(&mapFromBCtoNCG[0], -1, BYTECODE_SIZE_PER_METHOD * sizeof(mapFromBCtoNCG[0]));

    if(gDvm.executionMode == kExecutionModeNcgO1) {
        if (codeGenCFG(method) == -1)
            return -1;
    } else {
        if (codeGenO0(method) == -1)
            return -1;
    }

    performNCGWorklist(); //handle forward jump (GOTO, IF)
    performDataWorklist(); //handle SWITCH & FILL_ARRAY_DATA
    return 0;
}
#endif

bool existATryBlock(Method* method, int startPC, int endPC) {
    const DexCode* pCode = dvmGetMethodCode(method);
    u4 triesSize = pCode->triesSize;
    const DexTry* pTries = dexGetTries(pCode);
    unsigned int i;
    for (i = 0; i < triesSize; i++) {
        const DexTry* pTry = &pTries[i];
        u4 start = pTry->startAddr; //offsetPC
        u4 end = start + pTry->insnCount;
        //if [start, end] overlaps with [startPC, endPC] returns true
        if((int)end < startPC || (int)start > endPC) { //no overlap
        } else {
            return true;
        }
    }
    return false;
}

int mm_bytecode_size = 0;
int mm_ncg_size = 0;
int mm_relocation_size = 0;
int mm_map_size = 0;
void resetCodeSize() {
    mm_bytecode_size = 0;
    mm_ncg_size = 0;
    mm_relocation_size = 0;
    mm_map_size = 0;
}

bool bytecodeIsRemoved(const Method* method, u4 bytecodeOffset) {
    if(gDvm.executionMode == kExecutionModeNcgO0) return false;
    u4 ncgOff = mapFromBCtoNCG[bytecodeOffset];
    int k = bytecodeOffset+1;
    u2 insnsSize = dvmGetMethodInsnsSize(method);
    while(k < insnsSize) {
        if(mapFromBCtoNCG[k] < 0) {
            k++;
            continue;
        }
        if(mapFromBCtoNCG[k] == (int)ncgOff) return true;
        return false;
    }
    return false;
}
/*!
\brief wrapper to generate code for a method

It calls codeGen to generate native code, it also updates other sections associated with a method in the DEX file.
*/
#ifdef INC_NCG_O0
int ncgMethod(Method* method, int* stats) {
#ifdef DEBUG_EACH_BYTECODE
    return ncgMethodFake(method);
#endif
    dump_x86_inst = false;
    //if(!strcmp(method->name, "main")) dump_x86_inst = true;
    stream = globalDvmNcg->curStream;

    currentMethod = method;
    methodDataWorklist = NULL;
    aput_object_count = 0;
    globalShortWorklist = NULL;
    globalNCGWorklist = NULL;

    if(gDvm.executionMode == kExecutionModeNcgO1) {
        //to disable generating native code for certain methods, should return -1 not 0
        //if(strcmp(method->name, "getNextTokenFromStream")) return -1;
    }
#ifdef REGISTER_DEBUG
    LOGI("************Start of ncgMethod %s in class %s encoding ************\n", method->name, method->clazz->descriptor);
#endif
#ifdef DEBUG_NCG_CODE_SIZE
    LOGE("START method @ %p with name %s%s\n", method, method->clazz->descriptor, method->name);
#endif
#if 1//def DEBUG_NCG
    if(dump_x86_inst) LOGI("call ncg on method %s::%s\n", method->clazz->descriptor, method->name);
#endif

    //update code_item
    const DexCode* pCode = dvmGetMethodCode(method);
    u4 triesSize = pCode->triesSize;

    *((u2*)stream) = method->registersSize;
    stream += 2;
    *((u2*)stream) = method->insSize;
    stream += 2;
    *((u2*)stream) = method->outsSize;
    stream += 2;
    *((u2*)stream) = pCode->triesSize;
    stream += 2;
    char* new_debug_info_field = stream;
    *((u4*)stream) = pCode->debugInfoOff;
    stream += 4;
    *((u4*)stream) = 0; //patch up later
    stream += 4;

    u2 insnsSize = dvmGetMethodInsnsSize(method);
    char* beforeCodeGen = stream;
    if (codeGen(method) == -1) {
        LOGI("code generation error for method %s%s\n", method->clazz->descriptor, method->name);
        return -1;
    }

    if (FREE_METHOD_CODE_BUFFER_SIZE() < globalDvmNcg->ExceptAndDebugInfoSize) {
        return -2;
    }
    stats[0] = insnsSize*2;
    stats[1] = stream - beforeCodeGen;
    globalDvmNcg->streamCode = streamMethodStart;
    globalDvmNcg->methodSize= (u4)(stream - streamMethodStart);

    //update DexTry and DexCatchHandler
    if(triesSize > 0) { //padding
        int pad_bytes = ((u4)stream) & 3; //ncg method
        if(pad_bytes > 0) {
            pad_bytes = 4 - pad_bytes;
        }
        while(pad_bytes > 0) {
            *stream = 0;
            stream++;
            pad_bytes--;
        }
    }
    const DexTry* pTries = dexGetTries(pCode);
    DexTry* pTriesNew = (DexTry*)stream;
    int i;
    u4 j;
    for (i = 0; i < triesSize; i++) {
        DexTry tryNew;
        //update startAddr, insnCount, handlerOff
        const DexTry* pTry = &pTries[i];
        u4 start = pTry->startAddr; //offsetPC
        u4 end = start + pTry->insnCount;
        tryNew.startAddr = mapFromBCtoNCG[start];
        tryNew.insnCount = mapFromBCtoNCG[end] - tryNew.startAddr;
        tryNew.handlerOff = pTry->handlerOff; //CHECK
        memcpy(stream, &tryNew, sizeof(DexTry));
        stream += sizeof(DexTry);
    }
    const u1* baseData = dexGetCatchHandlerData(pCode);
    const u1* handlerStartOld = baseData;
    char* handlerStartNew = stream;
    u4 handlersSize = 0;

    if(triesSize > 0) {
        handlersSize = readUnsignedLeb128(&baseData);
        memcpy(stream, handlerStartOld, baseData-handlerStartOld); //number of entries
        stream += (baseData-handlerStartOld);
    }
    //map old offest to new offset
    int mapHandlerOffset[MAX_HANDLER_OFFSET];
    memset(mapHandlerOffset, 0, sizeof(int)*MAX_HANDLER_OFFSET);
    //catch-all is converted to an extra DexCatchHandler when using dexCatchIteratorNext
    //update DexTry.handlerOff
    for (i = 0; i < handlersSize; i++) {
        mapHandlerOffset[baseData - handlerStartOld] = stream - handlerStartNew;
        const u1* tmpOld = baseData;
        int tOffset = baseData - handlerStartOld;
        s4 tSize = readSignedLeb128(&baseData);
#ifdef DEBUG_NCG_1
        LOGI("update handler at offset %d size %d offset_new %d\n", tOffset, tSize, stream - handlerStartNew);
#endif
        memcpy(stream, tmpOld, baseData-tmpOld);
        stream += (baseData-tmpOld);
        bool hasCatchAll = (tSize <= 0);
        if(tSize < 0) {
            tSize = 0-tSize;
        }
        for(j = 0; j < (int)tSize; j++) {
            s4 tmp = readUnsignedLeb128(&baseData);
            writeUnsignedLeb128x((u1**)(&stream), tmp);
            tmp = readUnsignedLeb128(&baseData);
#if defined(ENABLE_TRACING) && !defined(TRACING_OPTION2)
            insertMapWorklist(tmp, mapFromBCtoNCG[tmp], 1);
#endif
            writeUnsignedLeb128x((u1**)(&stream), mapFromBCtoNCG[tmp]); //address
        }
        if(hasCatchAll) {
            s4 tmp = readUnsignedLeb128(&baseData);
#if defined(ENABLE_TRACING) && !defined(TRACING_OPTION2)
            insertMapWorklist(tmp, mapFromBCtoNCG[tmp], 1);
#endif
            writeUnsignedLeb128x((u1**)(&stream), mapFromBCtoNCG[tmp]); //address
        }
    }
    for (i = 0; i < triesSize; i++) {
        pTriesNew[i].handlerOff = mapHandlerOffset[pTries[i].handlerOff];
    }

    //////////////////////DEBUG
    const DexCode* pCodeNew = dvmGetMethodCode(method);
    const DexTry* pTmp = dexGetTries(pCodeNew);
    //offset relative to the start of the "handlers" section
    u4 offset = dexGetFirstHandlerOffset(pCodeNew);
    for (i = 0; i < handlersSize; i++) {
        DexCatchIterator iterator;
        dexCatchIteratorInit(&iterator, pCodeNew, offset);
        //update iterator, last is catch_all
        for (;;) {
            DexCatchHandler* handler = dexCatchIteratorNext(&iterator);
            if (handler == NULL) {
                break;
            }
        }
        offset = dexCatchIteratorGetEndOffset(&iterator, pCodeNew);
    }

    /////////////////////// update debug_info_item ////////////
    char* stream_debug_info_start = stream;
    const u1 *stream_old = dexGetDebugInfoStream(method->clazz->pDvmDex->pDexFile, pCodeNew);
    if (stream_old == NULL) {
        //method->insns = (u2*)streamMethodStart;
        globalDvmNcg->curStream = stream;
        return 0;
    }

    {
        u4 line;
        u4 parametersSize;
        const u1* name_start = stream_old;
        line = readUnsignedLeb128(&stream_old);
        parametersSize = readUnsignedLeb128(&stream_old);
        memcpy(stream, name_start, stream_old-name_start);
        stream += (stream_old-name_start);

        name_start = stream_old;
        while (parametersSize-- != 0) {
            readUnsignedLeb128(&stream_old);
        }
        memcpy(stream, name_start, stream_old-name_start);
        stream += (stream_old-name_start);

        u4 address = 0;
        u4 addr_native = 0;
        for (;;)  {
            u1 opcode = *stream_old++;
            u2 reg;
            switch (opcode) {
            case DBG_END_SEQUENCE:
                *stream = opcode;
                stream++;
                goto end;
            case DBG_ADVANCE_LINE:
                *stream = opcode;
                stream++;
                name_start = stream_old;
                readSignedLeb128(&stream_old);
                memcpy(stream, name_start, stream_old-name_start);
                stream += (stream_old-name_start);
                break;
            case DBG_START_LOCAL:
            case DBG_START_LOCAL_EXTENDED:
                *stream = opcode;
                stream++;
                name_start = stream_old;
                readUnsignedLeb128(&stream_old);
                readUnsignedLeb128(&stream_old);
                readUnsignedLeb128(&stream_old);
                if (opcode == DBG_START_LOCAL_EXTENDED) {
                    readUnsignedLeb128(&stream_old);
                }
                memcpy(stream, name_start, stream_old-name_start);
                stream += (stream_old-name_start);
                break;

            case DBG_END_LOCAL:
            case DBG_RESTART_LOCAL:
                *stream = opcode;
                stream++;
                name_start = stream_old;
                readUnsignedLeb128(&stream_old);
                memcpy(stream, name_start, stream_old-name_start);
                stream += (stream_old-name_start);
                break;

            case DBG_SET_PROLOGUE_END:
            case DBG_SET_EPILOGUE_BEGIN:
            case DBG_SET_FILE:
                *stream = opcode;
                stream++;
                break;

            case DBG_ADVANCE_PC:
                *stream = opcode;
                stream++;
                u4 tSize = readUnsignedLeb128(&stream_old);
                u4 newAddr = address + tSize;
                u4 newAddr_native = mapFromBCtoNCG[newAddr];
                writeUnsignedLeb128x((u1**)(&stream), newAddr_native-addr_native);
                address = newAddr;
                addr_native = newAddr_native;
                break;
            default: {
                int adjopcode = opcode - DBG_FIRST_SPECIAL;
                u4 newAddr = address + adjopcode / DBG_LINE_RANGE;
                s4 lineInc = DBG_LINE_BASE + (adjopcode % DBG_LINE_RANGE);
                u4 newAddr_native = mapFromBCtoNCG[newAddr];
                /* if the bytecode at newAddr is optimized away, (meaning newAddr->newNcg, newAddr is for line1
                   the next bytecode newAddr2 -> newNcg, newAddr2 is for line2), we need to replace the
                   special opcode to advance_pc & advance_line
                   The purpose is to remove the entries that maps newNcg to line number corresponding to newAddr
                */
                if(bytecodeIsRemoved(method, newAddr)) {
                    if(newAddr_native - addr_native > 0) {
                        *stream = DBG_ADVANCE_PC;
                        stream++;
                        writeUnsignedLeb128x((u1**)(&stream), newAddr_native-addr_native);
                    }
                    if(lineInc > 0) {
                        *stream = DBG_ADVANCE_LINE;
                        stream++;
                        writeSignedLeb128x((u1**)(&stream), lineInc);
                    }
                } else {
                    u4 tmpMod = lineInc - DBG_LINE_BASE;
                    u4 newOpcode = (newAddr_native-addr_native)*DBG_LINE_RANGE+DBG_FIRST_SPECIAL+tmpMod;
                    //addr_native + adjopcode / DBG_LINE_RANGE == newAddr_native
                    if(newOpcode > 255) {
                        u4 addr_native_t = newAddr_native - (255-tmpMod-DBG_FIRST_SPECIAL)/DBG_LINE_RANGE;
                        *stream = DBG_ADVANCE_PC;
                        stream++;
                        writeUnsignedLeb128x((u1**)(&stream), addr_native_t-addr_native);
                        addr_native = addr_native_t;
                        newOpcode = (newAddr_native-addr_native)*DBG_LINE_RANGE+DBG_FIRST_SPECIAL+tmpMod;
                    }
                    *stream = newOpcode;
                    stream++;
                }
                address = newAddr;
                addr_native = newAddr_native;
                break;
            }
            }
        }

    end:
        *((u4*)new_debug_info_field) = stream_debug_info_start - (char*)(method->clazz->pDvmDex->pDexFile->baseAddr);
    }

    globalDvmNcg->curStream = stream;
    //method->insns = (u2*)streamMethodStart;

    return 0;
}
#endif

int invoke_super_nsm();
/*!
\brief free the memory allocated to store the native code

*/
void freeNCG() {
}

#ifdef INC_NCG_O0
static void initDvmNcg(DvmDex *pDvmDex)
{
    globalDvmNcg = (DvmNcg *)malloc(sizeof(DvmNcg));
    if(globalDvmNcg == NULL) {
        LOGE("Memory allocation failed");
        return;
    }
    memset(globalDvmNcg, 0, sizeof(DvmNcg));

    ncgClassNum = 0;
    ncgMethodNum = 0;

    globalWorklist = NULL;

    ops[0] = (LowOp*)malloc(sizeof(LowOp) * BUFFER_SIZE); //for one bytecode or one global label

    globalDvmNcg->ncgClassData = globalDvmNcg->ncgClassDataPtr = (char *)malloc(sizeof(NcgClass) * pDvmDex->pHeader->typeIdsSize);
    globalDvmNcg->ncgMethodData = globalDvmNcg->ncgMethodDataPtr = (char *)malloc(sizeof(NcgMethod) * pDvmDex->pHeader->methodIdsSize);

    globalDvmNcg->curStream = globalDvmNcg->globalBuffer = (char*)malloc(NATIVE_SIZE_FOR_VM_STUBS);

#if defined(WITH_JIT)
    if(gDvm.executionMode != kExecutionModeJit)
        stream = globalDvmNcg->curStream;
#else
    stream = globalDvmNcg->curStream;
#endif

    isScratchPhysical = true;

    return;
}
#endif

void init_common(const char* curFileName, DvmDex *pDvmDex, bool forNCG); //forward declaration
void initGlobalMethods(); //forward declaration

#if defined(WITH_JIT)
//called once when compiler thread starts up
void initJIT(const char* curFileName, DvmDex *pDvmDex) {
    init_common(curFileName, pDvmDex, false);
}
#endif

/*!
\brief be prepared to generate native code

*/
void initNCG(const char* curFileName, DvmDex *pDvmDex) {
    LOGI("call initNCG execution mode %d", gDvm.executionMode);
    init_common(curFileName, pDvmDex, true);
}

void init_common(const char* curFileName, DvmDex *pDvmDex, bool forNCG) {

#if defined(WITH_JIT) && defined(INC_NCG_O0)
    dvmLockMutex(&gDvm.jitNcgInitLock);
#endif
    if(!gDvm.constInit) {
        globalMapNum = 0;
        globalMap = NULL;
        initConstDataSec();
        gDvm.constInit = true;
    }

#ifdef INC_NCG_O0
    if(forNCG)
        initDvmNcg(pDvmDex);
#endif

    //for initJIT: stream is already set
    //for initNCG: stream is not set if WITH_JIT & mode is Jit
    if(!gDvm.commonInit) {
#if defined(WITH_JIT) && defined(INC_NCG_O0)
        char* stream_for_jit = stream;
        if(forNCG && gDvm.executionMode == kExecutionModeJit) {
            LOGI("set stream to globalDvmNcg %p", globalDvmNcg->curStream);
            stream = globalDvmNcg->curStream;
        }
#endif
        initGlobalMethods();
#if defined(WITH_JIT) && defined(INC_NCG_O0)
        if(forNCG && gDvm.executionMode == kExecutionModeJit) {
            LOGI("set stream back %p", stream_for_jit);
            stream = stream_for_jit;
        }
#endif
        gDvm.commonInit = true;
    }

#ifdef INC_NCG_O0
    if(forNCG) {
        generateGlobalSymbols(); //Global data initilization for relocation in VM
        //Prepare the symbol table for the generated code
        ncgSymbolInit(globalMapNum + GLOBAL_SHARED_DATA_NUM + GLOBAL_VM_API_NUM + 1);
    }
#endif
#if defined(WITH_JIT) && defined(INC_NCG_O0)
    dvmUnlockMutex(&gDvm.jitNcgInitLock);
#endif
}

void initGlobalMethods() {
    bool old_dump_x86_inst = dump_x86_inst;
    bool old_scheduling = gDvmJit.scheduling;
    dump_x86_inst = false; // Enable this to debug common section

    //! \warning Scheduling should be turned off when creating common section
    //! because it relies on the fact the register allocation has already been
    //! done (either via register allocator or via hardcoded registers). But,
    //! when we get to this point, the execution mode is Jit instead of either
    //! NcgO1 or NcgO0, which leads to the unintended consequence that NcgO0
    //! path is taken, but logical registers are used instead of physical
    //! registers and thus relies on encoder to do the mapping, which the
    //! scheduler cannot predict for dependency graph creation.
    //! \todo The reason "logical" registers are used is because variable
    //! isScratchPhysical is set to false even when a physical register is
    //! used. This issue should be addressed at some point.
    gDvmJit.scheduling = false;

    // generate native code for function ncgGetEIP
    insertLabel("ncgGetEIP", false);
    move_mem_to_reg(OpndSize_32, 0, PhysicalReg_ESP, true, PhysicalReg_EDX, true);
    x86_return();

    //generate code for common labels
    //jumps within a helper function is treated as short labels
    globalShortMap = NULL;
    common_periodicChecks_entry();
    freeShortMap();
    common_periodicChecks4();
    freeShortMap();
    //common_invokeMethodNoRange();
    //common_invokeMethodRange();
#if defined(WITH_JIT)
    if(dump_x86_inst) LOGI("ArgsDone_Normal start");
    common_invokeArgsDone(ArgsDone_Normal, false);
    freeShortMap();
    if(dump_x86_inst) LOGI("ArgsDone_Native start");
    common_invokeArgsDone(ArgsDone_Native, false);
    freeShortMap();
    if(dump_x86_inst) LOGI("ArgsDone_Full start");
    common_invokeArgsDone(ArgsDone_Full, true/*isJitFull*/);
    if(dump_x86_inst) LOGI("ArgsDone_Full end");
    freeShortMap();
#endif
#ifdef INC_NCG_O0
    dump_x86_inst = false;
    common_invokeArgsDone(ArgsDone_Full, false/*isJitFull*/);
    dump_x86_inst = false;
    freeShortMap();
#endif
    common_backwardBranch();
    freeShortMap();
    common_exceptionThrown();
    freeShortMap();
    common_errNullObject();
    freeShortMap();
    common_errArrayIndex();
    freeShortMap();
    common_errArrayStore();
    freeShortMap();
    common_errNegArraySize();
    freeShortMap();
    common_errNoSuchMethod();
    freeShortMap();
    common_errDivideByZero();
    freeShortMap();
    common_gotoBail();
    freeShortMap();
    common_gotoBail_0();
    freeShortMap();
    invoke_super_nsm();
    freeShortMap();

    performCGWorklist(); //generate code for helper functions
    performLabelWorklist(); //it is likely that the common labels will jump to other common labels

    gDvmJit.scheduling = old_scheduling;
    dump_x86_inst = old_dump_x86_inst;
}

/*!
\brief post-processing after native code is generated

*/
int clearNCG(int fd/*file descriptor*/) {
#ifdef INC_NCG_O0
    ncgFileGeneration(fd);
#endif
    streamCode = NULL;
    freeLabelMap();
#ifdef INC_NCG_O0
    free_auxillary_tables();
    LOGI("code size for ia32 file: %d vs bytecode size %d relocation size %d map size %d\n",
         mm_ncg_size, mm_bytecode_size, mm_relocation_size, mm_map_size);
#endif
    return 0;
}

#ifdef INC_NCG_O0
/*!
\brief wrapper to generate native code for a class

*/
int ncgClass(ClassObject* clazz, const DexClassDef* pClassDef, int classIdx, int fd/*file descriptor*/, int* stats)
{
    NcgClass ncgClass;

    ncgClass.virtualMethodsSize = 0;
    ncgClass.directMethodsSize = 0;

    stats[0] = 0; stats[1] = 0; stats[2] = 0;
    if(clazz->directMethodCount == 0 && clazz->virtualMethodCount == 0) {
        return 0;
    }
    if(gDvm.executionMode == kExecutionModeNcgO1) {
      //to disable generating native code for certain class
      //if(!strcmp(clazz->descriptor, "Lspec/harness/BenchmarkTime;")) return 0; //no NCG if equal
    }

    if(gDvm.executionMode == kExecutionModeNcgO0) {
        isScratchPhysical = true;
    } else {
        isScratchPhysical = false;
    }

    //globalWorklist: check with buqi
    globalWorklist = NULL;

    char* classStart = stream;
#ifdef DEBUG_NCG_CODE_SIZE
    LOGE("CLASS %s %p direct count %d virtual count %d\n", clazz->descriptor, clazz, clazz->directMethodCount, clazz->virtualMethodCount);
#endif

    int i, retval = 0;
    int method_stats[2];
    for (i = 0; i < clazz->directMethodCount; i++) {
        int last_space = 0;
        bool gen_bin = true;
        if(dvmIsNativeMethod(&clazz->directMethods[i]))
             continue;
        if(dvmIsNcgMethod(&clazz->directMethods[i]))
             continue;

        while(1) {
            if (last_space)
                LOGI("Re-NCG happen\n");

            if (!initMethod(&clazz->directMethods[i], &last_space)) {
                LOGI("Give up compile method %s due to size\n", clazz->directMethods[i].name);
                gen_bin = false;
                stats[2]++;
                break;
            }

            int method_ret = ncgMethod(&clazz->directMethods[i], method_stats);
            if(method_ret == -1) {
                LOGI("Give up compile method %s due to codeGen\n", clazz->directMethods[i].name);
                gen_bin = false;
                stats[1]++;
                break;
            }
            if(method_ret == 0) {
                if(enoughSpaceForRelocation())
                    break;
            }
        }
        if (!gen_bin)
            continue;
        stats[0]++;
        mm_bytecode_size += method_stats[0];
        mm_ncg_size += method_stats[1];
        clazz->directMethods[i].insns = (u2 *)streamMethodStart;
        clazz->directMethods[i].size = globalDvmNcg->methodSize;
        performMethodLabelWorklist();
        char* tmpStream = globalDvmNcg->curStream;
        generateRelocationForMethod(globalDvmNcg, &clazz->directMethods[i]);
        mm_relocation_size += globalDvmNcg->curStream - tmpStream;
#ifdef ENABLE_TRACING
        tmpStream = globalDvmNcg->curStream;
        generateMapForMethod(globalDvmNcg, &clazz->directMethods[i]);
        mm_map_size += globalDvmNcg->curStream - tmpStream;
#endif
        ncgWriteMethodInfoIntoFile(fd, globalDvmNcg, &clazz->directMethods[i]);
        ncgSetMethodData(globalDvmNcg, &clazz->directMethods[i], globalDvmNcg->methodChecksum);
        ncgClass.directMethodsSize ++;
        globalDvmNcg->ncgMethodNum ++;
    }
    for (i = 0; i < clazz->virtualMethodCount; i++) {
        int last_space = 0;
        bool gen_bin = true;

        if(dvmIsNativeMethod(&clazz->virtualMethods[i]))
             continue;
        if(dvmIsNcgMethod(&clazz->virtualMethods[i]))
             continue;

        while(1) {

            if (last_space)
                LOGI("Re-NCG happen\n");

            if (!initMethod(&clazz->virtualMethods[i], &last_space)) {
                LOGI("Give up compile method %s due to size\n", clazz->virtualMethods[i].name);
                gen_bin = false;
                stats[2]++;
                break;
            }

            int method_ret = ncgMethod(&clazz->virtualMethods[i], method_stats);
            if(method_ret == -1) {
                LOGI("Give up compile method %s due to codeGen\n", clazz->virtualMethods[i].name);
                gen_bin = false;
                stats[1]++;
                break;
            }
            if (method_ret == 0) {
                if(enoughSpaceForRelocation())
                    break;
            }
        }
        if (!gen_bin)
            continue;
        stats[0]++;
        mm_bytecode_size += method_stats[0];
        mm_ncg_size += method_stats[1];
        clazz->virtualMethods[i].insns = (u2 *)streamMethodStart;
        clazz->virtualMethods[i].size = globalDvmNcg->methodSize;
        performMethodLabelWorklist();
        char* tmpStream = globalDvmNcg->curStream;
        generateRelocationForMethod(globalDvmNcg, &clazz->virtualMethods[i]);
        mm_relocation_size += globalDvmNcg->curStream - tmpStream;
#ifdef ENABLE_TRACING
        tmpStream = globalDvmNcg->curStream;
        generateMapForMethod(globalDvmNcg, &clazz->virtualMethods[i]);
        mm_map_size += globalDvmNcg->curStream - tmpStream;
#endif
        ncgWriteMethodInfoIntoFile(fd, globalDvmNcg, &clazz->virtualMethods[i]);
        ncgSetMethodData(globalDvmNcg, &clazz->virtualMethods[i], globalDvmNcg->methodChecksum);
        ncgClass.virtualMethodsSize ++;
        globalDvmNcg->ncgMethodNum ++;
    }

#ifdef DEBUG_NCG_CODE_SIZE
    LOGI("CLASS %s stream start %p end %p direct %d virtual %d \n",
         clazz->descriptor, classStart, stream, clazz->directMethodCount, clazz->virtualMethodCount);
#endif

    if (ncgClass.virtualMethodsSize || ncgClass.directMethodsSize) {
        ncgClass.id = pClassDef->classIdx;
        ncgSetClassData(globalDvmNcg, &ncgClass);
        globalDvmNcg->ncgClassNum ++;
    }

    return retval;
}
#endif

#if defined(WITH_JIT)
ExecutionMode origMode;

/**
 * @brief Lowers bytecode to native code
 * @param method parent method of trace
 * @param mir bytecode representation
 * @return true when NOT handled and false when it IS handled
 */
bool lowerByteCodeJit(const Method* method, const MIR * mir,
        const u2 * dalvikPC) {
    int retCode = lowerByteCode(method, mir, dalvikPC);
    freeShortMap();
    if(retCode >= 0) return false; //handled
    return true; //not handled
}

void startOfBasicBlock(BasicBlock* bb) {
    traceCurrentBB = bb;
    if(gDvm.executionMode == kExecutionModeNcgO0) {
        isScratchPhysical = true;
    } else {
        isScratchPhysical = false;
    }
}

void startOfTrace(const Method* method, LowOpBlockLabel* labelList, int exceptionBlockId,
                  CompilationUnit *cUnit) {
    origMode = gDvm.executionMode;
    gDvm.executionMode = kExecutionModeNcgO1;
    if(gDvm.executionMode == kExecutionModeNcgO0) {
        isScratchPhysical = true;
    } else {
        isScratchPhysical = false;
    }
    currentMethod = (Method*)method;
    currentExceptionBlockIdx = exceptionBlockId;
    methodDataWorklist = NULL;
    globalShortWorklist = NULL;
    globalNCGWorklist = NULL;

    streamMethodStart = stream;
    //initialize mapFromBCtoNCG
    memset(&mapFromBCtoNCG[0], -1, BYTECODE_SIZE_PER_METHOD * sizeof(mapFromBCtoNCG[0]));
    traceLabelList = labelList;
    if(gDvm.executionMode == kExecutionModeNcgO1)
        startOfTraceO1(method, labelList, exceptionBlockId, cUnit);
}

void endOfTrace(bool freeOnly) {
    if(freeOnly) {
        freeLabelWorklist();
        freeNCGWorklist();
        freeDataWorklist();
        freeChainingWorklist();
    }
    else {
        performLabelWorklist();
        performNCGWorklist(); //handle forward jump (GOTO, IF)
        performDataWorklist(); //handle SWITCH & FILL_ARRAY_DATA
        performChainingWorklist();
    }
    if(gDvm.executionMode == kExecutionModeNcgO1) {
        endOfTraceO1();
    }
    gDvm.executionMode = origMode;
}
#endif

/**
 * @brief Generates native code for the bytecode.
 * @details May update code stream.
 * @param method parent method of trace
 * @param mir bytecode representation
 * @return 0 or greater when handled
 */
int lowerByteCode(const Method* method, const MIR * mir, const u2 * dalvikPC) {
    /* offsetPC is used in O1 code generator, where it is defined as the sequence number
       use a local version to avoid overwriting */
    int offsetPC = mir->offset; //! \warning When doing method inlining, offsetPC
                                //! will be the same for the invoke and the inlined
                                //! bytecode. This WILL break mapping from BC to NCG
                                //! if more than one bytecode is inlined.

    if (dump_x86_inst) {
        char * decodedString = dvmCompilerGetDalvikDisassembly(&mir->dalvikInsn,
                NULL);
        const char * note;
        if (mir->OptimizationFlags & MIR_INLINED) {
            note = " (no-op)";
        } else if (mir->OptimizationFlags & MIR_INLINED_PRED) {
            note = " (prediction inline)";
        } else if (mir->OptimizationFlags & MIR_CALLEE) {
            note = " (inlined)";
        } else {
            note = "";
        }
        LOGI("LOWER %s%s with offsetPC %x offsetNCG %x @%p\n",
                decodedString, note, offsetPC, stream - streamMethodStart,
                stream);
    }

    //update mapFromBCtoNCG
    offsetNCG = stream - streamMethodStart;
    if(offsetPC >= BYTECODE_SIZE_PER_METHOD) LOGI("ERROR ERROR offsetPC %d exceeds BYTECODE_SIZE_PER_METHOD\n", offsetPC);
    mapFromBCtoNCG[offsetPC] = offsetNCG;
#if defined(ENABLE_TRACING) && defined(TRACING_OPTION2)
    insertMapWorklist(offsetPC, mapFromBCtoNCG[offsetPC], 1);
#endif
    //return number of LowOps generated
    switch (mir->dalvikInsn.opcode) {
    case OP_NOP:
        return op_nop(mir);
    case OP_MOVE:
    case OP_MOVE_OBJECT:
        return op_move(mir);
    case OP_MOVE_FROM16:
    case OP_MOVE_OBJECT_FROM16:
        return op_move_from16(mir);
    case OP_MOVE_16:
    case OP_MOVE_OBJECT_16:
        return op_move_16(mir);
    case OP_MOVE_WIDE:
        return op_move_wide(mir);
    case OP_MOVE_WIDE_FROM16:
        return op_move_wide_from16(mir);
    case OP_MOVE_WIDE_16:
        return op_move_wide_16(mir);
    case OP_MOVE_RESULT:
    case OP_MOVE_RESULT_OBJECT:
        return op_move_result(mir);
    case OP_MOVE_RESULT_WIDE:
        return op_move_result_wide(mir);
    case OP_MOVE_EXCEPTION:
        return op_move_exception(mir);
    case OP_RETURN_VOID:
    case OP_RETURN_VOID_BARRIER:
        return op_return_void(mir);
    case OP_RETURN:
    case OP_RETURN_OBJECT:
        return op_return(mir);
    case OP_RETURN_WIDE:
        return op_return_wide(mir);
    case OP_CONST_4:
        return op_const_4(mir);
    case OP_CONST_16:
        return op_const_16(mir);
    case OP_CONST:
        return op_const(mir);
    case OP_CONST_HIGH16:
        return op_const_high16(mir);
    case OP_CONST_WIDE_16:
        return op_const_wide_16(mir);
    case OP_CONST_WIDE_32:
        return op_const_wide_32(mir);
    case OP_CONST_WIDE:
        return op_const_wide(mir);
    case OP_CONST_WIDE_HIGH16:
        return op_const_wide_high16(mir);
    case OP_CONST_STRING:
        return op_const_string(mir);
    case OP_CONST_STRING_JUMBO:
        return op_const_string_jumbo(mir);
    case OP_CONST_CLASS:
        return op_const_class(mir);
    case OP_MONITOR_ENTER:
        return op_monitor_enter(mir);
    case OP_MONITOR_EXIT:
        return op_monitor_exit(mir);
    case OP_CHECK_CAST:
        return op_check_cast(mir);
    case OP_INSTANCE_OF:
        return op_instance_of(mir);
    case OP_ARRAY_LENGTH:
        return op_array_length(mir);
    case OP_NEW_INSTANCE:
        return op_new_instance(mir);
    case OP_NEW_ARRAY:
        return op_new_array(mir);
    case OP_FILLED_NEW_ARRAY:
        return op_filled_new_array(mir);
    case OP_FILLED_NEW_ARRAY_RANGE:
        return op_filled_new_array_range(mir);
    case OP_FILL_ARRAY_DATA:
        return op_fill_array_data(mir, dalvikPC);
    case OP_THROW:
        return op_throw(mir);
    case OP_THROW_VERIFICATION_ERROR:
        return op_throw_verification_error(mir);
    case OP_GOTO:
        return op_goto(mir);
    case OP_GOTO_16:
        return op_goto_16(mir);
    case OP_GOTO_32:
        return op_goto_32(mir);
    case OP_PACKED_SWITCH:
        return op_packed_switch(mir, dalvikPC);
    case OP_SPARSE_SWITCH:
        return op_sparse_switch(mir, dalvikPC);
    case OP_CMPL_FLOAT:
        return op_cmpl_float(mir);
    case OP_CMPG_FLOAT:
        return op_cmpg_float(mir);
    case OP_CMPL_DOUBLE:
        return op_cmpl_double(mir);
    case OP_CMPG_DOUBLE:
        return op_cmpg_double(mir);
    case OP_CMP_LONG:
        return op_cmp_long(mir);
    case OP_IF_EQ:
        return op_if_eq(mir);
    case OP_IF_NE:
        return op_if_ne(mir);
    case OP_IF_LT:
        return op_if_lt(mir);
    case OP_IF_GE:
        return op_if_ge(mir);
    case OP_IF_GT:
        return op_if_gt(mir);
    case OP_IF_LE:
        return op_if_le(mir);
    case OP_IF_EQZ:
        return op_if_eqz(mir);
    case OP_IF_NEZ:
        return op_if_nez(mir);
    case OP_IF_LTZ:
        return op_if_ltz(mir);
    case OP_IF_GEZ:
        return op_if_gez(mir);
    case OP_IF_GTZ:
        return op_if_gtz(mir);
    case OP_IF_LEZ:
        return op_if_lez(mir);
    case OP_AGET:
        return op_aget(mir);
    case OP_AGET_WIDE:
        return op_aget_wide(mir);
    case OP_AGET_OBJECT:
        return op_aget_object(mir);
    case OP_AGET_BOOLEAN:
        return op_aget_boolean(mir);
    case OP_AGET_BYTE:
        return op_aget_byte(mir);
    case OP_AGET_CHAR:
        return op_aget_char(mir);
    case OP_AGET_SHORT:
        return op_aget_short(mir);
    case OP_APUT:
        return op_aput(mir);
    case OP_APUT_WIDE:
        return op_aput_wide(mir);
    case OP_APUT_OBJECT:
        return op_aput_object(mir);
    case OP_APUT_BOOLEAN:
        return op_aput_boolean(mir);
    case OP_APUT_BYTE:
        return op_aput_byte(mir);
    case OP_APUT_CHAR:
        return op_aput_char(mir);
    case OP_APUT_SHORT:
        return op_aput_short(mir);
    case OP_IGET:
    case OP_IGET_VOLATILE:
        return op_iget(mir);
    case OP_IGET_WIDE:
        return op_iget_wide(mir, false /*isVolatile*/);
    case OP_IGET_WIDE_VOLATILE:
        return op_iget_wide(mir, true /*isVolatile*/);
    case OP_IGET_OBJECT:
    case OP_IGET_OBJECT_VOLATILE:
        return op_iget_object(mir);
    case OP_IGET_BOOLEAN:
        return op_iget_boolean(mir);
    case OP_IGET_BYTE:
        return op_iget_byte(mir);
    case OP_IGET_CHAR:
        return op_iget_char(mir);
    case OP_IGET_SHORT:
        return op_iget_short(mir);
    case OP_IPUT:
    case OP_IPUT_VOLATILE:
        return op_iput(mir);
    case OP_IPUT_WIDE:
        return op_iput_wide(mir, false /*isVolatile*/);
    case OP_IPUT_WIDE_VOLATILE:
        return op_iput_wide(mir, true /*isVolatile*/);
    case OP_IPUT_OBJECT:
    case OP_IPUT_OBJECT_VOLATILE:
        return op_iput_object(mir);
    case OP_IPUT_BOOLEAN:
        return op_iput_boolean(mir);
    case OP_IPUT_BYTE:
        return op_iput_byte(mir);
    case OP_IPUT_CHAR:
        return op_iput_char(mir);
    case OP_IPUT_SHORT:
        return op_iput_short(mir);
    case OP_SGET:
    case OP_SGET_VOLATILE:
        return op_sget(mir);
    case OP_SGET_WIDE:
        return op_sget_wide(mir, false /*isVolatile*/);
    case OP_SGET_WIDE_VOLATILE:
        return op_sget_wide(mir, true /*isVolatile*/);
    case OP_SGET_OBJECT:
    case OP_SGET_OBJECT_VOLATILE:
        return op_sget_object(mir);
    case OP_SGET_BOOLEAN:
        return op_sget_boolean(mir);
    case OP_SGET_BYTE:
        return op_sget_byte(mir);
    case OP_SGET_CHAR:
        return op_sget_char(mir);
    case OP_SGET_SHORT:
        return op_sget_short(mir);
    case OP_SPUT:
    case OP_SPUT_VOLATILE:
        return op_sput(mir, false /*isObj*/);
    case OP_SPUT_WIDE:
        return op_sput_wide(mir, false /*isVolatile*/);
    case OP_SPUT_WIDE_VOLATILE:
        return op_sput_wide(mir, true /*isVolatile*/);
    case OP_SPUT_OBJECT:
    case OP_SPUT_OBJECT_VOLATILE:
        return op_sput_object(mir);
    case OP_SPUT_BOOLEAN:
        return op_sput_boolean(mir);
    case OP_SPUT_BYTE:
        return op_sput_byte(mir);
    case OP_SPUT_CHAR:
        return op_sput_char(mir);
    case OP_SPUT_SHORT:
        return op_sput_short(mir);
    case OP_INVOKE_VIRTUAL:
        return op_invoke_virtual(mir);
    case OP_INVOKE_SUPER:
        return op_invoke_super(mir);
    case OP_INVOKE_DIRECT:
        return op_invoke_direct(mir);
    case OP_INVOKE_STATIC:
        return op_invoke_static(mir);
    case OP_INVOKE_INTERFACE:
        return op_invoke_interface(mir);
    case OP_INVOKE_VIRTUAL_RANGE:
        return op_invoke_virtual_range(mir);
    case OP_INVOKE_SUPER_RANGE:
        return op_invoke_super_range(mir);
    case OP_INVOKE_DIRECT_RANGE:
        return op_invoke_direct_range(mir);
    case OP_INVOKE_STATIC_RANGE:
        return op_invoke_static_range(mir);
    case OP_INVOKE_INTERFACE_RANGE:
        return op_invoke_interface_range(mir);
    case OP_NEG_INT:
        return op_neg_int(mir);
    case OP_NOT_INT:
        return op_not_int(mir);
    case OP_NEG_LONG:
        return op_neg_long(mir);
    case OP_NOT_LONG:
        return op_not_long(mir);
    case OP_NEG_FLOAT:
        return op_neg_float(mir);
    case OP_NEG_DOUBLE:
        return op_neg_double(mir);
    case OP_INT_TO_LONG:
        return op_int_to_long(mir);
    case OP_INT_TO_FLOAT:
        return op_int_to_float(mir);
    case OP_INT_TO_DOUBLE:
        return op_int_to_double(mir);
    case OP_LONG_TO_INT:
        return op_long_to_int(mir);
    case OP_LONG_TO_FLOAT:
        return op_long_to_float(mir);
    case OP_LONG_TO_DOUBLE:
        return op_long_to_double(mir);
    case OP_FLOAT_TO_INT:
        return op_float_to_int(mir);
    case OP_FLOAT_TO_LONG:
        return op_float_to_long(mir);
    case OP_FLOAT_TO_DOUBLE:
        return op_float_to_double(mir);
    case OP_DOUBLE_TO_INT:
        return op_double_to_int(mir);
    case OP_DOUBLE_TO_LONG:
        return op_double_to_long(mir);
    case OP_DOUBLE_TO_FLOAT:
        return op_double_to_float(mir);
    case OP_INT_TO_BYTE:
        return op_int_to_byte(mir);
    case OP_INT_TO_CHAR:
        return op_int_to_char(mir);
    case OP_INT_TO_SHORT:
        return op_int_to_short(mir);
    case OP_ADD_INT:
        return op_add_int(mir);
    case OP_SUB_INT:
        return op_sub_int(mir);
    case OP_MUL_INT:
        return op_mul_int(mir);
    case OP_DIV_INT:
        return op_div_int(mir);
    case OP_REM_INT:
        return op_rem_int(mir);
    case OP_AND_INT:
        return op_and_int(mir);
    case OP_OR_INT:
        return op_or_int(mir);
    case OP_XOR_INT:
        return op_xor_int(mir);
    case OP_SHL_INT:
        return op_shl_int(mir);
    case OP_SHR_INT:
        return op_shr_int(mir);
    case OP_USHR_INT:
        return op_ushr_int(mir);
    case OP_ADD_LONG:
        return op_add_long(mir);
    case OP_SUB_LONG:
        return op_sub_long(mir);
    case OP_MUL_LONG:
        return op_mul_long(mir);
    case OP_DIV_LONG:
        return op_div_long(mir);
    case OP_REM_LONG:
        return op_rem_long(mir);
    case OP_AND_LONG:
        return op_and_long(mir);
    case OP_OR_LONG:
        return op_or_long(mir);
    case OP_XOR_LONG:
        return op_xor_long(mir);
    case OP_SHL_LONG:
        return op_shl_long(mir);
    case OP_SHR_LONG:
        return op_shr_long(mir);
    case OP_USHR_LONG:
        return op_ushr_long(mir);
    case OP_ADD_FLOAT:
        return op_add_float(mir);
    case OP_SUB_FLOAT:
        return op_sub_float(mir);
    case OP_MUL_FLOAT:
        return op_mul_float(mir);
    case OP_DIV_FLOAT:
        return op_div_float(mir);
    case OP_REM_FLOAT:
        return op_rem_float(mir);
    case OP_ADD_DOUBLE:
        return op_add_double(mir);
    case OP_SUB_DOUBLE:
        return op_sub_double(mir);
    case OP_MUL_DOUBLE:
        return op_mul_double(mir);
    case OP_DIV_DOUBLE:
        return op_div_double(mir);
    case OP_REM_DOUBLE:
        return op_rem_double(mir);
    case OP_ADD_INT_2ADDR:
        return op_add_int_2addr(mir);
    case OP_SUB_INT_2ADDR:
        return op_sub_int_2addr(mir);
    case OP_MUL_INT_2ADDR:
        return op_mul_int_2addr(mir);
    case OP_DIV_INT_2ADDR:
        return op_div_int_2addr(mir);
    case OP_REM_INT_2ADDR:
        return op_rem_int_2addr(mir);
    case OP_AND_INT_2ADDR:
        return op_and_int_2addr(mir);
    case OP_OR_INT_2ADDR:
        return op_or_int_2addr(mir);
    case OP_XOR_INT_2ADDR:
        return op_xor_int_2addr(mir);
    case OP_SHL_INT_2ADDR:
        return op_shl_int_2addr(mir);
    case OP_SHR_INT_2ADDR:
        return op_shr_int_2addr(mir);
    case OP_USHR_INT_2ADDR:
        return op_ushr_int_2addr(mir);
    case OP_ADD_LONG_2ADDR:
        return op_add_long_2addr(mir);
    case OP_SUB_LONG_2ADDR:
        return op_sub_long_2addr(mir);
    case OP_MUL_LONG_2ADDR:
        return op_mul_long_2addr(mir);
    case OP_DIV_LONG_2ADDR:
        return op_div_long_2addr(mir);
    case OP_REM_LONG_2ADDR:
        return op_rem_long_2addr(mir);
    case OP_AND_LONG_2ADDR:
        return op_and_long_2addr(mir);
    case OP_OR_LONG_2ADDR:
        return op_or_long_2addr(mir);
    case OP_XOR_LONG_2ADDR:
        return op_xor_long_2addr(mir);
    case OP_SHL_LONG_2ADDR:
        return op_shl_long_2addr(mir);
    case OP_SHR_LONG_2ADDR:
        return op_shr_long_2addr(mir);
    case OP_USHR_LONG_2ADDR:
        return op_ushr_long_2addr(mir);
    case OP_ADD_FLOAT_2ADDR:
        return op_add_float_2addr(mir);
    case OP_SUB_FLOAT_2ADDR:
        return op_sub_float_2addr(mir);
    case OP_MUL_FLOAT_2ADDR:
        return op_mul_float_2addr(mir);
    case OP_DIV_FLOAT_2ADDR:
        return op_div_float_2addr(mir);
    case OP_REM_FLOAT_2ADDR:
        return op_rem_float_2addr(mir);
    case OP_ADD_DOUBLE_2ADDR:
        return op_add_double_2addr(mir);
    case OP_SUB_DOUBLE_2ADDR:
        return op_sub_double_2addr(mir);
    case OP_MUL_DOUBLE_2ADDR:
        return op_mul_double_2addr(mir);
    case OP_DIV_DOUBLE_2ADDR:
        return op_div_double_2addr(mir);
    case OP_REM_DOUBLE_2ADDR:
        return op_rem_double_2addr(mir);
    case OP_ADD_INT_LIT16:
        return op_add_int_lit16(mir);
    case OP_RSUB_INT:
        return op_rsub_int(mir);
    case OP_MUL_INT_LIT16:
        return op_mul_int_lit16(mir);
    case OP_DIV_INT_LIT16:
        return op_div_int_lit16(mir);
    case OP_REM_INT_LIT16:
        return op_rem_int_lit16(mir);
    case OP_AND_INT_LIT16:
        return op_and_int_lit16(mir);
    case OP_OR_INT_LIT16:
        return op_or_int_lit16(mir);
    case OP_XOR_INT_LIT16:
        return op_xor_int_lit16(mir);
    case OP_ADD_INT_LIT8:
        return op_add_int_lit8(mir);
    case OP_RSUB_INT_LIT8:
        return op_rsub_int_lit8(mir);
    case OP_MUL_INT_LIT8:
        return op_mul_int_lit8(mir);
    case OP_DIV_INT_LIT8:
        return op_div_int_lit8(mir);
    case OP_REM_INT_LIT8:
        return op_rem_int_lit8(mir);
    case OP_AND_INT_LIT8:
        return op_and_int_lit8(mir);
    case OP_OR_INT_LIT8:
        return op_or_int_lit8(mir);
    case OP_XOR_INT_LIT8:
        return op_xor_int_lit8(mir);
    case OP_SHL_INT_LIT8:
        return op_shl_int_lit8(mir);
    case OP_SHR_INT_LIT8:
        return op_shr_int_lit8(mir);
    case OP_USHR_INT_LIT8:
        return op_ushr_int_lit8(mir);
    case OP_EXECUTE_INLINE:
        return op_execute_inline(mir, false /*isRange*/);
    case OP_EXECUTE_INLINE_RANGE:
        return op_execute_inline(mir, true /*isRange*/);
    case OP_BREAKPOINT:
        printf("found bytecode OP_BREAKPOINT\n");
        exit(-1);
    case OP_IGET_QUICK:
        return op_iget_quick(mir);
    case OP_IGET_WIDE_QUICK:
        return op_iget_wide_quick(mir);
    case OP_IGET_OBJECT_QUICK:
        return op_iget_object_quick(mir);
    case OP_IPUT_QUICK:
        return op_iput_quick(mir);
    case OP_IPUT_WIDE_QUICK:
        return op_iput_wide_quick(mir);
    case OP_IPUT_OBJECT_QUICK:
        return op_iput_object_quick(mir);
    case OP_INVOKE_VIRTUAL_QUICK:
        return op_invoke_virtual_quick(mir);
    case OP_INVOKE_VIRTUAL_QUICK_RANGE:
        return op_invoke_virtual_quick_range(mir);
    case OP_INVOKE_SUPER_QUICK:
        return op_invoke_super_quick(mir);
    case OP_INVOKE_SUPER_QUICK_RANGE:
        return op_invoke_super_quick_range(mir);
    default:
        ALOGE("ERROR: JIT does not support bytecode 0x%hx\n",
                mir->dalvikInsn.opcode);
        assert(false && "All opcodes should be supported.");
        break;
    }
    return -1;
}

int op_nop(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_NOP);
    return 0;
}

#if defined VTUNE_DALVIK
/**
 * Send the label information (size, start_address and name) to VTune
 */
void sendLabelInfoToVTune(int startStreamPtr, int endStreamPtr, const char* labelName) {
    iJIT_Method_Load jitMethod;
    memset(&jitMethod, 0, sizeof(iJIT_Method_Load));
    jitMethod.method_id = iJIT_GetNewMethodID();
    jitMethod.method_name = const_cast<char *>(labelName);
    jitMethod.method_load_address = (void *)startStreamPtr;
    jitMethod.method_size = endStreamPtr-startStreamPtr;
    int res = iJIT_NotifyEvent(iJVM_EVENT_TYPE_METHOD_LOAD_FINISHED, (void*)&jitMethod);
    if (res) {
        LOGI("JIT API: a trace of %s method was written successfully.\n", labelName);
    } else {
        LOGI("JIT API: failed to write a trace of %s method.\n", labelName);
    }
}
#endif
