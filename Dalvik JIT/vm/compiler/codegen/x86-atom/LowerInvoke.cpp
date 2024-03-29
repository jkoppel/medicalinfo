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


/*! \file LowerInvoke.cpp
    \brief This file lowers the following bytecodes: INVOKE_XXX
*/
#include "libdex/DexOpcodes.h"
#include "libdex/DexFile.h"
#include "mterp/Mterp.h"
#include "Lower.h"
#include "NcgAot.h"
#include "enc_wrapper.h"

#if defined VTUNE_DALVIK
#include "compiler/JitProfiling.h"
#endif

char* streamMisPred = NULL;

/* according to callee, decide the ArgsDoneType*/
ArgsDoneType convertCalleeToType(const Method* calleeMethod) {
    if(calleeMethod == NULL)
        return ArgsDone_Full;
    if(dvmIsNativeMethod(calleeMethod))
        return ArgsDone_Native;
    return ArgsDone_Normal;
}
int common_invokeMethodRange(ArgsDoneType,
        const DecodedInstruction &decodedInst);
int common_invokeMethodNoRange(ArgsDoneType,
        const DecodedInstruction &decodedInst);
void gen_predicted_chain(bool isRange, u2 tmp, int IMMC, bool isInterface,
        int inputReg, const DecodedInstruction &decodedInst);

//inputs to common_invokeMethodRange: %ecx
//          common_errNoSuchMethod: %edx
#define P_GPR_1 PhysicalReg_ESI
#define P_GPR_2 PhysicalReg_EBX
#define P_GPR_3 PhysicalReg_ECX
#define P_SCRATCH_1 PhysicalReg_EDX
#define PP_GPR_1 PhysicalReg_EBX
#define PP_GPR_2 PhysicalReg_ESI
#define PP_GPR_3 PhysicalReg_EAX
#define PP_GPR_4 PhysicalReg_EDX

#ifdef WITH_JIT_INLINING_PHASE2
/*
 * The function here takes care the
 * branch over if prediction is correct and the misprediction target for misPredBranchOver.
 */
static void genLandingPadForMispredictedCallee(MIR* mir) {
    BasicBlock *fallThrough = traceCurrentBB->fallThrough;
    /* Bypass the move-result block if there is one */
    if (fallThrough->firstMIRInsn) {
        assert(fallThrough->firstMIRInsn->OptimizationFlags & MIR_INLINED_PRED);
        fallThrough = fallThrough->fallThrough;
    }
    /* Generate a branch over if the predicted inlining is correct */
    jumpToBasicBlock(stream, fallThrough->id);
    /* Hook up the target to the verification branch */
    int relativeNCG = stream - streamMisPred;
    unsigned instSize = encoder_get_inst_size(streamMisPred);
    relativeNCG -= instSize; //size of the instruction
    updateJumpInst(streamMisPred, OpndSize_8, relativeNCG);
}
#endif

//! LOWER bytecode INVOKE_VIRTUAL without usage of helper function

//!
int common_invoke_virtual_nohelper(bool isRange, u2 tmp, u2 vD, const MIR *mir)
{
    const DecodedInstruction &decodedInst = mir->dalvikInsn;

#ifdef WITH_JIT_INLINING_PHASE2
    /*
     * If the invoke has non-null misPredBranchOver, we need to generate
     * the non-inlined version of the invoke here to handle the
     * mispredicted case.
     */
    if (mir->meta.callsiteInfo->misPredBranchOver) {
        genLandingPadForMispredictedCallee (mir);
    }
#endif
    scratchRegs[2] = PhysicalReg_Null; scratchRegs[3] = PhysicalReg_Null;
    export_pc();
    constVREndOfBB();
    beforeCall("exception"); //dump GG, GL VRs
    /////////////////////////////////////////////////
#if !defined(WITH_JIT)
    scratchRegs[0] = PhysicalReg_SCRATCH_1; scratchRegs[1] = PhysicalReg_SCRATCH_2;
    get_res_methods(3, false);
    //LR[4] = vB*4(LR[3])
    move_mem_to_reg(OpndSize_32, tmp*4, 3, false, PhysicalReg_EAX, true);
    //cmp $0, LR[4]
    compare_imm_reg(OpndSize_32, 0, PhysicalReg_EAX, true); //resolved method
    //use P_GPR_3 to save return address
    conditional_jump(Condition_NE, ".invoke_virtual_resolved", true);
    rememberState(1);
    move_imm_to_reg(OpndSize_32, tmp, PhysicalReg_EAX, true);
    //use %eax, %esi, %edx, the register that will keep its value is: %ebx
    call_helper_API(".virtual_method_resolve");
    transferToState(1);
    insertLabel(".invoke_virtual_resolved", true);
#endif
    get_virtual_reg(vD, OpndSize_32, 5, false);

    if ( (mir->OptimizationFlags & MIR_IGNORE_NULL_CHECK) == 0)
    {
        simpleNullCheck(5, false, vD);
    }
#ifndef PREDICTED_CHAINING
    move_mem_to_reg(OpndSize_32, offObject_clazz, 5, false, 6, false); //clazz of "this"
    move_mem_to_reg(OpndSize_32, offClassObject_vtable, 6, false, 7, false); //vtable
#if defined(WITH_JIT)
    /* method is already resolved in trace-based JIT */
    int methodIndex =
                currentMethod->clazz->pDvmDex->pResMethods[tmp]->methodIndex;
    move_mem_to_reg(OpndSize_32, methodIndex*4, 7, false, PhysicalReg_ECX, true);
#else
    movez_mem_to_reg(OpndSize_16, offMethod_methodIndex, PhysicalReg_EAX, true, 8, false); //index to vtable
    move_mem_scale_to_reg(OpndSize_32, 7, false, 8, false, 4, PhysicalReg_ECX, true);
#endif
    if(isRange) {
        common_invokeMethodRange(ArgsDone_Full);
    }
    else {
        common_invokeMethodNoRange(ArgsDone_Full);
    }
#else
    int methodIndex =
                currentMethod->clazz->pDvmDex->pResMethods[tmp]->methodIndex;
    gen_predicted_chain(isRange, tmp, methodIndex * 4, false, 5/*tmp5*/,
            decodedInst);
#endif
    ///////////////////////////////////
    return 0;
}

#if 0 /* Code is deprecated. If reenabling, must add parameter for decoded instruction */
//! wrapper to call either common_invoke_virtual_helper or common_invoke_virtual_nohelper

//!
int common_invoke_virtual(bool isRange, u2 tmp, u2 vD) {
#ifdef INC_NCG_O0
    if(gDvm.helper_switch[1]) {// && (!isRange)) {
        return common_invoke_virtual_helper(isRange, tmp, vD);
    } else
#endif
    {
        return common_invoke_virtual_nohelper(isRange, tmp, vD);
    }
}
#endif
#undef P_GPR_1
#undef P_GPR_2
#undef P_GPR_3
#undef P_SCRATCH_1
#undef PP_GPR_1
#undef PP_GPR_2
#undef PP_GPR_3
#undef PP_GPR_4

#define P_GPR_1 PhysicalReg_ESI
#define P_GPR_2 PhysicalReg_EBX
#define P_GPR_3 PhysicalReg_EDX
#define PP_GPR_1 PhysicalReg_EBX
#define PP_GPR_2 PhysicalReg_ESI
#define PP_GPR_3 PhysicalReg_EAX
#define PP_GPR_4 PhysicalReg_EDX
//! common section to lower INVOKE_SUPER

//! It will use helper function if the switch is on
int common_invoke_super(bool isRange, u2 tmp,
        const DecodedInstruction &decodedInst) {
    export_pc();
    constVREndOfBB();
    beforeCall("exception"); //dump GG, GL VRs
#ifdef INC_NCG_O0
    if(gDvm.helper_switch[15]) {
        common_invoke_super_helper(isRange, tmp);
    }
    else
#endif
    {
        ///////////////////////
        scratchRegs[2] = PhysicalReg_Null; scratchRegs[3] = PhysicalReg_Null;
#if !defined(WITH_JIT)
        scratchRegs[0] = PhysicalReg_SCRATCH_1; scratchRegs[1] = PhysicalReg_SCRATCH_2;
        get_res_methods(3, false);
        //LR[4] = vB*4(LR[3])
        move_mem_to_reg(OpndSize_32, tmp*4, 3, false, PhysicalReg_EAX, true);
        //cmp $0, LR[4]
        compare_imm_reg(OpndSize_32, 0, PhysicalReg_EAX, true);

        conditional_jump(Condition_NE, ".LinvokeSuper_resolved", true);
        rememberState(1);
        move_imm_to_reg(OpndSize_32, tmp, PhysicalReg_EAX, true);
        call(".virtual_method_resolve"); //in %eax
        transferToState(1);
        insertLabel(".LinvokeSuper_resolved", true);
        scratchRegs[0] = PhysicalReg_SCRATCH_3; scratchRegs[1] = PhysicalReg_SCRATCH_4;
        get_glue_method_class(6, false);
        move_mem_to_reg(OpndSize_32, offClassObject_super, 6, false, 7, false);
        movez_mem_to_reg(OpndSize_16, offMethod_methodIndex, PhysicalReg_EAX, true, 8, false);
        compare_mem_reg(OpndSize_32, offClassObject_vtableCount, 7, false, 8, false);

        conditional_jump_global_API(Condition_NC, ".invoke_super_nsm", false);
        move_mem_to_reg(OpndSize_32, offClassObject_vtable, 7, false, 9, false);
        move_mem_scale_to_reg(OpndSize_32, 9, false, 8, false, 4, PhysicalReg_ECX, true);
        const Method *calleeMethod = NULL;
#else
        /* method is already resolved in trace-based JIT */
        int mIndex = currentMethod->clazz->pDvmDex->
                pResMethods[tmp]->methodIndex;
        const Method *calleeMethod =
                currentMethod->clazz->super->vtable[mIndex];
        move_imm_to_reg(OpndSize_32, (int) calleeMethod, PhysicalReg_ECX, true);
#endif
        if(isRange) {
            common_invokeMethodRange(convertCalleeToType(calleeMethod),
                    decodedInst);
        }
        else {
            common_invokeMethodNoRange(convertCalleeToType(calleeMethod),
                    decodedInst);
        }
        ///////////////////////////////
    }
    return 0;
}
#undef PP_GPR_1
#undef PP_GPR_2
#undef PP_GPR_3
#undef PP_GPR_4

//! helper function to handle no such method error

//!
int invoke_super_nsm() {
    insertLabel(".invoke_super_nsm", false);
    //NOTE: it seems that the name in %edx is not used in common_errNoSuchMethod
    move_mem_to_reg(OpndSize_32, offMethod_name, PhysicalReg_EAX, true, PhysicalReg_EDX, true); //method name
    unconditional_jump("common_errNoSuchMethod", false);
    return 0;
}
#undef P_GPR_1
#undef P_GPR_2
#undef P_GPR_3

#define P_GPR_1 PhysicalReg_EBX
#define P_GPR_2 PhysicalReg_ESI
#define P_GPR_3 PhysicalReg_ECX
#define PP_GPR_1 PhysicalReg_EBX
#define PP_GPR_2 PhysicalReg_ESI
#define PP_GPR_3 PhysicalReg_EAX
#define PP_GPR_4 PhysicalReg_EDX
//! common section to lower INVOKE_DIRECT

//! It will use helper function if the switch is on
int common_invoke_direct(bool isRange, u2 tmp, u2 vD, const MIR *mir)
{
    const DecodedInstruction &decodedInst = mir->dalvikInsn;
    //%ecx can be used as scratch when calling export_pc, get_res_methods and resolve_method
    export_pc();
    constVREndOfBB();
    beforeCall("exception"); //dump GG, GL VRs
#ifdef INC_NCG_O0
    if(gDvm.helper_switch[10]) {
        common_invoke_direct_helper(isRange, tmp, vD);
    }
    else
#endif
    {
        ////////////////////////////////////
#if !defined(WITH_JIT)
        scratchRegs[0] = PhysicalReg_SCRATCH_1; scratchRegs[1] = PhysicalReg_SCRATCH_2;
        scratchRegs[2] = PhysicalReg_Null; scratchRegs[3] = PhysicalReg_Null;
        get_res_methods(3, false);
        //LR[4] = vB*4(LR[3])
        move_mem_to_reg(OpndSize_32, tmp*4, 3, false, PhysicalReg_ECX, true);
#endif
        get_virtual_reg(vD, OpndSize_32, 5, false);
        if ( (mir->OptimizationFlags & MIR_IGNORE_NULL_CHECK) == 0)
        {
            simpleNullCheck(5, false, vD);
        }
#if !defined(WITH_JIT)
        //cmp $0, LR[4]
        compare_imm_reg(OpndSize_32, 0, PhysicalReg_ECX, true);
        conditional_jump(Condition_NE, ".LinvokeDirect_resolved", true);
        rememberState(1);
        move_imm_to_reg(OpndSize_32, tmp, PhysicalReg_EAX, true);
        call_helper_API(".direct_method_resolve"); //in %eax
        move_reg_to_reg(OpndSize_32, PhysicalReg_EAX, true, PhysicalReg_ECX, true);
        transferToState(1);
        insertLabel(".LinvokeDirect_resolved", true);
        const Method *calleeMethod = NULL;
#else
        /* method is already resolved in trace-based JIT */
        const Method *calleeMethod =
                currentMethod->clazz->pDvmDex->pResMethods[tmp];
        move_imm_to_reg(OpndSize_32, (int) calleeMethod, PhysicalReg_ECX, true);
#endif
        //%ecx passed to common_invokeMethod...
        if(isRange) {
            common_invokeMethodRange(convertCalleeToType(calleeMethod),
                    decodedInst);
        }
        else {
            common_invokeMethodNoRange(convertCalleeToType(calleeMethod),
                    decodedInst);
        }
        ////////////////////////////
    }
    return 0;
}
#undef P_GPR_1
#undef P_GPR_2
#undef P_GPR_3
#undef PP_GPR_1
#undef PP_GPR_2
#undef PP_GPR_3
#undef PP_GPR_4

