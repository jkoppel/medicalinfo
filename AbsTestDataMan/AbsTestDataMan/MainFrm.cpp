
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "AbsTestDataMan.h"

#include "MainFrm.h"
#include "FilterFormView.h"
#include "LeftTreeView.h"
#include "RightDrawAreaView.h"
#include "DlgDirSetup.h"
#include "GlobalFuncs.h"
//#include "BitmapEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_DIR_SETUP, &CMainFrame::OnDirSetup)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/*
//begin: test image creation, CBitmapEx
const DWORD STANDARD_PALETTE[] = {00,51,102,153,204,255};
const INT STANDARD_COLOR_SIZE = 6;
const INT STANDARD_PALETTE_VAL_DIF = 51;
static DWORD dwColorMapTable[216] = {0};

void SetMapTable()
{
	int  nColorMapIdx = 0;
	for (int nBlueIdx = 0; nBlueIdx < STANDARD_COLOR_SIZE; ++nBlueIdx)
	{
		for(int nGreenIdx = 0; nGreenIdx < STANDARD_COLOR_SIZE; ++nGreenIdx)
		{
			for(int nRedIdx = 0; nRedIdx < STANDARD_COLOR_SIZE; ++nRedIdx)
			{
				RGBQUAD objColor;
				objColor.rgbRed    = STANDARD_PALETTE[nRedIdx];
				objColor.rgbGreen = STANDARD_PALETTE[nGreenIdx];
				objColor.rgbBlue = STANDARD_PALETTE[nBlueIdx];
				objColor.rgbReserved = 0;
				memcpy(&dwColorMapTable[nColorMapIdx],&objColor,sizeof(RGBQUAD));
				++nColorMapIdx;
			}
		}
	}
}

UINT GetPixelValue(DWORD uPixelValue_i)
{
    UINT uRetValue = 0;
    UINT uPos = uPixelValue_i / STANDARD_PALETTE_VAL_DIF;
    if(0 == uPixelValue_i % STANDARD_PALETTE_VAL_DIF)
    {
        uRetValue = uPixelValue_i/STANDARD_PALETTE_VAL_DIF;
    }
    else
    {
        if(abs(uPixelValue_i - STANDARD_PALETTE [uPos]) > 
           abs(uPixelValue_i - STANDARD_PALETTE [uPos+1]))
        {
            uRetValue = uPos+1;
        }
        else
        {
            uRetValue = uPos;
        }
    }
    return uRetValue;
}

void ConvertFile()
{
	TCHAR tcInputFileName[] = _T("c:\\32BitImage.bmp");
	CBitmapEx InputImage;
	InputImage.Load(tcInputFileName);
	INT nImageHeight = InputImage .GetHeight();
	INT nImageWidth = InputImage .GetWidth();
	INT nPixelSize = nImageHeight * nImageWidth;
	BYTE byBitsPerPixel = 32; // Input image Bits per pixel.

	BYTE *pixels = new BYTE[nPixelSize];
	for(UINT nRow = 0; nRow < nImageHeight; ++nRow)
	{
		for(UINT nCol = 0; nCol < nImageWidth; ++nCol)
		{
			DWORD dwPixelVal;
			UINT i8bppPixel = nRow * nImageWidth + nCol;
			// Get pixel data from Input image.

			dwPixelVal = InputImage.GetPixel(nCol,nRow) & 0xFFFFFF;
			// Get RGB value from color data.

			int nRed = dwPixelVal >> 16;
			int nGreen = (dwPixelVal & 0x00FF00) >> 8 ;
			int nBlue = dwPixelVal& 0x0000FF;
			// Get Index of suitable color data in the palette table.

			UINT uRedValue = GetPixelValue(nRed);
			UINT uGreenValue = GetPixelValue(nGreen);
			UINT uBlueValue = GetPixelValue(nBlue);

			// Calculate Pixel color position

			// in the color map table using RGB values. 

			// Finally set this index in to the pixel data.

			UINT uPalettePos = uBlueValue*36+uGreenValue*6+uRedValue;
			pixels[i8bppPixel] =(BYTE)uPalettePos;
		}
	}
}

void CreateDataFileIcons()
{
	SetMapTable();

	CBitmapEx bitmap;
	bitmap.Load(_T("E:\\XBMC\\state_images.bmp"));
	bitmap.Scale2(16 * 216, 15);
	for(int y=0;y<15;y++){
		for(int x=0;x<216;x++){
			for(int i=0;i<16;i++){
				if(i>=2 && i<=13 && y>=1 && y<=12){
					bitmap.SetPixel(x*16+i, y, dwColorMapTable[x]);
				}
				else{
					bitmap.SetPixel(x*16+i, y, 0xFFFFFF);
				}
			}
		}
	}
	bitmap.Save(_T("E:\\XBMC\\s1.bmp"));
}
//end: test image creation, CBitmapEx
*/

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	extern void TestStringMatch();
	TestStringMatch();

	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2005);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;
	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("未能创建菜单栏\n");
		return -1;      // 未能创建
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 防止菜单栏在激活时获得焦点
	//CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// 允许用户定义的工具栏操作:
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	CBitmap bmpIcon;
	bmpIcon.LoadBitmap(IDB_BITMAP_TSINGHUA);
	m_wndStatusBar.SetPaneIcon(0, bmpIcon);
	m_wndStatusBar.SetPaneText(0, _T("清华大学汽车系研制 2010.7"));

	// TODO: 如果您不希望工具栏和菜单栏可停靠，请删除这五行
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);
	
	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 启用工具栏和停靠窗口菜单替换
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 启用快速(按住 Alt 拖动)工具栏自定义
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// 加载用户定义的工具栏图像
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			m_UserImages.SetImageSize(CSize(16, 16), FALSE);
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// 启用菜单个性化(最近使用的命令)
	// TODO: 定义您自己的基本命令，确保每个下拉菜单至少有一个基本命令。
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 扫描菜单*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 基类将执行真正的工作

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// 为所有用户工具栏启用自定义按钮
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	if(!m_wndSplitter1.CreateStatic(this,2,1)){
		return FALSE;
	}
	m_wndSplitter1.CreateView(0,0,RUNTIME_CLASS(CFilterFormView),CSize(500,50),pContext);
	m_wndSplitter2.CreateStatic(&m_wndSplitter1,1,2,WS_CHILD|WS_VISIBLE,m_wndSplitter1.IdFromRowCol(1, 0));
	m_wndSplitter2.CreateView(0,0,RUNTIME_CLASS(CLeftTreeView),CSize(200,100),pContext);
	m_wndSplitter2.CreateView(0,1,RUNTIME_CLASS(CRightDrawAreaView),CSize(100,100),pContext);

	SetActiveView((CView*)m_wndSplitter1.GetPane(0,0));

	m_wndSplitter1.LockBar();

	return CFrameWndEx::OnCreateClient(lpcs, pContext);
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

	CDlgDirSetup dlg;
	if(dlg.DoModal()==IDOK){
		CLeftTreeView *pView = (CLeftTreeView*)m_wndSplitter2.GetPane(0, 0);
		pView->InitTree(TRUE);
		CRightDrawAreaView *pView1 = (CRightDrawAreaView*)m_wndSplitter2.GetPane(0, 1);
		pView1->RedrawWindow();
	}
}
