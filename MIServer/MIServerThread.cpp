#include "stdafx.h"
#include "MIServerThread.h"
#include "ChatThread.h"

///���캯��
CMIServerThread::CMIServerThread(CWnd* pParent)
	: CDialog(CMIServerThread::IDD, pParent)
{
	//Ĭ����ΪTCP���ӣ��˿�5000
	m_nServerType = SOCK_STREAM;
	m_nPort = 5000;
}

BEGIN_MESSAGE_MAP(CMIServerThread, CDialog)
	//{{AFX_MSG_MAP(CMIServerThread)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_MESSAGE(ON_RECEIVE, OnReceiveData)
	ON_MESSAGE(ON_CLOSE, OnDisconnected)
	ON_MESSAGE(ON_ACCEPT, OnAccept)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///��������
CMIServerThread::~CMIServerThread()
{
	//�ͷ���Դ
	delete m_pSocketServer;
	m_pSocketServer = NULL;
}

///��ʾ���ڣ����ظú�����Ϊ��ʹ���ڲ����ػ�
void CMIServerThread::OnShowWindow(BOOL bShow, UINT nStatus) 
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
void CMIServerThread::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==100){
		this->KillTimer(nIDEvent);
		this->ShowWindow(SW_HIDE);
	}
	
	if(nIDEvent==101){
		this->StartServer();
		this->KillTimer(nIDEvent);
	}

	CDialog::OnTimer(nIDEvent);
}

///��ʼ���Ի���
BOOL CMIServerThread::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pSocketServer = new CMySocket(this);

	//���ö�ʱ��
	this->SetTimer(100, 1, NULL);
	this->SetTimer(101, 2, NULL);

	return TRUE;
}

///���յ�Զ�̿ͻ�������
LRESULT CMIServerThread::OnAccept(WPARAM wParam, LPARAM lParam)
{
	CChatThread *chatDlg;
	
	chatDlg = new CChatThread;
	chatDlg->ConnectTo((SOCKET) wParam);
	chatDlg->DoModal();

	return 0;
}

///���յ�����
LRESULT CMIServerThread::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	while(m_pSocketServer->GetDataSize() > 0 && m_pSocketServer->ReadString(m_sRecv))
		UpdateData(FALSE);
	return 0;
}

///Զ�̿ͻ��˶Ͽ�����
LRESULT CMIServerThread::OnDisconnected(WPARAM wParam, LPARAM lParam)
{
	if(m_pSocketServer->GetSocketType() == SOCK_DGRAM)
	{
		m_sRecv = "Remote host disconnected";
		return 0;
	}
	return 0;
}

///��������������ʼ����
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

void CMIServerThread::StopServer()
{
	m_pSocketServer->Disconnect();
}

