// file DeviceCommunication_CP2112.cpp
// 


////////////////////////////////////////////////////////////
// includes
////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "DeviceCommunication_CP2112.h"


////////////////////////////////////////////////////////////
// Library Dependencies
////////////////////////////////////////////////////////////
#pragma comment(lib, "SLABHIDtoSMBus.lib")


////////////////////////////////////////////////////////////
// members
////////////////////////////////////////////////////////////

// CP2112 device pointer
HID_SMBUS_DEVICE g_hidSmbus;

// stored current config
struct st_CP2112Conf g_stCP2112Conf;

// last error states
struct st_CP2112ErrStatus g_stCP2112ErrStatus;

// last warning states
struct st_CP2112WarnStatus g_stCP2112WarnStatus;

// constants to proceed R/W validation
struct st_CP2112ValidConditions g_stCP2112ValidConditions;

// current device addr index
WORD g_usByteAddrIndex = 0;

// buffers
BYTE gv_BufferRead[HID_SMBUS_MAX_READ_REQUEST_SIZE];


////////////////////////////////////////////////////////////
// functions
////////////////////////////////////////////////////////////

BYTE DeviceCP2112_Open(WORD usIndex)
{
	HID_SMBUS_STATUS status = HidSmbus_Open(&g_hidSmbus, usIndex, VID, PID);
	if (status != HID_SMBUS_SUCCESS)
	{
		// [FAIL]

		return ERROR_DEVICE_FAIL_WHILE_PERFORMING;
	}
	
	return ERROR_COMPLETE_WITHOUT_ERRORS;
}


void DeviceCP2112_OpenStatusUpdate()
{	
	BOOL bOpenState = 0;
	HID_SMBUS_STATUS status = HidSmbus_IsOpened(g_hidSmbus, &bOpenState);

	// reset state if device inaccesible
	if (status != HID_SMBUS_SUCCESS)
	{
		g_stCP2112ErrStatus.bLastOpenStatus = 0;
	}
	else
	{
		// try Read CP2112 device addr 0x02
		BYTE ucResult = ERROR_UNKNOWN_ERROR;

		// issue a read request
		ucResult = DeviceCP2112_ReadIIC_Request(0x02, 1);
		if (ucResult == ERROR_COMPLETE_WITHOUT_ERRORS)
		{
			// OK
			g_stCP2112ErrStatus.bLastOpenStatus = 1;
		}	
		else
		{
			g_stCP2112ErrStatus.bLastOpenStatus = 0;
		}
	}
}


BOOL DeviceCP2112_GetUpdateOpenState()
{
	// update
	DeviceCP2112_OpenStatusUpdate();

	return g_stCP2112ErrStatus.bLastOpenStatus;
}


BOOL DeviceCP2112_GetLastOpenState()
{
	return g_stCP2112ErrStatus.bLastOpenStatus;
}


BYTE DeviceCP2112_SetConfig(void)
{
	// set GPIO direction and mode bitmasks
	HID_SMBUS_STATUS status = HidSmbus_SetGpioConfig(g_hidSmbus, g_stCP2112Conf.direction, g_stCP2112Conf.mode, g_stCP2112Conf.function, 0);
	if (status != HID_SMBUS_SUCCESS)
	{
		// device connection error while performing
		return ERROR_DEVICE_FAIL_WHILE_PERFORMING;
	}

	// init Latch
	HidSmbus_WriteLatch(g_hidSmbus, 0x00, 0xFF);
	if (status != HID_SMBUS_SUCCESS)
	{
		// device connection error while performing
		return ERROR_DEVICE_FAIL_WHILE_PERFORMING;
	}

	return ERROR_COMPLETE_WITHOUT_ERRORS;
}

BYTE DeviceCP2112_Close(void)
{
	// reset CP2112
	HID_SMBUS_STATUS status = HidSmbus_Close(g_hidSmbus);
	if (status != HID_SMBUS_SUCCESS)
	{
		// device connection error while performing
		return ERROR_DEVICE_FAIL_WHILE_PERFORMING;
	}

	return ERROR_COMPLETE_WITHOUT_ERRORS;
}


