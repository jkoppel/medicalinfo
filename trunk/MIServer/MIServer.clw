; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMIServerDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "miserver.h"
LastPage=0

ClassCount=7
Class1=CChat
Class2=CChatThread
Class3=CMIServerApp
Class4=CMIServerDlg
Class5=CMIServerThread

ResourceCount=6
Resource1=IDD_SERVERTHREAD
Resource2=IDD_MISERVER_DIALOG
Resource3=IDD_CHATTHREAD
Resource4=IDD_PATIENT_LIST
Resource5=IDD_CHAT
Class6=CPatientListDlg
Class7=CPatientDlg
Resource6=IDD_PATIENT_ADDMOD

[CLS:CChat]
Type=0
BaseClass=CDialog
HeaderFile=Chat.h
ImplementationFile=Chat.cpp
LastObject=CChat

[CLS:CChatThread]
Type=0
BaseClass=CDialog
HeaderFile=ChatThread.h
ImplementationFile=ChatThread.cpp
LastObject=CChatThread

[CLS:CMIServerApp]
Type=0
BaseClass=CWinApp
HeaderFile=MIServer.h
ImplementationFile=MIServer.cpp
LastObject=CMIServerApp

[CLS:CMIServerDlg]
Type=0
BaseClass=CDialog
HeaderFile=MIServerDlg.h
ImplementationFile=MIServerDlg.cpp
LastObject=CMIServerDlg
Filter=D
VirtualFilter=dWC

[CLS:CMIServerThread]
Type=0
BaseClass=CDialog
HeaderFile=MIServerThread.h
ImplementationFile=MIServerThread.cpp
LastObject=CMIServerThread

[DLG:IDD_CHAT]
Type=1
Class=CChat
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDC_EXIT,button,1342242816
Control3=IDC_RECV,edit,1350631552
Control4=IDC_SEND,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352

[DLG:IDD_CHATTHREAD]
Type=1
Class=CChatThread
ControlCount=0

[DLG:IDD_MISERVER_DIALOG]
Type=1
Class=CMIServerDlg
ControlCount=14
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_RECV,edit,1352728580
Control4=IDC_SEND,edit,1352728580
Control5=IDC_SERVTCP,button,1342308361
Control6=IDC_SERVUDP,button,1342177289
Control7=IDC_STATIC,button,1342177287
Control8=IDC_START,button,1342242816
Control9=IDOK,button,1342242816
Control10=IDC_CLOSE,button,1342242816
Control11=IDC_EXIT,button,1342242816
Control12=IDC_STATIC,static,1342308352
Control13=IDC_PORT,edit,1350631552
Control14=IDC_TESTDB,button,1476460544

[DLG:IDD_SERVERTHREAD]
Type=1
Class=CMIServerThread
ControlCount=0

[DLG:IDD_PATIENT_LIST]
Type=1
Class=CPatientListDlg
ControlCount=4
Control1=IDC_LIST_PATIENT,SysListView32,1350631437
Control2=IDC_ADD,button,1342242816
Control3=IDC_EDIT,button,1342242816
Control4=IDC_DELETE,button,1342242816

[DLG:IDD_PATIENT_ADDMOD]
Type=1
Class=CPatientDlg
ControlCount=44
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_EDIT_SCANCODEID,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_EDIT_NUMBER,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_EDIT_NAME,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_EDIT_SEX,edit,1350631552
Control11=IDC_STATIC,static,1342308352
Control12=IDC_EDIT_AGE,edit,1350631552
Control13=IDC_STATIC,static,1342308352
Control14=IDC_EDIT_BIRTHDATE,edit,1350631552
Control15=IDC_STATIC,static,1342308352
Control16=IDC_EDIT_PEOPLE,edit,1350631552
Control17=IDC_STATIC,static,1342308352
Control18=IDC_EDIT_DEPARTMENT,edit,1350631552
Control19=IDC_STATIC,static,1342308352
Control20=IDC_EDIT_TYPEOFWORK,edit,1350631552
Control21=IDC_STATIC,static,1342308352
Control22=IDC_EDIT_PROVINCE,edit,1350631552
Control23=IDC_STATIC,static,1342308352
Control24=IDC_EDIT_CITY,edit,1350631552
Control25=IDC_STATIC,static,1342308352
Control26=IDC_EDIT_ADDRESS,edit,1350631552
Control27=IDC_STATIC,static,1342308352
Control28=IDC_EDIT_ZIPCODE,edit,1350631552
Control29=IDC_STATIC,static,1342308352
Control30=IDC_EDIT_TEL,edit,1350631552
Control31=IDC_STATIC,static,1342308352
Control32=IDC_EDIT_CLINICALDIAGNOSIS,edit,1350631552
Control33=IDC_STATIC,static,1342308352
Control34=IDC_EDIT_HEIGHT,edit,1350631552
Control35=IDC_STATIC,static,1342308352
Control36=IDC_EDIT_WEIGHT,edit,1350631552
Control37=IDC_STATIC,static,1342308352
Control38=IDC_EDIT_CHECKDATE,edit,1350631552
Control39=IDC_STATIC,static,1342308352
Control40=IDC_EDIT_HAZARDS,edit,1350631552
Control41=IDC_STATIC,static,1342308352
Control42=IDC_EDIT_PHARMACY,edit,1350631552
Control43=IDC_STATIC,static,1342308352
Control44=IDC_EDIT_PASTHISTORY,edit,1350631552

[CLS:CPatientListDlg]
Type=0
HeaderFile=PatientListDlg.h
ImplementationFile=PatientListDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CPatientListDlg

[CLS:CPatientDlg]
Type=0
HeaderFile=PatientDlg.h
ImplementationFile=PatientDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDCANCEL

