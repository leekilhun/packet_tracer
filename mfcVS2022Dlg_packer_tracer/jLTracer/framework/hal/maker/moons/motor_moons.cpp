#include "pch.h"
#include "hal_def.h"


#ifdef USE_HW_MOTOR_MOONS


#include "SCLLibHelper.h"
#include "motor_moons.h"


#define LOG_WRITE(fmt, ...)		m_log.syslog_Write(loglevel::info, m_Attribute.GetObjectID(), m_Attribute.GetLogFileName(), (__FILE__), __LOGFUNC__, __LINE__, fmt, ##__VA_ARGS__)
#define ERR_WRITE(fmt, ...)		m_log.syslog_Write(loglevel::err, m_Attribute.GetObjectID(), m_Attribute.GetLogFileName(), (__FILE__), __LOGFUNC__, __LINE__, fmt, ##__VA_ARGS__)

moons_motor::moons_motor(common::Attribute_t& common_data, cfg_t& cfg)
	: Icommon(common_data), m_pComm(cfg.pComm), m_AxisId(cfg.AxisId), m_IsMotorOn(false), m_cbObj(nullptr), m_func(), m_TrdLock(false)
{

  m_pComm->AttCallbackFunc(this, receiveDataCB);
	errno_t ret = m_pComm->Open();
	if (ret != ERROR_SUCCESS)
	{
		ERR_WRITE("serial port open error : error_code[%d] ", ret);
	}
  LOG_WRITE("Init moons_motor[%d] : ", m_Attribute.GetInstanceNo());
  threadRun();

#ifdef _DEBUG
  cliAdd(this, "moons", cliMoonsTest);
#endif

}


moons_motor::~moons_motor()
{
  threadStop();
}



errno_t moons_motor::Initialize()
{
  errno_t ret = 0;

  return ret;
}


errno_t moons_motor::MotorOnOff(bool on_off)
{

	std::array<uint8_t, 8> send_data{ 0x00, };
	uint8_t idx = 0;
	uint16_t result_crc = 0xFFFF;
	uint8_t set_data = 0x00;
	//
	m_packet.tx_packet.node_id = m_AxisId;
	send_data[idx++] = m_packet.tx_packet.node_id;
	//
	m_packet.tx_packet.func_type = static_cast<uint8_t>(func_e::write_SingleReg);
	send_data[idx++] = m_packet.tx_packet.func_type;
	//	
	m_packet.tx_packet.reg_addr = static_cast<uint16_t>(reg_e::cmd_Opcode);
	send_data[idx++] = (uint8_t)(m_packet.tx_packet.reg_addr>>8);
	send_data[idx++] = (uint8_t)(m_packet.tx_packet.reg_addr);

	if (on_off)
	{
		m_packet.tx_packet.content = static_cast<uint16_t>(Opcode_e::motor_enable);
	}
	else
	{
		m_packet.tx_packet.content = static_cast<uint16_t>(Opcode_e::motor_disable);
	}
	send_data[idx++] = (uint8_t)(m_packet.tx_packet.content >> 8);
	send_data[idx++] = (uint8_t)(m_packet.tx_packet.content);

	uint16_t ret_crc = (uint16_t)util::crc16_modbus_cal(send_data.data(), idx);
	send_data[idx++] = (uint8_t)ret_crc;
	send_data[idx++] = (uint8_t)(ret_crc >> 8);



  errno_t ret = ERROR_SUCCESS;    
	int size = (int)send_data.size();
	if (sendPacketWaitRxResp(send_data.data(), (uint32_t)size, 1000) != true)
	{
		ERR_PRINT("fail! moons_motor send packet data");
		ret = -1;
	}


	{ //TX
		std::vector<char> msg{ 'T','X' };
		for (char elm : send_data)
    {
      msg.push_back(elm);
    }

    msg.push_back('\r');
    msg.push_back('\n');
    int size = (int)msg.size();
    if (m_cbObj)
    {
      m_func(m_cbObj, &size, &msg[0]);
    }
  }

	{//RX
    std::vector<char> msg{ 'R','X' , (char)ret };
    if (ret != 0)
    {
      for (char elm : "wait packet timeout!")
      {
        msg.push_back(elm);
      }


      int size = (int)msg.size();
      if (m_cbObj)
      {
        m_func(m_cbObj, &size, &msg[0]);
      }
		}
    else
    {
      for (uint32_t i = 0; i < m_packet.index; i++)
      {
        msg.push_back(m_packet.rx_packet.buffer[i]);
      }

      int size = (int)msg.size();
      if (m_cbObj)
      {
        m_func(m_cbObj, &size, &msg[0]);
      }
    }
	}
	



	/*if (m_cbObj)
	{
		m_func(m_cbObj, &size, &send_data[0]);
	}*/


  return ret;
}
bool moons_motor::IsMotorOn() const
{
  return m_IsMotorOn;
}


