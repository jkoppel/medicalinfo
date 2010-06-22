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
	ON_MESSAGE(SOCK_ONCLOSE, OnDisconnect)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_ADD, &CMIMainDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_EDIT, &CMIMainDlg::OnBnClickedEdit)
	ON_BN_CLICKED(IDC_DELETE, &CMIMainDlg::OnBnClickedDelete)
	ON_BN_CLICKED(IDC_SEARCH, &CMIMainDlg::OnBnClickedSearch)
END_MESSAGE_MAP()

///接收到断开连接消息
LRESULT CMIMainDlg::OnDisconnect(WPARAM wParam, LPARAM lParam)
{
	SetConnectStatus(FALSE);
	ShowMsg("连接失败");
	OnBnClickedConnect();
	return 0;
}

///设置连接状态
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

///点连接/断开按钮
void CMIMainDlg::OnBnClickedConnect()
{
	if(g_bIsConnected){//已经连接，设为断开
		SetConnectStatus(FALSE);
		return;
	}
	
	///连接服务器
	CConnectDlg dlg;
	dlg.m_strAddress = m_strAddress;
	dlg.m_nPort = m_nPort;
	if(IDOK==dlg.DoModal()){
		if(g_pClientSocket->Create(SOCK_STREAM)){
 			if(g_pClientSocket->Connect(dlg.m_strAddress, dlg.m_nPort)){
				//由于连接一定会成功，所以设置定时器，25秒之后，没有DISCONNECT，则认为连接成功
				m_strAddress = dlg.m_strAddress;
				m_nPort = dlg.m_nPort;
				g_ProgressInfo.Show("正在连接服务器...");
				SetTimer(100, 2000, NULL);//TODO 2000->25000
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
	if(nIDEvent==100){//连接
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

///获取数据
int ReceiveData()
{
	UINT timeout = 100;
	while(g_bIsDataComing==FALSE && timeout--){//每100毫秒查一次，共等待30秒
		Sleep(100);
	}
	return g_bIsDataComing;
}

struct UserData{
	int id;
	char Name[20];
};

///连接命令
int CMIMainDlg::CmdConnect()
{
	g_bIsDataComing = FALSE;
	g_pClientSocket->Send("CMD1 \r\n", strlen("CMD1 \r\n"));
	if(!ReceiveData()){
		return FALSE;
	}
	if(strncmp(g_RecvData,"OK\r\n", 4)==0){
		return TRUE;
	}
	return FALSE;
}

int CMIMainDlg::CmdGetRecordNum()
{
	g_bIsDataComing = FALSE;
	g_pClientSocket->Send("CMD2 \r\n", strlen("CMD2 \r\n"));
	if(!ReceiveData()){
		ShowMsg("获取记录个数失败");
	}
	int num;
	char buf[1000],buf1[100], buf2[100];
	sscanf(g_RecvData, "%s %04d %s", buf1, &num, buf2);

	sprintf(buf, "rec num:%d", num);
	AfxMessageBox(CString(buf));
	return TRUE;
}

int CMIMainDlg::CmdGetRecordAt(int index)
{
	g_bIsDataComing = FALSE;
	char buf[1000];
	sprintf(buf, "%s %04d %s", "CMD3", index, "\r\n");
	g_pClientSocket->Send(buf, strlen(buf));
	if(!ReceiveData()){
		ShowMsg("获取记录失败");
	}
	struct UserData data;
	char buf1[100], buf2[100];
	sscanf(g_RecvData, "%s %04d %s %s", buf1, &data.id, data.Name, buf2);

	sprintf(buf, "%s %d %s", "rec data:", data.id, data.Name);
	ShowMsg(buf);

	return TRUE;
}

int CMIMainDlg::CmdAppendRecord(struct UserData data)
{
	g_bIsDataComing = FALSE;
	char buf[100];
	sprintf(buf, "%s %04d %s %s", "CMD4", data.id, data.Name, "\r\n");
	g_pClientSocket->Send(buf, strlen(buf));
	if(!ReceiveData()){
		ShowMsg("添加记录失败");
	}
	if(strncmp(g_RecvData,"OK\r\n", 4)==0){
		ShowMsg("添加记录成功");
		return TRUE;
	}
	ShowMsg("添加记录失败");
	return TRUE;
}

int CMIMainDlg::CmdDeleteRecordAt(int index)
{
	g_bIsDataComing = FALSE;
	char buf[100];
	sprintf(buf, "%s %04d %s", "CMD5", index, "\r\n");
	g_pClientSocket->Send(buf, strlen(buf));
	if(!ReceiveData()){
		ShowMsg("删除记录失败");
	}
	if(strncmp(g_RecvData,"OK\r\n", 4)==0){
		ShowMsg("删除记录成功");
		return TRUE;
	}
	ShowMsg("删除记录失败");
	return TRUE;
}

void CMIMainDlg::OnBnClickedAdd()
{
	CString str("OK");
	char buf[2000];
	CString2Char(str, buf);

	CmdConnect();
	CmdGetRecordNum();
	CmdGetRecordAt(0);
}

void CMIMainDlg::OnBnClickedEdit()
{
	struct UserData data;

	data.id = 0;
	sprintf(data.Name, "%s", "林仙立");
	CmdAppendRecord(data);
}

void CMIMainDlg::OnBnClickedDelete() 
{
	CmdDeleteRecordAt(0);
}

void CMIMainDlg::OnBnClickedSearch()
{
	CmdGetRecordAt(10);
}
