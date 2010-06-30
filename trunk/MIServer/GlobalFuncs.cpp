#include "stdafx.h"
#include "GlobalFuncs.h"
#include "DataFile.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

_ConnectionPtr	g_pDBConnection;

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
		if(tmp.IsEmpty()){
			tmp = " ";
		}
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

void MakeSendCmdFromRec(struct UserData data, CString &str)
{
	char buf[100];

	g_strList.RemoveAll();

	g_strList.AddTail("OK");

	sprintf(buf, "%d", data.ID);
	g_strList.AddTail(buf);

	sprintf(buf, "%d", data.Order);
	g_strList.AddTail(buf);

	g_strList.AddTail(data.ScancodeID);

	sprintf(buf, "%d", data.Number);
	g_strList.AddTail(buf);

	g_strList.AddTail(data.Name);
	g_strList.AddTail(data.Sex);

	sprintf(buf, "%d", data.Age);
	g_strList.AddTail(buf);

	g_strList.AddTail(data.BirthDate);
	g_strList.AddTail(data.People);
	g_strList.AddTail(data.Department);
	g_strList.AddTail(data.TypeOfWork);
	g_strList.AddTail(data.Province);
	g_strList.AddTail(data.City);
	g_strList.AddTail(data.Address);
	g_strList.AddTail(data.ZipCode);
	g_strList.AddTail(data.Tel);
	g_strList.AddTail(data.ClinicalDiagnosis);

	sprintf(buf, "%d", data.Height);
	g_strList.AddTail(buf);

	g_strList.AddTail(data.Weight);
	g_strList.AddTail(data.CheckDate);
	g_strList.AddTail(data.Hazards);
	g_strList.AddTail(data.Pharmacy);
	g_strList.AddTail(data.PastHistory);

	g_strList.AddTail("\r\n");

	MakeSeparatorString(str);
}

int ParseRecvDataToRec(CString str, struct UserData &data)
{
	//存到String List
	ParseSeparatorString(str);
	memset(&data, 0, sizeof(data));

	POSITION p = g_strList.GetHeadPosition();
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);//skip "CMD*"
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	sscanf(str.GetBuffer(str.GetLength()), "%d", &data.ID);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	sprintf(data.ScancodeID, "%s", str.GetBuffer(str.GetLength()));
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	sscanf(str.GetBuffer(str.GetLength()), "%d", &data.Number);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	sprintf(data.Name, "%s", str.GetBuffer(str.GetLength()));
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	sprintf(data.Sex, "%s", str.GetBuffer(str.GetLength()));
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	sscanf(str.GetBuffer(str.GetLength()), "%d", &data.Age);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	sprintf(data.BirthDate, "%s", str.GetBuffer(str.GetLength()));
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	sprintf(data.People, "%s", str.GetBuffer(str.GetLength()));
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	sprintf(data.Department, "%s", str.GetBuffer(str.GetLength()));
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	sprintf(data.TypeOfWork, "%s", str.GetBuffer(str.GetLength()));
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	sprintf(data.Province, "%s", str.GetBuffer(str.GetLength()));
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	sprintf(data.City, "%s", str.GetBuffer(str.GetLength()));
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	sprintf(data.Address, "%s", str.GetBuffer(str.GetLength()));
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	sprintf(data.ZipCode, "%s", str.GetBuffer(str.GetLength()));
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	sprintf(data.Tel, "%s", str.GetBuffer(str.GetLength()));
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	sprintf(data.ClinicalDiagnosis, "%s", str.GetBuffer(str.GetLength()));
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	sscanf(str.GetBuffer(str.GetLength()), "%d", &data.Height);
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	sprintf(data.Weight, "%s", str.GetBuffer(str.GetLength()));
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	sprintf(data.CheckDate, "%s", str.GetBuffer(str.GetLength()));
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	sprintf(data.Hazards, "%s", str.GetBuffer(str.GetLength()));
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	sprintf(data.Pharmacy, "%s", str.GetBuffer(str.GetLength()));
	if(!p){
		return FALSE;
	}

	str = g_strList.GetNext(p);
	sprintf(data.PastHistory, "%s", str.GetBuffer(str.GetLength()));

	return TRUE;
}

