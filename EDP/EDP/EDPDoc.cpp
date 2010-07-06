// EDPDoc.cpp : CEDPDoc 类的实现
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


// CEDPDoc 构造/析构

CEDPDoc::CEDPDoc()
{
	//初始化
}

CEDPDoc::~CEDPDoc()
{
}

BOOL CEDPDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}




// CEDPDoc 序列化

void CEDPDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CEDPDoc 诊断

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


// CEDPDoc 命令

BOOL CEDPDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	return TRUE;
}

void CEDPDoc::OnCloseDocument()
{
	CDocument::OnCloseDocument();
}
