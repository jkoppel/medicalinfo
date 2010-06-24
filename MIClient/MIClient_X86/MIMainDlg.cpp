// MIMainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MIClient.h"
#include "MIMainDlg.h"
#include "ConnectDlg.h"
#include "GlobalVars.h"
#include "GlobalFuncs.h"
#include "ProgressInfo.h"
#include "PatientDataDlg.h"


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
	DDX_Control(pDX, IDC_LIST_PATIENT, m_lstPatient);
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
		GetDlgItem(IDC_CONNECT)->SetWindowText(_T("�Ͽ�"));
		GetDlgItem(IDC_ADD)->EnableWindow();
		GetDlgItem(IDC_DELETE)->EnableWindow();
		GetDlgItem(IDC_EDIT)->EnableWindow();
		GetDlgItem(IDC_SEARCH)->EnableWindow();
		g_bIsConnected = TRUE;
	}
	else{
		GetDlgItem(IDC_CONNECT)->SetWindowText(_T("����"));
		GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_SEARCH)->EnableWindow(FALSE);
		m_lstPatient.DeleteAllItems();
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
	int ret, num, index;
	struct UserData data;
	char buf[1024];
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
				GetDlgItem(IDC_CONNECT)->EnableWindow(FALSE);

				//SetTimer(100, 2000, NULL);//TODO 2000->25000

				BOOL connected = FALSE;
				unsigned int timeout = 100;
				g_bIsDataComing = FALSE;
				while(timeout--){
					g_pClientSocket->Send("CMD1||\r\n", (int)strlen("CMD1||\r\n"));
					if(g_bIsDataComing==TRUE && strncmp(g_RecvData, "OK", 2)==0){
						connected = TRUE;
						break;
					}
					Sleep(100);
				}
				GetDlgItem(IDC_CONNECT)->EnableWindow(TRUE);
				if(!connected){
					SetConnectStatus(FALSE);
					ShowMsg("����ʧ��");
					return;
				}

				SetConnectStatus(TRUE);

				if(CmdConnect()){
					ret = CmdGetRecordNum(num);
					if(ret){
						for(index=0;index<num;index++){
							ret = CmdGetRecordAt(index, data);
							if(ret){
								this->m_pUserData[index] = data;
								m_lstPatient.InsertItem(index, CString("�û�����"));
								sprintf_s(buf, "%d", data.id);
								m_lstPatient.SetItemText(index, 0, CString(buf));
								m_lstPatient.SetItemText(index, 1, CString(data.Name));
							}
							else{
								SetConnectStatus(FALSE);
								ShowMsg("����ʧ��");
								return;
							}
						}
					}
				}
				ShowMsg("���ӳɹ�");
				return;
			}
		}
		ShowMsg("����ʧ��");
	}
	SetConnectStatus(FALSE);
}

///����ListBox��Ŀ��
void CMIMainDlg::InitListBox()
{
	m_lstPatient.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE);

	m_lstPatient.InsertColumn(0, CString("�û�ID"), LVCFMT_LEFT, 40);
	m_lstPatient.InsertColumn(1, CString("�û���"), LVCFMT_LEFT, 120);
}

BOOL CMIMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_pClientSocket = new CClientSocket(this);
	SetWindowPos(NULL, 0, 0, 240, 290, SWP_SHOWWINDOW);

	InitListBox();
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

///��������
int CMIMainDlg::CmdConnect()
{
	g_bIsDataComing = FALSE;
	g_pClientSocket->Send("CMD1||\r\n", (int)strlen("CMD1||\r\n"));
	if(!ReceiveData()){
		return FALSE;
	}
	if(strncmp(g_RecvData,"OK", 2)==0){
		return TRUE;
	}
	return FALSE;
}

int CMIMainDlg::CmdGetRecordNum(int &num)
{
	g_bIsDataComing = FALSE;
	g_pClientSocket->Send("CMD2||\r\n", (int)strlen("CMD2||\r\n"));
	if(!ReceiveData()){
		return -1;
	}

	char buf[256];
	CString strTmp;
	POSITION p;

	ParseSeparatorString(CString(g_RecvData));
	p = g_strList.GetHeadPosition();

	g_strList.GetNext(p);//cmd

	strTmp = g_strList.GetNext(p);//num
	CString2Char(strTmp, buf);
	sscanf_s(buf, "%d", &num);
	
	return TRUE;
}

