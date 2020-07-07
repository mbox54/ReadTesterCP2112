// ReadTesterDlg.cpp : implementation file
//

////////////////////////////////////////////////////////////
// includes
////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ReadTesterDlg.h"
#include "afxdialogex.h"


////////////////////////////////////////////////////////////
// static
////////////////////////////////////////////////////////////


struct st_InterfaceParams m_stInterfaceParams;
struct st_IICOpParams m_stIICOpParams;
static struct st_ExperimentParams m_stExperimentParams;

BYTE m_bDialogInited = 0;
CWnd* m_pTesterDlgWnd = NULL;
std::mutex m_mutexCP2112Device;




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


void CAboutDlg::OnBnClickedOk()
{

	CDialogEx::OnOK();
}


BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here


	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

////////////////////////////////////////////////////////////
// CReadTesterDlg dialog
////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CReadTesterDlg, CDialog)

CReadTesterDlg::CReadTesterDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_READ_TESTER_DLG, pParent)
	, m_strEdit_SlaveAddr(_T(""))
	, m_strEdit_ByteAddr(_T(""))
	, m_strEdit_Count(_T(""))
	, m_strEdit_Nbyte(_T(""))
	, m_strEdit_Packdelay(_T(""))
	, m_strEdit_Expcount(_T(""))
	, m_bCheck_AutoConnect(FALSE)
	, m_iRadio_Optype(0)
{

}


CReadTesterDlg::~CReadTesterDlg()
{
}

void CReadTesterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SlaveAddr, m_strEdit_SlaveAddr);
	DDX_Text(pDX, IDC_EDIT_ByteAddr, m_strEdit_ByteAddr);
	DDX_Text(pDX, IDC_EDIT_Count, m_strEdit_Count);
	DDX_Text(pDX, IDC_EDIT_Nbyte, m_strEdit_Nbyte);
	DDX_Text(pDX, IDC_EDIT_Packdelay, m_strEdit_Packdelay);
	DDX_Control(pDX, IDC_EDIT_errorlog, m_ctrlEdit_errorlog);
	DDX_Text(pDX, IDC_EDIT_Expcount, m_strEdit_Expcount);
	DDX_Check(pDX, IDC_CHECK_AUTO_RECONNECT, m_bCheck_AutoConnect);
	DDX_Radio(pDX, IDC_RADIO1, m_iRadio_Optype);
	DDX_Control(pDX, IDC_EDIT_ByteAddr, m_ctrlEdit_ByteAddr);
	DDX_Control(pDX, IDC_EDIT_Count, m_ctrlEdit_Count);
	DDX_Control(pDX, IDC_EDIT_Expcount, m_ctrlEdit_Expcount);
	DDX_Control(pDX, IDC_EDIT_Nbyte, m_ctrlEdit_Nbyte);
	DDX_Control(pDX, IDC_EDIT_Packdelay, m_ctrlEdit_Packdelay);
	DDX_Control(pDX, IDC_EDIT_SlaveAddr, m_ctrlEdit_SlaveAddr);
}


CString CReadTesterDlg::GetStr_DecFormatted(WORD usValue)
{
	CString strVal;

	strVal.Format(_T("%d"), usValue);

	return strVal;
}


CString CReadTesterDlg::GetStr_HexFormatted(WORD usValue)
{
	CString strVal;

	strVal.Format(_T("%02X"), usValue);

	return strVal;
}


WORD CReadTesterDlg::GetInt_DecFormatted(CString strValue)
{
	return (UINT)_tcstoul(strValue, NULL, 10);
}


WORD CReadTesterDlg::GetInt_HexFormatted(CString strValue)
{
	return (UINT)_tcstoul(strValue, NULL, 16);
}


void CReadTesterDlg::Trace(LPCTSTR szFmt, ...)
{
	CString str;

	// Format the message text
	va_list argptr;
	va_start(argptr, szFmt);
	str.FormatV(szFmt, argptr);
	va_end(argptr);

	str.Replace(_T("\n"), _T("\r\n"));

	CString strWndText;	
	m_ctrlEdit_errorlog.GetWindowText(strWndText);
	strWndText += str;
	m_ctrlEdit_errorlog.SetWindowText(strWndText);

	//	m_TraceWnd.SetSel(str.GetLength()-1, str.GetLength()-2, FALSE);
	m_ctrlEdit_errorlog.LineScroll(-m_ctrlEdit_errorlog.GetLineCount());
	m_ctrlEdit_errorlog.LineScroll(m_ctrlEdit_errorlog.GetLineCount() - 4);
}


void CReadTesterDlg::ClearLog(void)
{
	m_ctrlEdit_errorlog.SetWindowText((CString)"");
}