#define P_GPR_1  PhysicalReg_EBX
#define P_GPR_3  PhysicalReg_ECX
#define PP_GPR_1 PhysicalReg_EBX
#define PP_GPR_2 PhysicalReg_ESI
#define PP_GPR_3 PhysicalReg_EAX
#define PP_GPR_4 PhysicalReg_EDX
//! common section to lower INVOKE_STATIC

//! It will use helper function if the switch is on
int common_invoke_static(bool isRange, u2 tmp,
        const DecodedInstruction &decodedInst) {
    //%ecx can be used as scratch when calling export_pc, get_res_methods and resolve_method
    export_pc();
    constVREndOfBB();
    beforeCall("exception"); //dump GG, GL VRs
#ifdef INC_NCG_O0
    if(gDvm.helper_switch[2]) {
        common_invoke_static_helper(isRange, tmp);
    } else
#endif
    {
        ////////////////////////////
#if !defined(WITH_JIT)
        scratchRegs[0] = PhysicalReg_SCRATCH_1; scratchRegs[1] = PhysicalReg_SCRATCH_2;
        scratchRegs[2] = PhysicalReg_Null;      scratchRegs[3] = PhysicalReg_Null;
        get_res_methods(3, false);
        //LR[4] = vB*4(LR[3])
        move_mem_to_reg(OpndSize_32, tmp*4, 3, false, PhysicalReg_ECX, true);
        compare_imm_reg(OpndSize_32, 0, PhysicalReg_ECX, true);

        conditional_jump(Condition_NE, ".LinvokeStatic_resolved", true);
        rememberState(1);
        move_imm_to_reg(OpndSize_32, tmp, PhysicalReg_EAX, true);
        call(".static_method_resolve"); //in %eax
        move_reg_to_reg(OpndSize_32, PhysicalReg_EAX, true, PhysicalReg_ECX, true);
        transferToState(1);
        insertLabel(".LinvokeStatic_resolved", true);
        const Method *calleeMethod = NULL;
#else
        /* method is already resolved in trace-based JIT */
        const Method *calleeMethod =
                currentMethod->clazz->pDvmDex->pResMethods[tmp];
        move_imm_to_reg(OpndSize_32, (int) calleeMethod, PhysicalReg_ECX, true);
#endif
        //%ecx passed to common_invokeMethod...
        if(isRange) {
            common_invokeMethodRange(convertCalleeToType(calleeMethod),
                    decodedInst);
        }
        else {
            common_invokeMethodNoRange(convertCalleeToType(calleeMethod),
                    decodedInst);
        }
        ////////////////////////
    }
    return 0;
}
#undef P_GPR_1
#undef PP_GPR_1
#undef PP_GPR_2
#undef PP_GPR_3
#undef PP_GPR_4

#define P_GPR_1 PhysicalReg_EBX
#define P_GPR_2 PhysicalReg_EAX //scratch
#define P_GPR_3 PhysicalReg_ECX
#define P_SCRATCH_1 PhysicalReg_ESI //clazz of object
#define PP_GPR_1 PhysicalReg_EBX
#define PP_GPR_2 PhysicalReg_ESI
#define PP_GPR_3 PhysicalReg_EAX
#define PP_GPR_4 PhysicalReg_EDX
//! common section to lower INVOKE_INTERFACE

//! It will use helper function if the switch is on
int common_invoke_interface(bool isRange, u2 tmp, u2 vD, const MIR *mir) {

    const DecodedInstruction &decodedInst = mir->dalvikInsn;

#ifdef WITH_JIT_INLINING_PHASE2
    /*
     * If the invoke has non-null misPredBranchOver, we need to generate
     * the non-inlined version of the invoke here to handle the
     * mispredicted case.
     */
    if (mir->meta.callsiteInfo->misPredBranchOver) {
        genLandingPadForMispredictedCallee (mir);
    }
#endif
    export_pc(); //use %edx
    constVREndOfBB();
    beforeCall("exception"); //dump GG, GL VRs
#ifdef INC_NCG_O0
    if(gDvm.helper_switch[16]) {
        common_invoke_interface_helper(isRange, tmp, vD);
    }
    else
#endif
    {
        ///////////////////////
        scratchRegs[2] = PhysicalReg_Null; scratchRegs[3] = PhysicalReg_Null;
        get_virtual_reg(vD, OpndSize_32, 1, false);
        if ( (mir->OptimizationFlags & MIR_IGNORE_NULL_CHECK) == 0)
        {
            simpleNullCheck(1, false, vD);
        }

#ifndef PREDICTED_CHAINING
#if !defined(WITH_JIT)
        scratchRegs[0] = PhysicalReg_SCRATCH_1; scratchRegs[1] = PhysicalReg_Null;
        get_glue_dvmdex(3, false);
#endif
        load_effective_addr(-16, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
        move_imm_to_mem(OpndSize_32, tmp, 4, PhysicalReg_ESP, true);
#if !defined(WITH_JIT)
        move_reg_to_mem(OpndSize_32, 3, false, 12, PhysicalReg_ESP, true);
#else
        /* for trace-based JIT, pDvmDex is a constant at JIT time
           4th argument to dvmFindInterfaceMethodInCache at -4(%esp) */
        move_imm_to_mem(OpndSize_32, (int) currentMethod->clazz->pDvmDex, 12, PhysicalReg_ESP, true);
#endif
#if !defined(WITH_JIT)
        scratchRegs[0] = PhysicalReg_SCRATCH_2; scratchRegs[1] = PhysicalReg_Null;
        get_glue_method(4, false);
#endif
        move_mem_to_reg(OpndSize_32, offObject_clazz, 1, false, 5, false);
#if !defined(WITH_JIT)
        move_reg_to_mem(OpndSize_32, 4, false, 8, PhysicalReg_ESP, true);
#else
        /* for trace-based JIT, method is a constant at JIT time
           3rd argument to dvmFindInterfaceMethodInCache at 8(%esp) */
        move_imm_to_mem(OpndSize_32, (int) currentMethod, 8, PhysicalReg_ESP, true);
#endif
        move_reg_to_mem(OpndSize_32, 5, false, 0, PhysicalReg_ESP, true);
        scratchRegs[0] = PhysicalReg_SCRATCH_3; scratchRegs[1] = PhysicalReg_Null;
        call_dvmFindInterfaceMethodInCache();
        load_effective_addr(16, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
        compare_imm_reg(OpndSize_32, 0, PhysicalReg_EAX, true);

        conditional_jump_global_API(Condition_E, "common_exceptionThrown", false);
        move_reg_to_reg(OpndSize_32, PhysicalReg_EAX, true, PhysicalReg_ECX, true);
        if(isRange) {
            common_invokeMethodRange(ArgsDone_Full);
        }
        else {
            common_invokeMethodNoRange(ArgsDone_Full);
        }
#else
        gen_predicted_chain(isRange, tmp, -1, true /*interface*/, 1/*tmp1*/,
                decodedInst);
#endif
        ///////////////////////
    }
    return 0;
}
#undef PP_GPR_1
#undef PP_GPR_2
#undef PP_GPR_3
#undef PP_GPR_4
#undef P_GPR_1
#undef P_GPR_2
#undef P_GPR_3
#undef P_SCRATCH_1
//! lower bytecode INVOKE_VIRTUAL by calling common_invoke_virtual

//!
int op_invoke_virtual(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_INVOKE_VIRTUAL);
#ifdef WITH_JIT_INLINING
    /* An invoke with the MIR_INLINED is effectively a no-op */
    if (mir->OptimizationFlags & MIR_INLINED)
        return 0;
#endif
    // A|G|op BBBB F|E|D|C
    // C: the first argument, which is the "this" pointer
    // A: argument count
    // C, D, E, F, G: arguments
    u2 vD = mir->dalvikInsn.vC; /* Note: variable is still named vD because
                                               of historical reasons. In reality, first
                                               argument is in vC */
    u2 tmp = mir->dalvikInsn.vB;
    int retval = common_invoke_virtual_nohelper(false/*not range*/, tmp, vD, mir);
#if defined(ENABLE_TRACING) && !defined(TRACING_OPTION2)
    insertMapWorklist(offsetPC+3, stream - streamMethodStart, 1); //check when helper switch is on
#endif
    return retval;
}
//! lower bytecode INVOKE_SUPER by calling common_invoke_super

//!
int op_invoke_super(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_INVOKE_SUPER);
#ifdef WITH_JIT_INLINING
    /* An invoke with the MIR_INLINED is effectively a no-op */
    if (mir->OptimizationFlags & MIR_INLINED)
        return 0;
#endif
    // A|G|op BBBB F|E|D|C
    // C: the first argument, which is the "this" pointer
    // A: argument count
    // C, D, E, F, G: arguments
    u2 tmp = mir->dalvikInsn.vB;
    int retval = common_invoke_super(false/*not range*/, tmp, mir->dalvikInsn);
#if defined(ENABLE_TRACING) && !defined(TRACING_OPTION2)
    insertMapWorklist(offsetPC+3, stream - streamMethodStart, 1); //check when helper switch is on
#endif
    return retval;
}
//! lower bytecode INVOKE_DIRECT by calling common_invoke_direct

//!
int op_invoke_direct(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_INVOKE_DIRECT);
#ifdef WITH_JIT_INLINING
    /* An invoke with the MIR_INLINED is effectively a no-op */
    if (mir->OptimizationFlags & MIR_INLINED)
        return 0;
#endif
    // A|G|op BBBB F|E|D|C
    // C: the first argument, which is the "this" pointer
    // A: argument count
    // C, D, E, F, G: arguments
    u2 vD = mir->dalvikInsn.vC; /* Note: variable is still named vD because
                                               of historical reasons. In reality, first
                                               argument is in vC */
    u2 tmp = mir->dalvikInsn.vB;
    int retval = common_invoke_direct(false/*not range*/, tmp, vD, mir);
#if defined(ENABLE_TRACING) && !defined(TRACING_OPTION2)
    insertMapWorklist(offsetPC+3, stream - streamMethodStart, 1); //check when helper switch is on
#endif
    return retval;
}
//! lower bytecode INVOKE_STATIC by calling common_invoke_static

//!
int op_invoke_static(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_INVOKE_STATIC);
#ifdef WITH_JIT_INLINING
    /* An invoke with the MIR_INLINED is effectively a no-op */
    if (mir->OptimizationFlags & MIR_INLINED)
        return 0;
#endif
    // A|G|op BBBB F|E|D|C
    // C: the first argument, which is the "this" pointer
    // A: argument count
    // C, D, E, F, G: arguments
    u2 tmp = mir->dalvikInsn.vB;
    int retval = common_invoke_static(false/*not range*/, tmp, mir->dalvikInsn);
#if defined(ENABLE_TRACING) && !defined(TRACING_OPTION2)
    insertMapWorklist(offsetPC+3, stream - streamMethodStart, 1); //check when helper switch is on
#endif
    return retval;
}
//! lower bytecode INVOKE_INTERFACE by calling common_invoke_interface

//!
int op_invoke_interface(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_INVOKE_INTERFACE);
#ifdef WITH_JIT_INLINING
    /* An invoke with the MIR_INLINED is effectively a no-op */
    if (mir->OptimizationFlags & MIR_INLINED)
        return 0;
