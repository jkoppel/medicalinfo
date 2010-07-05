// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "EDP.h"

#include "MainFrm.h"
#include "LeftView.h"
#include "FormViewEx.h"
#include "EDPView.h"
#include "TreePropSheet\\TreePropSheet.h"
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
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO: �ڴ����ӳ�Ա��ʼ������
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
		TRACE0("δ�ܴ���������\n");
		return -1;      // δ�ܴ���
	}
	*/

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}

	// TODO: �������Ҫ��������ͣ������ɾ��������
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


// CMainFrame ��Ϣ��������




BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	if(!m_wndSplitter.CreateStatic(this,2,1)){
		return FALSE;
	}
	m_wndSplitter.CreateView(0,0,RUNTIME_CLASS(CFormViewEx),CSize(500,50),pContext);
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

	CPropertyPage page1(IDD_SETTING_DIR);
	//CDlgSettingDir page1;
	page1.m_psp.dwFlags&= ~PSP_HASHELP;
	// set images
	CImageList	DefaultImages, Images;
	DefaultImages.Create(IDB_BITMAP_SETTINGS, 16, 0, RGB(0x00, 0x80, 0x80));
	Images.Create(IDB_BITMAP_SETTINGS, 16, 0, RGB(0x00, 0x80, 0x80));
	
	CTreePropSheet::SetPageIcon(&page1, Images, 0);
	//CTreePropSheet::SetPageIcon(&wndPageIncoming, Images, 1);

	CTreePropSheet sht(_T("Preferences"));
	sht.m_psh.dwFlags&= ~PSH_HASHELP;
	sht.AddPage(&page1);

	sht.SetEmptyPageText(_T("Please select a child item of '%s'."));

	sht.SetTreeViewMode(TRUE, TRUE, TRUE);
	sht.SetTreeDefaultImages(&DefaultImages);
	sht.SetActivePage(&page1);

	sht.DoModal();

	sht.DestroyPageIcon(&page1);
	//sht.DestroyPageIcon(&wndPageIncoming);
	/*
	*/
}