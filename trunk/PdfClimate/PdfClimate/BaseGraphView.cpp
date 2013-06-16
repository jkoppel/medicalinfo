// GraphClassView.cpp : implementation file
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "BaseGraphView.h"
#include "resource.h"
#include "GlobalVars.h"
#include "ImageProcess/BaseDocument.h"
#include "ImageProcess/DottedGraph.h"
#include "ImageProcess/FacetGraph.h"
#include "ImageProcess/LinearGraph.h"
#include "ImageProcess/ColumnarGraph.h"
#include "ImageProcess/UnknownGraph.h"


// CBaseGraphView

IMPLEMENT_DYNCREATE(CBaseGraphView, CFormView)

void CBaseGraphView::initialize()
{
    m_pSrcImage = new CImage;
    m_pSrcBitmap = new CBitmap;
    m_bIsGraphInfoEditing = false;
    m_bIsCoorInfoEditing = false;
    m_bIsDragging = false;
    m_bDispCoorInfo = true;
    m_bDispDotedInfo = false;
    m_iDrawMode = Draw_None;
    m_iEditMode = Edit_None;
    m_iOldImageWidth = 0;
    m_iOldImageHeight = 0;
    m_ptCoorA = CPoint(-1, -1);
    m_ptCoorB = CPoint(-1, -1);
    m_hCross = AfxGetApp()->LoadStandardCursor(IDC_CROSS);
}

void CBaseGraphView::destroy()
{
    if (m_pSrcImage) {
        if (!m_pSrcImage->IsNull()) {
            m_pSrcImage->Detach();
        }
        delete m_pSrcBitmap;
        delete m_pSrcImage;
    }
}

CBaseGraphView::CBaseGraphView()
	: CFormView(CBaseGraphView::IDD)
{
    initialize();
    m_iGraphType = Graph_Unknown;
}

CBaseGraphView::CBaseGraphView(UINT nIDTemplate, GraphType iGraphType)
	: CFormView(nIDTemplate)
{
    initialize();
    m_iGraphType = iGraphType;
}

CBaseGraphView::~CBaseGraphView()
{
    destroy();
}

void CBaseGraphView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CMB_GRAPH_LIST, m_ctrlGraphList);
    DDX_Control(pDX, IDC_STATIC_MAIN_IMAGE, m_ctrlMainImage);
    DDX_Control(pDX, IDC_GRAPHINFO, m_ctrlGraphInfo);
    DDX_Control(pDX, IDC_GRAPHINFO_EDIT_NAME, m_ctrlGraphInfo_EditName);
    DDX_Control(pDX, IDC_GRAPHINFO_BTN_EDITSAVE, m_ctrlGraphInfo_BtnEditSave);
    DDX_Control(pDX, IDC_GRAPHINFO_EDIT_UNITNAME_X, m_ctrlGraphInfo_EditUnitName_X);
    DDX_Control(pDX, IDC_GRAPHINFO_EDIT_UNITNAME_Y, m_ctrlGraphInfo_EditUnitName_Y);
    DDX_Control(pDX, IDC_COORINFO_EDIT_COORA_PIX_VAL, m_ctrlCoorInfo_EditCoorAPixVal);
    DDX_Control(pDX, IDC_COORINFO_BTN_SNAP_COORA_PIX_VAL, m_ctrlCoorInfo_BtnSnapCoorA);
    DDX_Control(pDX, IDC_COORINFO_EDIT_COORA_X, m_ctrlCoorInfo_EditCoorA_X);
    DDX_Control(pDX, IDC_COORINFO_EDIT_COORA_Y, m_ctrlCoorInfo_EditCoorA_Y);
    DDX_Control(pDX, IDC_COORINFO_EDIT_COORB_PIX_VAL, m_ctrlCoorInfo_EditCoorBPixVal);
    DDX_Control(pDX, IDC_COORINFO_BTN_SNAP_COORB_PIX_VAL, m_ctrlCoorInfo_BtnSnapCoorB);
    DDX_Control(pDX, IDC_COORINFO_EDIT_COORB_X, m_ctrlCoorInfo_EditCoorB_X);
    DDX_Control(pDX, IDC_COORINFO_EDIT_COORB_Y, m_ctrlCoorInfo_EditCoorB_Y);
    DDX_Control(pDX, IDC_COORINFO_BTN_EDITSAVE, m_ctrlCoorInfo_BtnEditSave);
}

