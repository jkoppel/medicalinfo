// ZipEXView.cpp : implementation of the CZipEXView class
//

#include "stdafx.h"
//#include "ZipEX.h"
#include "smartzip.h"
//#include "ZipEXDoc.h"
//#include "ZipEXView.h"
#include "ExtractDlg.h"
#include "progress.h"
#include "SmartZipDoc.h"
#include "zipexview.h"
#include "TreeNode.h"
#include "NewWizDialog.h"
#include "FileProperties.h"
#include "GuiSolExplorer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CZipEXView

IMPLEMENT_DYNCREATE(CZipEXView, CTreeView)

BEGIN_MESSAGE_MAP(CZipEXView, CTreeView)
//{{AFX_MSG_MAP(CZipEXView)
ON_WM_CREATE()
ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemexpanding)
ON_NOTIFY_REFLECT(TVN_GETDISPINFO, OnGetdispinfo)
ON_NOTIFY_REFLECT(NM_RCLICK, OnTreeRclick)
ON_COMMAND(ID_TC_EXTRACT, OnTcExtract)
ON_COMMAND(ID_TC_DELETE, OnTcDelete)
ON_COMMAND(ID_TC_ADD, OnTcAdd)
ON_COMMAND(ID_TC_PROPERTIES, OnTcProperties)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CZipEXView construction/destruction

CZipEXView::CZipEXView()
{
	// TODO: add construction code here
	//	test="ahmed\\cool\\toto";
}

CZipEXView::~CZipEXView()
{
	//	delete GetTreeCtrl ().GetImageList (TVSIL_NORMAL);
}

BOOL CZipEXView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style |=TVS_HASBUTTONS |TVS_LINESATROOT ;
	cs.style |=TVS_HASLINES|TVS_TRACKSELECT|TVS_SHOWSELALWAYS;//;
	//cs.style |=TVS_CHECKBOXES ;
	return CTreeView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CZipEXView drawing

void CZipEXView::OnDraw(CDC* pDC)
{
	CSmartZipDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

void CZipEXView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();
}

/////////////////////////////////////////////////////////////////////////////
// CZipEXView diagnostics

#ifdef _DEBUG
void CZipEXView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CZipEXView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CSmartZipDoc* CZipEXView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSmartZipDoc)));
	return (CSmartZipDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CZipEXView message handlers

BOOL CZipEXView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}
int CZipEXView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_imgList.Create(16, 16, ILC_COLORDDB|ILC_MASK, 13, 1);
	bitmap.LoadBitmap(IDB_FOLDER);
	m_imgList.Add(&bitmap, RGB(255,255,255));
	CTreeCtrl &pCtrl=GetTreeCtrl ();
	pCtrl.SetImageList (&m_imgList,TVSIL_NORMAL);
	return 0;
}

void CZipEXView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	CTreeNode *c=GetDocument()->tree ;
	hSel=pNMTreeView->itemNew.hItem;
	CTreeNode* hold=NodeFromItem(pNMTreeView->itemNew.hItem,c);
	this->GetDocument ()->currentnode=hold;
    GetDocument ()->UpdateAllViews (this, 2, NULL);
	*pResult = 0;
}
/*
void CZipEXView::ParseStringToobj(CString &s,BOOL pFolder,int findex)
{
if(pFolder)	if(s.Right (1)!='\\') s+='\\';
CString temp;
int from=0,to=0;
BOOL pExist=FALSE;
CTreeNode *c=GetDocument()->tree ;
CTreeNode *hold;
for(;;)
{
to=s.Find ('\\',from);
if(to==-1) break;
temp=s.Mid (from,to-from);
int m=c->Childrens.GetSize ();
if(m>0)
{
for(int i=0;i<m;i++)
{
hold=(CTreeNode*)c->Childrens[i];
if(temp.CompareNoCase(hold->itemname)==0)
{
pExist=TRUE;break;
}
}
}
if(pExist==TRUE) 
c=hold;
else 
{
CTreeNode *node=new CTreeNode(temp);
c->Childrens.Add (node);c=node;
}
from=to+1;pExist=FALSE;
}
if(pFolder)
c->zip_index=findex;
else
{
to=s.ReverseFind ('\\')+1;
temp=s.Mid (to,s.GetLength ()-to);
INDEX *in=new INDEX;
in->n =findex;
c->fIndex .SetAt(temp,in);
}

  }
*/
void CZipEXView::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	CTreeCtrl &pCtrl=GetTreeCtrl ();
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	if(pNMTreeView->action==TVE_COLLAPSE)
	{
		pCtrl.Expand (pNMTreeView->itemNew.hItem,TVE_COLLAPSE |TVE_COLLAPSERESET);
		CTreeNode *c=GetDocument()->tree ;
		hSel=pNMTreeView->itemNew.hItem;
		CTreeNode* hold=NodeFromItem(pNMTreeView->itemNew.hItem,c);
		CSmartZipDoc *pDoc=GetDocument ();
		pDoc->currentnode=hold;
		pDoc->UpdateAllViews (this, 2,NULL );
	}
	else if(pNMTreeView->action==TVE_EXPAND)
	{
		CTreeNode *c=GetDocument()->tree ;
		CTreeNode *hold=NodeFromItem(pNMTreeView->itemNew.hItem,c);
		int m=hold->Childrens .GetSize ();
		for(int i=0;i<m;i++)
		{
			CString x=((CTreeNode*)hold->Childrens[i])->itemname;
			TVINSERTSTRUCT tvInsert;
			tvInsert.hParent = pNMTreeView->itemNew.hItem;
			tvInsert.hInsertAfter = NULL;
			tvInsert.item.mask = TVIF_TEXT|TVIF_IMAGE|TVIF_CHILDREN|TVIF_SELECTEDIMAGE;
			tvInsert.item.pszText =(LPTSTR)(LPCTSTR)x;
			tvInsert.item.iImage=0;
			tvInsert.item.iSelectedImage=1;
			tvInsert.item.cChildren =I_CHILDRENCALLBACK;
			pCtrl.InsertItem(&tvInsert);	
		}
	}
	pCtrl.SortChildren (pNMTreeView->itemNew.hItem);
	*pResult = 0;
}
void CZipEXView::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	if(pTVDispInfo->item.mask & TVIF_CHILDREN)
	{
		CTreeNode *c=GetDocument()->tree ;
		if(c!=NULL)
		{
			CTreeNode* hold=NodeFromItem(pTVDispInfo->item.hItem,c);
			pTVDispInfo->item.cChildren=hold->Childrens .GetSize ()>0;
		}
		else
			pTVDispInfo->item.cChildren=0;
	}
	*pResult = 0;
}

