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

#include "Checks.h"
#include "Dalvik.h"
#include "Dataflow.h"
#include "Loop.h"
#include "LoopOpt.h"

/**
 * @brief Static functions defined below
 */
static void startConstant (CompilationUnit *cUnit, SPass *curPass);
static void endConstant (CompilationUnit *cUnit, SPass *curPass);
static void startInductionVariable (CompilationUnit *cUnit, SPass *curPass);
static void endInductionVariable (CompilationUnit *cUnit, SPass *curPass);
static void startInductionAccessDetection (CompilationUnit *cUnit, SPass *curPass);
static void endInductionAccessDetection (CompilationUnit *cUnit, SPass *curPass);
static void simpleCountedLoopHandler (CompilationUnit *cUnit, SPass *curPass);
static void endHoisting (CompilationUnit *cUnit, SPass *curPass);
static bool simpleCountedGate (const CompilationUnit *cUnit, const SPass *curPass);

/**
 * @brief Three macros to help pass definitions
 */

#define START_PASSES \
    static SPass gPasses[] = {

#define NEW_PASS(NAME,TYPE,DATA,GATE,START,END,WORK,FLAG) \
        { \
            NAME, TYPE, DATA, GATE, START, END, WORK, FLAG \
        }

#define END_PASSES \
    };

START_PASSES
    NEW_PASS ("Constant values detection", kAllNodes, NULL, NULL,
            startConstant, endConstant, dvmCompilerDoConstantPropagation,0),
    NEW_PASS ("Induction variable detection", kAllNodes, NULL, NULL,
                 startInductionVariable, endInductionVariable, dvmCompilerFindInductionVariables,0),
    NEW_PASS ("Simple Counted Loop Detection", kAllNodes, NULL, NULL,
                 NULL, simpleCountedLoopHandler, NULL, 0),
    NEW_PASS ("Induction variable detection", kAllNodes, NULL, simpleCountedGate,
                 startInductionVariable, endInductionVariable, dvmCompilerFindInductionVariables, 0),
    NEW_PASS ("Induction array based checks", kAllNodes, NULL, simpleCountedGate,
                 startInductionAccessDetection, endInductionAccessDetection, NULL, 0),
    NEW_PASS ("Generate the hoisted code from induction array", kAllNodes, NULL, simpleCountedGate,
                 NULL, endHoisting, NULL, 0),
    NEW_PASS ("Check Removal", kAllNodes, NULL, NULL,
                dvmCompilerStartCheckRemoval, dvmCompilerEndCheckRemoval, dvmCompilerCheckRemoval, 0),

    /* Last element must have and be the only one to have a NULL name, it's our ending check */
    NEW_PASS (NULL,         /** Pass name */
         kAllNodes,     /** Type of traversal */
         NULL,          /** Data */
         NULL,          /** Gate function */
         NULL,          /** Start function */
         NULL,          /** End function */
         NULL,          /** Work function */
         0              /** Flags */
         ),
END_PASSES

/**
  * @brief Handle pass flag, for the moment the function is empty
  * @param cUnit the CompilationUnit
  * @param curPass the SPass
  */
static void handlePassFlag (CompilationUnit *cUnit, SPass *pass)
{
    /** Todo when needed */
    (void) cUnit;
    (void) pass;
}

 /**
  * @brief The loop pass driver
  * @param cUnit the CompilationUnit
  */
void dvmCompilerLoopOptimizationDriver (CompilationUnit *cUnit)
{
    //Go through the different elements
    SPass *curPass = gPasses;

    while (curPass->passName != NULL)
    {
        //We have a pass: do we apply it? By default yes
        bool applyPass = true;

        //But if a gate is defined, we call that
        if (curPass->gate != NULL)
        {
            applyPass = curPass->gate (cUnit, curPass);
        }

        if (applyPass == true)
        {
            //Applying the pass: first start, doWork, and end calls
            //Each function might not be defined
            if (curPass->start != NULL)
            {
                curPass->start (cUnit, curPass);
            }

            if (curPass->doWork != NULL)
            {
                //Set the data in cUnit
                LoopAnalysis *loopAnalysis = cUnit->loopAnalysis;
                loopAnalysis->passData = curPass->data;
                dvmCompilerDataFlowAnalysisDispatcher (cUnit, curPass->doWork, curPass->traversalType, false /* isIterative */);
                loopAnalysis->passData = NULL;
            }

            if (curPass->end != NULL)
            {
                curPass->end (cUnit, curPass);
            }

            //Do we need any clean up?
            handlePassFlag (cUnit, curPass);
        }

        //Go to next pass
        curPass++;
    }
}


