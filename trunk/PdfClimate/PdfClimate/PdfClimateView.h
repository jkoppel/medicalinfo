
// PdfClimateView.h : interface of the CPdfClimateView class
//


#pragma once

class CPdfClimateView : public CView
{
protected: // create from serialization only
    CPdfClimateView();
    DECLARE_DYNCREATE(CPdfClimateView)

// Attributes
public:
    CPdfClimateDoc* GetDocument() const;


// Operations
public:
    bool m_bFileOpened;
    int m_pageNum;
    int m_pageIndex;

    void openFile();
    void gotoFirstPage();
    void gotoLastPage();
    void gotoNextPage();
    void gotoPrevPage();

// Overrides
public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
    virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
    virtual ~CPdfClimateView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图

protected:

// Generated message map functions
protected:
    afx_msg void OnFilePrintPreview();
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // debug version in PdfClimateView.cpp
inline CPdfClimateDoc* CPdfClimateView::GetDocument() const
   { return reinterpret_cast<CPdfClimateDoc*>(m_pDocument); }
#endif

