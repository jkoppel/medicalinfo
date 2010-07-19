#pragma once

#define MAX_DATE_LENGTH		32
#define MAX_STD_SPEED_NUM	50      // ��Ƶ�����ݵĸ���
#define MAX_SPEED_NUM		15
#define MAX_FORCE_NUM		1600

#define MAX_REFDOT_NUM		7
#define SKIP_FIRSTDOT_NUM	125


#define M_PI	3.1415926
#pragma pack(8)

struct CCForceSensor          // ����������Ϣ 
{ 
	// ���������� 
	char    sSensorNo[33];             // ��������� 
	char sSensorModel[33];              // �������ͺ� 
	char sSensorManufacrturer[100];          // �������� 
	char sSensorStartUseDate[33];          // ��������ʼʹ������ 
	double  fSensorRange;             // ���������̣�N 
	BOOL bSensorPositive;              // TRUE�������ԣ�FALSE�������� 
	int    iSensorFullRangeVoltage;        // �����������������ѹ��V 

	int    iSensorCalibInterval;          // �������궨������� 
	char sSensorCalibDate[33];            // ���´������궨���� 
	char sSensorNextCalibDate[33];          // ��һ�δ������궨���� 

	double  fSensorSensitivity;          // ���´����������ȱ궨���(N/V) 

	double  fSensorZeroV;              // �����������غ�ʱ�������ѹ��V�� 

	BOOL bNeedCalibSensitivity; 
	BOOL bNeedCalibZero; 

	char sReserved1[40];              // ���� 
}; 

struct CCTempSensor          // �¶ȴ�������Ϣ 
{ 
	// ���������� 
	char    sSensorNo[33];             // ��������� 
	char sSensorModel[33];              // �������ͺ� 
	char sSensorManufacrturer[100];          // �������� 
	char sSensorStartUseDate[33];          // ��������ʼʹ������ 
	double  fSensorRange;             // ���������̣�N 
	int    iSensorFullRangeVoltage;        // �����������������ѹ��V 

	int    iSensorCalibInterval;          // �������궨������� 
	char sSensorCalibDate[33];            // ���´������궨���� 
	char sSensorNextCalibDate[33];          // ��һ�δ������궨���� 

	double  fSensorSensitivity;          // ���´����������ȱ궨���(N/V) 
	double  fSensorZeroV;              // �����������غ�ʱ�������ѹ��V�� 

	BOOL bNeedCalibSensitivity; 

	BOOL bNeedCalibZero; 

	char sReserved1[40];              // ���� 
}; 


struct CCDisplacementSensor       // λ�ƴ�������Ϣ 
{ 
	// ���������� 
	char   sSensorNo[33];              // ��������� 
	char sSensorModel[33];              // �������ͺ� 
	char sSensorManufacrturer[100];          // �������� 
	char sSensorStartUseDate[33];          // ��������ʼʹ������ 

	double  fDisplacementRange0;          // λ�ƴ������ı������(m) 
	double  fDisplacementRange;          // λ�ƴ�������ʵ������(m) 
	double  fDisplacementResistance;        // λ�ƴ�������������ֵ(��) 

	int    iSensorCalibInterval;          // �������궨������� 
	char sSensorCalibDate[33];            // ���´������궨���� 
	char sSensorNextCalibDate[33];          // ��һ�δ������궨���� 

	double  fSensorSensitivity;          // λ�ƴ�������������(m/v) 

	BOOL bNeedCalibSensitivity; 


	char sReserved1[40];              // ���� 
}; 

struct CCAdventechAIOCard        // ģ������������Ϣ 
{ 
	ULONG  lDevNum;               // ���������豸�� 
	char    sCardManufacture[100];          // �������������� 
	char    sCardName[100];             // ���������� 
	int    iAIChanNums;              // ģ������ͨ������ 
	int    iAIInputRange;            // AIͨ���������̵�ѹ��V 
	int    iGainListNum;             // ����������� 
	USHORT  usGainCodeList[20];          // ����������� 
	double  fGainList[20];            // ������ֵ���� 
	int    iFIFOSize;              // FIFO��С 

