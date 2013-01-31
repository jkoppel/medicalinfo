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

/*! \file LowerObject.cpp
    \brief This file lowers the following bytecodes: CHECK_CAST,
*/
#include "libdex/DexOpcodes.h"
#include "libdex/DexFile.h"
#include "Lower.h"
#include "NcgAot.h"
#include "enc_wrapper.h"

extern void markCard_filled(int tgtAddrReg, bool isTgtPhysical, int scratchReg, bool isScratchPhysical);

#define P_GPR_1 PhysicalReg_EBX
#define P_GPR_2 PhysicalReg_ECX
#define P_GPR_3 PhysicalReg_ESI
//! LOWER bytecode CHECK_CAST and INSTANCE_OF
//!   CALL class_resolve (%ebx is live across the call)
//!        dvmInstanceofNonTrivial
//!   NO register is live through function check_cast_helper
int check_cast_nohelper(u2 vA, u4 tmp, bool instance, u2 vDest) {
    get_virtual_reg(vA, OpndSize_32, 1, false); //object
    scratchRegs[2] = PhysicalReg_Null; scratchRegs[3] = PhysicalReg_Null;
    /* for trace-based JIT, it is likely that the class is already resolved */
    bool needToResolve = true;
#if defined(WITH_JIT)
    ClassObject *classPtr =
                (currentMethod->clazz->pDvmDex->pResClasses[tmp]);
    LOGV("in check_cast, class is resolved to %p", classPtr);
    if(classPtr != NULL) {
        needToResolve = false;
        LOGV("check_cast class %s", classPtr->descriptor);
    }
#endif
    if(needToResolve) {
        //get_res_classes is moved here for NCG O1 to improve performance of GLUE optimization
        scratchRegs[0] = PhysicalReg_SCRATCH_1; scratchRegs[1] = PhysicalReg_SCRATCH_2;
        get_res_classes(4, false);
    }
    compare_imm_reg(OpndSize_32, 0, 1, false);

    rememberState(1);
    //for private code cache, previously it jumped to .instance_of_okay_1
    //if object reference is null, jump to the handler for this special case
    if(instance) {
        conditional_jump(Condition_E, ".instance_of_null", true);
    }
    else {
        conditional_jump(Condition_E, ".check_cast_null", true);
    }
    //check whether the class is already resolved
    //if yes, jump to check_cast_resolved
    //if not, call class_resolve
    if(needToResolve) {
        move_mem_to_reg(OpndSize_32, tmp*4, 4, false, PhysicalReg_EAX, true);
        compare_imm_reg(OpndSize_32, 0, PhysicalReg_EAX, true);
        if(instance)
            conditional_jump(Condition_NE, ".instance_of_resolved", true);
        else
            conditional_jump(Condition_NE, ".check_cast_resolved", true);
        //try to resolve the class
        rememberState(2);
        move_imm_to_reg(OpndSize_32, tmp, PhysicalReg_EAX, true);
        export_pc(); //trying to resolve the class
        call_helper_API(".class_resolve");
        transferToState(2);
    } //needToResolve
#if defined(WITH_JIT)
    else {
        /* the class is already resolved and is constant */
        move_imm_to_reg(OpndSize_32, (int)classPtr, PhysicalReg_EAX, true);
    }
#endif
    //class is resolved, and it is in %eax
    if(!instance) {
        insertLabel(".check_cast_resolved", true);
    }
    else insertLabel(".instance_of_resolved", true);

    move_mem_to_reg(OpndSize_32, offObject_clazz, 1, false, 6, false); //object->clazz

    //%eax: resolved class
    //compare resolved class and object->clazz
    //if the same, jump to the handler for this special case
    compare_reg_reg(PhysicalReg_EAX, true, 6, false);
    rememberState(3);
    if(instance) {
        conditional_jump(Condition_E, ".instance_of_equal", true);
    } else {
        conditional_jump(Condition_E, ".check_cast_equal", true);
    }

    //prepare to call dvmInstanceofNonTrivial
    //INPUT: the resolved class & object reference
    load_effective_addr(-8, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, 6, false, 0, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, PhysicalReg_EAX, true, 4, PhysicalReg_ESP, true); //resolved class
    scratchRegs[0] = PhysicalReg_SCRATCH_3;
    nextVersionOfHardReg(PhysicalReg_EAX, 2); //next version has 2 refs
    call_dvmInstanceofNonTrivial();
    load_effective_addr(8, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    //
    if(instance) {
        //move return value to P_GPR_2
        move_reg_to_reg(OpndSize_32, PhysicalReg_EAX, true, 3, false);
        rememberState(4);
        unconditional_jump(".instance_of_okay", true);
    } else {
        //if return value of dvmInstanceofNonTrivial is zero, throw exception
        compare_imm_reg(OpndSize_32, 0,  PhysicalReg_EAX, true);
        rememberState(4);
        conditional_jump(Condition_NE, ".check_cast_okay", true);
        //two inputs for common_throw_message: object reference in eax, exception pointer in ecx
        nextVersionOfHardReg(PhysicalReg_EAX, 1); //next version has 1 ref
        move_reg_to_reg(OpndSize_32, 1, false, PhysicalReg_EAX, true);

        load_imm_global_data_API("strClassCastExceptionPtr", OpndSize_32, PhysicalReg_ECX, true);

        nextVersionOfHardReg(PhysicalReg_EDX, 2); //next version has 2 ref count
        export_pc();

        unconditional_jump_global_API("common_throw_message", false);
    }
    //handler for speical case where object reference is null
    if(instance)
        insertLabel(".instance_of_null", true);
    else insertLabel(".check_cast_null", true);
    goToState(1);
    if(instance) {
        move_imm_to_reg(OpndSize_32, 0, 3, false);
    }
    transferToState(4);
    if(instance)
        unconditional_jump(".instance_of_okay", true);
    else
        unconditional_jump(".check_cast_okay", true);

    //handler for special case where class of object is the same as the resolved class
    if(instance)
        insertLabel(".instance_of_equal", true);
    else insertLabel(".check_cast_equal", true);
    goToState(3);
    if(instance) {
        move_imm_to_reg(OpndSize_32, 1, 3, false);
    }
    transferToState(4);
    if(instance)
        insertLabel(".instance_of_okay", true);
    else insertLabel(".check_cast_okay", true);
    //all cases merge here and the value is put to virtual register
    if(instance) {
        set_virtual_reg(vDest, OpndSize_32, 3, false);
    }
    return 0;
}
//! common code to lower CHECK_CAST & INSTANCE_OF

//!
int common_check_cast_instance_of(u2 vA, u4 tmp, bool instance, u2 vDest) {
#ifdef INC_NCG_O0
    if(gDvm.helper_switch[8]) {
        // .check_cast_helper
        //   INPUT: in P_GPR_1 (virtual register for class object)
        //          in P_GPR_3 (const pool index)
        //   OUTPUT: in P_GPR_2
        //if(!instance) {
            export_pc();
        //}
        move_imm_to_reg(OpndSize_32, vA, P_GPR_1, true);
        move_imm_to_reg(OpndSize_32, tmp, P_GPR_3, true);
        spillVirtualReg(vA, LowOpndRegType_gp, true);
        if(instance)
            call_helper_API(".instance_of_helper");
        else
            call_helper_API(".check_cast_helper");

        if(instance) {
            set_virtual_reg(vDest, OpndSize_32, P_GPR_2, true);
        }
        return 0;
    }
    else
#endif
    {
        return check_cast_nohelper(vA, tmp, instance, vDest);
    }
}
#undef P_GPR_1
#undef P_GPR_2
#undef P_GPR_3

/**
 * @brief Generate native code for bytecode check-cast
 * @param mir bytecode representation
 * @return value >= 0 when handled
 */
int op_check_cast(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_CHECK_CAST);
    u2 vA = mir->dalvikInsn.vA;
    u4 tmp = mir->dalvikInsn.vB;
    common_check_cast_instance_of(vA, tmp, false, 0);
    return 0;
}

