#if !defined(AFX_VERIPASS_H__F55B715B_9761_4EED_95F5_04240D08C5E4__INCLUDED_)
#define AFX_VERIPASS_H__F55B715B_9761_4EED_95F5_04240D08C5E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VeriPass.h : header file
//
#include "BtnST.h"

/////////////////////////////////////////////////////////////////////////////
// CVeriPass dialog

class CVeriPass : public CDialog
{
// Construction
public:
	CVeriPass(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVeriPass)
	enum { IDD = IDD_VERIPASS };
	CButtonST	m_ok;
	CButtonST	m_close;
	CString	m_pass;
	CString	m_filename;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVeriPass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVeriPass)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VERIPASS_H__F55B715B_9761_4EED_95F5_04240D08C5E4__INCLUDED_)
