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


/*! \file NcgCodegenO1.cpp
  \brief This file implements code specific to NCG
*/
#include "libdex/DexOpcodes.h"
#include "libdex/DexFile.h"
#include "Lower.h"
#include "interp/InterpDefs.h"
#include "libdex/Leb128.h"
#ifdef NCG_O1
#ifndef WITH_JIT
/** entry point to generate native code for a method
    Go through all exception handlers and store the PC for handlers in exceptionHandlers; then call codeGenCFGPart
*/
int codeGenCFG(Method* method) {
    //iget_obj_inst = -1;
    //if(!strcmp(method->name, "buildPhase3Routine")) iget_obj_inst = 0;
    num_exception_handlers = 0;
    int i,j;
    const DexCode* pCode = dvmGetMethodCode(method);
    u4 triesSize = pCode->triesSize;
    const u1* baseData = dexGetCatchHandlerData(pCode);
    u4 handlersSize = 0;
    if(triesSize > 0) {
        handlersSize = readUnsignedLeb128(&baseData);
    }
    //assume handlers are in order
    //sort handlers and check for duplication
    for(i = 0; i < (int)handlersSize; i++) {
        s4 tSize = readSignedLeb128(&baseData);
        bool hasCatchAll = (tSize <= 0);
        if(tSize < 0) {
            tSize = 0-tSize;
        }
        for(j = 0; j < (int)tSize; j++) {
            s4 tmp = readUnsignedLeb128(&baseData);
            tmp = readUnsignedLeb128(&baseData);
            addExceptionHandler(tmp);
        }
        if(hasCatchAll) {
            s4 tmp = readUnsignedLeb128(&baseData);
            addExceptionHandler(tmp);
        }
    }
#ifdef PRINT_WARNING
    for(i = 0; i < num_exception_handlers; i++)
        LOGI("EXCEPTION handler %d: offset %x\n", i, exceptionHandlers[i]);
#endif
    codeGenCFGPart(method);
    return 0;
}

//! code generation for a method

