// DottedGraphView.cpp : implementation file
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "DottedGraphView.h"
#include "GlobalVars.h"
#include "LeftView.h"
#include "ImageProcess/DottedGraph.h"

// CDottedGraphView

IMPLEMENT_DYNCREATE(CDottedGraphView, CBaseGraphView)

CDottedGraphView::CDottedGraphView()
	: CBaseGraphView(CDottedGraphView::IDD, Graph_Dotted)
{
}

CDottedGraphView::~CDottedGraphView()
{
}

void CDottedGraphView::DoDataExchange(CDataExchange* pDX)
{
    CBaseGraphView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDottedGraphView, CBaseGraphView)
    ON_WM_CREATE()
    ON_BN_CLICKED(IDC_GRAPHINFO_BTN_EDITSAVE, &CDottedGraphView::OnBnClickedGraphinfoBtnEditsave)
END_MESSAGE_MAP()


// CDottedGraphView diagnostics

#ifdef _DEBUG
void CDottedGraphView::AssertValid() const
{
	CBaseGraphView::AssertValid();
}

#ifndef _WIN32_WCE
void CDottedGraphView::Dump(CDumpContext& dc) const
{
	CBaseGraphView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDottedGraphView message handlers


void CDottedGraphView::OnDraw(CDC* pDC)
{
    CBaseGraphView::OnDraw(pDC);
}

void CDottedGraphView::OnInitialUpdate()
{
    CBaseGraphView::OnInitialUpdate();
}


void CDottedGraphView::OnBnClickedGraphinfoBtnEditsave()
{
    CBaseGraphView::OnBnClickedGraphinfoBtnEditsave();
}
