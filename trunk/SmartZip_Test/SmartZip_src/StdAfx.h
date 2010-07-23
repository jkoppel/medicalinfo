// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__D08AD387_1D24_4AF2_8CEE_7634B437A651__INCLUDED_)
#define AFX_STDAFX_H__D08AD387_1D24_4AF2_8CEE_7634B437A651__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcview.h>
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <commctrl.h>
#endif // _AFX_NO_AFXCMN_SUPPORT
//////**************************
//#include <afxrich.h>
#include "seaShellext.h"
#define ID_PROPERTIES 0x332
#define ID_COMMENTS 0x333

#include "GuiLib.h"

//mine
//#include <afxmt.h>
#include "ZipArchive.h"

struct CDebugTimer
	{
		DWORD m_ticks;
		CDebugTimer(){m_ticks = GetTickCount();}
		void Stop(bool bDisplay = true)
		{
			m_ticks = GetTickCount() - m_ticks;
			if (bDisplay)
			{
					CString sz;
					sz.Format(_T("%d"), m_ticks);
					AfxMessageBox(sz);
	
			}
	}
};
typedef struct tagTHREADPARMS {
	CDocument* pDoc;
	CZipArchive*zip;
    CWnd* pWnd;
    CCriticalSection* pCriticalSection;
    CEvent* pEvent;
	void * pReserved;//CDwordArray* in extract&delete;cstringarray* in add
	int pFull;
	int bOverwrite;
	BOOL bEnc;
} THREADPARMS;
#define WM_USER_THREAD_UPDATE	WM_USER+0x101
#define WM_USER_THREAD_FINISHED	WM_USER+0x102
#define WM_USER_THREAD_ABORTED	WM_USER+0x103

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__D08AD387_1D24_4AF2_8CEE_7634B437A651__INCLUDED_)
