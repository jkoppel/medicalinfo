#include "stdafx.h"
#include "GlobalFuncs.h"
#include "GlobalVars.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//�ر���ʾ��
extern void HideProgressInfo();
//��ʾ�Ի���
extern int MyMessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption, UINT nType = MB_OK);

///��CStringת��char *
void CString2Char(CString source, char *dest)
{
	int length;
	length = source.GetLength();
	if(length>512){
		length = 512;
	}
	memset(dest, 0, 2*length+1);
	WideCharToMultiByte(
		CP_ACP, WC_COMPOSITECHECK|WC_DEFAULTCHAR,
		source.GetBuffer(length),
		length+1,
		dest,
		2*length,
		0,0);
	source.ReleaseBuffer();
}

///��char *תΪCString
void Char2CString(const char * buf, CString dst)
{
	dst = CString(buf);
}

///��ʾ��Ϣ
void ShowMsg(const char *msg)
{
	HideProgressInfo();
	MyMessageBox(CString(msg), CString("��Ϣ"), MB_ICONINFORMATION);
}

///��ʾ��Ϣ
void ShowMsg(CString msg)
{
	HideProgressInfo();
	MyMessageBox(msg, CString("��Ϣ"), MB_ICONINFORMATION);
}

///��ȫ�ֱ���g_strList�ڲ��������ַ�����װ�ɴ����͵�����ؼ�����"||"�ָ񣬿��ַ�����" "���
void MakeSeparatorString(CString &str)
{
	int num = (int)g_strList.GetCount();
	POSITION p;

	str.Empty();

	for(p=g_strList.GetHeadPosition();p!=NULL;num--){
		CString tmp = g_strList.GetNext(p);
		if(tmp.IsEmpty()){
			tmp = CString(" ");
		}
		if(num>1){
			str += tmp + CString("||");
		}
		else{
			str += tmp;
		}
	}
}

///������"||"�ָ����ַ����ɸ������ַ������洢��ȫ�ֱ���g_strList
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
	if(str.GetLength()>0){
		g_strList.AddTail(str);
	}
}

///����
void DebugSeparatorString()
{
	CString str, str1;
	g_strList.RemoveAll();
	char buf[20];
	for(int i=0;i<10;i++){
		sprintf_s(buf, "%d", i);
		g_strList.AddTail(CString("Hello")+CString(buf));
	}
	MakeSeparatorString(str);

	g_strList.RemoveAll();
	ParseSeparatorString(str);

	MakeSeparatorString(str1);
}

///���û�������װ�ɴ����͵���ӻ��޸�����
void MakeAddOrModRecordCmd(BOOL IsAdd, struct UserData data, CString &str)
{
	char buf[1024];

	g_strList.RemoveAll();

	if(IsAdd){
		sprintf_s(buf, "%s%d", "CMD", CMD_APPENDRECORD);
	}
	else{
		sprintf_s(buf, "%s%d", "CMD", CMD_EDITRECORDBYID);
	}
	g_strList.AddTail(CString(buf));

	sprintf_s(buf, "%d", data.ID);
	g_strList.AddTail(CString(buf));

	//ע�⣺������������Order�ֶ�

	sprintf_s(buf, "%d", data.Status);
	g_strList.AddTail(CString(buf));

	g_strList.AddTail(CString(data.ScancodeID));

	sprintf_s(buf, "%d", data.Number);
	g_strList.AddTail(CString(buf));

	g_strList.AddTail(CString(data.Name));
	g_strList.AddTail(CString(data.Sex));

	sprintf_s(buf, "%d", data.Age);
	g_strList.AddTail(CString(buf));

	g_strList.AddTail(CString(data.BirthDate));
	g_strList.AddTail(CString(data.People));
	g_strList.AddTail(CString(data.Department));
	g_strList.AddTail(CString(data.TypeOfWork));
	g_strList.AddTail(CString(data.Province));
	g_strList.AddTail(CString(data.City));
	g_strList.AddTail(CString(data.Address));
	g_strList.AddTail(CString(data.ZipCode));
	g_strList.AddTail(CString(data.Tel));
	g_strList.AddTail(CString(data.ClinicalDiagnosis));

	sprintf_s(buf, "%d", data.Height);
	g_strList.AddTail(CString(buf));

	g_strList.AddTail(CString(data.Weight));
	g_strList.AddTail(CString(data.CheckDate));
	g_strList.AddTail(CString(data.Hazards));
	g_strList.AddTail(CString(data.Pharmacy));
	g_strList.AddTail(CString(data.PastHistory));

	g_strList.AddTail(CString("\r\n"));

	MakeSeparatorString(str);
}

int ParseRecvDataToRec(CString str, struct UserData &data)
{
	char buf[2048];

	//�浽String List
	ParseSeparatorString(str);
	memset(&data, 0, sizeof(data));

	POSITION p = g_strList.GetHeadPosition();
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);//skip "OK"
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	sscanf_s(buf, "%d", &data.ID);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	sscanf_s(buf, "%d", &data.Order);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	sscanf_s(buf, "%d", &data.Status);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	_snprintf(data.ScancodeID, sizeof(data.ScancodeID), "%s", buf);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	sscanf_s(buf, "%d", &data.Number);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	sprintf_s(data.Name, "%s", buf);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	sprintf_s(data.Sex, "%s", buf);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	sscanf_s(buf, "%d", &data.Age);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	sprintf_s(data.BirthDate, "%s", buf);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	sprintf_s(data.People, "%s", buf);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	sprintf_s(data.Department, "%s", buf);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	sprintf_s(data.TypeOfWork, "%s", buf);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	sprintf_s(data.Province, "%s", buf);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	sprintf_s(data.City, "%s", buf);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	sprintf_s(data.Address, "%s", buf);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	sprintf_s(data.ZipCode, "%s", buf);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	sprintf_s(data.Tel, "%s", buf);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	sprintf_s(data.ClinicalDiagnosis, "%s", buf);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	sscanf_s(buf, "%d", &data.Height);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	sprintf_s(data.Weight, "%s", buf);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	sprintf_s(data.CheckDate, "%s", buf);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	sprintf_s(data.Hazards, "%s", buf);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	sprintf_s(data.Pharmacy, "%s", buf);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	CString2Char(str, buf);
	sprintf_s(data.PastHistory, "%s", buf);

	return TRUE;
}
