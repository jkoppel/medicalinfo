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
	char buf[100];
	int len = 100;
	hostent *name;

	gethostname(buf, len);
	name = gethostbyname(buf);
	m_strAddress = inet_ntoa(*(in_addr *)name->h_addr_list[0]);
	//m_strAddress = CString(buf);//CString("192.168.0.109");
	m_nPort = 5000;
	m_nRecNum = 0;
	m_bPageMode = TRUE;
	m_nCurrPageIndex = 0;

	m_bDragging = FALSE;
	m_nDragIndex = -1;
	m_nDropIndex = -1;
	m_pDragImage = NULL;
	m_pDragList = NULL;
	m_pDropList = NULL;
	m_pDropWnd = NULL;
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
	ON_BN_CLICKED(IDC_MOVEPREV, &CMIMainDlg::OnBnClickedMoveprev)
	ON_BN_CLICKED(IDC_MOVENEXT, &CMIMainDlg::OnBnClickedMovenext)
	ON_BN_CLICKED(IDC_PAGE_FIRST, &CMIMainDlg::OnBnClickedPageFirst)
	ON_BN_CLICKED(IDC_PAGE_PREV, &CMIMainDlg::OnBnClickedPagePrev)
	ON_BN_CLICKED(IDC_PAGE_NEXT, &CMIMainDlg::OnBnClickedPageNext)
	ON_BN_CLICKED(IDC_PAGE_LAST, &CMIMainDlg::OnBnClickedPageLast)
	ON_BN_CLICKED(IDC_SWITCH_PAGEMODE, &CMIMainDlg::OnBnClickedSwitchPagemode)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_PATIENT, &CMIMainDlg::OnLvnBegindragListPatient)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
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
	int num;
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
						if(num==0){
							m_nRecNum = 0;
							m_lstPatient.DeleteAllItems();
							ShowMsg("���ӳɹ�");
							return;
						}
						UpdateCurrPage();
						ShowMsg("���ӳɹ�");
						return;
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
	m_lstPatient.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE);//|LVS_EX_HEADERDRAGDROP);

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