BEGIN_MESSAGE_MAP(CReadTesterDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CReadTesterDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_GO, &CReadTesterDlg::OnBnClickedButtonGo)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, &CReadTesterDlg::OnBnClickedButtonPause)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CReadTesterDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_LOG, &CReadTesterDlg::OnBnClickedButtonClearLog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_LOG, &CReadTesterDlg::OnBnClickedButtonSaveLog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_CONF, &CReadTesterDlg::OnBnClickedButtonSaveConf)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_CONF, &CReadTesterDlg::OnBnClickedButtonLoadConf)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_BOARD_RESET, &CReadTesterDlg::OnBnClickedButtonBoardReset)
	ON_BN_CLICKED(IDC_BUTTON_BOARD_REOPEN, &CReadTesterDlg::OnBnClickedButtonBoardReopen)
	ON_BN_CLICKED(IDC_BUTTON_BOARD_CLOSE, &CReadTesterDlg::OnBnClickedButtonBoardClose)
	ON_BN_CLICKED(IDC_BUTTON_READ_INPUT, &CReadTesterDlg::OnBnClickedButtonReadInput)
	ON_BN_CLICKED(IDC_BUTTON_BOARD_DEVICE_NEW, &CReadTesterDlg::OnBnClickedButtonBoardDeviceNew)
	ON_BN_CLICKED(IDC_BUTTON_READ_OUTPUT, &CReadTesterDlg::OnBnClickedButtonReadOutput)
	ON_BN_CLICKED(IDC_BUTTON_READ_DEFINE, &CReadTesterDlg::OnBnClickedButtonReadDefine)
	ON_BN_CLICKED(IDC_BUTTON_WRITE_INPUT2, &CReadTesterDlg::OnBnClickedButtonWriteInput2)
	ON_BN_CLICKED(IDC_BUTTON_WRITE_OUTPUT2, &CReadTesterDlg::OnBnClickedButtonWriteOutput2)
	ON_BN_CLICKED(IDC_BUTTON_WRITE_DEFINE, &CReadTesterDlg::OnBnClickedButtonWriteDefine)
	ON_BN_CLICKED(IDC_BUTTON_HELP, &CReadTesterDlg::OnBnClickedButtonHelp)
END_MESSAGE_MAP()


////////////////////////////////////////////////////////////
// CReadTesterDlg message handlers
////////////////////////////////////////////////////////////

BOOL CReadTesterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// globals
	// dialog inited in mem
	m_bDialogInited = 1;

	// store HWnd
	m_pTesterDlgWnd = this;

	// defaults
	m_stInterfaceParams.bEnableFile = 0;
	m_stInterfaceParams.bEnableLog = 0;

	// control defaults
	m_strEdit_SlaveAddr = (CString)"A0";
	m_strEdit_ByteAddr = (CString)"00";
	m_strEdit_Count = (CString)"1";
	m_strEdit_Nbyte = (CString)"128";
	m_strEdit_Packdelay = (CString)"0";
	m_strEdit_Expcount = (CString)"1000";

	m_ctrlEdit_SlaveAddr.SetLimitText(2);
	m_ctrlEdit_ByteAddr.SetLimitText(2);
	m_ctrlEdit_Count.SetLimitText(3);
	m_ctrlEdit_Nbyte.SetLimitText(3);
	m_ctrlEdit_Packdelay.SetLimitText(4);
	m_ctrlEdit_Expcount.SetLimitText(7);


	CWnd* pStaticConnection = this->GetDlgItem(IDC_STATIC_CONNECTION);
	CFont* m_Font1 = new CFont;
	m_Font1->CreatePointFont(100, _T("Lucida console"));
	CFont* m_Font2 = new CFont;
	m_Font2->CreatePointFont(160, _T("Lucida console"));

	pStaticConnection->SetFont(m_Font2);

	m_bCheck_AutoConnect = 1;
	m_iRadio_Optype = 0;

	CString str;
	if (DeviceCP2112_GetLastOpenState())
	{
		// [CONNECT]

		str.Append(L"CONNECTED");
	}
	else
	{
		// [DISCONNECT]

		str.Append(L"DISCONNECTED");
	}

	CWnd* pStatic = GetDlgItem(IDC_STATIC_CONNECTION);
	pStatic->SetWindowTextW(str);

	UpdateData(FALSE);

	StartTimer();

	// !debug
	m_stInterfaceParams.bEnableLog = 1;



	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CReadTesterDlg::ExperimentStart()
{
	// get parameters from controls
	UpdateData(TRUE);

	struct st_IICOpParams stIICOpParams;

	// TODO:
	// prepare all other modes
	stIICOpParams.ucSlaveAddr = GetInt_HexFormatted(m_strEdit_SlaveAddr);
	stIICOpParams.ucByteAddr = GetInt_HexFormatted(m_strEdit_ByteAddr);
	stIICOpParams.usCount = GetInt_DecFormatted(m_strEdit_Count);
	stIICOpParams.usPacketDelay = GetInt_DecFormatted(m_strEdit_Packdelay);
	stIICOpParams.ucMode = m_iRadio_Optype;

	// output experiment parameters
	Trace(_T("Experiment ## \n"));
	Trace(_T("------------------------------------------ \n"));
	Trace(_T("config: \n"));
	Trace(_T("SlaveAddr = %s\n"), m_strEdit_SlaveAddr);
	Trace(_T("ByteAddr = %s\n"), m_strEdit_ByteAddr);
	Trace(_T("usCount = %s\n"), m_strEdit_Count);
	Trace(_T("PacketDelay = %s\n"), m_strEdit_Packdelay);
	Trace(_T("***\n\n"));	

	switch (stIICOpParams.ucMode)
	{
	case ReadIIC_CURRENT_ADDRESS:
		Trace(L"case: Read_CURRENT_ADDRESS\n");
		break;

	case ReadIIC_CURRENT_ADDRESS_SEQUENTIAL:
		Trace(L"case: Read_CURRENT_ADDRESS_SEQUENTIAL\n");		
		break;

	case ReadIIC_RANDOM_ADDRESS:
		Trace(L"case: Read_RANDOM_ADDRESS\n");
		break;

	case ReadIIC_RANDOM_ADDRESS_SEQUENTIAL:
		Trace(L"case: Read_RANDOM_ADDRESS_SEQUENTIAL\n");
		break;


	default:
		// err case
		break;
	}

	Trace(_T("***\n\n"));

	// create new thread with Experiment
	m_stIICOpParams = stIICOpParams;
	m_stExperimentParams.usNumber = 0;
	m_stExperimentParams.usCount = GetInt_DecFormatted(m_strEdit_Expcount);

	std::thread Thread_ReadCP2112(ExperimentRun);
	Thread_ReadCP2112.detach();
}