	double  fMaxADFreq;              // ��߲���Ƶ�ʣ�Hz 

	char sReserved1[24];              // ���� 
}; 

struct CCAdventechDIOCard        // ��������������Ϣ 
{ 
	ULONG  lDevNum;               // ���������豸�� 
	char    sCardManufacture[100];          // �������������� 
	char    sCardName[100];             // ���������� 
	BOOL bShareDIOChan;              // DI/DO�Ƿ���ͨ���� 

	int    iDIChanNums;              // DIͨ������ 
	int    iDOChanNums;              // DOͨ������ 
	char sReserved1[24];              // ���� 
}; 

struct CCCanCard            // CANͨѶ����Ϣ 
{ 
	char    sCardManufacture[100];          // �������������� 
	char    sCardName[100];             // ���������� 

	ULONG  lDevNum;               // ���������豸�� 

	int    iCanChanNums;             // Canͨ������ 

	int    iCanChan;  

	DWORD  dwDevIndex;              // ʹ�õ����ĸ�Canͨ�� 
	DWORD  dwCanIndex;              // �豸������ 
	int    iBpsNum; 
	int    iBps0[20]; 
	UCHAR  ucTimer00[20]; 
	UCHAR  ucTimer01[20]; 

	char sReserved1[24];              // ���� 
}; 


struct CCRs485Card           // 485ͨѶ����Ϣ 
{ 
	char    sCardManufacture[100];          // �������������� 
	char    sCardName[100];             // ���������� 

	UINT iPortNo;                 // 485ʹ�õĴ��ںţ�=COM1��=COM2���Դ����� 

}; 

struct CCTransducerPara         // ��Ƶ����Ϣ 
{ 
	// ��Ƶ������ 
	char sTransduccerModel[33];           // ��Ƶ���ͺ� 
	char sPortSettingStr[20];            // ��Ƶ���õĴ��������ַ������� ,e,8,2  

	// ���٣���Ƶ����ϵ�� 
	int    iMachineCalibDataNumHi;         // ����ת�ٱ궨����е����ݸ��� 
	double  fStdSpeedHi[MAX_STD_SPEED_NUM];      // ��������ת�٣���λrpm 
	char sReserved12[10];              // ���� 
	UINT uStdTransducerNoHi[MAX_STD_SPEED_NUM];    // ����ת�ٶ�Ӧ�ı�Ƶ������ 
	char sReserved13[10];              // ���� 

	// ���٣���Ƶ����ϵ�� 
	int    iMachineCalibDataNumLo;         // ����ת�ٱ궨����е����ݸ��� 
	double  fStdSpeedLo[MAX_STD_SPEED_NUM];      // ��������ת�٣���λrpm 
	char sReserved32[10];              // ���� 

	UINT uStdTransducerNoLo[MAX_STD_SPEED_NUM];    // ����ת�ٶ�Ӧ�ı�Ƶ������ 
	char sReserved33[10];              // ���� 

	char sReserved0[50];              // ���� 
}; 

struct CCExciterPara          // ��������Ϣ 
{ 
	char sExciterModel[100];            // �綯���ͺ� 

	UINT uCanSpeed;                // �綯��CAN�ӿ��ٶ�  KBPS 
	UINT uCanMoudleID;               // �綯���ŷ���������CAN ID 

	double   fExciterSpeedScaleFactor;        // �綯���ٶȱ���ϵ�� �������͸��綯�׵��ٶȣ�Ҫ����ٶȡ�fExciterSpeedScaleFactor 
	double   fExciterStrokeScaleFactor;        // �綯���г̱���ϵ�� �������͸��綯�׵��г̣�Ҫ����г̡�fExciterStrokeScaleFactor 

