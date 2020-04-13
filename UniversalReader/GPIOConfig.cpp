// GPIOConfig.cpp: файл реализации
//

#include "stdafx.h"
#include "UniversalReader.h"
#include "GPIOConfig.h"
#include "afxdialogex.h"


// диалоговое окно CGPIOConfig

IMPLEMENT_DYNAMIC(CGPIOConfig, CDialogEx)


/////////////////////////////////////////////////////////////////////////////
// Static Global Variables
// Set status bar pane 0 and 1 indicators
static UINT BASED_CODE StatusBarIndicators[] =
{
	ID_INDICATOR_DEVICE,
	ID_INDICATOR_STATUS
};

//CGPIOConfig::CGPIOConfig(CWnd* pParent /*=NULL*/)
//	: CDialogEx(IDD_GPIO_CONFIG, pParent)
//{}
//CGPIOConfig::CGPIOConfig(HID_SMBUS_DEVICE* pHidSmbus, CWnd* pParent /*=NULL*/)
//	: CDialogEx(IDD_GPIO_CONFIG, pParent), 	m_pHidSmbus(pHidSmbus)
//{
//}


CGPIOConfig::CGPIOConfig(CWnd * pParent)
{
}

CGPIOConfig::CGPIOConfig(HID_SMBUS_DEVICE * pHidSmbus, CWnd * pParent)
	: CDialogEx(IDD_GPIO_CONFIG, pParent), m_pHidSmbus(pHidSmbus)
{
}


CGPIOConfig::~CGPIOConfig()
{
}

// Initialize the status bar
// - Create the status bar object
// - Set number of panes to 2
// - Make pane 0 a fixed width
// - Make pane 1 fit the rest of the dialog
// - Draw the status bar
// - Set default status bar text to "Not Connected" and ""
void CGPIOConfig::InitStatusBar()
{
	// Create the status bar
	m_statusBar.Create(this);

	// Set number of panes
	m_statusBar.SetIndicators(StatusBarIndicators, sizeof(StatusBarIndicators) / sizeof(StatusBarIndicators[0]));

	// Make the device pane a fixed width
	m_statusBar.SetPaneInfo(0, ID_INDICATOR_DEVICE, SBPS_NORMAL, 100);

	// Make the status pane fit the rest of the status bar
	m_statusBar.SetPaneInfo(1, ID_INDICATOR_STATUS, SBPS_STRETCH, 0);

	// Draw the status bar
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, ID_INDICATOR_STATUS);

	SetStatusText0(_T(""));
}

void CGPIOConfig::SetStatusText0(const CString & text)
{
	// Set the status bar text
	m_statusBar.SetPaneText(1, text);
}

// Set status bar text for specified pane 1
void CGPIOConfig::SetStatusText1(const CString & text)
{
	// Set the status bar text
	m_statusBar.SetPaneText(1, text);
}

int CGPIOConfig::GetGPIOini(BYTE * gpio_val)
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

void CGPIOConfig::LatchController()
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
					SetStatusText1(statusText);

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
					SetStatusText1(statusText);
				}	//infomsg

					//restore timer
				this->StartTimer();


			} // m_gpio_latch != latchValue 	
		} // statusR == HID_SMBUS_SUCCESS
	}
}

void CGPIOConfig::LatchReset(BOOL bRest)
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

void CGPIOConfig::SetLatchValue(BYTE latchValue)
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


void CGPIOConfig::StartTimer()
{
	m_nTimer = SetTimer(1, 200, 0);
}

void CGPIOConfig::StopTimer()
{
	KillTimer(m_nTimer);
}

void CGPIOConfig::SetDefaults()
{
	m_gpio_latch = 0;
}

void CGPIOConfig::SetFromDevice()
{
	// Set default control values
	SetDefaults();

	// # Set logics /to RAM from ini
	// (pins haven't got any onchip memory)
	BYTE direction = 0;
	if (GetGPIOini(&direction) != 0)
	{
		//err
	}

	BYTE mode = 0xE0;
	BYTE function = 0x00;

	// Set GPIO direction and mode bitmasks
	HID_SMBUS_STATUS status = HidSmbus_SetGpioConfig(*m_pHidSmbus, direction, mode, function, 0);

	// starting checking plugged devices process
	LatchReset();
	StartTimer();

	UpdateData(FALSE);
}

void CGPIOConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGPIOConfig, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_RESET_LATCH, &CGPIOConfig::OnBnClickedButtonResetLatch)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_LATCH_GPIO_6, &CGPIOConfig::OnBnClickedCheckLatch)
	ON_BN_CLICKED(IDC_CHECK_LATCH_GPIO_7, &CGPIOConfig::OnBnClickedCheckLatch)
	ON_BN_CLICKED(IDC_CHECK_LATCH_GPIO_5, &CGPIOConfig::OnBnClickedCheckLatch)
END_MESSAGE_MAP()


// обработчики сообщений CGPIOConfig


void CGPIOConfig::OnBnClickedButtonResetLatch()
{
	LatchReset(TRUE);

}


void CGPIOConfig::OnTimer(UINT_PTR nIDEvent)
{
	LatchController();

	CDialogEx::OnTimer(nIDEvent);
}


BOOL CGPIOConfig::OnInitDialog()
{
	CDialogEx::OnInitDialog();
// TODO: 

	//statusbar
	InitStatusBar();
	//def
	SetDefaults();
	//device init write
	SetFromDevice();


	return TRUE;  // return TRUE unless you set the focus to a control
				  // Исключение: страница свойств OCX должна возвращать значение FALSE
}


void CGPIOConfig::OnBnClickedCheckLatch()
{
	// TODO: 
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
				//	this->m_gpio_latch = (this->m_gpio_latch & 0xDF);
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
				//		this->m_gpio_latch = (this->m_gpio_latch | HID_SMBUS_MASK_GPIO_5);
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
				//	this->m_gpio_latch = (this->m_gpio_latch & 0xBF);
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
					//	this->m_gpio_latch = (this->m_gpio_latch | HID_SMBUS_MASK_GPIO_6);
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
				//	this->m_gpio_latch = (this->m_gpio_latch & 0x7F);
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
				//		this->m_gpio_latch = (this->m_gpio_latch | HID_SMBUS_MASK_GPIO_7);
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
			SetStatusText1(statusText);

		} // info
	} //pMsg->message == WM_COMMAND 

}

