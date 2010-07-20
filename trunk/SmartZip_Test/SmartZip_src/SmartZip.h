// SmartZip.h : main header file for the SMARTZIP application
//

#if !defined(AFX_SMARTZIP_H__0757BD28_E484_4DA6_8C17_4209552E26F5__INCLUDED_)
#define AFX_SMARTZIP_H__0757BD28_E484_4DA6_8C17_4209552E26F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSmartZipApp:
// See SmartZip.cpp for the implementation of this class
//

class CSmartZipApp : public CWinApp
{
public:
	CSmartZipApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmartZipApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CSmartZipApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMARTZIP_H__0757BD28_E484_4DA6_8C17_4209552E26F5__INCLUDED_)
