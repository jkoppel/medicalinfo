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
class CBaseGraphView;
class CDocInfoView;
class CBaseDocument;

extern CLeftView *g_pLeftView;
extern CGraphSelectView *g_pGraphSelectView;
extern CPdfClimateView *g_pPdfClimateView;
extern CDottedGraphView *g_pDottedGraphView;
extern CLinearGraphView *g_pLinearGraphView;
extern CFacetGraphView *g_pFacetGraphView;
extern CColumnarGraphView *g_pColumnarGraphView;
extern CUnknownGraphView *g_pUnknownGraphView;
extern CBaseGraphView *g_pViewList[5];
extern CDocInfoView *g_pDocInfoView;
extern CBaseDocument *g_pCurrDocument;

#endif
