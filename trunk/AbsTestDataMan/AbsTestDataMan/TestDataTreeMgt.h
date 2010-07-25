#pragma once

#include <afxwin.h>
#include "FilterConfigMgt.h"

#define	MAX_BUF_LEN			1024	//最大缓冲区大小

#define MAX_STD_SPEED_NUM	50      // 变频器数据的个数
#define MAX_SPEED_NUM		15		// 最大速度个数
#define MAX_FORCE_NUM		1600	// 最大力个数

#define REFDOT_NUM			7		//参考点个数
#define REFDOT_ARRAY		{90, 270, 450, 630, 810, 990, 1260}	//参考点数组
#define SKIP_FIRSTDOT_NUM	125		//略过起始测试点个数

#define COEF_OF_FILTER		10		//滤波采样频率要乘以的倍数

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

	char sWorkDay[33];						// 测试工作日
	char sTestDate[33];						// 测试日期
	char sScheduledDate[33];				// 计划日期

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
	double  fFrictionPym;					// 最大压缩力，单位：N 
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

struct TestRecordFileNode;	//预先声明，下面结构要用到
///左侧列表树中叶子节点项对应的数据项的结构，叶子节点的数据指针指向本结构，从而得知叶子节点的数据信息
struct TreeItemData
{
	int iIndex;							//对应的速度索引，数组下标i，而非iSpdIndex[i]
	BOOL bSelected;						//是否被选中
	struct TestRecordFileNode *pNode;	//对应测试文件结点
};

///附加信息结构，存储经过计算后得到的相关有用数据
struct AdditionInfo
{
	double fDisplacementOfFilter[MAX_SPEED_NUM][MAX_FORCE_NUM];	//滤波后的位移值
	double fForceOfFilter[MAX_SPEED_NUM][MAX_FORCE_NUM];		//滤波后的力值
	double fSpeedOfFilter[MAX_SPEED_NUM][MAX_FORCE_NUM];		//滤波后的速度值(位移的微分)
	int iDataBandStart[MAX_SPEED_NUM];							//有效数据的起始点位置
	int iDataBandLen[MAX_SPEED_NUM];							//有效数据的宽度

	//摩擦力数据
	double fFrictionDisplacementOfFilter[MAX_FORCE_NUM];		//滤波后的摩擦力位移值
	double fFrictionForceOfFilter[MAX_FORCE_NUM];				//滤波后的摩擦力力值
	double fFrictionSpeedOfFilter[MAX_FORCE_NUM];				//滤波后的速度值(位移的微分)
	int iFrictionDataBandStart;									//摩擦力有效数据起始点位置
	int iFrictionDataBandLen;									//摩擦力有效数据宽度
};

///测试文件记录的主要信息，没有全部读出整个文件，而是提取了必要的信息，减少读取时间
struct CCTestRecordMainInfo
{
	BOOL bDataProcessed;					// =TRUE : 其中数据已经处理过，FALSE：没有处理过
	BOOL bDataValid;						// =TRUE : 含有效示功图数据,FALSE: 不含有效数据

	char sWorkDay[33];						// 测试工作日
	char sTestDate[33];						// 测试日期
	char sScheduledDate[33];				// 计划日期

	double  fTestTemp;						// 检测到的试验温度℃
	double  fSideForce;						// 检测到的的侧向力N

	BOOL bNormalSpeed;						// 是否包含常规速度测量结果

	DWORD  dwResultStatus;					// 测试结论

	int    iNumOfSpeed;						// 包含的已测量的速度的个数，最多MAX_SPEED_NUM

	int    iSpdIndex[MAX_SPEED_NUM];		// 此速度在测试规范中的速度序号
};

