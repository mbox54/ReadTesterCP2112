/////////////////////////////////////////////////////////////////////////////
// PinConfigurationPage.cpp : implementation file
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UniversalReader.h"
#include "PinConfigurationPage.h"
#include "UniversalReaderDlg.h"
#include "Utilities.h"
#include "stdio.h"
#include <string>

/////////////////////////////////////////////////////////////////////////////
// CPinConfigurationPage dialog
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CPinConfigurationPage, CDialog)

CPinConfigurationPage::CPinConfigurationPage(HID_SMBUS_DEVICE* pHidSmbus, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_GPIO_CONFIG, pParent)
	, m_pHidSmbus(pHidSmbus)
{
	SetDefaults();
}

CPinConfigurationPage::~CPinConfigurationPage()
{
}

void CPinConfigurationPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	// Call custom DDX and DDV functions from CustomDDX module
	CustomDataExchange(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CPinConfigurationPage - Public Methods
/////////////////////////////////////////////////////////////////////////////


// Set default control values
void CPinConfigurationPage::SetDefaults()
{
//	m_clkDiv = 0;
	m_gpio_latch = 0;

}

// Set control values by getting the device configuration
void CPinConfigurationPage::SetFromDevice()
{
	// Set default control values
	SetDefaults();

	// Update control values
	GetGpioConfig(TRUE);
	ReadLatch(TRUE);	

	// # Set logics /to RAM from ini
	// (pins haven't got any onchip memory)
	BYTE gpio_val = 0x00;
	if (GetGPIOini(&gpio_val) != 0) 
	{
		//err
	}
	
	SetDirection(gpio_val);
	SetGpioConfig();

	// starting checking plugged devices process
	LatchReset();
	StartTimer();


	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CPinConfigurationPage - Protected Methods
/////////////////////////////////////////////////////////////////////////////



// # Put logic here  /part _get


// Update control captions using latch bitmask
void CPinConfigurationPage::SetLatchValue(BYTE latchValue)
{
	if (latchValue & HID_SMBUS_MASK_GPIO_2)		SetDlgItemText(IDC_CHECK_LATCH_GPIO_2, _T("Empty"));
	else										SetDlgItemText(IDC_CHECK_LATCH_GPIO_2, _T("Plugged"));
	if (latchValue & HID_SMBUS_MASK_GPIO_3)		SetDlgItemText(IDC_CHECK_LATCH_GPIO_3, _T("Empty"));
	else										SetDlgItemText(IDC_CHECK_LATCH_GPIO_3, _T("Plugged"));
	if (latchValue & HID_SMBUS_MASK_GPIO_4)		SetDlgItemText(IDC_CHECK_LATCH_GPIO_4, _T("Empty"));
	else										SetDlgItemText(IDC_CHECK_LATCH_GPIO_4, _T("Plugged"));
	if (latchValue & HID_SMBUS_MASK_GPIO_5)		SetDlgItemText(IDC_CHECK_LATCH_GPIO_5, _T("On"));
	else										SetDlgItemText(IDC_CHECK_LATCH_GPIO_5, _T("Off"));
	if (latchValue & HID_SMBUS_MASK_GPIO_6)		SetDlgItemText(IDC_CHECK_LATCH_GPIO_6, _T("On"));
	else										SetDlgItemText(IDC_CHECK_LATCH_GPIO_6, _T("Off"));
	if (latchValue & HID_SMBUS_MASK_GPIO_7)		SetDlgItemText(IDC_CHECK_LATCH_GPIO_7, _T("On"));
	else										SetDlgItemText(IDC_CHECK_LATCH_GPIO_7, _T("Off"));
}


/////////////////////////////////////////////////////////////////////////////
// CPinConfigurationPage - Message Handlers
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CPinConfigurationPage, CDialog)
	ON_BN_CLICKED(IDOK, &CPinConfigurationPage::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPinConfigurationPage::OnBnClickedCancel)

	ON_BN_CLICKED(IDC_CHECK_LATCH_GPIO_2, &CPinConfigurationPage::OnBnClickedCheckLatch)
	ON_BN_CLICKED(IDC_CHECK_LATCH_GPIO_3, &CPinConfigurationPage::OnBnClickedCheckLatch)
	ON_BN_CLICKED(IDC_CHECK_LATCH_GPIO_4, &CPinConfigurationPage::OnBnClickedCheckLatch)
	ON_BN_CLICKED(IDC_CHECK_LATCH_GPIO_5, &CPinConfigurationPage::OnBnClickedCheckLatch)
	ON_BN_CLICKED(IDC_CHECK_LATCH_GPIO_6, &CPinConfigurationPage::OnBnClickedCheckLatch)
	ON_BN_CLICKED(IDC_CHECK_LATCH_GPIO_7, &CPinConfigurationPage::OnBnClickedCheckLatch)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_RESET_LATCH2, &CPinConfigurationPage::OnBnClickedButtonResetLatch2)
