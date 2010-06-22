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

///���յ��Ͽ�������Ϣ
LRESULT CMIMainDlg::OnDisconnect(WPARAM wParam, LPARAM lParam)
{
	SetConnectStatus(FALSE);
	ShowMsg("����ʧ��");
	OnBnClickedConnect();
	return 0;
}

///��������״̬
void CMIMainDlg::SetConnectStatus(BOOL status)
{
	if(status){
		GetDlgItem(IDC_CONNECT)->SetWindowText(L"�Ͽ�");
		GetDlgItem(IDC_ADD)->EnableWindow();
		GetDlgItem(IDC_DELETE)->EnableWindow();
		GetDlgItem(IDC_EDIT)->EnableWindow();
		GetDlgItem(IDC_SEARCH)->EnableWindow();
		g_bIsConnected = TRUE;
	}
	else{
		GetDlgItem(IDC_CONNECT)->SetWindowText(L"����");
		GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_SEARCH)->EnableWindow(FALSE);
		g_pClientSocket->Disconnect();
		g_bIsConnected = FALSE;
	}
}

// CMIMainDlg message handlers

///������/�Ͽ���ť
void CMIMainDlg::OnBnClickedConnect()
{
	if(g_bIsConnected){//�Ѿ����ӣ���Ϊ�Ͽ�
		SetConnectStatus(FALSE);
		return;
	}
	
	///���ӷ�����
	CConnectDlg dlg;
	dlg.m_strAddress = m_strAddress;
	dlg.m_nPort = m_nPort;
	if(IDOK==dlg.DoModal()){
		if(g_pClientSocket->Create(SOCK_STREAM)){
 			if(g_pClientSocket->Connect(dlg.m_strAddress, dlg.m_nPort)){
				//��������һ����ɹ����������ö�ʱ����25��֮��û��DISCONNECT������Ϊ���ӳɹ�
				m_strAddress = dlg.m_strAddress;
				m_nPort = dlg.m_nPort;
				g_ProgressInfo.Show("�������ӷ�����...");
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
	if(nIDEvent==100){//����
		KillTimer(nIDEvent);
		g_ProgressInfo.Hide();
		GetDlgItem(IDC_CONNECT)->EnableWindow(TRUE);
		if(g_bIsConnected){
			ShowMsg("���ӳɹ�");
			SetConnectStatus(TRUE);
		}
	}

	CDialog::OnTimer(nIDEvent);
}

///��ȡ����
int ReceiveData()
{
	UINT timeout = 100;
	while(g_bIsDataComing==FALSE && timeout--){//ÿ100�����һ�Σ����ȴ�30��
		Sleep(100);
	}
	return g_bIsDataComing;
}

struct UserData{
	int id;
	char Name[20];
};

///��������
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
		ShowMsg("��ȡ��¼����ʧ��");
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
		ShowMsg("��ȡ��¼ʧ��");
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
		ShowMsg("��Ӽ�¼ʧ��");
	}
	if(strncmp(g_RecvData,"OK\r\n", 4)==0){
		ShowMsg("��Ӽ�¼�ɹ�");
		return TRUE;
	}
	ShowMsg("��Ӽ�¼ʧ��");
	return TRUE;
}

int CMIMainDlg::CmdDeleteRecordAt(int index)
{
	g_bIsDataComing = FALSE;
	char buf[100];
	sprintf(buf, "%s %04d %s", "CMD5", index, "\r\n");
	g_pClientSocket->Send(buf, strlen(buf));
	if(!ReceiveData()){
		ShowMsg("ɾ����¼ʧ��");
	}
	if(strncmp(g_RecvData,"OK\r\n", 4)==0){
		ShowMsg("ɾ����¼�ɹ�");
		return TRUE;
	}
	ShowMsg("ɾ����¼ʧ��");
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
	sprintf(data.Name, "%s", "������");
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
