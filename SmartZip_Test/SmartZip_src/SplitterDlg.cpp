// SplitterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Smartzip.h"
#include "SplitterDlg.h"
#include "Shlwapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplitterDlg dialog

CSplitterDlg::CSplitterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSplitterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSplitterDlg)
	m_bps = 28800;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	length=-1;
}

void CSplitterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSplitterDlg)
	DDX_Control(pDX, IDOK, m_close);
	DDX_Control(pDX, IDC_EXPORT, m_export);
	DDX_Control(pDX, IDC_CALC, m_calc);
	DDX_Control(pDX, IDC_FILESIZE, m_filesize);
	DDX_Control(pDX, IDC_OUTPUT, m_output);
	DDX_Control(pDX, IDC_VERYCOOL, m_file);
	DDX_Control(pDX, IDC_MSFLEXGRID1, m_flex);
	DDX_Text(pDX, IDC_BPS, m_bps);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSplitterDlg, CDialog)
	//{{AFX_MSG_MAP(CSplitterDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CHOOSE, OnChoose)
	ON_BN_CLICKED(IDC_CALC, OnCalcSpecific)
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplitterDlg message handlers

BOOL CSplitterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	HICON icon=::LoadIcon(::AfxGetApp ()->m_hInstance , MAKEINTRESOURCE(IDI_BROWSE));
	((CButton*)GetDlgItem(IDC_CHOOSE))->SetIcon(icon);
	m_flex.ModifyStyleEx (WS_EX_CLIENTEDGE   ,0);
	m_flex.SetColWidth(0,1400);
	m_flex.SetColWidth(1,2200);
	m_flex.SetColAlignment(0,0);
	m_flex.SetColAlignment(1,0);
	m_flex.SetTextMatrix (0,0,"9600 bps");
	m_flex.SetTextMatrix (1,0,"14.4 Kbps");
	m_flex.SetTextMatrix (2,0,"28.8 Kbps");
	m_flex.SetTextMatrix (3,0,"33.6 Kbps");
	m_flex.SetTextMatrix (4,0,"56 Kbps");
	m_flex.SetTextMatrix (5,0,"ISDN (64 Kbps)");
	m_flex.SetTextMatrix (6,0,"ISDN (128 Kbps)");
	m_flex.SetTextMatrix (7,0,"ASDL (640 Kbps)");
	CButtonST	m_close;
	CButtonST	m_export;
	CButtonST	m_calc;

	COLORREF	crBtnColor = ::GetSysColor(COLOR_BTNFACE) + RGB(30, 30, 30);
	m_close.SetIcon(IDI_CANCEL3);
	m_close.SetColor(CButtonST::BTNST_COLOR_BK_IN, crBtnColor);
	m_close.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 128, 0));


	return TRUE;  // return TRUE  unless you set the focus to a control
}
void CSplitterDlg::OnChoose() 
{
	// TODO: Add your control notification handler code here
CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,"All Files (*.*)|*.*|");
	if(dlg.DoModal() == IDOK)
	{
		m_file.SetWindowText (dlg.GetPathName());

		CString filename;m_file.GetWindowText (filename);
CFile file(	filename,CFile::modeRead |CFile::shareDenyNone     );
length=file.GetLength();
	char buf[20];
	StrFormatByteSize (length,buf,20);
	m_filesize.SetWindowText (buf);
//9600 bps
int time=length*8/9600;
CTime m(1990,1,1,0,0,time);
m_flex.SetTextMatrix (0,1,m.Format ("%H:%M:%S"));
//14.4 bps
time=length*8/14400;
CTime m1(1990,1,1,0,0,time);
m_flex.SetTextMatrix (1,1,m1.Format ("%H:%M:%S"));
//9600 bps
time=length*8/28800;
CTime m2(1990,1,1,0,0,time);
m_flex.SetTextMatrix (2,1,m2.Format ("%H:%M:%S"));
//9600 bps
time=length*8/33600;
CTime m3(1990,1,1,0,0,time);
m_flex.SetTextMatrix (3,1,m3.Format ("%H:%M:%S"));
//9600 bps
time=length*8/56000;
CTime m4(1990,1,1,0,0,time);
m_flex.SetTextMatrix (4,1,m4.Format ("%H:%M:%S"));
//9600 bps
time=length*8/64000;
CTime m5(1990,1,1,0,0,time);
m_flex.SetTextMatrix (5,1,m5.Format ("%H:%M:%S"));
//9600 bps
time=length*8/128000;
CTime m6(1990,1,1,0,0,time);
m_flex.SetTextMatrix (6,1,m6.Format ("%H:%M:%S"));
//9600 bps
time=length*8/640000;
CTime m7(1990,1,1,0,0,time);
m_flex.SetTextMatrix (7,1,m7.Format ("%H:%M:%S"));
	}
}

