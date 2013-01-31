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

#include "Dalvik.h"
#include "Dataflow.h"
#include "Loop.h"
#include "LoopOpt.h"
#include "Checks.h"

#include <map>

/**
 * Note:
 *  The current file contains the hoisting pass framework, it uses two structures:
 *      - SRemoveData is the general pass information holder for the whole pass
 *      - STrackers is for a given BasicBlock, it is used to hold information passed through the pass' call chain
 *
 *  To add a new hoisting algorithm, follow the STEP keyword throughout the file, they
 *  show where to add your information (not including the two data structures)
 */

//STEP 0: Add any information required in the data structures
/**
 * @brief sRemoveData is used by the removal algorithm to remember which registers are live for each BasicBlock
 */
typedef struct sRemoveData
{
    std::map<int, BitVector*> nullChecks;                            /**< @brief map between BasicBlock id and a BitVector */
    std::map<int, std::map<int, BitVector *> > indexToArrayChecks;   /**< @brief map between BasicBlock id and a map of indices with arrays */

    bool hoistChecks;                                                /**< @brief Do we hoist the checks out or do we leave the first in place? */
    BitVector *hoistedNullChecks;                                    /**< @brief Remember which arrays we have hoisted already */
    std::map<int, BitVector *> hoistedArrayToIndexChecks;            /**< @brief Remember which array[index] we've hoisted already */
}SRemoveData;

/**
 * @brief sTrackers is used to track progress through a BasicBlock parsing
 */
typedef struct sTrackers
{
    BitVector *tempNullChecks;                          /**< @brief bit vector for the null checks guaranteed to have been done */
    std::map<int, BitVector *> *indexToArrayChecks;     /**< @brief map from index to Bitvector representing which checks have been done */
}STrackers;

//Forward declarations of static functions

/**
 * @brief Initialize the tempNullCheck bit vector with first predecessor
 * @param cUnit the CompilationUnit
 * @param removeData the current SRemoveData information
 * @param tempNullCheck bit vector representing which registers have been null checked
 * @param predBB predecessor block
 */
static void initializeNullChecks (CompilationUnit *cUnit, SRemoveData *removeData, BitVector *tempNullCheck, BasicBlock *predBB);

/**
 * @brief Handle new predecessor block
 * @param removeData the current SRemoveData information
 * @param tempNullCheck bit vector representing which registers have been null checked
 * @param predBB predecessor block
 */
static void handlePredecessorNull (SRemoveData *removeData, BitVector *tempNullCheck, BasicBlock *predBB);

/**
 * @brief Initialize the indexToArray map with first predecessor
 * @param cUnit the CompilationUnit
 * @param removeData the current SRemoveData information
 * @param indexToArrayChecks map between index and which array has been checked
 * @param predBB predecessor block
 */
static void initializeIndexToArray (CompilationUnit *cUnit, SRemoveData *removeData, std::map<int, BitVector *> *indexToArrayChecks, BasicBlock *predBB);

/**
 * @brief Initialize the indexToArray map with first predecessor
 * @param removeData the current SRemoveData information
 * @param indexToArrayChecks map between index and which array has been checked
 * @param predBB predecessor block
 */
static void handlePredecessorIndexToArray (SRemoveData *removeData, std::map<int, BitVector *> *indexToArrayChecks, BasicBlock *predBB);

/**
 * @brief Initialize data for the current BasicBlock
 * @param cUnit the CompilationUnit
 * @param ptrRemoveData the current SRemoveData information
 * @param bb the current BasicBlock
 * @param tracker the tracker structure for the basic block
 */
static void initializeData (CompilationUnit *cUnit, SRemoveData **ptrRemoveData, BasicBlock *bb, STrackers &tracker);

/**
 * @brief Handle the null check regarding a new MIR instruction
 * @param cUnit the CompilationUnit
 * @param removeData the current SRemoveData information
 * @param nullCheck the array index considered for nullCheck for the given mir
 * @param mir the MIR instruction
 * @param tracker the tracker structure for the basic block
 */
static void handleNullCheck (CompilationUnit *cUnit, SRemoveData *removeData, int nullCheck, MIR *mir, STrackers &tracker);


