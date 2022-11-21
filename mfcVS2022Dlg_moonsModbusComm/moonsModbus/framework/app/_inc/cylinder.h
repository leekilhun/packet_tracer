#pragma once


class MCylinder :public Icommon
{
public:
	enum class location_sensor_e
	{
		top,
		bottom,
		_max
	};

	enum class location_sol_e
	{
		sol_top,
		sol_bottom,
		_max
	};

	enum class time_e { move, settle };
	enum class type_e {
		unknwn,
		up_down,
		left_right,
		forward_backward,
		open_close,
		turn_return,
		lock_unlock,
		up_mid_down,
		left_mid_right,
		front_mid_back,
	};
	enum class sol_e {
		unkwn, one, reverse_one, two
	};


	struct cfg_t {
		uint8_t cyl_id{};
		std::array< uint32_t, static_cast<int>(location_sol_e::_max)> addr_sol_io{};
		std::array< uint32_t, static_cast<int>(location_sensor_e::_max)> addr_sensor_io{};
		type_e  cyl_type{};
		sol_e sol_type{};
		AP_DATA::GCylinder_dat::dat_t cyl_data{};
		std::shared_ptr<IIO> pIo{};

		inline void operator = (const cfg_t* p_cfg) {
			this->cyl_id = p_cfg->cyl_id;
			this->addr_sol_io = p_cfg->addr_sol_io;
			this->addr_sensor_io = p_cfg->addr_sensor_io;
			this->cyl_type = p_cfg->cyl_type;
			this->sol_type = p_cfg->sol_type;
			this->cyl_data = p_cfg->cyl_data;
			this->pIo = p_cfg->pIo;

		}
	};

private:
	cfg_t m_cfg;

public:
	MCylinder(common::Attribute_t& common_data, cfg_t& cfg)
		: Icommon(common_data), m_cfg(cfg)
	{
	}

	virtual ~MCylinder() = default;

private:


public:

	inline void SetConfigData(cfg_t& cfg) {
		m_cfg = cfg;
	}
	/*----------- 실린더 상태 확인  -----------------------*/
	inline bool IsUp() {
		std::array< bool, static_cast<int>(location_sensor_e::_max )> state{ false , };

		// UP Sensor가 On인지 확인
		constexpr int up_sensor = static_cast<int>(location_sensor_e::top);
		constexpr int dw_sensor = static_cast<int>(location_sensor_e::bottom);

		if (m_cfg.addr_sensor_io[up_sensor])
			state[up_sensor] = m_cfg.pIo->IsOn(m_cfg.addr_sensor_io[up_sensor]);

		// Down Sensor가 Off인지 확인
		if (m_cfg.addr_sensor_io[dw_sensor])
			state[dw_sensor] = m_cfg.pIo->IsOff(m_cfg.addr_sensor_io[dw_sensor]);

		//
		/*if (!state[up_sensor] || !state[dw_sensor])
			return false;*/

		return (state[up_sensor]&&state[dw_sensor]);
	}

	inline bool IsDown() {
		std::array< bool, static_cast<int>(location_sensor_e::_max)> state{ false , };

		// UP Sensor가 Off인지 확인
		constexpr int up_sensor = static_cast<int>(location_sensor_e::top);
		constexpr int dw_sensor = static_cast<int>(location_sensor_e::bottom);

		if (m_cfg.addr_sensor_io[up_sensor])
			state[up_sensor] = m_cfg.pIo->IsOff(m_cfg.addr_sensor_io[up_sensor]);

		// Down Sensor가 On인지 확인
		if (m_cfg.addr_sensor_io[dw_sensor])
			state[dw_sensor] = m_cfg.pIo->IsOn(m_cfg.addr_sensor_io[dw_sensor]);

		//
		/*if (!state[up_sensor] || !state[dw_sensor])
			return false;*/	

		return (state[up_sensor] && state[dw_sensor]);
	}

	inline bool IsLeft() {
		return IsUp();
	}

	inline bool IsRight() {
		return IsDown();
	}

	inline bool IsForward() {
		return IsUp();
	}

	inline bool IsBackward() {
		return IsDown();
	}

	inline bool IsOpen() {
		return IsUp();
	}

	inline bool IsClose() {
		return IsDown();
	}

	inline bool IsLock() {
		return IsUp();
	}

	inline bool IsUnlock() {
		return IsDown();
	}

	inline bool IsTurn() {
		return IsUp();
	}

