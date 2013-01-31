/*
 * Copyright (C) 2009 The Android Open Source Project
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
#include "libdex/DexOpcodes.h"
#include "libdex/DexCatch.h"
#include "interp/Jit.h"
#include "CompilerInternals.h"
#include "Dataflow.h"

#define BYTECODE_FILTER

#ifdef PROFILE_OPCODE
#include "dalvikvm/ProfileOpcodes.h"
extern int opcodeJit[kNumPackedOpcodes];
#endif

#if defined(VTUNE_DALVIK)
#include "JitProfiling.h"
#include <vector>
#include <algorithm>

static void SendTraceInfoToVTune(CompilationUnit *cUnit, JitTraceDescription *desc);
#endif

//Need it for UINT_MAX
#include <limits.h>


static inline bool contentIsInsn(const u2 *codePtr) {
    u2 instr = *codePtr;
    Opcode opcode = (Opcode)(instr & 0xff);

    /*
     * Since the low 8-bit in metadata may look like OP_NOP, we need to check
     * both the low and whole sub-word to determine whether it is code or data.
     */
    return (opcode != OP_NOP || instr == 0);
}

/*
 * Parse an instruction, return the length of the instruction
 */
static inline int parseInsn(const u2 *codePtr, DecodedInstruction *decInsn,
                            bool printMe)
{
    // Don't parse instruction data
    if (!contentIsInsn(codePtr)) {
        return 0;
    }

    u2 instr = *codePtr;
    Opcode opcode = dexOpcodeFromCodeUnit(instr);

    dexDecodeInstruction(codePtr, decInsn);
    if (printMe) {
        char *decodedString = dvmCompilerGetDalvikDisassembly(decInsn, NULL);
        ALOGD("%p: %#06x %s", codePtr, opcode, decodedString);
    }
    return dexGetWidthFromOpcode(opcode);
}

#define UNKNOWN_TARGET 0xffffffff

/*
 * Identify block-ending instructions and collect supplemental information
 * regarding the following instructions.
 */
static inline bool findBlockBoundary(const Method *caller, MIR *insn,
                                     unsigned int curOffset,
                                     unsigned int *target, bool *isInvoke,
                                     const Method **callee)
{
    switch (insn->dalvikInsn.opcode) {
        /* Target is not compile-time constant */
        case OP_RETURN_VOID:
        case OP_RETURN:
        case OP_RETURN_WIDE:
        case OP_RETURN_OBJECT:
        case OP_THROW:
          *target = UNKNOWN_TARGET;
          break;
        case OP_INVOKE_VIRTUAL:
        case OP_INVOKE_VIRTUAL_RANGE:
        case OP_INVOKE_INTERFACE:
        case OP_INVOKE_INTERFACE_RANGE:
        case OP_INVOKE_VIRTUAL_QUICK:
        case OP_INVOKE_VIRTUAL_QUICK_RANGE:
            *isInvoke = true;
            break;
        case OP_INVOKE_SUPER:
        case OP_INVOKE_SUPER_RANGE: {
            int mIndex = caller->clazz->pDvmDex->
                pResMethods[insn->dalvikInsn.vB]->methodIndex;
            const Method *calleeMethod =
                caller->clazz->super->vtable[mIndex];

            if (calleeMethod && !dvmIsNativeMethod(calleeMethod)) {
                *target = (unsigned int) calleeMethod->insns;
            }
            *isInvoke = true;
            *callee = calleeMethod;
            break;
        }
        case OP_INVOKE_STATIC:
        case OP_INVOKE_STATIC_RANGE: {
            const Method *calleeMethod =
                caller->clazz->pDvmDex->pResMethods[insn->dalvikInsn.vB];

            if (calleeMethod && !dvmIsNativeMethod(calleeMethod)) {
                *target = (unsigned int) calleeMethod->insns;
            }
            *isInvoke = true;
            *callee = calleeMethod;
            break;
        }
        case OP_INVOKE_SUPER_QUICK:
        case OP_INVOKE_SUPER_QUICK_RANGE: {
            const Method *calleeMethod =
                caller->clazz->super->vtable[insn->dalvikInsn.vB];

            if (calleeMethod && !dvmIsNativeMethod(calleeMethod)) {
                *target = (unsigned int) calleeMethod->insns;
            }
            *isInvoke = true;
            *callee = calleeMethod;
            break;
        }
        case OP_INVOKE_DIRECT:
        case OP_INVOKE_DIRECT_RANGE: {
            const Method *calleeMethod =
                caller->clazz->pDvmDex->pResMethods[insn->dalvikInsn.vB];
            if (calleeMethod && !dvmIsNativeMethod(calleeMethod)) {
                *target = (unsigned int) calleeMethod->insns;
            }
            *isInvoke = true;
            *callee = calleeMethod;
            break;
        }
        case OP_GOTO:
        case OP_GOTO_16:
        case OP_GOTO_32:
            *target = curOffset + (int) insn->dalvikInsn.vA;
            break;

        case OP_IF_EQ:
        case OP_IF_NE:
        case OP_IF_LT:
        case OP_IF_GE:
        case OP_IF_GT:
        case OP_IF_LE:
            *target = curOffset + (int) insn->dalvikInsn.vC;
            break;

        case OP_IF_EQZ:
        case OP_IF_NEZ:
        case OP_IF_LTZ:
        case OP_IF_GEZ:
        case OP_IF_GTZ:
        case OP_IF_LEZ:
            *target = curOffset + (int) insn->dalvikInsn.vB;
            break;

        default:
            return false;
    }
    return true;
}

static inline bool isGoto(MIR *insn)
{
    switch (insn->dalvikInsn.opcode) {
        case OP_GOTO:
        case OP_GOTO_16:
        case OP_GOTO_32:
            return true;
        default:
            return false;
    }
}

/*
 * Identify unconditional branch instructions
 */
static inline bool isUnconditionalBranch(MIR *insn)
{
    switch (insn->dalvikInsn.opcode) {
        case OP_RETURN_VOID:
        case OP_RETURN:
        case OP_RETURN_WIDE:
        case OP_RETURN_OBJECT:
            return true;
        default:
            return isGoto(insn);
    }
}

/*
 * dvmHashTableLookup() callback
 */
static int compareMethod(const CompilerMethodStats *m1,
                         const CompilerMethodStats *m2)
{
    return (int) m1->method - (int) m2->method;
}

/*
 * Analyze the body of the method to collect high-level information regarding
 * inlining:
 * - is empty method?
 * - is getter/setter?
 * - can throw exception?
 *
 * Currently the inliner only handles getters and setters. When its capability
 * becomes more sophisticated more information will be retrieved here.
 */
static int analyzeInlineTarget(DecodedInstruction *dalvikInsn, int attributes,
                               int offset)
{
    int flags = dexGetFlagsFromOpcode(dalvikInsn->opcode);
    int dalvikOpcode = dalvikInsn->opcode;

    if (flags & kInstrInvoke) {
        attributes &= ~METHOD_IS_LEAF;
    }

    if (!(flags & kInstrCanReturn)) {
        if (!(dvmCompilerDataFlowAttributes[dalvikOpcode] &
              DF_IS_GETTER)) {
            attributes &= ~METHOD_IS_GETTER;
        }
        if (!(dvmCompilerDataFlowAttributes[dalvikOpcode] &
              DF_IS_SETTER)) {
            attributes &= ~METHOD_IS_SETTER;
        }
    }

    /*
     * The expected instruction sequence is setter will never return value and
     * getter will also do. Clear the bits if the behavior is discovered
     * otherwise.
     */
    if (flags & kInstrCanReturn) {
        if (dalvikOpcode == OP_RETURN_VOID) {
            attributes &= ~METHOD_IS_GETTER;
        }
        else {
            attributes &= ~METHOD_IS_SETTER;
        }
    }

    if (flags & kInstrCanThrow) {
        attributes &= ~METHOD_IS_THROW_FREE;
    }

    if (offset == 0 && dalvikOpcode == OP_RETURN_VOID) {
        attributes |= METHOD_IS_EMPTY;
    }

    /*
     * Check if this opcode is selected for single stepping.
     * If so, don't inline the callee as there is no stack frame for the
     * interpreter to single-step through the instruction.
     */
    if (SINGLE_STEP_OP(dalvikOpcode)) {
        attributes &= ~(METHOD_IS_GETTER | METHOD_IS_SETTER);
    }

    return attributes;
}

/*
 * Analyze each method whose traces are ever compiled. Collect a variety of
 * statistics like the ratio of exercised vs overall code and code bloat
 * ratios. If isCallee is true, also analyze each instruction in more details
 * to see if it is suitable for inlining.
 */
CompilerMethodStats *dvmCompilerAnalyzeMethodBody(const Method *method,
                                                  bool isCallee)
{
    const DexCode *dexCode = dvmGetMethodCode(method);
    const u2 *codePtr = dexCode->insns;
    const u2 *codeEnd = dexCode->insns + dexCode->insnsSize;
    int insnSize = 0;
    int hashValue = dvmComputeUtf8Hash(method->name);

    CompilerMethodStats dummyMethodEntry; // For hash table lookup
    CompilerMethodStats *realMethodEntry; // For hash table storage

    /* For lookup only */
    dummyMethodEntry.method = method;
    realMethodEntry = (CompilerMethodStats *)
        dvmHashTableLookup(gDvmJit.methodStatsTable,
                           hashValue,
                           &dummyMethodEntry,
                           (HashCompareFunc) compareMethod,
                           false);

    /* This method has never been analyzed before - create an entry */
    if (realMethodEntry == NULL) {
        realMethodEntry =
            (CompilerMethodStats *) calloc(1, sizeof(CompilerMethodStats));
        realMethodEntry->method = method;

        dvmHashTableLookup(gDvmJit.methodStatsTable, hashValue,
                           realMethodEntry,
                           (HashCompareFunc) compareMethod,
                           true);
    }

    /* This method is invoked as a callee and has been analyzed - just return */
    if ((isCallee == true) && (realMethodEntry->attributes & METHOD_IS_CALLEE))
        return realMethodEntry;

    /*
     * Similarly, return if this method has been compiled before as a hot
     * method already.
     */
    if ((isCallee == false) &&
        (realMethodEntry->attributes & METHOD_IS_HOT))
        return realMethodEntry;

    int attributes;

    /* Method hasn't been analyzed for the desired purpose yet */
    if (isCallee) {
        /* Aggressively set the attributes until proven otherwise */
        attributes = METHOD_IS_LEAF | METHOD_IS_THROW_FREE | METHOD_IS_CALLEE |
                     METHOD_IS_GETTER | METHOD_IS_SETTER;
    } else {
        attributes = METHOD_IS_HOT;
    }

    /* Count the number of instructions */
    while (codePtr < codeEnd) {
        DecodedInstruction dalvikInsn;
        int width = parseInsn(codePtr, &dalvikInsn, false);

        /* Terminate when the data section is seen */
        if (width == 0)
            break;

        if (isCallee) {
            attributes = analyzeInlineTarget(&dalvikInsn, attributes, insnSize);
        }

        insnSize += width;
        codePtr += width;
    }

    /*
     * Only handle simple getters/setters with one instruction followed by
     * return
     */
    if ((attributes & (METHOD_IS_GETTER | METHOD_IS_SETTER)) &&
        (insnSize != 3)) {
        attributes &= ~(METHOD_IS_GETTER | METHOD_IS_SETTER);
    }

    realMethodEntry->dalvikSize = insnSize * 2;
    realMethodEntry->attributes |= attributes;

#if 0
    /* Uncomment the following to explore various callee patterns */
    if (attributes & METHOD_IS_THROW_FREE) {
        ALOGE("%s%s is inlinable%s", method->clazz->descriptor, method->name,
             (attributes & METHOD_IS_EMPTY) ? " empty" : "");
    }

    if (attributes & METHOD_IS_LEAF) {
        ALOGE("%s%s is leaf %d%s", method->clazz->descriptor, method->name,
             insnSize, insnSize < 5 ? " (small)" : "");
    }

    if (attributes & (METHOD_IS_GETTER | METHOD_IS_SETTER)) {
        ALOGE("%s%s is %s", method->clazz->descriptor, method->name,
             attributes & METHOD_IS_GETTER ? "getter": "setter");
    }
    if (attributes ==
        (METHOD_IS_LEAF | METHOD_IS_THROW_FREE | METHOD_IS_CALLEE)) {
        ALOGE("%s%s is inlinable non setter/getter", method->clazz->descriptor,
             method->name);
    }
#endif

    return realMethodEntry;
}

