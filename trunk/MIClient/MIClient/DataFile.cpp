#include "stdafx.h"
#include "DataFile.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MIBase.h"

///���������ļ�:�������ļ�,�����������ļ���д��:��¼����0(4�ֽ�),��¼����(4�ֽ�)
/**
  * path : �ļ�·�� \n
  * RecLength : ��¼�Ĵ�С \n
  * ���� : SUCC/FAIL
  */
int DataFile_Create(const char *path, unsigned int RecLength)
{
	int count;
	FILE * pFile = NULL;
	unsigned int RecNum;

	//�������
	if(path==NULL || RecLength==0){
		return FAIL;
	}

	//�����ļ�
	pFile = fopen(path, "wb+");
	if(pFile==NULL){
		return FAIL;
	}

	//д���¼����(=0)
	RecNum = 0;
	count = fwrite(&RecNum, 4, 1, pFile);
	if(count!=1){
		fclose(pFile);
		return FAIL;
	}

	//д���¼����
	count = fwrite(&RecLength, 4, 1, pFile);
	fclose(pFile);

	return (count==1) ? SUCC : FAIL;
}

///ɾ�������ļ�
/**
  * path : �ļ�·�� \n
  * ���� : SUCC/FAIL
  */
int DataFile_Delete(const char *path)
{
	int ret;

	//�������
	if(path==NULL){
		return FAIL;
	}

	//ɾ���ļ�
	
	//ret = unlink(path);//deleted by hwy, TODO
	return (ret==0) ? SUCC : FAIL;
}

///�������ļ���׷��һ����¼
/**
  * path : �����ļ�·�� \n
  * RecData : ��д������� \n
  * ���� : SUCC/FAIL
  */
int DataFile_AppendRec(const char *path,
						const void *RecData)
{
	int ret, count;
	FILE *pFile = NULL;
	unsigned int RecNum, RecLength;

	//�������
	if(path==NULL || RecData==NULL){
		return FAIL;
	}

	//���ļ�
	pFile = fopen(path, "rb+");
	if(pFile==NULL){
		return FAIL;
	}

	//��ȡ��¼����
	count = fread(&RecNum, 4, 1, pFile);
	if(count!=1 && !(count==0 && feof(pFile))){
		fclose(pFile);
		return FAIL;
	}

	//��ȡ��¼����
	count = fread(&RecLength, 4, 1, pFile);
	if(count!=1 && !(count==0 && feof(pFile))){
		fclose(pFile);
		return FAIL;
	}

	//��¼ָ��������д��λ��
	ret = fseek(pFile, 8+RecNum*RecLength, SEEK_SET);
	if(ret!=0){
		fclose(pFile);
		return FAIL;
	}

	//д���¼
	count = fwrite(RecData, RecLength, 1, pFile);
	if(count!=1){
		fclose(pFile);
		return FAIL;
	}

	//����¼������1
	RecNum ++;
	fseek(pFile, 0, SEEK_SET);
	count = fwrite(&RecNum, 4, 1, pFile);
	fclose(pFile);

	return (count==1) ? SUCC : FAIL;
}

///ɾ�������ļ���һ����¼
/**
  * path : ·�� \n
  * index : ��ɾ����¼������ \n
  * ���� : SUCC/����
  */
int DataFile_DeleteRec(const char *path, unsigned int index)
{
	int ret,count;
	FILE *pFile = NULL;
	void *RecData;
	unsigned int RecNum, RecLength;
	
	//�������
	if(path==NULL){
		return FAIL;
	}

	//���ļ�
	pFile = fopen(path, "rb+");
	if(pFile==NULL){
		return FAIL;
	}
	//��ȡ��¼����
	count = fread(&RecNum, 4, 1, pFile);
	if(count!=1 && !(count==0 && feof(pFile))){
		fclose(pFile);
		return FAIL;
	}

	//��ȡ��¼����
	count = fread(&RecLength, 4, 1, pFile);
	if(count!=1 && !(count==0 && feof(pFile))){
		fclose(pFile);
		return FAIL;
	}

	RecData = new char[RecLength];

	//�������
	if(index>=RecNum || index<0){
		fclose(pFile);
		delete []RecData;
		return FAIL;
	}
    //
	for(index;index<RecNum;index++){
		//�ƶ���¼ָ��
		ret = fseek(pFile, 8+(index+1)*RecLength, SEEK_SET);
		if(ret!=0){
			fclose(pFile);
			delete []RecData;
			return FAIL;
		}
		//��ȡ����
		count = fread(RecData, RecLength, 1, pFile);
		if(count!=1 && !(count==0 && feof(pFile))){
			fclose(pFile);
			delete []RecData;
			return FAIL;
		}
        //�ƶ���¼ָ��
		ret = fseek(pFile, 8+index*RecLength, SEEK_SET);
		if(ret!=0){
			fclose(pFile);
			delete []RecData;
			return FAIL;
		}
		//д������
		count = fwrite(RecData, RecLength, 1, pFile);
    	if(count!=1){
	    	fclose(pFile);
			delete []RecData;
		    return FAIL;
		}
	}
	//��¼����-1
	RecNum --;
	fseek(pFile, 0, SEEK_SET);
	count = fwrite(&RecNum, 4, 1, pFile);
	fclose(pFile);
	delete []RecData;
	return (count==1) ? SUCC : FAIL;

}

