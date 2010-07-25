#pragma once

#include <afxwin.h>

#define CONFIG_DIR_FILE		"directories.txt"

class CDirConfigMgt
{
public:
	CDirConfigMgt(void);
	~CDirConfigMgt(void);

	BOOL LoadDirFromConfigFile();
	BOOL SaveDirToConfigFile();

	BOOL IsFileInConfigDirList(const char *sFile);
	BOOL IsFileInConfigDirList(CString strFile);

	CStringArray m_saDirectories;
};
