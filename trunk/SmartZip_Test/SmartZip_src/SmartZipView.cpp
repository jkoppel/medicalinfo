// SmartZipView.cpp : implementation of the CSmartZipView class
//

#include "stdafx.h"
#include "SmartZip.h"
#include "mainfrm.h"

#include "SmartZipDoc.h"
#include "SmartZipView.h"
#include "NewWizDialog.h"
#include "ExtractDlg.h"
#include "progress.h"
#include "Shlwapi.h"
#include "AddChoose.h"
#include "AddFilesDlg.h"
#include "FolderAdd.h"
#include "AddOptions.h"
#include "FileView.h"
#include "FileProperties.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSmartZipView

IMPLEMENT_DYNCREATE(CSmartZipView, CListView)

BEGIN_MESSAGE_MAP(CSmartZipView, CListView)
//{{AFX_MSG_MAP(CSmartZipView)
ON_WM_CREATE()
ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
ON_WM_DESTROY()
ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
ON_NOTIFY_REFLECT(HDN_ENDDRAG, OnHeaderEnddrag)
ON_NOTIFY_REFLECT(HDN_ITEMCLICK, OnHeaderItemclick)
ON_NOTIFY_REFLECT(NM_RCLICK, OnItemRclick)
ON_COMMAND(ID_TC_EXTRACT, OnTcExtract)
ON_WM_CONTEXTMENU()
ON_NOTIFY(HDN_ITEMCLICK, 0, OnHdnItemClick)
ON_COMMAND(ID_TC_DELETE, OnTcDelete)
ON_COMMAND(ID_TC_PROPERTIES, OnTcProperties)
ON_COMMAND(ID_TC_ADD, OnTcAdd)
ON_COMMAND(ID_TC_VIEW, OnTcView)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginlabeledit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmartZipView construction/destruction

CSmartZipView::CSmartZipView()
{
	// TODO: add construction code here
	
}

CSmartZipView::~CSmartZipView()
{
}

BOOL CSmartZipView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style|=LVS_REPORT|LVS_AUTOARRANGE|LVS_EDITLABELS|LVS_SHAREIMAGELISTS|LVS_SHOWSELALWAYS ;
	
	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSmartZipView drawing

void CSmartZipView::OnDraw(CDC* pDC)
{
	CSmartZipDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

void CSmartZipView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();
	/*	if (m_headerctrl.GetSafeHwnd() == NULL && m_hWnd != NULL)
	{
	HWND h=GetListCtrl ().GetSafeHwnd (),h2=GetListCtrl ().GetHeaderCtrl ()->GetSafeHwnd ();
	if(h)
	m_headerctrl.SubclassWindow( ::GetDlgItem(h,0) );
	}
	*/
	CListView::OnInitialUpdate();
	CListCtrl &pCtrl=this->GetListCtrl ();
	//	pCtrl.SetImageList (&m_imgList,LVSIL_SMALL      );	
	CImageList* pil1 = &(((CMainFrame*)AfxGetMainWnd())->m_small);
	if (pil1->m_hImageList)
		pCtrl.SetImageList(pil1, LVSIL_SMALL);
	CImageList* pil2 = &(((CMainFrame*)AfxGetMainWnd())->m_large);
	if (pil2->m_hImageList)
		GetListCtrl().SetImageList(pil2, LVSIL_NORMAL);
	
	
	// TODO: You may populate your ListView with items by directly accessing
	//  its list control through a call to GetListCtrl().
}

/////////////////////////////////////////////////////////////////////////////
// CSmartZipView diagnostics

#ifdef _DEBUG
void CSmartZipView::AssertValid() const
{
	CListView::AssertValid();
}

