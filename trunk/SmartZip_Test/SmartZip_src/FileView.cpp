// FileView.cpp : implementation file
//

#include "stdafx.h"
#include "smartzip.h"
#include "FileView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileView dialog


CFileView::CFileView(CWnd* pParent /*=NULL*/)
	: CDialog(CFileView::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFileView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileView)
	DDX_Control(pDX, IDOK, m_close);
	DDX_Control(pDX, IDC_LIST1, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileView, CDialog)
	//{{AFX_MSG_MAP(CFileView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileView message handlers

BOOL CFileView::OnInitDialog() 
{
	CDialog::OnInitDialog();
		HDC hDC;                             
	hDC = ::GetDC(NULL);                 
	int height = MulDiv(8, ::GetDeviceCaps(hDC, LOGPIXELSY), 72);
	m_Font.CreateFont(-height, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0, _T("fixedsys"));	
	::ReleaseDC(NULL, hDC);
	m_list.SetFont(&m_Font);
	// TODO: Add extra initialization here
	m_list.LoadFile (&memfile,memfile.GetLength ());
COLORREF	crBtnColor = ::GetSysColor(COLOR_BTNFACE) + RGB(30, 30, 30);
	m_close.SetIcon(IDI_CANCEL3);
	m_close.SetColor(CButtonST::BTNST_COLOR_BK_IN, crBtnColor);
	m_close.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 128, 0));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
