// FormViewEx.cpp : implementation file
//

#include "stdafx.h"
#include "EDP.h"
#include "FormViewEx.h"


// CFormViewEx

IMPLEMENT_DYNCREATE(CFormViewEx, CFormView)

CFormViewEx::CFormViewEx()
	: CFormView(CFormViewEx::IDD)
{

}

CFormViewEx::~CFormViewEx()
{
}

void CFormViewEx::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFormViewEx, CFormView)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CFormViewEx diagnostics

#ifdef _DEBUG
void CFormViewEx::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormViewEx::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormViewEx message handlers

void CFormViewEx::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();


}

void CFormViewEx::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rect;

	GetClientRect(rect);
	COLORREF crLight = GetSysColor(COLOR_BTNHIGHLIGHT);
	COLORREF crShadow = GetSysColor(COLOR_BTNSHADOW);
	COLORREF crBtnFace = GetSysColor(COLOR_BTNFACE);
	dc.FillSolidRect(rect,crBtnFace);
	dc.Draw3dRect(rect,crLight,crShadow);
}
