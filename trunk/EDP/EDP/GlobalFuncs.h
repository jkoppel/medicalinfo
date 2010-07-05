#pragma once

#define MAX_DATE_LENGTH		32
#define MAX_SPEED_NUM		15
#define MAX_FORCE_NUM		1600

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

struct TestRecordNode{
	char dir[256];
	char file[128];
	struct TestRecord test_rec;
};

extern int g_iRecNum;
extern struct TestRecordNode *g_pRec;

extern BOOL LoadFile(const char *file, struct TestRecord &rec);
extern BOOL SaveFile(const char *file, struct TestRecord rec);
//��ʾ��Ϣ
extern void ShowMsg(const char *msg);
BOOL CheckStrDateFormat(const char *StrDateTime, int &year, int &month, int &day);
BOOL CheckStrTimeFormat(const char *StrTime, int &hour, int &minute);

