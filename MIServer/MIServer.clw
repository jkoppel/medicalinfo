; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CChatThread
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "miserver.h"
LastPage=0

ClassCount=5
Class1=CChat
Class2=CChatThread
Class3=CMIServerApp
Class4=CMIServerDlg
Class5=CMIServerThread

ResourceCount=4
Resource1=IDD_SERVERTHREAD
Resource2=IDD_MISERVER_DIALOG
Resource3=IDD_CHAT
Resource4=IDD_CHATTHREAD

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

[DLG:IDD_SERVERTHREAD]
Type=1
Class=CMIServerThread
ControlCount=0

