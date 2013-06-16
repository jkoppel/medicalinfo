// FacetGraphView.cpp : implementation file
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "FacetGraphView.h"


// CFacetGraphView

IMPLEMENT_DYNCREATE(CFacetGraphView, CGraphFormView)

CFacetGraphView::CFacetGraphView()
	: CGraphFormView(CFacetGraphView::IDD, Graph_Facet)
{
}

CFacetGraphView::~CFacetGraphView()
{
}

void CFacetGraphView::DoDataExchange(CDataExchange* pDX)
{
	CGraphFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFacetGraphView, CGraphFormView)
    ON_BN_CLICKED(IDC_GRAPHINFO_BTN_EDITSAVE, &CFacetGraphView::OnBnClickedGraphinfoBtnEditsave)
END_MESSAGE_MAP()


// CFacetGraphView diagnostics

#ifdef _DEBUG
void CFacetGraphView::AssertValid() const
{
	CGraphFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFacetGraphView::Dump(CDumpContext& dc) const
{
	CGraphFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFacetGraphView message handlers
void CFacetGraphView::OnDraw(CDC* pDC)
{
    CGraphFormView::OnDraw(pDC);
}



void CFacetGraphView::OnInitialUpdate()
{
    CGraphFormView::OnInitialUpdate();
}


void CFacetGraphView::OnBnClickedGraphinfoBtnEditsave()
{
    CGraphFormView::OnBnClickedGraphinfoBtnEditsave();
}
