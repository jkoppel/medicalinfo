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
			LoadFile(buf);
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
			if(!(IsFileInConfigDirList(pFileNode->sFileName) && ff.FindFile(pFileNode->sFileName))){
				//并非目录列表有该文件，并且该文件实际存在
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
	delete pFileNode->pMachineInfo;
	delete pFileNode->pProductInfo;
	delete pFileNode->pTestRecord;
	delete pFileNode->pAdditionInfo;
	delete pFileNode;
}

BOOL CTestDataTreeMgt::LoadFile(const char *sFileName)
{
	FileNodePtr pFileNode = NULL;
	ProductNodePtr pProductNode = NULL;

	if(SearchFileNode(sFileName, pFileNode)){
		if(pFileNode->bProcessed){
			return TRUE;
		}
		ProcessData(pFileNode);
		return TRUE;
	}
	pFileNode = new FileNode;
	memset(pFileNode, 0, sizeof(FileNode));
	if(!GetFileHeader(sFileName, pFileNode)){
		FreeFileNode(pFileNode);
		return FALSE;
	}
	char sProductNo[33];
	TCHAR2char(sProductNo, pFileNode->sProductNo, _tcslen(pFileNode->sProductNo));
	if(!SearchProductNode(sProductNo, pProductNode)){
		InsertProductNode(sProductNo, pProductNode);
	}
	if(!InsertFileNode(pProductNode, pFileNode)){
		FreeFileNode(pFileNode);
		return FALSE;
	}
	return TRUE;
}

BOOL CTestDataTreeMgt::GetFileHeader(const char *sFileName, FileNodePtr pFileNode)
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

	//略过前面MachineInfo,读取ProductInfo的ProductNo
	fseek(pFile, sizeof(struct CCMachineInfo)+30, SEEK_SET);
	ret = (int)fread(sBuf, 33, 1, pFile);
	if(ret!=1){
		fclose(pFile);
		return FALSE;
	}
	char2TCHAR(node.sProductNo, sBuf, 33);

	//直接读取数据
	fseek(pFile, sizeof(struct CCMachineInfo)+sizeof(struct CCProductInfo)+30, SEEK_SET);

	ret = (int)fread(&node.test_record_header, sizeof(struct CCTestRecordHeader), 1, pFile);
	fclose(pFile);
	if(ret!=1){
		return FALSE;
	}

	char2TCHAR(node.sFileName, sFileName, (int)strlen(sFileName));
	char2TCHAR(node.sTestDate, node.test_record_header.sTestDate, (int)strlen(node.test_record_header.sTestDate));

	node.bProcessed = FALSE;
	for(i=0;i<node.test_record_header.iNumOfSpeed;i++){
		node.tree_item_data[i].iIndex = i;
		node.tree_item_data[i].pNode = &node;
	}

	return TRUE;
}

