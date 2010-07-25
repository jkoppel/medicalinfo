#include "GlobalFuncs.h"
#include <afxdisp.h>
#include "ExtLibs\\Mime\\Mime.h"
#include "ExtLibs\\Mime\\MimeCode.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

///TCHAR*类型转char*类型(注意TCHAR为宏，UNICODE下为wchar_t，ANSI下为char,调用本函数务必保证szDst的大小足够，至少为iLength的两倍
void TCHAR2char(char *szDst, const TCHAR *tcSrc, int iLength)
{
#ifndef _UNICODE
	_snprintf_s(szDst, iLength, "%s", tcSource, iLength);
#else
	if(szDst==NULL){
		return;
	}
	memset(szDst, 0, iLength*2+1);
	WideCharToMultiByte(
		CP_ACP, WC_COMPOSITECHECK|WC_DEFAULTCHAR,
		tcSrc,
		iLength,
		szDst,
		2*iLength,
		0,0);
#endif
}

///char*转为TCHAR*
void char2TCHAR(TCHAR *tcDst, const char *szSrc, int iLength)
{
#ifndef _UNICODE
	memcpy(szDst, tcSource, iLength);
#else
	CString strDst = CString(szSrc);
	swprintf_s(tcDst, iLength*2, _T("%s"), strDst.GetBuffer(strDst.GetAllocLength()));
	strDst.ReleaseBuffer();
#endif
}

///CString类型转为char*类型
void CString2char(char *szDst, CString strSrc)
{
	TCHAR2char(szDst, strSrc.GetBuffer(strSrc.GetLength()), strSrc.GetLength());
	strSrc.ReleaseBuffer();
}

///显示消息
void ShowMsg(const char *msg)
{
	CWnd *pMainWnd = AfxGetApp()->GetMainWnd();
	if(pMainWnd!=NULL){
		CWnd *pActiveWnd = pMainWnd->GetActiveWindow();
		if(pActiveWnd){
			pActiveWnd->MessageBox(CString(msg), _T("消息"), MB_ICONINFORMATION);
			return;
		}
	}
	AfxMessageBox(CString(msg));
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

BOOL GetTimeFromStr(const char *sTime, SYSTEMTIME &st)
{
	if(sTime == NULL){
		return FALSE;
	}

	char buf[1024];
	CString strTime;
	
	strTime = CString(sTime);
	strTime = strTime.Trim();
	CString2char(buf, strTime);

	char years[5] ={""}, month[3] = {""}, day[3] = {""};
	char hour[3] = {""}, minute[3] = {""};

	strncpy_s(years, 5, buf, 4);
	strncpy_s(month, 3, buf+5, 2);
	strncpy_s(day, 3, buf+8, 2);
	strncpy_s(hour, 3, buf+11, 2);
	strncpy_s(minute, 3, buf+14, 2);

	memset(&st, 0, sizeof(SYSTEMTIME));
	st.wYear = atoi(years);
	st.wMonth = atoi(month);
	st.wDay = atoi(day);
	st.wHour = atoi(hour);
	st.wMinute = atoi(minute);

	return TRUE;
}

///测试Mime
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

    CMimeEnvironment::SetAutoFolding(TRUE); 
    int nSize = mail.GetLength();
    char* pBuff = new char[nSize];
    nSize = mail.Store(pBuff, nSize);

	CFile file;
	file.Open(_T("Debug.mht"), CFile::modeCreate | CFile::modeWrite);
	file.Write(pBuff, nSize);
	file.Close();

	delete pBuff;
}

#include <stdio.h>
#include <string.h>

///在lpszSour中查找lpszFind,从nStart开始
int FindString(const char * lpszSour,const char * lpszFind,int nStart=0);
///带通配符'?','*'的字符串匹配,bMatchCase=TRUE区分大小写

