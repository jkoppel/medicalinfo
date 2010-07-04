// ConnectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MIClient.h"
#include "ConnectDlg.h"
#include "..\\Common\\GlobalVars.h"
#include "..\\Common\\GlobalFuncs.h"
#include "..\\Common\\ProgressInfo.h"


// CConnectDlg dialog

IMPLEMENT_DYNAMIC(CConnectDlg, CDialog)

CConnectDlg::CConnectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConnectDlg::IDD, pParent)
	, m_strAddress(_T(""))
	, m_nPort(5000)
{

}

CConnectDlg::~CConnectDlg()
{
}

void CConnectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ADDRESS, m_strAddress);
	DDX_Text(pDX, IDC_PORT, m_nPort);
}


BEGIN_MESSAGE_MAP(CConnectDlg, CDialog)
	ON_BN_CLICKED(IDC_CONNECT, &CConnectDlg::OnBnClickedConnect)
END_MESSAGE_MAP()


void CConnectDlg::OnBnClickedConnect()
{
	UpdateData();
	if(!m_strAddress.GetLength()){
		ShowMsg("�������������ַ");
		return;
	}
	if(m_nPort<=0 || m_nPort>=10000){
		ShowMsg("�������˿ڷ�Χ��1~9999֮��");
		return;
	}
	CDialog::OnOK();
}