/**
 * @brief Handle the bound check regarding a new MIR instruction
 * @param cUnit the CompilationUnit
 * @param removeData the current SRemoveData information
 * @param arrayIndex the array index for the given mir
 * @param boundCheck the boundcheck index for the given mir
 * @param mir the MIR instruction
 * @param tracker the tracker structure for the basic block
 */
static void handleBoundCheck (CompilationUnit *cUnit, SRemoveData *removeData, int arrayIndex, int boundCheck, MIR *mir, STrackers &tracker);

/**
 * @brief Walk a BasicBlock, going through each instruction
 * @param cUnit the CompilationUnit
 * @param removeData the current SRemoveData information
 * @param first the first MIR instruction of the current BasicBlock
 * @param tracker the tracker structure for the basic block
 */
static void walkBasicBlock (CompilationUnit *cUnit, SRemoveData *removeData, MIR *first, STrackers &tracker);


/**
 * @brief Handle the hoisting of an array access via an index
 * @param cUnit the CompilationUnit
 * @param removeData the current SRemoveData information
 * @param mir the MIR instruction
 * @param array the array SSA register
 * @param index the array's index access SSA register
 */
static void handleIndexHoist (CompilationUnit *cUnit, SRemoveData *removeData, MIR *mir, int array, int index);

/**
 * @brief Handle the hoisting of a null check for an object access
 * @param cUnit the CompilationUnit
 * @param removeData the current SRemoveData information
 * @param mir the MIR instruction
 * @param array the array SSA register
 */
static void handleNullCheckHoist (CompilationUnit *cUnit, SRemoveData *removeData, MIR *mir, int array);



//IMPLEMENTATION

/**
 * @brief Start the removal pass, allocate the map
 * @param cUnit the CompilationUnit
 * @param curPass the SPass
 */
void dvmCompilerStartCheckRemoval (CompilationUnit *cUnit, SPass *curPass)
{
    //STEP 1: Initialize any thing for the pass data here
    SRemoveData *data = new SRemoveData;
    curPass->data = static_cast<void *> (data);

    //By default, we don't hoist the checks yet because the entry is not really the entry of the loop
    data->hoistChecks = false;
    data->hoistedNullChecks = dvmCompilerAllocBitVector (cUnit->numSSARegs, false);
    dvmClearAllBits(data->hoistedNullChecks);
}

/**
 * @brief End the removal pass, free the map
 * @param cUnit the CompilationUnit
 * @param curPass the SPass
 */
void dvmCompilerEndCheckRemoval (CompilationUnit *cUnit, SPass *curPass)
{
    SRemoveData *data = static_cast<SRemoveData *> (curPass->data);

    //STEP 2: Add any free code here

    /* We don't need to free the BitVectors: they are using the compiler heap
     * Therefore, just clear the map and free the SRemoveData structure
     */
    if (data != NULL)
    {
        data->nullChecks.clear ();

        /** Clear index to Array map
         *  Same as before, no need to free the BitVectors
         */
        for (std::map<int, std::map<int, BitVector *> >::iterator it = data->indexToArrayChecks.begin ();
                it != data->indexToArrayChecks.end ();
                it++)
        {
            std::map<int, BitVector *> &myMap = it->second;
            myMap.clear ();
        }

        data->indexToArrayChecks.clear ();

        //Now delete data and clear up curPass->data
        delete data, data = NULL;
        curPass->data = NULL;
    }
}

//NEXT COUPLE OF FUNCTIONS HANDLE DATA INITIALIZATION WHEN CONSIDERING A NEW BASIC BLOCK

void initializeIndexToArray (CompilationUnit *cUnit, SRemoveData *removeData,
                                    std::map<int, BitVector *> *indexToArrayChecks, BasicBlock *predBB)
{
    //Our current index to array map (in the indexToArrayChecks variable) is going to be the copy of the first one
    std::map<int, BitVector *> &predMap = removeData->indexToArrayChecks[predBB->id];

    for (std::map<int, BitVector *>::const_iterator it = predMap.begin ();
            it != predMap.end ();
            it++)
    {
        int key = it->first;
        BitVector *bv = it->second;
        //Create a new one for our new BasicBlock
        BitVector *newBV = dvmCompilerAllocBitVector (cUnit->numSSARegs, false);
        dvmCopyBitVector (newBV, bv);

        //Add it to our map
        (*indexToArrayChecks)[key] = newBV;
    }
}


