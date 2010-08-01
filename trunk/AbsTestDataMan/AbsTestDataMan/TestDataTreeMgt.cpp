#include "TestDataTreeMgt.h"
#include "stdafx.h"
#include "resource.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "DirConfigMgt.h"
#include "FilterConfigMgt.h"
#include "GlobalFuncs.h"

#include "DlgProgress.h"

CTestDataTreeMgt::CTestDataTreeMgt(void)
{
	CFilterConfigMgt fcMgt;

	fcMgt.GetFilterConfig(m_filterConfig);

	m_pProductTreeRoot = NULL;
}

CTestDataTreeMgt::~CTestDataTreeMgt(void)
{
	FreeTree();
}

static UINT InitTreeThread(LPVOID lpParam)
{
	CTestDataTreeMgt *pTestDataTreeMgt;
	CFilterConfigMgt fcMgt;
	CDirConfigMgt cMgt;

	pTestDataTreeMgt = (CTestDataTreeMgt*)lpParam;
	if(pTestDataTreeMgt==NULL){
		return -1;
	}

	fcMgt.GetFilterConfig(pTestDataTreeMgt->m_filterConfig);
	cMgt.LoadDirFromConfigFile();

	int i;
	CFileFind ff;
	BOOL bDecide = FALSE;
	CString str, name;
	char buf[MAX_BUF_LEN];

	pTestDataTreeMgt->FreeTree();

	pTestDataTreeMgt->m_pProductTreeRoot = new ProductTreeRoot;
	memset(pTestDataTreeMgt->m_pProductTreeRoot, 0, sizeof(ProductTreeRoot));
	pTestDataTreeMgt->m_pProductTreeRoot->pProductNodeListHead = new ProductNode;
	pTestDataTreeMgt->m_pProductTreeRoot->pProductNodeListTail = pTestDataTreeMgt->m_pProductTreeRoot->pProductNodeListHead;
	memset(pTestDataTreeMgt->m_pProductTreeRoot->pProductNodeListHead, 0, sizeof(ProductNode));

	if(!CDlgProgress::UpdateAndCheck(0)){
		return 0;
	}

	for(i=0;i<cMgt.m_saDirectories.GetCount();i++){
		if(!CDlgProgress::UpdateAndCheck(i * 100 / cMgt.m_saDirectories.GetCount())){
			return 0;
		}
		str = cMgt.m_saDirectories.GetAt(i);
		if(str.Right(1)!=_T('\\')){
			str += _T("\\");
		}

		bDecide = ff.FindFile(str+_T("*.DFT"));
		while(bDecide){
			bDecide = ff.FindNextFile();  
			if(ff.IsDirectory() || ff.IsDots()){//目录或.,..
				continue;
			}
			name = str + ff.GetFileName();
			CString2char(buf, name);
			pTestDataTreeMgt->LoadFile(buf);
			if(!CDlgProgress::UpdateAndCheck(i * 100 / cMgt.m_saDirectories.GetCount())){
				break;
			}
		}
		ff.Close();
	}
	return 0;
}

void CTestDataTreeMgt::InitTree(void)
{
#if 0
	CDlgProgress::m_lpParam = this;
	CDlgProgress::StartThread(InitTreeThread, _T("测试数据"));
#else
	CFilterConfigMgt fcMgt;
	CDirConfigMgt cMgt;

	fcMgt.GetFilterConfig(m_filterConfig);
	cMgt.LoadDirFromConfigFile();

	int i;
	CFileFind ff;
	BOOL bDecide = FALSE;
	CString str, name;
	char buf[MAX_BUF_LEN];

	FreeTree();

	m_pProductTreeRoot = new ProductTreeRoot;
	memset(m_pProductTreeRoot, 0, sizeof(ProductTreeRoot));
	m_pProductTreeRoot->pProductNodeListHead = new ProductNode;
	m_pProductTreeRoot->pProductNodeListTail = m_pProductTreeRoot->pProductNodeListHead;
	memset(m_pProductTreeRoot->pProductNodeListHead, 0, sizeof(ProductNode));

#ifdef _DEBUG
	int count = 0;
#endif
	for(i=0;i<cMgt.m_saDirectories.GetCount();i++){
		str = cMgt.m_saDirectories.GetAt(i);
		if(str.Right(1)!=_T('\\')){
			str += _T("\\");
		}

#ifdef _DEBUG
		if(count>=20){
			break;
		}
#endif
		bDecide = ff.FindFile(str+_T("*.DFT"));
		while(bDecide){
			bDecide = ff.FindNextFile();  
			if(ff.IsDirectory() || ff.IsDots()){//目录或.,..
				continue;
			}
			name = str + ff.GetFileName();
			CString2char(buf, name);
			LoadFile(buf);
#ifdef _DEBUG
			count ++;
			if(count>=20){
				break;
			}
#endif
		}
		ff.Close();
	}
#endif
}