/*
 * Crawl the stack of the thread that requesed compilation to see if any of the
 * ancestors are on the blacklist.
 */
static bool filterMethodByCallGraph(Thread *thread, const char *curMethodName)
{
    /* Crawl the Dalvik stack frames and compare the method name*/
    StackSaveArea *ssaPtr = ((StackSaveArea *) thread->interpSave.curFrame) - 1;
    while (ssaPtr != ((StackSaveArea *) NULL) - 1) {
        const Method *method = ssaPtr->method;
        if (method) {
            int hashValue = dvmComputeUtf8Hash(method->name);
            bool found =
                dvmHashTableLookup(gDvmJit.methodTable, hashValue,
                               (char *) method->name,
                               (HashCompareFunc) strcmp, false) !=
                NULL;
            if (found) {
                ALOGD("Method %s (--> %s) found on the JIT %s list",
                     method->name, curMethodName,
                     gDvmJit.includeSelectedMethod ? "white" : "black");
                return true;
            }

        }
        ssaPtr = ((StackSaveArea *) ssaPtr->prevFrame) - 1;
    };
    return false;
}

/*
 * Since we are including instructions from possibly a cold method into the
 * current trace, we need to make sure that all the associated information
 * with the callee is properly initialized. If not, we punt on this inline
 * target.
 *
 * TODO: volatile instructions will be handled later.
 */
bool dvmCompilerCanIncludeThisInstruction(const Method *method,
                                          const DecodedInstruction *insn)
{
    switch (insn->opcode) {
        case OP_NEW_INSTANCE:
        case OP_CHECK_CAST: {
            ClassObject *classPtr = (ClassObject *)(void*)
              (method->clazz->pDvmDex->pResClasses[insn->vB]);

            /* Class hasn't been initialized yet */
            if (classPtr == NULL) {
                return false;
            }
            return true;
        }
        case OP_SGET:
        case OP_SGET_WIDE:
        case OP_SGET_OBJECT:
        case OP_SGET_BOOLEAN:
        case OP_SGET_BYTE:
        case OP_SGET_CHAR:
        case OP_SGET_SHORT:
        case OP_SPUT:
        case OP_SPUT_WIDE:
        case OP_SPUT_OBJECT:
        case OP_SPUT_BOOLEAN:
        case OP_SPUT_BYTE:
        case OP_SPUT_CHAR:
        case OP_SPUT_SHORT: {
            void *fieldPtr = (void*)
              (method->clazz->pDvmDex->pResFields[insn->vB]);

            if (fieldPtr == NULL) {
                return false;
            }
            return true;
        }
        case OP_INVOKE_SUPER:
        case OP_INVOKE_SUPER_RANGE: {
            int mIndex = method->clazz->pDvmDex->
                pResMethods[insn->vB]->methodIndex;
            const Method *calleeMethod = method->clazz->super->vtable[mIndex];
            if (calleeMethod == NULL) {
                return false;
            }
            return true;
        }
        case OP_INVOKE_SUPER_QUICK:
        case OP_INVOKE_SUPER_QUICK_RANGE: {
            const Method *calleeMethod = method->clazz->super->vtable[insn->vB];
            if (calleeMethod == NULL) {
                return false;
            }
            return true;
        }
        case OP_INVOKE_STATIC:
        case OP_INVOKE_STATIC_RANGE:
        case OP_INVOKE_DIRECT:
        case OP_INVOKE_DIRECT_RANGE: {
            const Method *calleeMethod =
                method->clazz->pDvmDex->pResMethods[insn->vB];
            if (calleeMethod == NULL) {
                return false;
            }
            return true;
        }
        case OP_CONST_CLASS: {
            void *classPtr = (void*)
                (method->clazz->pDvmDex->pResClasses[insn->vB]);

            if (classPtr == NULL) {
                return false;
            }
            return true;
        }
        case OP_CONST_STRING_JUMBO:
        case OP_CONST_STRING: {
            void *strPtr = (void*)
                (method->clazz->pDvmDex->pResStrings[insn->vB]);

            if (strPtr == NULL) {
                return false;
            }
            return true;
        }
        default:
            return true;
    }
}

/* Split an existing block from the specified code offset into two */
static BasicBlock *splitBlock(CompilationUnit *cUnit,
                              unsigned int codeOffset,
                              BasicBlock *origBlock,
                              BasicBlock **immedPredBlockP)
{
    MIR *insn = origBlock->firstMIRInsn;
    while (insn) {
        if (insn->offset == codeOffset) break;
        insn = insn->next;
    }
    if (insn == NULL) {
        ALOGE("Break split failed");
        dvmAbort();
    }
    BasicBlock *bottomBlock = dvmCompilerNewBB(kDalvikByteCode,
                                               cUnit->numBlocks++);
    dvmInsertGrowableList(&cUnit->blockList, (intptr_t) bottomBlock);

    bottomBlock->startOffset = codeOffset;
    bottomBlock->firstMIRInsn = insn;
    bottomBlock->lastMIRInsn = origBlock->lastMIRInsn;

    /* Handle the taken path */
    bottomBlock->taken = origBlock->taken;
    if (bottomBlock->taken) {
        origBlock->taken = NULL;
        dvmCompilerClearBit(bottomBlock->taken->predecessors, origBlock->id);
        dvmCompilerSetBit(bottomBlock->taken->predecessors, bottomBlock->id);
    }

    /* Handle the fallthrough path */
    bottomBlock->needFallThroughBranch = origBlock->needFallThroughBranch;
    bottomBlock->fallThrough = origBlock->fallThrough;
    origBlock->fallThrough = bottomBlock;
    origBlock->needFallThroughBranch = true;
    dvmCompilerSetBit(bottomBlock->predecessors, origBlock->id);
    if (bottomBlock->fallThrough) {
        dvmCompilerClearBit(bottomBlock->fallThrough->predecessors,
                            origBlock->id);
        dvmCompilerSetBit(bottomBlock->fallThrough->predecessors,
                          bottomBlock->id);
    }

    /* Handle the successor list */
    if (origBlock->successorBlockList.blockListType != kNotUsed) {
        bottomBlock->successorBlockList = origBlock->successorBlockList;
        origBlock->successorBlockList.blockListType = kNotUsed;
        GrowableListIterator iterator;

        dvmGrowableListIteratorInit(&bottomBlock->successorBlockList.blocks,
                                    &iterator);
        while (true) {
            SuccessorBlockInfo *successorBlockInfo =
                (SuccessorBlockInfo *) dvmGrowableListIteratorNext(&iterator);
            if (successorBlockInfo == NULL) break;
            BasicBlock *bb = successorBlockInfo->block;
            dvmCompilerClearBit(bb->predecessors, origBlock->id);
            dvmCompilerSetBit(bb->predecessors, bottomBlock->id);
        }
    }

    origBlock->lastMIRInsn = insn->prev;

    insn->prev->next = NULL;
    insn->prev = NULL;

    /*
     * Update the immediate predecessor block pointer so that outgoing edges
     * can be applied to the proper block.
     */
    if (immedPredBlockP) {
        assert(*immedPredBlockP == origBlock);
        *immedPredBlockP = bottomBlock;
    }
    return bottomBlock;
}

/*
 * Given a code offset, find out the block that starts with it. If the offset
 * is in the middle of an existing block, split it into two. If immedPredBlockP
 * is non-null and is the block being split, update *immedPredBlockP to point
 * to the bottom block so that outgoing edges can be setup properly (by the
 * caller).
 */
static BasicBlock *findBlock(CompilationUnit *cUnit,
                             unsigned int codeOffset,
                             bool split, bool create,
                             BasicBlock **immedPredBlockP)
{
    GrowableList *blockList = &cUnit->blockList;
    BasicBlock *bb;
    unsigned int i;

    for (i = 0; i < blockList->numUsed; i++) {
        bb = (BasicBlock *) blockList->elemList[i];
        if (bb->blockType != kDalvikByteCode) continue;
        if (bb->startOffset == codeOffset) return bb;
        /* Check if a branch jumps into the middle of an existing block */
        if ((split == true) && (codeOffset > bb->startOffset) &&
            (bb->lastMIRInsn != NULL) &&
            (codeOffset <= bb->lastMIRInsn->offset)) {
            BasicBlock *newBB = splitBlock(cUnit, codeOffset, bb,
                                           bb == *immedPredBlockP ?
                                               immedPredBlockP : NULL);
            return newBB;
        }
    }
    if (create) {
          bb = dvmCompilerNewBB(kDalvikByteCode, cUnit->numBlocks++);
          dvmInsertGrowableList(&cUnit->blockList, (intptr_t) bb);
          bb->startOffset = codeOffset;
          return bb;
    }
    return NULL;
}

/**
  * @brief Request a File creation with a given name
  * @param cUnit the CompilationUnit
  * @param dirPrefix the directory prefix to be used
  * @param suffix a suffix string for the file name
  * @return returns a FILE or NULL if creation failed
  */
static FILE *dvmCompilerDumpGetFile (CompilationUnit *cUnit, const char *dirPrefix, const char *suffix = "")
{
    const Method *method;
    char *signature;
    char id[80];
    char startOffset[80];
    char *fileName;
    int length;

    //Not thread safe
    static int cnt = 0;

    //Start with some paranoid handling:
    if (cUnit == NULL || cUnit->method == NULL || cUnit->method->clazz == NULL)
        return NULL;

    //Create the filename
    method = cUnit->method;
    signature = dexProtoCopyMethodDescriptor(&method->prototype);

    //Add unique counter, and increment it
    snprintf (id, sizeof (id), "_%d", cnt);
    cnt++;

    //Also get minimim startOffset: no offset can be UINT_MAX so it will be our start test
    unsigned int minOffset = UINT_MAX;

    //Iterate through them
    GrowableListIterator iterator;
    dvmGrowableListIteratorInit(&cUnit->blockList, &iterator);
    while (true) {
        BasicBlock *bbscan = (BasicBlock *) dvmGrowableListIteratorNext(&iterator);
        if (bbscan == NULL) break;

        //We only care about code blocks for this
        if (bbscan->blockType != kDalvikByteCode)
        {
            continue;
        }

        //Get offset
        unsigned int tmpOffset = bbscan->startOffset;

        if (minOffset > tmpOffset)
        {
            minOffset = tmpOffset;
        }
    }

    snprintf (startOffset, sizeof (startOffset), "_%x", minOffset);

    length = strlen(dirPrefix) +
                 strlen(method->clazz->descriptor) +
                 strlen(method->name) +
                 strlen(signature) +
                 strlen(id) +
                 strlen(startOffset) +
                 strlen(suffix) +
                 strlen(".dot") + 1;
    fileName = (char *) dvmCompilerNew(length, true);

    snprintf(fileName, length, "%s%s%s%s%s%s%s.dot", dirPrefix,
            method->clazz->descriptor, method->name, signature, id, startOffset, suffix);
    free(signature), signature = NULL;

    /*
     * Convert the special characters into a filesystem- and shell-friendly
     * format.
     */
    int i;
    for (i = strlen(dirPrefix); fileName[i] != '\0'; i++) {
        if (fileName[i] == '/') {
            fileName[i] = '_';
        } else if (fileName[i] == ';') {
            fileName[i] = '#';
        } else if (fileName[i] == '$') {
            fileName[i] = '+';
        } else if (fileName[i] == '(' || fileName[i] == ')') {
            fileName[i] = '@';
        } else if (fileName[i] == '<' || fileName[i] == '>') {
            fileName[i] = '=';
        }
    }

    //Now actually return the file opening
    return fopen(fileName, "w");
}

