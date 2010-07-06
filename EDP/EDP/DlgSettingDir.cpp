// DlgSettingDir.cpp : implementation file
//

#include "stdafx.h"
#include "EDP.h"
#include "DlgSettingDir.h"
#include "GlobalVars.h"
#include "GlobalFuncs.h"


// CDlgSettingDir dialog

IMPLEMENT_DYNAMIC(CDlgSettingDir, CDialog)

CDlgSettingDir::CDlgSettingDir(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSettingDir::IDD, pParent)
{

}

CDlgSettingDir::~CDlgSettingDir()
{
}

void CDlgSettingDir::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DIR, m_ctrlListDir);
}


BEGIN_MESSAGE_MAP(CDlgSettingDir, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgSettingDir::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgSettingDir::OnBnClickedCancel)
END_MESSAGE_MAP()

// CDlgSettingDir message handlers

BOOL CDlgSettingDir::OnInitDialog()
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

void CDlgSettingDir::OnBnClickedOk()
{
	char dir[1024];
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

void CDlgSettingDir::OnBnClickedCancel()
{
	OnCancel();
}
