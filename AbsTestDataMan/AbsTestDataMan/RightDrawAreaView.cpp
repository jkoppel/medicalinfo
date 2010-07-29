// RightDrawAreaView.cpp : CRightDrawAreaView 类的实现
//

#include "stdafx.h"
#include "AbsTestDataMan.h"
#include "AbsTestDataManDoc.h"
#include "RightDrawAreaView.h"
#include "GlobalVars.h"
#include "GlobalFuncs.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRightDrawAreaView

IMPLEMENT_DYNCREATE(CRightDrawAreaView, CView)

BEGIN_MESSAGE_MAP(CRightDrawAreaView, CView)
	// 标准打印命令
	ON_NOTIFY(TCN_SELCHANGE, IDC_DRAW_TAB, &CRightDrawAreaView::OnTcnSelchangeTab)
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


void DrawCurve(struct CanvasInfo ci)
{
	int i, j;
	double fXMax, fXMin;
	double fYMax, fYMin;

	if(ci.iNumOfCurve>0){
		fXMax = ci.pCurveInfo[0].pXData[0];
		fXMin = fXMax;
		fYMax = ci.pCurveInfo[0].pYData[0];
		fYMin = fYMax;

		for(i=0;i<ci.iNumOfCurve;i++){
			for(j=0;j<ci.pCurveInfo[i].iNumOfPoint;j++){
				if(ci.pCurveInfo[i].pXData[j] > fXMax){
					fXMax = ci.pCurveInfo[i].pXData[j];
				}
				if(ci.pCurveInfo[i].pXData[j] < fXMin){
					fXMin = ci.pCurveInfo[i].pXData[j];
				}
				if(ci.pCurveInfo[i].pYData[j] > fYMax){
					fYMax = ci.pCurveInfo[i].pYData[j];
				}
				if(ci.pCurveInfo[i].pYData[j] < fYMin){
					fYMin = ci.pCurveInfo[i].pYData[j];
				}
			}
		}
	}
	else{
		fXMax = 900;
		fXMin = 100;
		fYMax = 900;
		fYMin = 100;
	}

	double fXRange = fXMax - fXMin;
	int iXMinUnitVal, iXMaxUnitVal;
	double fXUnitWidth;
	fXUnitWidth = 1;
	if(fXRange>=2){
		while( !(fXRange>=2 && fXRange<20) ){
			fXRange = fXRange / 10;
			fXUnitWidth *= 10;
		}
	}
	else{
		while( !(fXRange>=2 && fXRange<20) ){
			fXRange = fXRange * 10;
			fXUnitWidth /= 10;
		}
	}
	iXMaxUnitVal = (int)ceil( fXMax / fXUnitWidth + 0.5 );
	iXMinUnitVal = (int)floor( fXMin / fXUnitWidth - 0.5 );
	if(iXMaxUnitVal-iXMinUnitVal>10){
		fXUnitWidth *= 2;
		iXMaxUnitVal = (int)ceil( fXMax / fXUnitWidth );
		iXMinUnitVal = (int)floor( fXMin / fXUnitWidth );
	}

	double fYRange = fYMax - fYMin;
	int iYMinUnitVal, iYMaxUnitVal;
	double fYUnitWidth;
	fYUnitWidth = 1;
	if(fYRange>=2){
		while( !(fYRange>=2 && fYRange<20) ){
			fYRange = fYRange / 10;
			fYUnitWidth *= 10;
		}
	}
	else{
		while( !(fYRange>=2 && fYRange<20) ){
			fYRange = fYRange * 10;
			fYUnitWidth /= 10;
		}
	}
	iYMaxUnitVal = (int)ceil( fYMax / fYUnitWidth ) + 1;
	iYMinUnitVal = (int)floor( fYMin / fYUnitWidth ) - 1;
	if(iYMaxUnitVal-iYMinUnitVal>10){
		fYUnitWidth *= 2;
		iYMaxUnitVal = (int)ceil( fYMax / fYUnitWidth );
		iYMinUnitVal = (int)floor( fYMin / fYUnitWidth );
	}

	POINT pntRefPoint;
	int iDrawAreaWidth, iDrawAreaHeight;
	int iXSpan;
	int iYSpan;
	RECT rect;
	CPen pen;
	CBrush brush;
	char buf[MAX_BUF_LEN];
	CFont fontLogo,*oldFont;
	CDC *pDC;

	if((ci.rect.bottom - ci.rect.top<100) ||
	   (ci.rect.right - ci.rect.left<100) ){
		return;
	}

	//获取待画图区域大小位置
	pntRefPoint.x = ci.rect.left + 60;
	pntRefPoint.y = ci.rect.bottom - 50;
	iDrawAreaWidth = ci.rect.right - ci.rect.left - 100;
	iDrawAreaHeight = ci.rect.bottom - ci.rect.top - 100;
	iXSpan = iDrawAreaWidth / (iXMaxUnitVal - iXMinUnitVal);
	iYSpan = iDrawAreaHeight / (iYMaxUnitVal - iYMinUnitVal);
	iDrawAreaWidth = iXSpan * (iXMaxUnitVal - iXMinUnitVal);
	iDrawAreaHeight = iYSpan * (iYMaxUnitVal - iYMinUnitVal);

	pDC = ci.pDC;

	pen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	pDC->SelectObject(&pen);
	//画横轴
	pDC->MoveTo(pntRefPoint);
	pDC->LineTo(pntRefPoint.x + iDrawAreaWidth, pntRefPoint.y);
	//画纵轴
	pDC->MoveTo(pntRefPoint);
	pDC->LineTo(pntRefPoint.x, pntRefPoint.y - iDrawAreaHeight);

	//画轴刻度及文字标识
	for(i=0; i<=iXMaxUnitVal-iXMinUnitVal; i++){
		if(i>0){
			pDC->MoveTo(pntRefPoint.x + i * iXSpan, pntRefPoint.y);
			pDC->LineTo(pntRefPoint.x + i * iXSpan, pntRefPoint.y + 5);
		}

		if(fXUnitWidth>=1){
			sprintf_s(buf, "%d", (int)((iXMinUnitVal+i) * fXUnitWidth));
		}
		else{
			sprintf_s(buf, "%.2f", (iXMinUnitVal+i) * fXUnitWidth);
		}
		rect.left = pntRefPoint.x  + i * iXSpan - 25;
		rect.right = pntRefPoint.x + i * iXSpan + 25;
		rect.top = pntRefPoint.y;
		rect.bottom = pntRefPoint.y + 25;
		pDC->DrawText(CString(buf), &rect, DT_SINGLELINE | DT_VCENTER| DT_CENTER);
	}
	for(i=0; i<=iYMaxUnitVal-iYMinUnitVal; i++){
		if(i>0){
			pDC->MoveTo(pntRefPoint.x, pntRefPoint.y - i * iYSpan);
			pDC->LineTo(pntRefPoint.x - 5, pntRefPoint.y - i * iYSpan);
		}

		if(fYUnitWidth>=1){
			sprintf_s(buf, "%d", (int)((iYMinUnitVal+i) * fYUnitWidth));
		}
		else{
			sprintf_s(buf, "%.2f", (iYMinUnitVal+i) * fYUnitWidth);
		}
		rect.left = pntRefPoint.x  - 45;
		rect.right = pntRefPoint.x - 5;
		rect.top = pntRefPoint.y - i * iYSpan - 10;
		rect.bottom = pntRefPoint.y - i * iYSpan + 10;
		pDC->DrawText(CString(buf), &rect, DT_SINGLELINE | DT_VCENTER| DT_CENTER);
	}

	pen.DeleteObject();

	//画纵横虚线
	LOGBRUSH logbrush;
	logbrush.lbColor = RGB(192,192,192); 
	logbrush.lbStyle = PS_SOLID;
	pen.CreatePen(PS_ALTERNATE, 1, &logbrush);
	pDC->SelectObject(&pen);
	for(i=1;i<=iXMaxUnitVal-iXMinUnitVal;i++){
		pDC->MoveTo(pntRefPoint.x + i * iXSpan, pntRefPoint.y -2);
		pDC->LineTo(pntRefPoint.x + i * iXSpan, pntRefPoint.y- iDrawAreaHeight + 2);
	}
	for(i=1;i<=iYMaxUnitVal-iYMinUnitVal;i++){
		pDC->MoveTo(pntRefPoint.x + 2, pntRefPoint.y - iYSpan * i);
		pDC->LineTo(pntRefPoint.x + iDrawAreaWidth - 2, pntRefPoint.y - iYSpan * i);
	}
	pen.DeleteObject();

	//画标识

	//大标题
	pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	rect.left = pntRefPoint.x + iDrawAreaWidth/2 - 120;
	rect.right = pntRefPoint.x + iDrawAreaWidth/2 + 30;
	rect.top = pntRefPoint.y - iDrawAreaHeight - 20;
	rect.bottom = pntRefPoint.y - iDrawAreaHeight;
	fontLogo.CreateFont(20,10,0,0,0,FALSE,FALSE,FALSE,
						DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
						FIXED_PITCH | FF_ROMAN, _T("System"));
	pDC->SetBkMode(TRANSPARENT);
	oldFont=pDC->SelectObject(&fontLogo);
	pDC->DrawText(CString(ci.sCanvasInfo), &rect, DT_SINGLELINE | DT_LEFT);

	//横轴标题
	rect.left = pntRefPoint.x + iDrawAreaWidth/2 - 50;
	rect.right = pntRefPoint.x + iDrawAreaWidth/2 + 100;
	rect.top = pntRefPoint.y + 20;
	rect.bottom = pntRefPoint.y + 40;
	fontLogo.DeleteObject();
	fontLogo.CreateFont(16,8,0,0,0,FALSE,FALSE,FALSE,
						DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
						FIXED_PITCH | FF_ROMAN,_T("System"));
	pDC->SetBkMode(TRANSPARENT);
	oldFont=pDC->SelectObject(&fontLogo);
	pDC->DrawText(CString(ci.sXAxisInfo), &rect, DT_SINGLELINE | DT_LEFT );

	//纵轴标题
	rect.left = pntRefPoint.x - 60;
	rect.right = pntRefPoint.x - 20;
	rect.top = pntRefPoint.y - iDrawAreaHeight/2;
	rect.bottom = pntRefPoint.y - iDrawAreaHeight/2 - 100;
	fontLogo.DeleteObject();
	fontLogo.CreateFont(16, 0, 900, 900, 0, 0, 0, 0,
						DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
						DEFAULT_PITCH, _T("System"));
	pDC->SetBkMode(TRANSPARENT);
	oldFont=pDC->SelectObject(&fontLogo);
	pDC->DrawText(CString(ci.sYAxisInfo), &rect, DT_SINGLELINE | DT_LEFT );

	//画每一条曲线
	for(i=0;i<ci.iNumOfCurve;i++){
		int x_start, y_start, x, y;
		pen.DeleteObject();
		pen.CreatePen(PS_SOLID, 2, ci.pCurveInfo[i].crColor);
		pDC->SelectObject(&pen);
		for(j=0;j<ci.pCurveInfo[i].iNumOfPoint;j++){
			x = (int)(pntRefPoint.x + (ci.pCurveInfo[i].pXData[j] - iXMinUnitVal * fXUnitWidth) * iXSpan / fXUnitWidth);
			y = (int)(pntRefPoint.y - (ci.pCurveInfo[i].pYData[j] - iYMinUnitVal * fYUnitWidth) * iYSpan / fYUnitWidth);
			if(j==0){
				if(ci.pCurveInfo[i].bIsClosed){
					x_start = x;
					y_start = y;
				}
				pDC->SetPixel(x, y, ci.pCurveInfo[i].crColor);
				pDC->MoveTo(x, y);
			}
			else{
				pDC->LineTo(x, y);
			}
		}
		if(ci.pCurveInfo[i].bIsClosed){
			pDC->LineTo(x_start, y_start);
		}
	}
}

