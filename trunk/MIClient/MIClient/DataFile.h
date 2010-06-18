#ifndef _DATAFILE_H_
#define _DATAFILE_H_

//���������ļ�
extern int DataFile_Create(const char *path, unsigned int RecLength);
//ɾ�������ļ�
extern int DataFile_Delete(const char *path);
//�������ļ�׷��һ����¼
extern int DataFile_AppendRec(const char *path,
							  const void *RecData);
//ɾ�������ļ���һ����¼
extern int DataFile_DeleteRec(const char *path,unsigned int index);
//��ȡ��¼����
extern int DataFile_GetRecNum(const char *path, unsigned int &RecNum);
//�������ļ���ȡһ����¼
extern int DataFile_ReadRec(const char *path,
							unsigned int index,
							void *RecData);
//�޸������ļ���һ����¼
extern int DataFile_ModRec(const char *path,
						   unsigned int index,
						   const void *RecData);

//���������ļ�
extern void TestDataFile();

#endif

