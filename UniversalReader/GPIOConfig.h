#pragma once

#include "SLABCP2112.h"
#include "DeviceSelect.h"

// диалоговое окно CGPIOConfig

class CGPIOConfig : public CDialogEx
{
	DECLARE_DYNAMIC(CGPIOConfig)

public:
	CGPIOConfig(CWnd* pParent = NULL);   // стандартный конструктор
	CGPIOConfig(HID_SMBUS_DEVICE* pHidSmbus, CWnd* pParent = NULL);   

	virtual ~CGPIOConfig();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GPIO_CONFIG };
#endif

// Control Variables
protected:
	//plugged devices
	BYTE m_gpio_latch;

	//timer controler
	UINT_PTR m_nTimer;


/////////////
//	status bar
	CStatusBar	m_statusBar;

	void InitStatusBar();

	// Set the status bar text
	void SetStatusText0(const CString& text);
	void SetStatusText1(const CString& text);


// Protected Members
protected:
	HID_SMBUS_DEVICE* m_pHidSmbus;

// Protected Methods
protected:
	//read direction
	int  GetGPIOini(BYTE *gpio_val);

	//latch operations
	void LatchController();
	void LatchReset(BOOL bRest = FALSE);

	//reflect on dialog
	void SetLatchValue(BYTE latchValue);

	// timer
	void StartTimer();
	void StopTimer();

	// init
	void SetDefaults();
	void SetFromDevice();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonResetLatch();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckLatch();
};