// CRightDrawAreaView 构造/析构

CRightDrawAreaView::CRightDrawAreaView()
{
	//初始化值
	m_iDrawMode = DM_FILTER_ONLY;
	m_pntClientAreaRef.x = 0;
	m_pntClientAreaRef.y = 0;
	m_nClientAreaWidth = 0;
	m_nClientAreaHeight = 0;

	m_nMaxNumOfCurveToDraw = 20;
	m_pCurveInfo = new struct CurveInfo[m_nMaxNumOfCurveToDraw];
	memset(m_pCurveInfo, 0, m_nMaxNumOfCurveToDraw * sizeof(struct CurveInfo));

	m_pTab = NULL;

	g_pRightDrawAreaView = this;
}

CRightDrawAreaView::~CRightDrawAreaView()
{
	if(m_pTab){
		delete m_pTab;
	}
	if(m_pCurveInfo){
		delete m_pCurveInfo;
	}
}

BOOL CRightDrawAreaView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CRightDrawAreaView 绘制

void CRightDrawAreaView::OnDraw(CDC* /*pDC*/)
{
	CAbsTestDataManDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	//画图
	int iIndex = m_pTab->GetCurSel();
	switch(iIndex){
		case 0:
			Draw_Force_Vs_Position();
			break;
		case 1:
			Draw_Force_Vs_Speed();
			break;
		case 2:
			Draw_Force_Vs_PositionAndSpeed();
			break;
		case 3:
			Draw_MaxForce_Vs_Speed();
			break;
		case 4:
			Draw_Force_Vs_Time();
			break;
		case 5:
			Draw_Friction_Force_Vs_Position();
			break;
		default:
			break;
	}
}