int CMIMainDlg::CmdGetRecordAt(int index, struct UserData &data)
{
	g_bIsDataComing = FALSE;
	char buf[1000];
	sprintf_s(buf, "%s||%d||%s", "CMD3", index, "\r\n");
	g_pClientSocket->Send(buf, (int)strlen(buf));
	if(!ReceiveData()){
		return FALSE;
	}

	CString strTmp;
	POSITION p;

	ParseSeparatorString(CString(g_RecvData));
	p = g_strList.GetHeadPosition();

	g_strList.GetNext(p);//cmd

	strTmp = g_strList.GetNext(p);//id
	CString2Char(strTmp, buf);
	sscanf_s(buf, "%d", &data.id);

	strTmp = g_strList.GetNext(p);//id
	CString2Char(strTmp, data.Name);

	sprintf_s(buf, "%s %d %s", "rec data:", data.id, data.Name);

	return TRUE;
}

int CMIMainDlg::CmdAppendRecord(struct UserData data)
{
	g_bIsDataComing = FALSE;
	char buf[1024];
	CString cmdStr;

	g_strList.RemoveAll();
	g_strList.AddTail(CString("CMD4"));

	sprintf_s(buf, "%d", data.id);
	g_strList.AddTail(CString(buf));
	
	g_strList.AddTail(CString(data.Name));
	g_strList.AddTail(CString("\r\n"));
	MakeSeparatorString(cmdStr);

	CString2Char(cmdStr, buf);
	g_pClientSocket->Send(buf, (int)strlen(buf));
	if(!ReceiveData()){
		return FALSE;
	}

	return (strncmp(g_RecvData,"OK", 2)==0);
}

int CMIMainDlg::CmdDeleteRecordAt(int index)
{
	g_bIsDataComing = FALSE;
	char buf[100];
	sprintf_s(buf, "%s||%d||%s", "CMD5", index, "\r\n");
	g_pClientSocket->Send(buf, (int)strlen(buf));
	if(!ReceiveData()){
		return FALSE;
	}
	return (strncmp(g_RecvData,"OK\r\n", 4)==0);
}

void CMIMainDlg::OnBnClickedAdd()
{
	int ret;
	char buf[256];
	struct UserData data;
	CPatientDataDlg dlg;
	dlg.m_nFlag = 0;
	if(dlg.DoModal()==IDOK){
		data.id = dlg.m_nID;
		CString2Char(dlg.m_strName, data.Name);
		ret = CmdAppendRecord(data);
		if(ret){
			m_lstPatient.InsertItem(0, CString("�û�����"));
			sprintf_s(buf, "%d", data.id);
			m_lstPatient.SetItemText(0, 0, CString(buf));
			m_lstPatient.SetItemText(0, 1, CString(data.Name));
			ShowMsg("��ӳɹ�");
		}
		else{
			ShowMsg("���ʧ��");
		}
	}
}

void CMIMainDlg::OnBnClickedEdit()
{
	POSITION pos;
	int ret;
	char buf[256];
	struct UserData data;

	pos = m_lstPatient.GetFirstSelectedItemPosition();
	if(pos<=0){
		ShowMsg("��ѡ���¼");
		return;
	}

	CPatientDataDlg dlg;
	dlg.m_nFlag = 0;
	if(dlg.DoModal()==IDOK){
		data.id = dlg.m_nID;
		CString2Char(dlg.m_strName, data.Name);
		ret = CmdAppendRecord(data);
		if(ret){
			m_lstPatient.InsertItem(0, CString("�û�����"));
			sprintf_s(buf, "%s", data.id);
			m_lstPatient.SetItemText(0, 0, CString(buf));
			m_lstPatient.SetItemText(0, 1, CString(data.Name));
			ShowMsg("��ӳɹ�");
		}
		else{
			ShowMsg("���ʧ��");
		}
	}
}

void CMIMainDlg::OnBnClickedDelete() 
{
}

void CMIMainDlg::OnBnClickedSearch()
{
}

