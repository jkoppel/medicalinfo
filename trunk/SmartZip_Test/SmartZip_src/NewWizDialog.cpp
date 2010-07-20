// NewWizDialog.cpp : implementation file
//

#include "stdafx.h"
#include "NewWizDialog.h"
#include "NewWizPage.h"
#include "resource.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CNewWizDialog dialog



/*CNewWizDialog::CNewWizDialog(LPCTSTR lpszTemplateName, 
   CWnd* pParent):CDialog(lpszTemplateName,pParent)
{
  Init();
}

CNewWizDialog::CNewWizDialog(UINT nIDTemplate, 
	CWnd* pParent):CDialog(nIDTemplate,pParent)
{
  Init();
}*/
CNewWizDialog::CNewWizDialog(CWnd* pParent /*=NULL*/)
	:CDialog("",pParent)
{
	Init();
}
CNewWizDialog::~CNewWizDialog()
{
	if (m_bIsFontCreated) {
		delete m_pFont;
	}

}



void CNewWizDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewWizDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewWizDialog, CDialog)
	//{{AFX_MSG_MAP(CNewWizDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(ID_WIZFINISH, OnWizardFinish)
	ON_BN_CLICKED(ID_WIZBACK, OnWizardBack)
	ON_BN_CLICKED(ID_WIZNEXT, OnWizardNext)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewWizDialog message handlers


void CNewWizDialog::Init()
{
//  m_nPlaceholderID = 0;
  	m_pFont=0;//pointer
	m_bIsFontCreated = FALSE;
	m_wFontSize=0;
	m_DialogTemplate.style				=	WS_POPUP| WS_VISIBLE | DS_SETFONT;
	m_DialogTemplate.style |=WS_CAPTION |DS_MODALFRAME|WS_SYSMENU;   
	m_DialogTemplate.dwExtendedStyle = WS_EX_DLGMODALFRAME;
	m_DialogTemplate.x					= 0;
	m_DialogTemplate.y					= 0;
	m_DialogTemplate.cx					= 0; // 4 horizontal units are the width of one character
	m_DialogTemplate.cy					= 0; // 8 vertical units are the height of one character
	m_DialogTemplate.cdit				= 0;  // nr of dialog items in the dialog

}


BOOL CNewWizDialog::DestroyPage(CNewWizPage* pPage)
{
	if (pPage->m_bCreated)
	{
		if (pPage->OnKillActive() == FALSE) return FALSE;
		pPage->OnDestroyPage();
		pPage->DestroyWindow();
		pPage->m_bCreated = FALSE;
	}

	// return TRUE even if the page was never created
	return TRUE;
}



CNewWizPage* CNewWizDialog::GetFirstPage()
{
	return (CNewWizPage*)m_PageList.GetHead();	
}


CNewWizPage* CNewWizDialog::GetLastPage()
{
	return (CNewWizPage*)m_PageList.GetTail();	
}


CNewWizPage* CNewWizDialog::GetActivePage() const
{
	CNewWizPage* pPage;
	POSITION Pos = m_PageList.GetHeadPosition();
	while (Pos)
	{
		pPage = (CNewWizPage*)m_PageList.GetNext(Pos);
		if (pPage->m_bActive)
		{
			return pPage;
		}
	}
	return NULL;
}


// function to get the next page
CNewWizPage* CNewWizDialog::GetNextPage()
{
	CNewWizPage* pPage;
	POSITION Pos = m_PageList.GetHeadPosition();
	while (Pos)
	{
		pPage = (CNewWizPage*)m_PageList.GetNext(Pos);
		if (pPage->m_bActive)
		{
			if (Pos == NULL) return NULL;
			return (CNewWizPage*)m_PageList.GetAt(Pos);
		}
	}
	return NULL;
}