/**
  * @brief Dump a BasicBlock
  * @param cUnit the CompilationUnit
  * @param bb the BasicBlock
  * @param file the File in which to dump the BasicBlock
  * @param dominators do we wish to dump the domination information? (default == false)
  */
void dvmDumpBasicBlock (CompilationUnit *cUnit, BasicBlock *bb, FILE *file, bool dominators = false)
{
        char blockName[BLOCK_NAME_LEN], bbName[BLOCK_NAME_LEN];

        if (bb == NULL)
            return;

        //Get BB's name
        dvmGetBlockName(bb, bbName);

        if (bb->blockType == kEntryBlock) {
            fprintf(file, "  entry [shape=Mdiamond];\n");
        } else if (bb->blockType == kExitBlock) {
            fprintf(file, "  exit [shape=Mdiamond];\n");
        } else if (bb->blockType == kDalvikByteCode) {
            fprintf(file, "  %s [shape=record,label = \"{ \\\n",
                    bbName);
            const MIR *mir;
            fprintf(file, "    {block id %d\\l}%s\\\n", bb->id,
                    bb->firstMIRInsn ? " | " : " ");
            for (mir = bb->firstMIRInsn; mir; mir = mir->next) {
                const char *tmp = NULL;
                //Prefer the full information with SSA
                if (mir->ssaRep != NULL)
                {
                    tmp = dvmCompilerFullDisassembler(cUnit, mir);
                }
                else
                {
                    //Otherwise, prefer the dvmCompilerGetDalvikDisassembly output
                    tmp = dvmCompilerGetDalvikDisassembly (&(mir->dalvikInsn), NULL);

                    //If all else fails, get the opcode name but only if not extended
                    if (tmp == NULL)
                    {
                        if (static_cast<int> (mir->dalvikInsn.opcode) < static_cast<int> (kMirOpFirst))
                        {
                            tmp = dexGetOpcodeName(mir->dalvikInsn.opcode);
                        }
                        else
                        {
                            ExtendedMIROpcode value = static_cast<ExtendedMIROpcode> (mir->dalvikInsn.opcode);

                            switch (value)
                            {
                                case kMirOpPhi:
                                    tmp = "kMirOpPhi";
                                    break;
                                case kMirOpNullNRangeUpCheck:
                                    tmp = "kMirOpNullNRangeUpCheck";
                                    break;
                                case kMirOpNullNRangeDownCheck:
                                    tmp = "kMirOpNullNRangeDownCheck";
                                    break;
                                case kMirOpLowerBound:
                                    tmp = "kMirOpLowerBound";
                                    break;
                                case kMirOpPunt:
                                    tmp = "kMirPunt";
                                    break;
                                case kMirOpCheckInlinePrediction:
                                    {
                                        int len = 0;
                                        const char *start = "kMirOpCheckInlinePrediction: ";
                                        len = 1 + strlen (start) + strlen (mir->meta.callsiteInfo->classDescriptor);
                                        char *intern = static_cast<char *> (dvmCompilerNew (len, false));

                                        snprintf (intern, len, "%s%s", start, mir->meta.callsiteInfo->classDescriptor);
                                        tmp = intern;
                                    }
                                    break;
                                default:
                                    tmp = "Unknown Extended Opcode";
                                    break;
                            }
                        }
                    }
                }

                fprintf(file, "    {%04x %s\\l}%s\\\n", mir->offset, tmp, mir->next ? " | " : " ");
            }

            //Add information about fallThrough branches
            if (bb->needFallThroughBranch == true && bb->fallThrough != NULL)
            {
                dvmGetBlockName (bb->fallThrough, blockName);
                fprintf (file, "| FallThrough jmp %s", blockName);
            }
            fprintf(file, "  }\"];\n\n");
        } else if (bb->blockType == kExceptionHandling) {
            dvmGetBlockName(bb, blockName);
            fprintf(file, "  %s [shape=invhouse];\n", blockName);
        }

        if (bb->taken) {
            dvmGetBlockName(bb->taken, blockName);
            fprintf(file, "  %s:s -> %s:n [style=dotted]\n",
                    bbName, blockName);
        }
        if (bb->fallThrough) {
            dvmGetBlockName(bb->fallThrough, blockName);
            fprintf(file, "  %s:s -> %s:n\n", bbName, blockName);
        }

        if (bb->successorBlockList.blockListType != kNotUsed) {
            fprintf(file, "  succ%04x [shape=%s,label = \"{ \\\n",
                    bb->id,
                    (bb->successorBlockList.blockListType == kCatch) ?
                        "Mrecord" : "record");
            GrowableListIterator iterator;
            dvmGrowableListIteratorInit(&bb->successorBlockList.blocks,
                                        &iterator);
            SuccessorBlockInfo *successorBlockInfo =
                (SuccessorBlockInfo *) dvmGrowableListIteratorNext(&iterator);

            int succId = 0;
            while (true) {
                if (successorBlockInfo == NULL) break;

                BasicBlock *destBlock = successorBlockInfo->block;
                SuccessorBlockInfo *nextSuccessorBlockInfo =
                  (SuccessorBlockInfo *) dvmGrowableListIteratorNext(&iterator);

                fprintf(file, "    {<f%d> %04x: %04x\\l}%s\\\n",
                        succId++,
                        successorBlockInfo->key,
                        destBlock->id,
                        (nextSuccessorBlockInfo != NULL) ? " | " : " ");

                successorBlockInfo = nextSuccessorBlockInfo;
            }
            fprintf(file, "  }\"];\n\n");

            fprintf(file, "  %s:s -> succ%04x:n [style=dashed]\n",
                    bbName, bb->id);

            if (bb->successorBlockList.blockListType == kPackedSwitch ||
                bb->successorBlockList.blockListType == kSparseSwitch) {

                dvmGrowableListIteratorInit(&bb->successorBlockList.blocks,
                                            &iterator);

                succId = 0;
                while (true) {
                    SuccessorBlockInfo *successorBlockInfo =
                        (SuccessorBlockInfo *)
                            dvmGrowableListIteratorNext(&iterator);
                    if (successorBlockInfo == NULL) break;

                    BasicBlock *destBlock = successorBlockInfo->block;

                    dvmGetBlockName(destBlock, blockName);
                    fprintf(file, "  succ%04x:f%d:e -> %s:n\n",
                            bb->id, succId++,
                            blockName);
                }
            }
        }
        fprintf(file, "\n");

        /*
         * If we need to debug the dominator tree
         */
        if (dominators == true)
        {
            fprintf(file, "  cfg%s [label=\"%s\", shape=none];\n",
                    bbName, bbName);
            if (bb->iDom) {
                dvmGetBlockName(bb->iDom, blockName);
                fprintf(file, "  cfg%s:s -> cfg%s:n\n\n",
                        blockName, bbName);
            }
        }
}

/**
  * @brief Dump the CFG into a DOT graph
  * @param cUnit the CompilationUnit
  * @param dirPrefix the directory prefix to be used
  * @param suffix a suffix string for the file name
  */
void dvmDumpCFG(CompilationUnit *cUnit, const char *dirPrefix, const char *suffix)
{
    FILE *file = dvmCompilerDumpGetFile (cUnit, dirPrefix, suffix);

    if (file == NULL)
        return;

    fprintf(file, "digraph G {\n");

    fprintf(file, "  rankdir=TB\n");

    int numReachableBlocks = cUnit->numReachableBlocks;
    int idx;
    const GrowableList *blockList = &cUnit->blockList;

    //Dump only the reachable basic blocks
    for (idx = 0; idx < numReachableBlocks; idx++) {
        int blockIdx = cUnit->dfsOrder.elemList[idx];
        BasicBlock *bb = (BasicBlock *) dvmGrowableListGetElement(blockList,
                                                                  blockIdx);
        dvmDumpBasicBlock (cUnit, bb, file);
    }
    fprintf(file, "}\n");
    fclose(file);
}

/* It's ugly but it is the best method available */
static FILE *dvmCreateGraphFile = NULL;
/**
 * @brief Handler for the BasicBlock dumping into a DOT graph, if the block is visted already, do nothing
 * @param cUnit the CompilationUnit
 * @param curBlock current block to be dumped
 * @return whether or not this changes anything for the walker
 */
static bool dvmCompilerDumpBasicBlockHandler (CompilationUnit *cUnit, BasicBlock *curBlock)
{
    //Paranoid
    if (dvmCreateGraphFile == NULL)
        return false;

    //If visited, then we have already dumped it, therefore nothing to do
    if (curBlock->visited == true)
        return false;
    curBlock->visited = true;

    //Dump the basic block
    dvmDumpBasicBlock (cUnit, curBlock, dvmCreateGraphFile);

    //We did no change
    return false;
}


/**
 * @brief Dump the CFG of every BasicBlock into a DOT graph
 * @param cUnit the CompilationUnit
 * @param dirPrefix the directory prefix to be used
 * @param suffix a suffix string for the file name
 */
void dvmCompilerDumpBasicBlocks (CompilationUnit *cUnit, const char *dirPrefix, const char *suffix)
{
    //Clear visiting flags
    dvmCompilerDataFlowAnalysisDispatcher(cUnit,
            dvmCompilerClearVisitedFlag,
            kAllNodes,
            false /* isIterative */);

    dvmCreateGraphFile = dvmCompilerDumpGetFile (cUnit, dirPrefix, suffix);

    //Paranoid
    if (dvmCreateGraphFile != NULL)
    {
        //Prepare dot header
        fprintf (dvmCreateGraphFile, "digraph G {\n");

        //Dump the basic blocks
        dvmCompilerDataFlowAnalysisDispatcher (cUnit,
                dvmCompilerDumpBasicBlockHandler,
                kAllNodes,
                false);

        //Print out epilogue
        fprintf (dvmCreateGraphFile, "}\n");

        //Close file
        fclose (dvmCreateGraphFile), dvmCreateGraphFile = NULL;
    }

    //Clear visiting flags
    dvmCompilerDataFlowAnalysisDispatcher(cUnit,
            dvmCompilerClearVisitedFlag,
            kAllNodes,
            false /* isIterative */);
}


/* Verify if all the successor is connected with all the claimed predecessors */
static bool verifyPredInfo(CompilationUnit *cUnit, BasicBlock *bb)
{
    BitVectorIterator bvIterator;

    dvmBitVectorIteratorInit(bb->predecessors, &bvIterator);
    while (true) {
        int blockIdx = dvmBitVectorIteratorNext(&bvIterator);
        if (blockIdx == -1) break;
        BasicBlock *predBB = (BasicBlock *)
            dvmGrowableListGetElement(&cUnit->blockList, blockIdx);
        bool found = false;
        if (predBB->taken == bb) {
            found = true;
        } else if (predBB->fallThrough == bb) {
            found = true;
        } else if (predBB->successorBlockList.blockListType != kNotUsed) {
            GrowableListIterator iterator;
            dvmGrowableListIteratorInit(&predBB->successorBlockList.blocks,
                                        &iterator);
            while (true) {
                SuccessorBlockInfo *successorBlockInfo =
                    (SuccessorBlockInfo *)
                        dvmGrowableListIteratorNext(&iterator);
                if (successorBlockInfo == NULL) break;
                BasicBlock *succBB = successorBlockInfo->block;
                if (succBB == bb) {
                    found = true;
                    break;
                }
            }
        }
        if (found == false) {
            char blockName1[BLOCK_NAME_LEN], blockName2[BLOCK_NAME_LEN];
            dvmGetBlockName(bb, blockName1);
            dvmGetBlockName(predBB, blockName2);
            dvmDumpCFG(cUnit, "/sdcard/cfg/");
            ALOGE("Successor %s not found from %s",
                 blockName1, blockName2);
            dvmAbort();
        }
    }
    return true;
}

