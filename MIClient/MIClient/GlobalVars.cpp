#include "stdafx.h"
#include "GlobalVars.h"

CClientSocket * g_pClientSocket = NULL;
BOOL g_bIsConnected;

BOOL g_bIsDataComing = FALSE;
char g_RecvData[1024];