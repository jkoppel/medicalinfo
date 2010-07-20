#if !defined(AFX_BAROPERATION_H__A4D89C96_B36C_4DE4_A2AF_FFD970F62A4E__INCLUDED_)
#define AFX_BAROPERATION_H__A4D89C96_B36C_4DE4_A2AF_FFD970F62A4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BarOperation.h : header file
//
//#include "smartzipdoc.h"
#include "GuiOutLookView.h"
#include "GuiOutLook.h"

/////////////////////////////////////////////////////////////////////////////
// CBarOperation window

class  CBarOperation : public CGuiOutLookView  //CWnd
{
	DECLARE_DYNCREATE(CBarOperation)
// Construction
public:
	CBarOperation();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBarOperation)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBarOperation();
	CGuiOutLook Folder,File,Archive;
	// Generated message map functions
protected:
	//{{AFX_MSG(CBarOperation)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFavourites();
	afx_msg void OnTcDelete();
	afx_msg void OnTcExtract();
	afx_msg void OnTcProperties();
	afx_msg void OnTcView();
	afx_msg void OnTcAdd();
	afx_msg void OnMoveArchive();
	afx_msg void OnCopyArchive();
	afx_msg void OnRenameArchive();
	afx_msg void OnDeleteArchive();
	afx_msg void OnAddToFavourites();
	afx_msg void OnTestArchive();
	afx_msg void OnCreateSFX();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BAROPERATION_H__A4D89C96_B36C_4DE4_A2AF_FFD970F62A4E__INCLUDED_)
