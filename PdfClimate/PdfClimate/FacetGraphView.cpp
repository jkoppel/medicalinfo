// FacetGraphView.cpp : implementation file
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "FacetGraphView.h"


// CFacetGraphView

IMPLEMENT_DYNCREATE(CFacetGraphView, CFormView)

CFacetGraphView::CFacetGraphView()
	: CFormView(CFacetGraphView::IDD)
{

}

CFacetGraphView::~CFacetGraphView()
{
}

void CFacetGraphView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFacetGraphView, CFormView)
END_MESSAGE_MAP()


// CFacetGraphView diagnostics

#ifdef _DEBUG
void CFacetGraphView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFacetGraphView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFacetGraphView message handlers