void CRightDrawAreaView::SetDrawMode(CRightDrawAreaView::DRAW_MODE iMode)
{
	m_iDrawMode = iMode;
}

CRightDrawAreaView::DRAW_MODE CRightDrawAreaView::GetDrawMode()
{
	return m_iDrawMode;
}

// CRightDrawAreaView 打印

BOOL CRightDrawAreaView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CRightDrawAreaView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CRightDrawAreaView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清除过程
}


// CRightDrawAreaView 诊断

#ifdef _DEBUG
void CRightDrawAreaView::AssertValid() const
{
	CView::AssertValid();
}

void CRightDrawAreaView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAbsTestDataManDoc* CRightDrawAreaView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAbsTestDataManDoc)));
	return (CAbsTestDataManDoc*)m_pDocument;
}
#endif //_DEBUG

void CRightDrawAreaView::Draw_Force_Vs_Position()
{
	COLORREF color[] = {RGB(255,0,0),RGB(0,255,0),RGB(0,0,255),RGB(255,255,0),RGB(255,0,255),RGB(0,255,255),RGB(0,0,0),};
	struct CanvasInfo ci;

	memset(&ci, 0, sizeof(struct CanvasInfo));
	ci.pCurveInfo = m_pCurveInfo;

	ProductTreeRootPtr pProductTreeRoot = g_TestDataTreeMgt.GetProductTreeRootPtr();
	ProductNodePtr pProductNode = g_TestDataTreeMgt.GetProductTreeRootPtr()->pProductNodeListHead->pNext;

	while(pProductNode!=NULL && ci.iNumOfCurve < m_nMaxNumOfCurveToDraw){
		FileNodePtr pFileNode = pProductNode->pFileListHead->pNext;
		while(pFileNode!=NULL && ci.iNumOfCurve < m_nMaxNumOfCurveToDraw){
			int flag_selected = 0;
			for(int iIndex=0;iIndex<pFileNode->test_record_maininfo.iNumOfSpeed;iIndex++){
				struct TreeItemData *tp = pFileNode->tree_item_data + iIndex;
				if(tp->bSelected){
					flag_selected = 1;
					if(tp->pNode->bProcessed==FALSE){
						char sFileName[256];
						TCHAR2char(sFileName, tp->pNode->sFileName, _tcslen(tp->pNode->sFileName));
						g_TestDataTreeMgt.LoadAndProcessFile(sFileName, tp->pNode);
					}

					if(m_iDrawMode & DM_NORMAL_ONLY){
						ci.pCurveInfo[ci.iNumOfCurve].bIsClosed = TRUE;
						ci.pCurveInfo[ci.iNumOfCurve].crColor = color[tp->iIndex];
						ci.pCurveInfo[ci.iNumOfCurve].iNumOfPoint = tp->pNode->pAdditionInfo->iDataBandLen[tp->iIndex];
						ci.pCurveInfo[ci.iNumOfCurve].pXData = &tp->pNode->pTestRecord->fDisplacement[tp->iIndex][tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]];
						ci.pCurveInfo[ci.iNumOfCurve].pYData = &tp->pNode->pTestRecord->fForce[tp->iIndex][tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]];
						ci.pCurveInfo[ci.iNumOfCurve].iWidth = 2;

						ci.iNumOfCurve ++;
						if(ci.iNumOfCurve>=m_nMaxNumOfCurveToDraw){
							break;
						}
					}
					
					if(m_iDrawMode & DM_FILTER_ONLY){
						ci.pCurveInfo[ci.iNumOfCurve].bIsClosed = TRUE;
						ci.pCurveInfo[ci.iNumOfCurve].crColor = color[tp->iIndex];
						ci.pCurveInfo[ci.iNumOfCurve].iNumOfPoint = tp->pNode->pAdditionInfo->iDataBandLen[tp->iIndex];
						ci.pCurveInfo[ci.iNumOfCurve].pXData = &tp->pNode->pTestRecord->fDisplacement[tp->iIndex][tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]];
						ci.pCurveInfo[ci.iNumOfCurve].pYData = &tp->pNode->pAdditionInfo->fForceOfFilter[tp->iIndex][tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]];
						ci.pCurveInfo[ci.iNumOfCurve].iWidth = 2;

						ci.iNumOfCurve ++;
						if(ci.iNumOfCurve>=m_nMaxNumOfCurveToDraw){
							break;
						}
					}
				}
			}
			if(flag_selected==0 && pFileNode->bProcessed){
				delete pFileNode->pMachineInfo;
				pFileNode->pMachineInfo = NULL;
				delete pFileNode->pProductInfo;
				pFileNode->pProductInfo = NULL;
				delete pFileNode->pTestRecord;
				pFileNode->pTestRecord = NULL;
				delete pFileNode->pAdditionInfo;
				pFileNode->pAdditionInfo = NULL;
				pFileNode->bProcessed = FALSE;
			}
			pFileNode = pFileNode->pNext;
		}//END OF WHILE
		pProductNode = pProductNode->pNext;
	}

	ci.pDC = GetDC();
	GetClientRect(&ci.rect);
	sprintf(ci.sCanvasInfo, "%s", "Force vs.Position");
	sprintf(ci.sXAxisInfo, "%s", "Position [mm]");
	sprintf(ci.sYAxisInfo, "%s", "Force [N]");
	DrawCurve(ci);
}

