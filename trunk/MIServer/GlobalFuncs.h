#ifndef _GLOBALFUNCS_H_
#define _GLOBALFUNCS_H_

extern void GetAppDataDir(char *buf);
extern void CreateAppDataDir();
extern BOOL CreateFolder(CString strPath);

//��ӡ��Ϣ
extern void Printf(const char *format, ...);
//��ӡ����������
extern void PrintBuffer(const char *hint, const unsigned char *buffer,int len);

#endif