BEGIN_MESSAGE_MAP(CBaseGraphView, CFormView)
    ON_BN_CLICKED(IDC_COORINFO_BTN_EDITSAVE, &CBaseGraphView::OnBnClickedCoorinfoBtnEditsave)
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_BN_CLICKED(IDC_COORINFO_BTN_SNAP_COORA_PIX_VAL, &CBaseGraphView::OnBnClickedCoorinfoBtnSnapCooraPixVal)
    ON_BN_CLICKED(IDC_COORINFO_BTN_SNAP_COORB_PIX_VAL, &CBaseGraphView::OnBnClickedCoorinfoBtnSnapCoorbPixVal)
    ON_CBN_SELCHANGE(IDC_CMB_GRAPH_LIST, &CBaseGraphView::OnCbnSelchangeCmbGraphList)
END_MESSAGE_MAP()


// CBaseGraphView diagnostics

#ifdef _DEBUG
void CBaseGraphView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CBaseGraphView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CBaseGraphView message handlers


void CBaseGraphView::OnDraw(CDC* pDC)
{
    if (!m_pSrcImage->IsNull()) {
        CRect rect;
        int width, height;

        width = m_pSrcImage->GetWidth();
        height = m_pSrcImage->GetHeight();
        m_ctrlMainImage.SetWindowPos(NULL, 0, 0, width + IMAGE_X_OFFSET + 1,
                                                height + IMAGE_Y_OFFSET + 1,
                                                SWP_NOMOVE);
        pDC = m_ctrlMainImage.GetDC();
        m_pSrcImage->GetWidth();
        m_pSrcImage->Draw(pDC->m_hDC, IMAGE_X_OFFSET, IMAGE_Y_OFFSET, width, height);

        m_ctrlMainImage.GetWindowRect(rect);
        ScreenToClient(rect);
        if (m_iDrawMode == Draw_SnapCoorA || m_iDrawMode == Draw_SnapCoorB) {
            CClientDC dc(this);
            HBRUSH hb = (HBRUSH) GetStockObject(NULL_BRUSH);
            CBrush* brush = CBrush::FromHandle(hb);
            CBrush *pOldBrush = dc.SelectObject(brush);

            dc.MoveTo(rect.left + IMAGE_X_OFFSET, m_ptCurrent.y);
            dc.LineTo(rect.right, m_ptCurrent.y);
            dc.MoveTo(m_ptCurrent.x, rect.top + IMAGE_Y_OFFSET);
            dc.LineTo(m_ptCurrent.x, rect.bottom);

            dc.SetPixel(m_ptCurrent.x, m_ptCurrent.y, RGB(255, 0, 0));
            dc.SetPixel(m_ptCurrent.x-1, m_ptCurrent.y, RGB(255, 0, 0));
            dc.SetPixel(m_ptCurrent.x+1, m_ptCurrent.y, RGB(255, 0, 0));
            dc.SetPixel(m_ptCurrent.x, m_ptCurrent.y-1, RGB(255, 0, 0));
            dc.SetPixel(m_ptCurrent.x, m_ptCurrent.y+1, RGB(255, 0, 0));

            dc.SelectObject(pOldBrush);
        }
        if(m_bDispCoorInfo) {
            CClientDC dc(this);
            HBRUSH hb = (HBRUSH) GetStockObject(NULL_BRUSH);
            CBrush* brush = CBrush::FromHandle(hb);
            CBrush *pOldBrush = dc.SelectObject(brush);

            if (m_iDrawMode != Draw_SnapCoorA) {
                m_ptCurrent.x = m_ptCoorA.x + rect.left + IMAGE_X_OFFSET;
                m_ptCurrent.y = m_ptCoorA.y + rect.top + IMAGE_Y_OFFSET;
                dc.SetPixel(m_ptCurrent.x, m_ptCurrent.y, RGB(255, 0, 0));
                dc.SetPixel(m_ptCurrent.x-1, m_ptCurrent.y, RGB(255, 0, 0));
                dc.SetPixel(m_ptCurrent.x+1, m_ptCurrent.y, RGB(255, 0, 0));
                dc.SetPixel(m_ptCurrent.x, m_ptCurrent.y-1, RGB(255, 0, 0));
                dc.SetPixel(m_ptCurrent.x, m_ptCurrent.y+1, RGB(255, 0, 0));
            }
            if (m_iDrawMode != Draw_SnapCoorB) {
                m_ptCurrent.x = m_ptCoorB.x + rect.left + IMAGE_X_OFFSET;
                m_ptCurrent.y = m_ptCoorB.y + rect.top + IMAGE_Y_OFFSET;
                dc.SetPixel(m_ptCurrent.x, m_ptCurrent.y, RGB(255, 0, 0));
                dc.SetPixel(m_ptCurrent.x-1, m_ptCurrent.y, RGB(255, 0, 0));
                dc.SetPixel(m_ptCurrent.x+1, m_ptCurrent.y, RGB(255, 0, 0));
                dc.SetPixel(m_ptCurrent.x, m_ptCurrent.y-1, RGB(255, 0, 0));
                dc.SetPixel(m_ptCurrent.x, m_ptCurrent.y+1, RGB(255, 0, 0));
            }
        }

        if (width != m_iOldImageWidth || height != m_iOldImageHeight) {
            int left, top, cx;

            m_ctrlGraphInfo.GetWindowRect(rect);
            ScreenToClient(rect);
            left= width + 30;
            cx = left - rect.left;
            top = rect.top;

            for (int i=0; i<m_lstGraphInfoItemID.GetCount(); i++){
                int id = m_lstGraphInfoItemID.GetAt(m_lstGraphInfoItemID.FindIndex(i));
                CWnd *pCtrl = GetDlgItem(id);
                pCtrl->GetWindowRect(rect);
                ScreenToClient(rect);
                rect.left += cx;
                rect.right += cx;
                pCtrl->MoveWindow(rect);
            }

            for (int i=0; i<m_lstCoorInfoItemID.GetCount(); i++){
                int id = m_lstCoorInfoItemID.GetAt(m_lstCoorInfoItemID.FindIndex(i));
                CWnd *pCtrl = GetDlgItem(id);
                pCtrl->GetWindowRect(rect);
                ScreenToClient(rect);
                rect.left += cx;
                rect.right += cx;
                pCtrl->MoveWindow(rect);
            }
            m_iOldImageWidth = width;
            m_iOldImageHeight = height;
        }
    }
}


