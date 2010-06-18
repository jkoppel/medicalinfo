#include "stdafx.h"
#include "DataFile.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MIBase.h"

///创建数据文件:建立该文件,并且往数据文件中写入:记录个数0(4字节),记录长度(4字节)
/**
  * path : 文件路径 \n
  * RecLength : 记录的大小 \n
  * 返回 : SUCC/FAIL
  */
int DataFile_Create(const char *path, unsigned int RecLength)
{
	int count;
	FILE * pFile = NULL;
	unsigned int RecNum;

	//检查输入
	if(path==NULL || RecLength==0){
		return FAIL;
	}

	//创建文件
	pFile = fopen(path, "wb+");
	if(pFile==NULL){
		return FAIL;
	}

	//写入记录个数(=0)
	RecNum = 0;
	count = fwrite(&RecNum, 4, 1, pFile);
	if(count!=1){
		fclose(pFile);
		return FAIL;
	}

	//写入记录长度
	count = fwrite(&RecLength, 4, 1, pFile);
	fclose(pFile);

	return (count==1) ? SUCC : FAIL;
}

///删除数据文件
/**
  * path : 文件路径 \n
  * 返回 : SUCC/FAIL
  */
int DataFile_Delete(const char *path)
{
	int ret;

	//检查输入
	if(path==NULL){
		return FAIL;
	}

	//删除文件
	
	//ret = unlink(path);//deleted by hwy, TODO
	return (ret==0) ? SUCC : FAIL;
}

///向数据文件中追加一条记录
/**
  * path : 数据文件路径 \n
  * RecData : 待写入的数据 \n
  * 返回 : SUCC/FAIL
  */
int DataFile_AppendRec(const char *path,
						const void *RecData)
{
	int ret, count;
	FILE *pFile = NULL;
	unsigned int RecNum, RecLength;

	//检查输入
	if(path==NULL || RecData==NULL){
		return FAIL;
	}

	//打开文件
	pFile = fopen(path, "rb+");
	if(pFile==NULL){
		return FAIL;
	}

	//读取记录个数
	count = fread(&RecNum, 4, 1, pFile);
	if(count!=1 && !(count==0 && feof(pFile))){
		fclose(pFile);
		return FAIL;
	}

	//读取记录长度
	count = fread(&RecLength, 4, 1, pFile);
	if(count!=1 && !(count==0 && feof(pFile))){
		fclose(pFile);
		return FAIL;
	}

	//记录指针移至待写入位置
	ret = fseek(pFile, 8+RecNum*RecLength, SEEK_SET);
	if(ret!=0){
		fclose(pFile);
		return FAIL;
	}

	//写入记录
	count = fwrite(RecData, RecLength, 1, pFile);
	if(count!=1){
		fclose(pFile);
		return FAIL;
	}

	//将记录个数增1
	RecNum ++;
	fseek(pFile, 0, SEEK_SET);
	count = fwrite(&RecNum, 4, 1, pFile);
	fclose(pFile);

	return (count==1) ? SUCC : FAIL;
}

///删除数据文件的一条记录
/**
  * path : 路径 \n
  * index : 待删除记录的索引 \n
  * 返回 : SUCC/其它
  */
int DataFile_DeleteRec(const char *path, unsigned int index)
{
	int ret,count;
	FILE *pFile = NULL;
	void *RecData;
	unsigned int RecNum, RecLength;
	
	//检查输入
	if(path==NULL){
		return FAIL;
	}

	//打开文件
	pFile = fopen(path, "rb+");
	if(pFile==NULL){
		return FAIL;
	}
	//读取记录个数
	count = fread(&RecNum, 4, 1, pFile);
	if(count!=1 && !(count==0 && feof(pFile))){
		fclose(pFile);
		return FAIL;
	}

	//读取记录长度
	count = fread(&RecLength, 4, 1, pFile);
	if(count!=1 && !(count==0 && feof(pFile))){
		fclose(pFile);
		return FAIL;
	}

	RecData = new char[RecLength];

	//检查输入
	if(index>=RecNum || index<0){
		fclose(pFile);
		delete []RecData;
		return FAIL;
	}
    //
	for(index;index<RecNum;index++){
		//移动记录指针
		ret = fseek(pFile, 8+(index+1)*RecLength, SEEK_SET);
		if(ret!=0){
			fclose(pFile);
			delete []RecData;
			return FAIL;
		}
		//读取数据
		count = fread(RecData, RecLength, 1, pFile);
		if(count!=1 && !(count==0 && feof(pFile))){
			fclose(pFile);
			delete []RecData;
			return FAIL;
		}
        //移动记录指针
		ret = fseek(pFile, 8+index*RecLength, SEEK_SET);
		if(ret!=0){
			fclose(pFile);
			delete []RecData;
			return FAIL;
		}
		//写入数据
		count = fwrite(RecData, RecLength, 1, pFile);
    	if(count!=1){
	    	fclose(pFile);
			delete []RecData;
		    return FAIL;
		}
	}
	//记录个数-1
	RecNum --;
	fseek(pFile, 0, SEEK_SET);
	count = fwrite(&RecNum, 4, 1, pFile);
	fclose(pFile);
	delete []RecData;
	return (count==1) ? SUCC : FAIL;

}

///从数据文件中读取指定记录
/**
  * path : 数据文件路径 \n
  * index : 待读取的记录的索引号 \n
  * RecData : 返回读取的记录数据 \n
  * 返回 : SUCC/FAIL
  */
