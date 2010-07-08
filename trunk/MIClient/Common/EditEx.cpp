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
	m_ForeColor = RGB(0,0,0);					//������ɫ
	m_BackColor = RGB(255,255,255);				//����ɫ
	m_BkBrush.CreateSolidBrush(m_BackColor);	//����ˢ
	p_Font = NULL;								//����ָ��
}

CEditEx::~CEditEx()
{
	if ( p_Font )	delete p_Font;		//ɾ������
}


BEGIN_MESSAGE_MAP(CEditEx, CEdit)
	//{{AFX_MSG_MAP(CEditEx)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditEx message handlers

//��ClassWizard������Ϣ��=WM_CTLCOLOR��
HBRUSH CEditEx::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	pDC->SetTextColor( m_ForeColor );	//����������ɫ
	pDC->SetBkColor( m_BackColor );		//���ñ���ɫ
	return (HBRUSH)m_BkBrush.GetSafeHandle();
}

//�����ı���ɫ
void CEditEx::SetForeColor(COLORREF color)
{
	m_ForeColor = color;
}

//���ñ�����ɫ
void CEditEx::SetBkColor(COLORREF color)
{
	m_BackColor = color;
	m_BkBrush.Detach();
	m_BkBrush.CreateSolidBrush( m_BackColor );
}

//��������
void CEditEx::SetTextFont(int FontHight,LPCTSTR FontName)
{
	if ( p_Font )	delete p_Font;		//ɾ��������
	p_Font = new CFont;			//�����������
	p_Font->CreatePointFont( FontHight, FontName );	//��������
	SetFont( p_Font );			//���ÿؼ�����
}
