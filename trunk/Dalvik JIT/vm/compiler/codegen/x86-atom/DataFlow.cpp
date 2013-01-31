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

#ifndef WITH_JIT
#include "AnalysisO1.h"

int genSet[MAX_REG_PER_BYTECODE];
int killSet[MAX_REG_PER_BYTECODE];
int num_regs_gen; //per bytecode
int num_regs_kill; //per bytecode

int genSetBB[MAX_NUM_BBS_PER_METHOD][40];
int killSetBB[MAX_NUM_BBS_PER_METHOD][40]; //same as size of memVRTable
int num_gen_bb[MAX_NUM_BBS_PER_METHOD];
int num_kill_bb[MAX_NUM_BBS_PER_METHOD];

int tmpSetBB[40];
void dumpGenKill(int* gen, int num_gen, int* kill, int num_kill, int type);
void mergeSet(BasicBlock_O1* bb, int type, int index);
int setOpMinus(int* set1, int num_set1, int* set2, int num_set2, int* result);
int setOpPlus(int* set1, int num_set1, int* set2, int num_set2, int* result);
int setOpInsert(int* set, int num_set, int* tmpSet, int tmpSize);
bool setSame(int* set1, int num_set1, int* set2, int num_set2);

//compute gen&kill set for each BB
//type == 2: live variable
//           do not use visitor, use defUseTable
//           genSet: exposed usage (usages that are reached by a PC_FOR_START_OF_BB
//           killSet: all definitions
void computeSetForLiveVar(BasicBlock_O1* bb, int index) {
  num_gen_bb[index] = 0;
  num_kill_bb[index] = 0;
  DefUsePair* ptr = bb->defUseTable;
  DefOrUseLink* ptrUse = NULL;
  int tmpSet[2];
  while(ptr != NULL) {
    if(ptr->def.offsetPC == PC_FOR_START_OF_BB) {
      ptrUse = ptr->uses;
      while(ptrUse != NULL) {
	if(ptrUse->offsetPC != PC_FOR_END_OF_BB) {
	  //inset to genSetBB(exposed usage)
	  int tmpSize = getMemVR(ptrUse->physicalType, ptrUse->accessType,
				 ptrUse->regNum, tmpSet);
	  num_gen_bb[index] = setOpInsert(genSetBB[index], num_gen_bb[index], tmpSet, tmpSize);
	}
	ptrUse = ptrUse->next;
      } //for each usage of the def
    }
    else { //insert to killSetBB (a def)
      int tmpSize = getMemVR(ptr->def.physicalType, ptr->def.accessType,
			     ptr->def.regNum, tmpSet);
      num_kill_bb[index] = setOpInsert(killSetBB[index], num_kill_bb[index], tmpSet, tmpSize);
    }
    ptr = ptr->next;
  } //for each def
}
//v1, v1+1 or both depending on accessType & physicalType
int getMemVR(LowOpndRegType type, RegAccessType aType, int regNum, int* set) {
  int ret_num = 0;
  if(aType == REGACCESS_L || aType == REGACCESS_D) {
    set[ret_num++] = regNum;
  }
  if(getRegSize(type) == OpndSize_64 &&
     (aType == REGACCESS_H || aType == REGACCESS_D)) {
    set[ret_num++] = regNum+1;
  }
  return ret_num;
}
//compute gen&kill set for each BB
//type == 0: null check
//type == 1: glue opt
void computeSetForEachBB(BasicBlock_O1* bb, int type, int index) {
  int kk;
  num_gen_bb[index] = 0;
  num_kill_bb[index] = 0;
  //go through bytecode in the bb
  u2* rPC_start = (u2*)currentMethod->insns;
  offsetPC = bb->pc_start;
  rPC = rPC_start + offsetPC;
  for(; offsetPC < bb->pc_end; ) {
    inst = FETCH(0);
    if(type == 0)
      getNullCheckInfo(genSet, killSet);
    if(type == 1)
      getGlueInfo(genSet, killSet);
#ifdef DEBUG_DATAFLOW
    printf("bytecode @ %x -------\n", offsetPC);
#endif
    dumpGenKill(genSet, num_regs_gen, killSet, num_regs_kill, 0);
    //use genSet, killSet, num_regs_gen, num_regs_kill
    //to update genSetBB & killSetBB
    mergeSet(bb, type, index);
    offsetPC += getByteCodeSize();
    rPC = rPC_start + offsetPC;
  }
#ifdef DEBUG_DATAFLOW
  printf("for BB %d --------------------\n", index);
#endif
  dumpGenKill(genSetBB[index], num_gen_bb[index], killSetBB[index], num_kill_bb[index], 0);
}

