#pragma once


// CMFCStatusBarEx

class CMFCStatusBarEx : public CMFCStatusBar
{
	DECLARE_DYNAMIC(CMFCStatusBarEx)

public:
	CMFCStatusBarEx();
	virtual ~CMFCStatusBarEx();

protected:
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