//!one traversal to collect virtual register info for each basic block;
//!another traversal to generate native code;
//!the order of Basic Block traversal is according to program order
int codeGenCFGPart(Method* method) {
    num_compile_entries = 0;
    currentBB = NULL;

    //LOGI("code generation of method %s%s\n", method->clazz->descriptor, method->name);
    int k, ii;
#ifndef WITH_JIT
    int retval = createCFG(method);
#endif
    for(k = 0; k < num_bbs_for_method; k++) {
        method_bbs_sorted[k]->bb_index2 = k;
    }

    //update bb->infoBasicBlock
    for(k = 0; k < num_bbs_for_method; k++) {
        //LOGI("Collect info for BB %d\n", method_bbs_sorted[k]->bb_index);
        currentBB = method_bbs_sorted[k];
        for(ii = 0; ii < 8; ii++) {
            currentBB->allocConstraints[ii].physicalReg = ii;
            currentBB->allocConstraints[ii].count = 0;
        }
        //traverse method_bbs_sorted[k]
        collectInfoOfBasicBlock(method, method_bbs_sorted[k]);
        //dumpVirtualInfoOfBasicBlock(method_bbs_sorted[k]);
    }
    currentBB = NULL;

    setTypeOfVR();
    insertGlueReg();
    //this section was in codeGenBasicBlock after insertFromVirtualInfo
    /////// update defUseTable assuming a fake usage at end of BB
    int compile_entries_old = num_compile_entries;
    int k2, k3;
    for(k2 = 0; k2 < num_bbs_for_method; k2++) {
        //LOGI("fakeUsage for BB %d\n", method_bbs_sorted[k2]->bb_index);
        currentBB = method_bbs_sorted[k2];
        //at end of codeGenBasicBlock, remove entries for temporary registers, L VR and GL VR
        //GL VRs are from infoBasicBlock (insertFromVirtualInfo)
        //GG VRs are always there for each basic block
        //GG VRs are inserted to compileTable by insertGGVRFromVirtualInfo, which is called in setTypeOfVR
        for(k3 = 0; k3 < currentBB->num_regs; k3++) {
            insertFromVirtualInfo(currentBB, k3);
        }

        int offsetPC_back = offsetPC;
        offsetPC = PC_FOR_END_OF_BB;
        for(k = 0; k < num_compile_entries; k++) {
            currentInfo.regNum = compileTable[k].regNum;
            currentInfo.physicalType = compileTable[k].physicalType;
            if(isVirtualReg(compileTable[k].physicalType) &&
               compileTable[k].gType == GLOBALTYPE_GL) {
                fakeUsageAtEndOfBB(currentBB);
            }
            if(isVirtualReg(compileTable[k].physicalType) &&
               compileTable[k].gType == GLOBALTYPE_GG) {
                fakeUsageAtEndOfBB(currentBB);
            } //if GG
        } //for k
        offsetPC = offsetPC_back;
        num_compile_entries = compile_entries_old;
    }
    //calculate DEF & USE of live variable analysis
    //calculate KILL & GEN of null check analysis
    //calculate KILL & GEN of glue optimization

    //data flow analysis
#ifdef GLOBAL_NULLCHECK_OPT
    updateBBIndex();
    forwardAnalysis(0); //null check
#endif
    //refCount will be reduced each time registerAlloc[Move] is called
    //a physical register will be freed once the refCount is zero for
    //  virtual registers local to a basic block and temporary registers within a bytecode
    //Spill order:
    //  1> a virtual register that is not referenced in this basic block
    //  2> a virtual register that is not referenced in this bytecode
    //  3> a temporary register (_scratch or _gp) with fewest leftover reference
    //the order must match enum PhysicalReg
    initializeAllRegs();
    for(k = 0; k < num_bbs_for_method; k++) {
        //LOGI("CG for BB %d [%x %x)\n", method_bbs_sorted[k]->bb_index, method_bbs_sorted[k]->pc_start, method_bbs_sorted[k]->pc_end);
        currentBB = method_bbs_sorted[k];
        codeGenBasicBlock(method, method_bbs_sorted[k]);
    }
    //LOGI("done code generation of method %s%s\n", method->clazz->descriptor, method->name);
    freeCFG();
    currentBB = NULL;
    return 0;
}

//!clear the worklist to handle data section

//!
int clearDataWorklist() {
    DataWorklist* ptr = methodDataWorklist;
    if(ptr == NULL) return 0;
    while(ptr != NULL) {    //remove the item
        methodDataWorklist = ptr->nextItem;
        free(ptr);
        ptr = methodDataWorklist;
    }
    return 0;
}
CFGWork* CFGWorklist;
//! insert an entry to the worklist for creating CFG

//!
int insertWorklist(BasicBlock_O1* bb_prev, int targetOff) {
    CFGWork* item = (CFGWork*)malloc(sizeof(CFGWork));
    item->bb_prev = bb_prev;
    item->targetOff = targetOff;
    item->nextItem = CFGWorklist;
    CFGWorklist = item;
    return 0;
}

//! handles the worklist for creating CFG

//! if a target is in the middle of an existing basic block, split the basic block into two, and add an edge between the two basic blocks
void splitBB() {
    CFGWork* ptr = CFGWorklist;
    while(ptr != NULL) {
        BasicBlock_O1* bb_t = findForOffset(ptr->targetOff);
        if(bb_t->pc_start < ptr->targetOff) {
            BasicBlock_O1* bb_2nd = createBasicBlock(ptr->targetOff, bb_t->pc_end);
            bb_t->pc_end = ptr->targetOff;
            connectBasicBlock(bb_t, bb_2nd);
        }
        ptr = ptr->nextItem;
    }
}
//! handles the worklist for creating CFG

