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

//打印消息
extern void Printf(const char *format, ...);
//打印缓冲区数据
extern void PrintBuffer(const char *hint, const unsigned char *buffer,int len);

extern void MakeSeparatorString(CString &destStr);
extern void ParseSeparatorString(CString str);

///记录中字符串的最大长度(根据数据库CaseData.mdb的说明而设定的)
#define MAX_UDATA_STR_LENGTH	50
///病人数据的结构
struct UserData{
	int ID;										//ID号
	int Order;									//序号
	int Status;									//状态(未处理/已处理)
	char ScancodeID[MAX_UDATA_STR_LENGTH+1];	//扫描码
	int Number;									//病案号
	char Name[MAX_UDATA_STR_LENGTH+1];			//姓名
	char Sex[MAX_UDATA_STR_LENGTH+1];			//性别
	int Age;									//年龄
	char BirthDate[MAX_UDATA_STR_LENGTH+1];		//生日
	char People[MAX_UDATA_STR_LENGTH+1];		//民族
	char Department[MAX_UDATA_STR_LENGTH+1];	//部门
	char TypeOfWork[MAX_UDATA_STR_LENGTH+1];	//工种
	char Province[MAX_UDATA_STR_LENGTH+1];		//省份
	char City[MAX_UDATA_STR_LENGTH+1];			//城市
	char Address[MAX_UDATA_STR_LENGTH+1];		//地址
	char ZipCode[MAX_UDATA_STR_LENGTH+1];		//邮编
	char Tel[MAX_UDATA_STR_LENGTH+1];			//电话
	char ClinicalDiagnosis[MAX_UDATA_STR_LENGTH+1];//临床诊断
	int Height;									//身高
	char Weight[MAX_UDATA_STR_LENGTH+1];		//体重
	char CheckDate[MAX_UDATA_STR_LENGTH+1];		//检查日期
	char Hazards[MAX_UDATA_STR_LENGTH+1];		//危险因素
	char Pharmacy[MAX_UDATA_STR_LENGTH+1];		//用药
	char PastHistory[MAX_UDATA_STR_LENGTH+1];	//病史
};

///ID号和Order组合的结构，用于Order查询
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

