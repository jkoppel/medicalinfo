// Chat.cpp : implementation file
//

#include "stdafx.h"
#include "MIServer.h"
#include "ChatThread.h"
#include "GlobalFuncs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChatThread dialog
// ���Ի������MIServerThreadʹ�ã��ڿͻ������ӽ��������������ڣ�����������״̬
// ��������û����͹��������ݽ��д���


CChatThread::CChatThread(CWnd* pParent /*=NULL*/)
	: CDialog(CChatThread::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChatThread)
	//}}AFX_DATA_INIT
}


void CChatThread::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChatThread)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChatThread, CDialog)
	//{{AFX_MSG_MAP(CChatThread)
	ON_MESSAGE(ON_RECEIVE, OnReceiveData)
	ON_MESSAGE(ON_CLOSE, OnDisconnected)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChatThread message handlers

///���ӵ�SOCKET
void CChatThread::ConnectTo(SOCKET s)
{
	m_peer.SetParent(this);
	m_peer.AcceptServiceSocket(s);
}

///���յ��ͻ������ݵĴ����������ĺ���
LRESULT CChatThread::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	while(m_peer.GetDataSize() > 0 && m_peer.ReadString(m_sRecv));

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
		case CMD_CONNECT://����������˫��ͨ���Ƿ�����
			m_peer.Send(CString("OK||\r\n"));
			break;
		case CMD_GETRECORDNUM://��ȡ��¼����
			num = -1;
			if(!Cmd_GetRecordNum(num)){//��ȡ����
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			sprintf(buf, "%s||%d||%s", "OK", num, "\r\n");
			m_peer.Send(CString(buf));
			break;
		case CMD_GETRECORDBYID://����ID�Ż�ȡ��¼
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
		case CMD_APPENDRECORD://���һ����¼
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
		case CMD_DELETERECORDBYID://ɾ��һ����¼
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
		case CMD_EDITRECORDBYID://�޸�һ����¼
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

///�ͻ��˶˿����ӣ��رձ��Ի���
LRESULT CChatThread::OnDisconnected(WPARAM wParam, LPARAM lParam)
{
	CDialog::OnOK();
	return 0;
}

///��ʼ���Ի���
BOOL CChatThread::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowPos(&wndNoTopMost,0,0,0,0,SWP_HIDEWINDOW);  //������ʾ��ʽΪ���ϣ��Ҳ���ʾ��
	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);  //�������������в���ʾ
	SetWindowText(""); //��������������������������ﲻ����ʾ�ˡ�

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CChatThread::OnClose() 
{
	m_peer.Disconnect();
	
	CDialog::OnClose();
}
