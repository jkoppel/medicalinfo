#include "CfgFile.h"
#include "MIBase.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

///construction, init member vars
CfgFile::CfgFile()
{
	m_pFile = NULL;
	m_bFileOpened = FALSE;
}

CfgFile::~CfgFile()
{
	//remeber to close the file
	CloseFile();
}

///open config file
/**
  * szFileName : the config file name \n
  * return : TRUE/FALSE
  */
BOOL CfgFile::OpenFile(const char *szFileName)
{
	if(szFileName==NULL){
		return FALSE;
	}
	if(m_bFileOpened){//remember to close opened file first
		CloseFile();
	}

	m_pFile = fopen(szFileName, "rb+");
	if(m_pFile==NULL){
		return FALSE;
	}
	
	m_bFileOpened = TRUE;
	return TRUE;
}

///close config file
void CfgFile::CloseFile()
{
	if(m_bFileOpened || m_pFile!=NULL){
		fclose(m_pFile);
		//remember to reset status
		m_pFile = NULL;
		m_bFileOpened = FALSE;
	}
}

///get a int type value of a section from the config file
/**
  * szSection : the section name, e.g. "Settings" \n
  * szVarName : the config value name, e.g. "Contrast" \n
  * iReturnValue : return the value of the config \n
  * return : TRUE/FALSE
  */
BOOL CfgFile::GetVarInt(const char *szSection, const char *szVarName, int &iReturnValue)
{
	int ret;
	unsigned int i;
	char buf[1024];
	
	//check the config exist first
	ret = GetVarStr(szSection, szVarName, buf);
	if(ret!=TRUE){
		return ret;
	}

	//check if the return string is all digits or only started by '-'
	if(!(buf[0]=='-' || isdigit(buf[0]))){
		return FALSE;
	}
	for(i=1;i<strlen(buf);i++){
		if(!isdigit(buf[i])){
			return FALSE;
		}
	}

	iReturnValue = atoi(buf);

	return TRUE;
}

///get a string type value of a section from the config file
/**
  * szSection : the section name, e.g. "Settings" \n
  * szVarName : the config value name, e.g. "Contrast" \n
  * szReturnValue : return the value of the config \n
  * szLine : return the whole line of the config also, for convience, default NULL \n
  * return : TRUE/FALSE
  */
BOOL CfgFile::GetVarStr(const char *szSection, const char *szVarName, char *szReturnValue, char *szLine)
{
	int ret;
	int i, j;
	char buf_line[1025];

	//search section, now the file pointer is at the end of this line, point to '\r' char or EOF
	ret = SearchSection(szSection);
	if(ret!=TRUE){
		return ret;
	}

	while(1){
		if(feof(m_pFile)){
			return FALSE;
		}
		//read a line
		fgets(buf_line, 1024, m_pFile);
		if(szLine!=NULL){
			sprintf(szLine, "%s", buf_line);
		}
		//remove the possible '\r' character at the end!
		if(buf_line[strlen(buf_line)-1]=='\n'){
			buf_line[strlen(buf_line)-1] = 0;
			if(buf_line[strlen(buf_line)-1]=='\r'){
				buf_line[strlen(buf_line)-1] = 0;
			}
		}
		if(strlen(buf_line)==0){
			continue;
		}
		//skip the first spaces
		i = 0;
		while(buf_line[i]==' '){
			i++;
		};
		if(buf_line[0]=='['){//enter the next section
			return FALSE;
		}
		if(strncmp(buf_line+i, szVarName, strlen(szVarName))!=0){//not the config name we want to find
			continue;
		}
		//skip the next spaces
		i += (int)strlen(szVarName);
		while(buf_line[i]==' '){
			i++;
		};
		if(buf_line[i]!='='){
			continue;
		}
		//skip the spaces after '='
		i++;
		while(buf_line[i]==' '){
			i++;
		};
		j = i;
		while(buf_line[i]!=0 && !(buf_line[i]=='/' && buf_line[i+1]=='/')){//the comments
			i++;
		}
		if(i>j){
			memcpy(szReturnValue, buf_line + j, i-j);
			szReturnValue[i-j] = 0;
			break;
		}
	}
	
	//skip the last spaces
	i = (int)strlen(szReturnValue) - 1;
	while(i>=0 && szReturnValue[i]==' '){
		szReturnValue[i--] = 0;
	}
	return TRUE;
}

BOOL CfgFile::GetSectionOptions(const char *szSection, char (*szOptions)[MAX_OPTION_LEN], int iMaxOptionNum, int &iNumOptionRead)
{
	int ret;
	int start, end;
	char buf_line[1025];

	//search section, now the file pointer is at the end of this line, point to '\r' char or EOF
	ret = SearchSection(szSection);
	if(ret!=TRUE){
		return ret;
	}

	iNumOptionRead = 0;
	while(1){
		if(feof(m_pFile)){
			break;
		}
		//read a line
		fgets(buf_line, 1024, m_pFile);
		//remove the possible '\r' character at the end!
		if(buf_line[strlen(buf_line)-1]=='\n'){
			buf_line[strlen(buf_line)-1] = 0;
			if(buf_line[strlen(buf_line)-1]=='\r'){
				buf_line[strlen(buf_line)-1] = 0;
			}
		}
		if(strlen(buf_line)==0){
			continue;
		}
		//skip the first spaces
		start = 0;
		while(buf_line[start]==' ' && start<(int)strlen(buf_line)){
			buf_line[start] = 0;
			start++;
		};
		//skip the last spaces
		end = (int)strlen(buf_line)-1;
		while(buf_line[end]==' ' && end>=0){
			buf_line[end] = 0;
			end--;
		}
		if(strlen(buf_line+start)==0){
			continue;
		}
		if(strlen(buf_line+start)>=2 && buf_line[start]=='/' && buf_line[start+1]=='/'){//Comment
			continue;
		}
		if(buf_line[start]=='['){//enter the next section
			break;
		}
		//read the true string
		memset(szOptions[iNumOptionRead], 0, MAX_OPTION_LEN);
		memcpy(szOptions[iNumOptionRead], buf_line+start, strlen(buf_line+start));
		iNumOptionRead ++;
		if(iNumOptionRead>=iMaxOptionNum){
			break;
		}
	}
	
	return TRUE;
}

