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

//构造函数
CTCPClient_CE::CTCPClient_CE()
{
   //初始化socket环境
   WSADATA wsd;
   WSAStartup(MAKEWORD(1,1),&wsd);
   //创建线程退出事件句柄
   m_exitThreadEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
}

//析构函数
CTCPClient_CE::~CTCPClient_CE()
{
   //释放socket资源
   WSACleanup();
   //关闭线程退出事件句柄
   CloseHandle(m_exitThreadEvent);
}

/*--------------------------------------------------------------------
【函数介绍】:  此线程用于监听TCP客户端通讯的事件，例如当接收到数据、
			   连接断开和通讯过程发生错误等事件
【入口参数】:  lparam:无类型指针，可以通过此参数，向线程中传入需要用到的资源。
			   在这里我们将CTCPClient_CE类实例指针传进来
【出口参数】:  (无)
【返回  值】:  返回值没有特别的意义，在此我们将返回值设为0。
---------------------------------------------------------------------*/
DWORD CTCPClient_CE::SocketThreadFunc(LPVOID lparam)
{
	CTCPClient_CE *pSocket;
	//得到CTCPClient_CE实例指针
	pSocket = (CTCPClient_CE*)lparam;
	//定义读事件集合
	fd_set fdRead;
	int ret;
	//定义事件等待时间
	TIMEVAL	aTime;
	aTime.tv_sec = 1;
	aTime.tv_usec = 0;
	while (TRUE)
	{
        //收到退出事件，结束线程
		if (WaitForSingleObject(pSocket->m_exitThreadEvent,0) == WAIT_OBJECT_0)
		{
			break;
		}
		//置空fdRead事件为空
		FD_ZERO(&fdRead);
		//给客户端socket设置读事件
		FD_SET(pSocket->m_socket,&fdRead);
		//调用select函数，判断是否有读事件发生
		ret = select(0,&fdRead,NULL,NULL,&aTime);
		
		if (ret == SOCKET_ERROR)
		{
			//触发错误事件
			pSocket->OnError(pSocket->m_pOwnerWnd,1);
			//触发连接断开事件
			pSocket->OnDisConnect(pSocket->m_pOwnerWnd);
			//关闭客户端socket
			closesocket(pSocket->m_socket);
			break;
		}
		
		
		if (ret > 0)
		{
			if (FD_ISSET(pSocket->m_socket,&fdRead))
			{
				//发生读事件
				char recvBuf[1024];
				int recvLen;
				ZeroMemory(recvBuf,1024);
				//接收数据
				recvLen = recv(pSocket->m_socket,recvBuf, 1024,0); 
				if (recvLen == SOCKET_ERROR)
				{
					int iError = WSAGetLastError();
					//触发socket错误事件
					pSocket->OnError(pSocket->m_pOwnerWnd,iError);
					//触发与服务器断开事件
					pSocket->OnDisConnect(pSocket->m_pOwnerWnd);
					//关闭客户端socket
					closesocket(pSocket->m_socket);
					break;
				}
				else if (recvLen == 0)
				{
					//触发与服务器端断开事件
					pSocket->OnDisConnect(pSocket->m_pOwnerWnd);
					//关闭客户端socket
					closesocket(pSocket->m_socket);
					break;
				}
				else
				{
				   //触发数据接收事件
                   pSocket->OnRead(pSocket->m_pOwnerWnd,recvBuf,recvLen);
				}
			}
		}
	}
	return 0;
}

/*--------------------------------------------------------------------
【函数介绍】: 用于打开客户端socket
【入口参数】: pWnd用于指定父窗口句柄
【出口参数】: (无)
【返回  值】: TRUE:打开成功;FALSE:打开失败
---------------------------------------------------------------------*/
bool CTCPClient_CE::Open(CWnd * pWnd)
{
   //复位线程退出事件
   ResetEvent(m_exitThreadEvent);
   //存储父窗口句柄
   m_pOwnerWnd = pWnd;
   //创建TCP套接字 
   m_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
   if (m_socket == SOCKET_ERROR)
   {
	   return FALSE;
   }
   //创建通讯线程
   m_tcpThreadHandle = CreateThread(NULL,0,SocketThreadFunc,this,0,NULL);
   if (m_tcpThreadHandle == NULL)
   {
	   closesocket(m_socket);
	   return FALSE;
   }
   return TRUE;
}


/*--------------------------------------------------------------------
【函数介绍】: 用于关闭客户端socket
【入口参数】:  (无)
【出口参数】:  (无)
【返回  值】: TRUE:关闭成功;FALSE:关闭失败
---------------------------------------------------------------------*/
bool CTCPClient_CE::Close()
{
   //发送通讯线程结束事件
   SetEvent(m_exitThreadEvent);
   Sleep(1000);
   //关闭Socket，释放资源
   int err = closesocket(m_socket);
   if (err == SOCKET_ERROR)
   {
	   return FALSE;
   }
   return TRUE;
}


/*--------------------------------------------------------------------
【函数介绍】: 用于建立与TCP服务器连接
【入口参数】: (无)
【出口参数】: (无)
【返回  值】: TRUE:建立连接成功;FALSE:建立连接失败
---------------------------------------------------------------------*/
bool CTCPClient_CE::Connect()
{
   struct sockaddr_in addr;
   int err;

   addr.sin_family = AF_INET;
   addr.sin_port = htons(m_port);
   //此处要将双字节转换成单字节
   char ansiRemoteHost[255];
	ZeroMemory(ansiRemoteHost,255);
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,m_remoteHost,wcslen(m_remoteHost)
		,ansiRemoteHost,wcslen(m_remoteHost),NULL,NULL);

   addr.sin_addr.s_addr=inet_addr(ansiRemoteHost);
   //此时采用同步连接方式,connect直接返回成功或是失败
   err = connect(m_socket,(struct sockaddr *)&addr,sizeof(addr));
   if (err == SOCKET_ERROR) 
   {
	   return FALSE;
   }
   //设置通讯模式为异步模式
   DWORD ul= 1;
   ioctlsocket(m_socket,FIONBIO,&ul);
   return TRUE;
}

/*--------------------------------------------------------------------
【函数介绍】: 向服务器端发送数据
【入口参数】: buf:待发送的数据
              len:待发送的数据长度
【出口参数】: (无)
【返回  值】: TRUE:发送数据成功;FALSE:发送数据失败
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
			//触发socket的Error事件
			OnError(m_pOwnerWnd,iErrorCode);
			//触发与服务器端断开连接事件
			OnDisConnect(m_pOwnerWnd);
			//关闭socket
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