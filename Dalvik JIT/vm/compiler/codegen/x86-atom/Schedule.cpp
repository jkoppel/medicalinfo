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

/*! \file Schedule.cpp
    \brief This file implements the Atom Instruction Scheduler.
    \details Scheduling algorithm implemented is basic block scheduling.
*/

#include "Lower.h"
#include "interp/InterpDefs.h"
#include "Scheduler.h"

//! \brief Used to replace all calls to printf to calls to LOGD.
//! \details Needs to be commented out when running on host.
#define printf LOGD

//! \def DISABLE_ATOM_SCHEDULING_STATISTICS
//! \brief Disables printing of scheduling statistics.
//! \details Defining this macro disables printing of scheduling statistics pre
//! and post scheduling. Undefine macro when statistics are needed.
#define DISABLE_ATOM_SCHEDULING_STATISTICS

//! \def DISABLE_DEBUG_ATOM_SCHEDULER
//! \brief Disables debug printing for atom scheduler.
//! \details Defining macro DISABLE_DEBUG_ATOM_SCHEDULER disables debug printing.
//! Undefine macro when debugging scheduler implementation.
#define DISABLE_DEBUG_ATOM_SCHEDULER

//! \def ENABLE_BASIC_BLOCK_DUMP
//! \brief Enables scheduler to wait until it has formed a basic block.
//! \details Defining this macro allows scheduler to dump the x86 instructions as
//! a basic block instead of doing it one-at-a-time as they come in.
//! Undefined macro when trying to debug basic block detection.
#define ENABLE_BASIC_BLOCK_DUMP

//! \def DISABLE_MACHINE_MODEL_CHECK
//! \brief Disables checks on the machine model.
//! \details Defining this macro disables checks of the machine model.
//! Undefining this leads to Dalvik aborting when invalid information
//! is used from machine model. For example, Dalvik will abort when an
//! instruction does not have latency information.
#define DISABLE_MACHINE_MODEL_CHECK

//! \def g_SchedulerInstance
//! \brief Global Atom instruction scheduler instance
Scheduler g_SchedulerInstance;

//! \enum IssuePort
//! \brief Defines possible combinations of port-binding information for use
//! with information about each x86 mnemonic.
enum IssuePort {
    //! \brief invalid port, used for table only when some
    //! operands are not supported for the mnemonic
    INVALID_PORT = -1,
    //! \brief the mnemonic can only be issued on port 0
    PORT0 = 0,
    //! \brief the mnemonic can only be issued on port 1
    PORT1 = 1,
    //! \brief the mnemonic can be issued on either port
    EITHER_PORT,
    //! \brief both ports are used for the mnemonic
    BOTH_PORTS
};

//! \def MachineModelEntry
//! \brief Information needed to define the machine model for each x86 mnemonic.
struct MachineModelEntry {
    //! \brief which port the instruction can execute on
    IssuePort issuePortType;
    //! \brief execute to execute time for one instruction
    int executeToExecuteLatency;
};

//! \def INVP
//! \brief This is an abbreviation of INVALID_PORT and is used for readability
//! reasons.
#define INVP INVALID_PORT

//! \def INVN
//! \brief This is an abbreviation of invalid node latency and is used for
//! readability reasons.
#define INVN -1

//! \def REG_NOT_USED
//! \brief This is an abbreviation for register not used and is used for
//! readability reasons whenever a Scheduler method needs register type
//! to update some data structure but a register number does not make
//! sense in the context.
#define REG_NOT_USED -1

//! \brief This table lists the parameters for each Mnemonic in the Atom Machine Model.
//! \details This table includes port and latency information for each mnemonic for each possible
//! configuration of operands. 6 entries of MachineModelEntry are reserved for each Mnemonic:
//! - If a Mnemonic has zero operand, only the first entry is valid
//! - If a Mnemonic has a single operand, the first 3 entries are valid, for operand type
//! imm, reg and mem respectively
//! - If a Mnemonic has two operands, the last 5 entries are valid, for operand types
//! imm_to_reg, imm_to_mem, reg_to_reg, mem_to_reg and reg_to_mem
//!
//! This table matches content from Intel 64 and IA-32 Architectures Optimization
//! Reference Manual (April 2011), Section 13.4
//! \todo Make sure that the invalid entries are never used in the algorithm
MachineModelEntry atomMachineModel[Mnemonic_Count*6] = {
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //NULL, Null
    {INVP,INVN},{EITHER_PORT,1},{PORT0,1},{EITHER_PORT,1},{PORT0,1},{INVP,INVN}, //ADC
    {INVP,INVN},{EITHER_PORT,1},{PORT0,1},{EITHER_PORT,1},{PORT0,1},{INVP,INVN}, //ADD
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{PORT1,5},{BOTH_PORTS,5},{INVP,INVN}, //ADDSD
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{PORT1,5},{BOTH_PORTS,5},{INVP,INVN}, //ADDSS
    {INVP,INVN},{EITHER_PORT,1},{PORT0,1},{EITHER_PORT,1},{PORT0,1},{INVP,INVN}, //AND
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //BSF
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //BSR
    {BOTH_PORTS,1},{BOTH_PORTS,1},{EITHER_PORT,2},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //CALL
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //CMC
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{EITHER_PORT,1},{INVP,INVN},{INVP,INVN}, //CWD, CDQ

    {INVP,INVN},{INVP,INVN},{INVP,INVN},{EITHER_PORT,1},{INVP,INVN},{INVP,INVN}, //CMOV_O
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{EITHER_PORT,1},{INVP,INVN},{INVP,INVN}, //CMOV_NO
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{EITHER_PORT,1},{INVP,INVN},{INVP,INVN}, //CMOV_B,NAE,C
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{EITHER_PORT,1},{INVP,INVN},{INVP,INVN}, //CMOV_NB,AE,NC
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{EITHER_PORT,1},{INVP,INVN},{INVP,INVN}, //CMOV_Z,E
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{EITHER_PORT,1},{INVP,INVN},{INVP,INVN}, //CMOV_NZ,NE
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{EITHER_PORT,1},{INVP,INVN},{INVP,INVN}, //CMOV_BE,NA
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{EITHER_PORT,1},{INVP,INVN},{INVP,INVN}, //CMOV_NBE,A
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{EITHER_PORT,1},{INVP,INVN},{INVP,INVN}, //CMOV_S
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{EITHER_PORT,1},{INVP,INVN},{INVP,INVN}, //CMOV_NS
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{EITHER_PORT,1},{INVP,INVN},{INVP,INVN}, //CMOV_P,PE
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{EITHER_PORT,1},{INVP,INVN},{INVP,INVN}, //CMOV_NP,PO
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{EITHER_PORT,1},{INVP,INVN},{INVP,INVN}, //CMOV_L,NGE
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{EITHER_PORT,1},{INVP,INVN},{INVP,INVN}, //CMOV_NL,GE
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{EITHER_PORT,1},{INVP,INVN},{INVP,INVN}, //CMOV_LE,NG
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{EITHER_PORT,1},{INVP,INVN},{INVP,INVN}, //CMOV_NLE,G

    {INVP,INVN},{EITHER_PORT,1},{PORT0,1},{EITHER_PORT,1},{PORT0,1},{PORT0,1}, //CMP
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //CMPXCHG
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //CMPXCHG8B
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //CMPSB
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //CMPSW
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //CMPSD

    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //CVTSD2SS
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //CVTSD2SI
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //CVTTSD2SI
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //CVTSS2SD
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //CVTSS2SI
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //CVTTSS2SI
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //CVTSI2SD
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //CVTSI2SS

    {INVP,INVN},{BOTH_PORTS,9},{INVP,INVN},{BOTH_PORTS,9},{BOTH_PORTS,10},{INVP,INVN}, //COMISD
    {INVP,INVN},{BOTH_PORTS,9},{INVP,INVN},{BOTH_PORTS,9},{BOTH_PORTS,10},{INVP,INVN}, //COMISS
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //DEC
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{BOTH_PORTS,64},{BOTH_PORTS,64},{INVP,INVN}, //DIVSD
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{BOTH_PORTS,34},{BOTH_PORTS,34},{INVP,INVN}, //DIVSS

    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //ENTER
    {INVP,INVN},{INVP,INVN},{BOTH_PORTS,5},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FLDCW
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FADDP
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FLDZ
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FADD
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FSUBP
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FSUB
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FISUB
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FMUL
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FMULP
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FDIVP
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FDIV
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{PORT1,1},{INVP,INVN},{INVP,INVN}, //FUCOM
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FUCOMI
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{PORT1,1},{INVP,INVN},{INVP,INVN}, //FUCOMP
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FUCOMIP
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FUCOMPP
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FRNDINT
    {INVP,INVN},{INVP,INVN},{BOTH_PORTS,5},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FNSTCW
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FSTSW
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FNSTSW
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{BOTH_PORTS,5},{INVP,INVN}, //FILD
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{PORT0,1},{INVP,INVN}, //FLD
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FLDLG2
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FLDLN2
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FLD1

    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FCLEX
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FCHS
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FNCLEX
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{BOTH_PORTS,2}, //FIST
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{BOTH_PORTS,2}, //FISTP
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FISTTP
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FPREM
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FPREM1
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{BOTH_PORTS,2}, //FST fp_mem
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{BOTH_PORTS,2}, //FSTP
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FSQRT
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FABS
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FSIN
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FCOS
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FPTAN
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FYL2X
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FYL2XP1
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //F2XM1
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FPATAN
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FXCH
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //FSCALE

    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //XCHG
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //DIV
    {INVP,INVN},{BOTH_PORTS,57},{BOTH_PORTS,57},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //IDIV
    {INVP,INVN},{BOTH_PORTS,6},{BOTH_PORTS,6},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //MUL
    {INVP,INVN},{PORT0,5},{PORT0,5},{PORT0,5},{PORT0,5},{INVP,INVN}, //IMUL
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //INC
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //INT3

    {PORT1,1},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //Jcc_O
    {PORT1,1},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //Jcc_NO
    {PORT1,1},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //Jcc_B
    {PORT1,1},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //Jcc_NB
    {PORT1,1},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //Jcc_Z
    {PORT1,1},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //Jcc_NZ
    {PORT1,1},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //Jcc_BE
    {PORT1,1},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //Jcc_NBE
    {PORT1,1},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //Jcc_S
    {PORT1,1},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //Jcc_NS
    {PORT1,1},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //Jcc_P
    {PORT1,1},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //Jcc_NP
    {PORT1,1},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //Jcc_L
    {PORT1,1},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //Jcc_NL
    {PORT1,1},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //Jcc_LE
    {PORT1,1},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //Jcc_NLE

    {PORT1,1},{PORT1,1},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //JMP
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{PORT1,1},{INVP,INVN}, //LEA
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //LEAVE
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //LOOP
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //LOOPE
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //LOOPNE
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //LAHF

    {INVP,INVN},{EITHER_PORT,1},{PORT0,1},{EITHER_PORT,1},{PORT0,1},{PORT0,1}, //MOV
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //MOVD
    {INVP,INVN},{PORT0,1},{PORT0,1},{PORT0,1},{PORT0,1},{PORT0,1}, //MOVQ
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //MOVS8
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //MOVS16
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //MOVS32
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //MOVS64
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //MOVAPD
    {INVP,INVN},{PORT0,1},{INVP,INVN},{EITHER_PORT,1},{PORT0,1},{PORT0,1}, //MOVSD
    {INVP,INVN},{PORT0,1},{INVP,INVN},{EITHER_PORT,1},{PORT0,1},{PORT0,1}, //MOVSS
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{PORT0,1},{INVP,INVN}, //MOVSX
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{PORT0,1},{INVP,INVN}, //MOVZX

    {INVP,INVN},{INVP,INVN},{INVP,INVN},{PORT0,5},{PORT0,5},{INVP,INVN}, //MULSD
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{PORT0,4},{PORT0,4},{INVP,INVN}, //MULSS
    {INVP,INVN},{EITHER_PORT,1},{PORT0,10},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //NEG
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //NOP
    {INVP,INVN},{EITHER_PORT,1},{PORT0,10},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //NOT
    {INVP,INVN},{EITHER_PORT,1},{PORT0,1},{EITHER_PORT,1},{PORT0,1},{INVP,INVN}, //OR
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //PREFETCH

    {INVP,INVN},{EITHER_PORT,1},{PORT0,1},{EITHER_PORT,1},{PORT0,1},{INVP,INVN}, //PADDQ
    {INVP,INVN},{EITHER_PORT,1},{PORT0,1},{EITHER_PORT,1},{PORT0,1},{INVP,INVN}, //PAND
    {INVP,INVN},{EITHER_PORT,1},{PORT0,1},{EITHER_PORT,1},{PORT0,1},{INVP,INVN}, //POR
    {INVP,INVN},{EITHER_PORT,1},{PORT0,1},{EITHER_PORT,1},{PORT0,1},{INVP,INVN}, //PSUBQ
    {INVP,INVN},{EITHER_PORT,1},{PORT0,1},{EITHER_PORT,1},{PORT0,1},{INVP,INVN}, //PANDN
    {INVP,INVN},{PORT0,1},{INVP,INVN},{BOTH_PORTS,2},{BOTH_PORTS,3},{INVP,INVN}, //PSLLQ
    {INVP,INVN},{PORT0,1},{INVP,INVN},{BOTH_PORTS,2},{BOTH_PORTS,3},{INVP,INVN}, //PSRLQ
    {INVP,INVN},{EITHER_PORT,1},{PORT0,1},{EITHER_PORT,1},{PORT0,1},{INVP,INVN}, //PXOR
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //POP
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //POPFD
    {INVP,INVN},{BOTH_PORTS,1},{BOTH_PORTS,2},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //PUSH
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //PUSHFD
    {BOTH_PORTS,1},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //RET

    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //SET_O
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //SET_NO
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //SET_B
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //SET_NB
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //SET_Z
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //SET_NZ
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //SET_BE
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //SET_NBE
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //SET_S
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //SET_NS
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //SET_P
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //SET_NP
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //SET_L
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //SET_NL
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //SET_LE
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //SET_NLE

    {INVP,INVN},{PORT0,1},{PORT0,1},{PORT0,1},{INVP,INVN},{INVP,INVN}, //SAL,SHL
    {INVP,INVN},{PORT0,1},{PORT0,1},{PORT0,1},{INVP,INVN},{INVP,INVN}, //SAR
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //ROR
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //RCR
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //ROL
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //RCL
    {INVP,INVN},{PORT0,1},{PORT0,1},{PORT0,1},{INVP,INVN},{INVP,INVN}, //SHR
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //SHRD
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //SHLD
    {INVP,INVN},{EITHER_PORT,1},{PORT0,1},{EITHER_PORT,1},{PORT0,1},{INVP,INVN}, //SBB
    {INVP,INVN},{EITHER_PORT,1},{PORT0,1},{EITHER_PORT,1},{PORT0,1},{INVP,INVN}, //SUB
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{PORT1,5},{BOTH_PORTS,5},{INVP,INVN}, //SUBSD
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{PORT1,5},{BOTH_PORTS,5},{INVP,INVN}, //SUBSS

    {INVP,INVN},{EITHER_PORT,1},{PORT0,1},{EITHER_PORT,1},{INVP,INVN},{INVP,INVN}, //TEST
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //UCOMISD
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //UCOMISS
    {INVP,INVN},{EITHER_PORT,1},{PORT0,1},{EITHER_PORT,1},{PORT0,1},{INVP,INVN}, //XOR
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{EITHER_PORT,1},{PORT0,1},{INVP,INVN}, //XORPD
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //XORPS
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //CVTDQ2PD
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //CVTTPD2DQ
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //CVTDQ2PS
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //CVTTPS2DQ
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //STD
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //CLD
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //SCAS
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //STOS
    {INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN},{INVP,INVN}, //WAIT
};

