#pragma once

/*
Fastech Ethernet 타입 라이브러리 사용 클래스

각 모터별 TCP   client 객체 생성을 위해 IAxis로 직접 생성

*/



#ifdef USE_HW_MOTOR_FASTECH_E_LIB

// ## Library 추가하기
#pragma comment(lib, "lib/fastech_motor/Ver6_x64_E/EziMOTIONPlusE.lib")

#include "FAS_EziMOTIONPlusE.h"


#define MOTOR_FASTECH_E_UDP				(int)1
#define MOTOR_FASTECH_E_TCP				(int)0
#define MOTOR_FASTECH_E_CW				(int)1
#define MOTOR_FASTECH_E_CCW				(int)0
#define MOTOR_FASTECH_E_JOG_LIMIT_TIME  (int)5000
namespace GFastech
{
	using namespace LIB_FASTECH;
	using namespace PE;

	class GFastech_motorE_lib : public IAxis, public IIO
	{
		/****************************************************
		 *	data
		 ****************************************************/

	public:
		enum mode_e
		{
			TCP = 0, UDP
		};		

		struct origin_param_t {
			int orgSpeed{};
			int orgSearchSpeed{};
			int orgAccDecTime{};
			int orgMethod{};	// Origin Method = 2 is 'Limit Origin' in the Ezi-SERVOII Plus-E model.
			int orgDir{};
			int orgOffset{};
			int orgPositionSet{};
			int orgTorqueRatio{};

			origin_param_t() {
				orgSpeed = 50000;
				orgSearchSpeed = 1000;
				orgAccDecTime = 50;
				orgMethod = 2;	// Origin Method =
				orgDir = IAXIS_DIR_CW;
				orgOffset = 0;
				orgPositionSet = 0;
				orgTorqueRatio = 50;
			}

			inline void operator = (const origin_param_t* p_cfg) {
				this->orgSpeed = p_cfg->orgSpeed;
				this->orgSearchSpeed = p_cfg->orgSearchSpeed;
				this->orgAccDecTime = p_cfg->orgAccDecTime;
				this->orgMethod = p_cfg->orgMethod;
				this->orgDir = p_cfg->orgDir;
				this->orgOffset = p_cfg->orgOffset;
				this->orgPositionSet = p_cfg->orgPositionSet;
				this->orgTorqueRatio = p_cfg->orgTorqueRatio;
			}

		};

		struct cfg_t
		{

			IAxis::motion_param_t motion_param{};
			mode_e  mode{};
			char ip_str[SOCKET_IP_STR_MAX]{};


			inline int SetIpStr(const char* value) {
				return sprintf_s(ip_str, SOCKET_IP_STR_MAX, value);

			}

			inline int SetNetMode(char* value) {
				int ret = ERROR_SUCCESS;
				if (util::IsEqualStr("TCP_CLIENT", value))
				{
					mode = mode_e::TCP;
				}
				else if (util::IsEqualStr("UDP_CLIENT", value))
				{
					mode = mode_e::UDP;
				}
				else //if (util::IsEqualStr("UDP_SERVER", (char*)(p_ret)))
				{
					ret = -1;
				}
				return ret;
			}

			inline char* GetIpStr() {
				return &ip_str[0];
			}

			inline void operator = (const cfg_t* p_cfg) {
				this->motion_param = p_cfg->motion_param;
				this->mode = p_cfg->mode;
				this->SetIpStr(p_cfg->ip_str);
			}

		};



		enum class type_e
		{
			I16O16,
			I16,
			O16,
			O32,
			I32
		};

		struct ioCfg_t
		{
			type_e device_type{};
			uint32_t startAddrIn{};
			uint32_t lenghtIn{};
			uint32_t startAddrOut{};
			uint32_t lenghtOut{};

			inline void operator = (const ioCfg_t* p_cfg) {
				this->device_type = p_cfg->device_type;
				this->startAddrIn = p_cfg->startAddrIn;
				this->lenghtIn = p_cfg->lenghtIn;
				this->startAddrOut = p_cfg->startAddrOut;
				this->lenghtOut = p_cfg->lenghtOut;
			}

		};


		struct ioDat_t
		{
			type_e device_type{};
			uint32_t startAddrIn{};
			uint32_t lenghtIn{};
			uint32_t startAddrOut{};
			uint32_t lenghtOut{};
			std::array<uint8_t, 4> input{ 0, };
			std::array<uint8_t, 4> latch{ 0, };
			std::array<uint8_t, 4> set_out{ 0, };
			std::array<uint8_t, 4> reset_out{ 0, };
			//uint32_t input{};
			//uint32_t latch{};
			//uint32_t set_out{};
			//uint32_t reset_out{};


			inline void SetioCfg(ioCfg_t& data) {
				this->device_type = data.device_type;
				this->startAddrIn = data.startAddrIn;
				this->lenghtIn = data.lenghtIn;
				this->startAddrOut = data.startAddrOut;
				this->lenghtOut = data.lenghtOut;
			}



