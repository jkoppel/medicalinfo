// PatientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "miserver.h"
#include "PatientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPatientDlg dialog

///病人列表对话框，也是供调试使用，实际运行时和本文件没有关系


CPatientDlg::CPatientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPatientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPatientDlg)
	m_strAddress = _T("");
	m_nAge = 0;
	m_strBirthDate = _T("");
	m_strCheckDate = _T("");
	m_strCity = _T("");
	m_strClinicalDiagnosis = _T("");
	m_strDepartment = _T("");
	m_strHazards = _T("");
	m_strName = _T("");
	m_nNumber = 0;
	m_nHeight = 0;
	m_strPastHistory = _T("");
	m_strPeople = _T("");
	m_strPharmacy = _T("");
	m_strProvince = _T("");
	m_strScancodeID = _T("");
	m_strSex = _T("");
	m_strTel = _T("");
	m_strTypeOfWork = _T("");
	m_strWeight = _T("");
	m_strZipCode = _T("");
	//}}AFX_DATA_INIT
}


void CPatientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatientDlg)
	DDX_Text(pDX, IDC_EDIT_ADDRESS, m_strAddress);
	DDX_Text(pDX, IDC_EDIT_AGE, m_nAge);
	DDV_MinMaxUInt(pDX, m_nAge, 1, 150);
	DDX_Text(pDX, IDC_EDIT_BIRTHDATE, m_strBirthDate);
	DDX_Text(pDX, IDC_EDIT_CHECKDATE, m_strCheckDate);
	DDX_Text(pDX, IDC_EDIT_CITY, m_strCity);
	DDX_Text(pDX, IDC_EDIT_CLINICALDIAGNOSIS, m_strClinicalDiagnosis);
	DDX_Text(pDX, IDC_EDIT_DEPARTMENT, m_strDepartment);
	DDX_Text(pDX, IDC_EDIT_HAZARDS, m_strHazards);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_NUMBER, m_nNumber);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_nHeight);
	DDV_MinMaxUInt(pDX, m_nHeight, 0, 250);
	DDX_Text(pDX, IDC_EDIT_PASTHISTORY, m_strPastHistory);
	DDX_Text(pDX, IDC_EDIT_PEOPLE, m_strPeople);
	DDX_Text(pDX, IDC_EDIT_PHARMACY, m_strPharmacy);
	DDX_Text(pDX, IDC_EDIT_PROVINCE, m_strProvince);
	DDX_Text(pDX, IDC_EDIT_SCANCODEID, m_strScancodeID);
	DDX_Text(pDX, IDC_EDIT_SEX, m_strSex);
	DDX_Text(pDX, IDC_EDIT_TEL, m_strTel);
	DDX_Text(pDX, IDC_EDIT_TYPEOFWORK, m_strTypeOfWork);
	DDX_Text(pDX, IDC_EDIT_WEIGHT, m_strWeight);
	DDX_Text(pDX, IDC_EDIT_ZIPCODE, m_strZipCode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPatientDlg, CDialog)
	//{{AFX_MSG_MAP(CPatientDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatientDlg message handlers

BOOL CPatientDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPatientDlg::OnOK() 
{
	UpdateData(TRUE);
	if(m_strScancodeID.IsEmpty()){
		AfxMessageBox("请输入扫描码编号");
		return;
	}
	if(m_nNumber<=0){
		AfxMessageBox("请输入病案号");
		return;
	}
	if(m_strName.IsEmpty()){
		AfxMessageBox("请输入姓名");
		return;
	}
	
	CDialog::OnOK();
}

void CPatientDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}
