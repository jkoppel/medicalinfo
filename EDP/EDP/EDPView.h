// EDPView.h : CEDPView 类的接口
//


#pragma once

#include "EDPDoc.h"
#include "GlobalFuncs.h"

class CEDPView : public CView
{
protected: // 仅从序列化创建
	CEDPView();
	DECLARE_DYNCREATE(CEDPView)

// 属性
public:
	CEDPDoc* GetDocument() const;

// 操作
public:

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
	virtual ~CEDPView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	POINT m_pntClientAreaRef;	//参考位置，即左下角起始点
	POINT m_pntClientAreaOrig;	//真正坐标原点位置
	int m_nClientAreaWidth;		//作图区域宽度
	int m_nClientAreaHeight;	//作图区域高度
	int m_nXSpan;				//横轴一格对应的像素大小
	int m_nYSpan;				//纵轴一格对应的像素大小

	void DrawData();			//显示数据

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
};

#ifndef _DEBUG  // EDPView.cpp 中的调试版本
inline CEDPDoc* CEDPView::GetDocument() const
   { return reinterpret_cast<CEDPDoc*>(m_pDocument); }
#endif

