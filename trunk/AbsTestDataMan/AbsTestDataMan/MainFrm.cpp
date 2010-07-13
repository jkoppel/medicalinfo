// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "AbsTestDataMan.h"

#include "MainFrm.h"
#include "FilterFormView.h"
#include "LeftTreeView.h"
#include "RightDrawAreaView.h"
#include "DlgDirSetup.h"
#include "GlobalFuncs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///��ʾ��Ϣ����
int MyMessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption, UINT nType)
{
	return AfxGetApp()->GetMainWnd()->GetActiveWindow()->MessageBox(lpszText, lpszCaption, nType);
}

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_DIR_SETUP, &CMainFrame::OnDirSetup)
	ON_WM_DESTROY()
	ON_WM_MENUSELECT()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
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
		TRACE0("δ�ܴ���������\n");
		return -1;      // δ�ܴ���
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}
	//����һ����ʾ
	m_wndStatusBar.AddIndicator(0, INDICATOR_LOGO);
	int idx = m_wndStatusBar.CommandToIndex(INDICATOR_LOGO);
	m_wndStatusBar.SetPaneWidth(idx,120);
	m_wndStatusBar.SetPaneStyle(idx, m_wndStatusBar.GetPaneStyle(idx) | SBPS_NOBORDERS );

	// Create a log pane window, and append it to status bar
	MPCLogoPane * pLogo = new MPCLogoPane;
	pLogo->Create(_T("�廪����ϵ"),WS_CHILD|WS_VISIBLE,&m_wndStatusBar,120);
	pLogo->SetLogoFont(_T("Arial"), 18);
	m_wndStatusBar.AddControl(pLogo,INDICATOR_LOGO);

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return TRUE;
}


// CMainFrame ���

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


// CMainFrame ��Ϣ�������




BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	if(!m_wndSplitter.CreateStatic(this,2,1)){
		return FALSE;
	}
	m_wndSplitter.CreateView(0,0,RUNTIME_CLASS(CFilterFormView),CSize(500,60),pContext);
	m_wndSplitter2.CreateStatic(&m_wndSplitter,1,2,WS_CHILD|WS_VISIBLE,m_wndSplitter.IdFromRowCol(1, 0));
	m_wndSplitter2.CreateView(0,0,RUNTIME_CLASS(CLeftTreeView),CSize(200,100),pContext);
	m_wndSplitter2.CreateView(0,1,RUNTIME_CLASS(CRightDrawAreaView),CSize(100,100),pContext);
	SetActiveView((CView*)m_wndSplitter.GetPane(0,0));

	m_wndSplitter.LockBar();

	return TRUE;
}


void CMainFrame::OnDirSetup()
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
	tdc.BitBlt(rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top,pDC,0,0,SRCCOPY);//��ȡ��Ļ����bmp��bm�� 
	CImage *image=new CImage; 
	image->Attach(bm); 
	image->Save("a.jpg");//bmp����ת����jpeg���浽�ļ�
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
	tdc.BitBlt(0,0,Width,Height,&dc,0,0,SRCCOPY);//��ȡ��Ļ����bmp��bm�� 
	CImage *image=new CImage; 
	image->Attach(bm); 
	image->Save("a.jpg");//bmp����ת����jpeg���浽�ļ�
	*///��Ļ��ͼԴ��

	/*
	CBitmap *pBmp = pDC->GetCurrentBitmap();
	CImage imgTemp;
	imgTemp.Attach(pDC);
	imgTemp.Save(CString("a.jpg"));
	*/

	CDlgDirSetup dlg;
	if(dlg.DoModal()==IDOK){
		CLeftTreeView *pView = (CLeftTreeView*)m_wndSplitter2.GetPane(0, 0);
		pView->InitTree(TRUE);
		CRightDrawAreaView *pView1 = (CRightDrawAreaView*)m_wndSplitter2.GetPane(0, 1);
		pView1->RedrawWindow();
	}
}

void CMainFrame::OnDestroy()
{
	CFrameWnd::OnDestroy();
}

void CMainFrame::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	CFrameWnd::OnMenuSelect(nItemID, nFlags, hSysMenu);

	m_wndStatusBar.RemovePane(INDICATOR_LOGO);
}
