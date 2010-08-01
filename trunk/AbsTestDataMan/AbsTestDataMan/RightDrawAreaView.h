// RightDrawAreaManView.h : CRightDrawAreaView ��Ľӿ�
//


#pragma once
#include "AbsTestDataManDoc.h"
#include "ExtLibs\\LBTabCtrl\\LBTabCtrl.h"
#include "DrawAreaConfigMgt.h"

#define MAX_AXIS_BUF_LEN	256

#define DEFALUT_XAXIS_MIN	100
#define DEFALUT_XAXIS_MAX	900
#define DEFALUT_YAXIS_MIN	100
#define DEFALUT_YAXIS_MAX	900

#define DEFALUT_MIN_UNITCOUNT	2

#define DRAWARER_MIN_WIDTH	200
#define DRAWAREA_MIN_HEIGHT 200

#define DEFAULT_REFPOINT_OFFSET_X	60
#define DEFAULT_REFPOINT_OFFSET_Y	40

#define SECOND_CURVE_OFFSET	25

#define DEFAULT_GRID_COLOR	RGB(192,192,192)

struct CurveInfo
{
	double *pXData;
	double *pYData;
	int iNumOfPoint;

	COLORREF crCurve;
	int iWidth;
	BOOL bIsClosed;
	BOOL bMarkPoint;
	COLORREF crMark;
};

struct CanvasInfo
{
	RECT rect;
	CDC *pDC;
	struct CurveInfo *pCurveInfo;
	int iNumOfCurve;
	char sCanvasInfo[MAX_AXIS_BUF_LEN];
	char sXAxisInfo[MAX_AXIS_BUF_LEN];
	char sYAxisInfo[MAX_AXIS_BUF_LEN];

	BOOL bOverlapAnotherCurve;
	struct CurveInfo *pCurveInfo_Second;
	int iNumOfCurve_Second;
	char sXAxisInfo_Second[MAX_AXIS_BUF_LEN];
};

extern void DrawCurve(struct CanvasInfo ci);

class CRightDrawAreaView : public CView
{
protected: // �������л�����
	CRightDrawAreaView();
	DECLARE_DYNCREATE(CRightDrawAreaView)

	//ͼ����ʾģʽ
	enum DRAW_MODE { DM_NONE=0, DM_NORMAL_ONLY, DM_FILTER_ONLY, DM_ALL };
// ����
public:
	int m_iCurrTabIndex;		//��ǰѡ��ҳ��
	DRAW_MODE m_iDrawMode;		//ͼ����ʾ��ʽ
	POINT m_pntClientAreaRef;	//�ο�λ�ã������½���ʼ��
	POINT m_pntClientAreaOrig;	//��������ԭ��λ��
	int m_nClientAreaWidth;		//��ͼ������
	int m_nClientAreaHeight;	//��ͼ����߶�
	int m_nMaxNumOfCurveToDraw;	//�����ͼ����
	struct CurveInfo *m_pCurveInfo;
	struct DrawAreaConfig m_daConfig;

	CLBTabCtrl *m_pTab;
	CMenu *m_pPopupMenu;

// ����
public:
	CAbsTestDataManDoc* GetDocument() const;
	void Draw_Force_Vs_Position();
	void Draw_Force_Vs_PositionAndSpeed();
	void Draw_Force_Vs_Speed();
	void Draw_MaxForce_Vs_Speed();
	void Draw_Force_Vs_Time();
	void Draw_Friction_Force_Vs_Position();
	void Draw_Friction_Force_Vs_PositionAndSpeed();
	void Draw_Friction_Force_Vs_Speed();
	void Draw_Friction_MaxForce_Vs_Speed();
	void Draw_Friction_Force_Vs_Time();
	void SetDrawMode(CRightDrawAreaView::DRAW_MODE iMode);
	void SetDrawAreaConfig(struct DrawAreaConfig daConfig);
	CRightDrawAreaView::DRAW_MODE GetDrawMode();

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CRightDrawAreaView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnModeNormalCurve();
	afx_msg void OnModeFilterCurve();
	afx_msg void OnModeAllCurve();
};

#ifndef _DEBUG  // RightDrawAreaView.cpp �еĵ��԰汾
inline CAbsTestDataManDoc* CRightDrawAreaView::GetDocument() const
   { return reinterpret_cast<CAbsTestDataManDoc*>(m_pDocument); }
#endif

