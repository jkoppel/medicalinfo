
// LeftView.h : interface of the CLeftView class
//


#pragma once

class CPdfClimateDoc;

class CLeftView : public CTreeView
{
protected: // create from serialization only
    CLeftView();
    DECLARE_DYNCREATE(CLeftView)

// Attributes
public:
    CPdfClimateDoc* GetDocument();
    void addFile(const char *file);

// Operations
public:

// Overrides
    public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    protected:
    virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
    virtual ~CLeftView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    CStringList m_strFileList;
    HTREEITEM hRoot;

// Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
};

#ifndef _DEBUG  // debug version in LeftView.cpp
inline CPdfClimateDoc* CLeftView::GetDocument()
   { return reinterpret_cast<CPdfClimateDoc*>(m_pDocument); }
#endif

