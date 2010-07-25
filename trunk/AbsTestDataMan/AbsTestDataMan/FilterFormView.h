#pragma once
#include "afxwin.h"
#include "Resource.h"
#include "afxdtctl.h"


// CFilterFormView form view

class CFilterFormView : public CFormView
{
	DECLARE_DYNCREATE(CFilterFormView)

protected:
	CFilterFormView();           // protected constructor used by dynamic creation
	virtual ~CFilterFormView();

public:
	enum { IDD = IDD_FILTERFORMVIEW };
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
	afx_msg void OnBnClickedBtnTest1();
	afx_msg void OnBnClickedBtnTest2();
	afx_msg void OnBnClickedBtnTest3();
	afx_msg void OnBnClickedCheckTime();

	BOOL m_bCheckTime;
	CString m_strProductNo;
	CString m_strOperatorNo;

	CStatic m_ctrlGroupFilter;
	CButton m_ctrlCheckTime;
	CDateTimeCtrl m_ctrlStartTime;
	CDateTimeCtrl m_ctrlEndTime;
	CEdit m_ctrlProductNo;
	CEdit m_ctrlOperatorNo;
	CTime m_tStartTime;
	CTime m_tEndTime;
};


