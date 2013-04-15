
// PdfClimateView.cpp : implementation of the CPdfClimateView class
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "PdfClimateDoc.h"
#include "PdfClimateView.h"
#include "mupdf.h"
#include "CtrlLib/LBTabCtrl/LBTabCtrl.h"
#include "LeftView.h"
#include "GlobalVars.h"

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
END_MESSAGE_MAP()

// CPdfClimateView construction/destruction

CPdfClimateView::CPdfClimateView()
{
    m_bFileOpened = false;
    memset(m_sDocPath, 0, sizeof(m_sDocPath));
    memset(m_sDocTitle, 0, sizeof(m_sDocTitle));
    m_iPageNum = 0;
    m_iCurrPage = 0;
    m_iZoom = 100;
    m_iCX = 0;
    m_iCY = 0;

    m_ptOrig = CPoint(0, 0);
    m_bDragging = false;
    m_hCross = AfxGetApp()->LoadStandardCursor(IDC_CROSS);

    m_iRectNum = 0;

    mupdf_init();
}

CPdfClimateView::~CPdfClimateView()
{
    if (m_bFileOpened) {
        mupdf_close_file();
    }
    mupdf_destroy();
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
}


void CPdfClimateView::openFile()
{
    CFileDialog dlg(TRUE, 
                    "PDF Files (*.pdf)", 
                    NULL, 
                    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING,
                    _T("PDF Files (*.pdf)||"),
                    NULL
                   );
    if (dlg.DoModal()==IDCANCEL) {
        testSQL();
        return;
    }
    sprintf(m_sDocPath, "%s", dlg.GetPathName().GetBuffer(dlg.GetPathName().GetLength()));

    //gb2312 to unicode
    int wLen = MultiByteToWideChar(CP_ACP, 0, m_sDocPath, -1, NULL, 0);
    LPWSTR wStr = new WCHAR[wLen];
    MultiByteToWideChar(CP_ACP, 0, m_sDocPath, -1, wStr, wLen);
    //unicode to utf8
    int aLen = WideCharToMultiByte(CP_UTF8, 0, wStr, -1, NULL, 0, NULL, NULL);
    char* converted = new char[aLen];
    WideCharToMultiByte(CP_UTF8, 0, wStr, -1, converted, aLen, NULL, NULL);

    bool bRet = mupdf_open_file(converted);

    delete []wStr;
    delete []converted;

    if (!bRet) {
        AfxMessageBox("Open failed!", MB_OK);
        return;
    }

    ::AfxGetMainWnd()->SetWindowTextA(m_sDocPath);

    g_pLeftView->GetTreeCtrl().InsertItem(m_sDocPath);

    m_iPageNum = mupdf_get_page_count();
    m_iCurrPage = 0;
    m_iCX = 0;
    m_iCY = 0;
    m_iZoom = 100;
    mupdf_load_page(m_iCurrPage, m_iZoom);
    m_bFileOpened = true;
    RedrawWindow();
    this->SetFocus();
}

void CPdfClimateView::closeFile()
{
    if (m_bFileOpened) {
        if ( IDYES==::AfxMessageBox("close the file?", MB_YESNO) ){
            mupdf_close_file();
            m_iPageNum = 0;
            m_iCurrPage = 0;
            m_iCX = 0;
            m_iCY = 0;
            m_iZoom = 100;
            RedrawWindow();
        }
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

        this->GetClientRect(&rect);
        HDC hdc = ::GetDC(m_hWnd);
        mupdf_winblit(hdc, rect.Width(), rect.Height(), m_iCX, m_iCY);;
	    hdc = NULL;

        if (m_bDragging) {
            CClientDC dc(this);
            HBRUSH hb = (HBRUSH) GetStockObject(NULL_BRUSH);
            CBrush* brush = CBrush::FromHandle(hb);
            CBrush *pOldBrush = dc.SelectObject(brush);
            dc.Rectangle(CRect(m_ptOrig, m_ptDest));
            dc.SelectObject(pOldBrush);
        }
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

void CPdfClimateView::zoomIn()
{
    if (m_bFileOpened) {
        if (m_iZoom > 0) {
            m_iZoom = (int)(m_iZoom / 1.1f);
            mupdf_load_page(m_iCurrPage, m_iZoom);
            RedrawWindow();
        }
    }
}

void CPdfClimateView::zoomOut()
{
    if (m_bFileOpened) {
        if (m_iZoom < 500) {
            m_iZoom = (int)(m_iZoom * 1.1f);
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
            zoomIn();
            break;
        }
        case '=':
        {
            zoomOut();
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
	SetCapture();
	::SetCursor(m_hCross);
	m_ptOrig = point;
	m_bDragging = true;

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
	    CDC *pDC = GetDC();
	    CBitmap bm;

	    RECT rect;
        rect.left = m_ptOrig.x;
        rect.top = m_ptOrig.y;
        rect.right = point.x;
        rect.bottom = point.y;
        //this->ScreenToClient(&rect);
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

        bm.CreateCompatibleBitmap(pDC, rect.right - rect.left, rect.bottom - rect.top);
	    CDC tdc;
	    tdc.CreateCompatibleDC(pDC);
	    CBitmap *pOld=tdc.SelectObject(&bm);
	    tdc.BitBlt(0, 0, rect.right - rect.left, rect.bottom - rect.top,
                    pDC, rect.left, rect.top, SRCCOPY);
	    CImage *image = new CImage; 
	    image->Attach(bm); 
        image->Save("c:\\a.jpg");


        CClientDC dc(this);
        HBRUSH hb = (HBRUSH) GetStockObject(NULL_BRUSH);
        CBrush* brush = CBrush::FromHandle(hb);
        CBrush *pOldBrush = dc.SelectObject(brush);
        dc.Rectangle(CRect(m_ptOrig, point));
        dc.SelectObject(pOldBrush);

		m_bDragging = false;
		ReleaseCapture();
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

int CPdfClimateView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}
