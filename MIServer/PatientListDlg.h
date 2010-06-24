#if !defined(AFX_PATIENTLISTDLG_H__F4A7B2DA_C9DA_47CD_8282_F4A7871F4144__INCLUDED_)
#define AFX_PATIENTLISTDLG_H__F4A7B2DA_C9DA_47CD_8282_F4A7871F4144__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PatientListDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPatientListDlg dialog

class CPatientListDlg : public CDialog
{
// Construction
public:
	CPatientListDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPatientListDlg)
	enum { IDD = IDD_PATIENT_LIST };
	CListCtrl	m_lstPatient;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatientListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_nPatientNum;
	UINT m_nID;
	CString	m_strAddress;
	UINT	m_nAge;
	CString	m_strBirthDate;
	CString	m_strCheckDate;
	CString	m_strCity;
	CString	m_strClinicalDiagnosis;
	CString	m_strDepartment;
	CString	m_strHazards;
	CString	m_strName;
	UINT	m_nNumber;
	UINT	m_nHeight;
	CString	m_strPastHistory;
	CString	m_strPeople;
	CString	m_strPharmacy;
	CString	m_strProvince;
	CString	m_strScancodeID;
	CString	m_strSex;
	CString	m_strTel;
	CString	m_strTypeOfWork;
	CString	m_strWeight;
	CString	m_strZipCode;

	void InitListBox();
	void GetPatientData();

	// Generated message map functions
	//{{AFX_MSG(CPatientListDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATIENTLISTDLG_H__F4A7B2DA_C9DA_47CD_8282_F4A7871F4144__INCLUDED_)
