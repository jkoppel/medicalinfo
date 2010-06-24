#ifndef _GLOBALFUNCS_H_
#define _GLOBALFUNCS_H_

//
extern void CString2Char(CString, char *);
extern void Char2CString(char *, CString);

//œ‘ æœ˚œ¢
extern void ShowMsg(char msg[]);
extern void ShowMsg(CString msg);

extern CStringList g_strList;
extern void MakeSeparatorString(CString &destStr);
extern void ParseSeparatorString(CString str);

#endif

