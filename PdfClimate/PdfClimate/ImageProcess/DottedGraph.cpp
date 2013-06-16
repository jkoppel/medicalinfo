#include "stdafx.h"
#include "DottedGraph.h"

CDottedGraph::CDottedGraph(int iID, const char *sName, const char *sXUnitName, const char *sYUnitName)
    : CBaseGraph(iID, Graph_Dotted, sName, sXUnitName, sYUnitName)
{
}