///�������ļ��ж�ȡָ����¼
/**
  * path : �����ļ�·�� \n
  * index : ����ȡ�ļ�¼�������� \n
  * RecData : ���ض�ȡ�ļ�¼���� \n
  * ���� : SUCC/FAIL
  */
int DataFile_ReadRec(const char *path, 
					 unsigned int index,
					 void *RecData)
{
	int ret, count;
	unsigned int RecNum, RecLength;
	FILE *pFile = NULL;

	//�������
	if(path==NULL || RecData==NULL){
		return FAIL;
	}

	//���ļ�
	pFile = fopen(path, "rb");
	if(pFile==NULL){
		return FAIL;
	}

	//��ȡ��¼����
	count = fread(&RecNum, 4, 1, pFile);
	if(count!=1 && !(count==0 && feof(pFile))){
		fclose(pFile);
		return FAIL;
	}

	//��ȡ��¼����
	count = fread(&RecLength, 4, 1, pFile);
	if(count!=1 && !(count==0 && feof(pFile))){
		fclose(pFile);
		return FAIL;
	}

	//�������
	if(index>=RecNum){
		fclose(pFile);
		return FAIL;
	}

	//�ƶ���¼ָ��
	ret = fseek(pFile, 8+index*RecLength, SEEK_SET);
	if(ret!=0){
		fclose(pFile);
		return FAIL;
	}

	//��ȡ����
	count = fread(RecData, RecLength, 1, pFile);
	fclose(pFile);
	
	return (count==1 || (count==0 && feof(pFile))) ? SUCC : FAIL;
}

///��ȡ�����ļ��ļ�¼����
/**
  * path : �����ļ���·�� \n
  * RecNum : ���������ļ��ļ�¼���� \n
  * ���� : SUCC/FAIL
  */
int DataFile_GetRecNum(const char *path, unsigned int &RecNum)
{
	FILE *pFile = NULL;
	int count;

	//�������
	if(path==NULL){
		return FAIL;
	}

	//���ļ�
	pFile = fopen(path, "rb");
	if(pFile==NULL){
		return FAIL;
	}

	//��ȡ��¼����
	count = fread(&RecNum, 4, 1, pFile);
	fclose(pFile);
	
	return (count==1 || (count==0 && feof(pFile))) ? SUCC : FAIL;
}

///�޸������ļ��еļ�¼
/***
   * path : �����ļ�·�� \n
   * index : ���޸ĵļ�¼�������� \n
   * RecData : ��д������� \n
   * ���� : SUCC/FAIL
   */
int DataFile_ModRec(const char *path,unsigned int index,const void *RecData)
{
	int ret, count;
	unsigned int RecNum, RecLength;
	FILE *pFile = NULL;

	//�������
	if(path==NULL || RecData==NULL){
		return FAIL;
	}

	//���ļ�
	pFile = fopen(path, "rb+");
	if(pFile==NULL){
		return FAIL;
	}

	//��ȡ��¼����
	count = fread(&RecNum, 4, 1, pFile);
	if(count!=1 && !(count==0 && feof(pFile))){
		fclose(pFile);
		return FAIL;
	}

	//��ȡ��¼����
	count = fread(&RecLength, 4, 1, pFile);
	if(count!=1 && !(count==0 && feof(pFile))){
		fclose(pFile);
		return FAIL;
	}

	//�������
	if(index>=RecNum){
		fclose(pFile);
		return FAIL;
	}

	//�ƶ���¼ָ��
	ret = fseek(pFile, 8+index*RecLength, SEEK_SET);
	if(ret!=0){
		fclose(pFile);
		return FAIL;
	}
	//д������
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

//���������ļ�
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