BYTE DeviceCP2112_Reset(void)
{
	// reset CP2112
	HID_SMBUS_STATUS status = HidSmbus_Reset(g_hidSmbus);
	if (status != HID_SMBUS_SUCCESS)
	{
		// device connection error while performing
		return ERROR_DEVICE_FAIL_WHILE_PERFORMING;
	}

	// NOTE: To reset ~1300ms is needed.
	Sleep(1300);

	// open CP2112 last connection
	status = DeviceCP2112_Open(g_stCP2112Conf.ucDeviceNumber);
	if (status != HID_SMBUS_SUCCESS)
	{
		// device connection error while performing
		return ERROR_DEVICE_FAIL_WHILE_PERFORMING;
	}

	Sleep(50);

	// set last config
	status = DeviceCP2112_SetConfig();
	if (status != HID_SMBUS_SUCCESS)
	{
		// device connection error while performing
		return ERROR_DEVICE_FAIL_WHILE_PERFORMING;
	}

	return ERROR_COMPLETE_WITHOUT_ERRORS;
}

// open CP2112 last connection
BYTE DeviceCP2112_Reopen(void)
{	
	BYTE status = DeviceCP2112_Open(g_stCP2112Conf.ucDeviceNumber);
	if (status != HID_SMBUS_SUCCESS)
	{
		// device connection error while performing
		return ERROR_DEVICE_FAIL_WHILE_PERFORMING;
	}

	Sleep(50);

	// set last config
	status = DeviceCP2112_SetConfig();
	if (status != HID_SMBUS_SUCCESS)
	{
		// device connection error while performing
		return ERROR_DEVICE_FAIL_WHILE_PERFORMING;
	}

	return ERROR_COMPLETE_WITHOUT_ERRORS;
}


BYTE DeviceCP2112_ReadLatch(BYTE * ucLatchValue)
{
	// > Get Latch Values from CP2112
	BYTE latchValue = 0;
	HID_SMBUS_STATUS status = HidSmbus_ReadLatch(g_hidSmbus, &latchValue);
	if (status == HID_SMBUS_SUCCESS)
	{
		// [VALID]

		*ucLatchValue = latchValue;
		return ERROR_COMPLETE_WITHOUT_ERRORS;
	}
	else
	{
		// device connection error while performing
		return ERROR_DEVICE_FAIL_WHILE_PERFORMING;
	}
}

// simple request
BYTE DeviceCP2112_ReadIIC_Request(BYTE ucSlaveAddr, WORD usCount)
{
	// Issue a read request
	HID_SMBUS_STATUS status = HidSmbus_ReadRequest(g_hidSmbus, ucSlaveAddr, usCount);
	if (status != HID_SMBUS_SUCCESS)
	{
		// device connection error while performing
		g_stCP2112ErrStatus.ucLastReportState = status;
		return ERROR_DEVICE_FAIL_WHILE_PERFORMING;
	}

	return ERROR_COMPLETE_WITHOUT_ERRORS;
}


BYTE DeviceCP2112_ReadIIC_RequestAddr(BYTE ucSlaveAddr, WORD usByteAddr, WORD usCount)
{
	BOOL opened;
	
	// Make sure that the device is opened
	if (HidSmbus_IsOpened(g_hidSmbus, &opened) == HID_SMBUS_SUCCESS && opened)
	{
		// NOTE:
		// only 1 Byte Arrd mode implemented here
		BYTE targetAddress[HID_SMBUS_MAX_TARGET_ADDRESS_SIZE];
		targetAddress[0] = usByteAddr;

		// Issue an address read request
		HID_SMBUS_STATUS status = HidSmbus_AddressReadRequest(g_hidSmbus, ucSlaveAddr, usCount, 1, targetAddress);
	}

	return 0;
}


