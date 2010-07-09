// PatientDlg.cpp : implementation file
//

#include "stdafx.h"
#ifdef _WIN32_WCE
#include "..\\MIClient\\MIClient.h"
#else
#include "..\\MIClient_X86\\MIClient.h"
#endif
#include "PatientDlg.h"
#include "GlobalFuncs.h"
#include "GlobalVars.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// CPatientDlg dialog

IMPLEMENT_DYNAMIC(CPatientDlg, CDialog)

CPatientDlg::CPatientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPatientDlg::IDD, pParent)
{
	memset(&m_Data, 0, sizeof(m_Data));
}

CPatientDlg::~CPatientDlg()
{
}

void CPatientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_ctrlPatientTab);
}


BEGIN_MESSAGE_MAP(CPatientDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CPatientDlg::OnTcnSelchangeTab)
	ON_BN_CLICKED(IDOK, &CPatientDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CPatientDlg message handlers
BOOL CPatientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(m_sTitle);;

	m_ctrlPatientTab.InsertItem(0, CString("资料1"));
	m_ctrlPatientTab.InsertItem(1, CString("资料2"));
	m_ctrlPatientTab.InsertItem(2, CString("资料3"));

	m_dlgTab1.Create(IDD_PATIENT_TAB1,GetDlgItem(IDC_TAB));
	m_dlgTab1.SetParent(GetDlgItem(IDC_TAB));
	m_dlgTab2.Create(IDD_PATIENT_TAB2,GetDlgItem(IDC_TAB));
	m_dlgTab2.SetParent(GetDlgItem(IDC_TAB));
	m_dlgTab3.Create(IDD_PATIENT_TAB3,GetDlgItem(IDC_TAB));
	m_dlgTab3.SetParent(GetDlgItem(IDC_TAB));

	CRect rs; 
	m_dlgTab3.GetClientRect(&rs);
	rs.top+=25;
	rs.bottom+=24;
	rs.left+=5;
	rs.right+=5;

	m_dlgTab1.MoveWindow(&rs);
	m_dlgTab2.MoveWindow(&rs);
	m_dlgTab3.MoveWindow(&rs);

	m_ctrlPatientTab.SetCurSel(0);
	m_dlgTab1.ShowWindow(SW_SHOW);
	m_dlgTab2.ShowWindow(SW_HIDE);
	m_dlgTab3.ShowWindow(SW_HIDE);

	SetData(m_Data);

	SetWindowPos(NULL, 0, 0, 240, 300, SWP_SHOWWINDOW);


	return TRUE;
}


void CPatientDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int CurSel = m_ctrlPatientTab.GetCurSel(); 
    switch(CurSel) 
    { 
    case 0: 
        m_dlgTab1.ShowWindow(SW_SHOW); 
        m_dlgTab2.ShowWindow(SW_HIDE); 
        m_dlgTab3.ShowWindow(SW_HIDE); 
        break; 
    case 1: 
        m_dlgTab1.ShowWindow(SW_HIDE); 
        m_dlgTab2.ShowWindow(SW_SHOW); 
        m_dlgTab3.ShowWindow(SW_HIDE); 
        break; 
    case 2: 
        m_dlgTab1.ShowWindow(SW_HIDE); 
        m_dlgTab2.ShowWindow(SW_HIDE); 
        m_dlgTab3.ShowWindow(SW_SHOW); 
        break; 
    default: 
		break;
    }     
	*pResult = 0;
}

