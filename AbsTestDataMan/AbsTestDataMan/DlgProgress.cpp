// DlgProgress.cpp : implementation file
//

#include "stdafx.h"
#include "AbsTestDataMan.h"
#include "DlgProgress.h"
#include "GlobalFuncs.h"
#include "GlobalVars.h"
#include "LeftTreeView.h"

CWinThread *CDlgProgress::m_pThread = NULL;
CDlgProgress *CDlgProgress::m_pDlg = NULL;
AFX_THREADPROC CDlgProgress::m_ThreadProc = NULL;
LPVOID CDlgProgress::m_lpParam = NULL;

// CDlgProgress dialog

IMPLEMENT_DYNAMIC(CDlgProgress, CDialog)

CDlgProgress::CDlgProgress(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProgress::IDD, pParent)
{
	m_iMaxPos = 100;
	m_pProgress = NULL;
}

CDlgProgress::~CDlgProgress()
{
	//删除
	if(m_pProgress!=NULL){
		delete m_pProgress;
		m_pProgress = NULL;
	}
}

void CDlgProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgProgress, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CDlgProgress::OnBnClickedCancel)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_UPDATEDATA, &CDlgProgress::OnUpdateData)
END_MESSAGE_MAP()

// CDlgProgress message handlers

BOOL CDlgProgress::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(m_pProgress==NULL){
		CWnd *pPromptInfoWnd = GetDlgItem(IDC_PROMPT_INFO);
		m_pProgress = new CSkinProgress(GetDlgItem(IDC_PROGRESS), 
										m_iMaxPos,
										NULL,
										FALSE,
										pPromptInfoWnd,
										m_strPromptInfo,
										cSPT_PERCENT
										);
	}
	m_pDlg = this;

	return TRUE;
}

LRESULT CDlgProgress::OnUpdateData(WPARAM wParam, LPARAM lParam)
{
	if(m_pProgress!=NULL){
		m_pProgress->SetPos((int)lParam);
		if(m_pProgress->GetPos()>=m_pProgress->GetUpper()){
			m_pProgress->SetPos(1);
		}
	}
	return 0;
}

BOOL PeekAndPump()
{
	static MSG msg;

	while(::PeekMessage(&msg,(HWND)(-1),0,0,PM_NOREMOVE)){
		if(msg.message==WM_QUIT){
			return FALSE;
		}
		if(!AfxGetApp()->PumpMessage())
		{
			::PostQuitMessage(0);
			return FALSE;
		}
	}

	return TRUE;
}

void CDlgProgress::OnBnClickedCancel()
{
	if(m_pThread){
		m_pThread->PostThreadMessage(WM_QUIT, 0, 0);
	}
	//等待进程结束
	WaitForSingleObject(m_pThread, 20000);
	//进一步检查等待直到真正结束
	int timeout = 1000000;
	while((m_pThread!=NULL) && (timeout--));
	OnCancel();
}

void CDlgProgress::OnClose()
{
	WaitForSingleObject(m_pThread, 20000);
	int timeout = 1000000;
	while((m_pThread!=NULL) && (timeout--));
	CDialog::OnClose();
}

BOOL CDlgProgress::DestroyWindow()
{
	if(m_pProgress){
		delete m_pProgress;
		m_pProgress = NULL;
	}

	return CDialog::DestroyWindow();
}

static UINT ProgressThreadProc(LPVOID lpParam)
{
	//务必等到对话框都创建完毕，否则进程中无法向其更新进度条信息
	while(CDlgProgress::m_pDlg==NULL);
	while(CDlgProgress::m_pDlg->m_pProgress==NULL);

	if(CDlgProgress::m_ThreadProc!=NULL){
		CDlgProgress::UpdateAndCheck(0);
		CDlgProgress::m_ThreadProc(lpParam);
	}
	if(CDlgProgress::m_pDlg){
		::SendMessage(CDlgProgress::m_pDlg->m_hWnd, WM_CLOSE, 0, 0);
	}
	CDlgProgress::m_pThread = NULL;
	CDlgProgress::m_ThreadProc = NULL;
	AfxEndThread(0);

	return 0;
}

BOOL CDlgProgress::StartThread(AFX_THREADPROC ThreadProc, LPCTSTR sPromptInfo, int iMaxPos)
{
	if(m_pThread==NULL){
		m_ThreadProc = ThreadProc;
		m_pThread = AfxBeginThread(ProgressThreadProc, (LPVOID)m_lpParam);
		if(m_pThread==NULL){
			return FALSE;
		}
		if(m_pDlg==NULL){
			m_pDlg = new CDlgProgress();
			if(sPromptInfo==NULL){
				m_pDlg->m_strPromptInfo = CString(sPromptInfo);
			}
			else{
				m_pDlg->m_strPromptInfo = CString(_T("正在进行操作"));
			}
			m_pDlg->m_iMaxPos = iMaxPos;
			m_pDlg->DoModal();
			delete m_pDlg;
			m_pDlg = NULL;
		}
		return TRUE;
	}
	else{
		return FALSE;
	}
	return TRUE;
}

///更新进度条数据，并检查用户是否单击取消按钮
BOOL CDlgProgress::UpdateAndCheck(int iPercent)
{
	if(m_pDlg){
		if(iPercent>100){
			iPercent = 100;
		}
		if(iPercent<0){
			iPercent = 0;
		}
		if(m_pDlg->m_pProgress){
			int nCurrPos = (int)(m_pDlg->m_pProgress->GetUpper()*iPercent/100.0);
			::SendMessage(CDlgProgress::m_pDlg->m_hWnd, WM_UPDATEDATA, 0, nCurrPos);
		}
	}

	return PeekAndPump();
}