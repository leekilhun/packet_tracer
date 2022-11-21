#include "pch.h"
#include "hal_def.h"

#include "_inc/netSocket.h"
#include "_inc/SerialComm.h"

#include "_inc/io_fastech.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef USE_HW_IO_FASTECH


using namespace std;
using namespace GFastech;



#define COMM_STX_AA     (uint8_t)0xaa
#define COMM_RESERVED   (uint8_t)0x00
#define COMM_TIMEOUT_MS (uint32_t)100
#define COMM_BYTE_DIVISOR (int)8

#define COMM_DIV_DATA_LENGTH (int)-6
#define COMM_IO_REG_LENGTH (int) 12

constexpr uint8_t GetSlaveInfo = 0x01;
constexpr uint8_t IOGetOutput = 0xC5;
constexpr uint8_t IOSetOutput = 0xC6;
constexpr uint8_t IOGetInput = 0xC0;



GFastech_IO::GFastech_IO(GFastech_IO::cfg_t& cfg) :m_cfg(cfg)
, m_TrdLife(false), m_hThread(), m_TrdId(0), m_receiveData()
, m_commSendms(0), m_mutex()
{
	m_ioReg.startAddrIn = cfg.startAddrIn;
	m_ioReg.startAddrOut = cfg.startAddrOut;
	m_ioReg.lenghtIn = cfg.lenghtIn;
	m_ioReg.lenghtOut = cfg.lenghtOut;
	m_ioReg.device_type = cfg.device_type;
	m_cfg.p_Comm->AttCallbackFunc(this, receiveDataCB);
	
	threadRun();

}

GFastech_IO::~GFastech_IO()
{
	threadStop();
}

uint32_t GFastech_IO::GetResponseTime() const
{
	return m_commPacketData.resp_time;
}

bool GFastech_IO::receivePacket()
{
	bool ret = false;
	uint8_t rx_data;
	int rx_size = m_Que.GetSize();

	if (rx_size < 1)
	{
		return ret;
	}

	enum class state
	{
		headerAA,
		length,
		syncNo,
		reserved,
		frameType,
		comm_status,
		rec_data,
	};
	if (util::millis() - m_commSendms > COMM_TIMEOUT_MS)
	{
		m_commPacketData.state = static_cast<uint8_t>(state::headerAA);
	}

	//state parsing_state = static_cast<state>(m_commPacketData.state);
	bool is_done = false;
	for (size_t i = 0; i < rx_size; i++)
	{
		m_Que.Get(&rx_data);

		switch (static_cast<state>(m_commPacketData.state))
		{
		case state::headerAA:
			if (rx_data == COMM_STX_AA)
			{
				m_commPacketData.state = static_cast<uint8_t>(state::length);
				m_commPacketData.index = 0;
				m_commPacketData.rx_packet.Init();

				m_commPacketData.rx_packet.buffer[m_commPacketData.index++] = rx_data;
			}
			else
			{
				if (rx_data == '\0')
				{
					ret = false;
				}
			}
			break;

		case state::length:
			//m_receiveData.length = rx_data;
			m_commPacketData.rx_packet.buffer[m_commPacketData.index++] = rx_data;
			m_commPacketData.rx_packet.length = rx_data;
			m_commPacketData.state = static_cast<uint8_t>(state::syncNo);
			break;

		case state::syncNo:
			m_commPacketData.rx_packet.buffer[m_commPacketData.index++] = rx_data;
			m_commPacketData.rx_packet.rec_sync_no = rx_data;
			m_commPacketData.state = static_cast<uint8_t>(state::reserved);
			break;

		case state::reserved:
			m_commPacketData.rx_packet.buffer[m_commPacketData.index++] = rx_data;
			m_commPacketData.state = static_cast<uint8_t>(state::frameType);
			break;

		case state::frameType:
			m_commPacketData.rx_packet.buffer[m_commPacketData.index++] = rx_data;
			m_commPacketData.rx_packet.cmd_type = rx_data;
			m_commPacketData.state = static_cast<uint8_t>(state::comm_status);
			break;


		case state::comm_status:
			m_commPacketData.rx_packet.buffer[m_commPacketData.index] = rx_data;
			m_commPacketData.rx_packet.response = rx_data;
			m_commPacketData.state = static_cast<uint8_t>(state::rec_data);
			// assign data address
			m_commPacketData.rx_packet.data = &m_commPacketData.rx_packet.buffer[m_commPacketData.index + 1];
			if (rx_data != 0)
			{
				m_commPacketData.state = static_cast<uint8_t>(state::headerAA); 
				//i = rx_size;
			}
			if (m_commPacketData.rx_packet.length == (m_commPacketData.index -1))
			{
				m_commPacketData.state = static_cast<uint8_t>(state::headerAA);
				//i = rx_size;
			}
			++m_commPacketData.index;
			break;

		case state::rec_data:
			m_commPacketData.rx_packet.buffer[m_commPacketData.index] = rx_data;
			is_done = m_commPacketData.rx_packet.length == (m_commPacketData.index-1);
			if (is_done)
			{
				m_commPacketData.resp_time = util::millis() - m_commSendms;
				m_commPacketData.wait_resp = false;
				m_commPacketData.state = static_cast<uint8_t>(state::headerAA);
				ret = true;
				i = rx_size;
			}
			else
			{
				++m_commPacketData.index;
			}
			break;

		default:
			break;
		}
	}

	return ret;
}