// returns TRUE if the new page is activated
BOOL CNewWizDialog::ActivatePage(CNewWizPage* pPage)
{
//	ASSERT(m_nPlaceholderID != 0);
  ASSERT(pPage != NULL);
	ASSERT(::IsWindow(m_hWnd));

	// if the page has not been created, then create it
	if (pPage->m_bCreated == FALSE)
	{
		if (pPage->Create(pPage->m_nDialogID, this) == FALSE) return FALSE;
		pPage->m_bCreated = TRUE;
		pPage->m_pParent = this;

	  if (pPage->OnCreatePage() == FALSE) return FALSE;
	}

	// deactivate the current page
	if (!DeactivatePage()) return FALSE;

  CRect rect;
 /* CWnd *pWnd = GetDlgItem(m_nPlaceholderID);
  ASSERT(pWnd != NULL);
  ASSERT(IsWindow(pWnd->m_hWnd) != FALSE);

  pWnd->GetWindowRect(&rect);
  ScreenToClient(&rect);
  rect=CRect(0,0,10,20);*/
  pPage->SetWindowPos(NULL,0, 100, 0, 0, 
                       SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE );
  pPage->EnableWindow(TRUE);

  pPage->ShowWindow(SW_SHOW);
  pPage->InvalidateRect(NULL);
  pPage->UpdateWindow();
  pPage->OnSetActive();
  pPage->m_bActive = TRUE;
	return TRUE;
}


BOOL CNewWizDialog::DeactivatePage()
{
	CNewWizPage* pPage = GetActivePage();
	if (pPage == NULL) return TRUE;

  ASSERT(pPage->m_bCreated != FALSE);
  if (!pPage->OnKillActive()) return FALSE;
  pPage->ShowWindow(SW_HIDE);
  pPage->m_bActive = FALSE;
	return TRUE;
}


BOOL CNewWizDialog::OnInitDialog() 
{
	if (m_pFont == NULL) {
		LOGFONT LogFont;

		// Can do better???
		memset(&LogFont, 0x00, sizeof(LogFont));
		strcpy(LogFont.lfFaceName, _T("MS Sans Serif"));
		LogFont.lfHeight = 8;

		m_pFont = new CFont;
		m_pFont->CreateFontIndirect(&LogFont);
		SetFont(m_pFont);
		m_bIsFontCreated = TRUE;
	}
	CDialog::OnInitDialog();
  ModifyStyleEx (0, WS_EX_CONTROLPARENT);	

//	ASSERT(m_nPlaceholderID != 0); // Be sure to call SetPlaceholderID from
						// your dialogs OnInitDialog

 // for (int i=0 ;i<4;i++)
//{

	btn[0].Create (NULL, "Finish",WS_VISIBLE|WS_CHILD ,CRect(0,0,0,0),this,ID_WIZFINISH);
	btn[1].Create (NULL,"Cancel",WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,IDCANCEL);
	btn[2].Create (NULL,"Next",WS_VISIBLE|WS_CHILD ,CRect(0,0,0,0),this,ID_WIZNEXT);
	btn[3].Create (NULL,"back",WS_VISIBLE|WS_CHILD  ,CRect(0,0,0,0),this,ID_WIZBACK);
//}
COLORREF	crBtnColor = ::GetSysColor(COLOR_BTNFACE) + RGB(30, 30, 30);
//BACK
	btn[3].SetIcon(IDI_BACKHOT,IDI_BACK);
	btn[3].SetColor(CButtonST::BTNST_COLOR_BK_IN, crBtnColor);
	btn[3].SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 128, 0));
	// Next button
	btn[2].SetIcon(IDI_NEXTHOT,IDI_NEXT);
	btn[2].SetColor(CButtonST::BTNST_COLOR_BK_IN, crBtnColor);
	btn[2].SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 128, 0));
	btn[2].SetAlign(CButtonST::ST_ALIGN_HORIZ_RIGHT);
//CANCEL
	btn[1].SetIcon(IDI_CLOSEHOT,IDI_CLOSE);
	btn[1].SetColor(CButtonST::BTNST_COLOR_BK_IN, crBtnColor);
	btn[1].SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 128, 0));