void CMIMainDlg::UpdateCurrPage()
{
	int ret, index, offset, num, curr_page_size;
	int *pID = NULL;
	struct UserData data;

	if(!CmdGetRecordNum(num)){
		return;
	}
	if(num==0){
		return;
	}
	pID = new int[num+10];
	if(!CmdGetAllIDs(pID, num)){
		delete []pID;
		return;
	}

	if(m_bPageMode){
		offset = m_nCurrPageIndex * PAGE_SIZE;
		m_nPageNum = (num+PAGE_SIZE-1)/PAGE_SIZE;
		curr_page_size = (m_nCurrPageIndex < m_nPageNum - 1) ? PAGE_SIZE : num - (m_nPageNum-1) * PAGE_SIZE;
	}
	else{
		offset = 0;
		curr_page_size = num;
	}

	m_lstPatient.DeleteAllItems();
	for(index=0;index<curr_page_size;index++){
		ret = CmdGetRecordByID(pID[offset+index], data);
		if(ret){
			m_lstPatient.InsertItem(index, CString("���˼�¼"));
			UpdateRowData(index, data);
		}
	}
	delete []pID;
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

	strTmp = g_strList.GetNext(p);//Num
	if(!p){
		return FALSE;
	}
	if(!strTmp.IsEmpty()){
		CString2Char(strTmp, buf);
		sscanf_s(buf, "%d", &num);
		if(num==0){
			return TRUE;
		}
	}

	int num1 = 0;
	do{
		strTmp = g_strList.GetNext(p);//ID
		if(!strTmp.IsEmpty()){
			CString2Char(strTmp, buf);
			if(strncmp(buf, "\r\n", 2)!=0){
				sscanf_s(buf, "%d", &pID[num1++]);
			}
			else{
				break;
			}
		}
		else{
			break;
		}
	}while(p);
	if(num!=num1){
		return FALSE;
	}
	
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

int CMIMainDlg::CmdGetNextFreeOrder(int &order)
{
	g_bIsDataComing = FALSE;
	CString cmdStr;
	char buf[256];

	sprintf_s(buf, "%s", "CMD8||\r\n");
	g_pClientSocket->Send(buf, (int)strlen(buf));

	g_pClientSocket->Send(cmdStr);
	if(!ReceiveData()){
		return FALSE;
	}

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

	strTmp = g_strList.GetNext(p);//order
	CString2Char(strTmp, buf);
	sscanf_s(buf, "%d", &order);

	return TRUE;
}

int CMIMainDlg::CmdGetOrderByID(int ID, int &order)
{
	g_bIsDataComing = FALSE;
	char buf[256];
	sprintf_s(buf, "%s||%d||%s", "CMD9", ID, "\r\n");
	g_pClientSocket->Send(buf, (int)strlen(buf));
	if(!ReceiveData()){
		return FALSE;
	}

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

	strTmp = g_strList.GetNext(p);//order
	CString2Char(strTmp, buf);
	sscanf_s(buf, "%d", &order);

	return TRUE;
}

int CMIMainDlg::CmdSetOrderByID(int ID, int order)
{
	g_bIsDataComing = FALSE;
	char buf[256];
	sprintf_s(buf, "%s||%d||%s", "CMDA", ID, "\r\n");
	g_pClientSocket->Send(buf, (int)strlen(buf));
	if(!ReceiveData()){
		return FALSE;
	}

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

	return TRUE;
}

int CMIMainDlg::CmdMoveOrder(int org_order, int dst_order)
{
	g_bIsDataComing = FALSE;
	char buf[256];
	sprintf_s(buf, "%s||%d||%d||%s", "CMDB", org_order, dst_order, "\r\n");
	g_pClientSocket->Send(buf, (int)strlen(buf));
	if(!ReceiveData()){
		return FALSE;
	}

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

		if(!CmdGetNextFreeOrder(data.Order)){
			ShowMsg("���ʧ��");
			return;
		}
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


void CMIMainDlg::OnBnClickedMoveprev()
{
	if(!m_bPageMode){
		return;
	}
}

void CMIMainDlg::OnBnClickedMovenext()
{
	if(!m_bPageMode){
		return;
	}
}

void CMIMainDlg::OnBnClickedPageFirst()
{
	if(!m_bPageMode){
		return;
	}
	if(m_nCurrPageIndex==0){
		return;
	}
	m_nCurrPageIndex = 0;
	UpdateCurrPage();
}

void CMIMainDlg::OnBnClickedPagePrev()
{
	if(!m_bPageMode){
		return;
	}
	if(m_nCurrPageIndex==0){
		return;
	}
	m_nCurrPageIndex--;
	UpdateCurrPage();
}

void CMIMainDlg::OnBnClickedPageNext()
{
	if(!m_bPageMode){
		return;
	}
	if(m_nCurrPageIndex==m_nPageNum-1){
		return;
	}
	m_nCurrPageIndex++;
	UpdateCurrPage();
}

void CMIMainDlg::OnBnClickedPageLast()
{
	if(!m_bPageMode){
		return;
	}
	if(m_nCurrPageIndex==m_nPageNum-1){
		return;
	}
	m_nCurrPageIndex = m_nPageNum - 1;
	UpdateCurrPage();
}

void CMIMainDlg::OnBnClickedSwitchPagemode()
{
	m_bPageMode = !m_bPageMode;
	if(m_bPageMode){
		m_nCurrPageIndex = 0;
	}
	UpdateCurrPage();
}

void CMIMainDlg::OnLvnBegindragListPatient(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	m_nDragIndex = pNMLV->iItem;

	//// Create a drag image
	POINT pt;
	int nOffset = -10; //offset in pixels for drag image
	pt.x = nOffset;
	pt.y = nOffset;

	m_pDragImage = m_lstPatient.CreateDragImage(m_nDragIndex, &pt);
	ASSERT(m_pDragImage);//make sure it was created
	//We will call delete later (in LButtonUp) to clean this up

	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_DRAGDROP);
	m_pDragImage->Replace(0, &bitmap, &bitmap);

	//// Change the cursor to the drag image
	////	(still must perform DragMove() in OnMouseMove() to show it moving)
	m_pDragImage->BeginDrag(0, CPoint(nOffset, nOffset - 4));
	m_pDragImage->DragEnter(GetDesktopWindow(), pNMLV->ptAction);

	//// Set dragging flag and others
	m_bDragging = TRUE;	//we are in a drag and drop operation
	m_nDropIndex = -1;	//we don't have a drop index yet
	m_pDragList = &m_lstPatient; //make note of which list we are dragging from
	m_pDropWnd = &m_lstPatient;	//at present the drag list is the drop list

	//// Capture all mouse messages
	SetCapture ();

	*pResult = 0;
}

void CMIMainDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		//// Move the drag image
		CPoint pt(point);	//get our current mouse coordinates
		ClientToScreen(&pt); //convert to screen coordinates
		m_pDragImage->DragMove(pt); //move the drag image to those coordinates
		// Unlock window updates (this allows the dragging image to be shown smoothly)
		m_pDragImage->DragShowNolock(false);

		//// Get the CWnd pointer of the window that is under the mouse cursor
		CWnd* pDropWnd = WindowFromPoint (pt);
		ASSERT(pDropWnd); //make sure we have a window

		//// If we drag outside current window we need to adjust the highlights displayed
		if (pDropWnd != m_pDropWnd)
		{
			if (m_nDropIndex != -1) //If we drag over the CListCtrl header, turn off the hover highlight
			{
				TRACE("m_nDropIndex is -1\n");
				CListCtrl* pList = (CListCtrl*)m_pDropWnd;
				VERIFY (pList->SetItemState (m_nDropIndex, 0, LVIS_DROPHILITED));
				// redraw item
				VERIFY (pList->RedrawItems (m_nDropIndex, m_nDropIndex));
				pList->UpdateWindow ();
				m_nDropIndex = -1;
			}
			else //If we drag out of the CListCtrl altogether
			{
				TRACE("m_nDropIndex is not -1\n");
				CListCtrl* pList = (CListCtrl*)m_pDropWnd;
				int i = 0;
				int nCount = pList->GetItemCount();
				for(i = 0; i < nCount; i++)
				{
					pList->SetItemState(i, 0, LVIS_DROPHILITED);
				}
				pList->RedrawItems(0, nCount);
				pList->UpdateWindow();
			}
		}

		// Save current window pointer as the CListCtrl we are dropping onto
		m_pDropWnd = pDropWnd;

		// Convert from screen coordinates to drop target client coordinates
		pDropWnd->ScreenToClient(&pt);

		//If we are hovering over a CListCtrl we need to adjust the highlights
		if(pDropWnd->IsKindOf(RUNTIME_CLASS (CListCtrl)))
		{			
			//Note that we can drop here
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			UINT uFlags;
			CListCtrl* pList = (CListCtrl*)pDropWnd;

			// Turn off hilight for previous drop target
			pList->SetItemState (m_nDropIndex, 0, LVIS_DROPHILITED);
			// Redraw previous item
			pList->RedrawItems (m_nDropIndex, m_nDropIndex);
			
			// Get the item that is below cursor
			m_nDropIndex = ((CListCtrl*)pDropWnd)->HitTest(pt, &uFlags);
			// Highlight it
			pList->SetItemState(m_nDropIndex, LVIS_DROPHILITED, LVIS_DROPHILITED);
			// Redraw item
			pList->RedrawItems(m_nDropIndex, m_nDropIndex);
			pList->UpdateWindow();
		}
		else
		{
			//If we are not hovering over a CListCtrl, change the cursor
			// to note that we cannot drop here
			SetCursor(LoadCursor(NULL, IDC_NO));
		}
		// Lock window updates
		m_pDragImage->DragShowNolock(true);
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CMIMainDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		// Release mouse capture, so that other controls can get control/messages
		ReleaseCapture ();

		// Note that we are NOT in a drag operation
		m_bDragging = FALSE;

		// End dragging image
		m_pDragImage->DragLeave (GetDesktopWindow ());
		m_pDragImage->EndDrag ();
		delete m_pDragImage; //must delete it because it was created at the beginning of the drag

		CPoint pt (point); //Get current mouse coordinates
		ClientToScreen (&pt); //Convert to screen coordinates
		// Get the CWnd pointer of the window that is under the mouse cursor
		CWnd* pDropWnd = WindowFromPoint (pt);
		ASSERT (pDropWnd); //make sure we have a window pointer
		// If window is CListCtrl, we perform the drop
		if (pDropWnd->IsKindOf (RUNTIME_CLASS (CListCtrl)))
		{
			m_pDropList = (CListCtrl*)pDropWnd; //Set pointer to the list we are dropping on
			DropItemOnList(m_pDragList, m_pDropList); //Call routine to perform the actual drop
		}
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CMIMainDlg::DropItemOnList(CListCtrl* pDragList, CListCtrl* pDropList)
{
	//This routine performs the actual drop of the item dragged.
	//It simply grabs the info from the Drag list (pDragList)
	// and puts that info into the list dropped on (pDropList).
	//Send:	pDragList = pointer to CListCtrl we dragged from,
	//		pDropList = pointer to CListCtrl we are dropping on.
	//Return: nothing.

	////Variables


	// Unhilight the drop target
	pDropList->SetItemState (m_nDropIndex, 0, LVIS_DROPHILITED);

	//Set up the LV_ITEM for retrieving item from pDragList and adding the new item to the pDropList
	TCHAR szLabel[256];
	LVITEM lviT;

	ZeroMemory(&lviT, sizeof (LVITEM)); //allocate and clear memory space for LV_ITEM
	lviT.iItem		= m_nDragIndex;
	lviT.mask		= LVIF_TEXT;
	lviT.pszText	= szLabel;
	lviT.cchTextMax	= 255;

	lvItem lvi;
	lvi.plvi = &lviT;
	lvi.plvi->iItem = m_nDragIndex;
	lvi.plvi->mask = LVIF_TEXT;
	lvi.plvi->pszText = szLabel;
	lvi.plvi->cchTextMax = 255;

	// Get item that was dragged
	pDragList->GetItem (lvi.plvi);
	lvi.sCol2 = pDragList->GetItemText(lvi.plvi->iItem, 1);
	CString str = pDragList->GetItemText(lvi.plvi->iItem, 0);
	char buf[20];
	int ID;
	struct UserData data;
	CString2Char(str, buf);
	sscanf_s(buf, "%d", &ID);
	CmdGetRecordByID(ID, data);

	// Delete the original item (for Move operation)
	// This is optional. If you want to implement a Copy operation, don't delete.
	// This works very well though for re-arranging items within a CListCtrl.
	// It is written at present such that when dragging from one list to the other
	//  the item is copied, but if dragging within one list, the item is moved.
	if(pDragList == pDropList)
	{
		pDragList->DeleteItem (m_nDragIndex);
		if(m_nDragIndex < m_nDropIndex) m_nDropIndex--; //decrement drop index to account for item
														//being deleted above it
	}

	// Insert item into pDropList
	// if m_nDropIndex == -1, iItem = GetItemCount() (inserts at end of list), else iItem = m_nDropIndex
	lvi.plvi->iItem = (m_nDropIndex == -1) ? pDropList->GetItemCount () : m_nDropIndex;
	pDropList->InsertItem (lvi.plvi);
	UpdateRowData(lvi.plvi->iItem, data);

	// Select the new item we just inserted
	pDropList->SetItemState (lvi.plvi->iItem, LVIS_SELECTED, LVIS_SELECTED);
}
