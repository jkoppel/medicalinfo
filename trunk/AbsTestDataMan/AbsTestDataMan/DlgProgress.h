#pragma once

#include "SkinProgress\\SkinProgress.h"

#define WM_UPDATEDATA WM_USER + 1999

extern BOOL PeekAndPump();

// CDlgProgress dialog

class CDlgProgress : public CDialog
{
	DECLARE_DYNAMIC(CDlgProgress)

public:
	CDlgProgress(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgProgress();

// Dialog Data
	enum { IDD = IDD_PROGRESS };
	enum { THREAD_LOAD_NODE = 1,};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CSkinProgress *m_pProgress;

	int m_iThreadType;
	static CDlgProgress *m_pDlg;
	static CWinThread *m_pThread;
	static UINT ThreadProc(LPVOID lpParam);
public:
	void SetThreadType(int iThreadType);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClose();
	LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam);
	virtual BOOL DestroyWindow();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
