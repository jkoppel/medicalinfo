
/*
 * File:		SplitterWndEx.h
 *
 * Author:		Marc Richarme <devix@devix.cjb.net>
 *
 * Created:		28 Jan. 2001
 * Modified:	28 Jan. 2001
 *
 * Use as much as you want, wherever you want...
 * Claim you coded it, I don't mind.
 *
 */

#if !defined(AFX_SPLITTERWNDEX_H__61D2F7E7_7AAA_11D1_9F4C_008029E98A75__INCLUDED_)
#define AFX_SPLITTERWNDEX_H__61D2F7E7_7AAA_11D1_9F4C_008029E98A75__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CMySplitterWndEx

class CMySplitterWndEx : public CSplitterWnd
{
public:
	CMySplitterWndEx();
	virtual ~CMySplitterWndEx();

	virtual void OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rectArg);
	virtual void RecalcLayout();    // call after changing sizes

	static void DeferClientPos(AFX_SIZEPARENTPARAMS* lpLayout,
		CWnd* pWnd, int x, int y, int cx, int cy, BOOL bScrollBar);
	static void LayoutRowCol(CSplitterWnd::CRowColInfo* pInfoArray,
		int nMax, int nSize, int nSizeSplitter);

	BOOL IsBarLocked(){return m_bBarLocked;}
	void LockBar(BOOL bState=TRUE){m_bBarLocked=bState;}
	BOOL ReplaceView(int row, int col,CRuntimeClass * pViewClass,SIZE size);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMySplitterWndEx)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CMySplitterWndEx)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bBarLocked;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLATSPLITTERWND_H__61D2F7E7_7AAA_11D1_9F4C_008029E98A75__INCLUDED_)