//Functions used to emulate the original code structure
/**
  * @brief Initializes the constant detection data structures
  * @param cUnit the CompilationUnit
  * @param curPass the SPass
  */
static void startConstant (CompilationUnit *cUnit, SPass *curPass)
{
    /* Constant propagation */
    cUnit->isConstantV = dvmCompilerAllocBitVector(cUnit->numSSARegs, false);
    cUnit->constantValues = (int *)dvmCompilerNew(sizeof(int) * cUnit->numSSARegs, true);

    //Remove warning
    (void) curPass;
}

/**
  * @brief Dumps the loop constants
  * @param cUnit the CompilationUnit
  * @param curPass the SPass
  */
static void endConstant (CompilationUnit *cUnit, SPass *curPass)
{
    dvmCompilerDumpConstants (cUnit);

    //Remove warnings
    (void) cUnit;
    (void) curPass;
}

/**
  * @brief Initializes the data for the induction detection pass
  * @param cUnit the CompilationUnit
  * @param curPass the SPass
  */
static void startInductionVariable (CompilationUnit *cUnit, SPass *curPass)
{
    LoopAnalysis *loopAnalysis = cUnit->loopAnalysis;

    /* Find induction variables - basic and dependent */
    loopAnalysis->ivList = (GrowableList *)dvmCompilerNew(sizeof(GrowableList), true);
    dvmInitGrowableList(loopAnalysis->ivList, 4);
    loopAnalysis->isIndVarV = dvmCompilerAllocBitVector(cUnit->numSSARegs, false);

    //Remove warning
    (void) curPass;
}

/**
  * @brief Dumps the Induction variable list
  * @param cUnit the CompilationUnit
  * @param curPass the SPass
  */
static void endInductionVariable (CompilationUnit *cUnit, SPass *curPass)
{
    dvmCompilerDumpIVList (cUnit);

    //Remove warnings
    (void) cUnit;
    (void) curPass;
}

/**
  * @brief Initializes data for the induction access pass
  * @param cUnit the CompilationUnit
  * @param curPass the SPass
  */
static void startInductionAccessDetection (CompilationUnit *cUnit, SPass *curPass)
{
    LoopAnalysis *loopAnalysis = cUnit->loopAnalysis;
    loopAnalysis->arrayAccessInfo = (GrowableList *)dvmCompilerNew(sizeof(GrowableList), true);
    dvmInitGrowableList(loopAnalysis->arrayAccessInfo, 4);

    //Remove warning
    (void) curPass;
}

/**
  * @brief Perform the body code motion function: handles induction access detection
  * @param cUnit the CompilationUnit
  * @param curPass the SPass
  */
static void endInductionAccessDetection (CompilationUnit *cUnit, SPass *curPass)
{
    LoopAnalysis *loopAnalysis = cUnit->loopAnalysis;
    loopAnalysis->bodyIsClean = dvmCompilerIVArrayAccess (cUnit);

    dvmCompilerDumpHoistedChecks (cUnit);

    //Remove warning
    (void) curPass;
}

/**
  * @brief Determines whether the loop is a simple counted loop, ie:
        - Only one induction variable, knowing the lower/upper bounds
    Function also fills in the loopAnalysis->isSimpleCountedLoop variable
  * @param cUnit the CompilationUnit
  */
static void simpleCountedLoopHandler (CompilationUnit *cUnit, SPass *curPass)
{
    bool res = dvmCompilerSimpleCountedLoop(cUnit);

    //Set it in the loop analysis information
    LoopAnalysis *loopAnalysis = cUnit->loopAnalysis;
    loopAnalysis->isSimpleCountedLoop = res;

    //Remove warning
    (void) curPass;
}

/**
  * @brief Generates the hoisting of the checks
  * @param cUnit the CompilationUnit
  * @param curPass the SPass
  */
static void endHoisting (CompilationUnit *cUnit, SPass *curPass)
{
    /*
     * Convert the array access information into extended MIR code in the loop header.
     */
    dvmCompilerHoistIVArrayAccess (cUnit);

    //Remove warning
    (void) curPass;
}

/**
  * @brief The simple counted gate, checks the simple counted loop boolean in the LoopAnalysis data structure.
    It is filled up by the Simple Counted Loop Detection pass
  * @param cUnit the CompilationUnit
  * @param curPass the SPass
  * @return whether or not the isSimpleCountedLoop variable is set
  */
static bool simpleCountedGate (const CompilationUnit *cUnit, const SPass *curPass)
{
    LoopAnalysis *loopAnalysis = cUnit->loopAnalysis;

    //Remove warning
    (void) curPass;

    return loopAnalysis->isSimpleCountedLoop;
}
