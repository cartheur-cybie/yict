// YICT.h : main header file for the YICT application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMainApp:
// See YICT.cpp for the implementation of this class
//

class CMainApp : public CWinApp
{
public:
	CMainApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMainApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

#define TABSTOP_PERCENT		20
#define TABSTOP_STDNAME		30

/////////////////////////////////////////////////////////////////////////////

