#if !defined(AFX_PROGRESS_H__2C2F1BEE_04E1_4838_B3E4_24407FB50863__INCLUDED_)
#define AFX_PROGRESS_H__2C2F1BEE_04E1_4838_B3E4_24407FB50863__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Progress.h : header file
//
#include "smartzipDoc.h"
#include "TaskTimer.h"
/////////////////////////////////////////////////////////////////////////////
// CDProgress dialog
UINT Test (LPVOID pParam);
UINT Extract (LPVOID pParam);
UINT Delete (LPVOID pParam);
UINT Add (LPVOID pParam);
struct CAddInfo:public CObject
{
	CAddInfo(){}
	CAddInfo(const CString& szName, DWORD iSize, bool bDir = false)
		:m_szName(szName), m_iSize(iSize), m_bDir(bDir){}
	CString m_szName;
	bool m_bDir;
	DWORD m_iSize;
	
};
class CDProgress : public CDialog
{
// Construction
public:
	CDProgress(CWnd* pParent = NULL);   // standard constructor
	~CDProgress()
	{
		if(m_progressdata)
			delete m_progressdata;
	}
	struct CProgressData : public CZipActionCallback
	{
		CProgressData(CWnd* win)
		{	pWnd=win;}
		void begin(double iMax)
		{
			m_iPos = 0;max=iMax;
			pWnd->SendMessage (WM_USER_THREAD_UPDATE,100,-1);
			//	Send();
		}
		void CallbackEnd()
		{
		//	if (IsSubAction())
		//		m_bInit = true;
		}
		virtual bool Callback(ZIP_SIZE_TYPE iProgress)//modified by hwy
		{
		//	if (IsStopOnly() && IsStopping())
		//		return false; // can break safely
			m_iPos += iProgress;
			pWnd->PostMessage (WM_USER_THREAD_UPDATE,(int)(m_iPos/max*100));//m_iPos
			return TRUE;
		}
		CWnd *pWnd;
		double m_iPos,max;
	};
	class CFileFindEnh : public CFileFind
	{
	public:
		LPWIN32_FIND_DATA GetInfo() 
		{
			ASSERT(m_pFoundInfo);
			return (LPWIN32_FIND_DATA) m_pFoundInfo;
		}
	};	
	
	BOOL AddFolder(CString szFolder, CObArray &array);

TaskTimer tasktimer;

	CZipArchive*zip;
	CSmartZipDoc* m_pDoc;
	CProgressData *m_progressdata;
	CCriticalSection m_cs;
	CEvent m_event;
	HANDLE m_hThread;
	BOOL m_bWorking;
//	int m_nPercentDone;
	CString m_operation,m_extractpath;
	THREADPARMS* ptp;
	CString m_text;
	void *pReserved;
	BOOL pFull, bOverwrite; //full path in extract
BOOL bEnc;
// Dialog Data
	//{{AFX_DATA(CDProgress)
	enum { IDD = IDD_EXTRACT };
	CStatic	m_dir;
	CStatic	m_filename;
	CButton	m_cancel;
	CProgressCtrl	m_progress;
	CAnimateCtrl	m_animate;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDProgress)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDProgress)
	virtual BOOL OnInitDialog();
	afx_msg void OnStopClose();
	//}}AFX_MSG
	afx_msg LRESULT OnThreadUpdate (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnThreadFinished (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnThreadAborted (WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRESS_H__2C2F1BEE_04E1_4838_B3E4_24407FB50863__INCLUDED_)
