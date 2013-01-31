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
#include "Dalvik.h"
#include "alloc/RegionGC.h"
#include "alloc/CardTable.h"
#include "alloc/HeapInternal.h"
#include <cutils/properties.h>

#ifdef WITH_REGION_GC

/* The default dirty card ratio threshold. */
#define DIRTY_CARD_RATIO_THRESHOLD 0.4f

static RegionGC *gRegionGC = NULL;

void regionGcInitialization(RegionGC *regionGC)
{
    gRegionGC = regionGC;

    regionGC->regionGcType = REGION_GC_NULL;
    regionGC->forceMajor = false;
    regionGC->dirtyRatioThreshold = DIRTY_CARD_RATIO_THRESHOLD;
    regionGC->startRegionGC = false;
    regionGC->dirtyRatioZygoteHeap = 0;
}

/* Start major collection when:
 * 1. dirty cards percentage in zygote heap is larger than the threshold.
 * 2. forceMajor is set to true.
 */
static bool needStartMajor()
{
    return gRegionGC->dirtyRatioZygoteHeap > gRegionGC->dirtyRatioThreshold
           || gRegionGC->forceMajor;
}

void regionGcCheckProperty()
{
    RegionGC *regionGC = gRegionGC;
    /* If GC only has one heap or Region GC is disabled in command line,
     * Region GC will not be triggered.
     */
    if(isMultiHeapsEnabled() && gDvm.enableRegionGC) {
        /* Check the runtime option to enable and disable Region GC. */
        char propBuf[256];
        property_get("dalvik.vm.region-gc", propBuf, "");
        if(!strcmp(propBuf, "off")) {
            regionGC->startRegionGC = false;
        } else if(!strcmp(propBuf, "on")) {
            if( regionGC->startRegionGC == false){
                regionGC->forceMajor = true;
            } else {
                regionGC->forceMajor = false;
            }
            regionGC->startRegionGC = true;
        } else {
            regionGC->startRegionGC = true;
        }

        /* Check the runtime option to set the dirty threshold to trigger
         * major collection.
         */
        memset(propBuf, 0, 256);
        property_get("dalvik.vm.region-gc-dirty-threshold", propBuf, "-1.0");
        float dirty_threshold = atof(propBuf);
        if(dirty_threshold > 0 && dirty_threshold < 1) {
            regionGC->dirtyRatioThreshold = dirty_threshold;
        }
    }
}

void regionGcDecideGcMode()
{
    RegionGC *regionGC = gRegionGC;

    if(regionGC->startRegionGC) {
         if(!needStartMajor()) {
             /* Start minor collection. */
             regionGC->regionGcType = REGION_GC_MINOR;
         } else {
             /* Start major collection. */
             regionGC->regionGcType = REGION_GC_MAJOR;
             dvmSetEnableCrossHeapPointerCheck(true);
         }
    } else {
         regionGC->regionGcType = REGION_GC_NULL;
    }
}

void regionGcResetCardTables(RegionGC *regionGC)
{
    dvmClearShadowCardTable();
    dvmClearCardTable();
}

/* Merge the shadow card table and the card table. The results will be
 * stored back to the shadow card table and the card table.
 */
void regionGcMergeCardTables(RegionGC *regionGC)
{
    size_t numCard = 0;
    size_t numDirtyCard = 0;

    GcHeap *gcHeap = regionGC->gcHeap;
    u1 *cardAddr = &(gcHeap->cardTableBase[0]);
    u1 *shadowCardAddr = &(gcHeap->shadowCardTableBase[0]);

    size_t footprint = dvmGetZygoteHeapSize();
    u1 *limitCard = dvmCardFromAddr((u1 *)dvmHeapSourceGetBase() + footprint);

    for( ; cardAddr != limitCard; ++cardAddr, ++shadowCardAddr) {
        *shadowCardAddr = *cardAddr = *cardAddr | *shadowCardAddr;
        if(*shadowCardAddr != GC_CARD_CLEAN) numDirtyCard ++;
        numCard ++;
    }
    regionGC->dirtyRatioZygoteHeap = (float)numDirtyCard / numCard;
}

/* Merge the shadow card table and the card table. The results will be
 * stored back to the shadow card table.
 */
void regionGcBackupCardTable()
{
    RegionGC *regionGC = gRegionGC;

    if(regionGC->startRegionGC) {

        size_t numCard = 0;
        size_t numDirtyCard = 0;

        GcHeap *gcHeap = regionGC->gcHeap;
        u1 *cardAddr = &(gcHeap->cardTableBase[0]);
        u1 *shadowCardAddr = &(gcHeap->shadowCardTableBase[0]);

        size_t footprint = dvmGetZygoteHeapSize();
        u1 *limitCard = dvmCardFromAddr((u1 *)dvmHeapSourceGetBase() + footprint);

        for( ; cardAddr != limitCard; ++cardAddr, ++shadowCardAddr) {
            *shadowCardAddr = *cardAddr | *shadowCardAddr;
            if(*shadowCardAddr != GC_CARD_CLEAN) numDirtyCard ++;
            numCard ++;
        }
        regionGC->dirtyRatioZygoteHeap = (float)numDirtyCard / numCard;
    }
}

/* Prepare card tables for regional GC. */
void regionGcPrepareCardTable()
{
    if(gRegionGC->regionGcType == REGION_GC_MAJOR) {
        /* Major collection: clear both the card table and shadow card table.  */
        regionGcResetCardTables(gRegionGC);
    } else if(gRegionGC->regionGcType == REGION_GC_MINOR) {
        /* Minor collection: merge the card table and shadow card table and store
         * the results to both. Clear bits in  the card table for the active heap.
         */
        regionGcMergeCardTables(gRegionGC);
        dvmClearCardTablePartial(dvmGetActiveHeapBase(), dvmGetActiveHeapSize());
    }
}

bool regionGcIsMinorGc()
{
    if(gRegionGC->startRegionGC && gRegionGC->regionGcType == REGION_GC_MINOR) {
        return true;
    } else {
        return false;
    }
}

/* Only needs clear phase to rescan the dirty objects for minor collection. */
bool regionGcNeedClearPhase()
{
    return regionGcIsMinorGc();
}

/* Only starts partial makr to scan the active heap in minor collection. */
bool regionGcIsPartialMark()
{
    return regionGcIsMinorGc();
}
#endif
