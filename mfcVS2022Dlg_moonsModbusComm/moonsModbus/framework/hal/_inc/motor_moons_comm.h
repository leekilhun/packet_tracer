#pragma once
/*
moons rs485 modbus 통신을 위한 통신 클래스로
모터 객체에 shared된다.


*/


#ifdef USE_HW_COMM_MOONS

namespace GMoons
{


#define MOONS_COMM_BUFF_MAX  4096

	constexpr int MOONS_COMM_TIMEOUT = 1000;
	constexpr int MOONS_PACKET_BUFF_LENGTH = 100;
	constexpr int MOONS_MULTI_WRITE_DATA_LENGTH = 10;

	class GMoons_comm :public Icommon
	{
		/****************************************************
		 *	data
		 ****************************************************/
	public:
		struct cfg_t
		{
			IComm* pComm{};
			uint8_t base_node_id{};

			inline void operator = (const cfg_t* p_cfg) {
				this->base_node_id = p_cfg->base_node_id;
				this->pComm = p_cfg->pComm;
			}

		};

	private:
		struct tx_packet_t
		{
			uint8_t   node_id{};
			uint8_t   func_type{};
			uint16_t  reg_addr{};
			uint16_t  reg_length{};
			uint16_t  content{};  // case moons motor
			uint8_t   content_cnt{};

			uint16_t   check_sum{};
			uint16_t   check_sum_recv{};
			std::array <uint8_t, MOONS_PACKET_BUFF_LENGTH> buffer{};
		};

		struct rx_packet_t
		{
			uint8_t   node_id{};
			uint8_t   func_type{};
			uint16_t  reg_addr{};
			uint16_t  reg_length{};
			uint8_t   data_length{};
			uint8_t*	data{};
			uint16_t  check_sum{};
			uint16_t  check_sum_recv{};
			std::array <uint8_t, MOONS_PACKET_BUFF_LENGTH> buffer{};
		};


		struct modbus_t {
			uint8_t   state{};
			uint32_t  prev_ms{};
			uint32_t  resp_ms{};
			uint32_t  index{};
			uint32_t  data_cnt{};
			uint8_t   error{};

			tx_packet_t  tx_packet;
			rx_packet_t  rx_packet;
		};


	private:
		util::_que<uint8_t> m_Que;
		modbus_t m_packet;
		uint32_t m_packet_sending_ms;// 패킷 전송한 ms 시간
		std::mutex  m_mutax;
		void* m_cbObj;
		evt_cb m_func;

		bool m_TrdLock;
		bool m_TrdLife;
		HANDLE m_hThread;
		DWORD m_TrdId;


		cfg_t  m_cfg;


		/****************************************************
		   *	Constructor
		   ****************************************************/
	public:
		GMoons_comm(common::Attribute_t& common_data, cfg_t& cfg) :Icommon(common_data), m_cfg(cfg), m_cbObj(nullptr), m_func()
			, m_TrdLock(false), m_hThread(), m_TrdId(), m_packet_sending_ms{}
		{
			m_cfg.pComm->AttCallbackFunc(this, receiveDataFunc);
			errno_t ret = m_cfg.pComm->Open();
			if (ret != ERROR_SUCCESS)
			{
				ERR_PRINT("serial port open error : error_code[%d] ", ret);
			}
			LTRACE("Init moons_motor : serial port open \n");
			threadRun();
		}

		virtual ~GMoons_comm() {
			threadStop();
		}

		/****************************************************
		 *	func
		 ****************************************************/
	private:

