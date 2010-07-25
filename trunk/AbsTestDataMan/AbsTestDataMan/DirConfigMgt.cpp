#include "DirConfigMgt.h"

CDirConfigMgt::CDirConfigMgt(void)
{
	m_saDirectories.RemoveAll();
}

CDirConfigMgt::~CDirConfigMgt(void)
{
}

///�������ļ��ж�ȡĿ¼��Ϣ
BOOL CDirConfigMgt::LoadDirFromConfigFile()
{
	CStdioFile file;
	if(!file.Open(_T(CONFIG_DIR_FILE), CFile::modeNoTruncate | CFile::modeCreate)){
		return FALSE;
	}
	m_saDirectories.RemoveAll();
	TCHAR dir[1024];
	CString str;
	while(file.ReadString(dir, sizeof(dir)/sizeof(TCHAR))){
		if(_tcslen(dir)>0){
			if(dir[_tcslen(dir)-1]==_T('\n')){//ȥ�����з�
				dir[_tcslen(dir)-1]=_T('\0');
			}
			str.Format(_T("%s"), dir);
			str.Trim();
			if(str.GetLength()>0){
				m_saDirectories.Add(str);
			}
		}
	}
	file.Close();
	return TRUE;
}

///��Ŀ¼��Ϣ���浽�ļ���
BOOL CDirConfigMgt::SaveDirToConfigFile()
{
	CStdioFile file;
	if(!file.Open(_T(CONFIG_DIR_FILE), CFile::modeCreate | CFile::modeWrite)){
		return FALSE;
	}
	CString str;
	for(int i=0;i<m_saDirectories.GetCount();i++){
		str = m_saDirectories.GetAt(i);;
		str = str.Trim();
		if(str.GetLength()){
			str += CString("\n");
			file.WriteString(str);
		}
	}
	file.Close();
	return TRUE;
}

///����ļ��Ƿ���Ŀ¼����
BOOL CDirConfigMgt::IsFileInConfigDirList(CString strFile)
{
	int i;
	CString strDir;

	for(i=0;i<m_saDirectories.GetCount();i++){
		strDir = m_saDirectories.GetAt(i);
		if(strFile.Find(strDir)>=0){
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CDirConfigMgt::IsFileInConfigDirList(const char *sFile)
{
	return IsFileInConfigDirList(CString(sFile));
}

