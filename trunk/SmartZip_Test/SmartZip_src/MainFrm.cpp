// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SmartZip.h"

#include "MainFrm.h"
#include "SmartZipView.h"
#include "BarOperation.h"
#include "progress.h"
#include "CShellFileOp.h"
#include "ArchiveRenamer.h"
#include "BrowseForFolder.h"
#include "SplitterDlg.h"
#include "SplashWnd.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CGuiFrameWnd)//CFrameWnd

BEGIN_MESSAGE_MAP(CMainFrame, CGuiFrameWnd)//CFrameWnd
//{{AFX_MSG_MAP(CMainFrame)
ON_WM_CREATE()
ON_WM_SIZE()
ON_COMMAND(ID_TOOLS_TEST, OnTest)
ON_COMMAND(ID_FILE_COPYARCHIVE, OnCopyArchive)
ON_COMMAND(ID_FILE_MOVEARCHIVE, OnMoveArchive)
ON_UPDATE_COMMAND_UI(ID_FILE_DELETEARCHIVE, OnUpdateArchiveOperation)
ON_COMMAND(ID_FILE_DELETEARCHIVE, OnDeleteArchive)
ON_COMMAND(ID_FILE_RENAMEARCHIVE, OnRenameArchive)
ON_COMMAND(ID_VIEW_MYBAR, OnViewMybar)
ON_UPDATE_COMMAND_UI(ID_VIEW_MYBAR, OnUpdateViewMybar)
ON_COMMAND(ID_VIEW_SELECTION_SELECTALL, OnSelectall)
ON_COMMAND(ID_VIEW_SELECTION_SELECTINVERSE, OnSelectinverse)
ON_COMMAND(ID_VIEW_SELECTION_UNSELECTALL, OnUnselectall)
ON_COMMAND(ID_FILE_CLOSEARCHIVE, OnFileClose)
ON_UPDATE_COMMAND_UI(ID_FILE_CLOSEARCHIVE, OnUpdateTcAdd)
ON_COMMAND(ID_ACTION_EXPORT_HTML, OnActionExportHtml)
ON_COMMAND(ID_TOOLS_BACKUPWIZARD, OnToolsBackupwizard)
	ON_COMMAND(ID_TOOLS_TIMETOGET, OnToolsTimetoget)
	ON_COMMAND(ID_TOOLS_CTREATESFX, OnToolsCtreatesfx)
ON_UPDATE_COMMAND_UI(ID_TC_ADD, OnUpdateTcAdd)
ON_UPDATE_COMMAND_UI(ID_TC_EXTRACT, OnUpdateTcAdd)
ON_UPDATE_COMMAND_UI(ID_TC_VIEW, OnUpdateTcAdd)
ON_UPDATE_COMMAND_UI(ID_TC_DELETE, OnUpdateTcAdd)
ON_UPDATE_COMMAND_UI(ID_TC_PROPERTIES, OnUpdateTcAdd)
ON_UPDATE_COMMAND_UI(ID_FILE_MOVEARCHIVE, OnUpdateArchiveOperation)
ON_UPDATE_COMMAND_UI(ID_FILE_COPYARCHIVE, OnUpdateArchiveOperation)
ON_UPDATE_COMMAND_UI(ID_FILE_RENAMEARCHIVE, OnUpdateArchiveOperation)
ON_UPDATE_COMMAND_UI(ID_TOOLS_TEST, OnUpdateTcAdd)
ON_UPDATE_COMMAND_UI(ID_ACTION_EXPORT_HTML, OnUpdateTcAdd)
ON_UPDATE_COMMAND_UI(ID_TOOLS_CTREATESFX, OnUpdateTcAdd)
ON_COMMAND(ID_PROPERTIES,OnChange)
ON_COMMAND(ID_COMMENTS,OnSaveComments)
	ON_COMMAND(ID_HELP_HELP, OnHelpHelp)
	//}}AFX_MSG_MAP