END_MESSAGE_MAP()

// Override enter and cancel keys so that the property
// pages aren't closed
void CPinConfigurationPage::OnBnClickedOk()
{
}

// Override enter and cancel keys so that the property
// pages aren't closed
void CPinConfigurationPage::OnBnClickedCancel()
{
}

void CPinConfigurationPage::OnBnClickedButtonGetGpioConfig()
{
	if (UpdateData(TRUE))
	{
		GetGpioConfig();
		UpdateData(FALSE);
	}
}

void CPinConfigurationPage::OnBnClickedButtonSetGpioConfig()
{
	if (UpdateData(TRUE))
	{
		SetGpioConfig();

		// # Set logic /write GPIO ini
		BYTE direction;

		direction = GetDirection();
		SetGPIOini(direction);
	}
}

void CPinConfigurationPage::OnBnClickedButtonReadLatch()
{
	ReadLatch();
}

void CPinConfigurationPage::OnBnClickedButtonWriteLatch()
{
	WriteLatch();
}

// Handle the BN_CLICKED message for:
// IDC_CHECK_DIRECTION_GPIO_0
// IDC_CHECK_DIRECTION_GPIO_1
// IDC_CHECK_DIRECTION_GPIO_2
// IDC_CHECK_DIRECTION_GPIO_3
// IDC_CHECK_DIRECTION_GPIO_4
// IDC_CHECK_DIRECTION_GPIO_5
// IDC_CHECK_DIRECTION_GPIO_6
// IDC_CHECK_DIRECTION_GPIO_7
//
// Toggle direction between "Input" and "Output"
void CPinConfigurationPage::OnBnClickedCheckDirection()
{
	const MSG* pMsg = GetCurrentMessage();

	if (pMsg->message == WM_COMMAND)
	{
		// Get control ID
		int nID = (int)pMsg->wParam;

		// Get current button caption
		CString caption;
		GetDlgItemText(nID, caption);

		// Toggle caption
		if (caption == _T("Output"))
		{
			SetDlgItemText(nID, _T("Input"));
		}
		else
		{
			SetDlgItemText(nID, _T("Output"));
		}

		UpdateControlStates();
	}
}

// Handle the BN_CLICKED message for:
// IDC_CHECK_MODE_GPIO_0
// IDC_CHECK_MODE_GPIO_1
// IDC_CHECK_MODE_GPIO_2
// IDC_CHECK_MODE_GPIO_3
// IDC_CHECK_MODE_GPIO_4
// IDC_CHECK_MODE_GPIO_5
// IDC_CHECK_MODE_GPIO_6
// IDC_CHECK_MODE_GPIO_7
//
// Toggle direction between "Open-Drain" and "Push-Pull"
void CPinConfigurationPage::OnBnClickedCheckMode()
{
	const MSG* pMsg = GetCurrentMessage();

	if (pMsg->message == WM_COMMAND)
	{
		// Get control ID
		int nID = (int)pMsg->wParam;

		// Get current button caption
		CString caption;
		GetDlgItemText(nID, caption);

		// Toggle caption
		if (caption == _T("Push-Pull"))
		{
			SetDlgItemText(nID, _T("Open-Drain"));
		}
		else
		{
			SetDlgItemText(nID, _T("Push-Pull"));
		}
	}
}

// Handle the BN_CLICKED message for:
// IDC_CHECK_CLK_OUTPUT
// IDC_CHECK_TX_TOGGLE
// IDC_CHECK_RX_TOGGLE
void CPinConfigurationPage::OnBnClickedCheckFunction()
{
	UpdateControlStates();
}

