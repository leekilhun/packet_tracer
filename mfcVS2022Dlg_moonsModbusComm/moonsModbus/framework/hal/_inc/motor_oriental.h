#pragma once


#ifdef USE_HW_MOTOR_ORIENTAL


#define MOTOR_ORIENTAL_USE_COMM_LIB
namespace GOriental
{
	using namespace std;

	class GOriental_motor : public IAxis
	{
		/****************************************************
		 *	data
		 ****************************************************/
	public:
		struct cfg_t
		{
#ifdef MOTOR_ORIENTAL_USE_COMM_LIB
			shared_ptr<GOriental_motor_lib> pLib{};
#else
			IComm* pComm{};
#endif
			uint8_t AxisId{};
			motion_param_t motion_param;

		};
	private:

		/****************************************************
		 *	Oriental Moter
		 ****************************************************/
		typedef union
		{
			int	iStatus;
			struct
			{
				unsigned	AZ_FLAG_M0_R : 1;			// = 0x00000001;
				unsigned	AZ_FLAG_M1_R : 1;			// = 0x00000002;
				unsigned	AZ_FLAG_M2_R : 1;			// = 0x00000004;
				unsigned	AZ_FLAG_START_R : 1;	// = 0x00000008;
				unsigned	AZ_FLAG_HOME_END : 1;	// = 0x00000010;
				unsigned	AZ_FLAG_READY : 1;		// = 0x00000020;
				unsigned	AZ_FLAG_INFO : 1;			// = 0x00000040;
				unsigned	AZ_FLAG_ALM_A : 1;		// = 0x00000080;
				unsigned	AZ_FLAG_SYS_BSY : 1;	// = 0x00000100;
				unsigned	AZ_FLAG_AREA0 : 1;		// = 0x00000200;
				unsigned	AZ_FLAG_AREA1 : 1;		// = 0x00000400;
				unsigned	AZ_FLAG_AREA2 : 1;		// = 0x00000800;
				unsigned	AZ_FLAG_TIM : 1;			// = 0x00001000;
				unsigned	AZ_FLAG_MOVE : 1;			// = 0x00002000;
				unsigned	AZ_FLAG_IN_POS : 1;		// = 0x00004000;
				unsigned	AZ_FLAG_TLC : 1;			// = 0x00008000;
			};
		} ORIENRAL_AZ_STATUS;

		ORIENRAL_AZ_STATUS m_regAxis;
		motion_param_t m_motionParam;



		/**
		*	Oriental Motor - Parameter
		*/
		bool		m_IsConnect;					// -- 시리얼 통신 상태 변수
		int			m_DataNo = 0;		// 
#ifdef MOTOR_ORIENTAL_USE_COMM_LIB
		shared_ptr<GOriental_motor_lib> m_pLib;
#else
#endif
		int			m_AxisId;
		GOriental_motor::cfg_t m_cfg;

		/****************************************************
		 *	Constructor
		 ****************************************************/
	public:
		inline GOriental_motor(common::Attribute_t& common_data, cfg_t& cfg)
			:Icommon(common_data), m_AxisId(cfg.AxisId), m_cfg(cfg)
			, m_IsConnect(false), m_regAxis(), m_pLib(cfg.pLib)
			, m_motionParam(cfg.motion_param)
		{
			constexpr double default_rps = 5.0;
			m_motionParam.jog_speed = 1'000;    /*일단 그냥   (uint32_t)(m_motionParam.turn_pulse_count * default_rps);*/ //hz  1khz는 1초에 1'000펄스 발생
			m_motionParam.jog_accel = 100; //msec
			m_motionParam.jog_decel = 100; //msec

			m_motionParam.move_velocity = 3'000; /* 일단 그냥 (uint32_t)(m_motionParam.turn_pulse_count * default_rps * 3.0);*/  //hz
			m_motionParam.move_accel = 100;//msec
			m_motionParam.move_decel = 100;//msec

			m_motionParam.limit_velocity = (uint32_t)(m_motionParam.turn_pulse_count * m_motionParam.max_rps);




		}
		virtual ~GOriental_motor()
		{
			if (m_pLib == nullptr)
			{
				m_pLib.reset();
			}
		}