ON_UPDATE_COMMAND_UI_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnUpdateViewStyles)
ON_COMMAND_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnViewStyle)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
		ID_INDICATOR_CAPS,
		ID_INDICATOR_NUM,
		ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
	m_large.Detach ();
	m_small.Detach ();
	
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	if (CGuiFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;	
	CSplashWnd::ShowSplashScreen(3000, IDB_SPLASH, this);
	InitMenu(IDR_MAINFRAME);
	InitToolBar(IDR_MAINFRAME);
	InitStatusBar(indicators,sizeof(indicators)/sizeof(UINT));
	
	if (!m_Server.Create(_T("Explorer"),WS_CHILD | WS_VISIBLE, this, 0x996))//
    {
        TRACE0("Failed to create m_GuiSolEx\n");
        return -1;      
	}
	m_Server.SetBarStyle(m_Server.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	
	EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_Server.EnableDocking(CBRS_ALIGN_ANY);
	
	DockControlBar(&m_wndMenuBar);
	DockControlBar(&m_wndToolBar);
	DockControlBar(&m_Server, AFX_IDW_DOCKBAR_LEFT);
	sProfile = _T("GuiCool");
	LoadBars();
	
	// THE IMAGE LIST
	int iSize = GetWindowsDirectory(NULL, 0);
	//CString sz;
	CZipString sz;//modified by hwy
	if (iSize && GetWindowsDirectory(sz.GetBuffer(iSize), iSize))
	{
		sz.ReleaseBuffer();
		CZipPathComponent zpc(sz);
		sz = zpc.GetFileDrive();
		CZipPathComponent::AppendSeparator(sz);
		SHFILEINFO    shfi;
		m_small.Attach((HIMAGELIST)SHGetFileInfo( 
			sz, // we know it exists
			0, 
			&shfi, 
			sizeof(SHFILEINFO), 
			SHGFI_SYSICONINDEX | SHGFI_SMALLICON));
		m_large.Attach((HIMAGELIST)SHGetFileInfo( 
			sz, // we know it exists
			0, 
			&shfi, 
			sizeof(SHFILEINFO), 
			SHGFI_SYSICONINDEX | SHGFI_ICON));	
	}
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CGuiFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style &=~FWS_ADDTOTITLE  ;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CGuiFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CGuiFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CGuiFrameWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	m_Server.m_pCurrentDoc=pContext->m_pCurrentDoc;
	
	//	m_Server.m_zipview=new CZipEXView();
	//	::AFXGET
	//CCreateContext con;con.m_pCurrentDoc =pContext->m_pCurrentDoc;
	//	if(!m_Server.m_zipview->Create (NULL,"AHMED",WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),&m_Server.m_ctSolution,WM_USER+250,&con))
	//	{
	//		return FALSE;
	//	}
	//	m_Server.m_ctSolution.AddComponen (m_Server.m_zipview);
	// create splitter window
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;
	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CBarOperation), CSize(90, 200), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CSmartZipView), CSize(200, 200), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}
	return TRUE;
}

BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	return CGuiFrameWnd::OnCommand(wParam, lParam);
}

void CMainFrame::OnTest() 
{
	CDProgress d;d.m_text="Testing ...";
	d.m_pDoc=(CSmartZipDoc*)this->GetActiveDocument();
	d.m_operation="t";
	d.DoModal ();
	
}
/*
CRegDWORD  myword("Software\\Company\\Subkey\\mydword", 0, TRUE, HKEY_LOCAL_MACHINE);
*/
void CMainFrame::OnCopyArchive() 
{
	CSmartZipDoc* pDoc=(CSmartZipDoc*)this->GetActiveDocument ();	
	if(pDoc->tree !=0)
	{
		CFileDialog cFileDlg
			(	FALSE, 
			(LPCTSTR)"zip", 
			(LPCTSTR)"archive1", 
			OFN_ENABLESIZING|OFN_HIDEREADONLY|OFN_EXPLORER|OFN_LONGNAMES|OFN_OVERWRITEPROMPT|OFN_CREATEPROMPT, 
			_T("All Files (*.zip)|*.zip|"));
		cFileDlg.m_ofn.lpstrTitle = _T("Create New Archive...");
		if (cFileDlg.DoModal() != IDOK)
			return;
		CShellFileOp op;
		op.SetOperationFlags ( FO_COPY, CWnd::FromHandle (::GetDesktopWindow ()),NULL );
		op.AddSourceFile (pDoc->m_strfilename );
		op.AddDestFile (cFileDlg.m_ofn.lpstrFile );
		BOOL bSuccess, bAPICalled = FALSE, bAborted = FALSE;
		int  nAPIReturn = 0;
		bSuccess = op.Go ( &bAPICalled, &nAPIReturn, &bAborted );
	}
	else
	{
		AfxMessageBox ("Please Create new Zip file First");
	}				
}

