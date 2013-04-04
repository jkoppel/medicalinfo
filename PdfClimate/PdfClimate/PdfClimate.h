
// PdfClimate.h : main header file for the PdfClimate application
//
#pragma once

#ifndef __AFXWIN_H__
    #error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CPdfClimateApp:
// See PdfClimate.cpp for the implementation of this class
//

class CPdfClimateApp : public CWinAppEx
{
public:
    CPdfClimateApp();


// Overrides
public:
    virtual BOOL InitInstance();

// Implementation
    UINT  m_nAppLook;
    BOOL  m_bHiColorIcons;

    virtual void PreLoadState();
    virtual void LoadCustomState();
    virtual void SaveCustomState();

    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
};

extern CPdfClimateApp theApp;
