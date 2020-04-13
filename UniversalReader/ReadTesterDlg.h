#pragma once

////////////////////////////////////////////////////////////
// includes
////////////////////////////////////////////////////////////
#include "DeviceCommunication_CP2112.h"
#include "resource.h"

#include <thread>


////////////////////////////////////////////////////////////
// definitions
////////////////////////////////////////////////////////////

#define ReadIIC_CURRENT_ADDRESS						0
#define ReadIIC_CURRENT_ADDRESS_SEQUENTIAL			1


////////////////////////////////////////////////////////////
// typedefs
////////////////////////////////////////////////////////////

struct st_IICOpParams
{
	BYTE ucMode;
	BYTE ucSlaveAddr;
	BYTE ucByteAddr;
	BYTE usCount;
	WORD usPacketDelay;
};


////////////////////////////////////////////////////////////
// static functions
////////////////////////////////////////////////////////////

void ReadDevice(struct st_IICOpParams stIICOpParams);


////////////////////////////////////////////////////////////
// CReadTesterDlg dialog
////////////////////////////////////////////////////////////

class CReadTesterDlg : public CDialog
{
	DECLARE_DYNAMIC(CReadTesterDlg)

public:
	CReadTesterDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CReadTesterDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UNIVERSALREADER_DLG };
#endif


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:

	CString m_strEdit_SlaveAddr;
	CString m_strEdit_ByteAddr;
	CString m_strEdit_Count;
	CString m_strEdit_Nbyte;
	CString m_strEdit_Packdelay;
	CEdit m_ctrlEdit_errorlog;
	CString m_strEdit_Expcount;

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonGo();
	afx_msg void OnBnClickedButtonPause();
	afx_msg void OnBnClickedButtonStop();

	virtual BOOL OnInitDialog();
};
