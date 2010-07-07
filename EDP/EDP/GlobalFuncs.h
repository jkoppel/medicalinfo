#ifndef _GLOBALFUNCS_H_
#define _GLOBALFUNCS_H_

extern BOOL LoadFile(const char *file, struct CCTestRecord &rec);
extern BOOL SaveFile(const char *file, struct CCTestRecord rec);

extern BOOL LoadNode();
extern void ReleaseDirNode();

//��ʾ��Ϣ
extern void ShowMsg(const char *msg);
BOOL CheckStrDateFormat(const char *StrDateTime, int &year, int &month, int &day);
BOOL CheckStrTimeFormat(const char *StrTime, int &hour, int &minute);

extern BOOL LoadDirFromConfigFile();
extern BOOL SaveDirToConfigFile();

extern void TestMime();

extern int filter_new( 
			   double dat[],     // �������� 
			   double fs,      // ����Ƶ�� Hz 
			   double fc,       // �˲�Ƶ�� Hz 
			   int num        // ���ݸ��� 
			   );
extern int fir_dsgn(int Len, double FreqS, double FreqB , double **Coef1);

#endif
