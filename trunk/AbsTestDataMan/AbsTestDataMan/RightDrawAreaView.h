// RightDrawAreaManView.h : CRightDrawAreaView 类的接口
//


#pragma once
#include "AbsTestDataManDoc.h"

class CRightDrawAreaView : public CView
{
protected: // 仅从序列化创建
	CRightDrawAreaView();
	DECLARE_DYNCREATE(CRightDrawAreaView)

// 属性
public:
	POINT m_pntClientAreaRef;	//参考位置，即左下角起始点
	POINT m_pntClientAreaOrig;	//真正坐标原点位置
	int m_nClientAreaWidth;		//作图区域宽度
	int m_nClientAreaHeight;	//作图区域高度

// 操作
public:
	CAbsTestDataManDoc* GetDocument() const;
	void DrawData();			//显示数据

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
};

#ifndef _DEBUG  // RightDrawAreaView.cpp 中的调试版本
inline CAbsTestDataManDoc* CRightDrawAreaView::GetDocument() const
   { return reinterpret_cast<CAbsTestDataManDoc*>(m_pDocument); }
#endif

