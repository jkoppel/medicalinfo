// LeftView.cpp : implementation file
//

#include "stdafx.h"
#include <direct.h>
#include "LeftTreeView.h"
#include "RightDrawAreaView.h"
#include "AbsTestDataMan.h"
#include "GlobalFuncs.h"
#include "GlobalVars.h"
#include "SplitterWndEx\\SplitterWndEx.h"
#include "TestDataTreeMgt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLeftTreeView

IMPLEMENT_DYNCREATE(CLeftTreeView, CView)

CLeftTreeView::CLeftTreeView()
{
	m_ilDataFile.DeleteImageList();
	m_bCheckBoxes = TRUE;
}

CLeftTreeView::~CLeftTreeView()
{
	if(g_pTree!=NULL){
		delete g_pTree;
	}
}


BEGIN_MESSAGE_MAP(CLeftTreeView, CView)
	//{{AFX_MSG_MAP(CLeftTreeView)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(WM_XHTMLTREE_CHECKBOX_CLICKED, OnCheckbox)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLeftTreeView drawing

void CLeftTreeView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CLeftTreeView diagnostics

#ifdef _DEBUG
void CLeftTreeView::AssertValid() const
{
	CView::AssertValid();
}

void CLeftTreeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLeftTreeView message handlers
#define CONFIG_DIR_FILE		"directories.txt"

void CLeftTreeView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	// create image list for level 0 items
	m_ilDataFile.DeleteImageList();
	m_ilDataFile.Create(IDB_BITMAP_DATAFILE, 16, 1, RGB(255,255,255));

	InitTree();

	g_pLeftTreeView = this;
}

void CLeftTreeView::InitTree(BOOL bReloadMode)
{
	CTestDataTreeMgt mgt;
	if(!bReloadMode){
		g_TestDataTreeMgt.InitTree();
	}
	else{
		g_TestDataTreeMgt.ReloadTree();
	}

	COLORREF color[] = {RGB(255,0,0),RGB(0,255,0),RGB(0,0,255),RGB(255,255,0),RGB(255,0,255),RGB(0,255,255),RGB(0,0,0),};

	g_pTree->DeleteAllItems();
	g_pTree->SetImageList(&m_ilDataFile, TVSIL_NORMAL);
	g_pTree->Initialize(m_bCheckBoxes, TRUE);
	g_pTree->SetSmartCheckBox(TRUE);
	g_pTree->SetHtml(TRUE);
	g_pTree->SetImages(TRUE);
	g_pTree->SetTextColor(RGB(0, 0, 0));

	TV_INSERTSTRUCT tvRoot;//����
	tvRoot.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	ProductNodePtr pProductNode = g_TestDataTreeMgt.m_pProductTreeRoot->pProductNodeListHead->pNext;
	if(pProductNode==NULL){
		tvRoot.item.pszText = _T("������");
		tvRoot.hParent = NULL;
		tvRoot.item.iImage = 8;
		tvRoot.item.iSelectedImage = 8;
		g_pTree->InsertItem(&tvRoot);
		return;
	}

	while(pProductNode){
		TCHAR tcBuf[1024];
		tvRoot.hParent = NULL;
		tvRoot.item.pszText = pProductNode->sProductNo;
		tvRoot.item.iImage = 8;
		tvRoot.item.iSelectedImage = 8;
		HTREEITEM item_root=g_pTree->InsertItem(&tvRoot);

		FileNodePtr pFileNode;
		pFileNode = pProductNode->pFileListHead->pNext;
		while(pFileNode!=NULL){
			TV_INSERTSTRUCT tvChild;//��Ҷ
			tvChild.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			tvChild.item.pszText = pFileNode->addition_info.sTestDate;//pFileNode->addition_info.sFile;//
			tvChild.item.iImage= 10;
			tvChild.item.iSelectedImage = 10;
			tvChild.hParent = item_root;
			HTREEITEM item_child = g_pTree->InsertItem(&tvChild);
			for(int k=0;k<pFileNode->test_record.iNumOfSpeed;k++){
				TV_INSERTSTRUCT tvChild;//��Ҷ
				tvChild.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
				_stprintf(tcBuf, _T("%s%d"), _T("Speed"), k+1);
				tvChild.item.pszText = tcBuf;
				tvChild.item.iImage=(k % 12) + 2;
				tvChild.item.iSelectedImage = (k % 12) + 2;
				tvChild.hParent = item_child;
				HTREEITEM item_curr = g_pTree->InsertItem(&tvChild);
				g_pTree->SetItemTextColor(item_curr, color[k]);
				g_pTree->SetItemData(item_curr, (DWORD_PTR)(&pFileNode->tree_item_data[k]));
			}
			pFileNode = pFileNode->pNext;
		}
		pProductNode = pProductNode->pNext;
	}

	g_pTree->Expand(g_pTree->GetRootItem(), TVE_EXPAND);
	UpdateData(FALSE);
}

void CLeftTreeView::SetCheckBoxes(BOOL bCheckBoxes)
{
	m_bCheckBoxes = bCheckBoxes;
}

BOOL CLeftTreeView::GetCheckBoxes()
{
	return m_bCheckBoxes;
}

void CLeftTreeView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
}

void CLeftTreeView::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(!m_bCheckBoxes){
		NMTREEVIEW* pNMTreeView = (NMTREEVIEW*)pNMHDR;

		HTREEITEM hItem = pNMTreeView->itemNew.hItem;
		DWORD_PTR p = g_pTree->GetItemData(hItem);
		if(p!=NULL){
			g_TestDataTreeMgt.ResetNodeStatus();
			struct TreeItemData *tp = (struct TreeItemData *)p;
			tp->bSelected = TRUE;
		}

		g_pRightDrawAreaView->RedrawWindow();
	}

	*pResult = 0;
}

int CLeftTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	TRACE(_T("in CLeftTreeView::OnCreate\n"));

	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	g_pTree = new CXHtmlTree();
	ASSERT(g_pTree);

	// note:  TVS_NOTOOLTIPS is set in CXHtmlTree::PreCreateWindow()

	DWORD dwStyle = TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | 
					TVS_SHOWSELALWAYS | 
					WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP | WS_BORDER;
					//TVS_EDITLABELS(�ɱ༭)

	CRect rect(0,0,100,100);

	VERIFY(g_pTree->Create(dwStyle, rect, this, IDC_TREE));

	return 0;
}

void CLeftTreeView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (g_pTree && ::IsWindow(g_pTree->m_hWnd))
	{
		// stretch tree to fill window
		g_pTree->MoveWindow(0, 0, cx, cy);
	}
}

LRESULT CLeftTreeView::OnCheckbox(WPARAM wParam, LPARAM lParam)
//=============================================================================
{
	if(m_bCheckBoxes){
		g_TestDataTreeMgt.ResetNodeStatus();
		HTREEITEM hItem = g_pTree->GetFirstCheckedItem();
		while (hItem){
			DWORD_PTR p = g_pTree->GetItemData(hItem);
			hItem = g_pTree->GetNextCheckedItem(hItem);

			if(p==NULL){
				continue;
			}
			struct TreeItemData *tp = (struct TreeItemData *)p;
			tp->bSelected = TRUE;
		}

		g_pRightDrawAreaView->RedrawWindow();
	}

	return 0;
}