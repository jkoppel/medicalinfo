// PatientListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "miserver.h"
#include "PatientListDlg.h"
#include "PatientDlg.h"
#include "GlobalFuncs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPatientListDlg dialog


CPatientListDlg::CPatientListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPatientListDlg::IDD, pParent)
{
	m_nPatientNum = 0;
	//{{AFX_DATA_INIT(CPatientListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPatientListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatientListDlg)
	DDX_Control(pDX, IDC_LIST_PATIENT, m_lstPatient);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPatientListDlg, CDialog)
	//{{AFX_MSG_MAP(CPatientListDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatientListDlg message handlers

BOOL CPatientListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitListBox();

	GetPatientData();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///设置ListBox项目栏
void CPatientListDlg::InitListBox()
{
	m_lstPatient.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE);

	int i = 0;
	m_lstPatient.InsertColumn(i++, "ID", LVCFMT_LEFT, 80);
	m_lstPatient.InsertColumn(i++, "扫描码", LVCFMT_LEFT, 80);
	m_lstPatient.InsertColumn(i++, "病按号", LVCFMT_LEFT, 80);
	m_lstPatient.InsertColumn(i++, "姓名", LVCFMT_LEFT, 60);
	m_lstPatient.InsertColumn(i++, "性别", LVCFMT_LEFT, 60);
	m_lstPatient.InsertColumn(i++, "年龄", LVCFMT_LEFT, 60);
	m_lstPatient.InsertColumn(i++, "出生日期", LVCFMT_LEFT, 80);
	m_lstPatient.InsertColumn(i++, "民族", LVCFMT_LEFT, 60);
	m_lstPatient.InsertColumn(i++, "工作部门", LVCFMT_LEFT, 100);
	m_lstPatient.InsertColumn(i++, "工种", LVCFMT_LEFT, 100);
	m_lstPatient.InsertColumn(i++, "省份", LVCFMT_LEFT, 60);
	m_lstPatient.InsertColumn(i++, "城市", LVCFMT_LEFT, 60);
	m_lstPatient.InsertColumn(i++, "详细地址", LVCFMT_LEFT, 120);
	m_lstPatient.InsertColumn(i++, "邮编", LVCFMT_LEFT, 60);
	m_lstPatient.InsertColumn(i++, "电话", LVCFMT_LEFT, 80);
	m_lstPatient.InsertColumn(i++, "临床诊断", LVCFMT_LEFT, 100);
	m_lstPatient.InsertColumn(i++, "身高", LVCFMT_LEFT, 60);
	m_lstPatient.InsertColumn(i++, "体重", LVCFMT_LEFT, 60);
	m_lstPatient.InsertColumn(i++, "检查日期", LVCFMT_LEFT, 80);
	m_lstPatient.InsertColumn(i++, "危险因素", LVCFMT_LEFT, 100);
	m_lstPatient.InsertColumn(i++, "用药", LVCFMT_LEFT, 100);
	m_lstPatient.InsertColumn(i++, "既往病史", LVCFMT_LEFT, 100);
}

