#include "GlobalFuncs.h"
#include "AbsTestDataMan.h"
#include "GlobalVars.h"
#include "Mime\\Mime.h"
#include "Mime\\MimeCode.h"
#include "ProgressThread.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

///TCHAR����תchar*����(ע��TCHARΪ�꣬UNICODE��Ϊwchar_t��ANSI��Ϊchar,���ñ�������ر�֤szDst�Ĵ�С�㹻������ΪiLength������
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
	CString strDst = CString(szSrc);
	_swprintf(tcDst, _T("%s"), strDst.GetBuffer(strDst.GetAllocLength()));
	strDst.ReleaseBuffer();
#endif
}

void CString2char(char *szDst, CString strSrc)
{
	TCHAR2char(szDst, strSrc.GetBuffer(strSrc.GetLength()), strSrc.GetLength());
	strSrc.ReleaseBuffer();
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
	//MyMessageBox(msg, "��Ϣ", MB_ICONINFORMATION);
	AfxGetApp()->GetMainWnd()->GetActiveWindow()->MessageBox(CString(msg), _T("��Ϣ"), MB_ICONINFORMATION);
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
			if(dir[_tcslen(dir)-1]==_T('\n')){//ȥ�����з�
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

// ����ֵ�� 0: ������ ��1: �ڴ治�� 
int filter_new( 
			   double dat[],     // �������� 
			   double fs,      // ����Ƶ�� Hz 
			   double fc,       // �˲�Ƶ�� Hz 
			   int num        // ���ݸ��� 
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

#include <stdio.h>
#include <string.h>

//��lpszSour�в���lpszFind,��nStart��ʼ
int FindString(const char * lpszSour,const char * lpszFind,int nStart=0);
//��ͨ���'?','*'���ַ���ƥ��,bMatchCase=TRUE���ִ�Сд
BOOL MatchString(const char * lpszSour,const char * lpszMatch,BOOL bMatchCase=TRUE);
//����ƥ��
BOOL MultiMatch(const char * lpszSour,const char * lpszMatch,int nMatchLogic=0,BOOL bRetReversed=FALSE,BOOL bMatchCase=TRUE);

//�� ��: ��lpszSour�в����ַ���lpszFind,lpszFind�п��԰���ͨ���'?'
//�� ��: nStartδ��lpszSour�е���ʼ����λ��
//����ֵ: �ɹ�����ƥ��λ��,���򷵻�-1

int FindString(const char * lpszSour,const char * lpszFind,int nStart/*=0*/)
{
	if(lpszSour==NULL || lpszFind==NULL || nStart<0)
		return -1;
	int m=strlen(lpszSour);
	int n=strlen(lpszFind);
	if(nStart+n>m)
		return -1;
	if(n==0)
		return nStart;
	//KMP begin
	int * next=new int[n]; //�����ַ�����next����
	--n;
	int j,k;
	j=0,k=-1;
	next[0]=-1;
	while(j<n)
	{
		if(k==-1 || lpszFind[k]=='?' || lpszFind[j]==lpszFind[k])
		{
			++j,++k;
			next[j]=k;
		}
		else
			k=next[k];
	}
	++n;
	int i=nStart;
	j=0;
	while(i<m && j<n)
	{
		if(j==-1 || lpszFind[j]=='?' || lpszSour[i]==lpszFind[j])
		{
			++i;
			++j;
		}
		else
			j=next[j];
	}
	delete [] next;
	if(j>=n)
		return i-n;
	else
		return -1;
}

//�� ��: ��ͨ������ַ���ƥ��
//�� ��: lpszSour��һ����ͨ�ַ���
//    lpszMatch��һ�����԰���ͨ������ַ���
//    bMatchCase=TRUE:���ִ�Сд,=FALSE�����ִ�Сд
//����ֵ: ƥ�䷵��TRUE,���򷵻�FALSE
//˵ ��: '*' ƥ�����ⳤ���ַ���,�����մ�;'?'ƥ������һ���ַ�,�ǿ�
BOOL MatchString(const char * lpszSour,const char * lpszMatch,BOOL bMatchCase/*=TRUE*/)
{
	if(lpszSour==NULL || lpszMatch==NULL)
		return FALSE;
	if(lpszMatch[0]==0) //ģʽ����
	{
		if(lpszSour[0]==0)
			return TRUE;
		else
			return FALSE;
	}
	int i=0,j=0;
	char * szSource=new char[(j=strlen(lpszSour)+1)]; //�Ƚ�����ʱԴ�ַ���
	if(bMatchCase)
	{
		while(* (szSource+i)=*(lpszSour+i++));
	}
	else
	{
		i=0;
		while(lpszSour[i])
		{
			if(lpszSour[i]>='A' && lpszSour[i]<='Z')
				szSource[i]=lpszSour[i]-'A'+'a';
			else
				szSource[i]=lpszSour[i];
			++i;
		}
		szSource[i]=0; //end of source
	}
	char * szMatcher=new char[strlen(lpszMatch)+1]; //�Ƚ�����ʱƥ���ַ���
	//��lpszMatch����������'*'�ϲ���һ��'*'���Ƶ�szMatcher��
	i=j=0;
	while(lpszMatch[i])
	{
		szMatcher[j++]=(! bMatchCase) ? ((lpszMatch[i]>='A' && lpszMatch[i]<='Z') ? lpszMatch[i]-'A'+'a' : lpszMatch[i]) : lpszMatch[i];
		if(lpszMatch[i]=='*')
		{
			while(lpszMatch[++i]=='*');
		}
		else
			++i;
	}
	szMatcher[j]=0;
	//��ʼƥ��
	int nMatchOffset,nSourOffset;
	BOOL bIsMatched=TRUE;
	nMatchOffset=nSourOffset=0;
	while(szMatcher[nMatchOffset])
	{
		if(szMatcher[nMatchOffset]=='*')
		{
			if(szMatcher[nMatchOffset+1]==0) //szMatcher[nMatchOffset]�����һ���ַ�
			{
				bIsMatched=TRUE;
				break;
			}
			else //szMatcher[nMatchOffset+1]ֻ����'?'����ͨ�ַ�
			{
				int nSubOffset=nMatchOffset+1;
				while(szMatcher[nSubOffset])
				{
					if(szMatcher[nSubOffset]=='*')
						break;
					++nSubOffset;
				}
				if(strlen(szSource+nSourOffset)<size_t(nSubOffset-nMatchOffset-1)) //Դ�ַ���ʣ�µĳ���С��ƥ�䴮ʣ�µĳ���
				{
					bIsMatched=FALSE;
					break;
				}
				if(! szMatcher[nSubOffset]) //nSubOffset �ﵽszMatcher ĩβ ; ���ʣ�²����ַ��Ƿ�һһƥ��
				{
					--nSubOffset;
					int nTempSourOffset=strlen(szSource)-1;
					//�Ӻ���ǰ����ƥ��
					while(szMatcher[nSubOffset]!='*')
					{
						if(szMatcher[nSubOffset]=='?')
							;
						else
						{
							if(szMatcher[nSubOffset]!=szSource[nTempSourOffset])
							{
								bIsMatched=FALSE;
								break;
							}
						}
						--nSubOffset;
						--nTempSourOffset;
					}
					break;
				}
				else //szMatcher[nSubOffset]=='*'
				{
					nSubOffset-=nMatchOffset;
					char * szTempFinder=new char[nSubOffset];
					--nSubOffset;
					memcpy(szTempFinder,szMatcher+nMatchOffset+1,nSubOffset);
					szTempFinder[nSubOffset]=0;
					int nPos=FindString(szSource+nSourOffset,szTempFinder,0);
					delete [] szTempFinder;
					if(nPos!=-1) //��szSource+nSourOffset���ҵ�szTempFinder
					{
						nMatchOffset+=nSubOffset;
						nSourOffset+=(nPos+nSubOffset-1);
					}
					else
					{
						bIsMatched=FALSE;
						break;
					}
				}
			}
		}
		else if(szMatcher[nMatchOffset]=='?')
		{
			if(! szSource[nSourOffset])
			{
				bIsMatched=FALSE;
				break;
			}
			if(! szMatcher[nMatchOffset+1] && szSource[nSourOffset+1]) //szMatcher���һ���ַ� szSource�����һ���ַ�
			{
				bIsMatched=FALSE;
				break;
			}
			++nMatchOffset;
			++nSourOffset;
		}
		else //szMatcher[nMatchOffset] Ϊ�����ַ�
		{
			if(szSource[nSourOffset]!=szMatcher[nMatchOffset])
			{
				bIsMatched=FALSE;
				break;
			}
			if(! szMatcher[nMatchOffset+1] && szSource[nSourOffset+1])
			{
				bIsMatched=FALSE;
				break;
			}
			++nMatchOffset;
			++nSourOffset;
		}
	}
	delete [] szSource;
	delete [] szMatcher;
	return bIsMatched;
}

//�� ��: ����ƥ��,��ͬƥ���ַ���֮����','����;"*.h,*.cpp",����ƥ��"*.h","*.cpp";
//�� ��: nMatchLogic=0,��ͬƥ�����,else����; bMatchCase: �Ƿ��С����
//����ֵ: ���bRetReversed=0,ƥ�䷵��TRUE; ����ƥ�䷵��TRUE
BOOL MultiMatching(const char * lpszSour,const char * lpszMatch,int nMatchLogic/*=0*/,BOOL bRetReversed/*=0*/,BOOL bMatchCase/*=TRUE*/)   
{
	if(lpszSour==NULL || lpszMatch==NULL)
		return FALSE;
	char * szSubMatch=new char[strlen(lpszMatch)+1];
	BOOL bIsMatch;
	if(nMatchLogic==0) //��
	{
		bIsMatch=0;
		int i=0,j=0;
		while(TRUE)
		{
			if(lpszMatch[i]!=0 && lpszMatch[i]!=',')
				szSubMatch[j++]=lpszMatch[i];
			else
			{
				szSubMatch[j]=0;
				if(j!=0)
				{
					bIsMatch=MatchString(lpszSour,szSubMatch,bMatchCase);
					if(bIsMatch)
						break;
				}
				j=0;
			}
			if(lpszMatch[i]==0)
				break;
			++i;
		}
	}
	else //��
	{
		bIsMatch=1;
		int i=0,j=0;
		while(TRUE)
		{
			if(lpszMatch[i]!=0 && lpszMatch[i]!=',')
				szSubMatch[j++]=lpszMatch[i];
			else
			{
				szSubMatch[j]=0;
				bIsMatch=MatchString(lpszSour,szSubMatch,bMatchCase);
				if(! bIsMatch)
					break;
				j=0;
			}
			if(lpszMatch[i]==0)
				break;
			++i;
		}
	}
	delete [] szSubMatch;
	if(bRetReversed)
		return ! bIsMatch;
	else
		return bIsMatch;
}

int n = 10,q;
char dic[2000][1000];

int TestStringMatch()
{
	char s[] = "123AABBCC11";
	char s1[] = "123DF";

	BOOL ret;
	ret = MatchString(s, s1);
	/*
	int i;
	scanf("%d",&n);
	for(i=0;i<n;++i)
		scanf("%s",dic[i]);
	scanf("%d",&q);
	for(i=0;i<q;++i)
	{
		char tmp[10000];
		scanf("%s",tmp);
		int ans=0;
		for(int j=0;j<n;++j)
		{
			if(MatchString(dic[j],tmp))
				++ans;
		}
		printf("%d\n",ans);
	}
	*/
	return 0;
}
