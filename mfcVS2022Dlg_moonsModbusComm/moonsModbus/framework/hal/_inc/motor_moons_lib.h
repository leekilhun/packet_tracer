#pragma once



#ifdef USE_HW_MOTOR_MOONS_LIB

#include "SCLLibHelper.h"

using namespace LIB_MOONS;

namespace GMoons
{


#define MOONS_LIB_LOG_WRITE(fmt, ...)		m_log.syslog_Write(syslog::level::info, m_Attribute.GetObjectID(), m_Attribute.GetLogFileName(), (__FILE__), __LOGFUNC__, __LINE__, fmt, ##__VA_ARGS__)
#define MOONS_LIB_ERR_WRITE(fmt, ...)		m_log.syslog_Write(syslog::level::err, m_Attribute.GetObjectID(), m_Attribute.GetLogFileName(), (__FILE__), __LOGFUNC__, __LINE__, fmt, ##__VA_ARGS__)


	class GMoons_motor_lib : public IMotionLib
	{
		/****************************************************
		 *	data
		 ****************************************************/
		SCLLibHelper* m_moonsHelper;
		//SCLLibHelper*  m_moonsHelper;

	public:
		struct home_param_t {
			uint8_t home_x_no{};
			int find_home_dir{}; // 1 CW, -1 CCW
			double find_home_vel{};//rps
			char home_x_level{}; //'L' Low, 'H' High,

			home_param_t() {
				home_x_no = 5; // home sensor x5
				home_x_level = 'L';
				find_home_dir = 1;
				find_home_vel = 2.0;//rps
			}

			inline void setter(uint8_t x_no, int find_dir, double find_vel, char x_level) {
				home_x_no = x_no;
				home_x_level = x_level;
				find_home_dir = find_dir;
				find_home_vel = find_vel;//rps
			}

			inline void operator = (const home_param_t* p_cfg) {
				this->home_x_no = p_cfg->home_x_no;
				this->home_x_level = p_cfg->home_x_level;
				this->find_home_dir = p_cfg->find_home_dir;
				this->find_home_vel = p_cfg->find_home_vel;
			}

		};

		struct cfg_t {
			int			port_no{};					// 시리얼포트
			int			base_node_id{};
			int			baudrate{};					// Baudrate
			int			comm_delay_ms{};			// Delay Time 초기값 - 35ms
			/*
			double jog_speed{}; //rps
			double jog_accel{}; //rps/s
			double jog_decel{};
			double move_velocity{}; //rps
			double move_accel{};//rps/s
			double move_decel{};//rps/s
			uint8_t home_x_no{};
			int find_home_dir{}; // 1 CW, -1 CCW
			double find_home_vel{};//rps
			char home_x_level{}; //'L' Low, 'H' High,
			*/
			inline void operator = (const cfg_t* p_cfg) {
				this->port_no = p_cfg->port_no;
				this->base_node_id = p_cfg->base_node_id;
				this->baudrate = p_cfg->baudrate;
				this->comm_delay_ms = p_cfg->comm_delay_ms;

				/*
				this->jog_speed = p_cfg->jog_speed;
				this->jog_accel = p_cfg->jog_accel;
				this->jog_decel = p_cfg->jog_decel;
				this->move_velocity = p_cfg->move_velocity;
				this->move_accel = p_cfg->move_accel;
				this->move_decel = p_cfg->move_decel;
				this->home_x_no = p_cfg->home_x_no;
				this->find_home_dir = p_cfg->find_home_dir;
				this->find_home_vel = p_cfg->find_home_vel;
				this->home_x_level = p_cfg->home_x_level;
				*/
			}

			cfg_t()
			{
				port_no = 0;
				base_node_id = 0;
				baudrate = 9600;
				comm_delay_ms = 35;
				/*
				jog_speed = 2.0; //rps
				jog_accel = 100.0; //rps/s
				jog_decel = 100.0;
				move_velocity = 10.0; //rps
				move_accel = 100.0;//rps/s
				move_decel = 100.0;//rps/s
				home_x_no = 5; // home sensor x5
				home_x_level = 'L';
				find_home_dir = 1;
				find_home_vel = 2.0;//rps
				*/
			}


		};

	private:
		cfg_t m_cfg;
		BYTE  m_baseNodeId;
		home_param_t m_homeParam;
		std::mutex m_mutex;
		/****************************************************
		 *
		 ****************************************************/



		 /****************************************************
			*	Constructor
		  ****************************************************/
	public:
		GMoons_motor_lib(common::Attribute_t& common_data, cfg_t& cfg)
			:Icommon(common_data), m_cfg(cfg), m_baseNodeId(cfg.base_node_id), m_homeParam(), m_moonsHelper(nullptr)
		{
			m_moonsHelper = new SCLLibHelper();// 생성방법 1

		}

