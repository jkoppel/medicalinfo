#ifndef _CFGFILE_H_
#define _CFGFILE_H_

#include <stdio.h>
#include <stdlib.h>
#include <afxwin.h>

#define MAX_FILENAME_LEN	256
#define MAX_OPTION_LEN		256

///class CfgFile, read and write the config file, like .ini file
class CfgFile
{
public:
	CfgFile();
	~CfgFile();

public:
	BOOL OpenFile(const char *szFileName);
	void CloseFile();
	BOOL GetVarInt(const char *szSection, const char *szVarName, int &iReturnValue);
	BOOL GetVarStr(const char *szSection, const char *szVarName, char *szReturnValue, char *szLine=NULL);
	BOOL SetVarInt(const char *szSection, const char *szVarName, const int iValueToSet);
	BOOL SetVarStr(const char *szSection, const char *szVarName, const char *szValueToSet);
	BOOL GetSectionOptions(const char *szSection, char (*szOptions)[MAX_OPTION_LEN], int iMaxOptionNum, int &iNumOptionRead);
	
private:
	int SearchSection(const char *szSection);

	int m_bFileOpened;
	FILE *m_pFile;
};

#endif