			/// <summary>
			/// reg 정보 확인
			/// </summary>
			/// <param name="addr">시스템에 등록된 주소</param>
			/// <returns>on true, off false</returns>
			bool isOn(uint32_t addr) {
				bool ret = false;
				div_t div_result;
				if (addr > (startAddrOut - 1))
				{
					// case out
					switch (device_type)
					{
					case type_e::I16O16:
						div_result = div((addr - startAddrOut), 8);
						ret = (bool)((set_out[div_result.quot + 2] >> div_result.rem) & 1);
						if ((reset_out[div_result.quot + 2] >> div_result.rem) & 1)
						{
							ret = false;
						}
						break;

					case type_e::O16:
						__fallthrough;
					case type_e::O32:
						div_result = div((addr - startAddrOut), 8);
						ret = (bool)((set_out[div_result.quot] >> div_result.rem) & 1);
						if ((reset_out[div_result.quot] >> div_result.rem) & 1)
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



		/****************************************************
		 *	 Moter
		 ****************************************************/
	private:
		cfg_t m_cfg;
		ioCfg_t m_ioCfg;
		std::array<uint8_t, 4> m_ipAddr;
		bool m_IsConnect;					// -- 시리얼 통신 상태 변수
		int m_motorId;
		motion_param_t m_motionParam;
		origin_param_t m_originParam;
		EZISERVO2_AXISSTATUS m_AxisStatus;

		//io
		ioDat_t m_ioReg;
		/****************************************************
		 *	Constructor
		 ****************************************************/
	public:
		inline GFastech_motorE_lib(common::Attribute_t& common_data, cfg_t& cfg)
			:Icommon(common_data), m_IsConnect(), m_cfg(cfg), m_motorId(0), m_AxisStatus()
			, m_ipAddr(), m_ioReg(), m_ioCfg(), m_motionParam(cfg.motion_param)
		{

			constexpr double default_rps = 5.0;
			m_motionParam.jog_speed = (uint32_t)(m_motionParam.turn_pulse_count * default_rps); //pps
			m_motionParam.jog_accel = 100; //msec
			m_motionParam.jog_decel = 100; //msec

			m_motionParam.move_velocity = (uint32_t)(m_motionParam.turn_pulse_count * default_rps * 3.0);  //pulse
			m_motionParam.move_accel = 100;//msec
			m_motionParam.move_decel = 100;//msec

			m_motionParam.limit_velocity = (uint32_t)(m_motionParam.turn_pulse_count * m_motionParam.max_rps);

		}

		inline GFastech_motorE_lib(cfg_t& cfg, ioCfg_t& iocfg)
			:m_IsConnect(), m_cfg(cfg), m_motorId(0), m_AxisStatus()
			, m_ipAddr(), m_ioReg(), m_ioCfg(iocfg)
		{
			m_ioReg.SetioCfg(iocfg);
		}



		virtual ~GFastech_motorE_lib()
		{
			closeNet();
		}

		/****************************************************
		 *	func
		 ****************************************************/
	private:
		inline uint32_t cal_mms_to_pps_Velocity(uint32_t vel) {
			double target_rps = (double)vel / m_motionParam.one_turn_move_mm;
			if (target_rps > m_motionParam.max_rps)
				target_rps = m_motionParam.max_rps;
			return ((uint32_t)target_rps) * m_motionParam.turn_pulse_count;
		}

		inline void closeNet() {
			FAS_Close((short)m_motorId);
		}

		inline bool connect(int nCommType) {
			int max_args = 10;
			constexpr int max_ip_args = 4;
			constexpr int max_array_max = 16;
			std::vector<char*> v_dats(max_args);
			std::array<char, max_array_max> ip_str;
			memcpy(ip_str.data(), m_cfg.GetIpStr(), strlen(m_cfg.GetIpStr()) + 1);

			uint8_t args_cnt = util::trans::SplitArgs(ip_str.data(), v_dats.data(), ".");

			if (args_cnt != max_ip_args)
			{
				ERR_PRINT("TCP Connection Fail! args_cnt is not 4, cnt-%d  \n", args_cnt);
				return false;
			}

			for (int i = 0; i < max_ip_args; i++)
			{
				m_ipAddr[i] = (BYTE)atoi(v_dats[i]);
			}
			m_ipAddr[1] = (BYTE)atoi(v_dats[1]);
			m_ipAddr[2] = (BYTE)atoi(v_dats[2]);
			m_ipAddr[3] = (BYTE)atoi(v_dats[3]);
			std::array<BYTE, 4> byIP{
				(BYTE)atoi(v_dats[0])
					, (BYTE)atoi(v_dats[1])
					, (BYTE)atoi(v_dats[2])
					, (BYTE)atoi(v_dats[3])
			};
			m_motorId = byIP[3];
			//BYTE byIP[4] = { 192, 168, 0, 2 }; // IP : 192.168.0.2
			bool bSuccess = true;

			// Connection
			switch (nCommType)
			{
			case MOTOR_FASTECH_E_TCP:
				// TCP Connection
				if (FAS_ConnectTCP(byIP[0], byIP[1], byIP[2], byIP[3], m_motorId) == FALSE)
				{
					ERR_PRINT("TCP Connection Fail! \n");
					bSuccess = false;
				}
				break;

			case MOTOR_FASTECH_E_UDP:
				// UDP Connection
				if (FAS_Connect(byIP[0], byIP[1], byIP[2], byIP[3], m_motorId) == FALSE)
				{
					ERR_PRINT("UDP Connection Fail! \n");
					bSuccess = false;
				}
				break;

			default:
				ERR_PRINT("Wrong communication type. \n");
				bSuccess = false;

				break;
			}

			if (bSuccess)
			{
				m_IsConnect = true;
				getAxisStatus(m_AxisStatus.dwValue);
				LTRACE("Connected successfully. \n");
			}

			return bSuccess;
		}


		inline bool checkDriveInfo()
		{
			BYTE byType = 0;
			char IpBuff[256] = "";
			int nBuffSize = 256;
			int nRtn;

			// Read Drive's information
			nRtn = FAS_GetSlaveInfo(m_motorId, &byType, IpBuff, nBuffSize);
			if (nRtn != FMM_OK)
			{
				ERR_PRINT("Function(FAS_GetSlaveInfo) was failed.\n");
				return false;
			}

			LTRACE("Board ID %d : TYPE= %d, Version= %s\n", m_motorId, byType, IpBuff);

			return true;
		}
		inline errno_t clearPosition() {
			return (errno_t)FAS_ClearPosition((short)m_motorId);
		}

		inline errno_t alarmReset() {
			return (errno_t)FAS_ServoAlarmReset((short)m_motorId);
		}

		inline errno_t getAlarmType(DWORD& p_state) {
			return (errno_t)FAS_GetAlarmType((short)m_motorId, (BYTE*)&p_state);
		}

		inline errno_t getAxisStatus(DWORD& p_state) {
			if (FAS_GetAxisStatus(m_motorId, &p_state) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_GetAxisStatus) was failed.\n");
				return -1;
			}
			m_AxisStatus.dwValue = p_state;
			return ERROR_SUCCESS;
		}


		inline errno_t getCommandPosition(long& p_position) {
			return  (errno_t)FAS_GetCommandPos(m_motorId, &p_position);
		}

		inline errno_t getActualPosition(long& p_position) {
			return  (errno_t)FAS_GetActualPos(m_motorId, &p_position);
		}


		inline bool checkDriveErr() {


			if (FAS_GetAxisStatus(m_motorId, &(m_AxisStatus.dwValue)) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_GetAxisStatus) was failed.\n");
				return false;
			}

			if (m_AxisStatus.FFLAG_ERRORALL)
			{
				// if Drive's Error was detected, Reset the ServoAlarm
				if (FAS_ServoAlarmReset(m_motorId) != FMM_OK)
				{
					ERR_PRINT("Function(FAS_ServoAlarmReset) was failed.\n");
					return false;
				}
			}

			return true;
		}

		inline bool motorOff() {

			// if ServoOnFlagBit is OFF('0'), switch to ON('1')
			if (FAS_GetAxisStatus(m_motorId, &(m_AxisStatus.dwValue)) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_GetAxisStatus) was failed.\n");
				return false;
			}

			if (m_AxisStatus.FFLAG_SERVOON != 0)
			{
				if (FAS_ServoEnable(m_motorId, FALSE) != FMM_OK)
				{
					ERR_PRINT("Function(FAS_ServoEnable) was failed.\n");
					return false;
				}

				do
				{
					Sleep(1);

					if (FAS_GetAxisStatus(m_motorId, &(m_AxisStatus.dwValue)) != FMM_OK)
					{
						ERR_PRINT("Function(FAS_GetAxisStatus) was failed.\n");
						return false;
					}

					if (!m_AxisStatus.FFLAG_SERVOON)
						LTRACE("Servo Off \n");
				} while (m_AxisStatus.FFLAG_SERVOON); // Wait until FFLAG_SERVOON is ON
			}
			else
			{
				LTRACE("Servo is already Off \n");
			}

			return true;
		}


