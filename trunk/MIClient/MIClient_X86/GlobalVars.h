#ifndef _GLOBALVARS_H_
#define _GLOBALVARS_H_

#include "ClientSocket.h"

///类型定义,无符号字符,1字节
typedef unsigned char uchar;
///类型定义,无符号短整数,2字节
typedef unsigned short ushort;
///类型定义,无符号整数,4字节
typedef unsigned int uint;


#define MAX_PACKET_BUF_LENGTH	1024 * 1024

extern CClientSocket * g_pClientSocket;
extern BOOL g_bIsConnected;

extern BOOL g_bIsDataComing;
extern char g_RecvData[1024];
extern CStringList g_strList;

#define MAX_UDATA_STR_LENGTH	50	///字符串字段长度

///病人数据记录
struct UserData{
	int ID;									//ID
	int Order;								//顺序
	char ScancodeID[MAX_UDATA_STR_LENGTH+1];//扫描码
	int Number;								//病案号
	char Name[MAX_UDATA_STR_LENGTH+1];		//姓名
	char Sex[MAX_UDATA_STR_LENGTH+1];		//性别
	int Age;								//年龄
	char BirthDate[MAX_UDATA_STR_LENGTH+1];	//出生日期
	char People[MAX_UDATA_STR_LENGTH+1];	//民族
	char Department[MAX_UDATA_STR_LENGTH+1];//部门
	char TypeOfWork[MAX_UDATA_STR_LENGTH+1];//工种
	char Province[MAX_UDATA_STR_LENGTH+1];	//省份
	char City[MAX_UDATA_STR_LENGTH+1];		//城市
	char Address[MAX_UDATA_STR_LENGTH+1];	//住址
	char ZipCode[MAX_UDATA_STR_LENGTH+1];	//邮编
	char Tel[MAX_UDATA_STR_LENGTH+1];		//电话
	char ClinicalDiagnosis[MAX_UDATA_STR_LENGTH*4+1];//临床诊断
	int Height;								//身高
	char Weight[MAX_UDATA_STR_LENGTH+1];	//体重
	char CheckDate[MAX_UDATA_STR_LENGTH+1];	//检查日期
	char Hazards[MAX_UDATA_STR_LENGTH*4+1];	//危险因素
	char Pharmacy[MAX_UDATA_STR_LENGTH*4+1];	//用药
	char PastHistory[MAX_UDATA_STR_LENGTH*4+1];//既往病史
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