void initializeNullChecks (CompilationUnit *cUnit, SRemoveData *removeData, BitVector *tempNullCheck, BasicBlock *predBB)
{
    //In this case, for the null handling, we are going to be setting every bit to a potential handled
    //The intersection code will remove all the false positives
    dvmSetInitialBits (tempNullCheck, cUnit->numSSARegs);

    //First handle the NULL check bitvector
    BitVector *elem = removeData->nullChecks[predBB->id];

    //Ok if we don't have this, it is because we have a backward branch, ignore it
    if (elem != NULL)
    {
        /* tempNullCheck = tempNullCheck ^ constants already checked */
        dvmIntersectBitVectors(tempNullCheck, tempNullCheck, elem);
    }
}

void handlePredecessorNull (SRemoveData *removeData, BitVector *tempNullCheck, BasicBlock *predBB)
{
    //First handle the NULL check bitvector
    BitVector *elem = removeData->nullChecks[predBB->id];

    //Ok if we don't have this, it is because we have a backward branch, ignore it
    if (elem != NULL)
    {
        /* tempNullCheck = tempNullCheck ^ constants already checked */
        dvmIntersectBitVectors(tempNullCheck, tempNullCheck, elem);
    }
}

void handlePredecessorIndexToArray (SRemoveData *removeData, std::map<int, BitVector *> *indexToArrayChecks,
                                           BasicBlock *predBB)
{
    //Second intersection is the array access check, a little bit more tricky because it is a map first
    std::map<int, BitVector *> &predIndexToArrayChecks = removeData->indexToArrayChecks[predBB->id];

    //Now we go through ours, if we don't see it in predIndexToArray, we can remove it from ours entirely
    //We can't do it as we are iterating, so let's put it in a vector
    std::vector<int> toBeRemoved;

    for (std::map<int, BitVector *>::iterator it = predIndexToArrayChecks.begin ();
            it != predIndexToArrayChecks.end ();
            it++)
    {
        int index = it->first;
        BitVector *bv = it->second;

        //Let's see if we have it
        std::map<int, BitVector *>::iterator gotIt = indexToArrayChecks->find (index);

        if (gotIt != indexToArrayChecks->end ())
        {
            //We have more work to do then: we must do an intersection
            BitVector *ourBV = gotIt->second;
            dvmIntersectBitVectors(ourBV, ourBV, bv);

            //See if we can remove it
            if (dvmCountSetBits (ourBV) == 0)
            {
                toBeRemoved.push_back (index);
            }
        }
        else
        {
            //predIndexToArray does not have it, we can remove it entirely
            toBeRemoved.push_back (index);
        }
    }

    //Now remove the ones we don't need anymore
    for (std::vector<int>::const_iterator it = toBeRemoved.begin ();
            it != toBeRemoved.end ();
            it++)
    {
        //No need to free, the compiler heap will do it
        indexToArrayChecks->erase (*it);
    }
}

void initializeData (CompilationUnit *cUnit, SRemoveData **ptrRemoveData, BasicBlock *bb,
                            STrackers &tracker)
{
    SRemoveData *removeData = *ptrRemoveData;

    //Get the block list
    GrowableList *blockList = &cUnit->blockList;

    //Get data if in loop mode
    if (cUnit->loopAnalysis != NULL)
    {
        //Get the removeData
        removeData = static_cast<SRemoveData *> (cUnit->loopAnalysis->passData);
        *ptrRemoveData = removeData;

        if (removeData != NULL)
        {
            //STEP 3: Initialize the vectors and update the pointers, update removeData if needed

            //Retrieve our indexToArrayChecks map
            tracker.indexToArrayChecks = &(removeData->indexToArrayChecks[bb->id]);

            //Set back tempNullCheck to removeData
            removeData->nullChecks[bb->id] = tracker.tempNullChecks;

            //Get pointers from tracker, more efficient than passing tracker down stream
            std::map<int, BitVector *> *indexToArrayChecks = tracker.indexToArrayChecks;
            BitVector *tempNullChecks = tracker.tempNullChecks;

            //Next step is to get the constants that have already been tested from our predecessors
            BitVectorIterator bvIterator;

            /* Iterate through the predecessors */
            dvmBitVectorIteratorInit(bb->predecessors, &bvIterator);

            int predIdx = dvmBitVectorIteratorNext(&bvIterator);

            //If we have at least one predecessor
            if (predIdx != -1)
            {
                //Get the basic block
                BasicBlock *predBB = (BasicBlock *) dvmGrowableListGetElement(blockList, predIdx);

                //STEP 4: handle first predecessor if needed

                //Call for Null checks
                initializeNullChecks (cUnit, removeData, tempNullChecks, predBB);

                //Call for IndexToArray
                initializeIndexToArray (cUnit, removeData, indexToArrayChecks, predBB);

                //Get next predecessor
                predIdx = dvmBitVectorIteratorNext(&bvIterator);

                while (predIdx != -1) {
                    //Get the basic block
                    predBB = (BasicBlock *) dvmGrowableListGetElement(blockList, predIdx);

                    //STEP 5: handle new predecessor

                    //Call for null check
                    handlePredecessorNull (removeData, tempNullChecks, predBB);

                    //Call for IndexToArray
                    handlePredecessorIndexToArray (removeData, indexToArrayChecks, predBB);

                    //Next predecessor
                    predIdx = dvmBitVectorIteratorNext(&bvIterator);
                }
            }
        }
    }
}

