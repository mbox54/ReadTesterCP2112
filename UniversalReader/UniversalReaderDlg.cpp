
// UniversalReaderDlg.cpp : implementation file
//


////////////////////////////////////////////////////////////
// includes
////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "appcore.h"
#include "UniversalReaderDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUniversalReaderDlg dialog

CUniversalReaderDlg::CUniversalReaderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_BOARD_SEL_DLG, pParent)
	, m_iRadio_BoardType(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CUniversalReaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DEVICE_LIST, m_comboDeviceList);
	DDX_Radio(pDX, IDC_RADIO1, m_iRadio_BoardType);
}

BEGIN_MESSAGE_MAP(CUniversalReaderDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_DROPDOWN(IDC_COMBO_DEVICE_LIST, &CUniversalReaderDlg::OnDropdownComboDeviceList)
	ON_CBN_CLOSEUP(IDC_COMBO_DEVICE_LIST, &CUniversalReaderDlg::OnCloseupComboDeviceList)
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICE_LIST, &CUniversalReaderDlg::OnSelchangeComboDeviceList)
	ON_BN_CLICKED(IDCANCEL, &CUniversalReaderDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CUniversalReaderDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_CONNECT, &CUniversalReaderDlg::OnBnClickedCheckConnect)
END_MESSAGE_MAP()


// CUniversalReaderDlg message handlers

BOOL CUniversalReaderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CUniversalReaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUniversalReaderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUniversalReaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CUniversalReaderDlg::UpdateDeviceList()
{
	// Only update the combo list when the drop down list is closed
	if (!m_comboDeviceList.GetDroppedState())
	{
		int						sel;
		CString					serial;
		

		// Get previous combobox string selection
		GetSelectedDevice(serial);

		// Remove all strings from the combobox
		m_comboDeviceList.ResetContent();

		// Get number of HID devices with matching VID/PID (0/0 means not filtered)
		DWORD numDevices = DeviceCP2112_GetDeviceCount();

		if (numDevices > 0)
		{
			// Iterate through each HID device with matching VID/PID
			for (DWORD i = 0; i < numDevices; i++)
			{
				// Add serial strings to the combobox
				char strIDString[260];
				BYTE ucResult = DeviceCP2112_GetIDString(i, strIDString);

				if (ucResult == 0)
				{
					m_comboDeviceList.AddString(CString(strIDString));
				}
			}
		}

		sel = m_comboDeviceList.FindStringExact(-1, serial);

		// Select first combobox string
		if (sel == CB_ERR)
		{
			m_comboDeviceList.SetCurSel(0);
		}
		// Restore previous combobox selection
		else
		{
			m_comboDeviceList.SetCurSel(sel);
		}
	}
}

// Get the combobox device selection
// If a device is selected, return TRUE and return the serial string
// Otherwise, return FALSE
BOOL CUniversalReaderDlg::GetSelectedDevice(CString & serial)
{
	BOOL selected = FALSE;

	int			sel;
	CString		selText;

	// Get current selection index or CB_ERR(-1)
	// if no device is selected
	sel = m_comboDeviceList.GetCurSel();

	if (sel != CB_ERR)
	{
		// Get the selected device string
		m_comboDeviceList.GetLBText(sel, selText);
		selected = TRUE;
		serial = selText;
	}

	return selected;
}

// Search for an HID device with a matching device serial string
// If the device was found return TRUE and return the device number
// in deviceNumber
// Otherwise return FALSE
BOOL CUniversalReaderDlg::FindDevice(CString serial, DWORD & deviceNum)
{
	BOOL found = FALSE;
	
	DWORD numDevices = DeviceCP2112_GetDeviceCount();

	if (numDevices > 0)
	{
		for (DWORD i = 0; i < numDevices; i++)
		{
			char strIDString[260];
			BYTE ucResult = DeviceCP2112_GetIDString(i, strIDString);

			if (ucResult == 0)
			{
				if (serial.CompareNoCase(CString(strIDString)) == 0)
				{
					found = TRUE;
					deviceNum = i;
					break;
				}
			}
		}
	}

	return found;
}


