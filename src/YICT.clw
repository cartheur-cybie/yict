; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMainDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "yict.h"
LastPage=0

ClassCount=8
Class1=CPromptActionDlg
Class2=CPromptMultiActionDlg
Class3=CAboutDlg
Class4=CMainDlg
Class5=COpenDlg
Class6=CNewNameDlg
Class7=CMainApp

ResourceCount=7
Resource1=IDD_YICT_DIALOG
Resource2=IDD_OPENDIALOG
Resource3=IDD_MULTIACTION
Resource4=IDD_ABOUTBOX
Resource5=IDD_NEWACTION
Resource6=IDD_NEWNAME
Class8=CSoundPreviewDlg
Resource7=IDD_SOUNDPREVIEW

[CLS:CPromptActionDlg]
Type=0
BaseClass=CDialog
HeaderFile=customdlg.h
ImplementationFile=customdlg.cpp

[CLS:CPromptMultiActionDlg]
Type=0
BaseClass=CDialog
HeaderFile=customdlg.h
ImplementationFile=customdlg.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=maindlg.cpp
ImplementationFile=maindlg.cpp

[CLS:CMainDlg]
Type=0
BaseClass=CDialog
HeaderFile=maindlg.h
ImplementationFile=maindlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=IDC_COMBO_DARK

[CLS:COpenDlg]
Type=0
BaseClass=CDialog
HeaderFile=opendlg.h
ImplementationFile=opendlg.cpp
LastObject=COpenDlg

[CLS:CNewNameDlg]
Type=0
BaseClass=CDialog
HeaderFile=opendlg.h
ImplementationFile=opendlg.cpp

[CLS:CMainApp]
Type=0
BaseClass=CWinApp
HeaderFile=YICT.h
ImplementationFile=YICT.cpp

[DLG:IDD_NEWACTION]
Type=1
Class=CPromptActionDlg
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_LIST1,listbox,1352728961

[DLG:IDD_MULTIACTION]
Type=1
Class=CPromptMultiActionDlg
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_LIST1,listbox,1352728969

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352

[DLG:IDD_YICT_DIALOG]
Type=1
Class=CMainDlg
ControlCount=33
Control1=IDC_COMBO_IDLE,combobox,1344339971
Control2=IDC_COMBO_STRETCH,combobox,1344339971
Control3=IDC_COMBO_WAG,combobox,1344339971
Control4=IDC_NOCHARGER,button,1342259203
Control5=IDC_MUTEINIT,button,1342259203
Control6=IDC_COMBO3,combobox,1344339971
Control7=IDC_COMBO4,combobox,1344339971
Control8=IDC_COMBO5,combobox,1344339971
Control9=IDC_IMPORT_BEHAVIOR,button,1342242816
Control10=IDC_PREVIEW_SOUNDS,button,1342242816
Control11=IDC_TREE1,SysTreeView32,1350631463
Control12=IDC_LIST2,listbox,1352731009
Control13=IDC_ADD_ACTION,button,1342242816
Control14=IDC_CHANGE_ACTION,button,1342242816
Control15=IDC_REMOVE_REACTION,button,1342242816
Control16=ID_SAVE,button,1342242817
Control17=ID_SAVE_AND_TRANSFER,button,1476468736
Control18=ID_SAVE_AND_TRANSFER2,button,1476468736
Control19=IDC_DUAL,button,1073807360
Control20=IDC_STATIC,button,1342177543
Control21=IDC_STATIC,static,1342308352
Control22=IDC_STATIC,button,1342177543
Control23=IDC_STATIC,static,1342308352
Control24=IDC_STATIC,static,1342308352
Control25=IDC_STATIC,static,1342308352
Control26=IDC_TYPE,static,1342312588
Control27=IDC_STATIC,static,1342308352
Control28=IDC_STATIC,static,1342308352
Control29=IDC_STATIC,static,1342308352
Control30=IDC_STATIC,button,1342177543
Control31=IDC_STATIC,static,1342308352
Control32=IDC_TYPE2,static,1342312588
Control33=IDC_STATIC,static,1342308352

[DLG:IDD_OPENDIALOG]
Type=1
Class=COpenDlg
ControlCount=3
Control1=IDOK,button,1476460545
Control2=IDC_STATIC,static,1342308352
Control3=IDC_LIST1,listbox,1352728835

[DLG:IDD_NEWNAME]
Type=1
Class=CNewNameDlg
ControlCount=2
Control1=IDC_EDIT1,edit,1350631552
Control2=IDOK,button,1342242817

[CLS:CSoundPreviewDlg]
Type=0
HeaderFile=sounddlg.h
ImplementationFile=sounddlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CSoundPreviewDlg

[DLG:IDD_SOUNDPREVIEW]
Type=1
Class=CSoundPreviewDlg
ControlCount=2
Control1=IDC_LIST1,listbox,1352728833
Control2=IDC_PLAY,button,1342242816

