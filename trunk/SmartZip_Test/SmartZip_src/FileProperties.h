#if !defined(AFX_FILEPROPERTIES_H__EBDD1FA6_71DD_4417_AF69_8E99D5E78364__INCLUDED_)
#define AFX_FILEPROPERTIES_H__EBDD1FA6_71DD_4417_AF69_8E99D5E78364__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileProperties.h : header file
//
#include "ZipFileHeader.h"
#include "BtnST.h"
/////////////////////////////////////////////////////////////////////////////
// CFileProperties dialog

class CFileProperties : public CDialog
{
// Construction
public:
	void DisableFolderInfo();
	void DisableFileProp();
	CFileProperties(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFileProperties)
	enum { IDD = IDD_FILEPROP };
	CButtonST	m_close;
	CButtonST	m_ok;
	CEdit	m_comment;
	CStatic	m_fileicon;
	CString	m_attrib;
	CString	m_comratio;
	CString	m_comsize;
	CString	m_crc;
	CString	m_filename;
	CString	m_moddate;
	CString	m_type;
	CString	m_ucomsize;
	CString	m_contains;
	//}}AFX_DATA
	CString comment;
HICON icon;
CString m_strtitle;
BOOL bDir;
CZipFileHeader* fh;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileProperties)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFileProperties)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEPROPERTIES_H__EBDD1FA6_71DD_4417_AF69_8E99D5E78364__INCLUDED_)