//! \brief Get issue port for mnemonic with no operands
inline IssuePort getAtomMnemonicPort(Mnemonic m) {
    if (m >= Mnemonic_Count) return INVALID_PORT;
    return atomMachineModel[m*6].issuePortType;
}
//! \brief Get issue port for mnemonic with one immediate operand
inline IssuePort getAtomMnemonicPort_imm(Mnemonic m) {
    if (m >= Mnemonic_Count) return INVALID_PORT;
    return atomMachineModel[m*6].issuePortType;
}
//! \brief Get issue port for mnemonic with one register operand
inline IssuePort getAtomMnemonicPort_reg(Mnemonic m) {
    if (m >= Mnemonic_Count) return INVALID_PORT;
    return atomMachineModel[m*6+1].issuePortType;
}
//! \brief Get issue port for mnemonic with one memory operand
inline IssuePort getAtomMnemonicPort_mem(Mnemonic m) {
    if (m >= Mnemonic_Count) return INVALID_PORT;
    return atomMachineModel[m*6+2].issuePortType;
}
//! \brief Get issue port for mnemonic with two operands: immediate to register
inline IssuePort getAtomMnemonicPort_imm_to_reg(Mnemonic m) {
    if (m >= Mnemonic_Count) return INVALID_PORT;
    return atomMachineModel[m*6+1].issuePortType;
}
//! \brief Get issue port for mnemonic with two operands: immediate to memory
inline IssuePort getAtomMnemonicPort_imm_to_mem(Mnemonic m) {
    if (m >= Mnemonic_Count) return INVALID_PORT;
    return atomMachineModel[m*6+2].issuePortType;
}
//! \brief Get issue port for mnemonic with two operands: register to register
inline IssuePort getAtomMnemonicPort_reg_to_reg(Mnemonic m) {
    if (m >= Mnemonic_Count) return INVALID_PORT;
    return atomMachineModel[m*6+3].issuePortType;
}
//! \brief Get issue port for mnemonic with two operands: memory to register
inline IssuePort getAtomMnemonicPort_mem_to_reg(Mnemonic m) {
    if (m >= Mnemonic_Count) return INVALID_PORT;
    return atomMachineModel[m*6+4].issuePortType;
}
//! \brief Get issue port for mnemonic with two operands: register to memory
inline IssuePort getAtomMnemonicPort_reg_to_mem(Mnemonic m) {
    if (m >= Mnemonic_Count) return INVALID_PORT;
    return atomMachineModel[m*6+5].issuePortType;
}

//! \brief Get execute to execute latency for mnemonic with no operands
inline int getAtomMnemonicLatency(Mnemonic m) {
    if (m >= Mnemonic_Count) return INVN;
    return atomMachineModel[m*6].executeToExecuteLatency;
}
//! \brief Get execute to execute latency for mnemonic with one immediate operand
inline int getAtomMnemonicLatency_imm(Mnemonic m) {
    if (m >= Mnemonic_Count) return INVN;
    return atomMachineModel[m*6].executeToExecuteLatency;
}
//! \brief Get execute to execute latency for mnemonic with one register operand
inline int getAtomMnemonicLatency_reg(Mnemonic m) {
    if (m >= Mnemonic_Count) return INVN;
    return atomMachineModel[m*6+1].executeToExecuteLatency;
}
//! \brief Get execute to execute latency for mnemonic with one memory operand
inline int getAtomMnemonicLatency_mem(Mnemonic m) {
    if (m >= Mnemonic_Count) return INVN;
    return atomMachineModel[m*6+2].executeToExecuteLatency;
}
//! \brief Get execute to execute latency for mnemonic with two operands: immediate to register
inline int getAtomMnemonicLatency_imm_to_reg(Mnemonic m) {
    if (m >= Mnemonic_Count) return INVN;
    return atomMachineModel[m*6+1].executeToExecuteLatency;
}
//! \brief Get execute to execute latency for mnemonic with two operands: immediate to memory
inline int getAtomMnemonicLatency_imm_to_mem(Mnemonic m) {
    if (m >= Mnemonic_Count) return INVN;
    return atomMachineModel[m*6+2].executeToExecuteLatency;
}
//! \brief Get execute to execute latency for mnemonic with two operands: register to register
inline int getAtomMnemonicLatency_reg_to_reg(Mnemonic m) {
    if (m >= Mnemonic_Count) return INVN;
    return atomMachineModel[m*6+3].executeToExecuteLatency;
}
//! \brief Get execute to execute latency for mnemonic with two operands: memory to register
inline int getAtomMnemonicLatency_mem_to_reg(Mnemonic m) {
    if (m >= Mnemonic_Count) return INVN;
    return atomMachineModel[m*6+4].executeToExecuteLatency;
}
//! \brief Get execute to execute latency for mnemonic with two operands: register to memory
inline int getAtomMnemonicLatency_reg_to_mem(Mnemonic m) {
    if (m >= Mnemonic_Count) return INVN;
    return atomMachineModel[m*6+5].executeToExecuteLatency;
}

//! \def EDGE_LATENCY
//! \brief Defines weight of edges in the dependency graph.
#define EDGE_LATENCY 0

