/*
  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	文件：BSColorComboBox.h

	版权：Devia@163.com(Blue Sky) 保留

	描述：本程序主要完成颜色选择组合框。

	作者：李晓飞(Devia_Li)	2003.01.15

	联系：(Tel)13512524180	(E_mail)lixiaofei@163.net
  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
*/

#if !defined(_BS_BSCOLORCB)
#define _BS_BSCOLORCB

//!!!为使用模板CArray/CList之类的数据类型所必须的头文件!!!
#include <afxtempl.h>

//系统常用颜色的自定义名称
const static char* strColorName[] = 
{
	"crSCROLLBAR", "crBACKGROUND", "crACTIVECAPTION", "crINACTIVECAPTION", "crMENU", 

	"crWINDOW", "crWINDOWFRAME", "crMENUTEXT", "crWINDOWTEXT", "crCAPTIONTEXT", 

	"crACTIVEBORDER", "crINACTIVEBORDER", "crAPPWORKSPACE", "crHIGHLIGHT", "crHIGHLIGHTTEXT",

	"crBTNFACE", "crBTNSHADOW", "crGRAYTEXT", "crBTNTEXT", "crINACTIVECAPTIONTEXT",

	"crBTNHIGHLIGHT", "cr3DDKSHADOW", "cr3DLIGHT", "crINFOTEXT", "crINFOBK",

	"crHOTLIGHT", "crGRADIENTACTIVECAPTION", "crGRADIENTINACTIVECAPTION"	
};

typedef struct BSCBITEM
{
	int			iIndex;
	COLORREF	crColor;
	LPCTSTR		lpCaption;
}BSCBITEM, *LPBSCBITEM;

class CMyComboBoxEx : public CComboBox
{
	DECLARE_DYNCREATE(CMyComboBoxEx)		
public:
	CMyComboBoxEx();
	virtual ~CMyComboBoxEx();
	//
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//初始化组合框(第一个被调用的函数)
	void InitBSColorCB(void);
	//得到当前的颜色值或R/G/B值
	COLORREF GetColor();
	void GetRGBValue(int* R, int* G, int* B);
public:
	//{{AFX_VIRTUAL(CMyComboBoxEx)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

protected:
	bool m_bOverControl;	//鼠标的状态(是否处于按钮上)	
	int iIconX, iIconY;		//SMALL ICON的大小尺寸
	COLORREF m_crColor;		//当前选中的颜色
	CList<LPBSCBITEM, LPBSCBITEM> m_crItem;
		
	void OnCBPaint(CDC* pDC);
	LPBSCBITEM GetItem(int iIndex = 0);

protected:
	//{{AFX_MSG(CMyComboBoxEx)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSelchange();
	afx_msg void OnSelendok();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
#endif // !defined(_BS_BSCOLORCB)
