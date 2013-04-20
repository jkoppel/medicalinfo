// LinearGraphView.cpp : implementation file
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "LinearGraphView.h"


// CLinearGraphView

IMPLEMENT_DYNCREATE(CLinearGraphView, CFormView)

CLinearGraphView::CLinearGraphView()
	: CFormView(CLinearGraphView::IDD)
{
    m_rSrcRect = RECT();
    m_pSrcImage = new CImage;
    m_pSrcBitmap = new CBitmap;
}

CLinearGraphView::~CLinearGraphView()
{
    if (!m_pSrcImage->IsNull()) {
        m_pSrcImage->Detach();
    }
    delete m_pSrcBitmap;
    delete m_pSrcImage;
}

void CLinearGraphView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLinearGraphView, CFormView)
END_MESSAGE_MAP()


// CLinearGraphView diagnostics

#ifdef _DEBUG
void CLinearGraphView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CLinearGraphView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CLinearGraphView message handlers
void CLinearGraphView::OnDraw(CDC* pDC)
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

