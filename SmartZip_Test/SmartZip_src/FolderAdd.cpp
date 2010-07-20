// FolderAdd.cpp : implementation file
//

#include "stdafx.h"
#include "smartzip.h"
#include "FolderAdd.h"
#include "Shlobj.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFolderAdd dialog


CFolderAdd::CFolderAdd(CWnd* pParent /*=NULL*/)
	: CNewWizPage(CFolderAdd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFolderAdd)
	m_path = _T("");
	//}}AFX_DATA_INIT
}


void CFolderAdd::DoDataExchange(CDataExchange* pDX)
{
	CNewWizPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFolderAdd)
	DDX_Control(pDX, IDC_COMBOBOXEX1, m_cbShell);
	DDX_Control(pDX, IDC_SHELLTREE, m_shelltree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFolderAdd, CNewWizPage)
	//{{AFX_MSG_MAP(CFolderAdd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFolderAdd message handlers

// refer to CPropertyPage class members for a description of this function
// return -1 to prevent changing pages
// to move to a page other than the previous one, return the 
// Dialog resource ID of the page to display
LRESULT CFolderAdd::OnWizardBack()
{
	return IDD_ADDCHOOSE;
}


// refer to CPropertyPage class members for a description of this function
// return -1 to prevent changing pages
// to move to a page other than the next one, return the 
// Dialog resource ID of the page to display
LRESULT CFolderAdd::OnWizardNext()
{
	CString s=m_shelltree.GetSelectedPath();
	if(s=="")	
	{
		AfxMessageBox("Please Choose a folder first");return -1;
	}
	return IDD_ADDOPTIONS;
}

BOOL CFolderAdd::OnInitDialog() 
{
	CNewWizPage::OnInitDialog();
	
	// TODO: Add extra initialization here
//	SHGetSpecialFolderPath (
//	CString s;
//	SHGetSpecialFolderPath(this->m_hWnd ,s.GetBuffer (256),CSIDL_DESKTOP,FALSE);
//s.ReleaseBuffer(256);
m_shelltree.SetComboBoxWnd(m_cbShell.GetSafeHwnd());
m_cbShell.SetTreeCtrlWnd(m_shelltree.GetSafeHwnd());
//	m_shelltree.LoadFolderItems(s);
m_shelltree.LoadFolderItems(NULL);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CFolderAdd::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_path=m_shelltree.GetSelectedPath();
	return CNewWizPage::DestroyWindow();
}
