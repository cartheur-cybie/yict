// maindlg.h : header file

/////////////////////////////////////////////////////////////////////////////

#include "selectlist.h"


/////////////////////////////////////////////////////////////////////////////
// CMainDlg dialog

class CMainDlg : public CDialog
{
// Construction
public:
	CMainDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMainDlg)
	enum { IDD = IDD_YICT_DIALOG };
	// CButton	m_btnMakeSit;
	CButton	m_checkMuteInit;
	CButton	m_btnDual;
	CButton	m_checkDisableWUC;
	CButton	m_saveAndTransferButtonS;
	CSelectList	m_sslTiming;
	CButton	m_saveAndTransferButton;
	CSelectList	m_mfChoice;
	CSelectList	m_sslChoice;
	CTreeCtrl	m_condTree;
	CSelectList	m_stretchChoice;
	CSelectList	m_wagTailChoice;
	CSelectList	m_idleTimeChoice;
	CButton	m_changeButton;
	CButton	m_addButton;
	CButton	m_removeButton;
	CListBox	m_actionList;
	CString	m_name;
	CString	m_basename;

	CSelectList m_sslLightCheck;
	CSelectList m_sslDarkness;
	CSelectList m_sslLightness;

	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	void OnUpdateGeneral();
	int GetCond();
	void OnSelectCond();

	// void OnImport();
	void OnExport();
	
	void DoSaveAndTransfer(const char* szTool);
	void DoSpecial(const char* szAppendTag, bool bAdd);

	// Generated message map functions
	//{{AFX_MSG(CMainDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnSelectReaction();
	afx_msg void OnAddAction();
	afx_msg void OnRemoveReaction();
	afx_msg void OnChangeAction();
	afx_msg void OnSelchangeWagTail();
	afx_msg void OnSelchangeStretch();
	afx_msg void OnSelectCondition(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeCombo3();
	afx_msg void OnSelchangeCombo4();
	afx_msg void OnSaveAndTransfer();
	afx_msg void OnSave();
	afx_msg void OnSelchangeCombo5();
	afx_msg void OnImportBehavior();
	afx_msg void OnSaveAndTransfer2();
	afx_msg void OnPreviewSounds();
	afx_msg void OnMakeDual();
	afx_msg void OnChangeChecks();
	afx_msg void OnMakeSit();
	afx_msg void OnSelchangeIdleTime();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

