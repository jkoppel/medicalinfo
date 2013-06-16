#include "BaseDocument.h"

CBaseDocument::CBaseDocument()
{
    m_iID = 0;
    memset(m_sName, 0, sizeof(m_sName));
    memset(m_sPath, 0, sizeof(m_sPath));
    memset(m_sAuthor, 0, sizeof(m_sAuthor));
    m_iDottedGraphNum = 0;
    memset(m_pDottedGraphList, 0, sizeof(m_pDottedGraphList));
    m_iFacetGraphNum = 0;
    memset(m_pFacetGraphList, 0, sizeof(m_pFacetGraphList));
    m_iLinearGraphNum = 0;
    memset(m_pLinearGraphList, 0, sizeof(m_pLinearGraphList));
    m_iColumnarGraphNum = 0;
    memset(m_pColumnarGraphList, 0, sizeof(m_pColumnarGraphList));
    m_iUnknownGraphNum = 0;
    memset(m_pUnknownGraphList, 0, sizeof(m_pUnknownGraphList));
}

CBaseDocument::CBaseDocument(int iID, const char *sName, const char *sPath, const char *sAuthor) :
    m_iID(iID)
{
    snprintf(m_sName, sizeof(m_sName), "%s", sName);
    snprintf(m_sPath, sizeof(m_sPath), "%s", sPath);
    snprintf(m_sAuthor, sizeof(m_sAuthor), "%s", sAuthor);
    m_iDottedGraphNum = 0;
    memset(m_pDottedGraphList, 0, sizeof(m_pDottedGraphList));
    m_iFacetGraphNum = 0;
    memset(m_pFacetGraphList, 0, sizeof(m_pFacetGraphList));
    m_iLinearGraphNum = 0;
    memset(m_pLinearGraphList, 0, sizeof(m_pLinearGraphList));
    m_iColumnarGraphNum = 0;
    memset(m_pColumnarGraphList, 0, sizeof(m_pColumnarGraphList));
    m_iUnknownGraphNum = 0;
    memset(m_pUnknownGraphList, 0, sizeof(m_pUnknownGraphList));
}

CBaseDocument::~CBaseDocument()
{
    int i;

    for (i=0; i<m_iDottedGraphNum; i++) {
        delete m_pDottedGraphList[i];
    }
    for (i=0; i<m_iFacetGraphNum; i++) {
        delete m_pFacetGraphList[i];
    }
    for (i=0; i<m_iLinearGraphNum; i++) {
        delete m_pLinearGraphList[i];
    }
    for (i=0; i<m_iColumnarGraphNum; i++) {
        delete m_pColumnarGraphList[i];
    }
    for (i=0; i<m_iUnknownGraphNum; i++) {
        delete m_pUnknownGraphList[i];
    }
}

bool CBaseDocument::addDottedGraph(CBaseGraph *pDottedGraph)
{
    if (m_iDottedGraphNum< MAX_GRAPH_NUM) {
        m_pDottedGraphList[m_iDottedGraphNum++] = pDottedGraph;
        return true;
    }
    return false;
}

bool CBaseDocument::removeDottedGraph(int iIndex)
{
    if (iIndex >= 0 && iIndex < m_iDottedGraphNum) {
        delete m_pDottedGraphList[iIndex];
        for (int i=iIndex; i<m_iDottedGraphNum-1; i++) {
            m_pDottedGraphList[i] = m_pDottedGraphList[i++];
        }
        m_pDottedGraphList[m_iDottedGraphNum-1] = NULL;
        m_iDottedGraphNum--;
        return true;
    }

    return false;
}

bool CBaseDocument::addFacetGraph(CBaseGraph *pFacetGraph)
{
    if (m_iFacetGraphNum< MAX_GRAPH_NUM) {
        m_pFacetGraphList[m_iFacetGraphNum++] = pFacetGraph;
        return true;
    }
    return false;
}

bool CBaseDocument::removeFacetGraph(int iIndex)
{
    if (iIndex >= 0 && iIndex < m_iFacetGraphNum) {
        delete m_pFacetGraphList[iIndex];
        for (int i=iIndex; i<m_iFacetGraphNum-1; i++) {
            m_pFacetGraphList[i] = m_pFacetGraphList[i++];
        }
        m_pFacetGraphList[m_iFacetGraphNum-1] = NULL;
        m_iFacetGraphNum--;
        return true;
    }

    return false;
}

bool CBaseDocument::addLinearGraph(CBaseGraph *pLinearGraph)
{
    if (m_iLinearGraphNum< MAX_GRAPH_NUM) {
        m_pLinearGraphList[m_iLinearGraphNum++] = pLinearGraph;
        return true;
    }
    return false;
}

