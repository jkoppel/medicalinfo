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
	m_strAddress = CString("192.168.0.109");
	m_nPort = 5000;
	m_nRecNum = 0;
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
					if(CmdGetRecordNum(num)){
						int *pID = NULL;
						pID = new int[num+10];
						if(CmdGetAllIDs(pID, num)){
							for(index=0;index<num;index++){
								ret = CmdGetRecordByID(pID[index], data);
								if(ret){
									this->m_pUserData[index] = data;
									m_lstPatient.InsertItem(index, CString("�û�����"));
									//TODO
									sprintf_s(buf, "%d", data.id);
									m_lstPatient.SetItemText(index, 0, CString(buf));
									m_lstPatient.SetItemText(index, 1, CString(data.Name));
								}
								else{
									delete[]pID;
									SetConnectStatus(FALSE);
									ShowMsg("����ʧ��");
									return;
								}
							}
							delete []pID;
							ShowMsg("���ӳɹ�");
							return;
						}
						delete []pID;
					}
				}
			}
		}
		SetConnectStatus(FALSE);
		ShowMsg("����ʧ��");
		return;
	}
	SetConnectStatus(FALSE);
}

///����ListBox��Ŀ��
void CMIMainDlg::InitListBox()
{
	m_lstPatient.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE);

	int i = 0;

	m_lstPatient.InsertColumn(i++, CString("ID"), LVCFMT_LEFT, 30);
	m_lstPatient.InsertColumn(i++, CString("ɨ����"), LVCFMT_LEFT, 50);
	m_lstPatient.InsertColumn(i++, CString("������"), LVCFMT_LEFT, 50);
	m_lstPatient.InsertColumn(i++, CString("����"), LVCFMT_LEFT, 50);
	m_lstPatient.InsertColumn(i++, CString("�Ա�"), LVCFMT_LEFT, 40);
	m_lstPatient.InsertColumn(i++, CString("����"), LVCFMT_LEFT, 40);
	m_lstPatient.InsertColumn(i++, CString("��������"), LVCFMT_LEFT, 80);
	m_lstPatient.InsertColumn(i++, CString("����"), LVCFMT_LEFT, 40);
	m_lstPatient.InsertColumn(i++, CString("��������"), LVCFMT_LEFT, 80);
	m_lstPatient.InsertColumn(i++, CString("����"), LVCFMT_LEFT, 80);
	m_lstPatient.InsertColumn(i++, CString("ʡ��"), LVCFMT_LEFT, 60);
	m_lstPatient.InsertColumn(i++, CString("����"), LVCFMT_LEFT, 60);
	m_lstPatient.InsertColumn(i++, CString("��ϸ��ַ"), LVCFMT_LEFT, 120);
	m_lstPatient.InsertColumn(i++, CString("�ʱ�"), LVCFMT_LEFT, 40);
	m_lstPatient.InsertColumn(i++, CString("�绰"), LVCFMT_LEFT, 80);
	m_lstPatient.InsertColumn(i++, CString("�ٴ����"), LVCFMT_LEFT, 100);
	m_lstPatient.InsertColumn(i++, CString("���"), LVCFMT_LEFT, 40);
	m_lstPatient.InsertColumn(i++, CString("����"), LVCFMT_LEFT, 40);
	m_lstPatient.InsertColumn(i++, CString("�������"), LVCFMT_LEFT, 80);
	m_lstPatient.InsertColumn(i++, CString("Σ������"), LVCFMT_LEFT, 100);
	m_lstPatient.InsertColumn(i++, CString("��ҩ"), LVCFMT_LEFT, 100);
	m_lstPatient.InsertColumn(i++, CString("������ʷ"), LVCFMT_LEFT, 100);

	SCROLLINFO ScrollInfo;
	ScrollInfo.cbSize = sizeof (SCROLLINFO);
	ScrollInfo.fMask = SIF_PAGE;
	ScrollInfo.nMin = 0;
	ScrollInfo.nMax = 1000;
	ScrollInfo.nPage = 200;
	ScrollInfo.nPos = 0;
	ScrollInfo.nTrackPos = -1;
	m_lstPatient.SetScrollInfo(SB_HORZ, &ScrollInfo);
}

BOOL CMIMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_pClientSocket = new CClientSocket(this);
	SetWindowPos(NULL, 0, 0, 240, 300, SWP_SHOWWINDOW);

	InitListBox();
	return TRUE;  
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
		return FALSE;
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

	m_nRecNum = num;
	
	return TRUE;
}