		inline bool motorOn() {
			// Check Drive's Servo Status
			//EZISERVO2_AXISSTATUS AxisStatus;

			// if ServoOnFlagBit is OFF('0'), switch to ON('1')
			if (FAS_GetAxisStatus(m_motorId, &(m_AxisStatus.dwValue)) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_GetAxisStatus) was failed.\n");
				return false;
			}

			if (m_AxisStatus.FFLAG_SERVOON == 0)
			{
				if (FAS_ServoEnable(m_motorId, TRUE) != FMM_OK)
				{
					ERR_PRINT("Function(FAS_ServoEnable) was failed.\n");
					return false;
				}

				do
				{
					Sleep(1);

					if (FAS_GetAxisStatus(m_motorId, &(m_AxisStatus.dwValue)) != FMM_OK)
					{
						ERR_PRINT("Function(FAS_GetAxisStatus) was failed.\n");
						return false;
					}

					if (m_AxisStatus.FFLAG_SERVOON)
						LTRACE("Servo ON \n");
				} while (!m_AxisStatus.FFLAG_SERVOON); // Wait until FFLAG_SERVOON is ON
			}
			else
			{
				LTRACE("Servo is already ON \n");
			}

			return true;
		}

		inline errno_t moveStop() {
			return (errno_t)FAS_MoveStop((short)m_motorId);
		}

