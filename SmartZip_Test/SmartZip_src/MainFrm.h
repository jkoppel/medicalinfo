// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__A8E4C7E0_8CA0_484B_B081_5C4E8422C2B6__INCLUDED_)
#define AFX_MAINFRM_H__A8E4C7E0_8CA0_484B_B081_5C4E8422C2B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GuiSplitterWnd.h"
#include "GuiMiniSplitter.h"
#include "GuiFrameWnd.h"
#include "guisolexplorer.h"
#include "SmartZipView.h"
class CMainFrame : public CGuiFrameWnd//CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	CGuiSplitterWnd m_wndSplitter;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
	CSmartZipView* GetRightPane();
	afx_msg void OnUpdateViewStyles(CCmdUI* pCmdUI);
	afx_msg void OnViewStyle(UINT nCommandID);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // control bar embedded members
//	CStatusBar  m_wndStatusBar;
//	CToolBar    m_wndToolBar;
CGuiSolExplorer	 m_Server;
CImageList m_small;
CImageList m_large;

// Generated message map functions
public:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTest();
	afx_msg void OnCopyArchive();
	afx_msg void OnMoveArchive();
	afx_msg void OnUpdateArchiveOperation(CCmdUI* pCmdUI);
	afx_msg void OnDeleteArchive();
	afx_msg void OnRenameArchive();
	afx_msg void OnViewMybar();
	afx_msg void OnUpdateViewMybar(CCmdUI* pCmdUI);
	afx_msg void OnSelectall();
	afx_msg void OnSelectinverse();
	afx_msg void OnUnselectall();
	afx_msg void OnFileClose();
	afx_msg void OnUpdateTcAdd(CCmdUI* pCmdUI);
	afx_msg void OnActionExportHtml();
	afx_msg void OnToolsBackupwizard();
	afx_msg void OnToolsTimetoget();
	afx_msg void OnToolsCtreatesfx();
	afx_msg void OnChange();
	afx_msg void OnSaveComments();
	afx_msg void OnHelpHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__A8E4C7E0_8CA0_484B_B081_5C4E8422C2B6__INCLUDED_)
