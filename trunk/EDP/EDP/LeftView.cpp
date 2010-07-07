// LeftView.cpp : implementation file
//

#include "stdafx.h"
#include "LeftView.h"
#include "EDP.h"
#include "GlobalFuncs.h"
#include "direct.h"
#include "SplitterWndEx\\SplitterWndEx.h"
#include "EDPView.h"

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
}

CLeftView::~CLeftView()
{
}


BEGIN_MESSAGE_MAP(CLeftView, CView)
	//{{AFX_MSG_MAP(CLeftView)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchanged)
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(WM_XHTMLTREE_CHECKBOX_CLICKED, OnCheckbox)
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
#define CONFIG_DIR_FILE		"directories.txt"

void CLeftView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	// create image list for level 0 items
	m_ilDataFile.DeleteImageList();
	m_ilDataFile.Create(IDB_BITMAP_DATAFILE, 16, 1, RGB(255,255,255));

	InitTree();
}

void CLeftView::InitTree()
{
	g_pTree->DeleteAllItems();

	g_pTree->SetImageList(&m_ilDataFile, TVSIL_NORMAL);

	g_pTree->Initialize(TRUE, TRUE);
	g_pTree->SetSmartCheckBox(TRUE);
	g_pTree->SetHtml(TRUE);
	g_pTree->SetImages(TRUE);
	g_pTree->SetTextColor(RGB(0, 0, 0));

	COLORREF color[] = {RGB(255,0,0),RGB(0,255,0),RGB(0,0,255),RGB(255,255,0),RGB(255,0,255),RGB(0,255,255),RGB(0,0,0),};
	LoadNode();

	TV_INSERTSTRUCT tvRoot;//Ê÷¸ù
	tvRoot.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	for(int i=0;i<g_iDirNodeNum;i++){
		tvRoot.hParent = NULL;
		tvRoot.item.pszText = g_pDirNode[i].sDir;
		tvRoot.item.iImage = 8;
		tvRoot.item.iSelectedImage = 8;
	
		HTREEITEM item_root=g_pTree->InsertItem(&tvRoot);
		for(int j=0;j<g_pDirNode[i].iNum;j++){
			TV_INSERTSTRUCT tvChild;//Ê÷Ò¶
			tvChild.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			tvChild.item.pszText = g_pDirNode[i].pFileNode[j].addition_info.sFile;
			tvChild.item.iImage= 10;
			tvChild.item.iSelectedImage = 10;
			tvChild.hParent = item_root;
			HTREEITEM item_child = g_pTree->InsertItem(&tvChild);
			for(int k=0;k<g_pDirNode[i].pFileNode[j].test_record.iNumOfSpeed;k++){
				TV_INSERTSTRUCT tvChild;//Ê÷Ò¶
				tvChild.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
				char tmp[128];
				sprintf(tmp, "%s%d", "Speed ", k + 1);
				tvChild.item.pszText = tmp;
				tvChild.item.iImage=(k % 12) + 2;
				tvChild.item.iSelectedImage = (k % 12) + 2;
				tvChild.hParent = item_child;
				HTREEITEM item_curr = g_pTree->InsertItem(&tvChild);
				g_pTree->SetItemTextColor(item_curr, color[k]);
				g_pTree->SetItemData(item_curr, (DWORD_PTR)(&g_pDirNode[i].pFileNode[j].tree_item_data[k]));
			}
		}
	}
}

void CLeftView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
}

void CLeftView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMView = (NM_TREEVIEW*)pNMHDR;

	CSplitterWndEx* pSplitter=(CSplitterWndEx*)GetParent();
	CEDPView *pView = (CEDPView*)pSplitter->GetPane(0,1);
	pView->RedrawWindow();

	*pResult = 0;
}

int CLeftView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	TRACE(_T("in CLeftView::OnCreate\n"));

	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	g_pTree = new CXHtmlTree();
	ASSERT(g_pTree);

	// note:  TVS_NOTOOLTIPS is set in CXHtmlTree::PreCreateWindow()

	DWORD dwStyle = TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | 
					TVS_SHOWSELALWAYS | /*TVS_NOTOOLTIPS |*/
					WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP | WS_BORDER;
					//TVS_EDITLABELS : ¿ÉÒÔ±à¼­

	CRect rect(0,0,100,100);

	VERIFY(g_pTree->Create(dwStyle, rect, this, IDC_TREE));

	return 0;
}

void CLeftView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (g_pTree && ::IsWindow(g_pTree->m_hWnd))
	{
		// stretch tree to fill window
		g_pTree->MoveWindow(0, 0, cx, cy);
	}
}

LRESULT CLeftView::OnCheckbox(WPARAM wParam, LPARAM lParam)
//=============================================================================
{
	/*
	TRACE(_T("in CXHtmlTreeTestDlg::OnCheckbox\n"));

	XHTMLTREEMSGDATA *pData = (XHTMLTREEMSGDATA *)wParam;
	ASSERT(pData);

	BOOL bChecked = lParam;

	if (pData)
	{
		HTREEITEM hItem = pData->hItem;

		if (hItem)
		{
			if (m_bLog)
				m_List.Printf(CXListBox::Blue, CXListBox::White, 0, 
					_T("%04d  checkbox %schecked at '%s'"), m_nLineNo++,
					bChecked ? _T("") : _T("un"),
					m_Tree.GetItemText(hItem));

			DumpInfo(hItem);

			UINT uState = m_Tree.GetItemState(hItem, TVIS_STATEIMAGEMASK);
			uState = uState >> 12;
			int nStateImage = m_Tree.GetStateImage(hItem);
			TRACE(_T("bChecked=%d  uState=%X  nStateImage=%d\n"), bChecked, uState, nStateImage);

			if (m_pCheckedItemsDlg)
				OnShowChecked();
		}
	}
	*/
	CSplitterWndEx* pSplitter=(CSplitterWndEx*)GetParent();
	CEDPView *pView = (CEDPView*)pSplitter->GetPane(0,1);
	pView->RedrawWindow();


	return 0;
}
