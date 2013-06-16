#include "stdafx.h"
#include "LinearGraph.h"

CLinearGraph::CLinearGraph(int iID, const char *sName, const char *sXUnitName, const char *sYUnitName)
    : CBaseGraph(iID, Graph_Linear, sName, sXUnitName, sYUnitName)
{
}
