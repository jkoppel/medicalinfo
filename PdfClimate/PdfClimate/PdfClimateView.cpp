
// PdfClimateView.cpp : implementation of the CPdfClimateView class
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "PdfClimateDoc.h"
#include "PdfClimateView.h"
#include "MyPdf.h"
#include "CtrlLib/LBTabCtrl/LBTabCtrl.h"
#include "LeftView.h"
#include "GlobalVars.h"
#include "DottedGraphView.h"
#include "FacetGraphView.h"
#include "LinearGraphView.h"
#include "ColumnarGraphView.h"
#include "UnknownGraphView.h"
#include "GraphSelectView.h"
#include "MainFrm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPdfClimateView

IMPLEMENT_DYNCREATE(CPdfClimateView, CView)

BEGIN_MESSAGE_MAP(CPdfClimateView, CView)
    ON_WM_SIZE()
    ON_WM_CHAR()
    ON_WM_KEYDOWN()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_CREATE()
    ON_COMMAND(IDM_GOTO_FIRST_PAGE, &CPdfClimateView::OnPageFirst)
    ON_COMMAND(IDM_GOTO_LAST_PAGE, &CPdfClimateView::OnPageLast)
    ON_COMMAND(IDM_GOTO_PREV_PAGE, &CPdfClimateView::OnPagePrev)
    ON_COMMAND(IDM_GOTO_NEXT_PAGE, &CPdfClimateView::OnPageNext)
    ON_COMMAND(IDT_VIEW_ZOOMOUT, &CPdfClimateView::OnZoomOut)
    ON_COMMAND(IDT_VIEW_ZOOMIN, &CPdfClimateView::OnZoomIn)
    ON_COMMAND(IDD_DRAG_NORMAL, &CPdfClimateView::OnDragNormal)
    ON_COMMAND(IDD_DRAG_DOTTED, &CPdfClimateView::OnDragDotted)
    ON_COMMAND(IDD_DRAG_FACET, &CPdfClimateView::OnDragFacet)
    ON_COMMAND(IDD_DRAG_LINEAR, &CPdfClimateView::OnDragLinear)
    ON_COMMAND(IDD_DRAG_COLUMNAR, &CPdfClimateView::OnDragColumnar)
    ON_COMMAND(IDD_DRAG_UNKNOWN, &CPdfClimateView::OnDragUnknown)
END_MESSAGE_MAP()

// CPdfClimateView construction/destruction

CPdfClimateView::CPdfClimateView()
{
    m_bFileOpened = false;
    m_iPageNum = 0;
    m_iCurrPage = 0;
    m_iZoom = 100;
    m_iCX = 0;
    m_iCY = 0;

    m_iMode = Drag_Normal;
    m_ptOrig = CPoint(0, 0);
    m_bDragging = false;
    m_hCross = AfxGetApp()->LoadStandardCursor(IDC_CROSS);
}

CPdfClimateView::~CPdfClimateView()
{
}

BOOL CPdfClimateView::PreCreateWindow(CREATESTRUCT& cs)
{
    return CView::PreCreateWindow(cs);
}

void CPdfClimateView::OnInitialUpdate()
{
    CView::OnInitialUpdate();
}

void CPdfClimateView::OnContextMenu(CWnd* pWnd, CPoint point)
{
    theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CPdfClimateView diagnostics

#ifdef _DEBUG
void CPdfClimateView::AssertValid() const
{
    CView::AssertValid();
}

void CPdfClimateView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CPdfClimateDoc* CPdfClimateView::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPdfClimateDoc)));
    return (CPdfClimateDoc*)m_pDocument;
}
#endif //_DEBUG


void CPdfClimateView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);

    m_ctrlToolBar.AutoSize();
}


void CPdfClimateView::openFile()
{
    RECT rect;
    GetClientRect(&rect);

    m_iPageNum = mupdf_get_page_count();
    m_iCurrPage = 0;
    mupdf_load_page(m_iCurrPage, m_iZoom);
    int w, h;
    CRect crect;
    mupdf_get_size(&w, &h);
    GetClientRect(&crect);
    m_iCX = -(crect.Width() - w)/4;
    m_iCY = 80;
    m_iZoom = 100;
    m_bFileOpened = true;
    RedrawWindow();
    this->SetFocus();
}

