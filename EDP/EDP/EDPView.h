// EDPView.h : CEDPView ��Ľӿ�
//


#pragma once

#include "EDPDoc.h"
#include "GlobalFuncs.h"

class CEDPView : public CView
{
protected: // �������л�����
	CEDPView();
	DECLARE_DYNCREATE(CEDPView)

// ����
public:
	CEDPDoc* GetDocument() const;

// ����
public:

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
	virtual ~CEDPView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	POINT m_pntClientAreaRef;	//�ο�λ�ã������½���ʼ��
	POINT m_pntClientAreaOrig;	//��������ԭ��λ��
	int m_nClientAreaWidth;		//��ͼ������
	int m_nClientAreaHeight;	//��ͼ����߶�
	int m_nXSpan;				//����һ���Ӧ�����ش�С
	int m_nYSpan;				//����һ���Ӧ�����ش�С

	void DrawData();			//��ʾ����

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
};

#ifndef _DEBUG  // EDPView.cpp �еĵ��԰汾
inline CEDPDoc* CEDPView::GetDocument() const
   { return reinterpret_cast<CEDPDoc*>(m_pDocument); }
#endif

