// LeftView.cpp : implementation file
//

#include "stdafx.h"
#include "LeftView.h"
#include "EDP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CView)

CLeftView::CLeftView()
{
	m_ilDataFile.DeleteImageList();
	m_pTree = NULL;
}

CLeftView::~CLeftView()
{
}


BEGIN_MESSAGE_MAP(CLeftView, CView)
	//{{AFX_MSG_MAP(CLeftView)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLeftView drawing

void CLeftView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CLeftView diagnostics

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLeftView message handlers

void CLeftView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	// create image list for level 0 items
	m_ilDataFile.DeleteImageList();
	m_ilDataFile.Create(IDB_BITMAP_DATAFILE, 16, 1, RGB(255,255,255));
	m_pTree->SetImageList(&m_ilDataFile, TVSIL_NORMAL);

	m_pTree->Initialize(TRUE, TRUE);
	m_pTree->SetSmartCheckBox(TRUE);
	m_pTree->SetHtml(TRUE);
	m_pTree->SetImages(TRUE);

	int i=0;

	TV_INSERTSTRUCT tvRoot;//Ê÷¸ù
    TV_INSERTSTRUCT tvSecond;//Ê÷Ö¦
	TV_INSERTSTRUCT tvThree;//Ê÷Ò¶
	tvRoot.hParent=NULL;
	tvRoot.item.pszText="¸ùÄ¿Â¼/";
	tvRoot.item.mask=TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    tvRoot.item.iImage=8;
	tvRoot.item.iSelectedImage=8;
	HTREEITEM item_root=m_pTree->InsertItem(&tvRoot);
	m_pTree->SetTextColor(RGB(255, 0, 0));

	tvRoot.hParent = item_root;
	item_root = m_pTree->InsertItem(&tvRoot);
	m_pTree->SetTextColor(RGB(0, 255, 0));

	for(int i=0;i<9;i++){
		tvRoot.item.iImage=i;
		tvRoot.item.iSelectedImage=i;
		item_root = m_pTree->InsertItem(&tvRoot);
		m_pTree->SetItemTextColor(item_root, RGB(((i/6)%2)*255, ((i/3)%2)*255, ((i/5)%2)*255));
	}
}



void CLeftView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
}

void CLeftView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
}

int CLeftView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	TRACE(_T("in CLeftView::OnCreate\n"));

	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pTree = new CXHtmlTree();
	ASSERT(m_pTree);

	// note:  TVS_NOTOOLTIPS is set in CXHtmlTree::PreCreateWindow()

	DWORD dwStyle = TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | 
					TVS_EDITLABELS | TVS_SHOWSELALWAYS | /*TVS_NOTOOLTIPS |*/
					WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP | WS_BORDER;

	CRect rect(0,0,100,100);

	VERIFY(m_pTree->Create(dwStyle, rect, this, IDC_TREE));

	return 0;
}

void CLeftView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (m_pTree && ::IsWindow(m_pTree->m_hWnd))
	{
		// stretch tree to fill window
		m_pTree->MoveWindow(0, 0, cx, cy);
	}
}