///set the int type config value of a section to the config file
/**
  * szSection : the section name \n
  * szVarName : the config value name \n
  * iValueToSet : the int value to be set \n
  * return : TRUE/FALSE
  */
BOOL CfgFile::SetVarInt(const char *szSection, const char *szVarName, const int iValueToSet)
{
	char szValueToSet[256];

	sprintf(szValueToSet, "%d", iValueToSet);

	return SetVarStr(szSection, szVarName, szValueToSet);
}

///set the string type config value of a section to the config file
/**
  * szSection : the section name \n
  * szVarName : the config value name \n
  * szValueToSet : the string value to be set \n
  * return : TRUE/FALSE
  */
BOOL CfgFile::SetVarStr(const char *szSection, const char *szVarName, const char *szValueToSet)
{
	int ret;
	char szVal[256], szLine_org[500], szLine_Curr[500];
	
	//read the config value first, and read the whole line also.
	ret = GetVarStr(szSection, szVarName, szVal, szLine_org);
	if(ret!=TRUE){
		return ret;
	}
	if(strcmp(szValueToSet, szVal)==0){
		return TRUE;
	}

	memset(szLine_Curr, 0, sizeof(szLine_Curr));
	if(strlen(szLine_org) >= strlen(szVarName) + 1 + strlen(szValueToSet) + 1){
		//just add some spaces at the end
		sprintf(szLine_Curr, "%s%s%s", szVarName, "=", szValueToSet);
		memset(szLine_Curr + strlen(szLine_Curr), ' ', strlen(szLine_org) - strlen(szLine_Curr) - 1);
		szLine_Curr[strlen(szLine_Curr)] = '\n';
		fseek(m_pFile, 0 - (long)strlen(szLine_org), SEEK_CUR);
		fwrite(szLine_Curr, strlen(szLine_Curr), 1, m_pFile);
	}
	else{
		sprintf(szLine_Curr, "%s%s%s%s", szVarName, "=", szValueToSet, "\n");
		//move text to get the enough space for writting current line
		int start, end;
		char *buff = NULL;
		start = ftell(m_pFile);
		fseek(m_pFile, 0, SEEK_END);
		end = ftell(m_pFile);
		buff = new char[end-start];
		fseek(m_pFile, start, SEEK_SET);
		fread(buff, end-start, 1, m_pFile);
		fseek(m_pFile, start + (long)strlen(szLine_Curr)-(long)strlen(szLine_org), SEEK_SET);
		fwrite(buff, end-start, 1, m_pFile);
		fseek(m_pFile, start - (long)strlen(szLine_org), SEEK_SET);
		fprintf(m_pFile, "%s", szLine_Curr);

		delete [] buff;
	}
	
	return TRUE;
}

///search a section of the config file, after that, the file pointer is located at the end of the section line
/**
  * szSection : the section name \n
  * return : TRUE/NO
  */
BOOL CfgFile::SearchSection(const char *szSection)
{
	if(!m_bFileOpened){
		return FALSE;
	}

	int i;
	char buf_line[1025];

	fseek(m_pFile, 0, SEEK_SET);
	while(1){
		if(feof(m_pFile)){
			return FALSE;
		}
		fgets(buf_line, 1024, m_pFile);
		//remove the possible '\r' character at the end!
		if(buf_line[strlen(buf_line)-1]=='\n'){
			
			buf_line[strlen(buf_line)-1] = 0;
			if(buf_line[strlen(buf_line)-1]=='\r'){
				buf_line[strlen(buf_line)-1] = 0;
			}
		}
		if(strlen(buf_line)==0){
			continue;
		}
		//skip the first spaces
		i = 0;
		while(buf_line[i]==' '){
			i++;
		};
		if(buf_line[i++]!='['){
			continue;
		}
		//skip spaces after '['
		while(buf_line[i]==' '){
			i++;
		};
		if(strncmp(buf_line+i, szSection, strlen(szSection))!=0){
			continue;
		}
		//search the ']'
		i += (int)strlen(szSection);
		while(buf_line[i]==' '){
			i++;
		};
		if(buf_line[i]!=']'){
			continue;
		}
		//check if only the "//" comments follow after it
		i++;
		while(buf_line[i]==' '){
			i++;
		};
		if(buf_line[i]!=0){
			if(buf_line[i++]!='/'){
				return FALSE;
			}
			if(buf_line[i]!='/'){
				return FALSE;
			}
		}
		break;
	}

	return TRUE;
}

