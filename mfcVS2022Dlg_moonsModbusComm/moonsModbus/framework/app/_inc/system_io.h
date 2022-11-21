#pragma once

//class ioFastech;

#ifndef USE_IO_DATA_FORMAT_DAT
#define USE_IO_DATA_FORMAT_JSON
#define IO_DATA_DEFAULT_PATH     "./Data/io_dat.json"
#else
#define IO_DATA_DEFAULT_PATH     "./Data/io.dat"
#endif

#define IO_NAME_STR_MAX				    PRJ_NAME_STR_LENGTH_MAX

#define IO_DAT_ARG_TBL_CNT_MAX    3000

#pragma region io table class, script data parsing



class GDataTable_IO : public util::parser::json
{
	enum class type_e
	{
		in_t,
		out_t,
	};

public:
	struct dat_t {
		GDataTable_IO::type_e type{};
		char name[IO_NAME_STR_MAX]{};
		uint32_t addr{};
	};

private:
	//dat_t* m_pIn[IO_INPUT_MAX]{};
	std::vector<dat_t> m_vtIn;
	//uint32_t m_inCnt{};

	//dat_t* m_pOut[IO_OUTPUT_MAX]{};
	std::vector<dat_t> m_vtOut;
	// uint32_t m_outCnt{};

	char m_fileInfo[PRJ_FILE_DIR_STR_MAX]{};

	/****************************************************
	 *	Constructor
	 ****************************************************/
public:
	GDataTable_IO(char* file_info = IO_DATA_DEFAULT_PATH) :util::parser::json(file_info, IO_OUTPUT_MAX + IO_INPUT_MAX)/*m_pParser(nullptr)*/
	{
		strcpy_s(m_fileInfo, strlen(file_info) + 1, file_info);
		m_vtIn.reserve(IO_OUTPUT_MAX);
		m_vtOut.reserve(IO_OUTPUT_MAX);
	}

	virtual ~GDataTable_IO()
	{
#if 0
		while (m_inCnt)
		{
			if (m_pIn[m_inCnt])
			{
				delete m_pIn[m_inCnt];
			}
			m_pIn[m_inCnt--] = nullptr;
		}
		delete m_pIn[m_inCnt];
		m_pIn[m_inCnt] = nullptr;

		while (m_outCnt)
		{
			if (m_pOut[m_outCnt])
			{
				delete m_pOut[m_outCnt];
			}
			m_pOut[m_outCnt--] = nullptr;
		}
		delete m_pOut[m_outCnt];
		m_pOut[m_outCnt] = nullptr;
#endif
	}

	/****************************************************
	 *	func
	 ****************************************************/
private:
#if 0
	inline bool putData(char** pdata, uint8_t cnt) {
		return false;
	}

