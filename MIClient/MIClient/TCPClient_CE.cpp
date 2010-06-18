// TCPClient_CE.cpp: implementation of the CTCPClient_CE class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MIClient.h"
#include "TCPClient_CE.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//���캯��
CTCPClient_CE::CTCPClient_CE()
{
   //��ʼ��socket����
   WSADATA wsd;
   WSAStartup(MAKEWORD(1,1),&wsd);
   //�����߳��˳��¼����
   m_exitThreadEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
}

//��������
CTCPClient_CE::~CTCPClient_CE()
{
   //�ͷ�socket��Դ
   WSACleanup();
   //�ر��߳��˳��¼����
   CloseHandle(m_exitThreadEvent);
}

/*--------------------------------------------------------------------
���������ܡ�:  ���߳����ڼ���TCP�ͻ���ͨѶ���¼������統���յ����ݡ�
			   ���ӶϿ���ͨѶ���̷���������¼�
����ڲ�����:  lparam:������ָ�룬����ͨ���˲��������߳��д�����Ҫ�õ�����Դ��
			   ���������ǽ�CTCPClient_CE��ʵ��ָ�봫����
�����ڲ�����:  (��)
������  ֵ��:  ����ֵû���ر�����壬�ڴ����ǽ�����ֵ��Ϊ0��
---------------------------------------------------------------------*/
DWORD CTCPClient_CE::SocketThreadFunc(LPVOID lparam)
{
	CTCPClient_CE *pSocket;
	//�õ�CTCPClient_CEʵ��ָ��
	pSocket = (CTCPClient_CE*)lparam;
	//������¼�����
	fd_set fdRead;
	int ret;
	//�����¼��ȴ�ʱ��
	TIMEVAL	aTime;
	aTime.tv_sec = 1;
	aTime.tv_usec = 0;
	while (TRUE)
	{
        //�յ��˳��¼��������߳�
		if (WaitForSingleObject(pSocket->m_exitThreadEvent,0) == WAIT_OBJECT_0)
		{
			break;
		}
		//�ÿ�fdRead�¼�Ϊ��
		FD_ZERO(&fdRead);
		//���ͻ���socket���ö��¼�
		FD_SET(pSocket->m_socket,&fdRead);
		//����select�������ж��Ƿ��ж��¼�����
		ret = select(0,&fdRead,NULL,NULL,&aTime);
		
		if (ret == SOCKET_ERROR)
		{
			//���������¼�
			pSocket->OnError(pSocket->m_pOwnerWnd,1);
			//�������ӶϿ��¼�
			pSocket->OnDisConnect(pSocket->m_pOwnerWnd);
			//�رտͻ���socket
			closesocket(pSocket->m_socket);
			break;
		}
		
		
		if (ret > 0)
		{
			if (FD_ISSET(pSocket->m_socket,&fdRead))
			{
				//�������¼�
				char recvBuf[1024];
				int recvLen;
				ZeroMemory(recvBuf,1024);
				//��������
				recvLen = recv(pSocket->m_socket,recvBuf, 1024,0); 
				if (recvLen == SOCKET_ERROR)
				{
					int iError = WSAGetLastError();
					//����socket�����¼�
					pSocket->OnError(pSocket->m_pOwnerWnd,iError);
					//������������Ͽ��¼�
					pSocket->OnDisConnect(pSocket->m_pOwnerWnd);
					//�رտͻ���socket
					closesocket(pSocket->m_socket);
					break;
				}
				else if (recvLen == 0)
				{
					//������������˶Ͽ��¼�
					pSocket->OnDisConnect(pSocket->m_pOwnerWnd);
					//�رտͻ���socket
					closesocket(pSocket->m_socket);
					break;
				}
				else
				{
				   //�������ݽ����¼�
                   pSocket->OnRead(pSocket->m_pOwnerWnd,recvBuf,recvLen);
				}
			}
		}
	}
	return 0;
}

