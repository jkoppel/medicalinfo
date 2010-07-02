#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "GlobalFuncs.h"
#include "GlobalVars.h"


// CMIMainDlg dialog

typedef struct {
	LVITEM* plvi;
	CString sCol2;
} lvItem, *plvItem;


class CMIMainDlg : public CDialog
{
	DECLARE_DYNAMIC(CMIMainDlg)

public:
	CMIMainDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMIMainDlg();

// Dialog Data
	enum { IDD = IDD_MICLIENT_MAIN };
	enum { PAGE_SIZE = 10 };

protected:
	void SetConnectStatus(BOOL);

	CString m_strAddress;
	UINT m_nPort;
	CListCtrl m_lstPatient;
	int m_nRecNum;
	BOOL m_bPageMode;
	int m_nCurrPageIndex;
	int m_nPageNum;
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

	int CmdConnect();
	int CmdGetRecordNum(int &num);
	int CmdGetAllIDs(int *pID, int &num);
	int CmdGetRecordByID(int ID, struct UserData &data);
	int CmdAppendRecord(struct UserData &data);
	int CmdDeleteRecordByID(int ID);
	int CmdModifyRecordByID(int ID, struct UserData data);
	int CmdGetNextFreeOrder(int &order);
	int CmdGetOrderByID(int ID, int &order);
	int CmdSetOrderByID(int ID, int order);
	int CmdMoveOrder(int org_order, int dst_order);
	int CmdMoveOrderPrev(int order);
	int CmdMoveOrderNext(int order);
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
};
