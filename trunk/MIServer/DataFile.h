#ifndef _DATAFILE_H_
#define _DATAFILE_H_

//创建数据文件
extern int DataFile_Create(const char *path, unsigned int RecLength);
//删除数据文件
extern int DataFile_Delete(const char *path);
//向数据文件追加一条记录
extern int DataFile_AppendRec(const char *path,
							  const void *RecData);
//删除数据文件的一条记录
extern int DataFile_DeleteRec(const char *path,unsigned int index);
//读取记录个数
extern int DataFile_GetRecNum(const char *path, unsigned int &RecNum);
//从数据文件读取一条记录
extern int DataFile_ReadRec(const char *path,
							unsigned int index,
							void *RecData);
//修改数据文件的一条记录
extern int DataFile_ModRec(const char *path,
						   unsigned int index,
						   const void *RecData);

//测试数据文件
extern void TestDataFile();

#endif