///在lpszSour中查找字符串lpszFind，lpszFind中可以包含通配字符‘?’
/**
  * nStart : 在lpszSour中的起始查找位置 \n
  * 返回值 : 成功返回匹配位置，否则返回-1 \n
  * 注  意 : Called by “BOOL MatchString()”
  */
int FindingString(const char* lpszSour, const char* lpszFind, int nStart /* = 0 */)
{
	//	ASSERT(lpszSour && lpszFind && nStart >= 0);
	if(lpszSour == NULL || lpszFind == NULL || nStart < 0){
		return -1;
	}

	int m = strlen(lpszSour);
	int n = strlen(lpszFind);

	if( nStart+n > m ){
		return -1;
	}

	if(n == 0){
		return nStart;
	}

	//KMP算法
	int* next = new int[n];

	//得到查找字符串的next数组
	{
	n--;

	int j, k;
	j = 0;
	k = -1;
	next[0] = -1;

	while(j < n){
		if(k == -1 || lpszFind[k] == '?' || lpszFind[j] == lpszFind[k]){
			j++;
			k++;
			next[j] = k;
		}
		else{
			k = next[k];
		}
	}

	n++;
	}

	int i = nStart,	j = 0;
	while(i < m && j < n)
	{
		if(j == -1 || lpszFind[j] == '?' || lpszSour[i] == lpszFind[j])
		{	i++;
		j++;
		}
		else
			j = next[j];
	}

	delete []next;

	if(j >= n)
		return i-n;
	else
		return -1;
}

