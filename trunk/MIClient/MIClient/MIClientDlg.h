// MIClientDlg.h : 头文件
//

#pragma once

#include "ClientSocket.h"

// CMIClientDlg 对话框
class CMIClientDlg : public CDialog
{
// 构造
public:
	CMIClientDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CMIClientDlg();

// 对话框数据
	enum { IDD = IDD_MICLIENT_DIALOG };
	CButton	m_btTCP;
	CString	m_sAddress;
	CString	m_sRecv;
	CString	m_sSend;
	CString	m_sPort;


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	void Send();
	afx_msg LRESULT OnReceive(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDisconnect(WPARAM wParam, LPARAM lParam);

	HICON m_hIcon;
	CClientSocket *m_ClientSocket;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	virtual void OnOK();
	afx_msg void OnExit();
	afx_msg void OnConnect();
	DECLARE_MESSAGE_MAP()
};
