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

//---------------------------------------------------------------------------------------

#include "stdafx.h"
#include "mylistctrl.h"

//--------------------------------------------------------------------------------------------

void CMyListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	int nItem = lpDrawItemStruct->itemID;
//	m_fp = fopen((LPCTSTR)m_strFile, "rb");
	char szLine[500]; 
	memset(szLine, NULL, sizeof(szLine));
	long lOffset = (nItem * 16);
//	fseek(m_fp,lOffset, SEEK_SET );
//	fread(szLine, sizeof(char), 17, m_fp);
	memfile->Seek (lOffset,CZipAbstractFile::begin );
	memfile->Read (szLine,17);
szLine[16]=NULL;
	SetToDots(szLine);
//	fclose(m_fp);	
//	m_fp = NULL;

	// Format bytes to Hex
	char szHex[500];
	memset(szHex, NULL, sizeof(szHex));
	sprintf(szHex, "%02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x", 
		szLine[0],szLine[1],szLine[2],szLine[3],szLine[4],szLine[5],szLine[6],szLine[7],szLine[8],
		szLine[9],szLine[10],szLine[11],szLine[12],szLine[13],szLine[14],szLine[15]);
	_strupr(szHex);
	SetToDots(szHex);
	strcat(szHex, "  ");
	strcat(szHex, szLine);
	
	HDC hDC = lpDrawItemStruct->hDC;
	
	int nPos = GetScrollPos(SB_HORZ);

	// HEX widths
	int nCol1Width = m_nColWidth-nPos; 
	int nIdealWidth = ((32 + 16) * m_cxChar)-nPos;
	int nWidth = 0;

	if(nCol1Width > nIdealWidth)
		nWidth = nIdealWidth;
	else
		nWidth = nCol1Width;

	// ASCII widths
	int nCol2Width = m_nColWidth-nPos;
	int nAIdealWidth = ((32 + 16 + 2 + 16) * m_cxChar)-nPos;
	int nAWidth = 0;

	if(nCol2Width >= nAIdealWidth)
		nAWidth = nAIdealWidth;
	else
		nAWidth = nCol2Width;

	CRect tmpRect = lpDrawItemStruct->rcItem;
	tmpRect.right = nWidth;
	CRect rect = tmpRect;

	::FillRect(hDC, &(lpDrawItemStruct->rcItem), (HBRUSH)m_whbrush.m_hObject);
	if(((nItem >= m_nStartRow && nItem <= m_nEndRow) ||	(nItem <= m_nStartRow && nItem >= m_nEndRow)))
	{
		if(nItem == m_nStartRow) // Top Row
		{
			CRect x;
			x.left = (m_nTopRowCharStartIndex * m_cxChar)-nPos;
			x.top = rect.top;
			x.right = (m_nTopRowCharEndIndex * m_cxChar)-nPos;
			x.bottom = rect.bottom;
			// Hex
			::FillRect(hDC, &x, (HBRUSH)m_brush.m_hObject);
			// ASCII
			x.left = ((((16 + 32 + 2) * m_cxChar) + (m_nTopRowCharStartIndex/3) * m_cxChar)) - nPos;
			x.right = ((((16 + 32 + 2) * m_cxChar) + (m_nTopRowCharEndIndex/3) * m_cxChar)) - nPos;
			::FillRect(hDC, &x, (HBRUSH)m_brush.m_hObject);

			// Store details
			m_ASCIITopStartCol = x.left;
			m_ASCIITopEndCol = x.right;
		}
		else
		if(nItem == m_nEndRow) // Bottom Row
		{
			CRect y;
			y.left = (m_nBottRowCharStartIndex * m_cxChar)-nPos;
			y.top = rect.top;
			y.right = (m_nBottRowCharEndIndex * m_cxChar)-nPos;
			y.bottom = rect.bottom;
			::FillRect(hDC, &y, (HBRUSH)m_brush.m_hObject);

			// ASCII
			y.left = ((((16 + 32 + 2) * m_cxChar) + (m_nBottRowCharStartIndex/3) * m_cxChar)) - nPos;
			y.right = ((((16 + 32 + 2) * m_cxChar) + (m_nBottRowCharEndIndex/3) * m_cxChar)) - nPos;
			::FillRect(hDC, &y, (HBRUSH)m_brush.m_hObject);

			// Store details
			m_ASCIIBottStartCol = y.left; 
			m_ASCIIBottEndCol = y.right;
		}
		else
		{
			// Hex
			::FillRect(hDC, &rect, (HBRUSH)m_brush.m_hObject);
			//::InvertRect(hDC, &rect);
			// ASCII
			CRect rect = lpDrawItemStruct->rcItem;
			rect.left= ((16 + 32 + 2) * m_cxChar) - nPos;
			rect.right= nAWidth;
			::FillRect(hDC, &rect, (HBRUSH)m_brush.m_hObject);
			
		}
	}
	// Draw Hex
	::DrawText (hDC, szHex, -1, &(lpDrawItemStruct->rcItem), DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS|DT_VCENTER);
}