/* Identify code range in try blocks and set up the empty catch blocks */
static void processTryCatchBlocks(CompilationUnit *cUnit)
{
    const Method *meth = cUnit->method;
    const DexCode *pCode = dvmGetMethodCode(meth);
    int triesSize = pCode->triesSize;
    int i;
    int offset;

    if (triesSize == 0) {
        return;
    }

    const DexTry *pTries = dexGetTries(pCode);
    BitVector *tryBlockAddr = cUnit->tryBlockAddr;

    /* Mark all the insn offsets in Try blocks */
    for (i = 0; i < triesSize; i++) {
        const DexTry* pTry = &pTries[i];
        /* all in 16-bit units */
        int startOffset = pTry->startAddr;
        int endOffset = startOffset + pTry->insnCount;

        for (offset = startOffset; offset < endOffset; offset++) {
            dvmCompilerSetBit(tryBlockAddr, offset);
        }
    }

    /* Iterate over each of the handlers to enqueue the empty Catch blocks */
    offset = dexGetFirstHandlerOffset(pCode);
    int handlersSize = dexGetHandlersSize(pCode);

    for (i = 0; i < handlersSize; i++) {
        DexCatchIterator iterator;
        dexCatchIteratorInit(&iterator, pCode, offset);

        for (;;) {
            DexCatchHandler* handler = dexCatchIteratorNext(&iterator);

            if (handler == NULL) {
                break;
            }

            /*
             * Create dummy catch blocks first. Since these are created before
             * other blocks are processed, "split" is specified as false.
             */
            findBlock(cUnit, handler->address,
                      /* split */
                      false,
                      /* create */
                      true,
                      /* immedPredBlockP */
                      NULL);
        }

        offset = dexCatchIteratorGetEndOffset(&iterator, pCode);
    }
}

/* Process instructions with the kInstrCanBranch flag */
static void processCanBranch(CompilationUnit *cUnit, BasicBlock *curBlock,
                             MIR *insn, int curOffset, int width, int flags,
                             const u2* codePtr, const u2* codeEnd)
{
    int target = curOffset;
    switch (insn->dalvikInsn.opcode) {
        case OP_GOTO:
        case OP_GOTO_16:
        case OP_GOTO_32:
            target += (int) insn->dalvikInsn.vA;
            break;
        case OP_IF_EQ:
        case OP_IF_NE:
        case OP_IF_LT:
        case OP_IF_GE:
        case OP_IF_GT:
        case OP_IF_LE:
            target += (int) insn->dalvikInsn.vC;
            break;
        case OP_IF_EQZ:
        case OP_IF_NEZ:
        case OP_IF_LTZ:
        case OP_IF_GEZ:
        case OP_IF_GTZ:
        case OP_IF_LEZ:
            target += (int) insn->dalvikInsn.vB;
            break;
        default:
            ALOGE("Unexpected opcode(%d) with kInstrCanBranch set",
                 insn->dalvikInsn.opcode);
            dvmAbort();
    }
    BasicBlock *takenBlock = findBlock(cUnit, target,
                                       /* split */
                                       true,
                                       /* create */
                                       true,
                                       /* immedPredBlockP */
                                       &curBlock);
    curBlock->taken = takenBlock;
    dvmCompilerSetBit(takenBlock->predecessors, curBlock->id);

    /* Always terminate the current block for conditional branches */
    if (flags & kInstrCanContinue) {
        BasicBlock *fallthroughBlock = findBlock(cUnit,
                                                 curOffset +  width,
                                                 /*
                                                  * If the method is processed
                                                  * in sequential order from the
                                                  * beginning, we don't need to
                                                  * specify split for continue
                                                  * blocks. However, this
                                                  * routine can be called by
                                                  * compileLoop, which starts
                                                  * parsing the method from an
                                                  * arbitrary address in the
                                                  * method body.
                                                  */
                                                 true,
                                                 /* create */
                                                 true,
                                                 /* immedPredBlockP */
                                                 &curBlock);
        curBlock->fallThrough = fallthroughBlock;
        dvmCompilerSetBit(fallthroughBlock->predecessors, curBlock->id);
    } else if (codePtr < codeEnd) {
        /* Create a fallthrough block for real instructions (incl. OP_NOP) */
        if (contentIsInsn(codePtr)) {
            findBlock(cUnit, curOffset + width,
                      /* split */
                      false,
                      /* create */
                      true,
                      /* immedPredBlockP */
                      NULL);
        }
    }
}

/* Process instructions with the kInstrCanSwitch flag */
static void processCanSwitch(CompilationUnit *cUnit, BasicBlock *curBlock,
                             MIR *insn, int curOffset, int width, int flags)
{
    u2 *switchData= (u2 *) (cUnit->method->insns + curOffset +
                            insn->dalvikInsn.vB);
    int size;
    int *keyTable;
    int *targetTable;
    int i;
    int firstKey;

    /*
     * Packed switch data format:
     *  ushort ident = 0x0100   magic value
     *  ushort size             number of entries in the table
     *  int first_key           first (and lowest) switch case value
     *  int targets[size]       branch targets, relative to switch opcode
     *
     * Total size is (4+size*2) 16-bit code units.
     */
    if (insn->dalvikInsn.opcode == OP_PACKED_SWITCH) {
        assert(switchData[0] == kPackedSwitchSignature);
        size = switchData[1];
        firstKey = switchData[2] | (switchData[3] << 16);
        targetTable = (int *) &switchData[4];
        keyTable = NULL;        // Make the compiler happy
    /*
     * Sparse switch data format:
     *  ushort ident = 0x0200   magic value
     *  ushort size             number of entries in the table; > 0
     *  int keys[size]          keys, sorted low-to-high; 32-bit aligned
     *  int targets[size]       branch targets, relative to switch opcode
     *
     * Total size is (2+size*4) 16-bit code units.
     */
    } else {
        assert(switchData[0] == kSparseSwitchSignature);
        size = switchData[1];
        keyTable = (int *) &switchData[2];
        targetTable = (int *) &switchData[2 + size*2];
        firstKey = 0;   // To make the compiler happy
    }

    if (curBlock->successorBlockList.blockListType != kNotUsed) {
        ALOGE("Successor block list already in use: %d",
             curBlock->successorBlockList.blockListType);
        dvmAbort();
    }
    curBlock->successorBlockList.blockListType =
        (insn->dalvikInsn.opcode == OP_PACKED_SWITCH) ?
        kPackedSwitch : kSparseSwitch;
    dvmInitGrowableList(&curBlock->successorBlockList.blocks, size);

    for (i = 0; i < size; i++) {
        BasicBlock *caseBlock = findBlock(cUnit, curOffset + targetTable[i],
                                          /* split */
                                          true,
                                          /* create */
                                          true,
                                          /* immedPredBlockP */
                                          &curBlock);
        SuccessorBlockInfo *successorBlockInfo =
            (SuccessorBlockInfo *) dvmCompilerNew(sizeof(SuccessorBlockInfo),
                                                  false);
        successorBlockInfo->block = caseBlock;
        successorBlockInfo->key = (insn->dalvikInsn.opcode == OP_PACKED_SWITCH)?
                                  firstKey + i : keyTable[i];
        dvmInsertGrowableList(&curBlock->successorBlockList.blocks,
                              (intptr_t) successorBlockInfo);
        dvmCompilerSetBit(caseBlock->predecessors, curBlock->id);
    }

    /* Fall-through case */
    BasicBlock *fallthroughBlock = findBlock(cUnit,
                                             curOffset +  width,
                                             /* split */
                                             false,
                                             /* create */
                                             true,
                                             /* immedPredBlockP */
                                             NULL);
    curBlock->fallThrough = fallthroughBlock;
    dvmCompilerSetBit(fallthroughBlock->predecessors, curBlock->id);
}

/* Process instructions with the kInstrCanThrow flag */
static void processCanThrow(CompilationUnit *cUnit, BasicBlock *curBlock,
                            MIR *insn, int curOffset, int width, int flags,
                            BitVector *tryBlockAddr, const u2 *codePtr,
                            const u2* codeEnd)
{
    const Method *method = cUnit->method;
    const DexCode *dexCode = dvmGetMethodCode(method);

    /* In try block */
    if (dvmIsBitSet(tryBlockAddr, curOffset)) {
        DexCatchIterator iterator;

        if (!dexFindCatchHandler(&iterator, dexCode, curOffset)) {
            ALOGE("Catch block not found in dexfile for insn %x in %s",
                 curOffset, method->name);
            dvmAbort();

        }
        if (curBlock->successorBlockList.blockListType != kNotUsed) {
            ALOGE("Successor block list already in use: %d",
                 curBlock->successorBlockList.blockListType);
            dvmAbort();
        }
        curBlock->successorBlockList.blockListType = kCatch;
        dvmInitGrowableList(&curBlock->successorBlockList.blocks, 2);

        for (;;) {
            DexCatchHandler* handler = dexCatchIteratorNext(&iterator);

            if (handler == NULL) {
                break;
            }

            BasicBlock *catchBlock = findBlock(cUnit, handler->address,
                                               /* split */
                                               false,
                                               /* create */
                                               false,
                                               /* immedPredBlockP */
                                               NULL);

            SuccessorBlockInfo *successorBlockInfo =
              (SuccessorBlockInfo *) dvmCompilerNew(sizeof(SuccessorBlockInfo),
                                                    false);
            successorBlockInfo->block = catchBlock;
            successorBlockInfo->key = handler->typeIdx;
            dvmInsertGrowableList(&curBlock->successorBlockList.blocks,
                                  (intptr_t) successorBlockInfo);
            dvmCompilerSetBit(catchBlock->predecessors, curBlock->id);
        }
    } else {
        BasicBlock *ehBlock = dvmCompilerNewBB(kExceptionHandling,
                                               cUnit->numBlocks++);
        curBlock->taken = ehBlock;
        dvmInsertGrowableList(&cUnit->blockList, (intptr_t) ehBlock);
        ehBlock->startOffset = curOffset;
        dvmCompilerSetBit(ehBlock->predecessors, curBlock->id);
    }

    /*
     * Force the current block to terminate.
     *
     * Data may be present before codeEnd, so we need to parse it to know
     * whether it is code or data.
     */
    if (codePtr < codeEnd) {
        /* Create a fallthrough block for real instructions (incl. OP_NOP) */
        if (contentIsInsn(codePtr)) {
            BasicBlock *fallthroughBlock = findBlock(cUnit,
                                                     curOffset + width,
                                                     /* split */
                                                     false,
                                                     /* create */
                                                     true,
                                                     /* immedPredBlockP */
                                                     NULL);
            /*
             * OP_THROW and OP_THROW_VERIFICATION_ERROR are unconditional
             * branches.
             */
            if (insn->dalvikInsn.opcode != OP_THROW_VERIFICATION_ERROR &&
                insn->dalvikInsn.opcode != OP_THROW) {
                curBlock->fallThrough = fallthroughBlock;
                dvmCompilerSetBit(fallthroughBlock->predecessors, curBlock->id);
            }
        }
    }
}

/*
 * Similar to dvmCompileTrace, but the entity processed here is the whole
 * method.
 *
 * TODO: implementation will be revisited when the trace builder can provide
 * whole-method traces.
 */
