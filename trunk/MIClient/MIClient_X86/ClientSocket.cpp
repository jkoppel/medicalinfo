// ClientSocket.cpp: implementation of the CClientSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MIClient.h"
#include "ClientSocket.h"
#include "GlobalVars.h"
#include "GlobalFuncs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClientSocket::CClientSocket(CWnd * parent) : CCESocket()
{
	m_Parent = parent;
}

CClientSocket::~CClientSocket()
{
}

void CClientSocket::OnReceive()
{
	CString str, temp;
	while(g_pClientSocket->GetDataSize() > 0 && g_pClientSocket->ReadString(temp)){
		str += temp;
	}
	CString2Char(str, g_RecvData);
	g_bIsDataComing = TRUE;
}


void CClientSocket::OnClose(int closeEvent)
{
	if(m_Parent)
		::PostMessage(m_Parent->m_hWnd, SOCK_ONCLOSE, NULL, NULL);
}