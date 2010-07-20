#if !defined(AFX_FILEVIEW_H__782D2AF8_8108_4E47_8E04_FC1AF29E4987__INCLUDED_)
#define AFX_FILEVIEW_H__782D2AF8_8108_4E47_8E04_FC1AF29E4987__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileView.h : header file
//
#include "MyListCtrl.H"
#include "BtnST.h"

/////////////////////////////////////////////////////////////////////////////
// CFileView dialog

class CFileView : public CDialog
{
// Construction
public:
	CFileView(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFileView)
	enum { IDD = IDD_FILEVIEW };
	CButtonST	m_close;
	CMyListCtrl	m_list;
	//}}AFX_DATA
CZipMemFile memfile;
CFont m_Font;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFileView)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEVIEW_H__782D2AF8_8108_4E47_8E04_FC1AF29E4987__INCLUDED_)
