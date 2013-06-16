#ifndef _FACETGRAPH_H_
#define _FACETGRAPH_H_

#include "BaseGraph.h"

class CFacetGraph : public CBaseGraph
{
public:
    CFacetGraph(int iID, const char *sName, const char *sXUnitName, const char *sYUnitName);
};

#endif
