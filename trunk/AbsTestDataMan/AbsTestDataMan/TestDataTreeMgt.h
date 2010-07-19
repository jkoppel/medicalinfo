#pragma once

#define MAX_DATE_LENGTH		32
#define MAX_STD_SPEED_NUM	50      // 变频器数据的个数
#define MAX_SPEED_NUM		15
#define MAX_FORCE_NUM		1600

#define MAX_REFDOT_NUM		7
#define SKIP_FIRSTDOT_NUM	125


#define M_PI	3.1415926
#pragma pack(8)

struct CCForceSensor          // 力传感器信息 
{ 
	// 传感器参数 
	char    sSensorNo[33];             // 传感器编号 
	char sSensorModel[33];              // 传感器型号 
	char sSensorManufacrturer[100];          // 生产厂家 
	char sSensorStartUseDate[33];          // 传感器开始使用日期 
	double  fSensorRange;             // 传感器量程，N 
	BOOL bSensorPositive;              // TRUE：正极性，FALSE：负极性 
	int    iSensorFullRangeVoltage;        // 传感器满量程输出电压，V 

	int    iSensorCalibInterval;          // 传感器标定间隔天数 
	char sSensorCalibDate[33];            // 最新传感器标定日期 
	char sSensorNextCalibDate[33];          // 下一次传感器标定日期 

	double  fSensorSensitivity;          // 最新传感器灵敏度标定结果(N/V) 

	double  fSensorZeroV;              // 传感器的零载荷时的输出电压（V） 

	BOOL bNeedCalibSensitivity; 
	BOOL bNeedCalibZero; 

	char sReserved1[40];              // 保留 
}; 

struct CCTempSensor          // 温度传感器信息 
{ 
	// 传感器参数 
	char    sSensorNo[33];             // 传感器编号 
	char sSensorModel[33];              // 传感器型号 
	char sSensorManufacrturer[100];          // 生产厂家 
	char sSensorStartUseDate[33];          // 传感器开始使用日期 
	double  fSensorRange;             // 传感器量程，N 
	int    iSensorFullRangeVoltage;        // 传感器满量程输出电压，V 

	int    iSensorCalibInterval;          // 传感器标定间隔天数 
	char sSensorCalibDate[33];            // 最新传感器标定日期 
	char sSensorNextCalibDate[33];          // 下一次传感器标定日期 

	double  fSensorSensitivity;          // 最新传感器灵敏度标定结果(N/V) 
	double  fSensorZeroV;              // 传感器的零载荷时的输出电压（V） 

	BOOL bNeedCalibSensitivity; 

	BOOL bNeedCalibZero; 

	char sReserved1[40];              // 保留 
}; 


struct CCDisplacementSensor       // 位移传感器信息 
{ 
	// 传感器参数 
	char   sSensorNo[33];              // 传感器编号 
	char sSensorModel[33];              // 传感器型号 
	char sSensorManufacrturer[100];          // 生产厂家 
	char sSensorStartUseDate[33];          // 传感器开始使用日期 

	double  fDisplacementRange0;          // 位移传感器的标称量程(m) 
	double  fDisplacementRange;          // 位移传感器的实际量程(m) 
	double  fDisplacementResistance;        // 位移传感器满量程阻值(Ω) 

	int    iSensorCalibInterval;          // 传感器标定间隔天数 
	char sSensorCalibDate[33];            // 最新传感器标定日期 
	char sSensorNextCalibDate[33];          // 下一次传感器标定日期 

	double  fSensorSensitivity;          // 位移传感器的灵敏度(m/v) 

	BOOL bNeedCalibSensitivity; 


	char sReserved1[40];              // 保留 
}; 

struct CCAdventechAIOCard        // 模拟量采样卡信息 
{ 
	ULONG  lDevNum;               // 采样卡的设备号 
	char    sCardManufacture[100];          // 采样卡生产厂家 
	char    sCardName[100];             // 采样卡名称 
	int    iAIChanNums;              // 模拟输入通道个数 
	int    iAIInputRange;            // AI通道的满量程电压，V 
	int    iGainListNum;             // 增益数组个数 
	USHORT  usGainCodeList[20];          // 增益代码数组 
	double  fGainList[20];            // 增益数值数组 
	int    iFIFOSize;              // FIFO大小 

	double  fMaxADFreq;              // 最高采样频率，Hz 

	char sReserved1[24];              // 保留 
}; 

struct CCAdventechDIOCard        // 数字量采样卡信息 
{ 
	ULONG  lDevNum;               // 采样卡的设备号 
	char    sCardManufacture[100];          // 采样卡生产厂家 
	char    sCardName[100];             // 采样卡名称 
	BOOL bShareDIOChan;              // DI/DO是否共享通道数 