//NEXT COUPLE OF FUNCTIONS HANDLE THE WALKING THROUGH A BASIC BLOCK
void handleNullCheck (CompilationUnit *cUnit, SRemoveData *removeData, int nullCheck, MIR *mir, STrackers &tracker)
{
    //Do we have an index?
    if (nullCheck >= 0)
    {
        int reg = mir->ssaRep->uses[nullCheck];

        BitVector *tempNullCheck = tracker.tempNullChecks;

        //Has it already been tested
        if (dvmIsBitSet (tempNullCheck, reg) != 0)
        {
            //We can remove the null check then
            mir->OptimizationFlags |= MIR_IGNORE_NULL_CHECK;
        }
        else
        {
            //Otherwise, we'll leave this one and set it here
            dvmSetBit (tempNullCheck, reg);

            //Hoist if requested
            handleNullCheckHoist (cUnit, removeData, mir, reg);
        }
    }
}

void handleBoundCheck (CompilationUnit *cUnit, SRemoveData *removeData, int nullCheck, int boundCheck,
                              MIR *mir, STrackers &tracker)
{
    //Check the bound check via indexToArrayChecks
    if (boundCheck >= 0)
    {
        int array = mir->ssaRep->uses[nullCheck];
        int index = mir->ssaRep->uses[boundCheck];

        std::map<int, BitVector *> *indexToArrayChecks = tracker.indexToArrayChecks;

        //Check the bounds: first get the right BitVector
        BitVector *bv = (*indexToArrayChecks)[index];

        if (bv == NULL)
        {
            //Create it then
            bv = dvmCompilerAllocBitVector (cUnit->numSSARegs, false);
            dvmClearAllBits(bv);

            //Set it because no one ever has here
            dvmSetBit (bv, array);
            //Finally store it
            (*indexToArrayChecks)[index] = bv;

            //Hoist if requested
            handleIndexHoist (cUnit, removeData, mir, array, index);
        }
        else
        {
            //Otherwise is it set?
            if (dvmIsBitSet (bv, array) != 0)
            {
                //We can remove the null check then
                mir->OptimizationFlags |= MIR_IGNORE_RANGE_CHECK;
            }
            else
            {
                //Otherwise, we'll leave this one and set it here
                dvmSetBit (bv, array);

                //Hoist if requested
                handleIndexHoist (cUnit, removeData, mir, array, index);
            }
        }
    }
}