void CTestDataTreeMgt::ReloadTree(void)
{
	CFilterConfigMgt fcMgt;
	CDirConfigMgt cMgt;

	fcMgt.GetFilterConfig(m_filterConfig);
	cMgt.LoadDirFromConfigFile();

	int i;
	CFileFind ff;
	BOOL bDecide = FALSE;
	CString str, name;
	char buf[MAX_BUF_LEN];

	if(m_pProductTreeRoot==NULL){
		m_pProductTreeRoot = new ProductTreeRoot;
		memset(m_pProductTreeRoot, 0, sizeof(ProductTreeRoot));
		m_pProductTreeRoot->pProductNodeListHead = new ProductNode;
		memset(m_pProductTreeRoot->pProductNodeListHead, 0, sizeof(ProductNode));
	}

#ifdef _DEBUG
	int count = 0;
#endif
	for(i=0;i<cMgt.m_saDirectories.GetCount();i++){
		str = cMgt.m_saDirectories.GetAt(i);
		if(str.Right(1)!=_T('\\')){
			str += _T("\\");
		}

#ifdef _DEBUG
		if(count>=20){
			break;
		}
#endif
		bDecide = ff.FindFile(str+_T("*.DFT"));
		while(bDecide){
			bDecide = ff.FindNextFile();  
			if(ff.IsDirectory() || ff.IsDots()){//目录或.,..
				continue;
			}
			name = str + ff.GetFileName();
			CString2char(buf, name);
			LoadFile(buf);
#ifdef _DEBUG
			count ++;
			if(count>=20){
				break;
			}
#endif
		}
		ff.Close();
	}

	//查找树中文件实际不存在的结点，将其删除
	ProductNodePtr pProductNode_Ahead = m_pProductTreeRoot->pProductNodeListHead;
	ProductNodePtr pProductNode = m_pProductTreeRoot->pProductNodeListHead->pNext;
	while(pProductNode!=NULL){
		FileNodePtr pFileNode_Ahead = pProductNode->pFileListHead;
		FileNodePtr pFileNode = pProductNode->pFileListHead->pNext;
		while(pFileNode!=NULL){
			if(!(cMgt.IsFileInConfigDirList(pFileNode->sFileName) && ff.FindFile(pFileNode->sFileName)) ||
				!PassFilter(pFileNode)){
				//1.并非目录列表有该文件，并且该文件实际存在
				//2.通不过过滤器检测
				FileNodePtr pFileNode_Next = pFileNode->pNext;
				pFileNode_Ahead->pNext = pFileNode_Next;
				if(pFileNode_Next){
					pFileNode_Next->pPrev = pFileNode_Ahead;
				}
				else{
					pProductNode->pFileListTail = pFileNode_Ahead;
				}
				FreeFileNode(pFileNode);
				pFileNode = pFileNode_Next;
			}
			else{
				pFileNode_Ahead = pFileNode;
				pFileNode = pFileNode->pNext;
			}
		}
		if(pProductNode->pFileListHead->pNext==NULL){//文件结点都不存在，必须删除该型号结点
			delete pProductNode->pFileListHead;
			ProductNodePtr pProductNodeNext = pProductNode->pNext;
			pProductNode_Ahead->pNext = pProductNodeNext;
			if(pProductNodeNext){
				pProductNodeNext->pPrev = pProductNodeNext;
			}
			else{
				m_pProductTreeRoot->pProductNodeListTail = pProductNode_Ahead;
			}
			delete pProductNode;

			pProductNode = pProductNodeNext;
		}
		else{
			pProductNode_Ahead = pProductNode;
			pProductNode = pProductNode->pNext;
		}
	}
}

void CTestDataTreeMgt::FreeTree(void)
{
	if(m_pProductTreeRoot==NULL){
		return;
	}
	ProductNodePtr pProductNode_Ahead = m_pProductTreeRoot->pProductNodeListHead;
	ProductNodePtr pProductNode = m_pProductTreeRoot->pProductNodeListHead->pNext;
	while(pProductNode!=NULL){
		FileNodePtr pFileNode_Ahead = pProductNode->pFileListHead;
		FileNodePtr pFileNode = pProductNode->pFileListHead->pNext;
		while(pFileNode!=NULL){
			FileNodePtr pFileNode_Next = pFileNode->pNext;
			FreeFileNode(pFileNode);
			pProductNode->pFileListHead->pNext = pFileNode_Next;
			if(pFileNode_Next){
				pFileNode_Next->pPrev = NULL;
			}
			else{
				pProductNode->pFileListTail = pFileNode_Ahead;
			}
			pFileNode = pFileNode_Next;
		}
		delete pProductNode->pFileListHead;

		ProductNodePtr pProductNode_Next = pProductNode->pNext;
		delete pProductNode;
		m_pProductTreeRoot->pProductNodeListHead->pNext = pProductNode_Next;
		if(pProductNode_Next){
			pProductNode_Next->pPrev = NULL;
		}
		else{
			m_pProductTreeRoot->pProductNodeListTail = pProductNode_Ahead;
		}
		pProductNode = pProductNode_Next;
	}
	delete m_pProductTreeRoot->pProductNodeListHead;

	delete m_pProductTreeRoot;
	m_pProductTreeRoot = NULL;
}

void CTestDataTreeMgt::FreeFileNode(FileNodePtr pFileNode)
{
	if(pFileNode->pMachineInfo!=NULL){
		delete pFileNode->pMachineInfo;
		pFileNode->pMachineInfo = NULL;
	}
	if(pFileNode->pProductInfo!=NULL){
		delete pFileNode->pProductInfo;
		pFileNode->pProductInfo = NULL;
	}
	if(pFileNode->pTestRecord!=NULL){
		delete pFileNode->pTestRecord;
		pFileNode->pTestRecord = NULL;
	}
	if(pFileNode->pAdditionInfo!=NULL){
		delete pFileNode->pAdditionInfo;
		pFileNode->pAdditionInfo = NULL;
	}
	if(pFileNode!=NULL){
		delete pFileNode;
	}
}

BOOL CTestDataTreeMgt::LoadFile(const char *sFileName)
{
	FileNodePtr pFileNode = NULL;
	ProductNodePtr pProductNode = NULL;

	if(SearchFileNode(sFileName, pFileNode)){
		return TRUE;
	}
	pFileNode = new FileNode;
	memset(pFileNode, 0, sizeof(FileNode));
	if(!GetFileMainInfo(sFileName, pFileNode)){
		FreeFileNode(pFileNode);
		pFileNode = NULL;
		return FALSE;
	}

	//检查是否在过滤器范围内
	if(!PassFilter(pFileNode)){
		FreeFileNode(pFileNode);
		pFileNode = NULL;
		return FALSE;
	}

	char sProductNo[MAX_BUF_LEN];
	TCHAR2char(sProductNo, pFileNode->sProductNo, _tcslen(pFileNode->sProductNo));

	if(!SearchProductNode(sProductNo, pProductNode)){
		InsertProductNode(sProductNo, pProductNode);
	}
	if(!InsertFileNode(pProductNode, pFileNode)){
		FreeFileNode(pFileNode);
		pFileNode = NULL;
		return FALSE;
	}
	return TRUE;
}