//!Connect the basic block inclduing target to the basic block bb_prev
void performWorklist() {
    CFGWork* ptr = CFGWorklist;
    while(ptr != NULL) {
        BasicBlock_O1* bb_t = findForOffset(ptr->targetOff);
        connectBasicBlock(ptr->bb_prev, bb_t);
        CFGWorklist = ptr->nextItem;
        free(ptr);
        ptr = CFGWorklist;
    }
}

//!entry point to create CFG for a method

//!
int createCFG(Method* method) {
    if(dvmIsNativeMethod(method)) return 0;
#ifdef INC_NCG_O0
    if(dvmIsNcgMethod(method)) return 0;
#endif
    pc_start = -1;
    bb_entry = NULL;
    num_bbs_for_method = 0;
    currentMethod = method;
    methodDataWorklist = NULL;
    CFGWorklist = NULL;

    rPC = (u2*)method->insns;
    u2 insnsSize = dvmGetMethodInsnsSize(method);
    u2* insnEnd;
    insnEnd = rPC + insnsSize;
    pc_start = rPC - (u2*)method->insns;
    while(rPC < insnEnd) {
        inst = FETCH(0);
        offsetPC = rPC - (u2*)method->insns; //global offsetPC
#ifdef DEBUG_CFG
        LOGI("LOWER bytecode %x at offsetPC %x : %x %x\n", INST_INST(inst), offsetPC, inst, FETCH(1));
#endif
        current_bc_size = getByteCodeSize();
        //call handler, update methodDataWorklist
        createCFGHandler(method); //current_bc_size used
        rPC += current_bc_size;
        //update insnEnd when handling bytecodes with data section
        DataWorklist* ptr = methodDataWorklist;
        if(ptr != NULL) {
            int endPC = ptr->offsetPC + ptr->relativePC;
            //select the smaller one
            if((u2*)method->insns + endPC < insnEnd) {
                insnEnd = (u2*)method->insns + endPC;
#ifdef DEBUG_CFG
                LOGI("createCFG updates end pc %p\n", insnEnd);
#endif
            }
        }
    }
    offsetPC = rPC - (u2*)method->insns;
    if(pc_start != offsetPC && pc_start >= 0) createBasicBlock(pc_start, offsetPC);
#ifdef DEBUG_CFG
    LOGI("before split BBs\n");
#endif
    splitBB();
    performWorklist();
    dump_CFG(method);
    methodDataWorklist = NULL;
    return 0;
}

/** update compileTable with bb->infoBasicBlock[jj]
    set type of this VR to GG
*/
void insertGGVRFromVirtualInfo(BasicBlock_O1* bb, int jj) {
    int indexKK = searchCompileTable(
                                     LowOpndRegType_virtual | bb->infoBasicBlock[jj].physicalType,
                                     bb->infoBasicBlock[jj].regNum);
    if(indexKK < 0) {
        /* the virtual register is not in compileTable, insert it */
        compileTable[num_compile_entries].physicalType =
                 LowOpndRegType_virtual | bb->infoBasicBlock[jj].physicalType;
        compileTable[num_compile_entries].regNum = bb->infoBasicBlock[jj].regNum;
        compileTable[num_compile_entries].refCount = 0;
        compileTable[num_compile_entries].physicalReg = PhysicalReg_Null;
        compileTable[num_compile_entries].bb = NULL;
        compileTable[num_compile_entries].gType = GLOBALTYPE_GG;
        compileTable[num_compile_entries].accessType = REGACCESS_N;
        compileTable[num_compile_entries].physicalReg_prev = bb->infoBasicBlock[jj].physicalReg_GG;
        num_compile_entries++;
        if(num_compile_entries >= COMPILE_TABLE_SIZE) {
            LOGE("ERROR compileTable overflow");
            exit(-1);
        }
    }
}

//! insert a PC to exceptionHandlers