///测试数据文件结点结构，存储该文件处理前和处理后的所有信息
typedef struct TestRecordFileNode
{
	BOOL bProcessed;		//是否经过了处理
	TCHAR sProductNo[33];	//试件型号
	TCHAR sOperatorNo[33];	//操作员
	TCHAR sFileName[256];	//文件名称
	TCHAR sTestDate[33];	//测试日期

	struct CCTestRecordMainInfo test_record_maininfo;	//主要信息，在显示左侧列表树时读取
	struct CCMachineInfo *pMachineInfo;					//机器信息，处理数据时分配内存
	struct CCProductInfo *pProductInfo;					//产品信息，处理数据时分配内存
	struct CCTestRecord *pTestRecord;					//测试记录信息，处理数据时分配内存，并计算一些结果
	struct AdditionInfo *pAdditionInfo;					//附加信息，处理数据时分配内存，并计算该结果
	struct TreeItemData tree_item_data[MAX_SPEED_NUM];	//左侧树节点对应的信息
	struct TestRecordFileNode *pNext;					//链表后继
	struct TestRecordFileNode *pPrev;					//链表前驱
} FileNode, *FileNodePtr, *FileNodeList;				//结点，结点指针，链表

///型号结点数据结构，对应着左侧树列表中的一个型号树
typedef struct ProductNode
{
	TCHAR sProductNo[33];								//型号
	FileNodePtr pFileListHead;							//测试文件链表的表头，单独分配一个单元，其pNext指向第一个测试文件
	FileNodePtr pFileListTail;							//测试文件链表的表尾，仅指向测试文件，不分配一个单元
	struct ProductNode *pNext;							//链表前驱
	struct ProductNode *pPrev;							//链表后继
} ProductNode, *ProductNodePtr, *ProductNodeList;		//结点，结点指针，链表

///型号树结构，整个系统只需要这么一个结构
typedef struct ProductTreeRoot
{
	ProductNodeList pProductNodeListHead;
	ProductNodeList pProductNodeListTail;
} ProductTreeRoot, *ProductTreeRootPtr;

///测试数据树管理类
class CTestDataTreeMgt
{
public:
	CTestDataTreeMgt(void);
	~CTestDataTreeMgt(void);

	//初始化树
	void InitTree(void);
	//刷新树
	void ReloadTree(void);
	//释放树

	void FreeTree(void);
	//释放树的文件结点
	void FreeFileNode(FileNodePtr pFileNode);
	//加载文件到树中
	BOOL LoadFile(const char *sFileNmae);
	//加载文件到树中并处理文件
	BOOL LoadAndProcessFile(const char *sFileName, FileNodePtr pFileNode);
	//获取文件的主要信息
	BOOL GetFileMainInfo(const char *sFileName, FileNodePtr pFileNode);

	//对二级节点双循环链表的操作

	//插入文件结点
	BOOL InsertFileNode(ProductNodePtr pProductNode, FileNodePtr pFileNodeToInsert);
	//在型号链表中删除文件结点
	BOOL DeleteFileNode(ProductNodePtr pProductNode_Ahead, ProductNodePtr pProductNodet, FileNodePtr pFileNodeToDelete);
	//在整棵树中删除文件结点
	void DeleteFileNode(FileNodePtr pFileNodeToDelete);
	//按文件名在整棵树中删除文件结点
	void DeleteFileNode(const char *sFileName);
	//按文件名查找文件结点
	BOOL SearchFileNode(const char *sFileName, FileNodePtr &pFileNodeFound);

	//对一级节点双循环链表的操作

	//插入型号结点
	BOOL InsertProductNode(ProductNodePtr pProductNodeToInsert);
	//插入型号结点
	void InsertProductNode(const char *sProductNo, ProductNodePtr &pProductNodeInserted);
	//删除型号结点
	BOOL DeleteProductNode(ProductNodePtr pProductNodeToDelete);
	//删除型号结点
	void DeleteProductNode(const char *sProductNo);
	//查找型号结点
	BOOL SearchProductNode(const char *sProductNo, ProductNodePtr &pProductNodeFound);

	//重置结点状态，即将结点设置为全部未选中
	void ResetNodeStatus();

	//获取根子树的指针
	ProductTreeRootPtr GetProductTreeRootPtr();
protected:
	void ProcessData(FileNodePtr pFileNode);
	BOOL PassFilter(FileNodePtr pFileNode);
	int filter_new( 
		double dat[],     // 数据数组 
		double fs,      // 采样频率 Hz 
		double fc,       // 滤波频率 Hz 
		int num        // 数据个数 
		);
	int fir_dsgn(int Len, double FreqS, double FreqB , double **Coef1);
protected:
	ProductTreeRootPtr m_pProductTreeRoot;
	struct FilterConfig m_filterConfig;
};
