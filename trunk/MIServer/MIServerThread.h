#ifndef _MISERVERTHREAD_H_
#define _MISERVERTHREAD_H_

#include "StdAfx.h"
#include "MySocket.h"
#include "MIServer.h"

///���������̣���Dialog��������ΪҪʹ��������Ϣ����
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
	CString	m_sPort;//�˿�
	CString	m_sRecv;//���ջ���
	CString	m_sSend;//���ͻ���

protected:
	int m_nServerType;//���������ͣ�TCP/UDP
	int m_nPort;//�˿�
	CMySocket *m_pSocketServer;

	//{{AFX_MSG(CMIServerThread)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
