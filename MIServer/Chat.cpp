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
// 本类配合CMIServerDlg使用，如果启动了该窗口，则在被连接的时候弹出Chat对话框


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

	char cmd[100];
	char buf[100][256];
	int ID, num, cmdID, *pID;
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
	sprintf(cmd, "%s", strTmp.GetBuffer(strTmp.GetLength()));

	cmdID = int(cmd[3]-'0');
	switch(cmdID){
		case 1://联机，测试双方通信是否正常
			m_peer.Send(CString("OK||\r\n"));
			break;
		case 2://获取记录总数
			num = -1;
			if(!Cmd_GetRecordNum(num)){//读取错误
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			sprintf(buf[0], "%s||%d||%s", "OK", num, "\r\n");
			m_peer.Send(CString(buf[0]));
			break;
		case 3://获取第n条记录
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &ID);

			if(!Cmd_GetRecordByID(ID, data)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			MakeSeparatorStringFromRec(data, strTmp);
			m_peer.Send(strTmp);
			break;
		case 4://添加一条记录
			if(!ParseSeparatorStringToRec(m_sRecv, data)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}

			if(!Cmd_AppendRecord(data)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}

			m_peer.Send(CString("OK||\r\n"));
			break;
		case 5://删除一条记录
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &ID);

			if(!Cmd_DeleteRecordByID(ID)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			m_peer.Send(CString("OK||\r\n"));
			break;
		case 6://修改一条记录
			if(!ParseSeparatorStringToRec(m_sRecv, data)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}

			if(!Cmd_ModifyRecordByID(data.ID, data)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}

			m_peer.Send(CString("OK||\r\n"));
			break;
		case 7:
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
