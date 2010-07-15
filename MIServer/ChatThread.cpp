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
// 本对话框配合MIServerThread使用，在客户端连接进来后启动本窗口，但处于隐藏状态
// 启动后对用户发送过来的数据进行处理


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

///连接到SOCKET
void CChatThread::ConnectTo(SOCKET s)
{
	m_peer.SetParent(this);
	m_peer.AcceptServiceSocket(s);
}

///接收到客户端数据的处理函数，核心函数
LRESULT CChatThread::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	//读取数据
	while(m_peer.GetDataSize() > 0 && m_peer.ReadString(m_sRecv));

	char buf[2048];
	int ID, num, cmdID, *pID;
	int order, org_order, dst_order;
	int mode;
	CString strTmp;
	struct UserData data;
	POSITION p;

	//检查是否已过来的命令是否是"CMD...\r\n"
	if((m_sRecv.Find("CMD")!=0) || (m_sRecv.Find("\r\n")!=m_sRecv.GetLength()-2)){
		m_peer.Send(CString("ER||\r\n"));
		return -1;
	}
	//未连接数据库，返回错误编号1
	if(g_bIsDBConnected==FALSE){
		m_peer.Send(CString("ER||1||\r\n"));
		return -1;
	}
	//解析过来的命令串，按"||"分开成若干小串，存到g_strList中
	ParseSeparatorString(m_sRecv);
	//依次读取g_strList
	p = g_strList.GetHeadPosition();
	strTmp = g_strList.GetNext(p);
	//略过"CMD"，获取命令号
	strTmp = strTmp.Right(strTmp.GetLength()-3);
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
			//获取ID号
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &ID);
			strTmp.ReleaseBuffer();

			if(!Cmd_GetRecordByID(ID, data)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			//将记录数据组装成以"||"隔开的长字符串，并发送给客户端
			MakeSendCmdFromRec(data, strTmp);
			m_peer.Send(strTmp);
			break;
		case CMD_APPENDRECORD://添加一条记录
			//解析记录
			if(!ParseRecvDataToRec(m_sRecv, data)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			//添加记录
			if(!Cmd_AppendRecord(data)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			//返回记录ID号
			sprintf(buf, "%s||%d||%s", "OK", data.ID, "\r\n");
			m_peer.Send(CString(buf));
			break;
		case CMD_DELETERECORDBYID://删除一条记录
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			//获取ID号
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &ID);
			strTmp.ReleaseBuffer();
			//删除记录
			if(!Cmd_DeleteRecordByID(ID)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			m_peer.Send(CString("OK||\r\n"));
			break;
		case CMD_EDITRECORDBYID://修改一条记录
			//解析记录
			if(!ParseRecvDataToRec(m_sRecv, data)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			//修改记录
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
			//获取模式(all:所有记录；processed:已处理；unprocessed:未处理)
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &mode);
			strTmp.ReleaseBuffer();
			//获取记录数量
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
			//将ID号组装成字符串发送回去，注意第一个数据为ID数量
			MakeIDToSeparatorString(pID, num, strTmp);
			m_peer.Send(strTmp);
			delete []pID;
			break;
		case CMD_GETNEXTFREEORDER://获取下一个可用的Order即排序值
			if(!Cmd_GetNextFreeOrder(order)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			sprintf(buf, "%s||%d||%s", "OK", order, "\r\n");
			m_peer.Send(CString(buf));
			break;
		case CMD_GETORDERBYID://根据ID号获取记录的Order值
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			//获取记录ID号
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &ID);
			strTmp.ReleaseBuffer();
			//获取order
			if(!Cmd_GetOrderByID(ID, order)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			sprintf(buf, "%s||%d||%s", "OK", order, "\r\n");
			m_peer.Send(CString(buf));
			break;
		case CMD_SETORDERBYID://根据ID号设置order值
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			//获取ID号
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &ID);
			strTmp.ReleaseBuffer();

			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			//获取order值
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &order);
			strTmp.ReleaseBuffer();
			//设置order
			if(!Cmd_SetOrderByID(ID, order)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			sprintf(buf, "%s", "OK||\r\n");
			m_peer.Send(CString(buf));
			break;
		case CMD_MOVEORDER://移动order，即：org_order要移动dst_order的位置，而两者之间的依次前移或者后移
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			//获取org_order
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &org_order);
			strTmp.ReleaseBuffer();
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			//获取dst_order
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &dst_order);
			strTmp.ReleaseBuffer();
			//移动order
			if(!Cmd_MoveOrder(org_order, dst_order)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			sprintf(buf, "%s", "OK||\r\n");
			m_peer.Send(CString(buf));
			break;
		case CMD_MOVEORDERPREV://将order对应的记录前移一位
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			//获取order
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &order);
			strTmp.ReleaseBuffer();
			//前移一位
			if(!Cmd_MoveOrderPrev(order)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			sprintf(buf, "%s", "OK||\r\n");
			m_peer.Send(CString(buf));
			break;
		case CMD_MOVEORDERNEXT://将order对应的记录后移一位
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			//获取order
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%d", &order);
			strTmp.ReleaseBuffer();
			//后移一位
			if(!Cmd_MoveOrderNext(order)){
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			sprintf(buf, "%s", "OK||\r\n");
			m_peer.Send(CString(buf));
			break;
		case CMD_SEARCHBYSCANCODEID://按扫描码查询记录
			if(!p){
				m_peer.Send(CString("ER||\r\n"));
				return -1;
			}
			//获取扫描码并查询
			strTmp = g_strList.GetNext(p);
			sscanf(strTmp.GetBuffer(strTmp.GetLength()), "%s", buf);
			strTmp.ReleaseBuffer();
			pID = new int[1024];
			if(!Cmd_SearchByScancodeID(buf, pID, num)){
				delete []pID;
				m_peer.Send(CString("ER||\r\n"));
				break;
			}
			//将ID信息组装成返回的字符串
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

///客户端端口连接，关闭本对话框
LRESULT CChatThread::OnDisconnected(WPARAM wParam, LPARAM lParam)
{
	CDialog::OnOK();
	return 0;
}

///初始化对话框
BOOL CChatThread::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowPos(&wndNoTopMost,0,0,0,0,SWP_HIDEWINDOW);  //设置显示方式为左上且没有长宽，实际是不显示。
	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);  //设置在任务栏中不显示
	SetWindowText("");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CChatThread::OnClose() 
{
	m_peer.Disconnect();
	
	CDialog::OnClose();
}