//	btn[1].SetAlign(CButtonST::ST_ALIGN_HORIZ_RIGHT);
//CANCEL
	btn[0].SetIcon(IDI_FINISHHOT,IDI_FINISH);
	btn[0].SetColor(CButtonST::BTNST_COLOR_BK_IN, crBtnColor);
	btn[0].SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 128, 0));
	btn[0].SetAlign(CButtonST::ST_ALIGN_HORIZ_RIGHT);

	CRect rect,rect1;
  this->GetWindowRect (rect);
	this->MoveWindow (rect.left ,rect.top ,450,500);
  
 SizeButtons(450,485);
	// make the first page of the wizard active
	SetFirstPage();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CNewWizDialog::OnDestroy() 
{
	CNewWizPage* pPage;
	POSITION Pos = m_PageList.GetHeadPosition();
	while (Pos)
	{
		pPage = (CNewWizPage*)m_PageList.GetNext(Pos);

		// this function could return FALSE, but if it happens here
		// it is too late to do anything about it!
		VERIFY(DestroyPage(pPage));
	}

  CDialog::OnDestroy();
}


// function to add a page to the the list of pages
// nID is the reource ID of the page we are adding
void CNewWizDialog::AddPage(CNewWizPage* pPage, UINT nID)
{
	m_PageList.AddTail(pPage);
	pPage->m_nDialogID = nID;
}



// Activate the page with the specified dialog resource
void CNewWizDialog::SetActivePageByResource(UINT nResourceID)
{
	CNewWizPage* pPage = GetPageByResourceID(nResourceID);
	if (pPage == NULL) return;

	if (!DeactivatePage()) return;

	ActivatePage(pPage);
}


// function to return a page object based on it's dialog resource ID
CNewWizPage* CNewWizDialog::GetPageByResourceID(UINT nResourceID)
{
	CNewWizPage* pPage;
	POSITION Pos = m_PageList.GetHeadPosition();
	while (Pos)
	{
		pPage = (CNewWizPage*)m_PageList.GetNext(Pos);
		if (pPage->m_nDialogID == nResourceID)
		{
			return pPage;
		}
	}
	return NULL;
}


// Set first page as active page
BOOL CNewWizDialog::SetFirstPage()
{
  CNewWizPage* pPage = GetFirstPage();

	if (!DeactivatePage()) return FALSE;

	EnableBack(FALSE);
	
	if (m_PageList.GetCount() > 1)
	{
		EnableFinish(FALSE);
		EnableNext(TRUE);
	}
	else // wizard has only one page
	{
		EnableFinish(TRUE);
		EnableNext(FALSE);
	}
SizeButtons(450,485);
	if (ActivatePage(pPage)) return TRUE;
	return FALSE;
}


// Set next page as active page
void CNewWizDialog::SetNextPage()
{
  CNewWizPage* pPage = GetNextPage();
	if (ActivatePage(pPage))
	{
		EnableBack(TRUE);
	}
}


/////////////////////////////////////////////////////////////
//  CNewWizDlg Button Processing


// user pressed the Finish button
void CNewWizDialog::OnWizardFinish() 
{
	CNewWizPage* pPage;

	pPage = GetActivePage();

	// can we kill the active page?
	if (!pPage->OnKillActive()) return;

	
	// notify all pages that we are finishing
	POSITION Pos = m_PageList.GetHeadPosition();
	while (Pos)
	{
		pPage = (CNewWizPage*)m_PageList.GetNext(Pos);
		if (pPage->m_bCreated)
		{
			if (!pPage->OnWizardFinish())
			{
				// data validation failed for one of the pages so we can't close
				return;
			}
		}
	}

	// The only reason this line would be needed is if you had controls
	// place in your main wizard dialog outside of the wizard pages.
	// In most "normal" implementations, this line does nothing
	UpdateData(TRUE);

	// close the dialog and return ID_WIZFINISH
	CDialog::EndDialog(ID_WIZFINISH);
}