void CPdfClimateView::closeFile()
{
    if (m_bFileOpened) {
        mupdf_close_file();
        m_iPageNum = 0;
        m_iCurrPage = 0;
        m_iCX = 0;
        m_iCY = 0;
        m_iZoom = 100;
        m_bFileOpened = false;
        RedrawWindow();
    }
}

void CPdfClimateView::OnDraw(CDC* /*pDC*/)
{
	CPdfClimateDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

    if (this->m_bFileOpened){
        CRect rect;

        GetClientRect(&rect);
        HDC hdc = ::GetDC(m_hWnd);
        if ( !(m_bDragging && m_iMode==Drag_Normal) ) {
            mupdf_winblit(hdc, rect.Width(), rect.Height(), m_iCX, m_iCY);;
        }
        else {
            mupdf_winblit(hdc, rect.Width(), rect.Height(), m_iCX + m_ptDest.x - m_ptOrig.x, m_iCY + m_ptDest.y - m_ptOrig.y);;
        }
        if (m_bDragging && m_iMode!=Drag_Normal) {
            CClientDC dc(this);
            HBRUSH hb = (HBRUSH) GetStockObject(NULL_BRUSH);
            CBrush* brush = CBrush::FromHandle(hb);
            CBrush *pOldBrush = dc.SelectObject(brush);
            dc.Rectangle(CRect(m_ptOrig, m_ptDest));
            dc.SelectObject(pOldBrush);
        }
    }
    else {
        CRect rect;
        CClientDC dc(this);
        GetClientRect(&rect);
        CBrush* brush = new CBrush(RGB(128, 128, 128));
        CBrush *pOldBrush = dc.SelectObject(brush);
        dc.Rectangle(rect);
        dc.SelectObject(pOldBrush);
    }
}

void CPdfClimateView::gotoFirstPage()
{
    if (m_bFileOpened) {
        if (m_iCurrPage != 1){
            m_iCurrPage = 0;
            mupdf_load_page(m_iCurrPage, m_iZoom);
            this->RedrawWindow();
        }
    }
}

void CPdfClimateView::gotoLastPage()
{
    if (m_bFileOpened) {
        if (m_iCurrPage != m_iPageNum-1){
            m_iCurrPage = m_iPageNum-1;
            mupdf_load_page(m_iCurrPage, m_iZoom);
            this->RedrawWindow();
        }
    }
}

void CPdfClimateView::gotoNextPage()
{
    if (m_bFileOpened) {
        if (m_iCurrPage != m_iPageNum-1){
            m_iCurrPage ++;
            mupdf_load_page(m_iCurrPage, m_iZoom);
            this->RedrawWindow();
        }
    }
}

void CPdfClimateView::zoomOut()
{
    if (m_bFileOpened) {
        if (m_iZoom > 5) {
            m_iZoom = (int)(m_iZoom / 1.1f);
            mupdf_load_page(m_iCurrPage, m_iZoom);
            RedrawWindow();
        }
    }
}

void CPdfClimateView::zoomIn()
{
    if (m_bFileOpened) {
        if (m_iZoom < 500) {
            if (m_iZoom < 10) {
                m_iZoom = (int)(m_iZoom * 1.2f);
            }
            else {
                m_iZoom = (int)(m_iZoom * 1.1f);
            }
            mupdf_load_page(m_iCurrPage, m_iZoom);
            RedrawWindow();
        }
    }
}

void CPdfClimateView::resume()
{
    if (m_bFileOpened) {
        m_iZoom = 100;
        m_iCX = 0;
        m_iCY = 0;
        mupdf_load_page(m_iCurrPage, m_iZoom);
        RedrawWindow();
    }
}

void CPdfClimateView::moveLeft()
{
    if (m_bFileOpened) {
        m_iCX -= 10;
        RedrawWindow();
    }
}

void CPdfClimateView::moveRight()
{
    if (m_bFileOpened) {
        m_iCX += 10;
        RedrawWindow();
    }
}

