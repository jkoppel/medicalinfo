// RightDrawAreaView.cpp : CRightDrawAreaView ���ʵ��
//

#include "stdafx.h"
#include "AbsTestDataMan.h"

#include "AbsTestDataManDoc.h"
#include "RightDrawAreaView.h"
#include "GlobalVars.h"
#include "GlobalFuncs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRightDrawAreaView

IMPLEMENT_DYNCREATE(CRightDrawAreaView, CView)

BEGIN_MESSAGE_MAP(CRightDrawAreaView, CView)
	// ��׼��ӡ����
	ON_NOTIFY(TCN_SELCHANGE, IDC_DRAW_TAB, &CRightDrawAreaView::OnTcnSelchangeTab)
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CRightDrawAreaView ����/����

CRightDrawAreaView::CRightDrawAreaView()
{
	//��ʼ��ֵ
	m_iDrawMode = DM_FILTER_ONLY;
	m_pntClientAreaRef.x = 0;
	m_pntClientAreaRef.y = 0;
	m_nClientAreaWidth = 0;
	m_nClientAreaHeight = 0;

	m_pTab = NULL;

	g_pRightDrawAreaView = this;
}

CRightDrawAreaView::~CRightDrawAreaView()
{
	if(m_pTab){
		delete m_pTab;
	}
}

BOOL CRightDrawAreaView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CRightDrawAreaView ����

void CRightDrawAreaView::OnDraw(CDC* /*pDC*/)
{
	CAbsTestDataManDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	//��ͼ
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

// CRightDrawAreaView ��ӡ

BOOL CRightDrawAreaView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CRightDrawAreaView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CRightDrawAreaView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}


// CRightDrawAreaView ���

#ifdef _DEBUG
void CRightDrawAreaView::AssertValid() const
{
	CView::AssertValid();
}

void CRightDrawAreaView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAbsTestDataManDoc* CRightDrawAreaView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAbsTestDataManDoc)));
	return (CAbsTestDataManDoc*)m_pDocument;
}
#endif //_DEBUG

