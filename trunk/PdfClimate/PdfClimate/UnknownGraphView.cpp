// UnknownGraphView.cpp : implementation file
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "UnknownGraphView.h"


// CUnknownGraphView

IMPLEMENT_DYNCREATE(CUnknownGraphView, CGraphFormView)

CUnknownGraphView::CUnknownGraphView()
	: CGraphFormView(CUnknownGraphView::IDD, Graph_Unknown)
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
    ON_BN_CLICKED(IDC_GRAPHINFO_BTN_EDITSAVE, &CUnknownGraphView::OnBnClickedGraphinfoBtnEditsave)
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


void CUnknownGraphView::OnInitialUpdate()
{
    CGraphFormView::OnInitialUpdate();
}


void CUnknownGraphView::OnBnClickedGraphinfoBtnEditsave()
{
    CGraphFormView::OnBnClickedGraphinfoBtnEditsave();
}
