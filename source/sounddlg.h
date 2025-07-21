#if !defined(AFX_SOUNDDLG_H__C553CE94_0448_4491_BA2D_B5F3198A57A7__INCLUDED_)
#define AFX_SOUNDDLG_H__C553CE94_0448_4491_BA2D_B5F3198A57A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// sounddlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSoundPreviewDlg dialog

class CSoundPreviewDlg : public CDialog
{
// Construction
public:
	CSoundPreviewDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSoundPreviewDlg)
	enum { IDD = IDD_SOUNDPREVIEW };
	CListBox	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSoundPreviewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSoundPreviewDlg)
	afx_msg void OnPlay();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOUNDDLG_H__C553CE94_0448_4491_BA2D_B5F3198A57A7__INCLUDED_)
