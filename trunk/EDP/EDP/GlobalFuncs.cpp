#include "StdAfx.h"
#include "GlobalFuncs.h"


int g_iRecNum = 0;
struct TestRecordNode *g_pRec = NULL;

BOOL LoadFile(const char *file, struct TestRecord &rec)
{
	int ret;
	FILE *fp = NULL;
	char buf[256], buf1[256];

	if(file==NULL || strlen(file)==0){
		return FALSE;
	}

	if(fopen_s(&fp, file, "rb")!=0){
		return FALSE;
	}
	if(fp==NULL){
		return FALSE;
	}

	//�鿴�ļ�ͷ
	memset(buf, 0, sizeof(buf));
	fread(buf, 23, 1, fp);
	sprintf_s(buf1, "%s", "MyTester Data File ");
	if(strncmp(buf, buf1, strlen(buf1))!=0){
		fclose(fp);
		return FALSE;
	}
	//��ȡ�汾��
	double fDataVer = 0.0;
	if(isdigit(buf[19]) && buf[20]=='.' && isdigit(buf[21])){
		fDataVer = atof(buf+19);
	}
	else{
		fclose(fp);
		return FALSE;
	}

	//�Թ�6718�ֽ�
	if(fseek(fp, 6718, SEEK_SET)!=0){
		fclose(fp);
		return FALSE;
	}

	//ֱ�Ӷ�ȡ����
	ret = (int)fread(&rec, sizeof(rec), 1, fp);
	fclose(fp);

	return (ret==1);
}

BOOL SaveFile(const char *file, struct TestRecord rec)
{
	return TRUE;
}

extern int MyMessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption, UINT nType = MB_OK);

void ShowMsg(const char *msg)
{
	MyMessageBox(msg, "��Ϣ", MB_ICONINFORMATION);
}

///����ַ�����������ڸ�ʽ�Ƿ���ȷ
/**
  * StrDate : ����������,�ַ�����ʽ \n
  * ���� : TRUE/FALSE
  */
BOOL CheckStrDateFormat(const char *StrDate, int &year, int &month, int &day)
{
	//Check Input
	if(StrDate==NULL){
		return FALSE;
	}
	if(strlen((char*)StrDate)!=8){
		return FALSE;
	}
	for(int i=0;i<8;i++){
		if(!isdigit(StrDate[i])){
			return FALSE;
		}
	}

	unsigned char buf_year[5];
	unsigned char buf_month[3];
	unsigned char buf_day[3];
	memcpy(buf_year, StrDate, 4); buf_year[4] = '\0';
	memcpy(buf_month, StrDate+4, 2); buf_month[2] = '\0';
	memcpy(buf_day, StrDate+6, 2); buf_day[2] = '\0';
	
	year = atoi((char*)buf_year);
	month = atoi((char*)buf_month);
	day = atoi((char*)buf_day);

	int flag;

	if( year < 1900 ){
		return FALSE;
	}
	if( month > 12 || month <= 0 ){
		return FALSE;
	}
	if( day<=0 ){
		return FALSE;
	}

	if(month==1 || month==3 || month==5 || 
	    month==7 || month==8 || month==10 || month==12){
		if(day <= 31){
			return TRUE;
		}
		else{
			return FALSE;
		}
	}

	if(month==4 || month==6 || month==9 || month==11){
		if(day<=30){
			return TRUE;
		}
		else{
			return FALSE;
		}
	}

	//����Ƿ�����:400����,����4����������100����
	if( ((year%400) == 0 ) || ( (year%4==0) && (year%100!=0) ) ){
		flag = 1;
	}
	else {
		flag = 0;
	}

	if(flag && day>29){
		return FALSE;
	}
	if(!flag && day>28){
		return FALSE;
	}

	return TRUE;
}

///����ַ��������ʱ���ʽ�Ƿ���ȷ
/**
  * StrTime : ������ʱ��,�ַ�����ʽ \n
  * ���� : TRUE/FALSE
  */
BOOL CheckStrTimeFormat(const char *StrTime, int &hour, int &minute)
{
	//Check Input
	if(StrTime==NULL){
		return FALSE;
	}
	if(strlen((char*)StrTime)!=5){
		return FALSE;
	}
	for(int i=0;i<5;i++){
		if(i!=2){
			if(!isdigit(StrTime[i])){
				return FALSE;
			}
		}
		else{
			if(StrTime[i]!=':'){
				return FALSE;
			}
		}
	}

	char buf_hour[3];
	char buf_minute[3];
	memcpy(buf_hour, StrTime, 2); buf_hour[2] = '\0';
	memcpy(buf_minute, StrTime+3, 2); buf_minute[2] = '\0';
	
	hour = atoi((char*)buf_hour);
	minute = atoi((char*)buf_minute);

	if( hour < 0 || hour>23 ){
		return FALSE;
	}
	if( minute <0 || minute >59 ){
		return FALSE;
	}

	return TRUE;
}
