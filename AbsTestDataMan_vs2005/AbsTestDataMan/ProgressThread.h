#pragma once

#include "DlgProgress.h"

// CProgressThread

class CProgressThread : public CWinThread
{
	DECLARE_DYNCREATE(CProgressThread)

protected:
	CProgressThread();           // protected constructor used by dynamic creation
	virtual ~CProgressThread();
public:
	CDlgProgress *m_pDlgProgress;

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	static CProgressThread *m_pThread;
	static void Start(int nCount = 30000);
	static void End();
	static void StepIt();
	static void SetPos(int nPos);

protected:
	DECLARE_MESSAGE_MAP()
};


