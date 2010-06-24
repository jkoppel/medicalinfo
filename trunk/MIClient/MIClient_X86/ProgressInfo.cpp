// PrgressInfo.cpp : implementation file
//

#include "stdafx.h"
#include "MIClient.h"
#include "ProgressInfo.h"
//#include "..\\MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressInfo dialog


CProgressInfo::CProgressInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressInfo)
	//}}AFX_DATA_INIT
	m_WndCreateFlag = 0;
}

void CProgressInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressInfo)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressInfo, CDialog)
	//{{AFX_MSG_MAP(CProgressInfo)
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressInfo message handlers

void CProgressInfo::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
}

///显示提示菜单
/**
  * Prompt : 待提示的信息 \n
  */
void CProgressInfo::Show(const char *Promt)
{
	RECT rect;
	int width, height;

	if(m_WndCreateFlag==1){
		Destroy();
	}

	Create(IDD_PROGRESS_INFO, NULL);
	m_WndCreateFlag = 1;

	SetWindowText(CString(Promt));

	GetDesktopWindow()->GetWindowRect(&rect);

	width = (int)strlen(Promt) * 6;
	if(width<180){
		width = 180;
	}
	height = 25;

	SetWindowPos(NULL, (rect.right+rect.left-width)/2, (rect.bottom+rect.top)/2 - 50, width, height, SWP_SHOWWINDOW);
	ShowWindow(SW_SHOW);
}

///隐藏
void CProgressInfo::Hide()
{
	if(m_WndCreateFlag==1){
		Destroy();
		CWnd *pWnd = AfxGetApp()->GetMainWnd();
		if(pWnd!=NULL){
			pWnd->ShowWindow(SW_SHOW);
		}
		m_WndCreateFlag = 0;
	}
}

///销毁
void CProgressInfo::Destroy()
{
	if(m_WndCreateFlag==1){
		DestroyWindow();
		m_WndCreateFlag = 0;
	}
}

void CProgressInfo::OnDestroy() 
{
	CDialog::OnDestroy();
	
	m_WndCreateFlag = 0;
}

CProgressInfo g_ProgressInfo;
void HideProgressInfo()
{
	g_ProgressInfo.Hide();
}
