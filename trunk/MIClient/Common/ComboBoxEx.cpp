/*
  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	�ļ���BSColorComboBox.cpp

	��Ȩ��Devia@163.com(Blue Sky) ����

	��������������Ҫ�����ɫѡ����Ͽ�

	���ߣ�������(Devia_Li)	2003.01.15

	��ϵ��(Tel)13512524180	(E_mail)lixiaofei@163.net
  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
*/

#include "stdafx.h"
#include "ComboBoxEx.h"

//################################################################
CMyComboBoxEx::CMyComboBoxEx()
{	
	//��ǰ����Ƿ��ڶ�����
	m_bOverControl = false;

	//Сͼ��ߴ�
	iIconX = ::GetSystemMetrics(SM_CXSMICON);
	iIconY = ::GetSystemMetrics(SM_CYSMICON);	
}
//################################################################
CMyComboBoxEx::~CMyComboBoxEx()
{
	while(!m_crItem.IsEmpty())
	{
		LPBSCBITEM lpItem = m_crItem.RemoveHead();		
		delete lpItem;
	}
}
//################################################################
BOOL CMyComboBoxEx::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	DWORD dw = dwStyle;
	//dw |= CBS_OWNERDRAWVARIABLE;
	
	if( !CComboBox::Create(dw, rect, pParentWnd, nID) )
		return false;

	CFont * font = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	SetFont(font);

	return true;
}
//################################################################
IMPLEMENT_DYNCREATE(CMyComboBoxEx, CComboBox)

BEGIN_MESSAGE_MAP(CMyComboBoxEx, CComboBox)
	//{{AFX_MSG_MAP(CMyComboBoxEx)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(CBN_SELENDOK, OnSelendok)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//################################################################
