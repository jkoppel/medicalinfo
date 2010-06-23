#pragma once


// CPatientDataDlg dialog

class CPatientDataDlg : public CDialog
{
	DECLARE_DYNAMIC(CPatientDataDlg)

public:
	CPatientDataDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPatientDataDlg();
	virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_REC_ADDMOD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_nID;
	CString m_strName;
	int m_nFlag;
	afx_msg void OnBnClickedOk();
};
