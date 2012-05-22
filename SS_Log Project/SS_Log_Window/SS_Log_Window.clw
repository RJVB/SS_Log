; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=SS_Log_WindowView
LastTemplate=CWinThread
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "SS_Log_Window.h"
LastPage=0

ClassCount=6
Class1=SS_Log_WindowApp
Class2=SS_Log_WindowDoc
Class3=SS_Log_WindowView
Class4=CMainFrame

ResourceCount=2
Resource1=IDD_ABOUTBOX
Class5=CAboutDlg
Class6=SSPipeThread
Resource2=IDR_MAINFRAME

[CLS:SS_Log_WindowApp]
Type=0
HeaderFile=SS_Log_Window.h
ImplementationFile=SS_Log_Window.cpp
Filter=N

[CLS:SS_Log_WindowDoc]
Type=0
HeaderFile=SS_Log_WindowDoc.h
ImplementationFile=SS_Log_WindowDoc.cpp
Filter=N
LastObject=SS_Log_WindowDoc
BaseClass=CDocument
VirtualFilter=DC

[CLS:SS_Log_WindowView]
Type=0
HeaderFile=SS_Log_WindowView.h
ImplementationFile=SS_Log_WindowView.cpp
Filter=C
LastObject=SS_Log_WindowView
BaseClass=CListView
VirtualFilter=VWC


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T




[CLS:CAboutDlg]
Type=0
HeaderFile=SS_Log_Window.cpp
ImplementationFile=SS_Log_Window.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=3
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_OPEN
Command2=ID_FILE_SAVE
Command3=ID_FILE_SAVE_AS
Command4=ID_FILE_PRINT
Command5=ID_FILE_PRINT_PREVIEW
Command6=ID_FILE_PRINT_SETUP
Command7=ID_FILE_MRU_FILE1
Command8=ID_APP_EXIT
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_VIEW_TOOLBAR
Command14=ID_VIEW_STATUS_BAR
Command15=ID_APP_ABOUT
CommandCount=15

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_FILE_SAVE
Command3=ID_EDIT_CUT
Command4=ID_EDIT_COPY
Command5=ID_EDIT_PASTE
Command6=ID_FILE_PRINT
Command7=ID_APP_ABOUT
CommandCount=7

[CLS:SSPipeThread]
Type=0
HeaderFile=SSPipeThread.h
ImplementationFile=SSPipeThread.cpp
BaseClass=CWinThread
Filter=N

[DLG:IDR_MAINFRAME]
Type=1
Class=?
ControlCount=1
Control1=IDC_STATUS,static,1342308352

