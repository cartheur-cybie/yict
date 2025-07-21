// customdlg.cpp : implementation file
//

#include "stdafx.h"
#include "YICT.h"
#include "maindlg.h"
#include "custom.h"
#include "customdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CPromptActionDlg::CPromptActionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPromptActionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPromptActionDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPromptActionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPromptActionDlg)
	DDX_Control(pDX, IDC_LIST1, m_actionList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPromptActionDlg, CDialog)
	//{{AFX_MSG_MAP(CPromptActionDlg)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	ON_LBN_DBLCLK(IDC_LIST1, OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CPromptActionDlg::OnSelchangeList1() 
{
	int iItem = m_actionList.GetCurSel();
	ASSERT((int)m_actionList.GetItemData(iItem) == iItem);
	m_lastSel = iItem;
}

BOOL CPromptActionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int stops[] = { TABSTOP_STDNAME };
	m_actionList.SetTabStops(1, stops);

    FillListboxWithActions(m_actionList);

	m_actionList.SetCurSel(m_lastSel);

	return TRUE;
}

void CPromptActionDlg::OnOK() 
{
	CDialog::OnOK();	
}

/////////////////////////////////////////////////////////////////////////////
// CPromptMultiActionDlg dialog


CPromptMultiActionDlg::CPromptMultiActionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPromptMultiActionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPromptMultiActionDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPromptMultiActionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPromptMultiActionDlg)
	DDX_Control(pDX, IDC_LIST1, m_actionList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPromptMultiActionDlg, CDialog)
	//{{AFX_MSG_MAP(CPromptMultiActionDlg)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPromptMultiActionDlg::OnSelchangeList1() 
{
	// do all validation at the end
}

void CPromptMultiActionDlg::OnOK() 
{
	int nItems = m_actionList.GetSelCount();

	ASSERT(MAX_ACTIONS_PER_COND == 32);	// for message box below
	if (nItems > MAX_ACTIONS_PER_COND)
	{
		AfxMessageBox("Please select fewer items\n(zero to 32 per condition)");
		return;
	}
	m_actionList.GetSelItems(nItems, m_pSelections);
	m_nSelections = nItems;
	CDialog::OnOK();
}


BOOL CPromptMultiActionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int stops[] = { TABSTOP_STDNAME };
	m_actionList.SetTabStops(1, stops);

    FillListboxWithActions(m_actionList);

	for (int i = 0; i < m_nSelections; i++)
		m_actionList.SetSel(m_pSelections[i], TRUE);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

