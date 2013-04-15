
// LeftView.cpp : implementation of the CLeftView class
//

#include "stdafx.h"
#include "PdfClimate.h"

#include "PdfClimateDoc.h"
#include "LeftView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CTreeView)

BEGIN_MESSAGE_MAP(CLeftView, CTreeView)
    ON_NOTIFY_REFLECT(NM_DBLCLK, &CLeftView::OnNMDblclk)
END_MESSAGE_MAP()


// CLeftView construction/destruction

CLeftView::CLeftView()
{
    hRoot = NULL;
}

CLeftView::~CLeftView()
{
}

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs

    return CTreeView::PreCreateWindow(cs);
}

void CLeftView::OnInitialUpdate()
{
    CTreeView::OnInitialUpdate();

    this->GetTreeCtrl().ModifyStyle(0,TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS); 
}


// CLeftView diagnostics

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
    CTreeView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
    CTreeView::Dump(dc);
}

CPdfClimateDoc* CLeftView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPdfClimateDoc)));
    return (CPdfClimateDoc*)m_pDocument;
}
#endif //_DEBUG


void CLeftView::addFile(const char *file)
{
    if (!m_strFileList.Find(file)){
        m_strFileList.InsertAfter(m_strFileList.GetTailPosition(), file);
        HTREEITEM hItem = GetTreeCtrl().InsertItem(file);
        GetTreeCtrl().SelectItem(hItem);
    }
}
void CLeftView::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
    HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
    CString str = GetTreeCtrl().GetItemText(hItem);
    if (str.GetLength()>0) {
        char *file = str.GetBuffer(str.GetLength());
        CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
        //pFrame->dispFile(file);
    }
    
    *pResult = 0;
}