#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
//! \brief Transforms from LowOpndDefUse enum to string representation of the usedef
//! \see LowOpndDefUse
inline const char * getUseDefType(LowOpndDefUse defuse) {
    switch (defuse) {
    case LowOpndDefUse_Def:
        return "Def";
    case LowOpndDefUse_Use:
        return "Use";
    case LowOpndDefUse_UseDef:
        return "UseDef";
    }
    return "-";
}
#endif

#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
//! \brief Transforms from UseDefEntryType enum to a string representation.
//! \see UseDefEntryType
inline const char * getUseDefEntryType(UseDefEntryType type) {
    switch (type) {
    case UseDefType_Ctrl:
        return "Ctrl";
    case UseDefType_Float:
        return "Float";
    case UseDefType_MemVR:
        return "MemVR";
    case UseDefType_MemSpill:
        return "MemSpill";
    case UseDefType_MemUnknown:
        return "MemUnknown";
    case UseDefType_Reg:
        return "Reg";
    }
    return "-";
}
#endif

//! \brief Returns true if mnemonic is a variant of MOV.
inline bool isMoveMnemonic(Mnemonic m) {
    return m == Mnemonic_MOV || m == Mnemonic_MOVQ || m == Mnemonic_MOVSD
            || m == Mnemonic_MOVSS || m == Mnemonic_MOVZX || m == Mnemonic_MOVSX;
}

//! \brief Returns true if mnemonic uses and then defines the FLAGS register
inline bool usesAndDefinesFlags(Mnemonic m) {
    return m == Mnemonic_ADC || m == Mnemonic_SBB;
}

//! \brief Detects whether the mnemonic is a native basic block delimiter.
//! \details Unconditional jumps, conditional jumps, calls, and returns
//! always end a native basic block.
inline bool Scheduler::isBasicBlockDelimiter(Mnemonic m) {
    return (m == Mnemonic_JMP || m == Mnemonic_CALL
            || (m >= Mnemonic_Jcc && m < Mnemonic_JMP) || m == Mnemonic_RET);
}

//! \brief Given an access to a resource (Control, register, VR, unknown memory
//! access), this updates dependency graph, usedef information, and control flags.
//! \details Algorithm description for dependency update:
//! - for Use or UseDef:
//!   -# insert RAW dependency from producer for this op
//! - for Def or UseDef:
//!   -# insert WAR dependency from earlier user for this op
//!   -# insert WAW dependency from earlier producer for this op
//! - Internal data structure updates for record keeping:
//!   -# for Def or UseDef: update producerEntries
//!   -# for Def: clear corresponding use slots for entry in userEntries
//!   -# for UseDef: clear corresponding use slots for entry in userEntries
//!   -# for Use: update userEntries
//!
//! \param type resource that causes dependency
//! \param regNum is a number corresponding to a physical register or a Dalvik
//! virtual register. When physical, this is of enum type PhysicalReg.
//! \param defuse definition, usage, or both
//! \param op LIR for which to update dependencies
void Scheduler::updateDependencyGraph(UseDefEntryType type, int regNum,
        LowOpndDefUse defuse, LowOp* op) {
#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
    const char * string_defuse = getUseDefType(defuse);
    const char * string_type = getUseDefEntryType(type);
    printf("updateDependencyGraph for <%s %d> at slot %d %s\n", string_type,
            regNum, op->slotId, string_defuse);
#endif

    unsigned int k;
    unsigned int index_for_user = userEntries.size();
    unsigned int index_for_producer = producerEntries.size();

    if (type != UseDefType_Ctrl) {
        for (k = 0; k < producerEntries.size(); ++k) {
            if (producerEntries[k].type == type
                    && producerEntries[k].regNum == regNum) {
                index_for_producer = k;
                break;
            }
        }
    }
    for (k = 0; k < userEntries.size(); ++k) {
        if (userEntries[k].type == type && userEntries[k].regNum == regNum) {
            index_for_user = k;
            break;
        }
    }
#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
    printf("index_for_producer %d %d index_for_user %d %d\n",
            index_for_producer, producerEntries.size(),
            index_for_user, userEntries.size());
#endif

    if (defuse == LowOpndDefUse_Use) {
        // insert RAW from producer
        if (type != UseDefType_Ctrl
                && index_for_producer != producerEntries.size()) {
#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
            printf("insert RAW from %d to %d due to <%s %d>\n",
                    producerEntries[index_for_producer].producerSlot, op->slotId, string_type, regNum);
#endif
            DependencyInformation ds;
            ds.dataHazard = Dependency_RAW;
            ds.lowopSlotId = producerEntries[index_for_producer].producerSlot;
            ds.latency = EDGE_LATENCY;
            op->predecessorDependencies.push_back(ds);
        }
        if (type == UseDefType_Ctrl && ctrlEntries.size() > 0) {
            // insert RAW from the last producer
            int lastSlot = ctrlEntries.back();
#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
            printf("insert RAW from %d to %d due to Ctrl\n",
                    lastSlot, op->slotId);
#endif
            DependencyInformation ds;
            ds.dataHazard = Dependency_RAW;
            ds.lowopSlotId = lastSlot;
            ds.latency = EDGE_LATENCY;
            op->predecessorDependencies.push_back(ds);
            // insert WAW from earlier producers to the last producer
            unsigned int k2;
            LowOp* opLast = (queuedLIREntries[lastSlot]);
            for (k2 = 0; k2 < ctrlEntries.size() - 1; k2++) {
#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
                printf("insert WAW from %d to %d due to Ctrl\n", ctrlEntries[k2], lastSlot);
#endif
                DependencyInformation ds;
                ds.dataHazard = Dependency_WAW;
                ds.lowopSlotId = ctrlEntries[k2];
                ds.latency = EDGE_LATENCY;
                opLast->predecessorDependencies.push_back(ds);
            }
        }
        // update userEntries
        if (index_for_user == userEntries.size()) {
            // insert an entry in userTable
            UseDefUserEntry entry;
            entry.type = type;
            entry.regNum = regNum;
            userEntries.push_back(entry);
        } else if (type == UseDefType_Ctrl) {
            userEntries[index_for_user].useSlotsList.clear();
        }
#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
        printf("insert use for <%s %d> @ slot %d\n", string_type, regNum, op->slotId);
#endif
        userEntries[index_for_user].useSlotsList.push_back(op->slotId);
        if (type == UseDefType_Ctrl)
            ctrlEntries.clear();
    } else if (defuse == LowOpndDefUse_Def) {
        // insert WAR from earlier uses
        // insert WAW from earlier producer
        if (index_for_user != userEntries.size()) {
            for (unsigned int k2 = 0; k2 < userEntries[index_for_user].useSlotsList.size();
                    k2++) {
                if (userEntries[index_for_user].useSlotsList[k2] == op->slotId)
                    continue;
#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
                printf("insert WAR from %d to %d due to <%s %d>\n",
                        userEntries[index_for_user].useSlotsList[k2], op->slotId, string_type, regNum);
#endif
                DependencyInformation ds;
                ds.dataHazard = Dependency_WAR;
                ds.lowopSlotId = userEntries[index_for_user].useSlotsList[k2];
                ds.latency = EDGE_LATENCY;
                op->predecessorDependencies.push_back(ds);
            }
        }
        if (type != UseDefType_Ctrl
                && index_for_producer != producerEntries.size()) {
#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
            printf("insert WAW from %d to %d due to <%s %d>\n",
                    producerEntries[index_for_producer].producerSlot, op->slotId, string_type, regNum);
#endif
            DependencyInformation ds;
            ds.dataHazard = Dependency_WAW;
            ds.lowopSlotId = producerEntries[index_for_producer].producerSlot;
            ds.latency = EDGE_LATENCY;
            op->predecessorDependencies.push_back(ds);
        }
        // update producerTable
        // clear corresponding use slots for entry in userEntries
        if (type != UseDefType_Ctrl
                && index_for_producer == producerEntries.size()) {
            // insert an entry in producerEntries
            UseDefProducerEntry entry;
            entry.type = type;
            entry.regNum = regNum;
            producerEntries.push_back(entry);
        }
#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
        printf("insert def for <%s %d> @ slot %d\n", string_type, regNum, op->slotId);
#endif
        if (type != UseDefType_Ctrl)
            producerEntries[index_for_producer].producerSlot = op->slotId;
        else { // insert into ctrlEntries
            ctrlEntries.push_back(op->slotId);
        }
        if (type != UseDefType_Ctrl && index_for_user != userEntries.size()) {
            userEntries[index_for_user].useSlotsList.clear();
        }
    } else if (defuse == LowOpndDefUse_UseDef) {
        // type can not be Ctrl
        // insert RAW from producer
        // insert WAR from earlier user
        // insert WAW from earlier producer
        if (index_for_producer != producerEntries.size()) {
#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
            printf("insert RAW from %d to %d due to <%s %d>\n",
                    producerEntries[index_for_producer].producerSlot, op->slotId, string_type, regNum);
#endif
            DependencyInformation ds;
            ds.dataHazard = Dependency_RAW;
            ds.lowopSlotId = producerEntries[index_for_producer].producerSlot;
            ds.latency = EDGE_LATENCY;
            op->predecessorDependencies.push_back(ds);

#if 0 // inserting WAW is possibly redundant so section is commented out
#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
            printf("insert WAW from %d to %d due to <%s %d>",
                    producerEntries[index_for_producer].producerSlot, op->slotId, string_type, regNum);
#endif
            DependencyInformation ds_waw;
            ds_waw.dataHazard = Dependency_WAW;
            ds_waw.lowopSlotId =
                    producerEntries[index_for_producer].producerSlot;
            ds_waw.latency = EDGE_LATENCY;
            op->predecessorDependencies.push_back(ds_waw);
#endif


        }
        if (index_for_user != userEntries.size()) {
            for (unsigned int k2 = 0; k2 < userEntries[index_for_user].useSlotsList.size();
                    k2++) {
                if (userEntries[index_for_user].useSlotsList[k2] == op->slotId)
                    continue;
#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
                printf("insert WAR from %d to %d due to <%s %d>\n",
                        userEntries[index_for_user].useSlotsList[k2], op->slotId, string_type, regNum);
#endif
                DependencyInformation ds;
                ds.dataHazard = Dependency_WAR;
                ds.lowopSlotId = userEntries[index_for_user].useSlotsList[k2];
                ds.latency = EDGE_LATENCY;
                op->predecessorDependencies.push_back(ds);
            }
        }
        // update producerEntries
        // clear corresponding use slots for entry in userEntries
        if (index_for_producer == producerEntries.size()) {
            // insert an entry in producerEntries
            UseDefProducerEntry entry;
            entry.type = type;
            entry.regNum = regNum;
            producerEntries.push_back(entry);
        }
#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
        printf("insert def for <%s %d> @ slot %d\n", string_type, regNum, op->slotId);
#endif
        producerEntries[index_for_producer].producerSlot = op->slotId;
        if (index_for_user != userEntries.size()) {
            userEntries[index_for_user].useSlotsList.clear();
        }
    }
}