// Handle BN_CLICKED message for:
// IDC_CHECK_LATCH_GPIO_0
// IDC_CHECK_LATCH_GPIO_1
// IDC_CHECK_LATCH_GPIO_2
// IDC_CHECK_LATCH_GPIO_3
// IDC_CHECK_LATCH_GPIO_4
// IDC_CHECK_LATCH_GPIO_5
// IDC_CHECK_LATCH_GPIO_6
// IDC_CHECK_LATCH_GPIO_7
//
// Toggle latch value between "0", "1", and "X"
void CPinConfigurationPage::OnBnClickedCheckLatch()
{
	const MSG* pMsg = GetCurrentMessage();

	if (pMsg->message == WM_COMMAND)
	{
		int infomsg = 0;	//info

		// Get control ID
		int nID = (int)pMsg->wParam;

		switch (nID)	//button clicked
		{
		case IDC_CHECK_LATCH_GPIO_5:	//SFP
			if ((this->m_gpio_latch & HID_SMBUS_MASK_GPIO_2) == 0)	//SFP plugged
			{
				if ((this->m_gpio_latch & HID_SMBUS_MASK_GPIO_5) != 0)	//SFP active
				{
					//deactivating
					HID_SMBUS_STATUS statusW = HidSmbus_WriteLatch(*m_pHidSmbus, 0, HID_SMBUS_MASK_GPIO_5);

					//save new status
					this->m_activeDevice = (this->m_gpio_latch & 0xDF);
					//reflect in controls
					SetLatchValue(this->m_gpio_latch);					

					infomsg = 12;
				}
				else													//SFP none
				{
					//check multiple plugging
					BYTE bMultiple = FALSE;
					if ((HID_SMBUS_MASK_GPIO_3 & this->m_gpio_latch) == 0) { bMultiple = TRUE; }
					else
					{
						if ((HID_SMBUS_MASK_GPIO_4 & this->m_gpio_latch) == 0) { bMultiple = TRUE; }
					}			

					if (bMultiple)	//other devise is active
					{
						infomsg = 40;
					}
					else
					{
						//activating
						HID_SMBUS_STATUS statusW = HidSmbus_WriteLatch(*m_pHidSmbus, 255, HID_SMBUS_MASK_GPIO_5);

						//save new status
						this->m_activeDevice = (this->m_gpio_latch | HID_SMBUS_MASK_GPIO_5);
						//reflect in controls
						SetLatchValue(this->m_gpio_latch);

						infomsg = 11;
					}
				}
			}
			break;

		case IDC_CHECK_LATCH_GPIO_6:	//SFP
			if ((this->m_gpio_latch & HID_SMBUS_MASK_GPIO_3) == 0)	//SFP plugged
			{
				if ((this->m_gpio_latch & HID_SMBUS_MASK_GPIO_6) != 0)	//SFP active
				{
					//deactivating
					HID_SMBUS_STATUS statusW = HidSmbus_WriteLatch(*m_pHidSmbus, 0, HID_SMBUS_MASK_GPIO_6);

					//save new status
					this->m_activeDevice = (this->m_gpio_latch & 0xBF);
					//reflect in controls
					SetLatchValue(this->m_gpio_latch);

					infomsg = 22;
				}
				else													//SFP none
				{
					//check multiple plugging
					BYTE bMultiple = FALSE;
					if ((HID_SMBUS_MASK_GPIO_2 & this->m_gpio_latch) == 0) { bMultiple = TRUE; }
					else
					{
						if ((HID_SMBUS_MASK_GPIO_4 & this->m_gpio_latch) == 0) { bMultiple = TRUE; }
					}

					if (bMultiple)	//other devise is active
					{
						infomsg = 40;
					}
					else
					{
						//activating
						HID_SMBUS_STATUS statusW = HidSmbus_WriteLatch(*m_pHidSmbus, 255, HID_SMBUS_MASK_GPIO_6);

						//save new status
						this->m_activeDevice = (this->m_gpio_latch | HID_SMBUS_MASK_GPIO_6);
						//reflect in controls
						SetLatchValue(this->m_gpio_latch);

						infomsg = 21;
					}
				}
			}
			break;

		case IDC_CHECK_LATCH_GPIO_7:	//SFP
			if ((this->m_gpio_latch & HID_SMBUS_MASK_GPIO_4) == 0)	//SFP plugged
			{
				if ((this->m_gpio_latch & HID_SMBUS_MASK_GPIO_7) != 0)	//SFP active
				{
					//deactivating
					HID_SMBUS_STATUS statusW = HidSmbus_WriteLatch(*m_pHidSmbus, 0, HID_SMBUS_MASK_GPIO_7);

					//save new status
					this->m_activeDevice = (this->m_gpio_latch & 0x7F);
					//reflect in controls
					SetLatchValue(this->m_gpio_latch);

					infomsg = 32;
				}
				else													//SFP none
				{
					//check multiple plugging
					BYTE bMultiple = FALSE;
					if ((HID_SMBUS_MASK_GPIO_2 & this->m_gpio_latch) == 0) { bMultiple = TRUE; }
					else
					{
						if ((HID_SMBUS_MASK_GPIO_3 & this->m_gpio_latch) == 0) { bMultiple = TRUE; }
					}

					if (bMultiple)	//other devise is active
					{
						infomsg = 40;
					}
					else
					{
						//activating
						HID_SMBUS_STATUS statusW = HidSmbus_WriteLatch(*m_pHidSmbus, 255, HID_SMBUS_MASK_GPIO_7);

						//save new status
						this->m_activeDevice = (this->m_gpio_latch | HID_SMBUS_MASK_GPIO_7);
						//reflect in controls
						SetLatchValue(this->m_gpio_latch);

						infomsg = 31;
					}
				}
			}
			break;

		default:
			break;
		}
		
		if (infomsg != 0) 
		{
			CString statusText;

			switch (infomsg) {
			case 11: statusText = "SFP power on!";
				//AfxMessageBox((CString)"SFP has plagged in!", MB_OK, MB_ICONASTERISK); 
				break;

			case 12: statusText = "SFP power off!";
				//AfxMessageBox((CString)"SFP has plagged out!", MB_OK, MB_ICONASTERISK);
				break;

			case 21: statusText = "XFP power on!";
				//AfxMessageBox((CString)"XFP has plagged in!", MB_OK, MB_ICONASTERISK);
				break;

			case 22: statusText = "XFP power off!";
				//AfxMessageBox((CString)"XFP has plagged out!", MB_OK, MB_ICONASTERISK);
				break;

			case 31: statusText = "QSFP power on!";
				//AfxMessageBox((CString)"QSFP has plagged in!", MB_OK, MB_ICONASTERISK);
				break;

			case 32: statusText = "QSFP power off!";
				//AfxMessageBox((CString)"QSFP has plagged out!", MB_OK, MB_ICONASTERISK);

			case 40: statusText = "Another device has already activated!";
				//AfxMessageBox((CString)"QSFP has plagged out!", MB_OK, MB_ICONASTERISK);
				break;
			} // case

			  //status info
			((CHidSmbusExampleDlg*)GetParent())->SetStatusText(statusText);

		} // info
	} //pMsg->message == WM_COMMAND 
	
}