void dumpGenKill(int* gen, int num_gen, int* kill, int num_kill, int type) {
#ifdef DEBUG_DATAFLOW
  int kk;
  if(type == 0)
    printf("  GEN SET .......... ");
  else
    printf("  INB .......... ");
  for(kk = 0; kk < num_gen; kk++) printf("%d ", gen[kk]);
  if(type == 0)
    printf("\n  KILL SET ........... ");
  else
    printf("  OUTB .......... ");
  for(kk = 0; kk < num_kill; kk++) printf("%d ", kill[kk]);
  printf("\n");
#endif
}
//forward propagation
void mergeSet(BasicBlock_O1* bb, int type, int index) {
  //genBB = gen + (genBB - kill)
  int num_tmp_bb = setOpMinus(genSetBB[index], num_gen_bb[index], killSet, num_regs_kill, tmpSetBB);
  num_gen_bb[index] = setOpPlus(genSet, num_regs_gen, tmpSetBB, num_tmp_bb, genSetBB[index]);
  //killBB = kill + (killBB - gen)
  num_tmp_bb = setOpMinus(killSetBB[index], num_kill_bb[index], genSet, num_regs_gen, tmpSetBB);
  num_kill_bb[index] = setOpPlus(killSet, num_regs_kill, tmpSetBB, num_tmp_bb, killSetBB[index]);
#ifdef DEBUG_DATAFLOW
  printf("at mergeSet -------------------------\n");
#endif
  dumpGenKill(genSetBB[index], num_gen_bb[index], killSetBB[index], num_kill_bb[index], 0);
}
int setOpMinus(int* set1, int num_set1, int* set2, int num_set2, int* result) {
  int k1, k2;
  int ret_num = 0;
  for(k1 = 0; k1 < num_set1; k1++) {
    //check whether it is killed in set2
    bool killed = false;
    for(k2 = 0; k2 < num_set2; k2++) {
      if(set1[k1] == set2[k2]) {
	killed = true;
	break;
      }
    }
    if(!killed) {
      result[ret_num++] = set1[k1];
    }
  }
  return ret_num;
}
//returns number of elements in result
int setOpPlus(int* set1, int num_set1, int* set2, int num_set2, int* result) {
  int k1, k2;
  int ret_num = 0;
  for(k1 = 0; k1 < num_set1; k1++) { //assume no duplication in each input set
    result[ret_num++] = set1[k1];
  }
  for(k2 = 0; k2 < num_set2; k2++) {
    bool alreadyIn = false;
    for(k1 = 0; k1 < num_set1; k1++) {
      if(set1[k1] == set2[k2]) {
	alreadyIn = true;
	break;
      }
    }
    if(!alreadyIn) {
      result[ret_num++] = set2[k2];
    }
  }
  return ret_num;
}
int setOpInsert(int* set, int num_set, int* tmpSet, int tmpSize) {
  int k1, k2;
  int ret_num = num_set;
  for(k2 = 0; k2 < tmpSize; k2++) {
    bool alreadyIn = false;
    for(k1 = 0; k1 < num_set; k1++) {
      if(set[k1] == tmpSet[k2]) {
	alreadyIn = true;
	break;
      }
    }
    if(!alreadyIn) {
      set[ret_num++] = tmpSet[k2];
    }
  }
  return ret_num;
}
//are they in order?
bool setSame(int* set1, int num_set1, int* set2, int num_set2) {
  int k1, k2;
#ifdef DEBUG_DATAFLOW
  printf("setSame SET1 ------------ ");
  for(k1 = 0; k1 < num_set1; k1++) printf("%d ", set1[k1]);
  printf("\nsetSame SET2 ------------ ");
  for(k1 = 0; k1 < num_set1; k1++) printf("%d ", set2[k1]);
  printf("\n");
#endif
  if(num_set1 != num_set2) return false;
  for(k1 = 0; k1 < num_set1; k1++) {
    //check whether set1[k1] is in set2
    bool isIn = false;
    for(k2 = 0; k2 < num_set2; k2++) {
      if(set2[k2] == set1[k1]) {
        isIn = true;
        break;
      }
    }
    if(!isIn) return false;
  }
  return true;
}
//called before forwardAnalysis or backwardAnalysis
void updateBBIndex() {
  int k;
  for(k = 0; k < num_bbs_for_method; k++) {
    method_bbs_sorted[k]->bb_index2 = k;
#ifdef DEBUG_DATAFLOW
    printf("BB bb_index %d bb_index2 %d\n", method_bbs_sorted[k]->bb_index, k);
#endif
  }
}
//data structure for inB & outB: an array of virtual register
int inB[MAX_NUM_BBS_PER_METHOD][40];
int inSize[MAX_NUM_BBS_PER_METHOD];
int outB[MAX_NUM_BBS_PER_METHOD][40];
int outSize[MAX_NUM_BBS_PER_METHOD];

int nullCheck_inB[MAX_NUM_BBS_PER_METHOD][40];
int nullCheck_inSize[MAX_NUM_BBS_PER_METHOD];
int nullCheck_outB[MAX_NUM_BBS_PER_METHOD][40];
int nullCheck_outSize[MAX_NUM_BBS_PER_METHOD];

