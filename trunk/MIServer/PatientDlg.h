#if !defined(AFX_PATIENTDLG_H__8C4612A3_8AF5_458D_950F_821D81950148__INCLUDED_)
#define AFX_PATIENTDLG_H__8C4612A3_8AF5_458D_950F_821D81950148__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PatientDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPatientDlg dialog

class CPatientDlg : public CDialog
{
// Construction
public:
	CPatientDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPatientDlg)
	enum { IDD = IDD_PATIENT_ADDMOD };
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
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatientDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPatientDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATIENTDLG_H__8C4612A3_8AF5_458D_950F_821D81950148__INCLUDED_)
