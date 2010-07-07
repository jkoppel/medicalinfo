#ifndef _GLOBALVARS_H_
#define _GLOBALVARS_H_

#define CONFIG_DIR_FILE		"directories.txt"
#include "XHtmlTree\\XHtmlTree.h"

#define MAX_DATE_LENGTH		32
#define MAX_SPEED_NUM		15
#define MAX_FORCE_NUM		1600

#include <Afxcoll.h>

///�����ļ���¼�ṹ
struct TestRecord
{
	BOOL bDataProcessed;					// =TRUE : ���������Ѿ��������FALSE��û�д����
	BOOL bDataValid;						// =TRUE : ����Чʾ��ͼ����,FALSE: ������Ч����

	char sWorkDay[MAX_DATE_LENGTH+1];		// ���Թ�����
	char sTestDate[MAX_DATE_LENGTH+1];		// ��������
	char sScheduledDate[MAX_DATE_LENGTH+1];	// �ƻ�����

	double  fTestTemp;						// ��⵽�������¶ȡ�
	double  fSideForce;						// ��⵽�ĵĲ�����N

	BOOL bNormalSpeed;						// �Ƿ���������ٶȲ������

	DWORD  dwResultStatus;					// ���Խ���

	int    iNumOfSpeed;						// �������Ѳ������ٶȵĸ��������MAX_SPEED_NUM

	int    iSpdIndex[MAX_SPEED_NUM];		// ���ٶ��ڲ��Թ淶�е��ٶ����
	double  fSetSpeed[MAX_SPEED_NUM];		// �趨�ٶ����飬��λ: m/s
	double  fRealSpeed[MAX_SPEED_NUM];		// �ٶ����飬��λ: m/s
	double  fSetOffset[MAX_SPEED_NUM];		// �趨�г����飬��λ: m
	double  fRealOffset[MAX_SPEED_NUM];		// ������г����飬��λ: m
	double  fPym[MAX_SPEED_NUM];			// ���ѹ��������λ��N
	double  fPfm[MAX_SPEED_NUM];			// �������������λ��N
	int		iNumOfForce[MAX_SPEED_NUM];		// ÿ���ٶ��²��������ĸ��������MAX_SPEED_NUM
	double  fDataFreq[MAX_SPEED_NUM];		// ���������Ƶ�ʣ���λ: Hz
	double  fForce[MAX_SPEED_NUM][MAX_FORCE_NUM]; // ���ٶ������Ӧ��ÿ���ٶ��²������������飬��λ: N
	double  fDisplacement[MAX_SPEED_NUM][MAX_FORCE_NUM];// ���ٶ������Ӧ��ÿ���ٶ��²�������λ�����飬��λ: m
};

struct TreeItemData
{
	int iIndex;
	struct TestRecord *pRec;
};

struct TestRecordFileNode
{
	char sFile[128];
	BOOL bSelected;
	struct TestRecord rec;
	struct TreeItemData item_data[MAX_SPEED_NUM];
};

struct TestRecordDirNode{
	char sDir[256];
	int iNum;
	struct TestRecordFileNode *pFileNode;
};

extern int g_iDirNodeNum;
extern struct TestRecordDirNode *g_pDirNode;

extern CStringArray g_saDirectories;

extern CXHtmlTree *g_pTree;


#endif