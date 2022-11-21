#pragma once

class MVacuum :public Icommon
{
public:
	enum class location_sol_e
	{
		sol_suction,
		sol_blower,
		_max
	};
	enum class time_e { on, settle };
	enum class type_e {
		unkwn,
		suction,
		suction_blow,
		_max
	};
	struct cfg_t
	{
		uint8_t vac_id;
		std::array< uint32_t, static_cast<int>(location_sol_e::_max)> addr_sol_io{};
		uint32_t addr_on_io;
		type_e  vac_type{};
		AP_DATA::GVacuum_dat::dat_t vac_data{};
		std::shared_ptr<IIO> pIo{};

		inline void operator = (const cfg_t* p_cfg) {
			this->vac_id = p_cfg->vac_id;
			this->addr_sol_io = p_cfg->addr_sol_io;
			this->addr_on_io = p_cfg->addr_on_io;
			this->vac_type = p_cfg->vac_type;
			this->vac_data = p_cfg->vac_data;
			this->pIo = p_cfg->pIo;
		}

	};
private:
	cfg_t m_cfg;

public:
	MVacuum(common::Attribute_t& common_data, cfg_t& cfg)
		: Icommon(common_data), m_cfg(cfg)
	{
	}

	virtual ~MVacuum() = default;

private:

public:

	inline void SetConfigData(cfg_t& p_cfg) {
		m_cfg = p_cfg;
	}

	/*----------- Vacuum 상태 확인  -----------------------*/
	inline bool IsOn() {
		bool ret = false;
		if (m_cfg.addr_on_io) {
			ret = m_cfg.pIo->IsOn(m_cfg.addr_on_io);
		}
		return ret;
	}

	inline bool IsOff() {
		bool ret = false;
		if (m_cfg.addr_on_io) {
			ret = m_cfg.pIo->IsOff(m_cfg.addr_on_io);
		}
		return ret;
	}

	/*----------- Vacuum 동작 시작  -----------------------*/
	inline errno_t BlowOn() {
		constexpr int sol_blower = static_cast<int>(location_sol_e::sol_blower);
		return m_cfg.pIo->OutputOn(m_cfg.addr_sol_io[sol_blower]);
	}

	inline errno_t BlowOff() {
		constexpr int sol_blower = static_cast<int>(location_sol_e::sol_blower);
		constexpr int sol_suction = static_cast<int>(location_sol_e::sol_suction);
		m_cfg.pIo->OutputOn(m_cfg.addr_sol_io[sol_blower]);
		Sleep(50);		
		return m_cfg.pIo->OutputOff(m_cfg.addr_sol_io[sol_blower]);
	}

	/*----------- Vacuum 동작  -----------------------*/
	inline errno_t  On(bool skip_sensor = false) {
		constexpr int sol_blower = static_cast<int>(location_sol_e::sol_blower);
		constexpr int sol_suction = static_cast<int>(location_sol_e::sol_suction);
		switch (m_cfg.vac_type) {
		case type_e::suction:
			m_cfg.pIo->OutputOn(m_cfg.addr_sol_io[sol_suction]);
			break;
		case type_e::suction_blow:
			m_cfg.pIo->OutputOn(m_cfg.addr_sol_io[sol_suction]);
			m_cfg.pIo->OutputOff(m_cfg.addr_sol_io[sol_blower]);
			break;
		default:
			break;
		}

		if (skip_sensor) {
			return ERROR_SUCCESS;
		}
		return Wait4OnCplt();
	}

	inline errno_t Off(bool skip_sensor = false) {
		constexpr int sol_blower = static_cast<int>(location_sol_e::sol_blower);
		constexpr int sol_suction = static_cast<int>(location_sol_e::sol_suction);
		switch (m_cfg.vac_type) {
		case type_e::suction:
			m_cfg.pIo->OutputOff(m_cfg.addr_sol_io[sol_suction]);
			break;
		case type_e::suction_blow:
			m_cfg.pIo->OutputOff(m_cfg.addr_sol_io[sol_suction]);
			BlowOff();
			break;
		default:
			break;
		}

		if (skip_sensor) {
			return ERROR_SUCCESS;
		}
		return Wait4OffCplt();
	}


	/*----------- Vacuum 동작완료시까지 Sleep  -----------------------*/
	inline errno_t Wait4OnCplt() {
		int ret = -1;
		uint32_t pre_ms = util::millis();
		uint32_t timeout_ms = (uint32_t)m_cfg.vac_data.OnSettlingTime * 1000;
		while ((util::millis() - pre_ms) < timeout_ms){
			if (IsOn()) {
				ret = ERROR_SUCCESS;
				break;
			}
		}
		return ret;
	}

	inline errno_t Wait4OffCplt() {
		int ret = -1;
		uint32_t pre_ms = util::millis();
		uint32_t timeout_ms = (uint32_t)m_cfg.vac_data.OffSettlingTime * 1000;
		while ((util::millis() - pre_ms) < timeout_ms) {
			if (IsOff()) {
				ret = ERROR_SUCCESS;
				break;
			}
		}
		return ret;
	}
	/*------------------------ Common Interface -----------------------*/
	inline errno_t SetData(AP_DATA::GVacuum_dat::dat_t& p_data) {
		m_cfg.vac_data = p_data;
		return ERROR_SUCCESS;
	}

	inline errno_t GetData(AP_DATA::GVacuum_dat::dat_t& p_data) {
		p_data = m_cfg.vac_data;
		return ERROR_SUCCESS;
	}



};
