// FormViewEx.cpp : implementation file
//

#include "stdafx.h"
#include "AbsTestDataMan.h"
#include "FilterFormView.h"
#include "DlgProgress.h"
#include "GlobalFuncs.h"
#include "GlobalVars.h"
#include "ProgressThread.h"


// CFilterFormView

IMPLEMENT_DYNCREATE(CFilterFormView, CFormView)

CFilterFormView::CFilterFormView()
	: CFormView(CFilterFormView::IDD)
	, m_strStartTime(_T(""))
	, m_strEndTime(_T(""))
	, m_strProductNo(_T(""))
	, m_strOperatorNo(_T(""))
{
	g_pFilterFormView = this;
}

CFilterFormView::~CFilterFormView()
{
}

void CFilterFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STARTTIME, m_strStartTime);
	DDX_Text(pDX, IDC_ENDTIME, m_strEndTime);
	DDX_Text(pDX, IDC_PRODUCTNO, m_strProductNo);
	DDX_Text(pDX, IDC_OPERATORNO, m_strOperatorNo);
	DDX_Control(pDX, IDC_FILTER_GROUP, m_ctrlGroupFilter);
}

BEGIN_MESSAGE_MAP(CFilterFormView, CFormView)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_APPLY, &CFilterFormView::OnBnClickedBtnApply)
	ON_WM_SIZE()
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

void CFilterFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
}

void CFilterFormView::OnPaint()
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

void CFilterFormView::OnBnClickedBtnApply()
{
/*
	UpdateData(TRUE);

	int year1, month1, day1, hour1, minute1, year2, month2, day2, hour2, minute2;
	char buf1[256], buf2[256];
	m_strStartTime.Trim();
	sprintf_s(buf1, "%s", m_strStartTime.GetBuffer(0));
	if(strlen(buf1)>=8){
		memset(buf2, 0, sizeof(buf2));
		memcpy(buf2, buf1, 8);
		if(!CheckStrDateFormat(buf2, year1, month1, day1)){
			ShowMsg("ÊäÈë´íÎó");
			return;
		}
		if(strlen(buf1)>8){
			if(buf1[8]!=' '){
				ShowMsg("ÊäÈë´íÎó");
				return;
			}
			memset(buf2, 0, sizeof(buf2));
			memcpy(buf2, buf1+9, strlen(buf1)-9);
			if(!CheckStrTimeFormat(buf2, hour1, minute1)){
				ShowMsg("ÊäÈë´íÎó3");
				return;
			}
		}
	}
	else if(strlen(buf1)>0){
		ShowMsg("ÊäÈë´íÎó");
		return;
	}

*/
	/*
	if(pDlgProgress==NULL){
		pDlgProgress = new CDlgProgress();
		pDlgProgress->Create(IDD_PROGRESS);
	}
	pDlgProgress->m_pProgress->SetRange(1, 30000);
	pDlgProgress->m_pProgress->ShowWindow(SW_SHOW);

	if(pThread==NULL){
		pThread = AfxBeginThread(&MyThread, (LPVOID)0);
	}
	*/
	CDlgProgress dlg;
	dlg.DoModal();
}

void CFilterFormView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	//Òþ²Ø¹ö¶¯Ìõ
	SetScrollRange(0, 0, 0);
	SetScrollRange(1, 0, 0);
}