	double  fMaxSpeed, fMinSpeed;          // �綯�׵������С�ٶ�m/s 
	double  fMaxStroke;              // �綯�׵�����г�m 
	double  fMinStroke;              // �綯�׵���С�г�, 

	double  fGoHomeSpeed          ;    // m/s 
	double  fMoveSpeed;              // m/s 

	char sReserved0[100];              // ���� 

}; 


struct CCBeamPara            // ������Ϣ 
{ 
	UINT uCycleSteps;                // �������ÿȦ���� 
	double  fScrewPitch;              // ����˿���ݾ࣬m 

	double  fMoveSpeed;              // �����ƶ��ٶ�, m/s 


	DWORD  dwBeamStatus;             // ����״̬ 
	// 0: ����ʾ��(��ֹ���������� 
	// 1: ������ 
	// 2: �½��� 
	// 3: ������ 
	// 4: ������ 
	// 5: �ѷ��� 
	// 6: ��Ҫ�궨 


	double  fBeamPos0;              // ������ԭʼλ�ã��Ӵ�����λ��λ����ʱ��λ�ã�,m 
	double  fBeamPos;               // �����ĸ߶ȣ����λ��=fBeamPos0��,m 

	double  fLockCurrent;              // �����ﵽ����ʱ����������ͨ����ѹ��ֵV 
	double  fUnLockCurrent;             // �����ﵽ����ʱ����������ͨ����ѹ��ֵV 

	double  fCurrent;                // ��������ʱ����������ͨ����ѹV 

}; 

struct CCMachineInfo          // ����豸��Ϣ 
{ 
	char sMachineNo[33];              // �豸��� 
	char sMachineModel[33];             // �ͺ� 
	char sMachineManufacrturer[100];         // �������� 
	char sMachineStartUseDate[33];          // ��ʼʹ������ 
	// ���ٲ��� 
	int    iMachineCalibInterval;         // �����궨��� 
	char sMachineCalibDate[33];           // ����ת�ٱ궨���� 
	char sMachineNextCalibDate[33];          // ��һ��ת�ٱ궨���� 

	// �������������� 
	CCForceSensor tMainForceSensor; 
	BOOL bAutoMainForceZero;            // TRUE: �Զ��������������㣬FALSE:�ֶ��������������� 

	// ���������������� 
	CCForceSensor tSideForceSensor; 
	BOOL bManualSideForce;              // �޴��������ֹ��������� 

	// λ�ƴ��������� 
	CCDisplacementSensor tDisplacementSensor; 
	//  double  fDisplacementRange;          // λ�ƴ�����������(m) 
	//  double  fDisplacementResistance;        // λ�ƴ�������������ֵ(��) 
	// �¶ȴ��������� 
	CCTempSensor tTempSensor; 
	BOOL bManualTemp;                // �޴��������ֹ��������� 

	// ��λͣ������ 
	int    nSpeed;                // ͣ��ǰ����rpm 
	int    nCycles;               // ���ٺ�ȴ�����Ȧ�� 
	int    nMilSec;               // ��ʱ������ 

	// �ɼ�Ӳ������ 
	CCAdventechAIOCard tAIODev;           // ģ�����������豸 
	CCAdventechDIOCard tDIODev;           // �������������豸 
	BOOL       bStandAloneDIOCard;      // ���޶����������������Ƿ�ʹ��AIO��������ͨ���� 

	CCCanCard     tCanDev;           // CAN�豸 
	BOOL       bUseCanCard;          // �Ƿ�ʹ��CAN�豸 

	CCRs485Card      tRs485Dev;        // CAN�豸 
	BOOL       bUseRs485Card;        // �Ƿ�ʹ��CAN�豸 

	// ��Ƶ������ 
	CCTransducerPara  tTransducerPara; 

	// �綯�ײ��� 
	CCExciterPara   tExciterPara; 
	// �������� 
	CCBeamPara      tBeamPara; 

