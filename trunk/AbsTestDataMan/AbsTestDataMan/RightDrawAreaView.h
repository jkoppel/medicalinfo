// RightDrawAreaManView.h : CRightDrawAreaView ��Ľӿ�
//


#pragma once
#include "AbsTestDataManDoc.h"

class CRightDrawAreaView : public CView
{
protected: // �������л�����
	CRightDrawAreaView();
	DECLARE_DYNCREATE(CRightDrawAreaView)

// ����
public:
	POINT m_pntClientAreaRef;	//�ο�λ�ã������½���ʼ��
	POINT m_pntClientAreaOrig;	//��������ԭ��λ��
	int m_nClientAreaWidth;		//��ͼ�������
	int m_nClientAreaHeight;	//��ͼ����߶�

// ����
public:
	CAbsTestDataManDoc* GetDocument() const;
	void DrawData();			//��ʾ����

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
};

#ifndef _DEBUG  // RightDrawAreaView.cpp �еĵ��԰汾
inline CAbsTestDataManDoc* CRightDrawAreaView::GetDocument() const
   { return reinterpret_cast<CAbsTestDataManDoc*>(m_pDocument); }
#endif
