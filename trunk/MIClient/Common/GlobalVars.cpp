#include "stdafx.h"
#include "GlobalVars.h"

///ȫ��socket����
CClientSocket * g_pClientSocket = NULL;
///ȫ������״̬���Ƿ�����
BOOL g_bIsConnected;

///ȫ���Ƿ��ѽ��յ�����
BOOL g_bIsDataComing = FALSE;
///ȫ�ֽ������ݻ�����
char g_RecvData[MAX_BUF_LEN];
///ȫ���ַ����б�
CStringList g_strList;
