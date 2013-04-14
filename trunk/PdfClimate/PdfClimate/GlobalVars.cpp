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

CLeftView *g_pLeftView = NULL;
CGraphSelectView *g_pGraphSelectView = NULL;
CPdfClimateView *g_pPdfClimateView = NULL;
CDottedGraphView *g_pDottedGraphView = NULL;
CLinearGraphView *g_pLinearGraphView = NULL;
CFacetGraphView *g_pFacetGraphView = NULL;
CColumnarGraphView *g_pColumnarGraphView = NULL;