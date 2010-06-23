#ifndef _GLOBALFUNCS_H_
#define _GLOBALFUNCS_H_

#include "MIBase.h"

extern void GetAppDataDir(char *buf);
extern void CreateAppDataDir();
extern BOOL CreateFolder(CString strPath);

//��ӡ��Ϣ
extern void Printf(const char *format, ...);
//��ӡ����������
extern void PrintBuffer(const char *hint, const unsigned char *buffer,int len);

extern CStringList g_strList;
extern void MakeSeparatorString(CString &destStr);
extern void ParseSeparatorString(CString str);

struct UserData{
	int id;
	char Name[20];
};

extern void CreateDataFile();

extern int Cmd_GetRecordNum();

extern int Cmd_GetRecordAt(int index, struct UserData &rec);

extern int Cmd_AppendRecord(struct UserData rec);

extern int Cmd_DeleteRecordAt(int index);


#endif

