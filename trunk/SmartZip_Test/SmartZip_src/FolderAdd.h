#if !defined(AFX_FOLDERADD_H__FBA9BFDA_E3D8_4126_89A7_9F38C6CDC558__INCLUDED_)
#define AFX_FOLDERADD_H__FBA9BFDA_E3D8_4126_89A7_9F38C6CDC558__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FolderAdd.h : header file
//
#include "IEShellTreeCtrl.h"
#include "IEShellComboBox.h"
#include "NewWizPage.h"

/////////////////////////////////////////////////////////////////////////////
// CFolderAdd dialog

class CFolderAdd : public CNewWizPage//CDialog
{
// Construction
public:
	CFolderAdd(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFolderAdd)
	enum { IDD = IDD_ADDFOLDER };
	CIEShellComboBox	m_cbShell;
	CIEShellTreeCtrl	m_shelltree;
	//}}AFX_DATA
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	CString	m_path;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFolderAdd)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFolderAdd)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FOLDERADD_H__FBA9BFDA_E3D8_4126_89A7_9F38C6CDC558__INCLUDED_)
