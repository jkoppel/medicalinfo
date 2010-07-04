// DlgSettingDir.cpp : implementation file
//

#include "stdafx.h"
#include "EDP.h"
#include "DlgSettingDir.h"


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

#define CONFIG_DIR_FILE		"directories.txt"
// CDlgSettingDir message handlers

BOOL CDlgSettingDir::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add the listbox buddy
	m_ListBoxExBuddy.SubclassDlgItem( IDC_LISTBUDDY, this );
	m_ListBoxExBuddy.SetListbox( &m_ctrlListDir );

	char dir[1024];

	FILE *pFile = NULL;
	fopen_s(&pFile, CONFIG_DIR_FILE, "ab+");
	if(!pFile){
		return TRUE;
	}
	do{
		memset(dir, 0, sizeof(dir));
		fgets(dir, sizeof(dir), pFile);
		if(strlen(dir)>0){
			m_ctrlListDir.AddString(dir);
		}
		else{
			break;
		}
	} while(!feof(pFile));
	fclose(pFile);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSettingDir::OnBnClickedOk()
{
	char dir[1024];

	memset(dir, 0, sizeof(dir));
	FILE *pFile = NULL;
	fopen_s(&pFile, CONFIG_DIR_FILE, "wb+");
	if(!pFile){
		return;
	}
	for(int i=0;i<m_ctrlListDir.GetCount();i++){
		m_ctrlListDir.GetText(i, dir);
		fputs(dir, pFile);
	};
	fclose(pFile);

	OnOK();
}

void CDlgSettingDir::OnBnClickedCancel()
{
	OnCancel();
}
