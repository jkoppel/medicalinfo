#ifndef _COLUMNARGRAPH_H_
#define _COLUMNARGRAPH_H_

#include "BaseGraph.h"

class CColumnarGraph : public CBaseGraph
{
public:
    CColumnarGraph(int iID, const char *sName, const char *sXUnitName, const char *sYUnitName);
};

#endif
