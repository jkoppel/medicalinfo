// Chat.cpp : implementation file
//

#include "stdafx.h"
#include "MIServer.h"
#include "Chat.h"
#include "GlobalFuncs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChat dialog
// 本对话框类配合CMIServerDlg使用，如果启动了该窗口，则在被连接的时候弹出Chat对话框，提供

CChat::CChat(CWnd* pParent /*=NULL*/)
	: CDialog(CChat::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChat)
	m_sRecv = _T("");
	m_sSend = _T("");
	//}}AFX_DATA_INIT
}


void CChat::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChat)
	DDX_Text(pDX, IDC_RECV, m_sRecv);
	DDX_Text(pDX, IDC_SEND, m_sSend);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChat, CDialog)
	//{{AFX_MSG_MAP(CChat)
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	ON_MESSAGE(ON_RECEIVE, OnReceiveData)
	ON_MESSAGE(ON_CLOSE, OnDisconnected)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChat message handlers

void CChat::OnExit() 
{
	CDialog::OnCancel();
	m_peer.Disconnect();
}

void CChat::OnOK() 
{
	int len;
	UpdateData();
	m_sSend += "\r\n";
	len = m_sSend.GetLength();
	m_peer.Send(m_sSend, len);
}

void CChat::ConnectTo(SOCKET s)
{
	m_peer.SetParent(this);
	m_peer.AcceptServiceSocket(s);
}

LRESULT CChat::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	while(m_peer.GetDataSize() > 0 && m_peer.ReadString(m_sRecv)){
		GetDlgItem(IDC_RECV)->SetWindowText(m_sRecv);
	}

	char buf[2048];
	int ID, num, cmdID, *pID;
	int order, org_order, dst_order;
	CString strTmp;
	struct UserData data;
	POSITION p;

	if((m_sRecv.Find("CMD")!=0) || (m_sRecv.Find("\r\n")!=m_sRecv.GetLength()-2)){
		m_peer.Send(CString("ER||\r\n"));
		return -1;
	}
	ParseSeparatorString(m_sRecv);
	p = g_strList.GetHeadPosition();
	strTmp = g_strList.GetNext(p);
	strTmp = strTmp.Right(strTmp.GetLength()-3);//SKIP CMD
	sscanf(strTmp, "%d", &cmdID);
	switch(cmdID){
		case CMD_CONNECT://联机，测试双方通信是否正常
			m_peer.Send(CString("OK||\r\n"));
			break;
		case CMD_GETRECORDNUM://获取记录总数
			num = -1;
			if(!Cmd_GetRecordNum(num)){//读取错误
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			sprintf(buf, "%s||%d||%s", "OK", num, "\r\n");
			m_peer.Send(CString(buf));
			break;
		case CMD_GETRECORDBYID://根据ID号获取记录
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &ID);
			strTmp.ReleaseBuffer();

			if(!Cmd_GetRecordByID(ID, data)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			MakeSendCmdFromRec(data, strTmp);
			m_peer.Send(strTmp);
			break;
		case CMD_APPENDRECORD://添加一条记录
			if(!ParseRecvDataToRec(m_sRecv, data)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}

			if(!Cmd_AppendRecord(data)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}

			sprintf(buf, "%s||%d||%s", "OK", data.ID, "\r\n");
			m_peer.Send(CString(buf));
			break;
		case CMD_DELETERECORDBYID://删除一条记录
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &ID);
			strTmp.ReleaseBuffer();

			if(!Cmd_DeleteRecordByID(ID)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			m_peer.Send(CString("OK||\r\n"));
			break;
		case CMD_EDITRECORDBYID://修改一条记录
			if(!ParseRecvDataToRec(m_sRecv, data)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}

			if(!Cmd_ModifyRecordByID(data.ID, data)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}

			m_peer.Send(CString("OK||\r\n"));
			break;
		case CMD_GETALLID:
			if(!Cmd_GetRecordNum(num)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			pID = new int[num+10];
			if(!Cmd_GetAllIDs(pID, num)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			MakeIDToSeparatorString(pID, num, strTmp);
			m_peer.Send(strTmp);
			delete []pID;
			break;
		case CMD_GETNEXTFREEORDER:
			if(!Cmd_GetNextFreeOrder(order)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			sprintf(buf, "%s||%d||%s", "OK", order, "\r\n");
			m_peer.Send(CString(buf));
			break;
		case CMD_GETORDERBYID:
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &ID);
			strTmp.ReleaseBuffer();

			if(!Cmd_GetOrderByID(ID, order)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			sprintf(buf, "%s||%d||%s", "OK", order, "\r\n");
			m_peer.Send(CString(buf));
			break;
		case CMD_SETORDERBYID:
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &ID);
			strTmp.ReleaseBuffer();

			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &order);
			strTmp.ReleaseBuffer();

			if(!Cmd_SetOrderByID(ID, order)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			sprintf(buf, "%s", "OK||\r\n");
			m_peer.Send(CString(buf));
			break;
		case CMD_MOVEORDER:
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &org_order);
			strTmp.ReleaseBuffer();

			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &dst_order);
			strTmp.ReleaseBuffer();

			if(!Cmd_MoveOrder(org_order, dst_order)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			sprintf(buf, "%s", "OK||\r\n");
			m_peer.Send(CString(buf));
			break;
		case CMD_MOVEORDERPREV:
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &order);
			strTmp.ReleaseBuffer();

			if(!Cmd_MoveOrderPrev(order)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			sprintf(buf, "%s", "OK||\r\n");
			m_peer.Send(CString(buf));
			break;
		case CMD_MOVEORDERNEXT:
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &order);
			strTmp.ReleaseBuffer();

			if(!Cmd_MoveOrderNext(order)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			sprintf(buf, "%s", "OK||\r\n");
			m_peer.Send(CString(buf));
			break;
		default:
			m_peer.Send(CString("ER||\r\n"));
			break;
	}

	return 0;
}

LRESULT CChat::OnDisconnected(WPARAM wParam, LPARAM lParam)
{
	CDialog::OnOK();
	return 0;
}


void CChat::OnClose() 
{
	m_peer.Disconnect();
	
	CDialog::OnClose();
}
