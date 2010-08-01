// DlgDrawAreaSetup.cpp : 实现文件
//

#include "stdafx.h"
#include "AbsTestDataMan.h"
#include "DlgDrawAreaSetup.h"


// CDlgDrawAreaSetup 对话框

IMPLEMENT_DYNAMIC(CDlgDrawAreaSetup, CDialog)

CDlgDrawAreaSetup::CDlgDrawAreaSetup(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDrawAreaSetup::IDD, pParent)
{

}

CDlgDrawAreaSetup::~CDlgDrawAreaSetup()
{
}

void CDlgDrawAreaSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SPEEDCURVE_WIDTH, m_ctrlSpeedCurveWidth);
	for(int i=0;i<MAX_SPEED_NUM;i++){
		DDX_Control(pDX, IDC_CURVECOLOR_1 + i, m_ctrlBtnCurveColor[i]);
	}
}


BEGIN_MESSAGE_MAP(CDlgDrawAreaSetup, CDialog)
	ON_BN_CLICKED(IDC_CURVECOLOR_1, &CDlgDrawAreaSetup::OnBnClickedCurvecolor1)
	ON_BN_CLICKED(IDC_CURVECOLOR_2, &CDlgDrawAreaSetup::OnBnClickedCurvecolor2)
	ON_BN_CLICKED(IDC_CURVECOLOR_3, &CDlgDrawAreaSetup::OnBnClickedCurvecolor3)
	ON_BN_CLICKED(IDC_CURVECOLOR_4, &CDlgDrawAreaSetup::OnBnClickedCurvecolor4)
	ON_BN_CLICKED(IDC_CURVECOLOR_5, &CDlgDrawAreaSetup::OnBnClickedCurvecolor5)
	ON_BN_CLICKED(IDC_CURVECOLOR_6, &CDlgDrawAreaSetup::OnBnClickedCurvecolor6)
	ON_BN_CLICKED(IDC_CURVECOLOR_7, &CDlgDrawAreaSetup::OnBnClickedCurvecolor7)
	ON_BN_CLICKED(IDC_CURVECOLOR_8, &CDlgDrawAreaSetup::OnBnClickedCurvecolor8)
	ON_BN_CLICKED(IDC_CURVECOLOR_9, &CDlgDrawAreaSetup::OnBnClickedCurvecolor9)
	ON_BN_CLICKED(IDC_CURVECOLOR_10, &CDlgDrawAreaSetup::OnBnClickedCurvecolor10)
	ON_BN_CLICKED(IDC_CURVECOLOR_11, &CDlgDrawAreaSetup::OnBnClickedCurvecolor11)
	ON_BN_CLICKED(IDC_CURVECOLOR_12, &CDlgDrawAreaSetup::OnBnClickedCurvecolor12)
	ON_BN_CLICKED(IDC_CURVECOLOR_13, &CDlgDrawAreaSetup::OnBnClickedCurvecolor13)
	ON_BN_CLICKED(IDC_CURVECOLOR_14, &CDlgDrawAreaSetup::OnBnClickedCurvecolor14)
	ON_BN_CLICKED(IDC_CURVECOLOR_15, &CDlgDrawAreaSetup::OnBnClickedCurvecolor15)
	ON_BN_CLICKED(IDOK, &CDlgDrawAreaSetup::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgDrawAreaSetup 消息处理程序

BOOL CDlgDrawAreaSetup::OnInitDialog()
{
	CDialog::OnInitDialog();

	int i;

	CDrawAreaConfigMgt dacMgt;
	dacMgt.GetDrawAreaConfig(m_daConfig);

	for(i=0;i<MAX_SPEED_NUM;i++){
		m_ctrlBtnCurveColor[i].SetBkColor(m_daConfig.crSpeedCurve[i]);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgDrawAreaSetup::OnBnClickedCurvecolor1()
{
	CColorDialog dlg;
	if(dlg.DoModal()==IDOK){
		m_ctrlBtnCurveColor[0].SetBkColor(dlg.GetColor());
	}
}

void CDlgDrawAreaSetup::OnBnClickedCurvecolor2()
{
	CColorDialog dlg;
	if(dlg.DoModal()==IDOK){
		m_ctrlBtnCurveColor[1].SetBkColor(dlg.GetColor());
	}
}

void CDlgDrawAreaSetup::OnBnClickedCurvecolor3()
{
	CColorDialog dlg;
	if(dlg.DoModal()==IDOK){
		m_ctrlBtnCurveColor[2].SetBkColor(dlg.GetColor());
	}
}

void CDlgDrawAreaSetup::OnBnClickedCurvecolor4()
{
	CColorDialog dlg;
	if(dlg.DoModal()==IDOK){
		m_ctrlBtnCurveColor[3].SetBkColor(dlg.GetColor());
	}
}

void CDlgDrawAreaSetup::OnBnClickedCurvecolor5()
{
	CColorDialog dlg;
	if(dlg.DoModal()==IDOK){
		m_ctrlBtnCurveColor[4].SetBkColor(dlg.GetColor());
	}
}

void CDlgDrawAreaSetup::OnBnClickedCurvecolor6()
{
	CColorDialog dlg;
	if(dlg.DoModal()==IDOK){
		m_ctrlBtnCurveColor[5].SetBkColor(dlg.GetColor());
	}
}

void CDlgDrawAreaSetup::OnBnClickedCurvecolor7()
{
	CColorDialog dlg;
	if(dlg.DoModal()==IDOK){
		m_ctrlBtnCurveColor[6].SetBkColor(dlg.GetColor());
	}
}

void CDlgDrawAreaSetup::OnBnClickedCurvecolor8()
{
	CColorDialog dlg;
	if(dlg.DoModal()==IDOK){
		m_ctrlBtnCurveColor[7].SetBkColor(dlg.GetColor());
	}
}

void CDlgDrawAreaSetup::OnBnClickedCurvecolor9()
{
	CColorDialog dlg;
	if(dlg.DoModal()==IDOK){
		m_ctrlBtnCurveColor[8].SetBkColor(dlg.GetColor());
	}
}

void CDlgDrawAreaSetup::OnBnClickedCurvecolor10()
{
	CColorDialog dlg;
	if(dlg.DoModal()==IDOK){
		m_ctrlBtnCurveColor[9].SetBkColor(dlg.GetColor());
	}
}

void CDlgDrawAreaSetup::OnBnClickedCurvecolor11()
{
	CColorDialog dlg;
	if(dlg.DoModal()==IDOK){
		m_ctrlBtnCurveColor[10].SetBkColor(dlg.GetColor());
	}
}

void CDlgDrawAreaSetup::OnBnClickedCurvecolor12()
{
	CColorDialog dlg;
	if(dlg.DoModal()==IDOK){
		m_ctrlBtnCurveColor[11].SetBkColor(dlg.GetColor());
	}
}

void CDlgDrawAreaSetup::OnBnClickedCurvecolor13()
{
	CColorDialog dlg;
	if(dlg.DoModal()==IDOK){
		m_ctrlBtnCurveColor[12].SetBkColor(dlg.GetColor());
	}
}

void CDlgDrawAreaSetup::OnBnClickedCurvecolor14()
{
	CColorDialog dlg;
	if(dlg.DoModal()==IDOK){
		m_ctrlBtnCurveColor[13].SetBkColor(dlg.GetColor());
	}
}

void CDlgDrawAreaSetup::OnBnClickedCurvecolor15()
{
	CColorDialog dlg;
	if(dlg.DoModal()==IDOK){
		m_ctrlBtnCurveColor[14].SetBkColor(dlg.GetColor());
	}
}

void CDlgDrawAreaSetup::OnBnClickedOk()
{
	int i;
	for(i=0;i<MAX_SPEED_NUM;i++){
		 m_ctrlBtnCurveColor[i].GetBkColor(m_daConfig.crSpeedCurve[i]);
	}
	CDrawAreaConfigMgt dacMgt;
	dacMgt.SetDrawAreaConfig(m_daConfig);

	OnOK();
}
