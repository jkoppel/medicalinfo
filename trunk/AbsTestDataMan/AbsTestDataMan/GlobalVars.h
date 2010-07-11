#ifndef _GLOBALVARS_H_
#define _GLOBALVARS_H_

#include "XHtmlTree\\XHtmlTree.h"
#include "LeftTreeView.h"
#include "RightDrawAreaView.h"
#include "FilterFormView.h"
#include "TestDataTreeMgt.h"

#define CONFIG_DIR_FILE		"directories.txt"


struct TestRecordDirNode{
	TCHAR sDir[256];
	int iNum;
	struct TestRecordFileNode *pFileNode;
};

extern int g_iDirNodeNum;
extern struct TestRecordDirNode *g_pDirNode;

extern CStringArray g_saDirectories;

extern CXHtmlTree *g_pTree;
extern CLeftTreeView *g_pLeftTreeView;
extern CRightDrawAreaView *g_pRightDrawAreaView;
extern CFilterFormView *g_pFilterFormView;
extern CTestDataTreeMgt g_TestDataTreeMgt;

#endif