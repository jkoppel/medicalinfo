// LeftView.cpp : implementation file
//

#include "stdafx.h"
#include <direct.h>
#include "LeftTreeView.h"
#include "RightDrawAreaView.h"
#include "AbsTestDataMan.h"
#include "GlobalFuncs.h"
#include "GlobalVars.h"
#include "ExtLibs\\MySplitterWndEx\\MySplitterWndEx.h"
#include "TestDataTreeMgt.h"
#include "DlgProgress.h"
#include "DrawAreaConfigMgt.h"

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
	m_bMultiSelectMode = TRUE;
	m_pCurrFileItem = NULL;

	m_pPopupMenu = new CMenu();
	m_pPopupMenu->CreatePopupMenu();
	m_pPopupMenu->AppendMenu(MF_STRING, ID_FILEMODE_SINGLE, _T("单文件模式"));
	m_pPopupMenu->AppendMenu(MF_STRING, ID_FILEMODE_MULTI, _T("多文件模式"));
}

CLeftTreeView::~CLeftTreeView()
{
	m_pPopupMenu->DestroyMenu();
	delete m_pPopupMenu;

	if(g_pTree!=NULL){
		delete g_pTree;
	}
}


BEGIN_MESSAGE_MAP(CLeftTreeView, CView)
	//{{AFX_MSG_MAP(CLeftTreeView)
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(WM_XHTMLTREE_CHECKBOX_CLICKED, OnCheckbox)
	ON_NOTIFY(NM_RCLICK, IDC_TREE, OnRclickTree)
	ON_COMMAND(ID_FILEMODE_SINGLE, &CLeftTreeView::OnModeSingleFile)
	ON_COMMAND(ID_FILEMODE_MULTI, &CLeftTreeView::OnModeMultiFile)
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

	InitTree();

	g_pLeftTreeView = this;
}

int Color2Index(COLORREF color)
{
	DWORD dwPixelVal = color;

	int nBlue = dwPixelVal >> 16;
	int nGreen = (dwPixelVal & 0x00FF00) >> 8 ;
	int nRed = dwPixelVal & 0x0000FF;
	// Get Index of suitable color data in the palette table.

	UINT uRedValue = GetPixelValue(nRed);
	UINT uGreenValue = GetPixelValue(nGreen);
	UINT uBlueValue = GetPixelValue(nBlue);

	UINT uPalettePos = uBlueValue*36+uGreenValue*6+uRedValue;

	return (int)uPalettePos;
}

void CLeftTreeView::InitTree(BOOL bReloadMode)
{
	// create image list for level 0 items
	m_ilDataFile.DeleteImageList();
	m_ilDataFile.Create(IDB_BITMAP_DATAFILE, 16, 1, RGB(255,255,255));

	if(!bReloadMode){
		g_TestDataTreeMgt.InitTree();
	}
	else{
		g_TestDataTreeMgt.ReloadTree();
	}

	CDrawAreaConfigMgt dacMgt;
	struct DrawAreaConfig daConfig;
	dacMgt.GetDrawAreaConfig(daConfig);

	g_pTree->DeleteAllItems();
	g_pTree->SetImageList(&m_ilDataFile, TVSIL_NORMAL);
	g_pTree->Initialize(TRUE, TRUE);
	g_pTree->SetSmartCheckBox(TRUE);
	g_pTree->SetHtml(TRUE);
	g_pTree->SetImages(TRUE);
	g_pTree->SetTextColor(RGB(0, 0, 0));

	TV_INSERTSTRUCT tvRoot;//树根
	tvRoot.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	ProductNodePtr pProductNode = g_TestDataTreeMgt.GetProductTreeRootPtr()->pProductNodeListHead->pNext;
	if(pProductNode==NULL){
		tvRoot.item.pszText = _T("无数据");
		tvRoot.hParent = NULL;
		tvRoot.item.iImage = Color2Index(RGB(0, 0, 255));
		tvRoot.item.iSelectedImage = Color2Index(RGB(0, 0, 255));
		g_pTree->InsertItem(&tvRoot);
		return;
	}

	while(pProductNode){
		TCHAR tcBuf[1024];
		tvRoot.hParent = NULL;
		tvRoot.item.pszText = pProductNode->sProductNo;
		tvRoot.item.iImage = Color2Index(RGB(0, 0, 255));
		tvRoot.item.iSelectedImage = Color2Index(RGB(0, 0, 255));
		HTREEITEM item_root=g_pTree->InsertItem(&tvRoot);

		FileNodePtr pFileNode;
		pFileNode = pProductNode->pFileListHead->pNext;
		while(pFileNode!=NULL){
			TV_INSERTSTRUCT tvChild;//树叶
			tvChild.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			tvChild.item.pszText = pFileNode->sTestDate;
			tvChild.item.iImage= Color2Index(RGB(0xFF, 0x45, 0x00));
			tvChild.item.iSelectedImage = Color2Index(RGB(0xFF, 0x45, 0x00));
			tvChild.hParent = item_root;
			HTREEITEM item_child = g_pTree->InsertItem(&tvChild);
			for(int k=0;k<pFileNode->test_record_maininfo.iNumOfSpeed;k++){
				TV_INSERTSTRUCT tvChild;//树叶
				tvChild.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
				_stprintf(tcBuf, _T("%s%d"), _T("Speed"), k+1);
				tvChild.item.pszText = tcBuf;
				tvChild.item.iImage = Color2Index(daConfig.crSpeedCurve[k]);
				tvChild.item.iSelectedImage = Color2Index(daConfig.crSpeedCurve[k]);
				tvChild.hParent = item_child;
				HTREEITEM item_curr = g_pTree->InsertItem(&tvChild);
				g_pTree->SetItemTextColor(item_curr, daConfig.crSpeedCurve[k]);
				g_pTree->SetItemData(item_curr, (DWORD_PTR)(&pFileNode->tree_item_data[k]));
			}
			pFileNode = pFileNode->pNext;
		}
		pProductNode = pProductNode->pNext;
	}

	g_pTree->Expand(g_pTree->GetFirstVisibleItem(), TVE_EXPAND);
	if(g_pTree->ItemHasChildren(g_pTree->GetFirstVisibleItem())){
		g_pTree->Expand(g_pTree->GetChildItem(g_pTree->GetFirstVisibleItem()), TVE_EXPAND);
	}
	UpdateData(FALSE);
}

