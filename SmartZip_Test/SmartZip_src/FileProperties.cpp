// FileProperties.cpp : implementation file
//

#include "stdafx.h"
#include "smartzip.h"
#include "FileProperties.h"
#include "RESOURCE.H"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileProperties dialog


CFileProperties::CFileProperties(CWnd* pParent /*=NULL*/)
	: CDialog(CFileProperties::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileProperties)
	m_attrib = _T("");
	m_comratio = _T("");
	m_comsize = _T("");
	m_crc = _T("");
	m_filename = _T("");
	m_moddate = _T("");
	m_type = _T("");
	m_ucomsize = _T("");
	m_contains = _T("");
	//}}AFX_DATA_INIT
	m_strtitle="File/Folder(s) Properties";
	bDir=FALSE;
}


void CFileProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileProperties)
	DDX_Control(pDX, IDOK, m_close);
	DDX_Control(pDX, IDCANCEL, m_ok);
	DDX_Control(pDX, IDC_COMMENTS, m_comment);
	DDX_Control(pDX, IDC_FILEICON, m_fileicon);
	DDX_Text(pDX, IDC_ATTRIB, m_attrib);
	DDX_Text(pDX, IDC_COMRATIO, m_comratio);
	DDX_Text(pDX, IDC_COMSIZE, m_comsize);
	DDX_Text(pDX, IDC_CRC, m_crc);
	DDX_Text(pDX, IDC_FILENAME, m_filename);
	DDX_Text(pDX, IDC_MODDATE, m_moddate);
	DDX_Text(pDX, IDC_TYPE, m_type);
	DDX_Text(pDX, IDC_UCOMSIZE, m_ucomsize);
	DDX_Text(pDX, IDC_CONTAINS, m_contains);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileProperties, CDialog)
	//{{AFX_MSG_MAP(CFileProperties)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileProperties message handlers

BOOL CFileProperties::OnInitDialog() 
{
	CDialog::OnInitDialog();
			COLORREF	crBtnColor = ::GetSysColor(COLOR_BTNFACE) + RGB(30, 30, 30);
	m_ok.SetIcon(IDI_CANCEL3);
	m_ok.SetColor(CButtonST::BTNST_COLOR_BK_IN, crBtnColor);
	m_ok.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 128, 0));
	m_close.SetIcon(IDI_ZIP1);
	m_close.SetColor(CButtonST::BTNST_COLOR_BK_IN, crBtnColor);
	m_close.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 128, 0));

	if(bDir)
		this->DisableFileProp ();
	else
	{
		this->DisableFolderInfo ();
		m_comment.SetLimitText(65535);
		m_comment.SetWindowText (fh->GetComment ());
	}
	this->SetWindowText (m_strtitle	);
	m_fileicon.SetIcon (icon);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFileProperties::DisableFileProp()
{
(GetDlgItem (IDC_S1))->ShowWindow(SW_HIDE);
(GetDlgItem (IDC_S2))->ShowWindow(SW_HIDE);
(GetDlgItem (IDC_S4))->ShowWindow(SW_HIDE);
(GetDlgItem (IDC_S5))->ShowWindow(SW_HIDE);
(GetDlgItem (IDC_S6))->ShowWindow(SW_HIDE);
(GetDlgItem (IDC_COMRATIO))->ShowWindow(SW_HIDE);
(GetDlgItem (IDC_CRC))->ShowWindow(SW_HIDE);
(GetDlgItem (IDC_MODDATE))->ShowWindow(SW_HIDE);
(GetDlgItem (IDC_ATTRIB))->ShowWindow(SW_HIDE);
(GetDlgItem (IDC_COMMENTS))->ShowWindow(SW_HIDE);
(GetDlgItem (IDC_SEP1))->ShowWindow(SW_HIDE);
(GetDlgItem (IDC_SEP2))->ShowWindow(SW_HIDE);

}

BOOL CFileProperties::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	::DestroyIcon(icon);
	return CDialog::DestroyWindow();
}

void CFileProperties::DisableFolderInfo()
{
(GetDlgItem (IDC_S3))->ShowWindow(SW_HIDE);
(GetDlgItem (IDC_CONTAINS))->ShowWindow(SW_HIDE);

}

void CFileProperties::OnOK() 
{
	m_comment.GetWindowText (comment);
	CDialog::OnOK();
}
