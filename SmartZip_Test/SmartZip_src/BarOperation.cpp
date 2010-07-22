// BarOperation.cpp : implementation file
//

#include "stdafx.h"
#include "SmartZip.h"
#include "BarOperation.h"
#include "MainFrm.h"
#include "MyFavourites.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define IDO_FAVOURITES   5600
#define IDO_FAVOURITESADD 5601

/////////////////////////////////////////////////////////////////////////////
// CBarOperation
IMPLEMENT_DYNCREATE(CBarOperation,CGuiOutLookView)

CBarOperation::CBarOperation()
{
}

CBarOperation::~CBarOperation()
{
}


BEGIN_MESSAGE_MAP(CBarOperation, CGuiOutLookView  )
	//{{AFX_MSG_MAP(CBarOperation)
	ON_WM_CREATE()
	ON_COMMAND(IDO_FAVOURITES, OnFavourites)
	ON_COMMAND(ID_TC_DELETE, OnTcDelete)
	ON_COMMAND(ID_TC_EXTRACT, OnTcExtract)
	ON_COMMAND(ID_TC_PROPERTIES, OnTcProperties)
	ON_COMMAND(ID_TC_VIEW, OnTcView)
	ON_COMMAND(ID_TC_ADD, OnTcAdd)
	ON_COMMAND(ID_FILE_MOVEARCHIVE, OnMoveArchive)
	ON_COMMAND(ID_FILE_COPYARCHIVE, OnCopyArchive)
	ON_COMMAND(ID_FILE_RENAMEARCHIVE, OnRenameArchive)
	ON_COMMAND(ID_FILE_DELETEARCHIVE, OnDeleteArchive)
	ON_COMMAND(IDO_FAVOURITESADD, OnAddToFavourites)
	
	ON_COMMAND(ID_TOOLS_TEST, OnTestArchive)
	ON_COMMAND(ID_TOOLS_CTREATESFX, OnCreateSFX)
	//}}AFX_MSG_MAP 
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBarOperation message handlers

int CBarOperation::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
/*	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
*/	
	// TODO: Add your specialized creation code here
	if (CGuiOutLookView::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (!File.Create(WS_VISIBLE,
		CRect(0,0,0,0), &cf, 1))
		return -1;
	if (!Folder.Create(WS_VISIBLE,
		CRect(0,0,0,0), &cf, 1))
		return -1;
	if (!Archive.Create(WS_VISIBLE,
		CRect(0,0,0,0), &cf, 1))
		return -1;

	Folder.SetImageList(IDB_BITMAP2, 32, 0, RGB(206,0,206));
	Folder.AddItem(IDO_FAVOURITES,"Favorites",0);
	Folder.AddItem(IDO_FAVOURITES,"Favorites",0);
	Folder.AddItem(IDO_FAVOURITESADD,"Add to Favorites",1);
	Folder.AddItem(ID_TC_ADD,"Add file/folder",2);
	
	File.SetImageList(IDB_BITMAP2, 32, 0, RGB(206,0,206));
	File.AddItem(ID_TC_VIEW,"Create",3);//no need
	File.AddItem(ID_TC_VIEW,"Quick View",3);
	File.AddItem(ID_TC_EXTRACT,"Extract to",4);
	File.AddItem(ID_TC_DELETE,"Delete",5);
	File.AddItem(ID_TC_PROPERTIES,"Properties",6);
	File.AddItem(ID_TOOLS_TEST,"Test",10);
	File.AddItem(ID_TOOLS_CTREATESFX,"Create SFX",11);
	
	Archive.SetImageList(IDB_BITMAP2, 32, 0, RGB(206,0,206));
	Archive.AddItem(ID_FILE_MOVEARCHIVE,"Move Archive",7);
	Archive.AddItem(ID_FILE_MOVEARCHIVE,"Move Archive",7);
	Archive.AddItem(ID_FILE_COPYARCHIVE,"Copy Archive",8);
	Archive.AddItem(ID_FILE_RENAMEARCHIVE,"Rename Archive",9);
	Archive.AddItem(ID_FILE_DELETEARCHIVE,"Delete Archive",5);

	cf.AddFolder(&Folder,"Folders");
	cf.AddFolder(&File,"Files");
	cf.AddFolder(&Archive,"Archive");
	return 0;
}
void CBarOperation::OnFavourites()
{
	CMyFavourites dlg;
	dlg.DoModal ();
}

void CBarOperation::OnTcDelete() 
{
	this->GetDocument ()->UpdateAllViews (this,3,0);
}

void CBarOperation::OnTcExtract() 
{
	this->GetDocument ()->UpdateAllViews (this,5,0);
	
}

void CBarOperation::OnTcProperties() 
{
	this->GetDocument ()->UpdateAllViews (this,4,0);
	
}

void CBarOperation::OnTcView() 
{
	this->GetDocument ()->UpdateAllViews (this,6,0);
	
}

void CBarOperation::OnTcAdd() 
{
	this->GetDocument ()->UpdateAllViews (this,7,0);
	
}
void CBarOperation::OnMoveArchive()
{
	((CMainFrame*)::AfxGetMainWnd ())->OnMoveArchive();
}
void CBarOperation::OnCopyArchive()
{
	((CMainFrame*)::AfxGetMainWnd ())->OnCopyArchive();
}
void CBarOperation::OnRenameArchive()
{
	((CMainFrame*)::AfxGetMainWnd ())->OnRenameArchive();
}
void CBarOperation::OnDeleteArchive()
{
	((CMainFrame*)::AfxGetMainWnd ())->OnDeleteArchive();
}
void CBarOperation::OnAddToFavourites()
{
	if(((CSmartZipDoc*)GetDocument ())->tree !=0)
	{

	int b=AfxMessageBox ("Do you really want to add this archive to favourites list?",MB_OKCANCEL );
		if(b==IDOK)
		{
			CMyFavourites dlg;
			dlg.AddToFavo (((CSmartZipDoc*)GetDocument ())->m_strfilename);
		}
	}
}
void CBarOperation::OnTestArchive()
{
	((CMainFrame*)::AfxGetMainWnd ())->OnTest ();
}
void CBarOperation::OnCreateSFX()
{
((CMainFrame*)::AfxGetMainWnd ())->OnToolsCtreatesfx ();
}