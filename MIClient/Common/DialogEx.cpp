// DialogEx.cpp : implementation file
//

#include "stdafx.h"
#include "DialogEx.h"


// CDialogEx dialog

IMPLEMENT_DYNAMIC(CDialogEx, CDialog)

/*
CDialogEx::CDialogEx(CWnd* pParent )
	: CDialog(CDialogEx::IDD, pParent)
{
}
*/

CDialogEx::CDialogEx(CWnd* pParent/*=NULL*/, UINT nIDTemplate/*=100*/)
	: CDialog(nIDTemplate, pParent)
{
}

CDialogEx::~CDialogEx()
{
}

void CDialogEx::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogEx, CDialog)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CDialogEx message handlers

BOOL CDialogEx::OnEraseBkgnd(CDC* pDC)
{
	CBrush brush;
	CRect rect;
	COLORREF rgbBackGnd = RGB(255,100,100);

	GetClientRect(&rect);
	brush.CreateSolidBrush(rgbBackGnd);
	pDC-> FillRect(rect,&brush);
	return   TRUE; 

	//return CDialog::OnEraseBkgnd(pDC);
}
