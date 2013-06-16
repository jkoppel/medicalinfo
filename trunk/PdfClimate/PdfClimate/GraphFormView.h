#pragma once
#include "ImageProcess/GraphDefs.h"

// CGraphFormView form view

class CGraphFormView : public CFormView
{
	DECLARE_DYNCREATE(CGraphFormView)

protected:
    CGraphFormView(UINT nIDTemplate, GraphType iGraphType);
	CGraphFormView();           // protected constructor used by dynamic creation
	virtual ~CGraphFormView();

public:
    enum { IMAGE_X_OFFSET = 1};
    enum { IMAGE_Y_OFFSET = 7};
	enum { IDD = 0 };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
    enum DrawMode {
        Draw_None,
        Draw_SnapCoorA,
        Draw_SnapCoorB,
        Draw_EraseTool,
    };
    enum EditMode {
        Edit_None,
        Edit_Adding,
        Edit_Modifying,
    };

    CImage *getSrcImage() { return m_pSrcImage; }
    CBitmap *getSrcBitmap() { return m_pSrcBitmap; }
    bool isDataModified() { return m_bIsGraphInfoEditing | m_bIsCoorInfoEditing; }
    virtual void onAddGraph();
    virtual void loadDataFromDB();

    void setXUnitName(const char *sXUnitName) {
        m_ctrlGraphInfo_EditUnitName_X.SetWindowText(sXUnitName);
    }
    void getXUnitName(char *sXUnitName, int iMaxLength=-1) {
        m_ctrlGraphInfo_EditUnitName_X.GetWindowText(sXUnitName, iMaxLength);
    }
    void setYUnitName(const char *sYUnitName) {
        m_ctrlGraphInfo_EditUnitName_Y.SetWindowText(sYUnitName);
    }
    void getYUnitName(char *sYUnitName, int iMaxLength=-1) {
        m_ctrlGraphInfo_EditUnitName_Y.GetWindowText(sYUnitName, iMaxLength);
    }

    void updateGraphList();
    
protected:
    GraphType m_iGraphType;
    CImage *m_pSrcImage;
    CBitmap *m_pSrcBitmap;
    CList<int> m_lstGraphInfoItemID;
    CList<int> m_lstCoorInfoItemID;
    bool m_bIsGraphInfoEditing;
    bool m_bIsCoorInfoEditing;
    bool m_bIsDragging;
    bool m_bDispCoorInfo;
    bool m_bDispDotedInfo;
    DrawMode m_iDrawMode;
    HCURSOR m_hCross;
    CPoint m_ptCoorA;
    CPoint m_ptCoorB;
    CPoint m_ptCurrent;
    int m_iOldImageWidth;
    int m_iOldImageHeight;
    EditMode m_iEditMode;
    int m_iCurrGraphIndex;
    
    CComboBox m_ctrlGraphList;
    CStatic m_ctrlMainImage;
    CStatic m_ctrlGraphInfo;
    CEdit m_ctrlGraphInfo_EditName;
    CEdit m_ctrlGraphInfo_EditUnitName_X;
    CEdit m_ctrlGraphInfo_EditUnitName_Y;
    CEdit m_ctrlGraphInfo_BtnEditSave;
    CEdit m_ctrlCoorInfo_EditCoorAPixVal;
    CButton m_ctrlCoorInfo_BtnSnapCoorA;
    CEdit m_ctrlCoorInfo_EditCoorA_X;
    CEdit m_ctrlCoorInfo_EditCoorA_Y;
    CEdit m_ctrlCoorInfo_EditCoorBPixVal;
    CButton m_ctrlCoorInfo_BtnSnapCoorB;
    CEdit m_ctrlCoorInfo_EditCoorB_X;
    CEdit m_ctrlCoorInfo_EditCoorB_Y;
    CEdit m_ctrlCoorInfo_BtnEditSave;

protected:
    void initialize();
    void destroy();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
    virtual void OnDraw(CDC* /*pDC*/);
public:
    virtual void OnInitialUpdate();
    virtual void OnBnClickedGraphinfoBtnEditsave();
    afx_msg void OnBnClickedCoorinfoBtnEditsave();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnBnClickedCoorinfoBtnSnapCooraPixVal();
    afx_msg void OnBnClickedCoorinfoBtnSnapCoorbPixVal();
};
