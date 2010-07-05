#pragma once
#include "afxwin.h"



// CFormViewEx form view

class CFormViewEx : public CFormView
{
	DECLARE_DYNCREATE(CFormViewEx)

protected:
	CFormViewEx();           // protected constructor used by dynamic creation
	virtual ~CFormViewEx();

public:
	enum { IDD = IDD_FORMVIEWEX };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CString m_strStartTime;
	CStatic m_ctrlGroupFilter;
	CString m_strEndTime;
	CString m_strProductNo;
	CString m_strOperatorNo;
};


