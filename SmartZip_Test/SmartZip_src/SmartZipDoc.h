// SmartZipDoc.h : interface of the CSmartZipDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SMARTZIPDOC_H__47E44F45_E250_421E_8558_980D7C6E068B__INCLUDED_)
#define AFX_SMARTZIPDOC_H__47E44F45_E250_421E_8558_980D7C6E068B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ZipArchive.h"		//the archive lib
#include "ZipCollections.h"
#include "TreeNode.h"

class CSmartZipDoc : public CDocument
{
protected: // create from serialization only
	CSmartZipDoc();
	DECLARE_DYNCREATE(CSmartZipDoc)

// Attributes
public:
BOOL m_bOpenOK;
	CZipArchive m_zip;
	bool m_bAfterException;
	CTreeNode *tree,*currentnode;
//	CString m_filename;
	BOOL bStart;
	CString m_strfilename;
// Operations
public:
// MEMBER FUNC
	void MakeTree();
	BOOL OpenCurrentFile(LPCTSTR lpszPathName);
	void CloseCurrentFile();
	static bool RemoveFile(LPCTSTR szFile, bool bDisplay = false);
	BOOL OpenZipFile(CZipArchive &zip,LPCTSTR lpszZipFileName, long iMode, long iVolumeSize);
	void Close(CZipArchive &zip,BOOL bAfterException) ;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmartZipDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void OnCloseDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void DeleteContents();
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
	void ParseStringToobj(CString &s,BOOL pFolder,int findex);
	void AddSubFolders(CZipWordArray *arr,CTreeNode*hold);
	virtual ~CSmartZipDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSmartZipDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMARTZIPDOC_H__47E44F45_E250_421E_8558_980D7C6E068B__INCLUDED_)