BOOL Cmd_GetRecordNum(int &num)
{
	_RecordsetPtr	pHandlerRecordset;

	pHandlerRecordset.CreateInstance(__uuidof(Recordset));
	try{
		pHandlerRecordset->Open("SELECT COUNT(ID) FROM Case_Data",// 查询表中所有字段
		g_pDBConnection.GetInterfacePtr(),	 // 获取库接库的IDispatch指针
		adOpenDynamic,
		adLockOptimistic,
		adCmdText);
	}
	catch(_com_error *e){
		Printf(e->ErrorMessage());
		return FALSE;
	}  

	BOOL ret = FALSE;
	_variant_t var;

	if(!pHandlerRecordset->adoEOF){
		var = pHandlerRecordset->GetCollect((long)0);

		if(var.vt != VT_NULL){
			num = (int)(long)(var);
			ret = TRUE;
		}
	}

	pHandlerRecordset->Close();
	pHandlerRecordset.Release();
	pHandlerRecordset = NULL;

	return ret;
}

BOOL Cmd_GetAllIDs(int *pID, int &num)
{
	_RecordsetPtr	pHandlerRecordset;
	CString str;

	pHandlerRecordset.CreateInstance(__uuidof(Recordset));
	try{
		pHandlerRecordset->Open("SELECT ID,Order FROM Case_Data ORDER BY [Order] ASC",// 查询表中所有字段
		g_pDBConnection.GetInterfacePtr(),	 // 获取库接库的IDispatch指针
		adOpenDynamic,
		adLockOptimistic,
		adCmdText);
	}
	catch(_com_error *e){
		Printf(e->ErrorMessage());
		return FALSE;
	}  

	BOOL ret = FALSE;
	_variant_t var;

	num = 0;
	while(!pHandlerRecordset->adoEOF){
		var = pHandlerRecordset->GetCollect("ID");
		if(var.vt != VT_NULL){
			pID[num++] = (UINT)(long)(var);
		}
		else{
			break;
		}

		pHandlerRecordset->MoveNext();
	}

	pHandlerRecordset->Close();
	pHandlerRecordset.Release();
	pHandlerRecordset = NULL;

	return TRUE;
}

void MakeIDToSeparatorString(int *pID, int num, CString &str)
{
	char buf[100];

	str.Empty();
	str += "OK||";
	sprintf(buf, "%d||", num);
	str += buf;
	for(int i=0;i<num;i++){
		sprintf(buf, "%d%s", pID[i], "||");
		str += buf;
	}
	str += "\r\n";
}

BOOL Cmd_GetRecordByID(int ID, struct UserData &rec)
{
	_RecordsetPtr	pHandlerRecordset;
	char buf[256];
	CString str;

	sprintf(buf, "%s%d", "SELECT * FROM Case_Data WHERE ID=", ID);

	pHandlerRecordset.CreateInstance(__uuidof(Recordset));
	try{
		pHandlerRecordset->Open(buf,// 查询表中所有字段
		g_pDBConnection.GetInterfacePtr(),	 // 获取库接库的IDispatch指针
		adOpenDynamic,
		adLockOptimistic,
		adCmdText);
	}
	catch(_com_error *e){
		Printf(e->ErrorMessage());
		return FALSE;
	}  

	BOOL ret = FALSE;
	_variant_t var;

	memset(&rec, 0, sizeof(rec));
	if(!pHandlerRecordset->adoEOF){
		var = pHandlerRecordset->GetCollect("ID");
		if(var.vt != VT_NULL){
			rec.ID = (UINT)(long)(var);
		}

		var = pHandlerRecordset->GetCollect("Order");
		if(var.vt != VT_NULL){
			rec.Order = (UINT)(long)(var);
		}

		var = pHandlerRecordset->GetCollect("Address");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.Address, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Age");
		if(var.vt != VT_NULL){
			rec.Age = (UINT)(long)(var);
		}

		var = pHandlerRecordset->GetCollect("Birth_date");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.BirthDate, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Check_Date");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.CheckDate, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("City");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.City, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Clinical_Diagnosis");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.ClinicalDiagnosis, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Department");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.Department, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Hazards");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.Hazards, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Name");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.Name, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Number");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sscanf(str.GetBuffer(str.GetLength()), "%d", &rec.Number);
		}

		var = pHandlerRecordset->GetCollect("Height");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sscanf(str.GetBuffer(str.GetLength()), "%d", &rec.Height);
		}

		var = pHandlerRecordset->GetCollect("Past_History");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.PastHistory, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("People");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.People, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Pharmacy");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.Pharmacy, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Province");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.Province, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("ScancodeID");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.ScancodeID, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Sex");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.Sex, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Tel");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.Tel, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Type_Of_Work");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.TypeOfWork, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Weight");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.Weight, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("ZipCode");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.ZipCode, "%s", str.GetBuffer(str.GetLength()));
		}

		ret = TRUE;
	}

	pHandlerRecordset->Close();
	pHandlerRecordset.Release();
	pHandlerRecordset = NULL;

	return ret;
}