BOOL CTestDataTreeMgt::GetFileMainInfo(const char *sFileName, FileNodePtr pFileNode)
{
	int ret, i;
	FILE *pFile = NULL;
	char sBuf[MAX_BUF_LEN], sBuf1[MAX_BUF_LEN];
	FileNode &node = *pFileNode;

	if(sFileName==NULL || strlen(sFileName)==0){
		return FALSE;
	}
	if(pFileNode==NULL){
		return FALSE;
	}

	if(fopen_s(&pFile, sFileName, "rb")!=0){
		return FALSE;
	}
	if(pFile==NULL){
		return FALSE;
	}

	memset(&node, 0, sizeof(FileNode));
	//查看文件头
	memset(sBuf, 0, sizeof(sBuf));
	fread(sBuf, 23, 1, pFile);
	sprintf_s(sBuf1, "%s", "MyTester Data File ");//TODO, "Absorber Data File 6.0"
	if(strncmp(sBuf, sBuf1, strlen("MyTester Data File "))!=0){
		fclose(pFile);
		return FALSE;
	}
	//获取版本号
	double fDataVer = 0.0;
	if(isdigit(sBuf[19]) && sBuf[20]=='.' && isdigit(sBuf[21])){
		fDataVer = atof(sBuf+19);
	}
	else{
		fclose(pFile);
		return FALSE;
	}

	//略过30字节
	if(fseek(pFile, 30, SEEK_SET)!=0){
		fclose(pFile);
		return FALSE;
	}

	//获取操作员编号
	CCMachineInfo machine_info;
	CCMachineInfo *pMachineInfo = &machine_info;
	int offset = (int)(&pMachineInfo->sOperatorNo) - (int)pMachineInfo;

	fseek(pFile, offset+30, SEEK_SET);
	ret = (int)fread(sBuf, 33, 1, pFile);
	if(ret!=1){
		fclose(pFile);
		return FALSE;
	}
	char2TCHAR(node.sOperatorNo, sBuf, strlen(sBuf));

	//略过前面MachineInfo,读取ProductInfo的ProductNo
	fseek(pFile, sizeof(struct CCMachineInfo)+30, SEEK_SET);
	ret = (int)fread(sBuf, 33, 1, pFile);
	if(ret!=1){
		fclose(pFile);
		return FALSE;
	}
	char2TCHAR(node.sProductNo, sBuf, strlen(sBuf));

	//直接读取数据
	fseek(pFile, sizeof(struct CCMachineInfo)+sizeof(struct CCProductInfo)+30, SEEK_SET);

	ret = (int)fread(&node.test_record_maininfo, sizeof(struct CCTestRecordMainInfo), 1, pFile);
	fclose(pFile);
	if(ret!=1){
		return FALSE;
	}

	char2TCHAR(node.sFileName, sFileName, (int)strlen(sFileName));
	char2TCHAR(node.sTestDate, node.test_record_maininfo.sTestDate, (int)strlen(node.test_record_maininfo.sTestDate));
	node.sTestDate[10] = _T('\0');

	node.bProcessed = FALSE;
	for(i=0;i<node.test_record_maininfo.iNumOfSpeed;i++){
		node.tree_item_data[i].iIndex = i;
		node.tree_item_data[i].pNode = &node;
	}

	return TRUE;
}

BOOL CTestDataTreeMgt::LoadAndProcessFile(const char *sFileName, FileNodePtr pFileNode)
{
	int ret, i;
	FILE *pFile = NULL;
	char sBuf[MAX_BUF_LEN], sBuf1[MAX_BUF_LEN];
	FileNode &node = *pFileNode;

	if(sFileName==NULL || strlen(sFileName)==0){
		return FALSE;
	}
	if(pFileNode==NULL){
		return FALSE;
	}

	if(fopen_s(&pFile, sFileName, "rb")!=0){
		return FALSE;
	}
	if(pFile==NULL){
		return FALSE;
	}

	if(!node.bProcessed){
		node.pMachineInfo = new CCMachineInfo;
		node.pProductInfo = new CCProductInfo;
		node.pTestRecord = new CCTestRecord;
		node.pAdditionInfo = new AdditionInfo;
	}

	//查看文件头
	memset(sBuf, 0, sizeof(sBuf));
	fread(sBuf, 23, 1, pFile);
	sprintf_s(sBuf1, "%s", "MyTester Data File ");//TODO, "Absorber Data File 6.0"
	if(strncmp(sBuf, sBuf1, strlen("MyTester Data File "))!=0){
		fclose(pFile);
		return FALSE;
	}
	//获取版本号
	double fDataVer = 0.0;
	if(isdigit(sBuf[19]) && sBuf[20]=='.' && isdigit(sBuf[21])){
		fDataVer = atof(sBuf+19);
	}
	else{
		fclose(pFile);
		return FALSE;
	}

	//略过30字节
	if(fseek(pFile, 30, SEEK_SET)!=0){
		fclose(pFile);
		return FALSE;
	}

	ret = (int)fread(node.pMachineInfo, sizeof(struct CCMachineInfo), 1, pFile);
	if(ret!=1){
		fclose(pFile);
		return FALSE;
	}

	ret = (int)fread(node.pProductInfo, sizeof(struct CCProductInfo), 1, pFile);
	if(ret!=1){
		fclose(pFile);
		return FALSE;
	}

	ret = (int)fread(node.pTestRecord, sizeof(struct CCTestRecord), 1, pFile);
	if(ret!=1){
		fclose(pFile);
		return FALSE;
	}

	memcpy(&node.test_record_maininfo, node.pTestRecord, sizeof(struct CCTestRecordMainInfo));

	char2TCHAR(node.sProductNo, node.pProductInfo->sProductNo, (int)strlen(node.pProductInfo->sProductNo));
	char2TCHAR(node.sFileName, sFileName, (int)strlen(sFileName));
	char2TCHAR(node.sTestDate, node.test_record_maininfo.sTestDate, (int)strlen(node.test_record_maininfo.sTestDate));
	node.sTestDate[10] = _T('\0');

	for(i=0;i<node.test_record_maininfo.iNumOfSpeed;i++){
		node.tree_item_data[i].iIndex = i;
		node.tree_item_data[i].pNode = &node;
	}

	ProcessData(&node);

	return TRUE;
}