int DataFile_ReadRec(const char *path, 
					 unsigned int index,
					 void *RecData)
{
	int ret, count;
	unsigned int RecNum, RecLength;
	FILE *pFile = NULL;

	//检查输入
	if(path==NULL || RecData==NULL){
		return FAIL;
	}

	//打开文件
	pFile = fopen(path, "rb");
	if(pFile==NULL){
		return FAIL;
	}

	//读取记录个数
	count = fread(&RecNum, 4, 1, pFile);
	if(count!=1 && !(count==0 && feof(pFile))){
		fclose(pFile);
		return FAIL;
	}

	//读取记录长度
	count = fread(&RecLength, 4, 1, pFile);
	if(count!=1 && !(count==0 && feof(pFile))){
		fclose(pFile);
		return FAIL;
	}

	//检查输入
	if(index>=RecNum){
		fclose(pFile);
		return FAIL;
	}

	//移动记录指针
	ret = fseek(pFile, 8+index*RecLength, SEEK_SET);
	if(ret!=0){
		fclose(pFile);
		return FAIL;
	}

	//读取数据
	count = fread(RecData, RecLength, 1, pFile);
	fclose(pFile);
	
	return (count==1 || (count==0 && feof(pFile))) ? SUCC : FAIL;
}

///获取数据文件的记录个数
/**
  * path : 数据文件的路径 \n
  * RecNum : 返回数据文件的记录数量 \n
  * 返回 : SUCC/FAIL
  */
int DataFile_GetRecNum(const char *path, unsigned int &RecNum)
{
	FILE *pFile = NULL;
	int count;

	//检查输入
	if(path==NULL){
		return FAIL;
	}

	//打开文件
	pFile = fopen(path, "rb");
	if(pFile==NULL){
		return FAIL;
	}

	//读取记录个数
	count = fread(&RecNum, 4, 1, pFile);
	fclose(pFile);
	
	return (count==1 || (count==0 && feof(pFile))) ? SUCC : FAIL;
}

///修改数据文件中的记录
/***
   * path : 数据文件路径 \n
   * index : 待修改的记录的索引号 \n
   * RecData : 待写入的数据 \n
   * 返回 : SUCC/FAIL
   */
int DataFile_ModRec(const char *path,unsigned int index,const void *RecData)
{
	int ret, count;
	unsigned int RecNum, RecLength;
	FILE *pFile = NULL;

	//检查输入
	if(path==NULL || RecData==NULL){
		return FAIL;
	}

	//打开文件
	pFile = fopen(path, "rb+");
	if(pFile==NULL){
		return FAIL;
	}

	//读取记录个数
	count = fread(&RecNum, 4, 1, pFile);
	if(count!=1 && !(count==0 && feof(pFile))){
		fclose(pFile);
		return FAIL;
	}

	//读取记录长度
	count = fread(&RecLength, 4, 1, pFile);
	if(count!=1 && !(count==0 && feof(pFile))){
		fclose(pFile);
		return FAIL;
	}

	//检查输入
	if(index>=RecNum){
		fclose(pFile);
		return FAIL;
	}

	//移动记录指针
	ret = fseek(pFile, 8+index*RecLength, SEEK_SET);
	if(ret!=0){
		fclose(pFile);
		return FAIL;
	}
	//写入数据
	count = fwrite(RecData, RecLength, 1, pFile);
   	if(count!=1){
		fclose(pFile);
		return FAIL;
	}
	fclose(pFile);
	return SUCC;
}

#define INVOICE_CODE_LENGTH 20
struct BuyRollInfo{
	unsigned int StartNo;
	unsigned int EndNo;
	int RollNum;
	unsigned char Code[INVOICE_CODE_LENGTH];
};

//测试数据文件
void TestDataFile()
{
	int ret;
	char path[] = ".\\Test.dat";
	struct BuyRollInfo b1, b2;
	unsigned RecNum;

	ret = DataFile_Create(path, sizeof(struct BuyRollInfo));
	printf("ret = %d\n", ret);
	
	ret = DataFile_GetRecNum(path, RecNum);

	memset(b1.Code, 0xAA, INVOICE_CODE_LENGTH);
	b1.StartNo = 1;
	b1.EndNo = 10;
	b1.RollNum = 2;
	ret = DataFile_AppendRec(path, (void*)&b1);

	b1.StartNo = 11;
	b1.EndNo = 20;
	b1.RollNum = 1;
	ret = DataFile_AppendRec(path, (void*)&b1);

	b1.StartNo = 21;
	b1.EndNo = 30;
	b1.RollNum = 1;
	ret = DataFile_AppendRec(path, (void*)&b1);

	ret = DataFile_GetRecNum(path, RecNum);

	ret = DataFile_ReadRec(path, 0, (void*)&b2);
	ret = DataFile_ReadRec(path, 1, (void*)&b2);
	ret = DataFile_ReadRec(path, 2, (void*)&b2);
	ret = DataFile_ReadRec(path, 3, (void*)&b2);
	b1.StartNo = 1;
	b1.EndNo = 1;
	b1.RollNum = 1;
	ret = DataFile_ModRec(path,2,(void*)&b1);
	ret = DataFile_ReadRec(path, 0, (void*)&b2);
	ret = DataFile_ReadRec(path, 1, (void*)&b2);
	ret = DataFile_ReadRec(path, 2, (void*)&b2);
	ret = DataFile_ReadRec(path, 3, (void*)&b2);

	ret = DataFile_GetRecNum(path, RecNum);

	DataFile_Delete(path);
}