BOOL Cmd_GetRecordByOrder(int order, struct UserData &rec)
{
	_RecordsetPtr	pHandlerRecordset;
	char buf[256];
	CString str;

	sprintf(buf, "%s%d", "SELECT * FROM Case_Data WHERE [Order]=", order);

	pHandlerRecordset.CreateInstance(__uuidof(Recordset));
	try{
		pHandlerRecordset->Open(buf,// 查询表中所有字段
		g_pDBConnection.GetInterfacePtr(),	 // 获取库接库的IDispatch指针
		adOpenDynamic,
		adLockOptimistic,
		adCmdText);
	}
	catch(_com_error *e){
		Printf(e->ErrorMessage());
		return FALSE;
	}  

	BOOL ret = FALSE;
	_variant_t var;

	memset(&rec, 0, sizeof(rec));
	if(!pHandlerRecordset->adoEOF){
		var = pHandlerRecordset->GetCollect("ID");
		if(var.vt != VT_NULL){
			rec.ID = (UINT)(long)(var);
		}

		var = pHandlerRecordset->GetCollect("Address");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.Address, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Age");
		if(var.vt != VT_NULL){
			rec.Age = (UINT)(long)(var);
		}

		var = pHandlerRecordset->GetCollect("Birth_date");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.BirthDate, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Check_Date");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.CheckDate, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("City");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.City, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Clinical_Diagnosis");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.ClinicalDiagnosis, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Department");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.Department, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Hazards");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.Hazards, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Name");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.Name, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Number");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sscanf(str.GetBuffer(str.GetLength()), "%d", &rec.Number);
		}

		var = pHandlerRecordset->GetCollect("Height");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sscanf(str.GetBuffer(str.GetLength()), "%d", &rec.Height);
		}

		var = pHandlerRecordset->GetCollect("Past_History");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.PastHistory, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("People");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.People, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Pharmacy");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.Pharmacy, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Province");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.Province, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("ScancodeID");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.ScancodeID, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Sex");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.Sex, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Tel");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.Tel, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Type_Of_Work");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.TypeOfWork, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("Weight");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.Weight, "%s", str.GetBuffer(str.GetLength()));
		}

		var = pHandlerRecordset->GetCollect("ZipCode");
		if(var.vt != VT_NULL){
			str = (LPCSTR)_bstr_t(var);
			sprintf(rec.ZipCode, "%s", str.GetBuffer(str.GetLength()));
		}

		ret = TRUE;
	}

	pHandlerRecordset->Close();
	pHandlerRecordset.Release();
	pHandlerRecordset = NULL;

	return ret;
}

