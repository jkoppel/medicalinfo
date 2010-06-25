// MIMainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MIClient.h"
#include "MIMainDlg.h"
#include "ConnectDlg.h"
#include "GlobalVars.h"
#include "GlobalFuncs.h"
#include "ProgressInfo.h"
#include "PatientDlg.h"


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
		g_bIsConnected = TRUE;
	}
	else{
		GetDlgItem(IDC_CONNECT)->SetWindowText(_T("����"));
		GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
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
									m_lstPatient.InsertItem(index, CString("���˼�¼"));
									UpdateRowData(index, data);
								}
								else{
									delete[]pID;
									SetConnectStatus(FALSE);
									ShowMsg("����ʧ��");
									return;
								}
							}
							delete []pID;
							m_nRecNum = num;
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

void CMIMainDlg::UpdateRowData(int index, struct UserData data)
{
	int i=0;
	char buf[256];

	sprintf_s(buf, "%d", data.ID);
	m_lstPatient.SetItemText(index, i++, CString(buf));

	m_lstPatient.SetItemText(index, i++, CString(data.ScancodeID));

	sprintf_s(buf, "%d", data.Number);
	m_lstPatient.SetItemText(index, i++, CString(buf));

	m_lstPatient.SetItemText(index, i++, CString(data.Name));
	m_lstPatient.SetItemText(index, i++, CString(data.Sex));

	sprintf_s(buf, "%d", data.Age);
	m_lstPatient.SetItemText(index, i++, CString(buf));

	m_lstPatient.SetItemText(index, i++, CString(data.BirthDate));
	m_lstPatient.SetItemText(index, i++, CString(data.People));
	m_lstPatient.SetItemText(index, i++, CString(data.Department));
	m_lstPatient.SetItemText(index, i++, CString(data.TypeOfWork));
	m_lstPatient.SetItemText(index, i++, CString(data.Province));
	m_lstPatient.SetItemText(index, i++, CString(data.City));
	m_lstPatient.SetItemText(index, i++, CString(data.Address));
	m_lstPatient.SetItemText(index, i++, CString(data.ZipCode));
	m_lstPatient.SetItemText(index, i++, CString(data.Tel));
	m_lstPatient.SetItemText(index, i++, CString(data.ClinicalDiagnosis));

	sprintf_s(buf, "%d", data.Height);
	m_lstPatient.SetItemText(index, i++, CString(buf));

	m_lstPatient.SetItemText(index, i++, CString(data.Weight));
	m_lstPatient.SetItemText(index, i++, CString(data.CheckDate));
	m_lstPatient.SetItemText(index, i++, CString(data.Hazards));
	m_lstPatient.SetItemText(index, i++, CString(data.Pharmacy));
	m_lstPatient.SetItemText(index, i++, CString(data.PastHistory));

	UpdateData(FALSE);
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
	UINT timeout = 200;
	while(g_bIsDataComing==FALSE && timeout--){//ÿ100�����һ�Σ����ȴ�20��
		Sleep(100);
	}
	if(!g_bIsDataComing){
		return FALSE;
	}
	if(strncmp(g_RecvData, "OK", 2)!=0){
		return FALSE;
	}
	return TRUE;
}

///��������
int CMIMainDlg::CmdConnect()
{
	g_bIsDataComing = FALSE;
	g_pClientSocket->Send("CMD1||\r\n", (int)strlen("CMD1||\r\n"));
	if(!ReceiveData()){
		return FALSE;
	}
	return TRUE;
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

	if(!p){
		return FALSE;
	}

	g_strList.GetNext(p);//cmd
	if(!p){
		return FALSE;
	}

	strTmp = g_strList.GetNext(p);//num
	CString2Char(strTmp, buf);
	sscanf_s(buf, "%d", &num);

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

	if(!p){
		return FALSE;
	}

	g_strList.GetNext(p);//cmd
	if(!p){
		return FALSE;
	}

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

	return ParseRecvDataToRec(CString(g_RecvData), data);
}

int CMIMainDlg::CmdAppendRecord(struct UserData &data)
{
	g_bIsDataComing = FALSE;
	CString cmdStr;

	MakeAddOrModRecordCmd(TRUE, data, cmdStr);

	g_pClientSocket->Send(cmdStr);
	if(!ReceiveData()){
		return FALSE;
	}

	char buf[256];
	CString strTmp;
	POSITION p;

	ParseSeparatorString(CString(g_RecvData));
	p = g_strList.GetHeadPosition();

	if(!p){
		return FALSE;
	}

	g_strList.GetNext(p);//cmd
	if(!p){
		return FALSE;
	}

	strTmp = g_strList.GetNext(p);//ID
	if(!strTmp.IsEmpty()){
		CString2Char(strTmp, buf);
		sscanf_s(buf, "%d", &data.ID);
	}

	return TRUE;
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
	return TRUE;
}

int CMIMainDlg::CmdModifyRecordByID(int ID, struct UserData data)
{
	g_bIsDataComing = FALSE;
	CString cmdStr;

	if(ID!=data.ID){
		return FALSE;
	}

	MakeAddOrModRecordCmd(FALSE, data, cmdStr);

	g_pClientSocket->Send(cmdStr);
	if(!ReceiveData()){
		return FALSE;
	}

	return TRUE;
}


void CMIMainDlg::OnBnClickedAdd()
{
	int ret;
	struct UserData data;
	CPatientDlg dlg;

	if(dlg.DoModal()==IDOK){
		memset(&data, 0, sizeof(data));
		dlg.GetData(data);

		g_ProgressInfo.Show("�������...");

		ret = CmdAppendRecord(data);
		if(ret){
			m_lstPatient.InsertItem(m_nRecNum, CString("���˼�¼"));
			UpdateRowData(m_nRecNum++, data);
			ShowMsg("��ӳɹ�");
		}
		else{
			ShowMsg("���ʧ��");
		}
	}
}

void CMIMainDlg::OnBnClickedEdit()
{
	int index, ID;
	int ret;
	char buf[256];
	CString str;
	struct UserData data;

	index = (int)(m_lstPatient.GetFirstSelectedItemPosition() - 1);
	if(index<0){
		ShowMsg("��ѡ���¼");
		return;
	}

	str = m_lstPatient.GetItemText(index, 0);
	CString2Char(str, buf);
	sscanf_s(buf, "%d", &ID);

	ret = CmdGetRecordByID(ID, data);
	if(!ret){
		ShowMsg("��ȡ��¼ʧ��");
		return;
	}

	CPatientDlg dlg;
	memcpy(&dlg.m_Data, &data, sizeof(data));
	if(dlg.DoModal()==IDOK){
		g_ProgressInfo.Show("���ڱ���...");
		dlg.GetData(data);
		ret = CmdModifyRecordByID(ID, data);
		if(ret){
			UpdateRowData(index, data);
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
	str = m_lstPatient.GetItemText(index, 0);
	CString2Char(str, buf);
	sscanf_s(buf, "%d", &ID);

	g_ProgressInfo.Show("����ɾ��...");

	ret = CmdDeleteRecordByID(ID);
	if(ret){
		m_lstPatient.DeleteItem(index);
		m_nRecNum--;
		ShowMsg("ɾ���ɹ�");
	}
	else{
		ShowMsg("ɾ��ʧ��");
	}
}