//对二级节点双循环链表的操作
BOOL CTestDataTreeMgt::InsertFileNode(ProductNodePtr pProductNode, FileNodePtr pFileNodeToInsert)
{
	if(pProductNode==NULL || pFileNodeToInsert==NULL){
		return FALSE;
	}

	FileNodePtr pFileNode_Ahead = pProductNode->pFileListHead;
	FileNodePtr pFileNode = pProductNode->pFileListHead->pNext;

	while(pFileNode!=NULL){
		if(strcmp(pFileNodeToInsert->test_record_maininfo.sTestDate, pFileNode->test_record_maininfo.sTestDate)>=0){
			pFileNode_Ahead = pFileNode;
			pFileNode = pFileNode->pNext;
		}
		else{
			break;
		}
	}
	pFileNode_Ahead->pNext = pFileNodeToInsert;
	pFileNodeToInsert->pNext = pFileNode;
	pFileNodeToInsert->pPrev = pFileNode_Ahead;
	if(pFileNode!=NULL){
		pFileNode->pPrev = pFileNodeToInsert;
	}
	else{
		pProductNode->pFileListTail = pFileNodeToInsert;
	}

	return TRUE;
}

BOOL CTestDataTreeMgt::DeleteFileNode(ProductNodePtr pProductNode_Ahead, ProductNodePtr pProductNode, FileNodePtr pFileNodeToDelete)
{
	if(pProductNode_Ahead==NULL || pProductNode==NULL){
		return FALSE;
	}
	if(pFileNodeToDelete==NULL){
		return FALSE;
	}

	FileNodePtr pFileNode_Ahead = pProductNode->pFileListHead;
	FileNodePtr pFileNode = pProductNode->pFileListHead->pNext;
	while(pFileNode && pFileNode!=pFileNodeToDelete){
		pFileNode_Ahead = pFileNode;
		pFileNode = pFileNode->pNext;
	}
	if(pFileNode==NULL){
		return FALSE;
	}
	FileNodePtr pFileNode_Next = pFileNode->pNext;
	pFileNode_Ahead->pNext = pFileNode_Next;
	if(pFileNode_Next){
		pFileNode_Next->pPrev = pFileNode_Ahead;
	}
	else{
		pProductNode->pFileListTail = pFileNode_Ahead;
	}
	FreeFileNode(pFileNode);
	pFileNode = NULL;

	if(pProductNode->pFileListHead->pNext==NULL){//文件结点都不存在，必须删除该型号结点
		delete pProductNode->pFileListHead;
		ProductNodePtr pProductNodeNext = pProductNode->pNext;
		pProductNode_Ahead->pNext = pProductNodeNext;
		if(pProductNodeNext){
			pProductNodeNext->pPrev = pProductNodeNext;
		}
		else{
			m_pProductTreeRoot->pProductNodeListTail = pProductNode_Ahead;
		}
		delete pProductNode;
	}
	return TRUE;
}

void CTestDataTreeMgt::DeleteFileNode(FileNodePtr pFileNodeToDelete)
{
	if(pFileNodeToDelete==NULL){
		return;
	}

	ProductNodePtr pProductNode_Ahead = m_pProductTreeRoot->pProductNodeListHead;
	ProductNodePtr pProductNode = m_pProductTreeRoot->pProductNodeListHead->pNext;
	while(pProductNode){
		if(DeleteFileNode(pProductNode_Ahead, pProductNode, pFileNodeToDelete)){
			return;
		}
		pProductNode = pProductNode->pNext;
	}

	delete pFileNodeToDelete;
	pFileNodeToDelete = NULL;
}

void CTestDataTreeMgt::DeleteFileNode(const char *sFileName)
{
	if(sFileName==NULL || strlen(sFileName)==0){
		return;
	}

	ProductNodePtr pProductNode_Ahead = m_pProductTreeRoot->pProductNodeListHead;
	ProductNodePtr pProductNode = m_pProductTreeRoot->pProductNodeListHead->pNext;
	while(pProductNode){
		FileNodePtr pFileNode_Ahead = pProductNode->pFileListHead;
		FileNodePtr pFileNode = pProductNode->pFileListHead->pNext;
		while(pFileNode){
			char sBuf[MAX_BUF_LEN];
			TCHAR2char(sBuf, pFileNode->sFileName, _tcslen(pFileNode->sFileName));
			if(strcmp(sFileName, sBuf)!=0){
				pFileNode_Ahead = pFileNode;
				pFileNode = pFileNode->pNext;
			}
			else{
				break;
			}
		}
		if(pFileNode){
			FileNodePtr pFileNode_Next = pFileNode->pNext;
			pFileNode_Ahead->pNext = pFileNode_Next;
			if(pFileNode_Next){
				pFileNode_Next->pPrev = pFileNode_Ahead;
			}
			else{
				pProductNode->pFileListTail = pFileNode_Ahead;
			}
			FreeFileNode(pFileNode);
			pFileNode = NULL;
			
			if(pProductNode->pFileListHead->pNext==NULL){//文件链表为空，删除型号结点
				ProductNodePtr pProductNode_Next = pProductNode->pNext;
				delete pProductNode->pFileListHead;
				pProductNode_Ahead->pNext = pProductNode_Next;
				if(pProductNode_Next){
					pProductNode_Next->pPrev = pProductNode_Ahead;
				}
				else{
					m_pProductTreeRoot->pProductNodeListTail = pProductNode_Ahead;
				}
			}
			return;
		}
		pProductNode_Ahead = pProductNode;
		pProductNode = pProductNode->pNext;
	}
}

//对一级节点双循环链表的操作
BOOL CTestDataTreeMgt::InsertProductNode(ProductNodePtr pProductNodeToInsert)
{
	if(pProductNodeToInsert==NULL){
		return FALSE;
	}

	ProductNodePtr pProductNode_Ahead = m_pProductTreeRoot->pProductNodeListHead;
	ProductNodePtr pProductNode = m_pProductTreeRoot->pProductNodeListHead->pNext;
	while(pProductNode!=NULL){
		if(_tcscmp(pProductNodeToInsert->sProductNo, pProductNode->sProductNo)>=0){
			pProductNode_Ahead = pProductNode;
			pProductNode = pProductNode->pNext;
		}
		else{
			break;
		}
	}
	pProductNode_Ahead->pNext = pProductNodeToInsert;
	pProductNodeToInsert->pNext = pProductNode;
	pProductNodeToInsert->pPrev = pProductNode_Ahead;
	if(pProductNode){
		pProductNode->pPrev = pProductNodeToInsert;
	}
	else{
		m_pProductTreeRoot->pProductNodeListTail = pProductNode;
	}

	return TRUE;
}

