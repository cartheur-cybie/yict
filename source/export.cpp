// export.cpp : import-export
//

#include "stdafx.h"
#include "YICT.h"
#include "maindlg.h"

#include "custom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <direct.h>

//////////////////////////////////////////////////////////


#if 0
// old version
void CMainDlg::OnImport()
{
	char szIo[32];
	sprintf(szIo, "%s.icb", GetBaseName());
	DoImport(szIo, true);
}
#endif

// Import from .ICB file
void CMainDlg::OnImportBehavior() 
{
	char szDef[_MAX_PATH];
	sprintf(szDef, "%s.icb", g_cart.GetBaseName());

	char szOldDir[_MAX_PATH];
	getcwd(szOldDir, sizeof(szOldDir));

    // prompt for .ICB file
	CFileDialog dlg(TRUE, "icb", szDef, 0,
			"ICybie Behavior (*.icb)|*.icb|Older format (*.io)|*.io|All Files (*.*)|*.*||");
 
	bool bDo = (dlg.DoModal() == IDOK);
	chdir(szOldDir);	// do not change cwd

	if (bDo)
	{
		DoImport(dlg.GetPathName(), true);
		OnUpdateGeneral();
		OnSelectCond();	// update
	}
}

////////////////////////////////////////////////

bool DoImport(const char* szFile, bool bReport)
{
	FILE* pf = fopen(szFile, "rt");
	if (pf == NULL)
	{
		if (bReport)
			AfxMessageBox("No .icb file");
		return false;
	}

	int nErr = 0;
	char szLine[256];

	PrepareImportData();
	while (fgets(szLine, sizeof(szLine), pf) != NULL)
	{
		char* pch = strchr(szLine, '\n');
		if (pch != NULL)
			*pch = '\0';
		if (szLine[0] == '\0' || szLine[0] == ';')
			continue;
		if (!ImportDataLine(szLine))
		{
			TRACE("ERROR IN LINE\n");
			nErr++;
		}
	}
	fclose(pf);

	if (bReport)
	{
		if (nErr == 0)
		{
			AfxMessageBox("Imported ok!\n");
		}
		else
		{
			CString str;
			str.Format("ERROR: %d import errors", nErr);
			AfxMessageBox(str);
		}
	}
	return nErr == 0;
}

void CMainDlg::OnExport()
{
	char szIo[32];
	sprintf(szIo, "%s.icb", g_cart.GetBaseName());
	FILE* pf = fopen(szIo, "wt");
	if (pf == NULL)
	{
		AfxMessageBox("Can't open .icb file");
		return;
	}
	bool bOK = true;

	if (!ExportData(pf))
		bOK = false;
	if (fclose(pf) != 0)
		bOK = false;

	if (bOK)
		AfxMessageBox("Exported to .icb");
	else
		AfxMessageBox("ERROR: export data problem");
}