bool moons_motor::sendPacketWaitRxResp(uint8_t* p_data, uint32_t length, uint32_t timeout)
{
	bool  ret = false;
	uint32_t time_pre;

	m_TrdLock = true;

	m_pComm->SendData((char*)p_data, length);

	time_pre = util::millis();

	while (1)
	{
		if (receivePacket() == true)
		{
			ret = true;
			break;
		}

		if (util::millis() - time_pre >= timeout)
		{
			ERR_PRINT("wait packet timeout!  wait response packet data");
			break;
		}
		Sleep(10);
	}


	m_TrdLock = false;

	return ret;
}


void moons_motor::receiveCplt()
{
	{//RX
		std::vector<char> msg{ 'R','X' , (char)0 };
		{
			for (uint32_t i = 0; i < m_packet.index; i++)
			{
				msg.push_back(m_packet.rx_packet.buffer[i]);
			}

			int size = (int)msg.size();
			if (m_cbObj)
			{
				m_func(m_cbObj, &size, &msg[0]);
			}
		}
	}

}

bool moons_motor::receivePacket()
{
	// TODO: 여기에 구현 코드 추가.
	constexpr int STATE_WAIT_ID = 0;
	constexpr int STATE_WAIT_FUNCNTION = 1;
	constexpr int STATE_WAIT_LENGTH = 2;
	constexpr int STATE_WAIT_REG_ADDR_L = 3;
	constexpr int STATE_WAIT_REG_ADDR_H = 4;
	constexpr int STATE_WAIT_DATA = 5;
	constexpr int STATE_WAIT_CHECKSUM_L = 6;
	constexpr int STATE_WAIT_CHECKSUM_H = 7;



	bool ret = false;
	uint8_t rx_data = 0x00;
	std::array<uint8_t, MOONS_PACKET_BUFF_LENGTH> arry_data{ 0, };
	int data_length = m_Que.Available();
	if (data_length)
	{
		m_Que.Read(arry_data.data(), data_length);		
	}
	else
	{
		return false;
	}

	rx_packet_t* rx_packet = &m_packet.rx_packet;

	if (util::millis() - m_packet.pre_time >= 100)
	{
		m_packet.state = STATE_WAIT_ID;
		m_packet.index = 0;
		m_packet.data_cnt = 0;
		rx_packet->check_sum = 0xffff;
		memset(&rx_packet->buffer[0], 0x00, MOONS_PACKET_BUFF_LENGTH);
	}
	m_packet.pre_time = util::millis();


	for (int i = 0; i < data_length; i++)
	{
		rx_data = arry_data[i];
		switch (m_packet.state)
		{
		case STATE_WAIT_ID:
			rx_packet->node_id = rx_data;
			m_packet.state = STATE_WAIT_FUNCNTION;
			util::crc16_modbus_update(&rx_packet->check_sum, rx_data);
			rx_packet->buffer[m_packet.index++] = rx_data;
			break;

		case STATE_WAIT_FUNCNTION:
			rx_packet->func_type = rx_data;
			util::crc16_modbus_update(&rx_packet->check_sum, rx_data);
			rx_packet->buffer[m_packet.index++] = rx_data;

			switch (static_cast<func_e>(rx_data))
			{
			case moons_motor::func_e::read_HoldingReg:
				__fallthrough;
			case moons_motor::func_e::read_InputReg:
				m_packet.state = STATE_WAIT_LENGTH;
				break;

			case moons_motor::func_e::write_SingleReg:  // skip break;
				__fallthrough;
			case moons_motor::func_e::write_MultiReg:   // skip break;
				__fallthrough;
			default:
				m_packet.state = STATE_WAIT_REG_ADDR_L;
				break;
			}
			break;

		case STATE_WAIT_LENGTH:
			rx_packet->data_length = rx_data;
			m_packet.state = STATE_WAIT_DATA;
			util::crc16_modbus_update(&rx_packet->check_sum, rx_data);
			rx_packet->buffer[m_packet.index++] = rx_data;
			break;

		case STATE_WAIT_REG_ADDR_L:
			rx_packet->reg_addr = rx_data;
			m_packet.state = STATE_WAIT_REG_ADDR_H;
			util::crc16_modbus_update(&rx_packet->check_sum, rx_data);
			rx_packet->buffer[m_packet.index++] = rx_data;
			break;

		case STATE_WAIT_REG_ADDR_H:
			rx_packet->reg_addr |= (rx_data << 8);
			rx_packet->data_length = 2;// 0x06, 0x10 경우 2 byte; 
			m_packet.state = STATE_WAIT_DATA;
			util::crc16_modbus_update(&rx_packet->check_sum, rx_data);
			rx_packet->buffer[m_packet.index++] = rx_data;	
			break;

		case STATE_WAIT_DATA:
			if (m_packet.data_cnt++ == 0)
			{
				rx_packet->data = &rx_packet->buffer[m_packet.index];
			}
			if (m_packet.data_cnt == rx_packet->data_length)
			{
				m_packet.state = STATE_WAIT_CHECKSUM_L;
			}
			rx_packet->buffer[m_packet.index++] = rx_data;
			util::crc16_modbus_update(&rx_packet->check_sum, rx_data);
			break;


		case STATE_WAIT_CHECKSUM_L:
			rx_packet->check_sum_recv = rx_data;
			m_packet.state = STATE_WAIT_CHECKSUM_H;
			rx_packet->buffer[m_packet.index++] = rx_data;
			break;

		case STATE_WAIT_CHECKSUM_H:
			rx_packet->buffer[m_packet.index++] = rx_data;
			rx_packet->check_sum_recv |= (rx_data << 8);
			if (rx_packet->check_sum == rx_packet->check_sum_recv)
			{
				ret = true;
			}
			m_packet.state = STATE_WAIT_ID;
			break;
		}

	}


	return ret;
}