//! \brief Given an access to a memory location this updates dependency graph,
//! usedef information, and control flags.
//! \details This method uses updateDependencyGraph internally to update
//! dependency graph, but knows the type of memory resource that is being used.
//! \param mOpnd reference to the structure for memory operand
//! \param defuse definition, usage, or both
//! \param op LIR for which to update dependencies
void Scheduler::updateDependencyGraphForMem(LowOpndMem & mOpnd, LowOpndDefUse defuse,
        LowOp* op) {
    updateDependencyGraph(UseDefType_Reg, mOpnd.m_base.regNum, LowOpndDefUse_Use, op);
    if (mOpnd.hasScale)
        updateDependencyGraph(UseDefType_Reg, mOpnd.m_index.regNum, LowOpndDefUse_Use,
                op);
    MemoryAccessType mType = mOpnd.mType;
    int index = mOpnd.index;

    // be conservative, if one of the operands has size 64, assume it is size 64
    bool is64 = false;
    if (op->numOperands >= 1 && op->opndDest.size == OpndSize_64)
        is64 = true;
    if (op->numOperands >= 2 && op->opndSrc.size == OpndSize_64)
        is64 = true;

    if (mType == MemoryAccess_VR) {
        updateDependencyGraph(UseDefType_MemVR, index, defuse, op);
        if (is64)
            updateDependencyGraph(UseDefType_MemVR, index + 1, defuse, op);
    } else if (mType == MemoryAccess_SPILL) {
#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
        printf("UseDef MemSpill @%d slot %d\n", index, op->slotId);
#endif
        updateDependencyGraph(UseDefType_MemSpill, index, defuse, op);
        if (is64) {
#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
            printf("UseDef MemSpill2 @%d slot %d\n", index+4, op->slotId);
#endif
            updateDependencyGraph(UseDefType_MemSpill, index + 4, defuse, op);
        }
    } else
        updateDependencyGraph(UseDefType_MemUnknown, REG_NOT_USED, defuse, op);
}

//! \brief Updates dependency information for PUSH which uses then defines %esp
//! and also updates native stack.
void inline Scheduler::handlePushDependencyUpdate(LowOp* op) {
    if (op->opCode != Mnemonic_PUSH)
        return;
    updateDependencyGraph(UseDefType_Reg, PhysicalReg_ESP, LowOpndDefUse_UseDef, op);
    updateDependencyGraph(UseDefType_MemUnknown, REG_NOT_USED, LowOpndDefUse_Def, op);
}

//! \brief Updates dependency information for operations on floating point stack.
void inline Scheduler::handleFloatDependencyUpdate(LowOp* op) {
    updateDependencyGraph(UseDefType_Float, REG_NOT_USED, LowOpndDefUse_Def, op);
}

//! \brief Updates dependency information for LowOps with zero operands.
//! \param op has mnemonic RET
void Scheduler::updateUseDefInformation(LowOp * op) {
    op->instructionLatency = getAtomMnemonicLatency(op->opCode);
    op->portType = getAtomMnemonicPort(op->opCode);
#ifndef DISABLE_MACHINE_MODEL_CHECK
    if(op->instructionLatency == INVN || op->portType == INVP) {
        LOGE("ERROR: Atom Scheduler bug! Invalid Atom machine model "
                "entry for Mnemonic %d with zero operands.", op->opCode);
        dvmAbort();
    }
#endif
    updateDependencyGraph(UseDefType_Ctrl, REG_NOT_USED, LowOpndDefUse_Def, op);
    signalEndOfNativeBasicBlock(); // RET ends native basic block
}

//! \brief Updates dependency information for LowOps with a single immediate
//! operand.
//! \param op has mnemonic JMP, Jcc, or CALL
void Scheduler::updateUseDefInformation_imm(LowOp * op) {
    op->instructionLatency = getAtomMnemonicLatency_imm(op->opCode);
    op->portType = getAtomMnemonicPort_imm(op->opCode);
#ifndef DISABLE_MACHINE_MODEL_CHECK
    if(op->instructionLatency == INVN || op->portType == INVP) {
        LOGE("ERROR: Atom Scheduler bug! Invalid Atom machine model "
                "entry for Mnemonic %d with imm operand.", op->opCode);
        dvmAbort();
    }
#endif
    if (op->opCode == Mnemonic_CALL || op->opCode == Mnemonic_JMP)
        updateDependencyGraph(UseDefType_Ctrl, REG_NOT_USED, LowOpndDefUse_Def, op);
    else
        updateDependencyGraph(UseDefType_Ctrl, REG_NOT_USED, LowOpndDefUse_Use, op);
    if (isBasicBlockDelimiter(op->opCode))
        signalEndOfNativeBasicBlock();
#ifndef ENABLE_BASIC_BLOCK_DUMP
    signalEndOfNativeBasicBlock();
#endif
}

//! \brief Updates dependency information for LowOps with a single register operand.
//! \param op has mnemonic JMP, CALL, alu_unary_reg, PUSH or
//! alu_unary: neg, not, idiv, mul 32 bits operand
void Scheduler::updateUseDefInformation_reg(LowOpReg * op) {
    op->instructionLatency = getAtomMnemonicLatency_reg(op->opCode);
    op->portType = getAtomMnemonicPort_reg(op->opCode);
#ifndef DISABLE_MACHINE_MODEL_CHECK
    if(op->instructionLatency == INVN || op->portType == INVP) {
        LOGE("ERROR: Atom Scheduler bug! Invalid Atom machine model "
                "entry for Mnemonic %d with reg operand.", op->opCode);
        dvmAbort();
    }
#endif
    if (op->opCode == Mnemonic_CALL || op->opCode == Mnemonic_JMP
            || op->opCode == Mnemonic_PUSH)
        op->opndSrc.defuse = LowOpndDefUse_Use;
    if (op->opCode2 == ATOM_NORMAL_ALU)
        op->opndSrc.defuse = LowOpndDefUse_UseDef;
    // PUSH will not update control flag
    if (op->opCode != Mnemonic_PUSH)
        updateDependencyGraph(UseDefType_Ctrl, REG_NOT_USED, LowOpndDefUse_Def, op);
    updateDependencyGraph(UseDefType_Reg, op->regOpnd.regNum, op->opndSrc.defuse, op);
    Mnemonic m = op->opCode;
    if (m == Mnemonic_MUL || m == Mnemonic_IMUL || m == Mnemonic_DIV
            || m == Mnemonic_IDIV) {
        updateDependencyGraph(UseDefType_Reg, PhysicalReg_EAX, LowOpndDefUse_UseDef, op);
        updateDependencyGraph(UseDefType_Reg, PhysicalReg_EDX, LowOpndDefUse_UseDef, op);
    }
    handlePushDependencyUpdate(op);
    if (isBasicBlockDelimiter(op->opCode))
        signalEndOfNativeBasicBlock();
#ifndef ENABLE_BASIC_BLOCK_DUMP
    signalEndOfNativeBasicBlock();
#endif
}

//! \brief Updates dependency information for for LowOps with a single
//! memory operand.
//! \param op has mnemonic CALL, FLDCW, FNSTCW, alu_unary_mem, PUSH or
//! alu_unary: neg, not, idiv, mul 32 bits operand
void Scheduler::updateUseDefInformation_mem(LowOpMem * op) {
    op->instructionLatency = getAtomMnemonicLatency_mem(op->opCode);
    op->portType = getAtomMnemonicPort_mem(op->opCode);
#ifndef DISABLE_MACHINE_MODEL_CHECK
    if(op->instructionLatency == INVN || op->portType == INVP) {
        LOGE("ERROR: Atom Scheduler bug! Invalid Atom machine model "
                "entry for Mnemonic %d with mem operand.", op->opCode);
        dvmAbort();
    }
#endif
    // FLDCW: read memory location, affects FPU flags
    if (op->opCode == Mnemonic_CALL || op->opCode == Mnemonic_FLDCW
            || op->opCode == Mnemonic_PUSH)
        op->opndSrc.defuse = LowOpndDefUse_Use;
    if (op->opCode == Mnemonic_FNSTCW)
        op->opndSrc.defuse = LowOpndDefUse_Def;
    if (op->opCode2 == ATOM_NORMAL_ALU)
        op->opndSrc.defuse = LowOpndDefUse_UseDef;
    // PUSH will not update control flag
    if (op->opCode != Mnemonic_PUSH)
        updateDependencyGraph(UseDefType_Ctrl, REG_NOT_USED, LowOpndDefUse_Def, op);
    updateDependencyGraphForMem(op->memOpnd, op->opndSrc.defuse, op);
    handlePushDependencyUpdate(op);
    if (op->opCode == Mnemonic_FLDCW || op->opCode == Mnemonic_FNSTCW)
        handleFloatDependencyUpdate(op);
    if (isBasicBlockDelimiter(op->opCode))
        signalEndOfNativeBasicBlock();
#ifndef ENABLE_BASIC_BLOCK_DUMP
    signalEndOfNativeBasicBlock();
#endif
}

//! \brief Updates dependency information for LowOps with two operands:
//! immediate to register
//! \param op has mnemonic CMP, TEST, MOVQ, MOV, MOVSS, MOVSD, alu_binary,
//! COMISS, or COMISD
void Scheduler::updateUseDefInformation_imm_to_reg(LowOpImmReg * op) {
    bool isMove = isMoveMnemonic(op->opCode);
    op->instructionLatency = getAtomMnemonicLatency_imm_to_reg(op->opCode);
    op->portType = getAtomMnemonicPort_imm_to_reg(op->opCode);
#ifndef DISABLE_MACHINE_MODEL_CHECK
    if(op->instructionLatency == INVN || op->portType == INVP) {
        LOGE("ERROR: Atom Scheduler bug! Invalid Atom machine model "
                "entry for Mnemonic %d with imm to reg.", op->opCode);
        dvmAbort();
    }
#endif
    if (usesAndDefinesFlags(op->opCode))
        updateDependencyGraph(UseDefType_Ctrl, REG_NOT_USED, LowOpndDefUse_Use,
                op);
    if (!isMove)
        updateDependencyGraph(UseDefType_Ctrl, REG_NOT_USED, LowOpndDefUse_Def, op);

    if (isMove)
        op->opndDest.defuse = LowOpndDefUse_Def;
    else if (op->opCode2 == ATOM_NORMAL_ALU)
        op->opndDest.defuse = LowOpndDefUse_UseDef;
    else
        op->opndDest.defuse = LowOpndDefUse_Use;
    updateDependencyGraph(UseDefType_Reg, op->regDest.regNum, op->opndDest.defuse, op);
#ifndef ENABLE_BASIC_BLOCK_DUMP
    signalEndOfNativeBasicBlock();
#endif
}

