#ifndef _BASE_GRAPH_ITEM_H_
#define _BASE_GRAPH_ITEM_H_

#include "GraphDefs.h"

class BaseDocument;
class BaseGraph;

class BaseGraphItem {
public:
	BaseGraphItem();
    BaseGraphItem(const char *sName, int iPageIndex);
    ~BaseGraphItem();

    void setName(const char *sName) { snprintf(m_sName, sizeof(m_sName), "%s", sName); }
    const char *getName() { return m_sName; }
    void setID(int iID) { m_iID = iID; }
    int getID() { return m_iID; }
    void setPageIndex(int iPageIndex) { m_iPageIndex = iPageIndex; }
    int getPageIndex() { return m_iPageIndex; }
	void setBitmapInfo(const BITMAPINFO *bitmap);
    const BITMAPINFO *getBitmapInfo() { return m_pDIBInfo; }
    void setCoordinateInfo(CoorPixelPair map0, CoorPixelPair map1) { m_cppMapOfCoor[0] = map0; m_cppMapOfCoor[1] = map1; }
	void getCoordinateInfo(CoorPixelPair &map0, CoorPixelPair &map1) { map0 = m_cppMapOfCoor[0]; map1 = m_cppMapOfCoor[1]; }
    void setPixelRecognizedInfo(int iPixNum, const POINT *pPixelList);
    int BaseGraphItem::getPixelRecognizedNum() { return m_iPixelRecognizedNum; }
    const POINT *BaseGraphItem::getPixelRecognizedList() { return m_pPixelRecognizedList; }
    void setBaseGraph(const BaseGraph *pBaseGraph) { m_pBaseGraph = pBaseGraph; }
    const BaseGraph *getBaseGraph() { return m_pBaseGraph; }
    void setBaseDocument(const BaseDocument *pBaseDocument) {  m_pBaseDocument = pBaseDocument; }
    const BaseDocument *getBaseDocument() { return m_pBaseDocument; }
    virtual void drawGraph() {};

protected:
    int m_iID;
    char m_sName[256];
    int m_iPageIndex;
    BITMAPINFO *m_pDIBInfo;
    CoorPixelPair m_cppMapOfCoor[2];
    int m_iPixelRecognizedNum;
    POINT *m_pPixelRecognizedList;
    const BaseGraph *m_pBaseGraph;
    const BaseDocument *m_pBaseDocument;
};

#endif