void CReadTesterDlg::ExperimentProceed()
{
	
}


void CReadTesterDlg::ExperimentStop()
{
	// output log
	Trace(_T("\nExperiment ends. \n"));
	Trace(_T("------------------------------------------ \n"));
}


void CReadTesterDlg::StartTimer()
{
	m_nTimerMon = SetTimer(TIMER_ID_MON, 250, NULL);
}


void CReadTesterDlg::EditTimer()
{

}


void CReadTesterDlg::StopTimer()
{
	KillTimer(m_nTimerMon);
}


void CReadTesterDlg::OnBnClickedOk()
{
	// disable 'Enter'-key auto-exit
	// CDialog::OnOK();
}


void CReadTesterDlg::OnBnClickedButtonGo()
{
	ClearLog();
	ExperimentStart();
}


void CReadTesterDlg::OnBnClickedButtonPause()
{
	
}


void CReadTesterDlg::OnBnClickedButtonStop()
{
	// set stop FLAG
	m_stExperimentParams.bStopFLAG = 1;
}


////////////////////////////////////////////////////////////
// static functions
////////////////////////////////////////////////////////////
void Trace_Custom(CEdit* pEdit, CString str)
{
	CString strWndText;
	pEdit->GetWindowText(strWndText);
	strWndText += str;
	pEdit->SetWindowText(strWndText);

	pEdit->SetSel(str.GetLength() - 1, str.GetLength() - 2, FALSE);
	pEdit->LineScroll(-pEdit->GetLineCount());
	pEdit->LineScroll(pEdit->GetLineCount() - 4);
}


void OutputLog(LPCTSTR szFmt, ...)
{
	// get control from memory
	CWnd* pEditLog = m_pTesterDlgWnd->GetDlgItem(IDC_EDIT_errorlog);
	HWND hWND = pEditLog->GetSafeHwnd();

	// check control appropriate class
	wchar_t str_buf[256];
	GetClassName(hWND, str_buf, 255);
	if (StrCmpIW(str_buf, _T("Edit")) == 0)
	{
		// [VALID]

		// > perform op
		CEdit* pEdit = static_cast<CEdit*>(pEditLog);

		// format the message text
		CString str;
		va_list argptr;
		va_start(argptr, szFmt);
		str.FormatV(szFmt, argptr);
		va_end(argptr);

		str.Replace(_T("\n"), _T("\r\n"));

		// output to control
		Trace_Custom(pEdit, str);
	}
}

// FORMAT:
//#define HID_SMBUS_S0_IDLE								0x00
//#define HID_SMBUS_S0_BUSY								0x01
//#define HID_SMBUS_S0_COMPLETE							0x02
//#define HID_SMBUS_S0_ERROR							0x03
//
////# HID_SMBUS_TRANSFER_S0 = HID_SMBUS_S0_BUSY
//#define HID_SMBUS_S1_BUSY_ADDRESS_ACKED				0x00
//#define HID_SMBUS_S1_BUSY_ADDRESS_NACKED				0x01
//#define HID_SMBUS_S1_BUSY_READING						0x02
//#define HID_SMBUS_S1_BUSY_WRITING						0x03
//
////# HID_SMBUS_TRANSFER_S0 = HID_SMBUS_S0_ERROR
//#define HID_SMBUS_S1_ERROR_TIMEOUT_NACK				0x00
//#define HID_SMBUS_S1_ERROR_TIMEOUT_BUS_NOT_FREE		0x01
//#define HID_SMBUS_S1_ERROR_ARB_LOST					0x02
//#define HID_SMBUS_S1_ERROR_READ_INCOMPLETE			0x03
//#define HID_SMBUS_S1_ERROR_WRITE_INCOMPLETE			0x04
//#define HID_SMBUS_S1_ERROR_SUCCESS_AFTER_RETRY		0x05