	// ʾ����Ӳ�� 
	double  fRodLen;               // ���˳��ȣ�����λ�ơ��ٶ��ã���λ��m����λ�ƴ���������Դ˲��� 
	double  fHangWeight;              // �Ҿ���������λ��N 

	double  fMaxStroke0;              // ʾ������������г�(�������г�)��m 
	double  fMaxSpeed0;              // ʾ������������ٶȣ�m/s 

	// ʹ������Ϣ 
	char sOperatorNo[33];              // ����Ա��� 
	char sCooperationNameCn[33];           // ������������ 
	char sCooperationNameEn[33];           // ��������Ӣ�� 

	DWORD  dwMachineCapability;          //  
	int    iMode;                // �������ģʽ 
	char sReserved1[100];              // ���� 
}; 

struct CCProductInfo          // ��Ʒ��Ϣ 
{ 
	char sProductNo[33];              // ��Ʒ�ͺ� 
	char sProductOEM[33];              // ��ƷOEM 
	char sProductType[33];              // ��Ʒ��� 
	int    iProductSeqNo;            // ��ǰ��Ʒ����� 

	char sOrderNo[33];               // ��Ʒ����������� 

	double  fAbsorbWeight;            // ��������������λ��N 
	double  fAbsorbRodWeight;            // ������������������λ��N 

	double  fTestTemp;              // ����Ҫ����¶ȡ� 
	double  fSideForce;              // ����Ҫʩ�ӵĲ�����N 

	double  fCenterPos;              // ����ʱ�ļ�������λ��, m 
	double  fStartStopPos;             // ����ǰװ��������ֹͣ��λ��, m 

	BOOL bDifferentOffset;              // �Ƿ�����ٶȵ��������г� 
	double  fOffset;               // ���г�(���г̵�һ��)��bDifferentOffset��FALSEʱ��Ч����λ: m 


	double  fAirRebounceForce0;          // ��������������λ��N 

	double  fOil;                 // ע���� 

	int    iPointsPerCycle;            // ʾ��ͼÿȦ��������������< MAX_FORCE_NUM/3.5 
	int    iDataBandStart;            // ��ʾ��ͼʱ��Ч�������俪ʼ�ĽǶȣ���0��ʼ��ÿ���ٶȵ㹲1260�ȣ�360�ȣ�һ������ 
	int    iDataBandLen;             // ��ʾ��ͼʱ��Ч��������ĽǶȳ��� 

	// �����ٶ����ò���
	BOOL bChkNormalSpeed;              // �Ƿ���г����ٶȼ�� 
	int    iSpeedNum;              // �趨�����ٶȸ��� 

	BOOL bSkipSpeed[MAX_SPEED_NUM];          // �Ƿ񲻼����ٶ� 

	double  fSpeed0[MAX_SPEED_NUM];         // �趨����ٶ����飬����ֵ����λ��m/s 
	double  fRotateSpeed0[MAX_SPEED_NUM];      // �趨ת�����飬ʹ��ʱ����fSpeed[]�������λ: rpm 
	double  fSpeedOffset[MAX_SPEED_NUM];      // ���� (���г̵�һ��)��bDifferentOffset��TRUEʱ��Ч������ӦΪfOffset����λ: m 

	double  fMaxRecruitForce1[MAX_SPEED_NUM];    // ���ԭ���������飬����ֵ����λ��N 
	double  fMaxRecruitForce2[MAX_SPEED_NUM];    // ���ԭ���������飬����ֵ����λ��N 
	double  fMaxCompresForce1[MAX_SPEED_NUM];    // ���ѹ�����������飬����ֵ����λ��N 
	double  fMaxCompresForce2[MAX_SPEED_NUM];    // ���ѹ�����������飬����ֵ����λ��N 

	double  fTime2[MAX_SPEED_NUM];         // ����ʱ�����飬����ֵ����λ��s���ڵ綯��ʾ�����к��Դ��� 

	BOOL bChkFriction;               // �Ƿ���Ħ���� 

