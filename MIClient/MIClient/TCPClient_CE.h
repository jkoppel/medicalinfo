// TCPClient_CE.h: interface for the CTCPClient_CE class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCPCLIENT_CE_H__B7856B99_69E7_4868_9BA3_96152245C65E__INCLUDED_)
#define AFX_TCPCLIENT_CE_H__B7856B99_69E7_4868_9BA3_96152245C65E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsock.h>


//�������ӶϿ��¼�
typedef void (CALLBACK* ONDISCONNECT)(CWnd*);
//���嵱�����ݽ����¼�
typedef void (CALLBACK* ONREAD)(CWnd*,const char * buf,int len );
//����Socket�����¼�
typedef void (CALLBACK* ONERROR)(CWnd*,int nErrorCode);

class CTCPClient_CE  
{
public:
	CTCPClient_CE();
	virtual ~CTCPClient_CE();
public:
	//�򿪿ͻ���socket
	bool Open(CWnd * pWnd);
	//�رտͻ���socket
	bool Close();
	//��������˽�������
	bool Connect();
	//��������˷�������
	bool SendData(const char * buf , int len);
public:
	//Զ������IP��ַ
	CString	 m_remoteHost;  
	//Զ�������˿�
    int      m_port;
    
	/*--���¿ͻ���ͨѶ�¼�--*/
	//���ӶϿ��¼����ص�����
	ONDISCONNECT OnDisConnect;
	//���������¼����ص�����
	ONREAD       OnRead;
	//���������¼����ص�����
	ONERROR      OnError;
private:
	//ͨѶSocket���
	SOCKET m_socket;      
	//ͨѶ�߳��˳��¼����
	HANDLE m_exitThreadEvent; 
	//ͨѶ�߳̾��
	HANDLE m_tcpThreadHandle;
	//�����ھ��
	CWnd * m_pOwnerWnd;   
private:
    //ͨѶ�̺߳���
	static DWORD SocketThreadFunc(LPVOID lparam);
};

#endif // !defined(AFX_TCPCLIENT_CE_H__B7856B99_69E7_4868_9BA3_96152245C65E__INCLUDED_)
