#include "stdafx.h"
#include "MIServerThread.h"
#include "ChatThread.h"
#include "GlobalFuncs.h"
#include "CfgFile.h"

#define USE_UDL_FILE		1
#define CONFIG_FILE			"Configuration.ini"

///构造函数
CMIServerThread::CMIServerThread(CWnd* pParent)
	: CDialog(CMIServerThread::IDD, pParent)
{
	//默认设为TCP连接，端口5000
	m_nServerType = SOCK_STREAM;

	//从配置文件获取端口号
	CfgFile cf;
	cf.OpenFile(CONFIG_FILE);
	if(!cf.GetVarInt("PDAPORT", "Port", m_nPort)){
		m_nPort = 5000;
	}
	cf.CloseFile();
}

BEGIN_MESSAGE_MAP(CMIServerThread, CDialog)
	//{{AFX_MSG_MAP(CMIServerThread)
	ON_MESSAGE(ON_RECEIVE, OnReceiveData)
	ON_MESSAGE(ON_CLOSE, OnDisconnected)
	ON_MESSAGE(ON_ACCEPT, OnAccept)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///析构函数
CMIServerThread::~CMIServerThread()
{
	//释放资源
	delete m_pSocketServer;
	m_pSocketServer = NULL;
}

///初始化对话框
BOOL CMIServerThread::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowPos(&wndNoTopMost,0,0,0,0,SWP_HIDEWINDOW);  //设置显示方式为左上角，且不显示。
	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);  //设置在任务栏中不显示
	this->SetWindowText("");

	//启动服务器
	m_pSocketServer = new CMySocket(this);
	StartServer();

	// 初始化COM,创建ADO连接等操作
	AfxOleInit();
	g_pDBConnection.CreateInstance(__uuidof(Connection));
#ifdef	USE_UDL_FILE
	g_pDBConnection->ConnectionString = "File Name=miserver.udl";
#endif
	
	// 在ADO操作中建议语句中要常用try...catch()来捕获错误信息，
	// 因为它有时会经常出现一些意想不到的错误。jingzhou xu
	try                 
	{	
#ifdef USE_UDL_FILE
		g_pDBConnection->Open("","","",NULL);
#else
		g_pDBConnection->Open("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=CaseData.mdb","","",adModeUnknown);
#endif
	}
	catch(_com_error e)
	{
		Printf("数据库连接失败，确认miserver.udl是否在当前路径下!");
		g_pDBConnection.Release();
		return FALSE;
	}
	g_bIsDBConnected = TRUE;

	return TRUE;
}

///接收到远程客户端连接
LRESULT CMIServerThread::OnAccept(WPARAM wParam, LPARAM lParam)
{
	CChatThread *chatDlg;
	
	chatDlg = new CChatThread;
	chatDlg->ConnectTo((SOCKET) wParam);
	chatDlg->DoModal();

	return 0;
}

///接收到数据
LRESULT CMIServerThread::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

///远程客户端断开连接
LRESULT CMIServerThread::OnDisconnected(WPARAM wParam, LPARAM lParam)
{
	if(m_pSocketServer->GetSocketType() == SOCK_DGRAM)
	{
		m_sRecv = "Remote host disconnected";
		return 0;
	}
	return 0;
}

///启动服务器，开始接收
BOOL CMIServerThread::StartServer()
{
	BOOL ret;

	ret = m_pSocketServer->Create(m_nServerType);
	if(!ret){
		return ret;
	}
	ret = m_pSocketServer->Accept(m_nPort);
	if(!ret){
		m_pSocketServer->Disconnect();
		return ret;
	}

	return TRUE;
}

///关闭服务器
void CMIServerThread::StopServer()
{
	m_pSocketServer->Disconnect();
}

///关闭对话框
void CMIServerThread::OnClose() 
{
	if(g_bIsDBConnected){
		if(g_pDBConnection->State)
			g_pDBConnection->Close();
		g_pDBConnection= NULL;
	}
	g_bIsDBConnected = FALSE;
	
	CDialog::OnClose();
}
