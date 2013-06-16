#pragma once

#include "BaseGraphView.h"


// CFacetGraphView form view

class CFacetGraphView : public CBaseGraphView
{
	DECLARE_DYNCREATE(CFacetGraphView)

protected:
	CFacetGraphView();           // protected constructor used by dynamic creation
	virtual ~CFacetGraphView();

public:
	enum { IDD = IDD_FACET_GRAPH };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
protected:

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    virtual void OnDraw(CDC* /*pDC*/);
    virtual void OnInitialUpdate();
    afx_msg void OnBnClickedGraphinfoBtnEditsave();
};