	inline void parsingJson(char* p_data, uint32_t size)
	{
		enum class state_t {
			section,
			key,
			value,
			partition,
		};

		type_e data_type = type_e::in_t;
		uint32_t curr = 0;
		char data;
		state_t state = state_t::section;
		if (p_data[curr] != '{' && p_data[curr] != '[')
			return;

		curr++;
		// 문서 크기만큼 반복
		while (curr < size)
		{
			data = p_data[curr];
			switch (state)
			{
			case state_t::section:
			{
				switch (data)
				{
				case '\"':
				{
					state = state_t::key;
				}
				break;
				case '/':
				{
					if (p_data[curr + 1] == '/') // 주석 처리
					{
						char* begin = p_data + curr + 1;
						char* end = strchr(begin, '\n');
						size_t stringLength = end - begin;
						curr = curr + (uint32_t)stringLength + 1;
					}
				}
				break;
				case '\n':case '\t':case ' ': //
				default:
					break;
				}//switch

			}
			break;
			case state_t::key:
			{
				char* begin = p_data + curr;
				char* end = strchr(begin, '\"');
				char tmp_str[IO_NAME_STR_MAX] = { 0, };
				size_t stringLength = end - begin;
				memcpy(tmp_str, begin, stringLength % IO_NAME_STR_MAX);
				curr += (uint32_t)stringLength;

				state = state_t::partition;

				// check type
				if (util::IsEqualStr("input", tmp_str, true))
					data_type = type_e::in_t;
				else if (util::IsEqualStr("output", tmp_str, true))
					data_type = type_e::out_t;
				else
					state = state_t::section;

			}
			break;
			case state_t::partition:
			{
				if (data == ':')
				{
					state = state_t::value;
				}
				if (data == ',')
				{
					state = state_t::section;
				}
			}
			break;
			case state_t::value:
			{
				switch (data)
				{
				case '[':        // 문자가 [이면 배열
				{
					char* begin = p_data + curr;
					char* end = strchr(begin, ']');
					size_t length = end - begin;
					curr = curr + (uint32_t)length;
					size_t str_len = 0, loc = 1;
					uint32_t idx = 0;

					enum class _state_t {
						section,
						name,
						addr,
						partition,
					} _state;

					_state = _state_t::section;
					char* p_tmp = new char[length] {};
					memcpy(p_tmp, begin, length);
					//char* tmp_p[50] = { 0, };
					char** pp_tmp = new char* [IO_DAT_ARG_TBL_CNT_MAX] {0, };
					char tmp_addr[IO_NAME_STR_MAX] = { 0, };
					uint8_t cnt = trans::SplitArgs(p_tmp, pp_tmp, ",", 50);
					for (int i = 0; i < cnt; i++)
					{
						char* p_char = pp_tmp[i];
						str_len = strlen(p_char);
						loc = 1;
						while (str_len > loc)
						{
							char data = p_char[loc];

							switch (_state)
							{
							case _state_t::section:
							{
								switch (data)
								{
								case '\"':
								{
									_state = _state_t::name;
								}
								break;
								}
							}
							break;
							case _state_t::name:
							{
								char* begin = p_char + loc;
								char* end = strchr(begin, '\"');
								size_t length = end - begin;
								if (data_type == type_e::in_t)
								{
									m_pIn[m_inCnt % IO_INPUT_MAX] = new dat_t();
									m_pIn[m_inCnt % IO_INPUT_MAX]->type = data_type;
									memcpy(m_pIn[m_inCnt % IO_INPUT_MAX]->name, begin, length % IO_NAME_STR_MAX);
								}
								else
								{
									m_pOut[m_outCnt % IO_INPUT_MAX] = new dat_t();
									m_pOut[m_outCnt % IO_INPUT_MAX]->type = data_type;
									memcpy(m_pOut[m_outCnt % IO_INPUT_MAX]->name, begin, length % IO_NAME_STR_MAX);
								}

								loc += length;
								_state = _state_t::partition;
							}
							break;
							case _state_t::partition:
							{
								if (data == ':')
								{
									_state = _state_t::addr;
									idx = 0;
								}
							}
							break;
							case _state_t::addr:
							{

								switch (data)
								{
								case '0': case '1': case '2': case '3': case '4': case '5':    // 문자가숫자이면
								case '6': case '7': case '8': case '9':
									tmp_addr[idx++] = data;
									break;

								case '}':
								{
									if (data_type == type_e::in_t)
									{
										m_pIn[m_inCnt]->addr = atoi(tmp_addr);
										m_inCnt++;
									}
									else
									{
										m_pOut[m_outCnt]->addr = atoi(tmp_addr);
										m_outCnt++;
									}
									_state = _state_t::section;
									loc = str_len;
								}
								break;
								default:
									break;
								}//switch (data)

							}
							break;
							default:
								break;
							} // switch

							loc++;
						}// while

						p_char++;

					}// for 
					state = state_t::section;
					if (p_tmp) { delete[] p_tmp; p_tmp = nullptr; }
					if (pp_tmp) { delete[] pp_tmp; pp_tmp = nullptr; }
				}
				break;
				default:
					break;
				} //switch
			}
			break;
			default:
				break;
			}//switch

			curr++;

		}//while
	}