//! We check for duplication and the insertion is in program order
void addExceptionHandler(s4 tmp) {
    int k;
    int index = -1;
    for(k = 0; k < num_exception_handlers; k++) {
        if(exceptionHandlers[k] == tmp) return;
        if(exceptionHandlers[k] > tmp) {
            index = k;
            break;
        }
    }
    if(index == -1) exceptionHandlers[num_exception_handlers] = tmp;
    else {
        for(k = num_exception_handlers-1; k >= index; k--)
            exceptionHandlers[k+1] = exceptionHandlers[k];
        exceptionHandlers[index] = tmp;
    }
    num_exception_handlers++;
}
int num_GG_VRs_gp = 0;
int num_GG_VRs_xmm = 0;
int GG_VRs_gp[5];
int GG_VRs_xmm[5];
//update bb->infoBasicBlock[jj].physicalReg_GG
void allocateRegForGGVR(BasicBlock_O1* bb, int jj) {
    LowOpndRegType tmpPtype = bb->infoBasicBlock[jj].physicalType;
    if(tmpPtype == LowOpndRegType_xmm) {
        int the_num = -1;
        int kk;
        //check whether GG VR is already allocated to a physical register
        for(kk = 0; kk < num_GG_VRs_xmm; kk++) {
            if(GG_VRs_xmm[kk] == bb->infoBasicBlock[jj].regNum) {
                the_num = kk;
                break;
            }
        }
        if(the_num < 0) {
            the_num = num_GG_VRs_xmm;
            num_GG_VRs_xmm++;
        }
        GG_VRs_xmm[the_num] = bb->infoBasicBlock[jj].regNum;
        bb->infoBasicBlock[jj].physicalReg_GG = PhysicalReg_XMM0 + the_num;
    }
    else { //do not use eax or edx try to use ebx & esi & ebx & edx & eax
        int the_num = -1;
        int kk;
        for(kk = 0; kk < num_GG_VRs_gp; kk++) {
            if(GG_VRs_gp[kk] == bb->infoBasicBlock[jj].regNum) {
                the_num = kk;
                break;
            }
        }
        if(the_num < 0) {
            the_num = num_GG_VRs_gp;
            num_GG_VRs_gp++;
        }
        if(the_num == 0) {
            bb->infoBasicBlock[jj].physicalReg_GG = PhysicalReg_ESI;
        }
        else if(the_num == 1)
            bb->infoBasicBlock[jj].physicalReg_GG = PhysicalReg_EBX;
        else if(the_num == 2)
            bb->infoBasicBlock[jj].physicalReg_GG = PhysicalReg_ECX;
        else if(the_num == 3)
            bb->infoBasicBlock[jj].physicalReg_GG = PhysicalReg_EDX;
        else if(the_num == 4)
            bb->infoBasicBlock[jj].physicalReg_GG = PhysicalReg_EAX;
        else
            LOGE("ERROR: too many gp GG VRs\n");
        GG_VRs_gp[the_num] = bb->infoBasicBlock[jj].regNum;
    }
}
//! determines whether a VR is local (L), globally local (GL) or global (GG)

