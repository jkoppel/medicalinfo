#if !defined(AFX_ADDOPTIONS_H__67DAD480_8F39_11D5_B47F_977833C60F2E__INCLUDED_)
#define AFX_ADDOPTIONS_H__67DAD480_8F39_11D5_B47F_977833C60F2E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddOptions.h : header file
//
#include "NewWizPage.h"

/////////////////////////////////////////////////////////////////////////////
// CAddOptions dialog

class CAddOptions : public CNewWizPage//CDialog
{
// Construction
public:
	CAddOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddOptions)
	enum { IDD = IDD_ADDOPTIONS };
	CSliderCtrl	m_slider;
	BOOL	m_full;
	int		m_compression;
	BOOL	m_upass;
	//}}AFX_DATA
	virtual LRESULT OnWizardBack();
	virtual BOOL OnWizardFinish();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddOptions)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddOptions)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDOPTIONS_H__67DAD480_8F39_11D5_B47F_977833C60F2E__INCLUDED_)
