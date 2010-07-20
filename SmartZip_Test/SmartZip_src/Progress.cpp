// Progress.cpp : implementation file
//

#include "stdafx.h"
#include "smartzip.h"
#include "Progress.h"
#include "ZipPlatform.h"

#include "veripass.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDProgress dialog


CDProgress::CDProgress(CWnd* pParent /*=NULL*/)
	: CDialog(CDProgress::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDProgress)
	//}}AFX_DATA_INIT
	m_hThread = NULL;
    m_bWorking = FALSE;
	ptp=NULL;//m_nPercentDone=-1;
}


void CDProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDProgress)
	DDX_Control(pDX, IDC_FOLDER, m_dir);
	DDX_Control(pDX, IDC_FILE, m_filename);
	DDX_Control(pDX, IDC_CANCEL, m_cancel);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Control(pDX, IDC_VEDIO, m_animate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDProgress, CDialog)
	//{{AFX_MSG_MAP(CDProgress)
	ON_BN_CLICKED(IDC_CANCEL, OnStopClose)
	//}}AFX_MSG_MAP
	ON_MESSAGE (WM_USER_THREAD_UPDATE, OnThreadUpdate)
	ON_MESSAGE (WM_USER_THREAD_FINISHED, OnThreadFinished)
	ON_MESSAGE (WM_USER_THREAD_ABORTED, OnThreadAborted)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDProgress message handlers

int CDProgress::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialog::DoModal();
}

LRESULT CDProgress::OnThreadUpdate (WPARAM wParam, LPARAM lParam)
{
	int iValue = ((int) wParam);
	if((int)lParam==-1)
	{
	m_progress.SetRange(0, iValue);
	m_progress.SetPos(0);
	tasktimer.start ();
	}
	else
	{
	//	int l,p;
	//	m_progress.GetRange(l,p);
    //	CString c;c.Format ("%d  |%d |%d",iValue,l,p);
	//	m_estimate.SetWindowText(c);
		tasktimer.updateProgress (iValue);
		CStatic * s=(CStatic *)GetDlgItem(IDC_ELAPSED);
		CStatic * s1=(CStatic *)GetDlgItem(IDC_REMAINING);
		CString str1;
		tasktimer.getElapsedTime(str1);
		s->SetWindowText (str1);
		tasktimer.getRemainingTime(str1);
		s1->SetWindowText (str1);
		m_progress.SetPos(iValue);
    }
    return 0;
}

LRESULT CDProgress::OnThreadFinished (WPARAM wParam, LPARAM lParam)
{
		m_pDoc->m_zip.SetCallback(NULL, CZipArchive::cbAll);
		m_cancel.SetWindowText("Close");
	m_dir.SetWindowText("");
		m_filename.SetWindowText("");
	m_bWorking=FALSE;
tasktimer.stop ();
//		m_estimate.SetWindowText("Finished Successful");
if(wParam==-1)
{
	m_animate.Stop ();
AfxMessageBox ("Operation Canceled");
}
else
{
	AfxMessageBox ("Operation done Successful,No errors");
	CDialog::OnOK ();
}
	return 0;
	/*
	m_cancel.SetWindowText("Close");
	m_dir.SetWindowText("");
		m_filename.SetWindowText("");
	m_bWorking=FALSE;
	if(m_check.GetCheck())
		CDialog::OnOK ();
	if((int)wParam==-1)
	{
	//	m_estimate.SetWindowText("Canceled by user");
	//	AfxMessageBox ("Aborted");
		CDialog::OnOK ();
		
	}
	else
	{
		m_estimate.SetWindowText("Finished Successful");
		AfxMessageBox ("Successful");
	}
    
	return 0;*/
}

LRESULT CDProgress::OnThreadAborted (WPARAM wParam, LPARAM lParam)
{
	//AfxMessageBox ("Aborted");
	m_bWorking=FALSE;
	
    return 0;
}