//!A GL VR is dumped to memory at end of each basic block and at start of each basic block, we assume it is in memory;
//!A GG VR is in a pre-defined physical register at end of each basic block
void setTypeOfVR() {
    //update gtype of infoBasicBlock
    //this section updates allLogicalRegs.numExposedUsage, by calling hasExposedUsage2
    int k, jj, ii;
    UniqueRegister allLogicalRegs[MAX_REG_PER_METHOD];
    int num_logical_regs = 0;
    for(k = 0; k < num_bbs_for_method; k++) {
        for(jj = 0; jj < method_bbs_sorted[k]->num_regs; jj++) {
            LowOpndRegType tmpPtype = method_bbs_sorted[k]->infoBasicBlock[jj].physicalType;
            int tmpReg = method_bbs_sorted[k]->infoBasicBlock[jj].regNum;
            //will update aliased logical registers too
            //reg,fs will update reg,gp reg,xmm
            //reg,fs_s will update reg,gp reg,xmm
            method_bbs_sorted[k]->infoBasicBlock[jj].gType = GLOBALTYPE_GL; //initial value
            //if this register is already in allLogicalRegs, continue
            bool found = false;
            for(ii = 0; ii < num_logical_regs; ii++) {
                if(allLogicalRegs[ii].physicalType == tmpPtype &&
                   allLogicalRegs[ii].regNum == tmpReg) {
                    found = true;
                    break;
                }
            }
            if(hasExposedUsage2(method_bbs_sorted[k], jj)) {
                //increase numExposedUsage
                if(found) {
                    allLogicalRegs[ii].numExposedUsage++;
                } else {
                    allLogicalRegs[num_logical_regs].physicalType = tmpPtype;
                    allLogicalRegs[num_logical_regs].regNum = tmpReg;
                    allLogicalRegs[num_logical_regs].numExposedUsage = 1;
                    allLogicalRegs[num_logical_regs].physicalReg = PhysicalReg_Null;
                    num_logical_regs++;
                }
            }
        }//for jj
    }//for k
    //order VRs according to numExposedUsage
    UniqueRegister sortedLogicalRegs[MAX_REG_PER_METHOD];
    int num_sorted_VRs = 0;
    for(ii = 0; ii < num_logical_regs; ii++) {
        bool inserted = false;
        for(jj = 0; jj < num_sorted_VRs; jj++) {
            if(allLogicalRegs[ii].numExposedUsage > sortedLogicalRegs[jj].numExposedUsage) {
                inserted = true;
                for(k = num_sorted_VRs-1; k >= jj; k--)
                    sortedLogicalRegs[k+1] = sortedLogicalRegs[k];
                sortedLogicalRegs[jj] = allLogicalRegs[ii];
                num_sorted_VRs++;
                break;
            }
        }
        if(!inserted) {
            sortedLogicalRegs[num_sorted_VRs] = allLogicalRegs[ii];
            num_sorted_VRs++;
        }
    }
    //separate according to gp or xmm, max_index_global, max_index_global_xmm
    //NOTE: can't have a GG xmm aliased with a GG gp
    int max_index_global = num_sorted_VRs;
    int max_index_global_xmm = num_sorted_VRs;
    int index_array_gp[MAX_GLOBAL_VR];
    int index_array_xmm[MAX_GLOBAL_VR_XMM];
    int count1 = 0, count2 = 0;
    int num_global_gp = 0;
    int num_global_xmm = 0;
    for(ii = 0; ii < num_sorted_VRs; ii++) {
        if(sortedLogicalRegs[ii].physicalType == LowOpndRegType_gp) {
            index_array_gp[count1] = ii;
            count1++;
        }
        if(count1 == MAX_GLOBAL_VR) {
            max_index_global = ii;
            break;
        }
    }
    num_global_gp = count1;
    for(ii = 0; ii < num_sorted_VRs; ii++) {
        if(sortedLogicalRegs[ii].physicalType == LowOpndRegType_xmm) {
            int jj;
            bool hasAlias = false;
            for(jj = 0; jj < num_global_gp; jj++) {
                OverlapCase tmpT = getBPartiallyOverlapA(sortedLogicalRegs[ii].regNum, LowOpndRegType_xmm,
                                       sortedLogicalRegs[index_array_gp[jj]].regNum, LowOpndRegType_gp);
                if(tmpT != OVERLAP_NO) {
                    hasAlias = true;
                    break;
                }
            }
            if(!hasAlias) {
                index_array_xmm[count2] = ii;
                count2++;
            }
        }
        if(count2 == MAX_GLOBAL_VR_XMM) {
            max_index_global_xmm = ii;
            break;
        }
    }
    num_global_xmm = count2;

    //for VRs not in sortedLogicalRegs, set gType to GLOBALTYPE_L
    //for VRs at sortedLogicalRegs[0] to sortedLogicalRegs[MAX_GLOBAL_VR-1], set gType to GLOBALTYPE_GG
    num_GG_VRs_gp = 0;
    num_GG_VRs_xmm = 0;
    for(k = 0; k < num_bbs_for_method; k++) {
        for(jj = 0; jj < method_bbs_sorted[k]->num_regs; jj++) {
            LowOpndRegType tmpPtype = method_bbs_sorted[k]->infoBasicBlock[jj].physicalType;
            int tmpReg = method_bbs_sorted[k]->infoBasicBlock[jj].regNum;
            int index = -1;
            for(ii = 0; ii < num_sorted_VRs; ii++) {
                if(tmpPtype == sortedLogicalRegs[ii].physicalType && tmpReg == sortedLogicalRegs[ii].regNum) {
                    index = ii;
                    break;
                }
            }
            if(index < 0) {
                ////////////////// set to L if possible, affects correctness
                //If a gp VR (A) has no exposed usage,
                //  but a xmm VR that is aliased with A has exposed usage at another basic block
                //A can't be set to L, it should be dumped to memory
                if((method_bbs_sorted[k]->infoBasicBlock[jj].physicalType & MASK_FOR_TYPE) == LowOpndRegType_gp ||
                   (method_bbs_sorted[k]->infoBasicBlock[jj].physicalType & MASK_FOR_TYPE) == LowOpndRegType_ss ||
                   (method_bbs_sorted[k]->infoBasicBlock[jj].physicalType & MASK_FOR_TYPE) == LowOpndRegType_xmm) {
                    tmpReg = method_bbs_sorted[k]->infoBasicBlock[jj].regNum;
                    bool aliasExposed = hasOtherExposedUsage(getRegSize(method_bbs_sorted[k]->infoBasicBlock[jj].physicalType), tmpReg, method_bbs_sorted[k]);
                    if(!aliasExposed) {
                        method_bbs_sorted[k]->infoBasicBlock[jj].gType = GLOBALTYPE_L;
#ifdef DEBUG_GLOBALTYPE
                        LOGI("VIRTUAL register num %d type %d in basic block %d is set to L\n",
                               tmpReg,
                               method_bbs_sorted[k]->infoBasicBlock[jj].physicalType, method_bbs_sorted[k]->bb_index);
#endif
                    }
                }
            }//if index < 0
            else {
                //check whether we should set it to G
                bool shouldSet = false;
                if(tmpPtype == LowOpndRegType_xmm) {
                    for(ii = 0; ii < num_global_xmm; ii++) {
                        if(index == index_array_xmm[ii]) {
                            shouldSet = true;
                            break;
                        }
                    }
                    shouldSet = false; //disable global XMM VRs
                }
                if(tmpPtype == LowOpndRegType_gp) {
                    for(ii = 0; ii < num_global_gp; ii++) {
                        if(index == index_array_gp[ii]) {
                            shouldSet = true;
                            break;
                        }
                    }
                    /* disable GG VR since it does not improve performance */
                    shouldSet = false;
                }
                if(shouldSet && (sortedLogicalRegs[index].numExposedUsage > 1)) {
                    /* set type of the VR to GG */
                    method_bbs_sorted[k]->infoBasicBlock[jj].gType = GLOBALTYPE_GG;
                    allocateRegForGGVR(method_bbs_sorted[k], jj);
                    insertGGVRFromVirtualInfo(method_bbs_sorted[k], jj);
                }
            }
        }
    }
}

