#pragma once


// CMFCMenuBarEx

class CMFCMenuBarEx : public CMFCMenuBar
{
	DECLARE_DYNAMIC(CMFCMenuBarEx)

public:
	CMFCMenuBarEx();
	virtual ~CMFCMenuBarEx();

protected:
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


