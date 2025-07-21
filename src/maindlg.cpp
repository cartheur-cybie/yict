// maindlg.cpp : implementation file
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

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainDlg dialog

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMainDlg)
	m_name = _T("");
	m_basename = _T("");
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMainDlg)
//	DDX_Control(pDX, IDC_MAKESIT, m_btnMakeSit);
	DDX_Control(pDX, IDC_MUTEINIT, m_checkMuteInit);
	DDX_Control(pDX, IDC_DUAL, m_btnDual);
	DDX_Control(pDX, IDC_NOCHARGER, m_checkDisableWUC);
	DDX_Control(pDX, ID_SAVE_AND_TRANSFER2, m_saveAndTransferButtonS);
	DDX_Control(pDX, IDC_COMBO5, m_sslTiming);
	DDX_Control(pDX, ID_SAVE_AND_TRANSFER, m_saveAndTransferButton);
	DDX_Control(pDX, IDC_COMBO3, m_mfChoice);
	DDX_Control(pDX, IDC_COMBO4, m_sslChoice);
	DDX_Control(pDX, IDC_TREE1, m_condTree);
	DDX_Control(pDX, IDC_COMBO_STRETCH, m_stretchChoice);
	DDX_Control(pDX, IDC_COMBO_WAG, m_wagTailChoice);
	DDX_Control(pDX, IDC_CHANGE_ACTION, m_changeButton);
	DDX_Control(pDX, IDC_ADD_ACTION, m_addButton);
	DDX_Control(pDX, IDC_REMOVE_REACTION, m_removeButton);
	DDX_Control(pDX, IDC_LIST2, m_actionList);
	DDX_Control(pDX, IDC_COMBO_IDLE, m_idleTimeChoice);
	DDX_Text(pDX, IDC_TYPE, m_name);
	DDX_Text(pDX, IDC_TYPE2, m_basename);

	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	//{{AFX_MSG_MAP(CMainDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_LBN_SELCHANGE(IDC_LIST2, OnSelectReaction)
	ON_BN_CLICKED(IDC_ADD_ACTION, OnAddAction)
	ON_BN_CLICKED(IDC_REMOVE_REACTION, OnRemoveReaction)
	ON_BN_CLICKED(IDC_CHANGE_ACTION, OnChangeAction)
	ON_CBN_SELCHANGE(IDC_COMBO_WAG, OnSelchangeWagTail)
	ON_CBN_SELCHANGE(IDC_COMBO_STRETCH, OnSelchangeStretch)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnSelectCondition)
	ON_CBN_SELCHANGE(IDC_COMBO3, OnSelchangeCombo3)
	ON_CBN_SELCHANGE(IDC_COMBO4, OnSelchangeCombo4)
	ON_BN_CLICKED(ID_SAVE_AND_TRANSFER, OnSaveAndTransfer)
	ON_BN_CLICKED(ID_SAVE, OnSave)
	ON_CBN_SELCHANGE(IDC_COMBO5, OnSelchangeCombo5)
	ON_BN_CLICKED(IDC_IMPORT_BEHAVIOR, OnImportBehavior)
	ON_BN_CLICKED(ID_SAVE_AND_TRANSFER2, OnSaveAndTransfer2)
	ON_BN_CLICKED(IDC_PREVIEW_SOUNDS, OnPreviewSounds)
	ON_BN_CLICKED(IDC_DUAL, OnMakeDual)
	ON_BN_CLICKED(IDC_NOCHARGER, OnChangeChecks)
	ON_CBN_SELCHANGE(IDC_COMBO_IDLE, OnSelchangeIdleTime)
	ON_LBN_DBLCLK(IDC_LIST2, OnChangeAction)
	ON_BN_CLICKED(IDC_MUTEINIT, OnChangeChecks)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainDlg message handlers