		/****************************************************
	   *	func
	   ****************************************************/
	private:


		/**
		*	Oriental Moter - Status
		*/
		inline bool AZ_IsHome() const {
			return m_regAxis.AZ_FLAG_HOME_END;
		}
		inline bool AZ_IsReady() const {
			return m_regAxis.AZ_FLAG_READY;
		}
		inline bool AZ_IsInfo() const {
			return m_regAxis.AZ_FLAG_INFO;
		}
		inline bool AZ_IsAlarm() const {
			return m_regAxis.AZ_FLAG_ALM_A;
		}
		inline bool AZ_IsBusy() const {
			return m_regAxis.AZ_FLAG_SYS_BSY;
		}

		inline int AZ_getFlagStatus() {
			return (m_regAxis.iStatus = m_pLib->AZ_getFlagStatus(m_AxisId));
		}

		/**
		*	Oriental Moter - 동작
		*/
		inline errno_t AZ_Move() {
			return m_pLib->AZ_Move(m_AxisId);
		}
		inline errno_t AZ_Move_For(BOOL bOnOff) {
			return m_pLib->AZ_Move_For(m_AxisId, bOnOff);
		}

		inline errno_t  AZ_Start(int data_no) {	// Data No 설정값으로 동작
			//상태 체크
			AZ_getFlagStatus();
			if (!m_regAxis.AZ_FLAG_READY)
			{
				// 알람 추가하기.
				AZ_ERR_WRITE("error motor not ready !");
				return DEF_AZ_ERR_RET_FAIL;
			}
			// Move
			return m_pLib->AZ_Start(m_AxisId, data_no);;

		}
		inline errno_t AZ_Home() {
			errno_t ret = ERROR_SUCCESS;
			//TODO::


			return ret;
		}


		inline errno_t AZ_Stop() {
			errno_t ret = ERROR_SUCCESS;
			if (m_pLib->AZ_Stop(m_AxisId) != ERROR_SUCCESS)
			{
				ret = -1;
			}
			AZ_getFlagStatus();
			return ret;
		}

		inline errno_t AZ_FreeOn() {
			return m_pLib->AZ_FreeOn(m_AxisId);
		}
		inline errno_t AZ_FreeOff() {
			return m_pLib->AZ_FreeOff(m_AxisId);
		}


		/**
		*	Oriental Moter - Alarm, Reset, ... etc
		*/
		inline errno_t AZ_Alarm_Reset() {
			return m_pLib->AZ_Alarm_Reset(m_AxisId);
		}


		inline int AZ_Get_Current_Alarm() const {
			return m_pLib->AZ_Get_Current_Alarm(m_AxisId);
		}

		inline errno_t AZ_P_PReset() {
			return m_pLib->AZ_P_PReset(m_AxisId);;
		}



		/**
	*	Oriental Moter - Setting
	*/
		inline errno_t AZ_SetOperatingCurrent(int data_no, int value) {
			return m_pLib->AZ_SetOperatingCurrent(m_AxisId, data_no, value);
		}

		inline int AZ_GetOperatingCurrent(int data_no) const {
			return m_pLib->AZ_GetOperatingCurrent(m_AxisId, data_no);
		}

		inline int AZ_GetDataVelocity(int data_no) const {
			return m_pLib->AZ_GetDataVelocity(m_AxisId, data_no);
		}

		inline int AZ_GetCurrentVelocity() const {
			return m_pLib->AZ_GetCurrentVelocity(m_AxisId);;
		}


		inline errno_t AZ_Set_Velocity(int value = 1000, int data_no = -1) {
			AZ_getFlagStatus();
			return m_pLib->AZ_Set_Velocity(m_AxisId, data_no, value);
		}


		inline errno_t AZ_Set_Torque(int data_no, int value) {
			AZ_getFlagStatus();
			return m_pLib->AZ_Set_Torque(m_AxisId, data_no, value);
		}

		inline errno_t AZ_Set_Torque_For(int value) {
			AZ_getFlagStatus();
			return m_pLib->AZ_Set_Torque_For(m_AxisId, value);
		}

