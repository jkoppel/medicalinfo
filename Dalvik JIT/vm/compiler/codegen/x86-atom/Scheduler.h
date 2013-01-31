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

/*! \file Scheduler.h
    \brief This file implements the interface for Atom Scheduler
*/

#ifndef ATOM_SCHEDULER_H_
#define ATOM_SCHEDULER_H_

#include "Lower.h"

//! \brief Interface for Atom Instruction Scheduler
class Scheduler {
private:
    //! \brief Defines implementation of a native basic block for Atom LIRs.
    typedef std::vector<LowOp*> NativeBasicBlock;

    //! \brief Holds a list of all LIRs allocated via allocateNewEmptyLIR
    //! which are not yet in code stream.
    //! \details The field LowOp::slotId corresponds to index into this list
    //! when the LIR is allocated by scheduler via allocateNewEmptyLIR.
    //! \see allocateNewEmptyLIR
    NativeBasicBlock queuedLIREntries;

    //! \brief Holds a list of scheduled LIRs in their scheduled order.
    //! \details It contains the same LIRs are queuedLIREntries, just
    //! in a possibly different order.
    //! \see queuedLIREntries
    NativeBasicBlock scheduledLIREntries;

    //! \brief Used to keep track of writes to a resource.
    //! \details This is used only during dependency building but corresponding
    //! LIRs are also updated to keep track of their own dependencies which is
    //! used during scheduling.
    //! \see LowOp::predecessorDependencies
    //! \see UseDefUserEntry
    std::vector<UseDefProducerEntry> producerEntries;

    //! \brief Used to keep track of reads from a resource.
    //! \details This is used only during dependency building but corresponding
    //! LIRs are also updated to keep track of their own dependencies which is
    //! used during scheduling.
    //! \see LowOp::predecessorDependencies
    //! \see UseDefUserEntry
    std::vector<UseDefUserEntry> userEntries;

    //! \brief Used to keep track of dependencies on control flags.
    //! \details This is used only during dependency building but corresponding
    //! LIRs are also updated to keep track of their own dependencies which is
    //! used during scheduling. This list holds values from LowOp::slotId.
    //! \see LowOp::predecessorDependencies
    //! \see LowOp::slotId
    std::vector<int> ctrlEntries;

    // See documentation in Schedule.cpp or in doxygen output for undocumented prototypes.
    void updateDependencyGraph(UseDefEntryType type, int regNum,
            LowOpndDefUse defuse, LowOp* op);
    void updateDependencyGraphForMem(LowOpndMem & mOpnd, LowOpndDefUse defuse,
            LowOp* op);
    void handlePushDependencyUpdate(LowOp* op);
    void handleFloatDependencyUpdate(LowOp* op);
    void updateReadyOps(int chosenIdx, bool* scheduledOps, bool* pendingOps,
            bool* readyOps);
    bool isBasicBlockDelimiter(Mnemonic m);
    void generateAssembly(LowOp * op);
    void findLongestPath();
    void schedule();
    double basicBlockEditDistance(const NativeBasicBlock & block1,
            const NativeBasicBlock & block2);
    void printStatistics(bool prescheduling);
public:
    //! \brief Called by users of scheduler to allocate an empty LIR (no mnemonic
    //! or operands).
    //! \details The caller of this method takes the LIR, updates the mnemonic
    //! and operand information, and then calls one of the updateUseDefInformation
    //! methods in the scheduler with this LIR as parameter. This method should not
    //! be called when scheduling is not enabled because the LIR will never be freed.
    //! Internally, the scheduler will add this LIR to the native basic block it
    //! is building and also assign it an id.
    //! \tparam is a LowOp or any of its specialized children.
    //! \see LowOp
    template<typename LowOpType> LowOpType * allocateNewEmptyLIR() {
        LowOpType * op = new LowOpType;
        op->slotId = queuedLIREntries.size();
        queuedLIREntries.push_back(op);
        return op;
    }

    // See documentation in Schedule.cpp or in doxygen output for undocumented prototypes.
    void updateUseDefInformation(LowOp * op);
    void updateUseDefInformation_imm(LowOp * op);
    void updateUseDefInformation_reg(LowOpReg * op);
    void updateUseDefInformation_mem(LowOpMem * op);
    void updateUseDefInformation_imm_to_reg(LowOpImmReg * op);
    void updateUseDefInformation_imm_to_mem(LowOpImmMem * op);
    void updateUseDefInformation_reg_to_reg(LowOpRegReg * op);
    void updateUseDefInformation_mem_to_reg(LowOpMemReg * op);
    void updateUseDefInformation_reg_to_mem(LowOpRegMem * op);
    void updateUseDefInformation_mem_scale_to_reg(LowOpMemReg * op);
    void updateUseDefInformation_reg_to_mem_scale(LowOpRegMem * op);
    void updateUseDefInformation_fp_to_mem(LowOpRegMem * op);
    void updateUseDefInformation_mem_to_fp(LowOpMemReg * op);
    void signalEndOfNativeBasicBlock();
};

#endif /* ATOM_SCHEDULER_H_ */