void walkBasicBlock (CompilationUnit *cUnit, SRemoveData *removeData, MIR *first, STrackers &tracker)
{
    for (MIR *mir = first; mir != NULL; mir = mir->next)
    {
        DecodedInstruction *dInsn = &mir->dalvikInsn;
        int dfAttributes =
            dvmCompilerDataFlowAttributes[mir->dalvikInsn.opcode];

        /* Skip extended MIR instructions */
        if (dInsn->opcode >= kNumPackedOpcodes) continue;

        int instrFlags = dexGetFlagsFromOpcode(dInsn->opcode);

        /* Instruction is clean */
        if ((instrFlags & kInstrCanThrow) == 0) continue;

        /*
         * Currently we can only optimize away null and range checks. Punt on
         * instructions that can throw due to other exceptions.
         */
        if ( (dfAttributes & (DF_HAS_NR_CHECKS | DF_HAS_OBJECT_CHECKS)) == 0)
        {
            continue;
        }

        //STEP 6: add any value dependent on the DataFlow attribute
        //Set to -1 both
        int nullCheck = -1;
        int boundCheck = -1;

        //Depending on the instruction, the array and index are in different registers
        switch (dfAttributes & DF_HAS_NR_CHECKS) {
            case DF_NULL_N_RANGE_CHECK_0:
                nullCheck = 0;
                boundCheck = nullCheck + 1;
                break;
            case DF_NULL_N_RANGE_CHECK_1:
                nullCheck = 1;
                boundCheck = nullCheck + 1;
                break;
            case DF_NULL_N_RANGE_CHECK_2:
                nullCheck = 2;
                boundCheck = nullCheck + 1;
                break;
            default:
                break;
        }

        switch (dfAttributes & DF_HAS_OBJECT_CHECKS)
        {
            case DF_NULL_OBJECT_CHECK_0:
                nullCheck = 0;
                break;
            case DF_NULL_OBJECT_CHECK_1:
                nullCheck = 1;
                break;
            case DF_NULL_OBJECT_CHECK_2:
                nullCheck = 2;
                break;
            default:
                break;
        }

        //STEP 7: actually decide what to do

        //First check the null register
        //Little warning: if we are hoisting, we do want this test first before any bound checking
        //because we do not test null for arrays afterwards, we suppose it has already been done by this code
        handleNullCheck (cUnit, removeData, nullCheck, mir, tracker);

        //Now handle bound check
        handleBoundCheck (cUnit, removeData, nullCheck, boundCheck, mir, tracker);
    }
}

/**
 * @brief The removal pass: point of entry remove any redundant checks
 * @param cUnit the CompilationUnit
 * @param bb the BasicBlock
 */
bool dvmCompilerCheckRemoval (CompilationUnit *cUnit, BasicBlock *bb)
{
    SRemoveData *removeData = NULL;

    //Create a tracker
    STrackers tracker;
    memset (&tracker, 0, sizeof (tracker));

    //STEP 8: initialize tracker structure

    //Create a bit vector for this one
    tracker.tempNullChecks = dvmCompilerAllocBitVector (cUnit->numSSARegs, false);
    dvmClearAllBits(tracker.tempNullChecks);

    //There is a temporary index to array check map in case we are in trace mode
    std::map<int, BitVector *> temporaryIndexToArrayChecks;
    tracker.indexToArrayChecks = &temporaryIndexToArrayChecks;

    /* Initialize the data */
    initializeData (cUnit, &removeData, bb, tracker);

   /* Now tempNullCheck contains exactly which registers have already been tested as Null and the
    * indexToArrayChecks is up to date for bound checks
    * Let us go through the basic block and see if we can remove tests
    */
    walkBasicBlock (cUnit, removeData, bb->firstMIRInsn, tracker);

    //We have not changed the basic block
    return false;
}

