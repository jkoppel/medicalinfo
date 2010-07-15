#include "stdafx.h"
#include "MIServerThread.h"
#include "ChatThread.h"
#include "GlobalFuncs.h"
#include "CfgFile.h"

#define USE_UDL_FILE		1
#define CONFIG_FILE			"Configuration.ini"

///���캯��
CMIServerThread::CMIServerThread(CWnd* pParent)
	: CDialog(CMIServerThread::IDD, pParent)
{
	//Ĭ����ΪTCP���ӣ��˿�5000
	m_nServerType = SOCK_STREAM;

	//�������ļ���ȡ�˿ں�
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

///��������
CMIServerThread::~CMIServerThread()
{
	//�ͷ���Դ
	delete m_pSocketServer;
	m_pSocketServer = NULL;
}

///��ʼ���Ի���
BOOL CMIServerThread::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowPos(&wndNoTopMost,0,0,0,0,SWP_HIDEWINDOW);  //������ʾ��ʽΪ���Ͻǣ��Ҳ���ʾ��
	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);  //�������������в���ʾ
	this->SetWindowText("");

	//����������
	m_pSocketServer = new CMySocket(this);
	StartServer();

	// ��ʼ��COM,����ADO���ӵȲ���
	AfxOleInit();
	g_pDBConnection.CreateInstance(__uuidof(Connection));
#ifdef	USE_UDL_FILE
	g_pDBConnection->ConnectionString = "File Name=miserver.udl";
#endif
	
	// ��ADO�����н��������Ҫ����try...catch()�����������Ϣ��
	// ��Ϊ����ʱ�ᾭ������һЩ���벻���Ĵ���jingzhou xu
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
		Printf("���ݿ�����ʧ�ܣ�ȷ��miserver.udl�Ƿ��ڵ�ǰ·����!");
		g_pDBConnection.Release();
		return FALSE;
	}
	g_bIsDBConnected = TRUE;

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

///�رշ�����
void CMIServerThread::StopServer()
{
	m_pSocketServer->Disconnect();
}

///�رնԻ���
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
