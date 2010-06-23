#include "stdafx.h"
#include "GlobalFuncs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern void HideProgressInfo();
extern int MyMessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption, UINT nType = MB_OK);

void CString2Char(CString source, char *dest)
{
	int length;
	length = source.GetLength(); 
	memset(dest, 0, 2*length);
	WideCharToMultiByte(
		CP_ACP, WC_COMPOSITECHECK|WC_DEFAULTCHAR,
		source.GetBuffer(length),
		length+1,
		dest,
		2*length,
		0,0);
}

void Char2CString(char *, CString)
{
}

void ShowMsg(char msg[])
{
	HideProgressInfo();
	MyMessageBox(CString(msg), CString("消息"), MB_ICONINFORMATION);
}

void ShowMsg(CString msg)
{
	HideProgressInfo();
	MyMessageBox(msg, CString("消息"), MB_ICONINFORMATION);
}

CStringList g_strList;
void MakeSeparatorString(CString &str)
{
	int num = g_strList.GetCount();
	POSITION p;

	str.Empty();

	for(p=g_strList.GetHeadPosition();p!=NULL;num--){
		CString tmp = g_strList.GetNext(p);
		if(num>1){
			str += tmp + CString("||");
		}
		else{
			str += tmp;
		}
	}
}

void ParseSeparatorString(CString str)
{
	int offset;
	CString tmp;

	g_strList.RemoveAll();
	
	offset = str.Find(CString("||"));
	while(offset>0){
		tmp = str.Left(offset);
		g_strList.AddTail(tmp);
		str = str.Right(str.GetLength()-offset-2);
		offset = str.Find(CString("||"));
	}
}

void DebugSeparatorString()
{
	CString str, str1;
	g_strList.RemoveAll();
	char buf[20];
	for(int i=0;i<10;i++){
		sprintf(buf, "%d", i);
		g_strList.AddTail(CString("Hello")+CString(buf));
	}
	MakeSeparatorString(str);

	g_strList.RemoveAll();
	ParseSeparatorString(str);

	MakeSeparatorString(str1);
}
