// YICT.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "YICT.h"
#include "maindlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "custom.h"


/////////////////////////////////////////////////////////////////////////////
// CMainApp

BEGIN_MESSAGE_MAP(CMainApp, CWinApp)
	//{{AFX_MSG_MAP(CMainApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainApp construction

CMainApp::CMainApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMainApp object

CMainApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMainApp initialization

#include "opendlg.h"

BOOL CMainApp::InitInstance()
{
	if (strncmp(m_lpCmdLine, "-i:", 3) == 0)
	{
		// special import processing mode
		const char* szBase = m_lpCmdLine+3;
		bool bOrig;
		if (!g_cart.LoadRomImage(szBase, bOrig))
		{
			AfxMessageBox("Failed to load cartridge image");
			return FALSE;
		}

		if (!bOrig)
		{
			AfxMessageBox("Doing automerge on non-original copy");
			// probably bad
		}
		if (!ExtractAllPerfData())
		{
			AfxMessageBox("IMPORT interpretation error");
			return FALSE;
		}
		
		char szIo[_MAX_PATH];
		sprintf(szIo, "%s.icb", szBase); 
		if (!DoImport(szIo, false))
		{
			AfxMessageBox("IMPORT ERROR");
			return FALSE;
		}
		if (!SaveChanges(bOrig))
		{
			AfxMessageBox("Save error");
			return FALSE;
		}
		return FALSE; // ok
	}

	COpenDlg dlgO;
	if (dlgO.DoModal() != IDOK)
		return FALSE;


	bool bOrig;
    if (!g_cart.LoadRomImage(dlgO.m_selected, bOrig))
    {
        AfxMessageBox("Failed to load cartridge image");
        return FALSE;
    }



	if (bOrig)
	{
		CNewNameDlg dlg;
		char szName[LEN_CARTTYPE], szBaseName[LEN_CARTTYPE];
		g_cart.GetRomName(szName, szBaseName);
		dlg.m_name = "My" + CString(szBaseName).SpanExcluding(" ");
		if (dlg.DoModal() != IDOK)
			return FALSE;

		g_cart.SetBaseName(dlg.m_name);
	}

	if (!ExtractAllPerfData())
	{
		AfxMessageBox("Sorry, I can't understand the customization information\n"
			"Please save the broken version, re-install and try again");
		return FALSE;
	}

	char szName[LEN_CARTTYPE], szBaseName[LEN_CARTTYPE];
	g_cart.GetRomName(szName, szBaseName);

	CMainDlg dlg;
	m_pMainWnd = &dlg;
	dlg.m_name = szName;
	dlg.m_basename = szBaseName;
	dlg.DoModal();

	return FALSE;
}
