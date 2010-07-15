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
	//��ȡ����
	while(m_peer.GetDataSize() > 0 && m_peer.ReadString(m_sRecv));

	char buf[2048];
	int ID, num, cmdID, *pID;
	int order, org_order, dst_order;
	int mode;
	CString strTmp;
	struct UserData data;
	POSITION p;

	//����Ƿ��ѹ����������Ƿ���"CMD...\r\n"
	if((m_sRecv.Find("CMD")!=0) || (m_sRecv.Find("\r\n")!=m_sRecv.GetLength()-2)){
		m_peer.Send(CString("ER||\r\n"));
		return -1;
	}
	//δ�������ݿ⣬���ش�����1
	if(g_bIsDBConnected==FALSE){
		m_peer.Send(CString("ER||1||\r\n"));
		return -1;
	}
	//�����������������"||"�ֿ�������С�����浽g_strList��
	ParseSeparatorString(m_sRecv);
	//���ζ�ȡg_strList
	p = g_strList.GetHeadPosition();
	strTmp = g_strList.GetNext(p);
	//�Թ�"CMD"����ȡ�����
	strTmp = strTmp.Right(strTmp.GetLength()-3);
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
			//��ȡID��
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &ID);
			strTmp.ReleaseBuffer();

			if(!Cmd_GetRecordByID(ID, data)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			//����¼������װ����"||"�����ĳ��ַ����������͸��ͻ���
			MakeSendCmdFromRec(data, strTmp);
			m_peer.Send(strTmp);
			break;
		case CMD_APPENDRECORD://���һ����¼
			//������¼
			if(!ParseRecvDataToRec(m_sRecv, data)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			//��Ӽ�¼
			if(!Cmd_AppendRecord(data)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			//���ؼ�¼ID��
			sprintf(buf, "%s||%d||%s", "OK", data.ID, "\r\n");
			m_peer.Send(CString(buf));
			break;
		case CMD_DELETERECORDBYID://ɾ��һ����¼
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			//��ȡID��
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &ID);
			strTmp.ReleaseBuffer();
			//ɾ����¼
			if(!Cmd_DeleteRecordByID(ID)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			m_peer.Send(CString("OK||\r\n"));
			break;
		case CMD_EDITRECORDBYID://�޸�һ����¼
			//������¼
			if(!ParseRecvDataToRec(m_sRecv, data)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			//�޸ļ�¼
			if(!Cmd_ModifyRecordByID(data.ID, data)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			m_peer.Send(CString("OK||\r\n"));
			break;
		case CMD_GETALLID:
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			//��ȡģʽ(all:���м�¼��processed:�Ѵ���unprocessed:δ����)
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &mode);
			strTmp.ReleaseBuffer();
			//��ȡ��¼����
			if(!Cmd_GetRecordNum(num)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			pID = new int[num+10];
			if(!Cmd_GetAllIDs(pID, num, mode)){
				delete []pID;
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			//��ID����װ���ַ������ͻ�ȥ��ע���һ������ΪID����
			MakeIDToSeparatorString(pID, num, strTmp);
			m_peer.Send(strTmp);
			delete []pID;
			break;
		case CMD_GETNEXTFREEORDER://��ȡ��һ�����õ�Order������ֵ
			if(!Cmd_GetNextFreeOrder(order)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			sprintf(buf, "%s||%d||%s", "OK", order, "\r\n");
			m_peer.Send(CString(buf));
			break;
		case CMD_GETORDERBYID://����ID�Ż�ȡ��¼��Orderֵ
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			//��ȡ��¼ID��
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &ID);
			strTmp.ReleaseBuffer();
			//��ȡorder
			if(!Cmd_GetOrderByID(ID, order)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			sprintf(buf, "%s||%d||%s", "OK", order, "\r\n");
			m_peer.Send(CString(buf));
			break;
		case CMD_SETORDERBYID://����ID������orderֵ
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			//��ȡID��
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &ID);
			strTmp.ReleaseBuffer();

			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			//��ȡorderֵ
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &order);
			strTmp.ReleaseBuffer();
			//����order
			if(!Cmd_SetOrderByID(ID, order)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			sprintf(buf, "%s", "OK||\r\n");
			m_peer.Send(CString(buf));
			break;
		case CMD_MOVEORDER://�ƶ�order������org_orderҪ�ƶ�dst_order��λ�ã�������֮�������ǰ�ƻ��ߺ���
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			//��ȡorg_order
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &org_order);
			strTmp.ReleaseBuffer();
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			//��ȡdst_order
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &dst_order);
			strTmp.ReleaseBuffer();
			//�ƶ�order
			if(!Cmd_MoveOrder(org_order, dst_order)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			sprintf(buf, "%s", "OK||\r\n");
			m_peer.Send(CString(buf));
			break;
		case CMD_MOVEORDERPREV://��order��Ӧ�ļ�¼ǰ��һλ
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			//��ȡorder
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &order);
			strTmp.ReleaseBuffer();
			//ǰ��һλ
			if(!Cmd_MoveOrderPrev(order)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			sprintf(buf, "%s", "OK||\r\n");
			m_peer.Send(CString(buf));
			break;
		case CMD_MOVEORDERNEXT://��order��Ӧ�ļ�¼����һλ
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			//��ȡorder
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &order);
			strTmp.ReleaseBuffer();
			//����һλ
			if(!Cmd_MoveOrderNext(order)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			sprintf(buf, "%s", "OK||\r\n");
			m_peer.Send(CString(buf));
			break;
		case CMD_SEARCHBYSCANCODEID://��ɨ�����ѯ��¼
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			//��ȡɨ���벢��ѯ
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%s", buf);
			strTmp.ReleaseBuffer();
			pID = new int[1024];
			if(!Cmd_SearchByScancodeID(buf, pID, num)){
				delete []pID;
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			//��ID��Ϣ��װ�ɷ��ص��ַ���
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

	SetWindowPos(&wndNoTopMost,0,0,0,0,SWP_HIDEWINDOW);  //������ʾ��ʽΪ������û�г���ʵ���ǲ���ʾ��
	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);  //�������������в���ʾ
	SetWindowText("");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CChatThread::OnClose() 
{
	m_peer.Disconnect();
	
	CDialog::OnClose();
}
