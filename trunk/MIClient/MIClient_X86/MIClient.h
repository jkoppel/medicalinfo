// MIClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"

// CMIClientApp:
// �йش����ʵ�֣������ MIClient.cpp
//

class CMIClientApp : public CWinApp
{
public:
	CMIClientApp();
	
// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMIClientApp theApp;