//------------------------------------------------------------------------------------------

void CMyListCtrl::SetToDots(char* szLine)
{
	for(int i = 0; i < 16; ++i)
		if(!isprint(szLine[i]) || (szLine[i] < 0 || szLine[i] > 255))
				szLine[i] = 0x2E;
}

//------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)

ON_WM_LBUTTONDOWN()
ON_WM_RBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_MOUSEMOVE()
ON_WM_ACTIVATE()
ON_WM_DRAWITEM_REFLECT() 

END_MESSAGE_MAP()

//------------------------------------------------------------------------------------------

void CMyListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CListCtrl::OnRButtonDown(nFlags, point);

	if((m_nEndRow == m_nStartRow) && (m_ASCIITopStartCol == m_ASCIITopEndCol))
		return;
	
	CMenu menu;
	CPoint tmpPoint = point;
	ClientToScreen(&tmpPoint);

	menu.CreatePopupMenu ( );
	menu.AppendMenu (MF_STRING, COPY_COMMAND, "&Copy");
	int nRet = menu.TrackPopupMenu (TPM_LEFTALIGN|TPM_NONOTIFY|TPM_RETURNCMD, tmpPoint.x, tmpPoint.y, this);
	if(nRet == COPY_COMMAND)
	{
		int nPos = GetScrollPos(SB_HORZ);
		int nCol1Width = m_nColWidth-nPos;
		int nIdealWidth = ((32 + 16 + 2) * m_cxChar)-nPos;
		int nWidth = 0;

		if(nCol1Width > nIdealWidth)
			nWidth = nIdealWidth;
		else
			nWidth = nCol1Width;

		// Get selection
		char szArray[1000];
		char szTemp[100];
	
		memset(szArray, NULL, sizeof(szArray));
		memset(szTemp, NULL, sizeof(szTemp));
			
		if(m_nStartRow <= m_nEndRow) // GREATER (user is moving mouse down)
		{
			int ATopStartIndex = (m_ASCIITopStartCol-nWidth)/m_cxChar; 
			int ATopEndIndex = (m_ASCIITopEndCol-nWidth)/m_cxChar;
			int ABottStartIndex = (m_ASCIIBottStartCol-nWidth)/m_cxChar;
			int ABottEndIndex = (m_ASCIIBottEndCol-nWidth)/m_cxChar;

			for(int i = m_nStartRow; i <= m_nEndRow; i++)
			{
				if(i == m_nStartRow)
				{
					if(ATopStartIndex > ATopEndIndex)
					{
						int nTemp = ATopStartIndex;
						ATopStartIndex = ATopEndIndex;
						ATopEndIndex = nTemp;
					}
					GetSelFromFile(szTemp, sizeof(szTemp), ATopStartIndex, ATopEndIndex, i);
					strcat(szArray, szTemp);
				}
				else
				if(i == m_nEndRow)
				{
					if(ABottStartIndex > ABottEndIndex)
					{
						int nTemp = ABottStartIndex;
						ABottStartIndex = ABottEndIndex;
						ABottEndIndex = nTemp;
					}
					GetSelFromFile(szTemp, sizeof(szTemp), ABottStartIndex, ABottEndIndex, i);
					strcat(szArray, szTemp);
				}
				else // between
				{
					GetSelFromFile(szTemp, sizeof(szTemp), 0, 16, i);
					strcat(szArray, szTemp);
				}
			}
		}
		else // LESSER (user is moving mouse up)
		if(m_nStartRow >= m_nEndRow) 
		{
			int ATopStartIndex = (m_ASCIIBottStartCol-nWidth)/m_cxChar; 
			int ATopEndIndex = (m_ASCIIBottEndCol-nWidth)/m_cxChar;
			int ABottStartIndex = (m_ASCIITopStartCol-nWidth)/m_cxChar;
			int ABottEndIndex = (m_ASCIITopEndCol-nWidth)/m_cxChar;

			for(int i = m_nEndRow; i <= m_nStartRow; i++)
			{
				if(i == m_nStartRow)
				{
					if(ABottStartIndex > ABottEndIndex)
					{
						int nTemp = ABottStartIndex;
						ABottStartIndex = ABottEndIndex;
						ABottEndIndex = nTemp;
					}
					GetSelFromFile(szTemp, sizeof(szTemp), ABottStartIndex, ABottEndIndex, i);
					strcat(szArray, szTemp);
				}
				else
				if(i == m_nEndRow)
				{
					if(ATopStartIndex > ATopEndIndex)
					{
						int nTemp = ATopStartIndex;
						ATopStartIndex = ATopEndIndex;
						ATopEndIndex = nTemp;
					}
					GetSelFromFile(szTemp, sizeof(szTemp), ATopStartIndex, ATopEndIndex, i);
					strcat(szArray, szTemp);
				}
				else // between
				{
					GetSelFromFile(szTemp, sizeof(szTemp), 0, 16, i);
					strcat(szArray, szTemp);
				}
			}
		}

		// Copy data to clipboard 
		HGLOBAL	hglob = ::GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE, sizeof(szArray));
		PVOID pData = ::GlobalLock(hglob);
		char *pTemp = (char*)pData;
		CopyMemory (pTemp, szArray, sizeof(szArray));
		pTemp += sizeof(szArray)-1;
		*pTemp = NULL;
		::GlobalUnlock(hglob);
		::OpenClipboard(m_hWnd);
		::EmptyClipboard();
		::SetClipboardData(CF_TEXT, hglob);
		::CloseClipboard();
	}

	menu.DestroyMenu();
}

