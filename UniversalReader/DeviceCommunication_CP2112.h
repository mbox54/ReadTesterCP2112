// file DeviceCommunication_CP2112.h
// 
// purpose:
// to implement communication with CP2112 device
// stand alone thread for communication

// interface

#pragma once

////////////////////////////////////////////////////////////
// includes
////////////////////////////////////////////////////////////
#include "afxwin.h"

#include "SLABCP2112.h"


////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define VID										0
#define PID										0

// use to reload overflow Byte Addr index
// NOTE: must be lesser than HID_SMBUS_MAX_READ_REQUEST_SIZE now
// because 'gv_BufferRead' size is set to HID_SMBUS_MAX_READ_REQUEST_SIZE
#define DEVICE_MEMORY_SIZE						256

#define MAX_WAIT_TIME_FOR_READ					100
#define MAX_WAIT_TIME_FOR_WRITE					100
#define MAX_NACK_CASES_FOR_READ					20

#define OP_STATUS_DEVICE_CLOSED					0x20

// Read OP errors
#define ERROR_COMPLETE_WITHOUT_ERRORS			0x00
#define ERROR_DEVICE_FAIL_WHILE_PERFORMING		0x01
#define ERROR_COMPLETE_WITH_ERRORS				0x02
#define ERROR_PROCESS_TIMEOUT					0x03
#define ERROR_ADDRESS_NAKED_THRESHOLD			0x04
#define ERROR_CANT_GET_ALL_READ_BYTES			0x05

#define ERROR_DEVICE_FAIL_WHILE_PERFORMING_REQUEST	0x10
#define ERROR_DEVICE_FAIL_WHILE_PERFORMING_FORCE	0x11
#define ERROR_DEVICE_FAIL_WHILE_PERFORMING_GET_DATA	0x12

#define ERROR_CHIP_READ_TIMEOUT						0x21

#define ERROR_UNKNOWN_ERROR						0xFF


////////////////////////////////////////////////////////////
// typedefs
////////////////////////////////////////////////////////////

struct st_CP2112Conf
{
	BYTE ucDeviceNumber;
	BYTE ucBoardType;		// 0 = unknown, 1 = UniversalReader rev.1, 2 = SFPp TestBoard rev.2
	BYTE direction;			// NOTE: 0 = input, 1 = output
	BYTE mode;				// NOTE: 0 = open-drain, 1 = push-pull
	BYTE function;			// NOTE: 000 = no specFunct, 111 = all Enabled (Clk, Tx, Rx)
};


struct st_CP2112ErrStatus
{
	BOOL bLastOpenStatus;
	BYTE ucLastReportState;
	BYTE ucLastErrorS0;
	BYTE ucLastErrorS1;
};


struct st_CP2112WarnStatus
{
	BOOL bWarnFlag;
	BYTE usBusyCounter;
	BYTE usRetryCounter;
	BYTE usBusyNakedCases;
};


struct st_CP2112ValidConditions
{
	BYTE v_ucConstantTableValues[256];
	BYTE v_bConstantTableMask[256];
};


////////////////////////////////////////////////////////////
// externs
////////////////////////////////////////////////////////////

extern HID_SMBUS_DEVICE g_hidSmbus;
extern struct st_CP2112Conf g_stCP2112Conf;

extern struct st_CP2112ErrStatus g_stCP2112ErrStatus;
extern struct st_CP2112WarnStatus g_stCP2112WarnStatus;
extern struct st_CP2112ValidConditions g_stCP2112ValidConditions;
extern BYTE gv_BufferRead[HID_SMBUS_MAX_READ_REQUEST_SIZE];


////////////////////////////////////////////////////////////
// prototypes
////////////////////////////////////////////////////////////
// data structures manipulation
void Structures_CP2112DeviceBuffer_Init(void);
void Structures_CP2112DeviceConf_Init(void);
void Structures_CP2112EWStatuses_Init(void);
void Structures_CP2112ValueTables_Init(void);

void Structures_CP2112_InitAll(void);


// *** single proc perform ***
WORD DeviceCP2112_GetDeviceCount(void);
BYTE DeviceCP2112_GetIDString(WORD usIndex, char* strIDString);
BYTE DeviceCP2112_Open(WORD usIndex);
BYTE DeviceCP2112_SetConfig(void);
BYTE DeviceCP2112_Close(void);
BYTE DeviceCP2112_ReadLatch(BYTE* ucLatchValue);
BYTE DeviceCP2112_ReadIIC_Request(BYTE ucSlaveAddr, WORD usCount);
BYTE DeviceCP2112_ReadIIC_RequestAddr(BYTE ucSlaveAddr, WORD ucByteAddr, WORD usCount);
BYTE DeviceCP2112_ReadIIC_GetData(BYTE* usReceivedCountInfo, BYTE v_buffer[HID_SMBUS_MAX_READ_RESPONSE_SIZE]);

// *** complex proc perform ***
// service
BYTE DeviceCP2112_Reset(void);
BYTE DeviceCP2112_Reopen(void);
BYTE DeviceCP2112_ReadIIC_ForceResponceCheck(WORD usCount);

// read
BYTE DeviceCP2112_ReadIIC_CURRENT_ADDRESS(BYTE ucSlaveAddr);
BYTE DeviceCP2112_ReadIIC_CURRENT_ADDRESS_SEQUENTIAL(BYTE ucSlaveAddr, WORD usCount);
BYTE DeviceCP2112_ReadIIC_RANDOM_ADDRESS(BYTE ucSlaveAddr, BYTE ucByteAddr);
BYTE DeviceCP2112_ReadIIC_RANDOM_ADDRESS_SEQUENTIAL(BYTE ucSlaveAddr, BYTE ucByteAddr, WORD usCount);

// service
void DeviceCP2112_OpenStatusUpdate();
BOOL DeviceCP2112_GetUpdateOpenState();
BOOL DeviceCP2112_GetLastOpenState();