/**
 * @brief Generate native code for bytecode instance-of
 * @param mir bytecode representation
 * @return value >= 0 when handled
 */
int op_instance_of(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_INSTANCE_OF);
    u2 vA = mir->dalvikInsn.vA;
    u2 vB = mir->dalvikInsn.vB;
    u4 tmp = mir->dalvikInsn.vC;
    common_check_cast_instance_of(vB, tmp, true, vA);
    return 0;
}

#define P_GPR_1 PhysicalReg_EBX
#define P_GPR_2 PhysicalReg_ECX
//! LOWER bytecode MONITOR_ENTER without usage of helper function

//!   CALL dvmLockObject
int monitor_enter_nohelper(u2 vA, const MIR *mir) {
    scratchRegs[0] = PhysicalReg_SCRATCH_1;
    scratchRegs[2] = PhysicalReg_Null; scratchRegs[3] = PhysicalReg_Null;

    if((mir->OptimizationFlags & MIR_IGNORE_NULL_CHECK) == 0)
    {
        requestVRFreeDelay(vA,VRDELAY_NULLCHECK); // Request VR delay before transfer to temporary
    }

    //get_self_pointer is separated
    get_virtual_reg(vA, OpndSize_32, 1, false);
    //to optimize redundant null check, NCG O1 wraps up null check in a function: nullCheck
    get_self_pointer(3, false);
    //If we can't ignore the NULL check
    if((mir->OptimizationFlags & MIR_IGNORE_NULL_CHECK) == 0)
    {
        nullCheck(1, false, 1, vA); //maybe optimized away
        cancelVRFreeDelayRequest(vA,VRDELAY_NULLCHECK);
    }

    /////////////////////////////
    //prepare to call dvmLockObject, inputs: object reference and self
    // TODO: Should reset inJitCodeCache before calling dvmLockObject
    //       so that code cache can be reset if needed when locking object
    //       taking a long time. Not resetting inJitCodeCache may delay
    //       code cache reset when code cache is full, preventing traces from
    //       JIT compilation. This has performance implication.
    //       However, after resetting inJitCodeCache, the code should be
    //       wrapped in a helper instead of directly inlined in code cache.
    //       If the code after dvmLockObject call is in code cache and the code
    //       cache is reset during dvmLockObject call, execution after
    //       dvmLockObject will return to a cleared code cache region,
    //       resulting in seg fault.
    load_effective_addr(-8, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, 1, false, 4, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, 3, false, 0, PhysicalReg_ESP, true);
    scratchRegs[0] = PhysicalReg_SCRATCH_2;
    call_dvmLockObject();
    load_effective_addr(8, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    /////////////////////////////
    return 0;
}

/**
 * @brief Generate native code for bytecode monitor-enter
 * @param mir bytecode representation
 * @return value >= 0 when handled
 */
int op_monitor_enter(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_MONITOR_ENTER);
    u2 vA = mir->dalvikInsn.vA;
#ifdef INC_NCG_O0
    if(gDvm.helper_switch[11]) {
        // .monitor_enter_helper
        //   INPUT: P_GPR_1 (virtual register for object)
        //   OUTPUT: none
        //   %esi is live through function monitor_enter_helper
        export_pc(); //use %edx
        move_imm_to_reg(OpndSize_32, vA, P_GPR_1, true);
        spillVirtualReg(vA, LowOpndRegType_gp, true);
        call_helper_API(".monitor_enter_helper");
    }
    else
#endif
    {
        export_pc();
        monitor_enter_nohelper(vA, mir);
    }
    return 0;
}
#undef P_GPR_1
#undef P_GPR_2