BOOL Cmd_GetOrders(int order1, int order2, struct IDAndOrder *pIDAndOrder, int &num)
{
	_RecordsetPtr	pHandlerRecordset;
	CString str;
	char buf[100];
	int tmp;

	if(order1>order2){
		tmp = order1;
		order1 = order2;
		order2 = tmp;
	}

	sprintf(buf, "%s%d%s%d%s", "SELECT ID,Order FROM Case_Data WHERE [Order]>=", order1, " AND [Order]<=", order2, " ORDER BY [Order] ASC");

	pHandlerRecordset.CreateInstance(__uuidof(Recordset));
	try{
		pHandlerRecordset->Open(buf,// 查询表中所有字段
		g_pDBConnection.GetInterfacePtr(),	 // 获取库接库的IDispatch指针
		adOpenDynamic,
		adLockOptimistic,
		adCmdText);
	}
	catch(_com_error *e){
		Printf(e->ErrorMessage());
		return FALSE;
	}  

	_variant_t var;

	num = 0;
	while(!pHandlerRecordset->adoEOF){
		var = pHandlerRecordset->GetCollect("ID");
		if(var.vt != VT_NULL){
			pIDAndOrder[num].ID = (UINT)(long)(var);
		}
		else{
			break;
		}

		var = pHandlerRecordset->GetCollect("Order");
		if(var.vt != VT_NULL){
			pIDAndOrder[num].Order = (UINT)(long)(var);
		}
		else{
			break;
		}

		num ++;

		pHandlerRecordset->MoveNext();
	}

	pHandlerRecordset->Close();
	pHandlerRecordset.Release();
	pHandlerRecordset = NULL;

	return TRUE;
}

BOOL Cmd_AppendRecord(struct UserData &rec)
{
	int num1, num2, order;
	int *pID;

	if(!Cmd_GetRecordNum(num1)){
		return FALSE;
	}
	if(!Cmd_GetNextFreeOrder(order)){
		return FALSE;
	}
	rec.Order = order;

	CString strSql;
	strSql.Format("INSERT INTO Case_Data([Order],ScancodeID,[Number],Name,Sex,Age,Birth_date,People,Department,Type_Of_Work,Province,City,Address,ZipCode,Tel,Clinical_Diagnosis,Height,Weight,Check_Date,Hazards,Pharmacy,Past_History) VALUES(%d,'%s',%d,'%s','%s',%d,'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s',%d,'%s','%s','%s','%s','%s')",
					rec.Order,
					rec.ScancodeID,
					rec.Number,
					rec.Name,
					rec.Sex,
					rec.Age,
					rec.BirthDate,
					rec.People,
					rec.Department,
					rec.TypeOfWork,
					rec.Province,
					rec.City,
					rec.Address,
					rec.ZipCode,
					rec.Tel,
					rec.ClinicalDiagnosis,
					rec.Height,
					rec.Weight,
					rec.CheckDate,
					rec.Hazards,
					rec.Pharmacy,
					rec.PastHistory);
	_variant_t vAffected;
	g_pDBConnection->Execute(_bstr_t(strSql),&vAffected,adCmdText);

	pID = new int[num1+10];
	if(!Cmd_GetAllIDs(pID, num2)){
		delete []pID;
		return FALSE;
	}
	if(num2!=num1+1){
		delete []pID;
		return FALSE;
	}
	rec.ID = pID[num1];
	delete []pID;
	return TRUE;
}

BOOL Cmd_DeleteRecordByID(int ID)
{
	CString strSql;
	_variant_t vAffected;

	strSql.Format("%s%d", "DELETE FROM Case_Data WHERE ID=", ID);
	g_pDBConnection->Execute(_bstr_t(strSql),&vAffected,adCmdText);
	return TRUE;
}

BOOL Cmd_ModifyRecordByID(int ID, struct UserData rec)
{
	CString strSql;
	strSql.Format("UPDATE Case_Data SET ScancodeID='%s',[Number]=%d,Name='%s',Sex='%s',Age=%d,Birth_date='%s',People='%s',Department='%s',Type_Of_Work='%s',Province='%s',City='%s',Address='%s',ZipCode='%s',Tel='%s',Clinical_Diagnosis='%s',Height=%d,Weight='%s',Check_Date='%s',Hazards='%s',Pharmacy='%s',Past_History='%s' WHERE ID=%d",
					rec.ScancodeID,
					rec.Number,
					rec.Name,
					rec.Sex,
					rec.Age,
					rec.BirthDate,
					rec.People,
					rec.Department,
					rec.TypeOfWork,
					rec.Province,
					rec.City,
					rec.Address,
					rec.ZipCode,
					rec.Tel,
					rec.ClinicalDiagnosis,
					rec.Height,
					rec.Weight,
					rec.CheckDate,
					rec.Hazards,
					rec.Pharmacy,
					rec.PastHistory,
					ID
				);
	_variant_t vAffected;
	g_pDBConnection->Execute(_bstr_t(strSql),&vAffected,adCmdText);
	return TRUE;
}