#if 0
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
	//��ȡ����ͼ�����Сλ��
	m_pntClientAreaRef.x = rect.left + 50;
	m_pntClientAreaRef.y = rect.bottom - 50;
	m_nClientAreaWidth = rect.right - rect.left - 100;
	m_nClientAreaHeight = rect.bottom - rect.top - 100;

	CDC *pDC=GetDC();

	pen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	pDC->SelectObject(&pen);
	//������
	pDC->MoveTo(m_pntClientAreaRef);
	pDC->LineTo(m_pntClientAreaRef.x + m_nClientAreaWidth, m_pntClientAreaRef.y);
	//������
	pDC->MoveTo(m_pntClientAreaRef);
	pDC->LineTo(m_pntClientAreaRef.x, m_pntClientAreaRef.y - m_nClientAreaHeight);

	//�������꼰���ֱ�ʶ
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
	//ԭ��λ��
	m_pntClientAreaOrig.x = m_pntClientAreaRef.x + (int)(xspan*5.5);
	m_pntClientAreaOrig.y = m_pntClientAreaRef.y - m_nClientAreaHeight + (int)(yspan*4);

	pen.DeleteObject();

	//���ݺ�����
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

	//��ʶԭ��
	pDC->SetPixel(m_pntClientAreaOrig, RGB(255, 0, 0));

	//����ʶ
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
	ProductTreeRootPtr pProductTreeRoot = g_TestDataTreeMgt.m_pProductTreeRoot;
	ProductNodePtr pProductNode = g_TestDataTreeMgt.m_pProductTreeRoot->pProductNodeListHead->pNext;
	while(pProductNode!=NULL){
		FileNodePtr pFileNode = pProductNode->pFileListHead->pNext;
		while(pFileNode!=NULL){
			int flag_selected = 0;
			for(int iIndex=0;iIndex<pFileNode->test_record_header.iNumOfSpeed;iIndex++){
				struct TreeItemData *tp = pFileNode->tree_item_data + iIndex;
				if(tp->bSelected){
					flag_selected = 1;
					if(tp->pNode->bProcessed==FALSE){
						char sFileName[256];
						TCHAR2char(sFileName, tp->pNode->sFileName, _tcslen(tp->pNode->sFileName));
						g_TestDataTreeMgt.LoadAndProcessFile(sFileName, tp->pNode);
					}

					pen.DeleteObject();
					pen.CreatePen(PS_SOLID, 2, color[tp->iIndex]);
					pDC->SelectObject(&pen);

					if(m_iDrawMode & DM_NORMAL_ONLY){
						int x_start, y_start;
						for(j=tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex];j<tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]+tp->pNode->pAdditionInfo->iDataBandLen[tp->iIndex];j++){
							//X����ֵ�ͷ������ŵ���Ӧֵ
							x = (int)(tp->pNode->pTestRecord->fDisplacement[tp->iIndex][j] * 1000 * (xspan/10) + m_pntClientAreaOrig.x);
							//Yֱ��ʹ��ԭ��С
							y = (int)(m_pntClientAreaOrig.y - tp->pNode->pTestRecord->fForce[tp->iIndex][j] * (yspan/250));
							if(j==tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]){
								pDC->MoveTo(x, y);
								x_start = x;
								y_start = y;
								pDC->SetPixel(x, y, color[tp->iIndex]);
							}
							else{
								pDC->LineTo(x, y);
							}
						}
						pDC->LineTo(x_start, y_start);
					}

					if(m_iDrawMode & DM_FILTER_ONLY){
						int x_start, y_start; 
						for(j=tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex];j<tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]+tp->pNode->pAdditionInfo->iDataBandLen[tp->iIndex];j++){
							//X����ֵ�ͷ������ŵ���Ӧֵ
							x = (int)(tp->pNode->pTestRecord->fDisplacement[tp->iIndex][j] * 1000 * (xspan/10) + m_pntClientAreaOrig.x);
							//Yֱ��ʹ��ԭ��С
							y = (int)(m_pntClientAreaOrig.y - tp->pNode->pAdditionInfo->fForceOfFilter[tp->iIndex][j] * (yspan/250));
							if(j==tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]){
								pDC->MoveTo(x, y);
								x_start = x;
								y_start = y;
								pDC->SetPixel(x, y, color[tp->iIndex]);
							}
							else{
								pDC->LineTo(x, y);
							}
						}
						pDC->LineTo(x_start, y_start);
					}

					//��-�ٶ�����
					double *pStart = NULL;
					for(j=tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex];j<tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]+tp->pNode->pAdditionInfo->iDataBandLen[tp->iIndex];j++){
						/*
						double tmp;
						pStart = &(tp->pNode->pTestRecord->fDisplacement[tp->iIndex][j]);
						if(j<=tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]+tp->pNode->pAdditionInfo->iDataBandLen[tp->iIndex]-3){
							tmp = ( -3 * (*pStart) + 4 * (*(pStart+1)) - 1 * (*(pStart+2)) ) / 2 * tp->pNode->pTestRecord->fDataFreq[tp->iIndex];
						}
						else if(j==tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]+tp->pNode->pAdditionInfo->iDataBandLen[tp->iIndex]-2){
							tmp = ( -3 * (*pStart) + 4 * (*(pStart+1)) - 1 * (tp->pNode->pTestRecord->fDisplacement[tp->iIndex][tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]]) ) / 2 / tp->pNode->pTestRecord->fDataFreq[tp->iIndex];
						}
						else{
							tmp = ( -3 * (*pStart) + 4 * (tp->pNode->pTestRecord->fDisplacement[tp->iIndex][tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]]) - 1 * (tp->pNode->pTestRecord->fDisplacement[tp->iIndex][tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]+1]) ) / 2 / tp->pNode->pTestRecord->fDataFreq[tp->iIndex];
						}
						*///��������ֵ
						//����20������ƽ���ķ���
						double tmp = 0;
						pStart = &(tp->pNode->pTestRecord->fDisplacement[tp->iIndex][j]);
						for(int k=0;k<10;k++){
							tmp += *(pStart+k+10) - *(pStart+k);
						}
						tmp = tmp / 100 * tp->pNode->pTestRecord->fDataFreq[tp->iIndex];
							
						//X����ֵ�ͷ������ŵ���Ӧֵ
						tmp = tmp * 100 * xspan / 10;
						tmp += m_pntClientAreaOrig.x;
						x = (int)tmp;
						//Yֱ��ʹ��ԭ��С
						y = (int)(m_pntClientAreaOrig.y - tp->pNode->pTestRecord->fForce[tp->iIndex][j] * (yspan/250));
						if(j==tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]){
							pDC->MoveTo(x, y);
							pDC->SetPixel(x, y, color[tp->iIndex]);
						}
						else{
							pDC->LineTo(x, y);
						}
					}

					//��-ʱ������
					for(j=tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex];j<tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]+tp->pNode->pAdditionInfo->iDataBandLen[tp->iIndex];j++){
						//X����ֵ�ͷ������ŵ���Ӧֵ
						double tmp;
						tmp = j-tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex];
						tmp /= tp->pNode->pTestRecord->fDataFreq[tp->iIndex];
						tmp *= xspan * 5;
						tmp += m_pntClientAreaOrig.x;
						x = (int)tmp;
						//Yֱ��ʹ��ԭ��С
						y = (int)(m_pntClientAreaOrig.y - tp->pNode->pAdditionInfo->fForceOfFilter[tp->iIndex][j] * (yspan/250));
						if(j==tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]){
							pDC->MoveTo(x, y);
							pDC->SetPixel(x, y, color[tp->iIndex]);
						}
						else{
							pDC->LineTo(x, y);
						}
					}

					iNumDrawed ++;
					if(iNumDrawed>=20){
						break;
					}
				}//END OF IF
			}//END OF FOR
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

	pDC->SelectObject(oldFont);
}
#endif

