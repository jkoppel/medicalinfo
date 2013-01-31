
#ifdef PROFILE_OPCODE

#include "mterp/Mterp.h"

int opcodeFastInterp[512]={0};

void fastinterptouch(int inst) {
 opcodeFastInterp[inst & 0xff]++;
}

#endif