void CTestDataTreeMgt::InsertProductNode(const char *sProductNo, ProductNodePtr &pProductNodeInserted)
{
	if(sProductNo==NULL || strlen(sProductNo)==0){
		return;
	}

	pProductNodeInserted = new ProductNode;
	memset(pProductNodeInserted, 0, sizeof(ProductNode));
	pProductNodeInserted->pFileListHead = new FileNode;
	pProductNodeInserted->pFileListTail = pProductNodeInserted->pFileListHead;
	memset(pProductNodeInserted->pFileListHead, 0, sizeof(FileNode));
	char2TCHAR(pProductNodeInserted->sProductNo, sProductNo, (int)strlen(sProductNo));
	InsertProductNode(pProductNodeInserted);
}

BOOL CTestDataTreeMgt::DeleteProductNode(ProductNodePtr pProductNodeToDelete)
{
	if(pProductNodeToDelete==NULL){
		return FALSE;
	}

	ProductNodePtr pProductNode_Ahead = m_pProductTreeRoot->pProductNodeListHead;
	ProductNodePtr pProductNode = m_pProductTreeRoot->pProductNodeListHead->pNext;
	while(pProductNode!=NULL){
		if(pProductNode!=pProductNodeToDelete){
			pProductNode_Ahead = pProductNode;
			pProductNode = pProductNode->pNext;
		}
		else{
			break;
		}
	}
	if(pProductNode==NULL){//没有找到
		return FALSE;
	}
	ProductNodePtr pProductNode_Next = pProductNode->pNext;
	pProductNode_Ahead->pNext = pProductNode_Next;
	if(pProductNode_Next){
		pProductNode_Next->pPrev = pProductNode_Ahead;
	}
	else{
		m_pProductTreeRoot->pProductNodeListTail = pProductNode_Ahead;
	}

	while(pProductNode->pFileListHead->pNext){
		FileNodePtr pFileNode = pProductNode->pFileListHead->pNext;
		pProductNode->pFileListHead->pNext = pProductNode->pFileListHead->pNext->pNext;
		FreeFileNode(pFileNode);
		pFileNode = NULL;
	}
	delete pProductNodeToDelete->pFileListHead;
	pProductNodeToDelete->pFileListHead = NULL;
	pProductNodeToDelete->pFileListTail = NULL;
	delete pProductNodeToDelete;
	pProductNodeToDelete = NULL;

	return TRUE;
}

void CTestDataTreeMgt::DeleteProductNode(const char *sProductNo)
{
	if(sProductNo==NULL || strlen(sProductNo)==0){
		return;
	}

	ProductNodePtr pProductNode_Ahead = m_pProductTreeRoot->pProductNodeListHead;
	ProductNodePtr pProductNode = m_pProductTreeRoot->pProductNodeListHead->pNext;
	while(pProductNode){
		TCHAR tcProductNo[MAX_BUF_LEN];
		char2TCHAR(tcProductNo, sProductNo, (int)strlen(sProductNo));
		if(_tcscmp(tcProductNo, pProductNode->sProductNo)!=0){
			pProductNode_Ahead = pProductNode;
			pProductNode = pProductNode->pNext;
		}
		else{
			break;
		}
	}

	ProductNodePtr pProduct_Next = pProductNode->pNext;
	pProductNode_Ahead->pNext = pProductNode;
	if(pProduct_Next){
		pProduct_Next->pPrev = pProductNode_Ahead;
	}
	else{
		m_pProductTreeRoot->pProductNodeListTail = pProductNode_Ahead;
	}
	while(pProductNode->pFileListHead->pNext){
		FileNodePtr pFileNode = pProductNode->pFileListHead->pNext;
		pProductNode->pFileListHead->pNext = pProductNode->pFileListHead->pNext->pNext;
		FreeFileNode(pFileNode);
		pFileNode = NULL;
	}
	delete pProductNode->pFileListHead;
	pProductNode->pFileListHead = NULL;
	pProductNode->pFileListTail = NULL;
	delete pProductNode;
	pProductNode = NULL;
}

BOOL CTestDataTreeMgt::SearchFileNode(const char *sFileName, FileNodePtr &pFileNodeFound)
{
	if(sFileName==NULL || strlen(sFileName)==0){
		return FALSE;
	}

	ProductNodePtr pProductNode = m_pProductTreeRoot->pProductNodeListHead->pNext;
	while(pProductNode!=NULL){
		FileNodePtr pFileNode = pProductNode->pFileListHead->pNext;
		while(pFileNode!=NULL){
			char sBuf[MAX_BUF_LEN];
			TCHAR2char(sBuf, pFileNode->sFileName, _tcslen(pFileNode->sFileName));
			if(strcmp(sBuf, sFileName)==0){
				pFileNodeFound = pFileNode;
				return TRUE;
			}
			pFileNode = pFileNode->pNext;
		}
		pProductNode = pProductNode->pNext;
	}
	return FALSE;
}

