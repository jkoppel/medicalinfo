// ColumnGraphView.cpp : implementation file
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "ColumnarGraphView.h"


// CColumnarGraphView

IMPLEMENT_DYNCREATE(CColumnarGraphView, CGraphFormView)

CColumnarGraphView::CColumnarGraphView()
	: CGraphFormView(CColumnarGraphView::IDD)
{
}

CColumnarGraphView::~CColumnarGraphView()
{
}

void CColumnarGraphView::DoDataExchange(CDataExchange* pDX)
{
	CGraphFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CColumnarGraphView, CGraphFormView)
END_MESSAGE_MAP()


// CColumnarGraphView diagnostics

#ifdef _DEBUG
void CColumnarGraphView::AssertValid() const
{
	CGraphFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CColumnarGraphView::Dump(CDumpContext& dc) const
{
	CGraphFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CColumnarGraphView message handlers
void CColumnarGraphView::OnDraw(CDC* pDC)
{
    CGraphFormView::OnDraw(pDC);
}


void CColumnarGraphView::OnInitialUpdate()
{
    CGraphFormView::OnInitialUpdate();
}
