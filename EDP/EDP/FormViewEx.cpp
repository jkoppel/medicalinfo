// FormViewEx.cpp : implementation file
//

#include "stdafx.h"
#include "EDP.h"
#include "FormViewEx.h"
#include "DlgProgress.h"
#include "GlobalFuncs.h"


// CFormViewEx

IMPLEMENT_DYNCREATE(CFormViewEx, CFormView)

CFormViewEx::CFormViewEx()
	: CFormView(CFormViewEx::IDD)
	, m_strStartTime(_T(""))
	, m_strEndTime(_T(""))
	, m_strProductNo(_T(""))
	, m_strOperatorNo(_T(""))
{

}

CFormViewEx::~CFormViewEx()
{
}

void CFormViewEx::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STARTTIME, m_strStartTime);
	DDX_Control(pDX, IDC_FILTER_GROUP, m_ctrlGroupFilter);
	DDX_Text(pDX, IDC_ENDTIME, m_strEndTime);
	DDX_Text(pDX, IDC_PRODUCTNO, m_strProductNo);
	DDX_Text(pDX, IDC_OPERATORNO, m_strOperatorNo);
}

BEGIN_MESSAGE_MAP(CFormViewEx, CFormView)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_APPLY, &CFormViewEx::OnBnClickedBtnApply)
	ON_WM_SIZE()
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

void CFormViewEx::OnBnClickedBtnApply()
{
	UpdateData(TRUE);

	int year1, month1, day1, hour1, minute1, year2, month2, day2, hour2, minute2;
	char buf1[256], buf2[256];
	m_strStartTime.Trim();
	sprintf_s(buf1, "%s", m_strStartTime.GetBuffer(0));
	if(strlen(buf1)>=8){
		memset(buf2, 0, sizeof(buf2));
		memcpy(buf2, buf1, 8);
		if(!CheckStrDateFormat(buf2, year1, month1, day1)){
			ShowMsg(" ‰»Î¥ÌŒÛ");
			return;
		}
		if(strlen(buf1)>8){
			if(buf1[8]!=' '){
				ShowMsg(" ‰»Î¥ÌŒÛ");
				return;
			}
			memset(buf2, 0, sizeof(buf2));
			memcpy(buf2, buf1+9, strlen(buf1)-9);
			if(!CheckStrTimeFormat(buf2, hour1, minute1)){
				ShowMsg(" ‰»Î¥ÌŒÛ3");
				return;
			}
		}
	}
	else if(strlen(buf1)>0){
		ShowMsg(" ‰»Î¥ÌŒÛ");
		return;
	}

	CDlgProgress dlg;
	dlg.DoModal();
}

void CFormViewEx::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	SetScrollRange(0, 0, 0);
	SetScrollRange(1, 0, 0);
}
