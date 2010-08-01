// MyButton.cpp : implementation file
//

#include "stdafx.h"
#include "ButtonEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CButtonEx

CButtonEx::CButtonEx()
{
	m_Style = 0;			//按钮形状风格
	b_InRect = false;		//鼠标进入标志
	m_strText = _T("");		//按钮文字（使用默认文字）
	m_ForeColor = RGB(0,0,0);			//文字颜色（黑色）
	m_BackColor = RGB(243,243,243);		//背景色（灰白色）
	m_LockForeColor = GetSysColor(COLOR_GRAYTEXT);	//锁定按钮的文字颜色
	p_Font = NULL;							//字体指针
}

CButtonEx::~CButtonEx()
{
	if ( p_Font )	delete p_Font;		//删除字体
}


BEGIN_MESSAGE_MAP(CButtonEx, CButton)
	//{{AFX_MSG_MAP(CButtonEx)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CButtonEx message handlers

void CButtonEx::PreSubclassWindow() 
{
	ModifyStyle( 0, BS_OWNERDRAW );		//设置按钮属性为自画式
	
	CButton::PreSubclassWindow();
}

//消息函数：绘制按钮
void CButtonEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC *pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	m_ButRect = lpDrawItemStruct->rcItem;	//获取按钮尺寸

	if( m_strText.IsEmpty() )
		GetWindowText( m_strText );			//获取按钮文本

	int nSavedDC = pDC->SaveDC();
	VERIFY( pDC );
	DrawButton( pDC );				//绘制按钮
	pDC->RestoreDC( nSavedDC );
}

//消息函数：鼠标移动
void CButtonEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( !b_InRect || GetCapture()!=this )	//鼠标进入按钮
	{
		b_InRect = true;	//设置进入标志
		SetCapture();		//捕获鼠标
		m_Style = 1;		//设置按钮状态
		Invalidate();		//重绘按钮
	}
	else
	{
		if ( !m_ButRect.PtInRect(point) )			//鼠标离开按钮
		{
			b_InRect = false;	//清除进入标志
			ReleaseCapture();	//释放捕获的鼠标
			m_Style = 0;		//设置按钮状态
			Invalidate();		//重绘按钮
		}
	}
	
	CButton::OnMouseMove(nFlags, point);
}

//消息函数：单击左键
void CButtonEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_Style = 2;
	Invalidate();		//重绘按钮
	
	CButton::OnLButtonDown(nFlags, point);
}

//消息函数：弹起左键
void CButtonEx::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_Style = 1;
	Invalidate();		//重绘按钮
	
	CButton::OnLButtonUp(nFlags, point);
}

////////////////////////////////////////////////////////////////////////
//绘制按钮：

//画按钮（0-正常，1-当前，2-按下，3-锁定）
void CButtonEx::DrawButton(CDC *pDC)
{
	//调整状态
	if( m_Style==3 ) m_Style = 0;
	if( GetStyle() & WS_DISABLED )
		m_Style = 3;	//禁止状态
	//根据状态调整边框颜色和文字颜色
	COLORREF bColor, fColor;	//bColor为边框颜色，fColor为文字颜色
	switch( m_Style )
	{
	case 0: bColor = RGB(192,192,192); fColor = m_ForeColor; break;	//正常按钮
	case 1: bColor = RGB(255,255,255); fColor = m_ForeColor; break;	//鼠标进入时按钮
	case 2: bColor = RGB(192,192,192); fColor = m_ForeColor; break;	//按下的按钮
	case 3: bColor = m_BackColor; fColor = m_LockForeColor; break;	//锁定的按钮
	}
	//绘制按钮背景
	CBrush Brush;
	Brush.CreateSolidBrush( m_BackColor );	//背景刷
	pDC->SelectObject( &Brush );
	CPen Pen;
	Pen.CreatePen(PS_SOLID, 1, bColor );
	pDC->SelectObject( &Pen );
	pDC->RoundRect(&m_ButRect,CPoint(5,5));	//画圆角矩形
	//绘制按钮按下时的边框
	if( m_Style!=2 )
	{
		CRect Rect;
		Rect.SetRect( m_ButRect.left+2, m_ButRect.top+1, m_ButRect.right, m_ButRect.bottom );
		pDC->DrawEdge( &Rect, BDR_RAISEDINNER, BF_RECT );	//画边框
	}
	//绘制按钮文字
	pDC->SetTextColor( fColor );		//画文字
	pDC->SetBkMode( TRANSPARENT );
	pDC->DrawText( m_strText, &m_ButRect, DT_SINGLELINE | DT_CENTER
		| DT_VCENTER | DT_END_ELLIPSIS);
	//绘制拥有焦点按钮的虚线框
	if( GetFocus()==this )
	{
		CRect Rect;
		Rect.SetRect( m_ButRect.left+3, m_ButRect.top+2, m_ButRect.right-3, m_ButRect.bottom-2 );
		pDC->DrawFocusRect( &Rect );	//画拥有焦点的虚线框
	}
}

////////////////////////////////////////////////////////////////////////
//接口函数

//设置按钮文本
void CButtonEx::SetText(CString str)
{
	m_strText = _T("");
	SetWindowText(str);
}

//设置文本颜色
void CButtonEx::SetForeColor(COLORREF color)
{
	m_ForeColor = color;
	Invalidate();
}

//设置背景颜色
void CButtonEx::SetBkColor(COLORREF color)
{
	m_BackColor = color;
	Invalidate();
}

//获取背景颜色
void CButtonEx::GetBkColor(COLORREF &color)
{
	color = m_BackColor;
}

//设置字体(字体高度、字体名)
void CButtonEx::SetTextFont(int FontHight,LPCTSTR FontName)
{
	if ( p_Font )	delete p_Font;	//删除旧字体
	p_Font = new CFont;
	p_Font->CreatePointFont( FontHight, FontName );	//创建新字体
	SetFont( p_Font );				//设置字体
}
