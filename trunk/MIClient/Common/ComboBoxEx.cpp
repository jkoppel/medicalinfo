/*
  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	文件：BSColorComboBox.cpp

	版权：Devia@163.com(Blue Sky) 保留

	描述：本程序主要完成颜色选择组合框。

	作者：李晓飞(Devia_Li)	2003.01.15

	联系：(Tel)13512524180	(E_mail)lixiaofei@163.net
  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
*/

#include "stdafx.h"
#include "ComboBoxEx.h"

//################################################################
CMyComboBoxEx::CMyComboBoxEx()
{	
	//当前鼠标是否在对象上
	m_bOverControl = false;

	//小图标尺寸
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
	
	//初始化CB颜色列表框的Item(常见的SysColor值)
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

	//该Item是为了用户自定义颜色而设置
	LPBSCBITEM lpItem = new BSCBITEM;
	lpItem->iIndex = AddString(CString("More Colors"));
	lpItem->crColor = RGB(213, 233, 249);
	lpItem->lpCaption = CString("More Colors");
	
	if(m_crItem.IsEmpty())
		m_crItem.AddHead(lpItem);
	else
		m_crItem.AddTail(lpItem);		
	//初始化当前颜色
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
	
	//绘制客户区
	CDC dMemDC;
	dMemDC.CreateCompatibleDC(pDC);
	dMemDC.SetMapMode(pDC->GetMapMode());	

	//画动作
	CBitmap mNewBmp;
	RECT rc;
	GetClientRect(&rc);
	mNewBmp.CreateCompatibleBitmap(pDC, rc.right - rc.left, rc.bottom - rc.top);
	CBitmap* pOldBmp = dMemDC.SelectObject(&mNewBmp);

	//子类可以以friend方式来访问父类的protected成员变量和函数
	CWnd::DefWindowProc(WM_PAINT, (WPARAM)dMemDC.m_hDC, 0);
	
	pDC->BitBlt(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, &dMemDC,
		rc.left ,rc.top, SRCCOPY);
	
	//恢复
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
	
	//画笔
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	ASSERT(pDC->GetSafeHdc());
	
	//绘制区
	RECT rc = lpDIS->rcItem;
	RECT rcIcon(rc), rcTxt(rc);
	
	//当前的Item索引号
	LPBSCBITEM lpItem = GetItem(lpDIS->itemID);	
	if(lpItem != NULL)
	{		
		//画颜色Icon
		rcIcon.right = rcIcon.left + iIconX;
		rcIcon.top += (rc.bottom - rc.top - iIconY) / 2;
		rcIcon.bottom = rcIcon.top + iIconY;
		
		pDC->FillSolidRect(rcIcon.left, rcIcon.top, 
			rcIcon.right - rcIcon.left, rcIcon.bottom - rcIcon.top, lpItem->crColor);
		
		pDC->DrawEdge(&rcIcon, BDR_RAISEDINNER, BF_RECT);
		//开始画文字
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
	//当前的Item索引号
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
		//绘制区
		RECT rc;
		int iScrollX = ::GetSystemMetrics(SM_CXVSCROLL);
		GetClientRect(&rc);
		
		pDC->FillSolidRect(rc.left + 2, rc.top + 2, 
			rc.right - rc.left - iScrollX - 4, rc.bottom - rc.top - 2, 
			::GetSysColor(COLOR_WINDOW));		
		
		RECT rcIcon(rc), rcTxt(rc);
		
		//当前的Item索引号
		LPBSCBITEM lpItem = GetItem(iIndex);
		if(lpItem != NULL)
		{
			m_crColor = lpItem->crColor;
			
			//画颜色Icon
			rcIcon.left += 2;
			rcIcon.right = rcIcon.left + iIconX;
			rcIcon.top += (rc.bottom - rc.top - iIconY) / 2;
			rcIcon.bottom = rcIcon.top + iIconY;
			
			pDC->FillSolidRect(rcIcon.left, rcIcon.top, 
				rcIcon.right - rcIcon.left, rcIcon.bottom - rcIcon.top, lpItem->crColor);
			
			pDC->DrawEdge(&rcIcon, BDR_RAISEDINNER, BF_RECT);
			//开始画文字
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
