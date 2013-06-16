
// LeftView.h : interface of the CLeftView class
//


#pragma once

#include "ImageProcess/BaseDocument.h"

#define MAX_BASE_DOCUMENT_NUM   128

class CPdfClimateDoc;
class CXHtmlTree;

class CLeftView : public CView//CTreeView
{
protected: // create from serialization only
    CLeftView();
    DECLARE_DYNCREATE(CLeftView)

// Attributes
public:
    CPdfClimateDoc* GetDocument();
    void openFile();
    void closeFile();
    void selectFile(int index);
    int getCurrDocIndex();
    void updateView();

// Operations
public:

// Overrides
    public:
    protected:
    virtual void OnInitialUpdate(); // called first time after construct
    virtual void OnDraw(CDC* pDC);      // overridden to draw this view

// Implementation
public:
    virtual ~CLeftView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    CXHtmlTree *m_pTreeCtrl;
    CImageList m_ilColorList;
    int m_iBaseDocumentNum;
    CBaseDocument *m_pBaseDocumentList[MAX_BASE_DOCUMENT_NUM];
    int m_iCurrDocumentIndex;

// Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
};

#ifndef _DEBUG  // debug version in LeftView.cpp
inline CPdfClimateDoc* CLeftView::GetDocument()
   { return reinterpret_cast<CPdfClimateDoc*>(m_pDocument); }
#endif