#define P_GPR_1 PhysicalReg_EBX
#define P_GPR_2 PhysicalReg_ECX

/**
 * @brief Generate native code for bytecode monitor-exit
 * @param mir bytecode representation
 * @return value >= 0 when handled
 */
int op_monitor_exit(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_MONITOR_EXIT);
    u2 vA = mir->dalvikInsn.vA;
#ifdef INC_NCG_O0
    if(gDvm.helper_switch[11]) {
        export_pc();
        // .moniter_exit_helper
        //   INPUT: in P_GPR_1 (virtual register for object)
        //   OUTPUT: none
        //   %esi is live through function moniter_exit_helper
        move_imm_to_reg(OpndSize_32, vA, P_GPR_1, true);
        spillVirtualReg(vA, LowOpndRegType_gp, true);
        call_helper_API(".monitor_exit_helper");
    }
    else
#endif
    {
        ////////////////////
        //LOWER bytecode MONITOR_EXIT without helper function
        //   CALL dvmUnlockObject
        scratchRegs[0] = PhysicalReg_SCRATCH_1; scratchRegs[1] = PhysicalReg_SCRATCH_2;
        scratchRegs[2] = PhysicalReg_Null; scratchRegs[3] = PhysicalReg_Null;

        if((mir->OptimizationFlags & MIR_IGNORE_NULL_CHECK) == 0)
        {
            requestVRFreeDelay(vA,VRDELAY_NULLCHECK); // Request VR delay before transfer to temporary
        }

        get_virtual_reg(vA, OpndSize_32, 1, false);

        if((mir->OptimizationFlags & MIR_IGNORE_NULL_CHECK) == 0)
        {
            nullCheck(1, false, 1, vA); //maybe optimized away
            cancelVRFreeDelayRequest(vA,VRDELAY_NULLCHECK);
        }

        /////////////////////////////
        //prepare to call dvmUnlockObject, inputs: object reference and self
        push_reg_to_stack(OpndSize_32, 1, false);
        push_mem_to_stack(OpndSize_32, offEBP_self, PhysicalReg_EBP, true);
        scratchRegs[0] = PhysicalReg_SCRATCH_2;
        call_dvmUnlockObject();
        compare_imm_reg(OpndSize_32, 0, PhysicalReg_EAX, true);
        load_effective_addr(8, PhysicalReg_ESP, true, PhysicalReg_ESP, true);

#if defined(WITH_JIT)
        conditional_jump(Condition_NE, ".unlock_object_done", true);
        //jump to dvmJitToExceptionThrown
        scratchRegs[0] = PhysicalReg_SCRATCH_3;
        jumpToExceptionThrown(2/*exception number*/);
#else
        //throw exception if dvmUnlockObject returns 0
        char errName[256];
        sprintf(errName, "common_exceptionThrown");
        handlePotentialException(
                                           Condition_E, Condition_NE,
                                           2, errName);
#endif
        insertLabel(".unlock_object_done", true);
        ///////////////////////////
    }
    return 0;
}
#undef P_GPR_1
#undef P_GPR_2

#define P_GPR_1 PhysicalReg_EBX
#define P_GPR_2 PhysicalReg_ECX
#define P_GPR_3 PhysicalReg_EDX /*vA*/

/**
 * @brief Generate native code for bytecode array-length
 * @param mir bytecode representation
 * @return value >= 0 when handled
 */
int op_array_length(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_ARRAY_LENGTH);
    u2 vA = mir->dalvikInsn.vA;
    u2 vB = mir->dalvikInsn.vB;
#ifdef INC_NCG_O0
    if(gDvm.helper_switch[14]) {
        // .array_length_helper
        //   INPUT: P_GPR_1 (virtual register for array object)
        //          P_GPR_3 (virtual register for length)
        //   OUTPUT: none
        //   %eax, %esi, %ebx: live through function array_length_helper
        export_pc(); //use %edx
        move_imm_to_reg(OpndSize_32, vA, P_GPR_3, true);
        move_imm_to_reg(OpndSize_32, vB, P_GPR_1, true);
        call_helper_API(".array_length_helper");
    }
    else