/*--------------------------------------------------------------------
���������ܡ�: ���ڴ򿪿ͻ���socket
����ڲ�����: pWnd����ָ�������ھ��
�����ڲ�����: (��)
������  ֵ��: TRUE:�򿪳ɹ�;FALSE:��ʧ��
---------------------------------------------------------------------*/
bool CTCPClient_CE::Open(CWnd * pWnd)
{
   //��λ�߳��˳��¼�
   ResetEvent(m_exitThreadEvent);
   //�洢�����ھ��
   m_pOwnerWnd = pWnd;
   //����TCP�׽��� 
   m_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
   if (m_socket == SOCKET_ERROR)
   {
	   return FALSE;
   }
   //����ͨѶ�߳�
   m_tcpThreadHandle = CreateThread(NULL,0,SocketThreadFunc,this,0,NULL);
   if (m_tcpThreadHandle == NULL)
   {
	   closesocket(m_socket);
	   return FALSE;
   }
   return TRUE;
}


/*--------------------------------------------------------------------
���������ܡ�: ���ڹرտͻ���socket
����ڲ�����:  (��)
�����ڲ�����:  (��)
������  ֵ��: TRUE:�رճɹ�;FALSE:�ر�ʧ��
---------------------------------------------------------------------*/
bool CTCPClient_CE::Close()
{
   //����ͨѶ�߳̽����¼�
   SetEvent(m_exitThreadEvent);
   Sleep(1000);
   //�ر�Socket���ͷ���Դ
   int err = closesocket(m_socket);
   if (err == SOCKET_ERROR)
   {
	   return FALSE;
   }
   return TRUE;
}


/*--------------------------------------------------------------------
���������ܡ�: ���ڽ�����TCP����������
����ڲ�����: (��)
�����ڲ�����: (��)
������  ֵ��: TRUE:�������ӳɹ�;FALSE:��������ʧ��
---------------------------------------------------------------------*/
bool CTCPClient_CE::Connect()
{
   struct sockaddr_in addr;
   int err;

   addr.sin_family = AF_INET;
   addr.sin_port = htons(m_port);
   //�˴�Ҫ��˫�ֽ�ת���ɵ��ֽ�
   char ansiRemoteHost[255];
	ZeroMemory(ansiRemoteHost,255);
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,m_remoteHost,wcslen(m_remoteHost)
		,ansiRemoteHost,wcslen(m_remoteHost),NULL,NULL);

   addr.sin_addr.s_addr=inet_addr(ansiRemoteHost);
   //��ʱ����ͬ�����ӷ�ʽ,connectֱ�ӷ��سɹ�����ʧ��
   err = connect(m_socket,(struct sockaddr *)&addr,sizeof(addr));
   if (err == SOCKET_ERROR) 
   {
	   return FALSE;
   }
   //����ͨѶģʽΪ�첽ģʽ
   DWORD ul= 1;
   ioctlsocket(m_socket,FIONBIO,&ul);
   return TRUE;
}

/*--------------------------------------------------------------------
���������ܡ�: ��������˷�������
����ڲ�����: buf:�����͵�����
              len:�����͵����ݳ���
�����ڲ�����: (��)
������  ֵ��: TRUE:�������ݳɹ�;FALSE:��������ʧ��
---------------------------------------------------------------------*/
bool CTCPClient_CE::SendData(const char * buf , int len)
{
	int nBytes = 0;
	int nSendBytes=0;
			
	while (nSendBytes < len)
	{
	    nBytes = send(m_socket,buf+nSendBytes,len-nSendBytes,0);
		if (nBytes==SOCKET_ERROR )
		{
			int iErrorCode = WSAGetLastError();
			//����socket��Error�¼�
			OnError(m_pOwnerWnd,iErrorCode);
			//������������˶Ͽ������¼�
			OnDisConnect(m_pOwnerWnd);
			//�ر�socket
			Close();
			return FALSE;
		}

		nSendBytes = nSendBytes + nBytes;
		
		if (nSendBytes < len)
		{
		    Sleep(1000);
		}
	} 
	return TRUE; 
}