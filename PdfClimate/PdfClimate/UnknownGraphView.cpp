// UnknownGraphView.cpp : implementation file
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "UnknownGraphView.h"


// CUnknownGraphView

IMPLEMENT_DYNCREATE(CUnknownGraphView, CGraphFormView)

CUnknownGraphView::CUnknownGraphView()
	: CGraphFormView(CUnknownGraphView::IDD)
{
}

CUnknownGraphView::~CUnknownGraphView()
{
}

void CUnknownGraphView::DoDataExchange(CDataExchange* pDX)
{
	CGraphFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CUnknownGraphView, CGraphFormView)
END_MESSAGE_MAP()


// CUnknownGraphView diagnostics

#ifdef _DEBUG
void CUnknownGraphView::AssertValid() const
{
	CGraphFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CUnknownGraphView::Dump(CDumpContext& dc) const
{
	CGraphFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CUnknownGraphView message handlers
void CUnknownGraphView::OnDraw(CDC* pDC)
{
    CGraphFormView::OnDraw(pDC);
}
