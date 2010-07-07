#ifndef _GLOBALFUNCS_H_
#define _GLOBALFUNCS_H_

extern BOOL LoadFile(const char *file, struct TestRecord &rec);
extern BOOL SaveFile(const char *file, struct TestRecord rec);

extern BOOL LoadNode();
extern void ReleaseDirNode();

//œ‘ æœ˚œ¢
extern void ShowMsg(const char *msg);
BOOL CheckStrDateFormat(const char *StrDateTime, int &year, int &month, int &day);
BOOL CheckStrTimeFormat(const char *StrTime, int &hour, int &minute);

extern BOOL LoadDirFromConfigFile();
extern BOOL SaveDirToConfigFile();

extern void TestMime();

#endif
