// GraphClassView.cpp : implementation file
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "GraphFormView.h"


// CGraphFormView

IMPLEMENT_DYNCREATE(CGraphFormView, CFormView)

CGraphFormView::CGraphFormView()
	: CFormView(CGraphFormView::IDD)
{
    m_pSrcImage = new CImage;
    m_pSrcBitmap = new CBitmap;
}

CGraphFormView::CGraphFormView(UINT nIDTemplate)
	: CFormView(nIDTemplate)
{
    m_pSrcImage = new CImage;
    m_pSrcBitmap = new CBitmap;
}

CGraphFormView::~CGraphFormView()
{
    if (m_pSrcImage) {
        if (!m_pSrcImage->IsNull()) {
            m_pSrcImage->Detach();
        }
        delete m_pSrcBitmap;
        delete m_pSrcImage;
    }
}

void CGraphFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_MAIN_IMAGE, m_ctrlMainImage);
}

BEGIN_MESSAGE_MAP(CGraphFormView, CFormView)
END_MESSAGE_MAP()


// CGraphFormView diagnostics

#ifdef _DEBUG
void CGraphFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CGraphFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CGraphFormView message handlers


void CGraphFormView::OnDraw(CDC* pDC)
{
    if (!m_pSrcImage->IsNull()) {
        CRect rect;
        int width, height;

        width = m_pSrcImage->GetWidth();
        height = m_pSrcImage->GetHeight();
        m_ctrlMainImage.SetWindowPos(NULL, 0, 0, width, height + 6, SWP_NOMOVE);
        pDC = m_ctrlMainImage.GetDC();
        m_pSrcImage->GetWidth();
        m_pSrcImage->Draw(pDC->m_hDC, 0, 6, width, height);
    }
}
