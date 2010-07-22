// SmartZipDoc.cpp : implementation of the CSmartZipDoc class
//

#include "stdafx.h"
#include "SmartZip.h"

#include "SmartZipDoc.h"
#include "ZipPlatform.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSmartZipDoc

IMPLEMENT_DYNCREATE(CSmartZipDoc, CDocument)

BEGIN_MESSAGE_MAP(CSmartZipDoc, CDocument)
	//{{AFX_MSG_MAP(CSmartZipDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmartZipDoc construction/destruction

CSmartZipDoc::CSmartZipDoc()
{
	m_bAfterException = false;
	tree=0;currentnode=0;
bStart=FALSE;
}

CSmartZipDoc::~CSmartZipDoc()
{
}

BOOL CSmartZipDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
if(bStart)
{
	CFileDialog cFileDlg
	(	FALSE, 
	(LPCTSTR)"zip", 
	(LPCTSTR)"archive1", 
	OFN_ENABLESIZING|OFN_HIDEREADONLY|OFN_EXPLORER|OFN_LONGNAMES|OFN_OVERWRITEPROMPT|OFN_CREATEPROMPT, 
	_T("All Files (*.zip)|*.zip|"));
	cFileDlg.m_ofn.lpstrTitle = _T("Create New Archive...");
	if (cFileDlg.DoModal() != IDOK)
		return FALSE;
	if (!DoSave(cFileDlg.m_ofn.lpstrFile))
		return FALSE;
	BOOL b=TRUE;
	if (ZipPlatform::FileExists(m_strPathName) == 1)
		RemoveFile(m_strPathName);
	if (!OpenZipFile(m_zip,m_strPathName, CZipArchive::zipCreate,0))
		b = false;
		if(b)
		{
		
		MakeTree();
		m_strfilename=m_strPathName;
		this->UpdateAllViews(NULL);

		}

	return b;
}
else
{
	bStart=TRUE;
	return TRUE;
}
}



/////////////////////////////////////////////////////////////////////////////
// CSmartZipDoc serialization

void CSmartZipDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSmartZipDoc diagnostics

#ifdef _DEBUG
void CSmartZipDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSmartZipDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSmartZipDoc commands
void CSmartZipDoc::OnCloseDocument() 
{
CloseCurrentFile();
	CDocument::OnCloseDocument();
}

BOOL CSmartZipDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
return OpenCurrentFile(lpszPathName);
}

BOOL CSmartZipDoc::OpenZipFile(CZipArchive &zip, LPCTSTR lpszZipFileName, long iMode, long iVolumeSize) 
{
	bool bRep = m_bOpenOK = true;

	do 
	{
		TRY
		{
			m_bAfterException = false;
			zip.SetAdvanced(1500000);
			zip.Open(lpszZipFileName, iMode, iVolumeSize);
			CZipPathComponent zpc(lpszZipFileName);
			if (!zpc.GetFileExt().CompareNoCase(_T("jar"))){
				//zip.SetIgnoreCRC();
				zip.SetIgnoredConsistencyChecks();//modified by hwy
			}
			
		}
		CATCH(CException, pEx)
		{
			if (iMode == CZipArchive::zipOpen)
			{
				if (pEx->IsKindOf(RUNTIME_CLASS(CFileException))
					&& ((CFileException*)pEx)->m_cause == CFileException::accessDenied)
				{
					// we may have no write acces, try to open in read-only mode
					iMode = CZipArchive::zipOpenReadOnly;
					continue;
				}
				else if (//pEx->IsKindOf(RUNTIME_CLASS(CZipException)) && 
				((CZipException*)pEx)->m_iCause == CZipException::cdirNotFound &&
				ZipPlatform::IsDriveRemovable(lpszZipFileName))
				{
					if (AfxMessageBox("The central directory was not found in the archive. If you're trying to open a multi-disk archive, make sure you have inserted the last disk. Retry?", MB_YESNO|MB_ICONWARNING) == IDYES)
					{
						
						// we must close here
						zip.Close(CZipArchive::afAfterException);
						continue;
					}
					else 
						bRep = false;
				}
			}
			m_bOpenOK = false;
			if (bRep)
				MessageBox(NULL, "CANN'T OPEN FILE", "FAILED", MB_ICONSTOP);
			break;			
			
		}
		END_CATCH
		break;
	} while (true);

	
	return m_bOpenOK != 0;
}

