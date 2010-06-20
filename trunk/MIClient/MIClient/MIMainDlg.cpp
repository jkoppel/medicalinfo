// MIMainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MIClient.h"
#include "MIMainDlg.h"
#include "ConnectDlg.h"
#include "GlobalVars.h"
#include "GlobalFuncs.h"
#include "ProgressInfo.h"


// CMIMainDlg dialog

IMPLEMENT_DYNAMIC(CMIMainDlg, CDialog)

CMIMainDlg::CMIMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMIMainDlg::IDD, pParent)
{
	m_strAddress = CString("192.168.0.11");
	m_nPort = 5000;
}

CMIMainDlg::~CMIMainDlg()
{
}

void CMIMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMIMainDlg, CDialog)
	ON_BN_CLICKED(IDC_CONNECT, &CMIMainDlg::OnBnClickedConnect)
	ON_MESSAGE(SOCK_ONRECEIVE, OnReceive)
	ON_MESSAGE(SOCK_ONCLOSE, OnDisconnect)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_ADD, &CMIMainDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_EDIT, &CMIMainDlg::OnBnClickedEdit)
	ON_BN_CLICKED(IDC_DELETE, &CMIMainDlg::OnBnClickedDelete)
	ON_BN_CLICKED(IDC_SEARCH, &CMIMainDlg::OnBnClickedSearch)
END_MESSAGE_MAP()

LRESULT CMIMainDlg::OnReceive(WPARAM wParam, LPARAM lParam)
{
	CString str, temp;

	while(g_pClientSocket->GetDataSize() > 0 && g_pClientSocket->ReadString(temp))
		str += L" "+temp;

	AfxMessageBox(str);

	return 0;
}

LRESULT CMIMainDlg::OnDisconnect(WPARAM wParam, LPARAM lParam)
{
	SetConnectStatus(FALSE);
	ShowMsg("连接失败");
	OnBnClickedConnect();
	return 0;
}


void CMIMainDlg::SetConnectStatus(BOOL status)
{
	if(status){
		GetDlgItem(IDC_CONNECT)->SetWindowText(L"断开");
		GetDlgItem(IDC_ADD)->EnableWindow();
		GetDlgItem(IDC_DELETE)->EnableWindow();
		GetDlgItem(IDC_EDIT)->EnableWindow();
		GetDlgItem(IDC_SEARCH)->EnableWindow();
		g_bIsConnected = TRUE;
	}
	else{
		GetDlgItem(IDC_CONNECT)->SetWindowText(L"连接");
		GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_SEARCH)->EnableWindow(FALSE);
		g_pClientSocket->Disconnect();
		g_bIsConnected = FALSE;
	}
}

// CMIMainDlg message handlers

void CMIMainDlg::OnBnClickedConnect()
{
	if(g_bIsConnected){//已经连接，设为断开
		SetConnectStatus(FALSE);
		return;
	}
		
	CConnectDlg dlg;
	dlg.m_strAddress = m_strAddress;
	dlg.m_nPort = m_nPort;
	if(IDOK==dlg.DoModal()){
		if(g_pClientSocket->Create(SOCK_STREAM)){
			if(g_pClientSocket->Connect(dlg.m_strAddress, dlg.m_nPort)){
				m_strAddress = dlg.m_strAddress;
				m_nPort = dlg.m_nPort;
				g_ProgressInfo.Show("正在连接服务器...");
				SetTimer(100, 25000, NULL);
				g_bIsConnected = TRUE;
				GetDlgItem(IDC_CONNECT)->EnableWindow(FALSE);
				return;
			}
		}
	}
	SetConnectStatus(FALSE);
}

BOOL CMIMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_pClientSocket = new CClientSocket(this);
	SetWindowPos(NULL, 0, 0, 240, 290, SWP_SHOWWINDOW);

	return TRUE;  
}

void CMIMainDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent==100){
		KillTimer(nIDEvent);
		g_ProgressInfo.Hide();
		GetDlgItem(IDC_CONNECT)->EnableWindow(TRUE);
		if(g_bIsConnected){
			ShowMsg("连接成功");
			SetConnectStatus(TRUE);
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CMIMainDlg::OnBnClickedAdd()
{
	g_pClientSocket->SendLine(CString("Add"));
}

void CMIMainDlg::OnBnClickedEdit()
{
	g_pClientSocket->SendLine(CString("Edit"));
}

void CMIMainDlg::OnBnClickedDelete() 
{
	g_pClientSocket->SendLine(CString("Delete"));
}

void CMIMainDlg::OnBnClickedSearch()
{
	g_pClientSocket->SendLine(CString("Search"));
}
