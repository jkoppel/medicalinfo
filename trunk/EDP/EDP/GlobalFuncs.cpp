#include "StdAfx.h"
#include "GlobalFuncs.h"
#include "GlobalVars.h"
#include "Mime\\Mime.h"
#include "Mime\\MimeCode.h"

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

BOOL LoadNode()
{
	LoadDirFromConfigFile();

	int i;
	CFileFind ff;
	BOOL bDecide = FALSE;
	CString str, name;
	char buf[1024];

	g_iDirNodeNum = g_saDirectories.GetCount();
	g_pDirNode = new struct TestRecordDirNode[g_iDirNodeNum];
	memset(g_pDirNode, 0, sizeof(struct TestRecordDirNode) * g_iDirNodeNum);

	for(i=0;i<g_saDirectories.GetCount();i++){
		str = g_saDirectories.GetAt(i);
		if(str.Right(1)!='\\'){
			str += "\\";
		}
		sprintf(g_pDirNode[i].sDir, "%s", str);
		g_pDirNode[i].pFileNode = new struct TestRecordFileNode[20];

		bDecide = ff.FindFile(str+"*.DFT");
		g_pDirNode[i].iNum = 0;
		while(bDecide){
			bDecide = ff.FindNextFile();  
			if(ff.IsDirectory() || ff.IsDots()){//目录或.,..
				continue;
			}
			name = str + ff.GetFileName();
			sprintf(buf, "%s", name.GetBuffer(0));
			name.ReleaseBuffer();
			if(LoadFile(buf, g_pDirNode[i].pFileNode[g_pDirNode[i].iNum].rec)){
				name = ff.GetFileName();
				sprintf(g_pDirNode[i].pFileNode[g_pDirNode[i].iNum].sFile, "%s", name.GetBuffer(0));
				name.ReleaseBuffer();
				for(int s=0;s<g_pDirNode[i].pFileNode[g_pDirNode[i].iNum].rec.iNumOfSpeed;s++){
					g_pDirNode[i].pFileNode[g_pDirNode[i].iNum].item_data[s].iIndex = s;
					g_pDirNode[i].pFileNode[g_pDirNode[i].iNum].item_data[s].pRec = &g_pDirNode[i].pFileNode[g_pDirNode[i].iNum].rec;
				}
				g_pDirNode[i].iNum ++;
			}
			if(g_pDirNode[i].iNum>=20){
				break;
			}
		}
		ff.Close();
	}
	return TRUE;
}

void ReleaseDirNode()
{
	if(g_pDirNode!=NULL){
		for(int i=0;i<g_iDirNodeNum;i++){
			if(g_pDirNode[i].pFileNode!=NULL){
				delete []g_pDirNode[i].pFileNode;
				g_pDirNode[i].pFileNode = NULL;
			}
		}
		delete []g_pDirNode;
		g_pDirNode = NULL;
	}
}

extern int MyMessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption, UINT nType = MB_OK);

void ShowMsg(const char *msg)
{
	MyMessageBox(msg, "消息", MB_ICONINFORMATION);
}

///检查字符串输入的日期格式是否正确
/**
  * StrDate : 待检查的日期,字符串格式 \n
  * 返回 : TRUE/FALSE
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

	//检查是否闰年:400整除,或者4整除但不被100整除
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

///检查字符串输入的时间格式是否正确
/**
  * StrTime : 待检查的时间,字符串格式 \n
  * 返回 : TRUE/FALSE
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

BOOL LoadDirFromConfigFile()
{
	CStdioFile file;
	if(!file.Open(CONFIG_DIR_FILE, CFile::modeNoTruncate | CFile::modeCreate)){
		return FALSE;
	}
	g_saDirectories.RemoveAll();
	char dir[1024];
	CString str;
	while(file.ReadString(dir, sizeof(dir))){
		if(strlen(dir)>0){
			if(dir[strlen(dir)-1]=='\n'){//去掉换行符
				dir[strlen(dir)-1]='\0';
			}
			str.Format("%s", dir);
			str.Trim();
			if(str.GetLength()>0){
				g_saDirectories.Add(str);
			}
		}
	}
	file.Close();
	return TRUE;
}

BOOL SaveDirToConfigFile()
{
	CStdioFile file;
	if(!file.Open(CONFIG_DIR_FILE, CFile::modeCreate | CFile::modeWrite)){
		return FALSE;
	}
	CString str;
	for(int i=0;i<g_saDirectories.GetCount();i++){
		str = g_saDirectories.GetAt(i);;
		str = str.Trim();
		if(str.GetLength()){
			str += CString("\n");
			file.WriteString(str);
		}
	}
	file.Close();
	return TRUE;
}

void TestMime()
{
    CMimeMessage mail;

    // Initialize message header

    mail.SetDate(); // set 'Date' field to the current time

    mail.SetVersion();
    mail.SetFrom("sender@local.com");
    mail.SetTo("recipient1@server1.com, Nick Name <recipient2@server1.com>, \"Nick Name\" <recipient3@server3.com>");
    mail.SetCc("recipient4@server4.com");
    mail.SetSubject("Test message");
    mail.SetFieldValue("X-Priority", "3 (Normal)"); // extended field

    mail.SetFieldValue("X-My-Field", "My value");   // user-defined field


    // Initialize body part header

    mail.SetContentType("multipart/mixed");
    // generate a boundary delimeter automatically

    // if the parameter is NULL

    mail.SetBoundary(NULL);

    // Add a text body part

    // Content-Type is not specified, so the default

    // value "text/plain" is implicitly used

    // Content-Transfer-Encoding is not specified

    // so the default value "7bit" is implicitly used

    CMimeBody* pBp = mail.CreatePart();
    pBp->SetText("Hi, there");  // set the content of the body part


    // Add a file attachment body part

    pBp = mail.CreatePart();
    pBp->SetDescription("enclosed photo");
    pBp->SetTransferEncoding("base64");
    // if Content-Type is not specified, it'll be

    // set to "image/jpeg" by ReadFromFile()

    pBp->ReadFromFile("test.jpg"); 

    // Generate a simple message

    CMimeMessage mail2;
    mail2.SetFrom("abc@abc.com");
    mail2.SetTo("abc@abc.com");
    mail2.SetSubject("This is an attached message");
    mail2.SetText("Content of attached message.\r\n");

    // Attach the message

    pBp = mail.CreatePart();
    pBp->SetDescription("enclosed message");
    pBp->SetTransferEncoding("7bit");
    // if Content-Type is not specified, it'll be

    // set to "message/rfc822" by SetMessage()

    pBp->SetMessage(&mail2); 

    // Add an embeded multipart

    pBp = mail.CreatePart();
    pBp->SetContentType("multipart/alternative");
    pBp->SetBoundary("embeded_multipart_boundary");
    CMimeBody *pBpChild = pBp->CreatePart();
    pBpChild->SetText("Content of Part 1\r\n");
    pBpChild = pBp->CreatePart();
    pBpChild->SetText("Content of Part 2\r\n");

    // Store the message to buffer    

    // fold the long lines in the headers

    CMimeEnvironment::SetAutoFolding(true); 
    int nSize = mail.GetLength();
    char* pBuff = new char[nSize];
    nSize = mail.Store(pBuff, nSize);

	CFile file;
	file.Open("Debug.mht", CFile::modeCreate | CFile::modeWrite);
	file.Write(pBuff, nSize);
	file.Close();

	delete pBuff;
}
