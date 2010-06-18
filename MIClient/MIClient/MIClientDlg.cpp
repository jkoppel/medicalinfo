// MIClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MIClient.h"
#include "MIClientDlg.h"

#include "CESocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMIClientDlg �Ի���

CMIClientDlg::CMIClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMIClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMIClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMIClientDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CMIClientDlg ��Ϣ�������

BOOL CMIClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	/*
	int ret;
	ret = m_clientSocket.Create();
	if(!ret){
		AfxMessageBox(_T("Create Error"));
		return FALSE;
	}
	ret = m_clientSocket.Connect(CString("192.168.0.11"), 8081);
	if(!ret){
		AfxMessageBox(_T("Connect Error"));
		return FALSE;
	}

	CString a("192.168.0.11");
	*/

	CCESocket mySocket;
	int ret;
	ret = mySocket.Create(SOCK_STREAM);
	if(!ret){
		AfxMessageBox(_T("Create Error"));
		return FALSE;
	}
	ret = mySocket.Connect(CString("192.168.0.11"), 1234);
	if(!ret){
		AfxMessageBox(_T("Connect Error"));
		return FALSE;
	}

	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CMIClientDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_MICLIENT_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_MICLIENT_DIALOG));
	}
}
#endif