// Convert the CLK Output Speed using
// the CLK Output Divider
void CPinConfigurationPage::OnEnChangeEditClkDiv()
{
	CString clkDiv;
	GetDlgItemText(IDC_EDIT_CLK_DIV, clkDiv);

	UINT clkDivValue = _tcstoul(clkDiv, NULL, 10);

	CString clk_Hz;

	if (clkDivValue == 0)
	{
		clk_Hz = _T("48000000");
	}
	else if (clkDivValue >= 0 && clkDivValue <= 255)
	{
		clk_Hz.Format(_T("%u"), 48000000/(clkDivValue * 2));
	}
	else
	{
		clk_Hz = _T("Invalid");
	}

	SetDlgItemText(IDC_EDIT_CLK_FREQ, clk_Hz);
}

/////////////////////////////////////////////////////////////////////////////
// CPinConfigurationPage # Modifications #
/////////////////////////////////////////////////////////////////////////////

// Get GPIO vals from ini 
int CPinConfigurationPage::GetGPIOini(BYTE *gpio_val)
{
	FILE *fs;
	char* file_name = "cpiodirection.cfg";

	errno_t err;
	if ((err = fopen_s(&fs, file_name, "r")) != 0)
	{
		// "can't read file" 
		return 1;
	}
	else
	{
		char load_str[9] = "00000000";
		fgets(load_str, 9, fs);
		fclose(fs);

		int file_val = 0;

		int k = 1;
		for (int i = 0; i < 8; i++)
		{
			if (load_str[7 - i] == '0')
			{
				//	
			}
			else
			{
				if (load_str[7 - i] == '1')
				{
					file_val += k;
				}
				else
				{
					//bad file
					return 2;
				}
			}

			k = k * 2;
		}

		if (file_val < 256) {
			*gpio_val = (BYTE)file_val;
		}
		else {
			return 3;
		}
	}

	return 0;
}

