#include "BaseGraphItem.h"
#include "BaseGraph.h"
#include "BaseDocument.h"


CBaseGraphItem::CBaseGraphItem()
{
    memset(m_sName, 0, sizeof(m_sName));
    m_iPageIndex = 0;
    memset(m_cppMapOfCoor, 0, sizeof(m_cppMapOfCoor));
    m_iPixelRecognizedNum = 0;
    m_pPixelRecognizedList = NULL;
    m_pBaseDocument = NULL;
    m_pBaseGraph = NULL;

    m_pDIBInfo = dib_init();
}

CBaseGraphItem::CBaseGraphItem(const char *sName, int iPageIndex)
{
    memset(m_cppMapOfCoor, 0, sizeof(m_cppMapOfCoor));
    m_iPixelRecognizedNum = 0;
    m_pPixelRecognizedList = NULL;
    m_pBaseDocument = NULL;
    m_pBaseGraph = NULL;

    snprintf(m_sName, sizeof(m_sName), "%s", sName);
    m_iPageIndex = iPageIndex;

    m_pDIBInfo = dib_init();
}

CBaseGraphItem::~CBaseGraphItem()
{
    dib_destroy(m_pDIBInfo);
    if (m_pPixelRecognizedList != NULL) {
        delete []m_pPixelRecognizedList;
    }
}

void CBaseGraphItem::setBitmapInfo(const BITMAPINFO *bitmap)
{
    //TODO
}

void CBaseGraphItem::setPixelRecognizedInfo(int iPixNum, const POINT *pPixelList)
{
    m_iPixelRecognizedNum = iPixNum;
    if (m_pPixelRecognizedList != NULL) {
        delete []m_pPixelRecognizedList;
    }
    m_pPixelRecognizedList = new POINT[iPixNum];
    memcpy(m_pPixelRecognizedList, pPixelList, sizeof(POINT) * iPixNum);
}