CTreeNode* CZipEXView::NodeFromItem(HTREEITEM hItem,CTreeNode *c)
{
	CTreeCtrl &pCtrl=GetTreeCtrl ();
	CString s=pCtrl.GetItemText (hItem);
	s+= "\\dump";	
    while ((hItem = pCtrl.GetParentItem (hItem)) != NULL)
	{
        CString string = pCtrl.GetItemText (hItem);
        string += _T ("\\");
        s = string + s;
    }
	s=s.Right (s.GetLength ()-m_filelength-1); //to be put the file name
	CString temp;
	int from=0,to=0;
	CTreeNode *hold=c;
	for(;;)
	{
		to=s.Find ('\\',from);
		if(to==-1) 
		{
			break;
		}
		temp=s.Mid (from,to-from);
		int m=c->Childrens.GetSize ();
		if(m>0)
		{
			for(int i=0;i<m;i++)
			{
				hold=(CTreeNode*)c->Childrens[i];
				if(temp==(hold->itemname))
				{
					c=hold;break;
				}
			}
		}
		from=to+1;
	}
	return hold;
}
void CZipEXView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CSmartZipDoc* pDoc = GetDocument();
	CTreeCtrl &pCtrl=GetTreeCtrl ();
	ASSERT_VALID(pDoc);
	if(lHint==1)
	{
		if(pCtrl.ItemHasChildren(hSel))
		{
			
			CString temp=(LPCSTR)pHint;
			pCtrl.Expand (hSel,TVE_EXPAND);
			HTREEITEM hold=pCtrl.GetChildItem(hSel);
			while(hold!=NULL)
			{
				if(temp==(pCtrl.GetItemText (hold)))
				{
					hSel=hold;pCtrl.SelectItem (hold);					break;
				}
				hold=pCtrl.GetNextSiblingItem(hold);
			}
		}
	}
	else if(lHint==0)
	{
		//	CDebugTimer m;
		
		pCtrl.DeleteAllItems ();
		
		if(!pDoc->m_zip.IsClosed ())
		{
			TVINSERTSTRUCT tvInsert;
			tvInsert.hParent = NULL;
			tvInsert.hInsertAfter = NULL;
			tvInsert.item.mask = TVIF_TEXT|TVIF_IMAGE|TVIF_CHILDREN;
				int to;
				CString file,dir,temp(pDoc->GetPathName());
				to=temp.ReverseFind ('\\')+1;
				file=temp.Mid (to,temp.GetLength ()-to);
			m_filelength=file.GetLength ();
				//	char b[256];
			//		tvInsert.item.pszText="                           ";
			//	::lstrcpy (tvInsert.item.pszText,file);
			//	b[file.GetLength()]='\0';
			tvInsert.item.cchTextMax=256;
			tvInsert.item.pszText =file.GetBuffer (256);//"Zip File";//GetDocument()->m_filename.GetBuffer(GetDocument()->m_filename.GetLength());//"base";
			tvInsert.item.iImage=0;
			tvInsert.item.iSelectedImage=1;
			tvInsert.item.cChildren =I_CHILDRENCALLBACK;
			HTREEITEM V=pCtrl.InsertItem(&tvInsert);
				file.ReleaseBuffer (256);
			//delete b;
		}
		//	m.Stop ();
		pDoc->currentnode=pDoc->tree;
		hSel=pCtrl.GetRootItem ();
		//	AfxMessageBox (pDoc->m_zip .GetGlobalComment ());
		((CGuiSolExplorer*)(GetParent ()->GetParent ()->GetParent ()))->m_CommentView->GetEditCtrl().SetWindowText (pDoc->m_zip .GetGlobalComment ());
		pDoc->UpdateAllViews (this, 2,NULL );
	}
}
/*
void CZipEXView::OnTest() 
{
}*/