bool dvmCompileMethod(const Method *method, JitTranslationInfo *info)
{
    CompilationUnit cUnit;
    const DexCode *dexCode = dvmGetMethodCode(method);
    const u2 *codePtr = dexCode->insns;
    const u2 *codeEnd = dexCode->insns + dexCode->insnsSize;
    int numBlocks = 0;
    unsigned int curOffset = 0;

    /* Method already compiled */
    if (dvmJitGetMethodAddr(codePtr)) {
        info->codeAddress = NULL;
        return false;
    }

    memset(&cUnit, 0, sizeof(cUnit));
    cUnit.method = method;

    cUnit.jitMode = kJitMethod;

    /* Initialize the block list */
    dvmInitGrowableList(&cUnit.blockList, 4);

    /*
     * FIXME - PC reconstruction list won't be needed after the codegen routines
     * are enhanced to true method mode.
     */
    /* Initialize the PC reconstruction list */
    dvmInitGrowableList(&cUnit.pcReconstructionList, 8);

    /* Allocate the bit-vector to track the beginning of basic blocks */
    BitVector *tryBlockAddr = dvmCompilerAllocBitVector(dexCode->insnsSize,
                                                        true /* expandable */);
    cUnit.tryBlockAddr = tryBlockAddr;

    /* Create the default entry and exit blocks and enter them to the list */
    BasicBlock *entryBlock = dvmCompilerNewBB(kEntryBlock, numBlocks++);
    BasicBlock *exitBlock = dvmCompilerNewBB(kExitBlock, numBlocks++);

    cUnit.entryBlock = entryBlock;
    cUnit.exitBlock = exitBlock;

    dvmInsertGrowableList(&cUnit.blockList, (intptr_t) entryBlock);
    dvmInsertGrowableList(&cUnit.blockList, (intptr_t) exitBlock);

    /* Current block to record parsed instructions */
    BasicBlock *curBlock = dvmCompilerNewBB(kDalvikByteCode, numBlocks++);
    curBlock->startOffset = 0;
    dvmInsertGrowableList(&cUnit.blockList, (intptr_t) curBlock);
    entryBlock->fallThrough = curBlock;
    dvmCompilerSetBit(curBlock->predecessors, entryBlock->id);

    /*
     * Store back the number of blocks since new blocks may be created of
     * accessing cUnit.
     */
    cUnit.numBlocks = numBlocks;

    /* Identify code range in try blocks and set up the empty catch blocks */
    processTryCatchBlocks(&cUnit);

    /* Parse all instructions and put them into containing basic blocks */
    while (codePtr < codeEnd) {
        MIR *insn = (MIR *) dvmCompilerNew(sizeof(MIR), true);
        insn->offset = curOffset;
        int width = parseInsn(codePtr, &insn->dalvikInsn, false);
        insn->width = width;

        /* Terminate when the data section is seen */
        if (width == 0)
            break;

        dvmCompilerAppendMIR(curBlock, insn);

        codePtr += width;
        int flags = dexGetFlagsFromOpcode(insn->dalvikInsn.opcode);

        if (flags & kInstrCanBranch) {
            processCanBranch(&cUnit, curBlock, insn, curOffset, width, flags,
                             codePtr, codeEnd);
        } else if (flags & kInstrCanReturn) {
            curBlock->fallThrough = exitBlock;
            dvmCompilerSetBit(exitBlock->predecessors, curBlock->id);
            /*
             * Terminate the current block if there are instructions
             * afterwards.
             */
            if (codePtr < codeEnd) {
                /*
                 * Create a fallthrough block for real instructions
                 * (incl. OP_NOP).
                 */
                if (contentIsInsn(codePtr)) {
                    findBlock(&cUnit, curOffset + width,
                              /* split */
                              false,
                              /* create */
                              true,
                              /* immedPredBlockP */
                              NULL);
                }
            }
        } else if (flags & kInstrCanThrow) {
            processCanThrow(&cUnit, curBlock, insn, curOffset, width, flags,
                            tryBlockAddr, codePtr, codeEnd);
        } else if (flags & kInstrCanSwitch) {
            processCanSwitch(&cUnit, curBlock, insn, curOffset, width, flags);
        }
        curOffset += width;
        BasicBlock *nextBlock = findBlock(&cUnit, curOffset,
                                          /* split */
                                          false,
                                          /* create */
                                          false,
                                          /* immedPredBlockP */
                                          NULL);
        if (nextBlock) {
            /*
             * The next instruction could be the target of a previously parsed
             * forward branch so a block is already created. If the current
             * instruction is not an unconditional branch, connect them through
             * the fall-through link.
             */
            assert(curBlock->fallThrough == NULL ||
                   curBlock->fallThrough == nextBlock ||
                   curBlock->fallThrough == exitBlock);

            if ((curBlock->fallThrough == NULL) &&
                (flags & kInstrCanContinue)) {
                curBlock->fallThrough = nextBlock;
                dvmCompilerSetBit(nextBlock->predecessors, curBlock->id);
            }
            curBlock = nextBlock;
        }
    }

    if (cUnit.printMe) {
        dvmCompilerDumpCompilationUnit(&cUnit);
    }

    /* Adjust this value accordingly once inlining is performed */
    cUnit.numDalvikRegisters = cUnit.method->registersSize;

    /* Verify if all blocks are connected as claimed */
    /* FIXME - to be disabled in the future */
    dvmCompilerDataFlowAnalysisDispatcher(&cUnit, verifyPredInfo,
                                          kAllNodes,
                                          false /* isIterative */);


    /* Perform SSA transformation for the whole method */
    dvmCompilerMethodSSATransformation(&cUnit);

#ifndef ARCH_IA32
    dvmCompilerInitializeRegAlloc(&cUnit);  // Needs to happen after SSA naming

    /* Allocate Registers using simple local allocation scheme */
    dvmCompilerLocalRegAlloc(&cUnit);
#endif

    /* Convert MIR to LIR, etc. */
    dvmCompilerMethodMIR2LIR(&cUnit);

    // Debugging only
    //dvmDumpCFG(&cUnit, "/sdcard/cfg/");

    /* Method is not empty */
    if (cUnit.firstLIRInsn) {
        /* Convert LIR into machine code. Loop for recoverable retries */
        do {
            dvmCompilerAssembleLIR(&cUnit, info);
            cUnit.assemblerRetries++;
            if (cUnit.printMe && cUnit.assemblerStatus != kSuccess)
                ALOGD("Assembler abort #%d on %d",cUnit.assemblerRetries,
                      cUnit.assemblerStatus);
        } while (cUnit.assemblerStatus == kRetryAll);

        if (cUnit.printMe) {
            dvmCompilerCodegenDump(&cUnit);
        }

        if (info->codeAddress) {
            dvmJitSetCodeAddr(dexCode->insns, info->codeAddress,
                              info->instructionSet, true, 0);
            /*
             * Clear the codeAddress for the enclosing trace to reuse the info
             */
            info->codeAddress = NULL;
        }
    }

    return false;
}

/* Extending the trace by crawling the code from curBlock */
static bool exhaustTrace(CompilationUnit *cUnit, BasicBlock *curBlock)
{
    unsigned int curOffset = curBlock->startOffset;
    const u2 *codePtr = cUnit->method->insns + curOffset;

    if (curBlock->visited == true) return false;

    curBlock->visited = true;

    if (curBlock->blockType == kEntryBlock ||
        curBlock->blockType == kExitBlock) {
        return false;
    }

    /*
     * Block has been parsed - check the taken/fallThrough in case it is a split
     * block.
     */
    if (curBlock->firstMIRInsn != NULL) {
          bool changed = false;
          if (curBlock->taken)
              changed |= exhaustTrace(cUnit, curBlock->taken);
          if (curBlock->fallThrough)
              changed |= exhaustTrace(cUnit, curBlock->fallThrough);
          return changed;
    }
    while (true) {
        MIR *insn = (MIR *) dvmCompilerNew(sizeof(MIR), true);
        insn->offset = curOffset;
        int width = parseInsn(codePtr, &insn->dalvikInsn, false);
        insn->width = width;

        /* Terminate when the data section is seen */
        if (width == 0)
            break;

        dvmCompilerAppendMIR(curBlock, insn);

        codePtr += width;
        int flags = dexGetFlagsFromOpcode(insn->dalvikInsn.opcode);

        /* Stop extending the trace after seeing these instructions */
        if (flags & (kInstrCanReturn | kInstrCanSwitch | kInstrInvoke)) {
            curBlock->fallThrough = cUnit->exitBlock;
            dvmCompilerSetBit(cUnit->exitBlock->predecessors, curBlock->id);
            break;
        } else if (flags & kInstrCanBranch) {
            processCanBranch(cUnit, curBlock, insn, curOffset, width, flags,
                             codePtr, NULL);
            if (curBlock->taken) {
                exhaustTrace(cUnit, curBlock->taken);
            }
            if (curBlock->fallThrough) {
                exhaustTrace(cUnit, curBlock->fallThrough);
            }
            break;
        }
        curOffset += width;
        BasicBlock *nextBlock = findBlock(cUnit, curOffset,
                                          /* split */
                                          false,
                                          /* create */
                                          false,
                                          /* immedPredBlockP */
                                          NULL);
        if (nextBlock) {
            /*
             * The next instruction could be the target of a previously parsed
             * forward branch so a block is already created. If the current
             * instruction is not an unconditional branch, connect them through
             * the fall-through link.
             */
            assert(curBlock->fallThrough == NULL ||
                   curBlock->fallThrough == nextBlock ||
                   curBlock->fallThrough == cUnit->exitBlock);

            if ((curBlock->fallThrough == NULL) &&
                (flags & kInstrCanContinue)) {
                curBlock->needFallThroughBranch = true;
                curBlock->fallThrough = nextBlock;
                dvmCompilerSetBit(nextBlock->predecessors, curBlock->id);
            }
            /* Block has been visited - no more parsing needed */
            if (nextBlock->visited == true) {
                return true;
            }
            curBlock = nextBlock;
        }
    }
    return true;
}

