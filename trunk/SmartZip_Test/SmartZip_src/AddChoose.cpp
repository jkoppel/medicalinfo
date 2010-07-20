// AddChoose.cpp : implementation file
//

#include "stdafx.h"
#include "smartzip.h"
#include "AddChoose.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddChoose dialog


CAddChoose::CAddChoose(CWnd* pParent /*=NULL*/)
	: CNewWizPage(CAddChoose::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddChoose)
	m_choose = -1;
	//}}AFX_DATA_INIT
}


void CAddChoose::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddChoose)
	DDX_Radio(pDX, IDC_CFILE, m_choose);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddChoose, CNewWizPage)
	//{{AFX_MSG_MAP(CAddChoose)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// refer to CPropertyPage class members for a description of this function
// return -1 to prevent changing pages
// to move to a page other than the next one, return the 
// Dialog resource ID of the page to display
LRESULT CAddChoose::OnWizardNext()
{
	this->UpdateData ();
	if(m_choose)
	{
		return IDD_ADDFOLDER;
	}
	return IDD_ADDFILES;
}