		virtual ~GMoons_motor_lib() {
			if (m_moonsHelper->IsOpen())
			{
				m_moonsHelper->Close();
			}

			if (m_moonsHelper)
			{
				delete m_moonsHelper;
			}
			m_moonsHelper = nullptr;

		}


		/****************************************************
		 *	func
		 ****************************************************/
	public:


		inline BOOL searchHome(BYTE axis_id) {
			return  m_moonsHelper->WriteSeekHome(axis_id, m_homeParam.home_x_no, m_homeParam.home_x_level);
		}

		inline BOOL SetHomeParam(BYTE axis_id, home_param_t& param) {
			BOOL ret = TRUE;
			BYTE define_limit_type = param.home_x_level == 'L' ? 1 : 2;
			ret &= m_moonsHelper->WriteDefineLimits((BYTE)axis_id, define_limit_type);
			ret &= m_moonsHelper->WriteDistanceOrPosition((BYTE)axis_id, param.find_home_dir);
			ret &= m_moonsHelper->WriteVelocity((BYTE)axis_id, param.find_home_vel);
			m_homeParam = param;
		}



		inline BOOL SetMototionParam(BYTE axis_id, IAxis::motion_param_t& param) {
			BOOL ret = TRUE;
			ret &= m_moonsHelper->WriteVelocity((BYTE)axis_id, param.move_velocity);
			ret &= m_moonsHelper->WriteAccelerationRate((BYTE)axis_id, param.move_accel);
			ret &= m_moonsHelper->WriteDecelerationRate((BYTE)axis_id, param.move_decel);
			ret &= m_moonsHelper->WriteJogSpeed((BYTE)axis_id, param.jog_speed);
			ret &= m_moonsHelper->WriteJogAcceleration((BYTE)axis_id, param.jog_accel);
			ret &= m_moonsHelper->WriteJogDeceleration((BYTE)axis_id, param.jog_decel);
			return ret;
		}


		/* IMotionLib  함수*/

		inline errno_t Initialize() {
			errno_t ret = ERROR_SUCCESS;

			if (m_moonsHelper == nullptr)
				return -1;

			if (m_moonsHelper->IsOpen())
			{
				m_moonsHelper->SetCommParam((BYTE)m_baseNodeId, FALSE);
			}
			else
			{
				if (m_moonsHelper->Open(m_cfg.port_no, m_cfg.baudrate))
				{
					m_moonsHelper->SetCommParam((BYTE)m_baseNodeId, FALSE);
				}
				else
				{
					ERR_PRINT("m_moonsHelper->Open  fial!\n", );
					ret = -1;
				}
			}



			return  ret;
		}




		inline errno_t WaitDone(short axis_id, unsigned short timeout) {
			errno_t ret = -1;
			std::lock_guard <std::mutex>lock(m_mutex);
			//m_mutex.lock();
			uint32_t pre_ms = util::millis();
			while (util::millis() - pre_ms < timeout)
			{
				if (m_moonsHelper->IsInPosition((BYTE)axis_id))
				{
					ret = ERROR_SUCCESS;
					break;
				}
			}
			//m_mutex.unlock();
			return ret;
		}


		inline errno_t MotorOnOff(short axis_id, bool on_off) {
			errno_t ret = -1;
			BOOL result = FALSE;
			if (m_moonsHelper->IsOpen() == FALSE)
				return ret;
			std::lock_guard <std::mutex>lock(m_mutex);
			//m_mutex.lock();
			if (on_off)
			{
				result = m_moonsHelper->WriteMotorEnable((BYTE)axis_id);
			}
			else
			{
				result = m_moonsHelper->WriteMotorDisable((BYTE)axis_id);
			}
			//m_mutex.unlock();
			return result == TRUE ? ERROR_SUCCESS : -1;
		}



		inline bool IsMotorOn(short axis_id) {
			bool ret = false;

			return m_moonsHelper->IsMotorEnabled((BYTE)axis_id);
		}


		inline bool IsAxisDone(short axis_id) {
			return (bool)m_moonsHelper->IsInPosition((BYTE)axis_id);
		}

		inline errno_t GetAxisStatus(short axis_id, DWORD& p_state) {
			errno_t ret = -1;
			if (m_moonsHelper->IsOpen() == FALSE)
				return ret;
			std::lock_guard <std::mutex>lock(m_mutex);
			//m_mutex.lock();
			if (m_moonsHelper->ReadStatusCode((BYTE)axis_id, (int*)&p_state))
			{
				ret = ERROR_SUCCESS;
			}
			//m_mutex.unlock();
			return ret;
		}