void CMainFrame::OnMoveArchive() 
{
	if(((CSmartZipDoc*)this->GetActiveDocument ())->tree !=0)
	{
		CFileDialog cFileDlg
			(	FALSE, 
			(LPCTSTR)"zip", 
			(LPCTSTR)"archive1", 
			OFN_ENABLESIZING|OFN_HIDEREADONLY|OFN_EXPLORER|OFN_LONGNAMES|OFN_OVERWRITEPROMPT|OFN_CREATEPROMPT, 
			_T("All Files (*.zip)|*.zip|"));
		cFileDlg.m_ofn.lpstrTitle = _T("Create New Archive...");
		if (cFileDlg.DoModal() != IDOK)
			return;
		
		CSmartZipDoc* pDoc=(CSmartZipDoc*)this->GetActiveDocument ();	
		pDoc->CloseCurrentFile();
		CShellFileOp op;
		op.SetOperationFlags ( FO_MOVE, CWnd::FromHandle (::GetDesktopWindow ()),FOF_NOCONFIRMATION );
		op.AddSourceFile (pDoc->m_strfilename );
		op.AddDestFile (cFileDlg.m_ofn.lpstrFile );
		BOOL bSuccess, bAPICalled = FALSE, bAborted = FALSE;
		int  nAPIReturn = 0;
		bSuccess = op.Go ( &bAPICalled, &nAPIReturn, &bAborted );
		if(bSuccess)	pDoc->OpenCurrentFile(cFileDlg.m_ofn.lpstrFile);
	}
	else
	{
		AfxMessageBox ("Please Create new Zip file First");
	}			
}


void CMainFrame::OnDeleteArchive() 
{
	if(((CSmartZipDoc*)this->GetActiveDocument ())->tree !=0)
	{
		int b=AfxMessageBox ("Do you really want to Delete this archive ?",MB_OKCANCEL );
		if(b==IDOK)
		{
			
			CSmartZipDoc* pDoc=(CSmartZipDoc*)this->GetActiveDocument ();	
			pDoc->CloseCurrentFile();
			CShellFileOp op;
			op.SetOperationFlags ( FO_DELETE, CWnd::FromHandle (::GetDesktopWindow ()),FOF_NOCONFIRMATION );
			op.AddSourceFile (pDoc->m_strfilename  );
			BOOL bSuccess, bAPICalled = FALSE, bAborted = FALSE;
			int  nAPIReturn = 0;
			bSuccess = op.Go ( &bAPICalled, &nAPIReturn, &bAborted );
			if(bSuccess)	pDoc->UpdateAllViews (NULL,0,0);
		}
	}
	else
	{
		AfxMessageBox ("Please Create new Zip file First");
	}			
}

void CMainFrame::OnRenameArchive() 
{
	if(((CSmartZipDoc*)this->GetActiveDocument ())->tree !=0)
	{
		// TODO: Add your command handler code here
		CSmartZipDoc* pDoc=(CSmartZipDoc*)this->GetActiveDocument ();	
		CString path=pDoc->m_strfilename ;
		CString newfilename;
		//	CString strfilename=pDoc->m_strfilename;
		int to;
		CString file,dir,temp(pDoc->m_strfilename);
		to=temp.ReverseFind ('\\')+1;
		file=temp.Mid (to,temp.GetLength ()-to);
		dir=temp.Mid (0,to);
		//		char* m1=new char[256];::lstrcpy (m1,pDoc->m_strfilename );
		//		CString s=PathFindFileName((LPCTSTR)m1);//GetPathName() );
		//	delete m1;
		CArchiveRenamer dlg;
		dlg.m_from =file;
		dlg.m_to =file;
		if(dlg.DoModal ()==IDOK)
		{
			if(!(dlg.m_to==dlg.m_from ))
			{
				newfilename=dir+dlg.m_to;
				pDoc->CloseCurrentFile();
				CShellFileOp op;
				op.SetOperationFlags ( FO_RENAME , CWnd::FromHandle (::GetDesktopWindow ()),NULL  );
				op.AddSourceFile (path );
				op.AddDestFile (newfilename);
				BOOL bSuccess, bAPICalled = FALSE, bAborted = FALSE;
				int  nAPIReturn = 0;
				bSuccess = op.Go ( &bAPICalled, &nAPIReturn, &bAborted );
				if(bSuccess)
					pDoc->OpenCurrentFile(newfilename);
				
			}
		}
	}
	else
	{
		AfxMessageBox ("Please Create new Zip file First");
	}			
}