	int    iDIChanNums;              // DI通道个数 
	int    iDOChanNums;              // DO通道个数 
	char sReserved1[24];              // 保留 
}; 

struct CCCanCard            // CAN通讯卡信息 
{ 
	char    sCardManufacture[100];          // 采样卡生产厂家 
	char    sCardName[100];             // 采样卡名称 

	ULONG  lDevNum;               // 采样卡的设备号 

	int    iCanChanNums;             // Can通道个数 

	int    iCanChan;  

	DWORD  dwDevIndex;              // 使用的是哪个Can通道 
	DWORD  dwCanIndex;              // 设备索引号 
	int    iBpsNum; 
	int    iBps0[20]; 
	UCHAR  ucTimer00[20]; 
	UCHAR  ucTimer01[20]; 

	char sReserved1[24];              // 保留 
}; 


struct CCRs485Card           // 485通讯卡信息 
{ 
	char    sCardManufacture[100];          // 采样卡生产厂家 
	char    sCardName[100];             // 采样卡名称 

	UINT iPortNo;                 // 485使用的串口号，=COM1，=COM2，以此类推 

}; 

struct CCTransducerPara         // 变频器信息 
{ 
	// 变频器参数 
	char sTransduccerModel[33];           // 变频器型号 
	char sPortSettingStr[20];            // 变频器用的串口设置字符串，如 ,e,8,2  

	// 高速－变频调速系数 
	int    iMachineCalibDataNumHi;         // 最新转速标定结果中的数据个数 
	double  fStdSpeedHi[MAX_STD_SPEED_NUM];      // 减震器的转速，单位rpm 
	char sReserved12[10];              // 保留 
	UINT uStdTransducerNoHi[MAX_STD_SPEED_NUM];    // 以上转速对应的变频器整数 
	char sReserved13[10];              // 保留 

	// 低速－变频调速系数 
	int    iMachineCalibDataNumLo;         // 最新转速标定结果中的数据个数 
	double  fStdSpeedLo[MAX_STD_SPEED_NUM];      // 减震器的转速，单位rpm 
	char sReserved32[10];              // 保留 

	UINT uStdTransducerNoLo[MAX_STD_SPEED_NUM];    // 以上转速对应的变频器整数 
	char sReserved33[10];              // 保留 

	char sReserved0[50];              // 保留 
}; 

struct CCExciterPara          // 激振器信息 
{ 
	char sExciterModel[100];            // 电动缸型号 

	UINT uCanSpeed;                // 电动缸CAN接口速度  KBPS 
	UINT uCanMoudleID;               // 电动缸伺服启动器的CAN ID 

	double   fExciterSpeedScaleFactor;        // 电动缸速度比例系数 ＝，发送给电动缸的速度＝要求的速度×fExciterSpeedScaleFactor 
	double   fExciterStrokeScaleFactor;        // 电动缸行程比例系数 ＝，发送给电动缸的行程＝要求的行程×fExciterStrokeScaleFactor 

	double  fMaxSpeed, fMinSpeed;          // 电动缸的最大、最小速度m/s 
	double  fMaxStroke;              // 电动缸的最大行程m 
	double  fMinStroke;              // 电动缸的最小行程, 

	double  fGoHomeSpeed          ;    // m/s 
	double  fMoveSpeed;              // m/s 

	char sReserved0[100];              // 保留 

}; 


struct CCBeamPara            // 横梁信息 
{ 
	UINT uCycleSteps;                // 步进电机每圈步数 
	double  fScrewPitch;              // 顶杆丝杠螺距，m 

	double  fMoveSpeed;              // 横梁移动速度, m/s 


	DWORD  dwBeamStatus;             // 横梁状态 
	// 0: 可以示功(静止、已锁定） 
	// 1: 上升中 
	// 2: 下降中 
	// 3: 放松中 
	// 4: 锁紧中 
	// 5: 已放松 
	// 6: 需要标定 


	double  fBeamPos0;              // 横梁的原始位置（接触到低位限位开关时的位置）,m 
	double  fBeamPos;               // 横梁的高度（最低位置=fBeamPos0）,m 

	double  fLockCurrent;              // 横梁达到锁紧时的锁紧电流通道电压限值V 
	double  fUnLockCurrent;             // 横梁达到放松时的锁紧电流通道电压限值V 

	double  fCurrent;                // 横梁放松时的锁紧电流通道电压V 

}; 

struct CCMachineInfo          // 检测设备信息 
{ 
	char sMachineNo[33];              // 设备编号 
	char sMachineModel[33];             // 型号 
	char sMachineManufacrturer[100];         // 生产厂家 
	char sMachineStartUseDate[33];          // 开始使用日期 
	// 调速参数 
	int    iMachineCalibInterval;         // 机器标定间隔 
	char sMachineCalibDate[33];           // 最新转速标定日期 
	char sMachineNextCalibDate[33];          // 下一次转速标定日期 