void CSmartZipView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CSmartZipDoc* CSmartZipView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSmartZipDoc)));
	return (CSmartZipDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSmartZipView message handlers

int CSmartZipView::m_iColums[NCOLS] = {-1};

int CSmartZipView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,0,(LPARAM)LVS_EX_HEADERDRAGDROP|LVS_EX_FULLROWSELECT );//LVS_EX_SUBITEMIMAGES|
	//	HWND hWnd=this->GetListCtrl ().GetHeaderCtrl ()->m_hWnd;
	//	::SetWindowLong(hWnd ,GWL_STYLE,::GetWindowLong (hWnd,GWL_STYLE)|HDS_FILTERBAR);
	//LVS_EX_GRIDLINES |LVS_EX_ONECLICKACTIVATE||LVS_EX_UNDERLINEHOT
	/*CHeaderCtrl *c=this->GetListCtrl ().GetHeaderCtrl ();
	m_imgList.Create(16, 16, ILC_COLORDDB|ILC_MASK, 13, 1);
	bitmap.LoadBitmap(IDB_BITMAP1);
	m_imgList.Add(&bitmap, RGB(255,255,255));
	c->SetImageList (&m_imgList);*/
	
	/*	LVCOLUMN B;B.mask =LVCF_TEXT|LVCF_IMAGE|LVCF_WIDTH|LVCF_FMT;
	B.fmt=LVCFMT_BITMAP_ON_RIGHT|LVCFMT_COL_HAS_IMAGES ;
	B.pszText="File Name";
	B.iImage =1;
	B.cx=100;
	B.cchTextMax =sizeof("File Name");
	GetListCtrl().InsertColumn (0,&B);
	B.pszText="COOOLLgfdg";
	GetListCtrl().InsertColumn (0,&B);	
	*/
	for (int i = 0; i < NCOLS; i++)
	{
		CString col;
		col.LoadString(IDS_COL1 + i);
		this->GetListCtrl ().InsertColumn(i, col, (i == 0 ||i==1||i == NCOLS - 1) ? LVCFMT_LEFT : LVCFMT_RIGHT );
	}
	if (m_iColums[0] == -1) // initializing needed
	{
		GetListCtrl().SetRedraw(FALSE);
		int iPaddingSize = 4*GetSystemMetrics(SM_CXDLGFRAME);
		CDC* cdc = this->GetDC();
		
		CFont *pFont = this->GetFont();
		ASSERT(pFont);
		CFont* pPrevFont = cdc->SelectObject(pFont);
		
		TEXTMETRIC tm;
		cdc->GetTextMetrics(&tm);
		int nItems = GetListCtrl().GetItemCount();
		int iColumn;
		for (iColumn = 0; iColumn < NCOLS; iColumn++)
		{
			HDITEM hdi;
			TCHAR  lpBuffer[512];
			hdi.mask = HDI_TEXT | HDI_FORMAT;
			hdi.pszText = lpBuffer;
			hdi.cchTextMax = 512;
			GetListCtrl().GetHeaderCtrl()->GetItem(iColumn, &hdi);
			
			m_iColums[iColumn] = cdc->GetOutputTextExtent(lpBuffer).cx + 2*tm.tmAveCharWidth; // header width
		}
		
		
		
		for (iColumn = 0; iColumn < NCOLS-1 ; iColumn++) // leave Comment col
		{
			CString sz;
			switch (iColumn)
			{
			case 0:case 1:
				{
					//	for (int i = 0; i < 20; i ++)
					sz += _T("COOOOOOOOOOOOOOOOOL"); // average
					break;
				}
			case 2:case 3:
				sz = _T("999 999 999");
				break;
			case 4:
				sz = _T("100%");
				break;
			case 5:sz = _T("99 OOO 9999, 99:99:99");
				break;
			case 6:
				sz = _T("aaaa");
				break;
			case 7:
				sz = _T("0xfffffffffffff");
				break;
				
			}
			int width = cdc->GetOutputTextExtent(sz).cx;
			if (width > m_iColums[iColumn]) 
				m_iColums[iColumn] = width;
			m_iColums[iColumn] += iPaddingSize;	
		}
		cdc->SelectObject(pPrevFont);
		this->ReleaseDC(cdc);
		GetListCtrl().SetRedraw();
		GetListCtrl().RedrawWindow();	
	}
	for (int iColumn = 0; iColumn < NCOLS; iColumn++)
		GetListCtrl().SetColumnWidth(iColumn, m_iColums[iColumn]);	
	
	//	ConstructHeader();
	//ordering
	//	CHeaderCtrl* ctrl=GetListCtrl ().GetHeaderCtrl ();
	//	ctrl->SetOrderArray (ctrl->GetItemCount (),((CSmartZipApp*)::AfxGetApp ())->m_options.headerhid);
	
	return 0;
}
void CSmartZipView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if (lHint == 2) 
	{
		CListCtrl& pCtrl=GetListCtrl ();
		FreeItemMemory ();
		pCtrl.DeleteAllItems ();
		CSmartZipDoc* pDoc=this->GetDocument ();
		CTreeNode * current=pDoc->currentnode;
		if(current==NULL) return;
		int m=current->Childrens .GetSize ();
		int i;
		for (i=0;i<m;i++)
		{
			LV_ITEM lvi;
			lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM; 
			lvi.iItem = i; 
			lvi.iSubItem = 0; 
			lvi.pszText = LPSTR_TEXTCALLBACK; 
			LISTDATA* d=new LISTDATA;
			d->name =((CTreeNode*)current->Childrens[i])->itemname;
			d->zipindex =((CTreeNode*)current->Childrens[i])->zip_index;
			lvi.lParam = (LPARAM )d;
			SHFILEINFO    shfi;
			UINT flags = SHGFI_SYSICONINDEX  | SHGFI_USEFILEATTRIBUTES;//| SHGFI_SMALLICON
			SHGetFileInfo(_T("TMP") ,  FILE_ATTRIBUTE_DIRECTORY , &shfi, sizeof(SHFILEINFO), flags);
			lvi.iImage=shfi.iIcon;
			if (pCtrl.InsertItem (&lvi)==-1)
				return;
			
		}
		POSITION pos=current->fIndex.GetStartPosition ();
		CObject* pValue;CString iName;
		while(pos!=NULL)
		{
			
			current->fIndex.GetNextAssoc (pos,iName,pValue);
			LV_ITEM lvi;
			lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM; 
			lvi.iItem = i; 
			lvi.iSubItem = 0; 
			lvi.pszText = LPSTR_TEXTCALLBACK; 
			LISTDATA* d=new LISTDATA;
			d->zipindex =((INDEX*)pValue)->n;
			d->name=iName;
			lvi.lParam = (LPARAM )d;
			SHFILEINFO    shfi;
			UINT flags = SHGFI_SYSICONINDEX  | SHGFI_USEFILEATTRIBUTES;//| SHGFI_SMALLICON
			SHGetFileInfo(d->name ,  FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(SHFILEINFO), flags);
			lvi.iImage=shfi.iIcon;
			pCtrl.InsertItem (&lvi);
			i++;
		}
	}
	else if(lHint ==3)
	{
		OnTcDelete();
	}
	else if(lHint ==4)
	{
		OnTcProperties() ;
	}
	else if(lHint ==5)
	{
		OnTcExtract() ;
	}
	else if(lHint ==6)
	{
		OnTcView() ;
	}
	else if(lHint ==7)
	{
		OnTcAdd() ;
	}
	CListView::OnUpdate (pSender, lHint, pHint);	
}