void CMainFrame::OnUpdateArchiveOperation(CCmdUI* pCmdUI) 
{
if(this->GetActiveDocument ()==NULL) 
	{
		pCmdUI->Enable (FALSE);
		return;
	}	if(((CSmartZipDoc*)this->GetActiveDocument ())->tree !=0)
	{
		pCmdUI->Enable ();
	}
	else
	{
		pCmdUI->Enable (FALSE);
	}
}


CSmartZipView* CMainFrame::GetRightPane()
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 1);
	CSmartZipView* pView = DYNAMIC_DOWNCAST(CSmartZipView, pWnd);
	return pView;
}

void CMainFrame::OnUpdateViewStyles(CCmdUI* pCmdUI)
{
	// TODO: customize or extend this code to handle choices on the
	// View menu.
	
	CSmartZipView* pView = GetRightPane(); 
	
	// if the right-hand pane hasn't been created or isn't a view,
	// disable commands in our range
	
	if (pView == NULL)
		pCmdUI->Enable(FALSE);
	else
	{
		DWORD dwStyle = pView->GetStyle() & LVS_TYPEMASK;
		
		// if the command is ID_VIEW_LINEUP, only enable command
		// when we're in LVS_ICON or LVS_SMALLICON mode
		
		if (pCmdUI->m_nID == ID_VIEW_LINEUP)
		{
			if (dwStyle == LVS_ICON || dwStyle == LVS_SMALLICON)
				pCmdUI->Enable();
			else
				pCmdUI->Enable(FALSE);
		}
		else
		{
			// otherwise, use dots to reflect the style of the view
			pCmdUI->Enable();
			BOOL bChecked = FALSE;
			
			switch (pCmdUI->m_nID)
			{
			case ID_VIEW_DETAILS:
				bChecked = (dwStyle == LVS_REPORT);
				break;
				
			case ID_VIEW_SMALLICON:
				bChecked = (dwStyle == LVS_SMALLICON);
				break;
				
			case ID_VIEW_LARGEICON:
				bChecked = (dwStyle == LVS_ICON);
				break;
				
			case ID_VIEW_LIST:
				bChecked = (dwStyle == LVS_LIST);
				break;
				
			default:
				bChecked = FALSE;
				break;
			}
			
			pCmdUI->SetRadio(bChecked ? 1 : 0);
		}
	}
}


void CMainFrame::OnViewStyle(UINT nCommandID)
{
	// TODO: customize or extend this code to handle choices on the
	// View menu.
	CSmartZipView* pView = GetRightPane();
	
	// if the right-hand pane has been created and is a CTrexView,
	// process the menu commands...
	if (pView != NULL)
	{
		DWORD dwStyle = -1;
		DWORD dwExStyle=-1;
		switch (nCommandID)
		{
		case ID_VIEW_LINEUP:
			{
				// ask the list control to snap to grid
				CListCtrl& refListCtrl = pView->GetListCtrl();
				refListCtrl.Arrange(LVA_SNAPTOGRID);
			}
			break;
			
			// other commands change the style on the list control
		case ID_VIEW_DETAILS:
			dwStyle = LVS_REPORT;
			dwExStyle =LVS_EX_UNDERLINEHOT|LVS_EX_HEADERDRAGDROP ;
			
			break;
			
		case ID_VIEW_SMALLICON:
			dwStyle = LVS_SMALLICON;
			break;
			
		case ID_VIEW_LARGEICON:
			dwStyle = LVS_ICON;
			break;
			
		case ID_VIEW_LIST:
			dwStyle = LVS_LIST;
			break;
		}
		
		// change the style; window will repaint automatically
		if (dwStyle != -1)
			pView->ModifyStyle(LVS_TYPEMASK, dwStyle);
		if (dwExStyle!=-1)
			pView->ModifyStyleEx (LVS_TYPESTYLEMASK,dwExStyle);
	}
}