void CRightDrawAreaView::Draw_Force_Vs_Position()
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
	//��ȡ����ͼ�����Сλ��
	m_pntClientAreaRef.x = rect.left + 50;
	m_pntClientAreaRef.y = rect.bottom - 50;
	m_nClientAreaWidth = rect.right - rect.left - 100;
	m_nClientAreaHeight = rect.bottom - rect.top - 100;

	CDC *pDC=GetDC();

	pen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	pDC->SelectObject(&pen);
	//������
	pDC->MoveTo(m_pntClientAreaRef);
	pDC->LineTo(m_pntClientAreaRef.x + m_nClientAreaWidth, m_pntClientAreaRef.y);
	//������
	pDC->MoveTo(m_pntClientAreaRef);
	pDC->LineTo(m_pntClientAreaRef.x, m_pntClientAreaRef.y - m_nClientAreaHeight);

	//�������꼰���ֱ�ʶ
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
	//ԭ��λ��
	m_pntClientAreaOrig.x = m_pntClientAreaRef.x + (int)(xspan*5.5);
	m_pntClientAreaOrig.y = m_pntClientAreaRef.y - m_nClientAreaHeight + (int)(yspan*4);

	pen.DeleteObject();

	//���ݺ�����
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

	//��ʶԭ��
	pDC->SetPixel(m_pntClientAreaOrig, RGB(255, 0, 0));

	//����ʶ
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
	ProductTreeRootPtr pProductTreeRoot = g_TestDataTreeMgt.m_pProductTreeRoot;
	ProductNodePtr pProductNode = g_TestDataTreeMgt.m_pProductTreeRoot->pProductNodeListHead->pNext;
	while(pProductNode!=NULL){
		FileNodePtr pFileNode = pProductNode->pFileListHead->pNext;
		while(pFileNode!=NULL){
			int flag_selected = 0;
			for(int iIndex=0;iIndex<pFileNode->test_record_header.iNumOfSpeed;iIndex++){
				struct TreeItemData *tp = pFileNode->tree_item_data + iIndex;
				if(tp->bSelected){
					flag_selected = 1;
					if(tp->pNode->bProcessed==FALSE){
						char sFileName[256];
						TCHAR2char(sFileName, tp->pNode->sFileName, _tcslen(tp->pNode->sFileName));
						g_TestDataTreeMgt.LoadAndProcessFile(sFileName, tp->pNode);
					}

					pen.DeleteObject();
					pen.CreatePen(PS_SOLID, 2, color[tp->iIndex]);
					pDC->SelectObject(&pen);

					if(m_iDrawMode & DM_NORMAL_ONLY){
						int x_start, y_start;
						for(j=tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex];j<tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]+tp->pNode->pAdditionInfo->iDataBandLen[tp->iIndex];j++){
							//X����ֵ�ͷ������ŵ���Ӧֵ
							x = (int)(tp->pNode->pTestRecord->fDisplacement[tp->iIndex][j] * 1000 * (xspan/10) + m_pntClientAreaOrig.x);
							//Yֱ��ʹ��ԭ��С
							y = (int)(m_pntClientAreaOrig.y - tp->pNode->pTestRecord->fForce[tp->iIndex][j] * (yspan/250));
							if(j==tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]){
								pDC->MoveTo(x, y);
								x_start = x;
								y_start = y;
								pDC->SetPixel(x, y, color[tp->iIndex]);
							}
							else{
								pDC->LineTo(x, y);
							}
						}
						pDC->LineTo(x_start, y_start);
					}

					if(m_iDrawMode & DM_FILTER_ONLY){
						int x_start, y_start; 
						for(j=tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex];j<tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]+tp->pNode->pAdditionInfo->iDataBandLen[tp->iIndex];j++){
							//X����ֵ�ͷ������ŵ���Ӧֵ
							x = (int)(tp->pNode->pTestRecord->fDisplacement[tp->iIndex][j] * 1000 * (xspan/10) + m_pntClientAreaOrig.x);
							//Yֱ��ʹ��ԭ��С
							y = (int)(m_pntClientAreaOrig.y - tp->pNode->pAdditionInfo->fForceOfFilter[tp->iIndex][j] * (yspan/250));
							if(j==tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]){
								pDC->MoveTo(x, y);
								x_start = x;
								y_start = y;
								pDC->SetPixel(x, y, color[tp->iIndex]);
							}
							else{
								pDC->LineTo(x, y);
							}
						}
						pDC->LineTo(x_start, y_start);
					}

					iNumDrawed ++;
					if(iNumDrawed>=20){
						break;
					}
				}//END OF IF
			}//END OF FOR
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

	pDC->SelectObject(oldFont);
}