void CZipEXView::OnTreeRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint point;
	GetCursorPos (&point);
	CTreeCtrl &ctrl=this->GetTreeCtrl ();
	if (point != CPoint (-1, -1))
	{
		CPoint ptTree = point;
		ctrl.ScreenToClient (&ptTree);
		hCurItem = ctrl.HitTest (ptTree);	
		if (hCurItem != NULL)
		{
			CMenu menu,*m;
			menu.LoadMenu (IDR_TCONTEXT);
			m=menu.GetSubMenu (0);ctrl.SelectItem (hCurItem);
			m->TrackPopupMenu (TPM_LEFTBUTTON,point.x,point.y,this);
		}
	}
	*pResult = 0;
}
void CZipEXView::OnTcExtract() 
{
	CZipWordArray *arr=new CZipWordArray();
	this->GetDocument ()->AddSubFolders(arr,GetDocument ()->currentnode);
	CExtractDlg dlg;
	CNewWizDialog wiz;
	wiz.AddPage (&dlg,IDD_FORMEXTRACT);
	if(wiz.DoModal ()==ID_WIZFINISH)
	{
		CDProgress d;
		d.m_text="Extracting Files/folders";//Adding Filed/folders ...
		d.m_pDoc=this->GetDocument ();
		d.m_operation="e";
		d.pFull =dlg.m_bFullpath ;
		d.bOverwrite =dlg.m_bOverride ;
		d.m_extractpath=dlg.m_destination ;
		d.pReserved=arr;
		d.DoModal ();
	}
}
void CZipEXView::OnTcDelete() 
{
	CSmartZipDoc * pDoc=this->GetDocument ();
	CZipWordArray* arr = new CZipWordArray;
	pDoc->AddSubFolders(arr,GetDocument ()->currentnode);
	CDProgress d;
	d.m_text="Deleting Filed/folders ...";
	d.m_pDoc=this->GetDocument ();
	d.m_operation="d";
	d.pReserved=arr;
	d.DoModal ();
	if(pDoc->tree!=NULL)
		delete pDoc->tree;
	pDoc->tree=new CTreeNode();
	pDoc->UpdateAllViews (NULL,0,0);
	
}

void CZipEXView::OnTcAdd() 
{
	// TODO: Add your command handler code here
	this->GetDocument ()->UpdateAllViews (this,7,0);
}

void CZipEXView::OnTcProperties() 
{
	
	CSmartZipDoc* pDoc=GetDocument ();
	int nFiles=0,nFolders=0,tempindex=0;
	CTreeNode *hold=GetDocument ()->currentnode;
	if(hold==NULL) return;
	double sizecom=0,sizeucom=0;
	CString name=hold->itemname ;
	CZipWordArray wordarr;
	CString itemname=hold->itemname;
	pDoc->AddSubFolders (& wordarr,hold);
	int nn=wordarr.GetSize ();
	for (int i=0;i<nn;i++)
	{
		CZipFileHeader fh;
		pDoc->m_zip .GetFileInfo(fh,wordarr.GetAt (i));
		if(fh.IsDirectory ())
			nFolders++;
		else
			nFiles++;
		sizecom +=fh.m_uComprSize;
		sizeucom+=fh.m_uUncomprSize;
	}
	
	
	CFileProperties dlg;
	UINT flags = SHGFI_TYPENAME|SHGFI_ICON   | SHGFI_USEFILEATTRIBUTES;
	SHFILEINFO    shfi;
				SHGetFileInfo(_T("TMP") ,  FILE_ATTRIBUTE_DIRECTORY , &shfi, sizeof(SHFILEINFO), flags);
				dlg.bDir=TRUE;
				dlg.icon = shfi.hIcon;
				
				flags = SHGFI_TYPENAME|SHGFI_SYSICONINDEX  | SHGFI_USEFILEATTRIBUTES;
				//	SHFILEINFO    shfi;
				SHGetFileInfo(_T("TMP") ,  FILE_ATTRIBUTE_DIRECTORY , &shfi, sizeof(SHFILEINFO), flags);
				dlg.m_contains.Format ("%d Files, %d Folders",nFiles,nFolders);	
				dlg.m_type =shfi.szTypeName;
				dlg.m_filename=name;
				dlg.m_strtitle+=" - "+name;
				dlg.m_filename =name;
				StrFormatByteSize (sizecom,dlg.m_comsize.GetBuffer(20),20);
				dlg.m_comsize.ReleaseBuffer (20);
				StrFormatByteSize (sizeucom,dlg.m_ucomsize.GetBuffer (20),20);
				dlg.m_ucomsize.ReleaseBuffer (20);
				CZipFileHeader fh;
				pDoc->m_zip .GetFileInfo(fh,tempindex);		
				CTime t(fh.GetTime());
				dlg.m_moddate= t.Format(_T("%d %b %Y, %X"));	
				dlg.DoModal ();
}