void CSmartZipView::FreeItemMemory()
{
	int nCount = GetListCtrl ().GetItemCount ();
    if (nCount>0)
	{
		for (int i=0; i<nCount; i++)
            delete  (LISTDATA *)GetListCtrl ().GetItemData (i);
    }
	
}

CString CSmartZipView::FormatSize(DWORD iSize)
{
	CString sz;
	sz.Format(_T("%lu"), iSize);
	int iT = 0;
	for (int i = sz.GetLength() - 1; i > 0; i--)
		if (!(++iT % 3))
			sz.Insert(i, _T(" "));
		return sz;
}
void CSmartZipView::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	LISTDATA* data=(LISTDATA*)pDispInfo->item.lParam;
	int zipindex = (int)data->zipindex ;
	CZipFileHeader fh;
	BOOL IsFolder=FALSE;
	if(zipindex!=-1)
	{
		GetDocument ()->m_zip.GetFileInfo(fh,zipindex);
		if(fh.IsDirectory()) IsFolder=TRUE;
	}
	else IsFolder=TRUE;
	
	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CString pszText="";
		HDITEM v;v.mask =HDI_TEXT;
		TCHAR  lpBuffer[256];		
		v.pszText = lpBuffer;
		v.cchTextMax = 256;
		
		this->GetListCtrl ().GetHeaderCtrl ()->GetItem (pDispInfo->item.iSubItem,&v);
		CString hName=v.pszText;
		if(!hName.CompareNoCase ("FILE NAME"))
		{
			pszText=data->name;
		}
		
		else if(!hName.CompareNoCase ("TYPE"))
		{
			UINT flags = SHGFI_TYPENAME|SHGFI_SYSICONINDEX  | SHGFI_USEFILEATTRIBUTES;
			SHFILEINFO    shfi;
			if(IsFolder)
			{
				SHGetFileInfo(_T("TMP") ,  FILE_ATTRIBUTE_DIRECTORY , &shfi, sizeof(SHFILEINFO), flags);
			}
			else
			{
				SHGetFileInfo(data->name,  FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(SHFILEINFO), flags);
			}
			pszText=shfi.szTypeName;
		}
		else if(!hName.CompareNoCase ("SIZE"))
		{
			if(!IsFolder)
			{
				char buf[20];
				StrFormatByteSize (fh.m_uUncomprSize,buf,20);
				pszText=buf;
			}
		}
		else if(!hName.CompareNoCase ("PACKED SIZE"))
		{
			if(!IsFolder)
			{
				char buf[20];
				StrFormatByteSize (fh.m_uComprSize,buf,20);
				pszText=buf;
			}
		}
		else if(!hName.CompareNoCase ("RATIO"))
		{
			if(!IsFolder)
			{
				pszText.Format(_T("%.0f%%"), fh.GetCompressionRatio());
			}
		}
		else if(!hName.CompareNoCase ("MODIFIED"))
		{
			if(zipindex!=-1)
			{
				CTime t(fh.GetTime());
				pszText= t.Format(_T("%d %b %Y, %X"));	
			}
		}
		else if(!hName.CompareNoCase ("ATTRIBUTES"))
		{
			if(!IsFolder)
			{
				DWORD uAttr = fh.GetSystemAttr();
				TCHAR d = _T('-');
				pszText = uAttr & FILE_ATTRIBUTE_READONLY ? _T('r') : d;
				pszText += uAttr & FILE_ATTRIBUTE_ARCHIVE ? _T('a') : d;
				pszText += uAttr & FILE_ATTRIBUTE_HIDDEN ? _T('h') : d;
				pszText += uAttr & FILE_ATTRIBUTE_SYSTEM ? _T('s') : d;
			}
		}
		else if(!hName.CompareNoCase ("CRC"))
		{
			if(!IsFolder)
			{	
				char buf[20];
				::wsprintf (buf,"%#lx",fh.m_uCrc32);
				pszText=buf;
			}
		}
		else if(!hName.CompareNoCase ("COMMENTS"))
		{
			if(!IsFolder)
			{
				pszText=fh.GetComment();
				if (pszText.GetLength() > 255)
					pszText = pszText.Left(255);
			}
		}
		::lstrcpy (pDispInfo->item.pszText,pszText );
		
	}
	//	*pResult = 0;
	if(*pResult==1)
		*pResult=IsFolder;
}

