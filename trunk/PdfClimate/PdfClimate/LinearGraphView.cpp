// LinearGraphView.cpp : implementation file
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "LinearGraphView.h"


// CLinearGraphView

IMPLEMENT_DYNCREATE(CLinearGraphView, CBaseGraphView)

CLinearGraphView::CLinearGraphView()
	: CBaseGraphView(CLinearGraphView::IDD, Graph_Linear)
{
}

CLinearGraphView::~CLinearGraphView()
{
}

void CLinearGraphView::DoDataExchange(CDataExchange* pDX)
{
	CBaseGraphView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLinearGraphView, CBaseGraphView)
    ON_BN_CLICKED(IDC_GRAPHINFO_BTN_EDITSAVE, &CLinearGraphView::OnBnClickedGraphinfoBtnEditsave)
END_MESSAGE_MAP()


// CLinearGraphView diagnostics

#ifdef _DEBUG
void CLinearGraphView::AssertValid() const
{
	CBaseGraphView::AssertValid();
}

#ifndef _WIN32_WCE
void CLinearGraphView::Dump(CDumpContext& dc) const
{
	CBaseGraphView::Dump(dc);
}
#endif
#endif //_DEBUG


// CLinearGraphView message handlers
void CLinearGraphView::OnDraw(CDC* pDC)
{
    CBaseGraphView::OnDraw(pDC);
}



void CLinearGraphView::OnInitialUpdate()
{
    CBaseGraphView::OnInitialUpdate();
}


void CLinearGraphView::OnBnClickedGraphinfoBtnEditsave()
{
    CBaseGraphView::OnBnClickedGraphinfoBtnEditsave();
}