void CTestDataTreeMgt::ProcessData(FileNodePtr pFileNode)
{
	int i, j, iSpdIndex;
	double min, max, avg;

	FileNode &node = *pFileNode;

	//对文件做处理
	memset(node.pAdditionInfo, 0, sizeof(struct AdditionInfo));
	if(node.pTestRecord->bDataValid==FALSE){//错误数据，返回
		node.bProcessed = TRUE;
		return;
	}
	if(node.pTestRecord->bNormalSpeed){//包含正常速度
		for(i=0;i<node.pTestRecord->iNumOfSpeed;i++){//对每一个速度进行处理

			//速度索引
			iSpdIndex = node.pTestRecord->iSpdIndex[i];

			//常规速度
			node.pTestRecord->fSetSpeed[i] = node.pProductInfo->fSpeed0[iSpdIndex];
			//常规速度实验的设定行程
			if(node.pProductInfo->bDifferentOffset){
				node.pTestRecord->fSetOffset[i] = node.pProductInfo->fSpeedOffset[iSpdIndex];
			}
			else{
				node.pTestRecord->fSetOffset[i] = node.pProductInfo->fOffset;
			}

			//求最大值最小值
			min = node.pTestRecord->fDisplacement[i][0];
			max = min;
			for(j=0;j<node.pTestRecord->iNumOfForce[i];j++){
				if(node.pTestRecord->fDisplacement[i][j] < min){
					min = node.pTestRecord->fDisplacement[i][j];
				}
				if(node.pTestRecord->fDisplacement[i][j] > max){
					max = node.pTestRecord->fDisplacement[i][j];
				}
			}
			//实测行程
			node.pTestRecord->fRealOffset[i] = (max-min)/2;

			//所有位移都扣掉位移中值
			avg = (max+min)/2;
			for(j=0;j<node.pTestRecord->iNumOfForce[i];j++){
				node.pTestRecord->fDisplacement[i][j] -= avg;
			}

			//求取7个参考点位置P0~P7
			int P[REFDOT_NUM];	//待计算
			int PP[REFDOT_NUM] = REFDOT_ARRAY;//预先定义好的参考数据
			int iNumOfRefDot;

			iNumOfRefDot = 1;
			for(j=SKIP_FIRSTDOT_NUM;j<node.pTestRecord->iNumOfForce[i]-1;j++){//略过前面一些波动较大的点
				if(node.pTestRecord->fDisplacement[i][j]<0 && node.pTestRecord->fDisplacement[i][j+1]>=0){
					P[iNumOfRefDot] = j+1;
					iNumOfRefDot++;
				}
				else if(node.pTestRecord->fDisplacement[i][j]>0 && node.pTestRecord->fDisplacement[i][j+1]<=0){
					P[iNumOfRefDot] = j+1;
					iNumOfRefDot++;
				}
				if(iNumOfRefDot>=REFDOT_NUM-1){
					break;
				}
			}
			int T = P[2]-P[1];//T为半个周期，即相邻两个过0点的距离
			//由P2和P1往后推算P0
			P[0] = P[1] - T;
			//再依次得到后续的值
			for(;iNumOfRefDot<REFDOT_NUM;iNumOfRefDot++){
				P[iNumOfRefDot] = P[iNumOfRefDot-1] + T;
			}

			//等比例算出有效数据起始位置
			for(j=0;j<REFDOT_NUM;j++){
				if(node.pProductInfo->iDataBandStart>=PP[j] && node.pProductInfo->iDataBandStart<=PP[j+1]){
					node.pAdditionInfo->iDataBandStart[i] = P[j] + T * (node.pProductInfo->iDataBandStart-PP[j]) / (PP[2]-PP[1]);
					break;
				}
			}
			//等比例算出有效数据宽度
			node.pAdditionInfo->iDataBandLen[i] = T * node.pProductInfo->iDataBandLen / (PP[2]-PP[1]);

			//实际速度
			node.pTestRecord->fRealSpeed[i] = 2 * M_PI * node.pTestRecord->fDataFreq[i] / (PP[2]-PP[1]) * node.pTestRecord->fRealOffset[i];

			//对位移进行低通滤波
			memcpy(node.pAdditionInfo->fDisplacementOfFilter[i]+node.pAdditionInfo->iDataBandStart[i],
					node.pTestRecord->fDisplacement[i]+node.pAdditionInfo->iDataBandStart[i],
					(node.pAdditionInfo->iDataBandLen[i]+2) * sizeof(double));
			filter_new(node.pAdditionInfo->fDisplacementOfFilter[i] + node.pAdditionInfo->iDataBandStart[i],
						node.pTestRecord->fDataFreq[i],
						COEF_OF_FILTER*node.pTestRecord->fSetSpeed[i]/node.pTestRecord->fSetOffset[i]/(2*M_PI), 
						(node.pAdditionInfo->iDataBandLen[i]+2));

			//对滤波后的位移进行微分，作为速度值存储下来
			double *pStart = node.pAdditionInfo->fDisplacementOfFilter[i]+node.pAdditionInfo->iDataBandStart[i];
			double fStart_0 = *pStart;
			double fStart_1 = *(pStart+1);
			for(j=node.pAdditionInfo->iDataBandStart[i];j<node.pAdditionInfo->iDataBandStart[i]+node.pAdditionInfo->iDataBandLen[i];j++){
				node.pAdditionInfo->fSpeedOfFilter[i][j] = -3 * (*pStart) + 4 * (*(pStart+1)) - (*(pStart+2));
				/*
				if(j<=node.pAdditionInfo->iDataBandStart[i]+node.pAdditionInfo->iDataBandLen[i]-3){
					node.pAdditionInfo->fSpeedOfFilter[i][j] = -3 * (*pStart) + 4 * (*(pStart+1)) - (*(pStart+2));
				}
				else if(j==node.pAdditionInfo->iDataBandStart[i]+node.pAdditionInfo->iDataBandLen[i]-2){
					node.pAdditionInfo->fSpeedOfFilter[i][j] = -3 * (*pStart) + 4 * (*(pStart+1)) - (fStart_0);
				}
				else{
					node.pAdditionInfo->fSpeedOfFilter[i][j] = -3 * (*pStart) + 4 * (fStart_0) - (fStart_1);
				}
				*/
				node.pAdditionInfo->fSpeedOfFilter[i][j] = node.pAdditionInfo->fSpeedOfFilter[i][j] / 2 * node.pTestRecord->fDataFreq[i];
				pStart ++;
			}

			//对力进行低通滤波
			memcpy(node.pAdditionInfo->fForceOfFilter[i]+node.pAdditionInfo->iDataBandStart[i],
					node.pTestRecord->fForce[i]+node.pAdditionInfo->iDataBandStart[i],
					node.pAdditionInfo->iDataBandLen[i] * sizeof(double));
			filter_new(node.pAdditionInfo->fForceOfFilter[i] + node.pAdditionInfo->iDataBandStart[i],
						node.pTestRecord->fDataFreq[i],
						COEF_OF_FILTER*node.pTestRecord->fSetSpeed[i]/node.pTestRecord->fSetOffset[i]/(2*M_PI), 
						node.pAdditionInfo->iDataBandLen[i]);

			//时间序列
			for(j=node.pAdditionInfo->iDataBandStart[i];j<node.pAdditionInfo->iDataBandStart[i]+node.pAdditionInfo->iDataBandLen[i];j++){
				node.pAdditionInfo->fTime[i][j] = (j-node.pAdditionInfo->iDataBandStart[i]) / node.pTestRecord->fDataFreq[i];
			}

			//记录最大力值和最小力值
			min = node.pAdditionInfo->fForceOfFilter[i][node.pAdditionInfo->iDataBandStart[i]];
			max = node.pAdditionInfo->fForceOfFilter[i][node.pAdditionInfo->iDataBandStart[i]];
			for(j=0;j<node.pAdditionInfo->iDataBandLen[i];j++){
				if(node.pAdditionInfo->fForceOfFilter[i][node.pAdditionInfo->iDataBandStart[i]+j]<min){
					min = node.pAdditionInfo->fForceOfFilter[i][node.pAdditionInfo->iDataBandStart[i]+j];
				}
				if(node.pAdditionInfo->fForceOfFilter[i][node.pAdditionInfo->iDataBandStart[i]+j]>max){
					max = node.pAdditionInfo->fForceOfFilter[i][node.pAdditionInfo->iDataBandStart[i]+j];
				}
			}
			node.pTestRecord->fPfm[i] = max;
			if(min<0){
				min = min * -1;
			}
			node.pTestRecord->fPym[i] = min;
		}//END OF FOR EACH SPEED
	}//END OF IF NORMAL SPEED

	node.pTestRecord->bFrictionSpeed = TRUE;//just for test
	if(node.pTestRecord->bFrictionSpeed){
		node.pTestRecord->fFrictionSetSpeed = node.pProductInfo->fFrictionSpeed0;
		if(node.pProductInfo->bDifferentOffset){
			node.pTestRecord->fFrictionSetOffset = node.pProductInfo->fFrictionSpeedOffset;
		}
		else{
			node.pTestRecord->fFrictionSetOffset = node.pProductInfo->fOffset;
		}

		min = node.pTestRecord->fFrictionDisplacement[0];
		max = min;
		for(j=0;j<node.pTestRecord->iFrictionNumOfForce;j++){
			if(node.pTestRecord->fFrictionDisplacement[j] < min){
				min = node.pTestRecord->fFrictionDisplacement[j];
			}
			if(node.pTestRecord->fFrictionDisplacement[j] > max){
				max = node.pTestRecord->fFrictionDisplacement[j];
			}
		}
		node.pTestRecord->fFrictionRealOffset = (max-min)/2;

		avg = (max+min)/2;
		for(j=0;j<node.pTestRecord->iFrictionNumOfForce;j++){
			node.pTestRecord->fFrictionDisplacement[j] -= avg;
		}

		int P[REFDOT_NUM];
		int PP[REFDOT_NUM] = REFDOT_ARRAY;
		int iNumOfRefDot;

		iNumOfRefDot = 1;
		for(j=SKIP_FIRSTDOT_NUM;j<node.pTestRecord->iFrictionNumOfForce-1;j++){
			if(node.pTestRecord->fFrictionDisplacement[j]<0 && node.pTestRecord->fFrictionDisplacement[j+1]>=0){
				P[iNumOfRefDot] = j+1;
				iNumOfRefDot++;
			}
			else if(node.pTestRecord->fFrictionDisplacement[j]>0 && node.pTestRecord->fFrictionDisplacement[j+1]<=0){
				P[iNumOfRefDot] = j+1;
				iNumOfRefDot++;
			}
			if(iNumOfRefDot>=REFDOT_NUM-1){
				break;
			}
		}
		int T = P[2]-P[1];
		P[0] = P[1] - T;
		for(;iNumOfRefDot<REFDOT_NUM;iNumOfRefDot++){
			P[iNumOfRefDot] = P[iNumOfRefDot-1] + T;
		}

		//等比例获取iDataBandStart和iDataBandLen
		for(j=0;j<REFDOT_NUM;j++){
			if(node.pProductInfo->iDataBandStart>=PP[j] && node.pProductInfo->iDataBandStart<=PP[j+1]){
				node.pAdditionInfo->iFrictionDataBandStart = P[j] + T * (node.pProductInfo->iDataBandStart-PP[j]) / (PP[2]-PP[1]);
				break;
			}
		}
		node.pAdditionInfo->iFrictionDataBandLen = T * node.pProductInfo->iDataBandLen / (PP[2]-PP[1]);

		node.pTestRecord->fFrictionRealSpeed = 2 * M_PI * node.pTestRecord->fFrictionDataFreq / node.pAdditionInfo->iFrictionDataBandLen * node.pTestRecord->fFrictionRealOffset;

		memcpy(node.pAdditionInfo->fFrictionDisplacementOfFilter+node.pAdditionInfo->iFrictionDataBandStart,
				node.pTestRecord->fFrictionDisplacement+node.pAdditionInfo->iFrictionDataBandStart,
				node.pAdditionInfo->iFrictionDataBandLen * sizeof(double));
		filter_new(node.pAdditionInfo->fFrictionDisplacementOfFilter + node.pAdditionInfo->iFrictionDataBandStart,
					node.pTestRecord->fFrictionDataFreq,
					COEF_OF_FILTER*node.pTestRecord->fFrictionSetSpeed/node.pTestRecord->fFrictionSetOffset/(2*M_PI), 
					node.pAdditionInfo->iFrictionDataBandLen);

		//对滤波后的位移进行微分，作为速度值存储下来
		double *pStart = node.pAdditionInfo->fFrictionDisplacementOfFilter+node.pAdditionInfo->iFrictionDataBandStart;
		double fStart_0 = *pStart;
		double fStart_1 = *(pStart+1);
		for(j=node.pAdditionInfo->iFrictionDataBandStart;j<node.pAdditionInfo->iFrictionDataBandStart+node.pAdditionInfo->iFrictionDataBandLen;j++){
			node.pAdditionInfo->fFrictionSpeedOfFilter[j] = -3 * (*pStart) + 4 * (*(pStart+1)) - (*(pStart+2));
			/*
			if(j<=node.pAdditionInfo->iDataBandStart[i]+node.pAdditionInfo->iDataBandLen[i]-3){
				node.pAdditionInfo->fSpeedOfFilter[i][j] = -3 * (*pStart) + 4 * (*(pStart+1)) - (*(pStart+2));
			}
			else if(j==node.pAdditionInfo->iDataBandStart[i]+node.pAdditionInfo->iDataBandLen[i]-2){
				node.pAdditionInfo->fSpeedOfFilter[i][j] = -3 * (*pStart) + 4 * (*(pStart+1)) - (fStart_0);
			}
			else{
				node.pAdditionInfo->fSpeedOfFilter[i][j] = -3 * (*pStart) + 4 * (fStart_0) - (fStart_1);
			}
			*/
			node.pAdditionInfo->fFrictionSpeedOfFilter[j] = node.pAdditionInfo->fFrictionSpeedOfFilter[j] / 2 * node.pTestRecord->fFrictionDataFreq;
			pStart ++;
		}

		memcpy(node.pAdditionInfo->fFrictionForceOfFilter+node.pAdditionInfo->iFrictionDataBandStart,
				node.pTestRecord->fFrictionForce+node.pAdditionInfo->iFrictionDataBandStart,
				node.pAdditionInfo->iFrictionDataBandLen * sizeof(double));
		filter_new(node.pAdditionInfo->fFrictionForceOfFilter + node.pAdditionInfo->iFrictionDataBandStart,
					node.pTestRecord->fFrictionDataFreq,
					COEF_OF_FILTER*node.pTestRecord->fFrictionSetSpeed/node.pTestRecord->fFrictionSetOffset/(2*M_PI), 
					node.pAdditionInfo->iFrictionDataBandLen);

		//时间序列
		for(j=node.pAdditionInfo->iFrictionDataBandStart;j<node.pAdditionInfo->iFrictionDataBandStart+node.pAdditionInfo->iFrictionDataBandLen;j++){
			node.pAdditionInfo->fFrictionTime[j] = (j-node.pAdditionInfo->iFrictionDataBandStart) / node.pTestRecord->fFrictionDataFreq;
		}

		min = node.pAdditionInfo->fFrictionForceOfFilter[node.pAdditionInfo->iFrictionDataBandStart];
		max = node.pAdditionInfo->fFrictionForceOfFilter[node.pAdditionInfo->iFrictionDataBandStart];
		for(j=0;j<node.pAdditionInfo->iFrictionDataBandLen;j++){
			if(node.pAdditionInfo->fFrictionForceOfFilter[node.pAdditionInfo->iFrictionDataBandStart+j]<min){
				min = node.pAdditionInfo->fFrictionForceOfFilter[node.pAdditionInfo->iFrictionDataBandStart+j];
			}
			if(node.pAdditionInfo->fFrictionForceOfFilter[node.pAdditionInfo->iFrictionDataBandStart+j]>max){
				max = node.pAdditionInfo->fFrictionForceOfFilter[node.pAdditionInfo->iFrictionDataBandStart+j];
			}
		}
		node.pTestRecord->fFrictionPfm = max;
		if(min<0){
			min = min * -1;
		}
		node.pTestRecord->fFrictionPym = min;
	}//END OF IF FRICTION

	node.bProcessed = TRUE;
}