errno_t moons_motor::GetReg(moons_SS_reg_e reg)
{

	return errno_t();
}


int moons_motor::GetPosition()
{
	int ret = 0;


	return ret;
}

errno_t  moons_motor::GetMotorData()
{
	errno_t ret = ERROR_SUCCESS;
	std::array<uint8_t, 8> send_data{ 0x00, };
	uint8_t idx = 0;
	uint16_t result_crc = 0xFFFF;
	uint8_t set_data = 0x00;
	//
	m_packet.tx_packet.node_id = m_AxisId;
	send_data[idx++] = m_packet.tx_packet.node_id;
	//
	m_packet.tx_packet.func_type = static_cast<uint8_t>(func_e::read_HoldingReg);
	send_data[idx++] = m_packet.tx_packet.func_type;
	//	
	m_packet.tx_packet.reg_addr = static_cast<uint16_t>(reg_e::alarm_code);
	send_data[idx++] = (uint8_t)(m_packet.tx_packet.reg_addr >> 8);
	send_data[idx++] = (uint8_t)(m_packet.tx_packet.reg_addr);
	uint16_t get_reg_length = 14;
	m_packet.tx_packet.content = get_reg_length;	
	send_data[idx++] = (uint8_t)(m_packet.tx_packet.content >> 8);
	send_data[idx++] = (uint8_t)(m_packet.tx_packet.content);

	uint16_t ret_crc = (uint16_t)util::crc16_modbus_cal(send_data.data(), idx);
	send_data[idx++] = (uint8_t)ret_crc;
	send_data[idx++] = (uint8_t)(ret_crc >> 8);

	
	m_pComm->SendData((char*)send_data.data(), (uint32_t)send_data.size());

	return ret;
}



void moons_motor::threadStop(void)
{
  m_TrdLife = false;
  while (m_TrdId);
}
void moons_motor::threadRun(void)
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
void moons_motor::threadJob(void)
{
	if (m_TrdLock)
		return;
  //if (m_Que.Available() > 0)
  //{
  //  //
		//uint8_t data = 0;
		//m_Que.Get(&data);
  //}

	if (receivePacket() == true)
	{
		receiveCplt();
	}

}
UINT moons_motor::threadFunc(LPVOID pParam)
{
  moons_motor* pThis = (moons_motor*)pParam;
  LTRACE("moons_motor Thread Start\n\r");

  while (pThis->m_TrdLife)
  {
    pThis->threadJob();

    Sleep(10);
  }

  LTRACE("moons_motor Thread Terminatet\n\r");
  pThis->m_TrdId = 0;
  pThis->m_hThread = NULL;
  return 0;

}

/****************************************************
 *
 ****************************************************/

void moons_motor::AttCallbackFunc(void* obj, evt_cb cb)
{
	 m_cbObj = obj;
	 m_func = cb;
}


