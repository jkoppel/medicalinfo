#include "stdafx.h"
#include "UnknownGraph.h"

CUnknownGraph::CUnknownGraph(int iID, const char *sName, const char *sXUnitName, const char *sYUnitName)
    : CBaseGraph(iID, Graph_Unknown, sName, sXUnitName, sYUnitName)
{
}
