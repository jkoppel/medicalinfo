// MIClient.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "MIClient.h"
#include "MIMainDlg.h"
#include "MIClientDlg.h"
#include "GlobalFuncs.h"
#include "ProgressInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMIClientApp

BEGIN_MESSAGE_MAP(CMIClientApp, CWinApp)
END_MESSAGE_MAP()


// CMIClientApp ����
CMIClientApp::CMIClientApp()
	: CWinApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CMIClientApp ����
CMIClientApp theApp;

// CMIClientApp ��ʼ��

BOOL CMIClientApp::InitInstance()
{
	if (!AfxSocketInit()){
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CMIMainDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	/*
	CMIClientDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˴����ô����ʱ�á�ȷ�������ر�
		//  �Ի���Ĵ���
	}
	*/

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

int MyMessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption, UINT nType)
{
	return AfxGetApp()->GetMainWnd()->GetActiveWindow()->MessageBox(lpszText, lpszCaption, nType);
}
