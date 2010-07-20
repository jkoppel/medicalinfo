// ZipEXView.h : interface of the CZipEXView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZIPEXVIEW_H__66342E31_F368_4BFF_856C_3ED410A675B0__INCLUDED_)
#define AFX_ZIPEXVIEW_H__66342E31_F368_4BFF_856C_3ED410A675B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "smartzipdoc.h"
#include "treenode.h"


class CZipEXView : public CTreeView
{
public: // create from serialization only
	CZipEXView();
	DECLARE_DYNCREATE(CZipEXView)

// Attributes
public:
	CSmartZipDoc* GetDocument();

// Operations
public:
	CImageList m_imgList;
	CBitmap		bitmap;
	HTREEITEM hCurItem;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZipEXView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
//	void AddSubFolders(CZipWordArray *arr,CTreeNode*hold);
	HTREEITEM hSel;
	int m_filelength;
	CTreeNode* NodeFromItem(HTREEITEM hItem,CTreeNode *c);
//	void ParseStringToobj(CString &s,BOOL pFolder,int findex);
	virtual ~CZipEXView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
public:
	//{{AFX_MSG(CZipEXView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTcExtract();
	afx_msg void OnTcDelete();
	afx_msg void OnTcAdd();
	afx_msg void OnTcProperties();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#ifndef _DEBUG  // debug version in ZipEXView.cpp
inline CSmartZipDoc* CZipEXView::GetDocument()
   { return (CSmartZipDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZIPEXVIEW_H__66342E31_F368_4BFF_856C_3ED410A675B0__INCLUDED_)
