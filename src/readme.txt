Source code to YICT202 tool

Freeware, but still copyrighted (c) 2002-2003 ICybiePet
Please keep the credit to ICybiePet intact

-----------
Brief notes:

Requires Visual C++ to build. Version 6 recommended.

Please read general warnings in ..\readme.txt

-----------
Interesting files:

..\yictdata202.h - data structure for HIMAGE/YICT extensions

custom.cpp, custom.h - somewhat abstracted customization
	conditions.c_ - conditions
	mot2aud.c_ - which motions have sounds (for reporting)
	..\unames.h, ..\unames2.h - standardized action names

sounddlg.cpp, sounddlg.h - sound preview feature
	sunadpcm.c_ - decoding the SunPlus ADPCM

maindlg.cpp, maindlg.h - main dialog
	selectlist.h - helper class for droplists
customdlg.cpp, customdlg.h - add and change dialogs
opendlg.cpp, opendlg.h - open dialog

cart.cpp, cart.h - somewhat abstracted cartridge interface
        (load two .BIN files)
export.cpp - export and import

yict.cpp, yict.h - main
yict.rc, resource.h - general resources

yict.dsp, yict.dsw - Visual C++ 6.0 project file

-----------
