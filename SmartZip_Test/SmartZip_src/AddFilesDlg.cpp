// AddFilesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "smartzip.h"
#include "AddFilesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddFilesDlg dialog


CAddFilesDlg::CAddFilesDlg(CWnd* pParent /*=NULL*/)
	: CNewWizPage(CAddFilesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddFilesDlg)
	m_nof = _T("");
	//}}AFX_DATA_INIT
}


void CAddFilesDlg::DoDataExchange(CDataExchange* pDX)
{
	CNewWizPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddFilesDlg)
	DDX_Control(pDX, IDC_REMOVEFLIST, m_remove);
	DDX_Control(pDX, IDC_ADDFILES, m_add);
	DDX_Control(pDX, IDC_LIST2, m_fileslist);
	DDX_Text(pDX, IDC_NOF, m_nof);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddFilesDlg, CNewWizPage)
	//{{AFX_MSG_MAP(CAddFilesDlg)
	ON_BN_CLICKED(IDC_ADDFILES, OnAddfiles)
	ON_BN_CLICKED(IDC_REMOVEFLIST, OnRemoveflist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddFilesDlg message handlers

void CAddFilesDlg::OnAddfiles() 
{
	// TODO: Add your control notification handler code here
	CFileDialog cFileDlg (
		TRUE, 
		(LPCTSTR)NULL, 
		(LPCTSTR)NULL, 
		OFN_ENABLESIZING|OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_EXPLORER|OFN_LONGNAMES|OFN_OVERWRITEPROMPT|OFN_CREATEPROMPT, 
		_T("All Files (*.*)|*.*|"));
	cFileDlg.m_ofn.lpstrTitle = _T("Add Files To Archive");
	if (cFileDlg.DoModal() != IDOK)
		return;
	POSITION pos = cFileDlg.GetStartPosition();
	BOOL Break=FALSE;
	while (pos)
	{
		int m=m_fileslist.GetItemCount ();
		CString s=cFileDlg.GetNextPathName(pos);
		for(int i=0;i<m;i++)
		{
			if(s==m_fileslist.GetItemText (i,0))
			{Break=TRUE;}
		}
		if(!Break)
		{
			m_fileslist.InsertItem (m_fileslist.GetItemCount (),s);
		}
	}
	m_nof.Format ("%d",m_fileslist.GetItemCount ());
	this->UpdateData (FALSE);
}

void CAddFilesDlg::OnRemoveflist() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_fileslist.GetFirstSelectedItemPosition ();
	while(pos)
	{
		int n=m_fileslist.GetNextSelectedItem (pos);
		m_fileslist.DeleteItem (n);
	}
	m_nof.Format ("%d",m_fileslist.GetItemCount ());
this->UpdateData (FALSE);

}

BOOL CAddFilesDlg::OnInitDialog() 
{
	CNewWizPage::OnInitDialog();
		COLORREF	crBtnColor = ::GetSysColor(COLOR_BTNFACE) + RGB(30, 30, 30);
	m_add.SetIcon(IDI_ZIP1);
	m_add.SetColor(CButtonST::BTNST_COLOR_BK_IN, crBtnColor);
	m_add.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 128, 0));

	m_remove.SetIcon(IDI_CANCEL3);
	m_remove.SetColor(CButtonST::BTNST_COLOR_BK_IN, crBtnColor);
	m_remove.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 128, 0));

m_fileslist.InsertColumn (0,"File Pathes",LVCFMT_LEFT,120);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
// refer to CPropertyPage class members for a description of this function
// return -1 to prevent changing pages
// to move to a page other than the next one, return the 
// Dialog resource ID of the page to display
LRESULT CAddFilesDlg::OnWizardNext()
{
	if(m_fileslist.GetItemCount()<=0)
	{
		AfxMessageBox ("Please Add files first");return -1;
	}
	return IDD_ADDOPTIONS;
}

BOOL CAddFilesDlg::DestroyWindow() 
{
	int m=m_fileslist.GetItemCount ();
	for (int i=0;i<m;i++)
	{
		filesarr.Add (m_fileslist.GetItemText (i,0));
		
	}	
	return CDialog::DestroyWindow();
}