#endif
    {
        ////////////////////
        //no usage of helper function
        if((mir->OptimizationFlags & MIR_IGNORE_NULL_CHECK) == 0)
        {
            requestVRFreeDelay(vB,VRDELAY_NULLCHECK); // Request VR delay before transfer to temporary
        }

        get_virtual_reg(vB, OpndSize_32, 1, false);

        if((mir->OptimizationFlags & MIR_IGNORE_NULL_CHECK) == 0)
        {
            nullCheck(1, false, 1, vB); //maybe optimized away
            cancelVRFreeDelayRequest(vB,VRDELAY_NULLCHECK);
        }

        move_mem_to_reg(OpndSize_32, offArrayObject_length, 1, false, 2, false);
        set_virtual_reg(vA, OpndSize_32, 2, false);
        ///////////////////////
    }
    return 0;
}
#undef P_GPR_1
#undef P_GPR_2
#undef P_GPR_3

#define P_GPR_1 PhysicalReg_EBX
#define P_GPR_2 PhysicalReg_ECX
#define P_GPR_3 PhysicalReg_ESI

/**
 * @brief Generate native code for bytecode new-instance
 * @param mir bytecode representation
 * @return value >= 0 when handled
 */
int op_new_instance(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_NEW_INSTANCE);
    u2 vA = mir->dalvikInsn.vA;
    u4 tmp = mir->dalvikInsn.vB;
#ifdef INC_NCG_O0
    if(gDvm.helper_switch[4]) {
        // .new_instance_helper
        //   INPUT: P_GPR_3 (const pool index)
        //   OUTPUT: %eax
        //   no register is live through function array_length_helper
        export_pc();
        move_imm_to_reg(OpndSize_32, tmp, P_GPR_3, true);
        call_helper_API(".new_instance_helper");
    }
    else
