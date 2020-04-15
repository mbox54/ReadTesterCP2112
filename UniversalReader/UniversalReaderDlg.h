
// UniversalReaderDlg.h : header file
//

#pragma once


////////////////////////////////////////////////////////////
// includes
////////////////////////////////////////////////////////////
#include "afxwin.h"
#include "DeviceCommunication_CP2112.h"	
#include "resource.h"

#include "ReadTesterDlg.h"


////////////////////////////////////////////////////////////
// externs
////////////////////////////////////////////////////////////

extern HID_SMBUS_DEVICE g_hidSmbus;


////////////////////////////////////////////////////////////
// CUniversalReaderDlg dialog
////////////////////////////////////////////////////////////

class CUniversalReaderDlg : public CDialogEx
{
// Construction
public:
	CUniversalReaderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD_BOARD_SEL_DLG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();


protected:
	
	CComboBox m_comboDeviceList;

	void UpdateDeviceList();
	BOOL GetSelectedDevice(CString& serial);
	BOOL FindDevice(CString serial, DWORD& deviceNum);

	BOOL Connect();
	BOOL Disconnect();


	//CGPIOConfig	m_GPIOConfig;


	DECLARE_MESSAGE_MAP()


public:

	int m_iRadio_BoardType;

	void TryConnect(void);

	afx_msg void OnDropdownComboDeviceList();
	afx_msg void OnCloseupComboDeviceList();
	afx_msg void OnSelchangeComboDeviceList();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();	
	afx_msg void OnBnClickedCheckConnect();

};
