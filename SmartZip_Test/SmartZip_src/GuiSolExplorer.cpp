#include "StdAfx.h"
#include "guisolexplorer.h"
#include "resource.h"
#include "zipexview.h"


CGuiSolExplorer::CGuiSolExplorer(void)
{
}

CGuiSolExplorer::~CGuiSolExplorer(void)
{
}
BEGIN_MESSAGE_MAP(CGuiSolExplorer, CGuiControlBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_PROPERTIES,OnChange)
	ON_COMMAND(ID_COMMENTS,OnSaveComments)
END_MESSAGE_MAP()

int CGuiSolExplorer::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CGuiControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (!m_TabSolExplorer.Create(WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,0x9999))
		return -1;
////////////////////////////////////////////////
	if (!m_ctComment.Create(WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),&m_TabSolExplorer,124))
		return -1;
	m_ctComment.AddComponen(&m_miComment);
	m_miComment.AlingButtons(CGuiMiniTool::ALIGN_LEFT);
	m_miComment.SetImageList(IDB_FOLDER, 16,22, RGB (255, 255, 255));
	m_miComment.AddButton(3,ID_COMMENTS,NULL,_T("Save   "),"Save Comments");
	m_miComment.AutoSize(FALSE);
	m_miComment.SetColor(GuiDrawLayer::GetRGBColorXP());
m_CommentView=new CEditView();
	if (!m_CommentView->Create(NULL,NULL,WS_CHILD | WS_VISIBLE,	CRect(0,0,0,0), &m_ctComment, 2323))
		return -1;
	m_ctComment.AddComponen (m_CommentView);
	
////////////////////////////////////////////	
	//	m_imgList.Create (IDB_DBPROJECTS, 16, 20, RGB (255, 0, 0));
//	m_CommentView.SetWindowText (((CSmartZipDoc*)m_pCurrentDoc)->m_zip.GetGlobalComment());
	CreatContExplorer();
m_zipview=new CZipEXView();
//	::AFXGET
CCreateContext con;con.m_pCurrentDoc =m_pCurrentDoc;
	if(!m_zipview->Create (NULL,NULL,WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),&m_ctSolution,WM_USER+250,&con))
	{
		return -1;
	}
	m_ctSolution.AddComponen (m_zipview);
	CreatContClassView();
//	m_TreeClassView=(CTreeCtrl* )m_ctClassView.AddComponen(RUNTIME_CLASS(CTreeCtrl),3, TVS_HASLINES | 
//								TVS_LINESATROOT | TVS_HASBUTTONS );

//	m_TreeClassView->SetImageList(&m_imgList,TVSIL_NORMAL);
	m_TabSolExplorer.SetImageList(IDB_FOLDER, 16,1, RGB (255, 255, 255));
	m_TabSolExplorer.Addtab(&m_ctSolution,"Explorer",0);
//	m_TabSolExplorer.Addtab(&m_ctClassView,"Properties",1);
	m_TabSolExplorer.Addtab(&m_ctComment,"Comments",2);
	return 0;
}


int CGuiSolExplorer::CreatContClassView()
{
	if (!m_ctClassView.Create(WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),&m_TabSolExplorer,124))
		return -1;
	m_ctClassView.AddComponen(&m_miClassView);
	m_miClassView.AlingButtons(CGuiMiniTool::ALIGN_LEFT);
	m_miClassView.SetImageList(IDB_FOLDER, 16,24, RGB (255, 255, 255));
	m_miClassView.AutoSize(FALSE);//
	m_miClassView.SetColor(GuiDrawLayer::GetRGBColorXP());
	return 1;
}

int CGuiSolExplorer::CreatContExplorer()
{
	if (!m_ctSolution.Create(WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),&m_TabSolExplorer,124))
		return -1;
	m_ctSolution.AddComponen(&m_miSolution);
	m_miSolution.AlingButtons(CGuiMiniTool::ALIGN_LEFT);
	m_miSolution.SetImageList(IDB_DBPROJECTS, 16,22, RGB (255, 0, 0));
	m_miSolution.AddButton(21,ID_PROPERTIES,NULL,_T("Properties"),"Folder Properties");
	m_miSolution.AutoSize(FALSE);
	m_miSolution.SetColor(GuiDrawLayer::GetRGBColorXP());
	return 0;
}
BOOL CGuiSolExplorer::DestroyWindow() 
{
	return CGuiControlBar::DestroyWindow();
}
void CGuiSolExplorer::OnChange()
{
	m_zipview->OnTcProperties();
}
void CGuiSolExplorer::OnSaveComments()
{
	if(((CSmartZipDoc*)m_pCurrentDoc)->tree ==NULL)
	{
		AfxMessageBox ("Please Open a file first!!");
		m_CommentView->GetEditCtrl().SetWindowText ("");
		return;
	}
	CString s;
	m_CommentView->GetEditCtrl().GetWindowText (s);
	((CSmartZipDoc*)m_pCurrentDoc)->m_zip.SetGlobalComment (s);
	((CSmartZipDoc*)m_pCurrentDoc)->CloseCurrentFile();
	((CSmartZipDoc*)m_pCurrentDoc)->OpenCurrentFile(((CSmartZipDoc*)m_pCurrentDoc)->m_strfilename);
	
}