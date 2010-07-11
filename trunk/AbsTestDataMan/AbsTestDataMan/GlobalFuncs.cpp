#include "GlobalFuncs.h"
#include "AbsTestDataMan.h"
#include "GlobalVars.h"
#include "Mime\\Mime.h"
#include "Mime\\MimeCode.h"
#include "ProgressThread.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

///TCHAR类型转char*类型(注意TCHAR为宏，UNICODE下为wchar_t，ANSI下为char,调用本函数务必保证szDst的大小足够，至少为iLength的两倍
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

void char2TCHAR(TCHAR *tcDst, const char *szSrc, int iLength)
{
#ifndef _UNICODE
	memcpy(szDst, tcSource, iLength);
#else
	size_t convertedChars;

	mbstowcs_s(&convertedChars, tcDst, iLength+1, szSrc, _TRUNCATE);
#endif
}

void CString2char(char *szDst, CString strSrc)
{
	TCHAR2char(szDst, strSrc.GetBuffer(strSrc.GetLength()), strSrc.GetLength());
	strSrc.ReleaseBuffer();
}


BOOL LoadFile(const char *file, struct TestRecordFileNode &node)
{
	int ret, i, j;
	FILE *fp = NULL;
	char buf[256], buf1[256];
	double min, max, avg;

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
	if(fseek(fp, 30, SEEK_SET)!=0){
		fclose(fp);
		return FALSE;
	}

	ret = (int)fread(&node.machine_info, sizeof(struct CCMachineInfo), 1, fp);
	if(ret!=1){
		fclose(fp);
		return FALSE;
	}

	ret = (int)fread(&node.product_info, sizeof(struct CCProductInfo), 1, fp);
	if(ret!=1){
		fclose(fp);
		return FALSE;
	}

	//直接读取数据
	ret = (int)fread(&node.test_record, sizeof(struct CCTestRecord), 1, fp);
	fclose(fp);
	if(ret!=1){
		return FALSE;
	}

	char2TCHAR(node.addition_info.sFile, file, strlen(file)+1);

	memset(node.tree_item_data, 0, sizeof(node.tree_item_data));
	node.bDataProcessed = FALSE;
	for(i=0;i<node.test_record.iNumOfSpeed;i++){
		node.tree_item_data[i].iIndex = i;
		node.tree_item_data[i].pNode = &node;
	}

	//对文件做处理
	memset(&node.addition_info, 0, sizeof(node.addition_info));
	if(node.test_record.bDataValid==FALSE){
		return TRUE;
	}
	if(node.test_record.bNormalSpeed){
		for(i=0;i<node.test_record.iNumOfSpeed;i++){
			j = node.test_record.iSpdIndex[i];
			node.test_record.fSetSpeed[i] = node.product_info.fSpeed0[j];
			if(node.product_info.bDifferentOffset){
				node.test_record.fSetOffset[i] = node.product_info.fSpeedOffset[j];
			}
			else{
				node.test_record.fSetOffset[i] = node.product_info.fOffset;
			}

			min = node.test_record.fDisplacement[i][0];
			max = min;
			for(j=0;j<node.test_record.iNumOfSpeed;j++){
				if(node.test_record.fDisplacement[i][j] < min){
					min = node.test_record.fDisplacement[i][j];
				}
				if(node.test_record.fDisplacement[i][j] > max){
					max = node.test_record.fDisplacement[i][j];
				}
			}
			avg = (max+min)/2;
			for(j=0;j<node.test_record.iNumOfForce[i];j++){
				node.test_record.fDisplacement[i][j] -= avg;
			}
			node.addition_info.fDisplacementLength[i] = (max-min)/2;

			int P[7];
			int flag[7];
			int num = 0;
			for(j=100;j<node.test_record.iNumOfForce[i]-1;j++){
				if(node.test_record.fDisplacement[i][j]<0 && node.test_record.fDisplacement[i][j+1]>=0){
					P[num] = j+1;
					flag[num] = 0;
					num++;
				}
				else if(node.test_record.fDisplacement[i][j]>0 && node.test_record.fDisplacement[i][j+1]<=0){
					P[num] = j+1;
					flag[num] = 1;
					num++;
				}
				if(num>=7){
					break;
				}
			}
			int T = (P[2] - P[1])/2;
			for(j=0;j<num;j++){
				if(flag[j]==1 &&
					P[j]+T>=node.product_info.iDataBandStart &&
					P[j]+T<=node.product_info.iDataBandStart+node.product_info.iDataBandLen){
						node.addition_info.iDataBandStart[i] = P[j] + T;
						node.addition_info.iDataBandLen[i] = (P[2] - P[1])*2;
						break;
				}
			}
			node.test_record.fRealSpeed[i] = 2 * M_PI * node.test_record.fDataFreq[i] / node.addition_info.iDataBandLen[i] * node.test_record.fRealSpeed[i];

			memcpy(node.addition_info.fForceOfFilter[i]+node.addition_info.iDataBandStart[i],
					node.test_record.fForce[i]+node.addition_info.iDataBandStart[i],
					node.addition_info.iDataBandLen[i] * sizeof(double));
			filter_new(node.addition_info.fForceOfFilter[i] + node.addition_info.iDataBandStart[i],
						node.test_record.fDataFreq[i],
						10*node.test_record.fSetSpeed[i]/node.test_record.fSetOffset[i]/(2*M_PI), 
						node.addition_info.iDataBandLen[i]);

			min = node.addition_info.fForceOfFilter[i][node.addition_info.iDataBandStart[i]];
			max = node.addition_info.fForceOfFilter[i][node.addition_info.iDataBandStart[i]];
			for(j=0;j<node.addition_info.iDataBandLen[i];j++){
				if(node.addition_info.fForceOfFilter[i][node.addition_info.iDataBandStart[i]+j]<min){
					min = node.addition_info.fForceOfFilter[i][node.addition_info.iDataBandStart[i]+j];
				}
				if(node.addition_info.fForceOfFilter[i][node.addition_info.iDataBandStart[i]+j]>max){
					max = node.addition_info.fForceOfFilter[i][node.addition_info.iDataBandStart[i]+j];
				}
			}
			node.test_record.fPfm[i] = max;
			node.test_record.fPym[i] = min;
		}
	}

	/*
	if(node.test_record.bFrictionSpeed){
		for(i=0;i<node.test_record.iFrictionNumOfSpeed;i++){
			j = node.test_record.iFrictionSpdIndex[i];
			node.test_record.fFrictionSetSpeed[i] = node.product_info.fSpeed0[j];
			if(node.product_info.bDifferentOffset){
				node.test_record.fFrictionSetOffset[i] = node.product_info.fSpeedOffset[j];
			}
			else{
				node.test_record.fFrictionSetOffset[i] = node.product_info.fOffset;
			}

			min = node.test_record.fFrictionDisplacement[i][0];
			max = min;
			for(j=0;j<node.test_record.iFrictionNumOfSpeed;j++){
				if(node.test_record.fFrictionDisplacement[i][j] < min){
					min = node.test_record.fFrictionDisplacement[i][j];
				}
				if(node.test_record.fFrictionDisplacement[i][j] > max){
					max = node.test_record.fFrictionDisplacement[i][j];
				}
			}
			avg = (max+min)/2;
			for(j=0;j<node.test_record.iFrictionNumOfForce[i];j++){
				node.test_record.fFrictionDisplacement[i][j] -= avg;
			}
			node.addition_info.fFrictionDisplacementLength[i] = (max-min)/2;

			int P[7];
			int flag[7];
			int num = 0;
			for(j=100;j<node.test_record.iFrictionNumOfForce[i]-1;j++){
				if(node.test_record.fFrictionDisplacement[i][j]<0 && node.test_record.fFrictionDisplacement[i][j+1]>=0){
					P[num] = j+1;
					flag[num] = 0;
					num++;
				}
				else if(node.test_record.fFrictionDisplacement[i][j]>0 && node.test_record.fFrictionDisplacement[i][j+1]<=0){
					P[num] = j+1;
					flag[num] = 1;
					num++;
				}
				if(num>=7){
					break;
				}
			}
			int PP[2];
			int T = (P[2] - P[1])/2;
			for(j=0;j<num;j++){
				if(flag[j]==1 &&
					P[j]+T>=node.product_info.iDataBandStart &&
					P[j]+T<=node.product_info.iDataBandStart+node.product_info.iDataBandLen){
						node.addition_info.iDataBandStart[i] = P[j] + T;
						node.addition_info.iDataBandLen[i] = (P[2] - P[1])*2;
						break;
				}
			}
			node.test_record.fFrictionRealSpeed[i] = 2 * M_PI * node.test_record.fFrictionDataFreq[i] / node.addition_info.iFrictionDataBandLen[i] * node.test_record.fFrictionRealSpeed[i];

			memcpy(node.addition_info.fForceOfFilter[i]+node.addition_info.iDataBandStart[i],
					node.test_record.fForce[i]+node.addition_info.iDataBandStart[i],
					node.addition_info.iDataBandLen[i] * sizeof(double));
			filter_new(node.addition_info.fForceOfFilter[i] + node.addition_info.iDataBandStart[i],
						node.test_record.fDataFreq[i],
						10*node.test_record.fSetSpeed[i]/node.test_record.fSetOffset[i]/(2*M_PI), 
						node.addition_info.iDataBandLen[i]);

			min = node.addition_info.fForceOfFilter[i][node.addition_info.iDataBandStart[i]];
			max = node.addition_info.fForceOfFilter[i][node.addition_info.iDataBandStart[i]];
			for(j=0;j<node.addition_info.iDataBandLen[i];j++){
				if(node.addition_info.fForceOfFilter[i][node.addition_info.iDataBandStart[i]+j]<min){
					min = node.addition_info.fForceOfFilter[i][node.addition_info.iDataBandStart[i]+j];
				}
				if(node.addition_info.fForceOfFilter[i][node.addition_info.iDataBandStart[i]+j]>max){
					max = node.addition_info.fForceOfFilter[i][node.addition_info.iDataBandStart[i]+j];
				}
			}
			node.test_record.fPfm[i] = max;
			node.test_record.fPym[i] = min;
		}
	}
	*/

	node.bDataProcessed = TRUE;
	return TRUE;
}

