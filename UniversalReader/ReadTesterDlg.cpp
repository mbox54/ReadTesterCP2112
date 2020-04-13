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

BYTE m_bDialogInited = 0;


////////////////////////////////////////////////////////////
// CReadTesterDlg dialog
////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CReadTesterDlg, CDialog)

CReadTesterDlg::CReadTesterDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_UNIVERSALREADER_DLG, pParent)
	, m_strEdit_SlaveAddr(_T(""))
	, m_strEdit_ByteAddr(_T(""))
	, m_strEdit_Count(_T(""))
	, m_strEdit_Nbyte(_T(""))
	, m_strEdit_Packdelay(_T(""))
	, m_strEdit_Expcount(_T(""))
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
}


BEGIN_MESSAGE_MAP(CReadTesterDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CReadTesterDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_GO, &CReadTesterDlg::OnBnClickedButtonGo)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, &CReadTesterDlg::OnBnClickedButtonPause)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CReadTesterDlg::OnBnClickedButtonStop)
END_MESSAGE_MAP()



////////////////////////////////////////////////////////////
// CReadTesterDlg message handlers
////////////////////////////////////////////////////////////

BOOL CReadTesterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_bDialogInited = 1;


	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CReadTesterDlg::OnBnClickedOk()
{
	// disable 'Enter'-key auto-exit
	// CDialog::OnOK();
}


void CReadTesterDlg::OnBnClickedButtonGo()
{
	// TODO: Add your control notification handler code here
}


void CReadTesterDlg::OnBnClickedButtonPause()
{
	// TODO: Add your control notification handler code here
}


void CReadTesterDlg::OnBnClickedButtonStop()
{
	// TODO: Add your control notification handler code here
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
	CWnd* pEditLog = AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDIT_errorlog);
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


void ReadDevice(st_IICOpParams stIICOpParams)
{
	BYTE ucResult = ERROR_UNKNOWN_ERROR;
	switch (stIICOpParams.ucMode)
	{
	case ReadIIC_CURRENT_ADDRESS:

		break;

	case ReadIIC_CURRENT_ADDRESS_SEQUENTIAL:
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

	}
	else if (ucResult == ERROR_COMPLETE_WITHOUT_ERRORS)
	{
		// [COMPLETE SUCCESSFULL]

	}
	// DEVICE_FAIL_WHILE_PERFORMING GROUP
	else if (ucResult == ERROR_DEVICE_FAIL_WHILE_PERFORMING_REQUEST)
	{
		// [DEVICE FAIL: STAGE REQUEST]

	}
	else if (ucResult == ERROR_DEVICE_FAIL_WHILE_PERFORMING_GET_DATA)
	{
		// [DEVICE FAIL: STAGE GET DATA]

	}
	else if (ucResult == ERROR_DEVICE_FAIL_WHILE_PERFORMING_FORCE)
	{
		// [DEVICE FAIL: STAGE FORCE READ REQUEST]

	}
	else if (ucResult == ERROR_DEVICE_FAIL_WHILE_PERFORMING_FORCE)
	{
		// [DEVICE FAIL: STAGE FORCE READ REQUEST]

	}
	// FORCE READ REQUEST SPECIFIC GROUP
	else if (ucResult == ERROR_COMPLETE_WITH_ERRORS)
	{
		// [SEE STATUS1]

	}
	else if (ucResult == ERROR_PROCESS_TIMEOUT)
	{
		// [BUSY READING TIMEOUT]

	}
	else if (ucResult == ERROR_ADDRESS_NAKED_THRESHOLD)
	{
		// [NACKED COUNT THRESHOLD]

	}
	// ERROR / UNKNOWN USAGE CASE
	else if (ucResult == ERROR_UNKNOWN_ERROR)
	{
		// [UNKNOWN REASON]

	}
}


