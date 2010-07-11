// RightDrawAreaView.cpp : CRightDrawAreaView 类的实现
//

#include "stdafx.h"
#include "AbsTestDataMan.h"

#include "AbsTestDataManDoc.h"
#include "RightDrawAreaView.h"
#include "GlobalVars.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRightDrawAreaView

IMPLEMENT_DYNCREATE(CRightDrawAreaView, CView)

BEGIN_MESSAGE_MAP(CRightDrawAreaView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()


// CRightDrawAreaView 构造/析构

CRightDrawAreaView::CRightDrawAreaView()
{
	//初始化值
	m_pntClientAreaRef.x = 0;
	m_pntClientAreaRef.y = 0;
	m_nClientAreaWidth = 0;
	m_nClientAreaHeight = 0;

	g_pRightDrawAreaView = this;
}

CRightDrawAreaView::~CRightDrawAreaView()
{
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
	DrawData();
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

void CRightDrawAreaView::DrawData()
{
	RECT rect;
	CPen pen;
	CBrush brush;
	int i, j, x, y;
	char buf[100];
	struct CCTestRecord *pRec = NULL;
	CFont fontLogo,*oldFont;

	GetClientRect(&rect);
	if(rect.bottom<100 || rect.right<100){
		return;
	}
	//获取待画图区域大小位置
	m_pntClientAreaRef.x = rect.left + 50;
	m_pntClientAreaRef.y = rect.bottom - 50;
	m_nClientAreaWidth = rect.right - rect.left - 100;
	m_nClientAreaHeight = rect.bottom - rect.top - 100;

	CDC *pDC=GetDC();

	pen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	pDC->SelectObject(&pen);
	//画横轴
	pDC->MoveTo(m_pntClientAreaRef);
	pDC->LineTo(m_pntClientAreaRef.x + m_nClientAreaWidth, m_pntClientAreaRef.y);
	//画纵轴
	pDC->MoveTo(m_pntClientAreaRef);
	pDC->LineTo(m_pntClientAreaRef.x, m_pntClientAreaRef.y - m_nClientAreaHeight);

	//画轴坐标及文字标识
	double xspan = m_nClientAreaWidth / 11.0;
	double yspan= m_nClientAreaHeight / 5.5;
	for(i=0;i<11;i++){
		pDC->MoveTo(m_pntClientAreaRef.x + (int)(xspan/2) + (int)(xspan*i), m_pntClientAreaRef.y);
		pDC->LineTo(m_pntClientAreaRef.x + (int)(xspan/2) + (int)(xspan*i), m_pntClientAreaRef.y + 5);

		sprintf_s(buf, "%d", (i-5)*10);
		rect.left = m_pntClientAreaRef.x + (int)(xspan/2) + (int)(xspan*i) - 25;
		rect.right = m_pntClientAreaRef.x + (int)(xspan/2) + (int)(xspan*i) + 25;
		rect.top = m_pntClientAreaRef.y;
		rect.bottom = m_pntClientAreaRef.y + 25;
		pDC->DrawText(CString(buf), &rect, DT_SINGLELINE | DT_VCENTER| DT_CENTER);
	}
	for(i=0;i<6;i++){
		pDC->MoveTo(m_pntClientAreaRef.x, m_pntClientAreaRef.y - m_nClientAreaHeight + (int)(yspan*i));
		pDC->LineTo(m_pntClientAreaRef.x - 5, m_pntClientAreaRef.y - m_nClientAreaHeight + (int)(yspan*i));

		sprintf_s(buf, "%d", (4-i)*250);
		rect.left = m_pntClientAreaRef.x - 5 - 40;
		rect.right = m_pntClientAreaRef.x - 5;
		rect.top = m_pntClientAreaRef.y - m_nClientAreaHeight + (int)(yspan*i) - 10;
		rect.bottom = m_pntClientAreaRef.y - m_nClientAreaHeight + (int)(yspan*i) + 10;
		pDC->DrawText(CString(buf), &rect, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
	}
	//原点位置
	m_pntClientAreaOrig.x = m_pntClientAreaRef.x + (int)(xspan*5.5);
	m_pntClientAreaOrig.y = m_pntClientAreaRef.y - m_nClientAreaHeight + (int)(yspan*4);

	pen.DeleteObject();

	//画纵横虚线
	LOGBRUSH logbrush;
	logbrush.lbColor = RGB(192,192,192); 
	logbrush.lbStyle = PS_SOLID;
	pen.CreatePen(PS_ALTERNATE, 1, &logbrush);
	pDC->SelectObject(&pen);
	for(i=0;i<11;i++){
		pDC->MoveTo(m_pntClientAreaRef.x + (int)(xspan/2) + (int)(xspan*i), m_pntClientAreaRef.y -2);
		pDC->LineTo(m_pntClientAreaRef.x + (int)(xspan/2) + (int)(xspan*i), m_pntClientAreaRef.y - m_nClientAreaHeight + 2);
	}
	for(i=0;i<6;i++){
		pDC->MoveTo(m_pntClientAreaRef.x + 2, m_pntClientAreaRef.y - m_nClientAreaHeight + (int)(yspan*i));
		pDC->LineTo(m_pntClientAreaRef.x + m_nClientAreaWidth - 2, m_pntClientAreaRef.y - m_nClientAreaHeight + (int)(yspan*i));
	}
	pen.DeleteObject();

	//标识原点
	pDC->SetPixel(m_pntClientAreaOrig, RGB(255, 0, 0));

	//画标识
	pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	rect.left = m_pntClientAreaRef.x + m_nClientAreaWidth/2 - 120;
	rect.right = m_pntClientAreaRef.x + m_nClientAreaWidth/2 + 30;
	rect.top = m_pntClientAreaRef.y - m_nClientAreaHeight - 20;
	rect.bottom = m_pntClientAreaRef.y - m_nClientAreaHeight;
	fontLogo.CreateFont(20,10,0,0,0,FALSE,FALSE,FALSE,
						DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
						FIXED_PITCH | FF_ROMAN, _T("System"));
	pDC->SetBkMode(TRANSPARENT);
	oldFont=pDC->SelectObject(&fontLogo);
	pDC->DrawText(_T("Force vs.Position"), &rect, DT_SINGLELINE | DT_LEFT);

	rect.left = m_pntClientAreaRef.x + m_nClientAreaWidth/2 - 50;
	rect.right = m_pntClientAreaRef.x + m_nClientAreaWidth/2 + 100;
	rect.top = m_pntClientAreaRef.y + 20;
	rect.bottom = m_pntClientAreaRef.y + 40;
	fontLogo.DeleteObject();
	fontLogo.CreateFont(16,8,0,0,0,FALSE,FALSE,FALSE,
						DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
						FIXED_PITCH | FF_ROMAN,_T("System"));
	pDC->SetBkMode(TRANSPARENT);
	oldFont=pDC->SelectObject(&fontLogo);
	pDC->DrawText(_T("Position [mm]"), &rect, DT_SINGLELINE | DT_LEFT );

	rect.left = m_pntClientAreaRef.x - 48;
	rect.right = m_pntClientAreaRef.x - 28;
	rect.top = m_pntClientAreaRef.y - m_nClientAreaHeight/2;
	rect.bottom = m_pntClientAreaRef.y - m_nClientAreaHeight/2 - 100;
	fontLogo.DeleteObject();
	fontLogo.CreateFont(16, 0, 900, 900, 0, 0, 0, 0,
						DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
						DEFAULT_PITCH, _T("System"));
	pDC->SetBkMode(TRANSPARENT);
	oldFont=pDC->SelectObject(&fontLogo);
	pDC->DrawText(_T("Force [N]"), &rect, DT_SINGLELINE | DT_LEFT );

	if(g_pTree==NULL){
		return;
	}

	int iNumDrawed = 0;
	COLORREF color[] = {RGB(255,0,0),RGB(0,255,0),RGB(0,0,255),RGB(255,255,0),RGB(255,0,255),RGB(0,255,255),RGB(0,0,0),};
	HTREEITEM hItem = g_pTree->GetFirstCheckedItem();
	while (hItem)
	{
		DWORD_PTR p = g_pTree->GetItemData(hItem);

		hItem = g_pTree->GetNextCheckedItem(hItem);

		if(p==NULL){
			continue;
		}
		struct TreeItemData *tp = (struct TreeItemData *)p;
		if(tp->pNode->bDataProcessed==FALSE){
			g_TestDataTreeMgt.ProcessData(tp->pNode);
		}

		pen.DeleteObject();
		pen.CreatePen(PS_SOLID, 2, color[tp->iIndex]);
		pDC->SelectObject(&pen);
		for(j=tp->pNode->addition_info.iDataBandStart[tp->iIndex];
			j<tp->pNode->addition_info.iDataBandStart[tp->iIndex]+tp->pNode->addition_info.iDataBandLen[tp->iIndex];
			j++){
			//X按均值和幅度缩放到对应值
			x = (int)(tp->pNode->test_record.fDisplacement[tp->iIndex][j] * 1000 * (xspan/10) + m_pntClientAreaOrig.x);
			//Y直接使用原大小
			y = (int)(m_pntClientAreaOrig.y - tp->pNode->test_record.fForce[tp->iIndex][j] * (yspan/250));
			if(j==tp->pNode->addition_info.iDataBandStart[tp->iIndex]){
				pDC->MoveTo(x, y);
				pDC->SetPixel(x, y, color[tp->iIndex]);
			}
			else{
				pDC->LineTo(x, y);
			}
		}

		for(j=tp->pNode->addition_info.iDataBandStart[tp->iIndex];
			j<tp->pNode->addition_info.iDataBandStart[tp->iIndex]+tp->pNode->addition_info.iDataBandLen[tp->iIndex];
			j++){
			//X按均值和幅度缩放到对应值
			x = (int)(tp->pNode->test_record.fDisplacement[tp->iIndex][j] * 1000 * (xspan/10) + m_pntClientAreaOrig.x);
			//Y直接使用原大小
			y = (int)(m_pntClientAreaOrig.y - tp->pNode->addition_info.fForceOfFilter[tp->iIndex][j] * (yspan/250));
			if(j==tp->pNode->addition_info.iDataBandStart[tp->iIndex]){
				pDC->MoveTo(x, y);
				pDC->SetPixel(x, y, color[tp->iIndex]);
			}
			else{
				pDC->LineTo(x, y);
			}
		}

		/*
		for(j=tp->pNode->addition_info.iDataBandStart[tp->iIndex];
			j<tp->pNode->addition_info.iDataBandStart[tp->iIndex]+tp->pNode->addition_info.iDataBandLen[tp->iIndex];
			j++){
			//X按均值和幅度缩放到对应值
			double tmp;
			tmp = (j-tp->pNode->addition_info.iDataBandStart[tp->iIndex]-tp->pNode->addition_info.iDataBandLen[tp->iIndex]/2);
			//tmp /= tp->pNode->test_record.fDataFreq[tp->iIndex];
			tmp *= xspan / 100;
			tmp += m_pntClientAreaOrig.x;
			x = (int)tmp;
			//x = (int)((j-tp->pNode->addition_info.iDataBandStart[tp->iIndex])  * (xspan/100) / tp->pNode->test_record.fDataFreq[tp->iIndex] + m_pntClientAreaOrig.x);
			//Y直接使用原大小
			y = (int)(m_pntClientAreaOrig.y - tp->pNode->addition_info.fForceOfFilter[tp->iIndex][j] * (yspan/250));
			if(j==tp->pNode->addition_info.iDataBandStart[tp->iIndex]){
				pDC->MoveTo(x, y);
				pDC->SetPixel(x, y, color[tp->iIndex]);
			}
			else{
				pDC->LineTo(x, y);
			}
			if(tp->pNode->test_record.fForce[tp->iIndex][300+j]>1000){
				AfxMessageBox("Hello World!");
			}
		}
		*/

		iNumDrawed ++;
		if(iNumDrawed>=20){
			break;
		}
	}

	pDC->SelectObject(oldFont);
}


// CRightDrawAreaView 消息处理程序