/* Compile a loop */
static bool compileLoop(CompilationUnit *cUnit, unsigned int startOffset,
                        JitTraceDescription *desc, int numMaxInsts,
                        JitTranslationInfo *info, jmp_buf *bailPtr,
                        int optHints)
{
    int numBlocks = 0;
    unsigned int curOffset = startOffset;
    bool changed;
    BasicBlock *bb;
#if defined(WITH_JIT_TUNING)
    CompilerMethodStats *methodStats;
#endif

    cUnit->jitMode = kJitLoop;

    /* Initialize the block list */
    dvmInitGrowableList(&cUnit->blockList, 4);

    /* Initialize the PC reconstruction list */
    dvmInitGrowableList(&cUnit->pcReconstructionList, 8);

    /* Create the default entry and exit blocks and enter them to the list */
    BasicBlock *entryBlock = dvmCompilerNewBB(kEntryBlock, numBlocks++);
    entryBlock->startOffset = curOffset;
    BasicBlock *exitBlock = dvmCompilerNewBB(kExitBlock, numBlocks++);

    cUnit->entryBlock = entryBlock;
    cUnit->exitBlock = exitBlock;

    dvmInsertGrowableList(&cUnit->blockList, (intptr_t) entryBlock);
    dvmInsertGrowableList(&cUnit->blockList, (intptr_t) exitBlock);

    /* Current block to record parsed instructions */
    BasicBlock *curBlock = dvmCompilerNewBB(kDalvikByteCode, numBlocks++);
    curBlock->startOffset = curOffset;

    dvmInsertGrowableList(&cUnit->blockList, (intptr_t) curBlock);
    entryBlock->fallThrough = curBlock;
    dvmCompilerSetBit(curBlock->predecessors, entryBlock->id);

    /*
     * Store back the number of blocks since new blocks may be created of
     * accessing cUnit.
     */
    cUnit->numBlocks = numBlocks;

    do {
        dvmCompilerDataFlowAnalysisDispatcher(cUnit,
                                              dvmCompilerClearVisitedFlag,
                                              kAllNodes,
                                              false /* isIterative */);
        changed = exhaustTrace(cUnit, curBlock);
    } while (changed);

#ifdef BYTECODE_FILTER
    GrowableListIterator iterator;
    MIR *insn;
    dvmGrowableListIteratorInit(&cUnit->blockList, &iterator);
    while (true) {
        BasicBlock *bbscan = (BasicBlock *) dvmGrowableListIteratorNext(&iterator);
        if (bbscan == NULL) break;
        if (bbscan->blockType == kDalvikByteCode) {
            for (insn = bbscan->firstMIRInsn; insn; insn = insn->next)
                if (!dvmIsOpcodeSupportedByJit(insn->dalvikInsn))
                    goto bail;
        }
    }
#endif

    /* Backward chaining block */
    bb = dvmCompilerNewBB(kChainingCellBackwardBranch, cUnit->numBlocks++);
    dvmInsertGrowableList(&cUnit->blockList, (intptr_t) bb);
    cUnit->backChainBlock = bb;

    /* A special block to host PC reconstruction code */
    bb = dvmCompilerNewBB(kPCReconstruction, cUnit->numBlocks++);
    dvmInsertGrowableList(&cUnit->blockList, (intptr_t) bb);

    /* And one final block that publishes the PC and raises the exception */
    bb = dvmCompilerNewBB(kExceptionHandling, cUnit->numBlocks++);
    dvmInsertGrowableList(&cUnit->blockList, (intptr_t) bb);
    cUnit->puntBlock = bb;

    cUnit->numDalvikRegisters = cUnit->method->registersSize;

    /* Verify if all blocks are connected as claimed */
    /* FIXME - to be disabled in the future */
    dvmCompilerDataFlowAnalysisDispatcher(cUnit, verifyPredInfo,
                                          kAllNodes,
                                          false /* isIterative */);


    /* Try to identify a loop */
    if (!dvmCompilerBuildLoop(cUnit))
        goto bail;

    dvmCompilerLoopOpt(cUnit);

    /*
     * Change the backward branch to the backward chaining cell after dataflow
     * analsys/optimizations are done.
     */
    dvmCompilerInsertBackwardChaining(cUnit);

#if defined(ARCH_IA32)
    /* Convert MIR to LIR, etc. */
    dvmCompilerMIR2LIR(cUnit, info);
#else
    dvmCompilerInitializeRegAlloc(cUnit);

    /* Allocate Registers using simple local allocation scheme */
    dvmCompilerLocalRegAlloc(cUnit);

    /* Convert MIR to LIR, etc. */
    dvmCompilerMIR2LIR(cUnit);
#endif

    /* Loop contains never executed blocks / heavy instructions */
    if (cUnit->quitLoopMode) {
        if (cUnit->printMe || gDvmJit.receivedSIGUSR2) {
            ALOGD("Loop trace @ offset %04x aborted due to unresolved code info",
                 cUnit->entryBlock->startOffset);
        }
        goto bail;
    }

    /* Convert LIR into machine code. Loop for recoverable retries */
    do {
        dvmCompilerAssembleLIR(cUnit, info);
        cUnit->assemblerRetries++;
        if (cUnit->printMe && cUnit->assemblerStatus != kSuccess)
            ALOGD("Assembler abort #%d on %d", cUnit->assemblerRetries,
                  cUnit->assemblerStatus);
    } while (cUnit->assemblerStatus == kRetryAll);

    /* Loop is too big - bail out */
    if (cUnit->assemblerStatus == kRetryHalve) {
        goto bail;
    }

    if (cUnit->printMe || gDvmJit.receivedSIGUSR2) {
        ALOGD("Loop trace @ offset %04x", cUnit->entryBlock->startOffset);
        dvmCompilerCodegenDump(cUnit);
    }

    /*
     * If this trace uses class objects as constants,
     * dvmJitInstallClassObjectPointers will switch the thread state
     * to running and look up the class pointers using the descriptor/loader
     * tuple stored in the callsite info structure. We need to make this window
     * as short as possible since it is blocking GC.
     */
    if (cUnit->hasClassLiterals && info->codeAddress) {
        dvmJitInstallClassObjectPointers(cUnit, (char *) info->codeAddress);
    }

    /*
     * Since callsiteinfo is allocated from the arena, delay the reset until
     * class pointers are resolved.
     */
    dvmCompilerArenaReset();

    assert(cUnit->assemblerStatus == kSuccess);
#if defined(WITH_JIT_TUNING)
    /* Locate the entry to store compilation statistics for this method */
    methodStats = dvmCompilerAnalyzeMethodBody(desc->method, false);
    methodStats->nativeSize += cUnit->totalSize;
#endif

#if defined(VTUNE_DALVIK)
    /* Send the loop trace information to the VTune */
    if(gDvmJit.vtuneInfo != kVTuneInfoDisabled) {
        if(info->codeAddress) {
            SendTraceInfoToVTune(cUnit, desc);
        } else {
            LOGD("Invalid trace\n");
        }
    }
#endif

    return info->codeAddress != NULL;

bail:
    /* Retry the original trace with JIT_OPT_NO_LOOP disabled */
    dvmCompilerArenaReset();
    return dvmCompileTrace(desc, numMaxInsts, info, bailPtr,
                           optHints | JIT_OPT_NO_LOOP);
}

static bool searchClassTablePrefix(const Method* method) {
    if (gDvmJit.classTable == NULL) {
        return false;
    }
    HashIter iter;
    HashTable* pTab = gDvmJit.classTable;
    for (dvmHashIterBegin(pTab, &iter); !dvmHashIterDone(&iter);
        dvmHashIterNext(&iter))
    {
        const char* str = (const char*) dvmHashIterData(&iter);
        if (strncmp(method->clazz->descriptor, str, strlen(str)) == 0) {
            return true;
        }
    }
    return false;
}

/*
 * Main entry point to start trace compilation. Basic blocks are constructed
 * first and they will be passed to the codegen routines to convert Dalvik
 * bytecode into machine code.
 */
