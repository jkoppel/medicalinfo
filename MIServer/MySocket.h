// MySocket.h: interface for the CMySocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYSOCKET_H__13E94208_E06B_4E7E_8985_3B4E64E8E817__INCLUDED_)
#define AFX_MYSOCKET_H__13E94208_E06B_4E7E_8985_3B4E64E8E817__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CESocket.h"

class CMySocket : public CCESocket  
{
public:
	CMySocket();
	CMySocket(CWnd* parent);
	virtual ~CMySocket();

	void SetParent(CWnd* parent) {m_parent = parent;}
	
	virtual void OnReceive();
	virtual bool OnAccept(SOCKET serviceSocket);
	virtual void OnClose(int closeEvent);

protected:
	CWnd* m_parent;
};

#endif // !defined(AFX_MYSOCKET_H__13E94208_E06B_4E7E_8985_3B4E64E8E817__INCLUDED_)
