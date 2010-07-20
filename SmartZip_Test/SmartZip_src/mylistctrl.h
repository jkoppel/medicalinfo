//---------------------------------------------------------------------------------------

// Quick Hex Shell extension
// Copyright (c) 2000 by Shanker.C
// All rights reserved
// Author's consent required if this program is to be used for commercial purposes
// No warranty of any kind, expressed or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the user.
// Please send comments/suggestions/criticisms to: Shanker@xlprint.com
// March 7, 2000

//--------------------------------------------------------------------------------

#ifndef _MYLISTCTRL_H_
#define _MYLISTCTRL_H_

//--------------------------------------------------------------------------------

#include <afxcmn.h>
#include "ZipMemFile.h"
//--------------------------------------------------------------------------------

#define COPY_COMMAND 50000

//--------------------------------------------------------------------------------

class CMyListCtrl : public CListCtrl
{
	public:

		void LoadFile(CZipMemFile* memfile, int nFileSize);
		
	private:

		int m_nStartCol;
		int m_nStartRow;
		int m_nEndCol;
		int m_nEndRow;
		int m_TopRowStartCol;
		int m_TopRowEndCol;
		int m_BottRowStartCol;
		int m_BottRowEndCol;
		int m_StoreColX;
		int m_nTempEndRow;
		int m_cxChar;
		int m_nColWidth;
		int m_nTopRowCharStartIndex;
		int m_nTopRowCharEndIndex;
		int m_nBottRowCharStartIndex;
		int m_nBottRowCharEndIndex;
		int m_ASCIITopStartCol;
		int m_ASCIITopEndCol;
		int	m_ASCIIBottStartCol;
		int	m_ASCIIBottEndCol;
		int m_nStartRowStore;
		int m_nEndRowStore;
		int m_nFileSize;

		BOOL m_bLBTN;
		BOOL m_bLesser;

//		CString m_strFile;
//		FILE* m_fp;
CZipMemFile* memfile;

		// White and selection brushes
		CBrush m_brush;
		CBrush m_whbrush;

		
	protected:

		// Helper functions
		void GetSelFromFile(char* szTemp, const int& nTempSize, 
					const int& nStartIndex, const int& nEndIndex, const int& nRow);
		void SetToDots(char* szLine);
		void Reset();
	
	protected:

		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg void DrawItem (LPDRAWITEMSTRUCT lpDrawItemStruct); 
		afx_msg void OnSize(UINT nType, int cx, int cy);
		virtual void PreSubclassWindow();
				
		DECLARE_MESSAGE_MAP()
};

//--------------------------------------------------------------------------------

#endif //_MYLISTCTRL_H_

//--------------------------------------------------------------------------------
