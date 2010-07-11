// MIMainDlg.cpp : implementation file
//

#include "stdafx.h"
#ifdef _WIN32_WCE
#include "..\\MIClient\\MIClient.h"
#else
#include "..\\MIClient_X86\\MIClient.h"
#endif
#include "MIMainDlg.h"
#include "ConnectDlg.h"
#include "..\\Common\\DialogEx.h"
#include "..\\Common\\GlobalVars.h"
#include "..\\Common\\GlobalFuncs.h"
#include "..\\Common\\ProgressInfo.h"
#include "..\\Common\\PatientDlg.h"


// CMIMainDlg dialog

IMPLEMENT_DYNAMIC(CMIMainDlg, CDialogEx)

CMIMainDlg::CMIMainDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(pParent, CMIMainDlg::IDD)
	, m_strScancodeID(_T(""))
	, m_strPageInfo(_T("0/0"))
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
	m_nStatusMode = MODE_UNPROCESSED;
	m_bShowSearchResult = FALSE;

	m_bDragging = FALSE;
	m_nDragIndex = -1;
	m_nDropIndex = -1;
	m_pDragImage = NULL;
	m_pDragList = NULL;
	m_pDropList = NULL;
	m_pDropWnd = NULL;

	m_pMenu = new CMenu();
	m_pMenu->CreatePopupMenu();
	m_pMenu->AppendMenu(MF_STRING, ID_REC_MOVEPREV, _T("上移"));
	m_pMenu->AppendMenu(MF_STRING, ID_REC_MOVENEXT, _T("下移"));
	m_pMenu->AppendMenu(MF_STRING, ID_REC_MOVETOFIRST, _T("移到第一条"));
	m_pMenu->AppendMenu(MF_STRING, ID_REC_MOVETOLAST, _T("移到最后一条"));
}

CMIMainDlg::~CMIMainDlg()
{
	delete m_pMenu;
}

void CMIMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PATIENT, m_lstPatient);
	DDX_Control(pDX, IDC_STATUS, m_ctrlStatus);
	DDX_Text(pDX, IDC_SCANCODEID, m_strScancodeID);
	DDX_Text(pDX, IDC_PAGEINFO, m_strPageInfo);
	DDX_Control(pDX, IDC_PAGEMODE, m_ctrlPageMode);
	DDX_Control(pDX, IDC_CONNECT, m_ctrlConnect);
	DDX_Control(pDX, IDC_SCANCODEID, m_ctrlScancodeID);
	DDX_Control(pDX, IDC_STATIC_SCANCODE, m_ctrlScancodeID_Static);
}