// Set GPIO vals in ini 
int CPinConfigurationPage::SetGPIOini(BYTE gpio_val)
{

	FILE *fs;
	char* file_name = "cpiodirection.cfg";
	//	char load_string[8] = "none";

	errno_t err;
	if ((err = fopen_s(&fs, file_name, "w")) != 0)
	{
		// "can't write file" 
		return 1;
	}
	else
	{
		char save_str[9] = "00000000";

		int file_val = gpio_val;
		int k = 128;
		for (int i = 0; i < 8; i++)
		{
			if (file_val >= k)
			{
				save_str[i] = '1';
				file_val -= k;
			}
			else
			{
				save_str[i] = '0';
			}

			k = k / 2;
		}

		fputs(save_str, fs);
		fclose(fs);
	}


	return 0;
}

// # Set logic /Latch output control
void CPinConfigurationPage::LatchReset(BOOL bRest)
{
	// read hardware gpio pins / 00 01 02 03 04 needed
	BYTE latchValue = 0;
	HID_SMBUS_STATUS status = HidSmbus_ReadLatch(*m_pHidSmbus, &latchValue);

	if (status != HID_SMBUS_SUCCESS)
	{
	//err read
	}

	// prepare state for LatchControl procedure //deactivate all output
	//write GPOI 05 06 07 pins default (000)
	status = HidSmbus_WriteLatch(*m_pHidSmbus, 0, 0xE0);
	// renew for reflect
	latchValue = (latchValue & 0x1F);

	if (status != HID_SMBUS_SUCCESS)
	{
		//err write
	}

	if (bRest)
	{
		Sleep(10);	//rest between
	}

	//prepare GPOI 02 03 04 pins default (111)
	latchValue = (latchValue | 0x1C);
	// show renew outut
	SetLatchValue(latchValue);

	// initial status
	m_gpio_latch = latchValue;
}


