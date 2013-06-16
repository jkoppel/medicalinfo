#ifndef _UNKNOWNGRAPH_H_
#define _UNKNOWNGRAPH_H_

#include "BaseGraph.h"

class CUnknownGraph : public CBaseGraph
{
public:
    CUnknownGraph(int iID, const char *sName, const char *sXUnitName, const char *sYUnitName);
};

#endif