void CSmartZipView::OnDestroy() 
{
	FreeItemMemory();
	//	CHeaderCtrl* ctrl=GetListCtrl ().GetHeaderCtrl ();
	//	ctrl->GetOrderArray (((CSmartZipApp*)::AfxGetApp ())->m_options.headerhid,ctrl->GetItemCount ());
	CListView::OnDestroy();
	
	
}

void CSmartZipView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int selitem=pNMListView->iItem;//modified by hwy
	if(selitem==-1) return;
	CListCtrl& pCtrl=GetListCtrl ();
	LISTDATA* data=(LISTDATA*)pCtrl.GetItemData (selitem);
	CString itemname=data->name ;
	CSmartZipDoc* pDoc=GetDocument ();
	CZipFileHeader fh;
	if(data->zipindex !=-1)
		pDoc->m_zip .GetFileInfo(fh,data->zipindex);
	if(data->zipindex==-1 || fh.IsDirectory())
	{
		CTreeNode *c=pDoc->currentnode;
		CTreeNode *hold;
		int m=c->Childrens.GetSize ();
		for(int i=0;i<m;i++)
		{
			
			hold=(CTreeNode*)c->Childrens[i];
			if(itemname==(hold->itemname))
			{
				pDoc->currentnode=hold;break;
			}
		}
		OnUpdate(NULL,2,NULL);
		pDoc->UpdateAllViews (this, 1,(CObject*)(LPCSTR)itemname );
	}
	*pResult = 0;
}

void CSmartZipView::OnHeaderEnddrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY * phdn = (HD_NOTIFY *) pNMHDR;
	//	CHeaderCtrl* ctrl=GetListCtrl ().GetHeaderCtrl ();
	//	ctrl->GetOrderArray (((CSmartZipApp*)::AfxGetApp ())->m_options.headerhid,ctrl->GetItemCount ());
	*pResult = 0;
}

void CSmartZipView::ConstructHeader()
{
	
}

void CSmartZipView::OnHeaderItemclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CSmartZipView::OnItemRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CListCtrl &pCtrl=this->GetListCtrl ()	;
	CPoint point;
	GetCursorPos (&point);
	if(this->GetDocument ()->tree !=0)
	{
		CMenu m;m.LoadMenu (IDR_LCONTEXT);
		CMenu* m2;m2=m.GetSubMenu (0);
		m2->TrackPopupMenu (TPM_RIGHTBUTTON,point.x,point.y,this);
	}
