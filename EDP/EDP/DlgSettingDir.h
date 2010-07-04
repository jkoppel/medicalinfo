#pragma once
#include "afxwin.h"

#include "EditListBox\\ListBoxEx.h"
#include "EditListBox\\SHBrowseDlg.h"

class CMyListBoxEx: public CListBoxEx
{
public:

   CMyListBoxEx()
   {
      SetEditStyle( LBEX_EDITBUTTON );
   };

   virtual void OnBrowseButton( int iItem )
   {
      iItem;
      CSHBrowseDlg dlgBrowse;
      if ( dlgBrowse.DoModal() ) SetEditText( dlgBrowse.GetFullPath() );
   };
};

// CDlgSettingDir dialog

class CDlgSettingDir : public CDialog
{
	DECLARE_DYNAMIC(CDlgSettingDir)

public:
	CDlgSettingDir(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSettingDir();

// Dialog Data
	enum { IDD = IDD_SETTING_DIR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CMyListBoxEx m_ctrlListDir;
	CListBoxExBuddy m_ListBoxExBuddy;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