BYTE DeviceCP2112_ReadIIC_ForceResponceCheck(WORD usCount)
{
	// send request for OP status
	HID_SMBUS_STATUS status = HidSmbus_TransferStatusRequest(g_hidSmbus);
	if (status != HID_SMBUS_SUCCESS)
	{
		// device connection error while performing
		g_stCP2112ErrStatus.ucLastReportState = status;
		return ERROR_DEVICE_FAIL_WHILE_PERFORMING;
	}

	// reset global warnings
	g_stCP2112WarnStatus.bWarnFlag = 0;
	g_stCP2112WarnStatus.usBusyCounter = 0;
	g_stCP2112WarnStatus.usBusyNakedCases = 0;

	// reset global errors
	g_stCP2112ErrStatus.ucLastErrorS0 = 0;
	g_stCP2112ErrStatus.ucLastErrorS1 = 0;

	// get response for OP status
	HID_SMBUS_S0 status0;
	HID_SMBUS_S1 status1;
	WORD numRetries;
	WORD bytesRead;

	BYTE bAct = 1;
	BYTE ucErrNo = ERROR_UNKNOWN_ERROR;
	BYTE ucBusyTimer = 0;
	BYTE BusyNakedCases = 0;

	while (bAct)
	{
		// proc
		status = HidSmbus_GetTransferStatusResponse(g_hidSmbus, &status0, &status1, &numRetries, &bytesRead);

		// check
		if (status != HID_SMBUS_SUCCESS)
		{
			// get device connection error while performing
			bAct = 0;
			ucErrNo = ERROR_DEVICE_FAIL_WHILE_PERFORMING;

			g_stCP2112ErrStatus.ucLastReportState = status;
		}
		else if (status0 == HID_SMBUS_S0_COMPLETE)
		{
			// complete without errors
			bAct = 0;
			ucErrNo = ERROR_COMPLETE_WITHOUT_ERRORS;

			// check retries
			if (status1 == HID_SMBUS_S1_ERROR_SUCCESS_AFTER_RETRY)
			{
				// Succeeded after retries
				g_stCP2112WarnStatus.bWarnFlag = 1;
				g_stCP2112WarnStatus.usRetryCounter = numRetries;
			}

		} 
		else if (status0 == HID_SMBUS_S0_ERROR)
		{
			// complete with errors
			bAct = 0;
			ucErrNo = ERROR_COMPLETE_WITH_ERRORS;
			g_stCP2112ErrStatus.ucLastErrorS0 = status0;

			// check retries
			if (status1 == HID_SMBUS_S1_ERROR_SUCCESS_AFTER_RETRY)
			{
				// Succeeded after retries
				g_stCP2112WarnStatus.bWarnFlag = 1;
				g_stCP2112WarnStatus.usRetryCounter = numRetries;
				g_stCP2112ErrStatus.ucLastErrorS1 = status1;
			}
			
		}
		else if (status0 == HID_SMBUS_S0_BUSY)
		{
			if (status1 == HID_SMBUS_S1_BUSY_READING)
			{				
				// busy proceeding
				// check time
				if (ucBusyTimer < MAX_WAIT_TIME_FOR_READ)
				{
					// need to wait
					Sleep(1);
					ucBusyTimer++;	
					g_stCP2112ErrStatus.ucLastErrorS1 = status1;
				}
				else
				{
					// timeout
					bAct = 0;
					ucErrNo = ERROR_PROCESS_TIMEOUT;
				}
			}
			else if (status1 == HID_SMBUS_S1_BUSY_ADDRESS_NACKED)
			{
				// busy proceeding
				// check current case
				if (BusyNakedCases < MAX_NACK_CASES_FOR_READ)
				{
					// need to wait
					Sleep(1);
					BusyNakedCases++;
				}
				else
				{
					// timeout
					bAct = 0;
					ucErrNo = ERROR_ADDRESS_NAKED_THRESHOLD;
				}
				
				g_stCP2112ErrStatus.ucLastErrorS1 = status1;
			}
		}
	}
	
		
	// Notify device that it should send a read response back
	status = HidSmbus_ForceReadResponse(g_hidSmbus, usCount);

	// set warning states
	if (ucBusyTimer > 0)
	{
		// [ACTIVATED]

		g_stCP2112WarnStatus.bWarnFlag = 1;
		g_stCP2112WarnStatus.usBusyCounter = ucBusyTimer;
	}

	if (BusyNakedCases > 0)
	{
		// [ACTIVATED]

		g_stCP2112WarnStatus.bWarnFlag = 1;
		g_stCP2112WarnStatus.usBusyNakedCases = BusyNakedCases;
	}
	
	return ucErrNo;
}


