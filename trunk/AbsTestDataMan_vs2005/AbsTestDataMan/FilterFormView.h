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
	CString m_strStartTime;
	CStatic m_ctrlGroupFilter;
	CString m_strEndTime;
	CString m_strProductNo;
	CString m_strOperatorNo;
	afx_msg void OnBnClickedBtnTest1();
	afx_msg void OnBnClickedBtnTest2();
	afx_msg void OnBnClickedBtnTest3();
	CDateTimeCtrl m_ctrlDT_Start_Time;
	CDateTimeCtrl m_ctrlDT_Start_Date;
	CDateTimeCtrl m_ctrlDT_End_Date;
	CDateTimeCtrl m_ctrlDT_End_Time;
};