bool dvmCompileTrace(JitTraceDescription *desc, int numMaxInsts,
                     JitTranslationInfo *info, jmp_buf *bailPtr,
                     int optHints)
{
    const DexCode *dexCode = dvmGetMethodCode(desc->method);
    const JitTraceRun* currRun = &desc->trace[0];
    unsigned int curOffset = currRun->info.frag.startOffset;
    unsigned int startOffset = curOffset;
    unsigned int numInsts = currRun->info.frag.numInsts;
    const u2 *codePtr = dexCode->insns + curOffset;
    int traceSize = 0;  // # of half-words
    const u2 *startCodePtr = codePtr;
    BasicBlock *curBB, *entryCodeBB;
    int numBlocks = 0;
    static int compilationId;
    CompilationUnit cUnit;
    GrowableList *blockList;
#if defined(WITH_JIT_TUNING)
    CompilerMethodStats *methodStats;
#endif

    /* If we've already compiled this trace, just return success */
    if (dvmJitGetTraceAddr(startCodePtr) && !info->discardResult) {
        /*
         * Make sure the codeAddress is NULL so that it won't clobber the
         * existing entry.
         */
        info->codeAddress = NULL;
        return true;
    }

    /* If the work order is stale, discard it */
    if (info->cacheVersion != gDvmJit.cacheVersion) {
        return false;
    }

    compilationId++;
    memset(&cUnit, 0, sizeof(CompilationUnit));

#if defined(WITH_JIT_TUNING)
    /* Locate the entry to store compilation statistics for this method */
    methodStats = dvmCompilerAnalyzeMethodBody(desc->method, false);
#endif

    /* Set the recover buffer pointer */
    cUnit.bailPtr = bailPtr;

    /* Initialize the printMe flag */
    cUnit.printMe = gDvmJit.printMe;

    /* Setup the method */
    cUnit.method = desc->method;

    /* Store the trace descriptor and set the initial mode */
    cUnit.traceDesc = desc;
    cUnit.jitMode = kJitTrace;

    /* Initialize the PC reconstruction list */
    dvmInitGrowableList(&cUnit.pcReconstructionList, 8);

    /* Initialize the basic block list */
    blockList = &cUnit.blockList;
    dvmInitGrowableList(blockList, 8);

    /* Identify traces that we don't want to compile */
    if (gDvmJit.classTable) {
        bool classFound = searchClassTablePrefix(desc->method);
        if (gDvmJit.classTable && gDvmJit.includeSelectedMethod != classFound) {
            return false;
        }
    }
    if (gDvmJit.methodTable) {
        int len = strlen(desc->method->clazz->descriptor) +
                  strlen(desc->method->name) + 1;
        char *fullSignature = (char *)dvmCompilerNew(len, true);
        strcpy(fullSignature, desc->method->clazz->descriptor);
        strcat(fullSignature, desc->method->name);

        int hashValue = dvmComputeUtf8Hash(fullSignature);

        /*
         * Doing three levels of screening to see whether we want to skip
         * compiling this method
         */

        /* First, check the full "class;method" signature */
        bool methodFound =
            dvmHashTableLookup(gDvmJit.methodTable, hashValue,
                               fullSignature, (HashCompareFunc) strcmp,
                               false) !=
            NULL;

        /* Full signature not found - check the enclosing class */
        if (methodFound == false) {
            int hashValue = dvmComputeUtf8Hash(desc->method->clazz->descriptor);
            methodFound =
                dvmHashTableLookup(gDvmJit.methodTable, hashValue,
                               (char *) desc->method->clazz->descriptor,
                               (HashCompareFunc) strcmp, false) !=
                NULL;
            /* Enclosing class not found - check the method name */
            if (methodFound == false) {
                int hashValue = dvmComputeUtf8Hash(desc->method->name);
                methodFound =
                    dvmHashTableLookup(gDvmJit.methodTable, hashValue,
                                   (char *) desc->method->name,
                                   (HashCompareFunc) strcmp, false) !=
                    NULL;

                /*
                 * Debug by call-graph is enabled. Check if the debug list
                 * covers any methods on the VM stack.
                 */
                if (methodFound == false && gDvmJit.checkCallGraph == true) {
                    methodFound =
                        filterMethodByCallGraph(info->requestingThread,
                                                desc->method->name);
                }
            }
        }

        /*
         * Under the following conditions, the trace will be *conservatively*
         * compiled by only containing single-step instructions to and from the
         * interpreter.
         * 1) If includeSelectedMethod == false, the method matches the full or
         *    partial signature stored in the hash table.
         *
         * 2) If includeSelectedMethod == true, the method does not match the
         *    full and partial signature stored in the hash table.
         */
        if (gDvmJit.methodTable && gDvmJit.includeSelectedMethod != methodFound) {
#ifdef ARCH_IA32
            return false;
#else
            cUnit.allSingleStep = true;
#endif
        } else {
            /* Compile the trace as normal */

            /* Print the method we cherry picked */
            if (gDvmJit.includeSelectedMethod == true) {
                cUnit.printMe = true;
            }
        }
    }

    // Each pair is a range, check whether curOffset falls into a range.
    bool includeOffset = (gDvmJit.num_entries_pcTable < 2);
    for (int pcOff = 0; pcOff < gDvmJit.num_entries_pcTable; ) {
        if (pcOff+1 >= gDvmJit.num_entries_pcTable) {
          break;
        }
        if (curOffset >= gDvmJit.pcTable[pcOff] && curOffset <= gDvmJit.pcTable[pcOff+1]) {
            includeOffset = true;
            break;
        }
        pcOff += 2;
    }
    if (!includeOffset) {
        return false;
    }

    /* Allocate the entry block */
    curBB = dvmCompilerNewBB(kEntryBlock, numBlocks++);
    dvmInsertGrowableList(blockList, (intptr_t) curBB);
    curBB->startOffset = curOffset;

    entryCodeBB = dvmCompilerNewBB(kDalvikByteCode, numBlocks++);
    dvmInsertGrowableList(blockList, (intptr_t) entryCodeBB);
    entryCodeBB->startOffset = curOffset;
    curBB->fallThrough = entryCodeBB;
    curBB = entryCodeBB;

#ifdef BYTECODE_FILTER
// disable certain bytecodes
    while (1) {
        DecodedInstruction insn;
        int width = parseInsn(codePtr, &insn, false);
        if (!dvmIsOpcodeSupportedByJit(insn)) {
            return false;
        }

        assert(width);
        if (--numInsts == 0) {
            if (currRun->info.frag.runEnd) {
                break;
            } else {
                /* Advance to the next trace description (ie non-meta info) */
                do {
                    currRun++;
                } while (!currRun->isCode);

                /* Dummy end-of-run marker seen */
                if (currRun->info.frag.numInsts == 0) {
                    break;
                }

                curOffset = currRun->info.frag.startOffset;
                numInsts = currRun->info.frag.numInsts;
                codePtr = dexCode->insns + curOffset;
            }
        } else {
            curOffset += width;
            codePtr += width;
        }
    }
    currRun = &desc->trace[0];
    curOffset = currRun->info.frag.startOffset;
    numInsts = currRun->info.frag.numInsts;
    codePtr = dexCode->insns + curOffset;
#endif

    if (cUnit.printMe) {
        ALOGD("--------\nCompiler: Building trace for %s, offset %#x",
             desc->method->name, curOffset);
    }

    /*
     * Analyze the trace descriptor and include up to the maximal number
     * of Dalvik instructions into the IR.
     */
    while (1) {
        MIR *insn;
        int width;
        insn = (MIR *)dvmCompilerNew(sizeof(MIR), true);
        insn->offset = curOffset;
        width = parseInsn(codePtr, &insn->dalvikInsn, cUnit.printMe);

        /* The trace should never incude instruction data */
        assert(width);
        insn->width = width;
        traceSize += width;
        dvmCompilerAppendMIR(curBB, insn);
        cUnit.numInsts++;

        int flags = dexGetFlagsFromOpcode(insn->dalvikInsn.opcode);

        if (flags & kInstrInvoke) {
            const Method *calleeMethod = (const Method *)
                currRun[JIT_TRACE_CUR_METHOD].info.meta;
            assert(numInsts == 1);
            CallsiteInfo *callsiteInfo =
                (CallsiteInfo *)dvmCompilerNew(sizeof(CallsiteInfo), true);
            callsiteInfo->classDescriptor = (const char *)
                currRun[JIT_TRACE_CLASS_DESC].info.meta;
            callsiteInfo->classLoader = (Object *)
                currRun[JIT_TRACE_CLASS_LOADER].info.meta;
            callsiteInfo->method = calleeMethod;
            insn->meta.callsiteInfo = callsiteInfo;
        }

        /* Instruction limit reached - terminate the trace here */
        if (cUnit.numInsts >= numMaxInsts) {
            break;
        }
        if (--numInsts == 0) {
            if (currRun->info.frag.runEnd) {
                break;
            } else {
                /* Advance to the next trace description (ie non-meta info) */
                do {
                    currRun++;
                } while (!currRun->isCode);

                /* Dummy end-of-run marker seen */
                if (currRun->info.frag.numInsts == 0) {
                    break;
                }

                curBB = dvmCompilerNewBB(kDalvikByteCode, numBlocks++);
                dvmInsertGrowableList(blockList, (intptr_t) curBB);
                curOffset = currRun->info.frag.startOffset;
                numInsts = currRun->info.frag.numInsts;
                curBB->startOffset = curOffset;
                codePtr = dexCode->insns + curOffset;
            }
        } else {
            curOffset += width;
            codePtr += width;
        }
    }

#if defined(WITH_JIT_TUNING)
    /* Convert # of half-word to bytes */
    methodStats->compiledDalvikSize += traceSize * 2;
#endif

    /*
     * Now scan basic blocks containing real code to connect the
     * taken/fallthrough links. Also create chaining cells for code not included
     * in the trace.
     */
    size_t blockId;
    for (blockId = 0; blockId < blockList->numUsed; blockId++) {
        curBB = (BasicBlock *) dvmGrowableListGetElement(blockList, blockId);
        MIR *lastInsn = curBB->lastMIRInsn;
        /* Skip empty blocks */
        if (lastInsn == NULL) {
            continue;
        }
        curOffset = lastInsn->offset;
        unsigned int targetOffset = curOffset;
        unsigned int fallThroughOffset = curOffset + lastInsn->width;
        bool isInvoke = false;
        const Method *callee = NULL;

        findBlockBoundary(desc->method, curBB->lastMIRInsn, curOffset,
                          &targetOffset, &isInvoke, &callee);

        /* Link the taken and fallthrough blocks */
        BasicBlock *searchBB;

        int flags = dexGetFlagsFromOpcode(lastInsn->dalvikInsn.opcode);

        if (flags & kInstrInvoke) {
            cUnit.hasInvoke = true;
        }

        /* Backward branch seen */
        if (isInvoke == false &&
            (flags & kInstrCanBranch) != 0 &&
            targetOffset < curOffset &&
            (optHints & JIT_OPT_NO_LOOP) == 0) {
            dvmCompilerArenaReset();
            return compileLoop(&cUnit, startOffset, desc, numMaxInsts,
                               info, bailPtr, optHints);
        }

        /* No backward branch in the trace - start searching the next BB */
        size_t searchBlockId;
        for (searchBlockId = blockId+1; searchBlockId < blockList->numUsed;
             searchBlockId++) {
            searchBB = (BasicBlock *) dvmGrowableListGetElement(blockList,
                                                                searchBlockId);
            if (targetOffset == searchBB->startOffset) {
                curBB->taken = searchBB;
                dvmCompilerSetBit(searchBB->predecessors, curBB->id);
            }
            if (fallThroughOffset == searchBB->startOffset) {
                curBB->fallThrough = searchBB;
                dvmCompilerSetBit(searchBB->predecessors, curBB->id);

                /*
                 * Fallthrough block of an invoke instruction needs to be
                 * aligned to 4-byte boundary (alignment instruction to be
                 * inserted later.
                 */
                if (flags & kInstrInvoke) {
                    searchBB->isFallThroughFromInvoke = true;
                }
            }
        }

        /*
         * Some blocks are ended by non-control-flow-change instructions,
         * currently only due to trace length constraint. In this case we need
         * to generate an explicit branch at the end of the block to jump to
         * the chaining cell.
         */
        curBB->needFallThroughBranch =
            ((flags & (kInstrCanBranch | kInstrCanSwitch | kInstrCanReturn |
                       kInstrInvoke)) == 0);
        if (lastInsn->dalvikInsn.opcode == OP_PACKED_SWITCH ||
            lastInsn->dalvikInsn.opcode == OP_SPARSE_SWITCH) {
            int i;
            const u2 *switchData = desc->method->insns + lastInsn->offset +
                             lastInsn->dalvikInsn.vB;
            int size = switchData[1];
            int maxChains = MIN(size, MAX_CHAINED_SWITCH_CASES);

            /*
             * Generate the landing pad for cases whose ranks are higher than
             * MAX_CHAINED_SWITCH_CASES. The code will re-enter the interpreter
             * through the NoChain point.
             */
            if (maxChains != size) {
                cUnit.switchOverflowPad =
                    desc->method->insns + lastInsn->offset;
            }

            s4 *targets = (s4 *) (switchData + 2 +
                    (lastInsn->dalvikInsn.opcode == OP_PACKED_SWITCH ?
                     2 : size * 2));

            /* One chaining cell for the first MAX_CHAINED_SWITCH_CASES cases */
            for (i = 0; i < maxChains; i++) {
                BasicBlock *caseChain = dvmCompilerNewBB(kChainingCellNormal,
                                                         numBlocks++);
                dvmInsertGrowableList(blockList, (intptr_t) caseChain);
                caseChain->startOffset = lastInsn->offset + targets[i];
            }

            /* One more chaining cell for the default case */
            BasicBlock *caseChain = dvmCompilerNewBB(kChainingCellNormal,
                                                     numBlocks++);
            dvmInsertGrowableList(blockList, (intptr_t) caseChain);
            caseChain->startOffset = lastInsn->offset + lastInsn->width;
        /* Fallthrough block not included in the trace */
        } else if (!isUnconditionalBranch(lastInsn) &&
                   curBB->fallThrough == NULL) {
            BasicBlock *fallThroughBB;
            /*
             * If the chaining cell is after an invoke or
             * instruction that cannot change the control flow, request a hot
             * chaining cell.
             */
            if (isInvoke || curBB->needFallThroughBranch) {
                fallThroughBB = dvmCompilerNewBB(kChainingCellHot, numBlocks++);
            } else {
                fallThroughBB = dvmCompilerNewBB(kChainingCellNormal,
                                                 numBlocks++);
            }
            dvmInsertGrowableList(blockList, (intptr_t) fallThroughBB);
            fallThroughBB->startOffset = fallThroughOffset;
            curBB->fallThrough = fallThroughBB;
            dvmCompilerSetBit(fallThroughBB->predecessors, curBB->id);
        }
        /* Target block not included in the trace */
        if (curBB->taken == NULL &&
            (isGoto(lastInsn) || isInvoke ||
            (targetOffset != UNKNOWN_TARGET && targetOffset != curOffset))) {
            BasicBlock *newBB = NULL;
            if (isInvoke) {
                /* Monomorphic callee */
                if (callee) {
                    /* JNI call doesn't need a chaining cell */
                    if (!dvmIsNativeMethod(callee)) {
                        newBB = dvmCompilerNewBB(kChainingCellInvokeSingleton,
                                                 numBlocks++);
                        newBB->startOffset = 0;
                        newBB->containingMethod = callee;
                    }
                /* Will resolve at runtime */
                } else {
                    newBB = dvmCompilerNewBB(kChainingCellInvokePredicted,
                                             numBlocks++);
                    newBB->startOffset = 0;
                }
            /* For unconditional branches, request a hot chaining cell */
            } else {
#if !defined(WITH_SELF_VERIFICATION)
                newBB = dvmCompilerNewBB(dexIsGoto(flags) ?
                                                  kChainingCellHot :
                                                  kChainingCellNormal,
                                         numBlocks++);
                newBB->startOffset = targetOffset;
#else
                /* Handle branches that branch back into the block */
                if (targetOffset >= curBB->firstMIRInsn->offset &&
                    targetOffset <= curBB->lastMIRInsn->offset) {
                    newBB = dvmCompilerNewBB(kChainingCellBackwardBranch,
                                             numBlocks++);
                } else {
                    newBB = dvmCompilerNewBB(dexIsGoto(flags) ?
                                                      kChainingCellHot :
                                                      kChainingCellNormal,
                                             numBlocks++);
                }
                newBB->startOffset = targetOffset;
#endif
            }
            if (newBB) {
                curBB->taken = newBB;
                dvmCompilerSetBit(newBB->predecessors, curBB->id);
                dvmInsertGrowableList(blockList, (intptr_t) newBB);
            }
        }
    }

    /* Now create a special block to host PC reconstruction code */
    curBB = dvmCompilerNewBB(kPCReconstruction, numBlocks++);
    dvmInsertGrowableList(blockList, (intptr_t) curBB);

    /* And one final block that publishes the PC and raise the exception */
    curBB = dvmCompilerNewBB(kExceptionHandling, numBlocks++);
    dvmInsertGrowableList(blockList, (intptr_t) curBB);
    cUnit.puntBlock = curBB;

    if (cUnit.printMe) {
        char* signature =
            dexProtoCopyMethodDescriptor(&desc->method->prototype);
        ALOGD("TRACEINFO (%d): 0x%08x %s%s.%s %#x %d of %d, %d blocks",
            compilationId,
            (intptr_t) desc->method->insns,
            desc->method->clazz->descriptor,
            desc->method->name,
            signature,
            desc->trace[0].info.frag.startOffset,
            traceSize,
            dexCode->insnsSize,
            numBlocks);
        free(signature);
    }

    cUnit.numBlocks = numBlocks;

    /* Set the instruction set to use (NOTE: later components may change it) */
    cUnit.instructionSet = dvmCompilerInstructionSet();

    /* Inline transformation @ the MIR level */
    if (cUnit.hasInvoke && !(gDvmJit.disableOpt & (1 << kMethodInlining))) {
        dvmCompilerInlineMIR(&cUnit, info);
    }

    cUnit.numDalvikRegisters = cUnit.method->registersSize;

    /* Preparation for SSA conversion */
    dvmInitializeSSAConversion(&cUnit);

    dvmCompilerNonLoopAnalysis(&cUnit);

#ifndef ARCH_IA32
    dvmCompilerInitializeRegAlloc(&cUnit);  // Needs to happen after SSA naming
#endif

    if (cUnit.printMe) {
        dvmCompilerDumpCompilationUnit(&cUnit);
    }

#ifndef ARCH_IA32
    /* Allocate Registers using simple local allocation scheme */
    dvmCompilerLocalRegAlloc(&cUnit);

    /* Convert MIR to LIR, etc. */
    dvmCompilerMIR2LIR(&cUnit);
#else /* ARCH_IA32 */
    /* Convert MIR to LIR, etc. */
    dvmCompilerMIR2LIR(&cUnit, info);
#endif

    /* Convert LIR into machine code. Loop for recoverable retries */
    do {
        dvmCompilerAssembleLIR(&cUnit, info);
        cUnit.assemblerRetries++;
        if (cUnit.printMe && cUnit.assemblerStatus != kSuccess)
            ALOGD("Assembler abort #%d on %d",cUnit.assemblerRetries,
                  cUnit.assemblerStatus);
    } while (cUnit.assemblerStatus == kRetryAll);

    if (cUnit.printMe) {
        ALOGD("Trace Dalvik PC: %p", startCodePtr);
        dvmCompilerCodegenDump(&cUnit);
        ALOGD("End %s%s, %d Dalvik instructions",
             desc->method->clazz->descriptor, desc->method->name,
             cUnit.numInsts);
    }

    if (cUnit.assemblerStatus == kRetryHalve) {
        /* Reset the compiler resource pool before retry */
        dvmCompilerArenaReset();

        /* Halve the instruction count and start from the top */
        return dvmCompileTrace(desc, cUnit.numInsts / 2, info, bailPtr,
                               optHints);
    }

    /*
     * If this trace uses class objects as constants,
     * dvmJitInstallClassObjectPointers will switch the thread state
     * to running and look up the class pointers using the descriptor/loader
     * tuple stored in the callsite info structure. We need to make this window
     * as short as possible since it is blocking GC.
     */
    if (cUnit.hasClassLiterals && info->codeAddress) {
        dvmJitInstallClassObjectPointers(&cUnit, (char *) info->codeAddress);
    }

    /*
     * Since callsiteinfo is allocated from the arena, delay the reset until
     * class pointers are resolved.
     */
    dvmCompilerArenaReset();

    assert(cUnit.assemblerStatus == kSuccess);
#if defined(WITH_JIT_TUNING)
    methodStats->nativeSize += cUnit.totalSize;
#endif

#if defined(VTUNE_DALVIK)
    if(gDvmJit.vtuneInfo != kVTuneInfoDisabled) {
        if(info->codeAddress) {
            SendTraceInfoToVTune(&cUnit, desc);
        } else {
            LOGD("Invalid trace\n");
        }
    }
#endif

    return info->codeAddress != NULL;
}

