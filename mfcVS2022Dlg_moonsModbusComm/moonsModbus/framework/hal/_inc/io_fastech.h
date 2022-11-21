#pragma once

//#include <array>
#ifdef USE_HW_IO_FASTECH

#define NET_IO_1616_REG_OUT_1       B00000000000000010000000000000000 
#define NET_IO_1616_REG_OUT_2       B00000000000000100000000000000000 
#define NET_IO_1616_REG_OUT_3       B00000000000001000000000000000000 
#define NET_IO_1616_REG_OUT_4       B00000000000010000000000000000000 
#define NET_IO_1616_REG_OUT_5       B00000000000100000000000000000000 
#define NET_IO_1616_REG_OUT_6       B00000000001000000000000000000000 
#define NET_IO_1616_REG_OUT_7       B00000000010000000000000000000000 
#define NET_IO_1616_REG_OUT_8       B00000000100000000000000000000000 

#define NET_IO_1616_REG_OUT_9       B00000001000000000000000000000000 
#define NET_IO_1616_REG_OUT_10      B00000010000000000000000000000000 
#define NET_IO_1616_REG_OUT_11      B00000100000000000000000000000000 
#define NET_IO_1616_REG_OUT_12      B00001000000000000000000000000000 
#define NET_IO_1616_REG_OUT_13      B00010000000000000000000000000000 
#define NET_IO_1616_REG_OUT_14      B00100000000000000000000000000000 
#define NET_IO_1616_REG_OUT_15      B01000000000000000000000000000000 
#define NET_IO_1616_REG_OUT_16      B10000000000000000000000000000000 




#define NET_IO_FUNC_REG_GET_OUT     0xC5
#define NET_IO_FUNC_REG_SET_OUT     0xC6

#define NET_IO_FUNC_REG_GET_IN      0xC0

#define NET_IO_COMM_BUFF_MAX               (uint32_t)4096
#define DEF_FASTECH_COMM_BUFFER_MAX (uint32_t)4096

constexpr static int DEF_INPUT_BYTE_LENGTH = 4;

namespace GFastech
{

	using namespace std;


	class GFastech_IO :public IIO
	{
		/****************************************************
		 *	data
		 ****************************************************/
	public:
		enum class mode_e
		{
			RS_485,
			ETHERNET,
		};

		enum class type_e
		{
			I16O16,
			I16,
			O16,
			O32,
			I32
		};


		struct cfg_t
		{
			GFastech_IO::mode_e mode{};
			IComm* p_Comm{};
			type_e device_type{};
			uint32_t startAddrIn{};
			uint32_t lenghtIn{};
			uint32_t startAddrOut{};
			uint32_t lenghtOut{};

			inline void operator = (const cfg_t* p_cfg) {
				this->mode = p_cfg->mode;
				this->p_Comm = p_cfg->p_Comm;
				this->device_type = p_cfg->device_type;
				this->startAddrIn = p_cfg->startAddrIn;
				this->lenghtIn = p_cfg->lenghtIn;
				this->startAddrOut = p_cfg->startAddrOut;
				this->lenghtOut = p_cfg->lenghtOut;
			}

		};


		struct netIO_receiveData
		{
			uint8_t length{};
			uint8_t syncNo{};
			uint8_t reserved{};
			uint8_t frameType{};
			uint8_t comm_state{};
			uint8_t input[4]{};
			uint8_t latch[4]{};

		};

		struct packet_t
		{
			uint8_t  id{};
			uint8_t   rec_sync_no{};
			uint8_t   cmd_type{};
			uint8_t   response{};
			uint16_t  length{};
			uint32_t  check_sum{};
			uint16_t  check_sum_recv{};
			array<uint8_t, DEF_FASTECH_COMM_BUFFER_MAX> buffer{};
			uint8_t* data;


			void Init() {
				id = 0;
				rec_sync_no = 0;
				cmd_type = 0;
				response = 0;
				length = 0;
				check_sum = 0;
				check_sum_recv = 0;
				buffer.fill(0);
				data = nullptr;
			}
		};


		struct comm_t
		{
			bool      is_init{};
			//bool		wait_next{};
			bool      wait_resp{};
			uint32_t  resp_time{};
			uint8_t   state{};
			uint8_t   sync_no{};
			uint32_t  index{};
			uint8_t   error{};
			packet_t  rx_packet{};
		};