void CSplitterDlg::OnCalcSpecific() 
{
	// TODO: Add your control notification handler code here
UpdateData();
		CString filename;m_file.GetWindowText (filename);
		if(filename=="") {AfxMessageBox ("Please choose a file first.");return;}

	if(m_bps==0) {AfxMessageBox("Check if you write speed correctly, Please");return;}
int time=length*8/m_bps;
CTime m(1990,1,1,0,0,time);
m_output.SetWindowText (m.Format ("%H:%M:%S"));
	
}

void CSplitterDlg::OnExport() 
{
	// TODO: Add your control notification handler code here
		CString filename;m_file.GetWindowText (filename);
		if(filename=="") {AfxMessageBox ("Please choose a file first.");return;}

	CFileDialog dlg(FALSE,(LPCTSTR)"htm", 
			(LPCTSTR)"list",OFN_CREATEPROMPT|OFN_HIDEREADONLY,"htm Files (*.htm)|*.htm|");
	if(dlg.DoModal() == IDOK)
	{
		CString filename;m_file.GetWindowText (filename);
	//	AfxMessageBox (dlg.GetPathName ());
		CFile file(	dlg.GetPathName (),CFile::modeCreate | CFile::modeWrite  );
	CString filesize;m_filesize.GetWindowText (filesize);
	CString html="";
	html+="<html>"
		"<head>"
		"<meta name=\"GENERATOR\" content=\"Time To Get!\">"
		"<title>";
	html+=filename+" - "+filesize+"</title>";
	html+="</head>"
		"<body>"
	"<basefont face=\"Tahoma, Verdana, Arial\" size=\"2\">"
	"<b> File : </b>"+filename+" - "+filesize;
	html+="<hr><br>"
"<table align=\"center\" border=\"1\" cellspacing=\"0\" width=\"70%\">"
"<tr><th nowrap BGCOLOR=\"#C0C0C0\"><font size=\"2\">Conn. speed</font></th><th nowrap BGCOLOR=\"#C0C0C0\"><font size=\"2\">Estim. Download Time</font></th></tr>"
"<tr><td nowrap BGCOLOR=\"#E0E0E0\"><center><font size=\"2\">9600 bps</font></center></td><td><center><font size=\"2\">"+m_flex.GetTextMatrix (0,1);
//
	html+="</font></center></td></tr>"
"<tr><td nowrap BGCOLOR=\"#E0E0E0\"><center><font size=\"2\">14.4 Kbps</font></center></td><td><center><font size=\"2\">"+m_flex.GetTextMatrix (1,1);
//
	html+="</font></center></td></tr>"
"<tr><td nowrap BGCOLOR=\"#E0E0E0\"><center><font size=\"2\">14.4 Kbps</font></center></td><td><center><font size=\"2\">"+m_flex.GetTextMatrix (2,1);
//
	html+="</font></center></td></tr>"
"<tr><td nowrap BGCOLOR=\"#E0E0E0\"><center><font size=\"2\">14.4 Kbps</font></center></td><td><center><font size=\"2\">"+m_flex.GetTextMatrix (3,1);
//
	html+="</font></center></td></tr>"
"<tr><td nowrap BGCOLOR=\"#E0E0E0\"><center><font size=\"2\">14.4 Kbps</font></center></td><td><center><font size=\"2\">"+m_flex.GetTextMatrix (4,1);
//
	html+="</font></center></td></tr>"
"<tr><td nowrap BGCOLOR=\"#E0E0E0\"><center><font size=\"2\">14.4 Kbps</font></center></td><td><center><font size=\"2\">"+m_flex.GetTextMatrix (5,1);
//
	html+="</font></center></td></tr>"
"<tr><td nowrap BGCOLOR=\"#E0E0E0\"><center><font size=\"2\">14.4 Kbps</font></center></td><td><center><font size=\"2\">"+m_flex.GetTextMatrix (6,1);
//
	html+="</font></center></td></tr>"
"<tr><td nowrap BGCOLOR=\"#E0E0E0\"><center><font size=\"2\">14.4 Kbps</font></center></td><td><center><font size=\"2\">"+m_flex.GetTextMatrix (7,1);

	html+="</font></center></td></tr>"
"</table>"
"<br><hr><br>";
html+="<center>"
		"<font face=\"Arial Black\" size=5><i>SmartZip</font><font face=\"Arial Black\" size=3> 1.0</i></font><br>"
		"<font face=\"Tahoma\" size=2><b>Copyright ©2001, 2002 <a href=\"mailto:ahmed_ismaiel@hotmail.com\">A.I.Z Software</a></b></font>"
		"</body></html>";
	file.Write (html.GetBuffer (html.GetLength ()),html.GetLength ());
html.ReleaseBuffer ();
	}
}