void DecodeErrorStatus(void)
{
	OutputLog(L"State0 return: ");
	switch (g_stCP2112ErrStatus.ucLastErrorS0)
	{		
	case HID_SMBUS_S0_IDLE:
		OutputLog(L"* IDLE\n");

		break;

	case HID_SMBUS_S0_BUSY:
		OutputLog(L"* BUSY\n");

		break;

	case HID_SMBUS_S0_COMPLETE:
		OutputLog(L"* COMPLETE\n");

		break;

	case HID_SMBUS_S0_ERROR:
		OutputLog(L"* ERROR\n");

		break;


	default:
		OutputLog(L"UNKNOWN. BAD USAGE\n");

		break;
	}
	
	OutputLog(L"State1 return: ");
	if (g_stCP2112ErrStatus.ucLastErrorS0 == HID_SMBUS_S0_BUSY)
	{
		switch (g_stCP2112ErrStatus.ucLastErrorS1)
		{
		case HID_SMBUS_S1_BUSY_ADDRESS_ACKED:
			OutputLog(L"* BUSY_ADDRESS_ACKED\n");

			break;

		case HID_SMBUS_S1_BUSY_ADDRESS_NACKED:
			OutputLog(L"* BUSY_ADDRESS_NACKED\n");

			break;

		case HID_SMBUS_S1_BUSY_READING:
			OutputLog(L"* BUSY_READING\n");

			break;

		case HID_SMBUS_S1_BUSY_WRITING:
			OutputLog(L"* BUSY_WRITING\n");

			break;


		default:
			OutputLog(L"UNKNOWN. BAD USAGE\n");

			break;
		}
	}
	else if ((g_stCP2112ErrStatus.ucLastErrorS0 == HID_SMBUS_S0_ERROR)
		|| (g_stCP2112ErrStatus.ucLastErrorS0 == HID_SMBUS_S0_COMPLETE))
	{
		switch (g_stCP2112ErrStatus.ucLastErrorS1)
		{
		case HID_SMBUS_S1_ERROR_TIMEOUT_NACK:
			OutputLog(L"* ERROR_TIMEOUT_NACK\n");

			break;

		case HID_SMBUS_S1_ERROR_TIMEOUT_BUS_NOT_FREE:
			OutputLog(L"* ERROR_TIMEOUT_BUS_NOT_FREE\n");

			break;

		case HID_SMBUS_S1_ERROR_ARB_LOST:
			OutputLog(L"* ERROR_ARB_LOST\n");

			break;

		case HID_SMBUS_S1_ERROR_READ_INCOMPLETE:
			OutputLog(L"* ERROR_READ_INCOMPLETE\n");

			break;

		case HID_SMBUS_S1_ERROR_WRITE_INCOMPLETE:
			OutputLog(L"* ERROR_WRITE_INCOMPLETE\n");

			break;

		case HID_SMBUS_S1_ERROR_SUCCESS_AFTER_RETRY:
			OutputLog(L"* ERROR_SUCCESS_AFTER_RETRY\n");

			break;


		default:
			OutputLog(L"UNKNOWN. BAD USAGE\n");

			break;
		}
	}
}


void CheckOutputWarnings(void)
{
	// check
	if (g_stCP2112WarnStatus.bWarnFlag)
	{	
		if (g_stCP2112WarnStatus.usBusyCounter > 0)
		{
			OutputLog(L"* device was busy when proceed reading some time: %d ms\n", g_stCP2112WarnStatus.usBusyCounter);
		}

		if (g_stCP2112WarnStatus.usBusyNakedCases > 0)
		{
			OutputLog(L"* device return NACK when reading: %d\n", g_stCP2112WarnStatus.usBusyNakedCases);
		}

		if (g_stCP2112WarnStatus.usRetryCounter > 0)
		{
			OutputLog(L"* cp2112 use repeated start to retry Read, because first time read was inaccesible: %d\n", g_stCP2112WarnStatus.usRetryCounter);
		}
	}
	else
	{
		OutputLog(L"no warnings\n");
	}
}


void CheckOutputWarnings_ExistedOnly(void)
{
	// check
	if (g_stCP2112WarnStatus.bWarnFlag)
	{
		if (g_stCP2112WarnStatus.usBusyCounter > 0)
		{
			OutputLog(L"* device was busy when proceed reading some time: %d ms\n", g_stCP2112WarnStatus.usBusyCounter);
		}

		if (g_stCP2112WarnStatus.usBusyNakedCases > 0)
		{
			OutputLog(L"* device return NACK when reading: %d\n", g_stCP2112WarnStatus.usBusyNakedCases);
		}

		if (g_stCP2112WarnStatus.usRetryCounter > 0)
		{
			OutputLog(L"* cp2112 use repeated start to retry Read, because first time read was inaccesible: %d\n", g_stCP2112WarnStatus.usRetryCounter);
		}
	}
}


