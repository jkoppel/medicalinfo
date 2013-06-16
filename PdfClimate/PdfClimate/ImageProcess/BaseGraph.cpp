#include "BaseGraph.h"
#include "BaseGraphItem.h"
#include "BaseDocument.h"
#include "GraphDefs.h"

CBaseGraph::CBaseGraph()
{
    m_iID = 0;
    m_iType = Graph_Unknown;
    memset(m_sName, 0, sizeof(m_sName));
    m_pDIBInfo = NULL;
    memset(m_sXUnitName, 0, sizeof(m_sXUnitName));
    memset(m_sYUnitName, 0, sizeof(m_sYUnitName));
    m_iGraphItemNum = 0;
    memset(m_pGraphItemList, 0, sizeof(m_pGraphItemList));

    m_pDIBInfo = dib_init();
}

CBaseGraph::CBaseGraph(int iID, GraphType iType, const char *sName, const char *sXUnitName, const char *sYUnitName):
    m_iID(iID), m_iType(iType)
{
    snprintf(m_sName, sizeof(m_sName), "%s", sName);
    m_pDIBInfo = NULL;
    snprintf(m_sXUnitName, sizeof(m_sXUnitName), "%s", sXUnitName);
    snprintf(m_sYUnitName, sizeof(m_sYUnitName), "%s", sYUnitName);
    m_iGraphItemNum = 0;
    memset(m_pGraphItemList, 0, sizeof(m_pGraphItemList));

    m_pDIBInfo = dib_init();
}

CBaseGraph::~CBaseGraph()
{
    dib_destroy(m_pDIBInfo);
}

void CBaseGraph::setBitmapInfo(const BITMAPINFO *bitmap)
{
    //TODO
}

bool CBaseGraph::addGraphItem(CBaseGraphItem *pGraphItem)
{
    if (m_iGraphItemNum < MAX_GRAPH_ITEM_NUM) {
        m_pGraphItemList[m_iGraphItemNum++] = pGraphItem;
        return true;
    }
    return false;
}

bool CBaseGraph::removeGraphItem(int iIndex)
{
    if (iIndex >= 0 && iIndex < m_iGraphItemNum) {
        delete m_pGraphItemList[iIndex];
        for (int i=iIndex; i<m_iGraphItemNum-1; i++) {
            m_pGraphItemList[i] = m_pGraphItemList[i++];
        }
        m_pGraphItemList[m_iGraphItemNum-1] = NULL;
        m_iGraphItemNum --;
        return true;
    }

    return false;
}