void GFastech_IO::receiveCplt()
{
	int ret = 0;

	switch (m_commPacketData.rx_packet.cmd_type)
	{
	case GetSlaveInfo:
		m_commPacketData.is_init = true;
		for (size_t i = 0; i < m_commPacketData.rx_packet.length; i++)
		{
			m_infoDevice.emplace_back(m_commPacketData.rx_packet.data[i]);
		}
		break;

	case IOGetOutput:
		if (COMM_IO_REG_LENGTH == m_commPacketData.rx_packet.length)
		{
			int idx = 0;
			// I16O16 셋팅 레지스터랑 순서가 달라서 수신된 데이터의 순서를 송신하는 레지스터 배열과 맞춘다.
			switch (m_cfg.device_type)
			{
			case type_e::I16O16:
				idx = 2;
				m_ioReg.set_out[2] = m_commPacketData.rx_packet.data[idx++];
				m_ioReg.set_out[3] = m_commPacketData.rx_packet.data[idx++];
				break;

			case type_e::O16:
				__fallthrough;
			case type_e::O32:
				m_ioReg.set_out[0] = m_commPacketData.rx_packet.data[idx++];
				m_ioReg.set_out[1] = m_commPacketData.rx_packet.data[idx++];
				m_ioReg.set_out[2] = m_commPacketData.rx_packet.data[idx++];
				m_ioReg.set_out[3] = m_commPacketData.rx_packet.data[idx++];

				break;
			default:
				break;
			}
		}
		break;

	case IOSetOutput:
		break;

	case IOGetInput:
		if (COMM_IO_REG_LENGTH == m_commPacketData.rx_packet.length)
		{
			int idx = 0;
			m_ioReg.input[0] = m_commPacketData.rx_packet.data[idx++];
			m_ioReg.input[1] = m_commPacketData.rx_packet.data[idx++];
			m_ioReg.input[2] = m_commPacketData.rx_packet.data[idx++];
			m_ioReg.input[3] = m_commPacketData.rx_packet.data[idx++];

			m_ioReg.latch[0] = m_commPacketData.rx_packet.data[idx++];
			m_ioReg.latch[1] = m_commPacketData.rx_packet.data[idx++];
			m_ioReg.latch[2] = m_commPacketData.rx_packet.data[idx++];
			m_ioReg.latch[3] = m_commPacketData.rx_packet.data[idx++];
		}
		break;

	default:
		break;
	}
}


