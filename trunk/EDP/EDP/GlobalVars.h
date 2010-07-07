#ifndef _GLOBALVARS_H_
#define _GLOBALVARS_H_

#define CONFIG_DIR_FILE		"directories.txt"
#include "XHtmlTree\\XHtmlTree.h"

#define MAX_DATE_LENGTH		32
#define MAX_SPEED_NUM		15
#define MAX_FORCE_NUM		1600

#include <Afxcoll.h>

///测试文件记录结构
struct TestRecord
{
	BOOL bDataProcessed;					// =TRUE : 其中数据已经处理过，FALSE：没有处理过
	BOOL bDataValid;						// =TRUE : 含有效示功图数据,FALSE: 不含有效数据

	char sWorkDay[MAX_DATE_LENGTH+1];		// 测试工作日
	char sTestDate[MAX_DATE_LENGTH+1];		// 测试日期
	char sScheduledDate[MAX_DATE_LENGTH+1];	// 计划日期

	double  fTestTemp;						// 检测到的试验温度℃
	double  fSideForce;						// 检测到的的侧向力N

	BOOL bNormalSpeed;						// 是否包含常规速度测量结果

	DWORD  dwResultStatus;					// 测试结论

	int    iNumOfSpeed;						// 包含的已测量的速度的个数，最多MAX_SPEED_NUM

	int    iSpdIndex[MAX_SPEED_NUM];		// 此速度在测试规范中的速度序号
	double  fSetSpeed[MAX_SPEED_NUM];		// 设定速度数组，单位: m/s
	double  fRealSpeed[MAX_SPEED_NUM];		// 速度数组，单位: m/s
	double  fSetOffset[MAX_SPEED_NUM];		// 设定行程数组，单位: m
	double  fRealOffset[MAX_SPEED_NUM];		// 检测结果行程数组，单位: m
	double  fPym[MAX_SPEED_NUM];			// 最大压缩力，单位：N
	double  fPfm[MAX_SPEED_NUM];			// 最大拉伸力，单位：N
	int		iNumOfForce[MAX_SPEED_NUM];		// 每个速度下测量的力的个数，最多MAX_SPEED_NUM
	double  fDataFreq[MAX_SPEED_NUM];		// 检测结果采样频率，单位: Hz
	double  fForce[MAX_SPEED_NUM][MAX_FORCE_NUM]; // 与速度数组对应的每个速度下测量到的力数组，单位: N
	double  fDisplacement[MAX_SPEED_NUM][MAX_FORCE_NUM];// 与速度数组对应的每个速度下测量到的位移数组，单位: m
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