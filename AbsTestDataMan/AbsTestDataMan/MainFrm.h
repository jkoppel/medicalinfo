
// MainFrm.h : CMainFrame ��Ľӿ�
//

#pragma once
#include "ExtLibs\\MySplitterWndEx\\MySplitterWndEx.h"
#include "MFCStatusBarEx.h"
#include "MFCMenuBarEx.h"

class CMainFrame : public CFrameWndEx
{
	
protected: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // �ؼ���Ƕ���Ա
	CMFCMenuBarEx	  m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBarEx   m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	CMySplitterWndEx m_wndSplitter1;
	CMySplitterWndEx m_wndSplitter2; 

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnDirSetup();
	afx_msg void OnModeSingleFile();
	afx_msg void OnModeMultiFile();
	afx_msg void OnModeNormalCurve();
	afx_msg void OnModeFilterCurve();
	afx_msg void OnModeAllCurve();
	afx_msg void OnDrawAreaSetup();
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
};