//! \brief Updates dependency information for LowOps with two operands:
//! immediate and memory.
//! \param op has mnemonic MOV, MOVQ, CMP, TEST, alu_binary
void Scheduler::updateUseDefInformation_imm_to_mem(LowOpImmMem * op) {
    bool isMove = isMoveMnemonic(op->opCode);
    op->instructionLatency = getAtomMnemonicLatency_imm_to_mem(op->opCode);
    op->portType = getAtomMnemonicPort_imm_to_mem(op->opCode);
#ifndef DISABLE_MACHINE_MODEL_CHECK
    if(op->instructionLatency == INVN || op->portType == INVP) {
        LOGE("ERROR: Atom Scheduler bug! Invalid Atom machine model "
                "entry for Mnemonic %d with imm to mem.", op->opCode);
        dvmAbort();
    }
#endif
    if (usesAndDefinesFlags(op->opCode))
        updateDependencyGraph(UseDefType_Ctrl, REG_NOT_USED, LowOpndDefUse_Use,
                op);
    if (!isMove)
        updateDependencyGraph(UseDefType_Ctrl, REG_NOT_USED, LowOpndDefUse_Def, op);

    if (isMove)
        op->opndDest.defuse = LowOpndDefUse_Def;
    else if (op->opCode2 == ATOM_NORMAL_ALU)
        op->opndDest.defuse = LowOpndDefUse_UseDef;
    else
        op->opndDest.defuse = LowOpndDefUse_Use;
    updateDependencyGraphForMem(op->memDest, op->opndDest.defuse, op);
#ifndef ENABLE_BASIC_BLOCK_DUMP
    signalEndOfNativeBasicBlock();
#endif
}

//! \brief Updates dependency information for LowOps with two operands:
//! register to register.
//! \param op has mnemonic TEST, CDQ, CMP, COMISS, COMISD,
//! MOVQ, MOV, CMOVcc, MOVSS, MOVSD, FUCOM, FUCOMP, or
//! alu_sd_binary, alu_ss_binary, alu_binary,
void Scheduler::updateUseDefInformation_reg_to_reg(LowOpRegReg * op) {
    Mnemonic m = op->opCode;
    bool isMove = isMoveMnemonic(m);
    op->instructionLatency = getAtomMnemonicLatency_reg_to_reg(op->opCode);
    op->portType = getAtomMnemonicPort_reg_to_reg(op->opCode);
#ifndef DISABLE_MACHINE_MODEL_CHECK
    if(op->instructionLatency == INVN || op->portType == INVP) {
        LOGE("ERROR: Atom Scheduler bug! Invalid Atom machine model "
                "entry for Mnemonic %d with reg to reg.", op->opCode);
        dvmAbort();
    }
#endif
    if ((m >= Mnemonic_CMOVcc && m < Mnemonic_CMP)
            || usesAndDefinesFlags(op->opCode))
        updateDependencyGraph(UseDefType_Ctrl, REG_NOT_USED, LowOpndDefUse_Use,
                op);
    else if (!isMove)
        updateDependencyGraph(UseDefType_Ctrl, REG_NOT_USED, LowOpndDefUse_Def, op);

    // CDQ: use & def eax, def edx
    if (m == Mnemonic_CDQ)
        op->opndSrc.defuse = LowOpndDefUse_UseDef; //use & def eax
    else
        op->opndSrc.defuse = LowOpndDefUse_Use;
    updateDependencyGraph(UseDefType_Reg, op->regSrc.regNum, op->opndSrc.defuse, op);

    if (isMove || (m >= Mnemonic_CMOVcc && m < Mnemonic_CMP)
            || m == Mnemonic_CDQ)
        op->opndDest.defuse = LowOpndDefUse_Def;
    else if (op->opCode2 == ATOM_NORMAL_ALU)
        op->opndDest.defuse = LowOpndDefUse_UseDef;
    else
        op->opndDest.defuse = LowOpndDefUse_Use;
    updateDependencyGraph(UseDefType_Reg, op->regDest.regNum, op->opndDest.defuse, op);
    if (m == Mnemonic_FUCOM || m == Mnemonic_FUCOMP)
        handleFloatDependencyUpdate(op);
#ifndef ENABLE_BASIC_BLOCK_DUMP
    signalEndOfNativeBasicBlock();
#endif
}

//! \brief Updates dependency information for LowOps with two operands:
//! memory to register.
//! \param op has mnemonic LEA, CMP, COMISS, COMISD, MOVQ, MOV, MOVSS,
//! MOVSD, alu_binary, or alu_sd_binary
void Scheduler::updateUseDefInformation_mem_to_reg(LowOpMemReg * op) {
    Mnemonic m = op->opCode;
    bool isMove = isMoveMnemonic(m);
    op->instructionLatency = getAtomMnemonicLatency_mem_to_reg(op->opCode);
    op->portType = getAtomMnemonicPort_mem_to_reg(op->opCode);
#ifndef DISABLE_MACHINE_MODEL_CHECK
    if(op->instructionLatency == INVN || op->portType == INVP) {
        LOGE("ERROR: Atom Scheduler bug! Invalid Atom machine model "
                "entry for Mnemonic %d with mem to reg.", op->opCode);
        dvmAbort();
    }
#endif

    if (usesAndDefinesFlags(op->opCode))
        updateDependencyGraph(UseDefType_Ctrl, REG_NOT_USED, LowOpndDefUse_Use,
                op);
    if (!isMove && m != Mnemonic_LEA)
        updateDependencyGraph(UseDefType_Ctrl, REG_NOT_USED, LowOpndDefUse_Def, op);

    // Read from memory
    // However, LEA does not load from memory, and instead it uses the register
    op->opndSrc.defuse = LowOpndDefUse_Use;
    if (m != Mnemonic_LEA)
        updateDependencyGraphForMem(op->memSrc, op->opndSrc.defuse, op);
    else {
        updateDependencyGraph(UseDefType_Reg, op->memSrc.m_base.regNum,
                op->opndSrc.defuse, op);
    }
    if (isMove || m == Mnemonic_LEA)
        op->opndDest.defuse = LowOpndDefUse_Def;
    else if (op->opCode2 == ATOM_NORMAL_ALU)
        op->opndDest.defuse = LowOpndDefUse_UseDef;
    else
        op->opndDest.defuse = LowOpndDefUse_Use;
    updateDependencyGraph(UseDefType_Reg, op->regDest.regNum, op->opndDest.defuse, op);
#ifndef ENABLE_BASIC_BLOCK_DUMP
    signalEndOfNativeBasicBlock();
#endif
}

//! \brief Updates dependency information for LowOps with two operands:
//! register to memory.
//! \param op has mnemonic MOVQ, MOV, MOVSS, MOVSD, CMP, or alu_binary
void Scheduler::updateUseDefInformation_reg_to_mem(LowOpRegMem * op) {
    bool isMove = isMoveMnemonic(op->opCode);
    op->instructionLatency = getAtomMnemonicLatency_reg_to_mem(op->opCode);
    op->portType = getAtomMnemonicPort_reg_to_mem(op->opCode);
#ifndef DISABLE_MACHINE_MODEL_CHECK
    if(op->instructionLatency == INVN || op->portType == INVP) {
        LOGE("ERROR: Atom Scheduler bug! Invalid Atom machine model "
                "entry for Mnemonic %d with reg to mem.", op->opCode);
        dvmAbort();
    }
#endif
    if (usesAndDefinesFlags(op->opCode))
        updateDependencyGraph(UseDefType_Ctrl, REG_NOT_USED, LowOpndDefUse_Use,
                op);
    if (!isMove)
        updateDependencyGraph(UseDefType_Ctrl, REG_NOT_USED, LowOpndDefUse_Def, op);

    op->opndSrc.defuse = LowOpndDefUse_Use;
    updateDependencyGraph(UseDefType_Reg, op->regSrc.regNum, op->opndSrc.defuse, op);
    if (isMove)
        op->opndDest.defuse = LowOpndDefUse_Def;
    else if (op->opCode2 == ATOM_NORMAL_ALU)
        op->opndDest.defuse = LowOpndDefUse_UseDef;
    else
        op->opndDest.defuse = LowOpndDefUse_Use;
    updateDependencyGraphForMem(op->memDest, op->opndDest.defuse, op);
#ifndef ENABLE_BASIC_BLOCK_DUMP
    signalEndOfNativeBasicBlock();
#endif
}

//! \brief Updates dependency information for LowOps with two operands:
//! memory with scale to register.
//! \param op has mnemonic LEA, MOVQ, MOV, MOVSX, or MOVZX
void Scheduler::updateUseDefInformation_mem_scale_to_reg(LowOpMemReg * op) {
    op->instructionLatency = getAtomMnemonicLatency_mem_to_reg(op->opCode);
    op->portType = getAtomMnemonicPort_mem_to_reg(op->opCode);
#ifndef DISABLE_MACHINE_MODEL_CHECK
    if(op->instructionLatency == INVN || op->portType == INVP) {
        LOGE("ERROR: Atom Scheduler bug! Invalid Atom machine model "
                "entry for Mnemonic %d with mem scale to reg.", op->opCode);
        dvmAbort();
    }
#endif
    op->opndSrc.defuse = LowOpndDefUse_Use;
    Mnemonic m = op->opCode;
    if (m != Mnemonic_LEA)
        updateDependencyGraphForMem(op->memSrc, op->opndSrc.defuse, op);
    else {
        updateDependencyGraph(UseDefType_Reg, op->memSrc.m_base.regNum,
                op->opndSrc.defuse, op);
    }
    op->opndDest.defuse = LowOpndDefUse_Def;
    updateDependencyGraph(UseDefType_Reg, op->regDest.regNum, op->opndDest.defuse, op);
#ifndef ENABLE_BASIC_BLOCK_DUMP
    signalEndOfNativeBasicBlock();
#endif
}

