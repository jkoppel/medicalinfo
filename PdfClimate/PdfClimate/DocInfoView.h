#pragma once
#include "afxwin.h"



// CDocInfoView form view

class CDocInfoView : public CFormView
{
	DECLARE_DYNCREATE(CDocInfoView)

protected:
	CDocInfoView();           // protected constructor used by dynamic creation
	virtual ~CDocInfoView();

public:
	enum { IDD = IDD_DOC_INFO };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
    bool m_bIsEditing;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    void loadDataFromDB();
    afx_msg void OnBnClickedDocEditsave();
    CEdit m_ctrlEditDocName;
    CEdit m_ctrlEditDocAuthor;
    CButton m_ctrlBtnEditSave;
};
