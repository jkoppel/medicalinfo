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

// CDlgDirSetup dialog

class CDlgDirSetup : public CDialog
{
	DECLARE_DYNAMIC(CDlgDirSetup)

public:
	CDlgDirSetup(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDirSetup();

// Dialog Data
	enum { IDD = IDD_DIR_SETUP };

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
