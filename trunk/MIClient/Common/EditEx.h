#if !defined(AFX_EDITEX_H__2123ACF1_C741_4858_9D5D_BCFA44817143__INCLUDED_)
#define AFX_EDITEX_H__2123ACF1_C741_4858_9D5D_BCFA44817143__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditEx window

class CEditEx : public CEdit
{
// Construction
public:
	CEditEx();

private:
	COLORREF	m_ForeColor;	//�ı���ɫ
	COLORREF	m_BackColor;	//����ɫ
	CBrush		m_BkBrush;		//����ˢ
	CFont*		p_Font;			//����

// �ӿں���
public:
	void SetForeColor(COLORREF color);	//�����ı���ɫ 
	void SetBkColor(COLORREF color);	//���ñ�����ɫ 
	void SetTextFont(int FontHight,LPCTSTR FontName);	//��������

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditEx)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITEX_H__2123ACF1_C741_4858_9D5D_BCFA44817143__INCLUDED_)