void CBaseGraphView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();

    int ItemIDofGraphInfo[] = {
                                IDC_GRAPHINFO,
                                IDC_GRAPHINFO_LABEL_NAME,
                                IDC_GRAPHINFO_LABEL_UNITNAME_X,
                                IDC_GRAPHINFO_LABEL_UNITNAME_Y,
                                IDC_GRAPHINFO_EDIT_NAME,
                                IDC_GRAPHINFO_EDIT_UNITNAME_X,
                                IDC_GRAPHINFO_EDIT_UNITNAME_Y,
                                IDC_GRAPHINFO_BTN_EDITSAVE
                              };
    int ItemIDofCoorInfo[] = {
                                IDC_COORINFO,
                                IDC_COORINFO_BTN_EDITSAVE,
                                IDC_COORINFO_BTN_SNAP_COORA_PIX_VAL,
                                IDC_COORINFO_BTN_SNAP_COORB_PIX_VAL,
                                IDC_COORINFO_LABEL_COORA,
                                IDC_COORINFO_LABEL_COORB,
                                IDC_COORINFO_LABEL_COORA_PIX_VAL,
                                IDC_COORINFO_LABEL_COORA_X,
                                IDC_COORINFO_LABEL_COORA_Y,
                                IDC_COORINFO_LABEL_COORB_PIX_VAL,
                                IDC_COORINFO_LABEL_COORB_X,
                                IDC_COORINFO_LABEL_COORB_Y,
                                IDC_COORINFO_EDIT_COORA_PIX_VAL,
                                IDC_COORINFO_EDIT_COORB_PIX_VAL,
                                IDC_COORINFO_EDIT_COORA_X,
                                IDC_COORINFO_EDIT_COORA_Y,
                                IDC_COORINFO_EDIT_COORB_X,
                                IDC_COORINFO_EDIT_COORB_Y,
                             };

    int i;
    for (i=0; i < sizeof(ItemIDofGraphInfo)/sizeof(int); i++) {
        m_lstGraphInfoItemID.AddTail(ItemIDofGraphInfo[i]);
    }
    for (i=0; i < sizeof(ItemIDofCoorInfo)/sizeof(int); i++) {
        m_lstCoorInfoItemID.AddTail(ItemIDofCoorInfo[i]);
    }
}


