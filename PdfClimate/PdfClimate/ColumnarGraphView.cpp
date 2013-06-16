// ColumnGraphView.cpp : implementation file
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "ColumnarGraphView.h"


// CColumnarGraphView

IMPLEMENT_DYNCREATE(CColumnarGraphView, CBaseGraphView)

CColumnarGraphView::CColumnarGraphView()
	: CBaseGraphView(CColumnarGraphView::IDD, Graph_Columnar)
{
}

CColumnarGraphView::~CColumnarGraphView()
{
}

void CColumnarGraphView::DoDataExchange(CDataExchange* pDX)
{
	CBaseGraphView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CColumnarGraphView, CBaseGraphView)
    ON_BN_CLICKED(IDC_GRAPHINFO_BTN_EDITSAVE, &CColumnarGraphView::OnBnClickedGraphinfoBtnEditsave)
END_MESSAGE_MAP()


// CColumnarGraphView diagnostics

#ifdef _DEBUG
void CColumnarGraphView::AssertValid() const
{
	CBaseGraphView::AssertValid();
}

#ifndef _WIN32_WCE
void CColumnarGraphView::Dump(CDumpContext& dc) const
{
	CBaseGraphView::Dump(dc);
}
#endif
#endif //_DEBUG


// CColumnarGraphView message handlers
void CColumnarGraphView::OnDraw(CDC* pDC)
{
    CBaseGraphView::OnDraw(pDC);
}


void CColumnarGraphView::OnInitialUpdate()
{
    CBaseGraphView::OnInitialUpdate();
}


void CColumnarGraphView::OnBnClickedGraphinfoBtnEditsave()
{
    CBaseGraphView::OnBnClickedGraphinfoBtnEditsave();
}