void CPatientListDlg::GetPatientData()
{
	char buf[256];
	_RecordsetPtr	pHandlerRecordset;

	m_lstPatient.DeleteAllItems();
	pHandlerRecordset.CreateInstance(__uuidof(Recordset));
	try
	{
		pHandlerRecordset->Open("SELECT * FROM Case_Data ORDER BY ID DESC",// 查询表中所有字段
		g_pDBConnection.GetInterfacePtr(),	 // 获取库接库的IDispatch指针
		adOpenDynamic,
		adLockOptimistic,
		adCmdText);
	}
	catch(_com_error *e)
	{
		AfxMessageBox(e->ErrorMessage());
	}  
				
	_variant_t var;

	while(!pHandlerRecordset->adoEOF)
	{
		var = pHandlerRecordset->GetCollect("ID");
		if(var.vt != VT_NULL){
			m_nID = (UINT)(long)(var);
		}
		else{
			AfxMessageBox("读取ID号错误");
			return;
		}

		var = pHandlerRecordset->GetCollect("Address");
		if(var.vt != VT_NULL){
			m_strAddress = (LPCSTR)_bstr_t(var);
		}
		else{
			m_strAddress.Empty();
		}

		var = pHandlerRecordset->GetCollect("Age");
		if(var.vt != VT_NULL){
			m_nAge = (UINT)(long)(var);
		}
		else{
			m_nAge = -1;
		}

		var = pHandlerRecordset->GetCollect("Birth_date");
		if(var.vt != VT_NULL){
			m_strBirthDate = (LPCSTR)_bstr_t(var);
		}
		else{
			m_strBirthDate.Empty();
		}

		var = pHandlerRecordset->GetCollect("Check_Date");
		if(var.vt != VT_NULL){
			m_strCheckDate = (LPCSTR)_bstr_t(var);
		}
		else{
			m_strCheckDate.Empty();
		}

		var = pHandlerRecordset->GetCollect("City");
		if(var.vt != VT_NULL){
			m_strCity = (LPCSTR)_bstr_t(var);
		}
		else{
			m_strCity.Empty();
		}

		var = pHandlerRecordset->GetCollect("Clinical_Diagnosis");
		if(var.vt != VT_NULL){
			m_strClinicalDiagnosis = (LPCSTR)_bstr_t(var);
		}
		else{
			m_strClinicalDiagnosis.Empty();
		}

		var = pHandlerRecordset->GetCollect("Department");
		if(var.vt != VT_NULL){
			m_strDepartment = (LPCSTR)_bstr_t(var);
		}
		else{
			m_strDepartment.Empty();
		}

		var = pHandlerRecordset->GetCollect("Hazards");
		if(var.vt != VT_NULL){
			m_strHazards = (LPCSTR)_bstr_t(var);
		}
		else{
			m_strHazards.Empty();
		}

		var = pHandlerRecordset->GetCollect("Name");
		if(var.vt != VT_NULL){
			m_strName = (LPCSTR)_bstr_t(var);
		}
		else{
			m_strName.Empty();
		}

		var = pHandlerRecordset->GetCollect("Number");
		if(var.vt != VT_NULL){
			m_nNumber = (UINT)(long)(var);
		}
		else{
			m_nNumber = -1;
		}

		var = pHandlerRecordset->GetCollect("Height");
		if(var.vt != VT_NULL){
			m_nHeight = (UINT)(long)(var);
		}
		else{
			m_nHeight = -1;
		}

		var = pHandlerRecordset->GetCollect("Past_History");
		if(var.vt != VT_NULL){
			m_strPastHistory = (LPCSTR)_bstr_t(var);
		}
		else{
			m_strPastHistory.Empty();
		}

		var = pHandlerRecordset->GetCollect("People");
		if(var.vt != VT_NULL){
			m_strPeople = (LPCSTR)_bstr_t(var);
		}
		else{
			m_strPeople.Empty();
		}

		var = pHandlerRecordset->GetCollect("Pharmacy");
		if(var.vt != VT_NULL){
			m_strPharmacy = (LPCSTR)_bstr_t(var);
		}
		else{
			m_strPharmacy.Empty();
		}

		var = pHandlerRecordset->GetCollect("Province");
		if(var.vt != VT_NULL){
			m_strProvince = (LPCSTR)_bstr_t(var);
		}
		else{
			m_strProvince.Empty();
		}

		var = pHandlerRecordset->GetCollect("ScancodeID");
		if(var.vt != VT_NULL){
			m_strScancodeID = (LPCSTR)_bstr_t(var);
		}
		else{
			m_strScancodeID.Empty();
		}

		var = pHandlerRecordset->GetCollect("Sex");
		if(var.vt != VT_NULL){
			m_strSex = (LPCSTR)_bstr_t(var);
		}
		else{
			m_strSex.Empty();
		}

		var = pHandlerRecordset->GetCollect("Tel");
		if(var.vt != VT_NULL){
			m_strTel = (LPCSTR)_bstr_t(var);
		}
		else{
			m_strTel.Empty();
		}

		var = pHandlerRecordset->GetCollect("Type_Of_Work");
		if(var.vt != VT_NULL){
			m_strTypeOfWork = (LPCSTR)_bstr_t(var);
		}
		else{
			m_strTypeOfWork.Empty();
		}

		var = pHandlerRecordset->GetCollect("Weight");
		if(var.vt != VT_NULL){
			m_strWeight = (LPCSTR)_bstr_t(var);
		}
		else{
			m_strWeight.Empty();
		}

		var = pHandlerRecordset->GetCollect("ZipCode");
		if(var.vt != VT_NULL){
			m_strZipCode = (LPCSTR)_bstr_t(var);
		}
		else{
			m_strZipCode.Empty();
		}


		int i=0;
		m_lstPatient.InsertItem(m_nPatientNum, "病人记录");
		sprintf(buf, "%d", m_nID);
		m_lstPatient.SetItemText(m_nPatientNum, i++, buf);
		m_lstPatient.SetItemText(m_nPatientNum, i++, m_strScancodeID);
		sprintf(buf, "%d", m_nNumber);
		m_lstPatient.SetItemText(m_nPatientNum, i++, buf);
		m_lstPatient.SetItemText(m_nPatientNum, i++, m_strName);
		m_lstPatient.SetItemText(m_nPatientNum, i++, m_strSex);
		sprintf(buf, "%d", m_nAge);
		m_lstPatient.SetItemText(m_nPatientNum, i++, buf);
		m_lstPatient.SetItemText(m_nPatientNum, i++, m_strBirthDate);
		m_lstPatient.SetItemText(m_nPatientNum, i++, m_strPeople);
		m_lstPatient.SetItemText(m_nPatientNum, i++, m_strDepartment);
		m_lstPatient.SetItemText(m_nPatientNum, i++, m_strTypeOfWork);
		m_lstPatient.SetItemText(m_nPatientNum, i++, m_strProvince);
		m_lstPatient.SetItemText(m_nPatientNum, i++, m_strCity);
		m_lstPatient.SetItemText(m_nPatientNum, i++, m_strAddress);
		m_lstPatient.SetItemText(m_nPatientNum, i++, m_strZipCode);
		m_lstPatient.SetItemText(m_nPatientNum, i++, m_strTel);
		m_lstPatient.SetItemText(m_nPatientNum, i++, m_strClinicalDiagnosis);
		sprintf(buf, "%d", m_nHeight);
		m_lstPatient.SetItemText(m_nPatientNum, i++, buf);
		m_lstPatient.SetItemText(m_nPatientNum, i++, m_strWeight);
		m_lstPatient.SetItemText(m_nPatientNum, i++, m_strCheckDate);
		m_lstPatient.SetItemText(m_nPatientNum, i++, m_strHazards);
		m_lstPatient.SetItemText(m_nPatientNum, i++, m_strPharmacy);
		m_lstPatient.SetItemText(m_nPatientNum, i++, m_strPastHistory);

		pHandlerRecordset->MoveNext();
	}
	pHandlerRecordset->Close();
	pHandlerRecordset.Release();
	pHandlerRecordset = NULL;
}

