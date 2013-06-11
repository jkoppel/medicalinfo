
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
class CLeftView;
class CPdfClimateView;

#include "CtrlLib/MySplitterWndEx/MySplitterWndEx.h"

class CMainFrame : public CFrameWndEx
{
    
protected: // create from serialization only
    CMainFrame();
    DECLARE_DYNCREATE(CMainFrame)

// Attributes
protected:
    CMySplitterWndEx m_wndSplitter1;
    CMySplitterWndEx m_wndSplitter2; 
    CMySplitterWndEx m_wndSplitter3; 
    CMySplitterWndEx m_wndSplitter4; 
    bool             m_bViewCreated;
    int              m_iCurSel;

public:

// Operations
public:

// Overrides
public:
    virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// Implementation
public:
    virtual ~CMainFrame();
    void setActiveGraphView(int index);
    void showDocInfo(bool bShow = true);

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
    CMFCMenuBar       m_wndMenuBar;
    CMFCToolBar       m_wndToolBar;
    CMFCStatusBar     m_wndStatusBar;
    CMFCToolBarImages m_UserImages;

// Generated message map functions
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnViewCustomize();
    afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
    afx_msg void OnApplicationLook(UINT id);
    afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnFileOpen();
    afx_msg void OnFileClose();
    afx_msg void OnSize(UINT nType, int cx, int cy);
};


