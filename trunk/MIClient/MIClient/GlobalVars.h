#ifndef _GLOBALVARS_H_
#define _GLOBALVARS_H_

#include "ClientSocket.h"

///类型定义,无符号字符,1字节
typedef unsigned char uchar;
///类型定义,无符号短整数,2字节
typedef unsigned short ushort;
///类型定义,无符号整数,4字节
typedef unsigned int uint;


#define MAX_PACKET_BUF_LENGTH	1024 * 1024

extern CClientSocket * g_pClientSocket;
extern BOOL g_bIsConnected;

extern BOOL g_bIsDataComing;
extern char g_RecvData[1024];

#endif