//------------------------------------------------------------------------------------------

void CMyListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	//CListCtrl::OnLButtonDown(nFlags, point);
	SetCapture();
	Invalidate(FALSE);
	int nPos = GetScrollPos(SB_HORZ);
	int nCol1Width = m_nColWidth-nPos;
	int nIdealWidth = ((32 + 16) * m_cxChar)-nPos;
	int nWidth = 0;

	if(nCol1Width > nIdealWidth)
		nWidth = nIdealWidth;
	else
		nWidth = nCol1Width;

	if(point.x >= nWidth)
		return;

	int nVPos = GetScrollPos(SB_VERT);
	int off = nVPos * 14;
	int cnt = GetItemCount() * 14;
	if((point.y + off) > cnt)
		return;


	int orgXStartCol = 0;
	m_nStartRow = m_nEndRow = 0;
	m_nTempEndRow = m_nEndRow = m_nStartRow = HitTest(point);
	
	m_BottRowStartCol = 0;
	m_BottRowEndCol = 0;
	m_TopRowStartCol = 0;
	m_TopRowEndCol = 0;
	
	orgXStartCol -= nPos;
	while(orgXStartCol < point.x)
		orgXStartCol += m_cxChar * 3;
	
	orgXStartCol = point.x < 7 ? 0 : orgXStartCol;
	
	m_TopRowStartCol = orgXStartCol;
	m_StoreColX = orgXStartCol;
	m_TopRowEndCol = orgXStartCol;
	m_BottRowStartCol = orgXStartCol;
	m_BottRowEndCol = orgXStartCol;

	m_nTopRowCharStartIndex = (m_TopRowStartCol+nPos)/m_cxChar;
	m_nTopRowCharEndIndex = (m_TopRowEndCol+nPos)/m_cxChar;
	m_nBottRowCharStartIndex = (m_BottRowStartCol+nPos)/m_cxChar;
	m_nBottRowCharEndIndex = (m_BottRowEndCol+nPos)/m_cxChar;
	m_bLBTN = TRUE;
}

