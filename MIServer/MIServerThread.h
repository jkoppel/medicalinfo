#ifndef _MISERVERTHREAD_H_
#define _MISERVERTHREAD_H_

#include "StdAfx.h"
#include "MySocket.h"
#include "MIServer.h"

///服务器进程，从Dialog派生是因为要使用它的消息函数
class CMIServerThread : public CDialog
{
public:
	CMIServerThread(CWnd* pParent = NULL);
	~CMIServerThread();

	afx_msg LRESULT OnAccept(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReceiveData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDisconnected(WPARAM wParam, LPARAM lParam);

	BOOL StartServer();
	void StopServer();

	//{{AFX_DATA(CMIServerThread)
	enum { IDD = IDD_SERVERTHREAD };
	//}}AFX_DATA

private:
	CString	m_sPort;//端口
	CString	m_sRecv;//接收缓冲
	CString	m_sSend;//发送缓冲

protected:
	int m_nServerType;//服务器类型，TCP/UDP
	int m_nPort;//端口
	CMySocket *m_pSocketServer;

	//{{AFX_MSG(CMIServerThread)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
