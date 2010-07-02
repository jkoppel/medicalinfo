// PatientTab3.cpp : implementation file
//

#include "stdafx.h"
#include "MIClient.h"
#include "PatientTab3.h"


// CPatientTab3 dialog

IMPLEMENT_DYNAMIC(CPatientTab3, CDialog)

CPatientTab3::CPatientTab3(CWnd* pParent /*=NULL*/)
	: CDialog(CPatientTab3::IDD, pParent)
	, m_strClinicalDiagnosis(_T(""))
	, m_strPharmacy(_T(""))
	, m_strPastHistory(_T(""))
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
}


BEGIN_MESSAGE_MAP(CPatientTab3, CDialog)
END_MESSAGE_MAP()


// CPatientTab3 message handlers
