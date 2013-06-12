#ifndef _GLOBAL_VARS_H_
#define _GLOBAL_VARS_H_

class CLeftView;
class CPdfClimateView;
class CGraphSelectView;
class CDottedGraphView;
class CFacetGraphView;
class CLinearGraphView;
class CColumnarGraphView;
class CUnknownGraphView;
class CGraphFormView;

extern CLeftView *g_pLeftView;
extern CGraphSelectView *g_pGraphSelectView;
extern CPdfClimateView *g_pPdfClimateView;
extern CDottedGraphView *g_pDottedGraphView;
extern CLinearGraphView *g_pLinearGraphView;
extern CFacetGraphView *g_pFacetGraphView;
extern CColumnarGraphView *g_pColumnarGraphView;
extern CUnknownGraphView *g_pUnknownGraphView;
extern CGraphFormView *g_pViewList[5];

#endif