void CMainFrame::OnViewMybar() 
{
	ShowHideBar(&m_Server);	
}

void CMainFrame::OnUpdateViewMybar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_Server.IsWindowVisible ());
}

void CMainFrame::OnSelectall() 
{
	// TODO: Add your command handler code here
	CSmartZipView* pView = GetRightPane(); 
	pView->SelectAll(TRUE);
}

void CMainFrame::OnSelectinverse() 
{
	// TODO: Add your command handler code here
	CSmartZipView* pView = GetRightPane(); 
	pView->Selectinverse();
	
}

void CMainFrame::OnUnselectall() 
{
	// TODO: Add your command handler code here
	CSmartZipView* pView = GetRightPane(); 
	pView->SelectAll(FALSE);
	
}

void CMainFrame::OnFileClose() 
{
	// TODO: Add your command handler code here
	((CSmartZipDoc*)this->GetActiveDocument ())->CloseCurrentFile();
	((CSmartZipDoc*)this->GetActiveDocument ())->UpdateAllViews (NULL,0,0);
}
void CMainFrame::OnUpdateTcAdd(CCmdUI* pCmdUI) 
{
	if(this->GetActiveDocument ()==NULL) 
	{
		pCmdUI->Enable (FALSE);
		return;
	}
	if(((CSmartZipDoc*)this->GetActiveDocument ())->tree !=0)
	{
		pCmdUI->Enable ();
	}
	else
	{
		pCmdUI->Enable (FALSE);
	}
	
}

