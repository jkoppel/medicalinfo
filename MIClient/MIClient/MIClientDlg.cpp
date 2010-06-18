// MIClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MIClient.h"
#include "MIClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMIClientDlg �Ի���

CMIClientDlg::CMIClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMIClientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMIClientDlg)
	m_remoteHost = _T("");
	m_remotePort = 0;
	m_recvData = _T("");
	m_sendData = _T("");
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMIClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMIClientDlg)
	DDX_Text(pDX, IDC_EDTREMOTEHOST, m_remoteHost);
	DDX_Text(pDX, IDC_EDTREMOTEPORT, m_remotePort);
	DDX_Text(pDX, IDC_EDTRECV, m_recvData);
	DDX_Text(pDX, IDC_EDTSENDDATA, m_sendData);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMIClientDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
ON_BN_CLICKED(IDC_BTNCONN, OnBtnconn)
ON_BN_CLICKED(IDC_BTNDISCONN, OnBtndisconn)
ON_BN_CLICKED(IDC_BTNSENDDATA, OnBtnsenddata)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CMIClientDlg ��Ϣ�������

BOOL CMIClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	CenterWindow(GetDesktopWindow());	// center to the hpc screen
	
	//��ʼ������ֵ
	m_remoteHost = GetLocalIP();
	m_remotePort = 5000;
	UpdateData(FALSE);
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

//���ӶϿ��¼�
void CALLBACK CMIClientDlg::OnDisConnect(CWnd* pWnd)
{
	CMIClientDlg * pDlg = (CMIClientDlg*)pWnd;
	
	CStatic * pStatus = (CStatic *)pDlg->GetDlgItem(IDC_LBLCONNSTATUS);
	ASSERT(pStatus != NULL);
	pStatus->SetWindowText(_T("���ӶϿ�"));
	
	/*���ð�ť��������*/
	CButton * pBtnConn =(CButton*)pDlg->GetDlgItem(IDC_BTNCONN);
	CButton * pBtnDisConn = (CButton*)pDlg->GetDlgItem(IDC_BTNDISCONN);
	CButton * pBtnSendData =  (CButton*)pDlg->GetDlgItem(IDC_BTNSENDDATA);
	ASSERT(pBtnConn != NULL );
	ASSERT(pBtnDisConn != NULL );
	ASSERT(pBtnSendData != NULL );
	pBtnConn->EnableWindow(TRUE);
	pBtnDisConn->EnableWindow(FALSE);
	pBtnSendData->EnableWindow(FALSE);
}

//���ݽ����¼�
void CALLBACK CMIClientDlg::OnRead(CWnd* pWnd,const char * buf,int len )
{
	CMIClientDlg * pDlg = (CMIClientDlg*)pWnd;
	CEdit * pEdtRecv = (CEdit*)pDlg->GetDlgItem(IDC_EDTRECV);
	ASSERT(pEdtRecv != NULL);
	CString strRecv = CString(buf);
	//�����յ�������ʾ�������ı�����
	pEdtRecv->SetWindowText(strRecv);
}

//Socket�����¼�
void CALLBACK CMIClientDlg::OnError(CWnd* pWnd,int nErrorCode)
{
	AfxMessageBox(_T("�ͻ���socket��������"));
}


//�������Ӱ�ť
void CMIClientDlg::OnBtnconn() 
{
	UpdateData(TRUE);
	//����m_tcpClient����
	m_tcpClient.m_remoteHost = m_remoteHost;
	m_tcpClient.m_port = m_remotePort;
	m_tcpClient.OnDisConnect = OnDisConnect;
	m_tcpClient.OnRead = OnRead;
	m_tcpClient.OnError = OnError;
	//�򿪿ͻ���socket
	m_tcpClient.Open(this);
	
	//�����������������
	if (m_tcpClient.Connect())
	{
		CStatic *pStatus = (CStatic*)GetDlgItem(IDC_LBLCONNSTATUS);
		ASSERT(pStatus != NULL);
		pStatus->SetWindowText(L"��������");
		UpdateData(FALSE);
	}
	else
	{
		AfxMessageBox(_T("��������ʧ��"));
		CStatic *pStatus = (CStatic*)GetDlgItem(IDC_LBLCONNSTATUS);
		ASSERT(pStatus != NULL);
		pStatus->SetWindowText(L"���ӶϿ�");
		return;
	}
	/*���ð�ť��������*/
	CButton * pBtnConn =(CButton*)GetDlgItem(IDC_BTNCONN);
	CButton * pBtnDisConn = (CButton*)GetDlgItem(IDC_BTNDISCONN);
	CButton * pBtnSendData =  (CButton*)GetDlgItem(IDC_BTNSENDDATA);
	ASSERT(pBtnConn != NULL );
	ASSERT(pBtnDisConn != NULL );
	ASSERT(pBtnSendData != NULL );
	pBtnConn->EnableWindow(FALSE);
	pBtnDisConn->EnableWindow(TRUE);
	pBtnSendData->EnableWindow(TRUE);
	
}

