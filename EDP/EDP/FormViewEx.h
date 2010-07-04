#pragma once



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
};