void CMainFrame::OnActionExportHtml() 
{
	// TODO: Add your command handler code here
	CFileDialog cFileDlg
		(	FALSE, 
		(LPCTSTR)"htm", 
		(LPCTSTR)"fileslist", 
		OFN_ENABLESIZING|OFN_HIDEREADONLY|OFN_EXPLORER|OFN_LONGNAMES|OFN_OVERWRITEPROMPT|OFN_CREATEPROMPT, 
		_T("All Files (*.htm)|*.htm|"));
	cFileDlg.m_ofn.lpstrTitle = _T("Save Files List...");
	if (cFileDlg.DoModal() != IDOK)
		return;
	CFile file1(	cFileDlg.GetPathName (),CFile::modeCreate | CFile::modeWrite  );
	
	CSmartZipDoc* pDoc=(CSmartZipDoc*)this->GetActiveDocument ();
	CString path=pDoc->m_strfilename;
	int to;
	CString file,dir,temp(pDoc->m_strfilename);
	to=temp.ReverseFind ('\\')+1;
	file=temp.Mid (to,temp.GetLength ()-to);
	//	dir=temp.Mid (0,to);
	CTime time=CTime::GetCurrentTime();
	CString htmlfile;
	htmlfile+="<html>"
		"<head>"
		"<meta name=\"GENERATOR\" content=\"ZipGenius 1.4 by M.Dev Software\">"
		"<title>"
		"SmartZip 1.0 - "+file;
	htmlfile+="</title>"
		"</head>"
		"<body><font face=\"Tahoma, Verdana, Arial\" size=2>"
		"List of the files contained in the ";
	
	
	
	//pDoc->m_zip .ge
	htmlfile+=file +" archive. Made: "+time.Format(_T("%d %b %Y, %X"));	
	htmlfile+="<br><font face=\"Tahoma, Verdana, Arial\" size=2>File: <b>"+file+"</b> - <b> ";
	CZipWordArray wordarr;
	pDoc->AddSubFolders (&wordarr,pDoc->tree);
	int nn=wordarr.GetSize ();
	CString temp1;temp1.Format ("%d",nn);
	htmlfile+=" "+temp1+" files ";
	htmlfile+=" </b>.</font><hr><br>"
		"<table border=\"1\" cellspacing=\"0\" align=\"CENTER\" width=\"100%\">"
		"<font face=\"Tahoma, Verdana, Arial\" size=2>"
		"<tr>"
		"<td nowrap BGCOLOR=\"#D8D8D8\"><font face=\"Tahoma, Verdana, Arial\" size=2><B>File</B></td>"
		"<td nowrap BGCOLOR=\"#D8D8D8\" align=\"CENTER\"><font face=\"Tahoma, Verdana, Arial\" size=2><B>Date - time</B></td>"
		"<td nowrap BGCOLOR=\"#D8D8D8\" align=\"RIGHT\"><font face=\"Tahoma, Verdana, Arial\" size=2><B>Compr. size</B></td>"
		"<td nowrap BGCOLOR=\"#D8D8D8\"><font face=\"Tahoma, Verdana, Arial\" size=2><B>Orig. size</B></td>"
		"<td nowrap BGCOLOR=\"#D8D8D8\" align=\"CENTER\"><font face=\"Tahoma, Verdana, Arial\" size=2><B>Ratio</B></td>"
		"<td nowrap BGCOLOR=\"#D8D8D8\"><font face=\"Tahoma, Verdana, Arial\" size=2><B>Path</B></td>"
		"</tr>";
	int m=pDoc->m_zip.GetCount();
	for(int i=0;i<m;i++)
	{
		CZipFileHeader fh;
		pDoc->m_zip.GetFileInfo(fh,i);
		if(!fh.IsDirectory ())
		{
			CString c((LPCTSTR)fh.GetFileName ());
			temp=c;
			to=temp.ReverseFind ('\\')+1;
			file=temp.Mid (to,temp.GetLength ()-to);
			dir=temp.Mid (0,to);
			htmlfile+="<tr><td nowrap><font face=\"Tahoma, Verdana, Arial\" size=2>";
			htmlfile+=file+"</td><td nowrap align=\"CENTER\"><font face=\"Tahoma, Verdana, Arial\" size=2>";
			CTime t(fh.GetTime());
			temp1=t.Format(_T("%d %b %Y, %X"));
			htmlfile+=temp1;
			htmlfile+="</td><td nowrap align=\"CENTER\"><font face=\"Tahoma, Verdana, Arial\" size=2>";
			char buf[20];
			StrFormatByteSize (fh.m_uComprSize,buf,20);
			htmlfile+=buf;
			htmlfile+="</td><td nowrap align=\"RIGHT\"><font face=\"Tahoma, Verdana, Arial\" size=2>";
			StrFormatByteSize (fh.m_uUncomprSize,buf,20);
			htmlfile+=buf;
			htmlfile+="</td><td nowrap><font face=\"Tahoma, Verdana, Arial\" size=2>";
			temp1.Format(_T("%.0f%%"), fh.GetCompressionRatio());
			htmlfile+=temp1;
			htmlfile+="</td><td nowrap align=\"CENTER\"><font face=\"Tahoma, Verdana, Arial\" size=2>";
			htmlfile+=dir;
			htmlfile+=" </td><td nowrap><font face=\"Tahoma, Verdana, Arial\" size=2>  </td></tr>";
			file1.Write (htmlfile.GetBuffer (htmlfile.GetLength ()),htmlfile.GetLength ());
			htmlfile.ReleaseBuffer ();
			htmlfile="";
		}
		//	ParseStringToobj(c,fh.IsDirectory (),i);
	}
	htmlfile+="</table>"
		"</font><br>"
		"<hr>"
		"<center>"
		"<font face=\"Arial Black\" size=5><i>SmartZip</font><font face=\"Arial Black\" size=3> 1.0</i></font><br>"
		"<font face=\"Tahoma\" size=2><b>Copyright ?001, 2002 <a href=\"mailto:ahmed_ismaiel@hotmail.com\">A.I.Z Software</a></b></font>"
		"</body></html>";
	file1.Write (htmlfile.GetBuffer (htmlfile.GetLength ()),htmlfile.GetLength ());
	htmlfile.ReleaseBuffer ();
	
}

