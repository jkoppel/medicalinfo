#pragma once
#include "afxcmn.h"
#include "PatientTab1.h"
#include "PatientTab2.h"
#include "PatientTab3.h"
#include "GlobalVars.h"


// CPatientDlg dialog

class CPatientDlg : public CDialog
{
	DECLARE_DYNAMIC(CPatientDlg)

public:
	CPatientDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPatientDlg();
	virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_PATIENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_ctrlPatientTab;
	CPatientTab1 m_dlgTab1;
	CPatientTab2 m_dlgTab2;
	CPatientTab3 m_dlgTab3;
	struct UserData m_Data;
public:
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);

	void GetData(struct UserData &data);
	void SetData(struct UserData data);
public:
	afx_msg void OnBnClickedOk();
};
