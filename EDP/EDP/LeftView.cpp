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

IMPLEMENT_DYNCREATE(CLeftView, CTreeView)

CLeftView::CLeftView()
{
	m_lstState.DeleteImageList();
	m_lstFolder.DeleteImageList();
}

CLeftView::~CLeftView()
{
}


BEGIN_MESSAGE_MAP(CLeftView, CTreeView)
	//{{AFX_MSG_MAP(CLeftView)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	//}}AFX_MSG_MAP
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
	CTreeView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLeftView message handlers

void CLeftView::OnInitialUpdate() 
{
	CTreeView::OnInitialUpdate();

	m_lstState.DeleteImageList();
	m_lstState.Create(IDB_BITMAP_STATE, 13, 1, RGB(255,255,255));
	CTreeCtrl& ctlTree=(CTreeCtrl&)GetTreeCtrl();
	ctlTree.SetBkColor(RGB(210, 230, 190));
	ctlTree.SetImageList (&m_lstState, TVSIL_NORMAL);
	//m_lstFolder.Create(IDB_BITMAP_FODER,16, 1, RGB(255,255,255));
	
	/*
	m_lstState.Create (16,16,ILC_COLOR|ILC_MASK,3,3);
	m_lstState.Add (AfxGetApp()->LoadIcon (IDI_CK));
	m_lstState.Add (AfxGetApp()->LoadIcon (IDI_BASE_INFO));
	m_lstState.Add (AfxGetApp()->LoadIcon (IDI_INPUT));
	CTreeCtrl& ctlTree=(CTreeCtrl&)GetTreeCtrl();
	ctlTree.SetBkColor (RGB(210,230,190));
	ctlTree.SetImageList (&m_lstState, TVSIL_NORMAL);
	*/

	int i=0;

	TV_INSERTSTRUCT tvRoot;//树根
    TV_INSERTSTRUCT tvSecond;//树枝
	TV_INSERTSTRUCT tvThree;//树叶
	tvRoot.hParent=NULL;
	tvRoot.item.pszText="根目录/";
	tvRoot.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
    tvRoot.item.iImage=1;
	tvRoot.item.iSelectedImage=2;
	HTREEITEM item_root=ctlTree.InsertItem (&tvRoot);
    int ii;
	CString strSecond[]={"目录1"};
	CString strThree[1][1]={{"文件1"}};
	for(i=0;i<2;i++)
	{
		tvSecond.hParent=item_root;
		tvSecond.item.pszText=(LPTSTR)(LPCTSTR)strSecond[i];
		tvSecond.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
        tvSecond.item.iImage=1;
	    tvSecond.item.iSelectedImage=2;
		HTREEITEM item_second=ctlTree.InsertItem (&tvSecond);
		switch(i)
		{
		case 0:
			ii=3;
			break;
        case 1:
			ii=2;
			break;
		default:
			break;

		}
		for(int j=0;j<ii;j++)
		{tvThree.hParent=item_second;
		tvThree.item.pszText=(LPTSTR)(LPCTSTR)strThree[i][j];
		tvThree.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
        tvThree.item.iImage=1;
	    tvThree.item.iSelectedImage=2;
	    ctlTree.InsertItem (&tvThree);
		}
	}
	//设置列表视图的风格
	ctlTree.Expand(item_root,TVE_EXPAND);
	DWORD dwStyle=GetWindowLong(ctlTree.m_hWnd ,GWL_STYLE);
	dwStyle|=TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT;
	::SetWindowLong (ctlTree.m_hWnd ,GWL_STYLE,dwStyle);
}



void CLeftView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
}

void CLeftView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
}
