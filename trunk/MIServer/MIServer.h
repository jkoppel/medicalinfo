// MIServer.h : main header file for the MISERVER application
//

#if !defined(AFX_MISERVER_H__AD4C0668_04C6_434D_AFBE_FE36897787FC__INCLUDED_)
#define AFX_MISERVER_H__AD4C0668_04C6_434D_AFBE_FE36897787FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMIServerApp:
// See MIServer.cpp for the implementation of this class
//

class CMIServerApp : public CWinApp
{
public:
	CMIServerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMIServerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMIServerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MISERVER_H__AD4C0668_04C6_434D_AFBE_FE36897787FC__INCLUDED_)