void CPdfClimateView::moveUp()
{
    if (m_bFileOpened) {
        m_iCY += 10;
        RedrawWindow();
    }
}

void CPdfClimateView::moveDown()
{
    if (m_bFileOpened) {
        m_iCY -= 10;
        RedrawWindow();
    }
}

void CPdfClimateView::gotoPrevPage()
{
    if (m_bFileOpened) {
        if (m_iCurrPage >0){
            m_iCurrPage --;
            mupdf_load_page(m_iCurrPage, m_iZoom);
            RedrawWindow();
        }
    }
}

void CPdfClimateView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    switch (nChar) {
        case '-':
        {
            zoomOut();
            break;
        }
        case '=':
        {
            zoomIn();
            break;
        }
        case 'r':
        {
            resume();
            break;
        }
        case 'c':
        {
            closeFile();
            break;
        }
        default:
        {
            break;
        }
    }

    CView::OnChar(nChar, nRepCnt, nFlags);
}

void CPdfClimateView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    switch (nChar) {
        case VK_NEXT:
        {
            gotoNextPage();
            break;
        }
        case VK_PRIOR:
        {
            gotoPrevPage();
            break;
        }
        case VK_LEFT:
        {
            moveLeft();
            break;
        }
        case VK_RIGHT:
        {
            moveRight();
            break;
        }
        case VK_DOWN:
        {
            moveDown();
            break;
        }
        case VK_UP:
        {
            moveUp();
            break;
        }
        case VK_HOME:
        {
            gotoFirstPage();
            break;
        }
        case VK_END:
        {
            gotoLastPage();
            break;
        }
        default:
        {
            break;
        }
    }

    CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CPdfClimateView::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (m_bFileOpened) {
	    SetCapture();
        if (m_iMode != Drag_Normal) {
    	    ::SetCursor(m_hCross);
        }

	    m_bDragging = true;
        m_ptOrig = point;
        m_ptDest = point;
        RedrawWindow();
    }

    CView::OnLButtonDown(nFlags, point);
}

void CPdfClimateView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDragging) {
        m_ptDest = point;

        RedrawWindow();
    }

    CView::OnMouseMove(nFlags, point);
}

void CPdfClimateView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDragging) {
        if (m_iMode!=Drag_Normal) {
	        CDC *pDC = GetDC();
            CBitmap *pBitmap;
            CImage *pImage;
            switch (m_iMode) {
            case Drag_Dotted:
                pBitmap = g_pDottedGraphView->getSrcBitmap();
                pImage= g_pDottedGraphView->getSrcImage();
                break;
            case Drag_Facet:
                pBitmap = g_pFacetGraphView->getSrcBitmap();
                pImage= g_pFacetGraphView->getSrcImage();
                break;
            case Drag_Linear:
                pBitmap = g_pLinearGraphView->getSrcBitmap();
                pImage = g_pLinearGraphView->getSrcImage();
                break;
            case Drag_Columnar:
                pBitmap = g_pColumnarGraphView->getSrcBitmap();
                pImage= g_pColumnarGraphView->getSrcImage();
                break;
            case Drag_Unknow:
                pBitmap = g_pUnknownGraphView->getSrcBitmap();
                pImage = g_pUnknownGraphView->getSrcImage();
                break;
            }

	        RECT rect;
            rect.left = m_ptOrig.x;
            rect.top = m_ptOrig.y;
            rect.right = point.x;
            rect.bottom = point.y;
            if (rect.left > rect.right) {
                int tmp = rect.right;
                rect.right = rect.left;
                rect.left = tmp;
            }
            if (rect.top > rect.bottom) {
                int tmp = rect.top;
                rect.top = rect.bottom;
                rect.bottom = tmp;
            }

            pBitmap->DeleteObject();
            pBitmap->CreateCompatibleBitmap(pDC, rect.right - rect.left, rect.bottom - rect.top);
	        CDC tdc;
	        tdc.CreateCompatibleDC(pDC);
	        CBitmap *pOld=tdc.SelectObject(pBitmap);
	        tdc.BitBlt(0, 0, rect.right - rect.left, rect.bottom - rect.top,
                        pDC, rect.left, rect.top, SRCCOPY);

            if (!pImage->IsNull()){
                pImage->Detach();
            }
	        pImage->Attach(*pBitmap);

            switch(m_iMode){
            case Drag_Dotted:
                g_pDottedGraphView->setSrcRect(rect);
                break;
            case Drag_Facet:
                g_pFacetGraphView->setSrcRect(rect);
                break;
            case Drag_Linear:
                g_pLinearGraphView->setSrcRect(rect);
                break;
            case Drag_Columnar:
                g_pColumnarGraphView->setSrcRect(rect);
                break;
            case Drag_Unknow:
                g_pUnknownGraphView->setSrcRect(rect);
                break;
            }
            ((CMainFrame*)AfxGetMainWnd())->setActiveGraphView((int)m_iMode);
            g_pGraphSelectView->getTabCtrl()->SetCurSel((int)m_iMode);
        }
        else {
            m_iCX += m_ptDest.x - m_ptOrig.x;
            m_iCY += m_ptDest.y - m_ptOrig.y;
        }

		m_bDragging = false;
		ReleaseCapture();
        m_iMode = Drag_Normal;
	}

    CView::OnLButtonUp(nFlags, point);
}

