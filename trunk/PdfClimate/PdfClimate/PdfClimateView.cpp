
// PdfClimateView.cpp : implementation of the CPdfClimateView class
//

#include "stdafx.h"
#include "PdfClimate.h"

#include "PdfClimateDoc.h"
#include "PdfClimateView.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "fitz.h"
extern "C" {
    void mupdf_close();
    void mupdf_open(char *filename);
    int mupdf_get_page_count();
    const fz_pixmap *mupdf_load_page(int pagenumber, int zoom);
    void mupdf_winblit(HDC hdc, int winw, int winh);
}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPdfClimateView

IMPLEMENT_DYNCREATE(CPdfClimateView, CView)

BEGIN_MESSAGE_MAP(CPdfClimateView, CView)
    ON_WM_SIZE()
END_MESSAGE_MAP()

// CPdfClimateView construction/destruction

CPdfClimateView::CPdfClimateView()
{
    m_bFileOpened = false;
    m_pageNum = 0;
    m_pageIndex = 0;
}

CPdfClimateView::~CPdfClimateView()
{
    mupdf_close();
}

BOOL CPdfClimateView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    return CView::PreCreateWindow(cs);
}

void CPdfClimateView::OnInitialUpdate()
{
    CView::OnInitialUpdate();
}

void CPdfClimateView::OnRButtonUp(UINT nFlags, CPoint point)
{
    ClientToScreen(&point);
    OnContextMenu(this, point);
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
    char filename[256];
	int pagenumber = 2;
	int zoom = 100;
	int rotation = 0;

    CFileDialog dlg(TRUE, 
    "PDF Files (*.pdf)", 
    NULL, 
    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING,
    _T("PDF Files (*.pdf)||"),
    NULL);
    if (dlg.DoModal()==IDCANCEL) {
        return;
    }
    snprintf(filename, 256, "%s", dlg.GetPathName().GetBuffer(dlg.GetPathName().GetLength()));

    mupdf_open(filename);
    m_pageNum = mupdf_get_page_count();
    m_pageIndex = 1;
    mupdf_load_page(m_pageIndex, 100);
    this->m_bFileOpened = true;
    this->RedrawWindow();
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
        mupdf_winblit(hdc, rect.Width(), rect.Height());;
	    hdc = NULL;
    }
}

void CPdfClimateView::gotoFirstPage()
{
    if (m_bFileOpened) {
        if (m_pageIndex != 1){
            m_pageIndex = 0;
            mupdf_load_page(m_pageIndex, 100);
            this->RedrawWindow();
        }
    }
}

void CPdfClimateView::gotoLastPage()
{
    if (m_bFileOpened) {
        if (m_pageIndex != m_pageNum-1){
            m_pageIndex = m_pageNum-1;
            mupdf_load_page(m_pageIndex, 100);
            this->RedrawWindow();
        }
    }
}

void CPdfClimateView::gotoNextPage()
{
    if (m_bFileOpened) {
        if (m_pageIndex != m_pageNum-1){
            m_pageIndex ++;
            mupdf_load_page(m_pageIndex, 100);
            this->RedrawWindow();
        }
    }
}

void CPdfClimateView::gotoPrevPage()
{
    if (m_bFileOpened) {
        if (m_pageIndex >0){
            m_pageIndex --;
            mupdf_load_page(m_pageIndex, 100);
            this->RedrawWindow();
        }
    }
}
