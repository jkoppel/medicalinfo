#ifndef _GLOBALFUNCS_H_
#define _GLOBALFUNCS_H_

//
extern void CString2Char(CString, char *);
extern void Char2CString(char *, CString);

//��ʾ��Ϣ
extern void ShowMsg(char msg[]);
extern void ShowMsg(CString msg);
//CRCУ��
extern void CRC_16(uchar *input,uchar dlen, uchar *crc);

#endif

