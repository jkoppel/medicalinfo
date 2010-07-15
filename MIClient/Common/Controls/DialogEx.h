#pragma once


// CDialogEx dialog

class CDialogEx : public CDialog
{
	DECLARE_DYNAMIC(CDialogEx)

public:
	//CDialogEx(CWnd* pParent = NULL);   // standard constructor
	CDialogEx(CWnd* pParent=NULL, UINT nIDTemplate=100);
	virtual ~CDialogEx();

// Dialog Data
	enum { IDD = 100 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_nID;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
