#ifndef _GLOBALVARS_H_
#define _GLOBALVARS_H_

#include "ClientSocket.h"

///���Ͷ���,�޷����ַ�,1�ֽ�
typedef unsigned char uchar;
///���Ͷ���,�޷��Ŷ�����,2�ֽ�
typedef unsigned short ushort;
///���Ͷ���,�޷�������,4�ֽ�
typedef unsigned int uint;


#define MAX_PACKET_BUF_LENGTH	1024 * 1024

extern CClientSocket * g_pClientSocket;
extern BOOL g_bIsConnected;

extern BOOL g_bIsDataComing;
extern char g_RecvData[1024];

#endif