		inline errno_t ClearPosition(short axis_id) {
			errno_t ret = -1;
			if (m_moonsHelper->IsOpen() == FALSE)
				return ret;
			std::lock_guard <std::mutex>lock(m_mutex);
			//m_mutex.lock();
			if (m_moonsHelper->WriteEncoderPosition((BYTE)axis_id, 0)) // EP - Encoder Posiont Zero Set
			{
				ret = ERROR_SUCCESS;
			}
			if (m_moonsHelper->WriteSetPosition((BYTE)axis_id, 0)) // SP - Command Position  Zero Set 
			{
				ret = ERROR_SUCCESS;
			}
			//m_mutex.unlock();
			return ret;
		}

		inline errno_t AlarmReset(short axis_id) {
			errno_t ret = -1;
			if (m_moonsHelper->IsOpen() == FALSE)
				return ret;
			std::lock_guard <std::mutex>lock(m_mutex);
			//m_mutex.lock();
			if (m_moonsHelper->WriteAlarmReset((BYTE)axis_id, TRUE))
			{
				//m_moonsHelper->WriteRestart((BYTE)axis_id);
				ret = ERROR_SUCCESS;
			}
			//m_mutex.unlock();
			return ret;
		}

		inline errno_t GetAlarmType(short axis_id, DWORD& p_state) {
			errno_t ret = -1;
			if (m_moonsHelper->IsOpen() == FALSE)
				return ret;
			std::lock_guard <std::mutex>lock(m_mutex);
			//m_mutex.lock();
			if (m_moonsHelper->ReadAlarmCode((BYTE)axis_id, (int*)&p_state))
			{
				ret = ERROR_SUCCESS;
			}
			//m_mutex.unlock();
			return ret;
		}

		inline errno_t GetCommandPosition(short axis_id, long& p_position) {
			errno_t ret = -1;
			if (m_moonsHelper->IsOpen() == FALSE)
				return ret;
			std::lock_guard <std::mutex>lock(m_mutex);
			//m_mutex.lock();
			//if (m_moonsHelper->ReadDistanceOrPosition((BYTE)axis_id, (int*) & p_position)) // Get command Distance
			if (m_moonsHelper->ReadImmediatePosition((BYTE)axis_id, (int*)&p_position)) // Get command position
			{
				ret = ERROR_SUCCESS;
			}
			//m_mutex.unlock();
			return ret;
		}

		inline errno_t GetActualPosition(short axis_id, long& p_position) {
			errno_t ret = -1;
			if (m_moonsHelper->IsOpen() == FALSE)
				return ret;
			std::lock_guard <std::mutex>lock(m_mutex);
			//m_mutex.lock();
			if (m_moonsHelper->ReadImmediateEncoder((BYTE)axis_id, (int*)&p_position))
			{
				ret = ERROR_SUCCESS;
			}
			//m_mutex.unlock();
			return ret;
		}


		inline errno_t JogMove(short axis_id, DWORD velocity, bool is_cw) {
			errno_t ret = -1;
			if (m_moonsHelper->IsOpen() == FALSE)
				return ret;
			std::lock_guard <std::mutex>lock(m_mutex);
			//m_mutex.lock();
			constexpr int DIR_CW = 1;
			constexpr int DIR_CCW = -1;
			double speed = 10.0;
			int commd_distance = (is_cw) ? DIR_CW : DIR_CCW;

			/*
			if (m_moonsHelper->ReadDistanceOrPosition((BYTE)axis_id, &commd_distance) != TRUE) // Get command Distance
			{
				return ret;
			}
			*/

			// Set command Distance , + CW direction , - CCW direction 
			if (m_moonsHelper->WriteDistanceOrPosition((BYTE)axis_id, commd_distance) != TRUE)
			{
				goto func_out;
			}
			if (m_moonsHelper->WriteJogSpeed((BYTE)axis_id, speed) != TRUE)
			{
				goto func_out;
			}
			double jog_accel = 0.0;
			double jog_decel = 0.0;
			if (m_moonsHelper->ReadJogAcceleration((BYTE)axis_id, &jog_accel) != TRUE)
			{
				goto func_out;
			}
			if (m_moonsHelper->ReadJogDeceleration((BYTE)axis_id, &jog_decel) != TRUE)
			{
				goto func_out;
			}
			if (m_moonsHelper->WriteJogAcceleration((BYTE)axis_id, jog_accel) != TRUE)
			{
				goto func_out;
			}
			if (m_moonsHelper->WriteJogDeceleration((BYTE)axis_id, jog_decel) != TRUE)
			{
				goto func_out;
			}
			if (m_moonsHelper->WriteCommenceJogging((BYTE)axis_id) != TRUE)
			{
				goto func_out;
			}

			ret = ERROR_SUCCESS;
		func_out:

			//m_mutex.unlock();
			return ret;
		}

