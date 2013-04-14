
// PdfClimateView.cpp : implementation of the CGraphSelectView class
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "PdfClimateDoc.h"
#include "GraphSelectView.h"
#include "CtrlLib/LBTabCtrl/LBTabCtrl.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGraphSelectView

IMPLEMENT_DYNCREATE(CGraphSelectView, CView)

BEGIN_MESSAGE_MAP(CGraphSelectView, CView)
    ON_NOTIFY(TCN_SELCHANGE, IDC_DRAW_TAB, &CGraphSelectView::OnTcnSelchangeTab)
    ON_WM_SIZE()
    ON_WM_CREATE()
END_MESSAGE_MAP()

// CGraphSelectView construction/destruction

CGraphSelectView::CGraphSelectView()
{
    m_pTab = NULL;
}

CGraphSelectView::~CGraphSelectView()
{
    if (m_pTab != NULL) {
        delete m_pTab;
    }
}

BOOL CGraphSelectView::PreCreateWindow(CREATESTRUCT& cs)
{
    return CView::PreCreateWindow(cs);
}

void CGraphSelectView::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	RedrawWindow();

    ((CMainFrame*)AfxGetMainWnd())->setActiveGraphView(m_pTab->GetCurSel());
}

void CGraphSelectView::OnInitialUpdate()
{
    CView::OnInitialUpdate();

    if(m_pTab->GetItemCount()==0){
        m_pTab->InsertItem(0, _T("  ÎÄ  µµ  "));
        m_pTab->InsertItem(1, _T("  µã×´Í¼  "));
        m_pTab->InsertItem(2, _T("  Ãæ×´Í¼  "));
        m_pTab->InsertItem(3, _T("  Ïß×´Í¼  "));
        m_pTab->InsertItem(4, _T("  Öù×´Í¼  "));
        m_pTab->InsertItem(5, _T("  ÆÕÍ¨Í¼  "));
    }

}

// CGraphSelectView diagnostics

#ifdef _DEBUG
void CGraphSelectView::AssertValid() const
{
    CView::AssertValid();
}

void CGraphSelectView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CPdfClimateDoc* CGraphSelectView::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPdfClimateDoc)));
    return (CPdfClimateDoc*)m_pDocument;
}
#endif //_DEBUG


void CGraphSelectView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);

    if (m_pTab && ::IsWindow(m_pTab->m_hWnd))
	{
		// stretch tree to fill window
		m_pTab->MoveWindow(0, 0, cx, 25);
	}
}

int CGraphSelectView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

	m_pTab = new CLBTabCtrl();
	ASSERT(m_pTab);

	// note:  TVS_NOTOOLTIPS is set in CXHtmlTree::PreCreateWindow()

	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP;// | WS_BORDER;

	CRect rect(0, 0, 100, 25);

	VERIFY(m_pTab->Create(dwStyle, rect, this, IDC_DRAW_TAB));

    return 0;
}

void CGraphSelectView::OnDraw(CDC* /*pDC*/)
{
	CPdfClimateDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
}