		inline errno_t emergencyStop() {
			return (errno_t)FAS_EmergencyStop((short)m_motorId);
		}


		inline bool jogMove(DWORD velocity, bool is_cw) {
			// Jogmode Move 
			int direction = (is_cw == true ? 1 : 0);
			//printf("---------------------------\n");
			if (FAS_MoveVelocity(m_motorId, velocity, direction) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_MoveVelocity) was failed.\n");
				return false;
			}
			LTRACE("Move Motor! \n");

			/*	if (FAS_MoveStop(m_motorId) != FMM_OK)
				{
					ERR_PRINT("Function(FAS_MoveStop) was failed.\n");
					return false;
				}


				do
				{
					// Wait until FFLAG_MOTIONING is OFF
					Sleep(1);

					if (FAS_GetAxisStatus(m_motorId, &(m_AxisStatus.dwValue)) != FMM_OK)
					{
						ERR_PRINT("Function(FAS_GetAxisStatus) was failed.\n");
						return false;
					}

					if (m_AxisStatus.FFLAG_MOTIONING == 0)
					{
						LTRACE("Move Stop! \n");
					}
				} while (m_AxisStatus.FFLAG_MOTIONING);
				*/
			return true;
		}


		inline bool jogExMove(int nAccDecTime, int dir) {
			// MoveVelocityEx

			// Set velocity
			int nDirect = dir;
			long lVelocity = (long)m_motionParam.jog_speed;
			int nSeconds = 3 * 1000; // Wait 3 Sec

			//EZISERVO2_AXISSTATUS AxisStatus;
			VELOCITY_OPTION_EX opt = { 0 };

			// set user setting bit(BIT_USE_CUSTOMACCDEC) and acel/decel time value
			opt.flagOption.BIT_USE_CUSTOMACCDEC = 1;
			opt.wCustomAccDecTime = nAccDecTime;

			//printf("-----------------------------------------------------------\n");
			if (FAS_MoveVelocityEx(m_motorId, lVelocity, nDirect, &opt) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_MoveVelocityEx) was failed.\n");
				return false;
			}

			LTRACE("Move Motor(Jog Ex Mode)! \n");

			Sleep(nSeconds);