BOOL Cmd_GetNextFreeOrder(int &order)
{
	BOOL ret;
	int num;
	if(!Cmd_GetRecordNum(num)){
		return FALSE;
	}

	if(num==0){
		order = 1;
		return TRUE;
	}

	_RecordsetPtr	pHandlerRecordset;

	pHandlerRecordset.CreateInstance(__uuidof(Recordset));
	try{
		pHandlerRecordset->Open("SELECT MAX(Order) FROM Case_Data",// 查询表中所有字段
		g_pDBConnection.GetInterfacePtr(),	 // 获取库接库的IDispatch指针
		adOpenDynamic,
		adLockOptimistic,
		adCmdText);
	}
	catch(_com_error *e){
		Printf(e->ErrorMessage());
		return FALSE;
	}  

	ret = FALSE;
	_variant_t var;

	if(!pHandlerRecordset->adoEOF){
		var = pHandlerRecordset->GetCollect((long)0);

		if(var.vt != VT_NULL){
			order = (int)(long)(var) + 1;
			ret = TRUE;
		}
	}

	pHandlerRecordset->Close();
	pHandlerRecordset.Release();
	pHandlerRecordset = NULL;

	return ret;
}

BOOL Cmd_GetOrderByID(int ID, int &order)
{
	BOOL ret;
	_RecordsetPtr	pHandlerRecordset;
	char buf[100];

	sprintf(buf, "%s%d", "SELECT Order FROM Case_Data WHERE ID=", ID);
	pHandlerRecordset.CreateInstance(__uuidof(Recordset));
	try{
		pHandlerRecordset->Open(buf,// 查询表中所有字段
		g_pDBConnection.GetInterfacePtr(),	 // 获取库接库的IDispatch指针
		adOpenDynamic,
		adLockOptimistic,
		adCmdText);
	}
	catch(_com_error *e){
		Printf(e->ErrorMessage());
		return FALSE;
	}  

	ret = FALSE;
	_variant_t var;

	if(!pHandlerRecordset->adoEOF){
		var = pHandlerRecordset->GetCollect("Order");

		if(var.vt != VT_NULL){
			order = (int)(long)(var);
			ret = TRUE;
		}
	}

	pHandlerRecordset->Close();
	pHandlerRecordset.Release();
	pHandlerRecordset = NULL;

	return ret;
}

BOOL Cmd_SetOrderByID(int ID, int order)
{
	CString strSql;
	strSql.Format("UPDATE Case_Data SET [Order]=%d WHERE ID=%d", order, ID);
	_variant_t vAffected;
	g_pDBConnection->Execute(_bstr_t(strSql),&vAffected,adCmdText);
	return TRUE;
}

BOOL Cmd_MoveOrder(int org_order, int dst_order)
{
	_RecordsetPtr	pHandlerRecordset;
	CString str;
	struct UserData data;
	int num, num1;
	struct IDAndOrder *pOrder = NULL;

	if(org_order<=0){
		return FALSE;
	}
	if(dst_order<=0){
		return FALSE;
	}

	if(!Cmd_GetRecordByOrder(org_order, data)){
		return FALSE;
	}

	if(!Cmd_GetRecordByOrder(dst_order, data)){
		return FALSE;
	}

	if(!Cmd_GetRecordNum(num1)){
		return FALSE;
	}

	pOrder = new struct IDAndOrder[num1];

	if(!Cmd_GetOrders(org_order, dst_order, pOrder, num)){
		delete []pOrder;
		return FALSE;
	}

	if(org_order<=dst_order){
		for(int i=num-1;i>=1;i--){
			if(!Cmd_SetOrderByID(pOrder[i].ID, pOrder[i-1].Order)){
				delete []pOrder;
				return FALSE;
			}
		}
		Cmd_SetOrderByID(pOrder[0].ID, dst_order);
	}
	else{
		for(int i=0;i<num-1;i++){
			if(!Cmd_SetOrderByID(pOrder[i].ID, pOrder[i+1].Order)){
				delete []pOrder;
				return FALSE;
			}
		}
		Cmd_SetOrderByID(pOrder[num-1].ID, dst_order);
	}

	delete []pOrder;
	return TRUE;
}