BEGIN_MESSAGE_MAP(CMIMainDlg, CDialogEx)
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
	ON_CBN_SELCHANGE(IDC_STATUS, &CMIMainDlg::OnCbnSelchangeStatus)
	ON_BN_CLICKED(IDC_SEARCH, &CMIMainDlg::OnBnClickedSearch)
	ON_CBN_SELCHANGE(IDC_PAGEMODE, &CMIMainDlg::OnCbnSelchangePagemode)
	ON_BN_CLICKED(IDC_SETTING, &CMIMainDlg::OnBnClickedSetting)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_PATIENT, &CMIMainDlg::OnNMRclickListPatient)
	ON_COMMAND(ID_REC_MOVEPREV, &CMIMainDlg::OnRecMovePrev)
	ON_COMMAND(ID_REC_MOVENEXT, &CMIMainDlg::OnRecMoveNext)
	ON_COMMAND(ID_REC_MOVETOFIRST, &CMIMainDlg::OnRecMoveToFirst)
	ON_COMMAND(ID_REC_MOVETOLAST, &CMIMainDlg::OnRecMoveToLast)
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
		GetDlgItem(IDC_CONNECT)->SetWindowText(_T("断开"));
		GetDlgItem(IDC_ADD)->EnableWindow();
		GetDlgItem(IDC_DELETE)->EnableWindow();
		GetDlgItem(IDC_EDIT)->EnableWindow();
		GetDlgItem(IDC_MOVEPREV)->EnableWindow();
		GetDlgItem(IDC_MOVENEXT)->EnableWindow();
		g_bIsConnected = TRUE;
	}
	else{
		GetDlgItem(IDC_CONNECT)->SetWindowText(_T("连接"));
		GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_MOVEPREV)->EnableWindow(FALSE);
		GetDlgItem(IDC_MOVENEXT)->EnableWindow(FALSE);
		m_lstPatient.DeleteAllItems();
		g_pClientSocket->Disconnect();
		g_bIsConnected = FALSE;
		m_nRecNum = 0;
		m_nCurrPageIndex = 0;
		m_nPageNum = 0;
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
	int num;
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
				GetDlgItem(IDC_CONNECT)->EnableWindow(FALSE);

				BOOL connected = FALSE;
				unsigned int timeout = 100;
				g_bIsDataComing = FALSE;
				while(timeout-- && !g_bIsDataComing){
					g_pClientSocket->Send("CMD1||\r\n", (int)strlen("CMD1||\r\n"));
					Sleep(100);
				}
				if(g_bIsDataComing && strncmp(g_RecvData, "OK", 2)==0){
					connected = TRUE;
				}
				GetDlgItem(IDC_CONNECT)->EnableWindow(TRUE);
				if(!connected){
					SetConnectStatus(FALSE);
					if(strncmp(g_RecvData, "ER||1||\r\n", strlen("ER||1||\r\n"))==0){
						ShowMsg("连接失败，请确认服务器的数据库连接是否正常。");
					}
					else{
						ShowMsg("连接失败");
					}
					return;
				}

				SetConnectStatus(TRUE);

				if(CmdConnect()){
					if(CmdGetRecordNum(num)){
						m_nRecNum = num;
						m_bPageMode = TRUE;
						m_nStatusMode = MODE_ALL;
						m_nCurrPageIndex = 0;
						if(num==0){
							m_lstPatient.DeleteAllItems();
							ShowMsg("连接成功");
							return;
						}
						UpdateCurrPage();
						ShowMsg("连接成功");
						return;
					}
				}
			}
		}
		SetConnectStatus(FALSE);
		ShowMsg("连接失败");
		return;
	}
	SetConnectStatus(FALSE);
}

///设置ListBox项目栏
void CMIMainDlg::InitListBox()
{
	m_lstPatient.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE);//|LVS_EX_HEADERDRAGDROP);

	int i = 0;

	m_lstPatient.InsertColumn(i++, CString("ID"), LVCFMT_LEFT, 30);
	m_lstPatient.InsertColumn(i++, CString("扫描码"), LVCFMT_LEFT, 50);
	m_lstPatient.InsertColumn(i++, CString("病按号"), LVCFMT_LEFT, 50);
	m_lstPatient.InsertColumn(i++, CString("姓名"), LVCFMT_LEFT, 50);
	m_lstPatient.InsertColumn(i++, CString("性别"), LVCFMT_LEFT, 40);
	m_lstPatient.InsertColumn(i++, CString("年龄"), LVCFMT_LEFT, 40);
	m_lstPatient.InsertColumn(i++, CString("出生日期"), LVCFMT_LEFT, 80);
	m_lstPatient.InsertColumn(i++, CString("民族"), LVCFMT_LEFT, 40);
	m_lstPatient.InsertColumn(i++, CString("工作部门"), LVCFMT_LEFT, 80);
	m_lstPatient.InsertColumn(i++, CString("工种"), LVCFMT_LEFT, 80);
	m_lstPatient.InsertColumn(i++, CString("省份"), LVCFMT_LEFT, 60);
	m_lstPatient.InsertColumn(i++, CString("城市"), LVCFMT_LEFT, 60);
	m_lstPatient.InsertColumn(i++, CString("详细地址"), LVCFMT_LEFT, 120);
	m_lstPatient.InsertColumn(i++, CString("邮编"), LVCFMT_LEFT, 40);
	m_lstPatient.InsertColumn(i++, CString("电话"), LVCFMT_LEFT, 80);
	m_lstPatient.InsertColumn(i++, CString("临床诊断"), LVCFMT_LEFT, 100);
	m_lstPatient.InsertColumn(i++, CString("身高"), LVCFMT_LEFT, 40);
	m_lstPatient.InsertColumn(i++, CString("体重"), LVCFMT_LEFT, 40);
	m_lstPatient.InsertColumn(i++, CString("检查日期"), LVCFMT_LEFT, 80);
	m_lstPatient.InsertColumn(i++, CString("危险因素"), LVCFMT_LEFT, 100);
	m_lstPatient.InsertColumn(i++, CString("用药"), LVCFMT_LEFT, 100);
	m_lstPatient.InsertColumn(i++, CString("既往病史"), LVCFMT_LEFT, 100);

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

	m_lstPatient.DeleteAllItems();

	num = m_nRecNum;
	if(num<=0){
		return;
	}

	g_ProgressInfo.Show("正在更新数据");

	pID = new int[num+10];
	if(!CmdGetAllIDs(pID, num, m_nStatusMode)){
		delete []pID;
		g_ProgressInfo.Hide();
		return;
	}

	if(m_bPageMode){
		offset = m_nCurrPageIndex * PAGE_SIZE;
		m_nPageNum = (num+PAGE_SIZE-1)/PAGE_SIZE;
		curr_page_size = (m_nCurrPageIndex < m_nPageNum - 1) ? PAGE_SIZE : num - (m_nPageNum-1) * PAGE_SIZE;
		m_strPageInfo.Format(CString("%d/%d"), m_nCurrPageIndex+1, m_nPageNum);
	}
	else{
		offset = 0;
		curr_page_size = num;
		m_strPageInfo.Format(CString("%d/%d"), 1, 1);
	}

	for(index=0;index<curr_page_size;index++){
		ret = CmdGetRecordByID(pID[offset+index], data);
		if(ret){
			m_lstPatient.InsertItem(index, CString("病人记录"));
			UpdateRowData(index, data);
		}
	}
	delete []pID;
	g_ProgressInfo.Hide();
}


