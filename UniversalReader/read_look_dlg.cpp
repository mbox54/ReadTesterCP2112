// CReadLookDlg.cpp : implementation file
//

////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "read_look_dlg.h"
#include "afxdialogex.h"


////////////////////////////////////////////////////////////
// CReadLookDlg dialog
////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CReadLookDlg, CDialogEx)

CReadLookDlg::CReadLookDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_READ_LOOK_DLG, pParent)
{

}

CReadLookDlg::~CReadLookDlg()
{
}

void CReadLookDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRID, m_Grid);
}


BEGIN_MESSAGE_MAP(CReadLookDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_VALUE_SHOW, &CReadLookDlg::OnBnClickedButtonValueShow)
	ON_BN_CLICKED(IDC_BUTTON_VALUE_RESTORE, &CReadLookDlg::OnBnClickedButtonValueRestore)
	ON_BN_CLICKED(IDC_BUTTON_VALUE_RESET_SEL, &CReadLookDlg::OnBnClickedButtonValueResetSel)
END_MESSAGE_MAP()


////////////////////////////////////////////////////////////
// CReadLookDlg message handlers
////////////////////////////////////////////////////////////
// service
void CReadLookDlg::GridShowValues(void)
{
	m_Grid.GridSFF_Write(gv_BufferRead, 0, 256);
}

// control events
void CReadLookDlg::OnBnClickedButtonValueShow()
{
	// Load from buffer to Grid
	GridShowValues();
}


void CReadLookDlg::OnBnClickedButtonValueRestore()
{
	// TODO: Add your control notification handler code here
}


void CReadLookDlg::OnBnClickedButtonValueResetSel()
{
	// TODO: Add your control notification handler code here
}


BOOL CReadLookDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	// init Grid
	m_Grid.Init();



	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
