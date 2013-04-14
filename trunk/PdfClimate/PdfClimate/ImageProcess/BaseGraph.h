#ifndef _BASE_GRAPH_H_
#define _BASE_GRAPH_H_

#include "GraphDefs.h"
#include "BaseGraphItem.h"

class BaseDocument;

class BaseGraph {
public:
    BaseGraph();
    BaseGraph(int iID, GraphType iType, const char *sName, const char *sXUnitName, const char *sYUnitName);
    ~BaseGraph();
    void setID(int iID) { m_iID = iID; }
    int getID() { return m_iID; }
    void setName(const char *sName) { snprintf(m_sName, sizeof(m_sName), "%s", sName); }
    const char *getName() { return m_sName; }
    void setType(GraphType iType) { m_iType = iType; }
    GraphType getType() { return m_iType; }
	void setBitmapInfo(const BITMAPINFO *bitmap);
    const BITMAPINFO *getBitmapInfo() { return m_pDIBInfo; }
    void setXUnitName(const char sXUnitName) { snprintf(m_sXUnitName, sizeof(m_sXUnitName), "%s", sXUnitName); }
    const char *getXUnitName() { return m_sXUnitName; }
    void setYUnitName(const char sYUnitName) { snprintf(m_sYUnitName, sizeof(m_sYUnitName), "%s", sYUnitName); }
    const char *getYUnitName() { return m_sYUnitName; }
    bool addGraphItem(BaseGraphItem *pGraphItem);
    bool removeGraphItem(int iIndex);
    void setBaseDocument(const BaseDocument *pBaseDocument) { m_pBaseDocument = pBaseDocument; }
    const BaseDocument *getBaseDocument(const BaseDocument *pBaseDocument) { return m_pBaseDocument; }

protected:
    int m_iID;
    GraphType m_iType;
    char m_sName[256];
    BITMAPINFO *m_pDIBInfo;
    char m_sXUnitName[256];
    char m_sYUnitName[256];
    int m_iGraphItemNum;
    BaseGraphItem *m_pGraphItemList[MAX_GRAPH_ITEM_NUM];
    const BaseDocument *m_pBaseDocument;
};

#endif