void CMyComboBoxEx::InitBSColorCB(void)
{
	int iMinColor = COLOR_SCROLLBAR, 
		iMaxColor = COLOR_BTNHIGHLIGHT;

	if(WINVER >= 0x0400)
		iMaxColor = COLOR_INFOBK;

	if(WINVER >= 0x0500)
		iMaxColor = 28;
	
	//��ʼ��CB��ɫ�б���Item(������SysColorֵ)
	for(int iLoop = iMinColor; iLoop <= iMaxColor; ++iLoop)
	{
		LPBSCBITEM lpItem = new BSCBITEM;
		lpItem->iIndex = AddString(CString(strColorName[iLoop]));
		lpItem->crColor = ::GetSysColor(iLoop);
		lpItem->lpCaption = CString(strColorName[iLoop]);
		//
		if(m_crItem.IsEmpty())
			m_crItem.AddHead(lpItem);
		else
			m_crItem.AddTail(lpItem);		
	}

	//��Item��Ϊ���û��Զ�����ɫ������
	LPBSCBITEM lpItem = new BSCBITEM;
	lpItem->iIndex = AddString(CString("More Colors"));
	lpItem->crColor = RGB(213, 233, 249);
	lpItem->lpCaption = CString("More Colors");
	
	if(m_crItem.IsEmpty())
		m_crItem.AddHead(lpItem);
	else
		m_crItem.AddTail(lpItem);		
	//��ʼ����ǰ��ɫ
	m_crColor = m_crItem.GetHead()->crColor;
}
//################################################################
BOOL CMyComboBoxEx::OnEraseBkgnd(CDC* pDC) 
{	
	ASSERT(pDC->GetSafeHdc());
	return false;
}
//################################################################
void CMyComboBoxEx::OnPaint() 
{
	CPaintDC dc(this); 
	OnCBPaint(&dc);
}
//################################################################
void CMyComboBoxEx::OnCBPaint(CDC* pDC)
{
	ASSERT(pDC->GetSafeHdc());
	
	//���ƿͻ���
	CDC dMemDC;
	dMemDC.CreateCompatibleDC(pDC);
	dMemDC.SetMapMode(pDC->GetMapMode());	

	//������
	CBitmap mNewBmp;
	RECT rc;
	GetClientRect(&rc);
	mNewBmp.CreateCompatibleBitmap(pDC, rc.right - rc.left, rc.bottom - rc.top);
	CBitmap* pOldBmp = dMemDC.SelectObject(&mNewBmp);

	//���������friend��ʽ�����ʸ����protected��Ա�����ͺ���
	CWnd::DefWindowProc(WM_PAINT, (WPARAM)dMemDC.m_hDC, 0);
	
	pDC->BitBlt(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, &dMemDC,
		rc.left ,rc.top, SRCCOPY);
	
	//�ָ�
	dMemDC.SelectObject(pOldBmp);
	pOldBmp->DeleteObject();
	dMemDC.DeleteDC();

	GetWindowRect(&rc);
	ScreenToClient(&rc);
	pDC->DrawEdge(&rc, (m_bOverControl ? BDR_RAISEDINNER : BDR_SUNKENINNER), BF_RECT);
}
//################################################################
void CMyComboBoxEx::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 888 && IsWindowEnabled())
	{
		CPoint point;
		::GetCursorPos(&point);

		CRect rect;
		GetWindowRect(&rect);
		if(rect.PtInRect(point))
		{
			m_bOverControl = true;
		}
		else
		{
			m_bOverControl = false;
			KillTimer(nIDEvent);
		}
	}
	CComboBox::OnTimer(nIDEvent);
}
//################################################################
void CMyComboBoxEx::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemHeight = iIconY + 5;	
}
//################################################################
void CMyComboBoxEx::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{	
	ASSERT(lpDIS->CtlType == ODT_COMBOBOX);
	
	//����
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	ASSERT(pDC->GetSafeHdc());
	
	//������
	RECT rc = lpDIS->rcItem;
	RECT rcIcon(rc), rcTxt(rc);
	
	//��ǰ��Item������
	LPBSCBITEM lpItem = GetItem(lpDIS->itemID);	
	if(lpItem != NULL)
	{		
		//����ɫIcon
		rcIcon.right = rcIcon.left + iIconX;
		rcIcon.top += (rc.bottom - rc.top - iIconY) / 2;
		rcIcon.bottom = rcIcon.top + iIconY;
		
		pDC->FillSolidRect(rcIcon.left, rcIcon.top, 
			rcIcon.right - rcIcon.left, rcIcon.bottom - rcIcon.top, lpItem->crColor);
		
		pDC->DrawEdge(&rcIcon, BDR_RAISEDINNER, BF_RECT);
		//��ʼ������
		int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(0, 0, 0));
		
		rcTxt.left = rcIcon.right + 5;
		rcTxt.top = rcIcon.top;
		
		pDC->DrawText(lpItem->lpCaption, &rcTxt, 
			DT_VCENTER | DT_END_ELLIPSIS | DT_NOCLIP | DT_SINGLELINE);
		
		pDC->SetBkMode(nOldBkMode);
	}
}
//################################################################
void CMyComboBoxEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_bOverControl = true;
	SetTimer(888, 100, NULL);

	CComboBox::OnMouseMove(nFlags, point);
}
//################################################################
LPBSCBITEM CMyComboBoxEx::GetItem(int iIndex)
{
	//��ǰ��Item������
	POSITION pos = m_crItem.FindIndex(iIndex);
	if(pos)
	{
		LPBSCBITEM lpItem = m_crItem.GetAt(pos);

		ASSERT(lpItem);
		return lpItem;
	}
	else
		return (LPBSCBITEM)NULL;
}
//################################################################
COLORREF CMyComboBoxEx::GetColor()
{
	if(IsWindowEnabled())
		return m_crColor;
	else
	{
		return (m_crColor = GetItem(this->GetCurSel())->crColor);
	}
}
//################################################################
void CMyComboBoxEx::GetRGBValue(int* R, int* G, int* B)
{
	*R = GetRValue((DWORD)m_crColor);
	*G = GetGValue((DWORD)m_crColor);
	*B = GetBValue((DWORD)m_crColor);
}
//################################################################
void CMyComboBoxEx::OnSelchange() 
{
	int iIndex = GetCurSel();
	
	if(iIndex != CB_ERR && iIndex >= 0)
	{
		CDC* pDC = this->GetDC();
		//������
		RECT rc;
		int iScrollX = ::GetSystemMetrics(SM_CXVSCROLL);
		GetClientRect(&rc);
		
		pDC->FillSolidRect(rc.left + 2, rc.top + 2, 
			rc.right - rc.left - iScrollX - 4, rc.bottom - rc.top - 2, 
			::GetSysColor(COLOR_WINDOW));		
		
		RECT rcIcon(rc), rcTxt(rc);
		
		//��ǰ��Item������
		LPBSCBITEM lpItem = GetItem(iIndex);
		if(lpItem != NULL)
		{
			m_crColor = lpItem->crColor;
			
			//����ɫIcon
			rcIcon.left += 2;
			rcIcon.right = rcIcon.left + iIconX;
			rcIcon.top += (rc.bottom - rc.top - iIconY) / 2;
			rcIcon.bottom = rcIcon.top + iIconY;
			
			pDC->FillSolidRect(rcIcon.left, rcIcon.top, 
				rcIcon.right - rcIcon.left, rcIcon.bottom - rcIcon.top, lpItem->crColor);
			
			pDC->DrawEdge(&rcIcon, BDR_RAISEDINNER, BF_RECT);
			//��ʼ������
			int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(0, 0, 0));
			
			rcTxt.left = rcIcon.right + 5;
			rcTxt.top = rcIcon.top;
			
			CFont* font = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
			pDC->SelectObject(font);
			pDC->DrawText(lpItem->lpCaption, &rcTxt, 
				DT_VCENTER | DT_END_ELLIPSIS | DT_NOCLIP | DT_SINGLELINE);
			
			pDC->SetBkMode(nOldBkMode);
		}
		pDC->DeleteDC();
	}
}
//################################################################
void CMyComboBoxEx::OnSelendok() 
{
	int iIndex = this->GetCurSel();
	LPBSCBITEM lpTmpItem = GetItem(iIndex);
	if(lpTmpItem != NULL)
	{
		if(lpTmpItem->lpCaption == CString("More Colors"))
		{
			CColorDialog crDlg(RGB(255, 0, 0), CC_FULLOPEN);
			int iRet = crDlg.DoModal();
			if(iRet == IDOK)
			{
				m_crColor = crDlg.GetColor();
				LPBSCBITEM lpItem = m_crItem.GetTail();
				ASSERT(lpItem);
				lpItem->crColor = m_crColor;
				Invalidate();
			}
		}
	}
}
