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
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CString m_strClinicalDiagnosis;
	CString m_strPharmacy;
	CString m_strPastHistory;
	CComboBox m_ctrlStatus;
	CComboBox m_ctrlPharmacy;
	CComboBox m_ctrlPastHistory;

	int m_iStatus;
};
