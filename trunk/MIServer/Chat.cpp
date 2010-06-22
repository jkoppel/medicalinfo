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
// �������CMIServerDlgʹ�ã���������˸ô��ڣ����ڱ����ӵ�ʱ�򵯳�Chat�Ի���


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
	char buf[1000];
	int index;
	int num;
	struct UserData data;
	int ret = m_sRecv.GetLength();

	sprintf(cmd, "%s", m_sRecv.GetBuffer(m_sRecv.GetLength()));

	int cmdID = int(cmd[3]-'0');
	switch(cmdID){
		case 1:
			m_peer.Send(CString("OK\r\n"));
			break;
		case 2:
			num = Cmd_GetRecordNum();
			sprintf(buf, "%s %04d %s", "OK", num, "\r\n");
			m_peer.Send(CString(buf));
			break;
		case 3:
			sscanf(m_sRecv.GetBuffer(m_sRecv.GetLength()), "%s %04d %s", cmd, &index, buf);
			Cmd_GetRecordAt(index, data);
			sprintf(buf, "%s %04d %s %s", "OK", data.id, data.Name, "\r\n");
			m_peer.Send(CString(buf));
			break;
		case 4:
			sscanf(m_sRecv.GetBuffer(m_sRecv.GetLength()), "%s %04d %s %s", cmd, &data.id, data.Name, buf);
			Cmd_AppendRecord(data);
			m_peer.Send(CString("OK\r\n"));
			break;
		case 5:
			sscanf(m_sRecv.GetBuffer(m_sRecv.GetLength()), "%s %04d %s", cmd, &index, buf);
			Cmd_DeleteRecordAt(index);
			m_peer.Send(CString("OK\r\n"));
			break;
		default:
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