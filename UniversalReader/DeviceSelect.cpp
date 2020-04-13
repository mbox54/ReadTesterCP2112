// DeviceSelect.cpp: файл реализации
//

#include "stdafx.h"
#include "UniversalReader.h"
#include "DeviceSelect.h"
#include "afxdialogex.h"


// диалоговое окно DeviceSelect

IMPLEMENT_DYNAMIC(CDeviceSelect, CDialog)

CDeviceSelect::CDeviceSelect(BYTE* activeDevice, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DEVICESELECT, pParent), m_activeDevice(-1), p_activeDevice(activeDevice)
{

}

CDeviceSelect::~CDeviceSelect()
{
}

void CDeviceSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_activeDevice);
}


BEGIN_MESSAGE_MAP(CDeviceSelect, CDialog)
	ON_BN_CLICKED(IDOK, &CDeviceSelect::OnBnClickedOk)
END_MESSAGE_MAP()


// обработчики сообщений DeviceSelect


BOOL CDeviceSelect::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: 

		//disable empty slots /set initial radiobutton
	BOOL bInit = 0;
	//use input gpio vals
	//SFP
	if ((HID_SMBUS_MASK_GPIO_2 & *p_activeDevice) != 0)
	{
		CWnd *wnd = this->GetDlgItem(IDC_RADIO1);
		wnd->EnableWindow(FALSE);
	}
	else
	{
		if (!bInit) 
		{
			m_activeDevice = 0;
			bInit = TRUE;
		}
	}
	//QSFP
	if ((HID_SMBUS_MASK_GPIO_4 & *p_activeDevice) != 0)
	{
		CWnd *wnd = this->GetDlgItem(IDC_RADIO2);
		wnd->EnableWindow(FALSE);

	}
	else
	{
		if (!bInit)
		{
			m_activeDevice = 1;
			bInit = TRUE;
		}
	}
	//XFP
	if ((HID_SMBUS_MASK_GPIO_3 & *p_activeDevice) != 0)
	{
		CWnd *wnd = this->GetDlgItem(IDC_RADIO3);
		wnd->EnableWindow(FALSE);

	}
	else
	{
		if (!bInit)
		{
			m_activeDevice = 2;
			// bInit = TRUE;
		}
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // Исключение: страница свойств OCX должна возвращать значение FALSE
}


void CDeviceSelect::OnBnClickedOk()
{
	// TODO:
	UpdateData(TRUE);

	//get GPIO pins
	*p_activeDevice = *p_activeDevice & 0x1F;	//erase GPIO 05 06 07 

	switch (m_activeDevice)	{					//set GP in accordance with activeDevice
		case 0: //SFP
			*p_activeDevice = *p_activeDevice | HID_SMBUS_MASK_GPIO_5;
			break;

		case 1: //QSFP
			*p_activeDevice = *p_activeDevice | HID_SMBUS_MASK_GPIO_7;
			break;

		case 2: //XFP
			*p_activeDevice = *p_activeDevice | HID_SMBUS_MASK_GPIO_6;
			break;
	
	default:
		break;
	}

	CDialog::OnOK();
}
