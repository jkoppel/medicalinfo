// DocInfoView.cpp : implementation file
//

#include "stdafx.h"
#include "PdfClimate.h"
#include "DocInfoView.h"
#include "GlobalVars.h"
#include "ImageProcess/BaseDocument.h"


// CDocInfoView

IMPLEMENT_DYNCREATE(CDocInfoView, CFormView)

CDocInfoView::CDocInfoView()
	: CFormView(CDocInfoView::IDD)
{
    m_bIsEditing = false;
}

CDocInfoView::~CDocInfoView()
{
}

void CDocInfoView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DOC_NAME, m_ctrlEditDocName);
    DDX_Control(pDX, IDC_DOC_AUTHOR, m_ctrlEditDocAuthor);
    DDX_Control(pDX, IDC_DOC_EDITSAVE, m_ctrlBtnEditSave);
}

BEGIN_MESSAGE_MAP(CDocInfoView, CFormView)
    ON_BN_CLICKED(IDC_DOC_EDITSAVE, &CDocInfoView::OnBnClickedDocEditsave)
END_MESSAGE_MAP()


// CDocInfoView diagnostics

#ifdef _DEBUG
void CDocInfoView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDocInfoView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDocInfoView message handlers


void CDocInfoView::OnBnClickedDocEditsave()
{
    if (!m_bIsEditing) {
        m_ctrlEditDocName.SetReadOnly(FALSE);
        m_ctrlEditDocAuthor.SetReadOnly(FALSE);
        m_ctrlBtnEditSave.SetWindowText("±£´æ");
    }
    else {
        //TODO: save to database
        if (g_pCurrDocument) {
            char buf[256];
            m_ctrlEditDocName.GetWindowText(buf, sizeof(buf));
            g_pCurrDocument->setName(buf);
            m_ctrlEditDocAuthor.GetWindowText(buf, sizeof(buf));
            g_pCurrDocument->setAuthor(buf);
        }
        m_ctrlEditDocName.SetReadOnly();
        m_ctrlEditDocAuthor.SetReadOnly();
        m_ctrlBtnEditSave.SetWindowText("±à¼­");
    }
    m_bIsEditing = !m_bIsEditing;
}

void CDocInfoView::loadDataFromDB()
{
    m_ctrlEditDocName.SetReadOnly();
    m_ctrlEditDocAuthor.SetReadOnly();
    m_ctrlBtnEditSave.SetWindowText("±à¼­");
    m_bIsEditing = false;
    if (g_pCurrDocument) {
        m_ctrlEditDocName.SetWindowText( g_pCurrDocument->getName() );
        m_ctrlEditDocAuthor.SetWindowText( g_pCurrDocument->getAuthor() );
    }
    else {
        m_ctrlEditDocName.SetWindowText( "" );
        m_ctrlEditDocAuthor.SetWindowText( "");
    }
}