void CNewWizDialog::OnWizardBack() 
{
	CNewWizPage* pPage = GetActivePage();
	ASSERT(pPage);

	LRESULT lResult = pPage->OnWizardBack();
	if (lResult == -1) return; // don't change pages

	if (lResult == 0)
	{
		POSITION Pos = m_PageList.Find(pPage);
		ASSERT(Pos);
		m_PageList.GetPrev(Pos);
		if (Pos == NULL) return; // the current page was the first page
		pPage = (CNewWizPage*)m_PageList.GetAt(Pos);
	}
	else
	{
		pPage = GetPageByResourceID(lResult);
		if (pPage == NULL) return;
	}

	if (!ActivatePage(pPage)) return;	

	// if we are on the first page, then disable the back button
	if (pPage == GetFirstPage())
	{
		EnableBack(FALSE);
		EnableFinish(FALSE);
	}

	// enable the next button
	EnableNext(TRUE);
	SizeButtons(450,485);
}


void CNewWizDialog::OnWizardNext() 
{
	CNewWizPage* pPage = GetActivePage();
	ASSERT(pPage);

	LRESULT lResult = pPage->OnWizardNext();
	if (lResult == -1) return; // don't change pages

	if (lResult == 0)
	{
		POSITION Pos = m_PageList.Find(pPage);
		ASSERT(Pos);
		m_PageList.GetNext(Pos);
		if (Pos == NULL) return; // the current page was the last page
		pPage = (CNewWizPage*)m_PageList.GetAt(Pos);
	}
	else
	{
		pPage = GetPageByResourceID(lResult);
		if (pPage == NULL) return;
	}

	if (!ActivatePage(pPage)) return;	

	// if we are on the last page, then disable the next button
	if (pPage == GetLastPage())
	{
		EnableNext(FALSE);
		EnableFinish(TRUE);
	}
	EnableBack(TRUE);
	SizeButtons(450,485);
}


void CNewWizDialog::EnableFinish(BOOL bEnable)
{
	ASSERT(::IsWindow(m_hWnd));
	CWnd* pWnd = GetDlgItem(ID_WIZFINISH);
	ASSERT(pWnd); // You must have an ID_WIZFINISH on your dialog
	if (pWnd)
	{
		pWnd->EnableWindow(bEnable);		
	}
}


void CNewWizDialog::EnableBack(BOOL bEnable)
{
	ASSERT(::IsWindow(m_hWnd));
	CWnd* pWnd = GetDlgItem(ID_WIZBACK);
	ASSERT(pWnd); // You must have an ID_WIZBACK on your dialog
	if (pWnd)
	{
		pWnd->EnableWindow(bEnable);		
	}
}


void CNewWizDialog::EnableNext(BOOL bEnable)
{
	ASSERT(::IsWindow(m_hWnd));
	CWnd* pWnd = GetDlgItem(ID_WIZNEXT);
	ASSERT(pWnd); // You must have an ID_WIZNEXT on your dialog
	if (pWnd)
	{
		pWnd->EnableWindow(bEnable);		
	}
}


///////////////////////////////////////////////////////////////
//  Functions to mimic the behavior of CPropertySheet


// returns the index of the currently active page
int CNewWizDialog::GetActiveIndex() const
{
	CNewWizPage* pPage;
	POSITION Pos = m_PageList.GetHeadPosition();
	int nIndex = 0;
	while (Pos)
	{
		pPage = (CNewWizPage*)m_PageList.GetNext(Pos);
		if (pPage->m_bActive)
		{
			return nIndex;
		}
		++nIndex;
	}
	return -1;
}


int CNewWizDialog::GetPageIndex(CNewWizPage* pPage) const
{
	CNewWizPage* pTestPage;
	POSITION Pos = m_PageList.GetHeadPosition();
	int nIndex = 0;
	while (Pos)
	{
		pTestPage = (CNewWizPage*)m_PageList.GetNext(Pos);
		if (pTestPage == pPage)
		{
			return nIndex;
		}
		++nIndex;
	}
	return -1;
}


int CNewWizDialog::GetPageCount()
{
	return m_PageList.GetCount();
}


// get a page based on it's placement index in the list
CNewWizPage* CNewWizDialog::GetPage(int nPage) const
{
	POSITION Pos = m_PageList.FindIndex(nPage);
	if (Pos == NULL) return NULL;
	return (CNewWizPage*)m_PageList.GetAt(Pos);
}