void CRightDrawAreaView::Draw_Force_Vs_PositionAndSpeed()
{
}

void CRightDrawAreaView::Draw_Force_Vs_Speed()
{
	COLORREF color[] = {RGB(255,0,0),RGB(0,255,0),RGB(0,0,255),RGB(255,255,0),RGB(255,0,255),RGB(0,255,255),RGB(0,0,0),};
	struct CanvasInfo ci;

	memset(&ci, 0, sizeof(struct CanvasInfo));
	ci.pCurveInfo = m_pCurveInfo;

	ProductTreeRootPtr pProductTreeRoot = g_TestDataTreeMgt.GetProductTreeRootPtr();
	ProductNodePtr pProductNode = g_TestDataTreeMgt.GetProductTreeRootPtr()->pProductNodeListHead->pNext;

	while(pProductNode!=NULL && ci.iNumOfCurve < m_nMaxNumOfCurveToDraw){
		FileNodePtr pFileNode = pProductNode->pFileListHead->pNext;
		while(pFileNode!=NULL && ci.iNumOfCurve < m_nMaxNumOfCurveToDraw){
			int flag_selected = 0;
			for(int iIndex=0;iIndex<pFileNode->test_record_maininfo.iNumOfSpeed;iIndex++){
				struct TreeItemData *tp = pFileNode->tree_item_data + iIndex;
				if(tp->bSelected){
					flag_selected = 1;
					if(tp->pNode->bProcessed==FALSE){
						char sFileName[256];
						TCHAR2char(sFileName, tp->pNode->sFileName, _tcslen(tp->pNode->sFileName));
						g_TestDataTreeMgt.LoadAndProcessFile(sFileName, tp->pNode);
					}

					if(m_iDrawMode & DM_NORMAL_ONLY){
						ci.pCurveInfo[ci.iNumOfCurve].bIsClosed = TRUE;
						ci.pCurveInfo[ci.iNumOfCurve].crColor = color[tp->iIndex];
						ci.pCurveInfo[ci.iNumOfCurve].iNumOfPoint = tp->pNode->pAdditionInfo->iDataBandLen[tp->iIndex];
						ci.pCurveInfo[ci.iNumOfCurve].pXData = &tp->pNode->pAdditionInfo->fSpeedOfFilter[tp->iIndex][tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]];
						ci.pCurveInfo[ci.iNumOfCurve].pYData = &tp->pNode->pTestRecord->fForce[tp->iIndex][tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]];
						ci.pCurveInfo[ci.iNumOfCurve].iWidth = 2;

						ci.iNumOfCurve ++;
						if(ci.iNumOfCurve>=m_nMaxNumOfCurveToDraw){
							break;
						}
					}
					
					if(m_iDrawMode & DM_FILTER_ONLY){
						ci.pCurveInfo[ci.iNumOfCurve].bIsClosed = TRUE;
						ci.pCurveInfo[ci.iNumOfCurve].crColor = color[tp->iIndex];
						ci.pCurveInfo[ci.iNumOfCurve].iNumOfPoint = tp->pNode->pAdditionInfo->iDataBandLen[tp->iIndex];
						ci.pCurveInfo[ci.iNumOfCurve].pXData = &tp->pNode->pAdditionInfo->fSpeedOfFilter[tp->iIndex][tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]];
						ci.pCurveInfo[ci.iNumOfCurve].pYData = &tp->pNode->pAdditionInfo->fForceOfFilter[tp->iIndex][tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]];
						ci.pCurveInfo[ci.iNumOfCurve].iWidth = 2;

						ci.iNumOfCurve ++;
						if(ci.iNumOfCurve>=m_nMaxNumOfCurveToDraw){
							break;
						}
					}
				}
			}
			if(flag_selected==0 && pFileNode->bProcessed){
				delete pFileNode->pMachineInfo;
				pFileNode->pMachineInfo = NULL;
				delete pFileNode->pProductInfo;
				pFileNode->pProductInfo = NULL;
				delete pFileNode->pTestRecord;
				pFileNode->pTestRecord = NULL;
				delete pFileNode->pAdditionInfo;
				pFileNode->pAdditionInfo = NULL;
				pFileNode->bProcessed = FALSE;
			}
			pFileNode = pFileNode->pNext;
		}//END OF WHILE
		pProductNode = pProductNode->pNext;
	}

	ci.pDC = GetDC();
	GetClientRect(&ci.rect);
	sprintf(ci.sCanvasInfo, "%s", "Force vs.Speed");
	sprintf(ci.sXAxisInfo, "%s", "Speed [mm/s]");
	sprintf(ci.sYAxisInfo, "%s", "Force [N]");
	DrawCurve(ci);
}