void CMainFrame::OnToolsBackupwizard() 
{
	// TODO: Add your command handler code here
	CBrowseForFolder bf;
	CString sz;
	bf.strTitle="Choose a folder to backup";
	bf.strStartupDir = sz;
	if (bf.GetFolder(sz))
	{
		//GetDlgItem(IDC_DESTINATION)->SetWindowText(sz);	
		CFileDialog cFileDlg
			(	FALSE, 
			(LPCTSTR)"zip", 
			(LPCTSTR)"backup1", 
			OFN_ENABLESIZING|OFN_HIDEREADONLY|OFN_EXPLORER|OFN_LONGNAMES|OFN_OVERWRITEPROMPT|OFN_CREATEPROMPT, 
			_T("All Files (*.zip)|*.zip|"));
		cFileDlg.m_ofn.lpstrTitle = _T("Where do you want to save output...");
		if (cFileDlg.DoModal() != IDOK)
			return;
		CSmartZipDoc* pDoc=(CSmartZipDoc*)this->GetActiveDocument ();
		CZipArchive m_zip;
		//	OpenZipFile(lpszPathName, );
		pDoc->OpenZipFile (m_zip,cFileDlg.m_ofn.lpstrFile, CZipArchive::zipCreate,0);
		
		CStringArray* pArray = new CStringArray;
		pArray->Add(sz); // for SetRootPath
		pArray->Add(sz);
		
		CDProgress d;
		d.m_text="Backup Archive ...";
		d.m_pDoc=(CSmartZipDoc*)this->GetActiveDocument ();
		d.zip =&m_zip;
		d.m_operation="a";
		d.pFull =TRUE;
		d.bOverwrite =9;
		d.pReserved=pArray;
		d.DoModal ();
		pDoc->Close (m_zip,FALSE);
	}
}

void CMainFrame::OnToolsTimetoget() 
{
CSplitterDlg dlg;
dlg.DoModal ();	
}

void CMainFrame::OnToolsCtreatesfx() 
{
	// TODO: Add your command handler code here
	CSmartZipDoc* pDoc=(CSmartZipDoc*)this->GetActiveDocument ();
//	pDoc->SaveModified ();
pDoc->CloseCurrentFile();

	CFileDialog cFileDlg
			(	FALSE, 
			(LPCTSTR)"exe", 
			(LPCTSTR)"sfx", 
			OFN_ENABLESIZING|OFN_HIDEREADONLY|OFN_EXPLORER|OFN_LONGNAMES|OFN_OVERWRITEPROMPT|OFN_CREATEPROMPT, 
			_T("All Files (*.exe)|*.exe|"));
		cFileDlg.m_ofn.lpstrTitle = _T("Where do you want to save output...");
		if (cFileDlg.DoModal() != IDOK)
			return;
CFile file(pDoc->m_strfilename ,CFile::modeRead   |CFile::shareDenyNone   );
CFile sfx(cFileDlg.m_ofn.lpstrFile,CFile::modeWrite   |CFile::modeCreate   );
HINSTANCE insApp = AfxGetInstanceHandle();

	//Return the handle for the resource of type EXE and load it.
	HRSRC hResInfo = FindResource(insApp,(LPCSTR)IDR_SFX1,"SFX");
	HGLOBAL hRes = LoadResource(insApp,hResInfo );

	//Now Get the size of the EXE resource type witch will be
	//the size of exe file.
	DWORD dFileLength = SizeofResource( insApp, hResInfo ); 
	sfx.Write((LPSTR)hRes,dFileLength);
	char* b=new char[(int)file.GetLength ()];
	file.Read (b,(int)file.GetLength ());
	sfx.Write (b,(int)file.GetLength ());//modified by hwy, WriteHuge to Write, ReadHuge to Read
	delete b;
	pDoc->OpenCurrentFile (pDoc->m_strfilename );
}
void CMainFrame::OnChange()
{
	m_Server.OnChange();
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
	{
		return TRUE;
	}
	return CGuiFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::OnHelpHelp() 
{
	CString file;
	::GetModuleFileName(::GetModuleHandle ("Copier"),file.GetBuffer (256),255);
	file.ReleaseBuffer (256);//size-1
	LPSTR folder=file.GetBuffer (file.GetLength ());
	PathRemoveFileSpec(folder);
	CString f=folder;
	ShellExecute(NULL,"open",f+"\\SmartZip Help.chm",NULL,NULL,SW_MAXIMIZE );
}
void CMainFrame::OnSaveComments()
{
m_Server.OnSaveComments();
}