// MyFavourites.cpp : implementation file
//

#include "stdafx.h"
#include "smartzip.h"
#include "MyFavourites.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyFavourites dialog


CMyFavourites::CMyFavourites(CWnd* pParent /*=NULL*/)
: CDialog(CMyFavourites::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMyFavourites)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_reg.SetInfo("Settings", "Favo", this);
	m_reg.Load();
	
}
CMyFavourites::~CMyFavourites()
{
	m_reg.Save();
}

void CMyFavourites::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyFavourites)
	DDX_Control(pDX, IDCANCEL2, m_delete);
	DDX_Control(pDX, IDCANCEL, m_close);
	DDX_Control(pDX, IDOK, m_open);
	DDX_Control(pDX, IDC_LIST1, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMyFavourites, CDialog)
//{{AFX_MSG_MAP(CMyFavourites)
ON_BN_CLICKED(IDCANCEL2, OnDelete)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyFavourites message handlers

BOOL CMyFavourites::OnInitDialog() 
{
	CDialog::OnInitDialog();
	COLORREF	crBtnColor = ::GetSysColor(COLOR_BTNFACE) + RGB(30, 30, 30);
	m_open.SetIcon(IDI_ZIP1);
	m_open.SetColor(CButtonST::BTNST_COLOR_BK_IN, crBtnColor);
	m_open.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 128, 0));
	m_close.SetIcon(IDI_CANCEL3);
	m_close.SetColor(CButtonST::BTNST_COLOR_BK_IN, crBtnColor);
	m_close.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 128, 0));
	m_delete.SetIcon(IDI_DELETE);
	m_delete.SetColor(CButtonST::BTNST_COLOR_BK_IN, crBtnColor);
	m_delete.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 128, 0));
	m_list.InsertColumn (0,"My favourites list",LVCFMT_LEFT,320);	
	int m=filesarr.GetSize ();
	for(int i=0;i<m;i++)
	{
		m_list.InsertItem (m_list.GetItemCount (),filesarr.GetAt (i));
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMyFavourites::OnOK() 
{
	
	POSITION pos=m_list.GetFirstSelectedItemPosition ();
	int m=m_list.GetNextSelectedItem (pos);
	::AfxGetApp ()->OpenDocumentFile (m_list.GetItemText (m,0));
	//	
	CDialog::OnOK();
}



void CMyFavourites::Serialize(CArchive& ar) 
{
	filesarr.Serialize (ar);
	
}

void CMyFavourites::AddToFavo(CString &s)
{
	filesarr.Add (s);
}

void CMyFavourites::OnDelete() 
{
	POSITION pos=m_list.GetFirstSelectedItemPosition ();
	int m=m_list.GetNextSelectedItem (pos);
	if(m!=-1)
	{
		m_list.DeleteItem(m);
		filesarr.RemoveAt(m);
	}
}