	inline void parsingDat(char* p_data, uint32_t size)
	{
		enum class state_t {
			type,
			name,
			addr,
			wait,
		};

		type_e data_type = type_e::in_t;
		uint32_t curr = 0;
		char data;
		state_t state = state_t::type;

		curr++;
		// 문서 크기만큼 반복
		while (curr < size)
		{
			data = p_data[curr];
			switch (state)
			{
			case state_t::type:
			{
				switch (data)
				{
				case 'I':
				case 'i':
				{
					state = state_t::name;
					char* begin = p_data + curr;
					char* end = strchr(begin, ',');
					size_t stringLength = end - begin;
					curr += (uint32_t)stringLength + 1;
					data_type = type_e::in_t;
				}
				break;
				case 'O':
				case 'o':
				{
					state = state_t::name;
					char* begin = p_data + curr;
					char* end = strchr(begin, ',');
					size_t stringLength = end - begin;
					curr += (uint32_t)stringLength + 1;
					data_type = type_e::out_t;
				}
				break;
				case '/':
				{
					if (p_data[curr + 1] == '/') // 주석 처리
					{
						char* begin = p_data + curr + 1;
						char* end = strchr(begin, '\n');
						size_t stringLength = end - begin;
						curr = curr + (uint32_t)stringLength + 1;
					}
				}
				break;
				case '\n':case '\t':case ' ': //
				default:
					break;
				}//switch

			}
			break;
			case state_t::name:
			{
				switch (data)
				{
				case '\n':case '\t':case ' ': //
					break;
				default:
				{

					char* begin = p_data + curr;
					char* end = strchr(begin, ',');
					size_t stringLength = end - begin;
					if (data_type == type_e::in_t)
					{
						m_pIn[m_inCnt % IO_INPUT_MAX] = new dat_t();

						memcpy(m_pIn[m_inCnt]->name, begin, stringLength % IO_NAME_STR_MAX);
						m_pIn[m_inCnt]->type = type_e::in_t;
					}
					else
					{
						m_pOut[m_outCnt % IO_INPUT_MAX] = new dat_t();

						memcpy(m_pOut[m_outCnt]->name, begin, stringLength % IO_NAME_STR_MAX);
						m_pOut[m_outCnt]->type = type_e::out_t;
					}

					curr = curr + (uint32_t)stringLength + 1;
					state = state_t::addr;
				}
				break;
				}//switch        
			}
			break;
			case state_t::wait:
				break;
			case state_t::addr:
			{

				char* begin = p_data + curr;
				char* end = strchr(begin, '\n');
				size_t length = end - begin;
				curr = curr + (uint32_t)length;
				char tmp_addr[IO_NAME_STR_MAX] = { 0, };
				uint8_t idx = 0, loc = 0;
				while (length > 0)
				{
					switch (*(begin + idx))
					{
					case '0': case '1': case '2': case '3': case '4': case '5':    // 문자가숫자이면
					case '6': case '7': case '8': case '9':
						tmp_addr[loc++] = *(begin + idx);
						break;
					case '/':// 주석 처리
					{
						length = 1;
					}
					break;
					default:
						break;
					}//switch
					idx++;
					length--;
				}
				if (data_type == type_e::in_t)
				{
					m_pIn[m_inCnt]->addr = atoi(tmp_addr);
					m_inCnt++;
				}
				else
				{
					m_pOut[m_outCnt]->addr = atoi(tmp_addr);
					m_outCnt++;
				}

				state = state_t::type;
			}

			break;
			}//switch

			curr++;

		}//while
	}

#endif

public:
	inline uint32_t LoadData() {
		int load_cnt = util::parser::json::LoadData();
		dat_t data;
		util::parser::json_t jdata;
		for (int i = 0; i < load_cnt; i++)
		{
			jdata = util::parser::json::ReadData(i);
			if (util::IsEqualStr("input", jdata.key) || util::IsEqualStr("INPUT", jdata.key))
			{
				data.type = GDataTable_IO::type_e::in_t;
				memcpy(data.name, jdata.key_value->key, IO_NAME_STR_MAX);
				data.addr = jdata.key_value->number;
				m_vtIn.push_back(data);
			}
			else
			{
				data.type = GDataTable_IO::type_e::out_t;
				memcpy(data.name, jdata.key_value->key, IO_NAME_STR_MAX);
				data.addr = jdata.key_value->number;
				m_vtOut.push_back(data);
			}
		}
		return load_cnt;
	}

	inline uint32_t GetInDataCnt() const {
		return (uint32_t)m_vtIn.size();
	}

	inline GDataTable_IO::dat_t GetInData(uint32_t addr) {
		GDataTable_IO::dat_t ret;
		if (addr < (uint32_t)m_vtIn.size())
		{
			ret = m_vtIn[addr];
		}
		return ret;
	}

	inline uint32_t GetOutDataCnt() const {
		return (uint32_t)m_vtOut.size();
	}

