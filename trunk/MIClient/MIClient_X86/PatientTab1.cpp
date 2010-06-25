// PatientTab1.cpp : implementation file
//

#include "stdafx.h"
#include "MIClient.h"
#include "PatientTab1.h"


// CPatientTab1 dialog

IMPLEMENT_DYNAMIC(CPatientTab1, CDialog)

CPatientTab1::CPatientTab1(CWnd* pParent /*=NULL*/)
	: CDialog(CPatientTab1::IDD, pParent)
	, m_strScancodeID(_T(""))
	, m_nNumber(0)
	, m_strName(_T(""))
	, m_strSex(_T(""))
	, m_nAge(0)
	, m_strBirthDate(_T(""))
	, m_strPeople(_T(""))
	, m_strDepartment(_T(""))
	, m_strTypeOfWork(_T(""))
{

}

CPatientTab1::~CPatientTab1()
{
}

void CPatientTab1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SCANCODEID, m_strScancodeID);
	DDX_Text(pDX, IDC_EDIT_NUMBER, m_nNumber);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_SEX, m_strSex);
	DDX_Text(pDX, IDC_EDIT_AGE, m_nAge);
	DDX_Text(pDX, IDC_EDIT_BIRTHDATE, m_strBirthDate);
	DDX_Text(pDX, IDC_EDIT_PEOPLE, m_strPeople);
	DDX_Text(pDX, IDC_EDIT_DEPARTMENT, m_strDepartment);
	DDX_Text(pDX, IDC_EDIT_TYPEOFWORK, m_strTypeOfWork);
}


BEGIN_MESSAGE_MAP(CPatientTab1, CDialog)
END_MESSAGE_MAP()


// CPatientTab1 message handlers
