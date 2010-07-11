#include "StdAfx.h"
#include "TestDataTreeMgt.h"
#include "GlobalFuncs.h"
#include "GlobalVars.h"

CTestDataTreeMgt::CTestDataTreeMgt(void)
{
	m_pProductTreeRoot = NULL;
}

CTestDataTreeMgt::~CTestDataTreeMgt(void)
{
	FreeTree();
}

void CTestDataTreeMgt::InitTree(void)
{
	LoadDirFromConfigFile();

	int i;
	CFileFind ff;
	BOOL bDecide = FALSE;
	CString str, name;
	char buf[1024];

	FreeTree();

	m_pProductTreeRoot = new ProductTreeRoot;
	memset(m_pProductTreeRoot, 0, sizeof(ProductTreeRoot));
	m_pProductTreeRoot->pProductNodeListHead = new ProductNode;
	m_pProductTreeRoot->pProductNodeListTail = m_pProductTreeRoot->pProductNodeListHead;
	memset(m_pProductTreeRoot->pProductNodeListHead, 0, sizeof(ProductNode));

	for(i=0;i<g_saDirectories.GetCount();i++){
		str = g_saDirectories.GetAt(i);
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
			LoadFile(buf, FALSE);
		}
		ff.Close();
	}
}

void CTestDataTreeMgt::ReloadTree(void)
{
	LoadDirFromConfigFile();

	int i;
	CFileFind ff;
	BOOL bDecide = FALSE;
	CString str, name;
	char buf[1024];

	if(m_pProductTreeRoot==NULL){
		m_pProductTreeRoot = new ProductTreeRoot;
		memset(m_pProductTreeRoot, 0, sizeof(ProductTreeRoot));
		m_pProductTreeRoot->pProductNodeListHead = new ProductNode;
		memset(m_pProductTreeRoot->pProductNodeListHead, 0, sizeof(ProductNode));
	}

	for(i=0;i<g_saDirectories.GetCount();i++){
		str = g_saDirectories.GetAt(i);
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
			LoadFile(buf);
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
			if(!(IsFileInConfigDirList(pFileNode->addition_info.sFile) && ff.FindFile(pFileNode->addition_info.sFile))){
				//并非目录列表有该文件，并且该文件实际存在
				FileNodePtr pFileNode_Next = pFileNode->pNext;
				pFileNode_Ahead->pNext = pFileNode_Next;
				if(pFileNode_Next){
					pFileNode_Next->pPrev = pFileNode_Ahead;
				}
				else{
					pProductNode->pFileListTail = pFileNode_Ahead;
				}
				delete pFileNode;
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
			delete pFileNode;
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

BOOL CTestDataTreeMgt::LoadFile(const char *sFileName, BOOL bNeedProcessData)
{
	FileNodePtr pFileNode = NULL;
	ProductNodePtr pProductNode = NULL;

	if(SearchFileNode(sFileName, pFileNode)){
		if(pFileNode->bDataProcessed){
			return TRUE;
		}
		if(bNeedProcessData){
			ProcessData(pFileNode);
		}
		return TRUE;
	}
	pFileNode = new FileNode;
	memset(pFileNode, 0, sizeof(FileNode));
	if(!ReadFile(sFileName, bNeedProcessData, pFileNode)){
		delete pFileNode;
		return FALSE;
	}
	if(!SearchProductNode(pFileNode->product_info.sProductNo, pProductNode)){
		InsertProductNode(pFileNode->product_info.sProductNo, pProductNode);
	}
	if(!InsertFileNode(pProductNode, pFileNode)){
		delete pFileNode;
		return FALSE;
	}
	return TRUE;
}

BOOL CTestDataTreeMgt::ReadFile(const char *sFileName, BOOL bNeedProcessData, FileNodePtr pFileNode)
{
	int ret, i;
	FILE *pFile = NULL;
	char sBuf[256], sBuf1[256];
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

	//查看文件头
	memset(sBuf, 0, sizeof(sBuf));
	fread(sBuf, 23, 1, pFile);
	sprintf_s(sBuf1, "%s", "MyTester Data File ");
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

	//略过6718字节
	if(fseek(pFile, 30, SEEK_SET)!=0){
		fclose(pFile);
		return FALSE;
	}

	ret = (int)fread(&node.machine_info, sizeof(struct CCMachineInfo), 1, pFile);
	if(ret!=1){
		fclose(pFile);
		return FALSE;
	}

	ret = (int)fread(&node.product_info, sizeof(struct CCProductInfo), 1, pFile);
	if(ret!=1){
		fclose(pFile);
		return FALSE;
	}

	//直接读取数据
	ret = (int)fread(&node.test_record, sizeof(struct CCTestRecord), 1, pFile);
	fclose(pFile);
	if(ret!=1){
		return FALSE;
	}

	char2TCHAR(node.addition_info.sFile, sFileName, (int)strlen(sFileName));
	char2TCHAR(node.addition_info.sTestDate, node.test_record.sTestDate, (int)strlen(node.test_record.sTestDate));

	memset(node.tree_item_data, 0, sizeof(node.tree_item_data));
	node.bDataProcessed = FALSE;
	for(i=0;i<node.test_record.iNumOfSpeed;i++){
		node.tree_item_data[i].iIndex = i;
		node.tree_item_data[i].pNode = &node;
	}

	if(!bNeedProcessData){
		return TRUE;
	}

	ProcessData(&node);

	node.bDataProcessed = TRUE;
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
		if(strcmp(pFileNodeToInsert->test_record.sTestDate, pFileNode->test_record.sTestDate)>=0){
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
	delete pFileNode;

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
			char sBuf[256];
			TCHAR2char(sBuf, pFileNode->addition_info.sFile, _tcslen(pFileNode->addition_info.sFile));
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
			delete pFileNode;
			
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
		delete pFileNode;
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
		TCHAR tcProductNo[256];
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
		delete pFileNode;
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
			char sBuf[256];
			TCHAR2char(sBuf, pFileNode->addition_info.sFile, _tcslen(pFileNode->addition_info.sFile));
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
	int i, j;
	float min, max, avg;

	FileNode &node = *pFileNode;

	//对文件做处理
	memset(&node.addition_info, 0, sizeof(node.addition_info));
	if(node.test_record.bDataValid==FALSE){
		return;
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
			for(j=0;j<node.test_record.iNumOfForce[i];j++){
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
}

BOOL CTestDataTreeMgt::SearchProductNode(const char *sProductNo, ProductNodePtr &pProductNodeFound)
{
	if(sProductNo==NULL || strlen(sProductNo)==0){
		return FALSE;
	}

	ProductNodePtr pProductNode = m_pProductTreeRoot->pProductNodeListHead;
	while(pProductNode!=NULL){
		TCHAR tcProductNo[256];
		char2TCHAR(tcProductNo, sProductNo, (int)strlen(sProductNo));
		if(_tcscmp(pProductNode->sProductNo, tcProductNo)==0){
			pProductNodeFound = pProductNode;
			return TRUE;
		}
		pProductNode = pProductNode->pNext;
	}

	return FALSE;
}