void GFastech_IO::threadStop(void)
{
	m_TrdLife = false;
	while (m_TrdId);
}

void GFastech_IO::threadRun(void)
{
	m_TrdLife = true;
	// 포트 감시 스레드 생성
	m_hThread = CreateThread(
		(LPSECURITY_ATTRIBUTES)NULL,
		0,
		(LPTHREAD_START_ROUTINE)threadFunc,
		(LPVOID)this,
		0,
		&m_TrdId
	);
}

void GFastech_IO::threadJob(void)
{
	if (m_cfg.p_Comm->IsOpened() == false)
		return;

	errno_t ret;
	if (m_commPacketData.wait_resp == false)
	{
		if (m_commPacketData.is_init == false)
		{
			ret = getSlaveInfo(m_infoDevice);
			if (ret == ERROR_SUCCESS)
			{

			}
			//Sleep(100);
		}
		else
		{
			ret = getInputReg();
			ret = getOutReg();
		}
	}	
	else
	{
		if (util::millis() - m_commSendms > COMM_TIMEOUT_MS)
		{
			m_commPacketData.wait_resp = false;
		}
	}

	if (m_Que.Available())
	{
		if (receivePacket())
		{
			receiveCplt();
		}
	}
}




UINT GFastech_IO::threadFunc(LPVOID pParam)
{
	GFastech_IO* pThis = (GFastech_IO*)pParam;

	TRACE("fastech io commucation Thread Start\n\r");

	while (pThis->m_TrdLife)
	{
		pThis->threadJob();

		Sleep(10);
	}

	TRACE("fastech io commucation Thread Terminatet\n\r");
	pThis->m_TrdId = 0;
	pThis->m_hThread = NULL;
	return 0;
}

void GFastech_IO::receiveDataCB(void* obj, void* w_parm, void* l_parm)
{
	GFastech_IO* p_this = (GFastech_IO*)obj;
	if (w_parm == nullptr)
		return;
	int length = *((int*)w_parm);
	int index = 0;
	for (index = 0; index < length; index++)
	{
		uint8_t data = *((uint8_t*)l_parm + index);
		p_this->m_Que.Put(data);
	}
}





errno_t GFastech_IO::getSlaveInfo(vector<char>& ret_dat)
{
	errno_t ret = ERROR_SUCCESS;
	constexpr uint8_t Length = 0x03;
	array<uint8_t, 5> send_data{
			COMM_STX_AA,
			Length,
			++m_commPacketData.sync_no,
			COMM_RESERVED,
			GetSlaveInfo
	};
	m_commSendms = util::millis();
	//m_commPacketData.rx_packet.Init();
	int send_size = m_cfg.p_Comm->SendData((char*)send_data.data(), (uint32_t)send_data.size());
	m_commPacketData.wait_resp = true;
	if (send_size > 0)
	{
		uint32_t pre_ms = util::millis();
		bool result = true;
		while (receivePacket() == false)
		{
			if (pre_ms - util::millis() > COMM_TIMEOUT_MS)
			{
				result = false;
				ERR_PRINT("getSlaveInfo timeout!");
				break;
			}
		}
		if (result)
		{
			m_commPacketData.is_init = true;
			for (size_t i = 0; i < m_commPacketData.rx_packet.length; i++)
			{
				ret_dat.emplace_back(m_commPacketData.rx_packet.data[i]);
			}
		}
	}
	else
	{
		ret = -1;
	}

	return ret;
}


