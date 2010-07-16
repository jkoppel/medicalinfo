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
	ON_BN_CLICKED(IDC_BTN_TEST1, &CFilterFormView::OnBnClickedBtnTest1)
	ON_BN_CLICKED(IDC_BTN_TEST2, &CFilterFormView::OnBnClickedBtnTest2)
	ON_BN_CLICKED(IDC_BTN_TEST3, &CFilterFormView::OnBnClickedBtnTest3)
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
			ShowMsg("输入错误");
			return;
		}
		if(strlen(buf1)>8){
			if(buf1[8]!=' '){
				ShowMsg("输入错误");
				return;
			}
			memset(buf2, 0, sizeof(buf2));
			memcpy(buf2, buf1+9, strlen(buf1)-9);
			if(!CheckStrTimeFormat(buf2, hour1, minute1)){
				ShowMsg("输入错误3");
				return;
			}
		}
	}
	else if(strlen(buf1)>0){
		ShowMsg("输入错误");
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
}

void CFilterFormView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	//隐藏滚动条
	SetScrollRange(0, 0, 0);
	SetScrollRange(1, 0, 0);
}

UINT MyThreadProc(LPVOID lpParam)
{
	unsigned long MAX_COUNT = 0x5FF;
	unsigned long iMaxCount = MAX_COUNT;
	while(iMaxCount>0){
		if(!CDlgProgress::UpdateAndCheck((MAX_COUNT-iMaxCount)/(MAX_COUNT*1.0)*100)){
			break;
		}
		iMaxCount --;
	}
	return 0;
}

void CFilterFormView::OnBnClickedBtnTest1()
{
	CDlgProgress::StartThread(MyThreadProc, _T("正在测试..."));
}

void CFilterFormView::OnBnClickedBtnTest2()
{
	g_pLeftTreeView->SetMultiSelectMode(!g_pLeftTreeView->GetMultiSelectMode());
	g_pLeftTreeView->InitTree();
	g_pRightDrawAreaView->RedrawWindow();
	if(!g_pLeftTreeView->GetMultiSelectMode()){
		GetDlgItem(IDC_BTN_TEST2)->SetWindowText(_T("多文件"));
	}
	else{
		GetDlgItem(IDC_BTN_TEST2)->SetWindowText(_T("单文件"));
	}
}

void CFilterFormView::OnBnClickedBtnTest3()
{
	CRightDrawAreaView::DRAW_MODE iMode = (CRightDrawAreaView::DRAW_MODE)((g_pRightDrawAreaView->GetDrawMode()+1) % 4);
	g_pRightDrawAreaView->SetDrawMode(iMode);
	g_pRightDrawAreaView->RedrawWindow();
}
