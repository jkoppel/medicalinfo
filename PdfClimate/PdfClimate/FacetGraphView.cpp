// FacetGraphView.cpp : implementation file
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "FacetGraphView.h"


// CFacetGraphView

IMPLEMENT_DYNCREATE(CFacetGraphView, CBaseGraphView)

CFacetGraphView::CFacetGraphView()
	: CBaseGraphView(CFacetGraphView::IDD, Graph_Facet)
{
}

CFacetGraphView::~CFacetGraphView()
{
}

void CFacetGraphView::DoDataExchange(CDataExchange* pDX)
{
	CBaseGraphView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFacetGraphView, CBaseGraphView)
    ON_BN_CLICKED(IDC_GRAPHINFO_BTN_EDITSAVE, &CFacetGraphView::OnBnClickedGraphinfoBtnEditsave)
END_MESSAGE_MAP()


// CFacetGraphView diagnostics

#ifdef _DEBUG
void CFacetGraphView::AssertValid() const
{
	CBaseGraphView::AssertValid();
}

#ifndef _WIN32_WCE
void CFacetGraphView::Dump(CDumpContext& dc) const
{
	CBaseGraphView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFacetGraphView message handlers
void CFacetGraphView::OnDraw(CDC* pDC)
{
    CBaseGraphView::OnDraw(pDC);
}



void CFacetGraphView::OnInitialUpdate()
{
    CBaseGraphView::OnInitialUpdate();
}


void CFacetGraphView::OnBnClickedGraphinfoBtnEditsave()
{
    CBaseGraphView::OnBnClickedGraphinfoBtnEditsave();
}
