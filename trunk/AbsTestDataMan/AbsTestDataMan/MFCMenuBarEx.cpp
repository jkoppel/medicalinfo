// MFCMenuBarEx.cpp : 实现文件
//

#include "stdafx.h"
#include "AbsTestDataMan.h"
#include "MFCMenuBarEx.h"
#include "GlobalFuncs.h"


// CMFCMenuBarEx

IMPLEMENT_DYNAMIC(CMFCMenuBarEx, CMFCMenuBar)

CMFCMenuBarEx::CMFCMenuBarEx()
{

}

CMFCMenuBarEx::~CMFCMenuBarEx()
{
}


BEGIN_MESSAGE_MAP(CMFCMenuBarEx, CMFCMenuBar)
END_MESSAGE_MAP()

LRESULT CMFCMenuBarEx::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	//if(message==WM_NCHITTEST){
		//TRACE( "CMFCMenuBarEx:message:0x%X\n", message );
		if(message==WM_MENUSELECT){
			ShowMsg("WM_MENUSELECT");
		}
		if(message==WM_NEXTMENU){
			ShowMsg("WM_NEXTMENU");
		}
		if(message==WM_NCHITTEST){
			//ShowMsg("WM_NCHITTEST");
		}
	//}

	return CMFCMenuBar::WindowProc(message, wParam, lParam);
}
