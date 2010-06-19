// MySocket.cpp: implementation of the CMySocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MIServer.h"
#include "MySocket.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMySocket::CMySocket() : CCESocket()
{
	m_parent = NULL;
}

CMySocket::CMySocket(CWnd* parent) : CCESocket()
{
	m_parent = parent;
}

CMySocket::~CMySocket()
{
}

BOOL CMySocket::OnAccept(SOCKET serviceSocket)
{
	if(m_parent)
	{
		::PostMessage(m_parent->m_hWnd, ON_ACCEPT, (WPARAM) serviceSocket, NULL);
		return TRUE;
	}
	return FALSE;
}

void CMySocket::OnReceive()
{
	if(m_parent)
		::PostMessage(m_parent->m_hWnd, ON_RECEIVE, NULL, NULL);
}

void CMySocket::OnClose(int closeEvent)
{
	if(m_parent)
		::PostMessage(m_parent->m_hWnd, ON_CLOSE, NULL, NULL);
}