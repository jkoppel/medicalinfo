// EDPDoc.h : CEDPDoc ��Ľӿ�
//


#pragma once

#include "GlobalFuncs.h"


class CEDPDoc : public CDocument
{
protected: // �������л�����
	CEDPDoc();
	DECLARE_DYNCREATE(CEDPDoc)

// ����
public:
	int m_bDataExists;//�����ļ�
	struct TestRecord m_rec;//�������ļ�¼

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CEDPDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
public:
	virtual void OnCloseDocument();
};


