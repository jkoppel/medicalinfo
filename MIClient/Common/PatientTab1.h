#pragma once


// CPatientTab1 dialog

class CPatientTab1 : public CDialog
{
	DECLARE_DYNAMIC(CPatientTab1)

public:
	CPatientTab1(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPatientTab1();

// Dialog Data
	enum { IDD = IDD_PATIENT_TAB1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strScancodeID;
public:
	int m_nNumber;
public:
	CString m_strName;
public:
	CString m_strSex;
public:
	int m_nAge;
public:
	CString m_strBirthDate;
public:
	CString m_strPeople;
public:
	CString m_strDepartment;
public:
	CString m_strTypeOfWork;
};
