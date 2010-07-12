#pragma once
#include "afxwin.h"


// CPatientTab2 dialog

class CPatientTab2 : public CDialog
{
	DECLARE_DYNAMIC(CPatientTab2)

public:
	CPatientTab2(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPatientTab2();
	virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_PATIENT_TAB2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strProvince;
	CString m_strCity;
	CString m_strAddress;
	CString m_strZipCode;
	CString m_strTel;
	int m_nHeight;
	CString m_strWeight;
	CString m_strCheckDate;
	CString m_strHazards;
	CComboBox m_ctrlProvince;
	CComboBox m_ctrlCity;
	CComboBox m_ctrlHazards;
};