		inline void threadStop(void) {
			m_TrdLife = false;
			while (m_TrdId);
		}
		inline void threadRun(void) {
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
		inline void threadJob(void) {
			if (m_TrdLock == false)
			{
				if (receivePacket() == true)
				{
					receiveCplt();
				}
			}
		}


    inline void receiveCplt()
    {
			std::vector <uint8_t> rec_data(m_packet.rx_packet.data_length);
			memcpy(rec_data.data(), m_packet.rx_packet.data, m_packet.rx_packet.data_length);
      if (m_cbObj)
      {
        uint32_t length = (uint32_t)rec_data.size();
        m_func(m_cbObj, (void*)&length, (void*)rec_data.data());
      }
    }



		inline bool receivePacket()
		{
			// TODO: 여기에 구현 코드 추가.
			using  func_e = moons_modbus_func_e;

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
			int data_length = m_Que.Available() % MOONS_PACKET_BUFF_LENGTH;
			if (data_length)
			{
				m_Que.Read(arry_data.data(), data_length);
			}
			else
			{
				return false;
			}

			rx_packet_t* rx_packet = &m_packet.rx_packet;

			if (util::millis() - m_packet.prev_ms >= 200)
			{
				m_packet.state = STATE_WAIT_ID;
				m_packet.index = 0;
				m_packet.data_cnt = 0;
				rx_packet->check_sum = 0xffff;
				memset(&rx_packet->buffer[0], 0x00, MOONS_PACKET_BUFF_LENGTH);
			}
			m_packet.prev_ms = util::millis();


			for (int i = 0; i < data_length; i++)
			{
				rx_data = arry_data[i];
				switch (m_packet.state)
				{
				case STATE_WAIT_ID:
					rx_packet->node_id = rx_data;
					m_packet.state = STATE_WAIT_FUNCNTION;
					util::crc16_modbus_update(&rx_packet->check_sum, rx_data);
					rx_packet->buffer[m_packet.index/* % MOONS_PACKET_BUFF_LENGTH*/] = rx_data;
					m_packet.index++;
					break;

				case STATE_WAIT_FUNCNTION:
					rx_packet->func_type = rx_data;
					util::crc16_modbus_update(&rx_packet->check_sum, rx_data);
					rx_packet->buffer[m_packet.index /*% MOONS_PACKET_BUFF_LENGTH*/] = rx_data;
					m_packet.index++;					

					switch (static_cast<func_e>(rx_data))
					{
					case func_e::read_HoldingReg:
						__fallthrough;
					case func_e::read_InputReg:
						m_packet.state = STATE_WAIT_LENGTH;
						break;

					case func_e::write_SingleReg:  // skip break;
						__fallthrough;
					case func_e::write_MultiReg:   // skip break;
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
					rx_packet->buffer[m_packet.index /*% MOONS_PACKET_BUFF_LENGTH*/] = rx_data;
					m_packet.index++;
					break;

				case STATE_WAIT_REG_ADDR_L:
					rx_packet->reg_addr = rx_data;
					m_packet.state = STATE_WAIT_REG_ADDR_H;
					util::crc16_modbus_update(&rx_packet->check_sum, rx_data);
					rx_packet->buffer[m_packet.index /*% MOONS_PACKET_BUFF_LENGTH*/] = rx_data;
					m_packet.index++;
					break;

				case STATE_WAIT_REG_ADDR_H:
					rx_packet->reg_addr |= (rx_data << 8);
					rx_packet->data_length = 2;// 0x06, 0x10 경우 2 byte; 
					m_packet.state = STATE_WAIT_DATA;
					util::crc16_modbus_update(&rx_packet->check_sum, rx_data);
					rx_packet->buffer[m_packet.index/* % MOONS_PACKET_BUFF_LENGTH*/] = rx_data;
					m_packet.index++;
					break;

				case STATE_WAIT_DATA:
					if (m_packet.data_cnt++ == 0)
					{
						rx_packet->data = &rx_packet->buffer[m_packet.index];
					}
					if (m_packet.data_cnt >= rx_packet->data_length)
					{
						m_packet.state = STATE_WAIT_CHECKSUM_L;
					}
					rx_packet->buffer[m_packet.index/* % MOONS_PACKET_BUFF_LENGTH*/] = rx_data;
					m_packet.index++;
					util::crc16_modbus_update(&rx_packet->check_sum, rx_data);
					break;


				case STATE_WAIT_CHECKSUM_L:
					rx_packet->check_sum_recv = rx_data;
					m_packet.state = STATE_WAIT_CHECKSUM_H;
					rx_packet->buffer[m_packet.index /*% MOONS_PACKET_BUFF_LENGTH*/] = rx_data;
					m_packet.index++;
					break;

				case STATE_WAIT_CHECKSUM_H:
					rx_packet->buffer[m_packet.index /*% MOONS_PACKET_BUFF_LENGTH*/] = rx_data;
					m_packet.index++;
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


#if 0
		inline int sendData(char* p_data, uint32_t length)
		{

			int ret = ERROR_SUCCESS;

			std::vector<char> v_str(length);
			for (size_t i = 0; i < length; i++) {
				v_str[i] = p_data[i];
			}
			uint8_t idx = 0;
			uint16_t result_crc = 0xFFFF;
			uint16_t ret_crc = (uint16_t)util::crc16_modbus_cal((uint8_t*)p_data, length);

			v_str.emplace_back((uint8_t)ret_crc);
			v_str.emplace_back((uint8_t)(ret_crc >> 8));

			if (this->sendPacketWaitRxResp((uint8_t*)v_str.data(), (uint32_t)v_str.size(), MOONS_COMM_TIMEOUT) != true)
			{
				ERR_PRINT("fail! moons_motor send packet data");
				ret = -1;
			}

			return ret;
		}



		inline bool sendPacketWaitRxResp(uint8_t* p_data, uint32_t length, uint32_t timeout) {
			bool  ret = false;
			uint32_t time_pre;

			m_TrdLock = true;

			m_cfg.pComm->SendData((char*)p_data, length);

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
#endif



		static UINT threadFunc(LPVOID pParam) {
			GMoons_comm* pThis = (GMoons_comm*)pParam;
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



		static void receiveDataFunc(void* obj, void* w_parm, void* l_parm) {
			GMoons_comm* pThis = (GMoons_comm*)obj;
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



	private:
		


		//inline void getMotorStatus(uint8_t id)
		//{
		//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
		//	constexpr int array_max = 6;
		//	using func_e = moons_modbus_func_e;
		//	using reg_e = moons_SS_reg_e;

		//	uint8_t  func = static_cast<uint8_t>(func_e::read_HoldingReg);
		//	uint16_t regist_no = static_cast<uint16_t>(reg_e::Alarm_Code_AL_f);
		//	uint16_t data_cnt = 4;
		//	std::array<uint8_t, array_max> arry_data =
		//	{
		//		id,
		//		func,
		//		(uint8_t)(regist_no >> 8),
		//		(uint8_t)regist_no ,
		//		(uint8_t)(data_cnt >> 8),
		//		(uint8_t)data_cnt
		//	};

		//	errno_t result = send_data(arry_data.data(), (uint32_t)arry_data.size());

		//	if (result == ERROR_SUCCESS)
		//	{
		//		m_mtStatus.sc_status = (uint16_t)(m_packet.rx_packet.data[2] << 8) | (uint16_t)(m_packet.rx_packet.data[3] << 0);
		//		m_mtAlarmCode.al_status = (uint16_t)(m_packet.rx_packet.data[0] << 8) | (uint16_t)(m_packet.rx_packet.data[1] << 0);
		//	}

		//}


	public:
		inline bool IsOpened() const {
			return m_cfg.pComm->IsOpened();
		}

		inline void AttCallbackFunc(void* obj, evt_cb cb)
		{
			m_cbObj = obj;
			m_func = cb;
		}



		inline errno_t SendCmd(uint8_t* arry_data, uint32_t size) {
			errno_t ret = ERROR_SUCCESS;
			if (m_cfg.pComm->IsOpened() == false)
				return -1;


			std::vector<uint8_t> send_data(size);
			memcpy(send_data.data(), arry_data, size);

			uint16_t result_crc = 0xFFFF;
			uint16_t ret_crc = (uint16_t)util::crc16_modbus_cal(&send_data[0], size);

			send_data.emplace_back((uint8_t)ret_crc);
			send_data.emplace_back((uint8_t)(ret_crc >> 8));

			this->m_packet_sending_ms = util::millis();			
			
			int send_size = m_cfg.pComm->SendData((char*)send_data.data(), (uint32_t)send_data.size());
			if (send_size == 0)
				ret = -1;
			return ret;
		}


		inline errno_t SendCmdRxResp(uint8_t* arry_data, uint32_t size, uint32_t timeout) {
			errno_t ret = ERROR_SUCCESS;
			if (m_cfg.pComm->IsOpened() == false)
				return -1;
			
			uint32_t check_ms = 0;
			this->m_packet_sending_ms = util::millis();
			//m_packet.rx_packet.wait_resp = true;
			m_TrdLock = true;
			if (SendCmd(arry_data, size) == ERROR_SUCCESS)
			{
				uint32_t pre_ms = util::millis();
				bool result = true;
				while (receivePacket() == false)
				{
					if ((util::millis() - pre_ms) > 200)
					{
						result = false;
						ERR_PRINT("moons modbus communication send data - timeout!");
						break;
					}
					Sleep(10);
				}
				if (result)
				{
					check_ms = (util::millis() - pre_ms);
					receiveCplt();
				}
			}
			else
			{
				ret = -1;
			}
			m_TrdLock = false;
			return ret;
		}

	



	};

}
#endif // #ifndef USE_HW_COMM_MOONS