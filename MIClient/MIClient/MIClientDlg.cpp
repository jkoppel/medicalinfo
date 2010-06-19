// MIClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MIClient.h"
#include "MIClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMIClientDlg 对话框

CMIClientDlg::CMIClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMIClientDlg::IDD, pParent)
{
	m_sAddress = _T("");
	m_sRecv = _T("");
	m_sSend = _T("");
	m_sPort = _T("");

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_ClientSocket = NULL;
}

CMIClientDlg::~CMIClientDlg()
{
	if(m_ClientSocket)
		delete m_ClientSocket;

	CDialog::~CDialog();
}

void CMIClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMIClientDlg)
	DDX_Control(pDX, IDC_TCP, m_btTCP);
	DDX_Text(pDX, IDC_ADDRESS, m_sAddress);
	DDX_Text(pDX, IDC_NETRECV, m_sRecv);
	DDX_Text(pDX, IDC_NETSEND, m_sSend);
	DDX_Text(pDX, IDC_PORT, m_sPort);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMIClientDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	ON_BN_CLICKED(IDCANCEL, OnExit)
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_MESSAGE(SOCK_ONRECEIVE, OnReceive)
	ON_MESSAGE(SOCK_ONCLOSE, OnDisconnect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CMIClientDlg 消息处理程序

BOOL CMIClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_ClientSocket = new CClientSocket(this);
	m_ClientSocket->SetEolFormat(CCESocket::EOL_CR);
	m_btTCP.SetCheck(1);
	m_sAddress = "192.168.0.11";
	m_sPort = "5000";
	UpdateData(FALSE);
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CMIClientDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_MICLIENT_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_MICLIENT_DIALOG));
	}
}
#endif


void CMIClientDlg::Send()
{
	UpdateData();
	m_ClientSocket->SendLine(m_sSend);
}

LRESULT CMIClientDlg::OnReceive(WPARAM wParam, LPARAM lParam)
{
	CString str, temp;

	while(m_ClientSocket->GetDataSize() > 0 && m_ClientSocket->ReadString(temp))
		str += L" "+temp;

	GetDlgItem(IDC_NETRECV)->SetWindowText(str);

	return 0;
}

LRESULT CMIClientDlg::OnDisconnect(WPARAM wParam, LPARAM lParam)
{
	GetDlgItem(IDC_NETRECV)->SetWindowText(L"Disconnected!");
	GetDlgItem(IDC_CONNECT)->SetWindowText(L"Connect");
	return 0;
}

void CMIClientDlg::OnOK() 
{
	if(m_ClientSocket->GetSocketState() > CCESocket::CREATED)
		Send();
}

void CMIClientDlg::OnExit() 
{
	CDialog::OnOK();	
}

void CMIClientDlg::OnConnect() 
{
	int sockType;
	UINT port;

	//If it's already connected: disconnect.
	if(m_ClientSocket->GetSocketState() > CCESocket::CREATED)
	{
		m_ClientSocket->Disconnect();
		GetDlgItem(IDC_CONNECT)->SetWindowText(L"Connect");
		return;
	}

	UpdateData();

	if(m_btTCP.GetCheck() == 1)
		sockType = SOCK_STREAM;
	else
		sockType = SOCK_DGRAM;

	//此处要将双字节转换成单字节
	char sPort[255];
	ZeroMemory(sPort,255);
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,m_sPort,wcslen(m_sPort)
	,sPort,wcslen(m_sPort),NULL,NULL);
	port = atoi(sPort);

	if(m_ClientSocket->Create(sockType))
		if(m_ClientSocket->Connect(m_sAddress, port))
		{
			GetDlgItem(IDC_CONNECT)->SetWindowText(L"Disconnect");
			return;
		}
		
	m_ClientSocket->Disconnect();
}
