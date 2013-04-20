// ColumnGraphView.cpp : implementation file
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "ColumnarGraphView.h"


// CColumnarGraphView

IMPLEMENT_DYNCREATE(CColumnarGraphView, CFormView)

CColumnarGraphView::CColumnarGraphView()
	: CFormView(CColumnarGraphView::IDD)
{
    m_rSrcRect = RECT();
    m_pSrcImage = new CImage;
    m_pSrcBitmap = new CBitmap;
}

CColumnarGraphView::~CColumnarGraphView()
{
    if (!m_pSrcImage->IsNull()) {
        m_pSrcImage->Detach();
    }
    delete m_pSrcBitmap;
    delete m_pSrcImage;
}

void CColumnarGraphView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CColumnarGraphView, CFormView)
END_MESSAGE_MAP()


// CColumnarGraphView diagnostics

#ifdef _DEBUG
void CColumnarGraphView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CColumnarGraphView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CColumnarGraphView message handlers
void CColumnarGraphView::OnDraw(CDC* pDC)
{
    pDC=GetDC();

    if (!m_pSrcImage->IsNull()) {
        BITMAP bm;
        m_pSrcBitmap->GetBitmap(&bm);
        
        char *buf = new char[bm.bmWidth * bm.bmWidth];
        for (int i=0; i<bm.bmWidth; i++){
            m_pSrcImage->SetPixel(i, bm.bmHeight/2, RGB(255, 0, 0));
        }
        memset(buf, 0, bm.bmWidth * bm.bmWidth);
        m_pSrcBitmap->GetBitmapBits(bm.bmWidth * bm.bmWidth, buf);
        m_pSrcBitmap->SetBitmapBits(bm.bmWidth * bm.bmWidth, buf);

        m_pSrcImage->Draw(pDC->m_hDC, 100, 100,
                        m_rSrcRect.right - m_rSrcRect.left,
                        m_rSrcRect.bottom - m_rSrcRect.top
                       );

    }
}
