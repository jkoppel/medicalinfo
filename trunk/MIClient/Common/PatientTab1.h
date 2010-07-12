#pragma once


// CPatientTab1 dialog

class CPatientTab1 : public CDialog
{
	DECLARE_DYNAMIC(CPatientTab1)

public:
	CPatientTab1(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPatientTab1();
	virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_PATIENT_TAB1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strScancodeID;
	int m_nNumber;
	CString m_strName;
	CString m_strSex;
	int m_nAge;
	CString m_strBirthDate;
	CString m_strPeople;
	CString m_strDepartment;
	CString m_strTypeOfWork;
	CComboBox m_ctrlSex;
	CComboBox m_ctrlDepartment;
	CComboBox m_ctrlTypeOfWork;
};
