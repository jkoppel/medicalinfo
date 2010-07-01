// EDPDoc.h : CEDPDoc 类的接口
//


#pragma once

#include "GlobalFuncs.h"


class CEDPDoc : public CDocument
{
protected: // 仅从序列化创建
	CEDPDoc();
	DECLARE_DYNCREATE(CEDPDoc)

// 属性
public:
	int m_bDataExists;//读了文件
	struct TestRecord m_rec;//读出来的记录

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 实现
public:
	virtual ~CEDPDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
public:
	virtual void OnCloseDocument();
};


