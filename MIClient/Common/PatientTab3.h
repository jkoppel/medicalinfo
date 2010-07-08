#pragma once


// CPatientTab3 dialog

class CPatientTab3 : public CDialog
{
	DECLARE_DYNAMIC(CPatientTab3)

public:
	CPatientTab3(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPatientTab3();

// Dialog Data
	enum { IDD = IDD_PATIENT_TAB3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strClinicalDiagnosis;
public:
	CString m_strPharmacy;
public:
	CString m_strPastHistory;
	CComboBox m_ctrlStatus;
	virtual BOOL OnInitDialog();
	int m_iStatus;
};
