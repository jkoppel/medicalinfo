// PatientTab1.cpp : implementation file
//

#include "stdafx.h"
#ifdef _WIN32_WCE
#include "..\\MIClient\\MIClient.h"
#else
#include "..\\MIClient_X86\\MIClient.h"
#endif
#include "PatientTab1.h"
#include "MIBase.h"
#include "CfgFile.h"


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

BOOL CPatientTab1::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctrlSex.InsertString(0, _T("ÄÐ"));
	m_ctrlSex.InsertString(1, _T("Å®"));

	char szOptions[20][256];
	int num, i;

	CfgFile cf;
	cf.OpenFile(CONFIG_FILE);
	if(cf.GetSectionOptions("DEPARTMENT", szOptions, 20, num)){
		for(i=0;i<num;i++){
			m_ctrlDepartment.InsertString(i, CString(szOptions[i]));
		}
	}
	if(cf.GetSectionOptions("PROFESSION", szOptions, 20, num)){
		for(i=0;i<num;i++){
			m_ctrlTypeOfWork.InsertString(i, CString(szOptions[i]));
		}
	}
	cf.CloseFile();
	return TRUE;
}

void CPatientTab1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SCANCODEID, m_strScancodeID);
	DDX_Text(pDX, IDC_EDIT_NUMBER, m_nNumber);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDX_Text(pDX, IDC_COMBO_SEX, m_strSex);
	DDX_Text(pDX, IDC_EDIT_AGE, m_nAge);
	DDX_Text(pDX, IDC_EDIT_BIRTHDATE, m_strBirthDate);
	DDX_Text(pDX, IDC_EDIT_PEOPLE, m_strPeople);
	DDX_Text(pDX, IDC_COMBO_DEPARTMENT, m_strDepartment);
	DDX_Text(pDX, IDC_COMBO_TYPEOFWORK, m_strTypeOfWork);
	DDV_MaxChars(pDX, m_strScancodeID, 50);
	DDV_MaxChars(pDX, m_strName, 50);
	DDV_MaxChars(pDX, m_strSex, 50);
	DDV_MaxChars(pDX, m_strBirthDate, 50);
	DDV_MaxChars(pDX, m_strPeople, 50);
	DDV_MaxChars(pDX, m_strDepartment, 50);
	DDV_MaxChars(pDX, m_strTypeOfWork, 50);
	DDX_Control(pDX, IDC_COMBO_SEX, m_ctrlSex);
	DDX_Control(pDX, IDC_COMBO_DEPARTMENT, m_ctrlDepartment);
	DDX_Control(pDX, IDC_COMBO_TYPEOFWORK, m_ctrlTypeOfWork);
}


BEGIN_MESSAGE_MAP(CPatientTab1, CDialog)
END_MESSAGE_MAP()


// CPatientTab1 message handlers