/*	else
	{
		CMenu m;m.LoadMenu (IDR_NEWZIP);
		CMenu* m2;m2=m.GetSubMenu (0);
		int nRet =m2->TrackPopupMenu (TPM_RIGHTBUTTON,point.x,point.y,this);
		if(nRet ==ID_FILE_NEW)
		{
		//	::AfxGetApp ()->OnFileNew ();
			AfxMessageBox ("");
		}

		
	}*/
	*pResult = 0;
}

void CSmartZipView::OnTcExtract() 
{
	if(this->GetDocument ()->tree !=0)
	{
		
		CListCtrl &pCtrl=this->GetListCtrl ()	;
		POSITION pos=pCtrl.GetFirstSelectedItemPosition ();
		CZipWordArray *arr=new CZipWordArray();
		while(pos!=NULL)
		{
			int m=pCtrl.GetNextSelectedItem (pos);
			LISTDATA * data=(LISTDATA *)pCtrl.GetItemData (m);
			//////////////////////////////////////////
			CSmartZipDoc* pDoc=GetDocument ();
			CZipFileHeader fh;
			if(data->zipindex !=-1)
				pDoc->m_zip .GetFileInfo(fh,data->zipindex);
			if(data->zipindex==-1 || fh.IsDirectory())
			{
				CString itemname=data->name ;
				CTreeNode *c=pDoc->currentnode;
				CTreeNode *hold;
				int m=c->Childrens.GetSize ();
				for(int i=0;i<m;i++)
				{
					
					hold=(CTreeNode*)c->Childrens[i];
					if(itemname==(hold->itemname))
					{
						pDoc->AddSubFolders(arr,hold);break;
					}
				}
			}
			else
			{
				arr->Add (data->zipindex);
			}
			
			///////////////////////////////////
			
		}	
		
		CExtractDlg dlg;
		CNewWizDialog wiz;
		wiz.AddPage (&dlg,IDD_FORMEXTRACT);
		if(wiz.DoModal ()==ID_WIZFINISH)
		{
			CDProgress d;
			d.m_text="Extracting Files/folders";//Extracting Files/folders ...
			d.m_pDoc=this->GetDocument ();
			d.m_operation="e";
			d.pFull =dlg.m_bFullpath ;
			d.bOverwrite =dlg.m_bOverride ;
			d.m_extractpath=dlg.m_destination ;
			d.pReserved=arr;
			d.DoModal ();
			delete arr;
		}
	}
	else
	{
		AfxMessageBox ("Please Create new Zip file First");
	}
}

void CSmartZipView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
}
void CSmartZipView::OnHdnItemClick(NMHDR* pNMHDR, LRESULT* pResult)
{
/*	BOOL bAscending;
INT iOld = m_headerctrl.GetSortColumn(&bAscending);
INT iNew = ((LPNMHEADER)pNMHDR)->iItem;
if(iNew == iOld)
bAscending = !bAscending;
else
bAscending = TRUE;
m_headerctrl.SetSortColumn(iNew,bAscending);
CListCtrl *pCtrl=&(this->GetListCtrl());
	pCtrl->SortItems(CompareCountries,(LPARAM) this);*/
}
int CALLBACK CSmartZipView::CompareCountries(LPARAM lParam1, LPARAM lParam2, 
											 LPARAM lParamSort)
{
/*	CSmartZipView* pListView = (CSmartZipView*) lParamSort;
LV_DISPINFO* pDispInfo = new LV_DISPINFO;
memset(pDispInfo,0,sizeof(LV_DISPINFO));
long r=1,r1=1;BOOL asc;
pDispInfo->item.mask |= LVIF_TEXT;
pDispInfo->item.iSubItem=pListView->m_headerctrl.GetSortColumn (&asc);
CString    strItem1;
pDispInfo->item.pszText=strItem1.GetBuffer (256);
pDispInfo->item.lParam=lParam1;
pListView->OnGetdispinfo((NMHDR*)pDispInfo,&r);
strItem1.ReleaseBuffer();
CString    strItem2;
pDispInfo->item.pszText=strItem2.GetBuffer (256);
pDispInfo->item.lParam=lParam2;
pListView->OnGetdispinfo((NMHDR*)pDispInfo,&r1);
strItem2.ReleaseBuffer();
delete pDispInfo;
if(r&&r1)
{
if(!asc)
return strItem1.CompareNoCase(strItem2 );
else
return strItem2.CompareNoCase(strItem1);
}
else
{
if(r)
return false;
else
return true;
}*/
	return FALSE;
}

