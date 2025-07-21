// OpenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "YICT.h"
#include "OpenDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpenDlg dialog


COpenDlg::COpenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COpenDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COpenDlg)
	m_selected = _T("");
	//}}AFX_DATA_INIT
}


void COpenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpenDlg)
	DDX_Control(pDX, IDOK, m_openButton);
	DDX_Control(pDX, IDC_LIST1, m_nameList);
	DDX_LBString(pDX, IDC_LIST1, m_selected);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpenDlg, CDialog)
	//{{AFX_MSG_MAP(COpenDlg)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	ON_LBN_DBLCLK(IDC_LIST1, OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenDlg message handlers

#include <io.h>
static bool Fill(CListBox& lbox)
{
	// fill with files that match the pattern
	struct _finddata_t fd;
    long l = _findfirst("*-l.bin", &fd);
    if (l == -1)
        return false;
	int nAdded = 0;
    while (1)
    {
        if (!(fd.attrib & _A_SUBDIR))
        {
            // file
			int cch = strlen(fd.name);
			ASSERT(cch > 6);
			ASSERT(strcmpi(&fd.name[cch-6], "-l.bin") == 0);
            fd.name[cch-6] = '\0';
			lbox.AddString(fd.name);
			nAdded++;
        }
        if (_findnext(l, &fd) != 0)
            break;
    }
	return nAdded > 0;
}

BOOL COpenDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (!Fill(m_nameList))
	{
		AfxMessageBox("No personalities found in directory");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	return TRUE;
}

void COpenDlg::OnSelchangeList1() 
{
	m_openButton.EnableWindow(TRUE);	
}

void COpenDlg::OnOK() 
{
	CDialog::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
// CNewNameDlg dialog


CNewNameDlg::CNewNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewNameDlg)
	m_name = _T("");
	//}}AFX_DATA_INIT
}


void CNewNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewNameDlg)
	DDX_Text(pDX, IDC_EDIT1, m_name);
	DDV_MaxChars(pDX, m_name, 15);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewNameDlg, CDialog)
	//{{AFX_MSG_MAP(CNewNameDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewNameDlg message handlers

static bool Exists(const char* sz1, const char* sz2)
{
	char szT[_MAX_PATH];
	strcpy(szT, sz1);
	strcat(szT, sz2);
	FILE* pf = fopen(szT, "rb");
	if (pf == NULL)
		return false;
	fclose(pf);
	return true;
}

void CNewNameDlg::OnOK() 
{
	// check to see if the name already exists

	if (!UpdateData(TRUE))
		return;

	if (m_name.GetLength() < 1 || m_name.GetLength() > 15 ||
		m_name.FindOneOf("\\/:|") != -1)
	{
		AfxMessageBox("Invalid name");
		return;
	}


	if (Exists(m_name, "-l.bin") || Exists(m_name, "-h.bin"))
	{
		if (AfxMessageBox("That personality file already exists. Replace it ?", MB_YESNO | MB_DEFBUTTON2) != IDYES)
			return;
	}

	CDialog::OnOK();
}
