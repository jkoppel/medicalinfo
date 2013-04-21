// FacetGraphView.cpp : implementation file
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "FacetGraphView.h"


// CFacetGraphView

IMPLEMENT_DYNCREATE(CFacetGraphView, CFormView)

CFacetGraphView::CFacetGraphView()
	: CFormView(CFacetGraphView::IDD)
{
    m_rSrcRect = RECT();
    m_pSrcImage = new CImage;
    m_pSrcBitmap = new CBitmap;
}

CFacetGraphView::~CFacetGraphView()
{
    if (!m_pSrcImage->IsNull()) {
        m_pSrcImage->Detach();
    }
    delete m_pSrcBitmap;
    delete m_pSrcImage;
}

void CFacetGraphView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFacetGraphView, CFormView)
END_MESSAGE_MAP()


// CFacetGraphView diagnostics

#ifdef _DEBUG
void CFacetGraphView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFacetGraphView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFacetGraphView message handlers
void CFacetGraphView::OnDraw(CDC* pDC)
{
    pDC=GetDC();

    if (!m_pSrcImage->IsNull()) {
        BITMAP bm;
        m_pSrcBitmap->GetBitmap(&bm);

        /*
        char *buf = new char[bm.bmWidth * bm.bmWidth];
        memset(buf, 0, bm.bmWidth * bm.bmWidth);
        m_pSrcBitmap->GetBitmapBits(bm.bmWidth * bm.bmWidth, buf);
        m_pSrcBitmap->SetBitmapBits(bm.bmWidth * bm.bmWidth, buf);
        delete []buf;
        */

        m_pSrcImage->Draw(pDC->m_hDC, 100, 100,
                        m_rSrcRect.right - m_rSrcRect.left,
                        m_rSrcRect.bottom - m_rSrcRect.top
                       );

    }
}