void ReadDevice(st_IICOpParams stIICOpParams)
{
	// check resource availability
	if (!m_mutexCP2112Device.try_lock())
	{
		OutputLog(L"CP2112 is used now, retry later.\n");
		return;
	}	

	// log OP
	if (m_stInterfaceParams.bEnableLog)
	{
		OutputLog(L"status: started. \n");
	}
	
	
	BYTE ucResult = ERROR_UNKNOWN_ERROR;
	switch (stIICOpParams.ucMode)
	{
	case ReadIIC_CURRENT_ADDRESS:

		break;

	case ReadIIC_CURRENT_ADDRESS_SEQUENTIAL:

		// log OP
		if (m_stInterfaceParams.bEnableLog)
		{
			OutputLog(L"case: Read_CURRENT_ADDRESS_SEQUENTIAL\n\n");
		}

		ucResult = DeviceCP2112_ReadIIC_CURRENT_ADDRESS_SEQUENTIAL(stIICOpParams.ucSlaveAddr, stIICOpParams.usCount);

		break;


	default:
		// err case
		break;
	}
	
	// check statuses
	if (ucResult == OP_STATUS_DEVICE_CLOSED)
	{
		// [DEVICE CLOSED]

		// log OP
		if (m_stInterfaceParams.bEnableLog)
		{
			OutputLog(L"status: canceled. Device closed. Check CP2112 device.\n");
		}
	}
	else if (ucResult == ERROR_COMPLETE_WITHOUT_ERRORS)
	{
		// [COMPLETE SUCCESSFULL]

		OutputLog(L"status: success.\n");
		OutputLog(L"details:\n");

		DecodeErrorStatus();
		CheckOutputWarnings();

	}
	// DEVICE_FAIL_WHILE_PERFORMING GROUP
	else if (ucResult == ERROR_DEVICE_FAIL_WHILE_PERFORMING_REQUEST)
	{
		// [DEVICE FAIL: STAGE REQUEST]

		OutputLog(L"status: failed. \n * i2c-Device had been inaccessible while PERFORMING_REQUEST *\n");

	}
	else if (ucResult == ERROR_DEVICE_FAIL_WHILE_PERFORMING_GET_DATA)
	{
		// [DEVICE FAIL: STAGE GET DATA]

		OutputLog(L"status: failed. \n * i2c-Device had been inaccessible while GET_DATA *\n");

	}
	else if (ucResult == ERROR_DEVICE_FAIL_WHILE_PERFORMING_FORCE)
	{
		// [DEVICE FAIL: STAGE FORCE READ REQUEST]

		OutputLog(L"status: failed. \n * i2c-Device had been inaccessible while FORCE_READ_REQUEST *\n");

	}
	// FORCE READ REQUEST SPECIFIC GROUP
	else if (ucResult == ERROR_COMPLETE_WITH_ERRORS)
	{
		// [SEE STATUS1]

		OutputLog(L"status: failed. \n *** Complete with errors. ***\n");
		OutputLog(L"details:\n");

		DecodeErrorStatus();
		CheckOutputWarnings();

	}
	else if (ucResult == ERROR_PROCESS_TIMEOUT)
	{
		// [BUSY READING TIMEOUT]

		OutputLog(L"status: failed. Operation ceased by timeout.\n");
		OutputLog(L"details:\n");

		DecodeErrorStatus();
		CheckOutputWarnings();

	}
	else if (ucResult == ERROR_ADDRESS_NAKED_THRESHOLD)
	{
		// [NACKED COUNT THRESHOLD]

		OutputLog(L"status: failed. Operation ceased by NACK threshold.\n");
		OutputLog(L"details:\n");

		DecodeErrorStatus();
		CheckOutputWarnings();
	}
	// ERROR / UNKNOWN USAGE CASE
	else if (ucResult == ERROR_UNKNOWN_ERROR)
	{
		// [UNKNOWN REASON]

		OutputLog(L"status: failed. Unknown behavior, invalid usage.\n");
		OutputLog(L"details:\n");

		DecodeErrorStatus();
		CheckOutputWarnings();
	}

	// Read OP finish
	m_mutexCP2112Device.unlock();

	OutputLog(L"\nstatus: finished.\n");	

	// Experiment finish
	// TODO: place in a separate routine
	CReadTesterDlg* pReadTesterDlg = static_cast<CReadTesterDlg*>(m_pTesterDlgWnd);
	pReadTesterDlg->ExperimentStop();
}


