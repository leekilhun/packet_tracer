#pragma once

#include "Serial.h"

enum
{
	AR_SERIES   = 0,
	RK_SERIES   = 1,
	CRK_SERIES  = 2,
	AZ_SERIES   = 3,
	BLE_SERIES  = 4,
	PKA_SERIES  = 5,
};

enum
{
	ERROR_FAILED_FUNCTION		= -1,
	ERROR_SUCCESS_FUNCTION		= 1,
};

typedef struct INA_Serial
{
	CSerial				*port;
	HANDLE				hevtOverlapped;
	HANDLE				hevtStop;
	OVERLAPPED			ov;			
	bool				bConnected;
}INA_Serial;