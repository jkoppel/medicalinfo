#ifndef _BASE_DOCUMENT_H_
#define _BASE_DOCUMENT_H_

#include "GraphDefs.h"
#include "BaseGraph.h"

class BaseDocument {
public:
    BaseDocument();
    BaseDocument(int iID, const char *sName, const char *sPath, const char *sAuthor);
    ~BaseDocument();
    void setID(int iID) { m_iID = iID; }
    int getID() { return m_iID; }
    void setName(const char *sName) { snprintf(m_sName, sizeof(m_sName), "%s", sName); }
    const char *getName() { return m_sName; }
    void setPath(const char *sPath) { snprintf(m_sPath, sizeof(m_sPath), "%s", sPath); }
    const char *getPath() { return m_sPath; }
    void setAuthor(const char *sAuthor) { snprintf(m_sAuthor, sizeof(m_sAuthor), "%s", sAuthor); }
    const char *getAuthor() { return m_sAuthor; }
    int getDottedGraphNum() { return m_iDottedGraphNum; }
    BaseGraph *getDottedGraph(int index) { return m_pDottedGraphList[index]; }
    bool addDottedGraph(BaseGraph *pDottedGraph);
    bool removeDottedGraph(int index);
    int getFacetGraphNum() { return m_iFacetGraphNum; }
    BaseGraph *getFacetGraph(int index) { return m_pFacetGraphList[index]; }
    bool addFacetGraph(BaseGraph *pFacetGraph);
    bool removeFacetGraph(int index);
    int getLinearGraphNum() { return m_iLinearGraphNum; }
    BaseGraph *getLinearGraph(int index) { return m_pLinearGraphList[index]; }
    bool addLinearGraph(BaseGraph *pLinearGraph);
    bool removeLinearGraph(int index);
    int getColumnarGraphNum() { return m_iColumnarGraphNum; }
    BaseGraph *getColumnarGraph(int index) { return m_pColumnarGraphList[index]; }
    bool addColumnarGraph(BaseGraph *pColumnarGraph);
    bool removeColumnarGraph(int index);
    int getUnknownGraphNum() { return m_iUnknownGraphNum; }
    BaseGraph *getUnknownGraph(int index) { return m_pUnknownGraphList[index]; }
    bool addUnknownGraph(BaseGraph *pUnknownGraph);
    bool removeUnknownGraph(int index);

protected:
    int m_iID;
    char m_sName[256];
    char m_sPath[256];
    char m_sAuthor[256];
    int m_iDottedGraphNum;
    BaseGraph *m_pDottedGraphList[MAX_GRAPH_NUM];
    int m_iFacetGraphNum;
    BaseGraph *m_pFacetGraphList[MAX_GRAPH_NUM];
    int m_iLinearGraphNum;
    BaseGraph *m_pLinearGraphList[MAX_GRAPH_NUM];
    int m_iColumnarGraphNum;
    BaseGraph *m_pColumnarGraphList[MAX_GRAPH_NUM];
    int m_iUnknownGraphNum;
    BaseGraph *m_pUnknownGraphList[MAX_GRAPH_NUM];
};

#endif