
// AbsTestDataManDoc.cpp : CAbsTestDataManDoc ���ʵ��
//

#include "stdafx.h"
#include "AbsTestDataMan.h"

#include "AbsTestDataManDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAbsTestDataManDoc

IMPLEMENT_DYNCREATE(CAbsTestDataManDoc, CDocument)

BEGIN_MESSAGE_MAP(CAbsTestDataManDoc, CDocument)
END_MESSAGE_MAP()


// CAbsTestDataManDoc ����/����

CAbsTestDataManDoc::CAbsTestDataManDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CAbsTestDataManDoc::~CAbsTestDataManDoc()
{
}

BOOL CAbsTestDataManDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CAbsTestDataManDoc ���л�

void CAbsTestDataManDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// CAbsTestDataManDoc ���

#ifdef _DEBUG
void CAbsTestDataManDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAbsTestDataManDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CAbsTestDataManDoc ����
