// DlgDirSetup.cpp : implementation file
//

#include "stdafx.h"
#include "AbsTestDataMan.h"
#include "DlgDirSetup.h"
#include "GlobalVars.h"
#include "GlobalFuncs.h"


// CDlgDirSetup dialog

IMPLEMENT_DYNAMIC(CDlgDirSetup, CDialog)

CDlgDirSetup::CDlgDirSetup(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDirSetup::IDD, pParent)
{

}

CDlgDirSetup::~CDlgDirSetup()
{
}

void CDlgDirSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DIR, m_ctrlListDir);
}


BEGIN_MESSAGE_MAP(CDlgDirSetup, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgDirSetup::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgDirSetup::OnBnClickedCancel)
END_MESSAGE_MAP()

// CDlgDirSetup message handlers

BOOL CDlgDirSetup::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add the listbox buddy
	m_ListBoxExBuddy.SubclassDlgItem( IDC_LISTBUDDY, this );
	m_ListBoxExBuddy.SetListbox( &m_ctrlListDir );

	LoadDirFromConfigFile();
	for(int i=0;i<g_saDirectories.GetCount();i++){
		CString str = g_saDirectories.GetAt(i);
		m_ctrlListDir.AddString(str);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDirSetup::OnBnClickedOk()
{
	TCHAR dir[1024];
	CString str;

	g_saDirectories.RemoveAll();
	for(int i=0;i<m_ctrlListDir.GetCount();i++){
		m_ctrlListDir.GetText(i, dir);
		str = CString(dir);
		str = str.Trim();
		if(str.GetLength()){
			g_saDirectories.Add(str);
		}
	}
	SaveDirToConfigFile();

	OnOK();
}

void CDlgDirSetup::OnBnClickedCancel()
{
	OnCancel();
}
