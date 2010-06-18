; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMIServerDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "MIServer.h"

ClassCount=3
Class1=CMIServerApp
Class2=CMIServerDlg

ResourceCount=3
Resource2=IDD_MISERVER_DIALOG
Resource1=IDR_MAINFRAME
Class3=CChat
Resource3=IDD_CHAT

[CLS:CMIServerApp]
Type=0
HeaderFile=MIServer.h
ImplementationFile=MIServer.cpp
Filter=N

[CLS:CMIServerDlg]
Type=0
HeaderFile=MIServerDlg.h
ImplementationFile=MIServerDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CMIServerDlg



[DLG:IDD_MISERVER_DIALOG]
Type=1
Class=CMIServerDlg
ControlCount=13
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

[CLS:CChat]
Type=0
HeaderFile=Chat.h
ImplementationFile=Chat.cpp
BaseClass=CDialog
Filter=D
LastObject=CChat
VirtualFilter=dWC