#endif
    // A|G|op BBBB F|E|D|C
    // C: the first argument, which is the "this" pointer
    // A: argument count
    // C, D, E, F, G: arguments
    u2 vD = mir->dalvikInsn.vC; /* Note: variable is still named vD because
                                               of historical reasons. In reality, first
                                               argument is in vC */
    u2 tmp = mir->dalvikInsn.vB;
    int retval = common_invoke_interface(false/*not range*/, tmp, vD, mir);
#if defined(ENABLE_TRACING) && !defined(TRACING_OPTION2)
    insertMapWorklist(offsetPC+3, stream - streamMethodStart, 1); //check when helper switch is on
#endif
    return retval;
}
//! lower bytecode INVOKE_VIRTUAL_RANGE by calling common_invoke_virtual

//!
int op_invoke_virtual_range(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_INVOKE_VIRTUAL_RANGE);
#ifdef WITH_JIT_INLINING
    /* An invoke with the MIR_INLINED is effectively a no-op */
    if (mir->OptimizationFlags & MIR_INLINED)
        return 0;
#endif
    //AA|op BBBB CCCC
    //CCCC: the first argument, which is the "this" pointer
    //AA: argument count
    u2 vD = mir->dalvikInsn.vC; /* Note: variable is still named vD because
                                               of historical reasons. In reality, first
                                               argument is in vCCCC */
    u2 tmp = mir->dalvikInsn.vB; //BBBB, method index
    int retval = common_invoke_virtual_nohelper(true/*range*/, tmp, vD, mir);
#if defined(ENABLE_TRACING) && !defined(TRACING_OPTION2)
    insertMapWorklist(offsetPC+3, stream - streamMethodStart, 1); //check when helper switch is on
#endif
    return retval;
}
//! lower bytecode INVOKE_SUPER_RANGE by calling common_invoke_super

//!
int op_invoke_super_range(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_INVOKE_SUPER_RANGE);
#ifdef WITH_JIT_INLINING
    /* An invoke with the MIR_INLINED is effectively a no-op */
    if (mir->OptimizationFlags & MIR_INLINED)
        return 0;
#endif
    u2 tmp = mir->dalvikInsn.vB; //BBBB, method index
    int retval = common_invoke_super(true/*range*/, tmp, mir->dalvikInsn);
#if defined(ENABLE_TRACING) && !defined(TRACING_OPTION2)
    insertMapWorklist(offsetPC+3, stream - streamMethodStart, 1); //check when helper switch is on
#endif
    return retval;
}
//! lower bytecode INVOKE_DIRECT_RANGE by calling common_invoke_direct

//!
int op_invoke_direct_range(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_INVOKE_DIRECT_RANGE);
#ifdef WITH_JIT_INLINING
    /* An invoke with the MIR_INLINED is effectively a no-op */
    if (mir->OptimizationFlags & MIR_INLINED)
        return 0;
#endif
    u2 vD = mir->dalvikInsn.vC; /* Note: variable is still named vD because
                                               of historical reasons. In reality, first
                                               argument is in vCCCC */
    u2 tmp = mir->dalvikInsn.vB; //BBBB, method index
    int retval = common_invoke_direct(true/*range*/, tmp, vD, mir);
#if defined(ENABLE_TRACING) && !defined(TRACING_OPTION2)
    insertMapWorklist(offsetPC+3, stream - streamMethodStart, 1); //check when helper switch is on
#endif
    return retval;
}
//! lower bytecode INVOKE_STATIC_RANGE by calling common_invoke_static

//!
int op_invoke_static_range(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_INVOKE_STATIC_RANGE);
#ifdef WITH_JIT_INLINING
    /* An invoke with the MIR_INLINED is effectively a no-op */
    if (mir->OptimizationFlags & MIR_INLINED)
        return 0;
#endif
    u2 tmp = mir->dalvikInsn.vB; //BBBB, method index
    int retval = common_invoke_static(true/*range*/, tmp, mir->dalvikInsn);
#if defined(ENABLE_TRACING) && !defined(TRACING_OPTION2)
    insertMapWorklist(offsetPC+3, stream - streamMethodStart, 1); //check when helper switch is on
#endif
    return retval;
}
//! lower bytecode INVOKE_INTERFACE_RANGE by calling common_invoke_interface

//!
int op_invoke_interface_range(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_INVOKE_INTERFACE_RANGE);
#ifdef WITH_JIT_INLINING
    /* An invoke with the MIR_INLINED is effectively a no-op */
    if (mir->OptimizationFlags & MIR_INLINED)
        return 0;
#endif
    u2 vD = mir->dalvikInsn.vC; /* Note: variable is still named vD because
                                               of historical reasons. In reality, first
                                               argument is in vCCCC */
    u2 tmp = mir->dalvikInsn.vB; //BBBB, method index
    int retval = common_invoke_interface(true/*range*/, tmp, vD, mir);
#if defined(ENABLE_TRACING) && !defined(TRACING_OPTION2)
    insertMapWorklist(offsetPC+3, stream - streamMethodStart1); //check when helper switch is on
#endif
    return retval;
}

//used %ecx, %edi, %esp %ebp
#define P_GPR_1 PhysicalReg_EBX
#define P_SCRATCH_1 PhysicalReg_ESI
#define P_SCRATCH_2 PhysicalReg_EAX
#define P_SCRATCH_3 PhysicalReg_EDX
#define P_SCRATCH_4 PhysicalReg_ESI
#define P_SCRATCH_5 PhysicalReg_EAX
//! pass the arguments for invoking method without range

//!
int common_invokeMethodNoRange_noJmp(const DecodedInstruction &decodedInst) {
#if defined VTUNE_DALVIK
    int startStreamPtr = (int)stream;
#endif

    u2 count = decodedInst.vA;

    // Please note that vA, vD, vE, vF, and vG might have
    // invalid values. Check variable "count" before accessing.
    u2 vD = decodedInst.arg[0];
    u2 vE = decodedInst.arg[1];
    u2 vF = decodedInst.arg[2];
    u2 vG = decodedInst.arg[3];
    u2 vA = decodedInst.arg[4];

    int offsetFromSaveArea = -4;
    if(count == 5) {
        get_virtual_reg(vA, OpndSize_32, 22, false);
        move_reg_to_mem(OpndSize_32, 22, false, offsetFromSaveArea-sizeofStackSaveArea, PhysicalReg_FP, true);
        offsetFromSaveArea -= 4;
    }
    if(count >= 4) {
        get_virtual_reg(vG, OpndSize_32, 23, false);
        move_reg_to_mem(OpndSize_32, 23, false, offsetFromSaveArea-sizeofStackSaveArea, PhysicalReg_FP, true);
        offsetFromSaveArea -= 4;
    }
    if(count >= 3) {
        get_virtual_reg(vF, OpndSize_32, 24, false);
        move_reg_to_mem(OpndSize_32, 24, false, offsetFromSaveArea-sizeofStackSaveArea, PhysicalReg_FP, true);
        offsetFromSaveArea -= 4;
    }
    if(count >= 2) {
        get_virtual_reg(vE, OpndSize_32, 25, false);
        move_reg_to_mem(OpndSize_32, 25, false, offsetFromSaveArea-sizeofStackSaveArea, PhysicalReg_FP, true);
        offsetFromSaveArea -= 4;
    }
    if(count >= 1) {
        get_virtual_reg(vD, OpndSize_32, 26, false);
        move_reg_to_mem(OpndSize_32, 26, false, offsetFromSaveArea-sizeofStackSaveArea, PhysicalReg_FP, true);
    }

#if defined VTUNE_DALVIK
    if(gDvmJit.vtuneInfo != kVTuneInfoDisabled) {
        int endStreamPtr = (int)stream;
        sendLabelInfoToVTune(startStreamPtr, endStreamPtr, "common_invokeMethodNoRange_noJmp");
    }
#endif
    return 0;
}

