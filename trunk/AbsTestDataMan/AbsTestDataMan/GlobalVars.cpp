#include "GlobalVars.h"
#include "Resource.h"
#include "AbsTestDataMan.h"

int g_iDirNodeNum = 0;
struct TestRecordDirNode *g_pDirNode = NULL;

CStringArray g_saDirectories;

CXHtmlTree *g_pTree = NULL;
CLeftTreeView *g_pLeftTreeView = NULL;
CRightDrawAreaView *g_pRightDrawAreaView = NULL;
CFilterFormView *g_pFilterFormView = NULL;
CTestDataTreeMgt g_TestDataTreeMgt;
