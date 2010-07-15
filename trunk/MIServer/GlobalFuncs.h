#ifndef _GLOBALFUNCS_H_
#define _GLOBALFUNCS_H_

#include "StdAfx.h"
#include "MIBase.h"

extern CStringList g_strList;
extern _ConnectionPtr	g_pDBConnection;
extern BOOL g_bIsDBConnected;

extern void GetAppDataDir(char *buf);
extern void CreateAppDataDir();
extern BOOL CreateFolder(CString strPath);

//��ӡ��Ϣ
extern void Printf(const char *format, ...);
//��ӡ����������
extern void PrintBuffer(const char *hint, const unsigned char *buffer,int len);

extern void MakeSeparatorString(CString &destStr);
extern void ParseSeparatorString(CString str);

///��¼���ַ�������󳤶�(�������ݿ�CaseData.mdb��˵�����趨��)
#define MAX_UDATA_STR_LENGTH	50
///�������ݵĽṹ
struct UserData{
	int ID;										//ID��
	int Order;									//���
	int Status;									//״̬(δ����/�Ѵ���)
	char ScancodeID[MAX_UDATA_STR_LENGTH+1];	//ɨ����
	int Number;									//������
	char Name[MAX_UDATA_STR_LENGTH+1];			//����
	char Sex[MAX_UDATA_STR_LENGTH+1];			//�Ա�
	int Age;									//����
	char BirthDate[MAX_UDATA_STR_LENGTH+1];		//����
	char People[MAX_UDATA_STR_LENGTH+1];		//����
	char Department[MAX_UDATA_STR_LENGTH+1];	//����
	char TypeOfWork[MAX_UDATA_STR_LENGTH+1];	//����
	char Province[MAX_UDATA_STR_LENGTH+1];		//ʡ��
	char City[MAX_UDATA_STR_LENGTH+1];			//����
	char Address[MAX_UDATA_STR_LENGTH+1];		//��ַ
	char ZipCode[MAX_UDATA_STR_LENGTH+1];		//�ʱ�
	char Tel[MAX_UDATA_STR_LENGTH+1];			//�绰
	char ClinicalDiagnosis[MAX_UDATA_STR_LENGTH+1];//�ٴ����
	int Height;									//���
	char Weight[MAX_UDATA_STR_LENGTH+1];		//����
	char CheckDate[MAX_UDATA_STR_LENGTH+1];		//�������
	char Hazards[MAX_UDATA_STR_LENGTH+1];		//Σ������
	char Pharmacy[MAX_UDATA_STR_LENGTH+1];		//��ҩ
	char PastHistory[MAX_UDATA_STR_LENGTH+1];	//��ʷ
};

///ID�ź�Order��ϵĽṹ������Order��ѯ
struct IDAndOrder{
	int ID;
	int Order;
};

enum {MODE_ALL=0, MODE_UNPROCESSED, MODE_PROCESSED};

extern void MakeSendCmdFromRec(struct UserData data, CString &str);
extern int ParseRecvDataToRec(CString str, struct UserData &data);

extern void CreateDataFile();

extern BOOL Cmd_GetRecordNum(int &num);

extern BOOL Cmd_GetAllIDs(int *pID, int &num, int mode=MODE_ALL);

extern void MakeIDToSeparatorString(int *pID, int num, CString &str);

extern BOOL Cmd_GetRecordByID(int ID, struct UserData &rec);

extern BOOL Cmd_GetRecordByOrder(int order, struct UserData &rec);

extern BOOL Cmd_GetOrders(int order1, int order2, struct IDAndOrder *pIDAndOrder, int &num);

extern BOOL Cmd_AppendRecord(struct UserData &rec);

extern BOOL Cmd_DeleteRecordByID(int ID);

extern BOOL Cmd_ModifyRecordByID(int ID, struct UserData rec);

extern BOOL Cmd_GetNextFreeOrder(int &order);

extern BOOL Cmd_GetOrderByID(int ID, int &order);

extern BOOL Cmd_SetOrderByID(int ID, int order);

extern BOOL Cmd_MoveOrder(int org_order, int dst_order);

extern BOOL Cmd_MoveOrderPrev(int order);

extern BOOL Cmd_MoveOrderNext(int order);

extern BOOL Cmd_SearchByScancodeID(const char *scan_code_id, int *pID, int &num);

#endif