		struct ioDat_t
		{
			type_e device_type{};
			uint32_t startAddrIn{};
			uint32_t lenghtIn{};
			uint32_t startAddrOut{};
			uint32_t lenghtOut{};
			array<uint8_t, 4> input{ 0, };
			array<uint8_t, 4> latch{ 0, };
			array<uint8_t, 4> set_out{ 0, };
			array<uint8_t, 4> reset_out{ 0, };

			/// <summary>
			/// reg 정보 확인
			/// </summary>
			/// <param name="addr">시스템에 등록된 주소</param>
			/// <returns>on true, off false</returns>
			bool isOn(uint32_t addr) {
				bool ret = false;
				div_t div_result;
				int elm_idx = 0;
				int elm_ordr = 0;
				if (addr > (startAddrOut - 1))
				{
					// case out
					switch (device_type)
					{
					case GFastech_IO::type_e::I16O16:
						div_result = div((addr - startAddrOut), 8);
						elm_idx = div_result.quot + 2;
						elm_ordr = div_result.rem;
						ret = (((int)set_out[elm_idx]>>elm_ordr) & 1) == true;
						if (((int)reset_out[elm_idx] >> div_result.rem) & 1)
						{
							ret = false;
						}
						break;

					case GFastech_IO::type_e::O16:
						__fallthrough;
					case GFastech_IO::type_e::O32:
						div_result = div((addr - startAddrOut), 8);
						ret = (bool)((int)(set_out[div_result.quot] >> div_result.rem) & 1);
						if (((int)reset_out[div_result.quot] >> div_result.rem) & 1)
						{
							ret = false;
						}
						break;
					}
				}
				else
				{
					// case in
					div_result = div((addr - startAddrIn), 8);
					ret = (input[div_result.quot] >> div_result.rem) & 1;
				}
				return ret;
			}



		};




	private:
		uint32_t m_commSendms;
		cfg_t m_cfg;
		netIO_receiveData m_receiveData;
		comm_t m_commPacketData;
		mutex m_mutex;
		vector<char> m_infoDevice;
		ioDat_t m_ioReg;

		util::_que<uint8_t> m_Que{ NET_IO_COMM_BUFF_MAX };
		bool m_TrdLife;
		HANDLE m_hThread;
		DWORD m_TrdId;



		/****************************************************
		 *	Constructor
		 ****************************************************/
	public:
		GFastech_IO(GFastech_IO::cfg_t& cfg);
		virtual ~GFastech_IO();

		/****************************************************
		 *	func
		 ****************************************************/
	public:
		uint32_t GetResponseTime() const;
	private:
		bool receivePacket();
		void threadStop(void);
		void threadRun(void);
		void threadJob(void);
		static UINT threadFunc(LPVOID pParam);
		static void receiveDataCB(void* obj, void* w_parm, void* l_parm);


		void receiveCplt();


	public:
		inline bool IsOn(uint32_t addr) {
			bool ret = false;
			m_mutex.lock();
			ret = m_ioReg.isOn(addr);
			m_mutex.unlock();
			return ret;
		}
		inline bool IsOff(uint32_t addr) {
			bool ret = false;
			m_mutex.lock();
			ret = !m_ioReg.isOn(addr);
			m_mutex.unlock();
			return ret;
		}
		inline errno_t OutputOn(uint32_t addr) {
			errno_t ret = -1;
			m_mutex.lock();
			ret = setOutputReg(addr);
			m_mutex.unlock();
			return ret;
		}
		inline errno_t OutputOff(uint32_t addr) {
			errno_t ret = -1;
			m_mutex.lock();
			ret = setOutputReg(addr, false);
			m_mutex.unlock();
			return ret;
		}
		inline errno_t OutputToggle(uint32_t addr) {
			errno_t ret = -1;
			m_mutex.lock();
			ret = toggleOutReg(addr);
			m_mutex.unlock();
			return ret;
		}



	private:
		errno_t getSlaveInfo(vector<char>& ret_dat);
		errno_t getInputReg();
		errno_t setOutputReg(uint32_t addr, bool is_set = true);
		//errno_t sendOutReg();
		errno_t sendOutReg(uint8_t* p_set, uint8_t* p_reset);
		errno_t toggleOutReg(uint32_t addr);
		errno_t getOutReg();
	};

}
#endif
