// MIClientDlg.h : ͷ�ļ�
//

#pragma once

#include "TCPClient_CE.h"

// CMIClientDlg �Ի���
class CMIClientDlg : public CDialog
{
// ����
public:
	CMIClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MICLIENT_DIALOG };
	CString	m_remoteHost;
	int		m_remotePort;
	CString	m_recvData;
	CString	m_sendData;


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	afx_msg void OnBtnconn();
	afx_msg void OnBtndisconn();
	afx_msg void OnBtnsenddata();
	DECLARE_MESSAGE_MAP()

private:
	//����CTCPClient_CE����
	CTCPClient_CE m_tcpClient;
private:
	//���ӶϿ��¼�������
	static void CALLBACK OnDisConnect(CWnd* pWnd);
	//�������ݽ����¼�������
	static void CALLBACK OnRead(CWnd* pWnd,const char * buf,int len );
	//Socket�����¼�������
	static void CALLBACK OnError(CWnd* pWnd,int nErrorCode);
private:
	//�õ����ص�IP��ַ
	CString GetLocalIP();

};