void CRightDrawAreaView::Draw_Force_Vs_PositionAndSpeed()
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
	//��ȡ����ͼ�����Сλ��
	m_pntClientAreaRef.x = rect.left + 50;
	m_pntClientAreaRef.y = rect.bottom - 50;
	m_nClientAreaWidth = rect.right - rect.left - 100;
	m_nClientAreaHeight = rect.bottom - rect.top - 100;

	CDC *pDC=GetDC();

	pen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	pDC->SelectObject(&pen);
	//������
	pDC->MoveTo(m_pntClientAreaRef);
	pDC->LineTo(m_pntClientAreaRef.x + m_nClientAreaWidth, m_pntClientAreaRef.y);
	//������
	pDC->MoveTo(m_pntClientAreaRef);
	pDC->LineTo(m_pntClientAreaRef.x, m_pntClientAreaRef.y - m_nClientAreaHeight);

	//�������꼰���ֱ�ʶ
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
	//ԭ��λ��
	m_pntClientAreaOrig.x = m_pntClientAreaRef.x + (int)(xspan*5.5);
	m_pntClientAreaOrig.y = m_pntClientAreaRef.y - m_nClientAreaHeight + (int)(yspan*4);

	pen.DeleteObject();

	//���ݺ�����
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

	//��ʶԭ��
	pDC->SetPixel(m_pntClientAreaOrig, RGB(255, 0, 0));

	//����ʶ
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
	ProductTreeRootPtr pProductTreeRoot = g_TestDataTreeMgt.m_pProductTreeRoot;
	ProductNodePtr pProductNode = g_TestDataTreeMgt.m_pProductTreeRoot->pProductNodeListHead->pNext;
	while(pProductNode!=NULL){
		FileNodePtr pFileNode = pProductNode->pFileListHead->pNext;
		while(pFileNode!=NULL){
			int flag_selected = 0;
			for(int iIndex=0;iIndex<pFileNode->test_record_header.iNumOfSpeed;iIndex++){
				struct TreeItemData *tp = pFileNode->tree_item_data + iIndex;
				if(tp->bSelected){
					flag_selected = 1;
					if(tp->pNode->bProcessed==FALSE){
						char sFileName[256];
						TCHAR2char(sFileName, tp->pNode->sFileName, _tcslen(tp->pNode->sFileName));
						g_TestDataTreeMgt.LoadAndProcessFile(sFileName, tp->pNode);
					}

					pen.DeleteObject();
					pen.CreatePen(PS_SOLID, 2, color[tp->iIndex]);
					pDC->SelectObject(&pen);


					iNumDrawed ++;
					if(iNumDrawed>=20){
						break;
					}
				}//END OF IF
			}//END OF FOR
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

	pDC->SelectObject(oldFont);
}

void CRightDrawAreaView::Draw_Force_Vs_Speed()
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
	//��ȡ����ͼ�����Сλ��
	m_pntClientAreaRef.x = rect.left + 50;
	m_pntClientAreaRef.y = rect.bottom - 50;
	m_nClientAreaWidth = rect.right - rect.left - 100;
	m_nClientAreaHeight = rect.bottom - rect.top - 100;

	CDC *pDC=GetDC();

	pen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	pDC->SelectObject(&pen);
	//������
	pDC->MoveTo(m_pntClientAreaRef);
	pDC->LineTo(m_pntClientAreaRef.x + m_nClientAreaWidth, m_pntClientAreaRef.y);
	//������
	pDC->MoveTo(m_pntClientAreaRef);
	pDC->LineTo(m_pntClientAreaRef.x, m_pntClientAreaRef.y - m_nClientAreaHeight);

	//�������꼰���ֱ�ʶ
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
	//ԭ��λ��
	m_pntClientAreaOrig.x = m_pntClientAreaRef.x + (int)(xspan*5.5);
	m_pntClientAreaOrig.y = m_pntClientAreaRef.y - m_nClientAreaHeight + (int)(yspan*4);

	pen.DeleteObject();

	//���ݺ�����
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

	//��ʶԭ��
	pDC->SetPixel(m_pntClientAreaOrig, RGB(255, 0, 0));

	//����ʶ
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
	ProductTreeRootPtr pProductTreeRoot = g_TestDataTreeMgt.m_pProductTreeRoot;
	ProductNodePtr pProductNode = g_TestDataTreeMgt.m_pProductTreeRoot->pProductNodeListHead->pNext;
	while(pProductNode!=NULL){
		FileNodePtr pFileNode = pProductNode->pFileListHead->pNext;
		while(pFileNode!=NULL){
			int flag_selected = 0;
			for(int iIndex=0;iIndex<pFileNode->test_record_header.iNumOfSpeed;iIndex++){
				struct TreeItemData *tp = pFileNode->tree_item_data + iIndex;
				if(tp->bSelected){
					flag_selected = 1;
					if(tp->pNode->bProcessed==FALSE){
						char sFileName[256];
						TCHAR2char(sFileName, tp->pNode->sFileName, _tcslen(tp->pNode->sFileName));
						g_TestDataTreeMgt.LoadAndProcessFile(sFileName, tp->pNode);
					}

					pen.DeleteObject();
					pen.CreatePen(PS_SOLID, 2, color[tp->iIndex]);
					pDC->SelectObject(&pen);

					//��-�ٶ�����
					if(m_iDrawMode & DM_NORMAL_ONLY){
						double *pStart = NULL;
						for(j=tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex];j<tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]+tp->pNode->pAdditionInfo->iDataBandLen[tp->iIndex];j++){
							/*
							double tmp;
							pStart = &(tp->pNode->pTestRecord->fDisplacement[tp->iIndex][j]);
							if(j<=tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]+tp->pNode->pAdditionInfo->iDataBandLen[tp->iIndex]-3){
								tmp = ( -3 * (*pStart) + 4 * (*(pStart+1)) - 1 * (*(pStart+2)) ) / 2 * tp->pNode->pTestRecord->fDataFreq[tp->iIndex];
							}
							else if(j==tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]+tp->pNode->pAdditionInfo->iDataBandLen[tp->iIndex]-2){
								tmp = ( -3 * (*pStart) + 4 * (*(pStart+1)) - 1 * (tp->pNode->pTestRecord->fDisplacement[tp->iIndex][tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]]) ) / 2 / tp->pNode->pTestRecord->fDataFreq[tp->iIndex];
							}
							else{
								tmp = ( -3 * (*pStart) + 4 * (tp->pNode->pTestRecord->fDisplacement[tp->iIndex][tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]]) - 1 * (tp->pNode->pTestRecord->fDisplacement[tp->iIndex][tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]+1]) ) / 2 / tp->pNode->pTestRecord->fDataFreq[tp->iIndex];
							}
							*///��������ֵ
							//����20������ƽ���ķ���
							double tmp = 0;
							pStart = &(tp->pNode->pTestRecord->fDisplacement[tp->iIndex][j]);
							for(int k=0;k<10;k++){
								tmp += *(pStart+k+10) - *(pStart+k);
							}
							tmp = tmp / 100 * tp->pNode->pTestRecord->fDataFreq[tp->iIndex];
								
							//X����ֵ�ͷ������ŵ���Ӧֵ
							tmp = tmp * 100 * xspan / 10;
							tmp += m_pntClientAreaOrig.x;
							x = (int)tmp;
							//Yֱ��ʹ��ԭ��С
							y = (int)(m_pntClientAreaOrig.y - tp->pNode->pTestRecord->fForce[tp->iIndex][j] * (yspan/250));
							if(j==tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]){
								pDC->MoveTo(x, y);
								pDC->SetPixel(x, y, color[tp->iIndex]);
							}
							else{
								pDC->LineTo(x, y);
							}
						}
					}
					if(m_iDrawMode & DM_FILTER_ONLY){
						double *pStart = NULL;
						for(j=tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex];j<tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]+tp->pNode->pAdditionInfo->iDataBandLen[tp->iIndex];j++){
							/*
							double tmp;
							pStart = &(tp->pNode->pTestRecord->fDisplacement[tp->iIndex][j]);
							if(j<=tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]+tp->pNode->pAdditionInfo->iDataBandLen[tp->iIndex]-3){
								tmp = ( -3 * (*pStart) + 4 * (*(pStart+1)) - 1 * (*(pStart+2)) ) / 2 * tp->pNode->pTestRecord->fDataFreq[tp->iIndex];
							}
							else if(j==tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]+tp->pNode->pAdditionInfo->iDataBandLen[tp->iIndex]-2){
								tmp = ( -3 * (*pStart) + 4 * (*(pStart+1)) - 1 * (tp->pNode->pTestRecord->fDisplacement[tp->iIndex][tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]]) ) / 2 / tp->pNode->pTestRecord->fDataFreq[tp->iIndex];
							}
							else{
								tmp = ( -3 * (*pStart) + 4 * (tp->pNode->pTestRecord->fDisplacement[tp->iIndex][tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]]) - 1 * (tp->pNode->pTestRecord->fDisplacement[tp->iIndex][tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]+1]) ) / 2 / tp->pNode->pTestRecord->fDataFreq[tp->iIndex];
							}
							*///��������ֵ
							//����20������ƽ���ķ���
							double tmp = 0;
							pStart = &(tp->pNode->pTestRecord->fDisplacement[tp->iIndex][j]);
							for(int k=0;k<10;k++){
								tmp += *(pStart+k+10) - *(pStart+k);
							}
							tmp = tmp / 100 * tp->pNode->pTestRecord->fDataFreq[tp->iIndex];
								
							//X����ֵ�ͷ������ŵ���Ӧֵ
							tmp = tmp * 100 * xspan / 10;
							tmp += m_pntClientAreaOrig.x;
							x = (int)tmp;
							//Yֱ��ʹ��ԭ��С
							y = (int)(m_pntClientAreaOrig.y - tp->pNode->pAdditionInfo->fForceOfFilter[tp->iIndex][j] * (yspan/250));
							if(j==tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]){
								pDC->MoveTo(x, y);
								pDC->SetPixel(x, y, color[tp->iIndex]);
							}
							else{
								pDC->LineTo(x, y);
							}
						}
					}

					iNumDrawed ++;
					if(iNumDrawed>=20){
						break;
					}
				}//END OF IF
			}//END OF FOR
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

	pDC->SelectObject(oldFont);
}

