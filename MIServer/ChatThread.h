#if !defined(AFX_CHATTHREAD_H__364E035B_9C88_488F_AAA5_BCA7DD48E782__INCLUDED_)
#define AFX_CHATTHREAD_H__364E035B_9C88_488F_AAA5_BCA7DD48E782__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Chat.h : header file
//

#include "MySocket.h"

/////////////////////////////////////////////////////////////////////////////
// CChatThread dialog

class CChatThread : public CDialog
{
// Construction
public:
	CChatThread(CWnd* pParent = NULL);   // standard constructor

	void ConnectTo(SOCKET s);
	afx_msg LRESULT OnReceiveData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDisconnected(WPARAM wParam, LPARAM lParam);

// Dialog Data
	//{{AFX_DATA(CChatThread)
	enum { IDD = IDD_CHATTHREAD };
	CString	m_sRecv;
	CString	m_sSend;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChatThread)
	protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CMySocket m_peer;
	void SendError(int ErrNo);

	// Generated message map functions
	//{{AFX_MSG(CChatThread)
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHAT_H__364E035B_9C88_488F_AAA5_BCA7DD48E782__INCLUDED_)