// activate a page based on its place in the list
BOOL CNewWizDialog::SetActivePage(int nPage)
{
	CNewWizPage* pPage = GetPage(nPage);
	if (pPage == NULL) return FALSE;
	ActivatePage(pPage);
	return TRUE;
}


BOOL CNewWizDialog::SetActivePage(CNewWizPage* pPage)
{
	ActivatePage(pPage);
	return TRUE;
}


// set the title of the main wizard window
void CNewWizDialog::SetTitle(LPCTSTR lpszText)
{
	ASSERT(::IsWindow(m_hWnd));
	SetWindowText(lpszText);
}


void CNewWizDialog::SetTitle(UINT nIDText)
{
	CString s;
	s.LoadString(nIDText);
	SetTitle(s);
}


// set the text on the Finish button
void CNewWizDialog::SetFinishText(LPCTSTR lpszText)
{
	ASSERT(::IsWindow(m_hWnd));
	CWnd* pWnd = GetDlgItem(ID_WIZFINISH);
	ASSERT(pWnd); // You must have an ID_WIZFINISH on your dialog
	if (pWnd)
	{
		pWnd->SetWindowText(lpszText);		
	}
}

void CNewWizDialog::SetFinishText(UINT nIDText)
{
	CString s;
	s.LoadString(nIDText);
	SetFinishText(s);	
}


// user pressed the cancel button
void CNewWizDialog::OnCancel()
{
	CNewWizPage* pPage;
	POSITION Pos = m_PageList.GetHeadPosition();
	while (Pos)
	{
		pPage = (CNewWizPage*)m_PageList.GetNext(Pos);
		if (pPage->m_bCreated)
		{
			// can we cancel?
			if (pPage->OnQueryCancel() == FALSE) return;
		}
	}

	CDialog::OnCancel();
}
int CNewWizDialog::DoModal() 
{
/*	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DoModal();*/
	m_lpszTemplateName=NULL;
		CFont* pParentFont = m_pFont;
	if (pParentFont == NULL && m_pParentWnd != NULL) {
		pParentFont = m_pParentWnd->GetFont();
	}
	if (pParentFont == NULL && AfxGetApp()->m_pActiveWnd != NULL) {
		pParentFont = AfxGetApp()->m_pActiveWnd->GetFont();
	}
	LOGFONT LogFont;
	memset(&LogFont, 0x00, sizeof(LogFont));
	if (pParentFont != NULL) {
		pParentFont->GetLogFont(&LogFont);
	}
	else {
		// Can do better???
		strcpy(LogFont.lfFaceName, _T("MS Sans Serif"));
		LogFont.lfHeight = 8;
	}

	//Prework for setting font in dialog...
	int nFontNameLen = strlen(LogFont.lfFaceName) + 1;
	WCHAR *szFontName = new WCHAR[nFontNameLen];
	int ii = 0;
	const char *pp = LogFont.lfFaceName;
	while (*pp) {
		szFontName[ii] = *pp;
		pp++;
		ii++;
	}
	szFontName[ii] = 0;
	nFontNameLen = (ii + 1) * sizeof(WCHAR);

	if (m_wFontSize == 0) {
		m_wFontSize = (unsigned short)LogFont.lfHeight;
	}

	//Prework for setting caption in dialog...
	int szBoxLen = m_strCaption.GetLength() + 1;
	WCHAR *szBoxCaption = new WCHAR[szBoxLen];
	ii = 0;
	pp = LPCTSTR(m_strCaption);
	while (*pp) {
		szBoxCaption[ii] = *pp;
		pp++;
		ii++;
	}
	szBoxCaption[ii] = 0;
	szBoxLen = (ii + 1) * sizeof(WCHAR);

	//Here 's the stuff to build the dialog template in memory
	//without the controls being in the template
	//(Our first try, was this same template with some additional code
	//for each control placed on it, that's why this class is cold Ex :)
	//This gave some problems on WIN9x systems, where EDIT boxes
	//were not shown with 3D-look, but as flat controls)
	HLOCAL hLocal = LocalAlloc(LHND, sizeof(DLGTEMPLATE) + (2 * sizeof(WORD)) /*menu and class*/
		+ szBoxLen /*size of caption*/ + sizeof(WORD) /*fontsize*/ + nFontNameLen /*size of fontname*/);
	if (hLocal != NULL) {
		BYTE*	pBuffer = (BYTE*)LocalLock(hLocal);
		if (pBuffer == NULL) {
			LocalFree(hLocal);
			AfxMessageBox(_T("CDynDialogEx::DoModal() : LocalLock Failed"));
		}

		BYTE *pdest = pBuffer;
		// transfer DLGTEMPLATE structure to the buffer
		memcpy(pdest, &m_DialogTemplate, sizeof(DLGTEMPLATE));	// DLGTemplate
		pdest += sizeof(DLGTEMPLATE);
		*(WORD*)pdest = 0;									// no menu						 -- WORD to say it is 0 bytes
		pdest += sizeof(WORD);								// Increment
		*(WORD*)(pdest + 1) = 0;							// use default window class -- WORD to say it is 0 bytes
		pdest += sizeof(WORD);								// Increment
		memcpy(pdest, szBoxCaption, szBoxLen);			// Caption
		pdest += szBoxLen;

		*(WORD*)pdest = m_wFontSize;						// font size
		pdest += sizeof(WORD);
		memcpy(pdest, szFontName, nFontNameLen);		// font name
		pdest += nFontNameLen;

		//This is the MFC function, which does the job
		InitModalIndirect((LPDLGTEMPLATE)pBuffer, m_pParentWnd);
		
		int iRet = CDialog::DoModal();

		LocalUnlock(hLocal);
		LocalFree(hLocal);

		delete [] szBoxCaption;
		delete [] szFontName;
		return iRet;
	}
	else {
		AfxMessageBox(_T("CDynDialogEx::DoModal() : LocalAllock Failed"));
		return -1;
	}	
}

void CNewWizDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	SizeButtons(cx,cy);
	// TODO: Add your message handler code here

}

void CNewWizDialog::SizeButtons(int cx, int cy)
{
	int w=80,h=30;//,b=10;
	int x=cx;
	x=x-w-10;
//	if(btn[0].IsWindowEnabled ())
//	{
//		if(!btn[0].IsWindowVisible ())
//		{
//			btn[0].ShowWindow (SW_SHOW);
//		}
	if(btn[0].m_hWnd ==NULL) return;
	btn[0].MoveWindow (x,cy-h-20,w,h,TRUE);
	x=x-w-10;
//	}
//	else
//	{
//		if(btn[0].IsWindowVisible ())
//		{
//			btn[0].ShowWindow (SW_HIDE);
//		}
//	}
//	if(btn[3].IsWindowEnabled ())
//	{	
//		if(!btn[0].IsWindowVisible ())
//		{
//			btn[0].ShowWindow (SW_SHOW);
//		}
		btn[2].MoveWindow (x,cy-h-20,w,h,TRUE);

	x=x-w-10;
//	}
//	else
//	{
//		if(btn[0].IsWindowVisible ())
//		{
//			btn[0].ShowWindow (SW_HIDE);
//		}
//	}
//	if(btn[2].IsWindowEnabled ())
//	{
//		if(!btn[0].IsWindowVisible ())
//		{
//			btn[0].ShowWindow (SW_SHOW);
//		}
		btn[3].MoveWindow (x,cy-h-20,w,h,TRUE);
	x=x-w-10;
//	}
//	else
//	{
//		if(btn[0].IsWindowVisible ())
//		{
//			btn[0].ShowWindow (SW_HIDE);
//		}
//	}
	btn[1].MoveWindow (0,cy-h-20,w,h,TRUE);

}

void CNewWizDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CBitmap b;
	b.LoadBitmap (IDB_BITBAR);
	CDC memdc;memdc.CreateCompatibleDC (&dc);
	CBitmap * old=memdc.SelectObject (&b);
	dc.BitBlt (0,0,450,100,&memdc,0,0,SRCCOPY);
	// Do not call CDialog::OnPaint() for painting messages
}
