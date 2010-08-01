// FormViewEx.cpp : implementation file
//

#include "stdafx.h"
#include "AbsTestDataMan.h"
#include "FilterFormView.h"
#include "DlgProgress.h"
#include "GlobalFuncs.h"
#include "GlobalVars.h"
#include "FilterConfigMgt.h"


// CFilterFormView

IMPLEMENT_DYNCREATE(CFilterFormView, CFormView)

CFilterFormView::CFilterFormView()
	: CFormView(CFilterFormView::IDD)
	, m_strProductNo(_T(""))
	, m_strOperatorNo(_T(""))
	, m_bCheckTime(TRUE)
	, m_tStartTime(0)
	, m_tEndTime(0)
{
	g_pFilterFormView = this;
}

CFilterFormView::~CFilterFormView()
{
}

void CFilterFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PRODUCTNO, m_strProductNo);
	DDX_Text(pDX, IDC_OPERATORNO, m_strOperatorNo);
	DDX_Control(pDX, IDC_FILTER_GROUP, m_ctrlGroupFilter);
	DDX_Control(pDX, IDC_DTP_START_DATE, m_ctrlStartTime);
	DDX_Control(pDX, IDC_DTP_END_DATE, m_ctrlEndTime);
	DDX_Control(pDX, IDC_CHECK_TIME, m_ctrlCheckTime);
	DDX_Check(pDX, IDC_CHECK_TIME, m_bCheckTime);
	DDX_Control(pDX, IDC_PRODUCTNO, m_ctrlProductNo);
	DDX_Control(pDX, IDC_OPERATORNO, m_ctrlOperatorNo);
	DDX_DateTimeCtrl(pDX, IDC_DTP_START_DATE, m_tStartTime);
	DDX_DateTimeCtrl(pDX, IDC_DTP_END_DATE, m_tEndTime);
}

BEGIN_MESSAGE_MAP(CFilterFormView, CFormView)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_APPLY, &CFilterFormView::OnBnClickedBtnApply)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_TEST1, &CFilterFormView::OnBnClickedBtnTest1)
	ON_BN_CLICKED(IDC_CHECK_TIME, &CFilterFormView::OnBnClickedCheckTime)
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

	m_ctrlStartTime.SetFormat(_T("yyyy-MM-dd HH:mm"));
	m_ctrlEndTime.SetFormat(_T("yyyy-MM-dd HH:mm"));
	m_ctrlOperatorNo.SetLimitText(32);
	m_ctrlProductNo.SetLimitText(32);

	struct FilterConfig config;
	CFilterConfigMgt cMgt;

	cMgt.GetFilterConfig(config);
	m_bCheckTime = config.bCheckTime;
	m_tStartTime = CTime(config.StartTime);
	m_tEndTime = CTime(config.EndTime);
	m_strProductNo = CString(config.sProductNo);
	m_strOperatorNo = CString(config.sOperatorNo);
	m_ctrlStartTime.EnableWindow(config.bCheckTime);
	m_ctrlEndTime.EnableWindow(config.bCheckTime);

	UpdateData(FALSE);
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
	UpdateData(TRUE);

	struct FilterConfig config;
	CFilterConfigMgt cMgt;

	config.bCheckTime = m_bCheckTime;
	m_ctrlStartTime.GetTime(&config.StartTime);
	config.StartTime.wSecond = 0;
	m_ctrlEndTime.GetTime(&config.EndTime);
	config.EndTime.wSecond = 0;
	m_strProductNo = m_strProductNo.Trim();
	m_strOperatorNo = m_strOperatorNo.Trim();
	CString2char(config.sProductNo, m_strProductNo);
	CString2char(config.sOperatorNo, m_strOperatorNo);

	cMgt.SetFilterConfig(config);

	g_pLeftTreeView->InitTree(TRUE);
	g_pRightDrawAreaView->RedrawWindow();
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
		if(!CDlgProgress::UpdateAndCheck((int)((MAX_COUNT-iMaxCount)/(MAX_COUNT*1.0)*100))){
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

void CFilterFormView::OnBnClickedCheckTime()
{
	UpdateData(TRUE);
	m_ctrlStartTime.EnableWindow(m_bCheckTime);
	m_ctrlEndTime.EnableWindow(m_bCheckTime);
}