	// 主力传感器参数 
	CCForceSensor tMainForceSensor; 
	BOOL bAutoMainForceZero;            // TRUE: 自动检测力传感器零点，FALSE:手动检测力传感器零点 

	// 侧向力传感器参数 
	CCForceSensor tSideForceSensor; 
	BOOL bManualSideForce;              // 无传感器，手工输入数据 

	// 位移传感器参数 
	CCDisplacementSensor tDisplacementSensor; 
	//  double  fDisplacementRange;          // 位移传感器的量程(m) 
	//  double  fDisplacementResistance;        // 位移传感器满量程阻值(Ω) 
	// 温度传感器参数 
	CCTempSensor tTempSensor; 
	BOOL bManualTemp;                // 无传感器，手工输入数据 

	// 定位停车参数 
	int    nSpeed;                // 停车前降速rpm 
	int    nCycles;               // 降速后等待的整圈数 
	int    nMilSec;               // 延时毫秒数 

	// 采集硬件设置 
	CCAdventechAIOCard tAIODev;           // 模拟量采样卡设备 
	CCAdventechDIOCard tDIODev;           // 数字量采样卡设备 
	BOOL       bStandAloneDIOCard;      // 有无独立的数字量卡（是否使用AIO的数字量通道） 

	CCCanCard     tCanDev;           // CAN设备 
	BOOL       bUseCanCard;          // 是否使用CAN设备 

	CCRs485Card      tRs485Dev;        // CAN设备 
	BOOL       bUseRs485Card;        // 是否使用CAN设备 

	// 变频器参数 
	CCTransducerPara  tTransducerPara; 

	// 电动缸参数 
	CCExciterPara   tExciterPara; 
	// 横梁参数 
	CCBeamPara      tBeamPara; 

	// 示功机硬件 
	double  fRodLen;               // 连杆长度，计算位移、速度用，单位：m，带位移传感器则忽略此参数 
	double  fHangWeight;              // 挂具重量，单位：N 

	double  fMaxStroke0;              // 示功机的最大激振行程(完整的行程)，m 
	double  fMaxSpeed0;              // 示功机的最大激振速度，m/s 

	// 使用者信息 
	char sOperatorNo[33];              // 操作员编号 
	char sCooperationNameCn[33];           // 厂家名称中文 
	char sCooperationNameEn[33];           // 厂家名称英语 

	DWORD  dwMachineCapability;          //  
	int    iMode;                // 软件运行模式 
	char sReserved1[100];              // 保留 
}; 

struct CCProductInfo          // 产品信息 
{ 
	char sProductNo[33];              // 产品型号 
	char sProductOEM[33];              // 产品OEM 
	char sProductType[33];              // 产品编号 
	int    iProductSeqNo;            // 当前产品的序号 

	char sOrderNo[33];               // 产品所属订单编号 

	double  fAbsorbWeight;            // 减震器重量，单位：N 
	double  fAbsorbRodWeight;            // 减震器连杆重量，单位：N 

	double  fTestTemp;              // 试验要求的温度℃ 
	double  fSideForce;              // 试验要施加的侧向力N 

	double  fCenterPos;              // 试验时的激振中心位置, m 
	double  fStartStopPos;             // 试验前装卡和试验停止的位置, m 

	BOOL bDifferentOffset;              // 是否各个速度单独设置行程 
	double  fOffset;               // ±行程(总行程的一半)，bDifferentOffset＝FALSE时有效，单位: m 


	double  fAirRebounceForce0;          // 充气反驳力，单位：N 

	double  fOil;                 // 注油量 

	int    iPointsPerCycle;            // 示功图每圈采样点数，必须< MAX_FORCE_NUM/3.5 
	int    iDataBandStart;            // 做示功图时有效数据区间开始的角度，从0开始，每个速度点共1260度，360度＝一个周期 
	int    iDataBandLen;             // 做示功图时有效数据区间的角度长度 

	// 试验速度设置参数
	BOOL bChkNormalSpeed;              // 是否进行常规速度检测 
	int    iSpeedNum;              // 设定试验速度个数 

	BOOL bSkipSpeed[MAX_SPEED_NUM];          // 是否不检测此速度 

	double  fSpeed0[MAX_SPEED_NUM];         // 设定最大速度数组，输入值，单位：m/s 
	double  fRotateSpeed0[MAX_SPEED_NUM];      // 设定转速数组，使用时根据fSpeed[]计算出单位: rpm 
	double  fSpeedOffset[MAX_SPEED_NUM];      // ±本 (总行程的一半)，bDifferentOffset＝TRUE时有效，否则应为fOffset，单位: m 

