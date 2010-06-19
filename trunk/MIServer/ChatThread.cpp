// Chat.cpp : implementation file
//

#include "stdafx.h"
#include "MIServer.h"
#include "ChatThread.h"

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
	ON_WM_SHOWWINDOW()
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
	while(m_peer.GetDataSize() > 0 && m_peer.ReadString(m_sRecv)){
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

	//设置定时器，隐藏窗口
	this->SetTimer(100, 1, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

///显示窗口，重载该函数是为了使窗口不被重画
void CChatThread::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	if(GetStyle() & WS_VISIBLE){ 
		CDialog::OnShowWindow(bShow, nStatus); 
	}
	else{ 
		long Style = ::GetWindowLong(*this, GWL_STYLE); 
		::SetWindowLong(*this, GWL_STYLE, Style | WS_VISIBLE); 
		CDialog::OnShowWindow(SW_HIDE, nStatus); 
	}	
}

///在窗口没有被重画的情况下，用定时器将窗口隐藏
void CChatThread::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==100){
		this->KillTimer(nIDEvent);
		this->ShowWindow(SW_HIDE);
	}
	
	CDialog::OnTimer(nIDEvent);
}
