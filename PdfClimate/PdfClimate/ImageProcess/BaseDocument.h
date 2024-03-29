#ifndef _BASE_DOCUMENT_H_
#define _BASE_DOCUMENT_H_

#include "GraphDefs.h"
#include "BaseGraph.h"

class CBaseDocument {
public:
    CBaseDocument();
    CBaseDocument(int iID, const char *sName, const char *sPath, const char *sAuthor);
    ~CBaseDocument();
    void setID(int iID) { m_iID = iID; }
    int getID() { return m_iID; }

    void setName(const char *sName) { snprintf(m_sName, sizeof(m_sName), "%s", sName); }
    const char *getName() { return m_sName; }

    void setPath(const char *sPath) { snprintf(m_sPath, sizeof(m_sPath), "%s", sPath); }
    const char *getPath() { return m_sPath; }

    void setAuthor(const char *sAuthor) { snprintf(m_sAuthor, sizeof(m_sAuthor), "%s", sAuthor); }
    const char *getAuthor() { return m_sAuthor; }

    void setTreeItem(void *hItem) { m_hItem = hItem; }
    const void* getTreeItem() { return m_hItem; }

    int getDottedGraphNum() { return m_iDottedGraphNum; }
    CBaseGraph *getDottedGraph(int index) { return m_pDottedGraphList[index]; }
    bool addDottedGraph(CBaseGraph *pDottedGraph);
    bool removeDottedGraph(int index);
    int getFreeDottedGraphID() {
        return (m_iDottedGraphNum==0) ? 1: m_pDottedGraphList[m_iDottedGraphNum - 1]->getID() + 1;
    }

    int getFacetGraphNum() { return m_iFacetGraphNum; }
    CBaseGraph *getFacetGraph(int index) { return m_pFacetGraphList[index]; }
    bool addFacetGraph(CBaseGraph *pFacetGraph);
    bool removeFacetGraph(int index);
    int getFreeFacetGraphID() {
        return (m_iFacetGraphNum==0) ? 1: m_pFacetGraphList[m_iFacetGraphNum - 1]->getID() + 1;
    }

    int getLinearGraphNum() { return m_iLinearGraphNum; }
    CBaseGraph *getLinearGraph(int index) { return m_pLinearGraphList[index]; }
    bool addLinearGraph(CBaseGraph *pLinearGraph);
    bool removeLinearGraph(int index);
    int getFreeLinearGraphID() {
        return (m_iLinearGraphNum==0) ? 1: m_pLinearGraphList[m_iLinearGraphNum - 1]->getID() + 1;
    }

    int getColumnarGraphNum() { return m_iColumnarGraphNum; }
    CBaseGraph *getColumnarGraph(int index) { return m_pColumnarGraphList[index]; }
    bool addColumnarGraph(CBaseGraph *pColumnarGraph);
    bool removeColumnarGraph(int index);
    int getFreeColumnarGraphID() {
        return (m_iColumnarGraphNum==0) ? 1: m_pColumnarGraphList[m_iColumnarGraphNum - 1]->getID() + 1;
    }

    int getUnknownGraphNum() { return m_iUnknownGraphNum; }
    CBaseGraph *getUnknownGraph(int index) { return m_pUnknownGraphList[index]; }
    bool addUnknownGraph(CBaseGraph *pUnknownGraph);
    bool removeUnknownGraph(int index);
    int getFreeUnknownGraphID() {
        return (m_iUnknownGraphNum==0) ? 1: m_pUnknownGraphList[m_iUnknownGraphNum - 1]->getID() + 1;
    }

    int getGraphNum(GraphType iGraphType);
    CBaseGraph *getGraph(GraphType iGraphType, int index);
    bool addGraph(GraphType iGraphType, CBaseGraph* pGraph);
    bool removeGraph(GraphType iGraphType, int index);
    int getFreeGraphID(GraphType iGraphType);

protected:
    int m_iID;
    char m_sName[256];
    char m_sPath[256];
    char m_sAuthor[256];
    void* m_hItem;
    int m_iDottedGraphNum;
    CBaseGraph *m_pDottedGraphList[MAX_GRAPH_NUM];
    int m_iFacetGraphNum;
    CBaseGraph *m_pFacetGraphList[MAX_GRAPH_NUM];
    int m_iLinearGraphNum;
    CBaseGraph *m_pLinearGraphList[MAX_GRAPH_NUM];
    int m_iColumnarGraphNum;
    CBaseGraph *m_pColumnarGraphList[MAX_GRAPH_NUM];
    int m_iUnknownGraphNum;
    CBaseGraph *m_pUnknownGraphList[MAX_GRAPH_NUM];
};

#endif