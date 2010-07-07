// EDPView.cpp : CEDPView ���ʵ��
//

#include "stdafx.h"
#include "EDP.h"

#include "EDPDoc.h"
#include "EDPView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEDPView

IMPLEMENT_DYNCREATE(CEDPView, CView)

BEGIN_MESSAGE_MAP(CEDPView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CEDPView ����/����

CEDPView::CEDPView()
{
	//��ʼ��ֵ
	m_pntClientAreaRef.x = 0;
	m_pntClientAreaRef.y = 0;
	m_nClientAreaWidth = 0;
	m_nClientAreaHeight = 0;
	m_pTab = NULL;
}

CEDPView::~CEDPView()
{
}

BOOL CEDPView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CEDPView ����

void CEDPView::OnDraw(CDC* /*pDC*/)
{
	CEDPDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	//��ͼ
	DrawData();
}


// CEDPView ��ӡ

BOOL CEDPView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CEDPView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CEDPView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}


// CEDPView ���

#ifdef _DEBUG
void CEDPView::AssertValid() const
{
	CView::AssertValid();
}

void CEDPView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEDPDoc* CEDPView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEDPDoc)));
	return (CEDPDoc*)m_pDocument;
}
#endif //_DEBUG


// CEDPView ��Ϣ�������
void CEDPView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	m_pTab->InsertItem(0, "TAB1");
	m_pTab->InsertItem(1, "TAB2");
	m_pTab->InsertItem(2, "TAB3");
	m_pTab->InsertItem(3, "TAB4");
}

void CEDPView::DrawData()
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
		pDC->DrawText(buf, &rect, DT_SINGLELINE | DT_VCENTER| DT_CENTER);
	}
	for(i=0;i<6;i++){
		pDC->MoveTo(m_pntClientAreaRef.x, m_pntClientAreaRef.y - m_nClientAreaHeight + (int)(yspan*i));
		pDC->LineTo(m_pntClientAreaRef.x - 5, m_pntClientAreaRef.y - m_nClientAreaHeight + (int)(yspan*i));

		sprintf_s(buf, "%d", (4-i)*250);
		rect.left = m_pntClientAreaRef.x - 5 - 40;
		rect.right = m_pntClientAreaRef.x - 5;
		rect.top = m_pntClientAreaRef.y - m_nClientAreaHeight + (int)(yspan*i) - 10;
		rect.bottom = m_pntClientAreaRef.y - m_nClientAreaHeight + (int)(yspan*i) + 10;
		pDC->DrawText(buf, &rect, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
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
						FIXED_PITCH | FF_ROMAN,"System");
	pDC->SetBkMode(TRANSPARENT);
	oldFont=pDC->SelectObject(&fontLogo);
	pDC->DrawText("Force vs.Position", &rect, DT_SINGLELINE | DT_LEFT);

	rect.left = m_pntClientAreaRef.x + m_nClientAreaWidth/2 - 50;
	rect.right = m_pntClientAreaRef.x + m_nClientAreaWidth/2 + 100;
	rect.top = m_pntClientAreaRef.y + 20;
	rect.bottom = m_pntClientAreaRef.y + 40;
	fontLogo.DeleteObject();
	fontLogo.CreateFont(16,8,0,0,0,FALSE,FALSE,FALSE,
						DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
						FIXED_PITCH | FF_ROMAN,"System");
	pDC->SetBkMode(TRANSPARENT);
	oldFont=pDC->SelectObject(&fontLogo);
	pDC->DrawText("Position [mm]", &rect, DT_SINGLELINE | DT_LEFT );

	rect.left = m_pntClientAreaRef.x - 48;
	rect.right = m_pntClientAreaRef.x - 28;
	rect.top = m_pntClientAreaRef.y - m_nClientAreaHeight/2;
	rect.bottom = m_pntClientAreaRef.y - m_nClientAreaHeight/2 - 100;
	fontLogo.DeleteObject();
	fontLogo.CreateFont(16, 0, 900, 900, 0, 0, 0, 0,
						DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
						DEFAULT_PITCH, "System");
	pDC->SetBkMode(TRANSPARENT);
	oldFont=pDC->SelectObject(&fontLogo);
	pDC->DrawText("Force [N]", &rect, DT_SINGLELINE | DT_LEFT );

	if(g_pTree==NULL){
		return;
	}

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

		pen.DeleteObject();
		pen.CreatePen(PS_SOLID, 2, color[tp->iIndex]);
		pDC->SelectObject(&pen);
		for(j=tp->pNode->addition_info.iDataBandStart[tp->iIndex];
			j<tp->pNode->addition_info.iDataBandStart[tp->iIndex]+tp->pNode->addition_info.iDataBandLen[tp->iIndex];
			j++){
			//X����ֵ�ͷ������ŵ���Ӧֵ
			x = (int)(tp->pNode->test_record.fDisplacement[tp->iIndex][j] * 1000 * (xspan/10) + m_pntClientAreaOrig.x);
			//Yֱ��ʹ��ԭ��С
			y = (int)(m_pntClientAreaOrig.y - tp->pNode->test_record.fForce[tp->iIndex][j] * (yspan/250));
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

		for(j=tp->pNode->addition_info.iDataBandStart[tp->iIndex];
			j<tp->pNode->addition_info.iDataBandStart[tp->iIndex]+tp->pNode->addition_info.iDataBandLen[tp->iIndex];
			j++){
			//X����ֵ�ͷ������ŵ���Ӧֵ
			x = (int)(tp->pNode->test_record.fDisplacement[tp->iIndex][j] * 1000 * (xspan/10) + m_pntClientAreaOrig.x);
			//Yֱ��ʹ��ԭ��С
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

		/*
		for(j=tp->pNode->addition_info.iDataBandStart[tp->iIndex];
			j<tp->pNode->addition_info.iDataBandStart[tp->iIndex]+tp->pNode->addition_info.iDataBandLen[tp->iIndex];
			j++){
			//X����ֵ�ͷ������ŵ���Ӧֵ
			double tmp;
			tmp = (j-tp->pNode->addition_info.iDataBandStart[tp->iIndex]-tp->pNode->addition_info.iDataBandLen[tp->iIndex]/2);
			//tmp /= tp->pNode->test_record.fDataFreq[tp->iIndex];
			tmp *= xspan / 100;
			tmp += m_pntClientAreaOrig.x;
			x = (int)tmp;
			//x = (int)((j-tp->pNode->addition_info.iDataBandStart[tp->iIndex])  * (xspan/100) / tp->pNode->test_record.fDataFreq[tp->iIndex] + m_pntClientAreaOrig.x);
			//Yֱ��ʹ��ԭ��С
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
	}

	pDC->SelectObject(oldFont);
}


int CEDPView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pTab = new CLBTabCtrl();
	ASSERT(m_pTab);

	// note:  TVS_NOTOOLTIPS is set in CXHtmlTree::PreCreateWindow()

	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP;// | WS_BORDER;

	CRect rect(0,0,100,100);

	VERIFY(m_pTab->Create(dwStyle, rect, this, IDC_TREE));

	return 0;
}

void CEDPView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (m_pTab && ::IsWindow(m_pTab->m_hWnd))
	{
		// stretch tree to fill window
		m_pTab->MoveWindow(0, 0, cx, 22);
	}
}
