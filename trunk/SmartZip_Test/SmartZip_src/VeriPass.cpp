// VeriPass.cpp : implementation file
//

#include "stdafx.h"
#include "SmartZip.h"
#include "VeriPass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVeriPass dialog


CVeriPass::CVeriPass(CWnd* pParent /*=NULL*/)
	: CDialog(CVeriPass::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVeriPass)
	m_pass = _T("");
	m_filename = _T("");
	//}}AFX_DATA_INIT
}


void CVeriPass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVeriPass)
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Control(pDX, IDCANCEL, m_close);
	DDX_Text(pDX, IDC_PASSWORD, m_pass);
	DDX_Text(pDX, IDC_FILENAME, m_filename);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVeriPass, CDialog)
	//{{AFX_MSG_MAP(CVeriPass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVeriPass message handlers

BOOL CVeriPass::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
			COLORREF	crBtnColor = ::GetSysColor(COLOR_BTNFACE) + RGB(30, 30, 30);
	m_ok.SetIcon(IDI_ZIP1);
	m_ok.SetColor(CButtonST::BTNST_COLOR_BK_IN, crBtnColor);
	m_ok.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 128, 0));
	m_close.SetIcon(IDI_CANCEL3);
	m_close.SetColor(CButtonST::BTNST_COLOR_BK_IN, crBtnColor);
	m_close.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 128, 0));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
