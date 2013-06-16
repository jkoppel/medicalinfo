#include "stdafx.h"
#include "FacetGraph.h"

CFacetGraph::CFacetGraph(int iID, const char *sName, const char *sXUnitName, const char *sYUnitName)
    : CBaseGraph(iID, Graph_Facet, sName, sXUnitName, sYUnitName)
{
}