	double  fMaxRecruitForce1[MAX_SPEED_NUM];    // 最大复原力上限数组，输入值，单位：N 
	double  fMaxRecruitForce2[MAX_SPEED_NUM];    // 最大复原力下限数组，输入值，单位：N 
	double  fMaxCompresForce1[MAX_SPEED_NUM];    // 最大压缩力上限数组，输入值，单位：N 
	double  fMaxCompresForce2[MAX_SPEED_NUM];    // 最大压缩力下限数组，输入值，单位：N 

	double  fTime2[MAX_SPEED_NUM];         // 稳速时间数组，输入值，单位：s，在电动缸示功机中忽略此项 

	BOOL bChkFriction;               // 是否检测摩擦力 

	double  fFrictionSpeed0;            // 设定最大速度数组，输入值，单位：m/s 
	double  fFrictionRotateSpeed0;         // 设定转速数组，根据fSpeed[]计算出单位: rpm 
	double  fFrictionSpeedOffset;          // ±本 (总行程的一半)，bDifferentOffset＝TRUE时有效，否则应为fOffset，单位: m 

	double  fMaxFrictionRecruitForce1;        // 最大复原力上限数组，输入值，单位：N 
	double  fMaxFrictionRecruitForce2;        // 最大复原力下限数组，输入值，单位：N 
	double  fMaxFrictionCompresForce1;        // 最大压缩力上限数组，输入值，单位：N 
	double  fMaxFrictionCompresForce2;        // 最大压缩力下限数组，输入值，单位：N 

	double  fFrictionTime2;            // 稳速时间数组，输入值，单位：s，在电动缸示功机中忽略此项 

	int    iChkAirRebounce;            // 是否检测气压力 
	// 0: 不做 
	// 1: 利用摩擦力试验计算气压力 
	// 2: 匀速从最低点移动到最高点 
	// 3: 匀速从最低点移动到最高点，再回到最低点 
	// 4: 匀速从最低点移动到最低点 
	// 5: 匀速从最低点移动到最低点，再回到最高点 

	double  fChkAirReboundSpeed;          // 检测气压力时的移动速度    m/s 
	double  fChkAirReboundStroke;          // 检测气压力时的移动行程    m 

	int    iPreLoadTimes;            // 示功检测前的预拉次数（排气） 
	double  fPreLoadSpeed;            // 示功检测前的预拉速度（排气）  m/s 

	char sTestComment[80];              // 试验说明 
	char sCustomerName[40];             // 客户名称 
	char sReserved100[240];             // 保留 
}; 

///测试文件记录结构
struct CCTestRecord
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

	BOOL bFrictionSpeed;                  // 是否包含摩擦力测量结果 
	double  fFrictionSetSpeed;               // 设定速度，单位: m/s 
	double  fFrictionRealSpeed;              // 实测速度，单位: m/s 
	double  fFrictionSetOffset;              // 设定行程，单位: m 
	double  fFrictionRealOffset;              // 检测结果行程，单位: m 
	double  fFrictionPym;                 // 最大压缩力，单位：N 
	double  fFrictionPfm;                 // 最大拉伸力，单位：N 
	int    iFrictionNumOfForce;              // 每个速度下测量的力的个数，最多MAX_SPEED_NUM 
	double  fFrictionDataFreq;               // 检测结果采样频率，单位: Hz 
	double  fFrictionForce[MAX_FORCE_NUM];          // 与速度数组对应的每个速度下测量到的力数组，单位: N 
	double  fFrictionDisplacement[MAX_FORCE_NUM];       // 与速度数组对应的每个速度下测量到的位移数组，单位: m 

	int    iMode;                    // 软件运行模式 

	BOOL bUseDisplacementSensor;               // 是否使用位移传感器 

	double  fAirRebounceForce;               // 充气反驳力，单位：N 

	BOOL bHasAbsorb; 
	int    iChkAirRebounce;             
	char sResultComment[80];                // 试验结果说明 

	char sReserved100[316];                 // 保留 
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
	//对二级节点双循环链表的操作
	BOOL InsertFileNode(ProductNodePtr pProductNode, FileNodePtr pFileNodeToInsert);
	BOOL DeleteFileNode(ProductNodePtr pProductNode_Ahead, ProductNodePtr pProductNodet, FileNodePtr pFileNodeToDelete);
	void DeleteFileNode(FileNodePtr pFileNodeToDelete);
	void DeleteFileNode(const char *sFileName);
	BOOL SearchFileNode(const char *sFileName, FileNodePtr &pFileNodeFound);
	//对一级节点双循环链表的操作
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