	double  fFrictionSpeed0;            // �趨����ٶ����飬����ֵ����λ��m/s 
	double  fFrictionRotateSpeed0;         // �趨ת�����飬����fSpeed[]�������λ: rpm 
	double  fFrictionSpeedOffset;          // ���� (���г̵�һ��)��bDifferentOffset��TRUEʱ��Ч������ӦΪfOffset����λ: m 

	double  fMaxFrictionRecruitForce1;        // ���ԭ���������飬����ֵ����λ��N 
	double  fMaxFrictionRecruitForce2;        // ���ԭ���������飬����ֵ����λ��N 
	double  fMaxFrictionCompresForce1;        // ���ѹ�����������飬����ֵ����λ��N 
	double  fMaxFrictionCompresForce2;        // ���ѹ�����������飬����ֵ����λ��N 

	double  fFrictionTime2;            // ����ʱ�����飬����ֵ����λ��s���ڵ綯��ʾ�����к��Դ��� 

	int    iChkAirRebounce;            // �Ƿ�����ѹ�� 
	// 0: ���� 
	// 1: ����Ħ�������������ѹ�� 
	// 2: ���ٴ���͵��ƶ�����ߵ� 
	// 3: ���ٴ���͵��ƶ�����ߵ㣬�ٻص���͵� 
	// 4: ���ٴ���͵��ƶ�����͵� 
	// 5: ���ٴ���͵��ƶ�����͵㣬�ٻص���ߵ� 

	double  fChkAirReboundSpeed;          // �����ѹ��ʱ���ƶ��ٶ�    m/s 
	double  fChkAirReboundStroke;          // �����ѹ��ʱ���ƶ��г�    m 

	int    iPreLoadTimes;            // ʾ�����ǰ��Ԥ�������������� 
	double  fPreLoadSpeed;            // ʾ�����ǰ��Ԥ���ٶȣ�������  m/s 

	char sTestComment[80];              // ����˵�� 
	char sCustomerName[40];             // �ͻ����� 
	char sReserved100[240];             // ���� 
}; 

///�����ļ���¼�ṹ
struct CCTestRecord
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

	BOOL bFrictionSpeed;                  // �Ƿ����Ħ����������� 
	double  fFrictionSetSpeed;               // �趨�ٶȣ���λ: m/s 
	double  fFrictionRealSpeed;              // ʵ���ٶȣ���λ: m/s 
	double  fFrictionSetOffset;              // �趨�г̣���λ: m 
	double  fFrictionRealOffset;              // ������г̣���λ: m 
	double  fFrictionPym;                 // ���ѹ��������λ��N 
	double  fFrictionPfm;                 // �������������λ��N 
	int    iFrictionNumOfForce;              // ÿ���ٶ��²��������ĸ��������MAX_SPEED_NUM 
	double  fFrictionDataFreq;               // ���������Ƶ�ʣ���λ: Hz 
	double  fFrictionForce[MAX_FORCE_NUM];          // ���ٶ������Ӧ��ÿ���ٶ��²������������飬��λ: N 
	double  fFrictionDisplacement[MAX_FORCE_NUM];       // ���ٶ������Ӧ��ÿ���ٶ��²�������λ�����飬��λ: m 

	int    iMode;                    // �������ģʽ 

	BOOL bUseDisplacementSensor;               // �Ƿ�ʹ��λ�ƴ����� 

	double  fAirRebounceForce;               // ��������������λ��N 

	BOOL bHasAbsorb; 
	int    iChkAirRebounce;             
	char sResultComment[80];                // ������˵�� 

	char sReserved100[316];                 // ���� 
};

struct TestRecordFileNode;
struct TreeItemData
{
	int iIndex;
	BOOL bSelected;
	struct TestRecordFileNode *pNode;
};