void CRightDrawAreaView::Draw_MaxForce_Vs_Speed()
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
	//��ȡ����ͼ�����Сλ��
	m_pntClientAreaRef.x = rect.left + 50;
	m_pntClientAreaRef.y = rect.bottom - 50;
	m_nClientAreaWidth = rect.right - rect.left - 100;
	m_nClientAreaHeight = rect.bottom - rect.top - 100;

	CDC *pDC=GetDC();

	pen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	pDC->SelectObject(&pen);
	//������
	pDC->MoveTo(m_pntClientAreaRef);
	pDC->LineTo(m_pntClientAreaRef.x + m_nClientAreaWidth, m_pntClientAreaRef.y);
	//������
	pDC->MoveTo(m_pntClientAreaRef);
	pDC->LineTo(m_pntClientAreaRef.x, m_pntClientAreaRef.y - m_nClientAreaHeight);

	//�������꼰���ֱ�ʶ
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
	//ԭ��λ��
	m_pntClientAreaOrig.x = m_pntClientAreaRef.x + (int)(xspan*5.5);
	m_pntClientAreaOrig.y = m_pntClientAreaRef.y - m_nClientAreaHeight + (int)(yspan*4);

	pen.DeleteObject();

	//���ݺ�����
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

	//��ʶԭ��
	pDC->SetPixel(m_pntClientAreaOrig, RGB(255, 0, 0));

	//����ʶ
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
	ProductTreeRootPtr pProductTreeRoot = g_TestDataTreeMgt.m_pProductTreeRoot;
	ProductNodePtr pProductNode = g_TestDataTreeMgt.m_pProductTreeRoot->pProductNodeListHead->pNext;
	while(pProductNode!=NULL){
		FileNodePtr pFileNode = pProductNode->pFileListHead->pNext;
		while(pFileNode!=NULL){
			int flag_selected = 0;
			for(int iIndex=0;iIndex<pFileNode->test_record_header.iNumOfSpeed;iIndex++){
				struct TreeItemData *tp = pFileNode->tree_item_data + iIndex;
				if(tp->bSelected){
					flag_selected = 1;
					if(tp->pNode->bProcessed==FALSE){
						char sFileName[256];
						TCHAR2char(sFileName, tp->pNode->sFileName, _tcslen(tp->pNode->sFileName));
						g_TestDataTreeMgt.LoadAndProcessFile(sFileName, tp->pNode);
					}

					pen.DeleteObject();
					pen.CreatePen(PS_SOLID, 2, color[tp->iIndex]);
					pDC->SelectObject(&pen);


					iNumDrawed ++;
					if(iNumDrawed>=20){
						break;
					}
				}//END OF IF
			}//END OF FOR
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

	pDC->SelectObject(oldFont);
}

void CRightDrawAreaView::Draw_Force_Vs_Time()
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
	//��ȡ����ͼ�����Сλ��
	m_pntClientAreaRef.x = rect.left + 50;
	m_pntClientAreaRef.y = rect.bottom - 50;
	m_nClientAreaWidth = rect.right - rect.left - 100;
	m_nClientAreaHeight = rect.bottom - rect.top - 100;

	CDC *pDC=GetDC();

	pen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	pDC->SelectObject(&pen);
	//������
	pDC->MoveTo(m_pntClientAreaRef);
	pDC->LineTo(m_pntClientAreaRef.x + m_nClientAreaWidth, m_pntClientAreaRef.y);
	//������
	pDC->MoveTo(m_pntClientAreaRef);
	pDC->LineTo(m_pntClientAreaRef.x, m_pntClientAreaRef.y - m_nClientAreaHeight);

	//�������꼰���ֱ�ʶ
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
	//ԭ��λ��
	m_pntClientAreaOrig.x = m_pntClientAreaRef.x + (int)(xspan*5.5);
	m_pntClientAreaOrig.y = m_pntClientAreaRef.y - m_nClientAreaHeight + (int)(yspan*4);

	pen.DeleteObject();

	//���ݺ�����
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

	//��ʶԭ��
	pDC->SetPixel(m_pntClientAreaOrig, RGB(255, 0, 0));

	//����ʶ
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
	ProductTreeRootPtr pProductTreeRoot = g_TestDataTreeMgt.m_pProductTreeRoot;
	ProductNodePtr pProductNode = g_TestDataTreeMgt.m_pProductTreeRoot->pProductNodeListHead->pNext;
	while(pProductNode!=NULL){
		FileNodePtr pFileNode = pProductNode->pFileListHead->pNext;
		while(pFileNode!=NULL){
			int flag_selected = 0;
			for(int iIndex=0;iIndex<pFileNode->test_record_header.iNumOfSpeed;iIndex++){
				struct TreeItemData *tp = pFileNode->tree_item_data + iIndex;
				if(tp->bSelected){
					flag_selected = 1;
					if(tp->pNode->bProcessed==FALSE){
						char sFileName[256];
						TCHAR2char(sFileName, tp->pNode->sFileName, _tcslen(tp->pNode->sFileName));
						g_TestDataTreeMgt.LoadAndProcessFile(sFileName, tp->pNode);
					}

					pen.DeleteObject();
					pen.CreatePen(PS_SOLID, 2, color[tp->iIndex]);
					pDC->SelectObject(&pen);

					//��-ʱ������
					if(m_iDrawMode & DM_NORMAL_ONLY){
						for(j=tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex];j<tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]+tp->pNode->pAdditionInfo->iDataBandLen[tp->iIndex];j++){
							//X����ֵ�ͷ������ŵ���Ӧֵ
							double tmp;
							tmp = j-tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex];
							tmp /= tp->pNode->pTestRecord->fDataFreq[tp->iIndex];
							tmp *= xspan * 5;
							tmp += m_pntClientAreaOrig.x;
							x = (int)tmp;
							//Yֱ��ʹ��ԭ��С
							y = (int)(m_pntClientAreaOrig.y - tp->pNode->pTestRecord->fForce[tp->iIndex][j] * (yspan/250));
							if(j==tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]){
								pDC->MoveTo(x, y);
								pDC->SetPixel(x, y, color[tp->iIndex]);
							}
							else{
								pDC->LineTo(x, y);
							}
						}
					}

					if(m_iDrawMode & DM_FILTER_ONLY){
						for(j=tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex];j<tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]+tp->pNode->pAdditionInfo->iDataBandLen[tp->iIndex];j++){
							//X����ֵ�ͷ������ŵ���Ӧֵ
							double tmp;
							tmp = j-tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex];
							tmp /= tp->pNode->pTestRecord->fDataFreq[tp->iIndex];
							tmp *= xspan * 5;
							tmp += m_pntClientAreaOrig.x;
							x = (int)tmp;
							//Yֱ��ʹ��ԭ��С
							y = (int)(m_pntClientAreaOrig.y - tp->pNode->pAdditionInfo->fForceOfFilter[tp->iIndex][j] * (yspan/250));
							if(j==tp->pNode->pAdditionInfo->iDataBandStart[tp->iIndex]){
								pDC->MoveTo(x, y);
								pDC->SetPixel(x, y, color[tp->iIndex]);
							}
							else{
								pDC->LineTo(x, y);
							}
						}
					}

					iNumDrawed ++;
					if(iNumDrawed>=20){
						break;
					}
				}//END OF IF
			}//END OF FOR
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

	pDC->SelectObject(oldFont);
}

