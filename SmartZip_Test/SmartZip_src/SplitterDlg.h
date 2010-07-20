// SplitterDlg.h : header file
//
//{{AFX_INCLUDES()
#include "msflexgrid.h"
//}}AFX_INCLUDES

#if !defined(AFX_SPLITTERDLG_H__9B480139_0A77_4D66_9CD6_F8FF5B884E51__INCLUDED_)
#define AFX_SPLITTERDLG_H__9B480139_0A77_4D66_9CD6_F8FF5B884E51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BtnST.h"

/////////////////////////////////////////////////////////////////////////////
// CSplitterDlg dialog

class CSplitterDlg : public CDialog
{
// Construction
public:
	CSplitterDlg(CWnd* pParent = NULL);	// standard constructor
int length;
// Dialog Data
	//{{AFX_DATA(CSplitterDlg)
	enum { IDD = IDD_SPLITTER_DIALOG };
	CButtonST	m_close;
	CButtonST	m_export;
	CButtonST	m_calc;
	CEdit	m_filesize;
	CEdit	m_output;
	CEdit	m_file;
	CMSFlexGrid	m_flex;
	int		m_bps;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSplitterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChoose();
	afx_msg void OnCalcSpecific();
	afx_msg void OnExport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPLITTERDLG_H__9B480139_0A77_4D66_9CD6_F8FF5B884E51__INCLUDED_)