#if defined(VTUNE_DALVIK)
extern int mapFromBCtoNCG[]; /* declaration from Lower.h file */

typedef std::vector<LineNumberInfo> LineNumberInfoList;

struct SortLineNumberInfoByOffset {
    bool operator()(LineNumberInfo const& lhs, LineNumberInfo const& rhs) {
        return lhs.Offset < rhs.Offset;
    }
};

static char* makeDexDumpMethodName(const Method* method)
{
    char const* name = method->name;
    u4 name_len = strlen(name);
    char* signature = dexProtoCopyMethodDescriptor(&method->prototype);
    u4 signature_len = strlen(signature);
    char const* classD = method->clazz->descriptor;
    u4 classD_len = strlen(classD);
    u4 fullSignature_offset = 0;

    /* make the same name as dexdump output for DEX code */
    u4 fullSignature_len = classD_len - 1 /* L */ + name_len + 1 /* : */
        + signature_len + 1 /* '\0' */;
    char *fullSignature = (char *)dvmCompilerNew(fullSignature_len, false);

    strcpy(fullSignature, classD + 1 /* delete 'L' */);
    fullSignature_offset = classD_len - 1;
    for (u4 i = 0; i < fullSignature_offset; ++i) {
        if (fullSignature[i] == ';' || fullSignature[i] == '/') {
            fullSignature[i] = '.';
        }
    }
    strcpy(fullSignature + fullSignature_offset, name);
    fullSignature_offset += name_len;
    fullSignature[fullSignature_offset++] = ':';
    strcpy(fullSignature + fullSignature_offset, signature);
    fullSignature_offset += signature_len + 1 /* '\0' */;
    assert(fullSignature_len == fullSignature_offset);
    free(signature);

    return fullSignature;
}

static void getLineInfoForByteCode(const Method* method,
    LineNumberInfoList& lineInfoList)
{
    const DexCode* dexCode = dvmGetMethodCode(method);
    LineNumberInfo lineInfo;

    for (u4 offset = 0, i = 1; offset < dexCode->insnsSize; ++i) {
        if (mapFromBCtoNCG[offset] != -1) {
            lineInfo.Offset = mapFromBCtoNCG[offset];
            lineInfo.LineNumber = i;
            lineInfoList.push_back(lineInfo);
        }
        offset += dexGetWidthFromInstruction(dexCode->insns + offset);
    }
}

static void getLineInfoForJavaCode(const Method* method,
    LineNumberInfoList& lineInfoList)
{
    const DexCode* dexCode = dvmGetMethodCode(method);
    LineNumberInfo lineInfo;

    DexFile* pDexFile = method->clazz->pDvmDex->pDexFile;
    const u1 *dbgstream = dexGetDebugInfoStream(pDexFile, dexCode);
    if (method->clazz->sourceFile != NULL && dbgstream != NULL) {
        /* mapping from JITed code to Java source code */
        int adjopcode;
        u4 line, parametersSize, address = 0;
        line = readUnsignedLeb128(&dbgstream);
        parametersSize = readUnsignedLeb128(&dbgstream);

        while (parametersSize-- != 0) {
            readUnsignedLeb128(&dbgstream);
        }

        bool isEndSequence = false;
        while (!isEndSequence)  {
            u1 opcode = *dbgstream++;
            switch (opcode) {
            case DBG_END_SEQUENCE:
                isEndSequence = true;
                break;

            case DBG_ADVANCE_PC:
                address += readUnsignedLeb128(&dbgstream);
                break;

            case DBG_ADVANCE_LINE:
                line += readSignedLeb128(&dbgstream);
                break;

            case DBG_START_LOCAL:
            case DBG_START_LOCAL_EXTENDED:
                readUnsignedLeb128(&dbgstream);
                readUnsignedLeb128(&dbgstream);
                readUnsignedLeb128(&dbgstream);

                if (opcode == DBG_START_LOCAL_EXTENDED) {
                    readUnsignedLeb128(&dbgstream);
                }
                break;

            case DBG_END_LOCAL:
            case DBG_RESTART_LOCAL:
                readUnsignedLeb128(&dbgstream);
                break;

            case DBG_SET_PROLOGUE_END:
            case DBG_SET_EPILOGUE_BEGIN:
            case DBG_SET_FILE:
                break;

            default:
                adjopcode = opcode - DBG_FIRST_SPECIAL;
                address += adjopcode / DBG_LINE_RANGE;
                line += DBG_LINE_BASE + (adjopcode % DBG_LINE_RANGE);

                if (mapFromBCtoNCG[address] != -1) {
                    lineInfo.Offset = mapFromBCtoNCG[address];
                    lineInfo.LineNumber = line;
                    lineInfoList.push_back(lineInfo);
                }
                break;
            }
        }
    }
}

/* Notify the trace formation and send the trace information to VTune
*/
static void SendTraceInfoToVTune(CompilationUnit *cUnit, JitTraceDescription *desc) {
    if (cUnit->printMe) {
        LOGD("JIT API: write a trace of '%s' method in jit file.", cUnit->method->name);
    }

    DexStringCache params_string;
    dexStringCacheInit(&params_string);
    const char* params = dexProtoGetParameterDescriptors(&desc->method->prototype, &params_string);
    int len = strlen(params) + strlen(desc->method->name) + 3 ;
    char* name_with_params = (char*) dvmCompilerNew(len, true);
    strcpy(name_with_params, desc->method->name);
    strcat(name_with_params, "(");
    if (params) {
        strcat(name_with_params, params);
    }
    strcat(name_with_params, ")");
    dexStringCacheRelease(&params_string);

    //Structure to load all the trace details to be sent later to VTune
    iJIT_Method_Load jitMethod;
    memset(&jitMethod, 0, sizeof(iJIT_Method_Load));
    jitMethod.method_id = (int)(cUnit->method);
    jitMethod.method_name = name_with_params;
    jitMethod.class_file_name = (char*)(cUnit->method->clazz->descriptor);
    jitMethod.method_load_address = cUnit->baseAddr;
    jitMethod.method_size = *(u2 *)((char *)cUnit->baseAddr - 4);
    jitMethod.source_file_name = (char*)(cUnit->method->clazz->sourceFile);

    const Method* method = cUnit->method;
    LineNumberInfoList lineInfoList;

    if (gDvmJit.vtuneInfo == kVTuneInfoByteCode) {
        jitMethod.source_file_name = makeDexDumpMethodName(method);
        getLineInfoForByteCode(method, lineInfoList);
    } else
    if (gDvmJit.vtuneInfo == kVTuneInfoJavaCode) {
        getLineInfoForJavaCode(method, lineInfoList);
    }

    if (!lineInfoList.empty()) {
        std::sort(lineInfoList.begin(), lineInfoList.end(),
            SortLineNumberInfoByOffset());

        for (unsigned i = 0; i < lineInfoList.size() - 1; ++i) {
            lineInfoList[i].Offset = lineInfoList[i + 1].Offset;
        }
        lineInfoList[lineInfoList.size() - 1].Offset = jitMethod.method_size;

        jitMethod.line_number_size = lineInfoList.size();
        jitMethod.line_number_table = &lineInfoList[0];
    }

    // Send the trace load event to the VTune analyzer
    int res = iJIT_NotifyEvent(iJVM_EVENT_TYPE_METHOD_LOAD_FINISHED, (void*)&jitMethod);
    if (cUnit->printMe) {
        if (res) {
            LOGD("JIT API: a trace of '%s' method was written successfully.", cUnit->method->name);
        } else {
            LOGD("JIT API: failed to write a trace of '%s' method.", cUnit->method->name);
        }
    }
}
#endif
