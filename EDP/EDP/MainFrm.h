// MainFrm.h : CMainFrame ��Ľӿ�
//


#pragma once

#include "SplitterWndEx\\SplitterWndEx.h"


class CMainFrame : public CFrameWnd
{
	
protected: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// ����
public:
	CSplitterWndEx m_wndSplitter;
	CSplitterWndEx m_wndSplitter2; 

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // �ؼ���Ƕ���Ա
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
public:
	afx_msg void OnSettings();
};

extern BOOL PeekAndPump();
extern int MyMessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption, UINT nType = MB_OK);
