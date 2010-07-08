// EditEx.cpp : implementation file
//

#include "stdafx.h"
#include "EditEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditEx

CEditEx::CEditEx()
{
	m_ForeColor = RGB(0,0,0);					//文字颜色
	m_BackColor = RGB(255,255,255);				//背景色
	m_BkBrush.CreateSolidBrush(m_BackColor);	//背景刷
	p_Font = NULL;								//字体指针
}

CEditEx::~CEditEx()
{
	if ( p_Font )	delete p_Font;		//删除字体
}


BEGIN_MESSAGE_MAP(CEditEx, CEdit)
	//{{AFX_MSG_MAP(CEditEx)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditEx message handlers

//用ClassWizard重载消息“=WM_CTLCOLOR”
HBRUSH CEditEx::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	pDC->SetTextColor( m_ForeColor );	//设置文字颜色
	pDC->SetBkColor( m_BackColor );		//设置背景色
	return (HBRUSH)m_BkBrush.GetSafeHandle();
}

//设置文本颜色
void CEditEx::SetForeColor(COLORREF color)
{
	m_ForeColor = color;
}

//设置背景颜色
void CEditEx::SetBkColor(COLORREF color)
{
	m_BackColor = color;
	m_BkBrush.Detach();
	m_BkBrush.CreateSolidBrush( m_BackColor );
}

//设置字体
void CEditEx::SetTextFont(int FontHight,LPCTSTR FontName)
{
	if ( p_Font )	delete p_Font;		//删除旧字体
	p_Font = new CFont;			//生成字体对象
	p_Font->CreatePointFont( FontHight, FontName );	//创建字体
	SetFont( p_Font );			//设置控件字体
}