void CRightDrawAreaView::Draw_Friction_Force_Vs_Position()
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
	//��ȡ����ͼ�����Сλ��
	m_pntClientAreaRef.x = rect.left + 50;
	m_pntClientAreaRef.y = rect.bottom - 50;
	m_nClientAreaWidth = rect.right - rect.left - 100;
	m_nClientAreaHeight = rect.bottom - rect.top - 100;

	CDC *pDC=GetDC();

	pen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	pDC->SelectObject(&pen);
	//������
	pDC->MoveTo(m_pntClientAreaRef);
	pDC->LineTo(m_pntClientAreaRef.x + m_nClientAreaWidth, m_pntClientAreaRef.y);
	//������
	pDC->MoveTo(m_pntClientAreaRef);
	pDC->LineTo(m_pntClientAreaRef.x, m_pntClientAreaRef.y - m_nClientAreaHeight);

	//�������꼰���ֱ�ʶ
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
	//ԭ��λ��
	m_pntClientAreaOrig.x = m_pntClientAreaRef.x + (int)(xspan*5.5);
	m_pntClientAreaOrig.y = m_pntClientAreaRef.y - m_nClientAreaHeight + (int)(yspan*4);

	pen.DeleteObject();

	//���ݺ�����
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

	//��ʶԭ��
	pDC->SetPixel(m_pntClientAreaOrig, RGB(255, 0, 0));

	//����ʶ
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
	ProductTreeRootPtr pProductTreeRoot = g_TestDataTreeMgt.m_pProductTreeRoot;
	ProductNodePtr pProductNode = g_TestDataTreeMgt.m_pProductTreeRoot->pProductNodeListHead->pNext;
	while(pProductNode!=NULL){
		FileNodePtr pFileNode = pProductNode->pFileListHead->pNext;
		while(pFileNode!=NULL){
			int flag_selected = 0;
			for(int iIndex=0;iIndex<pFileNode->test_record_header.iNumOfSpeed;iIndex++){
				struct TreeItemData *tp = pFileNode->tree_item_data + iIndex;
				if(tp->bSelected){
					flag_selected = 1;
					if(tp->pNode->bProcessed==FALSE){
						char sFileName[256];
						TCHAR2char(sFileName, tp->pNode->sFileName, _tcslen(tp->pNode->sFileName));
						g_TestDataTreeMgt.LoadAndProcessFile(sFileName, tp->pNode);
					}

					if(!tp->pNode->pTestRecord->bFrictionSpeed){
						break;
					}

					pen.DeleteObject();
					pen.CreatePen(PS_SOLID, 2, color[tp->iIndex]);
					pDC->SelectObject(&pen);

					pDC->SelectObject(oldFont);
					if(m_iDrawMode & DM_NORMAL_ONLY){
						int x_start, y_start;
						for(j=tp->pNode->pAdditionInfo->iFrictionDataBandStart;j<tp->pNode->pAdditionInfo->iFrictionDataBandStart+tp->pNode->pAdditionInfo->iFrictionDataBandLen;j++){
							//X����ֵ�ͷ������ŵ���Ӧֵ
							x = (int)(tp->pNode->pTestRecord->fFrictionDisplacement[j] * 1000 * (xspan/10) + m_pntClientAreaOrig.x);
							//Yֱ��ʹ��ԭ��С
							y = (int)(m_pntClientAreaOrig.y - tp->pNode->pTestRecord->fFrictionForce[j] * (yspan/250));
							if(j==tp->pNode->pAdditionInfo->iFrictionDataBandStart){
								pDC->MoveTo(x, y);
								x_start = x;
								y_start = y;
								pDC->SetPixel(x, y, color[0]);
							}
							else{
								pDC->LineTo(x, y);
							}
						}
						pDC->LineTo(x_start, y_start);
					}

					if(m_iDrawMode & DM_FILTER_ONLY){
						int x_start, y_start;
						for(j=tp->pNode->pAdditionInfo->iFrictionDataBandStart;j<tp->pNode->pAdditionInfo->iFrictionDataBandStart+tp->pNode->pAdditionInfo->iFrictionDataBandLen;j++){
							//X����ֵ�ͷ������ŵ���Ӧֵ
							x = (int)(tp->pNode->pTestRecord->fFrictionDisplacement[j] * 1000 * (xspan/10) + m_pntClientAreaOrig.x);
							//Yֱ��ʹ��ԭ��С
							y = (int)(m_pntClientAreaOrig.y - tp->pNode->pAdditionInfo->fFrictionForceOfFilter[j] * (yspan/250));
							if(j==tp->pNode->pAdditionInfo->iFrictionDataBandStart){
								pDC->MoveTo(x, y);
								x_start = x;
								y_start = y;
								pDC->SetPixel(x, y, color[0]);
							}
							else{
								pDC->LineTo(x, y);
							}
						}
						pDC->LineTo(x_start, y_start);
					}

					iNumDrawed ++;
					if(iNumDrawed>=20){
						break;
					}

					break;
				}//END OF IF
			}//END OF FOR
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
}

// CRightDrawAreaView ��Ϣ�������

void CRightDrawAreaView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	m_pTab->InsertItem(0, _T("F vs P"));
	m_pTab->InsertItem(1, _T("F vs S"));
	m_pTab->InsertItem(2, _T("F vs PS"));
	m_pTab->InsertItem(3, _T("MaxF vs S"));
	m_pTab->InsertItem(4, _T("F vs T"));
	m_pTab->InsertItem(5, _T("FF vs P"));
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