void CSmartZipDoc::DeleteContents() 
{
//	Close(m_bAfterException);
/*	Close(m_bAfterException);
	if(tree!=NULL)
	delete tree;tree=0;	*/
	CloseCurrentFile();
	CDocument::DeleteContents();
}
void CSmartZipDoc::Close(CZipArchive &zip,BOOL bAfterException) 
{
	
	if (!m_bOpenOK)
		return;
	// should already be flushed or closed (when user says "no" to save)
	ASSERT(!IsModified() || zip.IsClosed());
	TRY
	{
		zip.Close(bAfterException ? CZipArchive::afWriteDir : CZipArchive::afNoException);

	}
	CATCH(CException, pEx)
	{
		zip.Close(CZipArchive::afAfterException);		// this time for sure	
	}
	END_CATCH

}
BOOL CSmartZipDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	if (!m_zip.IsClosed())
	{
	//m_zip.Flush();
	m_zip.FlushBuffers();//modified by hwy
//	SetModifiedFlag (FALSE);
	}
	return TRUE;
}

void CSmartZipDoc::AddSubFolders(CZipWordArray *arr, CTreeNode *hold)
{
	if(hold->zip_index!=-1)
	arr->Add (hold->zip_index);
	POSITION pos=hold->fIndex.GetStartPosition ();
	while(pos)
	{
		CString s;CObject*pOpj;
		hold->fIndex.GetNextAssoc (pos,s,pOpj);
		arr->Add (((INDEX*)pOpj)->n );
	}
	int m=hold->Childrens.GetSize ();
	for(int i=0;i<m;i++)
	{
		AddSubFolders(arr,(CTreeNode*)hold->Childrens[i]);
	}
}

BOOL CSmartZipDoc::SaveModified() 
{
	// TODO: Add your specialized code here and/or call the base class
	return OnSaveDocument(NULL);
//	return CDocument::SaveModified();
}
bool CSmartZipDoc::RemoveFile(LPCTSTR szFile, bool bDisplay)
{
	CString sz = szFile;
	if (sz.IsEmpty())
		return false;
	TRY
	{
		ZipPlatform::RemoveFile(sz);
	}
	CATCH(CException, pEx)
	{
		if (bDisplay)
		{
		
			AfxMessageBox("Failed to delete the document!!", MB_ICONSTOP);
		}
		return false;	
	}
	END_CATCH
	return true;
}
void CSmartZipDoc::CloseCurrentFile()
{
	SaveModified();
Close(m_zip,m_bAfterException);
		if(tree!=NULL)
	delete tree;tree=0;currentnode=0;	
}
BOOL CSmartZipDoc::OpenCurrentFile(LPCTSTR lpszPathName)
{
	bStart=TRUE;
	m_strfilename=lpszPathName;
	BOOL b=OpenZipFile(m_zip,lpszPathName, CZipArchive::zipOpen,0);
	if(b)
	{
		tree=new CTreeNode();
		MakeTree();	this->UpdateAllViews (NULL,0,0);

	}
	 return b;
}

void CSmartZipDoc::ParseStringToobj(CString &s, BOOL pFolder, int findex)
{
	if(pFolder)	if(s.Right (1)!='\\') s+='\\';
	CString temp;
	int from=0,to=0;
	BOOL pExist=FALSE;
	CTreeNode *c=tree ;
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

void CSmartZipDoc::MakeTree()
{
if(tree)
{delete tree;}
tree=new CTreeNode();
int m=	m_zip.GetCount();
			for(int i=0;i<m;i++)
			{
				CZipFileHeader fh;
				m_zip.GetFileInfo(fh,i);
				CString c((LPCTSTR)fh.GetFileName ());
				ParseStringToobj(c,fh.IsDirectory (),i);
			}
}