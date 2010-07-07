// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "EDP.h"

#include "MainFrm.h"
#include "LeftView.h"
#include "FormViewEx.h"
#include "EDPView.h"
#include "DlgSettingDir.h"
#include "GlobalFuncs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_SETTINGS, &CMainFrame::OnSettings)
	ON_WM_DESTROY()
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
	//TestMime();
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

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
}

void CMainFrame::OnSettings()
{
	/*
	CEDPView *pView1 = (CEDPView*)m_wndSplitter2.GetPane(0, 1);
	CDC *pDC = pView1->GetDC();
	CBitmap bm;
	RECT rect;

	pView1->GetClientRect(&rect);
	bm.CreateCompatibleBitmap(pDC, rect.right-rect.left, rect.bottom-rect.top);
	CDC tdc; 
	tdc.CreateCompatibleDC(pDC); 
	CBitmap*pOld=tdc.SelectObject(&bm); 
	tdc.BitBlt(rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top,pDC,0,0,SRCCOPY);//截取屏幕数据bmp到bm里 
	CImage *image=new CImage; 
	image->Attach(bm); 
	image->Save("a.jpg");//bmp数据转换成jpeg保存到文件
	*/
	/*
	CWindowDC dc(NULL);  
	CBitmap bm;  
	CRect rect(0,0,::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN));    

	int Width=rect.Width(); 
	int Height=rect.Height(); 
	bm.CreateCompatibleBitmap(&dc,Width,Height); 
	CDC tdc; 
	tdc.CreateCompatibleDC(&dc); 
	CBitmap*pOld=tdc.SelectObject(&bm); 
	tdc.BitBlt(0,0,Width,Height,&dc,0,0,SRCCOPY);//截取屏幕数据bmp到bm里 
	CImage *image=new CImage; 
	image->Attach(bm); 
	image->Save("a.jpg");//bmp数据转换成jpeg保存到文件
	*///屏幕截图源码

	/*
	CBitmap *pBmp = pDC->GetCurrentBitmap();
	CImage imgTemp;
	imgTemp.Attach(pDC);
	imgTemp.Save(CString("a.jpg"));
	*/

	CDlgSettingDir dlg;
	if(dlg.DoModal()==IDOK){
		CLeftView *pView = (CLeftView*)m_wndSplitter2.GetPane(0, 0);
		pView->InitTree();
		CEDPView *pView1 = (CEDPView*)m_wndSplitter2.GetPane(0, 1);
		pView1->RedrawWindow();
	}
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

void CMainFrame::OnDestroy()
{
	CFrameWnd::OnDestroy();

	ReleaseDirNode();
}
