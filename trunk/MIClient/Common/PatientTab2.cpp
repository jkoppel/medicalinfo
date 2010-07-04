// PatientTab2.cpp : implementation file
//

#include "stdafx.h"
#ifdef _WIN32_WCE
#include "..\\MIClient\\MIClient.h"
#else
#include "..\\MIClient_X86\\MIClient.h"
#endif
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
	DDV_MaxChars(pDX, m_strProvince, 50);
	DDV_MaxChars(pDX, m_strCity, 50);
	DDV_MaxChars(pDX, m_strAddress, 50);
	DDV_MaxChars(pDX, m_strZipCode, 50);
	DDV_MaxChars(pDX, m_strTel, 50);
	DDV_MaxChars(pDX, m_strWeight, 50);
	DDV_MaxChars(pDX, m_strCheckDate, 50);
	DDV_MaxChars(pDX, m_strHazards, 100);
}


BEGIN_MESSAGE_MAP(CPatientTab2, CDialog)
END_MESSAGE_MAP()


// CPatientTab2 message handlers
