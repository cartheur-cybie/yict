/////////////////////////////////////////////////////////////////////////////
// CPromptActionDlg dialog - single selection

class CPromptActionDlg : public CDialog
{
// Construction
public:
	CPromptActionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPromptActionDlg)
	enum { IDD = IDD_NEWACTION };
	CListBox	m_actionList;
	//}}AFX_DATA
	int m_lastSel;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPromptActionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPromptActionDlg)
	afx_msg void OnSelchangeList1();
	virtual BOOL OnInitDialog();
	afx_msg void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPromptMultiActionDlg dialog

class CPromptMultiActionDlg : public CDialog
{
// Construction
public:
	CPromptMultiActionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPromptMultiActionDlg)
	enum { IDD = IDD_MULTIACTION };
	CListBox	m_actionList;
	//}}AFX_DATA
    int m_nSelections;
    int* m_pSelections; // MAX_ACTIONS_PER_COND

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPromptMultiActionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPromptMultiActionDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeList1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

void FillListboxWithActions(CListBox& lbox);