BOOL CTestDataTreeMgt::PassFilter(FileNodePtr pFileNode)
{
	//检查文件是否符合过滤条件
	SYSTEMTIME st;
	//char sTestDate[MAX_BUF_LEN];
	char sOperatorNo[MAX_BUF_LEN];
	char sProductNo[MAX_BUF_LEN];

	//TCHAR2char(sTestDate, pFileNode->sTestDate, _tcslen(pFileNode->sTestDate));
	GetTimeFromStr(pFileNode->test_record_maininfo.sTestDate, st);

	if(m_filterConfig.bCheckTime){//检查时间
		CTime tTestDate(st);
		CTime tStartDate(m_filterConfig.StartTime);
		CTime tEndDate(m_filterConfig.EndTime);
		if(tTestDate.GetTime()<tStartDate.GetTime() || tTestDate.GetTime()>tEndDate.GetTime()){
			return FALSE;
		}
	}

	TCHAR2char(sProductNo, pFileNode->sProductNo, _tcslen(pFileNode->sProductNo));
	if((strlen(m_filterConfig.sProductNo)>0) && (!MatchString(sProductNo, m_filterConfig.sProductNo))){//检查型号
		return FALSE;
	}

	TCHAR2char(sOperatorNo, pFileNode->sOperatorNo, _tcslen(pFileNode->sOperatorNo));
	if((strlen(m_filterConfig.sOperatorNo)>0) && (!MatchString(sOperatorNo, m_filterConfig.sOperatorNo))){//检查操作员
		return FALSE;
	}

	return TRUE;
}

