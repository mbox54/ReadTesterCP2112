#pragma once

// GPIO Pin Bitmask
#define HID_SMBUS_MASK_GPIO_0						0x01
#define HID_SMBUS_MASK_GPIO_1						0x02
#define HID_SMBUS_MASK_GPIO_2						0x04
#define HID_SMBUS_MASK_GPIO_3						0x08
#define HID_SMBUS_MASK_GPIO_4						0x10
#define HID_SMBUS_MASK_GPIO_5						0x20
#define HID_SMBUS_MASK_GPIO_6						0x40
#define HID_SMBUS_MASK_GPIO_7						0x80

// диалоговое окно DeviceSelect

class CDeviceSelect : public CDialog
{
	DECLARE_DYNAMIC(CDeviceSelect)

public:
	CDeviceSelect(BYTE* activeDevice, CWnd* pParent = NULL);   // стандартный конструктор
	virtual ~CDeviceSelect();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEVICESELECT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	// choose active device
	int m_activeDevice;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

	// Atributes
private:
	BYTE* p_activeDevice;

};