//! entry point to generate native code for a class

//!
int createCFGOfClass(ClassObject* clazz) {
    if(clazz->directMethodCount == 0 && clazz->virtualMethodCount == 0) return 0;
    int i, retval = 0;
    Method* method;
    for (i = 0; i < clazz->directMethodCount; i++) {
        method = &clazz->directMethods[i];
        retval = createCFG(method);
    }
    for (i = 0; i < clazz->virtualMethodCount; i++) {
        method = &clazz->virtualMethods[i];
        retval = createCFG(method);
    }
    return 0;
}

Edge_O1* createEdge(BasicBlock_O1* src, BasicBlock_O1* dst) {
    Edge_O1* edge = (Edge_O1*)malloc(sizeof(Edge_O1));
    edge->src = src;
    edge->dst = dst;
    return edge;
}
void connectBasicBlock(BasicBlock_O1* src, BasicBlock_O1* dst) {
    //if already exist an edge, do nothing
    int k;
    for(k = 0; k < src->num_out_edges; k++) {
        if(dst== src->out_edges[k]->dst) {
            return;
        }
    }
    Edge_O1* edge = createEdge(src, dst);
    if(src->num_out_edges >= MAX_NUM_EDGE_PER_BB) {
        LOGE("ERROR too many edges from BB %d\n", src->bb_index);
        exit(-1);
    }
    src->out_edges[src->num_out_edges] = edge;
    src->num_out_edges++;
    if(dst->num_in_edges >= MAX_NUM_EDGE_PER_BB) {
        LOGE("ERROR too many edges to BB %d\n", dst->bb_index);
        exit(-1);
    }
    dst->in_edges[dst->num_in_edges] = edge;
    dst->num_in_edges++;
}
//! handle a jump from basic block bb_prev to a target bytecode at relOff

