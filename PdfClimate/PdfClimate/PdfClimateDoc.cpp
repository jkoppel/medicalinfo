
// PdfClimateDoc.cpp : implementation of the CPdfClimateDoc class
//

#include "stdafx.h"
#include "PdfClimate.h"

#include "PdfClimateDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPdfClimateDoc

IMPLEMENT_DYNCREATE(CPdfClimateDoc, CDocument)

BEGIN_MESSAGE_MAP(CPdfClimateDoc, CDocument)
END_MESSAGE_MAP()


// CPdfClimateDoc construction/destruction

CPdfClimateDoc::CPdfClimateDoc()
{
    // TODO: add one-time construction code here

}

CPdfClimateDoc::~CPdfClimateDoc()
{
}

BOOL CPdfClimateDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    SetTitle("Pdf Climate");

    return TRUE;
}




// CPdfClimateDoc serialization

void CPdfClimateDoc::Serialize(CArchive& ar)
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


// CPdfClimateDoc diagnostics

#ifdef _DEBUG
void CPdfClimateDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CPdfClimateDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG


// CPdfClimateDoc commands
