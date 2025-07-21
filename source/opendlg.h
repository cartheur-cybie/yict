#if !defined(AFX_OPENDLG_H__008152C9_A4DD_4533_BF29_5B962F8E3533__INCLUDED_)
#define AFX_OPENDLG_H__008152C9_A4DD_4533_BF29_5B962F8E3533__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OpenDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COpenDlg dialog

class COpenDlg : public CDialog
{
// Construction
public:
	COpenDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COpenDlg)
	enum { IDD = IDD_OPENDIALOG };
	CButton	m_openButton;
	CListBox	m_nameList;
	CString	m_selected;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COpenDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeList1();
	afx_msg void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CNewNameDlg dialog

class CNewNameDlg : public CDialog
{
// Construction
public:
	CNewNameDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewNameDlg)
	enum { IDD = IDD_NEWNAME };
	CString	m_name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewNameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewNameDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPENDLG_H__008152C9_A4DD_4533_BF29_5B962F8E3533__INCLUDED_)
