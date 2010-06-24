#pragma once


// CConnectDlg dialog

class CConnectDlg : public CDialog
{
	DECLARE_DYNAMIC(CConnectDlg)

public:
	CConnectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConnectDlg();

// Dialog Data
	enum { IDD = IDD_CONNECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strAddress;
	UINT m_nPort;
	afx_msg void OnBnClickedConnect();
};
