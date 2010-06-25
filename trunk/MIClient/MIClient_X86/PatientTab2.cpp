// PatientTab2.cpp : implementation file
//

#include "stdafx.h"
#include "MIClient.h"
#include "PatientTab2.h"


// CPatientTab2 dialog

IMPLEMENT_DYNAMIC(CPatientTab2, CDialog)

CPatientTab2::CPatientTab2(CWnd* pParent /*=NULL*/)
	: CDialog(CPatientTab2::IDD, pParent)
	, m_strProvince(_T(""))
	, m_strCity(_T(""))
	, m_strAddress(_T(""))
	, m_strZipCode(_T(""))
	, m_strTel(_T(""))
	, m_nHeight(0)
	, m_strWeight(_T(""))
	, m_strCheckDate(_T(""))
	, m_strHazards(_T(""))
{

}

CPatientTab2::~CPatientTab2()
{
}

void CPatientTab2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PROVINCE, m_strProvince);
	DDX_Text(pDX, IDC_EDIT_CITY, m_strCity);
	DDX_Text(pDX, IDC_EDIT_ADDRESS, m_strAddress);
	DDX_Text(pDX, IDC_EDIT_ZIPCODE, m_strZipCode);
	DDX_Text(pDX, IDC_EDIT_TEL, m_strTel);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_nHeight);
	DDX_Text(pDX, IDC_EDIT_WEIGHT, m_strWeight);
	DDX_Text(pDX, IDC_EDIT_CHECKDATE, m_strCheckDate);
	DDX_Text(pDX, IDC_EDIT_HAZARDS, m_strHazards);
}


BEGIN_MESSAGE_MAP(CPatientTab2, CDialog)
END_MESSAGE_MAP()


// CPatientTab2 message handlers
