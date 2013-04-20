// DottedGraphView.cpp : implementation file
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "DottedGraphView.h"
#include "GlobalVars.h"
#include "LeftView.h"


// CDottedGraphView

IMPLEMENT_DYNCREATE(CDottedGraphView, CFormView)

CDottedGraphView::CDottedGraphView()
	: CFormView(CDottedGraphView::IDD)
{
    m_rSrcRect = RECT();
    m_pSrcImage = new CImage;
    m_pSrcBitmap = new CBitmap;
}

CDottedGraphView::~CDottedGraphView()
{
    if (!m_pSrcImage->IsNull()) {
        m_pSrcImage->Detach();
    }
    delete m_pSrcBitmap;
    delete m_pSrcImage;
}

void CDottedGraphView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDottedGraphView, CFormView)
END_MESSAGE_MAP()


// CDottedGraphView diagnostics

#ifdef _DEBUG
void CDottedGraphView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDottedGraphView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDottedGraphView message handlers


void CDottedGraphView::OnDraw(CDC* pDC)
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

