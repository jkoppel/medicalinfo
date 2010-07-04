#pragma once
#include "afxwin.h"


// CPatientTab2 dialog

class CPatientTab2 : public CDialog
{
	DECLARE_DYNAMIC(CPatientTab2)

public:
	CPatientTab2(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPatientTab2();

// Dialog Data
	enum { IDD = IDD_PATIENT_TAB2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strProvince;
public:
	CString m_strCity;
public:
	CString m_strAddress;
public:
	CString m_strZipCode;
public:
	CString m_strTel;
public:
	int m_nHeight;
public:
	CString m_strWeight;
public:
	CString m_strCheckDate;
public:
	CString m_strHazards;
};
