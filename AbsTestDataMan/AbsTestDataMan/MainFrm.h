
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "ExtLibs\\MySplitterWndEx\\MySplitterWndEx.h"
#include "MFCStatusBarEx.h"
#include "MFCMenuBarEx.h"

class CMainFrame : public CFrameWndEx
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 属性
public:

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CMFCMenuBarEx	  m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBarEx   m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	CMySplitterWndEx m_wndSplitter1;
	CMySplitterWndEx m_wndSplitter2; 

// 生成的消息映射函数
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


