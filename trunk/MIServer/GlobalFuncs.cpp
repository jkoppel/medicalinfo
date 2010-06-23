#include "stdafx.h"
#include "GlobalFuncs.h"
#include "DataFile.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

BOOL FolderExist(CString strPath)
{
     WIN32_FIND_DATA   wfd;
     BOOL rValue = FALSE;
     HANDLE hFind = FindFirstFile(strPath, &wfd);
    if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
              rValue = TRUE;   
     }
     FindClose(hFind);
    return rValue;
}

BOOL CreateFolder(CString strPath)
{
     SECURITY_ATTRIBUTES attrib;
     attrib.bInheritHandle = FALSE;
     attrib.lpSecurityDescriptor = NULL;
     attrib.nLength =sizeof(SECURITY_ATTRIBUTES);
    //上面定义的属性可以省略。 直接return ::CreateDirectory( path, NULL); 即可
     return ::CreateDirectory( strPath, &attrib);
}

void GetAppDataDir(char *buf)
{
	::GetEnvironmentVariable("AppData", buf, 100);
	sprintf(buf, "%s%s", buf, "\\MIServer");
}

///在%AppData%目录下面创建MIServer目录
void CreateAppDataDir()
{
	char buf[100];

	GetAppDataDir(buf);
	CString strAppData = CString(buf);;
	if(FolderExist(strAppData)){
		return;
	}

	CreateFolder(strAppData);
}

///向文件打印调试信息
void Printf(const char *format, ...)
{
	char pstring[256];
	va_list ap;
	void *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9, *p10, *p11, *p12;
	va_start(ap, format);
	p1 = va_arg(ap, void*);
	p2 = va_arg(ap, void*);
	p3 = va_arg(ap, void*);
	p4 = va_arg(ap, void*);
	p5 = va_arg(ap, void*);
	p6 = va_arg(ap, void*);
	p7 = va_arg(ap, void*);
	p8 = va_arg(ap, void*);
	p9 = va_arg(ap, void*);
	p10 = va_arg(ap, void*);
	p11 = va_arg(ap, void*);
	p12 = va_arg(ap, void*);
	va_end(ap);

	char appdatadir[200];
	GetAppDataDir(appdatadir);
	sprintf(appdatadir, "%s%s", appdatadir, "\\debug.log");

	sprintf(pstring, format, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
	FILE *f_tmp = fopen(appdatadir, "ab");
	fprintf(f_tmp, pstring);
	fclose(f_tmp);
}

///按16进制显示缓冲区值,调试使用
/**
  * hint : 待显示的提示信息 \n
  * buffer : 待显示的缓冲区 \n
  * len : 待显示的缓冲区的大小
  */
void PrintBuffer(const char *hint, const unsigned char *buffer,int len)
{
	int	i;
	unsigned int	ui;

	Printf("%s(length=%d)\n",hint,len);
	if(len==0)
	{
		Printf("The number of data is zero!\n");
		return;
	}

	for(i=0;i<len;i++)
	{
		if((i%16==0)&&(i!=0)){
			Printf("\n", ui);
		}
		ui=(unsigned int)buffer[i];
		Printf("%02X ",ui);
	}
	Printf("\n");
	return;
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
	
	offset = str.Find("||");
	while(offset>0){
		tmp = str.Left(offset);
		g_strList.AddTail(tmp);
		str = str.Right(str.GetLength()-offset-2);
		offset = str.Find("||");
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

#define DATAFILE "d:\\test.dat"

void CreateDataFile()
{
	CFileStatus   status;
	if(!CFile::GetStatus(DATAFILE,status)){
		DataFile_Create(DATAFILE, sizeof(struct UserData));
	}
}

int Cmd_GetRecordNum()
{
	unsigned int num;
	int ret = DataFile_GetRecNum(DATAFILE, num);
	if(ret!=SUCC){
		return ret;
	}
	return num;
}

int Cmd_GetRecordAt(int index, struct UserData &data)
{
	return DataFile_ReadRec(DATAFILE, index, &data);
}

int Cmd_AppendRecord(struct UserData rec)
{
	return DataFile_AppendRec(DATAFILE, &rec);
}

int Cmd_DeleteRecordAt(int index)
{
	return DataFile_DeleteRec(DATAFILE, index);
}