#include <afxdb.h>

void CPdfClimateView::testSQL()
{
    CDatabase   m_db;
    CRecordset rs;
    
    TRY
    {
        m_db.OpenEx(_T("DSN=TestSQL01;"),CDatabase::noOdbcDialog);
        rs.m_pDatabase = &m_db;
        CString sql = _T("USE PdfClimate INSERT BookInfo(ID, Name) VALUES(123, 'Bob')");
        m_db.ExecuteSQL(sql);
    }
    CATCH(CDBException,ex)
    {
        AfxMessageBox(ex->m_strError);
        AfxMessageBox(ex->m_strStateNativeOrigin);
    }
    AND_CATCH(CMemoryException,pEx)
    {
        pEx->ReportError();
        AfxMessageBox(_T("memory exception"));
    }
    AND_CATCH(CException,e)
    {
        TCHAR szError[100];
        e->GetErrorMessage(szError,100);
        AfxMessageBox(szError);
    }
    END_CATCH

   return;
}

struct ToolbarButtonInfo {
    /* index in the toolbar bitmap (-1 for separators) */
    int           bmpIndex;
    int           cmdId;
    int           flags;
};

static ToolbarButtonInfo gToolbarButtons[] = {
    { -1,  0,                     0 },
    { 4,   IDM_GOTO_FIRST_PAGE,      0},
    { 5,   IDM_GOTO_LAST_PAGE,       0 },
    { -1,  0,                     0 },
    { 0,   IDM_GOTO_PREV_PAGE,    0 },
    { 1,   IDM_GOTO_NEXT_PAGE,    0 },
    { -1,  0,                     0},
    { 2,   IDT_VIEW_ZOOMOUT,      0},
    { 3,   IDT_VIEW_ZOOMIN,       0 },
    { -1,  0,                     0 },
    { 6,   IDD_DRAG_NORMAL,       0 },
    { 7,   IDD_DRAG_DOTTED,       0 },
    { 8,   IDD_DRAG_FACET,       0 },
    { 9,   IDD_DRAG_LINEAR,       0 },
    { 10,   IDD_DRAG_COLUMNAR,       0 },
    { 11,   IDD_DRAG_UNKNOWN,       0 },
};

#define TOOLBAR_BUTTONS_COUNT (sizeof(gToolbarButtons)/sizeof(ToolbarButtonInfo))

inline bool TbIsSeparator(ToolbarButtonInfo& tbi)
{
    return tbi.bmpIndex < 0;
}

static TBBUTTON TbButtonFromButtonInfo(int i) {
    TBBUTTON tbButton = { 0 };
    tbButton.idCommand = gToolbarButtons[i].cmdId;
    if (TbIsSeparator(gToolbarButtons[i])) {
        tbButton.fsStyle = TBSTYLE_SEP;
    } else {
        tbButton.iBitmap = gToolbarButtons[i].bmpIndex;
        tbButton.fsState = TBSTATE_ENABLED;
        tbButton.fsStyle = TBSTYLE_BUTTON;
        tbButton.iString = gToolbarButtons[i].cmdId;
    }
    return tbButton;
}

