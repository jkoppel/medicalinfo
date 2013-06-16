#pragma once
#include "BaseGraphView.h"


// CUnknownGraphView form view

class CUnknownGraphView : public CBaseGraphView
{
	DECLARE_DYNCREATE(CUnknownGraphView)

protected:
	CUnknownGraphView();           // protected constructor used by dynamic creation
	virtual ~CUnknownGraphView();

public:
	enum { IDD = IDD_UNKNOWN_GRAPH };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
    virtual void OnDraw(CDC* /*pDC*/);
public:
    virtual void OnInitialUpdate();
    afx_msg void OnBnClickedGraphinfoBtnEditsave();
};


