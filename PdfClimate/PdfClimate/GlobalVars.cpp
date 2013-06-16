#include "stdafx.h"
#include "GlobalVars.h"

#include "PdfClimate.h"
#include "PdfClimateDoc.h"
#include "PdfClimateView.h"
#include "LeftView.h"
#include "GraphSelectView.h"
#include "DottedGraphView.h"
#include "FacetGraphView.h"
#include "LinearGraphView.h"
#include "ColumnarGraphView.h"
#include "UnknownGraphView.h"
#include "BaseGraphView.h"
#include "DocInfoView.h"
#include "ImageProcess/BaseDocument.h"

CLeftView *g_pLeftView = NULL;
CGraphSelectView *g_pGraphSelectView = NULL;
CPdfClimateView *g_pPdfClimateView = NULL;
CDottedGraphView *g_pDottedGraphView = NULL;
CLinearGraphView *g_pLinearGraphView = NULL;
CFacetGraphView *g_pFacetGraphView = NULL;
CColumnarGraphView *g_pColumnarGraphView = NULL;
CUnknownGraphView *g_pUnknownGraphView = NULL;
CBaseGraphView *g_pViewList[5];
CDocInfoView *g_pDocInfoView = NULL;
CBaseDocument *g_pCurrDocument = NULL;