BYTE ReadDevice_Experiment()
{
	// wait for resource availability
	WORD usMutexLockTimeout = 0;

	while (!m_mutexCP2112Device.try_lock())
	{
		if (usMutexLockTimeout < 100)
		{
			std::chrono::milliseconds(1);
			usMutexLockTimeout++;
			OutputLog(L".");
		}
		else
		{			
			return ERROR_MUTEX_TIMEOUT;
		}		
	}

	BYTE ucResult = ERROR_UNKNOWN_ERROR;
	switch (m_stIICOpParams.ucMode)
	{
	case ReadIIC_CURRENT_ADDRESS:
		ucResult = DeviceCP2112_ReadIIC_CURRENT_ADDRESS(m_stIICOpParams.ucSlaveAddr);		
		break;

	case ReadIIC_CURRENT_ADDRESS_SEQUENTIAL:
		ucResult = DeviceCP2112_ReadIIC_CURRENT_ADDRESS_SEQUENTIAL(m_stIICOpParams.ucSlaveAddr, m_stIICOpParams.usCount);
		break;

	case ReadIIC_RANDOM_ADDRESS:
		ucResult = DeviceCP2112_ReadIIC_RANDOM_ADDRESS(m_stIICOpParams.ucSlaveAddr, m_stIICOpParams.ucByteAddr);
		break;

	case ReadIIC_RANDOM_ADDRESS_SEQUENTIAL:
		ucResult = DeviceCP2112_ReadIIC_RANDOM_ADDRESS_SEQUENTIAL(m_stIICOpParams.ucSlaveAddr, m_stIICOpParams.ucByteAddr, m_stIICOpParams.usCount);
		break;


	default:
		// err case
		break;
	}

	// check statuses
	if (ucResult == OP_STATUS_DEVICE_CLOSED)
	{
		// [DEVICE CLOSED]

		// log OP
		if (m_stInterfaceParams.bEnableLog)
		{
			OutputLog(L"status: canceled. Device closed. Check CP2112 device.\n");
		}
	}
	else if (ucResult == ERROR_COMPLETE_WITHOUT_ERRORS)
	{
		// [COMPLETE SUCCESSFULL]

		//OutputLog(L"status: success.\n");
		//OutputLog(L"details:\n");

		//DecodeErrorStatus();
		CheckOutputWarnings();
		CheckOutputWarnings_ExistedOnly();
	}
	// DEVICE_FAIL_WHILE_PERFORMING GROUP
	else if (ucResult == ERROR_DEVICE_FAIL_WHILE_PERFORMING_REQUEST)
	{
		// [DEVICE FAIL: STAGE REQUEST]

		OutputLog(L"status: failed. \n * i2c-Device had been inaccessible while PERFORMING_REQUEST *\n");

	}
	else if (ucResult == ERROR_DEVICE_FAIL_WHILE_PERFORMING_GET_DATA)
	{
		// [DEVICE FAIL: STAGE GET DATA]

		OutputLog(L"status: failed. \n * i2c-Device had been inaccessible while GET_DATA *\n");

	}
	else if (ucResult == ERROR_DEVICE_FAIL_WHILE_PERFORMING_FORCE)
	{
		// [DEVICE FAIL: STAGE FORCE READ REQUEST]

		OutputLog(L"status: failed. \n * i2c-Device had been inaccessible while FORCE_READ_REQUEST *\n");

	}
	// FORCE READ REQUEST SPECIFIC GROUP
	else if (ucResult == ERROR_CHIP_READ_TIMEOUT)
	{
		OutputLog(L"status: failed. \n *** Chip CP2112 stuttered. It needs more delay between packets to perforn operation! ***\n");
		OutputLog(L"Note, you need reset chip to continue.\n");

	}
	else if (ucResult == ERROR_COMPLETE_WITH_ERRORS)
	{
		// [SEE STATUS1]

		OutputLog(L"status: failed. \n *** Complete with errors. ***\n");
		OutputLog(L"details:\n");

		DecodeErrorStatus();
		CheckOutputWarnings();

	}
	else if (ucResult == ERROR_PROCESS_TIMEOUT)
	{
		// [BUSY READING TIMEOUT]

		OutputLog(L"status: failed. Operation ceased by timeout.\n");
		OutputLog(L"details:\n");

		DecodeErrorStatus();
		CheckOutputWarnings();

	}
	else if (ucResult == ERROR_ADDRESS_NAKED_THRESHOLD)
	{
		// [NACKED COUNT THRESHOLD]

		OutputLog(L"status: failed. Operation ceased by NACK threshold.\n");
		OutputLog(L"details:\n");

		DecodeErrorStatus();
		CheckOutputWarnings();
	}
	// ERROR / UNKNOWN USAGE CASE
	else if (ucResult == ERROR_UNKNOWN_ERROR)
	{
		// [UNKNOWN REASON]

		OutputLog(L"status: failed. Unknown behavior, invalid usage.\n");
		OutputLog(L"details:\n");

		DecodeErrorStatus();
		CheckOutputWarnings();
	}

	m_stExperimentParams.ucOpLastStatus = ucResult;
	
	m_mutexCP2112Device.unlock();

	return ucResult;
}