errno_t GFastech_IO::getInputReg()
{
	// TODO: 여기에 구현 코드 추가.
	errno_t ret = ERROR_SUCCESS;
	constexpr uint8_t Length = 0x03;
	array<uint8_t, 5> send_data{
			COMM_STX_AA,
			Length,
			++m_commPacketData.sync_no,
			COMM_RESERVED,
			IOGetInput
	};
	m_commSendms = util::millis();
	//m_commPacketData.rx_packet.Init();
	int send_size = m_cfg.p_Comm->SendData((char*)send_data.data(), (uint32_t)send_data.size());
	m_commPacketData.wait_resp = true;
	if (send_size > 0)
	{
		uint32_t pre_ms = util::millis();
		bool result = true;
		while (receivePacket() == false)
		{
			if (pre_ms - util::millis() > COMM_TIMEOUT_MS)
			{
				result = false;
				//ERR_PRINT("IOGetInput timeout!");
				break;
			}
		}
		if (result)
		{
			receiveCplt();			
		}
	}
	else
	{
		ret = -1;
	}

	
	return ret;
}



errno_t GFastech_IO::getOutReg()
{
	// TODO: 여기에 구현 코드 추가.
	errno_t ret = ERROR_SUCCESS;
	
	constexpr uint8_t Length = 0x03;
	array<uint8_t, 5> send_data{
			COMM_STX_AA,
			Length,
			++m_commPacketData.sync_no,
			COMM_RESERVED,
			IOGetOutput
	};
	m_commSendms = util::millis();
	//m_commPacketData.rx_packet.Init();
	int send_size = m_cfg.p_Comm->SendData((char*)send_data.data(), (uint32_t)send_data.size());
	m_commPacketData.wait_resp = true;
	if (send_size > 0)
	{
		uint32_t pre_ms = util::millis();
		bool result = true;
		while (receivePacket() == false)
		{
			if (pre_ms - util::millis() > COMM_TIMEOUT_MS)
			{
				result = false;
				//ERR_PRINT("IOGetOutput timeout!");
				break;
			}
		}
		if (result)
		{
			receiveCplt();
		}
	}
	else
	{
		ret = -1;
	}


	return ret;
}



errno_t GFastech_IO::sendOutReg(uint8_t* p_set, uint8_t* p_reset)
{
	errno_t ret = ERROR_SUCCESS;
	constexpr uint8_t Length = 0x0B;
	array<uint8_t, (2 + 11)> send_data{
			COMM_STX_AA,
			Length,
			++m_commPacketData.sync_no,
			COMM_RESERVED,
			IOSetOutput,
			p_set[0],
			p_set[1],
			p_set[2],
			p_set[3],
			p_reset[0],
			p_reset[1],
			p_reset[2],
			p_reset[3],
	};
	m_commSendms = util::millis();
	//m_commPacketData.rx_packet.Init();
	int send_size = m_cfg.p_Comm->SendData((char*)send_data.data(), (uint32_t)send_data.size());
	m_commPacketData.wait_resp = true;
	if (send_size > 0)
	{
		uint32_t pre_ms = util::millis();
		bool result = true;
		while (receivePacket() == false)
		{
			if (pre_ms - util::millis() > COMM_TIMEOUT_MS)
			{
				result = false;
				//ERR_PRINT("IOSetOutput timeout!");
				break;
			}
		}
		if (result)
		{
			receiveCplt();
		}
	}
	else
	{
		ret = -1;
	}


	return ret;
}


