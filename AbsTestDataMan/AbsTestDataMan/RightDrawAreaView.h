// RightDrawAreaManView.h : CRightDrawAreaView 类的接口
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
protected: // 仅从序列化创建
	CRightDrawAreaView();
	DECLARE_DYNCREATE(CRightDrawAreaView)

	//图像显示模式
	enum DRAW_MODE { DM_NONE=0, DM_NORMAL_ONLY, DM_FILTER_ONLY, DM_ALL };
// 属性
public:
	int m_iCurrTabIndex;		//当前选中页面
	DRAW_MODE m_iDrawMode;		//图像显示方式
	POINT m_pntClientAreaRef;	//参考位置，即左下角起始点
	POINT m_pntClientAreaOrig;	//真正坐标原点位置
	int m_nClientAreaWidth;		//作图区域宽度
	int m_nClientAreaHeight;	//作图区域高度
	int m_nMaxNumOfCurveToDraw;	//最多作图个数
	struct CurveInfo *m_pCurveInfo;
	struct DrawAreaConfig m_daConfig;

	CLBTabCtrl *m_pTab;
	CMenu *m_pPopupMenu;

// 操作
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

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CRightDrawAreaView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
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

#ifndef _DEBUG  // RightDrawAreaView.cpp 中的调试版本
inline CAbsTestDataManDoc* CRightDrawAreaView::GetDocument() const
   { return reinterpret_cast<CAbsTestDataManDoc*>(m_pDocument); }
#endif

