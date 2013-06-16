
// LeftView.cpp : implementation of the CLeftView class
//

#include "stdafx.h"
#include "PdfClimate.h"

#include "PdfClimateDoc.h"
#include "LeftView.h"
#include "MainFrm.h"
#include "PdfClimateView.h"
#include "CtrlLib/XHtmlTree/XHtmlTree.h"
#include "resource.h"
#include "MyPdf.h"
#include "GlobalVars.h"
#include "DocInfoView.h"
#include "DottedGraphView.h"
#include "FacetGraphView.h"
#include "LinearGraphView.h"
#include "ColumnarGraphView.h"
#include "UnknownGraphView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CView)

BEGIN_MESSAGE_MAP(CLeftView, CView)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_NOTIFY(NM_DBLCLK, IDC_LEFT_TREE, OnNMDblclk)
END_MESSAGE_MAP()


// CLeftView construction/destruction

CLeftView::CLeftView()
{
    m_iBaseDocumentNum = 0;
    memset(m_pBaseDocumentList, 0, sizeof(m_pBaseDocumentList));
    m_iCurrDocumentIndex = -1;

    mupdf_init();

    m_pTreeCtrl = NULL;
}

CLeftView::~CLeftView()
{
    int i;

    for (i=0; i<m_iBaseDocumentNum; i++) {
        delete m_pBaseDocumentList[i];
    }

    mupdf_destroy();

	if(m_pTreeCtrl!=NULL){
		delete m_pTreeCtrl;
	}
}

int CLeftView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	TRACE(_T("in CTreeView::OnCreate\n"));

	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pTreeCtrl = new CXHtmlTree();
	ASSERT(m_pTreeCtrl);

	DWORD dwStyle = TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS | 
					WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP | WS_BORDER;

	CRect rect(0,0,100,100);

	VERIFY(m_pTreeCtrl->Create(dwStyle, rect, this, IDC_LEFT_TREE));

	return 0;
}

void CLeftView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (m_pTreeCtrl && ::IsWindow(m_pTreeCtrl->m_hWnd))	{
		m_pTreeCtrl->MoveWindow(0, 0, cx, cy);
	}
}

void CLeftView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
}

const DWORD STANDARD_PALETTE[] = {00,51,102,153,204,255};
const INT STANDARD_COLOR_SIZE = 6;
const INT STANDARD_PALETTE_VAL_DIF = 51;
static DWORD dwColorMapTable[216] = {0};

void SetMapTable()
{
	int  nColorMapIdx = 0;
	for (int nBlueIdx = 0; nBlueIdx < STANDARD_COLOR_SIZE; ++nBlueIdx)
	{
		for(int nGreenIdx = 0; nGreenIdx < STANDARD_COLOR_SIZE; ++nGreenIdx)
		{
			for(int nRedIdx = 0; nRedIdx < STANDARD_COLOR_SIZE; ++nRedIdx)
			{
				RGBQUAD objColor;
				objColor.rgbRed = (BYTE)STANDARD_PALETTE[nRedIdx];
				objColor.rgbGreen = (BYTE)STANDARD_PALETTE[nGreenIdx];
				objColor.rgbBlue = (BYTE)STANDARD_PALETTE[nBlueIdx];
				objColor.rgbReserved = 0;
				memcpy(&dwColorMapTable[nColorMapIdx],&objColor,sizeof(RGBQUAD));
				++nColorMapIdx;
			}
		}
	}
}

UINT GetPixelValue(DWORD uPixelValue_i)
{
    UINT uRetValue = 0;
    UINT uPos = uPixelValue_i / STANDARD_PALETTE_VAL_DIF;
    if(0 == uPixelValue_i % STANDARD_PALETTE_VAL_DIF)
    {
        uRetValue = uPixelValue_i/STANDARD_PALETTE_VAL_DIF;
    }
    else
    {
        if(abs((double)(uPixelValue_i - STANDARD_PALETTE [uPos])) > 
           abs((double)(uPixelValue_i - STANDARD_PALETTE [uPos+1])))
        {
            uRetValue = uPos+1;
        }
        else
        {
            uRetValue = uPos;
        }
    }
    return uRetValue;
}

// Get Index of suitable color data in the palette table.
static int Color2Index(COLORREF color)
{
	DWORD dwPixelVal = color;

	int nBlue = dwPixelVal >> 16;
	int nGreen = (dwPixelVal & 0x00FF00) >> 8 ;
	int nRed = dwPixelVal & 0x0000FF;

	UINT uRedValue = GetPixelValue(nRed);
	UINT uGreenValue = GetPixelValue(nGreen);
	UINT uBlueValue = GetPixelValue(nBlue);

	UINT uPalettePos = uBlueValue*36+uGreenValue*6+uRedValue;

	return (int)uPalettePos;
}

