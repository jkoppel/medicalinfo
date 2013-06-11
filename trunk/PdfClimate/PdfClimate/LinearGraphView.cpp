// LinearGraphView.cpp : implementation file
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "LinearGraphView.h"


// CLinearGraphView

IMPLEMENT_DYNCREATE(CLinearGraphView, CGraphFormView)

CLinearGraphView::CLinearGraphView()
	: CGraphFormView(CLinearGraphView::IDD)
{
}

CLinearGraphView::~CLinearGraphView()
{
}

void CLinearGraphView::DoDataExchange(CDataExchange* pDX)
{
	CGraphFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLinearGraphView, CGraphFormView)
END_MESSAGE_MAP()


// CLinearGraphView diagnostics

#ifdef _DEBUG
void CLinearGraphView::AssertValid() const
{
	CGraphFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CLinearGraphView::Dump(CDumpContext& dc) const
{
	CGraphFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CLinearGraphView message handlers
void CLinearGraphView::OnDraw(CDC* pDC)
{
    CGraphFormView::OnDraw(pDC);
}