BOOL CTestDataTreeMgt::SearchProductNode(const char *sProductNo, ProductNodePtr &pProductNodeFound)
{
	if(sProductNo==NULL || strlen(sProductNo)==0){
		return FALSE;
	}

	ProductNodePtr pProductNode = m_pProductTreeRoot->pProductNodeListHead->pNext;
	while(pProductNode!=NULL){
		TCHAR tcProductNo[MAX_BUF_LEN];
		char2TCHAR(tcProductNo, sProductNo, (int)strlen(sProductNo));
		if(_tcscmp(pProductNode->sProductNo, tcProductNo)==0){
			pProductNodeFound = pProductNode;
			return TRUE;
		}
		pProductNode = pProductNode->pNext;
	}

	return FALSE;
}

void CTestDataTreeMgt::ResetNodeStatus()
{
	int i;
	ProductNodePtr pProductNode = m_pProductTreeRoot->pProductNodeListHead->pNext;
	while(pProductNode!=NULL){
		FileNodePtr pFileNode = pProductNode->pFileListHead->pNext;
		while(pFileNode!=NULL){
			for(i=0;i<pFileNode->test_record_maininfo.iNumOfSpeed;i++){
				pFileNode->tree_item_data[i].bSelected = FALSE;
			}
			pFileNode = pFileNode->pNext;
		}
		pProductNode = pProductNode->pNext;
	}
}

ProductTreeRootPtr CTestDataTreeMgt::GetProductTreeRootPtr()
{
	return m_pProductTreeRoot;
}

///滤波,返回值： 0: 正常， －1: 内存不足 
int CTestDataTreeMgt::filter_new( 
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
int CTestDataTreeMgt::fir_dsgn(int Len, double FreqS, double FreqB , double **Coef1)  
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
