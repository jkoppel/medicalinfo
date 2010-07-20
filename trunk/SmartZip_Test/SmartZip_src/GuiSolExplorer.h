#pragma once
#include "guicontrolbar.h"
#include "GuiTabWnd.h"
#include "GuiContainer.h"
#include "GuiMiniTool.h"
#include "zipexview.h"
//#include "GuiContainer.h"

class CGuiSolExplorer :	public CGuiControlBar
{
public:
	CGuiTabWnd	m_TabSolExplorer;
//	CTreeCtrl*   m_TreeSolExplorer;
	CZipEXView *m_zipview;
//	CTreeCtrl*   m_TreeClassView;
CGuiContainer m_ctComment;
CGuiMiniTool  m_miComment;
	CEditView*   m_CommentView;
//	CImageList	m_imgList;
	CGuiContainer m_ctSolution;
	CGuiMiniTool  m_miSolution;

	CGuiContainer m_ctClassView;
	CGuiMiniTool  m_miClassView;
CDocument *m_pCurrentDoc;
public:

public:
	CGuiSolExplorer(void);
	virtual ~CGuiSolExplorer(void);
	afx_msg void OnSaveComments();
	afx_msg void OnChange();
	int  CreatContExplorer();
	int CreatContClassView();
	virtual BOOL DestroyWindow();
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
};
