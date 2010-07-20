#if !defined(AFX_ARCSTATISTICS_H__3B66EAC0_91E0_11D5_B47F_EF1FA653C239__INCLUDED_)
#define AFX_ARCSTATISTICS_H__3B66EAC0_91E0_11D5_B47F_EF1FA653C239__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ArcStatistics.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CArcStatistics dialog

class CArcStatistics : public CDialog
{
// Construction
public:
	CArcStatistics(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CArcStatistics)
	enum { IDD = IDD_STATISTICS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

CSmartZipDoc*m_pDoc;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArcStatistics)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CArcStatistics)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARCSTATISTICS_H__3B66EAC0_91E0_11D5_B47F_EF1FA653C239__INCLUDED_)
