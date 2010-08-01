#pragma once

#include "ExtLibs\\SkinProgress\\SkinProgress.h"

#define WM_UPDATEDATA WM_USER + 1999

extern BOOL PeekAndPump();

// CDlgProgress dialog

///���̶Ի����࣬������ʾ���ȣ��������û�ȡ����������ִ�й����г�������״̬ʱ�����
class CDlgProgress : public CDialog
{
	DECLARE_DYNAMIC(CDlgProgress)

public:
	CDlgProgress(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgProgress();

// Dialog Data
	enum { IDD = IDD_PROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_iMaxPos;					//�����������λ�ã�Ĭ��100
	CString m_strPromptInfo;		//��ʾ��Ϣ
	CSkinProgress *m_pProgress;		//����������

	static LPVOID m_lpParam;
	static CDlgProgress *m_pDlg;	//�Ի������
	static CWinThread *m_pThread;	//�����Ľ��̾��
	static AFX_THREADPROC m_ThreadProc;//�û���������Ľ��̺��������û�Ҫִ�е������ڸú�����ʵ��
	static BOOL StartThread(AFX_THREADPROC ThreadProc, LPCTSTR sPromptInfo=NULL, int iMaxPos=100);//��������
	static BOOL UpdateAndCheck(int iPercent);//���°ٷֱȣ�������û��Ƿ����ȡ��
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClose();
	LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam);
	virtual BOOL DestroyWindow();
};