int CMIMainDlg::CmdGetAllIDs(int *pID, int &num)
{
	g_bIsDataComing = FALSE;
	g_pClientSocket->Send("CMD7||\r\n", (int)strlen("CMD7||\r\n"));
	if(!ReceiveData()){
		return FALSE;
	}

	char buf[256];
	CString strTmp;
	POSITION p;
	num = 0;

	ParseSeparatorString(CString(g_RecvData));
	p = g_strList.GetHeadPosition();

	g_strList.GetNext(p);//cmd

	do{
		strTmp = g_strList.GetNext(p);//ID
		if(!strTmp.IsEmpty()){
			CString2Char(strTmp, buf);
			sscanf_s(buf, "%d", &pID[num]);
			num++;
		}
		else{
			break;
		}
	}while(p);
	
	return TRUE;
}

int CMIMainDlg::CmdGetRecordByID(int ID, struct UserData &data)
{
	g_bIsDataComing = FALSE;
	char buf[1000];
	sprintf_s(buf, "%s||%d||%s", "CMD3", ID, "\r\n");
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

int CMIMainDlg::CmdDeleteRecordByID(int ID)
{
	g_bIsDataComing = FALSE;
	char buf[100];
	sprintf_s(buf, "%s||%d||%s", "CMD5", ID, "\r\n");
	g_pClientSocket->Send(buf, (int)strlen(buf));
	if(!ReceiveData()){
		return FALSE;
	}
	return (strncmp(g_RecvData,"OK", 2)==0);
}

int CMIMainDlg::CmdModifyRecordByID(int ID, struct UserData data)
{
	g_bIsDataComing = FALSE;
	char buf[1024];
	CString cmdStr;

	g_strList.RemoveAll();
	g_strList.AddTail(CString("CMD6"));

	sprintf_s(buf, "%d", ID);
	g_strList.AddTail(CString(buf));

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

		g_ProgressInfo.Show("�������...");

		ret = CmdAppendRecord(data);
		if(ret){
			m_lstPatient.InsertItem(m_nRecNum, CString("�û�����"));
			sprintf_s(buf, "%d", data.id);
			m_lstPatient.SetItemText(m_nRecNum, 0, CString(buf));
			m_lstPatient.SetItemText(m_nRecNum, 1, CString(data.Name));
			m_nRecNum ++;
			ShowMsg("��ӳɹ�");
		}
		else{
			ShowMsg("���ʧ��");
		}
	}
}

void CMIMainDlg::OnBnClickedEdit()
{
	int index;
	int ret;
	char buf[256];
	struct UserData data;

	index = (int)m_lstPatient.GetFirstSelectedItemPosition() - 1;
	if(index<0){
		ShowMsg("��ѡ���¼");
		return;
	}

	CPatientDataDlg dlg;
	dlg.m_nFlag = 0;
	ret = CmdGetRecordByID(index, data);
	if(!ret){
		ShowMsg("��ȡ��¼ʧ��");
		return;
	}

	dlg.m_nID = data.id;
	dlg.m_strName.Format(_T("%s"), data.Name);
	if(dlg.DoModal()==IDOK){
		data.id = dlg.m_nID;
		CString2Char(dlg.m_strName, data.Name);

		g_ProgressInfo.Show("����ɾ��...");

		ret = CmdModifyRecordByID(index, data);
		if(ret){
			sprintf_s(buf, "%d", data.id);
			m_lstPatient.SetItemText(index, 0, CString(buf));
			m_lstPatient.SetItemText(index, 1, CString(data.Name));
			ShowMsg("�༭�ɹ�");
		}
		else{
			ShowMsg("�༭ʧ��");
		}
	}
}

void CMIMainDlg::OnBnClickedDelete() 
{
	int index;
	int ret;

	index = (int)m_lstPatient.GetFirstSelectedItemPosition()-1;
	if(index<0){
		ShowMsg("��ѡ���¼");
		return;
	}

	if(MessageBox(_T("���Ҫɾ���ü�¼��"),0,MB_YESNO)!=IDYES){
		return;
	}

	CString str;
	char buf[100];
	int ID;
	m_lstPatient.GetItemText(index, 0);
	CString2Char(str, buf);
	sscanf_s(buf, "%d", &ID);

	g_ProgressInfo.Show("����ɾ��...");

	ret = CmdDeleteRecordByID(ID);
	if(ret){
		m_lstPatient.DeleteItem(ID);
		m_nRecNum--;
		ShowMsg("ɾ���ɹ�");
	}
	else{
		ShowMsg("ɾ��ʧ��");
	}
}

void CMIMainDlg::OnBnClickedSearch()
{
}

