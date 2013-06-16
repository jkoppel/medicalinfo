#ifndef _LINEARGRAPH_H_
#define _LINEARGRAPH_H_

#include "BaseGraph.h"

class CLinearGraph : public CBaseGraph
{
public:
    CLinearGraph(int iID, const char *sName, const char *sXUnitName, const char *sYUnitName);
};

#endif
