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

#define MAX_UDATA_STR_LENGTH	50
struct UserData{
	int ID;
	int Order;
	char ScancodeID[MAX_UDATA_STR_LENGTH+1];
	int Number;
	char Name[MAX_UDATA_STR_LENGTH+1];
	char Sex[MAX_UDATA_STR_LENGTH+1];
	int Age;
	char BirthDate[MAX_UDATA_STR_LENGTH+1];
	char People[MAX_UDATA_STR_LENGTH+1];
	char Department[MAX_UDATA_STR_LENGTH+1];
	char TypeOfWork[MAX_UDATA_STR_LENGTH+1];
	char Province[MAX_UDATA_STR_LENGTH+1];
	char City[MAX_UDATA_STR_LENGTH+1];
	char Address[MAX_UDATA_STR_LENGTH+1];
	char ZipCode[MAX_UDATA_STR_LENGTH+1];
	char Tel[MAX_UDATA_STR_LENGTH+1];
	char ClinicalDiagnosis[MAX_UDATA_STR_LENGTH+1];
	int Height;
	char Weight[MAX_UDATA_STR_LENGTH+1];
	char CheckDate[MAX_UDATA_STR_LENGTH+1];
	char Hazards[MAX_UDATA_STR_LENGTH+1];
	char Pharmacy[MAX_UDATA_STR_LENGTH+1];
	char PastHistory[MAX_UDATA_STR_LENGTH+1];
};

#endif

