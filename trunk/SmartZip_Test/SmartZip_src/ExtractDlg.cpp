// ExtractDlg.cpp : implementation file
//

#include "stdafx.h"
#include "smartzip.h"
#include "ExtractDlg.h"
#include "browseforfolder.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtractDlg dialog


CExtractDlg::CExtractDlg(CWnd* pParent /*=NULL*/)
	: CNewWizPage(CExtractDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExtractDlg)
	m_bFullpath = FALSE;
	m_destination = _T("");
	m_bOverride = FALSE;
	//}}AFX_DATA_INIT
}


void CExtractDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExtractDlg)
	DDX_Control(pDX, IDC_BROWSE, m_browse);
	DDX_Check(pDX, IDC_CHECK1, m_bFullpath);
	DDX_Text(pDX, IDC_DESTINATION, m_destination);
	DDX_Check(pDX, IDC_CHECK2, m_bOverride);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExtractDlg, CNewWizPage)//CDialog
	//{{AFX_MSG_MAP(CExtractDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtractDlg message handlers

void CExtractDlg::OnBrowse() 
{
CString sz;
	GetDlgItem(IDC_DESTINATION)->GetWindowText(sz);
	CBrowseForFolder bf;
	bf.strTitle="Choose the destinatio folder";
	bf.strStartupDir = sz;
	if (bf.GetFolder(sz))
		GetDlgItem(IDC_DESTINATION)->SetWindowText(sz);	
}

BOOL CExtractDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateData ();
	return CDialog::DestroyWindow();
}

BOOL CExtractDlg::OnInitDialog() 
{
	CNewWizPage::OnInitDialog();
	
			COLORREF	crBtnColor = ::GetSysColor(COLOR_BTNFACE) + RGB(30, 30, 30);
	m_browse.SetIcon(IDI_ZIP1);
	m_browse.SetColor(CButtonST::BTNST_COLOR_BK_IN, crBtnColor);
	m_browse.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 128, 0));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