void CSmartZipView::OnTcDelete() 
{
	if(this->GetDocument ()->tree !=0)
	{
		
		int b=AfxMessageBox ("Do you really want to Delete The  selected file/folers(s)?",MB_OKCANCEL );
			if(b==IDOK)
			{
				
				CListCtrl &pCtrl=this->GetListCtrl ()	;
				POSITION pos=pCtrl.GetFirstSelectedItemPosition ();
				CZipWordArray *arr=new CZipWordArray();
				while(pos!=NULL)
				{
					int m=pCtrl.GetNextSelectedItem (pos);
					LISTDATA * data=(LISTDATA *)pCtrl.GetItemData (m);
					//////////////////////////////////////////
					CSmartZipDoc* pDoc=GetDocument ();
					CZipFileHeader fh;
					if(data->zipindex !=-1)
						pDoc->m_zip .GetFileInfo(fh,data->zipindex);
					if(data->zipindex==-1 || fh.IsDirectory())
					{
						CString itemname=data->name ;
						CTreeNode *c=pDoc->currentnode;
						CTreeNode *hold;
						int m=c->Childrens.GetSize ();
						for(int i=0;i<m;i++)
						{
							
							hold=(CTreeNode*)c->Childrens[i];
							if(itemname==(hold->itemname))
							{
								pDoc->AddSubFolders(arr,hold);break;
							}
						}
					}
					else
					{
						arr->Add (data->zipindex);
					}
					
					///////////////////////////////////
					
				}	
				CDProgress d;
				d.m_text="Deleting Files/folders ...";
				d.m_pDoc=this->GetDocument ();
				d.m_operation="d";
				d.pReserved=arr;
				d.DoModal ();
				CSmartZipDoc * pDoc=this->GetDocument ();
				//	if(pDoc->tree!=NULL)
				//		delete pDoc->tree;
			//	pDoc->MakeTree (); //tree=new CTreeNode();
			//	pDoc->UpdateAllViews (NULL,0,0);
					d.m_pDoc->CloseCurrentFile ();
		d.m_pDoc ->OpenCurrentFile (d.m_pDoc->m_strfilename );

			}
	}
	else
	{
		AfxMessageBox ("Please Create new Zip file First");
	}
	
}