void CPatientDlg::SetData(struct UserData data)
{
	m_dlgTab1.m_strScancodeID = CString(data.ScancodeID);
	m_dlgTab1.m_nNumber = data.Number;
	m_dlgTab1.m_strName = CString(data.Name);
	m_dlgTab1.m_strSex = CString(data.Sex);
	m_dlgTab1.m_nAge = data.Age;
	m_dlgTab1.m_strBirthDate = CString(data.BirthDate);
	m_dlgTab1.m_strPeople = CString(data.People);
	m_dlgTab1.m_strDepartment = CString(data.Department);
	m_dlgTab1.m_strTypeOfWork = CString(data.TypeOfWork);
	m_dlgTab2.m_strProvince = CString(data.Province);
	m_dlgTab2.m_strCity = CString(data.City);
	m_dlgTab2.m_strAddress = CString(data.Address);
	m_dlgTab2.m_strZipCode = CString(data.ZipCode);
	m_dlgTab2.m_strTel = CString(data.Tel);
	m_dlgTab2.m_nHeight = data.Height;
	m_dlgTab2.m_strWeight = CString(data.Weight);
	m_dlgTab2.m_strCheckDate = CString(data.CheckDate);
	m_dlgTab2.m_strHazards = CString(data.Hazards);
	m_dlgTab3.m_strClinicalDiagnosis = CString(data.ClinicalDiagnosis);
	m_dlgTab3.m_strPharmacy = CString(data.Pharmacy);
	m_dlgTab3.m_strPastHistory = CString(data.PastHistory);
	m_dlgTab3.m_iStatus = data.Status;

	m_dlgTab1.UpdateData(FALSE);
	m_dlgTab2.UpdateData(FALSE);
	m_dlgTab3.UpdateData(FALSE);
}

void CPatientDlg::GetData(struct UserData &data)
{
	char buf[1024];

	CString2Char(m_dlgTab1.m_strScancodeID, buf);
	sprintf_s(data.ScancodeID, "%s", buf);
	data.Number = m_dlgTab1.m_nNumber;
	CString2Char(m_dlgTab1.m_strName, buf);
	sprintf_s(data.Name, "%s", buf);
	CString2Char(m_dlgTab1.m_strSex, buf);
	sprintf_s(data.Sex, "%s", buf);
	data.Age = m_dlgTab1.m_nAge;
	CString2Char(m_dlgTab1.m_strBirthDate, buf);
	sprintf_s(data.BirthDate, "%s", buf);
	CString2Char(m_dlgTab1.m_strPeople, buf);
	sprintf_s(data.People, "%s", buf);
	CString2Char(m_dlgTab1.m_strDepartment, buf);
	sprintf_s(data.Department, "%s", buf);
	CString2Char(m_dlgTab1.m_strTypeOfWork, buf);
	sprintf_s(data.TypeOfWork, "%s", buf);
	CString2Char(m_dlgTab2.m_strProvince, buf);
	sprintf_s(data.Province, "%s", buf);
	CString2Char(m_dlgTab2.m_strCity, buf);
	sprintf_s(data.City, "%s", buf);
	CString2Char(m_dlgTab2.m_strAddress, buf);
	sprintf_s(data.Address, "%s", buf);
	CString2Char(m_dlgTab2.m_strZipCode, buf);
	sprintf_s(data.ZipCode, "%s", buf);
	CString2Char(m_dlgTab2.m_strTel, buf);
	sprintf_s(data.Tel, "%s", buf);
	data.Height = m_dlgTab2.m_nHeight;
	CString2Char(m_dlgTab2.m_strWeight, buf);
	sprintf_s(data.Weight, "%s", buf);
	CString2Char(m_dlgTab2.m_strCheckDate, buf);
	sprintf_s(data.CheckDate, "%s", buf);
	CString2Char(m_dlgTab2.m_strHazards, buf);
	_snprintf_s(data.Hazards, sizeof(data.Hazards), "%s", buf);
	CString2Char(m_dlgTab3.m_strClinicalDiagnosis, buf);
	_snprintf_s(data.ClinicalDiagnosis, sizeof(data.ClinicalDiagnosis), "%s", buf);
	CString2Char(m_dlgTab3.m_strPharmacy, buf);
	_snprintf_s(data.Pharmacy, sizeof(data.Pharmacy), "%s", buf);
	CString2Char(m_dlgTab3.m_strPastHistory, buf);
	_snprintf_s(data.PastHistory, sizeof(data.PastHistory), "%s", buf);

	data.Status = m_dlgTab3.m_iStatus;
}

void CPatientDlg::OnBnClickedOk()
{
	if(!m_dlgTab1.UpdateData(TRUE)){
		return;
	}
	if(!m_dlgTab2.UpdateData(TRUE)){
		return;
	}
	if(!m_dlgTab3.UpdateData(TRUE)){
		return;
	}

	if(m_dlgTab1.m_strName.IsEmpty()){
		AfxMessageBox(CString("请输入姓名"));
		return;
	}

	OnOK();
}

