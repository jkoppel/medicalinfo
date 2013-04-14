// DottedGraphView.cpp : implementation file
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "DottedGraphView.h"


// CDottedGraphView

IMPLEMENT_DYNCREATE(CDottedGraphView, CFormView)

CDottedGraphView::CDottedGraphView()
	: CFormView(CDottedGraphView::IDD)
{

}

CDottedGraphView::~CDottedGraphView()
{
}

void CDottedGraphView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDottedGraphView, CFormView)
END_MESSAGE_MAP()


// CDottedGraphView diagnostics

#ifdef _DEBUG
void CDottedGraphView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDottedGraphView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDottedGraphView message handlers
