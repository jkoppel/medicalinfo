#pragma once

#include "ExtLibs\\SkinProgress\\SkinProgress.h"

#define WM_UPDATEDATA WM_USER + 1999

extern BOOL PeekAndPump();

// CDlgProgress dialog

///进程对话框类，用来提示进度，并允许用户取消，避免在执行过程中出现死机状态时间过长
class CDlgProgress : public CDialog
{
	DECLARE_DYNAMIC(CDlgProgress)

public:
	CDlgProgress(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgProgress();

// Dialog Data
	enum { IDD = IDD_PROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_iMaxPos;					//进度条的最大位置，默认100
	CString m_strPromptInfo;		//提示信息
	CSkinProgress *m_pProgress;		//进度条对象

	static LPVOID m_lpParam;
	static CDlgProgress *m_pDlg;	//对话框对象
	static CWinThread *m_pThread;	//启动的进程句柄
	static AFX_THREADPROC m_ThreadProc;//用户传入进来的进程函数，即用户要执行的任务在该函数中实现
	static BOOL StartThread(AFX_THREADPROC ThreadProc, LPCTSTR sPromptInfo=NULL, int iMaxPos=100);//启动进程
	static BOOL UpdateAndCheck(int iPercent);//更新百分比，并检查用户是否点了取消
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClose();
	LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam);
	virtual BOOL DestroyWindow();
};
