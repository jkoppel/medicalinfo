// ArchiveRenamer.cpp : implementation file
//

#include "stdafx.h"
#include "smartzip.h"
#include "ArchiveRenamer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArchiveRenamer dialog


CArchiveRenamer::CArchiveRenamer(CWnd* pParent /*=NULL*/)
	: CDialog(CArchiveRenamer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CArchiveRenamer)
	m_from = _T("");
	m_to = _T("");
	//}}AFX_DATA_INIT
}


void CArchiveRenamer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArchiveRenamer)
	DDX_Text(pDX, IDC_FROM, m_from);
	DDX_Text(pDX, IDC_TO, m_to);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CArchiveRenamer, CDialog)
	//{{AFX_MSG_MAP(CArchiveRenamer)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArchiveRenamer message handlers

BOOL CArchiveRenamer::DestroyWindow() 
{
this->UpdateData ();	
	return CDialog::DestroyWindow();
}
