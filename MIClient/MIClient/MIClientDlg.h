// MIClientDlg.h : 头文件
//

#pragma once

#include "TCPClient_CE.h"

// CMIClientDlg 对话框
class CMIClientDlg : public CDialog
{
// 构造
public:
	CMIClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MICLIENT_DIALOG };
	CString	m_remoteHost;
	int		m_remotePort;
	CString	m_recvData;
	CString	m_sendData;


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	afx_msg void OnBtnconn();
	afx_msg void OnBtndisconn();
	afx_msg void OnBtnsenddata();
	DECLARE_MESSAGE_MAP()

private:
	//定义CTCPClient_CE对象
	CTCPClient_CE m_tcpClient;
private:
	//连接断开事件处理函数
	static void CALLBACK OnDisConnect(CWnd* pWnd);
	//当有数据接收事件处理函数
	static void CALLBACK OnRead(CWnd* pWnd,const char * buf,int len );
	//Socket错误事件处理函数
	static void CALLBACK OnError(CWnd* pWnd,int nErrorCode);
private:
	//得到本地的IP地址
	CString GetLocalIP();

};
