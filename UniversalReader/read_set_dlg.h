#pragma once

////////////////////////////////////////////////////////////
// includes
////////////////////////////////////////////////////////////
#include "resource.h"
#include "GridSFF.h"

#include "DeviceCommunication_CP2112.h"


////////////////////////////////////////////////////////////
// CReadSetDlg dialog
////////////////////////////////////////////////////////////
class CReadSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CReadSetDlg)

public:
	CReadSetDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CReadSetDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_READ_SET_DLG };
#endif


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()


private:
	void Grid_RewriteValues(void);
	void Grid_RewriteMask(void);


public:

	CGridSFF m_Grid;

	// Grid events
	afx_msg void OnGridClick(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult);

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	afx_msg void OnBnClickedButtonValueApply();
	afx_msg void OnBnClickedButtonValueRestore();
	afx_msg void OnBnClickedButtonValueReset();
	afx_msg void OnBnClickedButtonValueApply2();
	afx_msg void OnBnClickedButtonValueRestore2();
	afx_msg void OnBnClickedButtonValueReset2();

	virtual BOOL OnInitDialog();


	afx_msg void OnBnClickedButtonValueShow();
};
