#pragma once


#ifdef USE_HW_COMM_FASTECH

#define COMM_BUFF_MAX  4096
class fm_comm
{
	/****************************************************
	 *	data
	 ****************************************************/
public:
  struct cfg_t
	{
		IComm* pComm{};
	};
	
	enum class error_code
	{
		Success
	};
private:
	cfg_t  m_commCfg;

	buffer::_que<uint8_t> m_Que{ COMM_BUFF_MAX };
	bool m_TrdLife;
	HANDLE m_hThread;
	DWORD m_TrdId;

  /****************************************************
	 *	Constructor
	 ****************************************************/
public:
	fm_comm(fm_comm::cfg_t &cfg);

	virtual ~fm_comm();
	
  /****************************************************
   *	func
   ****************************************************/
private:

	void threadStop(void);
	void threadRun(void);
	void threadJob(void);
	static UINT threadFunc(LPVOID pParam);
	static void receiveDataCB(void* obj, void* w_parm, void* l_parm);

public:
  //uint32_t SendReceiveData();

  //errno_t SendCmd(uint8_t cmd, uint8_t* p_data, uint8_t length);
  //errno_t SendResp(uint8_t cmd, uint8_t* p_data, uint8_t length);
  //errno_t SendCmdRxResp(uint8_t cmd, uint8_t* p_data, uint8_t length);

  //uint32_t ReadByte(uint8_t* resp, uint32_t len);
  //uint32_t WriteByte(uint8_t* send, uint32_t len);

};

#endif