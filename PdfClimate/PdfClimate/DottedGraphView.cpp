// DottedGraphView.cpp : implementation file
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "DottedGraphView.h"
#include "GlobalVars.h"
#include "LeftView.h"
#include "ImageProcess/DottedGraph.h"

// CDottedGraphView

IMPLEMENT_DYNCREATE(CDottedGraphView, CGraphFormView)

CDottedGraphView::CDottedGraphView()
	: CGraphFormView(CDottedGraphView::IDD, Graph_Dotted)
{
}

CDottedGraphView::~CDottedGraphView()
{
}

void CDottedGraphView::DoDataExchange(CDataExchange* pDX)
{
    CGraphFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDottedGraphView, CGraphFormView)
    ON_WM_CREATE()
    ON_BN_CLICKED(IDC_GRAPHINFO_BTN_EDITSAVE, &CDottedGraphView::OnBnClickedGraphinfoBtnEditsave)
END_MESSAGE_MAP()


// CDottedGraphView diagnostics

#ifdef _DEBUG
void CDottedGraphView::AssertValid() const
{
	CGraphFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDottedGraphView::Dump(CDumpContext& dc) const
{
	CGraphFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDottedGraphView message handlers


void CDottedGraphView::OnDraw(CDC* pDC)
{
    CGraphFormView::OnDraw(pDC);
}

void CDottedGraphView::OnInitialUpdate()
{
    CGraphFormView::OnInitialUpdate();
}


void CDottedGraphView::OnBnClickedGraphinfoBtnEditsave()
{
    CGraphFormView::OnBnClickedGraphinfoBtnEditsave();
}
