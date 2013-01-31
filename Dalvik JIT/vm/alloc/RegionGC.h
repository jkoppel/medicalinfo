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

#ifndef _DALVIK_ALLOC_REGIONGC
#define _DALVIK_ALLOC_REGIONGC

#include "alloc/Heap.h"

/* 1. REGION_GC_NULL: The default value. Region GC is not enabled yet.
 * 2. REGION_GC_MINOR:Minor collection in Region GC only scan and
 *    sweep active heap.
 * 3. REGION_GC_MAJOR: Major collection in Region GC scan both active heap
 *    and zygote heap. It's triggered for reduce the floating garbage
 *    introduced by Region GC minor collection.
 */
enum REGION_GC_TYPE { REGION_GC_NULL, REGION_GC_MINOR, REGION_GC_MAJOR };

struct RegionGC{
    GcHeap *gcHeap;

    /* When GC starts region GC in this collection cycle, the value
     * is set to true.
     */
    bool startRegionGC;
    REGION_GC_TYPE regionGcType;
    /* Set to true to force major collection.
     */
    bool forceMajor;
    /* The percentage of dirty cards in the zygote heap.
     */
    float dirtyRatioZygoteHeap;
    /* The budget of dirty cards in the zygote heap. If GC exceeds this
     * budget, a major collection is triggered to collect the floating
     * garbage.
     */
    float dirtyRatioThreshold;
};

/* Region GC intialization. */
void regionGcInitialization(RegionGC *regionGC);

/* Decide the GC mode. */
void regionGcDecideGcMode();

/* Clear both the card table and the shadow card table. */
void regionGcResetCardTables(RegionGC *regionGC);

/* Prepare Card Tables for regional GC. */
void regionGcPrepareCardTable();

/* Merge the shadow card table and the card table.*/
void regionGcBackupCardTable();

/* Decide if the regional GC need a final phase to rescan the dirty objects. */
bool regionGcNeedClearPhase();

/* Decide if the regional GC only needs partial mark. */
bool regionGcIsPartialMark();

/* Check properties for the Regional GC. */
void regionGcCheckProperty();
#endif //_DALVIK_ALLOC_REGIONGC
