#if !defined(AFX_EXTRACTDLG_H__A3D4EC3A_1F1A_4994_B780_13B11F94BCBA__INCLUDED_)
#define AFX_EXTRACTDLG_H__A3D4EC3A_1F1A_4994_B780_13B11F94BCBA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExtractDlg.h : header file
//
#include "BtnST.h"

#include "NewWizPage.h"
/////////////////////////////////////////////////////////////////////////////
// CExtractDlg dialog

class CExtractDlg : public CNewWizPage//CDialog
{
// Construction
public:
	CExtractDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CExtractDlg)
	enum { IDD = IDD_FORMEXTRACT };
	CButtonST	m_browse;
	BOOL	m_bFullpath;
	CString	m_destination;
	BOOL	m_bOverride;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtractDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExtractDlg)
	afx_msg void OnBrowse();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTRACTDLG_H__A3D4EC3A_1F1A_4994_B780_13B11F94BCBA__INCLUDED_)