//! \brief Updates dependency information for LowOps with two operands:
//! register to memory with scale.
//! \param op has mnemonic MOVQ or MOV
void Scheduler::updateUseDefInformation_reg_to_mem_scale(LowOpRegMem * op) {
    op->instructionLatency = getAtomMnemonicLatency_reg_to_mem(op->opCode);
    op->portType = getAtomMnemonicPort_reg_to_mem(op->opCode);
#ifndef DISABLE_MACHINE_MODEL_CHECK
    if(op->instructionLatency == INVN || op->portType == INVP) {
        LOGE("ERROR: Atom Scheduler bug! Invalid Atom machine model "
                "entry for Mnemonic %d with reg to mem scale.", op->opCode);
        dvmAbort();
    }
#endif
    op->opndSrc.defuse = LowOpndDefUse_Use;
    updateDependencyGraph(UseDefType_Reg, op->regSrc.regNum, op->opndSrc.defuse, op);
    op->opndDest.defuse = LowOpndDefUse_Def;
    updateDependencyGraphForMem(op->memDest, op->opndDest.defuse, op);
#ifndef ENABLE_BASIC_BLOCK_DUMP
    signalEndOfNativeBasicBlock();
#endif
}

//! \brief Updates dependency information for LowOps with two operands:
//! floating point stack to memory.
//! \param op has mnemonic FSTP, FST, FISTP, or FIST (write to memory)
void Scheduler::updateUseDefInformation_fp_to_mem(LowOpRegMem * op) {
    op->instructionLatency = getAtomMnemonicLatency_reg_to_mem(op->opCode);
    op->portType = getAtomMnemonicPort_reg_to_mem(op->opCode);
#ifndef DISABLE_MACHINE_MODEL_CHECK
    if(op->instructionLatency == INVN || op->portType == INVP) {
        LOGE("ERROR: Atom Scheduler bug! Invalid Atom machine model "
                "entry for Mnemonic %d with fp to mem.", op->opCode);
        dvmAbort();
    }
#endif
    updateDependencyGraph(UseDefType_Ctrl, REG_NOT_USED, LowOpndDefUse_Def, op);
    op->opndSrc.defuse = LowOpndDefUse_Use;
    updateDependencyGraph(UseDefType_Reg, PhysicalReg_ST0, op->opndSrc.defuse, op);
    op->opndDest.defuse = LowOpndDefUse_Def;
    updateDependencyGraphForMem(op->memDest, op->opndDest.defuse, op);
    handleFloatDependencyUpdate(op);
#ifndef ENABLE_BASIC_BLOCK_DUMP
    signalEndOfNativeBasicBlock();
#endif
}

//! \brief Updates dependency information for LowOps with two operands:
//! memory to floating point stack.
//! \param op has mnemonic FLD or FILD
void Scheduler::updateUseDefInformation_mem_to_fp(LowOpMemReg * op) {
    op->instructionLatency = getAtomMnemonicLatency_mem_to_reg(op->opCode);
    op->portType = getAtomMnemonicPort_mem_to_reg(op->opCode);
#ifndef DISABLE_MACHINE_MODEL_CHECK
    if(op->instructionLatency == INVN || op->portType == INVP) {
        LOGE("ERROR: Atom Scheduler bug! Invalid Atom machine model "
                "entry for Mnemonic %d with mem to fp.", op->opCode);
        dvmAbort();
    }
#endif
    updateDependencyGraph(UseDefType_Ctrl, REG_NOT_USED, LowOpndDefUse_Def, op);
    op->opndSrc.defuse = LowOpndDefUse_Use;
    updateDependencyGraphForMem(op->memSrc, op->opndSrc.defuse, op);
    if (op->opCode2 == ATOM_NORMAL_ALU)
        op->opndDest.defuse = LowOpndDefUse_UseDef;
    else
        op->opndDest.defuse = LowOpndDefUse_Def;
    updateDependencyGraph(UseDefType_Reg, PhysicalReg_ST0, op->opndDest.defuse, op);
    handleFloatDependencyUpdate(op);
#ifndef ENABLE_BASIC_BLOCK_DUMP
    signalEndOfNativeBasicBlock();
#endif
}

//! \brief Generates IA native code for given LowOp
//! \details This method takes a LowOp and generates x86 instructions into the
//! code stream by making calls to the encoder.
//! \param op to be encoded and placed into code stream.
void Scheduler::generateAssembly(LowOp * op) {
    if (op->numOperands == 0) {
        stream = encoder_return(stream);
    } else if (op->numOperands == 1) {
        if (op->opndSrc.type == LowOpndType_Label) {
            bool unknown;
            OpndSize size;
            int imm;
            if (op->opCode == Mnemonic_JMP)
                imm = getRelativeOffset(((LowOpLabel*) op)->labelOpnd.label,
                        ((LowOpLabel*) op)->labelOpnd.isLocal, JmpCall_uncond,
                        &unknown, &size);
            else if (op->opCode == Mnemonic_CALL)
                imm = getRelativeOffset(((LowOpLabel*) op)->labelOpnd.label,
                        ((LowOpLabel*) op)->labelOpnd.isLocal, JmpCall_call,
                        &unknown, &size);
            else
                imm = getRelativeOffset(((LowOpLabel*) op)->labelOpnd.label,
                        ((LowOpLabel*) op)->labelOpnd.isLocal, JmpCall_cond,
                        &unknown, &size);
            op->opndSrc.size = size;
            stream = encoder_imm(op->opCode, op->opndSrc.size, imm, stream);
        } else if (op->opndSrc.type == LowOpndType_BlockId) {
            bool unknown;
            OpndSize size;
            int imm;
            if (op->opCode == Mnemonic_JMP)
                imm = getRelativeNCG(((LowOpBlock*) op)->blockIdOpnd.value,
                        JmpCall_uncond, &unknown, &size);
            else
                imm = getRelativeNCG(((LowOpBlock*) op)->blockIdOpnd.value,
                        JmpCall_cond, &unknown, &size);
            op->opndSrc.size = size;
            stream = encoder_imm(op->opCode, op->opndSrc.size, imm, stream);
        } else if (op->opndSrc.type == LowOpndType_Imm) {
            stream = encoder_imm(op->opCode, op->opndSrc.size,
                    ((LowOpImm*) op)->immOpnd.value, stream);
        } else if (op->opndSrc.type == LowOpndType_Reg) {
            stream = encoder_reg(op->opCode, op->opndSrc.size,
                    ((LowOpReg*) op)->regOpnd.regNum,
                    ((LowOpReg*) op)->regOpnd.isPhysical,
                    ((LowOpReg*) op)->regOpnd.regType, stream);
        } else { // Corresponds to lower_mem
            stream = encoder_mem(op->opCode, op->opndSrc.size,
                    ((LowOpMem*) op)->memOpnd.m_disp.value,
                    ((LowOpMem*) op)->memOpnd.m_base.regNum,
                    ((LowOpMem*) op)->memOpnd.m_base.isPhysical, stream);
        }
    }
    // Number of operands is 2
    // Handles LowOps coming from  lower_imm_reg, lower_imm_mem,
    // lower_reg_mem, lower_mem_reg, lower_mem_scale_reg,
    // lower_reg_mem_scale, lower_reg_reg, lower_fp_mem, and lower_mem_fp
    else if (op->opndDest.type == LowOpndType_Reg
            && op->opndSrc.type == LowOpndType_Imm) {
        stream = encoder_imm_reg(op->opCode, op->opndDest.size,
                ((LowOpImmReg*) op)->immSrc.value,
                ((LowOpImmReg*) op)->regDest.regNum,
                ((LowOpImmReg*) op)->regDest.isPhysical,
                ((LowOpImmReg*) op)->regDest.regType, stream);
    } else if (op->opndDest.type == LowOpndType_Reg
            && op->opndSrc.type == LowOpndType_Chain) {
#if defined(WITH_JIT)
        insertChainingWorklist(((LowOpImmReg*) op)->immSrc.value, stream);
#endif
        stream = encoder_imm_reg(op->opCode, op->opndDest.size,
                ((LowOpImmReg*) op)->immSrc.value,
                ((LowOpImmReg*) op)->regDest.regNum,
                ((LowOpImmReg*) op)->regDest.isPhysical,
                ((LowOpImmReg*) op)->regDest.regType, stream);
    } else if (op->opndDest.type == LowOpndType_Mem
            && op->opndSrc.type == LowOpndType_Imm) {
        stream = encoder_imm_mem(op->opCode, op->opndDest.size,
                ((LowOpImmMem*) op)->immSrc.value,
                ((LowOpImmMem*) op)->memDest.m_disp.value,
                ((LowOpImmMem*) op)->memDest.m_base.regNum,
                ((LowOpImmMem*) op)->memDest.m_base.isPhysical, stream);
    } else if (op->opndDest.type == LowOpndType_Mem
            && op->opndSrc.type == LowOpndType_Chain) {
#if defined(WITH_JIT)
        insertChainingWorklist(((LowOpImmMem*) op)->immSrc.value, stream);
#endif
        stream = encoder_imm_mem(op->opCode, op->opndDest.size,
                ((LowOpImmMem*) op)->immSrc.value,
                ((LowOpImmMem*) op)->memDest.m_disp.value,
                ((LowOpImmMem*) op)->memDest.m_base.regNum,
                ((LowOpImmMem*) op)->memDest.m_base.isPhysical, stream);
    } else if (op->opndDest.type == LowOpndType_Reg
            && op->opndSrc.type == LowOpndType_Reg) {
        if (op->opCode == Mnemonic_FUCOMP || op->opCode == Mnemonic_FUCOM) {
            stream = encoder_compare_fp_stack(op->opCode == Mnemonic_FUCOMP,
                    ((LowOpRegReg*) op)->regSrc.regNum
                            - ((LowOpRegReg*) op)->regDest.regNum,
                    op->opndDest.size == OpndSize_64, stream);
        } else {
            stream = encoder_reg_reg(op->opCode, op->opndDest.size,
                    ((LowOpRegReg*) op)->regSrc.regNum,
                    ((LowOpRegReg*) op)->regSrc.isPhysical,
                    ((LowOpRegReg*) op)->regDest.regNum,
                    ((LowOpRegReg*) op)->regDest.isPhysical,
                    ((LowOpRegReg*) op)->regDest.regType, stream);
        }
    } else if (op->opndDest.type == LowOpndType_Reg
            && op->opndSrc.type == LowOpndType_Mem) {
        // Corresponds to lower_mem_reg, lower_mem_fp, or lower_mem_scale_reg
        LowOpMemReg * regmem_op = (LowOpMemReg*) op;

        if (regmem_op->regDest.regType == LowOpndRegType_fs)
            stream = encoder_mem_fp(regmem_op->opCode, regmem_op->opndSrc.size,
                    regmem_op->memSrc.m_disp.value,
                    regmem_op->memSrc.m_base.regNum,
                    regmem_op->memSrc.m_base.isPhysical,
                    regmem_op->regDest.regNum - PhysicalReg_ST0, stream);
        else if (regmem_op->memSrc.hasScale)
            stream = encoder_mem_disp_scale_to_reg_2(regmem_op->opCode,
                    regmem_op->opndSrc.size, regmem_op->memSrc.m_base.regNum,
                    regmem_op->memSrc.m_base.isPhysical,
                    regmem_op->memSrc.m_disp.value,
                    regmem_op->memSrc.m_index.regNum,
                    regmem_op->memSrc.m_index.isPhysical,
                    regmem_op->memSrc.m_scale.value, regmem_op->opndDest.size,
                    regmem_op->regDest.regNum, regmem_op->regDest.isPhysical,
                    regmem_op->regDest.regType, stream);
        else
            stream = encoder_mem_to_reg_2(regmem_op->opCode,
                    regmem_op->opndSrc.size, regmem_op->memSrc.m_disp.value,
                    regmem_op->memSrc.m_base.regNum,
                    regmem_op->memSrc.m_base.isPhysical,
                    regmem_op->opndDest.size, regmem_op->regDest.regNum,
                    regmem_op->regDest.isPhysical, regmem_op->regDest.regType,
                    stream);
    } else if (op->opndDest.type == LowOpndType_Mem
            && op->opndSrc.type == LowOpndType_Reg) {
        // Corresponds to lower_reg_mem, lower_fp_mem, or lower_reg_mem_scale
        LowOpRegMem * memreg_op = (LowOpRegMem*) op;
        if (memreg_op->regSrc.regType == LowOpndRegType_fs)
            stream = encoder_fp_mem(memreg_op->opCode, memreg_op->opndDest.size,
                    memreg_op->regSrc.regNum - PhysicalReg_ST0,
                    memreg_op->memDest.m_disp.value,
                    memreg_op->memDest.m_base.regNum,
                    memreg_op->memDest.m_base.isPhysical, stream);
        else if (memreg_op->memDest.hasScale)
            stream = encoder_reg_mem_disp_scale(memreg_op->opCode,
                    memreg_op->opndDest.size, memreg_op->regSrc.regNum,
                    memreg_op->regSrc.isPhysical,
                    memreg_op->memDest.m_base.regNum,
                    memreg_op->memDest.m_base.isPhysical,
                    memreg_op->memDest.m_disp.value,
                    memreg_op->memDest.m_index.regNum,
                    memreg_op->memDest.m_index.isPhysical,
                    memreg_op->memDest.m_scale.value,
                    memreg_op->regSrc.regType, stream);
        else
            stream = encoder_reg_mem(op->opCode, op->opndDest.size,
                    memreg_op->regSrc.regNum, memreg_op->regSrc.isPhysical,
                    memreg_op->memDest.m_disp.value,
                    memreg_op->memDest.m_base.regNum,
                    memreg_op->memDest.m_base.isPhysical,
                    memreg_op->regSrc.regType, stream);
    }
}

