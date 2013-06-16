// UnknownGraphView.cpp : implementation file
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "UnknownGraphView.h"


// CUnknownGraphView

IMPLEMENT_DYNCREATE(CUnknownGraphView, CBaseGraphView)

CUnknownGraphView::CUnknownGraphView()
	: CBaseGraphView(CUnknownGraphView::IDD, Graph_Unknown)
{
}

CUnknownGraphView::~CUnknownGraphView()
{
}

void CUnknownGraphView::DoDataExchange(CDataExchange* pDX)
{
	CBaseGraphView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CUnknownGraphView, CBaseGraphView)
    ON_BN_CLICKED(IDC_GRAPHINFO_BTN_EDITSAVE, &CUnknownGraphView::OnBnClickedGraphinfoBtnEditsave)
END_MESSAGE_MAP()


// CUnknownGraphView diagnostics

#ifdef _DEBUG
void CUnknownGraphView::AssertValid() const
{
	CBaseGraphView::AssertValid();
}

#ifndef _WIN32_WCE
void CUnknownGraphView::Dump(CDumpContext& dc) const
{
	CBaseGraphView::Dump(dc);
}
#endif
#endif //_DEBUG


// CUnknownGraphView message handlers
void CUnknownGraphView::OnDraw(CDC* pDC)
{
    CBaseGraphView::OnDraw(pDC);
}


void CUnknownGraphView::OnInitialUpdate()
{
    CBaseGraphView::OnInitialUpdate();
}


void CUnknownGraphView::OnBnClickedGraphinfoBtnEditsave()
{
    CBaseGraphView::OnBnClickedGraphinfoBtnEditsave();
}
