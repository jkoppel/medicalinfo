#ifndef _GLOBALFUNCS_H_
#define _GLOBALFUNCS_H_

#include <afxwin.h>

extern void TCHAR2char(char *szDst, const TCHAR *tcSrc, int iLength);
extern void char2TCHAR(TCHAR *tcDst, const char *szSrc, int iLength);
extern void CString2char(char *szDst, CString strSrc);

//œ‘ æœ˚œ¢
extern void ShowMsg(const char *msg);
extern BOOL CheckStrDateFormat(const char *StrDateTime, int &year, int &month, int &day);
extern BOOL CheckStrTimeFormat(const char *StrTime, int &hour, int &minute);
extern BOOL GetTimeFromStr(const char *sTime, SYSTEMTIME &st);

extern BOOL MatchString(const char* lpszSour, const char* lpszMatch, BOOL bMatchCase=TRUE);

extern void TestMime();


#endif

