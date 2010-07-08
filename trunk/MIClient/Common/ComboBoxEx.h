/*
  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	�ļ���BSColorComboBox.h

	��Ȩ��Devia@163.com(Blue Sky) ����

	��������������Ҫ�����ɫѡ����Ͽ�

	���ߣ�������(Devia_Li)	2003.01.15

	��ϵ��(Tel)13512524180	(E_mail)lixiaofei@163.net
  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
*/

#if !defined(_BS_BSCOLORCB)
#define _BS_BSCOLORCB

//!!!Ϊʹ��ģ��CArray/CList֮������������������ͷ�ļ�!!!
#include <afxtempl.h>

//ϵͳ������ɫ���Զ�������
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
	//��ʼ����Ͽ�(��һ�������õĺ���)
	void InitBSColorCB(void);
	//�õ���ǰ����ɫֵ��R/G/Bֵ
	COLORREF GetColor();
	void GetRGBValue(int* R, int* G, int* B);
public:
	//{{AFX_VIRTUAL(CMyComboBoxEx)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

protected:
	bool m_bOverControl;	//����״̬(�Ƿ��ڰ�ť��)	
	int iIconX, iIconY;		//SMALL ICON�Ĵ�С�ߴ�
	COLORREF m_crColor;		//��ǰѡ�е���ɫ
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