bool CBaseDocument::removeLinearGraph(int iIndex)
{
    if (iIndex >= 0 && iIndex < m_iLinearGraphNum) {
        delete m_pLinearGraphList[iIndex];
        for (int i=iIndex; i<m_iLinearGraphNum-1; i++) {
            m_pLinearGraphList[i] = m_pLinearGraphList[i++];
        }
        m_pLinearGraphList[m_iLinearGraphNum-1] = NULL;
        m_iLinearGraphNum--;
        return true;
    }

    return false;
}

bool CBaseDocument::addColumnarGraph(CBaseGraph *pColumnarGraph)
{
    if (m_iColumnarGraphNum< MAX_GRAPH_NUM) {
        m_pColumnarGraphList[m_iColumnarGraphNum++] = pColumnarGraph;
        return true;
    }
    return false;
}

bool CBaseDocument::removeColumnarGraph(int iIndex)
{
    if (iIndex >= 0 && iIndex < m_iColumnarGraphNum) {
        delete m_pColumnarGraphList[iIndex];
        for (int i=iIndex; i<m_iColumnarGraphNum-1; i++) {
            m_pColumnarGraphList[i] = m_pColumnarGraphList[i++];
        }
        m_pColumnarGraphList[m_iColumnarGraphNum-1] = NULL;
        m_iColumnarGraphNum--;
        return true;
    }

    return false;
}

bool CBaseDocument::addUnknownGraph(CBaseGraph *pUnknownGraph)
{
    if (m_iUnknownGraphNum< MAX_GRAPH_NUM) {
        m_pUnknownGraphList[m_iUnknownGraphNum++] = pUnknownGraph;
        return true;
    }
    return false;
}

bool CBaseDocument::removeUnknownGraph(int iIndex)
{
    if (iIndex >= 0 && iIndex < m_iUnknownGraphNum) {
        delete m_pUnknownGraphList[iIndex];
        for (int i=iIndex; i<m_iUnknownGraphNum-1; i++) {
            m_pUnknownGraphList[i] = m_pUnknownGraphList[i++];
        }
        m_pUnknownGraphList[m_iUnknownGraphNum-1] = NULL;
        m_iUnknownGraphNum--;
        return true;
    }

    return false;
}

int CBaseDocument::getFreeGraphID(GraphType iGraphType)
{
    switch (iGraphType) {
        case Graph_Dotted:
            return getFreeDottedGraphID();
        case Graph_Facet:
            return getFreeFacetGraphID();
        case Graph_Linear:
            return getFreeLinearGraphID();
        case Graph_Columnar:
            return getFreeColumnarGraphID();
        case Graph_Unknown:
            return getFreeUnknownGraphID();
    }

    return 0;
}

bool CBaseDocument::addGraph(GraphType iGraphType, CBaseGraph* pGraph)
{
    switch (iGraphType) {
        case Graph_Dotted:
            return addDottedGraph(pGraph);
        case Graph_Facet:
            return addFacetGraph(pGraph);
        case Graph_Linear:
            return addLinearGraph(pGraph);
        case Graph_Columnar:
            return addColumnarGraph(pGraph);
        case Graph_Unknown:
            return addUnknownGraph(pGraph);
    }
    return false;
}

bool CBaseDocument::removeGraph(GraphType iGraphType, int index)
{
    switch (iGraphType) {
        case Graph_Dotted:
            return removeDottedGraph(index);
        case Graph_Facet:
            return removeFacetGraph(index);
        case Graph_Linear:
            return removeLinearGraph(index);
        case Graph_Columnar:
            return removeColumnarGraph(index);
        case Graph_Unknown:
            return removeUnknownGraph(index);
    }
    return false;
}

int CBaseDocument::getGraphNum(GraphType iGraphType)
{
    switch (iGraphType) {
        case Graph_Dotted:
            return getDottedGraphNum();
        case Graph_Facet:
            return getFacetGraphNum();
        case Graph_Linear:
            return getLinearGraphNum();
        case Graph_Columnar:
            return getColumnarGraphNum();
        case Graph_Unknown:
            return getUnknownGraphNum();
    }

    return 0;
}

CBaseGraph *CBaseDocument::getGraph(GraphType iGraphType, int index)
{
    switch (iGraphType) {
        case Graph_Dotted:
            return getDottedGraph(index);
        case Graph_Facet:
            return getFacetGraph(index);
        case Graph_Linear:
            return getLinearGraph(index);
        case Graph_Columnar:
            return getColumnarGraph(index);
        case Graph_Unknown:
            return getUnknownGraph(index);
    }

    return NULL;
}
