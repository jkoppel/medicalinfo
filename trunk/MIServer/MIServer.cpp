// MIServer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MIServer.h"

//#define _DEBUG_VER_
#ifdef _DEBUG_VER_
#include "MIServerDlg.h"
#else
#include "MIServerThread.h"
#endif
#include "GlobalFuncs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMIServerApp

BEGIN_MESSAGE_MAP(CMIServerApp, CWinApp)
	//{{AFX_MSG_MAP(CMIServerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMIServerApp construction

CMIServerApp::CMIServerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMIServerApp object

CMIServerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMIServerApp initialization

BOOL CMIServerApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	//先创建系统缓存目录
	CreateAppDataDir();

#ifdef _DEBUG_VER_
	CMIServerDlg p;
#else
	CMIServerThread p;
#endif
	m_pMainWnd = &p;
	p.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
