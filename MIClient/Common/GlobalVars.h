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
extern CStringList g_strList;

#define MAX_UDATA_STR_LENGTH	50	///�ַ����ֶγ���

///�������ݼ�¼
struct UserData{
	int ID;									//ID
	int Order;								//˳��
	char ScancodeID[MAX_UDATA_STR_LENGTH+1];//ɨ����
	int Number;								//������
	char Name[MAX_UDATA_STR_LENGTH+1];		//����
	char Sex[MAX_UDATA_STR_LENGTH+1];		//�Ա�
	int Age;								//����
	char BirthDate[MAX_UDATA_STR_LENGTH+1];	//��������
	char People[MAX_UDATA_STR_LENGTH+1];	//����
	char Department[MAX_UDATA_STR_LENGTH+1];//����
	char TypeOfWork[MAX_UDATA_STR_LENGTH+1];//����
	char Province[MAX_UDATA_STR_LENGTH+1];	//ʡ��
	char City[MAX_UDATA_STR_LENGTH+1];		//����
	char Address[MAX_UDATA_STR_LENGTH+1];	//סַ
	char ZipCode[MAX_UDATA_STR_LENGTH+1];	//�ʱ�
	char Tel[MAX_UDATA_STR_LENGTH+1];		//�绰
	char ClinicalDiagnosis[MAX_UDATA_STR_LENGTH*4+1];//�ٴ����
	int Height;								//���
	char Weight[MAX_UDATA_STR_LENGTH+1];	//����
	char CheckDate[MAX_UDATA_STR_LENGTH+1];	//�������
	char Hazards[MAX_UDATA_STR_LENGTH*4+1];	//Σ������
	char Pharmacy[MAX_UDATA_STR_LENGTH*4+1];	//��ҩ
	char PastHistory[MAX_UDATA_STR_LENGTH*4+1];//������ʷ
};

#define CMD_CONNECT					1
#define CMD_GETRECORDNUM			2
#define CMD_GETALLID				3
#define CMD_GETRECORDBYID			4
#define CMD_APPENDRECORD			5
#define CMD_EDITRECORDBYID			6
#define CMD_DELETERECORDBYID		7
#define CMD_GETNEXTFREEORDER		8
#define CMD_GETORDERBYID			9
#define CMD_SETORDERBYID			10
#define CMD_MOVEORDER				11
#define CMD_MOVEORDERPREV			12
#define CMD_MOVEORDERNEXT			13

#endif