BOOL CUniversalReaderDlg::Connect()
{
	BOOL		connected = FALSE;
	CString		serial;
	DWORD		deviceNum;

	// Get selected device serial string
	if (GetSelectedDevice(serial))
	{
		// Find the selected device number
		if (FindDevice(serial, deviceNum))
		{			
			// Attempt to open the device
			BYTE ucResult = DeviceCP2112_Open(deviceNum);
			if (ucResult == HID_SMBUS_SUCCESS)
			{
				// save param /number
				g_stCP2112Conf.ucDeviceNumber = deviceNum;

				connected = TRUE;
			}
			else
			{
				MessageBox(_T("Failed to connect"), 0, MB_ICONWARNING);
			}
		}
	}

	// Connected
	if (connected)
	{

	}
	// Disconnected
	else
	{

	}

	return connected;
}


BOOL CUniversalReaderDlg::Disconnect()
{
	return 0;
}


void CUniversalReaderDlg::TryConnect(void)
{
	BYTE bConnectState = Connect();

	if (bConnectState)
	{
		// set device config
		UpdateData(TRUE);

		// get board types
		BYTE ucDeviceType = m_iRadio_BoardType;

		// save param /type
		g_stCP2112Conf.ucBoardType = ucDeviceType;

		// define other params
		switch (ucDeviceType)
		{
		case 0:		// Universal Reader
			// ### Config for FT Universal Reader rev.1 ###

			g_stCP2112Conf.direction = 0xE2;		// NOTE: 0 = input, 1 = output
			g_stCP2112Conf.mode = 0xE0;				// NOTE: 0 = open-drain, 1 = push-pull
			g_stCP2112Conf.function = 0x02;			// NOTE: 000 = no specFunct, 111 = all Enabled  ([1]Rx, [0]Tx, [7]Clk)

			break;


		case 1:		// Test Board
			// ### Config for FT SFPp TestBoard rev.2 ###

			g_stCP2112Conf.direction = 0xA8;		// NOTE: 0 = input, 1 = output
			g_stCP2112Conf.mode = 0x00;				// NOTE: 0 = open-drain, 1 = push-pull
			g_stCP2112Conf.function = 0x06;			// NOTE: 000 = no specFunct, 111 = all Enabled ([1]Rx, [0]Tx, [7]Clk)

			break;


		case 2:		// SFP Test Board v3.0
			// ### Config for FT SFPp TestBoard rev.3 ###
			// NOTE:
			// 0, 1			- service LED blink
			// 2			- Input		/ TxFault
			// 3			- Output	/ TxDisable
			// 4			- Input		/ Mod Abs
			// 5			- Output	/ LDO Enable
			// 6			- Input		/ RxLOS 
			// 7			- Output	/ none

			g_stCP2112Conf.direction = 0xA8;			// NOTE: 0 = input, 1 = output
			g_stCP2112Conf.mode = 0xA8;				// NOTE: 0 = open-drain, 1 = push-pull
			g_stCP2112Conf.function = 0x06;			// NOTE: 000 = no specFunct, 111 = all Enabled ([1]Rx, [0]Tx, [7]Clk)

			break;


		default:	// err state

			break;
		}

		// set device config
		DeviceCP2112_SetConfig();
	}
}


void CUniversalReaderDlg::OnDropdownComboDeviceList()
{
	// Automatically update the device list when the
	// list is opened/dropped down
	UpdateDeviceList();

}


void CUniversalReaderDlg::OnCloseupComboDeviceList()
{
	// TODO: 
	CString		serial;
	DWORD		deviceNum;

	if (GetSelectedDevice(serial))
	{
		// If the selected device has been removed
		if (!FindDevice(serial, deviceNum))
		{
			// Then update the device list
			UpdateDeviceList();
		}
	}
}


void CUniversalReaderDlg::OnSelchangeComboDeviceList()
{
	TryConnect();

	// check device open
	if (DeviceCP2112_GetUpdateOpenState())
	{
		// [OPEN]

		// open main dialog
		CReadTesterDlg dlgReadTesterDlg;
		dlgReadTesterDlg.DoModal();

	}

}


void CUniversalReaderDlg::OnBnClickedCancel()
{
	
	CDialogEx::OnCancel();
}


void CUniversalReaderDlg::OnBnClickedOk()
{
	
	CDialogEx::OnOK();
}


void CAboutDlg::OnBnClickedOk()
{
	
	CDialogEx::OnOK();
}


void CUniversalReaderDlg::OnBnClickedCheckConnect()
{
	
}


BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
