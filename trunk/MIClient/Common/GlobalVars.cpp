#include "stdafx.h"
#include "GlobalVars.h"

///全局socket变量
CClientSocket * g_pClientSocket = NULL;
///全局连接状态，是否连接
BOOL g_bIsConnected;

///全局是否已接收到数据
BOOL g_bIsDataComing = FALSE;
///全局接收数据缓冲区
char g_RecvData[1024];
///全局字符串列表
CStringList g_strList;
