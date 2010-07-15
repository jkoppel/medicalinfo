// RightDrawAreaManView.h : CRightDrawAreaView 类的接口
//


#pragma once
#include "AbsTestDataManDoc.h"
#include "LBTabCtrl\\LBTabCtrl.h"

class CRightDrawAreaView : public CView
{
protected: // 仅从序列化创建
	CRightDrawAreaView();
	DECLARE_DYNCREATE(CRightDrawAreaView)

	//图像显示模式
	enum DRAW_MODE { DM_NONE=0, DM_NORAML_ONLY, DM_FILTER_ONLY, DM_ALL };
// 属性
public:
	int m_iCurrTabIndex;		//当前选中页面
	DRAW_MODE m_iDrawMode;	//图像显示方式
	POINT m_pntClientAreaRef;	//参考位置，即左下角起始点
	POINT m_pntClientAreaOrig;	//真正坐标原点位置
	int m_nClientAreaWidth;		//作图区域宽度
	int m_nClientAreaHeight;	//作图区域高度

	CLBTabCtrl *m_pTab;

// 操作
public:
	CAbsTestDataManDoc* GetDocument() const;
	void DrawData();			//显示数据
	void SetDrawMode(CRightDrawAreaView::DRAW_MODE iMode);
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
};

#ifndef _DEBUG  // RightDrawAreaView.cpp 中的调试版本
inline CAbsTestDataManDoc* CRightDrawAreaView::GetDocument() const
   { return reinterpret_cast<CAbsTestDataManDoc*>(m_pDocument); }
#endif

