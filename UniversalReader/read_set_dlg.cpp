// CReadSetDlg.cpp : implementation file
//


////////////////////////////////////////////////////////////
// includes
////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "afxdialogex.h"
#include "read_set_dlg.h"

////////////////////////////////////////////////////////////
// CReadSetDlg dialog
////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CReadSetDlg, CDialogEx)

CReadSetDlg::CReadSetDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_READ_SET_DLG, pParent)
{

}

CReadSetDlg::~CReadSetDlg()
{
}

void CReadSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRID, m_Grid);
}


BEGIN_MESSAGE_MAP(CReadSetDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CReadSetDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CReadSetDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_VALUE_APPLY, &CReadSetDlg::OnBnClickedButtonValueApply)
	ON_BN_CLICKED(IDC_BUTTON_VALUE_RESTORE, &CReadSetDlg::OnBnClickedButtonValueRestore)
	ON_BN_CLICKED(IDC_BUTTON_VALUE_RESET, &CReadSetDlg::OnBnClickedButtonValueReset)
	ON_BN_CLICKED(IDC_BUTTON_VALUE_APPLY2, &CReadSetDlg::OnBnClickedButtonValueApply2)
	ON_BN_CLICKED(IDC_BUTTON_VALUE_RESTORE2, &CReadSetDlg::OnBnClickedButtonValueRestore2)
	ON_BN_CLICKED(IDC_BUTTON_VALUE_RESET2, &CReadSetDlg::OnBnClickedButtonValueReset2)
END_MESSAGE_MAP()


// CReadSetDlg message handlers


void CReadSetDlg::OnGridClick(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
}


void CReadSetDlg::OnGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	// > check Valid Input
	bool bInvalid = m_Grid.CheckValidHex(pItem->iRow, pItem->iColumn);
	*pResult = (bInvalid || m_Grid.m_bRejectEditChanges) ? -1 : 0;
}


void CReadSetDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//CDialogEx::OnOK();
}


void CReadSetDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}


void CReadSetDlg::OnBnClickedButtonValueApply()
{
	// TODO: Add your control notification handler code here
}


void CReadSetDlg::OnBnClickedButtonValueRestore()
{
	// TODO: Add your control notification handler code here
}


void CReadSetDlg::OnBnClickedButtonValueReset()
{
	// TODO: Add your control notification handler code here
}


void CReadSetDlg::OnBnClickedButtonValueApply2()
{
	// TODO: Add your control notification handler code here
}


void CReadSetDlg::OnBnClickedButtonValueRestore2()
{
	// TODO: Add your control notification handler code here
}


void CReadSetDlg::OnBnClickedButtonValueReset2()
{
	// TODO: Add your control notification handler code here
}


void CReadSetDlg::Grid_RewriteValues(void)
{
	m_Grid.GridSFF_Write(g_stCP2112ValidConditions.v_ucConstantTableValues, 0, 256);
}


void CReadSetDlg::Grid_RewriteMask(void)
{

}


BOOL CReadSetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	// init Grid
	m_Grid.Init();

	// output current config in Grid
	Grid_RewriteValues();
	Grid_RewriteMask();


	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