void CSmartZipView::OnTcProperties() 
{
	if(this->GetDocument ()->tree !=0)
	{
		CListCtrl &pCtrl=this->GetListCtrl ();
	///////////////////////////
		int nFiles=0,nFolders=0,tempindex=0;
		double sizecom=0,sizeucom=0;
		CString name;
		CZipWordArray wordarr;
		BOOL bDir;
	//////////////////////////////
		CSmartZipDoc* pDoc=GetDocument ();
		POSITION pos=pCtrl.GetFirstSelectedItemPosition ();
		while(pos!=NULL)
		{
			int m=pCtrl.GetNextSelectedItem (pos);
			LISTDATA* data=(LISTDATA* )pCtrl.GetItemData (m);
			CZipFileHeader fh;
			if(data->zipindex !=-1)
			{
				pDoc->m_zip .GetFileInfo(fh,data->zipindex);
				if(!fh.IsDirectory ())
				{
					name=data->name ;
					
					CZipFileHeader fh;
					pDoc->m_zip .GetFileInfo(fh,data->zipindex);
					tempindex=data->zipindex;
					bDir=false;
					sizecom +=fh.m_uComprSize;
					sizeucom+=fh.m_uUncomprSize;
					nFiles++;
				}
				else
				{
					CString itemname=data->name ;
					bDir=TRUE;name=data->name ;
					tempindex=data->zipindex ;
					CTreeNode *c=pDoc->currentnode;
					CTreeNode *hold;
					int m=c->Childrens.GetSize ();
					int i;
					for(i=0;i<m;i++)
					{
						
						hold=(CTreeNode*)c->Childrens[i];
						if(itemname==(hold->itemname))
						{
							pDoc->AddSubFolders (& wordarr,hold);
							break;
						}
						
					}
					int nn=wordarr.GetSize ();
					for (i=0;i<nn;i++)
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
					wordarr.RemoveAll ();
				}
			}
			else if(data->zipindex ==-1)
			{
				CString itemname=data->name ;
				bDir=TRUE;name=data->name ;
				CTreeNode *c=pDoc->currentnode;
				CTreeNode *hold;
				int m=c->Childrens.GetSize ();
				int i;
				for(i=0;i<m;i++)
				{
					
					hold=(CTreeNode*)c->Childrens[i];
					if(itemname==(hold->itemname))
					{
						pDoc->AddSubFolders (& wordarr,hold);
						break;
					}	
				}
				int nn=wordarr.GetSize ();
				for (i=0;i<nn;i++)
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
				wordarr.RemoveAll ();
			}
		}
		if(pCtrl.GetSelectedCount ()>1)
		{
			CFileProperties dlg;
			dlg.bDir=TRUE;
			dlg.m_type ="Multiple Types";
			dlg.m_strtitle+=" - Multiple selected";
			dlg.m_filename ="Multiple selected";
			dlg.icon =::LoadIcon (::AfxGetApp ()->m_hInstance ,MAKEINTRESOURCE(IDR_SMARTZTYPE));
			
			dlg.m_contains.Format ("%d Files, %d Folders",nFiles,nFolders);
			StrFormatByteSize (sizecom,dlg.m_comsize.GetBuffer(20),20);
			dlg.m_comsize.ReleaseBuffer (20);
			StrFormatByteSize (sizeucom,dlg.m_ucomsize.GetBuffer (20),20);
			dlg.m_ucomsize.ReleaseBuffer (20);
			dlg.DoModal ();
		//	AfxMessageBox ("Please select only one file");
		}
		else if(pCtrl.GetSelectedCount ()==1)
		{
			CFileProperties dlg;
			UINT flags = SHGFI_TYPENAME|SHGFI_ICON   | SHGFI_USEFILEATTRIBUTES;
			SHFILEINFO    shfi;
			if(bDir)
			{
				SHGetFileInfo(_T("TMP") ,  FILE_ATTRIBUTE_DIRECTORY , &shfi, sizeof(SHFILEINFO), flags);
				dlg.bDir=TRUE;
			}
			else
			{
				SHGetFileInfo(name,  FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(SHFILEINFO), flags);
			}
			dlg.icon = shfi.hIcon;

			flags = SHGFI_TYPENAME|SHGFI_SYSICONINDEX  | SHGFI_USEFILEATTRIBUTES;
		//	SHFILEINFO    shfi;
			if(bDir)
			{
				SHGetFileInfo(_T("TMP") ,  FILE_ATTRIBUTE_DIRECTORY , &shfi, sizeof(SHFILEINFO), flags);
			}
			else
			{
				SHGetFileInfo(name,  FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(SHFILEINFO), flags);
			}
			dlg.m_contains.Format ("%d Files, %d Folders",nFiles,nFolders);	
			dlg.m_type =shfi.szTypeName;
			dlg.m_filename=name;
			dlg.m_strtitle+=" - "+name;
			dlg.m_filename =name;
			StrFormatByteSize (sizecom,dlg.m_comsize.GetBuffer(20),20);
			dlg.m_comsize.ReleaseBuffer (20);
			StrFormatByteSize (sizeucom,dlg.m_ucomsize.GetBuffer (20),20);
			dlg.m_ucomsize.ReleaseBuffer (20);
			dlg.m_comratio.Format(_T("%.2f%%"),(double)sizecom/sizeucom);
			CZipFileHeader fh;
			pDoc->m_zip .GetFileInfo(fh,tempindex);		
			CTime t(fh.GetTime());
			dlg.m_moddate= t.Format(_T("%d %b %Y, %X"));	
			dlg.m_crc.Format ("%#lx",fh.m_uCrc32);
			dlg.fh =&fh;
			if(dlg.DoModal ()==IDOK)
			{
				//pDoc->m_zip.SetFileComment (tempindex,dlg.comment );
				pDoc->m_zip.SetGlobalComment (dlg.comment );//modified by hwy
				pDoc->SaveModified();
			}
		}
		
		//	dlg.m_strtitle ="one folder known!";
	}
	else
	{
		AfxMessageBox ("Please Create new Zip file First");
	}
	
}
void CSmartZipView::OnTcAdd() 
{
	if(this->GetDocument ()->tree !=0)
	{
		
		
		// TODO: Add your command handler code here
		CAddChoose dlg1;
		CAddFilesDlg dlg2;
		CFolderAdd dlg3;
		CAddOptions dlg4;
		CNewWizDialog wiz;
		wiz.AddPage (&dlg1,IDD_ADDCHOOSE);
		wiz.AddPage (&dlg2,IDD_ADDFILES);
		wiz.AddPage (&dlg3,IDD_ADDFOLDER);
		wiz.AddPage (&dlg4,IDD_ADDOPTIONS);
		
		if(wiz.DoModal ()==ID_WIZFINISH)
		{
			CStringArray* pArray = new CStringArray;
			//	AfxMessageBox ("finished");
			if(dlg1.m_choose )
			{
				CString s=dlg3.m_path;//m_shelltree.GetSelectedPath();
				pArray->Add(s); // for SetRootPath
				pArray->Add(s);
				
			}
			else
			{
				pArray->Add(_T("")); // for SetRootPath
				pArray->Append ( dlg2.filesarr);
				/*	int m=dlg2.m_fileslist.GetItemCount ();
				for (int i=0;i<m;i++)
				{
				pArray->Add ( dlg2.m_fileslist.GetItemText (i,0));
				
			}*/
			}
			CDProgress d;
			d.m_text="Adding Files/folders ...";
			d.m_pDoc=GetDocument ();
			d.zip =&d.m_pDoc->m_zip ;
			d.m_operation="a";
			d.pFull =dlg4.m_full;
			d.bOverwrite =dlg4.m_compression;
			d.pReserved=pArray;
			d.bEnc=dlg4.m_upass;
			d.DoModal ();
			CSmartZipDoc * pDoc=this->GetDocument ();
			//	if(pDoc->tree!=NULL)
			//		delete pDoc->tree;
		d.m_pDoc->CloseCurrentFile ();
		d.m_pDoc ->OpenCurrentFile (d.m_pDoc->m_strfilename );
			//	pDoc->MakeTree (); //tree=new CTreeNode();
		//	pDoc->UpdateAllViews (NULL,0,0);
			
		}
		
		
	}
	else
	{
		AfxMessageBox ("Please Create new Zip file First");
	}
}
void CSmartZipView::OnTcView() 
{
	this->SetFocus ();
	// TODO: Add your command handler code here
	if(this->GetDocument ()->tree !=0)
	{
		CListCtrl &pCtrl=this->GetListCtrl ();
		if(pCtrl.GetSelectedCount ()>1)
		{
			AfxMessageBox ("Please select only one file");
		}
		else
		{
			POSITION pos=pCtrl.GetFirstSelectedItemPosition ();
			int m=pCtrl.GetNextSelectedItem (pos);
			if(m==-1) return;
			LISTDATA* d=(LISTDATA* )pCtrl.GetItemData (m);
			if(d->zipindex!=-1)
			{
				CZipFileHeader fh;
				GetDocument ()->m_zip.GetFileInfo(fh,d->zipindex);
				if(fh.IsDirectory())
				{AfxMessageBox ("Please select a file");}else
				{
					CFileView dlg;
					BOOL b=GetDocument ()->m_zip.ExtractFile(d->zipindex,dlg.memfile );
					
					dlg.DoModal ();
				}
			}
			else
			{AfxMessageBox ("Please select a file");}
			
		}
	}
	else
	{
		AfxMessageBox ("Please Create new Zip file First");
	}
	
}