//! \brief Figures out which LowOps are ready after an instruction at chosenIdx
//! is scheduled.
//! \details It also updates the readyTime of every LowOp waiting to be scheduled.
//! \param chosenIdx is the index of the chosen instruction for scheduling
//! \param scheduledOps is an input list of scheduled LowOps
//! \param pendingOps is an input list of pending LowOps
//! \param readyOps is an output list of LowOps that are ready
void Scheduler::updateReadyOps(int chosenIdx, bool* scheduledOps,
        bool* pendingOps, bool* readyOps) {
    // Go through each successor LIR that depends on selected LIR
    for (unsigned int k = 0; k < queuedLIREntries[chosenIdx]->successorDependencies.size(); ++k) {
        int dst = queuedLIREntries[chosenIdx]->successorDependencies[k].lowopSlotId;
        bool isReady = true;
        int readyTime = -1;
        // If all predecessors are scheduled, insert into ready queue
        for (unsigned int k2 = 0; k2 < queuedLIREntries[dst]->predecessorDependencies.size(); ++k2) {
            int src = queuedLIREntries[dst]->predecessorDependencies[k2].lowopSlotId;
            if (!scheduledOps[src]) {
                isReady = false;
                break;
            }
            int tmpTime = queuedLIREntries[src]->scheduledTime
                    + queuedLIREntries[src]->instructionLatency;
            if (readyTime < tmpTime)
                // This is ready after ALL predecessors have finished executing
                readyTime = tmpTime;
        }
        if (isReady) {
            readyOps[dst] = true; // Update entry in ready queue
            // All already pending instructions should commit ahead of this LowOp
            for (unsigned int k2 = 0; k2 < queuedLIREntries.size(); ++k2) {
                if (pendingOps[k2]) {
                    int tmpTime = queuedLIREntries[k2]->scheduledTime
                            + queuedLIREntries[k2]->instructionLatency
                            - queuedLIREntries[dst]->instructionLatency; //TODO Is this correct?
                    if (readyTime < tmpTime) {
#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
                        printf("update readyTime from %d to %d of slot %d due to pending "
                                "op at slot %d\n", readyTime, tmpTime, dst, k2);
#endif
                        readyTime = tmpTime;
                    }
                }
            }
            queuedLIREntries[dst]->readyTime = readyTime;
#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
            printf("update readyTime of slot %d: %d\n", dst, readyTime);
#endif
        }
    }
}

//! \brief Defines the maximum latency in the dependency graph.
//! \details This value should be larger than any possible valid dependency latency.
#define MAXIMUM_LATENCY 10000

//! \brief Finds longest path latency to end for every LowOp in the basic block
//! \details This updates the longest path field of every LowOp in the current
//! native basic block.
//! \see queuedLIREntries
//! \todo Find out what algorithm is used here.
void Scheduler::findLongestPath() {
#if 1 // Algorithm 1
    std::vector<int> options1, options2;
    for (unsigned int k = 0; k < queuedLIREntries.size() - 1; ++k)
        options1.push_back(MAXIMUM_LATENCY);
    //destination is the last LowOp
    options1.push_back(-queuedLIREntries.back()->instructionLatency); //from dst to dst
    options2.resize(options1.size());

    bool inputIn1 = true;
    for (unsigned int i = 0; i < queuedLIREntries.size() - 1; ++i) { //number of arcs in shortest path
        for (unsigned int k = 0; k < queuedLIREntries.size(); ++k) { //for each v in V
            int m = inputIn1 ? options1[k] : options2[k];
            int m_prime = MAXIMUM_LATENCY;
            for (unsigned int k2 = 0; k2 < queuedLIREntries[k]->successorDependencies.size();
                    ++k2) { //for each edge from v
                int dst = queuedLIREntries[k]->successorDependencies[k2].lowopSlotId;
                int m_tmp = (inputIn1 ? options1[dst] : options2[dst])
                        + (-queuedLIREntries[k]->instructionLatency)
                        + queuedLIREntries[k]->successorDependencies[k2].latency;
                m_prime = (m_tmp < m_prime) ? m_tmp : m_prime;
            }
            if (inputIn1)
                options2[k] = (m_prime < m) ? m_prime : m;
            else
                options1[k] = (m_prime < m) ? m_prime : m;
        }
        inputIn1 = !inputIn1;
    }
    for (unsigned int k = 0; k < queuedLIREntries.size(); ++k) {
        int m = inputIn1 ? options1[k] : options2[k];
        if (m == MAXIMUM_LATENCY)
            m = queuedLIREntries[k]->instructionLatency;
        else
            m = -m;
        queuedLIREntries[k]->longestPath = m;
#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
        printf("longest path from %d to %d: %d\n", k, queuedLIREntries.size() - 1, m);
#endif
    }
#else // Algorithm 2

#endif
}