void CBaseGraphView::OnBnClickedGraphinfoBtnEditsave()
{
    if (m_bIsGraphInfoEditing && g_pCurrDocument) {
        char sName[256], sXUnitName[256], sYUnitName[256];
        m_ctrlGraphInfo_EditName.GetWindowText(sName, sizeof(sName));
        if (strlen(sName) == 0) {
            AfxMessageBox("ÇëÊäÈëÍ¼ÐÎÃû³Æ");
            return;
        }
        m_ctrlGraphInfo_EditUnitName_X.GetWindowText(sXUnitName, sizeof(sXUnitName));
        m_ctrlGraphInfo_EditUnitName_Y.GetWindowText(sYUnitName, sizeof(sYUnitName));
        if (m_iEditMode==Edit_Adding) {
            CBaseGraph *pBaseGraph;
            int iID = g_pCurrDocument->getFreeGraphID(m_iGraphType);

            switch (m_iGraphType) {
                case Graph_Dotted: {
                    CDottedGraph *dottedGraph = new CDottedGraph(iID, sName, sXUnitName, sYUnitName);
                    g_pCurrDocument->addGraph(m_iGraphType, dottedGraph);
                    pBaseGraph = dottedGraph;
                }
                break;
                case Graph_Facet: {
                    CFacetGraph *facetGraph = new CFacetGraph(iID, sName, sXUnitName, sYUnitName);
                    g_pCurrDocument->addGraph(m_iGraphType, facetGraph);
                    pBaseGraph = facetGraph;
                }
                break;
                case Graph_Linear: {
                    CLinearGraph *linearGraph = new CLinearGraph(iID, sName, sXUnitName, sYUnitName);
                    g_pCurrDocument->addGraph(m_iGraphType, linearGraph);
                    pBaseGraph = linearGraph;
                }
                break;
                case Graph_Columnar: {
                    CColumnarGraph *columnarGraph = new CColumnarGraph(iID, sName, sXUnitName, sYUnitName);
                    g_pCurrDocument->addGraph(m_iGraphType, columnarGraph);
                    pBaseGraph = columnarGraph;
                }
                break;
                case Graph_Unknown: {
                    CUnknownGraph *unknownGraph = new CUnknownGraph(iID, sName, sXUnitName, sYUnitName);
                    g_pCurrDocument->addGraph(m_iGraphType, unknownGraph);
                    pBaseGraph = unknownGraph;
                }
                break;
            }

            BITMAP bmp;
            m_pSrcBitmap->GetBitmap(&bmp);
            unsigned char * bits = new unsigned char[bmp.bmWidthBytes * bmp.bmHeight];
            m_pSrcBitmap->GetBitmapBits(bmp.bmWidthBytes * bmp.bmHeight, bits);
            pBaseGraph->setBitmap(bmp);
            pBaseGraph->setBitmapBits(bmp.bmWidthBytes * bmp.bmHeight, bits);
            delete []bits;

            m_iCurrGraphIndex = g_pCurrDocument->getGraphNum(m_iGraphType) - 1;
            updateGraphList();
            m_iEditMode = Edit_Modifying;
        }
        else if (m_iEditMode==Edit_Modifying) {
            CBaseGraph *baseGraph = g_pCurrDocument->getGraph(m_iGraphType, m_iCurrGraphIndex);
            baseGraph->setName(sName);
            baseGraph->setXUnitName(sXUnitName);
            baseGraph->setYUnitName(sYUnitName);

            updateGraphList();
        }
    }

    m_ctrlGraphInfo_EditName.SetReadOnly(m_bIsGraphInfoEditing);
    m_ctrlGraphInfo_EditUnitName_X.SetReadOnly(m_bIsGraphInfoEditing);
    m_ctrlGraphInfo_EditUnitName_Y.SetReadOnly(m_bIsGraphInfoEditing);
    m_ctrlGraphInfo_BtnEditSave.SetWindowText(m_bIsGraphInfoEditing ? "±à¼­" : "±£´æ");

    m_bIsGraphInfoEditing = !m_bIsGraphInfoEditing;
}