errno_t GFastech_IO::setOutputReg(uint32_t addr, bool is_set)
{
	// TODO: 여기에 구현 코드 추가.
	if (m_ioReg.startAddrOut > addr)
		return -1;

	div_t div_result = div((addr - m_ioReg.startAddrOut), COMM_BYTE_DIVISOR);
	if (is_set)
	{
		switch (m_cfg.device_type)
		{
		case type_e::I16O16:
			m_ioReg.reset_out[div_result.quot + 2] = 0;
			m_ioReg.set_out[div_result.quot + 2] |= (1 << div_result.rem);
			break;
		case type_e::O16:
			__fallthrough;
		case type_e::O32:
			m_ioReg.reset_out[div_result.quot] = 0;
			m_ioReg.set_out[div_result.quot] |= (1 << div_result.rem);
			break;
		}

	}
	else
	{
		switch (m_cfg.device_type)
		{
		case type_e::I16O16:
			m_ioReg.reset_out[div_result.quot + 2] |= (1 << div_result.rem);
			break;
		case type_e::O16:
			__fallthrough;
		case type_e::O32:
			m_ioReg.reset_out[div_result.quot] |= (1 << div_result.rem);
			break;
		}
	}

	array<uint8_t, 4> set_data{ 0, };
	array<uint8_t, 4> reset_data{ 0, };
	set_data = m_ioReg.set_out;
	reset_data = m_ioReg.reset_out;

	return sendOutReg(set_data.data(), reset_data.data());
}



errno_t GFastech_IO::toggleOutReg(uint32_t addr)
{
	if (m_ioReg.startAddrOut > addr)
		return -1;
	uint8_t data_set = 0;
	uint8_t data_reset = 0;
	bool is_reset = false;
	bool is_set = false;
	array<uint8_t, 4> set_data{ 0, };
	array<uint8_t, 4> reset_data{ 0, };
	set_data = m_ioReg.set_out;
	reset_data = m_ioReg.reset_out;
	div_t div_result = div((addr - m_ioReg.startAddrOut), COMM_BYTE_DIVISOR);

	if (m_ioReg.isOn(addr))
	{
		switch (m_cfg.device_type)
		{
		case type_e::I16O16:
			reset_data[div_result.quot + 2] |= (1 << div_result.rem);
			break;
		case type_e::O16:
			__fallthrough;
		case type_e::O32:
			reset_data[div_result.quot] |= (1 << div_result.rem);
			break;
		}
	}
	else
	{
		switch (m_cfg.device_type)
		{
		case type_e::I16O16:
			reset_data[div_result.quot + 2] = 0;
			set_data[div_result.quot + 2] |= (1 << div_result.rem);
			break;
		case type_e::O16:
			__fallthrough;
		case type_e::O32:
			reset_data[div_result.quot] = 0;
			set_data[div_result.quot] |= (1 << div_result.rem);
			break;
		}

	}
#if 0
	if (addr > 31)
		return -1;
	

	switch (m_cfg.device_type)
	{
	case type_e::I16O16:
		data_set = set_data[div_result.quot + 2];
		data_reset = reset_data[div_result.quot + 2];
		is_reset = (data_reset >> div_result.rem) & 1;
		is_set = (data_set >> div_result.rem) & 1;
		if (is_reset)
		{
			//on
			reset_data[div_result.quot + 2] = 0;
			set_data[div_result.quot + 2] |= (1 << div_result.rem);
		}
		else
		{
			if (is_set) 
			{
				//off
				reset_data[div_result.quot + 2] |= (1 << div_result.rem);
			}
			else
			{
				//on
				reset_data[div_result.quot + 2] = 0;
				set_data[div_result.quot + 2] |= (1 << div_result.rem);
			}
			
		}
		break;
	case type_e::O16:
		__fallthrough;
	case type_e::O32:
		data_set = set_data[div_result.quot];
		data_reset = reset_data[div_result.quot];
		is_reset = (data_reset >> div_result.rem) & 1;
		is_set = (data_set >> div_result.rem) & 1;
		if (is_reset)
		{
			//on
			reset_data[div_result.quot] = 0;
			set_data[div_result.quot] |= (1 << div_result.rem);
		}
		else
		{
			if (is_set)
			{
				//off
				reset_data[div_result.quot] |= (1 << div_result.rem);
			}
			else
			{
				//on
				reset_data[div_result.quot] = 0;
				set_data[div_result.quot] |= (1 << div_result.rem);
			}
			
		}
		break;
	}

#endif
	return sendOutReg(set_data.data(), reset_data.data());
}






#endif //





