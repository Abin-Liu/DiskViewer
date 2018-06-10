; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CDefectDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "DiskViewer.h"
LastPage=0

ClassCount=9
Class1=CDiskViewerApp
Class2=CDiskViewerDoc
Class3=CDiskViewerView
Class4=CMainFrame

ResourceCount=9
Resource1=IDD_DEFDLG (Chinese (P.R.C.))
Resource2=IDD_ABOUTBOX
Class5=CAboutDlg
Resource3=IDD_TOOLTIPDLG (English (U.S.))
Resource4=IDD_TOOLTIPDLG
Class6=CDefDlg
Resource5=IDD_OPENFILEDLG
Resource6=IDR_MAINFRAME
Class7=CTooltipWnd
Resource7=IDD_DEFECTDLG
Class8=COpenFileDlg
Resource8=IDD_ABOUTBOX (English (U.S.))
Class9=CDefectDlg
Resource9=IDR_MAINFRAME (English (U.S.))

[CLS:CDiskViewerApp]
Type=0
HeaderFile=DiskViewer.h
ImplementationFile=DiskViewer.cpp
Filter=N

[CLS:CDiskViewerDoc]
Type=0
HeaderFile=DiskViewerDoc.h
ImplementationFile=DiskViewerDoc.cpp
Filter=N
LastObject=CDiskViewerDoc
BaseClass=CDocument
VirtualFilter=DC

[CLS:CDiskViewerView]
Type=0
HeaderFile=DiskViewerView.h
ImplementationFile=DiskViewerView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC
LastObject=CDiskViewerView


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=ID_FILE_PRINT_PREVIEW
BaseClass=CFrameWnd
VirtualFilter=fWC




[CLS:CAboutDlg]
Type=0
HeaderFile=DiskViewer.cpp
ImplementationFile=DiskViewer.cpp
Filter=D
LastObject=ID_ADDDEFECTION

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_PRINT
Command6=ID_FILE_PRINT_PREVIEW
Command7=ID_FILE_PRINT_SETUP
Command8=ID_FILE_MRU_FILE1
Command9=ID_APP_EXIT
Command10=ID_EDIT_UNDO
Command11=ID_EDIT_CUT
Command12=ID_EDIT_COPY
Command13=ID_EDIT_PASTE
Command14=ID_VIEW_TOOLBAR
Command15=ID_VIEW_STATUS_BAR
Command16=ID_APP_ABOUT
CommandCount=16

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

[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=CMainFrame
Command1=ID_FILE_OPEN
Command2=ID_FILE_SAVE
Command3=ID_FILE_PRINT
Command4=ID_COPYIMG
Command5=ID_HEAD_0
Command6=ID_HEAD_1
Command7=ID_HEAD_2
Command8=ID_HEAD_3
Command9=ID_VIEW_DEFLIST
Command10=ID_APP_ABOUT
CommandCount=10

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=CMainFrame
Command1=ID_FILE_OPEN
Command2=ID_FILE_SAVE_AS
Command3=ID_FILE_PRINT
Command4=ID_FILE_PRINT_PREVIEW
Command5=ID_FILE_PRINT_SETUP
Command6=ID_APP_EXIT
Command7=ID_COPYIMG
Command8=ID_HEAD_0
Command9=ID_HEAD_1
Command10=ID_HEAD_2
Command11=ID_HEAD_3
Command12=ID_VIEW_DEFLIST
Command13=ID_VIEW_TOOLBAR
Command14=ID_VIEW_STATUS_BAR
Command15=ID_APP_ABOUT
CommandCount=15

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_COPYIMG
Command2=ID_VIEW_DEFLIST
Command3=ID_FILE_OPEN
Command4=ID_FILE_PRINT
Command5=ID_FILE_SAVE
Command6=ID_NEXT_PANE
Command7=ID_PREV_PANE
CommandCount=7

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352

[CLS:CDefDlg]
Type=0
HeaderFile=DefDlg.h
ImplementationFile=DefDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CDefDlg
VirtualFilter=dWC

[DLG:IDD_DEFDLG (Chinese (P.R.C.))]
Type=1
Class=CDefDlg
ControlCount=8
Control1=IDC_STATIC,static,1342308352
Control2=IDC_TRACK,edit,1350631552
Control3=IDC_TRACKRG,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_SECT,edit,1350631552
Control6=IDC_SECTORRG,static,1342308352
Control7=IDOK,button,1342242817
Control8=IDCANCEL,button,1342242816

[TB:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
Command9=ID_ADDDEFECTION
CommandCount=9

[DLG:IDD_TOOLTIPDLG]
Type=1
Class=CTooltipWnd
ControlCount=0

[CLS:CTooltipWnd]
Type=0
HeaderFile=TooltipWnd.h
ImplementationFile=TooltipWnd.cpp
BaseClass=CDialog
Filter=D
LastObject=CTooltipWnd
VirtualFilter=dWC

[DLG:IDD_TOOLTIPDLG (English (U.S.))]
Type=1
Class=CTooltipWnd
ControlCount=0

[DLG:IDD_OPENFILEDLG]
Type=1
Class=COpenFileDlg
ControlCount=6
Control1=IDC_STATIC,static,1342308352
Control2=IDC_ZONEFILE,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_DEFECTFILE,button,1342242816
Control5=IDOK,button,1342242817
Control6=IDCANCEL,button,1342242816

[CLS:COpenFileDlg]
Type=0
HeaderFile=OpenFileDlg.h
ImplementationFile=OpenFileDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=COpenFileDlg

[DLG:IDD_DEFECTDLG]
Type=1
Class=CDefectDlg
ControlCount=3
Control1=IDC_LIST1,SysListView32,1350631436
Control2=IDC_ZOOMTO,button,1476460544
Control3=IDCANCEL,button,1342242816

[CLS:CDefectDlg]
Type=0
HeaderFile=DefectDlg.h
ImplementationFile=DefectDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_LIST1

