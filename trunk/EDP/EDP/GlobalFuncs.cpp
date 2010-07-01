#include "StdAfx.h"
#include "GlobalFuncs.h"

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

	//查看文件头
	memset(buf, 0, sizeof(buf));
	fread(buf, 23, 1, fp);
	sprintf_s(buf1, "%s", "MyTester Data File ");
	if(strncmp(buf, buf1, strlen(buf1))!=0){
		fclose(fp);
		return FALSE;
	}
	//获取版本号
	double fDataVer = 0.0;
	if(isdigit(buf[19]) && buf[20]=='.' && isdigit(buf[21])){
		fDataVer = atof(buf+19);
	}
	else{
		fclose(fp);
		return FALSE;
	}

	//略过6718字节
	if(fseek(fp, 6718, SEEK_SET)!=0){
		fclose(fp);
		return FALSE;
	}

	//直接读取数据
	ret = (int)fread(&rec, sizeof(rec), 1, fp);
	fclose(fp);

	return (ret==1);
}

BOOL SaveFile(const char *file, struct TestRecord rec)
{
	return TRUE;
}

