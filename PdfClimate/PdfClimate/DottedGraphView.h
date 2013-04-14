#pragma once



// CDottedGraphView form view

class CDottedGraphView : public CFormView
{
	DECLARE_DYNCREATE(CDottedGraphView)

protected:
	CDottedGraphView();           // protected constructor used by dynamic creation
	virtual ~CDottedGraphView();

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