void moons_motor::receiveDataCB(void* obj, void* w_parm, void* l_parm)
{
  moons_motor* pThis = (moons_motor*)obj;
	if (w_parm == nullptr)
		return;
	int length = *((int*)w_parm);
	int index = 0;
	for (index = 0; index < length; index++)
	{
		uint8_t data = *((uint8_t*)l_parm + index);

		pThis->m_Que.Put(data);
	}
}




#ifdef _DEBUG

void moons_motor::cliMoonsTest(void* obj, cli_args_t* args)
{
  moons_motor* pThis = (moons_motor*)obj;
	bool ret = true;
	if (args->argc == 3)
	{
		if (args->isStr(0, "write_dio_out"))
		{

		}
		else if (args->isStr(0, "write_rio_out"))
		{

		}
		else
		{
			ret = false;
		}
	}
	else	if (args->argc == 2)
	{
		int value = (int)args->getData(1);
		if (args->isStr(0, "jog"))
		{
		
		}
		else if (args->isStr(0, "motion"))
		{
			

		}
		else
		{
			ret = false;
		}
	}
	else if (args->argc == 1)
	{
		errno_t result = 0;
		if (args->isStr(0, "motor_off"))
		{
			result = pThis->MotorOnOff( false);
			if (result == ERROR_SUCCESS)
			{
				cliPrintf("motor_off success\n");
			}
			else
			{
				cliPrintf("motor_off faile\n");
			}
		}
		else if (args->isStr(0, "motor_on"))
		{
			result = pThis->MotorOnOff(true);
			if (result == ERROR_SUCCESS)
			{
				cliPrintf("motor_on success\n");
			}
			else
			{
				cliPrintf("motor_on faile\n");
			}
		}
		else if (args->isStr(0, "alarm_reset"))
		{
			
		}
	
		
		else if (args->isStr(0, "move"))
		{
			

		}
		else if (args->isStr(0, "init"))
		{
			pThis->Initialize();
		}
		else
		{
			ret = false;
		}
	}
	else
	{
		ret = false;
	}


	if (ret == false)
	{
		cliPrintf("moons motor_on\n");
		cliPrintf("moons motor_off\n");
		cliPrintf("moons alarm_reset\n");
		cliPrintf("moons init\n");
		cliPrintf("moons move\n");


	}
}


#endif

// 패킷 데이터를 받아 crc 추가해 보낸다
int moons_motor::SendPacket(char** packets, int cnt)
{
	// TODO: 여기에 구현 코드 추가.
	int ret = ERROR_SUCCESS;
	std::vector<char*> rev_char(cnt);
	std::vector<uint8_t> send_data(cnt);
	for (int i = 0; i < cnt; i++)
	{
		rev_char[i] = packets[i];
		send_data[i] = (uint8_t)strtol(rev_char[i], nullptr, 16);
	}
	
	uint8_t idx = 0;
	uint16_t result_crc = 0xFFFF;
	uint16_t ret_crc = (uint16_t)util::crc16_modbus_cal(&send_data[0], (uint32_t)send_data.size());

	send_data.push_back((uint8_t)ret_crc);
	send_data.push_back((uint8_t)(ret_crc>>8));
	
	if (this->sendPacketWaitRxResp(&send_data[0], (uint32_t)send_data.size(), 1000) != true)
	{
		ERR_PRINT("fail! moons_motor send packet data");
		ret = -1;
	}


	{ //TX
		std::vector<char> msg{ 'T','X' };
		for (char elm : send_data)
		{
			msg.push_back(elm);
		}

		msg.push_back('\r');
		msg.push_back('\n');
		int size = (int)msg.size();
		if (m_cbObj)
		{
			m_func(m_cbObj, &size, &msg[0]);
		}
	}

	{//RX
		std::vector<char> msg{ 'R','X' , (char)ret };
		if (ret != ERROR_SUCCESS)
		{
			for (char elm : "wait packet timeout!")
			{
				msg.push_back(elm);
			}


			int size = (int)msg.size();
			if (m_cbObj)
			{
				m_func(m_cbObj, &size, &msg[0]);
			}
		}
		else
		{
			for (uint32_t i = 0; i < m_packet.index; i++)
			{
				msg.push_back(m_packet.rx_packet.buffer[i]);
			}

			int size = (int)msg.size();
			if (m_cbObj)
			{
				m_func(m_cbObj, &size, &msg[0]);
			}
		}
	}





	return 0;
}


#endif