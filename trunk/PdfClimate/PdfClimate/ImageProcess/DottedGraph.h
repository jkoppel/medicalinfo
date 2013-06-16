#ifndef _DOTTEDGRAPH_H_
#define _DOTTEDGRAPH_H_

#include "BaseGraph.h"

class CDottedGraph : public CBaseGraph
{
public:
    CDottedGraph(int iID, const char *sName, const char *sXUnitName, const char *sYUnitName);
};

#endif