int common_invokeMethod_Jmp(ArgsDoneType form) {
#if defined VTUNE_DALVIK
    int startStreamPtr = (int)stream;
#endif

#if defined(WITH_JIT)
    nextVersionOfHardReg(PhysicalReg_EDX, 1);
    move_imm_to_reg(OpndSize_32, (int)rPC, PhysicalReg_EDX, true);
    //arguments needed in ArgsDone:
    //    start of HotChainingCell for next bytecode: -4(%esp)
    //    start of InvokeSingletonChainingCell for callee: -8(%esp)
    load_effective_addr(-8, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    if(!gDvmJit.scheduling)
        insertChainingWorklist(traceCurrentBB->fallThrough->id, stream);
    move_chain_to_mem(OpndSize_32, traceCurrentBB->fallThrough->id, 4, PhysicalReg_ESP, true);
    // for honeycomb: JNI call doesn't need a chaining cell, so the taken branch is null
    if(!gDvmJit.scheduling && traceCurrentBB->taken)
        insertChainingWorklist(traceCurrentBB->taken->id, stream);
    int takenId = traceCurrentBB->taken ? traceCurrentBB->taken->id : 0;
    move_chain_to_mem(OpndSize_32, takenId, 0, PhysicalReg_ESP, true);
    if(form == ArgsDone_Full)
        unconditional_jump_global_API(".invokeArgsDone_jit", false);
    else if(form == ArgsDone_Native)
        unconditional_jump_global_API(".invokeArgsDone_native", false);
    else
        unconditional_jump_global_API(".invokeArgsDone_normal", false);
#else
    goto_invokeArgsDone();
#endif

#if defined VTUNE_DALVIK
    if(gDvmJit.vtuneInfo != kVTuneInfoDisabled) {
        int endStreamPtr = (int)stream;
        sendLabelInfoToVTune(startStreamPtr, endStreamPtr, "common_invokeMethod_Jmp");
    }
#endif
    return 0;
}

int common_invokeMethodNoRange(ArgsDoneType form, const DecodedInstruction &decodedInst) {
    common_invokeMethodNoRange_noJmp(decodedInst);
    common_invokeMethod_Jmp(form);
    return 0;
}

#undef P_GPR_1
#undef P_SCRATCH_1
#undef P_SCRATCH_2
#undef P_SCRATCH_3
#undef P_SCRATCH_4
#undef P_SCRATCH_5

//input: %ecx (method to call)
#define P_GPR_1 PhysicalReg_EBX
#define P_GPR_2 PhysicalReg_ESI
#define P_GPR_3 PhysicalReg_EDX //not used with P_SCRATCH_2
#define P_SCRATCH_1 PhysicalReg_EAX
#define P_SCRATCH_2 PhysicalReg_EDX
#define P_SCRATCH_3 PhysicalReg_EAX
#define P_SCRATCH_4 PhysicalReg_EDX
#define P_SCRATCH_5 PhysicalReg_EAX
#define P_SCRATCH_6 PhysicalReg_EDX
#define P_SCRATCH_7 PhysicalReg_EAX
#define P_SCRATCH_8 PhysicalReg_EDX
#define P_SCRATCH_9 PhysicalReg_EAX
#define P_SCRATCH_10 PhysicalReg_EDX
//! pass the arguments for invoking method with range

//! loop is unrolled when count <= 10
int common_invokeMethodRange_noJmp(const DecodedInstruction &decodedInst) {
#if defined VTUNE_DALVIK
    int startStreamPtr = (int)stream;
#endif

    u2 count = decodedInst.vA;
    u2 vD = decodedInst.vC; //the first argument

    savearea_from_fp(21, false);
    //vD to rFP-4*count-20
    //vD+1 to rFP-4*count-20+4 = rFP-20-4*(count-1)
    if(count >= 1 && count <= 10) {
        get_virtual_reg(vD, OpndSize_32, 22, false);
        move_reg_to_mem(OpndSize_32, 22, false, -4*count, 21, false);
    }
    if(count >= 2 && count <= 10) {
        get_virtual_reg(vD+1, OpndSize_32, 23, false);
        move_reg_to_mem(OpndSize_32, 23, false, -4*(count-1), 21, false);
    }
    if(count >= 3 && count <= 10) {
        get_virtual_reg(vD+2, OpndSize_32, 24, false);
        move_reg_to_mem(OpndSize_32, 24, false, -4*(count-2), 21, false);
    }
    if(count >= 4 && count <= 10) {
        get_virtual_reg(vD+3, OpndSize_32, 25, false);
        move_reg_to_mem(OpndSize_32, 25, false, -4*(count-3), 21, false);
    }
    if(count >= 5 && count <= 10) {
        get_virtual_reg(vD+4, OpndSize_32, 26, false);
        move_reg_to_mem(OpndSize_32, 26, false, -4*(count-4), 21, false);
    }
    if(count >= 6 && count <= 10) {
        get_virtual_reg(vD+5, OpndSize_32, 27, false);
        move_reg_to_mem(OpndSize_32, 27, false, -4*(count-5), 21, false);
    }
    if(count >= 7 && count <= 10) {
        get_virtual_reg(vD+6, OpndSize_32, 28, false);
        move_reg_to_mem(OpndSize_32, 28, false, -4*(count-6), 21, false);
    }
    if(count >= 8 && count <= 10) {
        get_virtual_reg(vD+7, OpndSize_32, 29, false);
        move_reg_to_mem(OpndSize_32, 29, false, -4*(count-7), 21, false);
    }
    if(count >= 9 && count <= 10) {
        get_virtual_reg(vD+8, OpndSize_32, 30, false);
        move_reg_to_mem(OpndSize_32, 30, false, -4*(count-8), 21, false);
    }
    if(count == 10) {
        get_virtual_reg(vD+9, OpndSize_32, 31, false);
        move_reg_to_mem(OpndSize_32, 31, false, -4*(count-9), 21, false);
    }
    if(count > 10) {
        //dump to memory first, should we set physicalReg to Null?
        //this bytecodes uses a set of virtual registers (update getVirtualInfo)
        //this is necessary to correctly insert transfer points
        int k;
        for(k = 0; k < count; k++) {
            spillVirtualReg(vD+k, LowOpndRegType_gp, true); //will update refCount
        }
        load_effective_addr(4*vD, PhysicalReg_FP, true, 12, false);
        alu_binary_imm_reg(OpndSize_32, sub_opc, 4*count, 21, false);
        move_imm_to_reg(OpndSize_32, count, 13, false);
        insertLabel(".invokeMethod_1", true); //if checkDup: will perform work from ShortWorklist
        rememberState(1);
        move_mem_to_reg(OpndSize_32, 0, 12, false, 14, false);
        move_reg_to_mem(OpndSize_32, 14, false, 0, 21, false);
        load_effective_addr(4, 12, false, 12, false);
        alu_binary_imm_reg(OpndSize_32, sub_opc, 1, 13, false);
        load_effective_addr(4, 21, false, 21, false);
        transferToState(1);
        conditional_jump(Condition_NE, ".invokeMethod_1", true); //backward branch
    }

#if defined VTUNE_DALVIK
    if(gDvmJit.vtuneInfo != kVTuneInfoDisabled) {
        int endStreamPtr = (int)stream;
        sendLabelInfoToVTune(startStreamPtr, endStreamPtr, "common_invokeMethodRange_noJmp");
    }
#endif
    return 0;
}

int common_invokeMethodRange(ArgsDoneType form, const DecodedInstruction &decodedInst) {
    common_invokeMethodRange_noJmp(decodedInst);
    common_invokeMethod_Jmp(form);
    return 0;
}
#undef P_GPR_1
#undef P_GPR_2
#undef P_GPR_3
#undef P_SCRATCH_1
#undef P_SCRATCH_2
#undef P_SCRATCH_3
#undef P_SCRATCH_4
#undef P_SCRATCH_5
#undef P_SCRATCH_6
#undef P_SCRATCH_7
#undef P_SCRATCH_8
#undef P_SCRATCH_9
#undef P_SCRATCH_10

#define P_GPR_1 PhysicalReg_EBX
#define P_GPR_3 PhysicalReg_ESI
#define P_SCRATCH_1 PhysicalReg_EAX
#define P_SCRATCH_2 PhysicalReg_EDX
#define P_SCRATCH_3 PhysicalReg_EAX
#define P_SCRATCH_4 PhysicalReg_EDX
#define P_SCRATCH_5 PhysicalReg_EAX
#define P_SCRATCH_6 PhysicalReg_EDX

//! spill a register to native stack

//! decrease %esp by 4, then store a register at 0(%esp)
int spill_reg(int reg, bool isPhysical) {
    load_effective_addr(-4, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, reg, isPhysical, 0, PhysicalReg_ESP, true);
    return 0;
}
//! get a register from native stack

//! load a register from 0(%esp), then increase %esp by 4
int unspill_reg(int reg, bool isPhysical) {
    move_mem_to_reg(OpndSize_32, 0, PhysicalReg_ESP, true, reg, isPhysical);
    load_effective_addr(4, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    return 0;
}

void generate_invokeNative(bool generateForNcg); //forward declaration
void generate_stackOverflow(); //forward declaration

//! common code to invoke a method after all arguments are handled

//!
//takes one argument to generate code
//  for invokeNativeSingle (form == ArgsDone_Native)
//   or invokeNonNativeSingle (form == ArgsDone_Normal) when WITH_JIT is true
//   to dynamically determine which one to choose (form == ArgsDone_Full)
/* common_invokeArgsDone is called at NCG time and
     at execution time during relocation
   generate invokeArgsDone for NCG if isJitFull is false && form == Full */
int common_invokeArgsDone(ArgsDoneType form, bool isJitFull) {
#if defined VTUNE_DALVIK
    int startStreamPtr = (int)stream;
#endif

    bool generateForNcg = false;
    if(form == ArgsDone_Full) {
        if(isJitFull)
            insertLabel(".invokeArgsDone_jit", false);
        else {
            insertLabel(".invokeArgsDone", false);
            generateForNcg = true;
        }
    }
    else if(form == ArgsDone_Normal)
        insertLabel(".invokeArgsDone_normal", false);
    else if(form == ArgsDone_Native)
        insertLabel(".invokeArgsDone_native", false);
    //%ecx: methodToCall
    movez_mem_to_reg(OpndSize_16, offMethod_registersSize, PhysicalReg_ECX, true, P_SCRATCH_1, true); //regSize
    scratchRegs[0] = PhysicalReg_EBX; scratchRegs[1] = PhysicalReg_ESI;
    scratchRegs[2] = PhysicalReg_EDX; scratchRegs[3] = PhysicalReg_Null;
    savearea_from_fp(P_GPR_3, true);
    alu_binary_imm_reg(OpndSize_32, shl_opc, 2, P_SCRATCH_1, true);
    alu_binary_reg_reg(OpndSize_32, sub_opc, P_SCRATCH_1, true, P_GPR_3, true);
    //update newSaveArea->savedPc, here P_GPR_3 is new FP
    move_reg_to_mem(OpndSize_32, PhysicalReg_EDX, true, offStackSaveArea_savedPc-sizeofStackSaveArea, P_GPR_3, true);
    movez_mem_to_reg(OpndSize_16, offMethod_outsSize, PhysicalReg_ECX, true, P_SCRATCH_2, true); //outsSize
    move_reg_to_reg(OpndSize_32, P_GPR_3, true, P_GPR_1, true); //new FP
    alu_binary_imm_reg(OpndSize_32, sub_opc, sizeofStackSaveArea, P_GPR_3, true);

    alu_binary_imm_reg(OpndSize_32, shl_opc, 2, P_SCRATCH_2, true);
    alu_binary_reg_reg(OpndSize_32, sub_opc, P_SCRATCH_2, true, P_GPR_3, true);
    get_self_pointer(P_SCRATCH_3, true);
    move_reg_to_mem(OpndSize_32, PhysicalReg_FP, true, offStackSaveArea_prevFrame-sizeofStackSaveArea, P_GPR_1, true); //set stack->prevFrame
    compare_mem_reg(OpndSize_32, offsetof(Thread, interpStackEnd), P_SCRATCH_3, true, P_GPR_3, true);
    conditional_jump(Condition_L, ".stackOverflow", true);

    if(form == ArgsDone_Full) {
        test_imm_mem(OpndSize_32, ACC_NATIVE, offMethod_accessFlags, PhysicalReg_ECX, true);
    }
    move_reg_to_mem(OpndSize_32, PhysicalReg_ECX, true, offStackSaveArea_method-sizeofStackSaveArea, P_GPR_1, true); //set stack->method

    if(form == ArgsDone_Native || form == ArgsDone_Full) {
        /* to correctly handle code cache reset:
           update returnAddr and check returnAddr after done with the native method
           if returnAddr is set to NULL during code cache reset,
           the execution will correctly continue with interpreter */
        //get returnAddr from 4(%esp) and update stack
        move_mem_to_reg(OpndSize_32, 4, PhysicalReg_ESP, true,
                        PhysicalReg_EDX, true);
        move_reg_to_mem(OpndSize_32, PhysicalReg_EDX, true,
                        offStackSaveArea_returnAddr-sizeofStackSaveArea, P_GPR_1, true);
    }
    if(form == ArgsDone_Native) {
        generate_invokeNative(generateForNcg);
        return 0;
    }
    if(form == ArgsDone_Full) {
        conditional_jump(Condition_NE, ".invokeNative", true);
    }
#ifdef INC_NCG_O0
    test_imm_mem(OpndSize_32, ACC_NCG, offMethod_accessFlags, PhysicalReg_ECX, true);
#endif
    move_mem_to_reg(OpndSize_32, offMethod_clazz, PhysicalReg_ECX, true, P_SCRATCH_4, true); //get method->claz
    move_mem_to_reg(OpndSize_32, offClassObject_pDvmDex, P_SCRATCH_4, true, P_SCRATCH_4, true); //get method->clazz->pDvmDex
    move_reg_to_reg(OpndSize_32, P_GPR_1, true, PhysicalReg_FP, true); //update rFP
    get_self_pointer(P_GPR_1, true);
    move_reg_to_mem(OpndSize_32, PhysicalReg_ECX, true, offsetof(Thread, interpSave.method), P_GPR_1, true); //glue->method
    move_reg_to_mem(OpndSize_32, P_SCRATCH_4, true, offsetof(Thread, interpSave.methodClassDex), P_GPR_1, true); //set_glue_dvmdex
    move_reg_to_mem(OpndSize_32, PhysicalReg_FP, true, offThread_curFrame, P_GPR_1, true); //set glue->self->frame
    if(!generateForNcg) {
        /* returnAddr updated already for Full */
        //get returnAddr from 4(%esp) and update stack
        if(form == ArgsDone_Normal)
            move_mem_to_reg(OpndSize_32, 4, PhysicalReg_ESP, true,
                            PhysicalReg_EDX, true);
        //for JIT: starting bytecode in %ebx to invoke JitToInterp
        move_mem_to_reg(OpndSize_32, offMethod_insns, PhysicalReg_ECX, true, PhysicalReg_EBX, true);
        if(form == ArgsDone_Normal)
            move_reg_to_mem(OpndSize_32, PhysicalReg_EDX, true,
                            offStackSaveArea_returnAddr-sizeofStackSaveArea, PhysicalReg_FP, true);
    }

#ifdef INC_NCG_O0
    move_mem_to_reg(OpndSize_32, offMethod_insns, PhysicalReg_ECX, true, PhysicalReg_EAX, true);
    conditional_jump(Condition_E, ".invokeInterp", true);
#ifdef DEBUG_CALL_STACK
    scratchRegs[0] = PhysicalReg_EDX;
    invokeNcg(false); //use %eax
#else
    unconditional_jump_reg(PhysicalReg_EAX, true);
#endif
#endif

    insertLabel(".invokeInterp", true);
    if(!generateForNcg) {
#ifdef WITH_JIT
    bool callNoChain = false;
#ifdef PREDICTED_CHAINING
    if(form == ArgsDone_Full) callNoChain = true;
#endif
    if(callNoChain) {
        scratchRegs[0] = PhysicalReg_EAX;
        load_effective_addr(8, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
#if defined(WITH_JIT_TUNING)
        /* Predicted chaining failed. Fall back to interpreter and indicated
         * inline cache miss.
         * Also see gen_predicted_chain_O1
         */
        move_imm_to_reg(OpndSize_32, kInlineCacheMiss, PhysicalReg_EDX, true);
#endif
        call_dvmJitToInterpTraceSelectNoChain(); //input: rPC in %ebx
    } else {
        //jump to the stub at (%esp)
        move_mem_to_reg(OpndSize_32, 0, PhysicalReg_ESP, true,
                 PhysicalReg_EDX, true);
        load_effective_addr(8, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
        unconditional_jump_reg(PhysicalReg_EDX, true);
    }
#endif
    }
    /* invokeInterpreter called when generating code for NCG */
    if(generateForNcg) {
#ifdef INC_NCG_O0
    scratchRegs[0] = PhysicalReg_EDX;

    invokeInterpreter(false); //use %eax
#endif
    }

    if(form == ArgsDone_Full) generate_invokeNative(generateForNcg);
    generate_stackOverflow();
#if defined VTUNE_DALVIK
    if(gDvmJit.vtuneInfo != kVTuneInfoDisabled) {
        int endStreamPtr = (int)stream;
        sendLabelInfoToVTune(startStreamPtr, endStreamPtr, "common_invokeArgsDone");
    }
#endif
    return 0;
}

/* when WITH_JIT is true,
     JIT'ed code invokes native method, after invoke, execution will continue
     with the interpreter or with JIT'ed code if chained
*/
void generate_invokeNative(bool generateForNcg) {
    insertLabel(".invokeNative", true);
    //if(!generateForNcg)
    //    load_effective_addr(-8, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    load_effective_addr(-28, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, P_GPR_1, true, 0, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, P_GPR_1, true, 20, PhysicalReg_ESP, true);
    scratchRegs[0] = PhysicalReg_EDX;
    get_self_pointer(P_SCRATCH_1, true); //glue->self
    move_reg_to_mem(OpndSize_32, PhysicalReg_ECX, true, 8, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, P_SCRATCH_1, true, 12, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, P_SCRATCH_1, true, 24, PhysicalReg_ESP, true);
    move_mem_to_reg(OpndSize_32, offThread_jniLocal_nextEntry, P_SCRATCH_1, true, P_SCRATCH_2, true); //get self->local_next
    scratchRegs[1] = PhysicalReg_EAX;
    move_reg_to_mem(OpndSize_32, P_SCRATCH_2, true, offStackSaveArea_localRefTop-sizeofStackSaveArea, P_GPR_1, true); //update jniLocalRef of stack
    move_reg_to_mem(OpndSize_32, P_GPR_1, true, offThread_curFrame, P_SCRATCH_1, true); //set self->curFrame
#ifdef WITH_JIT
    move_imm_to_mem(OpndSize_32, 0, offThread_inJitCodeCache, P_SCRATCH_1, true); //clear self->inJitCodeCache
#endif
    load_effective_addr(offsetof(Thread, interpSave.retval), P_SCRATCH_1, true, P_SCRATCH_3, true); //self->retval
    move_reg_to_mem(OpndSize_32, P_SCRATCH_3, true, 4, PhysicalReg_ESP, true);
    //NOTE: native method checks the interpreted stack for arguments
    //      The immediate arguments on native stack: address of return value, new FP, self
    call_mem(40, PhysicalReg_ECX, true);//*40(%ecx)
    //we can't assume the argument stack is unmodified after the function call
    //duplicate newFP & glue->self on stack: newFP (-28 & -8) glue->self (-16 & -4)
    move_mem_to_reg(OpndSize_32, 20, PhysicalReg_ESP, true, P_GPR_3, true); //new FP
    move_mem_to_reg(OpndSize_32, 24, PhysicalReg_ESP, true, P_GPR_1, true); //glue->self
    load_effective_addr(28, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    move_mem_to_reg(OpndSize_32, offStackSaveArea_localRefTop-sizeofStackSaveArea, P_GPR_3, true, P_SCRATCH_1, true); //newSaveArea->jniLocal
    compare_imm_mem(OpndSize_32, 0, offThread_exception, P_GPR_1, true); //self->exception
    if(!generateForNcg)
        load_effective_addr(8, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    //NOTE: PhysicalReg_FP should be callee-saved register
    move_reg_to_mem(OpndSize_32, PhysicalReg_FP, true, offThread_curFrame, P_GPR_1, true); //set self->curFrame
    move_reg_to_mem(OpndSize_32, P_SCRATCH_1, true, offThread_jniLocal_nextEntry, P_GPR_1, true); //set self->jniLocal
    conditional_jump(Condition_NE, "common_exceptionThrown", false);
    if(!generateForNcg) {
#ifdef WITH_JIT
    //get returnAddr, if it is not NULL,
    //    return to JIT'ed returnAddr after executing the native method
    /* to correctly handle code cache reset:
       update returnAddr and check returnAddr after done with the native method
       if returnAddr is set to NULL during code cache reset,
       the execution will correctly continue with interpreter */
    move_mem_to_reg(OpndSize_32, offStackSaveArea_returnAddr-sizeofStackSaveArea, P_GPR_3, true, P_SCRATCH_2, true);
    //set self->inJitCodeCache to returnAddr (P_GPR_1 is in %ebx)
    move_reg_to_mem(OpndSize_32, P_SCRATCH_2, true, offThread_inJitCodeCache, P_GPR_1, true);
    move_mem_to_reg(OpndSize_32, offStackSaveArea_savedPc-sizeofStackSaveArea, P_GPR_3, true, PhysicalReg_EBX, true); //savedPc
    compare_imm_reg(OpndSize_32, 0, P_SCRATCH_2, true);
    conditional_jump(Condition_E, ".nativeToInterp", true);
    unconditional_jump_reg(P_SCRATCH_2, true);
    //if returnAddr is NULL, return to interpreter after executing the native method
    insertLabel(".nativeToInterp", true);
    //move rPC by 6 (3 bytecode units for INVOKE)
    alu_binary_imm_reg(OpndSize_32, add_opc, 6, PhysicalReg_EBX, true);
    scratchRegs[0] = PhysicalReg_EAX;
#if defined(WITH_JIT_TUNING)
    /* Return address not in code cache. Indicate that continuing with interpreter
     */
    move_imm_to_reg(OpndSize_32, kCallsiteInterpreted, PhysicalReg_EDX, true);
#endif
    call_dvmJitToInterpTraceSelectNoChain(); //rPC in %ebx
#endif
    } else {
#ifdef INC_NCG_O0
    move_mem_to_reg(OpndSize_32, offStackSaveArea_savedPc-sizeofStackSaveArea, P_GPR_3, true, P_SCRATCH_2, true); //savedPc
    alu_binary_imm_reg(OpndSize_32, add_opc, NCG_OFF_FROM_SAVEDPC, P_SCRATCH_2, true);
    unconditional_jump_reg(P_SCRATCH_2, true);
#endif
    }
    return;
}
void generate_stackOverflow() {
    insertLabel(".stackOverflow", true);
    //load_effective_addr(-8, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, PhysicalReg_ECX, true, 4, PhysicalReg_ESP, true);
    get_self_pointer(P_GPR_1, true); //glue->self
    move_reg_to_mem(OpndSize_32, P_GPR_1, true, 0, PhysicalReg_ESP, true);
    call_dvmHandleStackOverflow();
    load_effective_addr(8, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    unconditional_jump("common_exceptionThrown", false);
}
#undef P_GPR_1
#undef P_GPR_2
#undef P_GPR_3
#undef P_SCRATCH_1
#undef P_SCRATCH_2
#undef P_SCRATCH_3
#undef P_SCRATCH_4
#undef P_SCRATCH_5
#undef P_SCRATCH_6

/////////////////////////////////////////////
#define P_GPR_1 PhysicalReg_EBX
#define P_GPR_2 PhysicalReg_ECX
#define P_SCRATCH_1 PhysicalReg_ESI
#define P_SCRATCH_2 PhysicalReg_EDX
#define P_SCRATCH_3 PhysicalReg_ESI
#define P_SCRATCH_4 PhysicalReg_EDX
//! lower bytecode EXECUTE_INLINE

//!
int op_execute_inline(const MIR * mir, bool isRange) {
    assert(mir->dalvikInsn.opcode == OP_EXECUTE_INLINE
            || mir->dalvikInsn.opcode == OP_EXECUTE_INLINE_RANGE);
    int num = mir->dalvikInsn.vA;
    u2 tmp = mir->dalvikInsn.vB;
    u2 vC, vD, vE, vF;
    if(!isRange) {
        // Note that vC, vD, vE, and vF might have bad values
        // depending on count. The variable "num" should be
        // checked before using any of these.
        vC = mir->dalvikInsn.arg[0];
        vD = mir->dalvikInsn.arg[1];
        vE = mir->dalvikInsn.arg[2];
        vF = mir->dalvikInsn.arg[3];
    } else {
        vC = mir->dalvikInsn.vC;
        vD = vC + 1;
        vE = vC + 2;
        vF = vC + 3;
    }
    export_pc();
#if defined(WITH_JIT)
    switch (tmp) {
        case INLINE_EMPTYINLINEMETHOD:
            return 0;  /* Nop */
        case INLINE_STRING_LENGTH:
            get_virtual_reg(vC, OpndSize_32, 1, false);
            compare_imm_reg(OpndSize_32, 0, 1, false);
            conditional_jump(Condition_NE, ".do_inlined_string_length", true);
            scratchRegs[0] = PhysicalReg_SCRATCH_1;
            rememberState(1);
            constVREndOfBB();
            beforeCall("exception"); //dump GG, GL VRs
            unconditional_jump("common_errNullObject", false);
            goToState(1);
            insertLabel(".do_inlined_string_length", true);
            move_mem_to_reg(OpndSize_32, 0x14, 1, false, 2, false);
            get_self_pointer(3, false);
            move_reg_to_mem(OpndSize_32, 2, false, offsetof(Thread, interpSave.retval), 3, false);
            return 0;
        case INLINE_STRING_IS_EMPTY:
            get_virtual_reg(vC, OpndSize_32, 1, false);
            compare_imm_reg(OpndSize_32, 0, 1, false);
            conditional_jump(Condition_NE, ".do_inlined_string_length", true);
            scratchRegs[0] = PhysicalReg_SCRATCH_1;
            rememberState(1);
            constVREndOfBB();
            beforeCall("exception"); //dump GG, GL VRs
            unconditional_jump("common_errNullObject", false);
            goToState(1);
            insertLabel(".do_inlined_string_length", true);
            compare_imm_mem(OpndSize_32, 0, 0x14, 1, false);
            conditional_jump(Condition_E, ".inlined_string_length_return_true",
                             true);
            get_self_pointer(2, false);
            move_imm_to_mem(OpndSize_32, 0, offsetof(Thread, interpSave.retval), 2, false);
            unconditional_jump(".inlined_string_length_done", true);
            insertLabel(".inlined_string_length_return_true", true);
            get_self_pointer(2, false);
            move_imm_to_mem(OpndSize_32, 1, offsetof(Thread, interpSave.retval), 2, false);
            insertLabel(".inlined_string_length_done", true);
            return 0;
        case INLINE_MATH_ABS_INT:
            get_virtual_reg(vC, OpndSize_32, 1, false);
            move_reg_to_reg(OpndSize_32, 1, false, 2, false);
            alu_binary_imm_reg(OpndSize_32, sar_opc, 0x1f, 2, false);
            alu_binary_reg_reg(OpndSize_32, xor_opc, 2, false, 1, false);
            alu_binary_reg_reg(OpndSize_32, sub_opc, 2, false, 1, false);
            get_self_pointer(3, false);
            move_reg_to_mem(OpndSize_32, 1, false, offsetof(Thread, interpSave.retval), 3, false);
            return 0;
        case INLINE_MATH_ABS_LONG:
            get_virtual_reg(vD, OpndSize_32, 1, false);
            move_reg_to_reg(OpndSize_32, 1, false, 2, false);
            alu_binary_imm_reg(OpndSize_32, sar_opc, 0x1f, 1, false);
            move_reg_to_reg(OpndSize_32, 1, false, 3, false);
            move_reg_to_reg(OpndSize_32, 1, false, 4, false);
            get_virtual_reg(vC, OpndSize_32, 5, false);
            alu_binary_reg_reg(OpndSize_32, xor_opc, 5, false, 1, false);
            get_self_pointer(6, false);
            move_reg_to_mem(OpndSize_32, 1, false, offsetof(Thread, interpSave.retval), 6, false);
            alu_binary_reg_reg(OpndSize_32, xor_opc, 2, false, 3, false);
            move_reg_to_mem(OpndSize_32, 3, false, 4 + offsetof(Thread, interpSave.retval), 6, false);
            alu_binary_reg_mem(OpndSize_32, sub_opc, 4, false, offsetof(Thread, interpSave.retval), 6, false);
            alu_binary_reg_mem(OpndSize_32, sbb_opc, 4, false, 4 + offsetof(Thread, interpSave.retval), 6, false);
            return 0;
        case INLINE_MATH_MAX_INT:
            get_virtual_reg(vC, OpndSize_32, 1, false);
            get_virtual_reg(vD, OpndSize_32, 2, false);
            compare_reg_reg(1, false, 2, false);
            conditional_move_reg_to_reg(OpndSize_32, Condition_GE, 2,
                                        false/*src*/, 1, false/*dst*/);
            get_self_pointer(3, false);
            move_reg_to_mem(OpndSize_32, 1, false, offsetof(Thread, interpSave.retval), 3, false);
            return 0;
        case INLINE_MATH_MIN_INT:
            get_virtual_reg(vC, OpndSize_32, 1, false);
            get_virtual_reg(vD, OpndSize_32, 2, false);
            compare_reg_reg(1, false, 2, false);
            conditional_move_reg_to_reg(OpndSize_32, Condition_LE, 2,
                                        false/*src*/, 1, false/*dst*/);
            get_self_pointer(3, false);
            move_reg_to_mem(OpndSize_32, 1, false, offsetof(Thread, interpSave.retval), 3, false);
            return 0;
        case INLINE_MATH_ABS_FLOAT:
            get_virtual_reg(vC, OpndSize_32, 1, false);
            alu_binary_imm_reg(OpndSize_32, and_opc, 0x7fffffff, 1, false);
            get_self_pointer(2, false);
            move_reg_to_mem(OpndSize_32, 1, false, offsetof(Thread, interpSave.retval), 2, false);
            return 0;
        case INLINE_MATH_ABS_DOUBLE:
            get_virtual_reg(vC, OpndSize_32, 1, false);
            get_virtual_reg(vD, OpndSize_32, 2, false);
            alu_binary_imm_reg(OpndSize_32, and_opc, 0x7fffffff, 2, false);
            get_self_pointer(3, false);
            move_reg_to_mem(OpndSize_32, 1, false, offsetof(Thread, interpSave.retval), 3, false);
            move_reg_to_mem(OpndSize_32, 2, false, 4 + offsetof(Thread, interpSave.retval), 3, false);
            return 0;
        case INLINE_STRING_CHARAT:
            get_virtual_reg(vC, OpndSize_32, 1, false);
            compare_imm_reg(OpndSize_32, 0, 1, false);
            conditional_jump(Condition_NE, ".inlined_string_CharAt_arg_validate_1", true);
            rememberState(1);
            constVREndOfBB();
            beforeCall("exception");
            unconditional_jump("common_errNullObject", false);
            goToState(1);
            insertLabel(".inlined_string_CharAt_arg_validate_1", true);
            get_virtual_reg(vD, OpndSize_32, 2, false);
            compare_mem_reg(OpndSize_32, 0x14, 1, false, 2, false);
            conditional_jump(Condition_L, ".inlined_string_CharAt_arg_validate_2", true);
            rememberState(2);
            constVREndOfBB();
            beforeCall("exception");
            unconditional_jump("common_errStringIndexOutOfBounds", false);
            goToState(2);
            insertLabel(".inlined_string_CharAt_arg_validate_2", true);
            compare_imm_reg(OpndSize_32, 0, 2, false);
            conditional_jump(Condition_NS, ".do_inlined_string_CharAt", true);
            rememberState(3);
            constVREndOfBB();
            beforeCall("exception");
            unconditional_jump("common_errStringIndexOutOfBounds", false);
            goToState(3);
            insertLabel(".do_inlined_string_CharAt", true);
            alu_binary_mem_reg(OpndSize_32, add_opc, 0x10, 1, false, 2, false);
            move_mem_to_reg(OpndSize_32, 0x8, 1, false, 1, false);
            movez_mem_disp_scale_to_reg(OpndSize_16, 1, false, offsetof(ArrayObject, contents)/*disp*/, 2, false, 2, 2, false);
            get_self_pointer(3, false);
            move_reg_to_mem(OpndSize_32, 2, false, offsetof(Thread, interpSave.retval), 3, false);
            return 0;
        case INLINE_STRING_FASTINDEXOF_II:
#if defined(USE_GLOBAL_STRING_DEFS)
            break;
#else
            get_virtual_reg(vC, OpndSize_32, 1, false);
            compare_imm_reg(OpndSize_32, 0, 1, false);
            get_virtual_reg(vD, OpndSize_32, 2, false);
            get_virtual_reg(vE, OpndSize_32, 3, false);
            conditional_jump(Condition_NE, ".do_inlined_string_fastIndexof",
                             true);
            scratchRegs[0] = PhysicalReg_SCRATCH_1;
            rememberState(1);
            constVREndOfBB();
            beforeCall("exception"); //dump GG, GL VRs
            unconditional_jump("common_errNullObject", false);
            goToState(1);
            insertLabel(".do_inlined_string_fastIndexof", true);
            move_mem_to_reg(OpndSize_32, 0x14, 1, false, 4, false);
            move_mem_to_reg(OpndSize_32, 0x8, 1, false, 5, false);
            move_mem_to_reg(OpndSize_32, 0x10, 1, false, 6, false);
            alu_binary_reg_reg(OpndSize_32, xor_opc, 1, false, 1, false);
            compare_imm_reg(OpndSize_32, 0, 3, false);
            conditional_move_reg_to_reg(OpndSize_32, Condition_NS, 3, false, 1,
                                        false);
            compare_reg_reg(4, false, 1, false);
            conditional_jump(Condition_GE,
                             ".do_inlined_string_fastIndexof_exitfalse", true);
            dump_mem_scale_reg(Mnemonic_LEA, OpndSize_32, 5, false, offsetof(ArrayObject, contents)/*disp*/,
                               6, false, 2, 5, false, LowOpndRegType_gp);
            movez_mem_disp_scale_to_reg(OpndSize_16, 5, false, 0, 1, false, 2,
                                        3, false);
            compare_reg_reg(3, false, 2, false);
            conditional_jump(Condition_E, ".do_inlined_string_fastIndexof_exit",
                             true);
            load_effective_addr(0x1, 1, false, 3, false);
            load_effective_addr_scale(5, false, 3, false, 2, 5, false);
            unconditional_jump(".do_inlined_string_fastIndexof_iter", true);
            insertLabel(".do_inlined_string_fastIndexof_ch_cmp", true);
            if(gDvm.executionMode == kExecutionModeNcgO1) {
                rememberState(1);
            }
            movez_mem_to_reg(OpndSize_16, 0, 5, false, 6, false);
            load_effective_addr(0x2, 5, false, 5, false);
            compare_reg_reg(6, false, 2, false);
            conditional_jump(Condition_E, ".do_inlined_string_fastIndexof_exit",
                             true);
            load_effective_addr(0x1, 3, false, 3, false);
            insertLabel(".do_inlined_string_fastIndexof_iter", true);
            compare_reg_reg(4, false, 3, false);
            move_reg_to_reg(OpndSize_32, 3, false, 1, false);
            if(gDvm.executionMode == kExecutionModeNcgO1) {
                transferToState(1);
            }
            conditional_jump(Condition_NE,
                             ".do_inlined_string_fastIndexof_ch_cmp", true);
            insertLabel(".do_inlined_string_fastIndexof_exitfalse", true);
            move_imm_to_reg(OpndSize_32, 0xffffffff, 1,  false);
            insertLabel(".do_inlined_string_fastIndexof_exit", true);
            get_self_pointer(7, false);
            move_reg_to_mem(OpndSize_32, 1, false, offsetof(Thread, interpSave.retval), 7, false);
            return 0;
#endif
        case INLINE_FLOAT_TO_RAW_INT_BITS:
            get_virtual_reg(vC, OpndSize_32, 1, false);
            get_self_pointer(2, false);
            move_reg_to_mem(OpndSize_32, 1, false, offsetof(Thread, interpSave.retval), 2, false);
            return 0;
        case INLINE_INT_BITS_TO_FLOAT:
            get_virtual_reg(vC, OpndSize_32, 1, false);
            get_self_pointer(2, false);
            move_reg_to_mem(OpndSize_32, 1, false, offsetof(Thread, interpSave.retval), 2, false);
            return 0;
        case INLINE_DOUBLE_TO_RAW_LONG_BITS:
            get_virtual_reg(vC, OpndSize_32, 1, false);
            get_self_pointer(3, false);
            move_reg_to_mem(OpndSize_32, 1, false, offsetof(Thread, interpSave.retval), 3, false);
            get_virtual_reg(vD, OpndSize_32, 2, false);
            move_reg_to_mem(OpndSize_32, 2, false, 4 + offsetof(Thread, interpSave.retval), 3, false);
            return 0;
        case INLINE_LONG_BITS_TO_DOUBLE:
            get_virtual_reg(vC, OpndSize_32, 1, false);
            get_virtual_reg(vD, OpndSize_32, 2, false);
            get_self_pointer(3, false);
            move_reg_to_mem(OpndSize_32, 2, false, 4 + offsetof(Thread, interpSave.retval), 3, false);
            move_reg_to_mem(OpndSize_32, 1, false, offsetof(Thread, interpSave.retval), 3, false);
            return 0;
        default:
                break;
    }
#endif
    get_self_pointer(PhysicalReg_SCRATCH_1, false);
    load_effective_addr(offsetof(Thread, interpSave.retval), PhysicalReg_SCRATCH_1, false, 1, false);
    load_effective_addr(-24, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, 1, false, 16, PhysicalReg_ESP, true);
    if(num >= 1) {
        get_virtual_reg(vC, OpndSize_32, 2, false);
        move_reg_to_mem(OpndSize_32, 2, false, 0, PhysicalReg_ESP, true);
    }
    if(num >= 2) {
        get_virtual_reg(vD, OpndSize_32, 3, false);
        move_reg_to_mem(OpndSize_32, 3, false, 4, PhysicalReg_ESP, true);
    }
    if(num >= 3) {
        get_virtual_reg(vE, OpndSize_32, 4, false);
        move_reg_to_mem(OpndSize_32, 4, false, 8, PhysicalReg_ESP, true);
    }
    if(num >= 4) {
        get_virtual_reg(vF, OpndSize_32, 5, false);
        move_reg_to_mem(OpndSize_32, 5, false, 12, PhysicalReg_ESP, true);
    }
    beforeCall("execute_inline");
    load_imm_global_data_API("gDvmInlineOpsTable", OpndSize_32, 6, false);
    call_mem(16*tmp, 6, false);//
    afterCall("execute_inline");
    compare_imm_reg(OpndSize_32, 0, PhysicalReg_EAX, true);

    load_effective_addr(24, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
#if defined(WITH_JIT)
    conditional_jump(Condition_NE, ".execute_inline_done", true);
    //jump to dvmJitToExceptionThrown
    scratchRegs[0] = PhysicalReg_SCRATCH_1;
    jumpToExceptionThrown(1/*exception number*/);
#else
    conditional_jump_global_API(Condition_E, "common_exceptionThrown", false);
#endif
    insertLabel(".execute_inline_done", true);
    return 0;
}
#undef P_GPR_1
#undef P_GPR_2
#undef P_SCRATCH_1
#undef P_SCRATCH_2
#undef P_SCRATCH_3
#undef P_SCRATCH_4

#define P_GPR_1 PhysicalReg_EBX
#define P_SCRATCH_1 PhysicalReg_ESI
#define P_SCRATCH_2 PhysicalReg_EDX
#define PP_GPR_1 PhysicalReg_EBX
#define PP_GPR_2 PhysicalReg_ESI
#define PP_GPR_3 PhysicalReg_EAX
#define PP_GPR_4 PhysicalReg_EDX
//! common code for INVOKE_VIRTUAL_QUICK

//! It uses helper function if the switch is on
int common_invoke_virtual_quick(bool hasRange, u2 vD, u2 IMMC, const MIR *mir) {

    const DecodedInstruction &decodedInst = mir->dalvikInsn;

#ifdef WITH_JIT_INLINING_PHASE2
    /*
     * If the invoke has non-null misPredBranchOver, we need to generate
     * the non-inlined version of the invoke here to handle the
     * mispredicted case.
     */
    if (mir->meta.callsiteInfo->misPredBranchOver) {
        genLandingPadForMispredictedCallee (mir);
    }
#endif
    export_pc();
    constVREndOfBB();
    beforeCall("exception"); //dump GG, GL VRs
#ifdef INC_NCG_O0
    if(gDvm.helper_switch[3]) {
        common_invoke_virtual_quick_helper(hasRange, vD, IMMC);
    }
    else
#endif
    {
        /////////////////////////////////////////////////
        get_virtual_reg(vD, OpndSize_32, 1, false);
        if ( (mir->OptimizationFlags & MIR_IGNORE_NULL_CHECK) == 0)
        {
            simpleNullCheck(1, false, vD);
        }
#ifndef PREDICTED_CHAINING
        move_mem_to_reg(OpndSize_32, 0, 1, false, 2, false);
        move_mem_to_reg(OpndSize_32, offClassObject_vtable, 2, false, 3, false);
        move_mem_to_reg(OpndSize_32, IMMC, 3, false, PhysicalReg_ECX, true);

        if(hasRange) {
            common_invokeMethodRange(ArgsDone_Full);
        }
        else {
            common_invokeMethodNoRange(ArgsDone_Full);
        }
#else
        gen_predicted_chain(hasRange, -1, IMMC, false, 1/*tmp1*/, decodedInst);
#endif
        ////////////////////////
    }
    return 0;
}
#undef P_GPR_1
#undef P_SCRATCH_1
#undef P_SCRATCH_2
#undef PP_GPR_1
#undef PP_GPR_2
#undef PP_GPR_3
#undef PP_GPR_4
//! lower bytecode INVOKE_VIRTUAL_QUICK by calling common_invoke_virtual_quick

//!
int op_invoke_virtual_quick(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_INVOKE_VIRTUAL_QUICK);
#ifdef WITH_JIT_INLINING
    /* An invoke with the MIR_INLINED is effectively a no-op */
    if (mir->OptimizationFlags & MIR_INLINED)
        return 0;
#endif
    u2 vD = mir->dalvikInsn.vC;
    u2 IMMC = 4 * mir->dalvikInsn.vB;
    int retval = common_invoke_virtual_quick(false, vD, IMMC, mir);
#if defined(ENABLE_TRACING) && !defined(TRACING_OPTION2)
    insertMapWorklist(offsetPC+3, stream - streamMethodStart, 1); //check when helper switch is on
#endif
    return retval;
}
//! lower bytecode INVOKE_VIRTUAL_QUICK_RANGE by calling common_invoke_virtual_quick

//!
int op_invoke_virtual_quick_range(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_INVOKE_VIRTUAL_QUICK_RANGE);
#ifdef WITH_JIT_INLINING
    /* An invoke with the MIR_INLINED is effectively a no-op */
    if (mir->OptimizationFlags & MIR_INLINED)
        return 0;
#endif
    u2 vD = mir->dalvikInsn.vC;
    u2 IMMC = 4 * mir->dalvikInsn.vB;
    int retval = common_invoke_virtual_quick(true, vD, IMMC, mir);
#if defined(ENABLE_TRACING) && !defined(TRACING_OPTION2)
    insertMapWorklist(offsetPC+3, stream - streamMethodStart, 1); //check when helper switch is on
#endif
    return retval;
}
#define P_GPR_1 PhysicalReg_EBX
#define P_GPR_2 PhysicalReg_ESI
#define P_SCRATCH_1 PhysicalReg_EDX
//! common code to lower INVOKE_SUPER_QUICK

//!
int common_invoke_super_quick(bool hasRange, u2 vD, u2 IMMC,
        const DecodedInstruction &decodedInst) {
    export_pc();
    constVREndOfBB();
    beforeCall("exception"); //dump GG, GL VRs
    compare_imm_VR(OpndSize_32, 0, vD);

    conditional_jump_global_API(Condition_E, "common_errNullObject", false);
#if !defined(WITH_JIT)
    scratchRegs[0] = PhysicalReg_SCRATCH_1; scratchRegs[1] = PhysicalReg_SCRATCH_2;
    scratchRegs[2] = PhysicalReg_Null; scratchRegs[3] = PhysicalReg_Null;
    get_glue_method_class(1, false);
    move_mem_to_reg(OpndSize_32, offClassObject_super, 1, false, 4, false);
    move_mem_to_reg(OpndSize_32, offClassObject_vtable, 4, false, 5, false);
    move_mem_to_reg(OpndSize_32, IMMC, 5, false, PhysicalReg_ECX, true);
    const Method *calleeMethod = NULL;
#else
    /* for trace-based JIT, callee is already resolved */
    int mIndex = IMMC/4;
    const Method *calleeMethod = currentMethod->clazz->super->vtable[mIndex];
    move_imm_to_reg(OpndSize_32, (int) calleeMethod, PhysicalReg_ECX, true);
#endif
    if(hasRange) {
        common_invokeMethodRange(convertCalleeToType(calleeMethod),
                decodedInst);
    }
    else {
        common_invokeMethodNoRange(convertCalleeToType(calleeMethod),
                decodedInst);
    }
    return 0;
}
#undef P_GPR_1
#undef P_GPR_2
#undef P_SCRATCH_1
//! lower bytecode INVOKE_SUPER_QUICK by calling common_invoke_super_quick

//!
int op_invoke_super_quick(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_INVOKE_SUPER_QUICK);
#ifdef WITH_JIT_INLINING
    /* An invoke with the MIR_INLINED is effectively a no-op */
    if (mir->OptimizationFlags & MIR_INLINED)
        return 0;
#endif
    u2 vD = mir->dalvikInsn.vC;
    u2 IMMC = 4 * mir->dalvikInsn.vB;
    int retval = common_invoke_super_quick(false, vD, IMMC, mir->dalvikInsn);
#if defined(ENABLE_TRACING) && !defined(TRACING_OPTION2)
    insertMapWorklist(offsetPC+3, stream - streamMethodStart, 1); //check when helper switch is on
#endif
    return retval;
}
//! lower bytecode INVOKE_SUPER_QUICK_RANGE by calling common_invoke_super_quick

//!
int op_invoke_super_quick_range(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_INVOKE_SUPER_QUICK_RANGE);
#ifdef WITH_JIT_INLINING
    /* An invoke with the MIR_INLINED is effectively a no-op */
    if (mir->OptimizationFlags & MIR_INLINED)
        return 0;
#endif
    u2 vD = mir->dalvikInsn.vC;
    u2 IMMC = 4 * mir->dalvikInsn.vB;
    int retval = common_invoke_super_quick(true, vD, IMMC, mir->dalvikInsn);
#if defined(ENABLE_TRACING) && !defined(TRACING_OPTION2)
    insertMapWorklist(offsetPC+3, stream - streamMethodStart, 1); //check when helper switch is on
#endif
    return retval;
}
/////// code to predict the callee method for invoke_virtual & invoke_interface
#if defined(WITH_JIT)
#define offChainingCell_clazz 8
#define offChainingCell_method 12
#define offChainingCell_counter 16
#define P_GPR_1 PhysicalReg_EBX
#define P_GPR_2 PhysicalReg_EAX
#define P_GPR_3 PhysicalReg_ESI
#define P_SCRATCH_2 PhysicalReg_EDX
/* TODO gingerbread: implemented for O1, but not for O0:
   valid input to JitToPatch & use icRechainCount */
/* update predicted method for invoke interface */
// 2 inputs: ChainingCell in P_GPR_1, current class object in P_GPR_3
void predicted_chain_interface_O0(u2 tmp) {
    LOGI("TODO chain_interface_O0");

    /* set up arguments to dvmFindInterfaceMethodInCache */
    load_effective_addr(-16, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    move_imm_to_mem(OpndSize_32, tmp, 4, PhysicalReg_ESP, true);
    move_imm_to_mem(OpndSize_32, (int) currentMethod->clazz->pDvmDex, 12, PhysicalReg_ESP, true);
    move_imm_to_mem(OpndSize_32, (int) currentMethod, 8, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, P_GPR_3, true, 0, PhysicalReg_ESP, true);
    scratchRegs[0] = PhysicalReg_EDX;
    call_dvmFindInterfaceMethodInCache();
    load_effective_addr(16, PhysicalReg_ESP, true, PhysicalReg_ESP, true);

    /* if dvmFindInterfaceMethodInCache returns NULL, throw exception
       otherwise, jump to .find_interface_done */
    compare_imm_reg(OpndSize_32, 0, PhysicalReg_EAX, true);
    conditional_jump(Condition_NE, ".find_interface_done", true);
    scratchRegs[0] = PhysicalReg_EAX;
    jumpToExceptionThrown(1/*exception number*/);

    /* the interface method is found */
    insertLabel(".find_interface_done", true);
    /* reduce counter in chaining cell by 1 */
    move_mem_to_reg(OpndSize_32, offChainingCell_counter, P_GPR_1, true, P_SCRATCH_2, true); //counter
    alu_binary_imm_reg(OpndSize_32, sub_opc, 0x1, P_SCRATCH_2, true);
    move_reg_to_mem(OpndSize_32, P_SCRATCH_2, true, offChainingCell_counter, P_GPR_1, true);

    /* if counter is still greater than zero, skip prediction
       if it is zero, update predicted method */
    compare_imm_reg(OpndSize_32, 0, P_SCRATCH_2, true);
    conditional_jump(Condition_G, ".skipPrediction", true);

    /* call dvmJitToPatchPredictedChain to update predicted method */
    //%ecx has callee method for virtual, %eax has callee for interface
    /* set up arguments for dvmJitToPatchPredictedChain */
    load_effective_addr(-16, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, PhysicalReg_EAX, true, 0, PhysicalReg_ESP, true);
    if(!gDvmJit.scheduling)
        insertChainingWorklist(traceCurrentBB->taken->id, stream);
    move_chain_to_mem(OpndSize_32, traceCurrentBB->taken->id, 8, PhysicalReg_ESP, true); //predictedChainCell
    move_reg_to_mem(OpndSize_32, P_GPR_3, true, 12, PhysicalReg_ESP, true);
    scratchRegs[0] = PhysicalReg_EAX;
    call_dvmJitToPatchPredictedChain(); //inputs: method, unused, predictedChainCell, clazz
    load_effective_addr(16, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    insertLabel(".skipPrediction", true);
    move_reg_to_reg(OpndSize_32, PhysicalReg_EAX, true, PhysicalReg_ECX, true);
}

// 2 inputs: ChainingCell in temp 41, current class object in temp 40
void predicted_chain_interface_O1(u2 tmp) {

    /* set up arguments to dvmFindInterfaceMethodInCache */
    load_effective_addr(-16, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    move_imm_to_mem(OpndSize_32, tmp, 4, PhysicalReg_ESP, true);
    move_imm_to_mem(OpndSize_32, (int) currentMethod->clazz->pDvmDex, 12, PhysicalReg_ESP, true);
    move_imm_to_mem(OpndSize_32, (int) currentMethod, 8, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, 40, false, 0, PhysicalReg_ESP, true);
    scratchRegs[0] = PhysicalReg_SCRATCH_10;
    call_dvmFindInterfaceMethodInCache();
    load_effective_addr(16, PhysicalReg_ESP, true, PhysicalReg_ESP, true);

    /* if dvmFindInterfaceMethodInCache returns NULL, throw exception
       otherwise, jump to .find_interface_done */
    compare_imm_reg(OpndSize_32, 0, PhysicalReg_EAX, true);
    conditional_jump(Condition_NE, ".find_interface_done", true);
    rememberState(3);
    scratchRegs[0] = PhysicalReg_SCRATCH_9;
    jumpToExceptionThrown(1/*exception number*/);

    goToState(3);
    /* the interface method is found */
    insertLabel(".find_interface_done", true);
#if 1 //
    /* for gingerbread, counter is stored in glue structure
       if clazz is not initialized, set icRechainCount to 0, otherwise, reduce it by 1 */
    /* for gingerbread: t43 = 0 t44 = t33 t33-- cmov_ne t43 = t33 cmov_ne t44 = t33 */
    move_mem_to_reg(OpndSize_32, offChainingCell_clazz, 41, false, 45, false);
    move_imm_to_reg(OpndSize_32, 0, 43, false);
    get_self_pointer(PhysicalReg_SCRATCH_7, isScratchPhysical);
    move_mem_to_reg(OpndSize_32, offsetof(Thread, icRechainCount), PhysicalReg_SCRATCH_7, isScratchPhysical, 33, false); //counter
    move_reg_to_reg(OpndSize_32, 33, false, 44, false);
    alu_binary_imm_reg(OpndSize_32, sub_opc, 0x1, 33, false);
    /* sub_opc will update control flags, so compare_imm_reg must happen after */
    compare_imm_reg(OpndSize_32, 0, 45, false);
    conditional_move_reg_to_reg(OpndSize_32, Condition_NZ, 33, false/*src*/, 43, false/*dst*/);
    conditional_move_reg_to_reg(OpndSize_32, Condition_NZ, 33, false/*src*/, 44, false/*dst*/);
    move_reg_to_mem(OpndSize_32, 44, false, offsetof(Thread, icRechainCount), PhysicalReg_SCRATCH_7, isScratchPhysical);
#else
    /* reduce counter in chaining cell by 1 */
    move_mem_to_reg(OpndSize_32, offChainingCell_counter, 41, false, 33, false); //counter
    alu_binary_imm_reg(OpndSize_32, sub_opc, 0x1, 33, false);
    move_reg_to_mem(OpndSize_32, 33, false, offChainingCell_counter, 41, false);
#endif

    /* if counter is still greater than zero, skip prediction
       if it is zero, update predicted method */
    compare_imm_reg(OpndSize_32, 0, 43, false);
    conditional_jump(Condition_G, ".skipPrediction", true);

    rememberState(4);
    /* call dvmJitToPatchPredictedChain to update predicted method */
    //%ecx has callee method for virtual, %eax has callee for interface
    /* set up arguments for dvmJitToPatchPredictedChain */
    load_effective_addr(-16, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, PhysicalReg_EAX, true, 0, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, PhysicalReg_SCRATCH_7, isScratchPhysical, 4, PhysicalReg_ESP, true);
   if(!gDvmJit.scheduling)
        insertChainingWorklist(traceCurrentBB->taken->id, stream);
    move_chain_to_mem(OpndSize_32, traceCurrentBB->taken->id, 8, PhysicalReg_ESP, true); //predictedChainCell
    move_reg_to_mem(OpndSize_32, 40, false, 12, PhysicalReg_ESP, true);
    scratchRegs[0] = PhysicalReg_SCRATCH_8;
    call_dvmJitToPatchPredictedChain(); //inputs: method, unused, predictedChainCell, clazz
    load_effective_addr(16, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    transferToState(4);

    insertLabel(".skipPrediction", true);
    move_reg_to_reg(OpndSize_32, PhysicalReg_EAX, true, PhysicalReg_ECX, true);
}

/* update predicted method for invoke virtual */
// 2 inputs: ChainingCell in P_GPR_1, current class object in P_GPR_3
void predicted_chain_virtual_O0(u2 IMMC) {
    LOGI("TODO chain_virtual_O0");

    /* reduce counter in chaining cell by 1 */
    move_mem_to_reg(OpndSize_32, offChainingCell_counter, P_GPR_1, true, P_GPR_2, true); //counter
    move_mem_to_reg(OpndSize_32, offClassObject_vtable, P_GPR_3, true, P_SCRATCH_2, true);
    alu_binary_imm_reg(OpndSize_32, sub_opc, 0x1, P_GPR_2, true);
    move_mem_to_reg(OpndSize_32, IMMC, P_SCRATCH_2, true, PhysicalReg_ECX, true);
    move_reg_to_mem(OpndSize_32, P_GPR_2, true, offChainingCell_counter, P_GPR_1, true);

    /* if counter is still greater than zero, skip prediction
       if it is zero, update predicted method */
    compare_imm_reg(OpndSize_32, 0, P_GPR_2, true);
    conditional_jump(Condition_G, ".skipPrediction", true);

    /* call dvmJitToPatchPredictedChain to update predicted method */
    //%ecx has callee method for virtual, %eax has callee for interface
    /* set up arguments for dvmJitToPatchPredictedChain */
    load_effective_addr(-16, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, PhysicalReg_ECX, true, 0,  PhysicalReg_ESP, true);
    if(!gDvmJit.scheduling)
        insertChainingWorklist(traceCurrentBB->taken->id, stream);
    move_chain_to_mem(OpndSize_32, traceCurrentBB->taken->id, 8, PhysicalReg_ESP, true); //predictedChainCell
    move_reg_to_mem(OpndSize_32, P_GPR_3, true, 12, PhysicalReg_ESP, true);
    scratchRegs[0] = PhysicalReg_EAX;
    call_dvmJitToPatchPredictedChain(); //inputs: method, unused, predictedChainCell, clazz
    load_effective_addr(16, PhysicalReg_ESP, true, PhysicalReg_ESP, true);

    //callee method in %ecx for invoke virtual
    move_reg_to_reg(OpndSize_32, PhysicalReg_EAX, true, PhysicalReg_ECX, true);
    insertLabel(".skipPrediction", true);
}

// 2 inputs: ChainingCell in temp 41, current class object in temp 40
// extra input: predicted clazz in temp 32
void predicted_chain_virtual_O1(u2 IMMC) {

    /* reduce counter in chaining cell by 1 */
    /* for gingerbread: t43 = 0 t44 = t33 t33-- cmov_ne t43 = t33 cmov_ne t44 = t33 */
    get_self_pointer(PhysicalReg_SCRATCH_7, isScratchPhysical);
    move_imm_to_reg(OpndSize_32, 0, 43, false);
    move_mem_to_reg(OpndSize_32, offsetof(Thread, icRechainCount), PhysicalReg_SCRATCH_7, isScratchPhysical, 33, false); //counter
    move_mem_to_reg(OpndSize_32, offClassObject_vtable, 40, false, 34, false);
    move_reg_to_reg(OpndSize_32, 33, false, 44, false);
    alu_binary_imm_reg(OpndSize_32, sub_opc, 0x1, 33, false);
    compare_imm_reg(OpndSize_32, 0, 32, false); // after sub_opc
    move_mem_to_reg(OpndSize_32, IMMC, 34, false, PhysicalReg_ECX, true);
    conditional_move_reg_to_reg(OpndSize_32, Condition_NZ, 33, false/*src*/, 43, false/*dst*/);
    conditional_move_reg_to_reg(OpndSize_32, Condition_NZ, 33, false/*src*/, 44, false/*dst*/);
    move_reg_to_mem(OpndSize_32, 44, false, offsetof(Thread, icRechainCount), PhysicalReg_SCRATCH_7, isScratchPhysical);

    /* if counter is still greater than zero, skip prediction
       if it is zero, update predicted method */
    compare_imm_reg(OpndSize_32, 0, 43, false);
    conditional_jump(Condition_G, ".skipPrediction", true);

    rememberState(2);
    /* call dvmJitToPatchPredictedChain to update predicted method */
    //%ecx has callee method for virtual, %eax has callee for interface
    /* set up arguments for dvmJitToPatchPredictedChain */
    load_effective_addr(-16, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, PhysicalReg_ECX, true, 0, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, PhysicalReg_SCRATCH_7, isScratchPhysical, 4, PhysicalReg_ESP, true);
    if(!gDvmJit.scheduling && traceCurrentBB->taken)
        insertChainingWorklist(traceCurrentBB->taken->id, stream);
    int traceTakenId = traceCurrentBB->taken ? traceCurrentBB->taken->id : 0;
    move_chain_to_mem(OpndSize_32, traceTakenId, 8, PhysicalReg_ESP, true); //predictedChainCell
    move_reg_to_mem(OpndSize_32, 40, false, 12, PhysicalReg_ESP, true);
    scratchRegs[0] = PhysicalReg_SCRATCH_10;
    call_dvmJitToPatchPredictedChain(); //inputs: method, unused, predictedChainCell, clazz
    load_effective_addr(16, PhysicalReg_ESP, true, PhysicalReg_ESP, true);

    //callee method in %ecx for invoke virtual
    move_reg_to_reg(OpndSize_32, PhysicalReg_EAX, true, PhysicalReg_ECX, true);
    transferToState(2);

    insertLabel(".skipPrediction", true);
}

static int invokeChain_inst = 0;
/* object "this" is in %ebx */
void gen_predicted_chain_O0(bool isRange, u2 tmp, int IMMC, bool isInterface,
        int inputReg, const DecodedInstruction &decodedInst) {
    LOGI("TODO predicted_chain_O0");

    /* get current class object */
    move_mem_to_reg(OpndSize_32, offObject_clazz, PhysicalReg_EBX, true,
             P_GPR_3, true);
#ifdef DEBUG_CALL_STACK3
    scratchRegs[0] = PhysicalReg_EAX;
    call_debug_dumpSwitch(); //%ebx, %eax, %edx
    move_imm_to_reg(OpndSize_32, 0xdd11, PhysicalReg_EBX, true);
    call_debug_dumpSwitch();
#endif

    /* get predicted clazz
       get predicted method
    */
    if(!gDvmJit.scheduling)
        insertChainingWorklist(traceCurrentBB->taken->id, stream);
    move_chain_to_reg(OpndSize_32, traceCurrentBB->taken->id, P_GPR_1, true); //predictedChainCell
    move_mem_to_reg(OpndSize_32, offChainingCell_clazz, P_GPR_1, true, P_SCRATCH_2, true);//predicted clazz
    move_mem_to_reg(OpndSize_32, offChainingCell_method, P_GPR_1, true, PhysicalReg_ECX, true);//predicted method

#ifdef DEBUG_CALL_STACK3
    load_effective_addr(-12, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, P_GPR_1, true, 8, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, P_SCRATCH_2, true, 4, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, P_GPR_3, true, 0, PhysicalReg_ESP, true);

    move_reg_to_reg(OpndSize_32, P_SCRATCH_2, true, PhysicalReg_EBX, true);
    call_debug_dumpSwitch();
    move_imm_to_reg(OpndSize_32, 0xdd22, PhysicalReg_EBX, true);
    scratchRegs[0] = PhysicalReg_EAX;
    call_debug_dumpSwitch(); //%ebx, %eax, %edx
    move_reg_to_reg(OpndSize_32, P_GPR_3, true, PhysicalReg_EBX, true);
    call_debug_dumpSwitch();
    move_reg_to_reg(OpndSize_32, PhysicalReg_ECX, true, PhysicalReg_EBX, true);
    call_debug_dumpSwitch();

    move_mem_to_reg(OpndSize_32, 8, PhysicalReg_ESP, true, P_GPR_1, true);
    move_mem_to_reg(OpndSize_32, 4, PhysicalReg_ESP, true, P_SCRATCH_2, true);
    move_mem_to_reg(OpndSize_32, 0, PhysicalReg_ESP, true, P_GPR_3, true);
    load_effective_addr(12, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
#endif

    /* compare current class object against predicted clazz
       if equal, prediction is still valid, jump to .invokeChain */
    //live registers: P_GPR_1, P_GPR_3, P_SCRATCH_2
    compare_reg_reg(P_GPR_3, true, P_SCRATCH_2, true);
    conditional_jump(Condition_E, ".invokeChain", true);
    invokeChain_inst++;

    //get callee method and update predicted method if necessary
    if(isInterface) {
        predicted_chain_interface_O0(tmp);
    } else {
        predicted_chain_virtual_O0(IMMC);
    }

#ifdef DEBUG_CALL_STACK3
    move_imm_to_reg(OpndSize_32, 0xeeee, PhysicalReg_EBX, true);
    scratchRegs[0] = PhysicalReg_EAX;
    call_debug_dumpSwitch(); //%ebx, %eax, %edx
    if(!gDvmJit.scheduling)
        insertChainingWorklist(traceCurrentBB->taken->id, stream);
    move_chain_to_reg(OpndSize_32, traceCurrentBB->taken->id, PhysicalReg_EBX, true);
    call_debug_dumpSwitch();
#endif

    if(isRange) {
        common_invokeMethodRange(ArgsDone_Full, decodedInst);
    }
    else {
        common_invokeMethodNoRange(ArgsDone_Full, decodedInst);
    }

    insertLabel(".invokeChain", true);
#ifdef DEBUG_CALL_STACK3
    move_imm_to_reg(OpndSize_32, 0xdddd, PhysicalReg_EBX, true);
    scratchRegs[0] = PhysicalReg_EAX;
    call_debug_dumpSwitch(); //%ebx, %eax, %edx
    if(!gDvmJit.scheduling)
        insertChainingWorklist(traceCurrentBB->taken->id, stream);
    move_chain_to_reg(OpndSize_32, traceCurrentBB->taken->id, PhysicalReg_EBX, true);
    call_debug_dumpSwitch();
    move_reg_to_reg(OpndSize_32, PhysicalReg_ECX, true, PhysicalReg_EBX, true);
    call_debug_dumpSwitch();
#endif

    if(isRange) {
        common_invokeMethodRange(ArgsDone_Normal, decodedInst);
    }
    else {
        common_invokeMethodNoRange(ArgsDone_Normal, decodedInst);
    }
}

/* object "this" is in inputReg: 5 for virtual, 1 for interface, 1 for virtual_quick */
void gen_predicted_chain_O1(bool isRange, u2 tmp, int IMMC, bool isInterface,
        int inputReg, const DecodedInstruction &decodedInst) {

    /* get current class object */
    move_mem_to_reg(OpndSize_32, offObject_clazz, inputReg, false,
             40, false);

    /* get predicted clazz
       get predicted method
    */
    if(!gDvmJit.scheduling && traceCurrentBB->taken)
        insertChainingWorklist(traceCurrentBB->taken->id, stream);
    int traceTakenId = traceCurrentBB->taken ? traceCurrentBB->taken->id : 0;
    move_chain_to_reg(OpndSize_32, traceTakenId, 41, false); //predictedChainCell
    move_mem_to_reg(OpndSize_32, offChainingCell_clazz, 41, false, 32, false);//predicted clazz
    move_mem_to_reg(OpndSize_32, offChainingCell_method, 41, false, PhysicalReg_ECX, true);//predicted method

    /* update stack with parameters first, then decide the callee */
    if(isRange) common_invokeMethodRange_noJmp(decodedInst);
    else common_invokeMethodNoRange_noJmp(decodedInst);

    /* compare current class object against predicted clazz
       if equal, prediction is still valid, jump to .invokeChain */
    compare_reg_reg(40, false, 32, false);
    conditional_jump(Condition_E, ".invokeChain", true);
    rememberState(1);
    invokeChain_inst++;

    //get callee method and update predicted method if necessary
    if(isInterface) {
        predicted_chain_interface_O1(tmp);
    } else {
        predicted_chain_virtual_O1(IMMC);
    }

    common_invokeMethod_Jmp(ArgsDone_Full); //will touch %ecx

    insertLabel(".invokeChain", true);
    goToState(1);
    common_invokeMethod_Jmp(ArgsDone_Normal);
}

void gen_predicted_chain(bool isRange, u2 tmp, int IMMC, bool isInterface,
        int inputReg, const DecodedInstruction &decodedInst) {
    return gen_predicted_chain_O1(isRange, tmp, IMMC, isInterface, inputReg,
            decodedInst);
}
#undef P_GPR_1
#undef P_GPR_2
#undef P_GPR_3
#undef P_SCRATCH_2
#endif
