#if !defined(AFX_LEFTVIEW_H__E9C523C8_0E0F_43A1_BD9A_2DB5723514C6__INCLUDED_)
#define AFX_LEFTVIEW_H__E9C523C8_0E0F_43A1_BD9A_2DB5723514C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LeftView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLeftTreeView view

#include   <afxcview.h>

class CLeftTreeView : public CView
{
protected:
	CLeftTreeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CLeftTreeView)

// Attributes
public:

// Operations
public:
	void InitTree(BOOL bReloadMode=FALSE);
	void SetMultiSelectMode(BOOL bMultiSelectMode);
	BOOL GetMultiSelectMode();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLeftTreeView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CLeftTreeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	CImageList m_ilDataFile;	//图标列表
	BOOL m_bMultiSelectMode;	//选中模式：多文件模式或单文件模式
	HTREEITEM m_pCurrFileItem;	//单文件模式下当前选中的文件结点

	//{{AFX_MSG(CLeftTreeView)
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnCheckbox(WPARAM, LPARAM);
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEFTVIEW_H__E9C523C8_0E0F_43A1_BD9A_2DB5723514C6__INCLUDED_)