struct AdditionInfo
{
	double fDisplacementLength[MAX_SPEED_NUM];
	double fForceOfFilter[MAX_SPEED_NUM][MAX_FORCE_NUM];
	int iDataBandStart[MAX_SPEED_NUM];
	int iDataBandLen[MAX_SPEED_NUM];

	double fFrictionDisplacementLength[MAX_SPEED_NUM];
	double fFrictionForceOfFilter[MAX_SPEED_NUM][MAX_FORCE_NUM];
	int iFrictionDataBandStart[MAX_SPEED_NUM];
	int iFrictionDataBandLen[MAX_SPEED_NUM];
};

//
struct CCTestRecordHeader
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
};

/*
typedef struct TestRecordFileNode
{
	struct CCMachineInfo machine_info;
	struct CCProductInfo product_info;
	struct CCTestRecord test_record;
	struct AdditionInfo addition_info;
	BOOL bDataProcessed;
	struct TreeItemData tree_item_data[MAX_SPEED_NUM];
	struct TestRecordFileNode *pNext;
	struct TestRecordFileNode *pPrev;
} FileNode, *FileNodePtr, *FileNodeList;
*/
typedef struct TestRecordFileNode
{
	BOOL bProcessed;
	TCHAR sProductNo[33];
	TCHAR sFileName[256];
	TCHAR sTestDate[33];

	struct CCTestRecordHeader test_record_header;
	struct CCMachineInfo *pMachineInfo;
	struct CCProductInfo *pProductInfo;
	struct CCTestRecord *pTestRecord;
	struct AdditionInfo *pAdditionInfo;
	struct TreeItemData tree_item_data[MAX_SPEED_NUM];
	struct TestRecordFileNode *pNext;
	struct TestRecordFileNode *pPrev;
} FileNode, *FileNodePtr, *FileNodeList;


typedef struct ProductNode
{
	TCHAR sProductNo[33];
	FileNodePtr pFileListHead;
	FileNodePtr pFileListTail;
	struct ProductNode *pNext;
	struct ProductNode *pPrev;
} ProductNode, *ProductNodePtr, *ProductNodeList;

typedef struct ProductTreeRoot
{
	ProductNodeList pProductNodeListHead;
	ProductNodeList pProductNodeListTail;
} ProductTreeRoot, *ProductTreeRootPtr;

class CTestDataTreeMgt
{
public:
	CTestDataTreeMgt(void);
	~CTestDataTreeMgt(void);
	void InitTree(void);
	void ReloadTree(void);
	void FreeTree(void);
	void FreeFileNode(FileNodePtr pFileNode);

	BOOL LoadFile(const char *sFileNmae);
	BOOL LoadAndProcessFile(const char *sFileName, FileNodePtr pFileNode);
	BOOL GetFileHeader(const char *sFileName, FileNodePtr pFileNode);
	//�Զ����ڵ�˫ѭ������Ĳ���
	BOOL InsertFileNode(ProductNodePtr pProductNode, FileNodePtr pFileNodeToInsert);
	BOOL DeleteFileNode(ProductNodePtr pProductNode_Ahead, ProductNodePtr pProductNodet, FileNodePtr pFileNodeToDelete);
	void DeleteFileNode(FileNodePtr pFileNodeToDelete);
	void DeleteFileNode(const char *sFileName);
	BOOL SearchFileNode(const char *sFileName, FileNodePtr &pFileNodeFound);
	//��һ���ڵ�˫ѭ������Ĳ���
	BOOL InsertProductNode(ProductNodePtr pProductNodeToInsert);
	void InsertProductNode(const char *sProductNo, ProductNodePtr &pProductNodeInserted);
	BOOL DeleteProductNode(ProductNodePtr pProductNodeToDelete);
	void DeleteProductNode(const char *sProductNo);
	BOOL SearchProductNode(const char *sProductNo, ProductNodePtr &pProductNodeFound);

	void ResetNodeStatus();

	ProductTreeRootPtr m_pProductTreeRoot;
protected:
	void ProcessData(FileNodePtr pFileNode);
};
