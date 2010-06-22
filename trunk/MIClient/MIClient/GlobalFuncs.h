#ifndef _GLOBALFUNCS_H_
#define _GLOBALFUNCS_H_

//
extern void CString2Char(CString, char *);
extern void Char2CString(char *, CString);

//显示消息
extern void ShowMsg(char msg[]);
extern void ShowMsg(CString msg);
//CRC校验
extern void CRC_16(uchar *input,uchar dlen, uchar *crc);

#endif

