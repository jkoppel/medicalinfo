#pragma once



// CColumnarGraphView form view

class CColumnarGraphView : public CFormView
{
	DECLARE_DYNCREATE(CColumnarGraphView)

protected:
	CColumnarGraphView();           // protected constructor used by dynamic creation
	virtual ~CColumnarGraphView();

public:
	enum { IDD = IDD_COLUMNAR };
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