//功	  能：带通配符的字符串匹配
//参	  数：lpszSour是一个普通字符串；
//			  lpszMatch是一可以包含通配符的字符串；
//			  bMatchCase为0，不区分大小写，否则区分大小写。
//返  回  值：匹配，返回1；否则返回0。
//通配符意义：
//		‘*’	代表任意字符串，包括空字符串；
//		‘?’	代表任意一个字符，不能为空；
//时	  间：	2001.11.02	13:00
BOOL MatchString(const char* lpszSour, const char* lpszMatch, BOOL bMatchCase /*  = TRUE */)
{
	//	ASSERT(AfxIsValidString(lpszSour) && AfxIsValidString(lpszMatch));
	if(lpszSour == NULL || lpszMatch == NULL)
		return FALSE;

	if(lpszMatch[0] == 0)//Is a empty string
	{
		if(lpszSour[0] == 0)
			return TRUE;
		else
			return FALSE;
	}

	int i = 0, j = 0;

	//生成比较用临时源字符串'szSource'
	char* szSource =
		new char[ (j = strlen(lpszSour)+1) ];

	if( bMatchCase )
	{	//memcpy(szSource, lpszSour, j);
		while( *(szSource+i) = *(lpszSour+i++) );
	}
	else
	{	//Lowercase 'lpszSour' to 'szSource'
		i = 0;
		while(lpszSour[i])
		{	if(lpszSour[i] >= 'A' && lpszSour[i] <= 'Z')
		szSource[i] = lpszSour[i] - 'A' + 'a';
		else
			szSource[i] = lpszSour[i];

		i++;
		}
		szSource[i] = 0;
	}

	//生成比较用临时匹配字符串'szMatcher'
	char* szMatcher = new char[strlen(lpszMatch)+1];

	//把lpszMatch里面连续的“*”并成一个“*”后复制到szMatcher中
	i = j = 0;
	while(lpszMatch[i])
	{
		szMatcher[j++] = (!bMatchCase) ?
			( (lpszMatch[i] >= 'A' && lpszMatch[i] <= 'Z') ?//Lowercase lpszMatch[i] to szMatcher[j]
			lpszMatch[i] - 'A' + 'a' :
		lpszMatch[i]
		) :
		lpszMatch[i];		 //Copy lpszMatch[i] to szMatcher[j]
		//Merge '*'
		if(lpszMatch[i] == '*')
			while(lpszMatch[++i] == '*');
		else
			i++;
	}
	szMatcher[j] = 0;

	//开始进行匹配检查

	int nMatchOffset, nSourOffset;

	BOOL bIsMatched = TRUE;
	nMatchOffset = nSourOffset = 0;
	while(szMatcher[nMatchOffset]){
		if(szMatcher[nMatchOffset] == '*'){
			if(szMatcher[nMatchOffset+1] == 0){
				//szMatcher[nMatchOffset]是最后一个字符
				bIsMatched = TRUE;
				break;
			}
			else{
				//szMatcher[nMatchOffset+1]只能是'?'或普通字符
				int nSubOffset = nMatchOffset+1;

				while(szMatcher[nSubOffset]){
					if(szMatcher[nSubOffset] == '*'){
						break;
					}
					nSubOffset++;
				}

				if( strlen(szSource+nSourOffset) < size_t(nSubOffset-nMatchOffset-1) ){
					//源字符串剩下的长度小于匹配串剩下要求长度
					bIsMatched = FALSE; //判定不匹配
					break;			//退出
				}

				if(!szMatcher[nSubOffset]){//nSubOffset is point to ender of 'szMatcher'
					//检查剩下部分字符是否一一匹配

					nSubOffset--;
					int nTempSourOffset = strlen(szSource)-1;
					//从后向前进行匹配
					while(szMatcher[nSubOffset] != '*'){
						if(szMatcher[nSubOffset] != '?'){
							if(szMatcher[nSubOffset] != szSource[nTempSourOffset]){
								bIsMatched = FALSE;
								break;
							}
						}
						nSubOffset--;
						nTempSourOffset--;
					}
					break;
				}
				else{//szMatcher[nSubOffset] == '*'
					nSubOffset -= nMatchOffset;

					char* szTempFinder = new char[nSubOffset];
					nSubOffset--;
					memcpy(szTempFinder, szMatcher+nMatchOffset+1, nSubOffset);
					szTempFinder[nSubOffset] = 0;

					int nPos = ::FindingString(szSource+nSourOffset, szTempFinder, 0);
					delete []szTempFinder;

					if(nPos != -1){//在'szSource+nSourOffset'中找到szTempFinder
						nMatchOffset += nSubOffset;
						nSourOffset += (nPos+nSubOffset-1);
					}
					else{
						bIsMatched = FALSE;
						break;
					}
				}
			}
		}		//end of "if(szMatcher[nMatchOffset] == '*')"
		else if(szMatcher[nMatchOffset] == '?'){
			if(!szSource[nSourOffset]){
				bIsMatched = FALSE;
				break;
			}
			if(!szMatcher[nMatchOffset+1] && szSource[nSourOffset+1]){
				//如果szMatcher[nMatchOffset]是最后一个字符，
				//且szSource[nSourOffset]不是最后一个字符
				bIsMatched = FALSE;
				break;
			}
			nMatchOffset++;
			nSourOffset++;
		}
		else{//szMatcher[nMatchOffset]为常规字符
			if(szSource[nSourOffset] != szMatcher[nMatchOffset]){
				bIsMatched = FALSE;
				break;
			}
			if(szMatcher[nMatchOffset+1] && !szSource[nSourOffset+1]){
				bIsMatched = FALSE;
				break;
			}
			nMatchOffset++;
			nSourOffset++;
		}
	}

	delete []szSource;
	delete []szMatcher;
	return bIsMatched;
}

///测试字符串匹配
void TestStringMatch()
{
	char* str = "String match";
	BOOL bMatch;
	char* sMatch;

	sMatch = "*ing??*c*";
	bMatch = MatchString(str, sMatch);//单匹配
	printf("\"%s\" %s match with \"%s\"\n\n", str, bMatch ? "is" : "is not", sMatch);

	sMatch = "*ing??*c*??";
	bMatch = MatchString(str, sMatch);//单匹配
	printf("\"%s\" %s match with \"%s\"\n\n", str, bMatch ? "is" : "is not", sMatch);
}