BYTE DeviceCP2112_ReadIIC_CURRENT_ADDRESS(BYTE ucSlaveAddr)
{
	return DeviceCP2112_ReadIIC_CURRENT_ADDRESS_SEQUENTIAL(ucSlaveAddr, 1);
}


BYTE DeviceCP2112_ReadIIC_CURRENT_ADDRESS_SEQUENTIAL(BYTE ucSlaveAddr, WORD usCount)
{
	BYTE ucErrorState = ERROR_UNKNOWN_ERROR;

	// make sure that the device is opened
	if (DeviceCP2112_GetUpdateOpenState() == 0)
	{
		// closed
		return OP_STATUS_DEVICE_CLOSED;
	}

	// issue a read request
	ucErrorState = DeviceCP2112_ReadIIC_Request(ucSlaveAddr, usCount);
	if (ucErrorState != ERROR_COMPLETE_WITHOUT_ERRORS)
	{
		return ERROR_DEVICE_FAIL_WHILE_PERFORMING_REQUEST;
	}

	// send cmd to return read values
	ucErrorState = DeviceCP2112_ReadIIC_ForceResponceCheck(usCount);
	if (ucErrorState != ERROR_COMPLETE_WITHOUT_ERRORS)
	{
		// define error details, skip performing
		if (ucErrorState == ERROR_DEVICE_FAIL_WHILE_PERFORMING)
		{
			return ERROR_DEVICE_FAIL_WHILE_PERFORMING_FORCE;
		}
		else
		{
			return ucErrorState;
		}				
	}

	// get values
	// NOTE:
	// up to 61 Byte = MAX_OP_BUFFER
	// TODO:
	// Cycle to read all MAX=512 Bytes
	BYTE ucReceivedCount = 0;
	BYTE v_roundBuffer[HID_SMBUS_MAX_READ_RESPONSE_SIZE];
	ucErrorState = DeviceCP2112_ReadIIC_GetData(&ucReceivedCount, v_roundBuffer);
	if (ucErrorState != ERROR_COMPLETE_WITHOUT_ERRORS)
	{
		return ERROR_DEVICE_FAIL_WHILE_PERFORMING_GET_DATA;
	}

	return ERROR_COMPLETE_WITHOUT_ERRORS;
}


BYTE DeviceCP2112_ReadIIC_RANDOM_ADDRESS(BYTE ucSlaveAddr, BYTE ucByteAddr)
{
	return DeviceCP2112_ReadIIC_RANDOM_ADDRESS_SEQUENTIAL(ucSlaveAddr, ucByteAddr, 1);
}