void CPatientListDlg::OnAdd() 
{
	CPatientDlg dlg;
	if(IDOK==dlg.DoModal()){
		CString strSql;
		strSql.Format("INSERT INTO Case_Data(ScancodeID,[Number],Name,Sex,Age,Birth_date,People,Department,Type_Of_Work,Province,City,Address,ZipCode,Tel,Clinical_Diagnosis,Height,Weight,Check_Date,Hazards,Pharmacy,Past_History) VALUES('%s',%d,'%s','%s',%d,'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s',%d,'%s','%s','%s','%s','%s')",
						dlg.m_strScancodeID,
						dlg.m_nNumber,
						dlg.m_strName,
						dlg.m_strSex,
						dlg.m_nAge,
						dlg.m_strBirthDate,
						dlg.m_strPeople,
						dlg.m_strDepartment,
						dlg.m_strTypeOfWork,
						dlg.m_strProvince,
						dlg.m_strCity,
						dlg.m_strAddress,
						dlg.m_strZipCode,
						dlg.m_strTel,
						dlg.m_strClinicalDiagnosis,
						dlg.m_nHeight,
						dlg.m_strWeight,
						dlg.m_strCheckDate,
						dlg.m_strHazards,
						dlg.m_strPharmacy,
						dlg.m_strPastHistory
					);
		AfxMessageBox(strSql);
		_variant_t vAffected;
		g_pDBConnection->Execute(_bstr_t(strSql),&vAffected,adCmdText);
		GetPatientData();
	}
}

