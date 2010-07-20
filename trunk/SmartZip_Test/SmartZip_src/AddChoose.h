#if !defined(AFX_ADDCHOOSE_H__4B013FBF_D397_467F_B916_DF1832D22337__INCLUDED_)
#define AFX_ADDCHOOSE_H__4B013FBF_D397_467F_B916_DF1832D22337__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddChoose.h : header file
//
#include "NewWizPage.h"

/////////////////////////////////////////////////////////////////////////////
// CAddChoose dialog

class CAddChoose : public CNewWizPage//CDialog
{
// Construction
public:
	CAddChoose(CWnd* pParent = NULL);   // standard constructor
	virtual LRESULT OnWizardNext();

// Dialog Data
	//{{AFX_DATA(CAddChoose)
	enum { IDD = IDD_ADDCHOOSE };
	int		m_choose;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddChoose)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddChoose)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDCHOOSE_H__4B013FBF_D397_467F_B916_DF1832D22337__INCLUDED_)
