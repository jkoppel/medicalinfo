// RightDrawAreaManView.h : CRightDrawAreaView ��Ľӿ�
//


#pragma once
#include "AbsTestDataManDoc.h"
#include "ExtLibs\\LBTabCtrl\\LBTabCtrl.h"

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
	DRAW_MODE m_iDrawMode;	//ͼ����ʾ��ʽ
	POINT m_pntClientAreaRef;	//�ο�λ�ã������½���ʼ��
	POINT m_pntClientAreaOrig;	//��������ԭ��λ��
	int m_nClientAreaWidth;		//��ͼ������
	int m_nClientAreaHeight;	//��ͼ����߶�

	CLBTabCtrl *m_pTab;

// ����
public:
	CAbsTestDataManDoc* GetDocument() const;
	void Draw_Force_Vs_Position();
	void Draw_Force_Vs_PositionAndSpeed();
	void Draw_Force_Vs_Speed();
	void Draw_MaxForce_Vs_Speed();
	void Draw_Force_Vs_Time();
	void Draw_Friction_Force_Vs_Position();
	void SetDrawMode(CRightDrawAreaView::DRAW_MODE iMode);
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
};

#ifndef _DEBUG  // RightDrawAreaView.cpp �еĵ��԰汾
inline CAbsTestDataManDoc* CRightDrawAreaView::GetDocument() const
   { return reinterpret_cast<CAbsTestDataManDoc*>(m_pDocument); }
#endif