BOOL CDProgress::OnInitDialog() 
{
	CDialog::OnInitDialog();
//	m_progress.SetRange( 0, 100 );
	SetWindowText(m_text);
	// TODO: Add extra initialization here
	m_bWorking = TRUE;
    m_event.ResetEvent ();
	////
	THREADPARMS* ptp = new THREADPARMS;
	ptp->pDoc =m_pDoc;
	ptp->pWnd = this;
	ptp->pCriticalSection = &m_cs;
	ptp->pEvent = &m_event;
	ptp->pReserved =pReserved;
	ptp->pFull =pFull;
	ptp->bOverwrite=bOverwrite;
	ptp->zip =zip;
	ptp->bEnc =bEnc;
//	m_progressdata.pWnd=ptp->pWnd;
	m_progressdata=new CProgressData(this);
	m_pDoc->m_zip.SetCallback(m_progressdata, CZipArchive::cbAll);
	////
	if(m_operation=="t")
	{
		m_animate.Open (IDR_AVITEST);
		m_animate.Play (0,-1,0);
	CWinThread* pThread = AfxBeginThread (Test, ptp,
		THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	
	::DuplicateHandle (GetCurrentProcess (),
		pThread->m_hThread, GetCurrentProcess (), &m_hThread,
		0, FALSE, DUPLICATE_SAME_ACCESS);
	
	pThread->ResumeThread ();
	}
	else if(m_operation=="e")
	{
		m_animate.Open (IDR_AVIEXTRACT);
		m_animate.Play (0,-1,0);
		CWinThread* pThread = AfxBeginThread (Extract, ptp,
		THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	
	::DuplicateHandle (GetCurrentProcess (),
		pThread->m_hThread, GetCurrentProcess (), &m_hThread,
		0, FALSE, DUPLICATE_SAME_ACCESS);
	
	pThread->ResumeThread ();
	}
	else if(m_operation=="d")
	{
		m_animate.Open (IDR_AVIDELETE);
		m_animate.Play (0,-1,0);
		CWinThread* pThread = AfxBeginThread (Delete, ptp,
		THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	
	::DuplicateHandle (GetCurrentProcess (),
		pThread->m_hThread, GetCurrentProcess (), &m_hThread,
		0, FALSE, DUPLICATE_SAME_ACCESS);
	
	pThread->ResumeThread ();
	}
	else if(m_operation=="a")
	{
		m_animate.Open (IDR_AVIEXTRACT);
		m_animate.Play (0,-1,0);
		CWinThread* pThread = AfxBeginThread (Add, ptp,
		THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	
	::DuplicateHandle (GetCurrentProcess (),
		pThread->m_hThread, GetCurrentProcess (), &m_hThread,
		0, FALSE, DUPLICATE_SAME_ACCESS);
	
	pThread->ResumeThread ();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
UINT Test (LPVOID pParam)
{
	THREADPARMS* ptp = (THREADPARMS*) pParam;
    CSmartZipDoc *pDoc=(CSmartZipDoc *)ptp->pDoc;
	CDProgress* pWnd = (CDProgress*)ptp->pWnd;
    CCriticalSection* pCriticalSection = ptp->pCriticalSection;
    CEvent* pKillEvent = ptp->pEvent;
	delete ptp;
	CZipArchive& zip=pDoc->m_zip;
	::Sleep (100);
	double iMax = 0;
	int m=zip.GetCount();
	for (int j = 0; j < m; j++)
	{
		CZipFileHeader fh;
		zip.GetFileInfo(fh,j);
		iMax += fh.m_uUncomprSize;
	}
	pWnd->m_progressdata->begin(iMax);
	for(int i=0;i<m;i++)
	{
		if (::WaitForSingleObject (pKillEvent->m_hObject, 0) ==WAIT_OBJECT_0) 
		{
			pWnd->PostMessage (WM_USER_THREAD_FINISHED, -1, 0);
			return (UINT) -1;
		}
		CZipFileHeader fh;
		zip.GetFileInfo(fh,i);
		if(!fh.IsDirectory())
		{
			int to;
			CString file,dir,temp(fh.GetFileName());
			to=temp.ReverseFind ('\\')+1;
			file=temp.Mid (to,temp.GetLength ()-to);
			dir=temp.Mid (0,to);
			pWnd->m_dir.SetWindowText("Directory \'" + dir+ "\'");
			pWnd->m_filename.SetWindowText("Checking \'" + file + "\'");

		TRY
		{
			zip.TestFile(i);
		}
		CATCH(CException, pEx)
		{			
			pEx->Delete ();
		}
		END_CATCH
		}
	}
	pWnd->PostMessage (WM_USER_THREAD_FINISHED, 0, 0);
    return 0;
}

void CDProgress::OnStopClose() 
{
	if(m_bWorking)
	{
		m_event.SetEvent ();
	}
	else
		CDialog::OnOK ();
}
UINT Extract (LPVOID pParam)
{
	THREADPARMS* ptp = (THREADPARMS*) pParam;
    CSmartZipDoc *pDoc=(CSmartZipDoc *)ptp->pDoc;
	CDProgress *pWnd = (CDProgress*)ptp->pWnd;
    CCriticalSection* pCriticalSection = ptp->pCriticalSection;
    CEvent *pKillEvent = ptp->pEvent;
	CZipWordArray * pArray=(CZipWordArray *)ptp->pReserved;
	BOOL useFullpath=ptp->pFull;
	BOOL bOverwrite=ptp->bOverwrite;
	delete ptp;
	CZipArchive& zip=pDoc->m_zip;
	::Sleep (100);
	double iMax = 0;
	int m=pArray->GetSize();
	for (int j = 0; j < m; j++)
	{
			CZipFileHeader fh;
			zip.GetFileInfo(fh,pArray->GetAt (j));
			if(fh.IsDirectory())
				continue;
			iMax += fh.m_uUncomprSize;
	}
	pWnd->m_progressdata->begin(iMax);
	for(int i=0;i<m;i++)
	{
		if (::WaitForSingleObject (pKillEvent->m_hObject, 0) ==WAIT_OBJECT_0) 
		{
			pWnd->PostMessage (WM_USER_THREAD_FINISHED, -1, 0);
			return (UINT) -1;
		}
		CZipFileHeader fh;
		zip.GetFileInfo(fh,pArray->GetAt (i));
		int to;
		CString file,dir,temp(fh.GetFileName());
		to=temp.ReverseFind ('\\')+1;
		file=temp.Mid (to,temp.GetLength ()-to);
		dir=temp.Mid (0,to);
		pWnd->m_dir.SetWindowText("Directory \'" + dir+ "\'");
		pWnd->m_filename.SetWindowText("Extracting \'" + file + "\'");	
		TRY
		{
			if(!fh.IsDirectory ())
			{
				if(fh.IsEncrypted ())
				{
					CVeriPass dlg;
					dlg.m_filename=file;
					if(dlg.DoModal ()==IDOK)
					{
						zip.SetPassword (dlg.m_pass );
					}
					else continue;
				}
			}
			if (!bOverwrite)
			{
				
				CString szPredicted = zip.PredictExtractedFileName(fh.GetFileName(), pWnd->m_extractpath, useFullpath != 0);
				if (ZipPlatform::FileExists(szPredicted))
				{
					continue;
				}
			}
				BOOL success=zip.ExtractFile(pArray->GetAt (i),pWnd->m_extractpath,useFullpath);
				if(!success)
				{
					CString gg;gg.Format (fh.GetFileName()+" %d",pArray->GetAt (i));
					AfxMessageBox (gg);
				}
				zip.SetPassword ();
		}
		CATCH(CException, pEx)
		{			
			pEx->Delete ();
		}
		END_CATCH
	}
	pWnd->PostMessage (WM_USER_THREAD_FINISHED, 0, 0);
    return 0;
}
UINT Delete (LPVOID pParam)
{
	THREADPARMS* ptp = (THREADPARMS*) pParam;
    CSmartZipDoc *pDoc=(CSmartZipDoc *)ptp->pDoc;
	CDProgress *pWnd = (CDProgress*)ptp->pWnd;
    CCriticalSection* pCriticalSection = ptp->pCriticalSection;
    CEvent *pKillEvent = ptp->pEvent;
	CZipWordArray * pArray=(CZipWordArray *)ptp->pReserved;
	delete ptp;
	CZipArchive& zip=pDoc->m_zip;
	::Sleep (100);
	double iMax = 0;
	int m=pArray->GetSize();
//	pDoc->SetModifiedFlag ();
		if (::WaitForSingleObject (pKillEvent->m_hObject, 0) ==WAIT_OBJECT_0) 
		{
			pWnd->PostMessage (WM_USER_THREAD_FINISHED, -1, 0);
			return (UINT) -1;
		}
		  TRY
			{
					zip.DeleteFiles (*pArray);
			}
			CATCH(CException, pEx)
			{			
				pEx->Delete ();
			}
			END_CATCH
	delete pArray;
	pWnd->PostMessage (WM_USER_THREAD_FINISHED, 0, 0);
    return 0;
}

BOOL CDProgress::AddFolder(CString szFolder, CObArray &array)
{
	szFolder.TrimRight(_T("\\"));
	CFileFindEnh ff;
	BOOL b = ff.FindFile(szFolder + _T("\\*"));
	CObArray folders;
	CObArray files;
	while (b)
	{
		b = ff.FindNextFile();
		if (ff.IsDots())
			continue;
		if (ff.IsDirectory())
			folders.Add(new CAddInfo(ff.GetFilePath(), 0, true));
		else
			files.Add(new CAddInfo(ff.GetFilePath(), ff.GetInfo()->nFileSizeLow));
	}
//	files.SetSortFunction(SortInfo, NULL);
//	folders.SetSortFunction(SortInfo, NULL);
//	files.Sort();
//	files.m_bDestroyOnDelete = false;	
	array.Append(files);
//	folders.Sort();
//	bool bRecursive = data.GetDoc()->m_options.m_bRecursive != 0;
	for (int i = 0; i < folders.GetSize(); i++)
	{
		CAddInfo* pInfo = (CAddInfo*)folders[i];
		array.Add(pInfo); // add the folder before adding its files											// it is not needed to add the root folder
		folders[i] = NULL;
		if (!AddFolder(pInfo->m_szName, array))
			return false;
	}
	return true;
}
UINT Add (LPVOID pParam)
{
	THREADPARMS* ptp = (THREADPARMS*) pParam;
    CSmartZipDoc *pDoc=(CSmartZipDoc *)ptp->pDoc;
	CDProgress *pWnd = (CDProgress*)ptp->pWnd;
    CCriticalSection* pCriticalSection = ptp->pCriticalSection;
    CEvent *pKillEvent = ptp->pEvent;
	CStringArray * pArray=(CStringArray *)ptp->pReserved;
	BOOL bFullPath=ptp->pFull;
	int iLevel=ptp->bOverwrite;
BOOL bEnc=ptp->bEnc ;
	CZipArchive& zip=*(ptp->zip);
	delete ptp;
	/////////////////////////////////////////
	CObArray array;
	CObArray folders;
	CObArray files;
	bool bContinue = true;
	int iSize =pArray->GetSize ();
	for (int i = 1; i < iSize; i++) // at 0 is the root path
	{
		CString sz = (*pArray)[i];
		CFileStatus fs;
		if (!CFile::GetStatus(sz, fs))
			continue;
		CAddInfo* ai = new CAddInfo(sz, fs.m_size, (fs.m_attribute & FILE_ATTRIBUTE_DIRECTORY) != 0);
		if (ai->m_bDir)
		{
			ai->m_iSize = 0;
			folders.Add(ai);
		}
		else
			files.Add(ai);
	}
	array.Append(files);	
	for (i = 0; i < folders.GetSize(); i++)
	{
		CAddInfo* pInfo =(CAddInfo*) folders[i];
		array.Add(pInfo); // add the folder before adding its files											// it is not needed to add the root folder
		folders[i] = NULL;
		bContinue = pWnd->AddFolder(pInfo->m_szName, array);
		if (!bContinue)
			break;
	}
	zip.SetRootPath(pArray->GetAt(0));
	delete pArray;
	if (!bContinue)
	{
		AfxMessageBox ("Fatal Error Cannot add files/folders");
		pWnd->PostMessage (WM_USER_THREAD_FINISHED, 0, 0);
		return 0;
	}
	iSize = array.GetSize();
	DWORD iMax = 0;
	for (i = 0; i < iSize; i++)
		iMax += ((CAddInfo*)array[i])->m_iSize;
	pWnd->m_progressdata->begin(iMax);

	for (i = 0; i < iSize; i++)
	{
		if (::WaitForSingleObject (pKillEvent->m_hObject, 0) ==WAIT_OBJECT_0) 
		{
			pWnd->PostMessage (WM_USER_THREAD_FINISHED, -1, 0);
			return (UINT) -1;
		}
		CAddInfo* ai = (CAddInfo*)array[i];

		int to;
		CString file,dir,temp(ai->m_szName);
		to=temp.ReverseFind ('\\')+1;
		file=temp.Mid (to,temp.GetLength ()-to);
		dir=temp.Mid (0,to);
		pWnd->m_dir.SetWindowText("Directory \'" + dir+ "\'");
		pWnd->m_filename.SetWindowText("Adding \'" + file + "\'");	

		bool bBreak = false;
		int iSmart=CZipArchive::zipsmCPassDir |CZipArchive::zipsmCPFile0 |CZipArchive::zipsmSmartAdd ;
		TRY
		{
			if(bEnc)
			{
				CVeriPass dlg;
				dlg.m_filename=file;
				if(dlg.DoModal ()==IDOK)
				{
					zip.SetPassword (dlg.m_pass );
				}
				else continue;
			}
			zip.AddNewFile(ai->m_szName, 
				iLevel, bFullPath, // we have a root path set, so set it to false
				iSmart);
			zip.SetPassword ();
		}
		CATCH(CException, pEx)
		{
			zip.CloseNewFile(true); // allow reusing the archive
			bBreak = true; // do not continue if an exception was thrown
			AfxMessageBox ("No Enough space");
		}
		END_CATCH
			if (bBreak)
				break;
	}
	
	for(i=0;i<iSize;i++)
	{
		delete (CAddInfo*)array[i];
	}
	pWnd->PostMessage (WM_USER_THREAD_FINISHED, 0, 0);
    return 0;
	
}