void CLeftTreeView::SetMultiSelectMode(BOOL bMultiSelectMode)
{
	m_bMultiSelectMode = bMultiSelectMode;
}

BOOL CLeftTreeView::GetMultiSelectMode()
{
	return m_bMultiSelectMode;
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
					//TVS_EDITLABELS(可编辑)

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

///用户单击复选框
LRESULT CLeftTreeView::OnCheckbox(WPARAM wParam, LPARAM lParam)
//=============================================================================
{
	if(m_bMultiSelectMode){//打开多选模式
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
		return 0;
	}
	else{//关闭多选模式，只能在一个文件结点下面进行操作
		XHTMLTREEMSGDATA *pData = (XHTMLTREEMSGDATA *)wParam;
		if(pData==NULL){
			return 0;
		}

		HTREEITEM hItem_child = NULL;	//孩子结点
		HTREEITEM hItem_checked = pData->hItem;//当前操作项
		BOOL bCheck = (BOOL)lParam;		//当前状态
		DWORD_PTR p;					//指向该项的data_ptr指针
		struct TreeItemData *tp = NULL;	//该项数据指针对应的结点数据
		int iNumOfSpeed;				//曲线数量
		static BOOL bFlagChecked = FALSE;//临时存储是否是从当前流程的SetCheck函数过来的，如果是，则不再处理

		if(bFlagChecked == TRUE){
			bFlagChecked = FALSE;
			return 0;
		}
		if(hItem_checked==NULL){
			return 0;
		}

		if(!bCheck){//从check到没有check
			if(m_pCurrFileItem==NULL){
				return 0;
			}
			if(!g_pTree->ItemHasChildren(hItem_checked)){//最底下的叶子结点
				if(!g_pTree->IsChildNodeOf(hItem_checked, m_pCurrFileItem)){
					//不是当前文件下面的结点,理论上不该出现
					return 0;
				}
				//是当前文件下面的结点
				if(g_pTree->GetChildrenCheckedCount(m_pCurrFileItem)<=0){//都没有被点中了
					g_TestDataTreeMgt.ResetNodeStatus();
					m_pCurrFileItem = NULL;
					g_pRightDrawAreaView->RedrawWindow();
					return 0;
				}
				//还剩有其它结点被选中，仅需清空当前选中的结点的状态即可
				p = g_pTree->GetItemData(hItem_checked);
				tp = (struct TreeItemData *)p;
				ASSERT(tp);
				tp->bSelected = FALSE;
				g_pRightDrawAreaView->RedrawWindow();
				return 0;
			}
			else{//非叶子节点,可能是文件结点,也可能是型号结点
				hItem_child = g_pTree->GetChildItem(hItem_checked);
				if(!g_pTree->ItemHasChildren(hItem_child)){//是文件结点
					if(hItem_checked==m_pCurrFileItem){//当前文件结点从点中到没点中
						m_pCurrFileItem = NULL;
						g_TestDataTreeMgt.ResetNodeStatus();
						g_pRightDrawAreaView->RedrawWindow();
						return 0;
					}
					else{//选中其它结点
						//视情况而定，目前仅做情况
						bFlagChecked = TRUE;
						g_pTree->SetCheck(hItem_checked);
						g_pRightDrawAreaView->RedrawWindow();
						return 0;
					}
				}
				else{//是型号结点
					m_pCurrFileItem = NULL;
					g_TestDataTreeMgt.ResetNodeStatus();
					g_pRightDrawAreaView->RedrawWindow();
					return 0;
				}					
			}
		}
		else{//从uncheck到check
			if(!g_pTree->ItemHasChildren(hItem_checked)){//最底下的叶子结点
				if(m_pCurrFileItem==NULL || g_pTree->IsChildNodeOf(hItem_checked, m_pCurrFileItem)){
					//先前未选中，或当前选中的是当前文件下面的结点
					p = g_pTree->GetItemData(hItem_checked);
					tp = (struct TreeItemData *)p;
					ASSERT(tp);
					tp->bSelected = TRUE;
					m_pCurrFileItem = g_pTree->GetParentItem(hItem_checked);
					g_pRightDrawAreaView->RedrawWindow();
					return 0;
				}
				else{
					//先前有选中，并且当前选中的是其它文件结点下面的叶子结点
					//视情况而定，目前只是不让选
					bFlagChecked = TRUE;
					g_pTree->SetCheck(hItem_checked, FALSE);
					g_pRightDrawAreaView->RedrawWindow();
					return FALSE;
				}
			}
			else{//非叶子节点,可能是文件结点,也可能是型号结点
				hItem_child = g_pTree->GetChildItem(hItem_checked);
				if(g_pTree->GetChildItem(hItem_child)==NULL){//是文件结点
					if(m_pCurrFileItem==NULL || hItem_checked==m_pCurrFileItem){
						//先前未选中，或当前选中的是当前文件结点
						p = g_pTree->GetItemData(hItem_child);
						ASSERT(p);
						tp = (struct TreeItemData *)p;
						ASSERT(tp);
						iNumOfSpeed = tp->pNode->test_record_maininfo.iNumOfSpeed;
						for(int i=0;i<iNumOfSpeed;i++){
							tp->pNode->tree_item_data[i].bSelected = TRUE;
						}
						m_pCurrFileItem = hItem_checked;
						g_pRightDrawAreaView->RedrawWindow();
						return 0;
					}
					else{//先前有选，且当前选的和上次不一样，视情况而定，目前先不让它操作
						bFlagChecked = TRUE;
						g_pTree->SetCheck(hItem_checked, FALSE);
						g_pRightDrawAreaView->RedrawWindow();
						return 0;
					}
				}
				else{//是型号结点
					g_TestDataTreeMgt.ResetNodeStatus();
					m_pCurrFileItem = NULL;
					bFlagChecked = TRUE;
					g_pTree->SetCheck(hItem_checked, FALSE);
					g_pRightDrawAreaView->RedrawWindow();
					return 0;
				}
			}
		}
	}

	return 0;
}

void CLeftTreeView::OnRclickTree(NMHDR* /*pNMHDR*/, LRESULT* pResult)
//=============================================================================
{
	// get the mouse position from this message
	const MSG* pMessage = GetCurrentMessage();
	ASSERT(pMessage);

	if (pMessage)
	{
		CPoint point = pMessage->pt;// get the point from the message
		m_pPopupMenu->CheckMenuRadioItem(ID_FILEMODE_SINGLE, ID_FILEMODE_MULTI, ID_FILEMODE_SINGLE + ((m_bMultiSelectMode)?1:0), MF_BYCOMMAND);
		m_pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	}

	*pResult = 0;
}

void CLeftTreeView::OnModeSingleFile()
{
	SetMultiSelectMode(FALSE);
	InitTree(TRUE);

	if(g_pRightDrawAreaView!=NULL){
		g_pRightDrawAreaView->RedrawWindow();
	}
}

void CLeftTreeView::OnModeMultiFile()
{
	SetMultiSelectMode(TRUE);
	InitTree(TRUE);

	if(g_pRightDrawAreaView!=NULL){
		g_pRightDrawAreaView->RedrawWindow();
	}
}