//------------------------------------------------------------------------------------------

void CMyListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_bLBTN = FALSE;
}

//------------------------------------------------------------------------------------------

void CMyListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	int nPos = GetScrollPos(SB_HORZ);
	
	int nCol1Width = m_nColWidth-nPos;
	int nIdealWidth = ((32 + 16) * m_cxChar)-nPos;
	int nWidth = 0;

	if(nCol1Width > nIdealWidth)
		nWidth = nIdealWidth; 
	else
		nWidth = nCol1Width;

	CRect rect;
	GetClientRect(&rect);
		
	if(point.x >= nWidth || point.y >= rect.bottom || point.y >= (GetItemCount() * 14) || point.x <= 0 || point.y <= 0)
		return;

	int nVPos = GetScrollPos(SB_VERT);
	int off = nVPos * 14;
	int cnt = GetItemCount() * 14;


	if(m_bLBTN)
	{
		if((point.y + off) >= cnt)
			return;

		m_nEndRow = HitTest(point);
		int orgXEndCol = 0;
		
		orgXEndCol -= nPos;
		while(orgXEndCol <= point.x)
			orgXEndCol += m_cxChar*3;
		
		orgXEndCol = point.x < 7 ? 0 : orgXEndCol;
				
		if(m_nEndRow < m_nTempEndRow)
		{
			if(!m_bLesser)
			{
				m_TopRowEndCol = m_TopRowStartCol;
				m_nTopRowCharEndIndex = (m_TopRowEndCol+nPos)/m_cxChar;
				m_StoreColX = m_TopRowStartCol;
				m_TopRowStartCol = 0;
				m_nTopRowCharStartIndex = 0;
				m_bLesser = TRUE;
			}
			m_BottRowStartCol = orgXEndCol;
			m_BottRowEndCol = nWidth;
			m_nBottRowCharStartIndex = (m_BottRowStartCol+nPos)/m_cxChar;
			m_nBottRowCharEndIndex = (m_BottRowEndCol+nPos)/m_cxChar;
		}
		else
		{
			m_bLesser = FALSE;
			if(m_TopRowStartCol == 0)
			{
				m_TopRowStartCol = m_StoreColX;
				m_nTopRowCharStartIndex = (m_TopRowStartCol+nPos)/m_cxChar;
			}
			if(m_nStartRow == m_nEndRow)
			{
				m_nTopRowCharStartIndex = (m_TopRowStartCol+nPos)/m_cxChar;
				m_nTopRowCharEndIndex = (m_TopRowEndCol+nPos)/m_cxChar;
				m_TopRowEndCol = orgXEndCol;
			}
			else
			{
				m_TopRowEndCol = nWidth;
				m_nTopRowCharEndIndex = (nWidth+nPos)/m_cxChar;
				m_BottRowStartCol = 0;
				m_BottRowEndCol = orgXEndCol;
				m_nBottRowCharStartIndex = 0;
				m_nBottRowCharEndIndex = (m_BottRowEndCol+nPos)/m_cxChar;
			}
		}
		
		rect.right = nCol1Width;
		InvalidateRect(&rect, FALSE);
		UpdateWindow();
	}
}

//------------------------------------------------------------------------------------------