BOOL SaveFile(const char *file, struct CCTestRecord rec)
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

	g_iDirNodeNum = (int)g_saDirectories.GetCount();
	g_pDirNode = new struct TestRecordDirNode[g_iDirNodeNum];
	memset(g_pDirNode, 0, sizeof(struct TestRecordDirNode) * g_iDirNodeNum);

	for(int k=0;k<50;k++)
	for(i=0;i<g_saDirectories.GetCount();i++){
		if(CDlgProgress::m_pDlg){
			::SendMessage(CDlgProgress::m_pDlg->m_hWnd, WM_UPDATEDATA, 0, 0);
		}

		if(!PeekAndPump()){
			break;
		}

		str = g_saDirectories.GetAt(i);
		if(str.Right(1)!=_T('\\')){
			str += _T("\\");
		}
		swprintf(g_pDirNode[i].sDir, _T("%s"), str.GetBuffer(str.GetLength()));;
		g_pDirNode[i].pFileNode = new struct TestRecordFileNode[20];

		bDecide = ff.FindFile(str+_T("*.DFT"));
		g_pDirNode[i].iNum = 0;
		while(bDecide){
			bDecide = ff.FindNextFile();  
			if(ff.IsDirectory() || ff.IsDots()){//目录或.,..
				continue;
			}
			name = str + ff.GetFileName();
			CString2char(buf, name);
			if(LoadFile(buf, g_pDirNode[i].pFileNode[g_pDirNode[i].iNum])){
				name = ff.GetFileName();
				swprintf(g_pDirNode[i].pFileNode[g_pDirNode[i].iNum].addition_info.sFile, _T("%s"), name);
				//CString2char(g_pDirNode[i].pFileNode[g_pDirNode[i].iNum].addition_info.sFile, name);;
				g_pDirNode[i].iNum ++;
			}
			if(g_pDirNode[i].iNum>=20){
				break;
			}

			if(CDlgProgress::m_pDlg){
				::SendMessage(CDlgProgress::m_pDlg->m_hWnd, WM_UPDATEDATA, 0, 0);
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

BOOL IsFileInConfigDirList(CString strFile)
{
	int i;
	CString strDir;

	for(i=0;i<g_saDirectories.GetCount();i++){
		strDir = g_saDirectories.GetAt(i);
		if(strFile.Find(strDir)>=0){
			return TRUE;
		}
	}
	return FALSE;
}

BOOL IsFileInConfigDirList(const char *sFile)
{
	return IsFileInConfigDirList(CString(sFile));
}

extern int MyMessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption, UINT nType = MB_OK);

void ShowMsg(const char *msg)
{
	//MyMessageBox(msg, "消息", MB_ICONINFORMATION);
	AfxGetApp()->GetMainWnd()->GetActiveWindow()->MessageBox(CString(msg), _T("消息"), MB_ICONINFORMATION);
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
	if(!file.Open(_T(CONFIG_DIR_FILE), CFile::modeNoTruncate | CFile::modeCreate)){
		return FALSE;
	}
	g_saDirectories.RemoveAll();
	TCHAR dir[1024];
	CString str;
	while(file.ReadString(dir, sizeof(dir)/sizeof(TCHAR))){
		if(_tcslen(dir)>0){
			if(dir[_tcslen(dir)-1]==_T('\n')){//去掉换行符
				dir[_tcslen(dir)-1]=_T('\0');
			}
			str.Format(_T("%s"), dir);
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
	if(!file.Open(_T(CONFIG_DIR_FILE), CFile::modeCreate | CFile::modeWrite)){
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
	file.Open(_T("Debug.mht"), CFile::modeCreate | CFile::modeWrite);
	file.Write(pBuff, nSize);
	file.Close();

	delete pBuff;
}

// 返回值： 0: 正常， －1: 内存不足 
int filter_new( 
			   double dat[],     // 数据数组 
			   double fs,      // 采样频率 Hz 
			   double fc,       // 滤波频率 Hz 
			   int num        // 数据个数 
			   ) 
{ 
	register int i,j; 

	int Len=31; 
	double *Coef; 
	double *X; 
	X= (double *)calloc(num+Len+50, sizeof(double)); 
	if(X==NULL) 
		return -1; 
	Len=fir_dsgn(Len, fs, fc, &Coef); 
	if(Len<0) 
	{ 
		free(X); 

		return -1; 
	} 

	for(i=0;i<num+Len;i++) 
	{ 
		X[i]=0; 
		if(i<num) 
			X[i]=dat[i]*Coef[0]; 
		for(j=1;j<=Len;j++) 
		{ 
			if(i>=j && i-j<num) 
				X[i]+=dat[i-j]*Coef[j]; 
			else if(i<j) 
				X[i]+=dat[0]*Coef[j]; 
			else if(i-j>=num) 
				X[i]+=dat[num-1]*Coef[j]; 

		} 
	} 

	memcpy(dat,&X[Len/2],sizeof(double)*num); 
	free(X); 
	free(Coef); 
	return 0; 
} 


/*---------------------------------------------------------------*/ 
/* Design FIR filter using window method. Hamming window is used */ 
/* If sucess, return a point to the filter coefficient array,    */ 
/* otherwise, return NULL. Calling program should release the    */ 
/* allocated memory in this subroutine                           */ 
/*                                                               */ 
/*                                                               */ 
/*  Suppose sampling rate is 2 Hz                                */ 
/*                                                               */ 
/*  Len : filter length, should be ODD and Len>=3                */ 
/*  CutLow : low cutoff, when lowpass, CutLow = 0.0              */ 
/*  CutHigh: high cutoff, when highpass, CutHigh = 1.0           */ 
/*  when bandpass,    0.0 < CutLow < CutHigh < 1.0               */ 
/*                                                               */ 
/*  example:                                                     */ 
/*      Coef = fir_dsgn(127, 0.3, 0.8);                          */ 
/*   return a bandpass filter                                    */ 
/*---------------------------------------------------------------*/ 
int fir_dsgn(int Len, double FreqS, double FreqB , double **Coef1)  
{ 

	double Sum, TmpFloat; 
	int CoefNum, HalfLen, Cnt; 
	double CutLow, CutHigh; 

	CutLow  = 0; 

	CutHigh = FreqB/FreqS*2.0; 

	/*---------------------------------------------*/ 
	/* adjust the number of coefficients to be ODD */ 
	/*---------------------------------------------*/ 
	CoefNum = Len; 
	if (Len % 2 == 0)  
	{ 
		CoefNum++; 
	} 
	HalfLen = (CoefNum - 1) / 2; 

	//-------------------------------------------------------- 
	// Allocate memory for coefficients if length changed 
	//-------------------------------------------------------- 

	double *Coef; 
	*Coef1 = (double *)calloc(CoefNum+50, sizeof(double)); 
	if (*Coef1 == NULL)  
	{ 
		return (-1); 
	} 
	int Order = CoefNum; 
	Coef = *Coef1; 

	double Pi=3.14159265; 


	/*------------------*/ 
	/*  Lowpass filter  */ 
	/*------------------*/ 
	if ((CutLow == 0.0) && (CutHigh < 1.0))  
	{ 

		Coef[HalfLen] = CutHigh; 
		for (Cnt=1; Cnt<=HalfLen; Cnt++)  
		{ 
			TmpFloat = Pi * Cnt; 
			Coef[HalfLen + Cnt] = sin(CutHigh * TmpFloat) / TmpFloat; 
			Coef[HalfLen - Cnt] = Coef[HalfLen + Cnt]; 
		} 

		/*------------------------------*/ 
		/* multiplying with a window    */ 
		/*------------------------------*/ 
		TmpFloat = 2.0 * Pi / (CoefNum - 1.0); 
		Sum = 0.0; 
		for (Cnt=0; Cnt<CoefNum; Cnt++)  
		{ 
			Coef[Cnt] *= (0.54 - 0.46 * cos(TmpFloat * Cnt)); 
			Sum += Coef[Cnt]; 
		} 


		/*------------------------------*/ 
		/* Normalize GAIN to 1          */ 
		/*------------------------------*/ 
		for (Cnt=0; Cnt<CoefNum; Cnt++)  
		{ 
			Coef[Cnt] /= fabs (Sum); 
		} 
		return (Order); 

	}  /* if Lowpass */ 


	/*------------------*/ 
	/* Highpass filter  */ 
	/*------------------*/ 
	if ((CutLow > 0.0) && (CutHigh == 1.0))  
	{ 

		Coef[HalfLen] = CutLow; 
		for (Cnt=1; Cnt<=HalfLen; Cnt++)  
		{ 
			TmpFloat = Pi * Cnt; 
			Coef[HalfLen + Cnt] = sin(CutLow * TmpFloat) / TmpFloat; 
			Coef[HalfLen - Cnt] = Coef[HalfLen + Cnt]; 
		} 


		/*------------------------------*/ 
		/* multiplying with a window    */ 
		/*------------------------------*/ 
		TmpFloat = 2.0 * Pi / (CoefNum - 1.0); 
		Sum = 0.0; 
		for (Cnt=0; Cnt<CoefNum; Cnt++)  
		{ 
			Coef[Cnt] *= -(0.54 - 0.46 * cos(TmpFloat * Cnt)); 
			if (Cnt % 2 == 0) Sum += Coef[Cnt];  /* poly(-1), even +, odd -*/ 
			else Sum -= Coef[Cnt] ; 
		} 

		Coef[HalfLen] += 1; 
		Sum += 1; 

		/*------------------------------*/ 
		/* Normalize GAIN to 1          */ 
		/*------------------------------*/ 
		for (Cnt=0; Cnt<CoefNum; Cnt++)  
		{ 
			Coef[Cnt] /= fabs (Sum); 
		}   
		return (Order); 

	} /* if HighPass */ 



	/*------------------*/ 
	/* Bandpass filter  */ 
	/*------------------*/ 
	if ((CutLow > 0.0) && (CutHigh < 1.0) && (CutLow < CutHigh))  
	{ 

		Coef[HalfLen] = CutHigh - CutLow; 
		for (Cnt=1; Cnt<=HalfLen; Cnt++)  
		{ 
			TmpFloat = Pi * Cnt; 
			Coef[HalfLen + Cnt] = 2.0 * sin( (CutHigh - CutLow) / 2.0 * TmpFloat) * 
				cos( (CutHigh + CutLow) / 2.0 * TmpFloat) / TmpFloat; 
			Coef[HalfLen - Cnt] = Coef[HalfLen + Cnt]; 
		} 

		/*------------------------------*/ 
		/* multiplying with a window    */ 
		/*------------------------------*/ 
		TmpFloat = 2.0 * Pi / (CoefNum - 1.0); 
		Sum = 0.0; 
		for (Cnt=0; Cnt<CoefNum; Cnt++)  
		{ 
			Coef[Cnt] *= (0.54 - 0.46 * cos(TmpFloat * Cnt)); 
			Sum += Coef[Cnt]; 
		} 


		return (Order); 

	} /* if */ 

	/*------------------*/ 
	/* Bandstop filter  */ 
	/*------------------*/ 
	if ((CutLow > 0.0) && (CutHigh < 1.0) && (CutLow>CutHigh))  
	{ 

		Coef[HalfLen] = CutLow - CutHigh; 
		for (Cnt=1; Cnt<=HalfLen; Cnt++)  
		{ 
			TmpFloat = Pi * Cnt; 
			Coef[HalfLen + Cnt] = 2.0 * sin( (CutLow - CutHigh) / 2.0 * TmpFloat) * 
				cos( (CutHigh + CutLow) / 2.0 * TmpFloat) / TmpFloat; 
			Coef[HalfLen - Cnt] = Coef[HalfLen + Cnt]; 
		} 

		/*------------------------------*/ 
		/* multiplying with a window    */ 
		/*------------------------------*/ 
		TmpFloat = 2.0 * Pi / (CoefNum - 1.0); 
		Sum = 0.0; 
		for (Cnt=0; Cnt<CoefNum; Cnt++)  

		{ 
			Coef[Cnt] *= -(0.54 - 0.46 * cos(TmpFloat * Cnt)); 
			Sum += Coef[Cnt]; 
		} 

		Coef[HalfLen] += 1; 
		Sum += 1; 

		/*------------------------------*/ 
		/* Normalize GAIN to 1          */ 
		/*------------------------------*/ 
		for (Cnt=0; Cnt<CoefNum; Cnt++)  
		{ 
			Coef[Cnt] /= fabs (Sum); 
		} 
		return (Order); 

	}  /* if */ 

	return (Order);    /* never reach here */ 
} 



/*
本主题演示如何将各种 C++ 字符串类型转换为其他字符串。可以转换的字符串类型包括 char *、wchar_t*、_bstr_t、CComBSTR、CString、basic_string 和 System.String。在所有情况下，在将字符串转换为新类型时，都会创建字符串的副本。对新字符串进行的任何更改都不会影响原始字符串，反之亦然。

从 char * 转换
示例
说明
此示例演示如何从 char * 转换为上面列出的其他字符串类型。

// convert_from_char.cpp
// compile with /clr /link comsuppw.lib
#include <iostream>
#include <stdlib.h>
#include <string>
#include "atlbase.h"
#include "atlstr.h"
#include "comutil.h"
using namespace std;
using namespace System;
int main()
{
	char *orig = "Hello, World!";
	cout << orig << " (char *)" << endl;
	// Convert to a wchar_t*
	size_t origsize = strlen(orig) + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t wcstring[newsize];
	mbstowcs_s(&convertedChars, wcstring, origsize, orig, _TRUNCATE);
	wcscat_s(wcstring, L" (wchar_t *)");
	wcout << wcstring << endl;
	// Convert to a _bstr_t
	_bstr_t bstrt(orig);
	bstrt += " (_bstr_t)";
	cout << bstrt << endl;
	// Convert to a CComBSTR
	CComBSTR ccombstr(orig);
	if (ccombstr.Append(L" (CComBSTR)") == S_OK)
	{
		CW2A printstr(ccombstr);
		cout << printstr << endl;
	}
	// Convert to a CString
	CString cstring(orig);
	cstring += " (CString)";
	cout << cstring << endl;
	// Convert to a basic_string
	string basicstring(orig);
	basicstring += " (basic_string)";
	cout << basicstring << endl;
	// Convert to a System::String
	String ^systemstring = gcnew String(orig);
	systemstring += " (System::String)";
	Console::WriteLine("{0}", systemstring);
	delete systemstring;
}
输出
Hello, World! (char *)
Hello, World! (wchar_t *)
Hello, World! (_bstr_t)
Hello, World! (CComBSTR)
Hello, World! (CString)
Hello, World! (basic_string)
Hello, World! (System::String)
从 wchar_t * 转换
示例
说明
此示例演示如何从 wchar_t * 转换为上面列出的其他字符串类型。

// convert_from_wchar_t.cpp
// compile with /clr /link comsuppw.lib
#include <iostream>
#include <stdlib.h>
#include <string>
#include "atlbase.h"
#include "atlstr.h"
#include "comutil.h"
using namespace std;
using namespace System;
int main()
{
	wchar_t *orig = L"Hello, World!";
	wcout << orig << L" (wchar_t *)" << endl;
	// Convert to a char*
	size_t origsize = wcslen(orig) + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	char nstring[newsize];
	wcstombs_s(&convertedChars, nstring, origsize, orig, _TRUNCATE);
	strcat_s(nstring, " (char *)");
	cout << nstring << endl;
	// Convert to a _bstr_t
	_bstr_t bstrt(orig);
	bstrt += " (_bstr_t)";
	cout << bstrt << endl;
	// Convert to a CComBSTR
	CComBSTR ccombstr(orig);
	if (ccombstr.Append(L" (CComBSTR)") == S_OK)
	{
		CW2A printstr(ccombstr);
		cout << printstr << endl;
	}
	// Convert to a CString
	CString cstring(orig);
	cstring += " (CString)";
	cout << cstring << endl;
	// Convert to a basic_string
	wstring basicstring(orig);
	basicstring += L" (basic_string)";
	wcout << basicstring << endl;
	// Convert to a System::String
	String ^systemstring = gcnew String(orig);
	systemstring += " (System::String)";
	Console::WriteLine("{0}", systemstring);
	delete systemstring;
}
输出
Hello, World! (wchar_t *)
Hello, World! (char *)
Hello, World! (_bstr_t)
Hello, World! (CComBSTR)
Hello, World! (CString)
Hello, World! (basic_string)
Hello, World! (System::String)
从 _bstr_t 转换
示例
说明
此示例演示如何从 _bstr_t 转换为上面列出的其他字符串类型。

// convert_from_bstr_t.cpp
// compile with /clr /link comsuppw.lib
#include <iostream>
#include <stdlib.h>
#include <string>
#include "atlbase.h"
#include "atlstr.h"
#include "comutil.h"
using namespace std;
using namespace System;
int main()
{
	_bstr_t orig("Hello, World!");
	wcout << orig << " (_bstr_t)" << endl;
	// Convert to a char*
	const size_t newsize = 100;
	char nstring[newsize];
	strcpy_s(nstring, (char *)orig);
	strcat_s(nstring, " (char *)");
	cout << nstring << endl;
	// Convert to a wchar_t*
	wchar_t wcstring[newsize];
	wcscpy_s(wcstring, (wchar_t *)orig);
	wcscat_s(wcstring, L" (wchar_t *)");
	wcout << wcstring << endl;
	// Convert to a CComBSTR
	CComBSTR ccombstr((char *)orig);
	if (ccombstr.Append(L" (CComBSTR)") == S_OK)
	{
		CW2A printstr(ccombstr);
		cout << printstr << endl;
	}
	// Convert to a CString
	CString cstring((char *)orig);
	cstring += " (CString)";
	cout << cstring << endl;
	// Convert to a basic_string
	string basicstring((char *)orig);
	basicstring += " (basic_string)";
	cout << basicstring << endl;
	// Convert to a System::String
	String ^systemstring = gcnew String((char *)orig);
	systemstring += " (System::String)";
	Console::WriteLine("{0}", systemstring);
	delete systemstring;
}
输出
Hello, World! (_bstr_t)
Hello, World! (char *)
Hello, World! (wchar_t *)
Hello, World! (CComBSTR)
Hello, World! (CString)
Hello, World! (basic_string)
Hello, World! (System::String)
从 CComBSTR 转换
示例
说明
此示例演示如何从 CComBSTR 转换为上面列出的其他字符串类型。

// convert_from_ccombstr.cpp
// compile with /clr /link comsuppw.lib
#include <iostream>
#include <stdlib.h>
#include <string>
#include "atlbase.h"
#include "atlstr.h"
#include "comutil.h"
#include "vcclr.h"
using namespace std;
using namespace System;
using namespace System::Runtime::InteropServices;
int main()
{
	CComBSTR orig("Hello, World!");
	CW2A printstr(orig);
	cout << printstr << " (CComBSTR)" << endl;
	// Convert to a char*
	const size_t newsize = 100;
	char nstring[newsize];
	CW2A tmpstr1(orig);
	strcpy_s(nstring, tmpstr1);
	strcat_s(nstring, " (char *)");
	cout << nstring << endl;
	// Convert to a wchar_t*
	wchar_t wcstring[newsize];
	wcscpy_s(wcstring, orig);
	wcscat_s(wcstring, L" (wchar_t *)");
	wcout << wcstring << endl;
	// Convert to a _bstr_t
	_bstr_t bstrt(orig);
	bstrt += " (_bstr_t)";
	cout << bstrt << endl;
	// Convert to a CString
	CString cstring(orig);
	cstring += " (CString)";
	cout << cstring << endl;
	// Convert to a basic_string
	wstring basicstring(orig);
	basicstring += L" (basic_string)";
	wcout << basicstring << endl;
	// Convert to a System::String
	String ^systemstring = gcnew String(orig);
	systemstring += " (System::String)";
	Console::WriteLine("{0}", systemstring);
	delete systemstring;
}
输出
Hello, World! (CComBSTR)
Hello, World! (char *)
Hello, World! (wchar_t *)
Hello, World! (_bstr_t)
Hello, World! (CString)
Hello, World! (basic_string)
Hello, World! (System::String)
从 CString 转换
示例
说明
此示例演示如何从 CString 转换为上面列出的其他字符串类型。

// convert_from_cstring.cpp
// compile with /clr /link comsuppw.lib
#include <iostream>
#include <stdlib.h>
#include <string>
#include "atlbase.h"
#include "atlstr.h"
#include "comutil.h"
using namespace std;
using namespace System;
int main()
{
	CString orig("Hello, World!");
	wcout << orig << " (CString)" << endl;
	// Convert to a char*
	const size_t newsize = 100;
	char nstring[newsize];
	strcpy_s(nstring, orig);
	strcat_s(nstring, " (char *)");
	cout << nstring << endl;
	// Convert to a wchar_t*
	// You must first convert to a char * for this to work.
	size_t origsize = strlen(orig) + 1;
	size_t convertedChars = 0;
	wchar_t wcstring[newsize];
	mbstowcs_s(&convertedChars, wcstring, origsize, orig, _TRUNCATE);
	wcscat_s(wcstring, L" (wchar_t *)");
	wcout << wcstring << endl;
	// Convert to a _bstr_t
	_bstr_t bstrt(orig);
	bstrt += " (_bstr_t)";
	cout << bstrt << endl;
	// Convert to a CComBSTR
	CComBSTR ccombstr(orig);
	if (ccombstr.Append(L" (CComBSTR)") == S_OK)
	{
		CW2A printstr(ccombstr);
		cout << printstr << endl;
	}
	// Convert to a basic_string
	string basicstring(orig);
	basicstring += " (basic_string)";
	cout << basicstring << endl;
	// Convert to a System::String
	String ^systemstring = gcnew String(orig);
	systemstring += " (System::String)";
	Console::WriteLine("{0}", systemstring);
	delete systemstring;
}
输出
Hello, World! (CString)
Hello, World! (char *)
Hello, World! (wchar_t *)
Hello, World! (_bstr_t)
Hello, World! (CComBSTR)
Hello, World! (basic_string)
Hello, World! (System::String)
从 basic_string 转换
示例
说明
此示例演示如何从 basic_string 转换为上面列出的其他字符串类型。

// convert_from_basic_string.cpp
// compile with /clr /link comsuppw.lib
#include <iostream>
#include <stdlib.h>
#include <string>
#include "atlbase.h"
#include "atlstr.h"
#include "comutil.h"
using namespace std;
using namespace System;
int main()
{
	string orig("Hello, World!");
	cout << orig << " (basic_string)" << endl;
	// Convert to a char*
	const size_t newsize = 100;
	char nstring[newsize];
	strcpy_s(nstring, orig.c_str());
	strcat_s(nstring, " (char *)");
	cout << nstring << endl;
	// Convert to a wchar_t*
	// You must first convert to a char * for this to work.
	size_t origsize = strlen(orig.c_str()) + 1;
	size_t convertedChars = 0;
	wchar_t wcstring[newsize];
	mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
	wcscat_s(wcstring, L" (wchar_t *)");
	wcout << wcstring << endl;
	// Convert to a _bstr_t
	_bstr_t bstrt(orig.c_str());
	bstrt += " (_bstr_t)";
	cout << bstrt << endl;
	// Convert to a CComBSTR
	CComBSTR ccombstr(orig.c_str());
	if (ccombstr.Append(L" (CComBSTR)") == S_OK)
	{
		CW2A printstr(ccombstr);
		cout << printstr << endl;
	}
	// Convert to a CString
	CString cstring(orig.c_str());
	cstring += " (CString)";
	cout << cstring << endl;
	// Convert to a System::String
	String ^systemstring = gcnew String(orig.c_str());
	systemstring += " (System::String)";
	Console::WriteLine("{0}", systemstring);
	delete systemstring;
}
输出
Hello, World! (basic_string)
Hello, World! (char *)
Hello, World! (wchar_t *)
Hello, World! (_bstr_t)
Hello, World! (CComBSTR)
Hello, World! (CString)
Hello, World! (System::String)
从 System::String 转换
示例
说明
此示例演示如何从 System.String 转换为上面列出的其他字符串类型。

// convert_from_system_string.cpp
// compile with /clr /link comsuppw.lib
#include <iostream>
#include <stdlib.h>
#include <string>
#include "atlbase.h"
#include "atlstr.h"
#include "comutil.h"
#include "vcclr.h"
using namespace std;
using namespace System;
using namespace System::Runtime::InteropServices;
int main()
{
	String ^orig = gcnew String("Hello, World!");
	Console::WriteLine("{0} (System::String)", orig);
	pin_ptr<const wchar_t> wch = PtrToStringChars(orig);
	// Convert to a char*
	size_t origsize = wcslen(wch) + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	char nstring[newsize];
	wcstombs_s(&convertedChars, nstring, origsize, wch, _TRUNCATE);
	strcat_s(nstring, " (char *)");
	cout << nstring << endl;
	// Convert to a wchar_t*
	wchar_t wcstring[newsize];
	wcscpy_s(wcstring, wch);
	wcscat_s(wcstring, L" (wchar_t *)");
	wcout << wcstring << endl;
	// Convert to a _bstr_t
	_bstr_t bstrt(wch);
	bstrt += " (_bstr_t)";
	cout << bstrt << endl;
	// Convert to a CComBSTR
	CComBSTR ccombstr(wch);
	if (ccombstr.Append(L" (CComBSTR)") == S_OK)
	{
		CW2A printstr(ccombstr);
		cout << printstr << endl;
	}
	// Convert to a CString
	CString cstring(wch);
	cstring += " (CString)";
	cout << cstring << endl;
	// Convert to a basic_string
	wstring basicstring(wch);
	basicstring += L" (basic_string)";
	wcout << basicstring << endl;
	delete orig;
}
输出
Hello, World! (System::String)
Hello, World! (char *)
Hello, World! (wchar_t *)
Hello, World! (_bstr_t)
Hello, World! (CComBSTR)
Hello, World! (CString)
Hello, World! (basic_string)
*/