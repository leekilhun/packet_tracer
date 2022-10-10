#pragma once
#include "..\hal_def.h"

/*
 * comport_serial.h
 * 
 * 작성자 : 이길훈
 * 22.04.04
 * 시리얼 통신 available 확인 Polling 
 * 1바이트 입력 시 마다 인터럽트 함수 호출 및 리턴 1
 */


#ifdef USE_HW_SERIAL_PORT

class SerialComport
{
public:
	struct cfg_t
	{
		char port_name[PRJ_NAME_STR_LENGTH_MAX]{};
		uint32_t baud{};
		uint8_t  port_no{};
		HANDLE serial_handle = {};

		inline int SetPortName(const char* value) {
			return sprintf_s(port_name, PRJ_NAME_STR_LENGTH_MAX, value);
		}


		inline char* GetPortName() {
			return &port_name[0];
		}

		inline void GetPortName(TCHAR* p_value, uint32_t len = PRJ_NAME_STR_LENGTH_MAX) {
			TCHAR tmp[PRJ_NAME_STR_LENGTH_MAX] = { 0, };
			trans::CharToLPTSTR(port_name, tmp, PRJ_NAME_STR_LENGTH_MAX);
			wsprintf(p_value, tmp);
		}

		inline void operator = (const cfg_t* p_cfg) {
			this->baud = p_cfg->baud;
			this->port_no = p_cfg->port_no;
			this->SetPortName(p_cfg->port_name);
		}

	};
private:
	/****************************************************
	 *	data
	 ****************************************************/
	enum class error_code
	{
		Success,
		GetCommState,
		SerCommState,
		SetCommMask,
		SetupComm,
		PurgeComm,
		ClearCommError,
		GetCommTimeouts,
		SetCommTimeouts,
		existPort,
		emptyPort,
		invalidHandleValue,
		failCreateEventRead,
		failCreateEventWrite,
		failCreateThread,
	};

private:

public:
	// 통신 포트 파일 핸들
	HANDLE m_Serial_handle;

	bool m_IsOpened;
	bool m_TrdLife;
	HANDLE m_hThread;
	DWORD m_TrdId;

	cfg_t m_Serialcfg;
	uint8_t  m_received_data;

	void* m_cbObj;
	evt_cb m_func;

	/****************************************************
	 *	Constructor
	 ****************************************************/
public:
	SerialComport(cfg_t* cfg);

	virtual ~SerialComport();

	/****************************************************
	 *	func
	 ****************************************************/
private:
	errno_t openPort(char* port_name, uint32_t baud);
	bool closePort();

	void threadStop(void);
	void threadRun(void);

	/// <summary>
	/// polling을 통해 수신된 정보를 확인한다.
	/// 수신 여부(available)
	/// </summary>
	/// <param name=""></param>
	void threadJob(void);
	static UINT threadFunc(LPVOID pParam);
public:

	errno_t Open();
	void Close();
	void Recovery();
	bool IsOpened() const;

	uint32_t Available();
	uint8_t Read();
	uint32_t Write(uint8_t* p_data, uint32_t length);

	uint32_t CommPrintf(char* fmt, ...);
	void AttCallbackFunc(void* obj, evt_cb cb);
	

};

#endif