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
	int			m_Style;			//��ť��״��0-������1-��ǰ��2-���£�3-������
	BOOL		b_InRect;			//�������־
	CString		m_strText;			//��ť����
	COLORREF	m_ForeColor;		//�ı���ɫ
	COLORREF	m_BackColor;		//����ɫ
	COLORREF	m_LockForeColor;	//������ť��������ɫ
	CRect		m_ButRect;			//��ť�ߴ�
	CFont*		p_Font;				//����

	void  DrawButton(CDC *pDC);		//�������İ�ť

// �ӿں���
public:
	void SetText(CString str);
	void SetForeColor(COLORREF color);		//�����ı���ɫ
	void SetBkColor(COLORREF color);		//���ñ�����ɫ
	void SetTextFont(int FontHight,LPCTSTR FontName);		//��������

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