	inline GDataTable_IO::dat_t GetOutData(uint32_t addr) {
		GDataTable_IO::dat_t ret;
		if (addr < (uint32_t)m_vtOut.size())
		{
			ret = m_vtOut[addr];
		}
		return ret;
	}


#if 0
	inline uint32_t LoadData() {


		FILE* p_file = nullptr;
		/* Open for read (will fail if file "data" does not exist) */
		fopen_s(&p_file, m_fileInfo, "rb");
		if (p_file == nullptr) {
			TRACE("The file 'data' was not opened! \n\r");
			return -1;
		}
		else
			TRACE("The file 'data' was opened \n\r");

		// 파일 크기 구하기
		fseek(p_file, 0, SEEK_END);
		size_t size = ftell(p_file);
		fseek(p_file, 0, SEEK_SET);

		// 파일 크기 + NULL 공간만큼 메모리를 할당하고 0으로 초기화
		char* file_binary = nullptr;
		file_binary = new char[size + 1] { 0 };
		if (file_binary != nullptr)
		{
			// 파일 내용 읽기
			if (fread(file_binary, size, 1, p_file) > 0) {
				//파싱하여 
#ifndef USE_IO_DATA_FORMAT_DAT
				parsingJson(file_binary, (uint32_t)size);
#else
				parsingDat(file_binary, (uint32_t)size);
#endif
			}
			else {
				size = 0;
			}
		}
		delete[] file_binary;
		return (uint32_t)size;
	}

#endif

};



#pragma endregion



namespace AP_SYS
{

	using namespace AP_OBJ;
	using namespace AP_DATA;




	class MSystemIO :public IIO
	{
		/****************************************************
		 *	data
		 ****************************************************/
	public:

		/// <summary>
		/// 사용될 io device를 열거한다
		/// </summary>
		enum class device_e
		{
			fastech_netio_i16o16,
			fastech_netio_out_16,
			fastech_netio_in_16,
			_max

		};
		struct obj_t
		{
			IIO* p_Io{};// 
			uint32_t index{};
		};



		struct device_t
		{
			device_e deviceType{};
			uint32_t startAddrIn{};
			uint32_t lenghtIn{};
			uint32_t startAddrOut{};
			uint32_t lenghtOut{};
			AP_OBJ::NET_OBJ net_id{};
			IIO* objIo{};

			void SetData(device_t& data)
			{
				deviceType = data.deviceType;
				startAddrIn = data.startAddrIn;
				lenghtIn = data.lenghtIn;
				startAddrOut = data.startAddrOut;
				lenghtOut = data.lenghtOut;
				objIo = data.objIo;
				net_id = data.net_id;
			}

			void init() {
				deviceType = device_e::_max;
				startAddrIn = 0;
				lenghtIn = 0;
				startAddrOut = 0;
				lenghtOut = 0;
				objIo = 0;
				net_id = AP_OBJ::NET_OBJ::_max;
			}

		};


		struct cfg_t
		{
			IComm* p_Comm{};//io 객체 생성에 필요한 생성된 통신 객체
			IIO* p_Io{};// 생성된 io 객체의 주소를 넘겨 받을 때 (ex:파스텍 모터 드라이브에서 io를 사용하게 될 때)
			AP_DATA::GConfig_dat* p_cfgDat{};
			device_t data;

			void init() {
				p_Comm = nullptr;
				p_Io = nullptr;
				p_cfgDat = nullptr;
				data.init();
			}

		};


	private:
		std::vector< IComm*> m_pComm;
		std::vector< IIO*> m_pIo;
		std::mutex m_mutex;

		std::vector< AP_SYS::MSystemIO::device_t> m_vDevice;
		//vector< GFastech_IO*> m_fastNetIO;
		std::vector< GFastech::GFastech_motorE_lib*> m_fastNetIO;

		bool m_TrdLife;
		bool m_IsOpened;
		DWORD m_TrdId;

		// 포트를 감시할 함수, 스레드의 핸들을 보관
		HANDLE m_hThread;


		/****************************************************
		 *	Constructor
		 ****************************************************/
	public:
		MSystemIO(common::Attribute_t common_data);
		virtual ~MSystemIO();


		/****************************************************
		 *	func
		 ****************************************************/
	public:
		errno_t StartThread();
		int AddDevice(MSystemIO::cfg_t& cfg);
		bool IsOn(uint32_t addr);
		bool IsOff(uint32_t addr);
		errno_t OutputOn(uint32_t addr);
		errno_t OutputOff(uint32_t addr);
		errno_t OutputToggle(uint32_t addr);
		uint32_t GetResponseTime(MSystemIO::device_e device) const;


	private:
		void threadStop(void);
		void threadRun(void);
		void threadJob(void);
		static UINT threadFunc(LPVOID pParam);


		bool getInObject(uint32_t addr, obj_t& obj);
		bool getOutObject(uint32_t addr, obj_t& obj);
	};


}