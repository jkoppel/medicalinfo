#pragma once
#include "afxwin.h"
#include "ExtLibs\\\\ButtonEx\\ButtonEx.h"
#include "DrawAreaConfigMgt.h"

#define MAX_SPEED_NUM	15

// CDlgDrawAreaSetup 对话框

class CDlgDrawAreaSetup : public CDialog
{
	DECLARE_DYNAMIC(CDlgDrawAreaSetup)

public:
	CDlgDrawAreaSetup(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDrawAreaSetup();

// 对话框数据
	enum { IDD = IDD_DRAWAREA_SETUP };

protected:
	struct DrawAreaConfig m_daConfig;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_ctrlSpeedCurveWidth;
	virtual BOOL OnInitDialog();
	CButtonEx m_ctrlBtnCurveColor[MAX_SPEED_NUM];
	afx_msg void OnBnClickedCurvecolor1();
	afx_msg void OnBnClickedCurvecolor2();
	afx_msg void OnBnClickedCurvecolor3();
	afx_msg void OnBnClickedCurvecolor4();
	afx_msg void OnBnClickedCurvecolor5();
	afx_msg void OnBnClickedCurvecolor6();
	afx_msg void OnBnClickedCurvecolor7();
	afx_msg void OnBnClickedCurvecolor8();
	afx_msg void OnBnClickedCurvecolor9();
	afx_msg void OnBnClickedCurvecolor10();
	afx_msg void OnBnClickedCurvecolor11();
	afx_msg void OnBnClickedCurvecolor12();
	afx_msg void OnBnClickedCurvecolor13();
	afx_msg void OnBnClickedCurvecolor14();
	afx_msg void OnBnClickedCurvecolor15();
	afx_msg void OnBnClickedOk();
};
