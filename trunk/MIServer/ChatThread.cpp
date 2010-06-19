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
	ON_WM_SHOWWINDOW()
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
	while(m_peer.GetDataSize() > 0 && m_peer.ReadString(m_sRecv)){
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

	//���ö�ʱ�������ش���
	this->SetTimer(100, 1, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

///��ʾ���ڣ����ظú�����Ϊ��ʹ���ڲ����ػ�
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

///�ڴ���û�б��ػ�������£��ö�ʱ������������
void CChatThread::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==100){
		this->KillTimer(nIDEvent);
		this->ShowWindow(SW_HIDE);
	}
	
	CDialog::OnTimer(nIDEvent);
}
