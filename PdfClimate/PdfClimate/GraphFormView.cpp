// GraphClassView.cpp : implementation file
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "GraphFormView.h"
#include "resource.h"


// CGraphFormView

IMPLEMENT_DYNCREATE(CGraphFormView, CFormView)

void CGraphFormView::initialize()
{
    m_pSrcImage = new CImage;
    m_pSrcBitmap = new CBitmap;
    m_bIsGraphInfoEditing = false;
    m_bIsCoorInfoEditing = false;
    m_bIsDragging = false;
    m_bDispCoorInfo = true;
    m_bDispDotedInfo = false;
    m_iDrawMode = Draw_None;
    m_iOldImageWidth = 0;
    m_iOldImageHeight = 0;
    m_ptCoorA = CPoint(-1, -1);
    m_ptCoorB = CPoint(-1, -1);
    m_hCross = AfxGetApp()->LoadStandardCursor(IDC_CROSS);
}

void CGraphFormView::destroy()
{
    if (m_pSrcImage) {
        if (!m_pSrcImage->IsNull()) {
            m_pSrcImage->Detach();
        }
        delete m_pSrcBitmap;
        delete m_pSrcImage;
    }
}

CGraphFormView::CGraphFormView()
	: CFormView(CGraphFormView::IDD)
{
    initialize();
}

CGraphFormView::CGraphFormView(UINT nIDTemplate)
	: CFormView(nIDTemplate)
{
    initialize();
}

CGraphFormView::~CGraphFormView()
{
    destroy();
}

void CGraphFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CMB_GRAPH_LIST, m_ctrlGraphList);
    DDX_Control(pDX, IDC_STATIC_MAIN_IMAGE, m_ctrlMainImage);
    DDX_Control(pDX, IDC_GRAPHINFO, m_ctrlGraphInfo);
    DDX_Control(pDX, IDC_GRAPHINFO_EDIT_NAME, m_ctrlGraphInfo_EditName);
    DDX_Control(pDX, IDC_GRAPHINFO_BTN_EDITSAVE, m_ctrlGraphInfo_BtnEditSave);
    DDX_Control(pDX, IDC_COORINFO_EDIT_UNITNAME, m_ctrlCoorInfo_EditUnitName);
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

BEGIN_MESSAGE_MAP(CGraphFormView, CFormView)
    ON_BN_CLICKED(IDC_GRAPHINFO_BTN_EDITSAVE, &CGraphFormView::OnBnClickedGraphinfoBtnEditsave)
    ON_BN_CLICKED(IDC_COORINFO_BTN_EDITSAVE, &CGraphFormView::OnBnClickedCoorinfoBtnEditsave)
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_BN_CLICKED(IDC_COORINFO_BTN_SNAP_COORA_PIX_VAL, &CGraphFormView::OnBnClickedCoorinfoBtnSnapCooraPixVal)
    ON_BN_CLICKED(IDC_COORINFO_BTN_SNAP_COORB_PIX_VAL, &CGraphFormView::OnBnClickedCoorinfoBtnSnapCoorbPixVal)
END_MESSAGE_MAP()


// CGraphFormView diagnostics

#ifdef _DEBUG
void CGraphFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CGraphFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CGraphFormView message handlers


void CGraphFormView::OnDraw(CDC* pDC)
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


void CGraphFormView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();

    int ItemIDofGraphInfo[] = {
                                IDC_GRAPHINFO,
                                IDC_GRAPHINFO_LABEL_NAME,
                                IDC_GRAPHINFO_EDIT_NAME,
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
                                IDC_COORINFO_LABEL_UNITNAME,
                                IDC_COORINFO_EDIT_COORA_PIX_VAL,
                                IDC_COORINFO_EDIT_COORB_PIX_VAL,
                                IDC_COORINFO_EDIT_COORA_X,
                                IDC_COORINFO_EDIT_COORA_Y,
                                IDC_COORINFO_EDIT_COORB_X,
                                IDC_COORINFO_EDIT_COORB_Y,
                                IDC_COORINFO_EDIT_UNITNAME,
                             };

    int i;
    for (i=0; i < sizeof(ItemIDofGraphInfo)/sizeof(int); i++) {
        m_lstGraphInfoItemID.AddTail(ItemIDofGraphInfo[i]);
    }
    for (i=0; i < sizeof(ItemIDofCoorInfo)/sizeof(int); i++) {
        m_lstCoorInfoItemID.AddTail(ItemIDofCoorInfo[i]);
    }
}


void CGraphFormView::OnBnClickedGraphinfoBtnEditsave()
{
    m_ctrlGraphInfo_EditName.SetReadOnly(m_bIsGraphInfoEditing);
    m_ctrlGraphInfo_BtnEditSave.SetWindowText(m_bIsGraphInfoEditing ? "±à¼­" : "±£´æ");

    if (m_bIsGraphInfoEditing) {
        //TODO SAVE THE DATA
    }
    m_bIsGraphInfoEditing = !m_bIsGraphInfoEditing;
}

void CGraphFormView::OnBnClickedCoorinfoBtnEditsave()
{

    m_ctrlCoorInfo_EditUnitName.SetReadOnly(m_bIsCoorInfoEditing);
    //m_ctrlCoorInfo_EditCoorAPixVal.SetReadOnly(m_bIsCoorInfoEditing);
    m_ctrlCoorInfo_EditCoorA_X.SetReadOnly(m_bIsCoorInfoEditing);
    m_ctrlCoorInfo_EditCoorA_Y.SetReadOnly(m_bIsCoorInfoEditing);
    m_ctrlCoorInfo_BtnSnapCoorA.EnableWindow(!m_bIsCoorInfoEditing);
    //m_ctrlCoorInfo_EditCoorBPixVal.SetReadOnly(m_bIsCoorInfoEditing);
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

void CGraphFormView::OnLButtonDown(UINT nFlags, CPoint point)
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

void CGraphFormView::OnMouseMove(UINT nFlags, CPoint point)
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

void CGraphFormView::OnLButtonUp(UINT nFlags, CPoint point)
{
    CRect rect;
    m_ctrlMainImage.GetWindowRect(rect);
    ScreenToClient(rect);

    if (!m_pSrcImage->IsNull() && PointInRect(point, rect) && m_iDrawMode!=Draw_None &&
        m_bIsDragging) {
        char buf[256];

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


void CGraphFormView::OnBnClickedCoorinfoBtnSnapCooraPixVal()
{
    m_iDrawMode = Draw_SnapCoorA;
}


void CGraphFormView::OnBnClickedCoorinfoBtnSnapCoorbPixVal()
{
    m_iDrawMode = Draw_SnapCoorB;
}

void CGraphFormView::onAddGraph()
{
    m_bIsGraphInfoEditing = false;
    m_bIsCoorInfoEditing = false;
    m_bIsDragging = false;
    m_bDispCoorInfo = true;
    m_bDispDotedInfo = false;
    m_iDrawMode = Draw_None;
    m_iOldImageWidth = 0;
    m_iOldImageHeight = 0;
    m_ptCoorA = CPoint(-1, -1);
    m_ptCoorB = CPoint(-1, -1);

    m_ctrlGraphInfo_EditName.SetReadOnly();
    m_ctrlGraphInfo_EditName.SetWindowText("");
    m_ctrlGraphInfo_BtnEditSave.SetWindowText("±à¼­");
    m_ctrlCoorInfo_EditUnitName.SetReadOnly();
    m_ctrlCoorInfo_EditUnitName.SetWindowText("");
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
