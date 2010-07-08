#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "..\\Common\\GlobalFuncs.h"
#include "..\\Common\\GlobalVars.h"
#include "..\\Common\\DialogEx.h"
#include "..\\Common\\ButtonEx.h"
#include "..\\Common\\EditEx.h"
#include "..\\Common\\StaticEx.h"
#include "..\\Common\\ComboBoxEx.h"

// CMIMainDlg dialog

typedef struct {
	LVITEM* plvi;
	CString sCol2;
} lvItem, *plvItem;


class CMIMainDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMIMainDlg)

public:
	CMIMainDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMIMainDlg();

// Dialog Data
	enum { IDD = IDD_MICLIENT_MAIN };
	enum { PAGE_SIZE = 10 };
	enum { MODE_ALL=0, MODE_UNPROCESSED, MODE_PROCESSED };

protected:
	void SetConnectStatus(BOOL);

	CString m_strAddress;
	UINT m_nPort;
	CListCtrl m_lstPatient;
	int m_nRecNum;
	BOOL m_bPageMode;
	int m_nCurrPageIndex;
	int m_nPageNum;
	int m_nStatusMode;
	BOOL m_bShowSearchResult;

	COLORREF m_crBackground;
protected:
	CListCtrl* m_pDragList;		//Which ListCtrl we are dragging FROM
	CListCtrl* m_pDropList;		//Which ListCtrl we are dropping ON
	CImageList* m_pDragImage;	//For creating and managing the drag-image
	BOOL		m_bDragging;	//T during a drag operation
	int			m_nDragIndex;	//Index of selected item in the List we are dragging FROM
	int			m_nDropIndex;	//Index at which to drop item in the List we are dropping ON
	CWnd*		m_pDropWnd;		//Pointer to window we are dropping on (will be cast to CListCtrl* type)

	afx_msg LRESULT OnDisconnect(WPARAM wParam, LPARAM lParam);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedConnect();
	virtual BOOL OnInitDialog();
	void SendCurPacket();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedEdit();
	afx_msg void OnBnClickedDelete();

	void InitListBox();
	void UpdateRowData(int index, struct UserData data);
	void UpdateCurrPage();
	void ShowTime();

	BOOL CmdConnect();
	BOOL CmdGetRecordNum(int &num);
	BOOL CmdGetAllIDs(int *pID, int &num, int mode=MODE_ALL);
	BOOL CmdGetRecordByID(int ID, struct UserData &data);
	BOOL CmdAppendRecord(struct UserData &data);
	BOOL CmdDeleteRecordByID(int ID);
	BOOL CmdModifyRecordByID(int ID, struct UserData data);
	BOOL CmdGetNextFreeOrder(int &order);
	BOOL CmdGetOrderByID(int ID, int &order);
	BOOL CmdSetOrderByID(int ID, int order);
	BOOL CmdMoveOrder(int org_order, int dst_order);
	BOOL CmdMoveOrderPrev(int order);
	BOOL CmdMoveOrderNext(int order);
	BOOL CmdSearchByScancodeID(const char *scan_code_id, int *pID, int &num);
public:
	afx_msg void OnBnClickedMoveprev();
public:
	afx_msg void OnBnClickedMovenext();
public:
	afx_msg void OnBnClickedPageFirst();
public:
	afx_msg void OnBnClickedPagePrev();
public:
	afx_msg void OnBnClickedPageNext();
public:
	afx_msg void OnBnClickedPageLast();
public:
	afx_msg void OnBnClickedSwitchPagemode();
public:
	afx_msg void OnLvnBegindragListPatient(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	void DropItemOnList(CListCtrl* pDragList, CListCtrl* pDropList);
	CString m_strScancodeID;
	afx_msg void OnCbnSelchangeStatus();
	afx_msg void OnBnClickedSearch();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCbnSelchangePagemode();
	afx_msg void OnBnClickedSetting();
public:
	CString m_strPageInfo;
	CComboBox m_ctrlStatus;
	CComboBox m_ctrlPageMode;
	CButtonEx m_ctrlConnect;
	CEditEx m_ctrlScancodeID;
	CStaticEx m_ctrlScancodeID_Static;
};