BOOL CMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	ASSERT((IDM_IMPORT & 0xFFF0) == IDM_IMPORT);
	ASSERT(IDM_IMPORT < 0xF000);
	ASSERT((IDM_EXPORT & 0xFFF0) == IDM_EXPORT);
	ASSERT(IDM_EXPORT < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
#if 0
		pSysMenu->AppendMenu(MF_STRING, IDM_IMPORT, "Import");
#endif
		pSysMenu->AppendMenu(MF_STRING, IDM_EXPORT, "Export");
		pSysMenu->AppendMenu(MF_STRING, IDM_ADDTOOLS, "Pro Tools");
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, "About...");
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	///////////////////////////
	// Fill Condition Listbox
	int iCond = 0;
	HTREEITEM hCat = NULL;
	bool bExpandThisCat = false;
    while (1)
	{
		const char* szCat = NULL;
		const char* szName = GetCondName(iCond, szCat);
		if (szName == NULL)
            break;

		if (szCat != NULL)
		{
			if (bExpandThisCat)
				m_condTree.Expand(hCat, TVE_EXPAND);

			// new category
			bExpandThisCat = false;
			if (szCat[0] == '+')
			{
				bExpandThisCat = true;
				szCat++;
			}
			hCat = m_condTree.InsertItem(szCat);
			m_condTree.SetItemData(hCat, -1);

		}
		ASSERT(hCat != NULL);
		HTREEITEM hItem = m_condTree.InsertItem(szName, hCat);
		m_condTree.SetItemData(hItem, iCond);
        iCond++;
	}
	if (bExpandThisCat)
		m_condTree.Expand(hCat, TVE_EXPAND);

	m_condTree.SelectSetFirstVisible(m_condTree.GetChildItem(TVI_ROOT));
	// UI setup
	int stops[] = { TABSTOP_PERCENT, TABSTOP_PERCENT+TABSTOP_STDNAME };
	m_actionList.SetTabStops(2, stops);


	InitGeneralProperties(m_idleTimeChoice, m_stretchChoice, m_wagTailChoice,
		m_mfChoice, m_sslChoice, m_sslTiming,
		m_checkDisableWUC, m_checkMuteInit);

	OnUpdateGeneral();

	OnSelectCond();	// disables buttons

	//BLOCK: enable transfer button if tools in same directory
	{
		FILE* pf;
		pf = fopen("sicburn.exe", "rb");
		if (pf != NULL)
		{
			m_saveAndTransferButtonS.EnableWindow(TRUE);
			fclose(pf);
		}

		pf = fopen("icburn.exe", "rb");
		if (pf != NULL)
		{
			m_saveAndTransferButton.EnableWindow(TRUE);
			fclose(pf);
		}
	}

	///////////////////////////

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	UINT id = nID & 0xFFF0;
	if (id == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
#if 0
	else if (id == IDM_IMPORT)
	{
		OnImport(); // now a main-stream feature
	}
#endif
	else if (id == IDM_EXPORT)
	{
		OnExport();
    }
	else if (id == IDM_ADDTOOLS)
	{
        m_btnDual.ShowWindow(TRUE);
        // m_btnMakeSit.ShowWindow(TRUE);
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CMainDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this);
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CMainDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMainDlg::OnOK() 
{
	AfxMessageBox("oops");
}

void CMainDlg::OnCancel() 
{
	if (g_cart.IsDirty() && AfxMessageBox("Changes not saved!\nCancel anyway?", MB_YESNO) != IDYES)
		return;
	CDialog::OnCancel();
}

////////////////////////////////////////////////////////////////////////////////////

int CMainDlg::GetCond()
{
	HTREEITEM hItem = m_condTree.GetSelectedItem();
	if (hItem == NULL)
		return -1;
	int iCond = m_condTree.GetItemData(hItem);
	return iCond;
}


void CMainDlg::OnSelectReaction() 
{
	int iCond = GetCond();
	if (iCond == -1)
		return;

	m_removeButton.EnableWindow(IsMultiCond(iCond));
}


void CMainDlg::OnRemoveReaction() 
{
	int iCond = GetCond();
	if (iCond == -1)
		return;

	// delete in reverse order
	for (int iItem = m_actionList.GetCount()-1; iItem >= 0; iItem--)
		if (m_actionList.GetSel(iItem) > 0)
			DeletePerformance(iCond, iItem);
	RebalanceProbabilities(iCond);
	OnSelectCond();	
}

void CMainDlg::OnAddAction() 
{
	int iCond = GetCond();
	if (iCond == -1)
		return;

	int selections[MAX_ACTIONS_PER_COND];
    int nOld = GetCurrentActions(iCond, selections);
	if (nOld == -1 || nOld+1 >= MAX_ACTIONS_PER_COND)
	{
		AfxMessageBox("Can't add any more");
		return;
	}
	m_actionList.SetSel(-1, FALSE);	// unselect all
	int iAction = PromptAddItem();
	if (iAction < 0)
		return;
	AddPerformance(iCond, iAction);	// adds at end
	RebalanceProbabilities(iCond);
	OnSelectCond();
}

void CMainDlg::OnChangeAction() 
{
	int iCond = GetCond();
	if (iCond == -1)
		return;

	if (IsMultiCond(iCond))
	{
		// multi-selection editing
		int selections[MAX_ACTIONS_PER_COND];
	    int nOld = GetCurrentActions(iCond, selections);
		if (nOld == -1)
		{
			AfxMessageBox("Unknown action used. Can't edit this safely");
            return;
		}

		m_actionList.SetSel(-1, TRUE);	// select all during changes
		int nNew = PromptChangeItems(selections, nOld);
		if (nNew >= 0)
		{
			for (int i = nOld-1; i >= 0; i--)
				DeletePerformance(iCond, i);
			for (/*int*/ i = 0; i < nNew; i++)
				AddPerformance(iCond, selections[i]);
			RebalanceProbabilities(iCond);
		}
	}		
	else
	{
		// single selection editing
		int iAction = PromptAddItem(GetCurrentAction(iCond));
		if (iAction < 0)
			return;
		ChangeSinglePerformance(iCond, iAction);
	}
	OnSelectCond();
}

void CMainDlg::OnSelectCondition(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
	OnSelectCond();
}

void CMainDlg::OnSelectCond()
{
	m_addButton.EnableWindow(FALSE);
	m_changeButton.EnableWindow(FALSE);
	m_removeButton.EnableWindow(FALSE);
	m_actionList.ResetContent();

	int iCond = GetCond();
	if (iCond == -1)
		return;

	// read-only 
	int iCondIter = StartPerfIterate(iCond);
	if (iCondIter == -1)
	{
		m_actionList.AddString("(do nothing)");
		m_actionList.EnableWindow(FALSE);
		m_addButton.EnableWindow(TRUE);

		ASSERT(IsMultiCond(iCond));
	}
	else
	{
		int percentLast = 0;
		int percent;
		CString strStdName;
		CString strActionName;
		while (percentLast < 100)
		{
			ParsePerfData(iCond, iCondIter, percent, strStdName, strActionName);
			ASSERT(percent > percentLast);
			CString str;

			if (percent == 999)
				str.Format("\t#%s\t%s", (const char*)strStdName, (const char*)strActionName);
			else
				str.Format("%d%%\t#%s\t%s", percent - percentLast, (const char*)strStdName, (const char*)strActionName);
			m_actionList.AddString(str);
			percentLast = percent;
		}


		m_actionList.EnableWindow(TRUE);

		ASSERT(percentLast == 100 || percentLast == 999);
			// 100 for PROBACTION, 999 for single action
		m_addButton.EnableWindow(percentLast != 999);
	}
	m_changeButton.EnableWindow(TRUE);
	m_removeButton.EnableWindow(FALSE);
}

/////////////////////////////////////////////////////////////////////////////////////////
// General Options

void CMainDlg::OnUpdateGeneral()
{
	m_idleTimeChoice.Update();
	m_stretchChoice.Update();
	m_wagTailChoice.Update();
	m_mfChoice.Update();
	m_sslChoice.Update();
	m_sslTiming.Update();

}

void CMainDlg::OnSelchangeIdleTime() 
{	
	m_idleTimeChoice.Select();
	OnUpdateGeneral();
}

void CMainDlg::OnSelchangeWagTail() 
{
	m_wagTailChoice.Select();
	OnUpdateGeneral();
}

void CMainDlg::OnSelchangeStretch() 
{
	m_stretchChoice.Select();
	OnUpdateGeneral();
}


void CMainDlg::OnSelchangeCombo3() 
{
	m_mfChoice.Select();
	OnUpdateGeneral();
}

void CMainDlg::OnSelchangeCombo4() 
{
	m_sslChoice.Select();
	OnUpdateGeneral();
}

void CMainDlg::OnSelchangeCombo5() 
{
	m_sslTiming.Select();
	OnUpdateGeneral();
}


void CMainDlg::OnChangeChecks() 
{
	// change checkboxes
	g_cart.GetYD()->boolDisableCharger = m_checkDisableWUC.GetCheck() != 0;	
	g_cart.GetYD()->boolMute = m_checkMuteInit.GetCheck() != 0;	
	g_cart.SetDirty(true);
	OnUpdateGeneral(); // not necessary
}

void CMainDlg::DoSaveAndTransfer(const char* szToolFmt)
{
	if (g_cart.IsDirty())
	{
		if (!SaveChanges())
			return;
	}

	char szCmd[128];
	sprintf(szCmd, szToolFmt, g_cart.GetBaseName());
	// AfxMessageBox(szCmd);

	CString strOldTitle;
	GetWindowText(strOldTitle);
	ShowWindow(SW_MINIMIZE);
	SetWindowText("YICT - minimized");
	
	system(szCmd);

	SetWindowText(strOldTitle);
	ShowWindow(SW_RESTORE);

	// stay running
}

void CMainDlg::OnSaveAndTransfer() 
{
	char szT[128];
	GetPrivateProfileString("YICT", "ICBURN_CMD", "icburn.exe %s", szT, sizeof(szT), ".\\YICT.INI");
	DoSaveAndTransfer(szT);
}

void CMainDlg::OnSaveAndTransfer2() 
{
	char szT[128];
	GetPrivateProfileString("YICT", "SICBURN_CMD", "sicburn.exe -s -run %s", szT, sizeof(szT), ".\\YICT.INI");
	DoSaveAndTransfer(szT);
}

void CMainDlg::OnSave() 
{
	// TODO: Add your control notification handler code here
	if (!g_cart.IsDirty())
	{
		AfxMessageBox("Nothing changed\nPress ESCAPE to cancel");
		return;
	}
	if (!SaveChanges())
		return;
	EndDialog(IDOK);
}

////////////////////////////////////////////////
// Import custom sound from another cartridge

#if 0

// not really necessary

#include "opendlg.h"

void CMainDlg::OnImportSounds() 
{
    // import from another cartridge image (2 .BINs)
	COpenDlg dlgO;
	if (dlgO.DoModal() != IDOK)
		return;

    CART_DATA cdFrom;
	bool bOrig;
    if (!cdFrom.LoadRomImage(dlgO.m_selected, bOrig))
    {
        AfxMessageBox("Failed to load other image");
        return;
    }

	AfxMessageBox("Sound import NYI");
}

#endif

////////////////////////////////////////////////
// Sound Preview

#include "sounddlg.h"

void CMainDlg::OnPreviewSounds() 
{
	CSoundPreviewDlg dlg;

	dlg.DoModal();
		
}

////////////////////////////////////////////////
static bool SelectionContains(int selections[], int nSel, int iMatch)
{
	for (int i = 0; i < nSel; i++)
		if (selections[i] == iMatch)
			return true;
	return false;
}

void CMainDlg::DoSpecial(const char* szAppendTag, bool bAdd) 
	// if !bAdd then replace
{
	int iCond = GetCond();
	if (iCond == -1)
	{
		AfxMessageBox("Select a condition first\nThen I will add dual sounds");
		return;
	}
	if (!IsMultiCond(iCond))
	{
		AfxMessageBox("Only a single action is supported for that condition");
		return;
	}

	int selections[MAX_ACTIONS_PER_COND];
    int nOld = GetCurrentActions(iCond, selections);
	if (nOld == -1)
	{
		AfxMessageBox("Error");
		return;
	}

	if (nOld == 0)
		return;	// benign

	int nNew = nOld;

	m_actionList.SetSel(-1, FALSE);	// unselect all

	bool bChange = false;
	for (int iOld = 0; iOld < nOld; iOld++)
	{
		int iOther = FindSimilarName(selections[iOld], szAppendTag);
		if (iOther != -1 && !SelectionContains(selections, nOld, iOther))
		{
			// add it if room
			if (nNew+1 > MAX_ACTIONS_PER_COND)
			{
				AfxMessageBox("No more room to add dual entries");
				break;
			}
			AddPerformance(iCond, iOther);	// adds at end -- REVIEW: sort later
			nNew++;
			bChange = true;

			if (!bAdd)
			{
				// remove old one
				DeletePerformance(iCond, iOld);	// assumes initial selection array same as performance
			}
		}
	}

	ASSERT(nNew >= nOld && nNew <= MAX_ACTIONS_PER_COND);
	if (!bChange)
		return;

#ifdef LATER
	SortSelections(selections, nNew);
			// resort them so "S" entries come right after non-S entries
#endif
	RebalanceProbabilities(iCond);

	OnSelectCond();
}

void CMainDlg::OnMakeDual()
{
	DoSpecial("S", true);
}

#ifdef LATER // maybe never
void CMainDlg::OnMakeSit() 
{
	DoSpecial("T", false);
}
#endif
