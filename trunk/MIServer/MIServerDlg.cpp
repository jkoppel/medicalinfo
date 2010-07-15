// MIServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MIServer.h"
#include "MIServerDlg.h"
#include "GlobalFuncs.h"
#include "PatientListDlg.h"
#include "CfgFile.h"

#define CONFIG_FILE			"Configuration.ini"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///本对话框类弹出实际的对话框，主要供调试用，实际服务器运行的时候不使用

/////////////////////////////////////////////////////////////////////////////
// CMIServerDlg dialog

CMIServerDlg::CMIServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMIServerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMIServerDlg)
	m_sRecv = _T("");
	m_sSend = _T("");
	m_sPort = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	g_bIsDBConnected = FALSE;
	g_pDBConnection = NULL;

	CfgFile cf;
	cf.OpenFile(CONFIG_FILE);
	char sPort[256];
	if(cf.GetVarStr("PDAPORT", "Port", sPort)){
		m_sPort = sPort;
	}
	cf.CloseFile();
}

void CMIServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMIServerDlg)
	DDX_Control(pDX, IDC_SERVTCP, m_bServerType);
	DDX_Text(pDX, IDC_RECV, m_sRecv);
	DDX_Text(pDX, IDC_SEND, m_sSend);
	DDX_Text(pDX, IDC_PORT, m_sPort);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMIServerDlg, CDialog)
	//{{AFX_MSG_MAP(CMIServerDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, OnStartServer)
	ON_BN_CLICKED(IDC_CLOSE, OnCloseServer)
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	ON_BN_CLICKED(IDC_TESTDB, OnTestdb)
	ON_MESSAGE(ON_RECEIVE, OnReceiveData)
	ON_MESSAGE(ON_CLOSE, OnDisconnected)
	ON_MESSAGE(ON_ACCEPT, OnAccept)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMIServerDlg message handlers

#define USE_UDL_FILE	1

BOOL CMIServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_pSocketServer = new CMySocket(this);
	m_bServerType.SetCheck(1);
	m_sPort = _T("5000");
	UpdateData(FALSE);

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
		AfxMessageBox("数据库连接失败，确认miserver.udl是否在当前路径下!");
		g_pDBConnection.Release();
		return FALSE;
	}

	g_bIsDBConnected = TRUE;
	//GetDlgItem(IDC_TESTDB)->EnableWindow(TRUE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMIServerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMIServerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

LRESULT CMIServerDlg::OnAccept(WPARAM wParam, LPARAM lParam)
{
	CChat *chatDlg;
	
	chatDlg = new CChat;
	chatDlg->ConnectTo((SOCKET) wParam);
	chatDlg->DoModal();

	return 0;
}

LRESULT CMIServerDlg::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	while(m_pSocketServer->GetDataSize() > 0 && m_pSocketServer->ReadString(m_sRecv))
		UpdateData(FALSE);
	return 0;
}

LRESULT CMIServerDlg::OnDisconnected(WPARAM wParam, LPARAM lParam)
{
	if(m_pSocketServer->GetSocketType() == SOCK_DGRAM)
	{
		m_sRecv = "Remote host disconnected";
		UpdateData(FALSE);
		return 0;
	}
	GetDlgItem(IDC_START)->EnableWindow(TRUE);
	return 0;
}

void CMIServerDlg::OnStartServer() 
{
	BOOL created;
	UpdateData();

	if(m_bServerType.GetCheck())
		created = m_pSocketServer->Create(SOCK_STREAM);
	else
		created = m_pSocketServer->Create(SOCK_DGRAM);

	if(created)
		if(m_pSocketServer->Accept(atoi(m_sPort)))
			GetDlgItem(IDC_START)->EnableWindow(FALSE);
}

void CMIServerDlg::OnCloseServer() 
{
	m_pSocketServer->Disconnect();
	GetDlgItem(IDC_START)->EnableWindow(TRUE);
}

void CMIServerDlg::OnOK() 
{
	int len;
	UpdateData();
	m_sSend += "\r\n";
	len = m_sSend.GetLength();
	m_pSocketServer->Send(m_sSend, len);
}

void CMIServerDlg::OnExit() 
{
	PostQuitMessage(0);	
}

void CMIServerDlg::OnTestdb() 
{
	if(g_bIsDBConnected){
		CPatientListDlg dlg;
		dlg.DoModal();
	}
}

void CMIServerDlg::OnClose() 
{
	if(g_bIsDBConnected){
		if(g_pDBConnection->State)
			g_pDBConnection->Close();
		g_pDBConnection= NULL;
	}
	g_bIsDBConnected = FALSE;
	
	CDialog::OnClose();
}