void CBaseGraphView::OnBnClickedCoorinfoBtnEditsave()
{
    m_ctrlCoorInfo_EditCoorA_X.SetReadOnly(m_bIsCoorInfoEditing);
    m_ctrlCoorInfo_EditCoorA_Y.SetReadOnly(m_bIsCoorInfoEditing);
    m_ctrlCoorInfo_BtnSnapCoorA.EnableWindow(!m_bIsCoorInfoEditing);
    m_ctrlCoorInfo_EditCoorB_X.SetReadOnly(m_bIsCoorInfoEditing);
    m_ctrlCoorInfo_EditCoorB_Y.SetReadOnly(m_bIsCoorInfoEditing);
    m_ctrlCoorInfo_BtnSnapCoorB.EnableWindow(!m_bIsCoorInfoEditing);
    m_ctrlCoorInfo_BtnEditSave.SetWindowText(m_bIsCoorInfoEditing? "±à¼­" : "±£´æ");

    if (m_bIsCoorInfoEditing) {
        //TODO SAVE THE DATA
    }
    m_bIsCoorInfoEditing = !m_bIsCoorInfoEditing;
}

static bool PointInRect(CPoint point, CRect rect)
{
    return (rect.left <= point.x && point.x <= rect.right &&
            rect.top <= point.y && point.y <= rect.bottom);
}

void CBaseGraphView::OnLButtonDown(UINT nFlags, CPoint point)
{
    CRect rect;
    m_ctrlMainImage.GetWindowRect(rect);
    ScreenToClient(rect);

    if (!m_pSrcImage->IsNull() && PointInRect(point, rect) && m_iDrawMode!=Draw_None) {
        char buf[256];

        SetCapture();
        if (m_iDrawMode == Draw_SnapCoorA || m_iDrawMode == Draw_SnapCoorB) {
    	    //::SetCursor(m_hCross);
            ::ShowCursor(false);
        }
        if (m_iDrawMode == Draw_SnapCoorA) {
            sprintf(buf, "(%d, %d)", point.x - rect.left, point.y - rect.top);
            m_ctrlCoorInfo_EditCoorAPixVal.SetWindowText(buf);
        }
        if (m_iDrawMode == Draw_SnapCoorB) {
            sprintf(buf, "(%d, %d)", point.x - rect.left, point.y - rect.top);
            m_ctrlCoorInfo_EditCoorBPixVal.SetWindowText(buf);
        }

        m_ptCurrent = point;
        RedrawWindow();
	    m_bIsDragging = true;
    }

    CView::OnLButtonDown(nFlags, point);
}

