
// AbsTestDataManDoc.h : CAbsTestDataManDoc ��Ľӿ�
//


#pragma once


class CAbsTestDataManDoc : public CDocument
{
protected: // �������л�����
	CAbsTestDataManDoc();
	DECLARE_DYNCREATE(CAbsTestDataManDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CAbsTestDataManDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};