void CRightDrawAreaView::Draw_MaxForce_Vs_Speed()
{
	COLORREF color[] = {RGB(255,0,0),RGB(0,255,0),RGB(0,0,255),RGB(255,255,0),RGB(255,0,255),RGB(0,255,255),RGB(0,0,0),};
	struct CanvasInfo ci;
	double fMaxPym[MAX_SPEED_NUM];

	memset(&ci, 0, sizeof(struct CanvasInfo));
	ci.pCurveInfo = m_pCurveInfo;

	ProductTreeRootPtr pProductTreeRoot = g_TestDataTreeMgt.GetProductTreeRootPtr();
	ProductNodePtr pProductNode = g_TestDataTreeMgt.GetProductTreeRootPtr()->pProductNodeListHead->pNext;

	while(pProductNode!=NULL && ci.iNumOfCurve < m_nMaxNumOfCurveToDraw){
		FileNodePtr pFileNode = pProductNode->pFileListHead->pNext;
		while(pFileNode!=NULL && ci.iNumOfCurve < m_nMaxNumOfCurveToDraw){
			int flag_selected = 0;
			for(int iIndex=0;iIndex<pFileNode->test_record_maininfo.iNumOfSpeed;iIndex++){
				struct TreeItemData *tp = pFileNode->tree_item_data + iIndex;
				if(tp->bSelected){
					flag_selected = 1;
					if(tp->pNode->bProcessed==FALSE){
						char sFileName[256];
						TCHAR2char(sFileName, tp->pNode->sFileName, _tcslen(tp->pNode->sFileName));
						g_TestDataTreeMgt.LoadAndProcessFile(sFileName, tp->pNode);
					}

					ci.pCurveInfo[ci.iNumOfCurve].bIsClosed = FALSE;
					ci.pCurveInfo[ci.iNumOfCurve].crColor = RGB(0, 0, 255);
					ci.pCurveInfo[ci.iNumOfCurve].iWidth = 2;
					ci.pCurveInfo[ci.iNumOfCurve].iNumOfPoint = tp->pNode->pTestRecord->iNumOfSpeed;
					ci.pCurveInfo[ci.iNumOfCurve].pXData = tp->pNode->pTestRecord->fRealSpeed;
					ci.pCurveInfo[ci.iNumOfCurve].pYData = tp->pNode->pTestRecord->fPfm;

					ci.iNumOfCurve ++;
					if(ci.iNumOfCurve>=m_nMaxNumOfCurveToDraw){
						break;
					}

					ci.pCurveInfo[ci.iNumOfCurve].bIsClosed = FALSE;
					ci.pCurveInfo[ci.iNumOfCurve].crColor = RGB(0, 0, 255);
					ci.pCurveInfo[ci.iNumOfCurve].iWidth = 2;
					ci.pCurveInfo[ci.iNumOfCurve].iNumOfPoint = tp->pNode->pTestRecord->iNumOfSpeed;
					ci.pCurveInfo[ci.iNumOfCurve].pXData = tp->pNode->pTestRecord->fRealSpeed;
					for(int k=0;k<ci.pCurveInfo[ci.iNumOfCurve].iNumOfPoint;k++){
						fMaxPym[k] = -tp->pNode->pTestRecord->fPym[k];
					}
					ci.pCurveInfo[ci.iNumOfCurve].pYData = fMaxPym;

					ci.iNumOfCurve ++;
					if(ci.iNumOfCurve>=m_nMaxNumOfCurveToDraw){
						break;
					}
					break;
				}
			}
			if(flag_selected==0 && pFileNode->bProcessed){
				delete pFileNode->pMachineInfo;
				pFileNode->pMachineInfo = NULL;
				delete pFileNode->pProductInfo;
				pFileNode->pProductInfo = NULL;
				delete pFileNode->pTestRecord;
				pFileNode->pTestRecord = NULL;
				delete pFileNode->pAdditionInfo;
				pFileNode->pAdditionInfo = NULL;
				pFileNode->bProcessed = FALSE;
			}
			pFileNode = pFileNode->pNext;
		}//END OF WHILE
		pProductNode = pProductNode->pNext;
	}

	ci.pDC = GetDC();
	GetClientRect(&ci.rect);
	sprintf(ci.sCanvasInfo, "%s", "MaxForce vs.Speed");
	sprintf(ci.sXAxisInfo, "%s", "Speed [mm/s]");
	sprintf(ci.sYAxisInfo, "%s", "MaxForce [N]");
	DrawCurve(ci);
}