void CSmartZipView::SelectAll(BOOL bSelect)
{
CListCtrl &pCtrl=this->GetListCtrl ();
	int count=pCtrl.GetItemCount ();
	for(int i=0;i<count;i++)
	{
		pCtrl.SetItemState(i,bSelect?LVIS_SELECTED:0,LVIS_SELECTED);
	}
		
}

void CSmartZipView::Selectinverse()
{
	CListCtrl &pCtrl=this->GetListCtrl ();
	int count=pCtrl.GetItemCount ();
	for(int i=0;i<count;i++)
	{
		int m=pCtrl.GetItemState (i,LVIS_SELECTED);
		pCtrl.SetItemState(i,m!=LVIS_SELECTED?LVIS_SELECTED:0,LVIS_SELECTED);

	}
}

void CSmartZipView::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
/*	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LISTDATA* data=(LISTDATA*)pDispInfo->item.lParam;
	int zipindex = (int)data->zipindex ;
	CZipFileHeader fh;
	CListCtrl &pCtrl=this->GetListCtrl ()	;

		GetDocument ()->m_zip.GetFileInfo(fh,zipindex);
	
		POSITION pos=pCtrl.GetFirstSelectedItemPosition ();
		int m=pCtrl.GetNextSelectedItem (pos);
//		fh.SetFileName(pDispInfo->item.pszText);
	*pResult = 1;
	*/
}

void CSmartZipView::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
/*	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LISTDATA* data=(LISTDATA*)pDispInfo->item.lParam;
	int zipindex = (int)data->zipindex ;
	CZipFileHeader fh;
	if(zipindex!=-1)
	{
		GetDocument ()->m_zip.GetFileInfo(fh,zipindex);
		if(fh.IsDirectory()) *pResult = 1;
		else *pResult = 0;
	}
	else *pResult = 1;*/
	*pResult =1;
}