BOOL CMIMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	g_pClientSocket = new CClientSocket(this);
	SetWindowPos(NULL, 0, 0, 240, 300, SWP_SHOWWINDOW);

	InitListBox();

	m_ctrlStatus.InsertString(0, CString("全部"));
	m_ctrlStatus.InsertString(1, CString("未处理"));
	m_ctrlStatus.InsertString(2, CString("已处理"));
	m_ctrlStatus.SetCurSel(0);

	m_ctrlPageMode.InsertString(0, CString("单页"));
	m_ctrlPageMode.InsertString(1, CString("多页"));
	m_ctrlPageMode.SetCurSel(1);

/*
	m_ctrlConnect.SetBkColor(RGB(0,255,0));
	m_ctrlScancodeID.SetBkColor(RGB(0,255,255));
	m_ctrlScancodeID_Static.SetBkColor(RGB(0,255,255));
*/
	return TRUE;
}

///获取数据
int ReceiveData()
{
	UINT timeout = 200;
	while(g_bIsDataComing==FALSE && timeout--){//每100毫秒查一次，共等待20秒
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

///连接命令
BOOL CMIMainDlg::CmdConnect()
{
	char buf[256];

	g_bIsDataComing = FALSE;
	sprintf_s(buf, "%s%d%s", "CMD", CMD_CONNECT, "||\r\n");
	g_pClientSocket->Send(buf);
	if(!ReceiveData()){
		return FALSE;
	}
	return TRUE;
}

BOOL CMIMainDlg::CmdGetRecordNum(int &num)
{
	char buf[256];
	CString strTmp;
	POSITION p;

	g_bIsDataComing = FALSE;
	sprintf_s(buf, "%s%d%s", "CMD", CMD_GETRECORDNUM, "||\r\n");
	g_pClientSocket->Send(buf);
	if(!ReceiveData()){
		return FALSE;
	}

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

BOOL CMIMainDlg::CmdGetAllIDs(int *pID, int &num, int mode)
{
	char buf[256];
	CString strTmp;
	POSITION p;
	num = 0;

	g_bIsDataComing = FALSE;
	sprintf_s(buf, "%s%d%s%d%s", "CMD", CMD_GETALLID, "||", mode, "||\r\n");
	g_pClientSocket->Send(buf);
	if(!ReceiveData()){
		return FALSE;
	}

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

BOOL CMIMainDlg::CmdGetRecordByID(int ID, struct UserData &data)
{
	char buf[256];

	g_bIsDataComing = FALSE;
	sprintf_s(buf, "%s%d%s%d%s", "CMD", CMD_GETRECORDBYID, "||", ID, "||\r\n");
	g_pClientSocket->Send(buf);
	if(!ReceiveData()){
		return FALSE;
	}

	return ParseRecvDataToRec(CString(g_RecvData), data);
}

BOOL CMIMainDlg::CmdAppendRecord(struct UserData &data)
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

BOOL CMIMainDlg::CmdDeleteRecordByID(int ID)
{
	char buf[256];

	g_bIsDataComing = FALSE;
	sprintf_s(buf, "%s%d%s%d%s", "CMD", CMD_DELETERECORDBYID, "||", ID, "||\r\n");
	g_pClientSocket->Send(buf, (int)strlen(buf));
	if(!ReceiveData()){
		return FALSE;
	}
	return TRUE;
}

BOOL CMIMainDlg::CmdModifyRecordByID(int ID, struct UserData data)
{
	CString cmdStr;

	if(ID!=data.ID){
		return FALSE;
	}

	MakeAddOrModRecordCmd(FALSE, data, cmdStr);

	g_bIsDataComing = FALSE;
	g_pClientSocket->Send(cmdStr);
	if(!ReceiveData()){
		return FALSE;
	}

	return TRUE;
}

BOOL CMIMainDlg::CmdGetNextFreeOrder(int &order)
{
	g_bIsDataComing = FALSE;
	CString cmdStr;
	char buf[256];

	sprintf_s(buf, "%s%d%s", "CMD", CMD_GETNEXTFREEORDER, "||\r\n");
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

BOOL CMIMainDlg::CmdGetOrderByID(int ID, int &order)
{
	char buf[256];

	g_bIsDataComing = FALSE;
	sprintf_s(buf, "%s%d%s%d%s", "CMD", CMD_GETORDERBYID, "||", ID, "||\r\n");
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

BOOL CMIMainDlg::CmdSetOrderByID(int ID, int order)
{
	char buf[256];

	g_bIsDataComing = FALSE;
	sprintf_s(buf, "%s%d%s%d%s", "CMD", CMD_SETORDERBYID, "||", ID, "||\r\n");
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

BOOL CMIMainDlg::CmdMoveOrder(int org_order, int dst_order)
{
	char buf[256];

	g_bIsDataComing = FALSE;
	sprintf_s(buf, "%s%d%s%d%s%d%s", "CMD", CMD_MOVEORDER, "||", org_order, "||", dst_order, "||\r\n");
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

BOOL CMIMainDlg::CmdMoveOrderPrev(int order)
{
	char buf[256];

	g_bIsDataComing = FALSE;
	sprintf_s(buf, "%s%d%s%d%s", "CMD", CMD_MOVEORDERPREV, "||", order, "||\r\n");
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

BOOL CMIMainDlg::CmdMoveOrderNext(int order)
{
	char buf[256];

	g_bIsDataComing = FALSE;
	sprintf_s(buf, "%s%d%s%d%s", "CMD", CMD_MOVEORDERNEXT, "||", order, "||\r\n");
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

BOOL CMIMainDlg::CmdSearchByScancodeID(const char *scan_code_id, int *pID, int &num)
{
	char buf[256];
	CString strTmp;
	POSITION p;
	num = 0;

	g_bIsDataComing = FALSE;
	sprintf_s(buf, "%s%d%s%s%s", "CMD", CMD_SEARCHBYSCANCODEID, "||", scan_code_id, "||\r\n");
	g_pClientSocket->Send(buf);
	if(!ReceiveData()){
		return FALSE;
	}

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


void CMIMainDlg::OnBnClickedAdd()
{
	int ret;
	struct UserData data;
	CPatientDlg dlg;

	dlg.m_sTitle = CString("添加病人资料");
	if(dlg.DoModal()==IDOK){
		memset(&data, 0, sizeof(data));
		dlg.GetData(data);

		g_ProgressInfo.Show("正在添加...");

		if(!CmdGetNextFreeOrder(data.Order)){
			ShowMsg("添加失败");
			return;
		}
		ret = CmdAppendRecord(data);
		if(ret){
			m_bShowSearchResult = FALSE;
			m_nStatusMode = MODE_ALL;
			m_ctrlStatus.SetCurSel(0);
			CmdGetRecordNum(m_nRecNum);
			if(!m_bPageMode){
				UpdateCurrPage();
			}
			else{
				m_nPageNum = (m_nRecNum+PAGE_SIZE-1)/PAGE_SIZE;
				m_nCurrPageIndex = m_nPageNum - 1;
				UpdateCurrPage();
			}
			m_lstPatient.SetItemState(m_lstPatient.GetItemCount()-1,LVNI_SELECTED,LVNI_SELECTED);

			ShowMsg("添加成功");
		}
		else{
			ShowMsg("添加失败");
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
		ShowMsg("请选择记录");
		return;
	}

	str = m_lstPatient.GetItemText(index, 0);
	CString2Char(str, buf);
	sscanf_s(buf, "%d", &ID);

	ret = CmdGetRecordByID(ID, data);
	if(!ret){
		ShowMsg("读取记录失败");
		return;
	}

	CPatientDlg dlg;
	dlg.m_sTitle = CString("编辑病人资料");
	memcpy(&dlg.m_Data, &data, sizeof(data));
	if(dlg.DoModal()==IDOK){
		g_ProgressInfo.Show("正在保存...");
		dlg.GetData(data);
		ret = CmdModifyRecordByID(ID, data);
		if(ret){
			UpdateRowData(index, data);
			ShowMsg("编辑成功");
		}
		else{
			ShowMsg("编辑失败");
		}
	}
}

void CMIMainDlg::OnBnClickedDelete() 
{
	int index;
	int ret;

	index = (int)m_lstPatient.GetFirstSelectedItemPosition()-1;
	if(index<0){
		ShowMsg("请选择记录");
		return;
	}

	if(MessageBox(_T("真的要删除该记录吗？"),0,MB_YESNO)!=IDYES){
		return;
	}

	CString str;
	char buf[100];
	int ID;
	str = m_lstPatient.GetItemText(index, 0);
	CString2Char(str, buf);
	sscanf_s(buf, "%d", &ID);

	g_ProgressInfo.Show("正在删除...");

	ret = CmdDeleteRecordByID(ID);
	if(ret){
		if(m_bShowSearchResult){
			m_lstPatient.DeleteItem(index);
			m_nRecNum--;
		}
		else{
			if(!m_bPageMode){
				m_lstPatient.DeleteItem(index);
				m_nRecNum--;
			}
			else{
				m_nRecNum --;
				m_nPageNum = (m_nRecNum + PAGE_SIZE -1)/PAGE_SIZE;
				if(m_nCurrPageIndex==m_nPageNum){
					m_nCurrPageIndex --;
				}
				UpdateCurrPage();
			}
		}

		ShowMsg("删除成功");
	}
	else{
		ShowMsg("删除失败");
	}
}


void CMIMainDlg::OnBnClickedMoveprev()
{
	int index;

	if(m_bShowSearchResult){
		return;
	}

	index = (int)m_lstPatient.GetFirstSelectedItemPosition()-1;
	if(index<0){
		ShowMsg("请选择记录");
		return;
	}

	if(!m_bPageMode && index==0){
		return;
	}
	if(m_bPageMode && m_nCurrPageIndex==0 && index==0){
		return;
	}

	CString str;
	char buf[100];
	int ID;
	struct UserData data;

	str = m_lstPatient.GetItemText(index, 0);
	CString2Char(str, buf);
	sscanf_s(buf, "%d", &ID);

	if(!CmdGetRecordByID(ID, data)){
		ShowMsg("上移失败");
		return;
	}
	if(!CmdMoveOrderPrev(data.Order)){
		ShowMsg("上移失败");
		return;
	}
	if(index>0){
		m_lstPatient.DeleteItem(index);
		m_lstPatient.InsertItem(index-1, CString("病人记录"));
		UpdateRowData(index-1, data);
		m_lstPatient.SetItemState(index-1, LVNI_SELECTED, LVNI_SELECTED);
	}
	else{
		m_nCurrPageIndex--;
		UpdateCurrPage();
		m_lstPatient.SetItemState(PAGE_SIZE-1, LVNI_SELECTED, LVNI_SELECTED);
	}
}

void CMIMainDlg::OnBnClickedMovenext()
{
	int index, page_size;

	if(m_bShowSearchResult){
		return;
	}

	index = (int)m_lstPatient.GetFirstSelectedItemPosition()-1;
	if(index<0){
		ShowMsg("请选择记录");
		return;
	}

	if(!m_bPageMode && index==m_nRecNum-1){
		return;
	}
	if(m_bPageMode && m_nCurrPageIndex==m_nPageNum-1 && index==m_nRecNum-m_nCurrPageIndex*PAGE_SIZE-1){
		return;
	}

	CString str;
	char buf[100];
	int ID;
	struct UserData data;

	str = m_lstPatient.GetItemText(index, 0);
	CString2Char(str, buf);
	sscanf_s(buf, "%d", &ID);

	if(!CmdGetRecordByID(ID, data)){
		ShowMsg("下移失败");
		return;
	}

	if(!CmdMoveOrderNext(data.Order)){
		ShowMsg("下移失败");
		return;
	}
	if(m_bPageMode){
		if(m_nCurrPageIndex<m_nPageNum-1){
			page_size = PAGE_SIZE;
		}
		else{
			page_size = m_nRecNum-m_nCurrPageIndex*PAGE_SIZE;
		}
	}
	if(!m_bPageMode || index < page_size-1){
		m_lstPatient.DeleteItem(index);
		m_lstPatient.InsertItem(index+1, CString("病人记录"));
		UpdateRowData(index+1, data);
		m_lstPatient.SetItemState(index+1, LVNI_SELECTED, LVNI_SELECTED);
	}
	else{
		m_nCurrPageIndex++;
		UpdateCurrPage();
		m_lstPatient.SetItemState(0, LVNI_SELECTED, LVNI_SELECTED);
	}
}

void CMIMainDlg::OnBnClickedPageFirst()
{
	if(!g_bIsConnected){
		return;
	}
	if(m_bShowSearchResult){
		return;
	}

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
	if(!g_bIsConnected){
		return;
	}
	if(m_bShowSearchResult){
		return;
	}

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
	if(!g_bIsConnected){
		return;
	}
	if(m_bShowSearchResult){
		return;
	}

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
	if(!g_bIsConnected){
		return;
	}
	if(m_bShowSearchResult){
		return;
	}

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
	if(m_bShowSearchResult){
		return;
	}

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
				TRACE(_T("m_nDropIndex is -1\n"));
				CListCtrl* pList = (CListCtrl*)m_pDropWnd;
				VERIFY (pList->SetItemState (m_nDropIndex, 0, LVIS_DROPHILITED));
				// redraw item
				VERIFY (pList->RedrawItems (m_nDropIndex, m_nDropIndex));
				pList->UpdateWindow ();
				m_nDropIndex = -1;
			}
			else //If we drag out of the CListCtrl altogether
			{
				TRACE(_T("m_nDropIndex is not -1\n"));
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

	CDialogEx::OnMouseMove(nFlags, point);
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

	CDialogEx::OnLButtonUp(nFlags, point);
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

	char buf[20];
	int ID_src, ID_dst;
	int order_src, order_dst;
	CString str;

	// Get item that was dragged
	pDragList->GetItem (lvi.plvi);
	lvi.sCol2 = pDragList->GetItemText(lvi.plvi->iItem, 1);

	if(m_nDragIndex==m_nDropIndex){
		return;
	}

	str = pDragList->GetItemText(lvi.plvi->iItem, 0);
	struct UserData data;
	CString2Char(str, buf);
	sscanf_s(buf, "%d", &ID_src);
	if(!CmdGetRecordByID(ID_src, data)){
		return;
	}

	str = pDropList->GetItemText(m_nDropIndex, 0);
	CString2Char(str, buf);
	sscanf_s(buf, "%d", &ID_dst);

	if(!CmdGetOrderByID(ID_src, order_src)){
		return;
	}
	if(!CmdGetOrderByID(ID_dst, order_dst)){
		return;
	}
	if(!CmdMoveOrder(order_src, order_dst)){
		return;
	}

	// Delete the original item (for Move operation)
	// This is optional. If you want to implement a Copy operation, don't delete.
	// This works very well though for re-arranging items within a CListCtrl.
	// It is written at present such that when dragging from one list to the other
	//  the item is copied, but if dragging within one list, the item is moved.
	if(pDragList == pDropList)
	{
		pDragList->DeleteItem (m_nDragIndex);
	}

	// Insert item into pDropList
	// if m_nDropIndex == -1, iItem = GetItemCount() (inserts at end of list), else iItem = m_nDropIndex
	lvi.plvi->iItem = (m_nDropIndex == -1) ? pDropList->GetItemCount () : m_nDropIndex;
	pDropList->InsertItem (lvi.plvi);
	UpdateRowData(lvi.plvi->iItem, data);

	// Select the new item we just inserted
	pDropList->SetItemState (lvi.plvi->iItem, LVIS_SELECTED, LVIS_SELECTED);
}

void CMIMainDlg::OnCbnSelchangeStatus()
{
	if(!g_bIsConnected){
		return;
	}

	m_nStatusMode = m_ctrlStatus.GetCurSel();
	m_bShowSearchResult = FALSE;

	m_nCurrPageIndex = 0;
	UpdateCurrPage();
}

void CMIMainDlg::OnBnClickedSearch()
{
	if(!g_bIsConnected){
		return;
	}

	UpdateData(TRUE);
	int num;
	int *pID = new int[m_nRecNum + 10];
	char buf[256];
	struct UserData data;

	g_ProgressInfo.Show("正在查询...");
	CString2Char(m_strScancodeID, buf);
	if(!CmdSearchByScancodeID(buf, pID, num)){
		delete []pID;
		g_ProgressInfo.Hide();
		ShowMsg("未找到数据");
		return;
	}
	m_bShowSearchResult = TRUE;
	m_lstPatient.DeleteAllItems();
	for(int i=0;i<num;i++){
		if(!CmdGetRecordByID(pID[i], data)){
			delete []pID;
			pID = NULL;
			g_ProgressInfo.Hide();
			ShowMsg("查询失败");
			return;
		}
		m_lstPatient.InsertItem(i, CString("病人资料"));
		UpdateRowData(i, data);
	}

	g_ProgressInfo.Hide();
	delete []pID;
	pID = NULL;
}

void CMIMainDlg::OnCbnSelchangePagemode()
{
	if(m_bShowSearchResult){
		return;
	}

	if(m_ctrlPageMode.GetCurSel()==0){;
		m_bPageMode = FALSE;
	}
	else{
		m_bPageMode = TRUE;
	}
	if(m_bPageMode){
		m_nCurrPageIndex = 0;
	}
	UpdateCurrPage();
}

void CMIMainDlg::OnBnClickedSetting()
{
	ShowMsg("设置");
}

void CMIMainDlg::OnNMRclickListPatient(NMHDR *pNMHDR, LRESULT *pResult)
{
	int index;
	POINT curPos;
	GetCursorPos(&curPos);

	if(!g_bIsConnected){
		*pResult = 0;
		return;
	}
	index = (int)(m_lstPatient.GetFirstSelectedItemPosition() - 1);
	if(index<0){
		*pResult = 0;
		return;
	}

	m_pMenu->TrackPopupMenu(TPM_LEFTALIGN, curPos.x, curPos.y, this);

	*pResult = 0;
}

void CMIMainDlg::OnRecMovePrev()
{
	OnBnClickedMoveprev();
}

void CMIMainDlg::OnRecMoveNext()
{
	OnBnClickedMovenext();
}

void CMIMainDlg::OnRecMoveToFirst()
{
	int index;

	index = (int)m_lstPatient.GetFirstSelectedItemPosition()-1;
	if(index<0){
		ShowMsg("请选择记录");
		return;
	}

	if(!m_bPageMode && index==0){
		return;
	}
	if(m_bPageMode && m_nCurrPageIndex==0 && index==0){
		return;
	}

	CString str;
	char buf[100];
	int ID_src, ID_dst, *pID;
	int num;
	int order_src, order_dst;

	str = m_lstPatient.GetItemText(index, 0);
	CString2Char(str, buf);
	sscanf_s(buf, "%d", &ID_src);

	g_ProgressInfo.Show("正在移动...");
	if(!CmdGetRecordNum(num)){
		ShowMsg("移动失败");
		return;
	}
	pID = new int[num];
	if(!CmdGetAllIDs(pID, num)){
		delete []pID;
		ShowMsg("移动失败");
		return;
	}
	ID_dst = pID[0];
	delete []pID;

	if(!CmdGetOrderByID(ID_src, order_src)){
		ShowMsg("移动失败");
		return;
	}
	if(!CmdGetOrderByID(ID_dst, order_dst)){
		ShowMsg("移动失败");
		return;
	}
	if(!CmdMoveOrder(order_src, order_dst)){
		ShowMsg("移动失败");
		return;
	}

	g_ProgressInfo.Hide();
	if(m_bShowSearchResult){
		m_bShowSearchResult = FALSE;
	}
	m_nCurrPageIndex = 0;
	UpdateCurrPage();
	m_lstPatient.SetItemState(0, LVNI_SELECTED, LVNI_SELECTED);
}

void CMIMainDlg::OnRecMoveToLast()
{
	int index;

	index = (int)m_lstPatient.GetFirstSelectedItemPosition()-1;
	if(index<0){
		ShowMsg("请选择记录");
		return;
	}

	if(!m_bPageMode && index==0){
		return;
	}
	if(m_bPageMode && m_nCurrPageIndex==m_nPageNum-1 && index==m_lstPatient.GetItemCount()-1){
		return;
	}

	CString str;
	char buf[100];
	int ID_src, ID_dst, *pID;
	int num;
	int order_src, order_dst;

	str = m_lstPatient.GetItemText(index, 0);
	CString2Char(str, buf);
	sscanf_s(buf, "%d", &ID_src);

	g_ProgressInfo.Show("正在移动...");
	if(!CmdGetRecordNum(num)){
		ShowMsg("移动失败");
		return;
	}
	pID = new int[num];
	if(!CmdGetAllIDs(pID, num)){
		delete []pID;
		ShowMsg("移动失败");
		return;
	}
	ID_dst = pID[num-1];
	delete []pID;

	if(!CmdGetOrderByID(ID_src, order_src)){
		ShowMsg("移动失败");
		return;
	}
	if(!CmdGetOrderByID(ID_dst, order_dst)){
		ShowMsg("移动失败");
		return;
	}
	if(!CmdMoveOrder(order_src, order_dst)){
		ShowMsg("移动失败");
		return;
	}

	g_ProgressInfo.Hide();
	if(m_bShowSearchResult){
		m_bShowSearchResult = FALSE;
	}
	m_nCurrPageIndex = m_nPageNum - 1;
	UpdateCurrPage();
	m_lstPatient.SetItemState(m_lstPatient.GetItemCount()-1, LVNI_SELECTED, LVNI_SELECTED);
}

#ifdef _WIN32_WCE
///重载窗口处理流程，处理触笔点按事件
LRESULT CMIMainDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	NMHDR *pNMHDR;

	switch(message){
		case WM_NOTIFY:
		{
			if((UINT)wParam==IDC_LIST_PATIENT){
				pNMHDR = (NMHDR*)lParam;
				if(pNMHDR->code==GN_CONTEXTMENU){
					int index;
					CPoint pt;
					NMRGINFO* pInfo;

					pInfo = (NMRGINFO*)pNMHDR;
					pt = pInfo->ptAction;

					if(!g_bIsConnected){
						break;;
					}
					index = (int)(m_lstPatient.GetFirstSelectedItemPosition() - 1);
					if(index<0){
						break;
					}

					m_pMenu->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);
				}
				break;
			}
		}
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}
#endif
