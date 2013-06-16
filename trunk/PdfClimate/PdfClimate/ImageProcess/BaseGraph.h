#ifndef _BASE_GRAPH_H_
#define _BASE_GRAPH_H_

#include "GraphDefs.h"
#include "BaseGraphItem.h"

class CBaseDocument;

class CBaseGraph {
public:
    CBaseGraph();
    CBaseGraph(int iID, GraphType iType, const char *sName, const char *sXUnitName, const char *sYUnitName);
    ~CBaseGraph();

    void setID(int iID) { m_iID = iID; }
    int getID() { return m_iID; }

    void setName(const char *sName) {
        snprintf(m_sName, sizeof(m_sName), "%s", sName);
    }
    void getName(char *sName, int iMaxLength = -1) {
        snprintf(sName, iMaxLength, "%s", m_sName);
    }

    void setType(GraphType iType) { m_iType = iType; }
    GraphType getType() { return m_iType; }

    void setBitmap(BITMAP bitmap) { m_bmpInfo = bitmap; }
    void getBitmap(BITMAP *pBitmap) { *pBitmap = m_bmpInfo; }
    void setBitmapBits(int nCount, const unsigned char *pBits) {
        if (m_pBmpBits) {
            delete []m_pBmpBits;
        }
        m_pBmpBits = new unsigned char[nCount];
        if (m_pBmpBits) {
            memcpy(m_pBmpBits, pBits, nCount);
        }
    }
    void getBitmapBits(int nCount, unsigned char *pBits) {
        if (m_pBmpBits) {
            memcpy(pBits, m_pBmpBits, m_bmpInfo.bmWidthBytes * m_bmpInfo.bmHeight);
        }
    }

    void setXUnitName(const char *sXUnitName) {
        snprintf(m_sXUnitName, sizeof(m_sXUnitName), "%s", sXUnitName);
    }
    void getXUnitName(char *sXUnitName, int iMaxLength=-1) {
        snprintf(sXUnitName, iMaxLength, "%s", m_sXUnitName);
    }
    void setYUnitName(const char *sYUnitName) {
        snprintf(m_sYUnitName, sizeof(m_sYUnitName), "%s", sYUnitName);
    }
    void getYUnitName(char *sYUnitName, int iMaxLength=-1) {
        snprintf(sYUnitName, iMaxLength, "%s", m_sYUnitName);
    }

    bool addGraphItem(CBaseGraphItem *pGraphItem);
    bool removeGraphItem(int iIndex);
    void setBaseDocument(const CBaseDocument *pBaseDocument) { m_pBaseDocument = pBaseDocument; }
    const CBaseDocument *getBaseDocument(const CBaseDocument *pBaseDocument) { return m_pBaseDocument; }

protected:
    int m_iID;
    GraphType m_iType;
    char m_sName[256];
    BITMAP m_bmpInfo;
    unsigned char *m_pBmpBits;
    char m_sXUnitName[256];
    char m_sYUnitName[256];
    int m_iGraphItemNum;
    CBaseGraphItem *m_pGraphItemList[MAX_GRAPH_ITEM_NUM];
    const CBaseDocument *m_pBaseDocument;
};

#endif