void CPinConfigurationPage::LatchController()
{
	//check & set output GPIO pins
	//GP02 in  - SFP, GP03 in  - XFP, GP04 in  - QSFP	/ 0 - active, 1 - none
	//GP05 out - SFP, GP06 out - XFP, GP07 out - QSFP	/ 0 - none,   1 - active

	BOOL opened;

	// Make sure that the device is opened

	if (HidSmbus_IsOpened(*m_pHidSmbus, &opened) == HID_SMBUS_SUCCESS && opened)
	{
	BYTE latchValue;

	// Read GPIO latch value
	HID_SMBUS_STATUS statusR = HidSmbus_ReadLatch(*m_pHidSmbus, &latchValue);

		if (statusR == HID_SMBUS_SUCCESS)
		{

			if (m_gpio_latch != latchValue)
			{
				//iterrupt timer
				this->StopTimer();

				BOOL bPluggedOut = 0;	//in case where we get plug out event with one remain deactive device

				BYTE infomsg = 0;
				//SFP check
				if ((HID_SMBUS_MASK_GPIO_2 & m_gpio_latch) != (HID_SMBUS_MASK_GPIO_2 & latchValue))
				{
					if ((HID_SMBUS_MASK_GPIO_2 & latchValue) == 0)	//SFP has being plagged in
					{
						//config latch, set GP05 
						HID_SMBUS_STATUS statusW = HidSmbus_WriteLatch(*m_pHidSmbus, 255, HID_SMBUS_MASK_GPIO_5);

						if (statusW == HID_SMBUS_SUCCESS)
						{
							//update for reflect latch value on screen
							latchValue = (latchValue | HID_SMBUS_MASK_GPIO_5);
							//info
							infomsg = 11;					
						}
					}
					else											//SFP has being plagged out
					{
						//config latch, set GP05 
						HID_SMBUS_STATUS statusW = HidSmbus_WriteLatch(*m_pHidSmbus, 0, HID_SMBUS_MASK_GPIO_5);
						//info
						if (statusW == HID_SMBUS_SUCCESS)
						{
							//update for reflect latch value on screen
							latchValue = (latchValue & 0xDF);
							//info
							infomsg = 12;	

							bPluggedOut = TRUE;
						}
					}
				}
				//XFP check
				if ((HID_SMBUS_MASK_GPIO_3 & m_gpio_latch) != (HID_SMBUS_MASK_GPIO_3 & latchValue))
				{
					if ((HID_SMBUS_MASK_GPIO_3 & latchValue) == 0)	//XFP has being plagged in
					{
						//config latch, set GP06 
						HID_SMBUS_STATUS statusW = HidSmbus_WriteLatch(*m_pHidSmbus, 255, HID_SMBUS_MASK_GPIO_6);

						if (statusW == HID_SMBUS_SUCCESS)
						{
							//update for reflect latch value on screen
							latchValue = (latchValue | HID_SMBUS_MASK_GPIO_6);
							//info
							infomsg = 21;
						}
					}
					else											//XFP has being plagged out
					{
						//config latch, set GP06 
						HID_SMBUS_STATUS statusW = HidSmbus_WriteLatch(*m_pHidSmbus, 0, HID_SMBUS_MASK_GPIO_6);
						//info
						if (statusW == HID_SMBUS_SUCCESS)
						{
							//update for reflect latch value on screen
							latchValue = (latchValue & 0xBF);
							//info
							infomsg = 22;

							bPluggedOut = TRUE;
						}
					}
				}
				//QSFP check
				if ((HID_SMBUS_MASK_GPIO_4 & m_gpio_latch) != (HID_SMBUS_MASK_GPIO_4 & latchValue))
				{
					if ((HID_SMBUS_MASK_GPIO_4 & latchValue) == 0)	//QSFP has being plagged in
					{
						//config latch, set GP07 
						HID_SMBUS_STATUS statusW = HidSmbus_WriteLatch(*m_pHidSmbus, 255, HID_SMBUS_MASK_GPIO_7);

						if (statusW == HID_SMBUS_SUCCESS)
						{
							//update for reflect latch value on screen
							latchValue = (latchValue | HID_SMBUS_MASK_GPIO_7);
							//info
							infomsg = 31;
						}
					}
					else											//QSFP has being plagged out
					{
						//config latch, set GP07 
						HID_SMBUS_STATUS statusW = HidSmbus_WriteLatch(*m_pHidSmbus, 0, HID_SMBUS_MASK_GPIO_7);
						//info
						if (statusW == HID_SMBUS_SUCCESS)
						{
							//update for reflect latch value on screen
							latchValue = (latchValue & 0x7F);
							//info
							infomsg = 32;

							bPluggedOut = TRUE;
						}
					}
				}

				m_gpio_latch = latchValue;

				if (infomsg != 0) {

					CString statusText;

					switch (infomsg) {
					case 11: statusText = "SFP has plagged in!";
						//AfxMessageBox((CString)"SFP has plagged in!", MB_OK, MB_ICONASTERISK); 
						break;

					case 12: statusText = "SFP has plagged out!";
						//AfxMessageBox((CString)"SFP has plagged out!", MB_OK, MB_ICONASTERISK);
						break;

					case 21: statusText = "XFP has plagged in!";
						//AfxMessageBox((CString)"XFP has plagged in!", MB_OK, MB_ICONASTERISK);
						break;

					case 22: statusText = "XFP has plagged out!";
						//AfxMessageBox((CString)"XFP has plagged out!", MB_OK, MB_ICONASTERISK);
						break;

					case 31: statusText = "QSFP has plagged in!";
						//AfxMessageBox((CString)"QSFP has plagged in!", MB_OK, MB_ICONASTERISK);
						break;

					case 32: statusText = "QSFP has plagged out!";
						//AfxMessageBox((CString)"QSFP has plagged out!", MB_OK, MB_ICONASTERISK);
						break;
					} // case

					//status info
					((CHidSmbusExampleDlg*)GetParent())->SetStatusText(statusText);

				} // info

				//check multiple plugging
				BYTE sum = 0;
				infomsg = 0;	//for active/deactive informing

				if ((HID_SMBUS_MASK_GPIO_2 & latchValue) == 0) { sum++; }
				if ((HID_SMBUS_MASK_GPIO_3 & latchValue) == 0) { sum++; }
				if ((HID_SMBUS_MASK_GPIO_4 & latchValue) == 0) { sum++; }

				if (sum > 1)
				{
					// user need to define active device
					CDeviceSelect deviceSelectDlg(&this->m_gpio_latch);
					deviceSelectDlg.DoModal();

					//config latch

					HID_SMBUS_STATUS statusW = HidSmbus_WriteLatch(*m_pHidSmbus, m_gpio_latch, 255);

					//renew for reflect latch value on screen
					latchValue = m_gpio_latch;

					//info_msg active device
					if ((HID_SMBUS_MASK_GPIO_5 & latchValue) != 0)
					{
						infomsg = 11;
					}
					else
					{
						if ((HID_SMBUS_MASK_GPIO_6 & latchValue) != 0)
						{
							infomsg = 21;
						}
						else
						{
							infomsg = 31;
						}
					}
				}	//multiple plugging

				// activate last remain device event
				if ((sum == 1) && (bPluggedOut))
				{
					if ((HID_SMBUS_MASK_GPIO_2 & latchValue) == 0)
					{
						m_gpio_latch = m_gpio_latch | HID_SMBUS_MASK_GPIO_5;
						infomsg = 11;
					}
					else
					{
						if ((HID_SMBUS_MASK_GPIO_3 & latchValue) == 0)
						{
							m_gpio_latch = m_gpio_latch | HID_SMBUS_MASK_GPIO_6;
							infomsg = 21;
						}
						else
						{
							m_gpio_latch = m_gpio_latch | HID_SMBUS_MASK_GPIO_7;
							infomsg = 31;
						}
					}


					//config latch, set output pin
					HID_SMBUS_STATUS statusW = HidSmbus_WriteLatch(*m_pHidSmbus, m_gpio_latch, 255);

					//renew for reflect latch value on screen
					latchValue = m_gpio_latch;
				}	//activate last remain device event

				// Update controls to reflect latch value
				SetLatchValue(latchValue);

				if (infomsg != 0)
				{
					CString statusText;

					switch (infomsg) {
					case 11: statusText = "SFP power on!";
						//AfxMessageBox((CString)"SFP has plagged in!", MB_OK, MB_ICONASTERISK); 
						break;

					case 12: statusText = "SFP power off!";
						//AfxMessageBox((CString)"SFP has plagged out!", MB_OK, MB_ICONASTERISK);
						break;

					case 21: statusText = "XFP power on!";
						//AfxMessageBox((CString)"XFP has plagged in!", MB_OK, MB_ICONASTERISK);
						break;

					case 22: statusText = "XFP power off!";
						//AfxMessageBox((CString)"XFP has plagged out!", MB_OK, MB_ICONASTERISK);
						break;

					case 31: statusText = "QSFP power on!";
						//AfxMessageBox((CString)"QSFP has plagged in!", MB_OK, MB_ICONASTERISK);
						break;

					case 32: statusText = "QSFP power off!";
						//AfxMessageBox((CString)"QSFP has plagged out!", MB_OK, MB_ICONASTERISK);

					case 40: statusText = "Another device has already activated!";
						//AfxMessageBox((CString)"QSFP has plagged out!", MB_OK, MB_ICONASTERISK);
						break;
					} // case

					  //status info
					((CHidSmbusExampleDlg*)GetParent())->SetStatusText(statusText);
				}	//infomsg

				//restore timer
				this->StartTimer();


			} // m_gpio_latch != latchValue 	
		} // statusR == HID_SMBUS_SUCCESS
	}
}

// timer process
void CPinConfigurationPage::StartTimer()
{
	m_nTimer = SetTimer(1, 1000, 0);
}


void CPinConfigurationPage::StopTimer()
{
	KillTimer(m_nTimer);
}


void CPinConfigurationPage::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: append
	LatchController();

	CDialog::OnTimer(nIDEvent);
}


void CPinConfigurationPage::OnBnClickedButtonResetLatch2()
{
	LatchReset(TRUE);


/*	BYTE latchValue;

	latchValue = 255;
	latchValue = latchValue & 0xFB;
	HID_SMBUS_STATUS statusW = HidSmbus_WriteLatch(*m_pHidSmbus, 0, HID_SMBUS_MASK_GPIO_5);

	char buf[10];
	_itoa_s(this->m_activeDevice, buf, 10);

	CString str(buf);
	AfxMessageBox(str, MB_OK, MB_ICONEXCLAMATION);
	*/

}
