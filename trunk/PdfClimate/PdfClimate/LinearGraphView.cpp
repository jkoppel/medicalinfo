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

}

CLinearGraphView::~CLinearGraphView()
{
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