void CRightDrawAreaView::Draw_Force_Vs_Time()
{
	COLORREF color[] = {RGB(255,0,0),RGB(0,255,0),RGB(0,0,255),RGB(255,255,0),RGB(255,0,255),RGB(0,255,255),RGB(0,0,0),};
	struct CanvasInfo ci;

	memset(&ci, 0, sizeof(struct CanvasInfo));
	ci.pCurveInfo = m_pCurveInfo;

	ProductTreeRootPtr pProductTreeRoot = g_TestDataTreeMgt.GetProductTreeRootPtr();
	ProductNodePtr pProductNode = g_TestDataTreeMgt.GetProductTreeRootPtr()->pProductNodeListHead->pNext;

	while(pProductNode!=NULL && ci.iNumOfCurve < m_nMaxNumOfCurveToDraw){
		FileNodePtr pFileNode = pProductNode->pFileListHead->pNext;
		while(pFileNode!=NULL && ci.iNumOfCurve < m_nMaxNumOfCurveToDraw){
			int flag_selected = 0;
			for(int iIndex=0;iIndex<pFileNode->test_record_maininfo.iNumOfSpeed;iIndex++){
				struct TreeItemData *tp = pFileNode->tree_item_data + iIndex;
				if(tp->bSelected){
					flag_selected = 1;
					if(tp->pNode->bProcessed==FALSE){
						char sFileName[256];
						TCHAR2char(sFileName, tp->pNode->sFileName, _tcslen(tp->pNode->sFileName));
						g_TestDataTreeMgt.LoadAndProcessFile(sFileName, tp->pNode);
					}

					if(m_iDrawMode & DM_NORMAL_ONLY){
						ci.pCurveInfo[ci.iNumOfCurve].bIsClosed = FALSE;
						ci.pCurveInfo[ci.iNumOfCurve].crColor = color[tp->iIndex];
						ci.pCurveInfo[ci.iNumOfCurve].iNumOfPoint = tp->pNode->pAdditionInfo->iDataBandLen[tp->iIndex];
						ci.pCurveInfo[ci.iNumOfCurve].pXData = &tp->pNode->pAdditionInfo->fTime[tp->iIndex][tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]];
						ci.pCurveInfo[ci.iNumOfCurve].pYData = &tp->pNode->pTestRecord->fForce[tp->iIndex][tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]];
						ci.pCurveInfo[ci.iNumOfCurve].iWidth = 2;

						ci.iNumOfCurve ++;
						if(ci.iNumOfCurve>=m_nMaxNumOfCurveToDraw){
							break;
						}
					}
					
					if(m_iDrawMode & DM_FILTER_ONLY){
						ci.pCurveInfo[ci.iNumOfCurve].bIsClosed = FALSE;
						ci.pCurveInfo[ci.iNumOfCurve].crColor = color[tp->iIndex];
						ci.pCurveInfo[ci.iNumOfCurve].iNumOfPoint = tp->pNode->pAdditionInfo->iDataBandLen[tp->iIndex];
						ci.pCurveInfo[ci.iNumOfCurve].pXData = &tp->pNode->pAdditionInfo->fTime[tp->iIndex][tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]];
						ci.pCurveInfo[ci.iNumOfCurve].pYData = &tp->pNode->pAdditionInfo->fForceOfFilter[tp->iIndex][tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]];
						ci.pCurveInfo[ci.iNumOfCurve].iWidth = 2;

						ci.iNumOfCurve ++;
						if(ci.iNumOfCurve>=m_nMaxNumOfCurveToDraw){
							break;
						}
					}
				}
			}
			if(flag_selected==0 && pFileNode->bProcessed){
				delete pFileNode->pMachineInfo;
				pFileNode->pMachineInfo = NULL;
				delete pFileNode->pProductInfo;
				pFileNode->pProductInfo = NULL;
				delete pFileNode->pTestRecord;
				pFileNode->pTestRecord = NULL;
				delete pFileNode->pAdditionInfo;
				pFileNode->pAdditionInfo = NULL;
				pFileNode->bProcessed = FALSE;
			}
			pFileNode = pFileNode->pNext;
		}//END OF WHILE
		pProductNode = pProductNode->pNext;
	}

	ci.pDC = GetDC();
	GetClientRect(&ci.rect);
	sprintf(ci.sCanvasInfo, "%s", "Force vs.Time");
	sprintf(ci.sXAxisInfo, "%s", "Time [s]");
	sprintf(ci.sYAxisInfo, "%s", "Force [N]");
	DrawCurve(ci);
}

