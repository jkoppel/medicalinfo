#if !defined(AFX_BUTTONEX_H__CB4940BF_2442_4FDF_8C7F_6CB245EE1DE8__INCLUDED_)
#define AFX_BUTTONEX_H__CB4940BF_2442_4FDF_8C7F_6CB245EE1DE8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CButtonEx window

class CButtonEx : public CButton
{
// Construction
public:
	CButtonEx();

private:
	int			m_Style;			//按钮形状（0-正常，1-当前，2-按下，3-锁定）
	BOOL		b_InRect;			//鼠标进入标志
	CString		m_strText;			//按钮文字
	COLORREF	m_ForeColor;		//文本颜色
	COLORREF	m_BackColor;		//背景色
	COLORREF	m_LockForeColor;	//锁定按钮的文字颜色
	CRect		m_ButRect;			//按钮尺寸
	CFont*		p_Font;				//字体

	void  DrawButton(CDC *pDC);		//画正常的按钮

// 接口函数
public:
	void SetText(CString str);
	void SetForeColor(COLORREF color);		//设置文本颜色
	void SetBkColor(COLORREF color);		//设置背景颜色
	void SetTextFont(int FontHight,LPCTSTR FontName);		//设置字体

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CButtonEx)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CButtonEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CButtonEx)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYBUTTON_H__CB4940BF_2442_4FDF_8C7F_6CB245EE1DE8__INCLUDED_)
