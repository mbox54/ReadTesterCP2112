#pragma once


////////////////////////////////////////////////////////////
// includes
////////////////////////////////////////////////////////////
#include "resource.h"
#include "GridSFF.h"

#include "DeviceCommunication_CP2112.h"


////////////////////////////////////////////////////////////
// CReadLookDlg dialog
////////////////////////////////////////////////////////////
class CReadLookDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CReadLookDlg)

public:
	CReadLookDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CReadLookDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_READ_LOOK_DLG	};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CGridSFF m_Grid;



	// service
	void GridShowValues(void);

	// control events
	afx_msg void OnBnClickedButtonValueShow();
	afx_msg void OnBnClickedButtonValueRestore();
	afx_msg void OnBnClickedButtonValueResetSel();
	virtual BOOL OnInitDialog();
};
