#include "BaseDocument.h"

BaseDocument::BaseDocument()
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

BaseDocument::BaseDocument(int iID, const char *sName, const char *sPath, const char *sAuthor) :
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

BaseDocument::~BaseDocument()
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

bool BaseDocument::addDottedGraph(BaseGraph *pDottedGraph)
{
    if (m_iDottedGraphNum< MAX_GRAPH_NUM) {
        m_pDottedGraphList[m_iDottedGraphNum++] = pDottedGraph;
        return true;
    }
    return false;
}

bool BaseDocument::removeDottedGraph(int iIndex)
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

bool BaseDocument::addFacetGraph(BaseGraph *pFacetGraph)
{
    if (m_iFacetGraphNum< MAX_GRAPH_NUM) {
        m_pFacetGraphList[m_iFacetGraphNum++] = pFacetGraph;
        return true;
    }
    return false;
}

bool BaseDocument::removeFacetGraph(int iIndex)
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

bool BaseDocument::addLinearGraph(BaseGraph *pLinearGraph)
{
    if (m_iLinearGraphNum< MAX_GRAPH_NUM) {
        m_pLinearGraphList[m_iLinearGraphNum++] = pLinearGraph;
        return true;
    }
    return false;
}

bool BaseDocument::removeLinearGraph(int iIndex)
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

bool BaseDocument::addColumnarGraph(BaseGraph *pColumnarGraph)
{
    if (m_iColumnarGraphNum< MAX_GRAPH_NUM) {
        m_pColumnarGraphList[m_iColumnarGraphNum++] = pColumnarGraph;
        return true;
    }
    return false;
}

bool BaseDocument::removeColumnarGraph(int iIndex)
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

bool BaseDocument::addUnknownGraph(BaseGraph *pUnknownGraph)
{
    if (m_iUnknownGraphNum< MAX_GRAPH_NUM) {
        m_pUnknownGraphList[m_iUnknownGraphNum++] = pUnknownGraph;
        return true;
    }
    return false;
}

bool BaseDocument::removeUnknownGraph(int iIndex)
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
