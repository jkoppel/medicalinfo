// StaticEx.cpp : implementation file
//

#include "stdafx.h"
#include "StaticEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStaticEx

CStaticEx::CStaticEx()
{
	m_ForeColor = RGB(0,0,0);					//������ɫ
	m_BackColor = RGB(255,255,255);				//����ɫ
	m_BkBrush.CreateSolidBrush(m_BackColor);	//����ˢ
	p_Font = NULL;								//����ָ��
}

CStaticEx::~CStaticEx()
{
	if ( p_Font )	delete p_Font;		//ɾ������
}


BEGIN_MESSAGE_MAP(CStaticEx, CStatic)
	//{{AFX_MSG_MAP(CStaticEx)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticEx message handlers

//��ClassWizard������Ϣ��=WM_CTLCOLOR��
HBRUSH CStaticEx::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	pDC->SetTextColor( m_ForeColor );	//����������ɫ
	pDC->SetBkColor( m_BackColor );		//���ñ���ɫ
	return (HBRUSH)m_BkBrush.GetSafeHandle();
}

//�����ı���ɫ
void CStaticEx::SetForeColor(COLORREF color)
{
	m_ForeColor = color;
}

//���ñ�����ɫ
void CStaticEx::SetBkColor(COLORREF color)
{
	m_BackColor = color;
	m_BkBrush.Detach();
	m_BkBrush.CreateSolidBrush( m_BackColor );
}

//��������
void CStaticEx::SetTextFont(int FontHight,LPCTSTR FontName)
{
	if ( p_Font )	delete p_Font;		//ɾ��������
	p_Font = new CFont;			//�����������
	p_Font->CreatePointFont( FontHight, FontName );	//��������
	SetFont( p_Font );			//���ÿؼ�����
}