BOOL CTestDataTreeMgt::LoadAndProcessFile(const char *sFileName, FileNodePtr pFileNode)
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

	memcpy(&node.test_record_header, node.pTestRecord, sizeof(struct CCTestRecordHeader));

	char2TCHAR(node.sProductNo, node.pProductInfo->sProductNo, (int)strlen(node.pProductInfo->sProductNo));
	char2TCHAR(node.sFileName, sFileName, (int)strlen(sFileName));
	char2TCHAR(node.sTestDate, node.test_record_header.sTestDate, (int)strlen(node.test_record_header.sTestDate));

	for(i=0;i<node.test_record_header.iNumOfSpeed;i++){
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
		if(strcmp(pFileNodeToInsert->test_record_header.sTestDate, pFileNode->test_record_header.sTestDate)>=0){
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
		FreeFileNode(pFileNode);
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
	int i, j;
	float min, max, avg;

	FileNode &node = *pFileNode;

	//对文件做处理
	memset(node.pAdditionInfo, 0, sizeof(node.pAdditionInfo));
	if(node.pTestRecord->bDataValid==FALSE){
		node.bProcessed = TRUE;
		return;
	}
	if(node.pTestRecord->bNormalSpeed){
		for(i=0;i<node.pTestRecord->iNumOfSpeed;i++){
			j = node.pTestRecord->iSpdIndex[i];
			node.pTestRecord->fSetSpeed[i] = node.pProductInfo->fSpeed0[j];
			if(node.pProductInfo->bDifferentOffset){
				node.pTestRecord->fSetOffset[i] = node.pProductInfo->fSpeedOffset[j];
			}
			else{
				node.pTestRecord->fSetOffset[i] = node.pProductInfo->fOffset;
			}

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
			avg = (max+min)/2;
			for(j=0;j<node.pTestRecord->iNumOfForce[i];j++){
				node.pTestRecord->fDisplacement[i][j] -= avg;
			}
			node.pAdditionInfo->fDisplacementLength[i] = (max-min)/2;

			int P[MAX_REFDOT_NUM];
			int PP[MAX_REFDOT_NUM] = {90, 270, 450, 630, 810, 990, 1260};
			int iNumOfRefDot;

			iNumOfRefDot = 1;
			for(j=SKIP_FIRSTDOT_NUM;j<node.pTestRecord->iNumOfForce[i]-1;j++){
				if(node.pTestRecord->fDisplacement[i][j]<0 && node.pTestRecord->fDisplacement[i][j+1]>=0){
					P[iNumOfRefDot] = j+1;
					iNumOfRefDot++;
				}
				else if(node.pTestRecord->fDisplacement[i][j]>0 && node.pTestRecord->fDisplacement[i][j+1]<=0){
					P[iNumOfRefDot] = j+1;
					iNumOfRefDot++;
				}
				if(iNumOfRefDot>=MAX_REFDOT_NUM-1){
					break;
				}
			}
			int T = P[2]-P[1];
			P[0] = P[1] - T;
			for(;iNumOfRefDot<MAX_REFDOT_NUM;iNumOfRefDot++){
				P[iNumOfRefDot] = P[iNumOfRefDot-1] + T;
			}

			//等比例获取iDataBandStart和iDataBandLen
			for(j=0;j<MAX_REFDOT_NUM;j++){
				if(node.pProductInfo->iDataBandStart>=PP[j] && node.pProductInfo->iDataBandStart<=PP[j+1]){
					node.pAdditionInfo->iDataBandStart[i] = P[j] + T * (node.pProductInfo->iDataBandStart-PP[j]) / (PP[2]-PP[1]);
					break;
				}
			}
			node.pAdditionInfo->iDataBandLen[i] = T * node.pProductInfo->iDataBandLen / (PP[2]-PP[1]);

			node.pTestRecord->fRealSpeed[i] = 2 * M_PI * node.pTestRecord->fDataFreq[i] / node.pAdditionInfo->iDataBandLen[i] * node.pTestRecord->fRealSpeed[i];

			memcpy(node.pAdditionInfo->fForceOfFilter[i]+node.pAdditionInfo->iDataBandStart[i],
					node.pTestRecord->fForce[i]+node.pAdditionInfo->iDataBandStart[i],
					node.pAdditionInfo->iDataBandLen[i] * sizeof(double));
			filter_new(node.pAdditionInfo->fForceOfFilter[i] + node.pAdditionInfo->iDataBandStart[i],
						node.pTestRecord->fDataFreq[i],
						10*node.pTestRecord->fSetSpeed[i]/node.pTestRecord->fSetOffset[i]/(2*M_PI), 
						node.pAdditionInfo->iDataBandLen[i]);

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

			//TODO 处理摩擦力

			//处理微分 (-3Dj + 4Dj+1 - Dj+2) / 2dt
			//单文件模式的两种模式
			//文件最多加载一些，没有的就release
			//
		}
	}
	node.bProcessed = TRUE;
}

BOOL CTestDataTreeMgt::SearchProductNode(const char *sProductNo, ProductNodePtr &pProductNodeFound)
{
	if(sProductNo==NULL || strlen(sProductNo)==0){
		return FALSE;
	}

	ProductNodePtr pProductNode = m_pProductTreeRoot->pProductNodeListHead->pNext;
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

void CTestDataTreeMgt::ResetNodeStatus()
{
	int i;
	ProductNodePtr pProductNode = m_pProductTreeRoot->pProductNodeListHead->pNext;
	while(pProductNode!=NULL){
		FileNodePtr pFileNode = pProductNode->pFileListHead->pNext;
		while(pFileNode!=NULL){
			for(i=0;i<pFileNode->test_record_header.iNumOfSpeed;i++){
				pFileNode->tree_item_data[i].bSelected = FALSE;
			}
			pFileNode = pFileNode->pNext;
		}
		pProductNode = pProductNode->pNext;
	}
}
