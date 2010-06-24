#if !defined(AFX_PRGRESSINFO_H__E11D1045_17DC_43D8_9F96_DA9768C52D54__INCLUDED_)
#define AFX_PRGRESSINFO_H__E11D1045_17DC_43D8_9F96_DA9768C52D54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// prgressinfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProgressInfo dialog

///������ʾ��,��ʹ��g_ProgressInfo����ʾ��Ϣ
/**
  * ʹ�÷���:	\n
  * (1)��ʾ,��g_ProgressInfo.Show("���ڽ��п�Ʊ...");	\n
  * (2)���о���Ĳ���,�翪Ʊ	\n
  * (3)����,g_ProgressInfo.Hide();	\n
  * ��� : testdlg.h, testdlg.cpp
  */
class CProgressInfo : public CDialog
{
// Construction
public:
	CProgressInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProgressInfo)
	enum { IDD = IDD_PROGRESS_INFO };
	//}}AFX_DATA

	void Show(const char *Promt);
	void Hide();
	void Destroy();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProgressInfo)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int m_WndCreateFlag;
};

extern CProgressInfo g_ProgressInfo;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRGRESSINFO_H__E11D1045_17DC_43D8_9F96_DA9768C52D54__INCLUDED_)