void CRightDrawAreaView::Draw_Friction_Force_Vs_Position()
{
	COLORREF color[] = {RGB(255,0,0),RGB(0,255,0),RGB(0,0,255),RGB(255,255,0),RGB(255,0,255),RGB(0,255,255),RGB(0,0,0),};
	struct CanvasInfo ci;

	memset(&ci, 0, sizeof(struct CanvasInfo));
	ci.pCurveInfo = m_pCurveInfo;

	ProductTreeRootPtr pProductTreeRoot = g_TestDataTreeMgt.GetProductTreeRootPtr();
	ProductNodePtr pProductNode = g_TestDataTreeMgt.GetProductTreeRootPtr()->pProductNodeListHead->pNext;

	while(pProductNode!=NULL && ci.iNumOfCurve < m_nMaxNumOfCurveToDraw){
		FileNodePtr pFileNode = pProductNode->pFileListHead->pNext;
		while(pFileNode!=NULL && ci.iNumOfCurve < m_nMaxNumOfCurveToDraw){
			int flag_selected = 0;
			for(int iIndex=0;iIndex<pFileNode->test_record_maininfo.iNumOfSpeed;iIndex++){
				struct TreeItemData *tp = pFileNode->tree_item_data + iIndex;
				if(tp->bSelected){
					flag_selected = 1;
					if(tp->pNode->bProcessed==FALSE){
						char sFileName[256];
						TCHAR2char(sFileName, tp->pNode->sFileName, _tcslen(tp->pNode->sFileName));
						g_TestDataTreeMgt.LoadAndProcessFile(sFileName, tp->pNode);
					}

					if(tp->pNode->pTestRecord->bFrictionSpeed){
						if(m_iDrawMode & DM_NORMAL_ONLY){
							ci.pCurveInfo[ci.iNumOfCurve].bIsClosed = TRUE;
							ci.pCurveInfo[ci.iNumOfCurve].crColor = color[tp->iIndex];
							ci.pCurveInfo[ci.iNumOfCurve].iNumOfPoint = tp->pNode->pAdditionInfo->iFrictionDataBandLen;
							ci.pCurveInfo[ci.iNumOfCurve].pXData = &tp->pNode->pTestRecord->fFrictionDisplacement[tp->pNode->pAdditionInfo->iFrictionDataBandStart];
							ci.pCurveInfo[ci.iNumOfCurve].pYData = &tp->pNode->pTestRecord->fFrictionForce[tp->pNode->pAdditionInfo->iFrictionDataBandStart];
							ci.pCurveInfo[ci.iNumOfCurve].iWidth = 2;

							ci.iNumOfCurve ++;
							if(ci.iNumOfCurve>=m_nMaxNumOfCurveToDraw){
								break;
							}
						}
						
						if(m_iDrawMode & DM_FILTER_ONLY){
							ci.pCurveInfo[ci.iNumOfCurve].bIsClosed = TRUE;
							ci.pCurveInfo[ci.iNumOfCurve].crColor = color[tp->iIndex];
							ci.pCurveInfo[ci.iNumOfCurve].iNumOfPoint = tp->pNode->pAdditionInfo->iFrictionDataBandLen;
							ci.pCurveInfo[ci.iNumOfCurve].pXData = &tp->pNode->pTestRecord->fFrictionDisplacement[tp->pNode->pAdditionInfo->iFrictionDataBandStart];
							ci.pCurveInfo[ci.iNumOfCurve].pYData = &tp->pNode->pAdditionInfo->fFrictionForceOfFilter[tp->pNode->pAdditionInfo->iFrictionDataBandStart];
							ci.pCurveInfo[ci.iNumOfCurve].iWidth = 2;

							ci.iNumOfCurve ++;
							if(ci.iNumOfCurve>=m_nMaxNumOfCurveToDraw){
								break;
							}
						}
					}
					break;
				}
			}
			if(flag_selected==0 && pFileNode->bProcessed){
				delete pFileNode->pMachineInfo;
				pFileNode->pMachineInfo = NULL;
				delete pFileNode->pProductInfo;
				pFileNode->pProductInfo = NULL;
				delete pFileNode->pTestRecord;
				pFileNode->pTestRecord = NULL;
				delete pFileNode->pAdditionInfo;
				pFileNode->pAdditionInfo = NULL;
				pFileNode->bProcessed = FALSE;
			}
			pFileNode = pFileNode->pNext;
		}//END OF WHILE
		pProductNode = pProductNode->pNext;
	}

	ci.pDC = GetDC();
	GetClientRect(&ci.rect);
	sprintf(ci.sCanvasInfo, "%s", "Force vs.Position");
	sprintf(ci.sXAxisInfo, "%s", "Position [mm]");
	sprintf(ci.sYAxisInfo, "%s", "Force [N]");
	DrawCurve(ci);
}

// CRightDrawAreaView 消息处理程序

void CRightDrawAreaView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	m_pTab->InsertItem(0, _T("  F vs P  "));
	m_pTab->InsertItem(1, _T("  F vs S  "));
	m_pTab->InsertItem(2, _T("  F vs PS  "));
	m_pTab->InsertItem(3, _T("  MaxF vs S  "));
	m_pTab->InsertItem(4, _T("  F vs T  "));
	m_pTab->InsertItem(5, _T("  FF vs P  "));
}

int CRightDrawAreaView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pTab = new CLBTabCtrl();
	ASSERT(m_pTab);

	// note:  TVS_NOTOOLTIPS is set in CXHtmlTree::PreCreateWindow()

	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP;// | WS_BORDER;

	CRect rect(0,0,100,100);

	VERIFY(m_pTab->Create(dwStyle, rect, this, IDC_DRAW_TAB));


	return 0;
}

void CRightDrawAreaView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (m_pTab && ::IsWindow(m_pTab->m_hWnd))
	{
		// stretch tree to fill window
		m_pTab->MoveWindow(0, 0, cx, 22);
	}
}

void CRightDrawAreaView::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	this->RedrawWindow();
}