void ResetDevice(void)
{
	// check resource availability
	if (!m_mutexCP2112Device.try_lock())
	{
		OutputLog(L"\nCP2112 is used now, retry later.\n");
		return;
	}

	// PROC
	OutputLog(L"Try to reset CP2112 device");

	BYTE ucResult = DeviceCP2112_Reset();
	if (ucResult != ERROR_COMPLETE_WITHOUT_ERRORS)
	{
		OutputLog(L"\nReset complete with errors.\n");
	}
	else
	{
		OutputLog(L"\nReset complete successfully.\n");
	}

	m_mutexCP2112Device.unlock();	
}


void ReopenDevice(void)
{
	// check resource availability
	if (!m_mutexCP2112Device.try_lock())
	{
		OutputLog(L"\nCP2112 is used now, retry later.\n");
		return;
	}

	// PROC
	OutputLog(L"Try to reopen CP2112 device");

	BYTE ucResult = DeviceCP2112_Reopen();
	if (ucResult != ERROR_COMPLETE_WITHOUT_ERRORS)
	{
		OutputLog(L"\nReopen complete with errors.\n");
	}
	else
	{
		OutputLog(L"\nReopen complete successfully.\n");
	}

	m_mutexCP2112Device.unlock();	
}


void CloseDevice(void)
{
	// check resource availability
	if (!m_mutexCP2112Device.try_lock())
	{
		OutputLog(L"\nCP2112 is used now, retry later.\n");
		return;
	}

	// PROC
	OutputLog(L"Try to close CP2112 device");

	BYTE ucResult = DeviceCP2112_Close();
	if (ucResult != ERROR_COMPLETE_WITHOUT_ERRORS)
	{
		OutputLog(L"\nClose complete with errors.\n");
	}
	else
	{
		OutputLog(L"\nClose complete successfully.\n");
	}

	m_mutexCP2112Device.unlock();	
}


void ReopenDevice_NoLogOutput(void)
{
	// check resource availability
	if (!m_mutexCP2112Device.try_lock())
	{
		return;
	}

	// PROC
	BYTE ucResult = DeviceCP2112_Reopen();

	m_mutexCP2112Device.unlock();
}


void UpdateConnectionState(void)
{
	if (m_mutexCP2112Device.try_lock())
	{
		// [FREE]

		// PROC
		BYTE ucPreviousState = DeviceCP2112_GetLastOpenState();

		if (DeviceCP2112_GetUpdateOpenState() != ucPreviousState)
		{
			// [NEED TO UPDATE]

			CString str;
			if (DeviceCP2112_GetLastOpenState())
			{
				// [CONNECT]

				str.Append(L"CONNECTED");
			}
			else
			{
				// [DISCONNECT]

				str.Append(L"DISCONNECTED");
			}

			CWnd* pStatic = m_pTesterDlgWnd->GetDlgItem(IDC_STATIC_CONNECTION);
			pStatic->SetWindowTextW(str);
		}

		m_mutexCP2112Device.unlock();
	}
	else
	{
		// [BUSY]

		// skip
		// show wait status
		OutputLog(L".");
	}
}

// thread-proc
void ExperimentRun(void)
{
	wchar_t chBuf64[64];

	// init state without errs
	m_stExperimentParams.ucOpLastStatus = ERROR_COMPLETE_WITHOUT_ERRORS;

	CWnd* pStatic_expmax = m_pTesterDlgWnd->GetDlgItem(IDC_STATIC_expmax);
	wsprintf(chBuf64, L"/ %07d", m_stExperimentParams.usCount);
	pStatic_expmax->SetWindowTextW((LPCTSTR)chBuf64);


	// output controls
	CWnd* pStatic_expnumber = m_pTesterDlgWnd->GetDlgItem(IDC_STATIC_expnumber);
	CWnd* pStatic_timeremain = m_pTesterDlgWnd->GetDlgItem(IDC_STATIC_timeremain);
	CWnd* pProgressbarWnd = m_pTesterDlgWnd->GetDlgItem(IDC_PROGRESS_test);
	CProgressCtrl* pProgressbarCtrl = static_cast<CProgressCtrl*> (pProgressbarWnd);

	OutputLog(L"Run. \n");
	pProgressbarCtrl->SetPos(0);

	for (WORD k = 0; k < m_stExperimentParams.usCount; k++)
	{
		m_stExperimentParams.usNumber = k + 1;

		// output experiment number
		
		wsprintf(chBuf64, L"%07d", m_stExperimentParams.usNumber);
		pStatic_expnumber->SetWindowTextW((LPCTSTR)chBuf64);

		// proc operation
		BYTE ucResult = ReadDevice_Experiment();
		
		// check result
		if (ucResult == ERROR_COMPLETE_WITHOUT_ERRORS)
		{
			// success
		}
		else if (ucResult == ERROR_MUTEX_TIMEOUT)
		{
			// skip with warning
			OutputLog(L"\nCP2112 device unavailable too long, aborted!\n");
		}
		else
		{
			// cease experiment
			OutputLog(L"Error break! \n");
			break;
		}
		
		// global stop FLAG
		if (m_stExperimentParams.bStopFLAG)
		{
			// cease experiment
			OutputLog(L"User stop. \n");
			break;
		}

		// update progress
		pProgressbarCtrl->SetPos(m_stExperimentParams.usNumber * 100 / m_stExperimentParams.usCount);
	}
	OutputLog(L"Stop. \n");

	// Experiment finish
	// form report
	if (m_stExperimentParams.bStopFLAG)
	{
		// user stop
		OutputLog(L"\nreport: user cease experiment\n");
		OutputLog(L"\nlast operation number: %d \n", m_stExperimentParams.usNumber);

		// reset FLAG
		m_stExperimentParams.bStopFLAG = 0;
	}

	if (m_stExperimentParams.ucOpLastStatus != ERROR_COMPLETE_WITHOUT_ERRORS)
	{
		// caceled by ocurred error
		OutputLog(L"\nreport: error occurred, experiment canceled\n");
		OutputLog(L"\nlast operation number: %d \n", m_stExperimentParams.usNumber);
		OutputLog(L"see log for error details.\n\n");
	}
	else
	{
		OutputLog(L"\nreport: Excelent! No errors occurred.\n");
	}

	pProgressbarCtrl->SetPos(100);
}


