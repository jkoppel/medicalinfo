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

}

CColumnarGraphView::~CColumnarGraphView()
{
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