#define WS_TOOLBAR (WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | \
                    TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | CCS_TOP | \
                    TBSTYLE_LIST | CCS_NODIVIDER | CCS_NOPARENTALIGN)

#define BUTTON_SIZE 16

int CPdfClimateView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    CBitmap bm, bm1;
    HBITMAP hbmp, hbmp1;
    BITMAP bmpInfo;

    bm.LoadBitmap(IDB_TOOLBAR);
    hbmp = bm;
    GetObject(hbmp, sizeof(BITMAP), &bmpInfo);
    hbmp1 = (HBITMAP)CopyImage(hbmp, IMAGE_BITMAP, bmpInfo.bmWidth * BUTTON_SIZE/16, bmpInfo.bmHeight * BUTTON_SIZE/16, LR_COPYDELETEORG);
    bm1.Attach(hbmp1);
    GetObject(hbmp1, sizeof(BITMAP), &bmpInfo);

    m_ilToolBar.DeleteImageList();
    m_ilToolBar.Create(BUTTON_SIZE, BUTTON_SIZE, ILC_COLORDDB | ILC_MASK, 0, 0);
    m_ilToolBar.Add(&bm1, RGB(0xFF, 0, 0xFF));
    m_ilToolBar.Remove(5);
    m_ilToolBar.Remove(4);
    m_ilToolBar.Remove(1);
    m_ilToolBar.Remove(0);

    m_ctrlToolBar.Create(WS_TOOLBAR, CRect(0,0,0,0), this, IDC_TOOLBAR);
    m_ctrlToolBar.SetImageList(&m_ilToolBar);

    TBBUTTON *ptbButtons = new TBBUTTON[TOOLBAR_BUTTONS_COUNT];
    int tt = TOOLBAR_BUTTONS_COUNT;
    for (int i = 0; i < TOOLBAR_BUTTONS_COUNT; i++) {
        ptbButtons[i] = TbButtonFromButtonInfo(i);
    }
    m_ctrlToolBar.AddButtons(TOOLBAR_BUTTONS_COUNT, ptbButtons);
    m_ctrlToolBar.SetStyle(TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | CCS_TOP);
    CSize size(24, 24);
    m_ctrlToolBar.SetButtonSize(size);

    m_ctrlToolTip.Create(this);
    for (int i = 0; i < TOOLBAR_BUTTONS_COUNT; i ++) {
        CRect toolButtonRect;
        TBBUTTON btn;
        if (m_ctrlToolBar.GetButton(i, &btn) && btn.fsStyle == TBSTYLE_BUTTON) {
            m_ctrlToolBar.GetItemRect(i, &toolButtonRect);
            m_ctrlToolTip.AddTool(&m_ctrlToolBar,  btn.idCommand, toolButtonRect, btn.idCommand);
        } 
    }
    m_ctrlToolBar.SetToolTips(&m_ctrlToolTip);

    return 0;
}

void CPdfClimateView::OnPageFirst()
{
    gotoFirstPage();
}

void CPdfClimateView::OnPageLast()
{
    gotoLastPage();
}

void CPdfClimateView::OnPagePrev()
{
    gotoPrevPage();
}

void CPdfClimateView::OnPageNext()
{
    gotoNextPage();
}

void CPdfClimateView::OnZoomOut()
{
    zoomOut();
}

void CPdfClimateView::OnZoomIn()
{
    zoomIn();
}

void CPdfClimateView::OnDragNormal()
{
    m_iMode = Drag_Normal;
}

void CPdfClimateView::OnDragDotted()
{
    m_iMode = Drag_Dotted;
}

void CPdfClimateView::OnDragFacet()
{
    m_iMode = Drag_Facet;
}

void CPdfClimateView::OnDragLinear()
{
    m_iMode = Drag_Linear;
}

void CPdfClimateView::OnDragColumnar()
{
    m_iMode = Drag_Columnar;
}

void CPdfClimateView::OnDragUnknown()
{
    m_iMode = Drag_Unknow;
}
