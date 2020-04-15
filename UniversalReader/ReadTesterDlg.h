#pragma once

////////////////////////////////////////////////////////////
// includes
////////////////////////////////////////////////////////////
#include "DeviceCommunication_CP2112.h"
#include "resource.h"

#include <thread>
#include <mutex>


////////////////////////////////////////////////////////////
// definitions
////////////////////////////////////////////////////////////

// system
#define TIMER_ID_MON								0x01

// colors for connection state
#define clr_Orange_head RGB(255, 203, 164)
#define clr_Orange_body RGB(255, 217, 174)
#define clr_Red_head RGB(254, 132, 135)
#define clr_Red_body RGB(254, 182, 175)
#define clr_Green_head RGB(107, 243, 106)
#define clr_Green_body RGB(167, 250, 166)


// user operation modes
#define ReadIIC_CURRENT_ADDRESS						0
#define ReadIIC_CURRENT_ADDRESS_SEQUENTIAL			1
#define ReadIIC_RANDOM_ADDRESS						2
#define ReadIIC_RANDOM_ADDRESS_SEQUENTIAL			3
#define WriteIIC_RANDOM_ADDRESS						4
#define WriteIIC_RANDOM_ADDRESS_SEQUENTIAL			5


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


struct st_InterfaceParams
{
	BYTE bEnableLog;
	BYTE bEnableFile;
};


////////////////////////////////////////////////////////////
// static functions
////////////////////////////////////////////////////////////
void Trace_Custom(CEdit* pEdit, CString str);
void OutputLog(LPCTSTR szFmt, ...);

void DecodeErrorStatus(void);
void CheckOutputWarnings(void);

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
	enum { IDD = IDD_READ_TESTER_DLG };
#endif


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()


private:

	UINT_PTR m_nTimerMon;

	// support
	CString GetStr_DecFormatted(WORD usValue);
	CString GetStr_HexFormatted(WORD usValue);
	WORD GetInt_DecFormatted(CString strValue);
	WORD GetInt_HexFormatted(CString strValue);
	


public:

	CString m_strEdit_SlaveAddr;
	CString m_strEdit_ByteAddr;
	CString m_strEdit_Count;
	CString m_strEdit_Nbyte;
	CString m_strEdit_Packdelay;
	CEdit m_ctrlEdit_errorlog;
	CString m_strEdit_Expcount;

	void Trace(LPCTSTR szFmt, ...);
	void ClearLog(void);

	void ExperimentStart();
	void ExperimentProceed();
	void ExperimentStop();

	// Timer PROCs
	void StartTimer();
	void EditTimer();
	void StopTimer();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonGo();
	afx_msg void OnBnClickedButtonPause();
	afx_msg void OnBnClickedButtonStop();

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonClearLog();
	afx_msg void OnBnClickedButtonSaveLog();
	afx_msg void OnBnClickedButtonSaveConf();
	afx_msg void OnBnClickedButtonLoadConf();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
