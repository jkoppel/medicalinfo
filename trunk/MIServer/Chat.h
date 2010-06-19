#if !defined(AFX_CHAT_H__364E035B_9C88_488F_AAA5_BCA7DD48E782__INCLUDED_)
#define AFX_CHAT_H__364E035B_9C88_488F_AAA5_BCA7DD48E782__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Chat.h : header file
//

#include "MySocket.h"

/////////////////////////////////////////////////////////////////////////////
// CChat dialog

class CChat : public CDialog
{
// Construction
public:
	CChat(CWnd* pParent = NULL);   // standard constructor

	void ConnectTo(SOCKET s);
	afx_msg LRESULT OnReceiveData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDisconnected(WPARAM wParam, LPARAM lParam);

// Dialog Data
	//{{AFX_DATA(CChat)
	enum { IDD = IDD_CHAT };
	CString	m_sRecv;
	CString	m_sSend;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CMySocket m_peer;

	// Generated message map functions
	//{{AFX_MSG(CChat)
	afx_msg void OnExit();
	virtual void OnOK();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHAT_H__364E035B_9C88_488F_AAA5_BCA7DD48E782__INCLUDED_)
