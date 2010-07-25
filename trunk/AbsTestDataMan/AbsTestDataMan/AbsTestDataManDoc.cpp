
// AbsTestDataManDoc.cpp : CAbsTestDataManDoc 类的实现
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


// CAbsTestDataManDoc 构造/析构

CAbsTestDataManDoc::CAbsTestDataManDoc()
{
	// TODO: 在此添加一次性构造代码

}

CAbsTestDataManDoc::~CAbsTestDataManDoc()
{
}

BOOL CAbsTestDataManDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CAbsTestDataManDoc 序列化

void CAbsTestDataManDoc::Serialize(CArchive& ar)
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


// CAbsTestDataManDoc 诊断

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


// CAbsTestDataManDoc 命令