void CPatientListDlg::OnDelete() 
{
	POSITION pos;
	_RecordsetPtr	pHandlerRecordset;
	_variant_t RecordsAffected;

	pos = m_lstPatient.GetFirstSelectedItemPosition();

	if(pos<=0){
		AfxMessageBox("请先选择要记录.");
		return;
	}

	if(MessageBox("是否要删除该记录?", "确认删除", MB_YESNO)!=IDYES){
		return;
	}

	UINT index = (unsigned int)pos-1;
	char buf[100];
	CString strSql;
	m_lstPatient.GetItemText(index, 0, buf, 20);
	strSql.Format("%s%s", "DELETE FROM Case_Data WHERE ID=", buf);
	g_pDBConnection->Execute(_bstr_t(strSql),&RecordsAffected,adCmdText);

	GetPatientData();
}

void CPatientListDlg::OnEdit() 
{
	POSITION pos;
	_RecordsetPtr	pHandlerRecordset;

	pos = m_lstPatient.GetFirstSelectedItemPosition();

	if(pos<=0){
		AfxMessageBox("请先选择要记录.");
		return;
	}

	UINT index = (unsigned int)pos-1;
	char buf[100], buf1[100];
	m_lstPatient.GetItemText(index, 0, buf, 20);
	sprintf(buf1, "%s%s", "SELECT * FROM Case_Data WHERE ID=", buf);
	
	pHandlerRecordset.CreateInstance(__uuidof(Recordset));
	try
	{
		pHandlerRecordset->Open(buf1,// 查询表中所有字段
		g_pDBConnection.GetInterfacePtr(),	 // 获取库接库的IDispatch指针
		adOpenDynamic,
		adLockOptimistic,
		adCmdText);
	}
	catch(_com_error *e)
	{
		AfxMessageBox(e->ErrorMessage());
	}  

	if(pHandlerRecordset->adoEOF){
		AfxMessageBox("未找到记录");
	}
				
	_variant_t var;
	CPatientDlg dlg;

	var = pHandlerRecordset->GetCollect("Address");
	if(var.vt != VT_NULL){
		dlg.m_strAddress = (LPCSTR)_bstr_t(var);
	}
	else{
		dlg.m_strAddress.Empty();
	}

	var = pHandlerRecordset->GetCollect("Age");
	if(var.vt != VT_NULL){
		dlg.m_nAge = (UINT)(long)(var);
	}
	else{
		dlg.m_nAge = -1;
	}

	var = pHandlerRecordset->GetCollect("Birth_date");
	if(var.vt != VT_NULL){
		dlg.m_strBirthDate = (LPCSTR)_bstr_t(var);
	}
	else{
		m_strBirthDate.Empty();
	}

	var = pHandlerRecordset->GetCollect("Check_Date");
	if(var.vt != VT_NULL){
		dlg.m_strCheckDate = (LPCSTR)_bstr_t(var);
	}
	else{
		dlg.m_strCheckDate.Empty();
	}

	var = pHandlerRecordset->GetCollect("City");
	if(var.vt != VT_NULL){
		dlg.m_strCity = (LPCSTR)_bstr_t(var);
	}
	else{
		dlg.m_strCity.Empty();
	}

	var = pHandlerRecordset->GetCollect("Clinical_Diagnosis");
	if(var.vt != VT_NULL){
		dlg.m_strClinicalDiagnosis = (LPCSTR)_bstr_t(var);
	}
	else{
		dlg.m_strClinicalDiagnosis.Empty();
	}

	var = pHandlerRecordset->GetCollect("Department");
	if(var.vt != VT_NULL){
		dlg.m_strDepartment = (LPCSTR)_bstr_t(var);
	}
	else{
		dlg.m_strDepartment.Empty();
	}

	var = pHandlerRecordset->GetCollect("Hazards");
	if(var.vt != VT_NULL){
		dlg.m_strHazards = (LPCSTR)_bstr_t(var);
	}
	else{
		dlg.m_strHazards.Empty();
	}

	var = pHandlerRecordset->GetCollect("Name");
	if(var.vt != VT_NULL){
		dlg.m_strName = (LPCSTR)_bstr_t(var);
	}
	else{
		dlg.m_strName.Empty();
	}

	var = pHandlerRecordset->GetCollect("Number");
	if(var.vt != VT_NULL){
		dlg.m_nNumber = (UINT)(long)(var);
	}
	else{
		dlg.m_nNumber = -1;
	}

	var = pHandlerRecordset->GetCollect("Height");
	if(var.vt != VT_NULL){
		dlg.m_nHeight = (UINT)(long)(var);
	}
	else{
		dlg.m_nHeight = -1;
	}

	var = pHandlerRecordset->GetCollect("Past_History");
	if(var.vt != VT_NULL){
		dlg.m_strPastHistory = (LPCSTR)_bstr_t(var);
	}
	else{
		dlg.m_strPastHistory.Empty();
	}

	var = pHandlerRecordset->GetCollect("People");
	if(var.vt != VT_NULL){
		dlg.m_strPeople = (LPCSTR)_bstr_t(var);
	}
	else{
		dlg.m_strPeople.Empty();
	}

	var = pHandlerRecordset->GetCollect("Pharmacy");
	if(var.vt != VT_NULL){
		dlg.m_strPharmacy = (LPCSTR)_bstr_t(var);
	}
	else{
		dlg.m_strPharmacy.Empty();
	}

	var = pHandlerRecordset->GetCollect("Province");
	if(var.vt != VT_NULL){
		dlg.m_strProvince = (LPCSTR)_bstr_t(var);
	}
	else{
		dlg.m_strProvince.Empty();
	}

	var = pHandlerRecordset->GetCollect("ScancodeID");
	if(var.vt != VT_NULL){
		dlg.m_strScancodeID = (LPCSTR)_bstr_t(var);
	}
	else{
		dlg.m_strScancodeID.Empty();
	}

	var = pHandlerRecordset->GetCollect("Sex");
	if(var.vt != VT_NULL){
		dlg.m_strSex = (LPCSTR)_bstr_t(var);
	}
	else{
		dlg.m_strSex.Empty();
	}

	var = pHandlerRecordset->GetCollect("Tel");
	if(var.vt != VT_NULL){
		dlg.m_strTel = (LPCSTR)_bstr_t(var);
	}
	else{
		dlg.m_strTel.Empty();
	}

	var = pHandlerRecordset->GetCollect("Type_Of_Work");
	if(var.vt != VT_NULL){
		dlg.m_strTypeOfWork = (LPCSTR)_bstr_t(var);
	}
	else{
		dlg.m_strTypeOfWork.Empty();
	}

	var = pHandlerRecordset->GetCollect("Weight");
	if(var.vt != VT_NULL){
		dlg.m_strWeight = (LPCSTR)_bstr_t(var);
	}
	else{
		dlg.m_strWeight.Empty();
	}

	var = pHandlerRecordset->GetCollect("ZipCode");
	if(var.vt != VT_NULL){
		dlg.m_strZipCode = (LPCSTR)_bstr_t(var);
	}
	else{
		dlg.m_strZipCode.Empty();
	}

	if(IDOK==dlg.DoModal()){
		CString strSql;
		strSql.Format("UPDATE Case_Data SET ScancodeID='%s',[Number]=%d,Name='%s',Sex='%s',Age=%d,Birth_date='%s',People='%s',Department='%s',Type_Of_Work='%s',Province='%s',City='%s',Address='%s',ZipCode='%s',Tel='%s',Clinical_Diagnosis='%s',Height=%d,Weight='%s',Check_Date='%s',Hazards='%s',Pharmacy='%s',Past_History='%s' WHERE ID=%s",
						dlg.m_strScancodeID,
						dlg.m_nNumber,
						dlg.m_strName,
						dlg.m_strSex,
						dlg.m_nAge,
						dlg.m_strBirthDate,
						dlg.m_strPeople,
						dlg.m_strDepartment,
						dlg.m_strTypeOfWork,
						dlg.m_strProvince,
						dlg.m_strCity,
						dlg.m_strAddress,
						dlg.m_strZipCode,
						dlg.m_strTel,
						dlg.m_strClinicalDiagnosis,
						dlg.m_nHeight,
						dlg.m_strWeight,
						dlg.m_strCheckDate,
						dlg.m_strHazards,
						dlg.m_strPharmacy,
						dlg.m_strPastHistory,
						buf
					);
		_variant_t vAffected;
		g_pDBConnection->Execute(_bstr_t(strSql),&vAffected,adCmdText);
		GetPatientData();
	}
}
