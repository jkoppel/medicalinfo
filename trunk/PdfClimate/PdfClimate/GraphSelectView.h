
// PdfClimateView.h : interface of the CGraphSelectView class
//


#pragma once

class CLBTabCtrl;

class CGraphSelectView : public CView
{
protected: // create from serialization only
    CGraphSelectView();
    DECLARE_DYNCREATE(CGraphSelectView)

// Attributes
public:
    CPdfClimateDoc* GetDocument() const;


// Operations
public:

// Overrides
public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
    virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
    virtual ~CGraphSelectView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
    CLBTabCtrl *getTabCtrl() { return m_pTab; }

protected:
    int m_iCurrTabIndex;		//当前选中页面
	CLBTabCtrl *m_pTab;

// Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

#ifndef _DEBUG  // debug version in PdfClimateView.cpp
inline CPdfClimateDoc* CGraphSelectView::GetDocument() const
   { return reinterpret_cast<CPdfClimateDoc*>(m_pDocument); }
#endif

