#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "GlobalFuncs.h"
#include "GlobalVars.h"


// CMIMainDlg dialog

class CMIMainDlg : public CDialog
{
	DECLARE_DYNAMIC(CMIMainDlg)

public:
	CMIMainDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMIMainDlg();

// Dialog Data
	enum { IDD = IDD_MICLIENT_MAIN };

protected:
	void SetConnectStatus(BOOL);

	CString m_strAddress;
	UINT m_nPort;
	CListCtrl m_lstPatient;
	struct UserData m_pUserData[1000];
protected:
	afx_msg LRESULT OnDisconnect(WPARAM wParam, LPARAM lParam);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedConnect();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void SendCurPacket();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedEdit();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedSearch();

	void InitListBox();

	int CmdConnect();
	int CmdGetRecordNum(int &num);
	int CmdGetRecordAt(int index, struct UserData &data);
	int CmdAppendRecord(struct UserData data);
	int CmdDeleteRecordAt(int index);
};