//! \brief Reorders basic block to minimize block latency and make use of both
//! Atom issue ports.
//! \details The result of scheduling is stored in scheduledLIREntries. Additionally,
//! each LowOp individually stores its scheduled time (logical based on index ordering).
//!
//! Algorithm details:
//! - select one LIR from readyQueue with 2 criteria:
//!   -# smallest readyTime
//!   -# on critical path
//! - A pair of LowOps can be issued at the same time slot if they use different issue ports.
//! - A LowOp can be issued if
//!   -# all pending ops can commit ahead of this LowOp (restriction reflected in readyTime)
//!   -# it is ready
//! - At end, currentTime is advanced to readyTime of the selected LowOps
//! - If any LIR has jmp, jcc, call, or ret mnemonic, it must be scheduled last
//!
//! \see scheduledLIREntries
//! \post Scheduler::scheduledLIREntries is same size as Scheduler::queuedLIREntries
//! \post If last LIR in Scheduler::queuedLIREntries is a jump, call, or return, it must
//! also be the last LIR in Scheduler::scheduledLIREntries
void Scheduler::schedule() {
    // Initialize data structures for scheduling
    bool* readyOps = (bool *) malloc(sizeof(bool) * (queuedLIREntries.size())); // ready for scheduling
    bool* scheduledOps = (bool *) malloc(
            sizeof(bool) * (queuedLIREntries.size())); // scheduled
    bool* pendingOps = (bool *) malloc(
            sizeof(bool) * (queuedLIREntries.size())); // scheduled but not yet finished executing
    unsigned int* candidateArray = (unsigned int *) malloc(
            sizeof(unsigned int) * queuedLIREntries.size());
    unsigned int num_candidates = 0, numScheduled = 0, k;
    int currentTime = 0;

    // Predecessor dependencies have already been initialized in the dependency graph building.
    // Now, initialize successor dependencies to complete dependency graph.
    for (unsigned int k = 0; k < queuedLIREntries.size(); ++k) {
        for (unsigned int k2 = 0; k2 < queuedLIREntries[k]->predecessorDependencies.size(); ++k2) {
            int src = queuedLIREntries[k]->predecessorDependencies[k2].lowopSlotId;
            DependencyInformation ds;
            ds.lowopSlotId = k;
            ds.latency = -queuedLIREntries[k]->predecessorDependencies[k2].latency; //negative weight
            queuedLIREntries[src]->successorDependencies.push_back(ds);
        }
    }

    // Find longest path from each LIR to end of basic block
    findLongestPath();

    // When a LowOp is ready, it means all its predecessors are scheduled
    // and the readyTime of this LowOp has been set already.
    for (k = 0; k < queuedLIREntries.size(); k++) {
#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
        printf("-- slot %d: latency %d port type %d\n", k, queuedLIREntries[k]->instructionLatency,
                queuedLIREntries[k]->portType);
#endif
        scheduledOps[k] = false;
        readyOps[k] = false;
        pendingOps[k] = false;
        if (queuedLIREntries[k]->predecessorDependencies.size() == 0) {
            readyOps[k] = true;
#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
            printf("slot %d is ready\n", k);
#endif
            queuedLIREntries[k]->readyTime = 0;
        }
    }

    // Schedule each of LIRs in the basic block
    while (numScheduled < queuedLIREntries.size()) {
        int nextTime = currentTime;
        unsigned int chosenIdx1 = queuedLIREntries.size(), chosenIdx2 =
                queuedLIREntries.size(), idx;
        num_candidates = 0;
        // candidates: ops with readyTime <= currentTime
        // if no ops are ready to be issued, ops with smallest readyTime
        for (k = 0; k < queuedLIREntries.size(); k++) {
            if (readyOps[k] && queuedLIREntries[k]->readyTime <= currentTime
                    && (!isBasicBlockDelimiter(queuedLIREntries[k]->opCode)))
                candidateArray[num_candidates++] = k;
        }
        if (num_candidates == 0) {
            for (k = 0; k < queuedLIREntries.size(); k++) {
                if (readyOps[k]
                        && (!isBasicBlockDelimiter(queuedLIREntries[k]->opCode))) {
                    if (chosenIdx1 == queuedLIREntries.size()
                            || queuedLIREntries[k]->readyTime
                                    < queuedLIREntries[chosenIdx1]->readyTime) {
                        chosenIdx1 = k;
                        nextTime = queuedLIREntries[k]->readyTime;
                    }
                }
            }
            for (k = 0; k < queuedLIREntries.size(); k++) {
                if (readyOps[k] && (!isBasicBlockDelimiter(queuedLIREntries[k]->opCode))
                        && queuedLIREntries[k]->readyTime <= nextTime) {
                    candidateArray[num_candidates++] = k;
                }
            }
        }
        if (num_candidates == 0)
            candidateArray[num_candidates++] = queuedLIREntries.size() - 1;
        chosenIdx1 = queuedLIREntries.size();
        for (idx = 0; idx < num_candidates; idx++) {
            k = candidateArray[idx];
            if (chosenIdx1 == queuedLIREntries.size()
                    || queuedLIREntries[k]->longestPath
                            > queuedLIREntries[chosenIdx1]->longestPath) {
                chosenIdx1 = k;
            }
        }
        // Pick 2 candidates if possible.
        if (queuedLIREntries[chosenIdx1]->portType == BOTH_PORTS)
            num_candidates = 0;
        for (idx = 0; idx < num_candidates; idx++) {
            k = candidateArray[idx];
            if (k == chosenIdx1)
                continue;
            // Check whether candidateArray[k] can be issued at the same time.
            // Namely, check port conflict
            if (queuedLIREntries[k]->portType == BOTH_PORTS)
                continue;
            if (queuedLIREntries[chosenIdx1]->portType == EITHER_PORT
                    || queuedLIREntries[k]->portType == EITHER_PORT
                    || (queuedLIREntries[chosenIdx1]->portType == PORT0
                            && queuedLIREntries[k]->portType == PORT1)
                    || (queuedLIREntries[chosenIdx1]->portType == PORT1
                            && queuedLIREntries[k]->portType == PORT0)) {
                chosenIdx2 = k;
                break;
            }
        }
#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
        printf("pick ready instructions at slots %d %d\n", chosenIdx1, chosenIdx2);
#endif
        currentTime = nextTime;
#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
        printf("advance time to %d\n", nextTime);
#endif
        for (k = 0; k < queuedLIREntries.size(); k++) {
            if (scheduledOps[k]
                    && currentTime
                            >= queuedLIREntries[k]->scheduledTime
                                    + queuedLIREntries[k]->instructionLatency)
                // This instruction is retired and thus no longer pending
                pendingOps[k] = false;
        }
        if (chosenIdx1 < queuedLIREntries.size()) {
            scheduledLIREntries.push_back(queuedLIREntries[chosenIdx1]);
            scheduledOps[chosenIdx1] = true;
            pendingOps[chosenIdx1] = true;
            readyOps[chosenIdx1] = false;
            queuedLIREntries[chosenIdx1]->scheduledTime = currentTime;
            numScheduled++;
        }
        if (chosenIdx2 < queuedLIREntries.size()) {
            scheduledLIREntries.push_back(queuedLIREntries[chosenIdx2]);
            scheduledOps[chosenIdx2] = true;
            pendingOps[chosenIdx2] = true;
            readyOps[chosenIdx2] = false;
            queuedLIREntries[chosenIdx2]->scheduledTime = currentTime;
            numScheduled++;
        }

        // When an instruction is scheduled, insert ready instructions to readyQueue.
        // Check dependencies from chosen instructions to other instructions.
        if (chosenIdx1 < queuedLIREntries.size())
            updateReadyOps(chosenIdx1, scheduledOps, pendingOps, readyOps);
        if (chosenIdx2 < queuedLIREntries.size())
            updateReadyOps(chosenIdx2, scheduledOps, pendingOps, readyOps);
        currentTime++;
    }

    // Free temporary data structures used during scheduling
    free(readyOps);
    free(scheduledOps);
    free(pendingOps);
    free(candidateArray);

    // Make sure that original and scheduled basic blocks are same size
    if (scheduledLIREntries.size() != queuedLIREntries.size()) {
        LOGE("ERROR: Atom Scheduler bug! Original basic block is not same "
                "size as the scheduled basic block");
        dvmAbort();
    }

    // Make sure that basic block delimiter mnemonic is always last one in
    // scheduled basic block
    if (isBasicBlockDelimiter(queuedLIREntries.back()->opCode)
            && !isBasicBlockDelimiter(scheduledLIREntries.back()->opCode)) {
        LOGE("ERROR: Atom Scheduler bug! Sync point should be the last "
        "scheduled instruction.");
        dvmAbort();
    }
}

//! \brief Called to signal the scheduler that the native basic block it has
//! been building is finished.
//! \details This method should be called from other modules to signal that the
//! native basic block the Scheduler has been building is finished. This has
//! side effects because it starts the scheduling process using already created
//! dependency graphs and then updates the code stream with the scheduled
//! instructions.
//! \warning Jumps to immediate must signal end of native basic block for target.
//! If the target has a label, then this is not a problem. But if jumping to an
//! address without label, this method must be called before building dependency
//! graph for target basic block.
void Scheduler::signalEndOfNativeBasicBlock() {
    if(queuedLIREntries.empty())
            return; // No need to do any work

    printStatistics(true /*prescheduling*/);
    schedule();
    printStatistics(false /*prescheduling*/);

    unsigned int k;
    for(k = 0; k < scheduledLIREntries.size(); ++k) {
        generateAssembly(scheduledLIREntries[k]);
        delete scheduledLIREntries[k]; // LowOp is no longer needed, can deallocate
    }

    // Clear all scheduler data structures:
    queuedLIREntries.clear();
    scheduledLIREntries.clear();
    userEntries.clear();
    producerEntries.clear();
    ctrlEntries.clear();
}

#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
//! \brief Transforms LowOpndType enum to string
//! \see LowOpndType
inline const char * operandTypeToString(LowOpndType type) {
    switch (type) {
    case LowOpndType_Imm:
        return "Imm";
    case LowOpndType_Reg:
        return "Reg";
    case LowOpndType_Mem:
        return "Mem";
    case LowOpndType_Label:
        return "Label";
    case LowOpndType_BlockId:
        return "BlockId";
    case LowOpndType_Chain:
        return "Chain";
    }
    return "-";
}
#endif

//! \brief Returns a scaled distance between two basic blocks.
//! \details Computes the Hamming distance between two basic blocks and then scales
//! result by block size and turns it into percentage. block1 and block2 must
//! have same size.
//! \retval scaled Hamming distance
inline double Scheduler::basicBlockEditDistance(const NativeBasicBlock & block1,
        const NativeBasicBlock & block2) {
#ifndef DISABLE_ATOM_SCHEDULING_STATISTICS
    int distance = 0;
    assert(block1.size() == block2.size());
    for(unsigned int i = 0; i < block1.size(); ++i) {
        if(block1[i] != block2[i]) {
            distance += 1;
        }
    }
    return (distance * 100.0) / block1.size();
#else
    return 0.0;
#endif
}

//! \brief Prints Atom Instruction Scheduling statistics.
//! Details prints block size and basic block difference.
//! \todo Comparing basic block latencies pre and post scheduling is a useful
//! statistic.
//! \param prescheduling is used to indicate whether the statistics are requested
//! before the scheduling
void Scheduler::printStatistics(bool prescheduling) {
#ifndef DISABLE_ATOM_SCHEDULING_STATISTICS
    const char * message_tag =
            prescheduling ?
                    "Atom Sched Stats: Pre-schedule:" :
                    "Atom Sched Stats: Post-schedule:";
    NativeBasicBlock * lowOpList;
    if (prescheduling)
        lowOpList = &queuedLIREntries;
    else
        lowOpList = &scheduledLIREntries;

    printf("%s The block size is %d\n", message_tag, lowOpList->size());
    for (unsigned int i = 0; i < lowOpList->size(); ++i) {
#ifndef DISABLE_DEBUG_ATOM_SCHEDULER
        printf(
                "   LIR with opcode %d and with %d operands which are %s and %s\n",
                (*lowOpList)[i]->opCode, (*lowOpList)[i]->numOperands,
                (*lowOpList)[i]->numOperands >= 1 ?
                operandTypeToString((*lowOpList)[i]->opndDest.type) : "-",
                (*lowOpList)[i]->numOperands >= 2 ?
                operandTypeToString((*lowOpList)[i]->opndSrc.type) : "-");
#endif
    }
    if (!prescheduling) {
        printf("%s Difference in basic blocks after scheduling is %5.2f%%\n",
                message_tag, basicBlockEditDistance(queuedLIREntries, scheduledLIREntries));
    }
#endif
}