void CLeftView::OnInitialUpdate()
{
    CView::OnInitialUpdate();

    SetMapTable();

	m_ilColorList.DeleteImageList();
	m_ilColorList.Create(IDB_COLOR_LIST, 16, 1, RGB(255,255,255));
	m_pTreeCtrl->DeleteAllItems();
	m_pTreeCtrl->SetImageList(&m_ilColorList, TVSIL_NORMAL);
	m_pTreeCtrl->Initialize(FALSE, TRUE);
	m_pTreeCtrl->SetSmartCheckBox(FALSE);
	m_pTreeCtrl->SetHtml(TRUE);
	m_pTreeCtrl->SetImages(TRUE);
	m_pTreeCtrl->SetTextColor(RGB(0, 0, 0));
}


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

CPdfClimateDoc* CLeftView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPdfClimateDoc)));
    return (CPdfClimateDoc*)m_pDocument;
}
#endif //_DEBUG


void CLeftView::openFile()
{
    if (m_iBaseDocumentNum >= MAX_BASE_DOCUMENT_NUM) {
        return;
    }

    char sDocPath[256];

    CFileDialog dlg(TRUE, 
                    "PDF Files (*.pdf)", 
                    NULL, 
                    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING,
                    _T("PDF Files (*.pdf)|*.pdf|All Files(*.*)|*.*||"),
                    NULL
                   );
    if (dlg.DoModal()==IDCANCEL) {
        return;
    }
    sprintf(sDocPath, "%s", dlg.GetPathName().GetBuffer(dlg.GetPathName().GetLength()));

    //gb2312 to unicode
    int wLen = MultiByteToWideChar(CP_ACP, 0, sDocPath, -1, NULL, 0);
    LPWSTR wStr = new WCHAR[wLen];
    MultiByteToWideChar(CP_ACP, 0, sDocPath, -1, wStr, wLen);
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

    ::AfxGetMainWnd()->SetWindowTextA(sDocPath);

    char sDocName[256];
    sprintf(sDocName, "%s", dlg.GetFileName().GetBuffer(dlg.GetFileName().GetLength()));

    m_pBaseDocumentList[m_iBaseDocumentNum] = new CBaseDocument();
    m_pBaseDocumentList[m_iBaseDocumentNum]->setName(sDocName);
    m_pBaseDocumentList[m_iBaseDocumentNum]->setPath(sDocPath);

    if (strcmp(_strlwr(sDocName + strlen(sDocName) - 4), ".pdf") == 0) {
        sDocName[strlen(sDocName)-4] = 0;
    }

	TV_INSERTSTRUCT tvRoot;
	tvRoot.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvRoot.hParent = NULL;
	tvRoot.item.pszText = (LPSTR)m_pBaseDocumentList[m_iBaseDocumentNum]->getName();
	tvRoot.item.iImage = Color2Index(RGB(0, 0, 255));
	tvRoot.item.iSelectedImage = Color2Index(RGB(0, 0, 255));

    HTREEITEM hItem = m_pTreeCtrl->InsertItem(&tvRoot);

    m_pTreeCtrl->SetItemData(hItem, (DWORD_PTR)m_pBaseDocumentList[m_iBaseDocumentNum]);
    m_pBaseDocumentList[m_iBaseDocumentNum]->setTreeItem(hItem);

    tvRoot.hParent = hItem;
    tvRoot.item.pszText = "µã×´Í¼";
    tvRoot.item.iImage = Color2Index(RGB(0, 255, 0));
	tvRoot.item.iSelectedImage = Color2Index(RGB(0, 255, 0));
    m_pTreeCtrl->InsertItem(&tvRoot);

    tvRoot.item.pszText = "Ãæ×´Í¼";
    tvRoot.item.iImage = Color2Index(RGB(0, 255, 0));
	tvRoot.item.iSelectedImage = Color2Index(RGB(0, 255, 0));
    m_pTreeCtrl->InsertItem(&tvRoot);

    tvRoot.item.pszText = "Ïß×´×´Í¼";
    tvRoot.item.iImage = Color2Index(RGB(0, 255, 0));
	tvRoot.item.iSelectedImage = Color2Index(RGB(0, 255, 0));
    m_pTreeCtrl->InsertItem(&tvRoot);

    tvRoot.item.pszText = "Öù×´Í¼";
    tvRoot.item.iImage = Color2Index(RGB(0, 255, 0));
	tvRoot.item.iSelectedImage = Color2Index(RGB(0, 255, 0));
    m_pTreeCtrl->InsertItem(&tvRoot);

    tvRoot.item.pszText = "ÆÕÍ¨Í¼";
    tvRoot.item.iImage = Color2Index(RGB(0, 255, 0));
	tvRoot.item.iSelectedImage = Color2Index(RGB(0, 255, 0));
    m_pTreeCtrl->InsertItem(&tvRoot);

    if (m_iCurrDocumentIndex >=0 ){
        HTREEITEM hItemOld = (HTREEITEM)m_pBaseDocumentList[m_iCurrDocumentIndex]->getTreeItem();
        m_pTreeCtrl->SetItemImage(hItemOld, Color2Index(RGB(0, 0, 0)), Color2Index(RGB(0, 0, 0)));
    }

    m_iCurrDocumentIndex = m_iBaseDocumentNum;
    m_iBaseDocumentNum ++;
    m_pTreeCtrl->CollapseAll();
    m_pTreeCtrl->Expand(hItem, TVE_EXPAND);
    m_pTreeCtrl->SelectItem(hItem);

    g_pPdfClimateView->openFile();
    updateView();
}

