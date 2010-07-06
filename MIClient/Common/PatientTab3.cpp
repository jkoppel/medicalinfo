// PatientTab3.cpp : implementation file
//

#include "stdafx.h"
#ifdef _WIN32_WCE
#include "..\\MIClient\\MIClient.h"
#else
#include "..\\MIClient_X86\\MIClient.h"
#endif
#include "PatientTab3.h"


// CPatientTab3 dialog

IMPLEMENT_DYNAMIC(CPatientTab3, CDialog)

CPatientTab3::CPatientTab3(CWnd* pParent /*=NULL*/)
	: CDialog(CPatientTab3::IDD, pParent)
	, m_strClinicalDiagnosis(_T(""))
	, m_strPharmacy(_T(""))
	, m_strPastHistory(_T(""))
	, m_iStatus(0)
{

}

CPatientTab3::~CPatientTab3()
{
}

void CPatientTab3::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CLINICALDIAGNOSIS, m_strClinicalDiagnosis);
	DDX_Text(pDX, IDC_EDIT_PHARMACY, m_strPharmacy);
	DDX_Text(pDX, IDC_EDIT_PASTHISTORY, m_strPastHistory);
	DDV_MaxChars(pDX, m_strClinicalDiagnosis, 50);
	DDV_MaxChars(pDX, m_strPharmacy, 50);
	DDV_MaxChars(pDX, m_strPastHistory, 50);
	DDX_Control(pDX, IDC_STATUS, m_ctrlStatus);
	DDX_CBIndex(pDX, IDC_STATUS, m_iStatus);
}


BEGIN_MESSAGE_MAP(CPatientTab3, CDialog)
END_MESSAGE_MAP()


// CPatientTab3 message handlers

BOOL CPatientTab3::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctrlStatus.InsertString(0, CString("未处理"));
	m_ctrlStatus.InsertString(1, CString("已处理"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
