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
	m_pPopupMenu->AppendMenu(MF_STRING, ID_FILEMODE_SINGLE, _T("���ļ�ģʽ"));
	m_pPopupMenu->AppendMenu(MF_STRING, ID_FILEMODE_MULTI, _T("���ļ�ģʽ"));
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

	TV_INSERTSTRUCT tvRoot;//����
	tvRoot.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	ProductNodePtr pProductNode = g_TestDataTreeMgt.GetProductTreeRootPtr()->pProductNodeListHead->pNext;
	if(pProductNode==NULL){
		tvRoot.item.pszText = _T("������");
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
			TV_INSERTSTRUCT tvChild;//��Ҷ
			tvChild.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			tvChild.item.pszText = pFileNode->sTestDate;
			tvChild.item.iImage= Color2Index(RGB(0xFF, 0x45, 0x00));
			tvChild.item.iSelectedImage = Color2Index(RGB(0xFF, 0x45, 0x00));
			tvChild.hParent = item_root;
			HTREEITEM item_child = g_pTree->InsertItem(&tvChild);
			for(int k=0;k<pFileNode->test_record_maininfo.iNumOfSpeed;k++){
				TV_INSERTSTRUCT tvChild;//��Ҷ
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

///�û�������ѡ��
LRESULT CLeftTreeView::OnCheckbox(WPARAM wParam, LPARAM lParam)
//=============================================================================
{
	if(m_bMultiSelectMode){//�򿪶�ѡģʽ
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
	else{//�رն�ѡģʽ��ֻ����һ���ļ����������в���
		XHTMLTREEMSGDATA *pData = (XHTMLTREEMSGDATA *)wParam;
		if(pData==NULL){
			return 0;
		}

		HTREEITEM hItem_child = NULL;	//���ӽ��
		HTREEITEM hItem_checked = pData->hItem;//��ǰ������
		BOOL bCheck = (BOOL)lParam;		//��ǰ״̬
		DWORD_PTR p;					//ָ������data_ptrָ��
		struct TreeItemData *tp = NULL;	//��������ָ���Ӧ�Ľ������
		int iNumOfSpeed;				//��������
		static BOOL bFlagChecked = FALSE;//��ʱ�洢�Ƿ��Ǵӵ�ǰ���̵�SetCheck���������ģ�����ǣ����ٴ���

		if(bFlagChecked == TRUE){
			bFlagChecked = FALSE;
			return 0;
		}
		if(hItem_checked==NULL){
			return 0;
		}

		if(!bCheck){//��check��û��check
			if(m_pCurrFileItem==NULL){
				return 0;
			}
			if(!g_pTree->ItemHasChildren(hItem_checked)){//����µ�Ҷ�ӽ��
				if(!g_pTree->IsChildNodeOf(hItem_checked, m_pCurrFileItem)){
					//���ǵ�ǰ�ļ�����Ľ��,�����ϲ��ó���
					return 0;
				}
				//�ǵ�ǰ�ļ�����Ľ��
				if(g_pTree->GetChildrenCheckedCount(m_pCurrFileItem)<=0){//��û�б�������
					g_TestDataTreeMgt.ResetNodeStatus();
					m_pCurrFileItem = NULL;
					g_pRightDrawAreaView->RedrawWindow();
					return 0;
				}
				//��ʣ��������㱻ѡ�У�������յ�ǰѡ�еĽ���״̬����
				p = g_pTree->GetItemData(hItem_checked);
				tp = (struct TreeItemData *)p;
				ASSERT(tp);
				tp->bSelected = FALSE;
				g_pRightDrawAreaView->RedrawWindow();
				return 0;
			}
			else{//��Ҷ�ӽڵ�,�������ļ����,Ҳ�������ͺŽ��
				hItem_child = g_pTree->GetChildItem(hItem_checked);
				if(!g_pTree->ItemHasChildren(hItem_child)){//���ļ����
					if(hItem_checked==m_pCurrFileItem){//��ǰ�ļ����ӵ��е�û����
						m_pCurrFileItem = NULL;
						g_TestDataTreeMgt.ResetNodeStatus();
						g_pRightDrawAreaView->RedrawWindow();
						return 0;
					}
					else{//ѡ���������
						//�����������Ŀǰ�������
						bFlagChecked = TRUE;
						g_pTree->SetCheck(hItem_checked);
						g_pRightDrawAreaView->RedrawWindow();
						return 0;
					}
				}
				else{//���ͺŽ��
					m_pCurrFileItem = NULL;
					g_TestDataTreeMgt.ResetNodeStatus();
					g_pRightDrawAreaView->RedrawWindow();
					return 0;
				}					
			}
		}
		else{//��uncheck��check
			if(!g_pTree->ItemHasChildren(hItem_checked)){//����µ�Ҷ�ӽ��
				if(m_pCurrFileItem==NULL || g_pTree->IsChildNodeOf(hItem_checked, m_pCurrFileItem)){
					//��ǰδѡ�У���ǰѡ�е��ǵ�ǰ�ļ�����Ľ��
					p = g_pTree->GetItemData(hItem_checked);
					tp = (struct TreeItemData *)p;
					ASSERT(tp);
					tp->bSelected = TRUE;
					m_pCurrFileItem = g_pTree->GetParentItem(hItem_checked);
					g_pRightDrawAreaView->RedrawWindow();
					return 0;
				}
				else{
					//��ǰ��ѡ�У����ҵ�ǰѡ�е��������ļ���������Ҷ�ӽ��
					//�����������Ŀǰֻ�ǲ���ѡ
					bFlagChecked = TRUE;
					g_pTree->SetCheck(hItem_checked, FALSE);
					g_pRightDrawAreaView->RedrawWindow();
					return FALSE;
				}
			}
			else{//��Ҷ�ӽڵ�,�������ļ����,Ҳ�������ͺŽ��
				hItem_child = g_pTree->GetChildItem(hItem_checked);
				if(g_pTree->GetChildItem(hItem_child)==NULL){//���ļ����
					if(m_pCurrFileItem==NULL || hItem_checked==m_pCurrFileItem){
						//��ǰδѡ�У���ǰѡ�е��ǵ�ǰ�ļ����
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
					else{//��ǰ��ѡ���ҵ�ǰѡ�ĺ��ϴβ�һ���������������Ŀǰ�Ȳ���������
						bFlagChecked = TRUE;
						g_pTree->SetCheck(hItem_checked, FALSE);
						g_pRightDrawAreaView->RedrawWindow();
						return 0;
					}
				}
				else{//���ͺŽ��
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
