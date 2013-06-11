#pragma once
#include "GraphFormView.h"


// CLinearGraphView form view

class CLinearGraphView : public CGraphFormView
{
	DECLARE_DYNCREATE(CLinearGraphView)

protected:
	CLinearGraphView();           // protected constructor used by dynamic creation
	virtual ~CLinearGraphView();

public:
	enum { IDD = IDD_LINEAR_GRAPH };
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
};


