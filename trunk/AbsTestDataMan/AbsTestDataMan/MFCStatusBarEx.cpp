// MFCStatusBarEx.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AbsTestDataMan.h"
#include "MFCStatusBarEx.h"
#include "GlobalFuncs.h"


// CMFCStatusBarEx

IMPLEMENT_DYNAMIC(CMFCStatusBarEx, CMFCStatusBar)

CMFCStatusBarEx::CMFCStatusBarEx()
{

}

CMFCStatusBarEx::~CMFCStatusBarEx()
{
}


BEGIN_MESSAGE_MAP(CMFCStatusBarEx, CMFCStatusBar)
END_MESSAGE_MAP()

LRESULT CMFCStatusBarEx::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	//TRACE( "CMFCStatusBarEx:message:0x%X\n", message );
	if(WM_SETTEXT==message){
		TCHAR *tc = (TCHAR*)lParam;
		if(tc==NULL || (_tcscmp(tc, _T("����"))==0) || (_tcscmp(tc, _T("Ready")))==0){
			SetPaneText(0, _T("�廪��ѧ����ϵ���� 2010.7"));
			return 0;
		}
	}

	return CMFCStatusBar::WindowProc(message, wParam, lParam);
}