//!
void handleJump(BasicBlock_O1* bb_prev, int relOff) { //
    int targetOff = offsetPC + relOff;
    //create BB starting from the target
    if(relOff > 0) {
#ifdef DEBUG_CFG
        LOGI("forward jump: insert to worklist BB %d target %x\n", bb_prev->bb_index, targetOff);
#endif
        insertWorklist(bb_prev, targetOff);
    }
    if(relOff < 0) {
        //find the block that has targetOff
        BasicBlock_O1* bb_m = findForOffset(targetOff);
#ifdef DEBUG_CFG
        LOGI("backward jump: start of the target BB %d: %x, target %x\n", bb_m->bb_index, bb_m->pc_start, targetOff);
#endif
        //check if the target is in the middle
        if(bb_m->pc_start == targetOff) {
            connectBasicBlock(bb_prev, bb_m);
        }
        else {
            BasicBlock_O1* bb_2nd = createBasicBlock(targetOff, bb_m->pc_end);
            bb_m->pc_end = targetOff;
            //edges from bb_m are moved to start from bb_2nd
            //update in edges too !
            int k, k2;
            for(k = 0; k < bb_m->num_out_edges; k++) {
                bb_2nd->out_edges[k] = bb_m->out_edges[k];
                //bb_m->out_edges[k]->dst->in_edges from bb_2nd
                BasicBlock_O1* dst = bb_m->out_edges[k]->dst;
                for(k2 = 0; k2 < dst->num_in_edges; k2++) {
                    if(dst->in_edges[k2]->src->bb_index == bb_m->bb_index)
                        dst->in_edges[k2]->src = bb_2nd;
                }
            }
            bb_2nd->num_out_edges = bb_m->num_out_edges;
            bb_m->num_out_edges = 0;
            connectBasicBlock(bb_m, bb_2nd);
            connectBasicBlock(bb_prev, bb_2nd);
        }
    }
    if(relOff == 0) {
        LOGE("ERROR: dead loop\n");
    }
}
//! print a CFG node

//!
void dump_node(BasicBlock_O1* bb) {
    LOGI("IDX %d [%x, %x)\n", bb->bb_index, bb->pc_start, bb->pc_end);
    LOGI("  OUT ");
    int k;
    for(k = 0; k < bb->num_out_edges; k++) {
        LOGI("%d ", bb->out_edges[k]->dst->bb_index);
    }
    LOGI("\n");
    LOGI("  IN ");
    for(k = 0; k < bb->num_in_edges; k++) {
        LOGI("%d ", bb->in_edges[k]->src->bb_index);
    }
    LOGI("\n");
}
//! print a control flow graph

//!
void dump_CFG(Method* method) { //start from bb_entry, go through method_bbs
#ifdef DEBUG_CFG
    LOGI("CFG of method %s%s\n", method->clazz->descriptor, method->name);
    if(bb_entry == NULL) return;
    int k;
    for(k = 0; k < num_bbs_for_method; k++)
        dump_node(method_bbs_sorted[k]);
#endif
}

#endif //!WITH_JIT
#endif //NCG_O1
