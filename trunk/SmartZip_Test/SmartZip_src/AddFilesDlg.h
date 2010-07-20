#if !defined(AFX_ADDFILESDLG_H__9B608F53_9CA6_4BD1_96FB_793DA3A0A549__INCLUDED_)
#define AFX_ADDFILESDLG_H__9B608F53_9CA6_4BD1_96FB_793DA3A0A549__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddFilesDlg.h : header file
//
#include "NewWizPage.h"
#include "BtnST.h"

/////////////////////////////////////////////////////////////////////////////
// CAddFilesDlg dialog

class CAddFilesDlg : public CNewWizPage//CDialog
{
// Construction
public:
	CAddFilesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddFilesDlg)
	enum { IDD = IDD_ADDFILES };
	CButtonST	m_remove;
	CButtonST	m_add;
	CListCtrl	m_fileslist;
	CString	m_nof;
	//}}AFX_DATA
	virtual LRESULT OnWizardNext();
CStringArray filesarr;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddFilesDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddFilesDlg)
	afx_msg void OnAddfiles();
	afx_msg void OnRemoveflist();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDFILESDLG_H__9B608F53_9CA6_4BD1_96FB_793DA3A0A549__INCLUDED_)