	inline bool IsReturn() {
		return IsDown();
	}
	/*----------- 실린더 이동   -----------------------*/

	inline errno_t Up(bool skip_sensor = false) {
		constexpr int up_sol = static_cast<int>(location_sensor_e::top);
		constexpr int dw_sol = static_cast<int>(location_sensor_e::bottom);
		switch (m_cfg.sol_type) {
		case sol_e::one:
			m_cfg.pIo->OutputOn(m_cfg.addr_sol_io[up_sol]);
			break;
		case sol_e::reverse_one:
			m_cfg.pIo->OutputOff(m_cfg.addr_sol_io[up_sol]);
			break;
		case sol_e::two:
			m_cfg.pIo->OutputOn(m_cfg.addr_sol_io[up_sol]);
			m_cfg.pIo->OutputOff(m_cfg.addr_sol_io[dw_sol]);
			break;
		default:
			break;
		}

		if (skip_sensor) {
			return ERROR_SUCCESS;
		}
		return Wait4UpCplt();
	}

	inline errno_t Down(bool skip_sensor = false) {
		constexpr int up_sol = static_cast<int>(location_sensor_e::top);
		constexpr int dw_sol = static_cast<int>(location_sensor_e::bottom);
		switch (m_cfg.sol_type) {
		case sol_e::one:
			m_cfg.pIo->OutputOn(m_cfg.addr_sol_io[dw_sol]);
			break;
		case sol_e::reverse_one:
			m_cfg.pIo->OutputOff(m_cfg.addr_sol_io[dw_sol]);
			break;
		case sol_e::two:
			m_cfg.pIo->OutputOff(m_cfg.addr_sol_io[up_sol]);
			m_cfg.pIo->OutputOn(m_cfg.addr_sol_io[dw_sol]);
			break;
		default:
			break;
		}

		if (skip_sensor) {
			return ERROR_SUCCESS;
		}
		return Wait4UpCplt();
	}

	inline int Left(bool skip_sensor = false) {
		return Up(skip_sensor);
	}

	inline int Right(bool skip_sensor = false) {
		return Down(skip_sensor);
	}

	inline int Forward(bool skip_sensor = false) {
		return Up(skip_sensor);
	}

	inline int Backward(bool skip_sensor = false) {
		return Down(skip_sensor);
	}

	inline int Open(bool skip_sensor = false) {
		return Up(skip_sensor);
	}

	inline int Close(bool skip_sensor = false) {
		return Down(skip_sensor);
	}

	inline int Lock(bool skip_sensor = false) {
		return Up(skip_sensor);
	}

	inline int Unlock(bool skip_sensor = false) {
		return Down(skip_sensor);
	}

	inline int Return(bool skip_sensor = false) {
		return Up(skip_sensor);
	}

	inline int Turn(bool skip_sensor = false) {
		return Down(skip_sensor);
	}

	/*----------- 실린더 이동 시작  -----------------------*/
	inline int Start() {
		return Up(true);
	}

	/*---------- 실린더 동작완료때까지 Sleep   --------------*/
	inline errno_t Wait4UpCplt() {
		errno_t ret = -1;
		uint32_t pre_ms = util::millis();
		uint32_t timeout_ms = (uint32_t)m_cfg.cyl_data.MovingTime * 1000;
		while ((util::millis() - pre_ms) < timeout_ms) {
			if (IsUp()) {
				ret = ERROR_SUCCESS;
				break;
			}
		}
		return ret;
	}

	inline errno_t Wait4DownCplt() {
		errno_t ret = -1;
		uint32_t pre_ms = util::millis();
		uint32_t timeout_ms = (uint32_t)m_cfg.cyl_data.MovingTime * 1000;
		while ((util::millis() - pre_ms) < timeout_ms) {
			if (IsDown()) {
				ret = ERROR_SUCCESS;
				break;
			}
		}
		return ret;
	}

	/*------------------------ Common Interface -----------------------*/

	inline errno_t SetData(AP_DATA::GCylinder_dat::dat_t &p_data) {
		m_cfg.cyl_data = p_data;
		return ERROR_SUCCESS;
	}

	inline errno_t GetData(AP_DATA::GCylinder_dat::dat_t &p_data) {
		p_data = m_cfg.cyl_data;
		return ERROR_SUCCESS;
	}


	inline MCylinder::cfg_t MCylinder::GetConfigData() const
	{
		return m_cfg;
	}
};



