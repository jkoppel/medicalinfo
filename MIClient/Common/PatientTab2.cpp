// PatientTab2.cpp : implementation file
//

#include "stdafx.h"
#ifdef _WIN32_WCE
#include "..\\MIClient\\MIClient.h"
#else
#include "..\\MIClient_X86\\MIClient.h"
#endif
#include "PatientTab2.h"
#include "CfgFile.h"
#include "MIBase.h"
#include "GlobalFuncs.h"


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

BOOL CPatientTab2::OnInitDialog()
{
	CDialog::OnInitDialog();

	char szOptions[40][256];
	int num, i;

	//打开INI文件，读取省份，城市，危险因素等数据
	CfgFile cf;
	cf.OpenFile(CONFIG_FILE);

	if(cf.GetSectionOptions("PROVINCE", szOptions, 20, num)){
		m_nNumOfProvinceCode = 0;
		for(i=0;i<num;i++){
			CString str(szOptions[i]);
			int index = str.Find(_T("="));
			if(index>=0){
				m_strProvinceCode[m_nNumOfProvinceCode] = str.Left(index);
				m_strProvinceCode[m_nNumOfProvinceCode] = m_strProvinceCode[m_nNumOfProvinceCode].Trim();
				m_nNumOfProvinceCode ++;
				str = str.Right(str.GetLength()-index-1);
				str = str.Trim();
				m_ctrlProvince.InsertString(i, str);
			}
		}
	}
	if(cf.GetSectionOptions("HAZARDS", szOptions, 20, num)){
		for(i=0;i<num;i++){
			m_ctrlHazards.InsertString(i, CString(szOptions[i]));
		}
	}
	cf.CloseFile();

	return TRUE;
}

void CPatientTab2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_COMBO_PROVINCE, m_strProvince);
	DDX_Text(pDX, IDC_COMBO_CITY, m_strCity);
	DDX_Text(pDX, IDC_EDIT_ADDRESS, m_strAddress);
	DDX_Text(pDX, IDC_EDIT_ZIPCODE, m_strZipCode);
	DDX_Text(pDX, IDC_EDIT_TEL, m_strTel);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_nHeight);
	DDX_Text(pDX, IDC_EDIT_WEIGHT, m_strWeight);
	DDX_Text(pDX, IDC_EDIT_CHECKDATE, m_strCheckDate);
	DDX_Text(pDX, IDC_COMBO_HAZARDS, m_strHazards);
	DDV_MaxChars(pDX, m_strProvince, 50);
	DDV_MaxChars(pDX, m_strCity, 50);
	DDV_MaxChars(pDX, m_strAddress, 50);
	DDV_MaxChars(pDX, m_strZipCode, 50);
	DDV_MaxChars(pDX, m_strTel, 50);
	DDV_MaxChars(pDX, m_strWeight, 50);
	DDV_MaxChars(pDX, m_strCheckDate, 50);
	DDV_MaxChars(pDX, m_strHazards, 100);
	DDX_Control(pDX, IDC_COMBO_PROVINCE, m_ctrlProvince);
	DDX_Control(pDX, IDC_COMBO_CITY, m_ctrlCity);
	DDX_Control(pDX, IDC_COMBO_HAZARDS, m_ctrlHazards);
}


BEGIN_MESSAGE_MAP(CPatientTab2, CDialog)
ON_CBN_SELCHANGE(IDC_COMBO_PROVINCE, &CPatientTab2::OnCbnSelchangeProvince)
END_MESSAGE_MAP()


// CPatientTab2 message handlers

///省份下拉框变动
void CPatientTab2::OnCbnSelchangeProvince()
{
	char buf[256];
	CfgFile cf;
	char szOptions[50][256];
	int num;
	int index = m_ctrlProvince.GetCurSel();

	if(index>=0){
		CString2Char(m_strProvinceCode[index], buf);

		//重新读入该省份对应的城市值
		cf.OpenFile(CONFIG_FILE);
		cf.GetSectionOptions(buf, szOptions, 50, num);
		cf.CloseFile();
		m_ctrlCity.ResetContent();
		//m_ctrlZipcode.ResetContent();
		for(int i=0;i<num;i++){
			CString str = CString(szOptions[i]);
			int pos = str.Find(_T("="));
			if(pos>=1){
				CString str_city = str.Left(pos);
				str_city = str_city.Trim();
				m_ctrlCity.InsertString(m_ctrlCity.GetCount(), str_city);
/*
				str = str.Right(str.GetLength()-pos-1);
				str = str.Trim();
				pos = str.Find(_T(","));
				if(pos>=1){
					CString str_zipcode = str.Left(pos);
					m_ctrlZipcode.InsertString(m_ctrlZipcode.GetCount(), str_zipcode);
				}
*///可能的邮政编码输入
			}
		}
	}

}