		inline errno_t AZ_Set_Torque_Back(int value) {
			AZ_getFlagStatus();
			return m_pLib->AZ_Set_Torque_Back(m_AxisId, value);
		}

		inline errno_t JogPitchMove(int position, DWORD velocity = 1000, short accel = 100, short decel = 100) {
			return m_pLib->JogPitchMove(m_AxisId, position, velocity, accel, decel);
		}



		inline errno_t setMotionParam(motion_param_t& param) {
			AZ_Set_Velocity(param.move_velocity);
			m_motionParam = param;
			return ERROR_SUCCESS;
		}






	public:






		/* IMotionLib  함수*/

		inline errno_t Initialize() {
			errno_t ret = ERROR_SUCCESS;
			int az_ret = 0; // retrun 1 : Success , 0 : fail
			if (m_pLib->Initialize() == ERROR_SUCCESS)
			{
				ret = AZ_Stop();
				Sleep(100);
				ret = AZ_Alarm_Reset();

				ret = AZ_P_PReset();

				Sleep(100);
				m_IsConnect = true;
			}
			return  ret;
		}


		inline errno_t MotorOnOff(bool on_off) {
			if (m_IsConnect != true)
				return false;
			AZ_getFlagStatus();
			return m_pLib->MotorOnOff(m_AxisId, on_off);
		}

		inline bool IsMotorOn() {
			if (m_IsConnect != true)
				return false;
			AZ_getFlagStatus();
			return m_pLib->IsMotorOn(m_AxisId);
		}


		inline errno_t WaitDone(uint32_t timeout) {
			return m_pLib->WaitDone(m_AxisId, (unsigned short)timeout);
		}



		inline bool IsAxisDone() {
			if (m_IsConnect != true)
				return false;
			AZ_getFlagStatus();
			return m_pLib->IsAxisDone(m_AxisId);
		}


		inline errno_t ClearState() {
			return m_pLib->AlarmReset(m_AxisId);
		}

		inline uint32_t GetMotorState() {
			errno_t ret = -1;
			DWORD state = 0;
			ret = m_pLib->GetAxisStatus((short)m_AxisId, state);
			if (ret == ERROR_SUCCESS)
			{
				m_regAxis.iStatus = (int)state;
			}
			return (uint32_t)m_regAxis.iStatus;
		}

		inline errno_t SetMotionData(motion_param_t& param) {
			return setMotionParam(param);
		}


		inline errno_t OriginAxis() {
			return m_pLib->OriginAxis(m_AxisId);
		}


		inline errno_t ClearPosition() {
			return m_pLib->ClearPosition(m_AxisId);
		}

		inline errno_t Move(int cmd_pos, uint32_t cmd_vel = 0, uint32_t acc = 100, uint32_t decel = 100) {
			m_motionParam.move_accel = acc;
			m_motionParam.move_decel = decel;
			m_motionParam.move_velocity = cmd_vel;
			return m_pLib->Move(m_AxisId, cmd_pos, cmd_vel);
		}

		inline errno_t Stop() {
			return m_pLib->MoveStop(m_AxisId);
		}

		inline errno_t VStop() {
			return m_pLib->MoveStop(m_AxisId);
		}

		inline errno_t EStop() {
			return m_pLib->MoveStop(m_AxisId);
		}


		inline errno_t JogStop() {
			return m_pLib->JogStop(m_AxisId);
		}

		inline errno_t JogMove(uint32_t velocity, bool is_cw) {
			return m_pLib->JogMove(m_AxisId, (DWORD)velocity, is_cw);
		}



		inline errno_t GetAlarmType(DWORD& p_state) {
			return m_pLib->GetAlarmType(m_AxisId, p_state);
		}

		inline errno_t GetCommandPosition(long& p_position) {
			return m_pLib->GetCommandPosition(m_AxisId, p_position);
		}

		inline errno_t GetActualPosition(long& p_position) {
			return m_pLib->GetActualPosition(m_AxisId, p_position);
		}









	};


}
#endif