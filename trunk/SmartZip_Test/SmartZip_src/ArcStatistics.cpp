// ArcStatistics.cpp : implementation file
//

#include "stdafx.h"
#include "smartzip.h"
#include "ArcStatistics.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArcStatistics dialog


CArcStatistics::CArcStatistics(CWnd* pParent /*=NULL*/)
	: CDialog(CArcStatistics::IDD, pParent)
{
	//{{AFX_DATA_INIT(CArcStatistics)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CArcStatistics::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArcStatistics)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CArcStatistics, CDialog)
	//{{AFX_MSG_MAP(CArcStatistics)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArcStatistics message handlers

BOOL CArcStatistics::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
//	m_pDoc->m_zip .GetCount ();
if(hold->zip_index!=-1)
	arr->Add (hold->zip_index);
	POSITION pos=hold->fIndex.GetStartPosition ();
	while(pos)
	{
		CString s;CObject*pOpj;
		hold->fIndex.GetNextAssoc (pos,s,pOpj);
		arr->Add (((INDEX*)pOpj)->n );
	}
	int m=hold->Childrens.GetSize ();
	for(int i=0;i<m;i++)
	{
		AddSubFolders(arr,(CTreeNode*)hold->Childrens[i]);
	}
	for(int i=0;i<;i++)
	{

	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
