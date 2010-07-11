#pragma once



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
};


