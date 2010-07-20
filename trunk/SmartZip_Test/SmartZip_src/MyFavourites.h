#if !defined(AFX_MYFAVOURITES_H__2FD669BF_A78B_45F9_9C67_CCF32CD703E0__INCLUDED_)
#define AFX_MYFAVOURITES_H__2FD669BF_A78B_45F9_9C67_CCF32CD703E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyFavourites.h : header file
//
#include "RegistrySerialize.h"
#include "BtnST.h"

/////////////////////////////////////////////////////////////////////////////
// CMyFavourites dialog

class CMyFavourites : public CDialog
{
// Construction
public:
	void AddToFavo(CString& s);
	CMyFavourites(CWnd* pParent = NULL);   // standard constructor
~CMyFavourites();
// Dialog Data
	//{{AFX_DATA(CMyFavourites)
	enum { IDD = IDD_MYFAVORITES };
	CButtonST	m_delete;
	CButtonST	m_close;
	CButtonST	m_open;
	CListCtrl	m_list;
	//}}AFX_DATA

CStringArray filesarr;
	CRegistrySerialize m_reg;	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyFavourites)
	public:
	virtual void Serialize(CArchive& ar);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMyFavourites)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYFAVOURITES_H__2FD669BF_A78B_45F9_9C67_CCF32CD703E0__INCLUDED_)
