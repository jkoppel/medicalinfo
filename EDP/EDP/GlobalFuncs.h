#ifndef _GLOBALFUNCS_H_
#define _GLOBALFUNCS_H_

extern BOOL LoadFile(const char *file, struct CCTestRecord &rec);
extern BOOL SaveFile(const char *file, struct CCTestRecord rec);

extern BOOL LoadNode();
extern void ReleaseDirNode();

//显示消息
extern void ShowMsg(const char *msg);
BOOL CheckStrDateFormat(const char *StrDateTime, int &year, int &month, int &day);
BOOL CheckStrTimeFormat(const char *StrTime, int &hour, int &minute);

extern BOOL LoadDirFromConfigFile();
extern BOOL SaveDirToConfigFile();

extern void TestMime();

extern int filter_new( 
			   double dat[],     // 数据数组 
			   double fs,      // 采样频率 Hz 
			   double fc,       // 滤波频率 Hz 
			   int num        // 数据个数 
			   );
extern int fir_dsgn(int Len, double FreqS, double FreqB , double **Coef1);

#endif
