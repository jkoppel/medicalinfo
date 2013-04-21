// UnknownGraphView.cpp : implementation file
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "UnknownGraphView.h"


// CUnknownGraphView

IMPLEMENT_DYNCREATE(CUnknownGraphView, CFormView)

CUnknownGraphView::CUnknownGraphView()
	: CFormView(CUnknownGraphView::IDD)
{
    m_rSrcRect = RECT();
    m_pSrcImage = new CImage;
    m_pSrcBitmap = new CBitmap;
}

CUnknownGraphView::~CUnknownGraphView()
{
    if (!m_pSrcImage->IsNull()) {
        m_pSrcImage->Detach();
    }
    delete m_pSrcBitmap;
    delete m_pSrcImage;
}

void CUnknownGraphView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CUnknownGraphView, CFormView)
END_MESSAGE_MAP()


// CUnknownGraphView diagnostics

#ifdef _DEBUG
void CUnknownGraphView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CUnknownGraphView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CUnknownGraphView message handlers
void CUnknownGraphView::OnDraw(CDC* pDC)
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
