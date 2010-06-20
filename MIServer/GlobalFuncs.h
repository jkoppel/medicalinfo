#ifndef _GLOBALFUNCS_H_
#define _GLOBALFUNCS_H_

extern void GetAppDataDir(char *buf);
extern void CreateAppDataDir();
extern BOOL CreateFolder(CString strPath);

//打印消息
extern void Printf(const char *format, ...);
//打印缓冲区数据
extern void PrintBuffer(const char *hint, const unsigned char *buffer,int len);

#endif