		inline errno_t JogStop(short axis_id) {
			errno_t ret = -1;
			if (m_moonsHelper->IsOpen() == FALSE)
				return ret;
			std::lock_guard <std::mutex>lock(m_mutex);
			//m_mutex.lock();
			if (m_moonsHelper->WriteStopJogging((BYTE)axis_id))
			{
				ret = ERROR_SUCCESS;
			}
			//m_mutex.unlock();
			return ret;
		}


		inline errno_t MoveStop(short axis_id) {
			errno_t ret = -1;
			if (m_moonsHelper->IsOpen() == FALSE)
				return ret;
			std::lock_guard <std::mutex>lock(m_mutex);
			//m_mutex.lock();
			if (m_moonsHelper->WriteStop((BYTE)axis_id))
			{
				ret = ERROR_SUCCESS;
			}
			//m_mutex.unlock();
			return ret;
		}

		inline errno_t Move(short axis_id, long position, DWORD velocity, bool wait_done = false) {
			errno_t ret = -1;
			if (m_moonsHelper->IsOpen() == FALSE)
				return ret;
			std::lock_guard <std::mutex>lock(m_mutex);
			//m_mutex.lock();
			if (m_moonsHelper->WriteDistanceOrPosition((BYTE)axis_id, (int)position) != TRUE)
			{
				goto func_out;
			}
			if (m_moonsHelper->WriteVelocity((BYTE)axis_id, (double)velocity) != TRUE)
			{
				goto func_out;
			}

			double move_accel = 0.0;
			double move_decel = 0.0;
			if (m_moonsHelper->ReadAccelerationRate((BYTE)axis_id, &move_accel) != TRUE)
			{
				goto func_out;
			}
			if (m_moonsHelper->ReadDecelerationRate((BYTE)axis_id, &move_decel) != TRUE)
			{
				goto func_out;
			}

			if (m_moonsHelper->WriteAccelerationRate((BYTE)axis_id, move_accel) != TRUE)
			{
				goto func_out;
			}
			if (m_moonsHelper->WriteDecelerationRate((BYTE)axis_id, move_decel) != TRUE)
			{
				goto func_out;
			}

			if (m_moonsHelper->WriteFeedtoPosition((BYTE)axis_id) != TRUE)
			{
				goto func_out;
			}

			ret = ERROR_SUCCESS;

		func_out:

			//m_mutex.unlock();
			return ret;
		}

		inline errno_t OriginAxis(short axis_id) {
			errno_t ret = -1;
			if (m_moonsHelper->IsOpen() == FALSE)
				return ret;
			std::lock_guard <std::mutex>lock(m_mutex);
			//m_mutex.lock();
			double  move_accel = 0.0, move_decel = 0.0;
			/*
			if (m_moonsHelper->ReadDistanceOrPosition((BYTE)axis_id, &find_home_dir) != TRUE)
			{
				return ret;
			}

			if (m_moonsHelper->ReadVelocity((BYTE)axis_id, &find_home_vel) != TRUE)
			{
				return ret;
			}
			*/
			if (m_moonsHelper->ReadAccelerationRate((BYTE)axis_id, &move_accel) != TRUE)
			{
				goto func_out;
			}
			if (m_moonsHelper->ReadDecelerationRate((BYTE)axis_id, &move_decel) != TRUE)
			{
				goto func_out;
			}




			if (m_moonsHelper->WriteAccelerationRate((BYTE)axis_id, move_accel) != TRUE)
			{
				goto func_out;
			}
			if (m_moonsHelper->WriteDecelerationRate((BYTE)axis_id, move_decel) != TRUE)
			{
				goto func_out;
			}

			if (m_moonsHelper->WriteDistanceOrPosition((BYTE)axis_id, m_homeParam.find_home_dir) != TRUE)
			{
				goto func_out;
			}
			if (m_moonsHelper->WriteVelocity((BYTE)axis_id, m_homeParam.find_home_vel) != TRUE)
			{
				goto func_out;
			}
			if (m_moonsHelper->WriteSeekHome((BYTE)axis_id, m_homeParam.home_x_no, m_homeParam.home_x_level) != TRUE)
			{
				goto func_out;
			}

			ret = ERROR_SUCCESS;

		func_out:

			//m_mutex.unlock();
			return ret;
		}






	};

}

#endif //USE_HW_MOTOR_MOONS_LIB