#ifndef _GLOBALVARS_H_
#define _GLOBALVARS_H_

#include "XHtmlTree\\XHtmlTree.h"
#include "LeftTreeView.h"
#include "RightDrawAreaView.h"
#include "FilterFormView.h"
#include "TestDataTreeMgt.h"

#define CONFIG_DIR_FILE		"directories.txt"

extern CStringArray g_saDirectories;

extern CXHtmlTree *g_pTree;
extern CLeftTreeView *g_pLeftTreeView;
extern CRightDrawAreaView *g_pRightDrawAreaView;
extern CFilterFormView *g_pFilterFormView;
extern CTestDataTreeMgt g_TestDataTreeMgt;

#endif