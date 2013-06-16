#include "stdafx.h"
#include "ColumnarGraph.h"

CColumnarGraph::CColumnarGraph(int iID, const char *sName, const char *sXUnitName, const char *sYUnitName)
    : CBaseGraph(iID, Graph_Columnar, sName, sXUnitName, sYUnitName)
{
}
