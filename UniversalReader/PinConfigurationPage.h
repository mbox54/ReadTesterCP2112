#pragma once

/////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////

#include "SLABCP2112.h"
#include "DeviceSelect.h"

/////////////////////////////////////////////////////////////////////////////
// CPinConfigurationPage dialog
/////////////////////////////////////////////////////////////////////////////

class CPinConfigurationPage : public CDialog
{
	DECLARE_DYNAMIC(CPinConfigurationPage)

public:
	CPinConfigurationPage(HID_SMBUS_DEVICE* pHidSmbus, CWnd* pParent = NULL);   // standard constructor
	virtual ~CPinConfigurationPage();

// Dialog Data
//	enum { IDD = IDD_PROPPAGE_PIN_CONFIGURATION };

// Control Variables
protected:
//	BYTE m_clkDiv;  //GPIO 00 01 pins

	//plugged devices
	BYTE m_gpio_latch;

	//timer controler
	UINT_PTR m_nTimer;

	//glugged device
	int m_activeDevice;

// Protected Members
protected:
	HID_SMBUS_DEVICE* m_pHidSmbus;

// Public Methods
public:
	void UpdateControlStates();

	void SetDefaults();
	void SetFromDevice();
	int  GetGPIOini(BYTE *gpio_val);
	int  SetGPIOini(BYTE gpio_val);
	void LatchController();
	void LatchReset(BOOL bRest = FALSE);

// Protected Methods
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void CustomDataExchange(CDataExchange* pDX);

	void SetDirection(BYTE direction);
	BYTE GetDirection();
	void SetMode(BYTE mode);
	BYTE GetMode();
	void SetFunction(BYTE function);
	BYTE GetFunction();
	void SetLatchValue(BYTE latchValue);
	BYTE GetLatchValue(BYTE& mask);

	void GetGpioConfig(BOOL silent = FALSE);
	void SetGpioConfig();
	void ReadLatch(BOOL silent = FALSE);
	void WriteLatch();
	// timer
	void StartTimer();
	void StopTimer();
	// device servising
	void OutputStatus(CString functionName, HID_SMBUS_STATUS status);

// Message Handlers
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	afx_msg void OnBnClickedCheckLatch();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonResetLatch2();
};
