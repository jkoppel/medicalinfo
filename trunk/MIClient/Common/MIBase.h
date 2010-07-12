#ifndef _MIBASE_H_
#define _MIBASE_H_

#include <math.h>
#include <afxtempl.h>

///成功
#define SUCC		0
///失败
#define FAIL		-1
///是
#define YES			1
///否
#define NO			0
///OK
#define OK			2
///取消
#define CANCEL		3

///服务器消息
struct SServerMsg
{
	UINT m_msgType;
	char m_msgBuff[2048];
};

///病人信息
struct SUserInfo
{
	UINT m_infoID;
	char m_infoNick[64];
	char m_infoName[64];
	UINT m_infoPSW;
	UINT m_infoAge;
	UINT m_infoSex;
	UINT m_infoPicture;
	UINT m_infoIsOnLine;
	UINT m_infoFriendList[64];
	UINT m_infoBlackList [64];
	char m_infoAddr[20];
	UINT m_infoPort;
	UINT m_infoTime;
	void * m_infoSocket;
};

#ifdef _WIN32_WCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define sprintf_s	sprintf
#define _snprintf_s	_snprintf
#define sscanf_s	sscanf

#endif

#define CONFIG_FILE		"Configuration.ini"

#endif//_MIBASE_H_