void CLeftView::closeFile()
{
    int index;
    HTREEITEM hItem = m_pTreeCtrl->GetSelectedItem();
    if (!hItem) {
        return;
    }

    DWORD_PTR ptr = m_pTreeCtrl->GetItemData(hItem);
    CBaseGraph *pBaseGraph = (CBaseGraph*)ptr;

    for (int i=0; i<m_iBaseDocumentNum; i++) {
        if (ptr == (DWORD_PTR)m_pBaseDocumentList[i]) {
            index = i;
            break;
        }
    }

    if (index<0 || index>=m_iBaseDocumentNum) {
        return;
    }

    if (m_iCurrDocumentIndex==index) {
        g_pPdfClimateView->closeFile();
        mupdf_close_file();
        m_iCurrDocumentIndex = -1;
        updateView();
    }

    m_pTreeCtrl->DeleteItem((HTREEITEM)(m_pBaseDocumentList[index]->getTreeItem()));
    delete m_pBaseDocumentList[index];
    for (int i=index; i<m_iBaseDocumentNum-1; i++) {
        m_pBaseDocumentList[i] = m_pBaseDocumentList[i++];
    }
    m_pBaseDocumentList[m_iBaseDocumentNum-1] = 0;
    m_iBaseDocumentNum --;
}

void CLeftView::selectFile(int index)
{
    if (index<0 || index>=m_iBaseDocumentNum) {
        return;
    }

    if (m_iCurrDocumentIndex==index) {
        return;
    }

    if (m_iCurrDocumentIndex >= 0) {
        HTREEITEM hItemOld = (HTREEITEM)m_pBaseDocumentList[m_iCurrDocumentIndex]->getTreeItem();
        m_pTreeCtrl->SetItemImage(hItemOld, Color2Index(RGB(0, 0, 0)), Color2Index(RGB(0, 0, 0)));

        g_pPdfClimateView->closeFile();
        mupdf_close_file();
    }

    char sDocPath[256];
    sprintf(sDocPath, "%s", m_pBaseDocumentList[index]->getPath());

    //gb2312 to unicode
    int wLen = MultiByteToWideChar(CP_ACP, 0, sDocPath, -1, NULL, 0);
    LPWSTR wStr = new WCHAR[wLen];
    MultiByteToWideChar(CP_ACP, 0, sDocPath, -1, wStr, wLen);
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

    m_iCurrDocumentIndex = index;
    ::AfxGetMainWnd()->SetWindowTextA(sDocPath);
    HTREEITEM hItem = (HTREEITEM)m_pBaseDocumentList[m_iCurrDocumentIndex]->getTreeItem();
    m_pTreeCtrl->SetItemImage(hItem, Color2Index(RGB(0, 0, 255)), Color2Index(RGB(0, 0, 255)));
    m_pTreeCtrl->CollapseAll();
    m_pTreeCtrl->Expand(hItem, TVE_EXPAND);
    m_pTreeCtrl->SelectItem(hItem);

    g_pPdfClimateView->openFile();
    updateView();
}

int CLeftView::getCurrDocIndex()
{
    //TODO, set item index to real item
    return m_iCurrDocumentIndex;
}

void CLeftView::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
    HTREEITEM hItem = m_pTreeCtrl->GetSelectedItem();
    DWORD_PTR ptr = m_pTreeCtrl->GetItemData(hItem);

    CBaseGraph *pBaseGraph = (CBaseGraph*)ptr;

    for (int i=0; i<m_iBaseDocumentNum; i++) {
        if (ptr == (DWORD_PTR)m_pBaseDocumentList[i]) {
            selectFile(i);
            break;
        }
    }
    m_pTreeCtrl->Expand(hItem, TVE_TOGGLE);

    *pResult = 0;
}

void CLeftView::updateView()
{
    g_pCurrDocument = m_iCurrDocumentIndex>=0 ? m_pBaseDocumentList[m_iCurrDocumentIndex] : NULL;
    g_pDocInfoView->loadDataFromDB();
    g_pDottedGraphView->loadDataFromDB();
    g_pFacetGraphView->loadDataFromDB();
    g_pLinearGraphView->loadDataFromDB();
    g_pColumnarGraphView->loadDataFromDB();
    g_pUnknownGraphView->loadDataFromDB();
}