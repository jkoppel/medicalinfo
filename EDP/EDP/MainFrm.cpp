// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "EDP.h"

#include "MainFrm.h"
#include "LeftView.h"
#include "FormViewEx.h"
#include "EDPView.h"
#include "DlgSettingDir.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_SETTINGS, &CMainFrame::OnSettings)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


int MyMessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption, UINT nType)
{
	return AfxGetApp()->GetMainWnd()->GetActiveWindow()->MessageBox(lpszText, lpszCaption, nType);
}


// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	/*
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}
	*/

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	// TODO: 如果不需要工具栏可停靠，则删除这三行
	/*
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	*/

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}


// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 消息处理程序




BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	if(!m_wndSplitter.CreateStatic(this,2,1)){
		return FALSE;
	}
	m_wndSplitter.CreateView(0,0,RUNTIME_CLASS(CFormViewEx),CSize(500,60),pContext);
	m_wndSplitter2.CreateStatic(&m_wndSplitter,1,2,WS_CHILD|WS_VISIBLE,m_wndSplitter.IdFromRowCol(1, 0));
	m_wndSplitter2.CreateView(0,0,RUNTIME_CLASS(CLeftView),CSize(200,100),pContext);
	m_wndSplitter2.CreateView(0,1,RUNTIME_CLASS(CEDPView),CSize(100,100),pContext);
	SetActiveView((CView*)m_wndSplitter.GetPane(0,0));

	m_wndSplitter.LockBar();
	return TRUE;

	/*
	if(!m_wndSplitter.CreateStatic (this,1,2)){
		return FALSE;
	}
	if(!m_wndSplitter.CreateView(0,0,RUNTIME_CLASS(CLeftView),CSize(200,100),pContext)||
		!m_wndSplitter.CreateView (0,1,RUNTIME_CLASS(CEDPView),CSize(100,100),pContext)){
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}
	return TRUE;
	*/

	//return CFrameWnd::OnCreateClient(lpcs, pContext);//the org oncreateclient content
}

void CMainFrame::OnSettings()
{
	CDlgSettingDir dlg;
	dlg.DoModal();
	return;
}

BOOL PeekAndPump()
{
  static MSG msg;

  while(::PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
  {
	  if(!AfxGetApp()->PumpMessage())
    {
		  ::PostQuitMessage(0);
		  return FALSE;
	  }	
  }

  return TRUE;
}
