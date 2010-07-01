// EDPDoc.cpp : CEDPDoc ���ʵ��
//

#include "stdafx.h"
#include "EDP.h"

#include "EDPDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEDPDoc

IMPLEMENT_DYNCREATE(CEDPDoc, CDocument)

BEGIN_MESSAGE_MAP(CEDPDoc, CDocument)
END_MESSAGE_MAP()


// CEDPDoc ����/����

CEDPDoc::CEDPDoc()
{
	//��ʼ��
	m_bDataExists = FALSE;
}

CEDPDoc::~CEDPDoc()
{
}

BOOL CEDPDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	//��ʼ��
	m_bDataExists = FALSE;

	return TRUE;
}




// CEDPDoc ���л�

void CEDPDoc::Serialize(CArchive& ar)
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


// CEDPDoc ���

#ifdef _DEBUG
void CEDPDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEDPDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CEDPDoc ����

BOOL CEDPDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	//�����ݲ��������ƹ���
	struct TestRecord rec;
	if(!LoadFile(lpszPathName, rec)){
		return FALSE;
	}
	m_bDataExists = TRUE;
	memcpy(&m_rec, &rec, sizeof(struct TestRecord));

	return TRUE;
}

void CEDPDoc::OnCloseDocument()
{
	//���ñ�־
	m_bDataExists = FALSE;

	CDocument::OnCloseDocument();
}