void CMyListCtrl::LoadFile(CZipMemFile* memfile1, int nFileSize)
{
		Reset();
//		m_strFile = strFile;
		memfile=memfile1;

		m_nFileSize = nFileSize;		
		// Set the item count, so we will get the item no. to be drawn in OnDraw.
		int nItems = m_nFileSize/16; // 16 bytes per row
		int nRem = m_nFileSize % 16; // Anything left over?
		if(nRem) // If yes, add an extra item to hold the left over bytes
			++nItems;
		SetItemCount(nItems);
		InsertColumn(0, "Hex Display");
		CRect rect;
		GetClientRect(&rect);
		int nWidth = (32 + 16 + 2 + 16) * m_cxChar;
		SetColumnWidth(0, nWidth);
}

//------------------------------------------------------------------------------------------

void CMyListCtrl::PreSubclassWindow()
{
	m_nStartCol = 0;
	m_nStartRow = 0;
	m_nEndCol = 0;
	m_nEndRow = 0;
	m_bLBTN = FALSE;
	m_cxChar = 8;
	m_TopRowStartCol = 0;
	m_TopRowEndCol = 0;
	m_BottRowStartCol = 0;
	m_BottRowEndCol = 0;
	m_nTempEndRow = 0;
	m_StoreColX = 0;
	m_nColWidth = (32 + 16 + 2 + 16) * m_cxChar;
	m_bLesser = FALSE;
	m_ASCIITopStartCol = 0;
	m_ASCIITopEndCol = 0;
	m_ASCIIBottStartCol = 0;
	m_ASCIIBottEndCol = 0;
	m_nStartRowStore = 0;
	m_nEndRowStore = 0;
	m_brush.CreateSolidBrush(RGB(0, 255, 0)); 
	m_whbrush.CreateSolidBrush(GetSysColor(COLOR_WINDOW));
}

//------------------------------------------------------------------------------------------

void CMyListCtrl::Reset()
{ 
	m_nStartCol = 0;
	m_nStartRow = 0;
	m_nEndCol = 0;
	m_nEndRow = 0;
	m_TopRowStartCol = 0;
	m_TopRowEndCol = 0;
	m_BottRowStartCol = 0;
	m_BottRowEndCol = 0;
	m_StoreColX = 0;
	m_nTempEndRow = 0;
	m_cxChar = 8;
	m_nColWidth = (32 + 16 + 2 + 16) * m_cxChar;
	m_nTopRowCharStartIndex = 0;
	m_nTopRowCharEndIndex = 0;
	m_nBottRowCharStartIndex = 0;
	m_nBottRowCharEndIndex = 0;
	m_ASCIITopStartCol = 0;
	m_ASCIITopEndCol = 0;
	m_ASCIIBottStartCol = 0;
	m_ASCIIBottEndCol = 0;
	m_nStartRowStore = 0;
	m_nEndRowStore = 0;
	m_nFileSize = 0;
	m_bLBTN = FALSE;
	m_bLesser = FALSE;
}

//------------------------------------------------------------------------------------------
	
void CMyListCtrl::GetSelFromFile(char* szTemp, const int& nTempSize, 
					const int& nStartIndex, const int& nEndIndex, const int& nRow)
{
	memset(szTemp, NULL, nTempSize);
//	FILE* fp = fopen((LPSTR)(LPCTSTR)m_strFile, "rb");
//	fseek(fp, (long)(nRow * 16), SEEK_SET);
//	fseek(fp, (long)(nStartIndex), SEEK_CUR);
	memfile->Seek ((long)(nRow * 16),CZipAbstractFile::begin );
	memfile->Seek ((long)(nStartIndex),CZipAbstractFile::current);
	
	memfile->Read (szTemp,nEndIndex-nStartIndex);
//szTemp[nTempSize] = NULL;
/*	int nTemp = nStartIndex;
	int index = 0;
	while(nTemp != nEndIndex)
	{
		szTemp[index++] = fgetc(fp);
		++nTemp;
	}
	
	fclose(fp);*/
}

//------------------------------------------------------------------------------------------
