// TCPClient_CE.h: interface for the CTCPClient_CE class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCPCLIENT_CE_H__B7856B99_69E7_4868_9BA3_96152245C65E__INCLUDED_)
#define AFX_TCPCLIENT_CE_H__B7856B99_69E7_4868_9BA3_96152245C65E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsock.h>


//定义连接断开事件
typedef void (CALLBACK* ONDISCONNECT)(CWnd*);
//定义当有数据接收事件
typedef void (CALLBACK* ONREAD)(CWnd*,const char * buf,int len );
//定义Socket错误事件
typedef void (CALLBACK* ONERROR)(CWnd*,int nErrorCode);

class CTCPClient_CE  
{
public:
	CTCPClient_CE();
	virtual ~CTCPClient_CE();
public:
	//打开客户端socket
	bool Open(CWnd * pWnd);
	//关闭客户端socket
	bool Close();
	//与服务器端建立连接
	bool Connect();
	//向服务器端发送数据
	bool SendData(const char * buf , int len);
public:
	//远程主机IP地址
	CString	 m_remoteHost;  
	//远程主机端口
    int      m_port;
    
	/*--以下客户端通讯事件--*/
	//连接断开事件，回调函数
	ONDISCONNECT OnDisConnect;
	//接收数据事件，回调函数
	ONREAD       OnRead;
	//发生错误事件，回调函数
	ONERROR      OnError;
private:
	//通讯Socket句柄
	SOCKET m_socket;      
	//通讯线程退出事件句柄
	HANDLE m_exitThreadEvent; 
	//通讯线程句柄
	HANDLE m_tcpThreadHandle;
	//父窗口句柄
	CWnd * m_pOwnerWnd;   
private:
    //通讯线程函数
	static DWORD SocketThreadFunc(LPVOID lparam);
};

#endif // !defined(AFX_TCPCLIENT_CE_H__B7856B99_69E7_4868_9BA3_96152245C65E__INCLUDED_)
