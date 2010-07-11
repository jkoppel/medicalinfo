// ProgressThread.cpp : implementation file
//

#include "stdafx.h"
#include "AbsTestDataMan.h"
#include "ProgressThread.h"


// CProgressThread

IMPLEMENT_DYNCREATE(CProgressThread, CWinThread)

CProgressThread *CProgressThread::m_pThread = NULL;


CProgressThread::CProgressThread()
{
	m_pDlgProgress = NULL;
}

CProgressThread::~CProgressThread()
{
}

BOOL CProgressThread::InitInstance()
{
	if(m_pDlgProgress==NULL){
		m_pDlgProgress = new CDlgProgress();
		m_pDlgProgress->Create(IDD_PROGRESS);
	}

	return TRUE;
}

int CProgressThread::ExitInstance()
{
	if(m_pDlgProgress!=NULL){
		m_pDlgProgress->DestroyWindow();
		delete m_pDlgProgress;
		m_pDlgProgress = NULL;
	}

	return CWinThread::ExitInstance();
}

void CProgressThread::Start(int nCount)
{
	if(m_pThread==NULL){
		m_pThread = (CProgressThread*) AfxBeginThread(
											RUNTIME_CLASS(CProgressThread),
											THREAD_PRIORITY_NORMAL,
											0, CREATE_SUSPENDED);
		ASSERT(m_pThread->IsKindOf(RUNTIME_CLASS(CProgressThread)));
	}
	m_pThread->InitInstance();
	m_pThread->ResumeThread();
	m_pThread->m_pDlgProgress->m_pProgress->SetRange(1, nCount);
	Sleep(1);
	m_pThread->m_pDlgProgress->ShowWindow(SW_SHOW);
}

void CProgressThread::End()
{
	if(m_pThread!=NULL){
		m_pThread->ExitInstance();
		m_pThread = NULL;
	}
}

void CProgressThread::StepIt()
{
	if(m_pThread){
		m_pThread->m_pDlgProgress->m_pProgress->StepIt();
	}
}

void CProgressThread::SetPos(int nPos)
{
	if(m_pThread){
		m_pThread->m_pDlgProgress->m_pProgress->StepIt();
	}
}

BEGIN_MESSAGE_MAP(CProgressThread, CWinThread)
END_MESSAGE_MAP()


// CProgressThread message handlers
