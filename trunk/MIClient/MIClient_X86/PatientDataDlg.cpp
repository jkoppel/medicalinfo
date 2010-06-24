// PatientDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MIClient.h"
#include "PatientDataDlg.h"
#include "GlobalFuncs.h"


// CPatientDataDlg dialog

IMPLEMENT_DYNAMIC(CPatientDataDlg, CDialog)

CPatientDataDlg::CPatientDataDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPatientDataDlg::IDD, pParent)
	, m_nID(0)
	, m_strName(_T(""))
{

}

CPatientDataDlg::~CPatientDataDlg()
{
}

void CPatientDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ID, m_nID);
	DDX_Text(pDX, IDC_NAME, m_strName);
}


BEGIN_MESSAGE_MAP(CPatientDataDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CPatientDataDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CPatientDataDlg message handlers
BOOL CPatientDataDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(m_nFlag==0){//ADD
		m_nID = 0;
		m_strName.Empty();
		UpdateData(FALSE);
	}
	else{
		UpdateData(TRUE);
	}

	return TRUE;
}
void CPatientDataDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	if(m_strName.IsEmpty()){
		ShowMsg("请输入用户名");
		return;
	}

	OnOK();
}