//�Ͽ����Ӱ�ť�����¼�
void CMIClientDlg::OnBtndisconn() 
{
	if (m_tcpClient.Close())
	{
		CStatic *pStatus = (CStatic*)GetDlgItem(IDC_LBLCONNSTATUS);
		ASSERT(pStatus != NULL);
		pStatus->SetWindowText(L"���ӶϿ�");
		CButton * pBtnConn =(CButton*)GetDlgItem(IDC_BTNCONN);
		CButton * pBtnDisConn = (CButton*)GetDlgItem(IDC_BTNDISCONN);
		CButton * pBtnSendData = (CButton*)GetDlgItem(IDC_BTNSENDDATA);
		ASSERT(pBtnConn != NULL );
		ASSERT(pBtnDisConn != NULL );
		ASSERT(pBtnSendData != NULL );
		pBtnConn->EnableWindow(TRUE);
		pBtnDisConn->EnableWindow(FALSE);
		pBtnSendData->EnableWindow(FALSE);
	}
	else
	{
		AfxMessageBox(_T("���ӶϿ�ʧ��"));
	}	
}

//�������ݰ�ť�����¼�����
void CMIClientDlg::OnBtnsenddata() 
{
	char * sendBuf;
	int sendLen=0;
	UpdateData(TRUE);
	
	sendLen = m_sendData.GetLength();
	sendBuf = new char[sendLen*2];
	wcstombs(sendBuf,m_sendData,sendLen);
	if (!m_tcpClient.SendData(sendBuf,sendLen))
	{
		AfxMessageBox(_T("����ʧ��"));
	}
	delete[] sendBuf;
	sendBuf = NULL;		
}

//�õ����ص�IP��ַ
CString CMIClientDlg::GetLocalIP()
{
	
	HOSTENT *LocalAddress;
	char	*Buff;
	TCHAR	*wBuff;
	CString strReturn = _T("");
	
	
	//�����µĻ�����
	Buff = new char[256];
	wBuff = new TCHAR[256];
	//�ÿջ�����
	memset(Buff, '\0', 256);
	memset(wBuff, TEXT('\0'), 256*sizeof(TCHAR));
	//�õ����ؼ������
	if (gethostname(Buff, 256) == 0)
	{
		//ת����˫�ֽ��ַ���
		mbstowcs(wBuff, Buff, 256);
		//�õ����ص�ַ
		LocalAddress = gethostbyname(Buff);
		//�ÿ�buff
		memset(Buff, '\0', 256);
		//��ϱ���IP��ַ
		sprintf(Buff, "%d.%d.%d.%d\0", LocalAddress->h_addr_list[0][0] & 0xFF,
			LocalAddress->h_addr_list[0][1] & 0x00FF, LocalAddress->h_addr_list[0][2] & 0x0000FF, LocalAddress->h_addr_list[0][3] & 0x000000FF);
		//�ÿ�wBuff
		memset(wBuff, TEXT('\0'), 256*sizeof(TCHAR));
		//ת����˫�ֽ��ַ���
		mbstowcs(wBuff, Buff, 256);
        //���÷���ֵ
		strReturn = wBuff;
	}
	else
	{
	}
	
	//�ͷ�Buff������
	delete[] Buff;
	//�ͷ�wBuff������
	delete[] wBuff;
	return strReturn;
}
