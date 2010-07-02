// MIServerDlg.h : header file
//

#if !defined(AFX_MISERVERDLG_H__0B2C27BD_9CBA_4469_A29F_24D935E989A1__INCLUDED_)
#define AFX_MISERVERDLG_H__0B2C27BD_9CBA_4469_A29F_24D935E989A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MIBase.h"
#include "MySocket.h"
#include "Chat.h"
#include <math.h>
#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CMIServerDlg dialog

class CMIServerDlg : public CDialog
{
// Construction
public:
	CMIServerDlg(CWnd* pParent = NULL);	// standard constructor

	afx_msg LRESULT OnAccept(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReceiveData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDisconnected(WPARAM wParam, LPARAM lParam);

// Dialog Data
	//{{AFX_DATA(CMIServerDlg)
	enum { IDD = IDD_MISERVER_DIALOG };
	CButton	m_bServerType;
	CString	m_sRecv;
	CString	m_sSend;
	CString	m_sPort;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMIServerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CMySocket *m_pSocketServer;

	// Generated message map functions
	//{{AFX_MSG(CMIServerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStartServer();
	afx_msg void OnCloseServer();
	virtual void OnOK();
	afx_msg void OnExit();
	afx_msg void OnTestdb();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MISERVERDLG_H__0B2C27BD_9CBA_4469_A29F_24D935E989A1__INCLUDED_)