BOOL Cmd_MoveOrderPrev(int order)
{
	_RecordsetPtr	pHandlerRecordset;
	CString str;
	char buf[256];
	struct IDAndOrder iao[2];

	sprintf(buf, "%s%d%s", "SELECT ID,[Order] FROM Case_Data WHERE [Order]<=", order, " ORDER BY [Order] DESC");
	pHandlerRecordset.CreateInstance(__uuidof(Recordset));
	try{
		pHandlerRecordset->Open(buf,// 查询表中字段
		g_pDBConnection.GetInterfacePtr(),	 // 获取库接库的IDispatch指针
		adOpenDynamic,
		adLockOptimistic,
		adCmdText);
	}
	catch(_com_error *e){
		Printf(e->ErrorMessage());
		return FALSE;
	}  

	BOOL ret = FALSE;
	_variant_t var;

	int num = 0;
	while(!pHandlerRecordset->adoEOF){
		var = pHandlerRecordset->GetCollect("ID");
		if(var.vt != VT_NULL){
			iao[num].ID = (UINT)(long)(var);
		}
		else{
			break;
		}

		var = pHandlerRecordset->GetCollect("Order");
		if(var.vt != VT_NULL){
			iao[num].Order = (UINT)(long)(var);
		}
		else{
			break;
		}

		num ++;
		if(num==2){
			break;
		}

		pHandlerRecordset->MoveNext();
	}

	pHandlerRecordset->Close();
	pHandlerRecordset.Release();
	pHandlerRecordset = NULL;

	if(num<2){
		return FALSE;
	}

	if(iao[0].Order!=order){
		return FALSE;
	}

	if(!Cmd_SetOrderByID(iao[0].ID, iao[1].Order)){
		return FALSE;
	}
	if(!Cmd_SetOrderByID(iao[1].ID, iao[0].Order)){
		return FALSE;
	}

	return TRUE;
}

BOOL Cmd_MoveOrderNext(int order)
{
	_RecordsetPtr	pHandlerRecordset;
	CString str;
	char buf[256];
	struct IDAndOrder iao[2];

	sprintf(buf, "%s%d%s", "SELECT ID,[Order] FROM Case_Data WHERE [Order]>=", order, " ORDER BY [Order] ASC");
	pHandlerRecordset.CreateInstance(__uuidof(Recordset));
	try{
		pHandlerRecordset->Open(buf,// 查询表中字段
		g_pDBConnection.GetInterfacePtr(),	 // 获取库接库的IDispatch指针
		adOpenDynamic,
		adLockOptimistic,
		adCmdText);
	}
	catch(_com_error *e){
		Printf(e->ErrorMessage());
		return FALSE;
	}  

	BOOL ret = FALSE;
	_variant_t var;

	int num = 0;
	while(!pHandlerRecordset->adoEOF){
		var = pHandlerRecordset->GetCollect("ID");
		if(var.vt != VT_NULL){
			iao[num].ID = (UINT)(long)(var);
		}
		else{
			break;
		}

		var = pHandlerRecordset->GetCollect("Order");
		if(var.vt != VT_NULL){
			iao[num].Order = (UINT)(long)(var);
		}
		else{
			break;
		}

		num ++;
		if(num==2){
			break;
		}

		pHandlerRecordset->MoveNext();
	}

	pHandlerRecordset->Close();
	pHandlerRecordset.Release();
	pHandlerRecordset = NULL;

	if(num<2){
		return FALSE;
	}

	if(iao[0].Order!=order){
		return FALSE;
	}

	if(!Cmd_SetOrderByID(iao[0].ID, iao[1].Order)){
		return FALSE;
	}
	if(!Cmd_SetOrderByID(iao[1].ID, iao[0].Order)){
		return FALSE;
	}

	return TRUE;
}
