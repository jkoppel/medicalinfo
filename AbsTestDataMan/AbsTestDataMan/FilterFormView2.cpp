// CFilterFormView.cpp : implementation file
//

#include "stdafx.h"
#include "AbsTestDataMan.h"
#include "FilterFormView.h"


// CFilterFormView

IMPLEMENT_DYNCREATE(CFilterFormView, CFormView)

CFilterFormView::CFilterFormView()
	: CFormView(CFilterFormView::IDD)
{

}

CFilterFormView::~CFilterFormView()
{
}

void CFilterFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFilterFormView, CFormView)
END_MESSAGE_MAP()


// CFilterFormView diagnostics

#ifdef _DEBUG
void CFilterFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFilterFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFilterFormView message handlers