#endif
    {
        export_pc();
#if defined(WITH_JIT)
        /* for trace-based JIT, class is already resolved */
        ClassObject *classPtr =
              (currentMethod->clazz->pDvmDex->pResClasses[tmp]);
        assert(classPtr != NULL);
        assert(classPtr->status & CLASS_INITIALIZED);
        /*
         * If it is going to throw, it should not make to the trace to begin
         * with.  However, Alloc might throw, so we need to genExportPC()
        */
        assert((classPtr->accessFlags & (ACC_INTERFACE|ACC_ABSTRACT)) == 0);
#else
        //////////////////////////////////////////
        //resolve class, check whether it has been resolved
        //if yes, jump to resolved
        //if no, call class_resolve
        scratchRegs[0] = PhysicalReg_SCRATCH_1; scratchRegs[1] = PhysicalReg_SCRATCH_2;
        scratchRegs[2] = PhysicalReg_Null; scratchRegs[3] = PhysicalReg_Null;
        get_res_classes(3, false);
        move_mem_to_reg(OpndSize_32, tmp*4, 3, false, PhysicalReg_EAX, true);
        compare_imm_reg(OpndSize_32, 0, PhysicalReg_EAX, true); //resolved class
        conditional_jump(Condition_NE, ".new_instance_resolved", true);
        rememberState(1);
        move_imm_to_reg(OpndSize_32, tmp, PhysicalReg_EAX, true);
        call_helper_API(".class_resolve");
        transferToState(1);

        //here, class is resolved
        insertLabel(".new_instance_resolved", true);
        //check whether the class is initialized
        //if yes, jump to initialized
        //if no, call new_instance_needinit
        movez_mem_to_reg(OpndSize_8, offClassObject_status, PhysicalReg_EAX, true, 5, false);
        compare_imm_reg(OpndSize_32, CLASS_INITIALIZED, 5, false);
        conditional_jump(Condition_E, ".new_instance_initialized", true);
        rememberState(2);
        call_helper_API(".new_instance_needinit");
        transferToState(2);
        //here, class is already initialized
        insertLabel(".new_instance_initialized", true);
        //check whether the class is an interface or abstract, if yes, throw exception
        move_mem_to_reg(OpndSize_32, offClassObject_accessFlags, PhysicalReg_EAX, true, 6, false);
        test_imm_reg(OpndSize_32, ACC_INTERFACE|ACC_ABSTRACT, 6, false); //access flags

        //two inputs for common_throw_message: object reference in eax, exception pointer in ecx
        handlePotentialException(
                                           Condition_NE, Condition_E,
                                           2, "common_throw_message");
#endif
        //prepare to call dvmAllocObject, inputs: resolved class & flag ALLOC_DONT_TRACK
        load_effective_addr(-8, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
#if defined(WITH_JIT)
        /* 1st argument to dvmAllocObject at -8(%esp) */
        move_imm_to_mem(OpndSize_32, (int)classPtr, 0, PhysicalReg_ESP, true);
#else
        move_reg_to_mem(OpndSize_32, PhysicalReg_EAX, true, 0, PhysicalReg_ESP, true); //resolved class
#endif
        move_imm_to_mem(OpndSize_32, ALLOC_DONT_TRACK, 4, PhysicalReg_ESP, true);
        scratchRegs[0] = PhysicalReg_SCRATCH_3;
        nextVersionOfHardReg(PhysicalReg_EAX, 3); //next version has 3 refs
        call_dvmAllocObject();
        load_effective_addr(8, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
        //return value of dvmAllocObject is in %eax
        //if return value is null, throw exception
        compare_imm_reg(OpndSize_32, 0, PhysicalReg_EAX, true);
#if defined(WITH_JIT)
        conditional_jump(Condition_NE, ".new_instance_done", true);
        //jump to dvmJitToExceptionThrown
        scratchRegs[0] = PhysicalReg_SCRATCH_4;
        jumpToExceptionThrown(3/*exception number*/);
#else
        handlePotentialException(
                                           Condition_E, Condition_NE,
                                           3, "common_exceptionThrown");
#endif
    }
    insertLabel(".new_instance_done", true);
    set_virtual_reg(vA, OpndSize_32, PhysicalReg_EAX, true);
    return 0;
}

//! function to initialize a class

//!INPUT: %eax (class object) %eax is recovered before return
//!OUTPUT: none
//!CALL: dvmInitClass
//!%eax, %esi, %ebx are live through function new_instance_needinit
int new_instance_needinit() {
    insertLabel(".new_instance_needinit", false);
    load_effective_addr(-8, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, PhysicalReg_EAX, true, 0, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, PhysicalReg_EAX, true, 4, PhysicalReg_ESP, true);
    scratchRegs[0] = PhysicalReg_ECX;
    call_dvmInitClass();
    //if return value of dvmInitClass is zero, throw exception
    compare_imm_reg(OpndSize_32, 0, PhysicalReg_EAX, true);
    //recover EAX with the class object
    move_mem_to_reg(OpndSize_32, 4, PhysicalReg_ESP, true, PhysicalReg_EAX, true);
    load_effective_addr(8, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    conditional_jump(Condition_E, "common_exceptionThrown", false);
    x86_return();
    return 0;
}
#undef P_GPR_1
#undef P_GPR_2
#undef P_GPR_3

#define P_GPR_1 PhysicalReg_EBX //live through C function, must in callee-saved reg
#define P_GPR_2 PhysicalReg_ECX
#define P_GPR_3 PhysicalReg_EDX

/**
 * @brief Generate native code for bytecode new-array
 * @param mir bytecode representation
 * @return value >= 0 when handled
 */
int op_new_array(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_NEW_ARRAY);
    u2 vA = mir->dalvikInsn.vA; //destination
    u2 vB = mir->dalvikInsn.vB; //length
    u4 tmp = mir->dalvikInsn.vC;
#ifdef INC_NCG_O0
    if(gDvm.helper_switch[17]) {
        // .new_array_helper
        //   INPUT: P_GPR_3 (const pool index)
        //          P_GPR_1 (virtual register with size of the array)
        //   OUTPUT: %eax
        //   no reg is live through function new_array_helper
        export_pc(); //use %edx
        move_imm_to_reg(OpndSize_32, tmp, P_GPR_3, true);
        move_imm_to_reg(OpndSize_32, vB, P_GPR_1, true);
        spillVirtualReg(vB, LowOpndRegType_gp, true);
        call_helper_API(".new_array_helper");
        set_virtual_reg(vA, OpndSize_32, PhysicalReg_EAX, true);
    }
    else
#endif
    {
        /////////////////////////
        //   REGS used: %esi, %eax, P_GPR_1, P_GPR_2
        //   CALL class_resolve, dvmAllocArrayByClass
        export_pc(); //use %edx
        //check size of the array, if negative, throw exception
        get_virtual_reg(vB, OpndSize_32, 5, false);
        compare_imm_reg(OpndSize_32, 0, 5, false);
        handlePotentialException(
                                           Condition_S, Condition_NS,
                                           1, "common_errNegArraySize");
#if defined(WITH_JIT)
       void *classPtr = (void*)
            (currentMethod->clazz->pDvmDex->pResClasses[tmp]);
       assert(classPtr != NULL);
#else
        //try to resolve class, if already resolved, jump to resolved
        //if not, call class_resolve
        scratchRegs[0] = PhysicalReg_SCRATCH_1; scratchRegs[1] = PhysicalReg_SCRATCH_2;
        scratchRegs[2] = PhysicalReg_Null; scratchRegs[3] = PhysicalReg_Null;
        get_res_classes(3, false);
        move_mem_to_reg(OpndSize_32, tmp*4, 3, false, PhysicalReg_EAX, true);
        compare_imm_reg(OpndSize_32, 0, PhysicalReg_EAX, true);
        conditional_jump(Condition_NE, ".new_array_resolved", true);
        rememberState(1);
        move_imm_to_reg(OpndSize_32, tmp, PhysicalReg_EAX, true);
        call_helper_API(".class_resolve");
        transferToState(1);
#endif
        //here, class is already resolved, the class object is in %eax
        //prepare to call dvmAllocArrayByClass with inputs: resolved class, array length, flag ALLOC_DONT_TRACK
        insertLabel(".new_array_resolved", true);
        load_effective_addr(-12, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
#if defined(WITH_JIT)
        /* 1st argument to dvmAllocArrayByClass at 0(%esp) */
        move_imm_to_mem(OpndSize_32, (int)classPtr, 0, PhysicalReg_ESP, true);
#else
        move_reg_to_mem(OpndSize_32, PhysicalReg_EAX, true, 0, PhysicalReg_ESP, true);
#endif
        move_reg_to_mem(OpndSize_32, 5, false, 4, PhysicalReg_ESP, true);
        move_imm_to_mem(OpndSize_32, ALLOC_DONT_TRACK, 8, PhysicalReg_ESP, true);
        scratchRegs[0] = PhysicalReg_SCRATCH_3;
        nextVersionOfHardReg(PhysicalReg_EAX, 3); //next version has 3 refs
        call_dvmAllocArrayByClass();
        load_effective_addr(12, PhysicalReg_ESP, true, PhysicalReg_ESP, true);

        //the allocated object is in %eax
        //check whether it is null, throw exception if null
        compare_imm_reg(OpndSize_32, 0, PhysicalReg_EAX, true);
#if defined(WITH_JIT)
        conditional_jump(Condition_NE, ".new_array_done", true);
        //jump to dvmJitToExceptionThrown
        scratchRegs[0] = PhysicalReg_SCRATCH_4;
        jumpToExceptionThrown(2/*exception number*/);
#else
        handlePotentialException(
                                           Condition_E, Condition_NE,
                                           2, "common_exceptionThrown");
#endif
        insertLabel(".new_array_done", true);
        set_virtual_reg(vA, OpndSize_32, PhysicalReg_EAX, true);
        //////////////////////////////////////
    }
    return 0;
}
#undef P_GPR_1
#undef P_GPR_2
#undef P_GPR_3

#define P_GPR_1 PhysicalReg_EBX
#define P_GPR_2 PhysicalReg_ECX
#define P_GPR_3 PhysicalReg_ESI
//! common code to lower FILLED_NEW_ARRAY

//! call: class_resolve call_dvmAllocPrimitiveArray
//! exception: filled_new_array_notimpl common_exceptionThrown
int common_filled_new_array(u2 length, u4 tmp, bool hasRange) {
#if defined(WITH_JIT)
    ClassObject *classPtr =
              (currentMethod->clazz->pDvmDex->pResClasses[tmp]);
    if(classPtr != NULL) LOGI("FILLED_NEW_ARRAY class %s", classPtr->descriptor);
#endif
    //check whether class is resolved, if yes, jump to resolved
    //if not, call class_resolve
    scratchRegs[0] = PhysicalReg_SCRATCH_1; scratchRegs[1] = PhysicalReg_SCRATCH_2;
    scratchRegs[2] = PhysicalReg_Null; scratchRegs[3] = PhysicalReg_Null;
    get_res_classes(3, false);
    move_mem_to_reg(OpndSize_32, tmp*4, 3, false, PhysicalReg_EAX, true);
    export_pc();
    compare_imm_reg(OpndSize_32, 0, PhysicalReg_EAX, true); //resolved class
    conditional_jump(Condition_NE, ".filled_new_array_resolved", true);
    rememberState(1);
    move_imm_to_reg(OpndSize_32, tmp, PhysicalReg_EAX, true);
    call_helper_API(".class_resolve");
    transferToState(1);
    //here, class is already resolved
    insertLabel(".filled_new_array_resolved", true);
    //check descriptor of the class object, if not implemented, throws exception
    move_mem_to_reg(OpndSize_32, 24, PhysicalReg_EAX, true, 5, false);
    //load a single byte of the descriptor
    movez_mem_to_reg(OpndSize_8, 1, 5, false, 6, false);
    compare_imm_reg(OpndSize_32, 'I', 6, false);
    conditional_jump(Condition_E, ".filled_new_array_impl", true);
    compare_imm_reg(OpndSize_32, 'L', 6, false);
    conditional_jump(Condition_E, ".filled_new_array_impl", true);
    compare_imm_reg(OpndSize_32, '[', 6, false);
    conditional_jump(Condition_NE, ".filled_new_array_notimpl", false);

    insertLabel(".filled_new_array_impl", true);
    //prepare to call dvmAllocArrayByClass with inputs: classObject, length, flag ALLOC_DONT_TRACK
    load_effective_addr(-12, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    move_imm_to_mem(OpndSize_32, (int)classPtr, 0, PhysicalReg_ESP, true);
    move_imm_to_mem(OpndSize_32, length, 4, PhysicalReg_ESP, true);
    move_imm_to_mem(OpndSize_32, ALLOC_DONT_TRACK, 8, PhysicalReg_ESP, true);
    scratchRegs[0] = PhysicalReg_SCRATCH_3; scratchRegs[1] = PhysicalReg_Null;
    if(hasRange) {
        nextVersionOfHardReg(PhysicalReg_EAX, 5+(length >= 1 ? LOOP_COUNT : 0)); //next version
    }
    else {
        nextVersionOfHardReg(PhysicalReg_EAX, 5+length); //next version
    }
    call_dvmAllocArrayByClass();
    load_effective_addr(12, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    //return value of dvmAllocPrimitiveArray is in %eax
    //if the return value is null, throw exception
    compare_imm_reg(OpndSize_32, 0, PhysicalReg_EAX, true);
    handlePotentialException(
                                       Condition_E, Condition_NE,
                                       3, "common_exceptionThrown");

    /* we need to mark the card of the new array, if it's not an int */
    compare_imm_reg(OpndSize_32, 'I', 6, false);
    conditional_jump(Condition_E, ".dont_mark_filled_new_array", true);

    // Need to make copy of EAX, because it's used later in op_filled_new_array()
    move_reg_to_reg(OpndSize_32, PhysicalReg_EAX, true, 6, false);

    markCard_filled(6, false, PhysicalReg_SCRATCH_4, false);

    insertLabel(".dont_mark_filled_new_array", true);

    //return value of bytecode FILLED_NEW_ARRAY is in GLUE structure
    scratchRegs[0] = PhysicalReg_SCRATCH_4; scratchRegs[1] = PhysicalReg_Null;
    set_return_value(OpndSize_32, PhysicalReg_EAX, true);
    return 0;
}

/**
 * @brief Generate native code for bytecode filled-new-array
 * @param mir bytecode representation
 * @return value >= 0 when handled
 */
int op_filled_new_array(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_FILLED_NEW_ARRAY);
    u2 length = mir->dalvikInsn.vA;
    u4 tmp = mir->dalvikInsn.vB;
    u2 v1, v2, v3, v4, v5;

    // Note that v1, v2, v3, v4, and/or v5 may not be valid.
    // Always check "length" before using any of them.
    v5 = mir->dalvikInsn.arg[4];
    v4 = mir->dalvikInsn.arg[3];
    v3 = mir->dalvikInsn.arg[2];
    v2 = mir->dalvikInsn.arg[1];
    v1 = mir->dalvikInsn.arg[0];

    common_filled_new_array(length, tmp, false);
    if(length >= 1) {
        //move from virtual register to contents of array object
        get_virtual_reg(v1, OpndSize_32, 7, false);
        move_reg_to_mem(OpndSize_32, 7, false, offArrayObject_contents, PhysicalReg_EAX, true);
    }
    if(length >= 2) {
        //move from virtual register to contents of array object
        get_virtual_reg(v2, OpndSize_32, 8, false);
        move_reg_to_mem(OpndSize_32, 8, false, offArrayObject_contents+4, PhysicalReg_EAX, true);
    }
    if(length >= 3) {
        //move from virtual register to contents of array object
        get_virtual_reg(v3, OpndSize_32, 9, false);
        move_reg_to_mem(OpndSize_32, 9, false, offArrayObject_contents+8, PhysicalReg_EAX, true);
    }
    if(length >= 4) {
        //move from virtual register to contents of array object
        get_virtual_reg(v4, OpndSize_32, 10, false);
        move_reg_to_mem(OpndSize_32, 10, false, offArrayObject_contents+12, PhysicalReg_EAX, true);
    }
    if(length >= 5) {
        //move from virtual register to contents of array object
        get_virtual_reg(v5, OpndSize_32, 11, false);
        move_reg_to_mem(OpndSize_32, 11, false, offArrayObject_contents+16, PhysicalReg_EAX, true);
    }
    return 0;
}
//! function to handle the error of array not implemented

//!
int filled_new_array_notimpl() {
    //two inputs for common_throw:
    insertLabel(".filled_new_array_notimpl", false);
    move_imm_to_reg(OpndSize_32, LstrFilledNewArrayNotImpl, PhysicalReg_EAX, true);
    move_imm_to_reg(OpndSize_32, (int) gDvm.exInternalError, PhysicalReg_ECX, true);
    unconditional_jump("common_throw", false);
    return 0;
}

#define P_SCRATCH_1 PhysicalReg_EDX

/**
 * @brief Generate native code for bytecode filled-new-array/range
 * @param mir bytecode representation
 * @return value >= 0 when handled
 */
int op_filled_new_array_range(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_FILLED_NEW_ARRAY_RANGE);
    u2 length = mir->dalvikInsn.vA;
    u4 tmp = mir->dalvikInsn.vB;
    u4 vC = mir->dalvikInsn.vC;
    common_filled_new_array(length, tmp, true/*hasRange*/);
    //here, %eax points to the array object
    if(length >= 1) {
        //dump all virtual registers used by this bytecode to stack, for NCG O1
        int k;
        for(k = 0; k < length; k++) {
            spillVirtualReg(vC+k, LowOpndRegType_gp, true); //will update refCount
        }
        //address of the first virtual register that will be moved to the array object
        load_effective_addr(vC*4, PhysicalReg_FP, true, 7, false); //addr
        //start address for contents of the array object
        load_effective_addr(offArrayObject_contents, PhysicalReg_EAX, true, 8, false); //addr
        //loop counter
        move_imm_to_reg(OpndSize_32, length-1, 9, false); //counter
        //start of the loop
        insertLabel(".filled_new_array_range_loop1", true);
        rememberState(1);
        move_mem_to_reg(OpndSize_32, 0, 7, false, 10, false);
        load_effective_addr(4, 7, false, 7, false);
        move_reg_to_mem(OpndSize_32, 10, false, 0, 8, false);
        load_effective_addr(4, 8, false, 8, false);
        alu_binary_imm_reg(OpndSize_32, sub_opc, 1, 9, false);
        transferToState(1);
        //jump back to the loop start
        conditional_jump(Condition_NS, ".filled_new_array_range_loop1", true);
    }
    return 0;
}
#undef P_GPR_1
#undef P_GPR_2
#undef P_GPR_3
#undef P_SCRATCH_1

#define P_GPR_1 PhysicalReg_EBX

/**
 * @brief Generate native code for bytecode fill-array-data
 * @details Calls dvmInterpHandleFillArrayData
 * @param mir bytecode representation
 * @param dalvikPC program counter for Dalvik bytecode
 * @return value >= 0 when handled
 */
int op_fill_array_data(const MIR * mir, const u2 * dalvikPC) {
    assert(mir->dalvikInsn.opcode == OP_FILL_ARRAY_DATA);
    u2 vA = mir->dalvikInsn.vA;
    u4 tmp = mir->dalvikInsn.vB;
    scratchRegs[0] = PhysicalReg_SCRATCH_1;
    scratchRegs[1] = PhysicalReg_Null;
    scratchRegs[2] = PhysicalReg_Null; scratchRegs[3] = PhysicalReg_Null;
    get_virtual_reg(vA, OpndSize_32, 1, false);
    //prepare to call dvmInterpHandleFillArrayData, input: array object, address of the data
    load_effective_addr(-8, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, 1, false, 0, PhysicalReg_ESP, true);
#if !defined(WITH_JIT)
    //get eip at run time
    //add eip with the relative offset between eip and data
    get_eip_API();
    //calculate the address of the data to fill in the array object
    insertDataWorklist((s4)tmp, stream); //relativePC & stream
    alu_binary_imm_reg(OpndSize_32, add_opc, 0, PhysicalReg_EDX, true);
    move_reg_to_mem(OpndSize_32, PhysicalReg_EDX, true, 4, PhysicalReg_ESP, true);
#else
    /* 2nd argument to dvmInterpHandleFillArrayData at 4(%esp) */
    move_imm_to_mem(OpndSize_32, (int)(dalvikPC+tmp), 4, PhysicalReg_ESP, true);
#endif
    call_dvmInterpHandleFillArrayData();
    load_effective_addr(8, PhysicalReg_ESP, true, PhysicalReg_ESP, true);

    //check return value of dvmInterpHandleFillArrayData, if zero, throw exception
    compare_imm_reg(OpndSize_32, 0, PhysicalReg_EAX, true);
#if defined(WITH_JIT)
    conditional_jump(Condition_NE, ".fill_array_data_done", true);
    //jump to dvmJitToExceptionThrown
    scratchRegs[0] = PhysicalReg_SCRATCH_2;
    jumpToExceptionThrown(2/*exception number*/);
#else
    handlePotentialException(
                                       Condition_E, Condition_NE,
                                       2, "common_exceptionThrown");
#endif
    insertLabel(".fill_array_data_done", true);
    return 0;
}
#undef P_GPR_1

#define P_GPR_1 PhysicalReg_EBX

/**
 * @brief Generate native code for bytecode throw
 * @param mir bytecode representation
 * @return value >= 0 when handled
 */
int op_throw(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_THROW);
    u2 vA = mir->dalvikInsn.vA;
    export_pc();
#ifdef INC_NCG_O0
    if(gDvm.helper_switch[12]) {
        // .throw_helper
        //   INPUT: P_GPR_1 (virtual register for object)
        move_imm_to_reg(OpndSize_32, vA, P_GPR_1, true);
        unconditional_jump_global_API(".throw_helper", false);
    }
    else
#endif
    {
        get_virtual_reg(vA, OpndSize_32, 1, false);
        //null check
        compare_imm_reg(OpndSize_32, 0, 1, false);
        conditional_jump(Condition_E, "common_errNullObject", false);
        //set glue->exception & throw exception
        scratchRegs[2] = PhysicalReg_Null; scratchRegs[3] = PhysicalReg_Null;
        scratchRegs[0] = PhysicalReg_SCRATCH_1; scratchRegs[1] = PhysicalReg_SCRATCH_2;
        set_exception(1, false);
        unconditional_jump("common_exceptionThrown", false);
    }
    return 0;
}
#undef P_GPR_1
#define P_GPR_1 PhysicalReg_EBX

/**
 * @brief Generate native code for bytecode throw-verification-error
 * @param mir bytecode representation
 * @return value >= 0 when handled
 */
int op_throw_verification_error(const MIR * mir) {
    assert(mir->dalvikInsn.opcode == OP_THROW_VERIFICATION_ERROR);
    u2 vA = mir->dalvikInsn.vA;
    u2 vB = mir->dalvikInsn.vB;

    export_pc();
    scratchRegs[0] = PhysicalReg_SCRATCH_1;
    get_glue_method(1, false);

    load_effective_addr(-12, PhysicalReg_ESP, true, PhysicalReg_ESP, true);
    move_imm_to_mem(OpndSize_32, vB, 8, PhysicalReg_ESP, true);
    move_imm_to_mem(OpndSize_32, vA, 4, PhysicalReg_ESP, true);
    move_reg_to_mem(OpndSize_32, 1, false, 0, PhysicalReg_ESP, true);
    scratchRegs[0] = PhysicalReg_SCRATCH_2;
    call_dvmThrowVerificationError();
    load_effective_addr(12, PhysicalReg_ESP, true, PhysicalReg_ESP, true);

    unconditional_jump("common_exceptionThrown", false);
    return 0;
}
#undef P_GPR_1
