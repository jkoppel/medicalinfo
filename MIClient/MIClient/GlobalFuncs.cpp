#include "stdafx.h"
#include "GlobalFuncs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern void HideProgressInfo();
extern int MyMessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption, UINT nType = MB_OK);

void CString2Char(CString source, char *dest)
{
	LPTSTR lpsz = new TCHAR[source.GetLength()+1];
	_tcscpy(lpsz, source);
	memcpy(dest, lpsz, source.GetLength()+1);
	delete []lpsz;
}

void Char2CString(char *, CString)
{
}

void ShowMsg(char msg[])
{
	HideProgressInfo();
	MyMessageBox(CString(msg), CString("ÏûÏ¢"), MB_ICONINFORMATION);
}