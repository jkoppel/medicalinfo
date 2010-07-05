// DlgProgress.cpp : implementation file
//

#include "stdafx.h"
#include "EDP.h"
#include "DlgProgress.h"


// CDlgProgress dialog

IMPLEMENT_DYNAMIC(CDlgProgress, CDialog)

CDlgProgress::CDlgProgress(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProgress::IDD, pParent)
{
	m_pProgress = NULL;
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
END_MESSAGE_MAP()


// CDlgProgress message handlers

BOOL CDlgProgress::OnInitDialog()
{
	CDialog::OnInitDialog();

	

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgProgress::OnBnClickedOk()
{
	extern BOOL PeekAndPump();
	m_pProgress = new CSkinProgress(GetDlgItem(IDC_PROGRESS), 300000);
	for(int i=0; i<300000; i++)
	{
		m_pProgress->StepIt();
		PeekAndPump();
//		Sleep(1);
	}

	delete m_pProgress;
	m_pProgress = NULL;

	//OnOK();
}

void CDlgProgress::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CDlgProgress::OnClose()
{
	if(m_pProgress){
		delete []m_pProgress;
		m_pProgress = NULL;
	}

	CDialog::OnClose();
}

BOOL CDlgProgress::DestroyWindow()
{
	if(m_pProgress){
		delete []m_pProgress;
		m_pProgress = NULL;
	}

	return CDialog::DestroyWindow();
}
