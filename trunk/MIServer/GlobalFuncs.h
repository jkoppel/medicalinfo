#ifndef _GLOBALFUNCS_H_
#define _GLOBALFUNCS_H_

#include "StdAfx.h"
#include "MIBase.h"

extern void GetAppDataDir(char *buf);
extern void CreateAppDataDir();
extern BOOL CreateFolder(CString strPath);

//打印消息
extern void Printf(const char *format, ...);
//打印缓冲区数据
extern void PrintBuffer(const char *hint, const unsigned char *buffer,int len);

extern CStringList g_strList;
extern void MakeSeparatorString(CString &destStr);
extern void ParseSeparatorString(CString str);

#define MAX_UDATA_STR_LENGTH	50
struct UserData{
	int ID;
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

extern void MakeSendCmdFromRec(struct UserData data, CString &str);
extern int ParseRecvDataToRec(CString str, struct UserData &data);

extern void CreateDataFile();

extern BOOL Cmd_GetRecordNum(int &num);

extern BOOL Cmd_GetAllIDs(int *pID, int &num);

extern void MakeIDToSeparatorString(int *pID, int num, CString &str);

extern BOOL Cmd_GetRecordByID(int ID, struct UserData &rec);

extern BOOL Cmd_AppendRecord(struct UserData &rec);

extern BOOL Cmd_DeleteRecordByID(int ID);

extern BOOL Cmd_ModifyRecordByID(int ID, struct UserData rec);

extern _ConnectionPtr	g_pDBConnection;


#endif