BYTE DeviceCP2112_ReadIIC_RANDOM_ADDRESS_SEQUENTIAL(BYTE ucSlaveAddr, BYTE ucByteAddr, WORD usCount)
{
	// default error index
	BYTE ucErrorState = ERROR_UNKNOWN_ERROR;

	// update current address index
	g_usByteAddrIndex = ucByteAddr;

	// make sure that the device is opened
	if (DeviceCP2112_GetUpdateOpenState() == 0)
	{
		// closed
		return OP_STATUS_DEVICE_CLOSED;
	}

	// issue a read request
	ucErrorState = DeviceCP2112_ReadIIC_RequestAddr(ucSlaveAddr, ucByteAddr, usCount);
	if (ucErrorState != ERROR_COMPLETE_WITHOUT_ERRORS)
	{
		return ERROR_DEVICE_FAIL_WHILE_PERFORMING_REQUEST;
	}

	// get values
	// NOTE:
	// up to 61 Byte = MAX_OP_BUFFER
	// TODO:
	// Cycle to read all MAX=512 Bytes
	WORD usBytesRemaining = usCount;
	while (usBytesRemaining > 0)
	{
		// define this roung byte read count
		BYTE ucBytesRound;
		if (usBytesRemaining > HID_SMBUS_MAX_READ_RESPONSE_SIZE)
		{
			ucBytesRound = HID_SMBUS_MAX_READ_RESPONSE_SIZE;
		}
		else
		{
			ucBytesRound = usBytesRemaining;
		}
		usBytesRemaining -= ucBytesRound;

		// send cmd to return read values
		ucErrorState = DeviceCP2112_ReadIIC_ForceResponceCheck(ucBytesRound);
		if (ucErrorState != ERROR_COMPLETE_WITHOUT_ERRORS)
		{
			// define error details, skip performing
			if (ucErrorState == ERROR_DEVICE_FAIL_WHILE_PERFORMING)
			{
				return ERROR_DEVICE_FAIL_WHILE_PERFORMING_FORCE;
			}
			else
			{
				return ucErrorState;
			}
		}

		BYTE ucReceivedCount = 0;
		BYTE v_roundBuffer[HID_SMBUS_MAX_READ_RESPONSE_SIZE];
		ucErrorState = DeviceCP2112_ReadIIC_GetData(&ucReceivedCount, v_roundBuffer);
		if (ucErrorState != ERROR_COMPLETE_WITHOUT_ERRORS)
		{
			return ERROR_DEVICE_FAIL_WHILE_PERFORMING_GET_DATA;
		}

		if (ucReceivedCount != ucBytesRound)
		{
			return ERROR_CANT_GET_ALL_READ_BYTES;
		}

		// store in module static memory for CP2112 device
		memcpy(gv_BufferRead + g_usByteAddrIndex, v_roundBuffer, ucBytesRound);
		g_usByteAddrIndex += ucBytesRound;
	}	

	return ERROR_COMPLETE_WITHOUT_ERRORS;
}


BYTE DeviceCP2112_ReadIIC_GetData(BYTE* usReceivedCountInfo, BYTE v_buffer[HID_SMBUS_MAX_READ_RESPONSE_SIZE])
{
	HID_SMBUS_S0 status0;

	// Wait for a read response
	HID_SMBUS_STATUS status = HidSmbus_GetReadResponse(g_hidSmbus, &status0, v_buffer, HID_SMBUS_MAX_READ_RESPONSE_SIZE, usReceivedCountInfo);

	return 0;
}


BYTE DeviceCP2112_ReadIIC(BYTE * v_Buffer, BYTE ucSlaveAddr, WORD ucCount)
{

	return 0;
}


WORD  DeviceCP2112_GetDeviceCount(void)
{
	DWORD usDeviceCount;

	// Get number of HID devices with matching VID/PID (0/0 means not filtered)
	if (HidSmbus_GetNumDevices(&usDeviceCount, VID, PID) == HID_SMBUS_SUCCESS)
	{
		// [VALID]

		return usDeviceCount;
	}
	
	return 0;
}


BYTE DeviceCP2112_GetIDString(WORD usIndex, char* strIDString)
{
	if (HidSmbus_GetString(usIndex, VID, PID, strIDString, HID_SMBUS_GET_SERIAL_STR) == HID_SMBUS_SUCCESS)
	{
		// [VALID]

		return 0;
	}

	return 1;
}