void CBaseGraphView::OnMouseMove(UINT nFlags, CPoint point)
{
    CRect rect;
    m_ctrlMainImage.GetWindowRect(rect);
    ScreenToClient(rect);

    if (!m_pSrcImage->IsNull() && PointInRect(point, rect) && m_iDrawMode!=Draw_None &&
        m_bIsDragging) {
        char buf[256];

        sprintf(buf, "(%d, %d)",
                point.x - rect.left - IMAGE_X_OFFSET,
                point.y - rect.top - IMAGE_Y_OFFSET);
        if (m_iDrawMode == Draw_SnapCoorA) {
            m_ctrlCoorInfo_EditCoorAPixVal.SetWindowText(buf);
        }
        if (m_iDrawMode == Draw_SnapCoorB) {
            m_ctrlCoorInfo_EditCoorBPixVal.SetWindowText(buf);
        }

        m_ptCurrent = point;
        RedrawWindow();
    }

    CView::OnMouseMove(nFlags, point);
}

void CBaseGraphView::OnLButtonUp(UINT nFlags, CPoint point)
{
    CRect rect;
    m_ctrlMainImage.GetWindowRect(rect);
    ScreenToClient(rect);

    if (!m_pSrcImage->IsNull() && PointInRect(point, rect) && m_iDrawMode!=Draw_None &&
        m_bIsDragging) {
        if (m_iDrawMode == Draw_SnapCoorA) {
            m_ptCoorA = CPoint(point.x - rect.left - IMAGE_X_OFFSET, point.y - rect.top - IMAGE_Y_OFFSET);
        }
        if (m_iDrawMode == Draw_SnapCoorB) {
            m_ptCoorB = CPoint(point.x - rect.left - IMAGE_X_OFFSET, point.y - rect.top - IMAGE_Y_OFFSET);
        }

        m_bIsDragging = false;
		ReleaseCapture();
        m_iDrawMode = Draw_None;
        ::ShowCursor(true);
        RedrawWindow();
    }

    CView::OnLButtonUp(nFlags, point);
}


void CBaseGraphView::OnBnClickedCoorinfoBtnSnapCooraPixVal()
{
    m_iDrawMode = Draw_SnapCoorA;
}


void CBaseGraphView::OnBnClickedCoorinfoBtnSnapCoorbPixVal()
{
    m_iDrawMode = Draw_SnapCoorB;
}

void CBaseGraphView::onAddGraph()
{
    m_bIsGraphInfoEditing = false;
    m_bIsCoorInfoEditing = false;
    m_bIsDragging = false;
    m_bDispCoorInfo = true;
    m_bDispDotedInfo = false;
    m_iDrawMode = Draw_None;
    m_iEditMode = Edit_Adding;
    m_iOldImageWidth = 0;
    m_iOldImageHeight = 0;
    m_ptCoorA = CPoint(-1, -1);
    m_ptCoorB = CPoint(-1, -1);

    m_ctrlGraphInfo_EditName.SetReadOnly();
    m_ctrlGraphInfo_EditName.SetWindowText("");
    m_ctrlGraphInfo_BtnEditSave.SetWindowText("±à¼­");
    m_ctrlGraphInfo_EditUnitName_X.SetReadOnly();
    m_ctrlGraphInfo_EditUnitName_X.SetWindowText("");
    m_ctrlGraphInfo_EditUnitName_Y.SetReadOnly();
    m_ctrlGraphInfo_EditUnitName_Y.SetWindowText("");
    m_ctrlCoorInfo_EditCoorAPixVal.SetWindowText("");
    m_ctrlCoorInfo_EditCoorA_X.SetReadOnly();
    m_ctrlCoorInfo_EditCoorA_X.SetWindowText("");
    m_ctrlCoorInfo_EditCoorA_Y.SetReadOnly();
    m_ctrlCoorInfo_EditCoorA_Y.SetWindowText("");
    m_ctrlCoorInfo_BtnSnapCoorA.EnableWindow(false);
    m_ctrlCoorInfo_EditCoorBPixVal.SetWindowText("");
    m_ctrlCoorInfo_EditCoorB_X.SetReadOnly();
    m_ctrlCoorInfo_EditCoorB_X.SetWindowText("");
    m_ctrlCoorInfo_EditCoorB_Y.SetReadOnly();
    m_ctrlCoorInfo_EditCoorB_Y.SetWindowText("");
    m_ctrlCoorInfo_BtnSnapCoorB.EnableWindow(false);
    m_ctrlCoorInfo_BtnEditSave.SetWindowText("±à¼­");
}

