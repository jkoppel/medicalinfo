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
#include "BitmapEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///显示消息函数
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
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 构造/析构

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

CMainFrame::CMainFrame()
{
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
	//创建一个提示
	m_wndStatusBar.AddIndicator(0, INDICATOR_LOGO);
	int idx = m_wndStatusBar.CommandToIndex(INDICATOR_LOGO);
	m_wndStatusBar.SetPaneWidth(idx,120);
	m_wndStatusBar.SetPaneStyle(idx, m_wndStatusBar.GetPaneStyle(idx) | SBPS_NOBORDERS );

	// Create a log pane window, and append it to status bar
	m_wndStatusBar.ShowLogoPane();

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

void CMainFrame::OnDestroy()
{
	CFrameWnd::OnDestroy();
}

void CMainFrame::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	CFrameWnd::OnMenuSelect(nItemID, nFlags, hSysMenu);

	m_wndStatusBar.HideLogoPane();
}