//compute inB & outB for all BBs
void forwardAnalysis(int type) {
  int k, k2;
  //initialize IN(bb) to NULL
  for(k = 0; k < num_bbs_for_method; k++) {
    //this function updates genSetBB, killSetBB[], num_gen_bb, num_kill_bb;
    computeSetForEachBB(method_bbs_sorted[k], type, k/*index of the basic block*/);
    inSize[k] = 0;
    outSize[k] = num_gen_bb[k];
    for(k2 = 0; k2 < num_gen_bb[k]; k2++) {
      outB[k][k2] = genSetBB[k][k2];
    }
  }
  int oldOut[40];
  int num_old = 0;
  bool changed = true;
  while(changed) {
#ifdef DEBUG_DATAFLOW
    printf("inside data flow analysis loop .............\n");
#endif
    changed = false;
    for(k = 0; k < num_bbs_for_method; k++) {
#ifdef DEBUG_DATAFLOW
      printf("propagate info for BB %d\n", k);
#endif
      currentBB = method_bbs_sorted[k];
      //backup outB[k]
      for(k2 = 0; k2 < outSize[k]; k2++) {
	oldOut[k2] = outB[k][k2];
      }
      num_old = outSize[k];
      //update inB[k]
      inSize[k] = 0;
      if(isFirstOfHandler(currentBB)) {
	//null for first basic block of exception handler
      }
      else {
	for(k2 = 0; k2 < currentBB->num_in_edges; k2++) {
	  //for each predecessor of currentBB
	  BasicBlock_O1* tmpB = currentBB->in_edges[k2]->src;
	  inSize[k] = setOpInsert(inB[k], inSize[k], outB[tmpB->bb_index2],
				  outSize[tmpB->bb_index2]);
	}
      }
      //update outB[k] with GEN(X) + (IN(X) - KILL(X))
      int num_tmp_bb = setOpMinus(inB[k], inSize[k], killSetBB[k], num_kill_bb[k], tmpSetBB);
      outSize[k] = setOpPlus(genSetBB[k], num_gen_bb[k], tmpSetBB, num_tmp_bb, outB[k]);
      if(!setSame(oldOut, num_old, outB[k], outSize[k])) {
	changed = true;
      }
    }//for each BB
  }//while changed
#ifdef DEBUG_DATAFLOW
  printf("DATA FLOW RESULT ------------------- \n");
#endif
  for(k = 0; k < num_bbs_for_method; k++) {
    dumpGenKill(inB[k], inSize[k], outB[k], outSize[k], 1);
    if(type == 0) {
      nullCheck_inSize[k] = inSize[k];
      int k2;
      for(k2 = 0; k2 < inSize[k]; k2++) {
        nullCheck_inB[k][k2] = inB[k][k2];
      }
      nullCheck_outSize[k] = outSize[k];
      for(k2 = 0; k2 < inSize[k]; k2++) {
        nullCheck_outB[k][k2] = outB[k][k2];
      }
    }
  }
}

//live variable analysis
//  GEN: USE (exposed)
//  KILL: DEF (set of variabled defined)
//  IN: set of variables that are live at the entry point
//  OUT(X) = Union(IN(Y)) for all successors Y of X
//  IN(X) = USE(X) + (OUT(X) - DEF(X))
//  if(old_IN != IN(X)) change = 1
void backwardAnalysis() {
  int k, k2;
  //initialize IN(bb) to NULL
  for(k = 0; k < num_bbs_for_method; k++) {
    inSize[k] = 0;
    //this function updates genSetBB, killSetBB[], num_gen_bb, num_kill_bb;
    computeSetForLiveVar(currentBB, k);
  }
  int oldIn[40];
  int num_old = 0;
  bool changed = 1;
  while(changed) {
    changed = 0;
    for(k = 0; k < num_bbs_for_method; k++) {
      currentBB = method_bbs_sorted[k];
      //backup inB[k]
      for(k2 = 0; k2 < inSize[k]; k2++) {
	oldIn[k2] = inB[k][k2];
      }
      num_old = inSize[k];
      //update outB[k]
      outSize[k] = 0;
      for(k2 = 0; k2 < currentBB->num_out_edges; k2++) { //for each successor of currentBB
	BasicBlock_O1* tmpB = currentBB->out_edges[k2]->dst; //index of tmpB to method_bbs
	outSize[k] = setOpInsert(outB[k], outSize[k], inB[tmpB->bb_index2],
				 inSize[tmpB->bb_index2]);
      }
      //update inB[k] with USE(X) + (OUT(X) - killSetBB)
      int num_tmp_bb = setOpMinus(outB[k], outSize[k], killSetBB[k], num_kill_bb[k], tmpSetBB);
      inSize[k] = setOpPlus(genSetBB[k], num_gen_bb[k], tmpSetBB, num_tmp_bb, inB[k]);
      if(!setSame(oldIn, num_old, inB[k], inSize[k])) {
	changed = true;
      }
    }//for each BB
  }//while changed
}
//special handling for start of an exception handler
//--> no edge to an exception handler

//nullCheck & glue opt
//without global analysis
//--> initial state at start of BB: null
//with global analysis
//--> initial state at start of BB: inB[k] (TODO)
//--> inB is null for start of an exception handler

//live variable (which VRs are global at end of BB)
//without global analysis
//with global analysis
//--> when dumping GL VRs to memory, first check outB[k] to see whether it is live (TODO)
//--> dumping GL VRs to memory before jumping to an exception handler
#endif

