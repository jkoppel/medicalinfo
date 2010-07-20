// SmartZipView.h : interface of the CSmartZipView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SMARTZIPVIEW_H__984F701D_25DD_4EE2_BA0F_C93D03C09001__INCLUDED_)
#define AFX_SMARTZIPVIEW_H__984F701D_25DD_4EE2_BA0F_C93D03C09001__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "smartzipdoc.h"
#define NCOLS 9


class CSmartZipView : public CListView
{
		struct LISTDATA
	{
		int zipindex;CString name;
	};

protected: // create from serialization only
	CSmartZipView();
	DECLARE_DYNCREATE(CSmartZipView)

// Attributes
public:
	CSmartZipDoc* GetDocument();
static int CALLBACK CompareCountries(LPARAM lParam1, LPARAM lParam2, 
                                      LPARAM lParamSort);
	void ConstructHeader();
	void FreeItemMemory();
	static CString FormatSize(DWORD iSize);

// Operations
public:
	static int m_iColums[NCOLS];
	CImageList m_imgList;
	CBitmap		bitmap;
// Operations
public:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmartZipView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	void Selectinverse();
	void SelectAll(BOOL bSelect);
	virtual ~CSmartZipView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSmartZipView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnDblclk(NMITEMACTIVATE* pNMHDR, LRESULT* pResult);
	afx_msg void OnHeaderEnddrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHeaderItemclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTcExtract();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnHdnItemClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTcDelete();
	afx_msg void OnTcProperties();
	afx_msg void OnTcAdd();
	afx_msg void OnTcView();
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SmartZipView.cpp
inline CSmartZipDoc* CSmartZipView::GetDocument()
   { return (CSmartZipDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMARTZIPVIEW_H__984F701D_25DD_4EE2_BA0F_C93D03C09001__INCLUDED_)