void CReadTesterDlg::OnBnClickedButtonClearLog()
{
	ClearLog();
}


void CReadTesterDlg::OnBnClickedButtonSaveLog()
{
	// TODO: Add your control notification handler code here
}


void CReadTesterDlg::OnBnClickedButtonSaveConf()
{
	// TODO: Add your control notification handler code here
}


void CReadTesterDlg::OnBnClickedButtonLoadConf()
{
	// TODO: Add your control notification handler code here
}


void CReadTesterDlg::OnTimer(UINT_PTR nIDEvent)
{
	// PROC
	std::thread thrDeviceOP(UpdateConnectionState);
	thrDeviceOP.detach();

	// auto connect option
	UpdateData(TRUE);
	if (m_bCheck_AutoConnect)
	{
		// [AUTO CONNECTION]

		// check connection state
		if (!DeviceCP2112_GetLastOpenState())
		{
			// [DISCONNECTED]

			// try to connect
			std::thread thrDeviceOP(ReopenDevice_NoLogOutput);
			thrDeviceOP.detach();
		}
	}	

	CDialog::OnTimer(nIDEvent);
}


HBRUSH CReadTesterDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	CWnd* pStaticBulb01 = this->GetDlgItem(IDC_STATIC_CONNECTION);

	if (pWnd->GetSafeHwnd() == pStaticBulb01->GetSafeHwnd())
	{
		COLORREF clrValue;

		if (g_stCP2112ErrStatus.bLastOpenStatus)
		{
			// [CONNECTED]

			clrValue = clr_Green_body;
		}
		else
		{
			// [DISCONNECTED]

			clrValue = clr_Red_body;
		}

		pDC->SetBkColor(clrValue);
//		pDC->SetTextColor(clrValue);
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

// dialog buttons
void CReadTesterDlg::OnBnClickedButtonBoardReset()
{
	// try to reset
	std::thread thrDeviceOP(ResetDevice);
	thrDeviceOP.detach();
}


void CReadTesterDlg::OnBnClickedButtonBoardReopen()
{
	// try to reopen
	std::thread thrDeviceOP(ReopenDevice);
	thrDeviceOP.detach();
}


void CReadTesterDlg::OnBnClickedButtonBoardClose()
{
	// try to close
	std::thread thrDeviceOP(CloseDevice);
	thrDeviceOP.detach();
}


void CReadTesterDlg::OnBnClickedButtonBoardDeviceNew()
{
	// reset Slave device current byte addr index
	Structures_CP2112DeviceBuffer_Init();
}


void CReadTesterDlg::OnBnClickedButtonReadInput()
{
	// open config dlg
	CReadSetDlg dlgReadSet;
	dlgReadSet.DoModal();

}


void CReadTesterDlg::OnBnClickedButtonReadDefine()
{
	// TODO: Add your control notification handler code here
}


void CReadTesterDlg::OnBnClickedButtonWriteInput2()
{
	// TODO: Add your control notification handler code here
}


void CReadTesterDlg::OnBnClickedButtonWriteOutput2()
{
	// TODO: Add your control notification handler code here
}


void CReadTesterDlg::OnBnClickedButtonWriteDefine()
{
	// TODO: Add your control notification handler code here
}


void CReadTesterDlg::OnBnClickedButtonReadOutput()
{
	// open config dlg
	CReadLookDlg dlgLookSet;
	dlgLookSet.DoModal();
}


void CReadTesterDlg::OnBnClickedButtonHelp()
{
	CAboutDlg dlg;
	dlg.DoModal();
}
