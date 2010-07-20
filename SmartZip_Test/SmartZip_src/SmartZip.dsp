# Microsoft Developer Studio Project File - Name="SmartZip" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SmartZip - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SmartZip.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SmartZip.mak" CFG="SmartZip - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SmartZip - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "SmartZip - Win32 STATIC Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "ziparchive\\" /I "header\\" /I "SeaShellExt\include\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ZipArcdebug.lib Shlwapi.lib Shell32.lib debug\GuiLib.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SmartZip___Win32_STATIC_Release"
# PROP BASE Intermediate_Dir "SmartZip___Win32_STATIC_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "ziparchive\\" /I "header\\" /I "SeaShellExt\include\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /I "ziparchive\\" /I "header\\" /I "SeaShellExt\include\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Shlwapi.lib ZipArcrelease.lib Shell32.lib guilibrel.lib comctl32.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /incremental:yes /map /debug /nodefaultlib
# ADD LINK32 Shlwapi.lib ZipArcrelease.lib Shell32.lib guilibrel.lib comctl32.lib /nologo /subsystem:windows /machine:I386 /out:"Release/SmartZip1.exe"
# SUBTRACT LINK32 /incremental:yes /map /debug /nodefaultlib

!ENDIF 

# Begin Target

# Name "SmartZip - Win32 Debug"
# Name "SmartZip - Win32 STATIC Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AddChoose.cpp
# End Source File
# Begin Source File

SOURCE=.\AddFilesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AddOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\ArchiveRenamer.cpp
# End Source File
# Begin Source File

SOURCE=.\BarOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\BrowseForFolder.cpp
# End Source File
# Begin Source File

SOURCE=.\BtnST.cpp
# End Source File
# Begin Source File

SOURCE=.\CShellFileOp.cpp
# End Source File
# Begin Source File

SOURCE=.\ExtractDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FileProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\FileView.cpp
# End Source File
# Begin Source File

SOURCE=.\FolderAdd.cpp
# End Source File
# Begin Source File

SOURCE=.\GuiSolExplorer.cpp
# End Source File
# Begin Source File

SOURCE=.\HyperLink.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\msflexgrid.cpp
# End Source File
# Begin Source File

SOURCE=.\MyFavourites.cpp
# End Source File
# Begin Source File

SOURCE=.\mylistctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\NewWizDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\NewWizPage.cpp
# End Source File
# Begin Source File

SOURCE=.\Progress.cpp
# End Source File
# Begin Source File

SOURCE=.\RegistrySerialize.cpp
# End Source File
# Begin Source File

SOURCE=.\SmartZip.cpp
# End Source File
# Begin Source File

SOURCE=.\SmartZip.rc
# End Source File
# Begin Source File

SOURCE=.\SmartZipDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\SmartZipView.cpp
# End Source File
# Begin Source File

SOURCE=.\SplashWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\SplitterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TaskTimer.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeNode.cpp
# End Source File
# Begin Source File

SOURCE=.\VeriPass.cpp
# End Source File
# Begin Source File

SOURCE=.\ZipEXView.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AddChoose.h
# End Source File
# Begin Source File

SOURCE=.\AddFilesDlg.h
# End Source File
# Begin Source File

SOURCE=.\AddOptions.h
# End Source File
# Begin Source File

SOURCE=.\ArchiveRenamer.h
# End Source File
# Begin Source File

SOURCE=.\BarOperation.h
# End Source File
# Begin Source File

SOURCE=.\BrowseForFolder.h
# End Source File
# Begin Source File

SOURCE=.\BtnST.h
# End Source File
# Begin Source File

SOURCE=.\CShellFileOp.h
# End Source File
# Begin Source File

SOURCE=.\ExtractDlg.h
# End Source File
# Begin Source File

SOURCE=.\FileProperties.h
# End Source File
# Begin Source File

SOURCE=.\FileView.h
# End Source File
# Begin Source File

SOURCE=.\FolderAdd.h
# End Source File
# Begin Source File

SOURCE=.\GuiSolExplorer.h
# End Source File
# Begin Source File

SOURCE=.\HyperLink.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\msflexgrid.h
# End Source File
# Begin Source File

SOURCE=.\MyFavourites.h
# End Source File
# Begin Source File

SOURCE=.\mylistctrl.h
# End Source File
# Begin Source File

SOURCE=.\NewWizDialog.h
# End Source File
# Begin Source File

SOURCE=.\NewWizPage.h
# End Source File
# Begin Source File

