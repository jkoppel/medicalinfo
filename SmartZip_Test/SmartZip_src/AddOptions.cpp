// AddOptions.cpp : implementation file
//

#include "stdafx.h"
#include "smartzip.h"
#include "AddOptions.h"
#include "AddChoose.h"
#include "NewWizDialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CAddOptions dialog


CAddOptions::CAddOptions(CWnd* pParent /*=NULL*/)
	: CNewWizPage(CAddOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddOptions)
	m_full = FALSE;
	m_compression = 0;
	m_upass = FALSE;
	//}}AFX_DATA_INIT
}


void CAddOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddOptions)
	DDX_Control(pDX, IDC_COMPRESSIONLEVEL, m_slider);
	DDX_Check(pDX, IDC_FULLPATH, m_full);
	DDX_Slider(pDX, IDC_COMPRESSIONLEVEL, m_compression);
	DDX_Check(pDX, IDC_CHECK1, m_upass);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddOptions, CNewWizPage)
	//{{AFX_MSG_MAP(CAddOptions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddOptions message handlers
// refer to CPropertyPage class members for a description of this function
// return -1 to prevent changing pages
// to move to a page other than the previous one, return the 
// Dialog resource ID of the page to display
LRESULT CAddOptions::OnWizardBack()
{
	CNewWizDialog * dlg=(CNewWizDialog*)this->GetParent ();
	CAddChoose* ch=(CAddChoose*)dlg->GetPage ( 0);
	if(	ch->m_choose )
	{
		return IDD_ADDFOLDER;
	}
	return IDD_ADDFILES;
}

// refer to CPropertyPage class members for a description of this function
BOOL CAddOptions::OnWizardFinish()
{
	return TRUE;
}
BOOL CAddOptions::OnInitDialog() 
{
	CNewWizPage::OnInitDialog();
	
m_slider.SetRange (0,9);
m_slider.SetPos(9);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CAddOptions::DestroyWindow() 
{
this->UpdateData ();	
	return CDialog::DestroyWindow();
}