void handleIndexHoist (CompilationUnit *cUnit, SRemoveData *removeData, MIR *mir, int arrayReg, int indexReg)
{
    //We only hoist if removeData is set, we know what has been hoisted, and we do want to hoist
    if (removeData != NULL && removeData->hoistedNullChecks != NULL && removeData->hoistChecks == true)
    {
        //Ok before going anywhere, we must first check if we hoisted the arrayReg already?
        //Normally we should have but let us be paranoid
        if (dvmIsBitSet (removeData->hoistedNullChecks, arrayReg) == true)
        {
            //Actually, we only hoist this check if it is a constant (sub == 0)
            //Technically we could also hoist induction variables but this is currently done
            //in a previous pass, we could merge them however at some point
            //We do test index and array because we can only hoist it if both are constants
            int subNRegArray = dvmConvertSSARegToDalvik(cUnit, arrayReg);
            int arraySub = DECODE_SUB(subNRegArray);
            int subNRegIndex = dvmConvertSSARegToDalvik(cUnit, indexReg);
            int indexSub = DECODE_SUB(subNRegIndex);

            //Now whether we can hoist depends first on the index: is it a constant or an invariant
            bool canHoist = ( (indexSub == 0) || (dvmIsBitSet (cUnit->isConstantV, indexReg) == true));

            //Then the array must be invariant
            canHoist = (canHoist && arraySub == 0);

            if (canHoist == true)
            {
                //Get entry block
                BasicBlock *entry = cUnit->entryBlock;

                if (entry != NULL)
                {
                    //We can remove the null check then
                    // Either we already have hoisted it and we are safe
                    // Or we are going to
                    mir->OptimizationFlags |= MIR_IGNORE_RANGE_CHECK;

                    //Get bitvector for the array, we use arrays here because we assume there are less arrays than indices..
                    BitVector *bv = removeData->hoistedArrayToIndexChecks[arrayReg];

                    //For readability, let's reset canHoist to true though it is already
                    canHoist = true;

                    //If not found, we can try to hoist
                    if (bv == NULL)
                    {
                        //But first create it
                        bv = dvmCompilerAllocBitVector (cUnit->numSSARegs, false);
                        dvmClearAllBits (bv);
                        dvmSetBit (bv, indexReg);
                    }
                    else
                    {
                        //Then have we already bound checked with indexReg?
                        // If yes, we don't hoist here
                        // If no, we hoist
                        if (dvmIsBitSet (bv, indexReg) == true)
                        {
                            canHoist = false;
                        }
                        else
                        {
                            //Technically we have not hoisted yet but the following tests will always have
                            //the same outcome, so bail before finding that out next time
                            dvmSetBit (bv, indexReg);
                        }
                    }

                    if (canHoist == true)
                    {
                        //Actually generate the hoisting code
                        MIR *boundCheck = static_cast<MIR *> (dvmCompilerNew (sizeof (*boundCheck), true));
                        boundCheck->dalvikInsn.opcode = static_cast<Opcode> (kMirOpBoundCheck);
                        //We only care about the object register and index register
                        boundCheck->dalvikInsn.vA = arrayReg;

                        //The rest of the bound check depends on whether it is a register or constant
                        if (indexSub == 0)
                        {
                            boundCheck->dalvikInsn.arg[0] = MIR_BOUND_CHECK_REG;
                            boundCheck->dalvikInsn.arg[1] = indexReg;
                        }
                        else
                        {
                            boundCheck->dalvikInsn.arg[0] = MIR_BOUND_CHECK_CST;
                            boundCheck->dalvikInsn.arg[1] = cUnit->constantValues[indexReg];
                        }

                        boundCheck->dalvikInsn.vC = 0;
                        boundCheck->ssaRep = mir->ssaRep;
                        dvmCompilerAppendMIR(entry, boundCheck);
                    }
                }
            }
        }
    }

}


void handleNullCheckHoist (CompilationUnit *cUnit, SRemoveData *removeData, MIR *mir, int objectReg)
{
    //Do we hoist ?
    if (removeData != NULL && removeData->hoistChecks == true)
    {
        int subNRegObject = dvmConvertSSARegToDalvik(cUnit, objectReg);
        int objectSub = DECODE_SUB(subNRegObject);

        //If so, we can actually only hoist it if the sub is 0
        //0 meaning that it has never been assigned (and thus never changed in the loop)
        if (objectSub == 0)
        {
            //Get entry block
            BasicBlock *entry = cUnit->entryBlock;

            if (entry != NULL)
            {
                //Have we hoisted it already?
                if (dvmIsBitSet (removeData->hoistedNullChecks, objectReg) == false)
                {
                    //Though we have not hoisted yet, the reasons below will be true for any subsequent request
                    dvmSetBit (removeData->hoistedNullChecks, objectReg);
                        //Actually generate the hoisting code
                        MIR *nullCheck = static_cast<MIR *> (dvmCompilerNew (sizeof (*nullCheck), true));
                        nullCheck->dalvikInsn.opcode = static_cast<Opcode> (kMirOpNullCheck);
                        //We only care about the object register
                        nullCheck->dalvikInsn.vA = objectReg;
                        nullCheck->dalvikInsn.vB = 0;
                        nullCheck->dalvikInsn.vC = 0;
                        nullCheck->ssaRep = mir->ssaRep;
                        dvmCompilerAppendMIR(entry, nullCheck);
                }

                //In either case, we can remove it:
                // Either we have already hoisted it
                // Or we haven't and we just did
                mir->OptimizationFlags |= MIR_IGNORE_NULL_CHECK;
            }
        }
    }
}