void CBaseGraphView::loadData()
{
    if (m_iCurrGraphIndex >= 0) {
        char sName[256], sXUnitName[256], sYUnitName[256];
        CBaseGraph *graph;

        graph = g_pCurrDocument->getGraph(m_iGraphType, m_iCurrGraphIndex);
        graph->getName(sName, sizeof(sName));
        m_ctrlGraphInfo_EditName.SetWindowText(sName);
        graph->getXUnitName(sXUnitName, sizeof(sXUnitName));
        m_ctrlGraphInfo_EditUnitName_X.SetWindowText(sXUnitName);
        graph->getYUnitName(sYUnitName, sizeof(sYUnitName));
        m_ctrlGraphInfo_EditUnitName_Y.SetWindowText(sYUnitName);

        BITMAP bmp;   
        graph->getBitmap(&bmp);
        unsigned char *bits = new unsigned char[bmp.bmWidthBytes * bmp.bmHeight];
        graph->getBitmapBits(bmp.bmWidthBytes * bmp.bmHeight, bits);

        m_pSrcBitmap->DeleteObject();
        m_pSrcBitmap->CreateBitmap(bmp.bmWidth, bmp.bmHeight, bmp.bmPlanes, bmp.bmBitsPixel, bits);
        m_pSrcImage->Detach();
        m_pSrcImage->Attach(*m_pSrcBitmap);
    }
    else {
        m_ctrlGraphInfo_EditName.SetWindowText("");
        m_ctrlGraphInfo_EditUnitName_X.SetWindowText("");
        m_ctrlGraphInfo_EditUnitName_Y.SetWindowText("");
    }
    updateGraphList();
}

void CBaseGraphView::loadDataFromDB()
{
    if (g_pCurrDocument && g_pCurrDocument->getGraphNum(m_iGraphType) > 0) {
        m_iCurrGraphIndex = 0;
    }
    else{
        m_iCurrGraphIndex = -1;
    }

    loadData();
}

void CBaseGraphView::updateGraphList()
{
    int num, i;
    char sName[256], buf[256];

    m_ctrlGraphList.ResetContent();
    num = g_pCurrDocument->getGraphNum(m_iGraphType);
    for (i=0; i<num; i++) {
        g_pCurrDocument->getGraph(m_iGraphType, i)->getName(sName, sizeof(sName));
        snprintf(buf, sizeof(buf), "%d. %s", i+1, sName);
        m_ctrlGraphList.AddString(buf);
    }
    m_ctrlGraphList.SetCurSel(m_iCurrGraphIndex);

    UpdateData();
    RedrawWindow();
}


void CBaseGraphView::OnCbnSelchangeCmbGraphList()
{
    int iCurSel = m_ctrlGraphList.GetCurSel();

    m_iCurrGraphIndex = iCurSel;
    loadData();
}