			if (FAS_MoveStop(m_motorId) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_MoveStop) was failed.\n");
				return false;
			}

			do
			{
				// Wait until FFLAG_MOTIONING is OFF
				Sleep(1);

				if (FAS_GetAxisStatus(m_motorId, &(m_AxisStatus.dwValue)) != FMM_OK)
				{
					ERR_PRINT("Function(FAS_GetAxisStatus) was failed.\n");
					return false;
				}

				if (m_AxisStatus.FFLAG_MOTIONING == 0)
				{
					LTRACE("Move Stop! \n");
				}
			} while (m_AxisStatus.FFLAG_MOTIONING);

			return true;
		}

		inline bool moveIncPos(long position, long velocity) {
			// Move AxisIncPos
			//EZISERVO2_AXISSTATUS AxisStatus;
			long lIncPos;
			long lVelocity = 0;

			//printf("---------------------------\n");
			// Increase the motor by 370000 pulse (target position : Relative position)

			lIncPos = position;
			lVelocity = velocity;

			LTRACE("[Inc Mode] Move Motor ! \n");

			if (FAS_MoveSingleAxisIncPos(m_motorId, lIncPos, lVelocity) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_MoveSingleAxisIncPos) was failed.\n");
				return false;
			}

			// Check the Axis status until motor stops and the Inposition value is checked
			do
			{
				Sleep(1);

				if (FAS_GetAxisStatus(m_motorId, &(m_AxisStatus.dwValue)) != FMM_OK)
				{
					ERR_PRINT("Function(FAS_GetAxisStatus) was failed.\n");
					return false;
				}

			} while (m_AxisStatus.FFLAG_MOTIONING || !(m_AxisStatus.FFLAG_INPOSITION));

			return true;
		}

		inline bool moveAbsPos(long position, DWORD velocity) {
			// Move AxisAbsPos 

			if (FAS_MoveSingleAxisAbsPos(m_motorId, position, velocity) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_MoveSingleAxisAbsPos) was failed.\n");
				return false;
			}

			// Check the Axis status until motor stops and the Inposition value is checked
			do
			{
				Sleep(1);

				if (FAS_GetAxisStatus(m_motorId, &(m_AxisStatus.dwValue)) != FMM_OK)
				{
					ERR_PRINT("Function(FAS_GetAxisStatus) was failed.\n");
					return false;
				}

			} while (m_AxisStatus.FFLAG_MOTIONING || !(m_AxisStatus.FFLAG_INPOSITION));

			return true;
		}



		inline bool movePos(int position, int nAccDecTime, uint32_t velocity) {
			// SetParameter & MoveSingleAxisIncPos

			//EZISERVO2_AXISSTATUS AxisStatus;
			// Set Acc Time
			if (FAS_SetParameter(m_motorId, SERVO2_AXISACCTIME, nAccDecTime) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_SetParameter) was failed.\n");
				return false;
			}

			// Set Dec Time
			if (FAS_SetParameter(m_motorId, SERVO2_AXISDECTIME, nAccDecTime) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_SetParameter) was failed.\n");
				return false;
			}

			//printf("---------------------------\n");

			// Move the motor by [nDistance] pulse (target position : Absolute position)

			LTRACE("Move Motor! \n");
			if (FAS_MoveSingleAxisIncPos(m_motorId, (long)position, (DWORD)velocity) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_MoveSingleAxisIncPos) was failed.\n");
				return false;
			}

			// Check the Axis status until FFLAG_MOTIONING is ON
			do
			{
				Sleep(1);

				if (FAS_GetAxisStatus(m_motorId, &(m_AxisStatus.dwValue)) != FMM_OK)
				{
					ERR_PRINT("Function(FAS_GetAxisStatus) was failed.\n");
					return false;
				}

			} while (m_AxisStatus.FFLAG_MOTIONING);

			return true;
		}
		inline bool movePosEx(int position, int nAccDecTime, uint32_t velocity) {
			// MoveSingleAxisIncPosEx
			//EZISERVO2_AXISSTATUS AxisStatus;
			MOTION_OPTION_EX opt = { 0 };
			long lIncPos;
			long lVelocity;

			//printf("---------------------------\n");

			lIncPos = position;
			lVelocity = velocity;
			opt.flagOption.BIT_USE_CUSTOMACCEL = 1;
			opt.flagOption.BIT_USE_CUSTOMDECEL = 1;
			opt.wCustomAccelTime = nAccDecTime;
			opt.wCustomDecelTime = nAccDecTime;

			LTRACE("Move Motor! [Ex]\n");
			if (FAS_MoveSingleAxisIncPosEx(m_motorId, (long)position, (DWORD)velocity, &opt) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_MoveSingleAxisIncPosEx) was failed.\n");
				return false;
			}

			// Check the Axis status until motor stops and the Inposition value is checked
			do
			{
				Sleep(1);

				if (FAS_GetAxisStatus(m_motorId, &(m_AxisStatus.dwValue)) != FMM_OK)
				{
					ERR_PRINT("Function(FAS_GetAxisStatus) was failed.\n");
					return false;
				}

			} while (m_AxisStatus.FFLAG_MOTIONING || !(m_AxisStatus.FFLAG_INPOSITION));

			return true;
		}


		inline bool setOriginParameter(origin_param_t& params) {
			// Set Origin Parameter
			m_originParam = params;

			if (FAS_SetParameter(m_motorId, SERVO2_ORGSPEED, params.orgSpeed) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_SetParameter[SERVO2_ORGSPEED]) was failed.\n");
				return false;
			}

			if (FAS_SetParameter(m_motorId, SERVO2_ORGSEARCHSPEED, params.orgSearchSpeed) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_SetParameter[SERVO2_ORGSEARCHSPEED]) was failed.\n");
				return false;
			}

			if (FAS_SetParameter(m_motorId, SERVO2_ORGACCDECTIME, params.orgAccDecTime) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_SetParameter[SERVO2_ORGACCDECTIME]) was failed.\n");
				return false;
			}

			if (FAS_SetParameter(m_motorId, SERVO2_ORGMETHOD, params.orgMethod) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_SetParameter[SERVO2_ORGMETHOD]) was failed.\n");
				return false;
			}

			if (FAS_SetParameter(m_motorId, SERVO2_ORGDIR, params.orgDir) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_SetParameter[SERVO2_ORGDIR]) was failed.\n");
				return false;
			}

			if (FAS_SetParameter(m_motorId, SERVO2_ORGOFFSET, params.orgOffset) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_SetParameter[SERVO2_ORGOFFSET]) was failed.\n");
				return false;
			}

			if (FAS_SetParameter(m_motorId, SERVO2_ORGPOSITIONSET, params.orgPositionSet) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_SetParameter[SERVO2_ORGPOSITIONSET]) was failed.\n");
				return false;
			}

			if (FAS_SetParameter(m_motorId, SERVO2_ORGTORQUERATIO, params.orgTorqueRatio) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_SetParameter[SERVO2_ORGTORQUERATIO]) was failed.\n");
				return false;
			}

			return true;
		}
		inline bool originSearch() {
			// Act Origin Search Function
			//EZISERVO2_AXISSTATUS AxisStatus;

			//printf("---------------------------\n");

			// Origin Search Start
			if (FAS_MoveOriginSingleAxis(m_motorId) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_MoveOriginSingleAxis) was failed.\n");
				return false;
			}

			// Check the Axis status until OriginReturning value is released.
			do
			{
				Sleep(1);

				if (FAS_GetAxisStatus(m_motorId, &(m_AxisStatus.dwValue)) != FMM_OK)
				{
					ERR_PRINT("Function(FAS_GetAxisStatus) was failed.\n");
					return false;
				}

			} while (m_AxisStatus.FFLAG_ORIGINRETURNING);

			if (m_AxisStatus.FFLAG_ORIGINRETOK)
			{
				LTRACE("Origin Search Success! \n");
				return true;
			}
			else
			{
				ERR_PRINT("Origin Search Fail !\n");
				return false;
			}
		}


		inline bool setMotionParam(motion_param_t& param) {

			// Set Acc Time
			if (FAS_SetParameter(m_motorId, SERVO2_AXISACCTIME, param.move_accel) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_SetParameter) was failed.\n");
				return false;
			}

			// Set Dec Time
			if (FAS_SetParameter(m_motorId, SERVO2_AXISDECTIME, param.move_decel) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_SetParameter) was failed.\n");
				return false;
			}

			m_motionParam = param;

			return true;
		}

		inline void getInput()
		{
			DWORD ret_input = 0;
			DWORD ret_latch = 0;
			//FAS_SetIOInput(m_motorId, DWORD dwIOSETMask, DWORD dwIOCLRMask);
			FAS_GetInput(m_motorId, &ret_input, &ret_latch);
			m_ioReg.input[0] = (ret_input >> 0) & 0xff;
			m_ioReg.input[1] = (ret_input >> 8) & 0xff;
			m_ioReg.input[2] = (ret_input >> 16) & 0xff;
			m_ioReg.input[3] = (ret_input >> 24) & 0xff;
			//FAS_SetIOOutput(int iBdID, DWORD dwIOSETMask, DWORD dwIOCLRMask);
			//FAS_GetIOOutput(int iBdID, DWORD * dwIOOutput);
		}

		inline errno_t setOutput(uint32_t addr, bool is_set)
		{
			errno_t ret = ERROR_SUCCESS;
			constexpr int BYTE_DIVISOR = 8;
			// TODO: 여기에 구현 코드 추가.
			if (m_ioReg.startAddrOut > addr)
				return -1;

			div_t div_result = div((addr - m_ioReg.startAddrOut), BYTE_DIVISOR);
			if (is_set)
			{
				switch (m_ioCfg.device_type)
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
				switch (m_ioCfg.device_type)
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


			uint32_t set_data = util::DwToUint(m_ioReg.set_out.data());
			uint32_t reset_mask = util::DwToUint(m_ioReg.reset_out.data());

			if (FAS_SetOutput(m_motorId, (unsigned long)set_data, (unsigned long)reset_mask) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_SetParameter) was failed.\n");
				ret = -1;
			}

			return ret;

		}



		inline void getOutput()
		{
			DWORD return_val = 0;
			DWORD return_status = 0;
			FAS_GetOutput(m_motorId, &return_val, &return_status);
			m_ioReg.set_out[0] = (return_val >> 0) & 0xff;
			m_ioReg.set_out[1] = (return_val >> 8) & 0xff;
			m_ioReg.set_out[2] = (return_val >> 16) & 0xff;
			m_ioReg.set_out[3] = (return_val >> 24) & 0xff;
		}




		errno_t toggleOutReg(uint32_t addr)
		{
			errno_t ret = ERROR_SUCCESS;
			if (m_ioReg.startAddrOut > addr)
				return -1;
			constexpr int BYTE_DIVISOR = 8;
			bool is_reset = false;
			bool is_set = false;
			std::array<uint8_t, 4> set_data{ 0, };
			std::array<uint8_t, 4> reset_data{ 0, };
			div_t div_result = div((addr - m_ioReg.startAddrOut), BYTE_DIVISOR);

			if (m_ioReg.isOn(addr))
			{
				switch (m_ioCfg.device_type)
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
				switch (m_ioCfg.device_type)
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
			m_ioReg.set_out = set_data;
			m_ioReg.reset_out = reset_data;
			uint32_t data_set = util::DwToUint(&set_data[0]);
			uint32_t data_reset = util::DwToUint(&reset_data[0]);

			if (FAS_SetOutput(m_motorId, (unsigned long)data_set, (unsigned long)data_reset) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_SetParameter) was failed.\n");
				ret = -1;
			}

			return ret;
		}





	public:
		inline bool IsConnected() {
			return this->m_IsConnect;
		}

		inline void GetOutput()
		{
			getOutput();
		}

		inline void GetInput()
		{
			getInput();
		}

		inline errno_t SetOriginParam(origin_param_t& params) {
			bool ret = setOriginParameter(params);
			(ret == true ? (errno_t)ERROR_SUCCESS : -1);
		}


		/*--------------------------------------------------------------------------------------*/

		inline errno_t Initialize() {
			errno_t ret = -1;

			if (m_cfg.mode == GFastech_motorE_lib::mode_e::TCP)
			{
				if (FAS_IsSlaveExist(m_motorId) == FALSE)
				{
					closeNet();
					Sleep(300);
					if (m_IsConnect)
					{
						closeNet();
						Sleep(100);
						FAS_Reconnect(m_motorId);
					}
					else if (connect(MOTOR_FASTECH_E_TCP))
					{
						ret = ERROR_SUCCESS;
					}
				}
				else
				{
					if (connect(MOTOR_FASTECH_E_TCP))
					{
						ret = ERROR_SUCCESS;
					}
				}
			}
			else
			{
				if (FAS_IsSlaveExist(m_motorId) == FALSE)
				{
					if (connect(MOTOR_FASTECH_E_UDP))
					{
						ret = ERROR_SUCCESS;
					}
				}
				else
				{
					if (connect(MOTOR_FASTECH_E_UDP))
					{
						ret = ERROR_SUCCESS;
					}
				}
			}


			return ret;
		}

		/*inline errno_t MotorOnOff(short axis_id, bool on_off) {
			bool ret = false;
			if (on_off)
			{
				ret = motorOn((int)axis_id);
			}
			else
			{
				ret = motorOff((int)axis_id);
			}
			return (ret ==true? (errno_t)ERROR_SUCCESS:-1);
		}*/

		/*inline bool  IsMotorOn(short axis_id)  {
			return m_AxisStatus.FFLAG_SERVOON;

		}*/


		/*inline errno_t WaitDone(short axis_id, unsigned short timeout) {
			uint32_t pre_ms = util::millis();

			while (util::millis()-pre_ms <timeout)
			{
				if (m_AxisStatus.FFLAG_INPOSITION)
				{
					return ERROR_SUCCESS;
				}
			}

			return -1;
		}*/

		/*inline  bool IsAxisDone(short axis_id) {
			return m_AxisStatus.FFLAG_INPOSITION;
		}*/

		/*inline errno_t GetAxisStatus(short axis_id, DWORD& p_state)	{
			return getAxisStatus(axis_id, p_state);
		}*/

		/*inline errno_t ClearPosition(short axis_id) {
			return clearPosition(axis_id);
		}*/

		/*inline errno_t AlarmReset(short axis_id) {
			return alarmReset(axis_id);
		}*/

		/*inline errno_t GetAlarmType(short axis_id, DWORD& p_state) {
			return getAlarmType(axis_id, p_state);
		}*/

		/*inline errno_t GetCommandPosition(short axis_id, long& p_position) {
			return getCommandPosition(axis_id, p_position);
		}*/

		/*inline errno_t GetActualPosition(short axis_id, long& p_position) {
			return getActualPosition(axis_id, p_position);
		}*/

		/*inline errno_t JogMove(short axis_id, DWORD velocity, bool is_cw) {
			bool ret = jogMove(axis_id, velocity, is_cw);
			return (ret == true ? (errno_t)ERROR_SUCCESS : -1);
		}*/

		/*inline errno_t JogStop(short axis_id) {
			bool ret = JogStop(axis_id);
			return (ret == true ? (errno_t)ERROR_SUCCESS : -1);
		}*/


		/*inline errno_t MoveStop(short axis_id) {
			return moveStop(axis_id);
		}*/

		/*inline errno_t Move(short axis_id, long position, DWORD velocity, bool wait_done = false) {
			bool ret = moveAbsPos(axis_id, position, velocity);
			while (wait_done && ret)
			{
				if (m_AxisStatus.FFLAG_INPOSITION)
				{
					break;
				}
			}
			return (ret == true ? (errno_t)ERROR_SUCCESS : -1);
		}*/

		/*inline errno_t OriginAxis(short axis_id) {
			bool ret = originSearch(axis_id);
			return (ret == true ? (errno_t)ERROR_SUCCESS : -1);
		}*/








		inline errno_t MotorOnOff(bool on_off) {
			if (m_IsConnect != true)
				return -1;

			bool ret;
			getAxisStatus(m_AxisStatus.dwValue);
			if (on_off)
			{
				ret = motorOn();
			}
			else
			{
				ret = motorOff();
			}
			return (ret == true ? (errno_t)ERROR_SUCCESS : -1);
		}
		inline bool  IsMotorOn() {
			getAxisStatus(m_AxisStatus.dwValue);
			return m_AxisStatus.FFLAG_SERVOON;
		}

		inline errno_t WaitDone(uint32_t timeout) {
			uint32_t pre_ms = util::millis();
			if (m_IsConnect != true)
				return -1;

			while (util::millis() - pre_ms < timeout)
			{
				if (m_AxisStatus.FFLAG_INPOSITION)
				{
					return ERROR_SUCCESS;
				}
			}

			return -1;
		}
		inline bool IsAxisDone() {
			return m_AxisStatus.FFLAG_INPOSITION;
		}
		inline errno_t  ClearState() {
			if (m_IsConnect != true)
				return -1;
			return alarmReset();
		}

		inline uint32_t GetMotorState() {
			if (m_IsConnect != true)
				return 0;

			getAxisStatus(m_AxisStatus.dwValue);
			return (uint32_t)m_AxisStatus.dwValue;
		}

		inline errno_t SetMotionData(motion_param_t& param) {
			if (m_IsConnect != true)
				return -1;

			return ((setMotionParam(param)) == true ? (errno_t)ERROR_SUCCESS : -1);
		}

		inline errno_t OriginAxis() {
			if (m_IsConnect != true)
				return -1;
			bool ret = originSearch();
			return (ret == true ? (errno_t)ERROR_SUCCESS : -1);
		}

		inline errno_t ClearPosition() {
			if (m_IsConnect != true)
				return -1;
			return clearPosition();
		}

		inline errno_t Move(int cmd_pos, uint32_t cmd_vel = 0, uint32_t acc = 100, uint32_t decel = 100) {
			if (m_IsConnect != true)
				return -1;


			//m_motionParam.move_accel = acc;
			//m_motionParam.move_decel = decel;
			//m_motionParam.move_velocity = cmd_vel;
			uint32_t velocity_pps = m_motionParam.move_velocity;
			if (cmd_vel > 0)
				velocity_pps = cal_mms_to_pps_Velocity(cmd_vel);


			if (FAS_SetParameter(m_motorId, SERVO2_AXISACCTIME, acc) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_SetParameter) was failed.\n");
				return -1;
			}

			// Set Dec Time
			if (FAS_SetParameter(m_motorId, SERVO2_AXISDECTIME, decel) != FMM_OK)
			{
				ERR_PRINT("Function(FAS_SetParameter) was failed.\n");
				return -1;
			}
			bool ret = moveAbsPos((long)cmd_pos, (DWORD)velocity_pps);
			return (ret == true ? (errno_t)ERROR_SUCCESS : -1);
		}

		//축을 정지한다. (한개의 축에 대한 정지)
		inline errno_t Stop() {
			if (m_IsConnect != true)
				return -1;
			return moveStop();
		}
		//축을 등속이동에 대해 정지한다.
		inline errno_t VStop() {
			if (m_IsConnect != true)
				return -1;
			return moveStop();
		}
		// 축을 비상정지한다.
		inline errno_t EStop() {
			return emergencyStop();
		}

		inline errno_t JogStop() {
			if (m_IsConnect != true)
				return -1;
			return moveStop();
		}

		inline errno_t JogMove(uint32_t cmd_vel = 0, bool is_cw = true) {
			if (m_IsConnect != true)
				return -1;
			uint32_t velocity_pps = m_motionParam.jog_speed;
			if (cmd_vel > 0)
				velocity_pps = cal_mms_to_pps_Velocity(cmd_vel);

			return jogMove((DWORD)velocity_pps, is_cw);
		}




		inline bool IsOn(uint32_t addr) {
			bool ret = false;
			ret = m_ioReg.isOn(addr);
			return ret;
		}
		inline bool IsOff(uint32_t addr) {
			bool ret = false;
			ret = !m_ioReg.isOn(addr);
			return ret;
		}
		inline errno_t OutputOn(uint32_t addr) {
			errno_t ret = -1;
			ret = setOutput(addr, true);
			return ret;
		}
		inline errno_t OutputOff(uint32_t addr) {
			errno_t ret = -1;
			ret = setOutput(addr, false);
			return ret;
		}
		inline errno_t OutputToggle(uint32_t addr) {
			errno_t ret = -1;
			ret = toggleOutReg(addr);
			return ret;
		}


		inline errno_t GetCommandPosition(long& p_position) {
			return 0;
		}

		inline errno_t GetActualPosition(long& p_position) {
			return 0;
		}




	};

}


#endif//USE_HW_MOTOR_FASTECH_E_LIB
