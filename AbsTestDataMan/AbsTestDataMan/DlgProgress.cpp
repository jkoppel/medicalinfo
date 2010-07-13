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
int flag_cancel = 0;


// CDlgProgress dialog

IMPLEMENT_DYNAMIC(CDlgProgress, CDialog)

CDlgProgress::CDlgProgress(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProgress::IDD, pParent)
{
	m_pProgress = NULL;
	m_iThreadType = THREAD_LOAD_NODE;
}

CDlgProgress::~CDlgProgress()
{
}

void CDlgProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgProgress, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgProgress::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgProgress::OnBnClickedCancel)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_UPDATEDATA, &CDlgProgress::OnUpdateData)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

void CDlgProgress::SetThreadType(int iThreadType)
{
	m_iThreadType = iThreadType;
}

// CDlgProgress message handlers

BOOL CDlgProgress::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pProgress = new CSkinProgress(GetDlgItem(IDC_PROGRESS), 300000);
	m_pDlg = this;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CDlgProgress::OnUpdateData(WPARAM wParam, LPARAM lParam)
{
	if(m_pProgress){
		//m_pProgress->StepIt();
		m_pProgress->SetPos(m_pProgress->GetPos()+1000);
		if(m_pProgress->GetPos()>=m_pDlg->m_pProgress->GetUpper()){
			m_pDlg->m_pProgress->SetPos(0);
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

UINT CDlgProgress::ThreadProc(LPVOID lpParm)
{
	/*
	while(1)
	{
		if(m_pDlg){
			::SendMessage(m_pDlg->m_hWnd, WM_UPDATEDATA, 0, 0);
		}

		if(!PeekAndPump()){
			break;
		}
	}
	*/

	//Do Things ...
		/*
		//use the following function to update step
		if(CDlgProgress::m_pDlg){
			::SendMessage(CDlgProgress::m_pDlg->m_hWnd, WM_UPDATEDATA, 0, 0);
		}

		//use the following to break the loop
		if(!PeekAndPump()){
			break;
		}
		*/

	//After thread, we need to release resources
	m_pThread->ExitInstance();
	m_pThread = NULL;

	if(m_pDlg){
		::SendMessage(m_pDlg->m_hWnd, WM_CLOSE, 0, 0);
	}

	return 0;
}

void CDlgProgress::OnBnClickedOk()
{
	if(m_pThread==NULL){
		m_pThread = AfxBeginThread(&CDlgProgress::ThreadProc, (LPVOID)0);
	}
}

void CDlgProgress::OnBnClickedCancel()
{
	if(m_pThread){
		m_pThread->PostThreadMessage(WM_QUIT, 0, 0);
	}
	WaitForSingleObject(m_pThread, 20000);
	int timeout = 1000000;
	while(m_pThread!=NULL && timeout--);
	if(m_pProgress){
		delete m_pProgress;
		m_pProgress = NULL;
	}
	m_pDlg = NULL;
	OnCancel();
}

void CDlgProgress::OnClose()
{
	WaitForSingleObject(m_pThread, 20000);
	int timeout = 1000000;
	while(m_pThread!=NULL && timeout--);
	if(m_pProgress){
		delete m_pProgress;
		m_pProgress = NULL;
	}
	m_pDlg = NULL;
	CDialog::OnClose();
}

BOOL CDlgProgress::DestroyWindow()
{
	m_pDlg = NULL;

	return CDialog::DestroyWindow();
}


void CDlgProgress::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if(m_pThread==NULL){
		if(m_iThreadType==THREAD_LOAD_NODE){
			m_pThread = AfxBeginThread(&CDlgProgress::ThreadProc, (LPVOID)0);
		}
	}
}
