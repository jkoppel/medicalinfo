#ifndef _GLOBALFUNCS_H_
#define _GLOBALFUNCS_H_

#include <afxwin.h>

extern void TCHAR2char(char *szDst, const TCHAR *tcSrc, int iLength);
extern void char2TCHAR(TCHAR *tcDst, const char *szSrc, int iLength);
extern void CString2char(char *szDst, CString strSrc);

extern BOOL LoadFile(const char *file, struct CCTestRecord &rec);
extern BOOL SaveFile(const char *file, struct CCTestRecord rec);

extern BOOL IsFileInConfigDirList(const char *sFile);
extern BOOL IsFileInConfigDirList(CString strFile);

//��ʾ��Ϣ
extern void ShowMsg(const char *msg);
BOOL CheckStrDateFormat(const char *StrDateTime, int &year, int &month, int &day);
BOOL CheckStrTimeFormat(const char *StrTime, int &hour, int &minute);

extern BOOL LoadDirFromConfigFile();
extern BOOL SaveDirToConfigFile();

extern void TestMime();

extern int filter_new( 
			   double dat[],     // �������� 
			   double fs,      // ����Ƶ�� Hz 
			   double fc,       // �˲�Ƶ�� Hz 
			   int num        // ���ݸ��� 
			   );
extern int fir_dsgn(int Len, double FreqS, double FreqB , double **Coef1);


#endif