SOURCE=.\Progress.h
# End Source File
# Begin Source File

SOURCE=.\RegistrySerialize.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SmartZip.h
# End Source File
# Begin Source File

SOURCE=.\SmartZipDoc.h
# End Source File
# Begin Source File

SOURCE=.\SmartZipView.h
# End Source File
# Begin Source File

SOURCE=.\SplashWnd.h
# End Source File
# Begin Source File

SOURCE=.\SplitterDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TaskTimer.h
# End Source File
# Begin Source File

SOURCE=.\TreeNode.h
# End Source File
# Begin Source File

SOURCE=.\VeriPass.h
# End Source File
# Begin Source File

SOURCE=.\ZipEXView.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\back.ico
# End Source File
# Begin Source File

SOURCE=.\Res\backhot.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Bar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\baroper.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Cancel3_32x32x256.ico
# End Source File
# Begin Source File

SOURCE=.\res\dbprojec.bmp
# End Source File
# Begin Source File

SOURCE=.\res\default1.bin
# End Source File
# Begin Source File

SOURCE=.\DockOffice.bmp
# End Source File
# Begin Source File

SOURCE=.\res\DockOffice.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\finish.ico
# End Source File
# Begin Source File

SOURCE=.\Res\finishho.ico
# End Source File
# Begin Source File

SOURCE=.\res\folders.bmp
# End Source File
# Begin Source File

SOURCE=.\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\Res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\Icon_127.ico
# End Source File
# Begin Source File

SOURCE=.\res\Icon_132.ico
# End Source File
# Begin Source File

SOURCE=.\res\Icon_23.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Icon_237.ico
# End Source File
# Begin Source File

SOURCE=.\res\Icon_39.ico
# End Source File
# Begin Source File

SOURCE=.\res\Icon_58.ico
# End Source File
# Begin Source File

SOURCE=.\res\Icon_67.ico
# End Source File
# Begin Source File

SOURCE=.\res\Icon_86.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Left2_32x32x16.ico
# End Source File
# Begin Source File

SOURCE=.\res\mainfram.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\nexthot.ico
# End Source File
# Begin Source File

SOURCE=.\Open.ico
# End Source File
# Begin Source File

SOURCE=.\res\Open.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Right2_32x32x16.ico
# End Source File
# Begin Source File

SOURCE=.\res\SmartZip.ico
# End Source File
# Begin Source File

SOURCE=.\res\SmartZip.rc2
# End Source File
# Begin Source File

SOURCE=.\res\SmartZipDoc.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Splash.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Winzip1.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Winzip2.ico
# End Source File
# Begin Source File

SOURCE=".\res\Zip Document.ico"
# End Source File
# End Group
# Begin Group "seashell"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SeaShellExt\cbformats.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\HtmlCtrl.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\IEFolderTreeCtrl.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\IEShellComboBox.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\IEShellDragDrop.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\IEShellListCtrl.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\IEShellListView.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\IEShellTreeCtrl.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\IEShellTreeView.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\InPlaceEdit.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\LocaleInfo.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\PIDL.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\ShellContextMenu.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\ShellDetails.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\ShellPidl.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\ShellSettings.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\TextParse.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\TextProgressCtrl.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\UIApp.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\UICONT.CPP

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\UICoolBar.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\UICoolMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\Uictrl.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\UIDATA.CPP

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\uidragdropctrl.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\UIDragDropTree.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\uidragimage.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\uidroptarget.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\UIExplorerFrameWnd.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\UIFixTB.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\UIFlatBar.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\UIFrameWnd.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\UIHtmlView.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\UIImageDropTarget.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\UIListView.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\UIMenuBar.cpp
# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\UIModulVer.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\UIStatusBar.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\UISubclass.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\UITabSplitterWnd.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\UITreeCtrl.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\UITreeView.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeaShellExt\WindowPlacement.cpp

!IF  "$(CFG)" == "SmartZip - Win32 Debug"

# ADD CPP /I "SeaShellExt\include"

!ELSEIF  "$(CFG)" == "SmartZip - Win32 STATIC Release"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\res\Data_5.avi
# End Source File
# Begin Source File

SOURCE=.\res\Data_8.avi
# End Source File
# Begin Source File

SOURCE=.\res\Data_9.avi
# End Source File
# Begin Source File

SOURCE=.\res\ex.exe
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\Res\SFX.exe
# End Source File
# End Target
# End Project
